/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Main program , run correlation and alphas for  QW event 
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
  const char *inpPath="/home/cdaq/qweak/QwScratch/rootfiles/";
  const char * outPath="./";
  // decode input params
  printf("Syntax: %s [neve]    OR   %s  [neve] [skipEve]\n",argv[0], argv[0]);
  if ( argc>=2) mxEve=atoi(argv[1]);
  if ( argc>=3) skipEve=atoi(argv[2]);
  int runId=5848;
  int runSeq=0;


  // printf("Selected run=%d_%03i  mxEve=%d nSkip=%d\n", runId,runSeq,mxEve,skipEve);
 printf("Selected mxEve=%d nSkip=%d\n",mxEve,skipEve);

  //...... access to inpute leafs
  JbLeafTransform eve("tran"); // changes content of event
  JbCorrelator corA("Acor"); //   correlator , passive 

  //..... try to open known correlation coef's.......
  double *alphas=0; // indicator that coef's exist
  JbCorrelator *corB=0;
  double *YvecNew=0;
  TString corFileName="Acor_alphas.root";
  TFile*  corFile=new TFile(corFileName);
  if(corFile->IsOpen()) {
    TH2D *hal=(TH2D*) corFile->Get("Acor");
    if(hal) {
      printf("opened %s, Alphas found\n",corFile->GetName());
      assert(hal->GetXaxis()->GetNbins()==eve.nP);
      assert(hal->GetYaxis()->GetNbins()==eve.nY);
      alphas=new double [eve.nP*eve.nY];
      corB=new JbCorrelator("Bcor"); //   2nd correlator after regression
      YvecNew=new double [eve.nY];
      for (int iy = 0; iy <eve.nY; iy++) {
	for (int ix = 0; ix < eve.nP; ix++) {
	  double val= hal->GetBinContent(ix+1,iy+1);
	  double sig= hal->GetBinError(ix+1,iy+1);
	  printf("  iy=%d ix=%d val=%f sig=%f\n",iy,ix,val,sig);
	  alphas[ix*eve.nY +iy]=val;
	}
      }
    }
    corFile->Close();
  }
  
  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");  

 
#if 0
  int i=0;
  char text[100];
  sprintf(text,"%sQweak_%d.%03i.root",inpPath,runId, runSeq); 
  chain->Add(text);
  printf("%d =%s=\n",i,text);
#endif

  char *runL[]={"5848.000","5848.001","5820.002"}; int nr=1;
  for(int i=0;i<nr;i++) {
     TString treeFile=Form("%sQweak_%s.root",inpPath,runL[i]);
     chain->Add(treeFile);
     printf("%d =%s=\n",i,treeFile.Data());
  }
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain \nscan leafs for iv & dv ...\n",nEve);
  eve.findInputLeafs(chain);
  //eve.print();
  // return 0;

  TString hfileName=Form("%scorKent_%d.%03i.hist.root",outPath,runId, runSeq);
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak data");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());
  corA.init(eve.nP, eve.nY, eve.Pname, eve.Yname);   // creates histo 
  if(alphas) corB->init(eve.nP, eve.nY, eve.Pname, eve.Yname); 

  int t1=time(0);
  int ie;
  if(nEve>mxEve) nEve=mxEve;
  for( ie=skipEve;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    if(ie%5000==0) printf(" ieve=%d of %d ...\n",ie,nEve);
    if(!eve.unpackEvent()) continue;
    corA.addEvent(eve.Pvec, eve.Yvec);
    if(alphas) {// regress dv's
      for (int iy = 0; iy <eve.nY; iy++) {
	YvecNew[iy]=eve.Yvec[iy];
	for (int ix = 0; ix < eve.nP; ix++) {
	  YvecNew[iy]-=eve.Pvec[ix]*alphas[ix*eve.nY +iy];
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
  if(alphas) corB->finish();
  mHfile->Write(); 
  mHfile->Close();
  corA.exportAlphas(outPath); 

 



}


