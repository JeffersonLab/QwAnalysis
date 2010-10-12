/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Generator of  synthetic QW event with build in fals asymmetries
 *********************************************************************/

//#include <ostream>
using namespace std;

#include <TH2.h> 
#include <TList.h>
#include <TMarker.h> 
#include <TFile.h> 
#include <TTree.h> 

#include <math.h>
#include <TMath.h>
#include <TRandom3.h>

#include "JbToy6DGen.h"
static ToyQwChannel myQwChannel[mxPY]; // iv + dv
Double_t pattern_number;  // holds helicity info

const char *Pname[mxPY]={"beam_x","beam_nx","beam_y","beam_ny","beam_E","beam_Q","yield_MD1L","yield_MD1R","yield_MD2L","yield_MD2R"};


//========================
//========================
JbToy6DGen::JbToy6DGen() {
  printf("constr of JbToy6DGen mxPY=%d\n",mxPY);
  rnd=new TRandom3;
  mTree = new TTree("Hel_Tree","M-C Qweak events for regression analysis.");
  mTree->Branch("pattern_number",&pattern_number,"pattern_number/D");
  for(int ip=0;ip<mxPY;ip++)
    mTree->Branch(Pname[ip],myQwChannel+ip,"block0/D:block1:hw_sum");
  
  memset(par_Alpha, 0, sizeof(par_Alpha));
  memset(par_Ymean,0,sizeof(par_Ymean));
  memset(par_Ysig,0,sizeof(par_Ysig));
  memset(mYield, 0, sizeof(mYield));
  mTotEve=0;
  pattern_number=0; // works as event counter for the tree
}

//========================
void JbToy6DGen::setSeed(int seed) {  
    rnd->SetSeed(seed);  
    printf("JbToy6DGen seed=%d\n",seed); 
}



//________________________________________________
//________________________________________________
void
JbToy6DGen::throwEvent(int ieve){
  mTotEve++;
   
  // clear old event
  for(int ip=0;ip<mxPY;ip++)
    memset(myQwChannel,0,sizeof(myQwChannel));
  
  int heli=mTotEve%2; // assign helicity 0=+, 1=-    
  pattern_number=mTotEve;

  double P[mxP]; // those are true independent beam params: x,nx,y,ny,E,Q
  
  //........... generate new beam position  in X-nX plane (and in Y-nY plane)
  for(int ix=0;ix<mXY;ix++) {
    double rnd1=rnd->Gaus(0,par_sig1[ix]);
    double rnd2=rnd->Gaus(0,par_sig2[ix]);
    P[0+2*ix]= par_P[0+2*ix][heli] + rnd1*mCosA[ix] - rnd2*mSinA[ix];//position
    P[1+2*ix]= par_P[1+2*ix][heli] + rnd1*mSinA[ix] + rnd2*mCosA[ix];//angle
  }
  
  //........ generate beam energy and charge fluctuation  
  P[4]=rnd->Gaus(par_P[4][heli],par_sigE);// energy
  P[5]=rnd->Gaus(par_P[5][heli],par_sigQ); // charge
  
  //..... add correlation resulting from  beam displacements
  for(int iy=0;iy<mxY;iy++) {// loop over dv
    // start with unbiased yield 
    double yield=rnd->Gaus(par_Ymean[iy],par_Ysig[iy]); 
    for(int ip=0;ip<mxP;ip++){ //introduce linear correlations
      yield+= (P[ip]- par_Pavr[ip]) *  par_Alpha[iy][ip];
    } 
    myQwChannel[mxP+iy].hw_sum=yield; 
    
    // local QA 
    mYield[iy][heli]+=yield;
  }

  //..... smear true beam params as measured by BPMs, only diagonal
  for(int ip=0;ip<mxP;ip++){
    myQwChannel[ip].hw_sum= rnd->Gaus(P[ip],par_sigBpm[ip]);      
  }

  mTree->Fill(); //..... save event in to tree
   

  //.... accumulate internal sums for QA, remove later
  // .... QA histos ....
  // 2D for iv
  ((TH2F*)hA[1])->Fill(  myQwChannel[0].hw_sum, myQwChannel[1].hw_sum);
  ((TH2F*)hA[2])->Fill(  myQwChannel[2].hw_sum, myQwChannel[3].hw_sum);
  ((TH2F*)hA[3])->Fill(  myQwChannel[4].hw_sum, myQwChannel[5].hw_sum);
  
  // 1D for iv
  for(int ip=0;ip<mxP;ip++){
    hA[10+ip]->Fill( myQwChannel[ip].hw_sum);
  }
 // 1D for dv
  for(int iy=0;iy<mxY;iy++){
    hA[20+iy]->Fill( myQwChannel[mxP+iy].hw_sum);
  }

}

