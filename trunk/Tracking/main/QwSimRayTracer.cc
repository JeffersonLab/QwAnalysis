// This is an example for testing the bridging code

// Qweak tracking headers
#include "QwOptionsTracking.h"
#include "QwParameterFile.h"
#include "QwBridgingTrackFilter.h"
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

  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWSCRATCH") + "/setupfiles");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");

  /// For the tracking analysis we create the QwSubsystemArrayTracking list
  /// which contains the VQwSubsystemTracking objects.
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking();

  // Region 1 GEM
  detectors->push_back(new QwGasElectronMultiplier("R1"));
  detectors->GetSubsystemByName("R1")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystemByName("R1"))->LoadGeometryDefinition("qweak_new.geo");

  // Region 2 HDC
  detectors->push_back(new QwDriftChamberHDC("R2"));
  detectors->GetSubsystemByName("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystemByName("R2"))->LoadGeometryDefinition("qweak_new.geo");

  // Region 3 VDC
  detectors->push_back(new QwDriftChamberVDC("R3"));
  detectors->GetSubsystemByName("R3")->LoadChannelMap("TDCtoDL.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystemByName("R3"))->LoadGeometryDefinition("qweak_new.geo");

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  ((VQwSubsystemTracking*) detectors->GetSubsystemByName("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystemByName("R3"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystemByName("R1"))->GetDetectorInfo(detector_info);

  /// Create a track filter
  QwBridgingTrackFilter* trackfilter = new QwBridgingTrackFilter();

  /// Create a lookup table bridging method
  QwMatrixLookup* matrixlookup = new QwMatrixLookup();
  // Determine lookup table file from environment variables
  std::string trajmatrix = "";
  if (getenv("QW_LOOKUP"))
    trajmatrix = std::string(getenv("QW_LOOKUP")) + "/QwTrajMatrix.root";
  else
    QwWarning << "Environment variable QW_LOOKUP not defined." << QwLog::endl;
  // Load lookup table
  if (! matrixlookup->LoadTrajMatrix(trajmatrix))
    QwError << "Could not load trajectory lookup table!" << QwLog::endl;

  /// Create a ray tracer bridging method
  QwRayTracer* raytracer = new QwRayTracer();
  // Determine magnetic field file from environment variables
  std::string fieldmap = "";
  if (getenv("QW_FIELDMAP"))
    fieldmap = std::string(getenv("QW_FIELDMAP")) + "/peiqing_2007.dat";
  else
    QwWarning << "Environment variable QW_FIELDMAP not defined." << QwLog::endl;
  // Load magnetic field map
  if (! QwRayTracer::LoadMagneticFieldMap(fieldmap))
    QwError << "Could not load magnetic field map!" << QwLog::endl;


  /// Load the simulated event file
  QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer(detector_info);
  treebuffer->ProcessOptions(gQwOptions);

  ///  Start loop over all runs
  while (treebuffer->OpenNextFile() == 0) {

    // Setup the output file for bridgingresults
    TString outputfilename = Form(getenv_safe_TString("QWSCRATCH") + "/rootfiles/QwSimBridge_%d.root", treebuffer->GetRunNumber());
    TFile *file = new TFile(outputfilename, "RECREATE", "Bridging result");
    file->cd();
    TTree *tree = new TTree("tree", "Bridging");

    Double_t CpuTime, RealTime;

    QwEvent* event = 0;
    tree->Branch("events", "QwEvent", &event);

    /// We loop over all requested events.
    while (treebuffer->GetNextEvent() == 0) {

      /// Get the generated event
      event = treebuffer->GetEvent();
      QwEventHeader header(treebuffer->GetRunNumber(),treebuffer->GetEventNumber());
      event->SetEventHeader(header);

      /// Get the partial tracks in the front and back region
      std::vector<QwPartialTrack*> tracks_r2 = treebuffer->GetPartialTracks(kRegionID2);
      std::vector<QwPartialTrack*> tracks_r3 = treebuffer->GetPartialTracks(kRegionID3);

      // Process the partial tracks in region 2 and region 3
      for (size_t i = 0; i < tracks_r2.size(); i++) {
        for (size_t j = 0; j < tracks_r3.size(); j++) {

          // Filter tracks based on parameters (TODO filter should go somewhere else)
          int status = trackfilter->Filter(tracks_r2.at(i), tracks_r3.at(j));
          if (status != 0) {
            QwMessage << "Track did not pass filter." << QwLog::endl;
            continue;
          }

          // Bridge using the lookup table
          timer.Start();
          status = matrixlookup->Bridge(tracks_r2.at(i), tracks_r3.at(j));
          timer.Stop();
          CpuTime = timer.CpuTime();
          RealTime = timer.RealTime();
          timer.Reset();
          if (status == 0) {
            event->AddTrackList(matrixlookup->GetListOfTracks());
            continue;
          } else QwMessage << "Matrix lookup: " << status << QwLog::endl;

          // Bridge using the ray tracer
          timer.Start();
          status = raytracer->Bridge(tracks_r2.at(i), tracks_r3.at(j));
          timer.Stop();
          CpuTime = timer.CpuTime();
          RealTime = timer.RealTime();
          timer.Reset();
          if (status == 0) {
            event->AddTrackList(raytracer->GetListOfTracks());
            continue;
          } else QwMessage << "Ray tracer: " << status << QwLog::endl;

        } // end of back track loop
      } // end of front track loop


      // Print the event
      if (debug) event->Print();

      // Write event to tree
      tree->Fill();


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

