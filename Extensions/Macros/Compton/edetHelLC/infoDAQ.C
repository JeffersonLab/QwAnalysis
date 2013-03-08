#ifndef __INFODAQ_F
#define __INFODAQ_F
#include <rootClass.h>
#include "comptonRunConstants.h"

void infoDAQ(Int_t runnum)
{
  cout<<blue<<"\nStarting into infoDAQ.C **************\n"<<normal<<endl;
  maskSet = 1; //to state that the masks have been set//this would be checked before calling the infoDAQ function
  Bool_t debug=1,autoDebug=1;
  Bool_t additionalStripMask=0;
  Double_t bMask[nPlanes][nStrips];
  const Int_t errFlag=100;
  Int_t acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay;

  Double_t bAcTrigSlave[nModules],bEvTrigSlave[nModules],bMinWidthSlave[nModules],bFirmwareRevSlave[nModules],bPWTLSlave[nModules],bPWTL2Slave[nModules],bHoldOffSlave[nModules],bPipelineDelaySlave[nModules];
  //several variables relevant to this function are declared in comptonRunConstants.h to allow usage in other files
  ofstream infoDAQthisRun,debugInfoDAQ,infoStripMask;
  //should put a check if the file was not opened successfully 
  //TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_%i.000.root",runnum));//ensure to read in only the first runlet
  TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_Pass2b_%i.000.root",runnum));//ensure to read in only the first runlet
  TTree *configTree = (TTree*)file->Get("Config_Tree");
  filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
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

  for(Int_t p = startPlane; p <nPlanes-1; p++) {
    for(Int_t s =startStrip; s <endStrip; s++) {
      mask[p][s] = (Int_t)bMask[p][s];
    }
  }
  
  infoStripMask.open(Form("%s/%s/%sinfoStripMask.txt",pPath,webDirectory,filePrefix.Data()));
  infoStripMask<<";plane\tmaskedStrips:";
  //for(Int_t p = startPlane; p <nPlanes-1; p++) {
  for(Int_t p = startPlane; p <endPlane; p++) {
    infoStripMask<<"\n"<<p+1<<"\t";
    for(Int_t s =startStrip; s <endStrip; s++) {
      if (mask[p][s] == 0) {
	infoStripMask<<s+1<<"\t";
	//skipStrip.insert(s+1); //idea of declaraing it as a 'set' did not work
	skipStrip.push_back(s+1);//notice that the strip#s are pushed in as human count #s
	if(debug) cout<<blue<<"masked strip "<<s+1<<normal<<endl;
      }
    }
  }
  infoStripMask.close();
  ///list all those strips that you want in the list for in case it was not already a part of the above created vector
  if(additionalStripMask) {
    skipStrip.push_back(2);//notice that the strip number pushed is in human counts
    skipStrip.push_back(10);//notice that the strip number pushed is in human counts
    cout<<red<<"apart from strips masked in DAQ, am ignoring strip # 2 & 10 accross all planes"<<normal<<endl;//!update this with above list
  }
  for(Int_t m = 0; m <nModules; m++) {
    acTrigSlave[m]=(Int_t)bAcTrigSlave[m];  
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
  infoDAQthisRun<<";runnum\tacTrig\tevTrig\tminW\tfirmware\tpwtl1\tpwtl2\tholdOff\tpipelineDelay"<<endl;
  infoDAQthisRun<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOffSlave[0],pipelineDelaySlave[0]);
  infoDAQthisRun.close();

  if(debug) {
    cout<<";runnum\tacTrig\tevTrig\tminW\tfirmware\tpwtl1\tpwtl2\tholdOff\tpipelineDelay"<<endl;
    cout<<Form("%d\t%d\t%d\t%d\t%X\t%d\t%d\t%d\t%d\n",runnum,acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay);
  }
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