//========================
//========================
void JbToy6DGen::print() {

  printf("Input params:\n   gen beam means \n");
  for(int i=0;i<mxP;i++) printf("%10s :P%d avr=%f   Del=%f   sigBmp=%f\n",Pname[i],i,par_Pavr[i],par_P[i][kheliP]-par_P[i][kheliN], par_sigBpm[i]);
  printf("sread beam in X  : s1=%.3f s2=%.3f corAngle=%.1f\n", par_sig1[0],par_sig2[0],par_phi[0]);
  printf("spread beam in Y : s1=%.3f s2=%.3f corAngle=%.1f\n", par_sig1[0],par_sig2[0],par_phi[0]);
  printf("spread beam in E=%.3f  Q=%.3f \n", par_sigE,par_sigQ);

  printf("assumed ALPHAS for %d yields\n                    ",mxY);  
  for(int iy=0;iy<mxY;iy++) printf("%12s  ",Pname[mxP+iy]);
  printf("\n           Ymean:  ");
  for(int iy=0;iy<mxY;iy++) printf("%12f ",par_Ymean[iy]);
  printf("\n           Ysigma: ");
  for(int iy=0;iy<mxY;iy++) printf("%12f ",par_Ysig[iy]);
  printf("\n");
  for(int ip=0;ip<mxP;ip++) {
    printf("%10s :P%d alpha=> ",Pname[ip],ip);
    for(int iy=0;iy<mxY;iy++) printf("%9f    ",par_Alpha[iy][ip]);
    printf("\n");
  }
  
  
}

//========================
//========================
void JbToy6DGen::init() {

  const double PI=TMath::Pi();
  const double deg2rad=PI/180.;
  
  for(int ix=0;ix<mXY;ix++) {
    mCosA[ix]=cos(par_phi[ix]*deg2rad);  
    mSinA[ix]=sin(par_phi[ix]*deg2rad);
  }
  
  for(int i=0;i<mxP;i++)  {
    par_Pavr[i]= (par_P[i][kheliP]+par_P[i][kheliN])/2.;
  }
  
  initHistos(); 

}


//________________________________________________
//________________________________________________
void
JbToy6DGen::initHistos(){
 
  //...... data histograms
  memset(hA,0,sizeof(hA));
  TList *Lx; TH1 *h; TMarker *mar;
  hA[1]=h=new TH2F("beamXnX","Beam position X-nx; BPM x(mm); BPM nx",100,-1.4,1.4,100,-1.,1.);
   
  // center markers
  Lx=h->GetListOfFunctions(); 
  int heliC[mxHeli]={kBlack,kBlue};
  for(int heli=0;heli<mxHeli;heli++){
    mar=new TMarker(par_P[0][heli],par_P[1][heli],5);
    Lx->Add(mar);mar->SetMarkerColor(heliC[heli]); 
    mar->SetMarkerSize(2); 
  }
  
    hA[2]=h=new TH2F("beamYnY","Beam position Y-ny; BPM y(mm); BPM ny",100,-1.4,1.4,100,-1.,1.);
  // center markers
  Lx=h->GetListOfFunctions();
  for(int heli=0;heli<mxHeli;heli++){
    mar=new TMarker(par_P[2][heli],par_P[3][heli],5);
    Lx->Add(mar);mar->SetMarkerColor(heliC[heli]); 
    mar->SetMarkerSize(2); 
  }


  double fac=8;
  double x1=par_P[4][kheliP]-fac*par_sigE,x2=x1+2*fac*par_sigE;
  double y1=par_P[5][kheliP]-fac*par_sigQ,y2=y1+2*fac*par_sigQ;
  hA[3]=h=new TH2F("beamEQa","Beam true Q vs. E angle, as generated; true E (a.u.); true charge (a.u.)",100,x1,x2,100,y1,y2);
  Lx=h->GetListOfFunctions();
  // center markers
  Lx=h->GetListOfFunctions();
  for(int heli=0;heli<mxHeli;heli++){
    mar=new TMarker(par_P[4][heli],par_P[5][heli],5);
    Lx->Add(mar);mar->SetMarkerColor(heliC[heli]); 
    mar->SetMarkerSize(2); 
  }

  // free 4-9

  //..... BPM's histos [10-19] - reserved
  for(int ibp=0;ibp<mxP;ibp++) {
    float x1=-1,x2=1;
    if(ibp>=4) {// E or Q , have no correlation and average is NE 1
      x1=x2=par_P[ibp][kheliP];
      if(ibp==4) { x1-=fac*par_sigE; x2+=fac*par_sigE; }
      if(ibp==5) { x1-=fac*par_sigQ; x2+=fac*par_sigQ; }     
    } 
    hA[10+ibp]=h=new TH1F(Form("bpm%d",ibp),Form("BPM%d  , spin averaged; matched to %s(a.u.) ",ibp, Pname[ibp]),1000,x1,x2);
  }
  
  //..... yields histos [20-29] - reserved
  double delx=0.6;
  for(int iy=0;iy<mxY;iy++) {
    float x1=par_Ymean[iy]-delx,x2=x1+2*delx;
    hA[20+iy]=h=new TH1F(Form("Y%d",iy),Form("yield%d , spin averaged; %s ",iy, Pname[mxP+iy]),1000,x1,x2);
  }  
}


//________________________________________________
//________________________________________________
void
JbToy6DGen::finish(){
  printf("::::::::::::::::JbToy6DGen::finish() :::::::::::\n");

  mTree->Print(); 
  mTree->Show(1);// event id startting from 0
 
  //......  
  printf("\ngenerated false ASY for %d yields based on %d events\n",mxY,mTotEve);  
  for(int iy=0;iy<mxY;iy++) {
    

    double asy=999, err=888;
    double sum=mYield[iy][kheliP] + mYield[iy][kheliN];
    double avrY=99999;
    if(sum>5) {
      asy=(mYield[iy][kheliP] - mYield[iy][kheliN])/sum;
      err=1./sqrt(sum);
      avrY=sum/mTotEve;
    }
    printf("%s :  avrY=%f,  ASY=%12.2g +/-%12.2g  (nSig=%.1f)\n",Pname[mxP+iy],avrY,asy,err,asy/err);
  }  
}
