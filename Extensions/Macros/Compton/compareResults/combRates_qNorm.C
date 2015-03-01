#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "readFortOut.C"

//const Int_t runBegin = 24000, runEnd = 24500;
const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
Int_t combRates_qNorm(Int_t run1=runBegin, Int_t run2=runEnd)
{
  ifstream fIn;
  TString file;
  TString ver1 = "Ac_noDT", ver2 = "Ac_2parDT";
  std::vector<Int_t> dumI;///dummy integer vector
  std::vector<Double_t> dumD;///dummy double vector

  ///read in the good runs' list
  Double_t tRun,tYield,tYieldEr;
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
  TString s1, s2, s3, s4, s5, s6, s7, s8, s9;
  char firmW[5]="0000";
  fIn.open("run2DAQ.info");
  if (fIn.is_open()) { 
    fIn>> s1 >>s2 >>s3 >>s4 >>s5 >>s6 >>s7 >>s8 >>s9;
    while(fIn>>tRun>>acTrig>>evTrig>>minWidth>>firmW>>pwtl1>>pwtl2>>holdOff>>pipelineDelay && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        runDAQ.push_back((Double_t)tRun);
        acTrigRun2.push_back(acTrig);
      }
    }
    fIn.close();
  } else cout<<red<<"*** Could not open file with DAQ info"<<normal<<endl; 
  cout<<blue<<runDAQ.size()<<" runs found in DAQ file"<<normal<<endl;

  ///read in the Rates from the version 1 file
  std::vector<Double_t> rListV1,yieldV1,yieldErV1;
  std::vector<Int_t> fitStat;
  ///write the lasCyc based polarization into one central file
  //fIn.open("polAc_k2parDT.info");
  //fIn.open("qNormAggAcYield_noDT_01Oct14.info");///Ac_noDT
  //fIn.open("tNormAggAcRateB1L0_noDT_01Oct14.info");///Ac_noDT//B1L0
  file = "aggAcYield_ver001_29Oct14.info";
  fIn.open(file);
  if (fIn.is_open()) { 
    //while(fIn>>tRun>>tYield>>tYieldEr>>tChiSq>>tNDF>>CE>>CEEr>>effStrip>>effStripEr>>pl>>goodCyc && fIn.good()) {
    while(fIn >>tRun >>tYield >>tYieldEr && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        rListV1.push_back((Double_t)tRun);
        yieldV1.push_back(fabs(tYield));
        yieldErV1.push_back(tYieldEr);
      } //else cout<<"for runum:"<<tRun<<", tYield:"<<tYield<<" +/- "<<tYieldEr<<", CE:"<<comptEdge<<endl;
    }
    fIn.close();
  } else cout<<red<<"*** Could not open file with V1 tYield info"<<normal<<endl; 
  cout<<blue<<rListV1.size()<<" runs found in "+ver1<<normal<<endl;

  ///read in the Rates from version 2 file
  std::vector<Double_t> rListV2,yieldV2,yieldErV2;
  std::vector<Double_t > NDF_V2;
  //fIn.open("polAc_noDTRun2.info");
  //fIn.open("tNormAggAcRateB1L1_noDT_01Oct14.info");///Ac_2parDT//B1L1
  file = "aggScYield_ver001_29Oct14.info";
  fIn.open(file);
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tYield >>tYieldEr && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        rListV2.push_back(tRun);
        yieldV2.push_back(fabs(tYield));
        yieldErV2.push_back(tYieldEr);
      } //else cout<<blue<<"in V2, tRun:"<<tRun<<", yieldV2:"<<tYield<<"+/-"<<tYieldEr<<normal<<endl;
    }
    fIn.close();
  } else cout<<file<<" failed to open"<<endl;
  cout<<blue<<rListV2.size()<<" runs found in analyzed runs of "<<ver2<<normal<<endl;

  //Read in Rates from *_noDTRun2.info
  std::vector<Double_t> rListV3,yieldV3,yieldErV3;
  std::vector<Double_t > NDF_V3;
  //fIn.open("polSc_noDTRun2.info");
  //fIn.open("tNormAggScRateB1L0_noDT_01Oct14.info");///Sc_noDT
  file = "aggAcNoDTYield_ver001_29Oct14.info";
  fIn.open(file);
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tYield >>tYieldEr && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        rListV3.push_back(tRun);
        yieldV3.push_back(fabs(tYield));
        yieldErV3.push_back(tYieldEr);
      } //else cout<<blue<<"in V3, tRun:"<<tRun<<", yieldV3:"<<tYield<<"+/-"<<tYieldEr<<normal<<endl;
    }
    fIn.close();
  } else cout<<file<<" failed to open"<<endl;
  cout<<blue<<rListV3.size()<<" runs found in analyzed runs of Scalar"<<normal<<endl;

  file = "aggAcNoDTqNormCntsB1H1L1P1_ver001_29Oct14.info";
  std::vector<Double_t> dRun, dRate, dEr;
  fIn.open(file);
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tYield >>tYieldEr && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
      dRun.push_back(tRun);
      dRate.push_back(fabs(tYield));
      dEr.push_back(tYieldEr);
      //cout<<blue<<"tRun:"<<tRun<<", yield:"<<tYield<<"+/-"<<tYieldEr<<normal<<endl;
    }
    fIn.close();
  } else cout<<file<<" failed to open"<<endl;
  cout<<blue<<dRun.size()<<" runs found in "<<file<<normal<<endl;

  //////////////// Having read in all relevant files, Lets write one big file but double checking the run#a
  //I have 7 major lists, which needs to be synchronized while making the combined file
  //rListV1  : information of pol% of Ac_noDT
  //rListV2  : information of pol% of Ac_2parDT
  //rListV3  : info of pol% of Sc (noDT)
  //rListP4  : plane4 aggregate rate
  //runDAQ     : trig and DAQ info
  //rListBeam: beam max and beam trip info
  //runScR     : plane1 aggregate rates in Scaler mode

  std::vector<Double_t>finalList;
  ofstream fOut;
  //fOut.open("combqNormRates_01Oct14.info");
  //file = "combRates_ver001_29Oct14.info";
  file = "combRates_ver001_02Nov14.info";
  fOut.open(file);
  if (fOut.is_open()) {
    //fOut<<"run\tpolAcCleanNoDT  Er\tpolAcClean2parDT  Er\tpolScCleanNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub\tbeamMean\tbeamRMS"<<endl;
    fOut<<"run\tyieldAc  Er\tyieldSc  Er\tyieldAcNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;
    for(int r1=0; r1<(int)goodRuns.size(); r1++) {// Good list
      for(int r2=0; r2<(int)runDAQ.size(); r2++) {// DAQ runs
        if(goodRuns.at(r1) == runDAQ.at(r2)) {
          for(int r3=0; r3<(int)rListV1.size(); r3++) {//Ac_withDT
            if(goodRuns.at(r1) == rListV1.at(r3)) {
              for(int r4=0; r4<(int)rListV2.size(); r4++) {//Sc_noDT
                if(goodRuns.at(r1) == rListV2.at(r4)) {
                  for(int r5=0; r5<(int)runScR.size(); r5++) {//aggregate rate Sc
                    if(goodRuns.at(r1) == runScR.at(r5)) {
                      for(int r6=0; r6<(int)rListV3.size(); r6++) {//Ac_noDT
                        if(goodRuns.at(r1) == rListV3.at(r6)) {
                          for(int r7=0; r7<(int)rListP4.size(); r7++) {//
                            if(goodRuns.at(r1) == rListP4.at(r7)) {
                              for(int r8=0; r8<(int)dRun.size(); r8++) {
                                if(goodRuns.at(r1) == dRun.at(r8)) {
                                  finalList.push_back(goodRuns.at(r1));
                                  fOut<<Form("%5.0f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%5.0f\t%.0f\t%.0f\t%f\t%f\n", goodRuns.at(r1), yieldV1.at(r3), yieldErV1.at(r3), yieldV2.at(r4), yieldErV2.at(r4), yieldV3.at(r6), yieldErV3.at(r6), acTrigRun2.at(r2), rateScP4H1L1.at(r7), ratesScH1L1.at(r5) - ratesScH1L0.at(r5), dRate.at(r8), dEr.at(r8));
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
      }
    }
    fOut.close();
    cout<<"wrote "<<file<<endl;
  } else cout<<red<<"could not open "<<file <<normal<<endl;
  cout<<blue<<finalList.size()<<" runs in the final list"<<normal<<endl;
  return 1;
  }
  //      if(std::find(runDAQ.begin(), runDAQ.end(), goodRun.at(r)) != runDAQ.end() && 
  //        std::find(rListBeam.begin(), rListBeam.end(), goodRun.at(r) != rListBeam.end() &&
