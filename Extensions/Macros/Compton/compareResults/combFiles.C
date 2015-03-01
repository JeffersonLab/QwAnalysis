#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "readFortOut.C"
#include "getIndex.C"

//const Int_t runBegin = 24250, runEnd = 24450;
const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
Int_t combFiles(Int_t run1=runBegin, Int_t run2=runEnd)
{
  ifstream fIn;
  ofstream fCheck;
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

  fCheck.open("combFiles_excluded.txt");
  if(!fCheck.is_open()) cout<<" could not open combFiles_excluded.txt"<<endl;
  ///read in the DAQ (trig) info for all runs
  std::vector<Double_t> runDAQ, acTrigRun2;
  TString s1, s2, s3, s4, s5, s6, s7, s8, s9;
  Double_t d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
  char firmW[5]="0000";
  fIn.open("run2DAQ.info");
  if (fIn.is_open()) { 
    fIn>> s1 >>s2 >>s3 >>s4 >>s5 >>s6 >>s7 >>s8 >>s9;
    while(fIn>>tRun>>acTrig>>evTrig>>minWidth>>firmW>>pwtl1>>pwtl2>>holdOff>>pipelineDelay && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
      if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
      if(acTrig==2 || acTrig==3) {
        runDAQ.push_back((Double_t)tRun);
        acTrigRun2.push_back(acTrig);
      } else {
        //cout<<tRun<<"   "<<acTrig<<endl;
        fCheck<<tRun<<"\t"<<acTrig<<endl;
        continue;
      }
    }
    fIn.close();
  } else cout<<red<<"*** Could not open file with DAQ info"<<normal<<endl; 
  cout<<blue<<runDAQ.size()<<" runs found in DAQ file"<<normal<<endl;

  ///to read in the infoBeamLas.txt version written before 12Sep14
  std::vector<Double_t> rListBeam, maxBeam, bmRMS, bmMean, bmMeanEr, tripBeam, maxLas, nCycles, runTime;
  Double_t nBeamTrips =0.0, nLasCyc =0.0, nEntries =0.0, rms =0.0, mean =0.0, meanEr =0.0;
  //fIn.open("run2BeamLas.info");
  //file = "infoBeamLas_28Sep14.info";
  file = "infoBeamLas_Ver001.info";
  fIn.open(file);
  if(fIn.is_open()) {
    //while(fIn>>tRun>>beamMax>>nBeamTrips>>laserMax>>nLasCyc>>nEntries  && fIn.good()) {
    while(fIn>>tRun >>beamMax >>rms >>mean >>meanEr >>nBeamTrips >>laserMax >>nLasCyc >>nEntries  && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if the run exists in goodList
        if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
        rListBeam.push_back(tRun), maxBeam.push_back(beamMax), tripBeam.push_back(nBeamTrips);
        bmRMS.push_back(rms), bmMean.push_back(mean), bmMeanEr.push_back(meanEr);
        maxLas.push_back(laserMax), nCycles.push_back(nLasCyc), runTime.push_back(nEntries/helRate);
      }
    }
    fIn.close();
  } else cout<<red<<"couldn't open"<< file<<endl;
  cout<<blue<<rListBeam.size()<<" runs found in beam las info list"<<normal<<endl;

  ///to reach the scintillator rates
  std::vector<Double_t> rListScint, scintRate, scintRateEr, scintRMS;
  Double_t sci, sciEr, sciRMS;
  file = "scintRate_ver001_31Oct14.info";
  fIn.open(file);
  if(fIn.is_open()) {
    while(fIn >>tRun >>sci >>sciEr >>sciRMS && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
      if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
      rListScint.push_back(tRun);
      scintRate.push_back(sci);
      scintRateEr.push_back(sciEr);
      scintRMS.push_back(sciRMS);
    }
    fIn.close();
  } else cout<<red<<"couldn't open "<<file<<normal<<endl;
  cout<<blue<<rListScint.size()<<" runs found in scint rate file"<<normal<<endl;

    file = "beamProp_ver002_10Nov14.info";
    std::vector<Double_t> rListE, hWien, vWien, ihwp1set, ihwp1read, rhwp, ihwp2read, edetPos, meanE, meanE_RMS;
    fIn.open(file);
    if(fIn.is_open()) {
        //fBeamProp<<"run\tHWien\tVWien\t\tIHWP1set\tIHWP1read\tRHWP\tIHWP2read\tedetPos\tmeanE\tmeanE_RMS"<<endl;
        while(fIn>>d1 >>d2 >>d3 >>d4 >>d5 >>d6 >>d7 >>d8 >>d9 >>d10 && fIn.good()) {
            if(std::find(goodRuns.begin(), goodRuns.end(), d1) == goodRuns.end()) continue;
            if(d1<run1 || d1>run2) continue; ///the list needs only results between these runs
            //printf("%f\t%f\t%f\n",d1, d9, d10);
            rListE.push_back(d1);
            meanE.push_back(d9);
            meanE_RMS.push_back(d10);
        }
        fIn.close();
    } else cout<<red<<"failed to open "<<file<<normal<<endl;
    cout<<blue<<rListE.size()<<" runs found in "<<file<<normal<<endl;

  ///read in the polarization values from the version 1 file
  std::vector<Double_t> rListV1,polV1,polErV1,comptEdgeV1,chiSqV1,edgeErV1,goodCycV1;
  std::vector<Int_t> fitStat;
  ///write the lasCyc based polarization into one central file
  //fIn.open("polAc_k2parDT.info");
  //fIn.open("polAcCleanLCNoDT_29Sep14.info");
  //file = "polAcVer001_22Oct14.info";
  //file = "polAcVer002_29Oct14.info";
  //file = "polAcVer003_10Nov14.info";
  //file = "polAcVer004_2parDT_05Nov14.info";///only for range of stable scaler rates
  file = "polAcVer005_12Nov14.info";
  fIn.open(file);
  if (fIn.is_open()) { 
    //while(fIn>>tRun>>tPol>>tPolEr>>tChiSq>>tNDF>>CE>>CEEr>>effStrip>>effStripEr>>pl>>goodCyc && fIn.good()) {
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus>>goodCyc && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
      if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
        if(tPolEr>1.5 || tPolEr<0.06 || fabs(tPol)<80.0 || tCEEr<0.05) {
        fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
        continue;
      }
      rListV1.push_back((Double_t)tRun);
      polV1.push_back((tPol));
      polErV1.push_back(tPolEr);
      comptEdgeV1.push_back(tCE);
      edgeErV1.push_back(tCEEr);
      chiSqV1.push_back(tChiSq/tNDF);
      goodCycV1.push_back(goodCyc);
      fitStat.push_back(fitStatus);
      //} //else cout<<"for runum:"<<tRun<<", tPol:"<<tPol<<" +/- "<<tPolEr<<", CE:"<<comptEdge<<endl;
  }
  fIn.close();
  } else {
    cout<<red<<file<<" failed to open"<<normal<<endl;
    return -1;
  }
  cout<<blue<<rListV1.size()<<" runs found in "<<file<<normal<<endl;

  ///read in the polarization values from version 2 file
  std::vector<Double_t> rListV2,polV2,polErV2,comptEdgeV2,chiSqV2,edgeErV2,goodCycV2; 
  std::vector<Double_t > NDF_V2;
  //fIn.open("polAc_noDTRun2.info");
  //fIn.open("polAcCleanLC2parDT_28Sep14.info");///this analysis uses only clean LC
  //file = "polScVer001_22Oct14.info";///this analysis uses only clean LC
  //file = "polAcVer004_H0DT_05Nov14.info";///DT corr for H0 applied to both helicities
  //file = "polAcVer004_1parDT_05Nov14.info";
  //file = "polAcVer001_22Oct14.info";
  //file = "polAcVer002_29Oct14.info";
  file = "polScVer005_12Nov14.info";
  fIn.open(file);
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus >>goodCyc && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
        if(tPolEr>1.5 || tPolEr<0.06 || fabs(tPol)<80.0 || tCEEr<0.05) {
          fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
          continue;
        }
        rListV2.push_back(tRun);
        //polV2.push_back(fabs(tPol));
        polV2.push_back(tPol);
        polErV2.push_back(tPolEr);
        comptEdgeV2.push_back(tCE);
        edgeErV2.push_back(tCEEr);
        NDF_V2.push_back(tNDF);///if reading from file generated by me
        chiSqV2.push_back(tChiSq/tNDF);///if reading from file generated by me
        goodCycV2.push_back(goodCyc);
      } //else cout<<blue<<"in V2, tRun:"<<tRun<<", polV2:"<<tPol<<"+/-"<<tPolEr<<normal<<endl;
    }
    fIn.close();
  } else {
    cout<<red<<file<<" failed to open"<<normal<<endl;
    return -1;
  }
  cout<<blue<<rListV2.size()<<" runs found in "<<file<<normal<<endl;

  //Read in polarization from polSc_noDTRun2.info
  std::vector<Double_t> rListV3,polV3,polErV3,comptEdgeV3,chiSqV3,edgeErV3,goodCycV3; 
  std::vector<Double_t > NDF_V3;
  //fIn.open("polSc_noDTRun2.info");
  //fIn.open("polScCleanLCNoDT_28Sep14.info");///this analysis uses only clean LC for Sc mode data also
  //fIn.open("polAcCleanLCNoDT_29Sep14.info");
  file = "polAcNoDTVer001_25Oct14.info";
  //file = "polAcVer004_NoDT_05Nov14.info";
  fIn.open(file);
  if(fIn.is_open()) { //this is available only for pl 1
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus >>goodCyc && fIn.good()) {
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {///populate only if it exists in goodList
        if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
        if(tPolEr>1.5 || tPolEr<0.06 || fabs(tPol)<80.0 || tCEEr<0.05) {
          fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
          continue;
        }
        rListV3.push_back(tRun);
        polV3.push_back((tPol));
        polErV3.push_back(tPolEr);
        comptEdgeV3.push_back(tCE);
        edgeErV3.push_back(tCEEr);
        NDF_V3.push_back(tNDF);///if reading from file generated by me
        chiSqV3.push_back(tChiSq/tNDF);///if reading from file generated by me
        goodCycV3.push_back(goodCyc);
      } //else cout<<blue<<"in V3, tRun:"<<tRun<<", polV3:"<<tPol<<"+/-"<<tPolEr<<normal<<endl;
    }
    fIn.close();
  } else {
    cout<<red<<file<<" failed to open"<<normal<<endl;
    return -1;
  }
  cout<<blue<<rListV3.size()<<" runs found in "<<file<<normal<<endl;
  fCheck.close();
  
  //////////////// Having read in all relevant files, Lets write one big file but double checking the run#a
  //I have 7 major lists, which needs to be synchronized while making the combined file
  //rListV1  : info of pol% of Ac_2parDT or most recent
  //rListV2  : info of pol% of Sc (noDT) or most basic
  //rListV3  : info of pol% of Ac_1parDT or intermediate step
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
  //file = "combFiles_ver001_02Nov14.info";
  //file = "combFiles_ver004_05Nov14.info";
  //file = "combFiles_ver001_06Nov14.info";
  //file = "combFiles_ver001_11Nov14.info";
  file = "combFiles_ver005_12Nov14.info";
  ofstream fOut;
  fOut.open(file);
  if (fOut.is_open()) {
    //fOut<<"run\tpolAcCleanNoDT  Er\tpolAcClean2parDT  Er\tpolScCleanNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub\tbmMean\tbmRMS"<<endl;
    //fOut<<"run\tpolAcFixedE  Er\tpolAcVariableE  Er\tpolAcNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;///regular case
    fOut<<"run\tpolAc  Er\tpolSc  Er\tpolAcNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;///regular case
    //fOut<<"run\tpolAc  Er\tpolH0DT  Er\tpolAcNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;//temp:
    //fOut<<"run\tpolAc  Er\tpol1parDT  Er\tpolAcNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;//temp:
    //fOut<<"run\tpolAc2parDT  Er\tpolScNoDT  Er\tacTrig\tp4On\tp1ScH1BgdSub"<<endl;
    for(int r1=0; r1<(int)goodRuns.size(); r1++) {// Good list
      int r2, r3, r4, r5, r6, r7, r8, r9;
      int run = (int)goodRuns[r1];
      if(getIndex(run, runDAQ, r2) && getIndex(run, rListV1, r3) && getIndex(run, rListV2, r4) && getIndex(run, runScR, r5) && getIndex(run, rListV3, r6) && getIndex(run, rListP4, r7) && getIndex(run, rListScint, r8) && getIndex(run, rListE, r9)) {
        finalList.push_back(goodRuns.at(r1));
        fOut<<Form("%5.0f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%5.0f\t%.0f\t%.0f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\n", goodRuns.at(r1), polV1.at(r3), polErV1.at(r3), polV2.at(r4), polErV2.at(r4), polV3.at(r6), polErV3.at(r6), acTrigRun2.at(r2), rateScP4H1L1.at(r7), ratesScH1L1.at(r5) - ratesScH1L0.at(r5), scintRate.at(r8), scintRateEr.at(r8), scintRMS.at(r8), meanE.at(r9));
      }
    }
    fOut.close();
    cout<<"wrote "<<file<<endl;
  } else cout<<red<<"could not open a "<<file<<normal<<endl;
  cout<<blue<<finalList.size()<<" runs in the final list"<<normal<<endl;
  return 1;
  }
