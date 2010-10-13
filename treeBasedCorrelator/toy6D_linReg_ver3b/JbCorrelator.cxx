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
#include <TTree.h> 

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
JbCorrelator::addEvent(Double_t **valPA){

 
  // correlation uses spin averaged data

  //...... recover  iv
  for(int ip=0;ip<niv();ip++){
    Pvec[ip]=*valPA[ip];
    //printf("P%d %f\n",ip,Pvec[ip]);
    if( Pvec[ip]!=Pvec[ip]) return; // skip bad entreies
  }
 
  

  //...... recover  dv
  for(int iy=0;iy<ndv();iy++){
    Yvec[iy]=*valPA[niv()+iy];
    //printf("Y%d %f\n",iy,Yvec[iy]);
    if( Yvec[iy]!=Yvec[iy]) return; // skip bad entreies
  }
  //assert(2==3);
  
  //.... monitor correlations
  linReg.accumulate(Pvec, Yvec);

}


//========================
//========================
void JbCorrelator::init() {
  initHistos(); 
  linReg.setDims(niv(),ndv()); linReg.init();
  Pvec=new Double_t[niv()];
  Yvec=new Double_t[ndv()];
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
    if(ibp==4) { x1=1259.1; x2=1261.3;}
    if(ibp==5) { x1=0.4; x2=1.6;}
    hA[10+ibp]=h=new TH1D(Form(mCore+"P%dpCr",ibp),Form("corr Y P%d heli +;meas %s (a.u.) ",ibp, ivName[ibp].Data()),500,x1,x2);
    h->SetLineColor(heliC[0]);

    hA[20+ibp]=h=new TH1D(Form(mCore+"P%dnCr",ibp),Form("corr Y P%d heli -; meas %s (a.u.) ",ibp, Pname2[ibp]),500,x1,x2);
    h->SetLineColor(heliC[1]);
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

#if 0
  double DelP[mxP];
  double falseAsy=0;
  for(int ip=0;ip<mxP;ip++) {
    double avr[mxHeli];
    for(int ih=0;ih<mxHeli;ih++)  avr[ih]=mSumP[ip][ih]/mEve[ih];
    DelP[ip]=avr[kheliP]-avr[kheliN];
    falseAsy+=DelP[ip]*linReg.Alpha(ip);
    printf("%10s : P%d  avr +heli=%f -heli=%f del=%f  foundAlpha=%f\n",Pname2[ip],ip,avr[0],avr[1],DelP[ip],linReg.Alpha(ip));

  }
  falseAsy/=mYmeas[kheliP]/mEve[kheliP] + mYmeas[kheliN]/mEve[kheliN];
  printAsy(mYmeas,"uncorrected yields");
  printf("       Asy correction =%f  based on found alphas\n",falseAsy);
  printAsy(mYcorr,"corrected yields");
 
#endif
 
  delete [] Pvec ;
  delete [] Yvec;

}

#if 0
//________________________________________________
//________________________________________________
void
JbCorrelator::printAsy(double *mY, const char * txt){
 //...... 
 double asy=999, err=888;
 double sum=mY[kheliP] + mY[kheliN];
 if(sum>5) {
   asy=(mY[kheliP] - mY[kheliN])/sum;
   err=1./sqrt(sum);
 }
 printf("Compute %s asy =%12.2g +/-%12.2g (nSig=%.2f)\n",txt,asy,err,asy/err);

}

#endif
