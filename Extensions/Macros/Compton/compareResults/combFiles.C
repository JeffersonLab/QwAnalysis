#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "readFortOut.C"

//const Int_t runBegin = 24000, runEnd = 24500;
const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
Int_t combFiles(Int_t run1=runBegin, Int_t run2=runEnd)
{
  ifstream fIn;
  TString file;
  //TString ver1 = "Ac_noDT", ver2 = "Ac_2parDT";
  TString ver1 = "Ac_2parDT", ver2 = "Sc_noDT";
  std::vector<Int_t> dumI;///dummy integer vector
  std::vector<Double_t> dumD;///dummy double vector

  ///read in the good runs' list
  Double_t tRun,tPol,tPolEr,tChiSq,tCE,tCEEr;
  Int_t pl,goodCyc, fitStatus;
  Int_t tNDF;
  std::vector<Double_t> goodRuns;
  fIn.open("/w/hallc/compton/users/narayan/my_scratch/data/goodList_longPol.txt");
  if(fIn.is_open()) {
    while(fIn>>tRun && fIn.good()) {
      goodRuns.push_back(tRun);
    }
    fIn.close();
  } else cout<<red<<"couldn't open the list of good runs"<<normal<<endl;
  cout<<blue<<goodRuns.size()<<" runs found in the goodList"<<endl;

  ///read in Scaler aggregate rate file
  std::vector<Double_t> ratesScH1L1, ratesScH1L0, ratesScH0L1, ratesScH0L0, runScR;
  file ="/w/hallc/compton/users/narayan/my_scratch/data/aggrate_run2.dat";
  readFortOut(file,runScR,ratesScH1L1,ratesScH1L0,ratesScH0L1,ratesScH0L0,run1,run2);
  cout<<blue<<ratesScH1L1.size()<<" runs in the aggregate Sc list"<<normal<<endl;

  ///Read in the p4 rate file to find out if it was 2/3 or 2/4 etc
  std::vector<Double_t> rListP4, rateScP4H1L1, rateScP4H1L0, rateScP4H0L1, rateScP4H0L0;
  file ="/w/hallc/compton/users/narayan/my_scratch/data/p4sc_run2.dat";
  readFortOut(file,rListP4,rateScP4H1L1,rateScP4H1L0,rateScP4H0L1,rateScP4H0L0,run1,run2);
  cout<<blue<<rateScP4H1L1.size()<<" runs in the Plane4 Sc list "<<normal<<endl;

  ///read in the DAQ (trig) info for all runs
  std::vector<Double_t> runDAQ, acTrigRun2;
  char firmW[5]="0000";
  fIn.open("run2DAQ.info");
  if (fIn.is_open()) { 
    while(fIn>>tRun>>acTrig>>evTrig>>minWidth>>firmW>>pwtl1>>pwtl2>>holdOff>>pipelineDelay && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if the run exists in goodList
        runDAQ.push_back((Double_t)tRun);
        acTrigRun2.push_back(acTrig);
      }
    }
    fIn.close();
  } else cout<<red<<"*** Could not open file with DAQ info"<<normal<<endl; 
  cout<<blue<<runDAQ.size()<<" runs found in DAQ file"<<normal<<endl;

  ///to read in the infoBeamLas.txt version written before 12Sep14
  std::vector<Double_t> rListBeam, maxBeam, bmRMS, bmMean, bmMeanEr, tripBeam, maxLas, nCycles, runTime;
  Double_t runD =0.0, nBeamTrips =0.0, nLasCyc =0.0, nEntries =0.0, rms =0.0, mean =0.0, meanEr =0.0;
  //fIn.open("run2BeamLas.info");
  //file = "infoBeamLas_28Sep14.info";
  file = "infoBeamLas_Ver001.info";
  fIn.open(file);
  if(fIn.is_open()) {
    //while(fIn>>runD>>beamMax>>nBeamTrips>>laserMax>>nLasCyc>>nEntries  && fIn.good()) {
    while(fIn>>runD >>beamMax >>rms >>mean >>meanEr >>nBeamTrips >>laserMax >>nLasCyc >>nEntries  && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), runD) != goodRuns.end()) {///populate only if the run exists in goodList
        rListBeam.push_back(runD), maxBeam.push_back(beamMax), tripBeam.push_back(nBeamTrips);
        bmRMS.push_back(rms), bmMean.push_back(mean), bmMeanEr.push_back(meanEr);
        maxLas.push_back(laserMax), nCycles.push_back(nLasCyc), runTime.push_back(nEntries/helRate);
      }
    }
    fIn.close();
  } else cout<<red<<"couldn't open"<< file<<endl;
  cout<<blue<<rListBeam.size()<<" runs found in beam las info list"<<normal<<endl;

  ///to reach the scintillator rates
  std::vector<Double_t> rListScint, scintRate, scintRateEr;
  Double_t scint, scintEr, scintRMS;
  file = "scintRate_14Oct14.info";
  fIn.open(file);
  if(fIn.is_open()) {
    while(fIn >>runD >>scint >>scintEr >>scintRMS && fIn.good()) {
      rListScint.push_back(runD);
      scintRate.push_back(scint);
      scintRateEr.push_back(scintEr);
    }
    fIn.close();
  } else cout<<red<<"couldn't open "<<file<<normal<<endl;
  cout<<blue<<rListScint.size()<<" runs found in scint rate file"<<normal<<endl;

  ///read in the polarization values from the version 1 file
  std::vector<Double_t> rListV1,polV1,polErV1,comptEdgeV1,chiSqV1,edgeErV1,goodCycV1;
  std::vector<Int_t> fitStat;
  ///write the lasCyc based polarization into one central file
  //fIn.open("polAc_k2parDT.info");
  //fIn.open("polAcCleanLCNoDT_29Sep14.info");
  fIn.open("polAcVer001_22Oct14.info");
  if (fIn.is_open()) { 
    //while(fIn>>tRun>>tPol>>tPolEr>>tChiSq>>tNDF>>CE>>CEEr>>effStrip>>effStripEr>>pl>>goodCyc && fIn.good()) {
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus>>goodCyc && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        rListV1.push_back((Double_t)tRun);
        polV1.push_back(fabs(tPol));
        polErV1.push_back(tPolEr);
        comptEdgeV1.push_back(tCE);
        edgeErV1.push_back(tCEEr);
        chiSqV1.push_back(tChiSq/tNDF);
        goodCycV1.push_back(goodCyc);
        fitStat.push_back(fitStatus);
        //chiSqV1.push_back(tChiSq);
      } //else cout<<"for runum:"<<tRun<<", tPol:"<<tPol<<" +/- "<<tPolEr<<", CE:"<<comptEdge<<endl;
    }
    fIn.close();
  } else cout<<red<<"*** Could not open file with V1 tPol info"<<normal<<endl; 
  cout<<blue<<rListV1.size()<<" runs found in "+ver1<<normal<<endl;

  ///read in the polarization values from version 2 file
  std::vector<Double_t> rListV2,polV2,polErV2,comptEdgeV2,chiSqV2,edgeErV2,goodCycV2; 
  std::vector<Double_t > NDF_V2;
  //fIn.open("polAc_noDTRun2.info");
  //fIn.open("polAcCleanLC2parDT_28Sep14.info");///this analysis uses only clean LC
  fIn.open("polScVer001_22Oct14.info");///this analysis uses only clean LC
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus >>goodCyc && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        rListV2.push_back(tRun);
        polV2.push_back(fabs(tPol));
        polErV2.push_back(tPolEr);
        comptEdgeV2.push_back(tCE);
        edgeErV2.push_back(tCEEr);
        NDF_V2.push_back(tNDF);///if reading from file generated by me
        chiSqV2.push_back(tChiSq/tNDF);///if reading from file generated by me
        goodCycV2.push_back(goodCyc);
      } //else cout<<blue<<"in V2, tRun:"<<tRun<<", polV2:"<<tPol<<"+/-"<<tPolEr<<normal<<endl;
    }
    fIn.close();
  } else cout<<"*** Alert: could not open runlet based pol file"<<endl;
  cout<<blue<<rListV2.size()<<" runs found in analyzed runs of "<<ver2<<normal<<endl;

  //Read in polarization from polSc_noDTRun2.info
  std::vector<Double_t> rListV3,polV3,polErV3,comptEdgeV3,chiSqV3,edgeErV3,goodCycV3; 
  std::vector<Double_t > NDF_V3;
  //fIn.open("polSc_noDTRun2.info");
  //fIn.open("polScCleanLCNoDT_28Sep14.info");///this analysis uses only clean LC for Sc mode data also
  //fIn.open("polAcCleanLCNoDT_29Sep14.info");
  file = "polAcNoDTVer001_25Oct14.info";
  fIn.open(file);
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus >>goodCyc && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        rListV3.push_back(tRun);
        polV3.push_back(fabs(tPol));
        polErV3.push_back(tPolEr);
        comptEdgeV3.push_back(tCE);
        edgeErV3.push_back(tCEEr);
        NDF_V3.push_back(tNDF);///if reading from file generated by me
        chiSqV3.push_back(tChiSq/tNDF);///if reading from file generated by me
        goodCycV3.push_back(goodCyc);
      } //else cout<<blue<<"in V3, tRun:"<<tRun<<", polV3:"<<tPol<<"+/-"<<tPolEr<<normal<<endl;
    }
    fIn.close();
  } else cout<<"*** Alert: could not open runlet based pol file"<<endl;
  cout<<blue<<rListV3.size()<<" runs found in analyzed runs of Scalar"<<normal<<endl;

  //////////////// Having read in all relevant files, Lets write one big file but double checking the run#a
  //I have 7 major lists, which needs to be synchronized while making the combined file
  //rListV1  : information of pol% of Ac_noDT
  //rListV2  : information of pol% of Ac_2parDT
  //rListV3  : info of pol% of Sc (noDT)
  //runDAQ     : trig and DAQ info
  //rListBeam: beam max and beam trip info
  //runScR     : plane1 aggregate rates in Scaler mode
  //rListP4    : plane4 aggregate rate

  //!for test
  //for(int i=0; i<(int)rListV1.size()/30; i++) {
  //  //cout<<rListV1.at(i)<<endl;
  //  cout<<runDAQ.at(i)<<endl;
  //}

  std::vector<Double_t>finalList;
  //file = "combFiles_03Oct14.info";
  file = "combFiles_Ver001.txt";
  ofstream fOut;
  fOut.open(file);
  if (fOut.is_open()) {
    //fOut<<"run\tpolAcCleanNoDT  Er\tpolAcClean2parDT  Er\tpolScCleanNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub\tbmMean\tbmRMS"<<endl;
    fOut<<"run\tpolAc  Er\tpolSc  Er\tpolAcNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;
    //fOut<<"run\tpolAc2parDT  Er\tpolScNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;
    for(int r1=0; r1<(int)goodRuns.size(); r1++) {// Good list
      for(int r2=0; r2<(int)runDAQ.size(); r2++) {// DAQ runs
        if(goodRuns.at(r1) == runDAQ.at(r2)) {
          for(int r3=0; r3<(int)rListV1.size(); r3++) {//Ac_noDT
            if(goodRuns.at(r1) == rListV1.at(r3)) {
              for(int r4=0; r4<(int)rListV2.size(); r4++) {//Ac_2parDT
                if(goodRuns.at(r1) == rListV2.at(r4)) {
                  for(int r5=0; r5<(int)runScR.size(); r5++) {//aggregate rate Sc
                    if(goodRuns.at(r1) == runScR.at(r5)) {
                      for(int r6=0; r6<(int)rListV3.size(); r6++) {//Sc_noDT
                        if(goodRuns.at(r1) == rListV3.at(r6)) {
                          for(int r7=0; r7<(int)rListP4.size(); r7++) {//plane4 aggregate rates
                            if(goodRuns.at(r1) == rListP4.at(r7)) {
                              finalList.push_back(goodRuns.at(r1));
                              fOut<<Form("%5.0f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%5.0f\t%.0f\t%.0f\n", goodRuns.at(r1), polV1.at(r3), polErV1.at(r3), polV2.at(r4), polErV2.at(r4), polV3.at(r6), polErV3.at(r6), acTrigRun2.at(r2), rateScP4H1L1.at(r7), ratesScH1L1.at(r5) - ratesScH1L0.at(r5));
                              //cout<<Form("%5.0f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%5.0f\t%.0f\t%.0f\n", goodRuns.at(r1), polV1.at(r3), polErV1.at(r3), polV2.at(r4), polErV2.at(r4), polV3.at(r6), polErV3.at(r6), acTrigRun2.at(r2), rateScP4H1L1.at(r7), ratesScH1L1.at(r5) - ratesScH1L0.at(r5));
                              break;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    fOut.close();
    cout<<"wrote "<<file<<endl;
  } else cout<<red<<"could not open a "<<file<<normal<<endl;
  cout<<blue<<finalList.size()<<" runs in the final list"<<normal<<endl;
  return 1;
  }

  //      if(std::find(runDAQ.begin(), runDAQ.end(), goodRun.at(r)) != runDAQ.end() && 
  //        std::find(rListBeam.begin(), rListBeam.end(), goodRun.at(r) != rListBeam.end() &&
