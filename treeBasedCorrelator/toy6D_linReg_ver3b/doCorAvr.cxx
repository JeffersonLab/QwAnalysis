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

#include "JbCorrelator.h"
// use only double

int main(int argc, char *argv[]) {
  int mxEve=50000, skipEve=0;

  // decode input params
  printf("Syntax: %s [neve]    OR   %s  [neve] [skipEve]\n",argv[0], argv[0]);
  if ( argc>=2) mxEve=atoi(argv[1]);
  if ( argc>=3) skipEve=atoi(argv[2]);

  printf("Selected  mxEve=%d nSkip=%d\n", mxEve,skipEve);
 
  // read TTree  with precompiled class

  const char * outPath="./";

  //........  correlator class ...............
  JbCorrelator corA("corA"); // Uncorrected 
  const char *confName="controlAvr.conf";
  FILE *fp =   fopen(confName, "rb"); assert(fp);
  assert(corA.harvestNames(fp)==2); // otherwise config file is worng
  fclose(fp);
  //1 corA.print();
  corA.init();   
  // corA.printCE();   
  //...... setup container for tree-variables
  int nP=corA.ivName.size(), nY=corA.dvName.size(), nPY=nP+nY;
  assert(nP>0);
  assert(nY>0);  
  Double_t **eveValueYpn=new Double_t*[2*nY];// dv , separately for pos& neg
  Double_t eveValueYavr[nY]; // computed in fly
  Double_t **eveValueP=new Double_t*[nPY];// iv + dv
  
  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");  
  const char *inpPath=outPath;
  int i=0;
  char text[100];

  sprintf(text,"%sQweak_5762.000.root",inpPath); // freash file, Oct-12-2010

  chain->Add(text);
  printf("%d =%s=\n",i,text);
  
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain  nPY=%d,\nscan leafs for iv & dv ...\n",nEve,nPY);

  // special treatement for dv's , they will by averaged in fly
  for(int i=0;i<nP;i++){
    TString name=corA.ivName[i];
    name.ReplaceAll(".","/");
    TLeaf *lf=chain->GetLeaf(name); 
    printf("%d  leaf=%s=  ptr=%p\n",i,name.Data(),lf);
    assert(lf);
    eveValueP[i]=(Double_t*)lf->GetValuePointer();    
    assert(eveValueP[i]);
  }

  for(int i=0;i<nY;i++){
    TString name=corA.dvName[i];
    name.ReplaceAll(".","/");
    eveValueP[i+nP]=&eveValueYavr[i];

   //..... positive ....
    name.ReplaceAll("avr","pos");
    TLeaf *lf=chain->GetLeaf(name); 
    printf("%d  leaf=%s=  ptr=%p\n",i,name.Data(),lf);
    assert(lf);
    assert(eveValueYpn[2*i+0]=(Double_t*)lf->GetValuePointer());    
 
   //..... negative ....
    name.ReplaceAll("pos","neg");
    lf=chain->GetLeaf(name); 
    printf("%d  leaf=%s=  ptr=%p\n",i,name.Data(),lf);
    assert(lf);
    assert(eveValueYpn[2*i+1]=(Double_t*)lf->GetValuePointer());    
  }


  // eveValues[0]=(Double_t*)chain->GetLeaf("beam_nx/hw_sum")->GetValuePointer();

 
  //for(int iy=0;iy<nPY;iy++) printf("i=%d p=%p\n",iy,eveValueP[iy]);
  // chain->Print();
  // chain->Show(0); // event id startting from 0
  // 

  TString hfileName=outPath;  hfileName+="/toy6Dregr.hist.root";
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak yields");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());

  int t1=time(0);
  int ie;
  if(nEve>mxEve) nEve=mxEve;
  for( ie=skipEve;ie<nEve;ie++) { 
    chain->GetEntry(ie);

    //  dv's pos+neg are  averaged in fly
    for(int i=0;i<nY;i++)
      eveValueYavr[i]=0.5*( (*eveValueYpn[2*i+0]) +(*eveValueYpn[2*i+1]));

    if(ie%5000==0)
      printf("--- done eve=%d of %d  .. Bpm[0]=%f %f\n",ie,nEve,*eveValueP[0],*eveValueP[1]);
    corA.addEvent(eveValueP); 
  }
  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);
  corA.finish(); 
  mHfile->Write();


}


