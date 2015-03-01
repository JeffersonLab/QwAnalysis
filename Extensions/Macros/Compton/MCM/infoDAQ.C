#ifndef __INFODAQ_F
#define __INFODAQ_F
#include "rootClass.h"
#include "comptonRunConstants.h"
#include "ValueLookup.C"

Int_t infoDAQ(Int_t runnum)
{
  cout<<blue<<"\nStarting into infoDAQ.C **************\n"<<normal<<endl;
  gSystem->mkdir(Form("%s/%s/r%d",pPath, txt,runnum));
  filePre = Form(filePrefix,runnum,runnum);
  const Bool_t debug = 0;
  TString erBeamVar = "";
  TString erDAQVar = "";
  Double_t bMask[nPlanes][nStrips];
  Double_t mask[nPlanes][nStrips];
  Double_t bAcTrigSlave[nModules],bEvTrigSlave[nModules],bMinWidthSlave[nModules],bFirmwareRevSlave[nModules],bPWTLSlave[nModules],bPWTL2Slave[nModules],bHoldOffSlave[nModules],bPipelineDelaySlave[nModules];
  Double_t rawE, ibcm1;

  ofstream flagsfile,debugDAQ,infoStripMask,fBeamProp;
  //TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_Pass2b_%d.000.root",runnum));//ensure to read in only the first runlet

  TChain *slowChain = new TChain("Slow_Tree");//to scan through the entire run if a parameter changed midway
  Int_t slowExists = slowChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  if(slowChain ==0) {/// exit if rootfiles do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist at "<<Form("$QW_ROOTFILES")<<"***\n\n"<<endl;
    return -1;
  } else cout<<"Attached "<<slowExists<<" files to slow chain for Run # "<<runnum<<endl;

  Double_t hWien, vWien, ihwp1set, rhwp, ihwp1read, ihwp2read, hccedpos;
  //Double_t hallcPhI,hallcWaveL;
  //Double_t weinAngle,qeHallA,qeHallB,qeHallC;
  //Double_t hallcTrans,

  slowChain->SetBranchStatus("*",1);
  //slowChain->SetBranchAddress("HWienAngle",&hWienAngle);
  //slowChain->SetBranchAddress("VWienAngle",&vWienAngle);
  //slowChain->SetBranchAddress("IGL1I00OD16_16",&ihwp1set);
  //slowChain->SetBranchAddress("IGL1I00DI24_24M",&ihwp1read);
  //slowChain->SetBranchAddress("psub_pl_pos",&rhwp);
  //slowChain->SetBranchAddress("IGL1I00DIOFLRD",&ihwp2read);
  //slowChain->SetBranchAddress("hallc_photocurrent",&hallcPhI);
  ////slowChain->SetBranchAddress("hallb_transmission",&);
  //slowChain->SetBranchAddress("laser_c_wavelength",&hallcWaveL);
  //slowChain->SetBranchAddress("WienAngle",&weinAngle);
  //slowChain->SetBranchAddress("qe_halla",&qeHallA);
  //slowChain->SetBranchAddress("qe_hallb",&qeHallB);
  //slowChain->SetBranchAddress("qe_hallc",&qeHallC);
  //slowChain->SetBranchAddress("hallc_transmission",&hallcTrans);
  //slowChain->SetBranchAddress("HCCEDPOS",&hccedpos);
  slowChain->SetBranchAddress("HALLC_p", &rawE);
  slowChain->SetBranchAddress("ibcm1", &ibcm1);

  TH1D *hCheck = new TH1D("hCheck","dummy",100,-0.0,0.1);//typical value of maximum beam current

  Double_t scintRate, scintRateRMS, scintRateEr;
  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("cComptScintRateNorm>>hCheck","","goff");
  scintRate = hCheck->GetMean();
  scintRateRMS = hCheck->GetRMS();
  scintRateEr = hCheck->GetMeanError();
  hCheck->Reset();
  ///an additional check
  Double_t maxScintRate = slowChain->GetMaximum("cComptScintRateNorm");
  Double_t minScintRate = slowChain->GetMinimum("cComptScintRateNorm");
  if((maxScintRate - minScintRate) > 20*scintRateRMS) {
    cout<<red<<"the scintillator rate varied significantly during the run:"<<maxScintRate<<"\t"<<minScintRate<<normal<<endl;
    erBeamVar += "\tscintRate";
  }
  
  Double_t setDipoleI, setDipoleIRMS, setDipoleIEr, maxSetDipoleI, minSetDipoleI;
  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("MCP3P01>>hCheck","","goff");
  setDipoleI = hCheck->GetMean();
  setDipoleIRMS = hCheck->GetRMS();
  setDipoleIEr = hCheck->GetMeanError();
  hCheck->Reset();
  ///an additional check
  maxSetDipoleI = slowChain->GetMaximum("MCP3P01");
  minSetDipoleI = slowChain->GetMinimum("MCP3P01");
  //if((maxSetDipoleI - minSetDipoleI) > 50000000.0) {
  //  erBeamVar += "\tdipoleISetValue";
  //  cout<<red<<"the dipole set current changed during the run:"<<maxSetDipoleI<<"\t"<<minSetDipoleI<<normal<<endl;
  //}
  //cout<<red<<maxSetDipoleI<<"\t"<<minSetDipoleI<<normal<<endl;

  ///These operations are now shifted into ValueLookup.C
  Double_t dipoleI, dipoleIRMS, dipoleIEr, maxDipoleI, minDipoleI;
  //hCheck->SetBit(TH1::kCanRebin);
  //slowChain->Draw("MCP3P01M>>hCheck","","goff");
  //dipoleI = hCheck->GetMean();
  //dipoleIRMS = hCheck->GetRMS();
  //dipoleIEr = hCheck->GetMeanError();
  //hCheck->Reset();
  maxDipoleI = slowChain->GetMaximum("MCP3P01M");
  minDipoleI = slowChain->GetMinimum("MCP3P01M");
  if((maxDipoleI - minDipoleI)>1000000.0) {
    erBeamVar += "\tdipoleIValue";
    cout<<"the dipole readback current changed during the run:"<<maxDipoleI<<"\t"<<minDipoleI<<normal<<endl;///false alarm
  }
  ValueLookup(runnum, dipoleI, dipoleIEr, dipoleIRMS);

  //if((setDipoleI - dipoleI) > 1000000) {///this often gives a false alarm
  //  erBeamVar += "\tSetReadDipoleDisagree";
  //  cout<<red<<"set and read back dipole current disagree"<<normal<<endl;
  //}

  Double_t maxEnergyLockFB = slowChain->GetMaximum("FB_C_FB_on");
  Double_t minEnergyLockFB = slowChain->GetMinimum("FB_C_FB_on");
  cout<<"Fast feedback on energy lock max/min: "<<maxEnergyLockFB<<"\t"<<minEnergyLockFB<<endl;
  if(maxEnergyLockFB != minEnergyLockFB) erBeamVar += "\tenergyLock";

  Double_t maxPosLockFB = slowChain->GetMaximum("FB_C_use_RF");
  Double_t minPosLockFB = slowChain->GetMinimum("FB_C_use_RF");
  cout<<"Fast feedback on position lock max/min: "<<maxPosLockFB<<"\t"<<minPosLockFB<<endl;
  if(maxPosLockFB != minPosLockFB) erBeamVar += "\tpositionLock";

  Double_t maxHwien = slowChain->GetMaximum("HWienAngle");
  Double_t minHwien = slowChain->GetMinimum("HWienAngle");
  cout<<"hwien max/min: "<<maxHwien<<"\t"<<minHwien<<endl;
  if(maxHwien == minHwien) hWien=maxHwien;
  else {
    erBeamVar += "\thWien";
    hWien = 1234.0;
  }

  Double_t maxVwien = slowChain->GetMaximum("VWienAngle");
  Double_t minVwien = slowChain->GetMinimum("VWienAngle");
  cout<<"vwien max, min: "<<maxVwien<<"\t"<<minVwien<<endl;
  if(maxVwien == minVwien) vWien = maxVwien;
  //else vWien = errFlag;
  else {
    erBeamVar += "\tvWien";
    vWien = 1234.0;
  }

  Double_t maxIhwp1Set = slowChain->GetMaximum("IGL1I00OD16_16");
  Double_t minIhwp1Set = slowChain->GetMinimum("IGL1I00OD16_16");
  cout<<"IHWP1Set max is: "<<  maxIhwp1Set<<", min is: "<<minIhwp1Set<<endl;
  if(maxIhwp1Set == minIhwp1Set) ihwp1set = maxIhwp1Set;
  else {
    erBeamVar += "\tihwp1set";
    ihwp1set = 1234.0;
  }

  Double_t maxIhwp1 = slowChain->GetMaximum("IGL1I00DI24_24M");
  Double_t minIhwp1 = slowChain->GetMinimum("IGL1I00DI24_24M");
  cout<<"ihwp1 max, min: "<<maxIhwp1<<"\t"<<minIhwp1<<endl;
  if(maxIhwp1 == minIhwp1) ihwp1read = maxIhwp1;
  else {
    erBeamVar += "\tihwp1read";
    ihwp1read = 1234.0;
  }

  if((ihwp1set!=ihwp1read) && (erBeamVar!="ihwp1set" || erBeamVar != "hwp1read")) {
    cout<<red<<"\nAlert****: The IHWP set and read back not same\n"<<normal<<endl;
    erBeamVar += "\tihwp1setEr";///Lets get this run# identified instead of simply exiting the execution
  }

  Double_t maxRhwp = slowChain->GetMaximum("psub_pl_pos");
  Double_t minRhwp = slowChain->GetMinimum("psub_pl_pos");
  cout<<"RHWP max, min : "<<maxRhwp<<"\t"<<minRhwp<<endl;
  if(maxRhwp == minRhwp) rhwp = maxRhwp;
  else {
    erBeamVar += "\trhwp";
    rhwp = 1234.0;
  }

  Double_t maxIhwp2 = slowChain->GetMaximum("IGL1I00DIOFLRD");
  Double_t minIhwp2 = slowChain->GetMinimum("IGL1I00DIOFLRD");
  cout<<"Ihwp2 max, min: "<<maxIhwp2<<"\t"<<minIhwp2<<endl;
  if(maxIhwp2 == minIhwp2) ihwp2read = maxIhwp2;
  else {
    erBeamVar += "\tihwp2read";
    ihwp2read = 1234.0;
  }

  Double_t maxPos = slowChain->GetMaximum("HCCEDPOS");
  Double_t minPos =  slowChain->GetMinimum("HCCEDPOS");
  cout<<"edet Pos max is: "<<  maxPos<<", min is: "<<minPos<<endl;
  if(maxPos == minPos) hccedpos = maxPos;
  else {
    erBeamVar += "\thccedpos";
    hccedpos = 1234.0;
  }

  //hCheck->SetBit(TH1::kCanRebin);
  //slowChain->Draw("HALLC_p>>hCheck","","goff");
  //eEnergy = hCheck->GetMean();
  //rms_eEnergy = hCheck->GetRMS();
  //eEnergyEr = hCheck->GetMeanError();
  //hCheck->Reset();

  TH1D *hEnergy = new TH1D("hEnergy","dummy",100,0,0.01);//typical value of maximum laser power
  hEnergy->SetBit(TH1::kCanRebin);
  Int_t totEntries = slowChain->GetEntries();
  if(debug) cout<<blue<<"total number of entries in slow tree is : "<<totEntries<<normal<<endl;

  Double_t maxE = slowChain->GetMaximum("HALLC_p");
  Double_t minE = slowChain->GetMinimum("HALLC_p");
  Double_t newEmean;
  cout<<"beam energy: max, min: "<<maxE<<"\t"<<minE<<endl;
  if((maxE - minE)<1 && (maxE != 0)) {///within 1 MeV
    cout<<"max and min energy measurement are consistent"<<endl;
    for(int i=0; i<totEntries; i++) {///because the max and min E are consistent, following should work
      slowChain->GetEntry(i);
      hEnergy->Fill(rawE);
      if(ibcm1 > highBeamCut) hEnergy->Fill(rawE);///The max-min < 1 is a good enough condition to check though
    }
    newEmean = hEnergy->GetMean();
    rms_eEnergy = hEnergy->GetRMS();
    eEnergyEr = hEnergy->GetMeanError();
    cout<<blue<<"new energy mean: "<<newEmean<<normal<<endl;
    if(newEmean <= maxE && newEmean >= minE) eEnergy = newEmean;///energy still in MeV
  } else {
    eEnergy = eEnergyRun2; ///default histogram value for whole run
    cout<<blue<<"eEnergy being hard set to "<<eEnergyRun2<<normal<<endl;
    erBeamVar += "\teEnergyEr";///to assert that something spurious happened in energy measurement
  }

  if(debug) {
    cout<<blue<<"HWien\tVWien\tIHWP1set\tIHWP1read\tRHWP\tIHWP2read\tedetPos"<<endl;
    cout<<hWien<<"\t"<<vWien<<"\t"<<ihwp1set<<"\t"<<ihwp1read<<"\t"<< rhwp<<"\t"<< ihwp2read<<"\t"<< hccedpos<<endl;
    cout<<"beam Energy: "<<eEnergy<<"\t"<<rms_eEnergy<<"\t"<<eEnergyEr<<normal<<endl;
  }
  eEnergy = (eEnergy+0.8)/1000.0; ///adding the difference of tiffenback measurement and convert to GeV
  rms_eEnergy = rms_eEnergy/1000.0;

  fBeamProp.open(Form("%s/%s/%sbeamProp.txt",pPath, txt,filePre.Data()));//,std::fstream::app);
  if(fBeamProp.is_open()) {
    fBeamProp<<"run\tHWien\tVWien\t\tIHWP1set\tIHWP1read\tRHWP\tIHWP2read\tedetPos\tmeanE\tmeanE_RMS"<<endl;
    fBeamProp<<Form("%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.3f\t%.5f\t%.5f\n",runnum, hWien, vWien, ihwp1set, ihwp1read, rhwp, ihwp2read, hccedpos,eEnergy,rms_eEnergy);
    fBeamProp.close();
    cout<<"wrote beamProperties info to "<<Form("%s/%s/%sbeamProp.txt",pPath, txt,filePre.Data())<<endl;
  } else cout<<red<<"could not open file to write the beam properties"<<normal<<endl;
  
  //fBeamProp.open(Form("%s/%s/%sdipole.txt",pPath, txt,filePre.Data()));//,std::fstream::app);
  //if(fBeamProp.is_open()) {
  //  fBeamProp<<"runnum\tsetdipoleI\tsetdipoleIEr\tsetdipoleIRMS\tdipoleI\tdipoleIEr\tdipoleIRMS\tPosLockMax\tposLockMin\tenergyLockMax\tEnergyLockMin"<<endl;
  //  fBeamProp<<runnum<<"\t"<<setDipoleI<<"\t"<<setDipoleIEr<<"\t"<<setDipoleIRMS<<"\t"<<dipoleI<<"\t"<<dipoleIEr<<"\t"<<dipoleIRMS<<"\t"<<maxPosLockFB<<"\t"<<minPosLockFB<<"\t"<<maxEnergyLockFB<<"\t"<<minEnergyLockFB<<endl;
  //  fBeamProp.close();
  //  cout<<"wrote dipole info to "<<Form("%s/%s/%sdipole.txt",pPath, txt,filePre.Data())<<endl;
  //} else cout<<red<<"could not open file to write the dipole info"<<normal<<endl;

  fBeamProp.open(Form("%s/%s/%sscintRateNorm.txt",pPath, txt,filePre.Data()));//,std::fstream::app);
  if(fBeamProp.is_open()) {
    fBeamProp<<"runnum\tscintRate\tscintRateEr\tscintRateRMS"<<endl;
    fBeamProp<<runnum<<"\t"<<scintRate<<"\t"<<scintRateEr<<"\t"<<scintRateRMS<<endl;
    fBeamProp.close();
    cout<<"wrote scintillator rate to "<<Form("%s/%s/%sscintRateNorm.txt",pPath, txt,filePre.Data())<<endl;
  } else cout<<red<<"could not open file to write the scintillator normalized rate"<<normal<<endl;

  if(erBeamVar != "") {
    ofstream fTag;
    fTag.open(Form("%s/%s/%srunRemark.txt",pPath, txt,filePre.Data()));
    if(fTag.is_open()) {
      fTag<<runnum<<"\t"<<erBeamVar<<endl;///whatever may be the parameter changed in the middle, it will get listed
      fTag.close();
      cout<<magenta<<"something changed, during the run; its being listed in "<<Form("%s/%s/%srunRemark.txt",pPath, txt,filePre.Data())<<normal<<endl;
    }
  }

  TChain *confChain = new TChain("Config_Tree");//chain of run segments
  Int_t chainExists = confChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  if(!chainExists) {/// exit if rootfiles do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
    return -1;
  }
  confChain->SetBranchStatus("*",1); 

  for(Int_t p = 0; p <nPlanes; p++) {
    confChain->SetBranchAddress(Form("v1495InfoPlane%d",p+1),&bMask[p]);
  } //the branch for each plane is named from 1 to 4 

  for(Int_t m = 0; m <nModules; m++) {
    confChain->SetBranchAddress(Form("slave%d_acTrig",m+1),&bAcTrigSlave[m]);
    confChain->SetBranchAddress(Form("slave%d_evTrig",m+1),&bEvTrigSlave[m]);
    confChain->SetBranchAddress(Form("slave%d_minWidth",m+1),&bMinWidthSlave[m]);
    confChain->SetBranchAddress(Form("slave%d_firmWareRevision",m+1),&bFirmwareRevSlave[m]);
    confChain->SetBranchAddress(Form("slave%d_PWTL",m+1),&bPWTLSlave[m]);
    confChain->SetBranchAddress(Form("slave%d_PWTL2",m+1),&bPWTL2Slave[m]);
    confChain->SetBranchAddress(Form("slave%d_HoldOff",m+1),&bHoldOffSlave[m]);
    confChain->SetBranchAddress(Form("slave%d_PipelineDelay",m+1),&bPipelineDelaySlave[m]);
  }
  ///use the first runlet'f config tree to identify the strip mask
  confChain->GetEntry(0);

  for(Int_t p = startPlane; p <nPlanes; p++) {
    for(Int_t s =startStrip; s <endStrip; s++) {
      mask[p][s] = (Int_t)bMask[p][s];
    }
  }

  infoStripMask.open(Form("%s/%s/%sinfoStripMask.txt",pPath, txt,filePre.Data()));
  if(infoStripMask.is_open()) {
    infoStripMask<<"plane\t\trow of masked strips in this plane -->"<<endl;
    for(Int_t p = startPlane; p <nPlanes; p++) { //this works but didn't appear needful hence commented out
      if(debug) cout<<blue<<"in plane# "<<p+1<<normal<<endl;
      infoStripMask<<"\n"<<p+1<<"\t";
      for(Int_t s =startStrip; s <endStrip; s++) {
        if (mask[p][s] == 0) {
          infoStripMask<<s+1<<"\t";//notice that the strip#s are pushed in as human count #s 
          if(debug) cout<<"masked strip "<<s+1<<endl;
        }//check if strip is masked
      }//for all strips
    }//for all planes
    infoStripMask.close();
    cout<<"wrote strip mask information to "<<Form("%s/%s/%sinfoStripMask.txt",pPath, txt,filePre.Data())<<endl;
  } else cout<<red<<"could not open a file to write the stripMaskInfo"<<normal<<endl;

  ///the config tree gets written only in the beginning of the run, the subsequent runlets carry a null entry
  for(Int_t m = 0; m <nModules; m++) {
    acTrigSlave[m]        = bAcTrigSlave[m];            //(Int_t)
    evTrigSlave[m]        = bEvTrigSlave[m];            //(Int_t)
    minWidthSlave[m]      = bMinWidthSlave[m];          //(Int_t)
    fwRev[m]   = bFirmwareRevSlave[m];       //(Int_t)
    pwtlSlave[m]          = bPWTLSlave[m];              //(Int_t)
    pwtl2Slave[m]         = bPWTL2Slave[m];             //(Int_t)
    holdOffSlave[m]       = bHoldOffSlave[m];           //(Int_t)
    plDelay[m] = bPipelineDelaySlave[m];     //(Int_t)
  }

  if(acTrigSlave[0] == acTrigSlave[1]) acTrig = acTrigSlave[1];
  else erDAQVar += "\tacTrig";
  if(evTrigSlave[0] == evTrigSlave[1]) evTrig = evTrigSlave[1];
  else erDAQVar += "\tevTrig";
  if(minWidthSlave[0] == minWidthSlave[1]) minWidth = minWidthSlave[1];
  else erDAQVar += "\tminWidth";
  if(fwRev[0] == fwRev[1]) firmwareRev = fwRev[1];
  else erDAQVar += "\tfirmwareRev";
  if(pwtlSlave[0] == pwtlSlave[1]) pwtl1 = pwtlSlave[1];
  else erDAQVar += "\tpwtl1";
  if(pwtl2Slave[0] == pwtl2Slave[1]) pwtl2 = pwtl2Slave[1];
  else erDAQVar += "\tpwtl2";
  if(holdOffSlave[0] == holdOffSlave[1]) holdOff = holdOffSlave[0];
  else erDAQVar += "\tholdOff";
  //holdOff = holdOffSlave[0];///due to a programming error, it was not written in correctly to the output for slave 2
  if(plDelay[0] == plDelay[1]) pipelineDelay = plDelay[0];
  else erDAQVar += "\tplDelay";

  //cout<<red<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay)<<normal<<endl;

  flagsfile.open(Form("%s/%s/%sinfoDAQ.txt",pPath, txt,filePre.Data()));
  if(flagsfile.is_open()) {
    flagsfile<<";runnum\tacTrig\tevTrig\tminW\tfWare\tpwtl1\tpwtl2\thOff\tplDelay"<<endl;
    flagsfile<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOffSlave[0],plDelay[0]);
    flagsfile.close();
    cout<<"wrote the flagsfile info to "<<Form("%s/%s/%sinfoDAQ.txt",pPath, txt,filePre.Data())<<endl;
  } else {
    cout<<"could not open file for writing flags file info "<<Form("%s/%s/%sinfoDAQ.txt",pPath, txt,filePre.Data())<<endl;
  }

  if (erDAQVar != "") {
    cout<<magenta<<"found a potential error in flags file as found in rootfile, printing debugDAQ "<<normal<<endl;
    debugDAQ.open(Form("%s/%s/%sdebugDAQ.txt",pPath, txt,filePre.Data()));
    debugDAQ<<"module\tacTrig\tevTrig\tminW\tfwRev\tpwtl1\tpwtl2\tholdOff\tplDelay"<<endl;
    for(Int_t m = 0; m <nModules; m++) {
      cout<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",m,acTrigSlave[m],evTrigSlave[m],minWidthSlave[m],fwRev[m],pwtlSlave[m],pwtl2Slave[m],holdOffSlave[m],plDelay[m]);
      debugDAQ<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",m,acTrigSlave[m],evTrigSlave[m],minWidthSlave[m],fwRev[m],pwtlSlave[m],pwtl2Slave[m],holdOffSlave[m],plDelay[m]);
    }
    debugDAQ<<runnum<<"\t"<<erDAQVar<<endl;
    debugDAQ.close();
  }

  if(debug) {
    cout<<";runnum\tacTrig\tevTrig\tminW\tfWare\tpwtl1\tpwtl2\tholdOff\tplDelay"<<endl;
    cout<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay);
  }
  delete hEnergy;
  delete slowChain;
  delete confChain;
  return 1;
}

/***********************
  Choosing how to skip a strip that was not masked in DAQ:
 *I can look at the asymmetry numerator and visually look for outliers
 *I can look at the asymmetry fit residuals and visually identify outliers
 *As of now, the above two methods do not seem to be in agreement for some choices
 **this list may need to become dependent on run-ranges because some strips started growing bad with time
 **some part of the bias in the evaluation may be coming from the symmetry in the trigger generation. 
 **This symmetry gets biased by masking one strip in a plane while leaving it unmasked in another plane. 

 *Instead of going through the skipStrip mechanism of ignoring a strip, 
 *why not simply modify the mask[p][s] with this additional information of the 
 *strips that I think should be masked off ? 
 *For:we may miss the possibility of understanding this behaviour of strips getting noisy, 
 * eg: the time scale on which this is happening or the possibility of seeing the time scale 
 * of the strip's behaviour of slowing getting noisy. 
 * Also, For an ongoing analysis, I should have an eye on the things that I choose to not contribute to polarization. 
 *Against: everytime we show this plot to a non-Compton group, we would be asked the reason
 * to remove a strip just because it was not in line with the result that we were anticipating. 
 *************************/

#endif
