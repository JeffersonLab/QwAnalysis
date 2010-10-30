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
// use only double

int main(int argc, char *argv[]) {
  int mxEve=500, skipEve=0;
  int runSeg=0;
  const char * inpPath="/home/cdaq/qweak/QwScratch/rootfiles/QwPass1_"; //cdaq:Pass1
  //const char * inpPath="/group/qweak/QwAnalysis/common/QwScratch/rootfiles/QwPass1.1_";//*QwPass1.1*

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

  printf("Selected run=%s  mxEve=%d  skipEve=%d config=%s  slope=%s\n", runName.Data(),mxEve,skipEve,configFName,slopeFName);


  //...... access to inpute leafs
  JbLeafTransform eve("filter"); // filter content of run, unpacks event
  eve.readConfig(configFName);
  JbCorrelator corA("input"); //   correlator , passive 

  //..... try to open known correlation coef's.......
  JbCorrelator *corB=0;
  double *YvecNew=0;
  TMatrixD *alphasM=0; // indicator that coef's exist
  if( slopeFName) {
    TString corFileName=Form("%s%s",outPath,slopeFName);
    TFile*  corFile=new TFile(corFileName);
    if( !corFile->IsOpen()) {
      printf("Failed to open %s, Alphas NOT found\n",corFile->GetName());
      return 0;
    }
    alphasM=(TMatrixD *) corFile->Get("slopes");
    assert(alphasM);
    corB=new JbCorrelator("regres"); //   2nd correlator after regression
    YvecNew=new double [eve.ndv()];
    
    printf("opened %s, Alphas found, dump:\n",corFile->GetName());
    alphasM->Print();
    corFile->Close();
  }

  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");  
  TString treeFile=Form("%s%s.root",inpPath,runName.Data());
  printf("add =%s=\n",treeFile.Data());
  chain->Add(treeFile);

  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain \nscan leafs for iv & dv ...\n",nEve);
  printf("#totEve %d\n",nEve);
  if(nEve <5000) {
    printf("aborting lin regerssion due to small # of events\n");
    printf("#abort JB1 nEve=%d\n",nEve);
    exit(1);
  }

  TString hfileName=Form("%sblueR%s.hist.root",outPath,runName.Data());
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak data");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());

  eve.init();  // creates histo: WARN: output histo file must be already opened   
  eve.findInputLeafs(chain);
  corA.init( eve.ivName, eve.dvName );   // creates histo 
  if(alphasM) corB->init( eve.ivName, eve.dvName); 


  // filter events with custom cut.
  TString cutFormula="1==1"; // alwasy true
  if(eve.cutFormula.Sizeof()>1) {
    printf("Main: filter events with custom cut: name=%s  formula='%s'\n",eve.cutName.Data(), eve.cutFormula.Data());
    cutFormula=eve.cutFormula;
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
  for( ie=skipEve;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    if(ie%5000==0) printf(" ieve=%d of %d , seen=%d ...\n",ie,nEve,seenEve);
    if(!list->Contains(ie)) continue;
    //printf("xx %d \n",ie);
    if(!eve.unpackEvent()) continue;
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
    }
  }// end of event loop

  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);
  printf("#seenEve %d\n",seenEve);

  corA.finish();
  eve.finish();

  if(alphasM) corB->finish();
  mHfile->Write(); 
  mHfile->Close();
  TString outAlphas=Form("%sblueR%snew.slope.root",outPath,runName.Data());
  corA.exportAlphas(outAlphas); 

  TString outAlias=Form("%sregalias_r%s.C",outPath,runName.Data());
  corA.exportAlias((char*)outAlias.Data(), runNo, eve.ivName, eve.dvName); 

  printf("#success JB1 nEve=%d  for %s\n",nEve,runName.Data());
}


