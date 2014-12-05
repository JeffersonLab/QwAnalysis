/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Main program , run correlation and computes alphas for  QW event 
 * 
 *********************************************************************/
#include <cstdio>// C++ standard since 1999
#include <iostream>
using namespace std;

#include <TFile.h>
#include <TH2.h>
#include <TTree.h> 

#include <TChain.h>
#include <TObjArray.h>
#include <TEventList.h> //tmp

#include "JbLeafTransform.h"
#include "JbCorrelator.h"
#include "JbBlueTree.h"
// use only double

int parMinEve=5000;

int main(int argc, char *argv[]) {
  int mxEve=500;
  int runSeg=0;
  //const char * inpPath="/home/cdaq/qweak/QwScratch/rootfiles/QwPass1_"; //cdaq:Pass1
  //const char * inpPath="/group/qweak/QwAnalysis/common/QwScratch/rootfiles/QwPass1.1_";//*QwPass1.1* from Paul
  const char * inpPath="/cache/mss/hallc/qweak/rootfiles/pass0/QwPass1_";// from the silo

  const char * outPath="./out/";
  const char * configFName="blueReg.conf";
  const char * slopeFName=0;

  // decode input params
  if(argc<2) {
    printf("Syntax: %s runNo [runSeg]  [neve] [config] [slopes] \n STOP",argv[0]);
    printf("    e.g.  %s 5762 [000]  [6000] [blueReg.conf]  [blue.slope.root] \n",argv[0]); return -1;
  }
  int runNo=atoi(argv[1]);
  if ( argc>=3)  runSeg=atoi(argv[2]);
  if ( argc>=4)  mxEve =atoi(argv[3]);
  if ( argc>=5)  configFName=argv[4];
  if ( argc>=6)  slopeFName =argv[5];

  TString runName=Form("%d.%03d",runNo,runSeg);

  printf("Selected run=%s  mxEve=%d   config=%s  slope=%s\n", runName.Data(),mxEve,configFName,slopeFName);
  TString treeInpFile=Form("%s%s.root",inpPath,runName.Data());
  TString xxx=treeInpFile;
  char *xx2=strstr(xxx,"/QwPass")+1;
  TString treeOutName=Form("%sreg_%s",outPath,xx2);

  //...... access to inpute leafs
  JbLeafTransform eve("filter"); // filter content of run, unpacks event
  eve.readConfig(configFName);
  JbCorrelator corA("input"); //   correlator , passive 

  //..... try to open known correlation coef's.......
  JbCorrelator *corB=0;
  double *YvecNew=0;
  TMatrixD *alphasM=0; // indicator that coef's exist
  JbBlueTree *blueTree=0;
  TFile*  mBlueFile=0;  

  if( slopeFName) {
    TString corFileName=Form("%s%s",outPath,slopeFName);
    TFile*  corFile=new TFile(corFileName);
    if( !corFile->IsOpen()) {
      printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
      return 0;
    }
    //    alphasM=(TMatrixD *) corFile->Get("IV_covariance");assert(alphasM);
    //  printf("opened %s,  IVcorel found, dump:\n",corFile->GetName());  alphasM->Print();

    alphasM=(TMatrixD *) corFile->Get("slopes");
    assert(alphasM);
    corB=new JbCorrelator("regres"); //   2nd correlator after regression
    YvecNew=new double [eve.ndv()];
    
    printf("opened %s, slopes found, dump:\n",corFile->GetName());
    alphasM->Print();
    corFile->Close();

    mBlueFile=new TFile(treeOutName,"RECREATE"," regressed  Qweak tree");
    printf("Open to write  =%s=\n",treeOutName.Data());
    blueTree=new JbBlueTree(eve.dvName);
  }
  
  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");  
  printf("Open to read  =%s=\n",treeInpFile.Data());
  chain->Add(treeInpFile);
  
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain \nscan leafs for iv & dv ...\n",nEve);
  printf("#totEve %d\n",nEve);
  if(nEve <parMinEve) {
    printf("aborting linear regerssion due to small # of events\n");
    printf("#abort JB1 nEve=%d\n",nEve);
    exit(1);
  }

  TString hfileName=Form("%sblueR%s.hist.root",outPath,runName.Data());
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak data");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());

  eve.init();  // creates histo: WARN: output histo file must be already opened   
  eve.print();
  eve.findInputLeafs(chain);
  corA.init( eve.ivName, eve.dvName );   // creates histo 
  if(alphasM) corB->init( eve.ivName, eve.dvName); 


  // filter events with custom cut.
  TString cutFormula="ErrorFlag==0"; // apply general pattern QA cut
  //TString cutFormula="1==1"; // tmp

  if(eve.cutFormula.Sizeof()>1) {
    printf("Main: filter events with custom cut: name=%s  formula='%s'\n",eve.cutName.Data(), eve.cutFormula.Data());
    cutFormula+="&&"+eve.cutFormula;
  }
  chain->Draw(">>listA",cutFormula); // custom cleanup cut
  TEventList *list = (TEventList*)gDirectory->Get("listA"); 
  list->Print();
  double listA_len= list->GetN();
  printf("main list A size=%d\n", list->GetN()); 

  chain->Draw("yield_qwk_bcm1.hw_sum>>histA",cutFormula);  
  TH1* histA=(TH1*)gDirectory->Get("histA");
  double xAvr=histA->GetMean();
  int bin=histA->GetMaximumBin();
  double xMPV=histA->GetXaxis()->GetBinCenter(bin);

  printf("BCM1 average=%.1f (uA)  MPV=%.1f (uA), nEntries=%.0f\n",xAvr,xMPV,histA->GetEntries());
  double bcmThres=xMPV-5.0;
  cutFormula="("+cutFormula+Form(")&&(yield_qwk_bcm1.hw_sum >%.1f)",bcmThres);
  printf("new cutForm='%s'\n",cutFormula.Data());
  chain->Draw(">>listB",cutFormula); // cut includes BCM1 
  list = (TEventList*)gDirectory->Get("listB"); 
  list->Print();
  double listB_len= list->GetN();
  printf("main list B size=%d\n", list->GetN()); 

  eve.presetMyStat(chain->GetEntries(),  listA_len,bcmThres,listB_len);

  int t1=time(0);
  int ie;
  int seenEve=0;
  if(nEve>mxEve) nEve=mxEve;
  for( ie=0;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    //printf("ee=%d pat=%.1f\n",ie,eve.pattern());
    if(ie%5000==0) printf(" ieve=%d of %d , seen=%d ...\n",ie,nEve,seenEve);
    if(!list->Contains(ie)) { 
      if(blueTree) blueTree->FillEmpty(eve.pattern());
      continue;
   }
    //printf("xx %d \n",ie);
    if(!eve.unpackEvent()) {
      if(blueTree)blueTree->FillEmpty(eve.pattern());
      continue;
    }

    seenEve++;
    corA.addEvent(eve.Pvec, eve.Yvec);
    if(alphasM) {// regress dv's
      for (int iy = 0; iy <eve.ndv(); iy++) {
	YvecNew[iy]=eve.Yvec[iy];
	for (int ix = 0; ix < eve.niv(); ix++) {
	  YvecNew[iy]-=eve.Pvec[ix]*(*alphasM)(ix,iy);
	}
      }
      corB->addEvent(eve.Pvec, YvecNew);
      blueTree->Fill(eve.pattern(),YvecNew);
      
    }
  }// end of event loop

  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);
  if(seenEve <parMinEve) {
    printf("aborting linear regerssion due to small # of events\n");
    printf("#abort JB2 seenEve=%d\n",seenEve);
    mHfile->Write(); 
    mHfile->Close();
    exit(1);
  }

  printf("#seenEve %d\n",seenEve);

  corA.finish();
  eve.finish();

  if(alphasM) {
    corB->finish();
    blueTree->finish();
    mBlueFile->Write();
    mBlueFile->Close();
    printf("\n Histo saved -->%s<\n",treeOutName.Data());
  }

  mHfile->Write(); 
  mHfile->Close();
  TString outAlphas=Form("%sblueR%snew.slope.root",outPath,runName.Data());
  corA.exportAlphas(outAlphas, eve.ivName, eve.dvName); 
  if(alphasM) {
    outAlphas=Form("%sblueR%snewB.slope.root",outPath,runName.Data());
    corB->exportAlphas(outAlphas, eve.ivName, eve.dvName); 
  }


  corA.exportAlias(outPath, "regalias_"+runName, eve.ivName, eve.dvName); 

  printf("#success JB1 nEve=%d  for %s\n",nEve,runName.Data());
}

