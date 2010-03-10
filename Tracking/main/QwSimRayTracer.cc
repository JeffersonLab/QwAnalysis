// This is an example for testing the bridging code in the qwtrajectory class

// Qweak Tracking headers
#include "QwOptionsTracking.h"
#include "QwParameterFile.h"
#include "Det.h"
#include "QwTrajectory.h"

int main (int argc, char* argv[]) {

    bool debug = false;

    // Create a timer
    TStopwatch timer;

  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwsimtracking.conf");
  // Define the command line options
  DefineOptionsTracking(gQwOptions);

  /// Now we setup the message logging facilities with the requested loglevels.
  if (gQwOptions.HasValue("QwLog.logfile"))
    gQwLog.InitLogFile(gQwOptions.GetValue<string>("QwLog.logfile"));
  gQwLog.SetFileThreshold(QwLog::QwLogLevel(gQwOptions.GetValue<int>("QwLog.loglevel-file")));
  gQwLog.SetScreenThreshold(QwLog::QwLogLevel(gQwOptions.GetValue<int>("QwLog.loglevel-screen")));

  /// We fill the search paths for the parameter files.
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");




    QwTrajectory* trajectory = new QwTrajectory();

    trajectory->LoadMagneticFieldMap();
    //trajectory->GenerateLookUpTable();

    trajectory->LoadMomentumMatrix();

    for (UInt_t runnumber =  (UInt_t) gQwOptions.GetIntValuePairFirst("run");
            runnumber <= (UInt_t) gQwOptions.GetIntValuePairLast("run");
            runnumber++) {

        // Load the simulated event file
        TString inputfilename = Form(TString(getenv("QWSCRATCH")) + "/data/QwSim_%d.root", runnumber);
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

        Int_t evtnum=0;
        TrackID = 0;
        for ( eventnumber  = gQwOptions.GetIntValuePairFirst("event");
                eventnumber <= gQwOptions.GetIntValuePairLast("event"); eventnumber++) {

            // vector to hold multi-hit
            std::vector<TVector3> startpoint;
            std::vector<TVector3> startpointdirection;
            std::vector<TVector3> endpoint;
            std::vector<TVector3> endpointdirection;

            // This is slow due to accessing the disk every time
            int status = trajectory->ReadSimPartialTrack(inputfilename, eventnumber,
                         &startpoint, &startpointdirection,
                         &endpoint, &endpointdirection);
            if (status == -1) {
                break;
            }

            evtnum++;

            // TODO process multi-hit
            for (size_t i=0; i<startpoint.size(); i++) {
                for (size_t j=0; j<endpoint.size(); j++) {
                    trajectory->SetStartAndEndPoints(startpoint[i], startpointdirection[i],
                                                     endpoint[j], endpointdirection[j]);
                    timer.Start();
                    status = trajectory->BridgeFrontBackPartialTrack();
                    timer.Stop();
                    CpuTime = timer.CpuTime();
                    RealTime = timer.RealTime();
                    timer.Reset();

                    if (status == 0) {
                        if (debug) {
                            std::cout<<"======>>>> Bridged a track"<<std::endl;
                            trajectory->PrintInfo();
                        }
                        TrackID++;
                        trajectory->GetBridgingResult(bridgingresult);
                        tree->Fill();
                    } else
                        if (debug) std::cout<<"======>>>> No luck on bridging this track."<<std::endl;


                } //end of j-loop
            } // end of i-loop


            if (evtnum%100==0) {
                tree->AutoSave();
                gDirectory->Purge();
                std::cout<<"Processed "<<evtnum<<" events"<<std::endl;
            }
        }

        outfile->Write(0, TObject::kOverwrite);
        outfile->Close();
        delete outfile;
    }

    delete trajectory;

    return 0;

}

