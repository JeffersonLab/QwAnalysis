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
#include <TChain.h> 
#include <TLeaf.h>

#include <math.h>
#include <TMath.h>


#include "JbLeafTransform.h"

//========================
//========================
JbLeafTransform::JbLeafTransform(const char *core) {
  mCore=core;
  printf("constr of JbLeafTransform=%s\n",mCore.Data());
}


//========================
//========================
void JbLeafTransform::findInputLeafs(TChain *chain){
  printf("JbLeafTransform_%s findInputLeafs\n",mCore.Data());
  TString cBPM[mxBPM]={"3h04X","3h04Y","3h09X","3h09Y","3c12X"};

  // set  iv names
  for(int i=0;i<nP;i++) Pname[i]=cBPM[i];

  // Access MD leafs
  for(int imd=0;imd<mxMD;imd++){
    Yname[imd]=Form("md%d",imd+1);
    TString name="asym_"+Yname[imd]+"barsum/hw_sum";
    TLeaf *lf=chain->GetLeaf(name); 
    printf("imd=%d  name=%s= lf=%p\n",imd,name.Data(),lf);
    assert(lf);
    Double_t* p=(Double_t*)lf->GetValuePointer();    
    assert(p);
    pLeafMD[imd]=p;    
  }

  // set  remaining dv names
  Yname[8]="mdh";
  Yname[9]="mdv";
  Yname[10]="mdd1";
  Yname[11]="mdd2";

  Yname[12]="mdc";
  Yname[13]="mdx";
  Yname[14]="mda";


#if 1
  {  // Access other iv leafs
    TString other="yield_qwk_bcm1/hw_sum";
    TLeaf *lf=chain->GetLeaf(other);
    assert(lf);
    Double_t* p=(Double_t*)lf->GetValuePointer();    
    assert(p);
    pLeafBCM=p;
  }
#endif


  // Access dv leafs
  for(int ib=0;ib<mxBPM;ib++){
    TString name="diff_qwk_bpm"+cBPM[ib]+"/hw_sum";
    TLeaf *lf=chain->GetLeaf(name); 
    printf("ib=%d name=%s= lf=%p\n",ib,name.Data(),lf);
    assert(lf);
    Double_t* p=(Double_t*)lf->GetValuePointer();    
    assert(p);
    pLeafBPM[ib]=p;
  }
  
}


//========================
//========================
bool JbLeafTransform::unpackEvent(){
  hA[0]->Fill("inp", 1.);
  // printf("\nJbLeafTransform_%s unpackEvent\n",mCore.Data());
  double amd[mxMD];

  // Access MD leafs
  for(int imd=0;imd<mxMD;imd++){
    amd[imd]=1e6*(*pLeafMD[imd]);
    // printf("  imd=%d amd=%g\n",imd,amd[imd]);
  }
  
  /* arrays w/ final 15 dv & iv variables
     1-bar tube combos:    MD 1-8 (all bars)
     2-bar combos:             MD H (3+7), V (1+5), D1 (2+6), D2(4+8)
     4-bar combos:             MD C (1+3+5+7), MD X (2+4+6+8)
     all:	                               MD ALL (1+2+3+4+5+6+7+8)

     named: md1,...,8, h,v,d1,d2,c,x,all
  */


  double mix[nY];
  for(int i=0;i<mxMD;i++) mix[i]=amd[i];
  mix[8]=(mix[2]+mix[6])/2.;   //mdh
  mix[9]=(mix[0]+mix[4])/2.;   //mdv
  mix[10]=(mix[1]+mix[5])/2.;  //mdd1
  mix[11]=(mix[3]+mix[7])/2.;  //mdd2

  mix[12]=(amd[0]+amd[2]+amd[4]+amd[6])/4.; //mdc
  mix[13]=(amd[1]+amd[3]+amd[5]+amd[7])/4.; //mdx
  mix[14]=(mix[12]+mix[13])/2.; //mda


  // compute final dv's
  for(int i=0;i<nY;i++) {
    Yvec[i]=mix[i];
    if(Yvec[i]!=Yvec[i]) return false; // corrupted event
    //printf("%s dv_i=%d val=%g,   mix=%g bcm=%g  \n",Yname[i].Data(),i,Yvec[i],mix[i],bcm);    
  }

  // copy iv's to unify the interface
  for(int i=0;i<nP;i++) {
    Pvec[i]=1e6*(*pLeafBPM[i]);
    if(Pvec[i]!=Pvec[i]) return false; // corrupted event
    // printf("%s iv_i=%d val=%g \n",Pname[i].Data(),i,Pvec[i]);
  }

  double bcm1_uA=(*pLeafBCM);
  
  if(bcm1_uA!= bcm1_uA)  return false; // corrupted event
  hA[1]->Fill(bcm1_uA);
  
  hA[0]->Fill("ok", 1.);
  return true;
  
}




//========================
//========================
void JbLeafTransform::init() {
  initHistos(); 
}


//________________________________________________
//________________________________________________
void
JbLeafTransform::initHistos(){
  printf("JbLeafTransform::initHistos()\n");

  TH1F *h;
  //...... data histograms
  memset(hA,0,sizeof(hA));
  int nCase=4;
  hA[0]=h=new TH1F("myStat","Qweak: event count; case",nCase,0,nCase);
  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);
  h->SetLineColor(kBlue);h->SetLineWidth(2);
  h->SetMarkerSize(2);//<-- large text

  // char key[][200]={"inp","BHT3Id","L2wId"};
  // for(int i=0;i<4;i++) h->Fill(key[i],0.); // preset the order of keys

  hA[1]=h=new TH1F("inpBcm1","BCM1 yield  before cuts; current  #mu A ",128,0,0);
  h->SetFillColor(kBlue);
  h->SetBit(TH1::kCanRebin);


}


//________________________________________________
//________________________________________________
void
JbLeafTransform::finish(){

  printf("::::::::::::::::JbLeafTransform::finish(%s) :::::::::::\n",mCore.Data());
 


}

//========================
//========================
void JbLeafTransform::print() {

  printf("JbLeafTransform_%s print:\n  nP=%d  nY=%d\n",mCore.Data(),nP,nY);
#if 0
  for(int i=0;i<;i++) printf("%10s : P%d avr=%f   alpha=%.3f\n",Pname2[i],i,par_Pavr[i], par_Alpha[i]);
#endif
}

