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
  stream << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";
  return stream;
}

int main (int argc, char* argv[])
{
    bool debug = false;

    // Create a timer
    TStopwatch timer;

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
    //detectors->GetSubsystem("R1")->LoadChannelMap("qweak_cosmics_hits.map");
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
    if (! QwMatrixLookup::LoadTrajMatrix(trajmatrix)) {
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
    for (UInt_t runnumber =  (UInt_t) gQwOptions.GetIntValuePairFirst("run");
                runnumber <= (UInt_t) gQwOptions.GetIntValuePairLast("run");
                runnumber++) {

        // Setup the output file
        TString outputfilename = Form(TString(getenv("QWSCRATCH")) + "/rootfiles/QwSimBridge_%d.root", runnumber);
        TFile *outfile = new TFile(outputfilename, "RECREATE", "Bridging result");
        outfile->cd();
        TTree *tree = new TTree("tree", "Bridging");
        Double_t bridgingresult[43];
        Int_t eventnumber, TrackID;
        Double_t CpuTime, RealTime;

        tree->Branch("eventnumber",&eventnumber,"eventnumber/I");
        tree->Branch("TrackID",&TrackID,"TrackID/I");

        tree->Branch("StartPositionX",&bridgingresult[0],"StartPositionX/D");
        tree->Branch("StartPositionY",&bridgingresult[1],"StartPositionY/D");
        tree->Branch("StartPositionZ",&bridgingresult[2],"StartPositionZ/D");
        tree->Branch("StartPositionR",&bridgingresult[3],"StartPositionR/D");
        tree->Branch("StartPositionPhi",&bridgingresult[4],"StartPositionPhi/D");
        tree->Branch("StartDirectionX",&bridgingresult[5],"StartDirectionX/D");
        tree->Branch("StartDirectionY",&bridgingresult[6],"StartDirectionY/D");
        tree->Branch("StartDirectionZ",&bridgingresult[7],"StartDirectionZ/D");
        tree->Branch("StartDirectionTheta",&bridgingresult[8],"StartDirectionTheta/D");
        tree->Branch("StartDirectionPhi",&bridgingresult[9],"StartDirectionPhi/D");

        tree->Branch("EndPositionX",&bridgingresult[10],"EndPositionX/D");
        tree->Branch("EndPositionY",&bridgingresult[11],"EndPositionY/D");
        tree->Branch("EndPositionZ",&bridgingresult[12],"EndPositionZ/D");
        tree->Branch("EndPositionR",&bridgingresult[13],"EndPositionR/D");
        tree->Branch("EndPositionPhi",&bridgingresult[14],"EndPositionPhi/D");
        tree->Branch("EndDirectionX",&bridgingresult[15],"EndDirectionX/D");
        tree->Branch("EndDirectionY",&bridgingresult[16],"EndDirectionY/D");
        tree->Branch("EndDirectionZ",&bridgingresult[17],"EndDirectionZ/D");
        tree->Branch("EndDirectionTheta",&bridgingresult[18],"EndDirectionTheta/D");
        tree->Branch("EndDirectionPhi",&bridgingresult[19],"EndDirectionPhi/D");

        tree->Branch("HitLocationX",&bridgingresult[20],"HitLocationX/D");
        tree->Branch("HitLocationY",&bridgingresult[21],"HitLocationY/D");
        tree->Branch("HitLocationZ",&bridgingresult[22],"HitLocationZ/D");
        tree->Branch("HitLocationR",&bridgingresult[23],"HitLocationR/D");
        tree->Branch("HitLocationPhi",&bridgingresult[24],"HitLocationPhi/D");
        tree->Branch("HitDirectionX",&bridgingresult[25],"HitDirectionX/D");
        tree->Branch("HitDirectionY",&bridgingresult[26],"HitDirectionY/D");
        tree->Branch("HitDirectionZ",&bridgingresult[27],"HitDirectionZ/D");
        tree->Branch("HitDirectionTheta",&bridgingresult[28],"HitDirectionTheta/D");
        tree->Branch("HitDirectionPhi",&bridgingresult[29],"HitDirectionPhi/D");

        tree->Branch("PositionROff",&bridgingresult[30],"PositionROff/D");
        tree->Branch("PositionPhiOff",&bridgingresult[31],"PositionPhiOff/D");
        tree->Branch("DirectionThetaOff",&bridgingresult[32],"DirectionThetaOff/D");
        tree->Branch("DirectionPhiOff",&bridgingresult[33],"DirectionPhiOff/D");
        tree->Branch("DeterminedMomentum",&bridgingresult[34],"DeterminedMomentum/D");
        tree->Branch("PreScatteringMomentum",&bridgingresult[35],"PreScatteringMomentum/D");
        tree->Branch("DeterminedQ2",&bridgingresult[36],"DeterminedQ2/D");

        tree->Branch("OriginVertexMomentum",&bridgingresult[37],"OriginVertexMomentum/D");
        tree->Branch("PrimaryQ2",&bridgingresult[38],"PrimaryQ2/D");

        tree->Branch("MomentumOff",&bridgingresult[39],"MomentumOff/D");
        tree->Branch("Q2_Off",&bridgingresult[40],"Q2_Off/D");
        tree->Branch("CrossSectionWeight",&bridgingresult[41],"CrossSectionWeight/D");

        tree->Branch("MatchFlag",&bridgingresult[42],"MatchFlag/D");
        tree->Branch("CpuTime",&CpuTime,"CpuTime/D");
        tree->Branch("RealTime",&RealTime,"RealTime/D");


        /// Load the simulated event file
        TString input = Form(TString(getenv("QWSCRATCH")) + "/data/QwSim_%d.root", runnumber);
        QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer (input, detector_info);
        treebuffer->EnableResolutionEffects();


        /// We loop over all requested events.
        Int_t entries = treebuffer->GetEntries();
        Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
        Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
        for (eventnumber  = eventnumber_min;
             eventnumber <= eventnumber_max &&
             eventnumber  < entries; eventnumber++) {

            /// Print event number
            QwMessage << "Event " << eventnumber << QwLog::endl;

            /// Read the event from the tree
            treebuffer->GetEntry(eventnumber);

            /// Get the partial tracks in the front and back region
            std::vector<QwPartialTrack*> tracks_r2 = treebuffer->GetPartialTracks(kRegionID2);
            std::vector<QwPartialTrack*> tracks_r3 = treebuffer->GetPartialTracks(kRegionID3);

            // Reset the track id
            TrackID = 0;


            // TODO process multi-hit
            // Take only one hit in each region for now by setting i<1 && j<1
            for (size_t i = 0; i < tracks_r2.size() && i < 1; i++) {
                for (size_t j = 0; j < tracks_r3.size() && j < 1; j++) {

                    // Determine the start and end points and directions
                    // TODO Parametrize to use a detector, not hardcoded (wrong!) position
                    TVector3 _startpoint = tracks_r2.at(i)->GetPosition(-330.685);
                    TVector3 _startpointdirection = tracks_r2.at(i)->GetMomentumDirection();
                    TVector3 _endpoint = tracks_r3.at(j)->GetPosition(439.625);
                    TVector3 _endpointdirection = tracks_r3.at(j)->GetMomentumDirection();

                    // Set the start and end points and directions
                    matrixlookup->SetStartAndEndPoints(_startpoint, _startpointdirection,
                                                     _endpoint, _endpointdirection);
                    raytracer->SetStartAndEndPoints(_startpoint, _startpointdirection,
                                                     _endpoint, _endpointdirection);

                    // Filter tracks based on parameters (TODO still in QwRayTracer)
                    int status = raytracer->Filter();
                    if (status == -1) {
                        QwMessage << "Track did not pass filter" << QwLog::endl;
                        continue;
                    }
                    status = -1;

                    // Bridge using the lookup table
                    if (status == -1) {
                        timer.Start();
                        status = matrixlookup->BridgeFrontBackPartialTrack();
                        timer.Stop();
                        CpuTime = timer.CpuTime();
                        RealTime = timer.RealTime();
                        timer.Reset();
                        if (status == 0) matrixlookup->GetBridgingResult(bridgingresult);
                        else QwMessage << "Matrix lookup: " << status << QwLog::endl;
                    }

                    // Bridge using the ray tracer
                    if (status == -1) {
                        timer.Start();
                        status = raytracer->BridgeFrontBackPartialTrack();
                        timer.Stop();
                        CpuTime = timer.CpuTime();
                        RealTime = timer.RealTime();
                        timer.Reset();
                        if (status == 0) raytracer->GetBridgingResult(bridgingresult);
                        else QwMessage << "Ray tracer: " << status << QwLog::endl;
                    }

                    if (status == 0) {
                        if (debug) {
                            std::cout<<"======>>>> Bridged a track"<<std::endl;
                            raytracer->PrintInfo();
                        }
                        TrackID++;
                        tree->Fill();
                    } else
                        if (debug) std::cout<<"======>>>> No luck on bridging this track."<<std::endl;


                } //end of j-loop
            } // end of i-loop


            if (eventnumber > 0 && eventnumber % 100 == 0) {
                tree->AutoSave();
                gDirectory->Purge();
                QwMessage << "Processed " << eventnumber << " events" << QwLog::endl;
            }

        } // end of the loop over events

        // Write output root file
        outfile->Write(0, TObject::kOverwrite);
        outfile->Close();
        delete outfile;

    } // end of the loop over runs

    // Delete objects
    delete raytracer;
    delete matrixlookup;

    return 0;

}

