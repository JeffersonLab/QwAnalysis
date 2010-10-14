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
#include <TTree.h> 
#include <TLeaf.h>//tmp 
#include <TChain.h>
#include <TObjArray.h>

#include "JbLeafTransform.h"
#include "JbCorrelator.h"
// use only double

int main(int argc, char *argv[]) {
  int mxEve=5000, skipEve=0;

  // decode input params
  printf("Syntax: %s [neve]    OR   %s  [neve] [skipEve]\n",argv[0], argv[0]);
  if ( argc>=2) mxEve=atoi(argv[1]);
  if ( argc>=3) skipEve=atoi(argv[2]);

  printf("Selected  mxEve=%d nSkip=%d\n", mxEve,skipEve);
 
  // read TTree  with precompiled class

  const char * outPath="./";

  //...... access to inpute leafs
  JbLeafTransform eve("trA"); // changes content of event
  JbCorrelator corA("corA"); //   correlator , passive 

  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");  
  const char *inpPath=outPath;
 
  int i=0;
  char text[100];
  sprintf(text,"%sQweak_5762.000.root",inpPath); // freash file, Oct-12-2010
  chain->Add(text);
  printf("%d =%s=\n",i,text);
  
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain \nscan leafs for iv & dv ...\n",nEve);
  eve.findInputLeafs(chain);
  //eve.print();

  corA.init(eve.nP, eve.nY);   

  TString hfileName=outPath;  hfileName+="/toy6Dregr.hist.root";
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak yields");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());

  int t1=time(0);
  int ie;
  if(nEve>mxEve) nEve=mxEve;
  for( ie=skipEve;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    if(!eve.unpackEvent()) continue;
    corA.addEvent(eve.Pvec, eve.Yvec);
  }
  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);
  corA.finish(); 
  // mHfile->Write();



}


