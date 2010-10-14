/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Finds correlation and alphas for  QW event 
 *********************************************************************/

#include <cstdio>
#include <iostream>
using namespace std;

#include <TH2.h> 
#include <TProfile.h> 
#include <TList.h>
#include <TLine.h>
#include <TFile.h> 

#include <math.h>
#include <TMath.h>

#include "JbCorrelator.h"

//========================
//========================
JbCorrelator::JbCorrelator(const char *core) {
  mCore=core;
  printf("constr of JbCorrelator=%s\n",mCore.Data());
}

//========================
//========================
void JbCorrelator::print() {

#if 0
  printf("input params:\n    beam means \n");
  for(int i=0;i<;i++) printf("%10s : P%d avr=%f   alpha=%.3f\n",Pname2[i],i,par_Pavr[i], par_Alpha[i]);
#endif
}

//________________________________________________
//________________________________________________
void
JbCorrelator::addEvent(double *Pvec, double *Yvec){

  linReg.accumulate(Pvec, Yvec);

}


//========================
//========================
void JbCorrelator::init(int nP, int nY) {
  initHistos(); 
  linReg.setDims(nP, nY); 
  linReg.init();
}


//________________________________________________
//________________________________________________
void
JbCorrelator::initHistos(){
  printf("JbCorrelator::initHistos() - empty\n");
#if 0
  //...... data histograms
  memset(hA,0,sizeof(hA));
  TH1 *h; 
  
  //.....1D  P raw Y +heli histos [10-19]   
  //.....1D  P raw Y -heli histos [20-29] 
  int heliC[mxHeli]={36,kBlue};


  for(int ibp=0;ibp<niv();ibp++) {
    float x1=-1,x2=1;
    hA[10+ibp]=h=new TH1D(Form(mCore+"P%dpCr",ibp),Form("corr Y P%d heli +;meas %s (a.u.) ",ibp, ivName[ibp].Data()),500,x1,x2);
    h->SetLineColor(heliC[0]);
  }
#endif 
}


//________________________________________________
//________________________________________________
void
JbCorrelator::finish(){

  printf("::::::::::::::::JbCorrelator::finish(%s) :::::::::::\n",mCore.Data());
  linReg.printSummaryP();
  linReg.printSummaryY();
  linReg.solve();
  linReg.printSummaryAlphas();

}

