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
  h1iv=0;
  printf("constr of JbCorrelator=%s\n",mCore.Data());
}

//========================
//========================
JbCorrelator::~JbCorrelator(){
  printf("destructor JbCorrelator=%s\n",mCore.Data());
  if(h1iv) { // only if previously allocated
    delete  [] h1iv;
    delete  [] h2iv;
    delete  [] h1dv;
    delete  [] h2dv;
  }
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
    h->SetBit(TH1::kCanRebin);
  }
  
  double x1=5e2;
  //..... 2D,  iv correlations
  h2iv=new TH1 *[nP*nP]; // not all are used
  for(int i=0;i<nP;i++) {    
    for(int j=i+1;j<nP;j++) {    
      h2iv[i*nP+j]=h=new TH2D(Form(mCore+"P%d_P%d",i,j),Form("iv correlation  P%d_P%d, pass=%s ;P%d=%s;P%d=%s     ",i,j,mCore.Data(),i,Pname[i].Data(),j,Pname[j].Data()),64,-x1,x1,64,-x1,x1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
      h->SetBit(TH1::kCanRebin); 
    }
  }
  
  //..... 1D,  dv
  h1dv=new TH1 *[nY];
  for(int i=0;i<nY;i++) {    
    h1dv[i]=h=new TH1D(Form(mCore+"Y%d",i),Form("dv Y%d=%s, pass=%s ;dv=%s",i,Yname[i].Data(),mCore.Data(),Yname[i].Data()),64,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
    h->SetBit(TH1::kCanRebin);
  }
  
  
  double y1=3e1;
  //..... 2D,  dv-iv correlations
  h2dv=new TH1 *[nP*nY]; // not all are used
  for(int i=0;i<nP;i++) {    
    for(int j=0;j<nY;j++) {    
      h2dv[i*nY+j]=h=new TH2D(Form(mCore+"P%d_Y%d",i,j),Form("iv-dv correlation  P%d_Y%d, pass=%s ;P%d=%s;Y%d=%s     ",i,j,mCore.Data(),i,Pname[i].Data(),j,Yname[j].Data()),64,-x1,x1,64,-y1,y1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
      h->SetBit(TH1::kCanRebin);
    }
  }
  
}


//________________________________________________
//________________________________________________
void
JbCorrelator::finish(){

  printf("::::::::::::::::JbCorrelator::finish(%s) :::::::::::START\n",mCore.Data());
  linReg.printSummaryP();
  linReg.printSummaryY();
  linReg.solve();
  linReg.printSummaryAlphas();

  // assemble string for automatic WEB-based monitoring
  TString rmsStr, avrStr;
  double avrL=99999.99,avrH=99999.9;
  for (int i = nY-1; i >=0; i--) {
    double meanI,sigI;
    assert( linReg.getMeanY(i,meanI)==0);
    assert( linReg.getSigmaY(i,sigI)==0);
    if(i==nY-1) avrL=avrH=meanI;
    else {
      if(avrL>meanI) avrL=meanI;
      if(avrH<meanI) avrH=meanI;
    }
    if(sigI<10000)
      rmsStr+=Form("<td> %.0f",sigI);
    else
      rmsStr+=Form("<td> > 10k ");

  }
  cout<<"#"<<mCore<<"RMS,"<<rmsStr<<endl;
  cout<<Form("#%sAVR, %.1f  to  %.1f",mCore.Data(),avrL, avrH) <<endl;


  printf("::::::::::::::::JbCorrelator::finish(%s) :::::::::::END\n",mCore.Data());
}


//________________________________________________
//________________________________________________
void
JbCorrelator::exportAlphas(TString outName){
  printf("::::::::::::::::JbCorrelator::exportAlphas(%s) :::::::::::\n",outName.Data());  
  TFile*  hFile=new TFile(outName,"RECREATE","correlation coefficents");

  linReg.mA.Write("alphas");
  linReg.mAsig.Write("sigma");
  hFile->Close();
  printf("saved %s\n",hFile->GetName());
}



//________________________________________________
//________________________________________________
void
JbCorrelator::exportAlias(char * outName, int runId){

  printf("::::::::::::::::JbCorrelator::exportAlias(%s) :::::::::::\n",outName);
  char *preDV="asym_";
  char *preIV="diff_qwk_bpm";

  FILE *fd=fopen(outName,"w");
  fprintf(fd,"regalias_r%d() {\n",runId);
  for (int iy = 0; iy <nY; iy++) {
    fprintf(fd,"  Hel_Tree->SetAlias(\"reg_%s%s\",\n         \"%s%s",preDV,Yname[iy].Data(),preDV,Yname[iy].Data());
    for (int j = 0; j < nP; j++) {
      double val= -linReg.mA(j,iy);
      if(val>0)  fprintf(fd,"+");
      fprintf(fd,"%.4e*%s%s",val,preIV,Pname[j].Data());
    }
    fprintf(fd,"\");\n");

  }


#if 0 // format
regalias_r5848() {
	Hel_Tree->SetAlias("reg_<dv1>","<dv1> +<slp_dv1_iv1>*<iv1>+<slp_dv1_iv2>*<iv2>+...");
	Hel_Tree->SetAlias("reg_<dv2>","<dv2> +<slp_dv2_iv1>*<iv1>+<slp_dv2_iv2>*<iv2>+...");
	.
	.
}

#endif 
  fprintf(fd,"}\n");
  fclose(fd);
  printf("saved %s\n",outName);
}
