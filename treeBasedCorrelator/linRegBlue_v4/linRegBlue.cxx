/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Main program , run correlation and computes alphas for  QW event 
 * special version, automaticaly averaged pos+neg signals
 *********************************************************************/

#include <stdio.h>  
//#include <cstdio>  // C++ standard since 1999
#include <TFile.h>
#include <TH2.h>
#include <TTree.h> 
#include <TLeaf.h>//tmp 
#include <TChain.h>
#include <TObjArray.h>

#include "JbLeafTransform.h"
#include "JbCorrelator.h"
// use only double

int main(int argc, char *argv[]) {
  int mxEve=500, skipEve=0;
  // const char *inpPath="/home/cdaq/qweak/QwScratch/rootfiles/"; //cdaq
  const char *inpPath="/u/home/cdaq/balewski_tmp/"; //ifarm4, R 5762.000
  const char * outPath="./";

  // decode input params
  if(argc<2) {
    printf("Syntax: %s runNo.runSeq  [neve] [skipEve]\n STOP",argv[0]);
    printf("    e.g.  %s 5762.000 500 \n",argv[0]); return -1;
  }
  char *runName=argv[1];
  if ( argc>=3) mxEve=atoi(argv[2]);
  if ( argc>=4) skipEve=atoi(argv[3]);
  int runId=atoi(runName);

  printf("Selected run=%s  mxEve=%d nSkip=%d\n", runName,mxEve,skipEve);
  
  //...... access to inpute leafs
  JbLeafTransform eve("tran"); // changes content of event
  JbCorrelator corA("input"); //   correlator , passive 

  //..... try to open known correlation coef's.......
  JbCorrelator *corB=0;
  double *YvecNew=0;
  TString corFileName=Form("R%d_alphas.root",runId);
  TFile*  corFile=new TFile(corFileName);
  TMatrixD *alphasM=0; // indicator that coef's exist
  if(corFile->IsOpen()) {
    alphasM=(TMatrixD *) corFile->Get("alphas");
    if(alphasM) {
      corB=new JbCorrelator("regres"); //   2nd correlator after regression
      YvecNew=new double [eve.nY];
 
      printf("opened %s, Alphas found, list below\n",corFile->GetName());
      for (int iy = 0; iy <eve.nY; iy++) {
	for (int ix = 0; ix < eve.nP; ix++) {
	  printf("  iy=%d ix=%d val=%f \n",iy,ix,(*alphasM)(ix,iy));
	}
      }
    }
    corFile->Close();
  } else
    printf("Failed to open %s, Alphas NOT found\n",corFile->GetName());
  
  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");  

  char *runL[]={runName}; int nr=1;
  for(int i=0;i<nr;i++) {
    TString treeFile=Form("%sQweak_%s.root",inpPath,runL[i]);
    chain->Add(treeFile);
    printf("%d =%s=\n",i,treeFile.Data());
  }
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain \nscan leafs for iv & dv ...\n",nEve);
  eve.findInputLeafs(chain);
  //eve.print();
  //  return 0;

  TString hfileName=Form("%sR%s.hist.root",outPath,runName);
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak data");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());
  corA.init(eve.nP, eve.nY, eve.Pname, eve.Yname);   // creates histo 
  if(alphasM) corB->init(eve.nP, eve.nY, eve.Pname, eve.Yname); 

  int t1=time(0);
  int ie;
  if(nEve>mxEve) nEve=mxEve;
  for( ie=skipEve;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    if(ie%500==0) printf(" ieve=%d of %d ...\n",ie,nEve);
    if(!eve.unpackEvent()) continue;
    corA.addEvent(eve.Pvec, eve.Yvec);
    if(alphasM) {// regress dv's
      for (int iy = 0; iy <eve.nY; iy++) {
	YvecNew[iy]=eve.Yvec[iy];
	for (int ix = 0; ix < eve.nP; ix++) {
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
  corA.finish();
  if(alphasM) corB->finish();
  mHfile->Write(); 
  mHfile->Close();
  TString outAlphas=Form("%sR%d_alphasNew.root",outPath,runId);
  corA.exportAlphas(outAlphas); 

}


