#ifndef __INFODAQ_F
#define __INFODAQ_F
#include <rootClass.h>
#include "comptonRunConstants.h"

void infoDAQ(Int_t runnum)
{
  Bool_t debug=0,autoDebug=1;
  Double_t bMask[nPlanes][nStrips];
  const Int_t errFlag=100;
  Int_t acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay;

  Double_t bAcTrigSlave[nModules],bEvTrigSlave[nModules],bMinWidthSlave[nModules],bFirmwareRevSlave[nModules],bPWTLSlave[nModules],bPWTL2Slave[nModules],bHoldOffSlave[nModules],bPipelineDelaySlave[nModules];
  Int_t acTrigSlave[nModules],evTrigSlave[nModules],minWidthSlave[nModules],firmwareRevSlave[nModules],pwtlSlave[nModules],pwtl2Slave[nModules],holdOffSlave[nModules],pipelineDelaySlave[nModules];
  //several variables relevant to this function are declared in comptonRunConstants.h to allow usage in other files
  ofstream infoDAQthisRun,debugInfoDAQ;
  //should put a check if the file was not opened successfully 
  //TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_%i.000.root",runnum));//ensure to read in only the first runlet
  TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_Pass2_%i.000.root",runnum));//ensure to read in only the first runlet
  TTree *configTree = (TTree*)file->Get("Config_Tree");
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  configTree->ResetBranchAddresses();
  configTree->SetBranchStatus("*",1); 
 
  for(Int_t p = 0; p <nPlanes; p++) {      
    configTree->SetBranchAddress(Form("v1495InfoPlane%d",p+1),&bMask[p]);
  }//the branch for each plane is named from 1 to 4

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

  for(Int_t p = startPlane; p <endPlane; p++) {
    for(Int_t s =startStrip; s <endStrip; s++) {
      mask[p][s] = (Int_t)bMask[p][s];
    }
  }

  for(Int_t m = 0; m <nModules; m++) {
    acTrigSlave[m]=(Int_t)bAcTrigSlave[m];  
    evTrigSlave[m]        = (Int_t)bEvTrigSlave[m];
    minWidthSlave[m]      = (Int_t)bMinWidthSlave[m];
    firmwareRevSlave[m]   = (Int_t)bFirmwareRevSlave[m];
    pwtlSlave[m]	      = (Int_t)bPWTLSlave[m];
    pwtl2Slave[m]	      = (Int_t)bPWTL2Slave[m];
    holdOffSlave[m]	      = (Int_t)bHoldOffSlave[m];
    pipelineDelaySlave[m]= (Int_t)bPipelineDelaySlave[m];
  }

  if(debug) {
    for(Int_t p = startPlane; p <endPlane; p++) {
      for(Int_t s =startStrip; s <12; s++) {
	cout<<Form("p%ds%dMask ",p+1,s+1)<<mask[p][s]<<endl;
      }
    }
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
  // holdOff = holdOffSlave[0];//==holdOffSlave[1] ? holdOffSlave[1] : errFlag;
  // pipelineDelay = bPipelineDelaySlave[0];//==bPipelineDelaySlave[1] ? bPipelineDelaySlave[1] : errFlag;
 
  if ((acTrig==errFlag)||(evTrig==errFlag)||(minWidth==errFlag)||(firmwareRev==errFlag)||(pwtl1==errFlag)||(pwtl2==errFlag)||(holdOff==errFlag)||(pipelineDelay==errFlag)) autoDebug=1;

  if(autoDebug) {
    debugInfoDAQ.open(Form("%s/%s/%sdebugInfoDAQ.txt",pPath,webDirectory,filePrefix.Data()));
    debugInfoDAQ<<";module\tacTrig\tevTrig\tminWidth\tfirmwareRev\tpwtl1\tpwtl2\tholdOff\tpipelineDelay"<<endl;
    for(Int_t m = 0; m <nModules; m++) {
      debugInfoDAQ<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",m,acTrigSlave[m],evTrigSlave[m],minWidthSlave[m],firmwareRevSlave[m],pwtlSlave[m],pwtl2Slave[m],holdOffSlave[m],pipelineDelaySlave[m]);
    }
    debugInfoDAQ.close();
  }

  infoDAQthisRun.open(Form("%s/%s/%sinfoDAQ.txt",pPath,webDirectory,filePrefix.Data()));
  infoDAQthisRun<<";runnum\tacTrig\tevTrig\tminWidth\tfirmwareRev\tpwtl1\tpwtl2\tholdOff\tpipelineDelay"<<endl;
  infoDAQthisRun<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOffSlave[0],pipelineDelaySlave[0]);
  infoDAQthisRun.close();

  if(debug) {
    cout<<";runnum\tacTrig\tevTrig\tminWidth\tfirmwareRev\tpwtl1\tpwtl2\tholdOff\tpipelineDelay"<<endl;
    cout<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay);
  }
}
#endif
