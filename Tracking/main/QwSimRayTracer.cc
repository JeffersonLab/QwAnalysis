// This is an example for testing the bridging code

// Qweak tracking headers
#include "QwOptionsTracking.h"
#include "QwParameterFile.h"
#include "QwRayTracer.h"
#include "QwMatrixLookup.h"

#include "QwTreeEventBuffer.h"
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"

// Qweak event
#include "QwEvent.h"

// Deprecated
#include "Det.h"


/**
 * Method to print vectors conveniently
 * @param stream Output stream
 * @param v Vector
 * @return Output stream
 */
inline ostream& operator<< (ostream& stream, const TVector3& v)
{
  return stream << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";
}

/**
 * Main function
 * @param argc Number of arguments
 * @param argv[] List of arguments
 * @return Return code
 */
int main (int argc, char* argv[])
{
  // Debug flag
  bool debug = true;

  // Create a timer
  TStopwatch timer;

  /// Command line options
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwsimtracking.conf");
  // Define the command line options
  DefineOptionsTracking(gQwOptions);

  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");

  /// For the tracking analysis we create the QwSubsystemArrayTracking list
  /// which contains the VQwSubsystemTracking objects.
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking();

  // Region 1 GEM
  detectors->push_back(new QwGasElectronMultiplier("R1"));
  detectors->GetSubsystem("R1")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R1"))->LoadQweakGeometry("qweak_new.geo");

  // Region 2 HDC
  detectors->push_back(new QwDriftChamberHDC("R2"));
  detectors->GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");

  // Region 3 VDC
  detectors->push_back(new QwDriftChamberVDC("R3"));
  detectors->GetSubsystem("R3")->LoadChannelMap("TDCtoDL.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R3"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R1"))->GetDetectorInfo(detector_info);


  /// Create a lookup table bridging method
  std::string trajmatrix = std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";
  QwMatrixLookup* matrixlookup = new QwMatrixLookup();
  if (! matrixlookup->LoadTrajMatrix(trajmatrix)) {
    QwError << "Could not load trajectory lookup table!" << QwLog::endl;
    return false;
  }

  /// Create a ray tracer bridging method
  std::string fieldmap = std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat";
  QwRayTracer* raytracer = new QwRayTracer();
  if (! QwRayTracer::LoadMagneticFieldMap(fieldmap)) {
    QwError << "Could not load magnetic field map!" << QwLog::endl;
    return false;
  }


  // Loop over the runs
  for (UInt_t runnumber  = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
              runnumber <= (UInt_t) gQwOptions.GetIntValuePairLast("run");
              runnumber++) {

    // Setup the output file for bridgingresults
    TString outputfilename = Form(TString(getenv("QWSCRATCH")) + "/rootfiles/QwSimBridge_%d.root", runnumber);
    TFile *file = new TFile(outputfilename, "RECREATE", "Bridging result");
    file->cd();
    TTree *tree = new TTree("tree", "Bridging");

    Double_t CpuTime, RealTime;

    QwEvent* event = 0;
    tree->Branch("events", "QwEvent", &event);


    /// Load the simulated event file
    TString input = Form(TString(getenv("QWSCRATCH")) + "/data/QwSim_%d.root", runnumber);
    QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer (input, detector_info);


    /// We loop over all requested events.
    Int_t entries = treebuffer->GetEntries();
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    if (eventnumber_max > entries) eventnumber_max = entries;
    for (Int_t eventnumber  = eventnumber_min;
               eventnumber <= eventnumber_max;
               eventnumber++) {

      /// Print event number
      QwMessage << "Event " << eventnumber << QwLog::endl;

      /// Read the entry from the tree
      treebuffer->GetEntry(eventnumber);


      /// Get the generated event
      event = treebuffer->GetEvent();
      event->GetEventHeader()->SetRunNumber(runnumber);
      event->GetEventHeader()->SetEventNumber(eventnumber);

      /// Get the partial tracks in the front and back region
      std::vector<QwPartialTrack*> tracks_r2 = treebuffer->GetPartialTracks(kRegionID2);
      std::vector<QwPartialTrack*> tracks_r3 = treebuffer->GetPartialTracks(kRegionID3);

      // Process the partial tracks in region 2 and region 3
      for (size_t i = 0; i < tracks_r2.size(); i++) {
        for (size_t j = 0; j < tracks_r3.size(); j++) {

          // Filter tracks based on parameters (TODO filter should go somewhere else)
          int status = raytracer->Filter(tracks_r2.at(i), tracks_r3.at(j));
          if (status == -1) {
            QwMessage << "Track did not pass filter." << QwLog::endl;
            continue;
          }
          status = -1;

          std::vector<QwTrack*> tracklist;

          // Bridge using the lookup table
          if (status == -1) {
            timer.Start();
            status = matrixlookup->Bridge(tracks_r2.at(i), tracks_r3.at(j));
            timer.Stop();
            CpuTime = timer.CpuTime();
            RealTime = timer.RealTime();
            timer.Reset();
            if (status == 0)
              tracklist = matrixlookup->GetListOfTracks();
            else QwMessage << "Matrix lookup: " << status << QwLog::endl;
          }

          // Bridge using the ray tracer
          if (status == -1) {
            timer.Start();
            status = raytracer->Bridge(tracks_r2.at(i), tracks_r3.at(j));
            timer.Stop();
            CpuTime = timer.CpuTime();
            RealTime = timer.RealTime();
            timer.Reset();
            if (status == 0)
              tracklist = raytracer->GetListOfTracks();
            else QwMessage << "Ray tracer: " << status << QwLog::endl;
          }

          if (status == 0) {
            event->AddTrackList(tracklist);
          }

        } // end of back track loop
      } // end of front track loop


      // Print the event
      if (debug) event->Print();

      // Write event to tree
      //tree->Fill();


      // Progress message
      if (eventnumber > 0 && eventnumber % 100 == 0) {
        tree->AutoSave();
        gDirectory->Purge();
        QwMessage << "Processed " << eventnumber << " events" << QwLog::endl;
      }

    } // end of the loop over events

    // Write output root file
    file->Write(0, TObject::kOverwrite);
    file->Close();
    delete file;

  } // end of the loop over runs

  // Delete objects
  delete raytracer;
  delete matrixlookup;

  return 0;

}

