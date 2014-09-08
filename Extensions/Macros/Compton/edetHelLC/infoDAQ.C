#ifndef __INFODAQ_F
#define __INFODAQ_F
#include "rootClass.h"
#include "comptonRunConstants.h"
//several variables relevant to this function are declared in comptonRunConstants.h to allow usage in other files

Int_t infoDAQ(Int_t runnum)
{
  cout<<blue<<"\nStarting into infoDAQ.C **************\n"<<normal<<endl;
  maskSet = 1; //to state that the masks have been set//this would be checked before calling the infoDAQ function
  gSystem->mkdir(Form("%s/%s/run_%d",pPath,webDirectory,runnum));
  filePre = Form(filePrefix,runnum,runnum);
  const Bool_t debug = 0;
  const Int_t errFlag=100;
  Bool_t additionalStripMask=1;///this will be my primary tool to skip masked strips in asymFit.C
  Double_t bMask[nPlanes][nStrips];
  Double_t mask[nPlanes][nStrips];
  Double_t bAcTrigSlave[nModules],bEvTrigSlave[nModules],bMinWidthSlave[nModules],bFirmwareRevSlave[nModules],bPWTLSlave[nModules],bPWTL2Slave[nModules],bHoldOffSlave[nModules],bPipelineDelaySlave[nModules];
  ofstream flagsfile,debugInfoDAQ,infoStripMask,fBeamProp;
  //TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_Pass2b_%d.000.root",runnum));//ensure to read in only the first runlet
  if(additionalStripMask) {//notice that the strip number pushed is in human counts 
    skipStrip.push_back(2);
    skipStrip.push_back(6);
    skipStrip.push_back(10);
    skipStrip.push_back(20);
    skipStrip.push_back(39);
    cout<<red<<"masked strips # 2,6,10,20,39 accross all planes"<<normal<<endl;//!update this with above list
  }

  ///For further processing, I need the rootfile
  TFile file(Form("$QW_ROOTFILES/Compton_Pass2b_%d.000.root",runnum));//need to read in only the first runlet
  if(file.IsZombie()) {
    cout<<red<<"couldn't open the rootfile needed by infoDAQ.C"<<normal<<endl;
    return 1;
    ///Even without the rootfile the masks are set so I can use this macro so lets return gracefully 
    ///But can't execute the process the rest of the macro so exiting;
  }
  //TTree *slowTree = (TTree*)file.Get("Slow_Tree");
  TChain *slowChain = new TChain("Slow_Tree");//to scan through the entire run if a parameter changed midway
  Int_t slowExists = slowChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  cout<<"Attached "<<slowExists<<" files to chain for Run # "<<runnum<<endl;

  Double_t hWien, rms_hWien, vWien, rms_vWien;
  Double_t ihwp1set,rms_ihwp1set, rhwp, rms_rhwp,ihwp1read, rms_ihwp1read, ihwp2read, rms_ihwp2read;   
  //Double_t hallcPhI,hallcWaveL;
  //Double_t weinAngle,qeHallA,qeHallB,qeHallC;
  //Double_t hallcTrans,
  Double_t hccedpos, rms_hccedpos;

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
  ////slowChain->SetBranchAddress("",&);

  //Int_t totEntries = slowChain->GetEntries();

  TH1D *hCheck = new TH1D("hCheck","dummy",100,-0.0,0.1);//typical value of maximum beam current
  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("HWienAngle>>hCheck","","goff");
  hWien = hCheck->GetMean();
  rms_hWien = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("VWienAngle>>hCheck","","goff");
  vWien = hCheck->GetMean();
  rms_vWien = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("IGL1I00OD16_16>>hCheck","","goff");
  ihwp1set = hCheck->GetMean();
  rms_ihwp1set = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("IGL1I00DI24_24M>>hCheck","","goff");
  ihwp1read = hCheck->GetMean();
  rms_ihwp1read = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("psub_pl_pos>>hCheck","","goff");
  rhwp = hCheck->GetMean();
  rms_rhwp = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("IGL1I00DIOFLRD>>hCheck","","goff");
  ihwp2read = hCheck->GetMean();
  rms_ihwp2read = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("HCCEDPOS>>hCheck","","goff");
  hccedpos = hCheck->GetMean();
  rms_hccedpos = hCheck->GetRMS();
  hCheck->Reset();

  hCheck->SetBit(TH1::kCanRebin);
  slowChain->Draw("HALLC_p>>hCheck","","goff");
  eEnergy = hCheck->GetMean();
  rms_eEnergy = hCheck->GetRMS();
  hCheck->Reset();

  if(debug) {
    cout<<blue<<"HWien\tVWien\tIHWP1set\tIHWP1read\tRHWP\tIHWP2read\tedetPos\n"<<endl;
    cout<<hWien<<"\t"<<vWien<<"\t"<<ihwp1set<<"\t"<<ihwp1read<<"\t"<< rhwp<<"\t"<< ihwp2read<<"\t"<< hccedpos<<endl;
    cout<<rms_hWien<<"\t"<< rms_vWien<<"\t"<< rms_ihwp1set<<"\t"<<rms_ihwp1read<<"\t"<< rms_rhwp<<"\t"<< rms_ihwp2read<<"\t"<< rms_hccedpos <<normal<<endl;
  }
  eEnergy = eEnergy/1000.0; ///to convert to GeV
  rms_eEnergy = rms_eEnergy/1000.0;
  if(debug) cout<<"beam Energy: "<<eEnergy<<"+/-"<<rms_eEnergy<<endl;
  eEnergy = 1.159;//!!!
  cout<<red<<"temporarily setting energy =1.159 GeV, though its differnt from readback value"<<normal<<endl;

  fBeamProp.open(Form("%s/%s/%sbeamProp.txt",pPath,webDirectory,filePre.Data()));//,std::fstream::app);
  if(fBeamProp.is_open()) {
    fBeamProp<<runnum<<"\t"<<hWien<<"\t"<<vWien<<"\t"<<ihwp1set<<"\t"<<ihwp1read<<"\t"<< rhwp<<"\t"<< ihwp2read<<"\t"<< hccedpos<<endl;
    fBeamProp<<rms_hWien<<"\t"<< rms_vWien<<"\t"<< rms_ihwp1set<<"\t"<<rms_ihwp1read<<"\t"<< rms_rhwp<<"\t"<< rms_ihwp2read<<"\t"<< rms_hccedpos<<endl;
    fBeamProp<<eEnergy<<"\t"<<rms_eEnergy<<endl;
    fBeamProp.close();
  } else cout<<red<<"could not open file to write the beam properties"<<normal<<endl;

  if(ihwp1set!=ihwp1read) {
    cout<<red<<"\nAlert****: The IHWP set and read back not same\n"<<normal<<endl;
    return -1;
  }
  if(rms_ihwp1read/ihwp1read > rmsLimit || rms_ihwp1set/ihwp1set > rmsLimit || rms_rhwp/rhwp >rmsLimit || rms_ihwp2read/ihwp2read >rmsLimit) {
    cout<<red<<"\nAlert****: The IHWP was probably CHANGED in the middle of run\n"<<normal<<endl;
    cout<<blue<<"HWien\tVWien\tIHWP1set\tIHWP1read\tRHWP\tIHWP2read\tedetPos\n"<<endl;
    cout<<hWien<<"\t"<<vWien<<"\t"<<ihwp1set<<"\t"<<ihwp1read<<"\t"<< rhwp<<"\t"<< ihwp2read<<"\t"<< hccedpos<<endl;
    cout<<rms_hWien<<"\t"<< rms_vWien<<"\t"<< rms_ihwp1set<<"\t"<<rms_ihwp1read<<"\t"<< rms_rhwp<<"\t"<< rms_ihwp2read<<"\t"<< rms_hccedpos <<normal<<endl;
    return -1;
  }
  if(rms_hccedpos/hccedpos > rmsLimit) {
    cout<<red<<"\nAlert*****: The detector was probably moving while data was taken\n"<<normal<<endl;
    cout<<blue<<"edetPos   rms_edetPos"<<endl;
    cout<<hccedpos<<"\t"<< rms_hccedpos<<endl;
    return -1;
  }

  TTree *configTree = (TTree*)file.Get("Config_Tree");
  //configTree->ResetBranchAddresses();
  configTree->SetBranchStatus("*",1); 

  for(Int_t p = startPlane; p <nPlanes; p++) {
    configTree->SetBranchAddress(Form("v1495InfoPlane%d",plane),&bMask[p]);
  } //the branch for each plane is named from 1 to 4 

  for(Int_t m = 0; m <nModules; m++) {
    configTree->SetBranchAddress(Form("slave%d_acTrig",m+1),&bAcTrigSlave[m]);
    configTree->SetBranchAddress(Form("slave%d_evTrig",m+1),&bEvTrigSlave[m]);
    configTree->SetBranchAddress(Form("slave%d_minWidth",m+1),&bMinWidthSlave[m]);
    configTree->SetBranchAddress(Form("slave%d_firmWareRevision",m+1),&bFirmwareRevSlave[m]);
    configTree->SetBranchAddress(Form("slave%d_PWTL",m+1),&bPWTLSlave[m]);
    configTree->SetBranchAddress(Form("slave%d_PWTL2",m+1),&bPWTL2Slave[m]);
    configTree->SetBranchAddress(Form("slave%d_HoldOff",m+1),&bHoldOffSlave[m]);
    configTree->SetBranchAddress(Form("slave%d_PipelineDelay",m+1),&bPipelineDelaySlave[m]);
  }

  configTree->GetEntry(0);

  for(Int_t p = startPlane; p <nPlanes; p++) {
    for(Int_t s =startStrip; s <endStrip; s++) {
      mask[p][s] = (Int_t)bMask[p][s];
    }
  }

  infoStripMask.open(Form("%s/%s/%sinfoStripMask.txt",pPath,webDirectory,filePre.Data()));
  infoStripMask<<";plane\tmaskedStrips:";
  if(infoStripMask.is_open()) {
    infoStripMask<<";plane\trow of masked strips in this plane -->"<<endl;
  for(Int_t p = startPlane; p <nPlanes; p++) { //this works but didn't appear needful hence commented out
    infoStripMask<<"\n"<<p+1<<"\t";
    for(Int_t s =startStrip; s <endStrip; s++) {
      if (mask[p][s] == 0) {
        infoStripMask<<s+1<<"\t";//notice that the strip#s are pushed in as human count #s 
        if(debug) cout<<blue<<"masked strip "<<s+1<<normal<<endl;
      }//check if strip is masked
    }//for all strips
  }//for all planes
  infoStripMask.close();
  } else cout<<red<<"could not open a file to write the stripMaskInfo"<<normal<<endl;

  ///list all those strips that you want in the list for in case it was not already a part of the above created vector
  for(Int_t m = 0; m <nModules; m++) {
    acTrigSlave[m]        = (Int_t)bAcTrigSlave[m];  
    evTrigSlave[m]        = (Int_t)bEvTrigSlave[m];
    minWidthSlave[m]      = (Int_t)bMinWidthSlave[m];
    firmwareRevSlave[m]   = (Int_t)bFirmwareRevSlave[m];
    pwtlSlave[m]          = (Int_t)bPWTLSlave[m];
    pwtl2Slave[m]         = (Int_t)bPWTL2Slave[m];
    holdOffSlave[m]       = (Int_t)bHoldOffSlave[m];
    pipelineDelaySlave[m] = (Int_t)bPipelineDelaySlave[m];
  }

  ///Note: "100" is being used an an error flag in this case to alert that the values were different between two slaves
  acTrig = acTrigSlave[0]==acTrigSlave[1] ? acTrigSlave[1] : errFlag;
  evTrig = evTrigSlave[0]==evTrigSlave[1] ? evTrigSlave[1] : errFlag;
  minWidth = minWidthSlave[0]==minWidthSlave[1] ? minWidthSlave[1] : errFlag;
  firmwareRev = firmwareRevSlave[0]==firmwareRevSlave[1] ? firmwareRevSlave[1] : errFlag;
  pwtl1 = pwtlSlave[0]==pwtlSlave[1] ? pwtlSlave[1] :errFlag;
  pwtl2 = pwtl2Slave[0]==pwtl2Slave[1] ? pwtl2Slave[1] :errFlag;
  holdOff = holdOffSlave[0]==holdOffSlave[1] ? holdOffSlave[1] : errFlag;
  pipelineDelay = pipelineDelaySlave[0]==pipelineDelaySlave[1] ? pipelineDelaySlave[1] : errFlag;

  flagsfile.open(Form("%s/%s/%sinfoDAQ.txt",pPath,webDirectory,filePre.Data()));
  if(flagsfile.is_open()) {
    flagsfile<<";runnum\tacTrig\tevTrig\tminW\tfWare\tpwtl1\tpwtl2\thOff\tplDelay"<<endl;
    flagsfile<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOffSlave[0],pipelineDelaySlave[0]);
    flagsfile.close();
    cout<<"wrote the flagsfile info to "<<Form("%s/%s/%sinfoDAQ.txt",pPath,webDirectory,filePre.Data())<<endl;
  } else {
    cout<<"could not open file for writing flags file info"<<endl;
    cout<<Form("%s/%s/%sinfoDAQ.txt",pPath,webDirectory,filePre.Data())<<endl;
  }

  if ((acTrig==errFlag)||(evTrig==errFlag)||(minWidth==errFlag)||(firmwareRev==errFlag)||(pwtl1==errFlag)||(pwtl2==errFlag)||(holdOff==errFlag)||(pipelineDelay==errFlag)) {
    cout<<"found a potential error in flags file as found in rootfile, printing debugInfoDAQ "<<endl;
    debugInfoDAQ.open(Form("%s/%s/%sdebugInfoDAQ.txt",pPath,webDirectory,filePre.Data()));
    debugInfoDAQ<<";module\tacTrig\tevTrig\tminWidth\tfirmwareRev\tpwtl1\tpwtl2\tholdOff\tpipelineDelay"<<endl;
    for(Int_t m = 0; m <nModules; m++) {
      debugInfoDAQ<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",m,acTrigSlave[m],evTrigSlave[m],minWidthSlave[m],firmwareRevSlave[m],pwtlSlave[m],pwtl2Slave[m],holdOffSlave[m],pipelineDelaySlave[m]);
    }
    debugInfoDAQ.close();
  }

  if(debug) {
    cout<<";runnum\tacTrig\tevTrig\tminW\tfWare\tpwtl1\tpwtl2\tholdOff\tplDelay"<<endl;
    cout<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay);
  }
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
