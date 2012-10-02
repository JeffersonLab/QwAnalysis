// This is an example for testing the bridging code

// Qweak tracking headers
#include "QwOptionsTracking.h"
#include "QwParameterFile.h"
#include "QwBridgingTrackFilter.h"
#include "QwRayTracer.h"
#include "QwMatrixLookup.h"

#include "QwTreeEventBuffer.h"
#include "QwSubsystemArrayTracking.h"
//#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"

// Qweak event
#include "QwEvent.h"


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

  ///  Load the tracking detectors from file
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking(gQwOptions);
  detectors->ProcessOptions(gQwOptions);

  // Get detector geometry
  QwGeometry geometry = detectors->GetGeometry();

  /// Create a track filter
  QwBridgingTrackFilter* trackfilter = new QwBridgingTrackFilter();

  /// Create a lookup table bridging method
  QwMatrixLookup* matrixlookup = new QwMatrixLookup(gQwOptions);
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
  QwRayTracer* raytracer = new QwRayTracer(gQwOptions);

  /// Load the simulated event file
  QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer(geometry);
  treebuffer->ProcessOptions(gQwOptions);

  ///  Start loop over all runs
  while (treebuffer->OpenNextFile() == 0) {

    // Setup the output file for bridgingresults
    TString outputfilename = Form(getenv_safe_TString("QWSCRATCH") + "/rootfiles/QwSimBridge_%d.root", treebuffer->GetRunNumber());
    TFile *file = new TFile(outputfilename, "RECREATE", "Bridging result");
    file->cd();
    TTree *tree = new TTree("tree", "Bridging");

    QwEvent* event = 0;
    tree->Branch("events", "QwEvent", &event);

    /// We loop over all requested events.
    while (treebuffer->GetNextEvent() == 0) {

      /// Get the generated event
      event = treebuffer->GetCurrentEvent();
      QwEventHeader header(treebuffer->GetRunNumber(),treebuffer->GetEventNumber());
      event->SetEventHeader(header);

      /// Get the partial tracks in the front and back region
      std::vector<boost::shared_ptr<QwPartialTrack> > tracks_r2 = treebuffer->CreatePartialTracks(kRegionID2);
      std::vector<boost::shared_ptr<QwPartialTrack> > tracks_r3 = treebuffer->CreatePartialTracks(kRegionID3);

      // Process the partial tracks in region 2 and region 3
      for (size_t i = 0; i < tracks_r2.size(); i++) {
        for (size_t j = 0; j < tracks_r3.size(); j++) {

          // Filter tracks based on parameters (TODO filter should go somewhere else)
          int status = trackfilter->Filter(tracks_r2.at(i).get(), tracks_r3.at(j).get());
          if (status != 0) {
            QwMessage << "Track did not pass filter." << QwLog::endl;
            continue;
          }

          // Bridge using the lookup table
          timer.Start();
          const QwTrack* track1 = matrixlookup->Bridge(tracks_r2.at(i).get(), tracks_r3.at(j).get());
          timer.Stop();
          timer.Reset();
          if (track1) {
            event->AddTrack(track1);
            continue;
          } else QwMessage << "Matrix lookup failed." << QwLog::endl;

          // Bridge using the ray tracer
          timer.Start();
          const QwTrack* track2 = raytracer->Bridge(tracks_r2.at(i).get(), tracks_r3.at(j).get());
          timer.Stop();
          timer.Reset();
          if (track2) {
            event->AddTrack(track2);
            continue;
          } else QwMessage << "Ray tracer failed." << QwLog::endl;

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

