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
JbCorrelator::~JbCorrelator(){
  printf("destructor JbCorrelator=%s\n",mCore.Data());
  delete  [] h1iv;
  delete  [] h2iv;
  delete  [] h1dv;
  delete  [] h2dv;
}

//========================
//========================
void JbCorrelator::print() {

}


//========================
//========================
void
JbCorrelator::addEvent(double *Pvec, double *Yvec){
  linReg.accumulate(Pvec, Yvec);
  // .... monitoring 
  for(int i=0;i<nP;i++) {    
    h1iv[i]->Fill(Pvec[i]);
    for(int j=i+1;j<nP;j++) h2iv[i*nP+j]->Fill(Pvec[i],Pvec[j]);
  }
  for(int j=0;j<nY;j++) {
    h1dv[j]->Fill(Yvec[j]);
    for(int i=0;i<nP;i++)  h2dv[i*nY+j]->Fill(Pvec[i],Yvec[j]);
  }
  
}


//========================
//========================
void JbCorrelator::init(int nP0, int nY0, TString *Pname0, TString *Yname0) {
  nP=nP0;  nY=nY0;
  Pname=Pname0;   Yname=Yname0;
  initHistos(); 
  linReg.setDims(nP, nY); 
  linReg.init();
}


//________________________________________________
//________________________________________________
void
JbCorrelator::initHistos(){
  printf("JbCorrelator::initHistos()\n");
  TH1*h;

  //..... 1D,  iv
  h1iv=new TH1 *[nP];
  for(int i=0;i<nP;i++) {    
    h1iv[i]=h=new TH1D(Form(mCore+"P%d",i),Form("iv P%d=%s, pass=%s ;iv=%s",i,Pname[i].Data(),mCore.Data(),Pname[i].Data()),64,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
  }
  
  double x1=50e3;
  //..... 2D,  iv correlations
  h2iv=new TH1 *[nP*nP]; // not all are used
  for(int i=0;i<nP;i++) {    
    for(int j=i+1;j<nP;j++) {    
      h2iv[i*nP+j]=h=new TH2D(Form(mCore+"P%d_P%d",i,j),Form("iv correlation  P%d_P%d, pass=%s ;P%d=%s;P%d=%s",i,j,mCore.Data(),i,Pname[i].Data(),j,Pname[j].Data()),32,-x1,x1,32,-x1,x1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
    }
  }
  
  //..... 1D,  dv
  h1dv=new TH1 *[nY];
  for(int i=0;i<nY;i++) {    
    h1dv[i]=h=new TH1D(Form(mCore+"Y%d",i),Form("dv Y%d=%s, pass=%s ;dv=%s",i,Yname[i].Data(),mCore.Data(),Yname[i].Data()),64,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
  }
  
  
  double y1=3e3;
  //..... 2D,  dv-iv correlations
  h2dv=new TH1 *[nP*nY]; // not all are used
  for(int i=0;i<nP;i++) {    
    for(int j=0;j<nY;j++) {    
      h2dv[i*nY+j]=h=new TH2D(Form(mCore+"P%d_Y%d",i,j),Form("iv-dv correlation  P%d_Y%d, pass=%s ;P%d=%s;Y%d=%s",i,j,mCore.Data(),i,Pname[i].Data(),j,Yname[j].Data()),32,-x1,x1,32,-y1,y1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
    }
  }
  


#if 0
  //...... data histograms
  memset(hA,0,sizeof(hA));
 
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


//________________________________________________
//________________________________________________
void
JbCorrelator::exportAlphas(TString outPath){

  printf("::::::::::::::::JbCorrelator::exportAlphas(%s) :::::::::::\n",mCore.Data());
  TString hfileName=outPath+mCore+"_alphasX.root";
  TFile*  hFile=new TFile(hfileName,"RECREATE","correlation coefficents");
  
  TH2D *hal=new TH2D(mCore,"correlation coef, pass="+mCore+"iv index; dv index",nP,0,nP-1,nY,0,nY-1);
  for (int iy = 0; iy <nY; iy++) {
    cout << Form("dv=Y%d: ",iy)<<endl;
    for (int j = 0; j < nP; j++) {
      double val= linReg.mA(j,iy);
      double sig= linReg.mAsig(j,iy);
      // printf("iy=%d j=%d val=%f sig=%f\n",iy,j,val,sig);
      hal->SetBinContent(j+1,iy+1,val);
      hal->SetBinError(j+1,iy+1,sig);
    }
  }
  hFile->Write();
  hFile->Close();
  printf("saved %s\n",hFile->GetName());
}
