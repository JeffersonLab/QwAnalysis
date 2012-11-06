#include <rootClass.h>
#include "comptonRunConstants.h"

void infoDAQ(Int_t runnum)
{
  Bool_t debug=0;
  Double_t bMask[nPlanes][nStrips];
  Double_t bAcTrigSlave[nModules],bEvTrigSlave[nModules],bMinWidthSlave[nModules],bFirmwareRevSlave[nModules],bPWTLSlave[nModules],bPWTL2Slave[nModules],bHoldOffSlave[nModules],bPipelineDelaySlave[nModules];
  //several variables relevant to this function are declared in comptonRunConstants.h to allow usage in other files

  //should put a check if the file was not opened successfully 
  TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_%i.000.root",runnum));//ensure to read in only the first runlet
  TTree *configTree = (TTree*)file->Get("Config_Tree");
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
    bPipelineDelaySlave[m]= (Int_t)bPipelineDelaySlave[m];
  }

  if(debug){
    for(Int_t p = startPlane; p <endPlane; p++) {
      for(Int_t s =startStrip; s <12; s++) {
	cout<<Form("p%ds%dMask ",p+1,s+1)<<mask[p][s]<<endl;
      }
    }
  }
}
