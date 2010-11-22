/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Maps leafes form the root tree to variables in this code,
 *  according to external config file
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
  myName=core;
  cutName="none"; cutFormula="";
  afixName="afixMe3"; nLeafError=0;
  printf("constr of JbLeafTransform=%s\n",myName.Data());
}

//========================
//========================
Double_t * 
JbLeafTransform::getOneLeaf(TChain *chain, TString name,TString sub) {
  // name.ToLower(); will crash :  diff_qwk_bpm3h09Y
  if(sub.Sizeof()>1) name+="/"+sub;
  TLeaf *lf=chain->GetLeaf(name); 
  if(lf==0) printf("Failed leaf=%s= \n",name.Data());
  assert(lf);
  Double_t* p=(Double_t*)lf->GetValuePointer();    
  assert(p);
  return p;
}

//========================
//========================
void 
JbLeafTransform::findInputLeafs(TChain *chain){
  printf("JbLeafTransform_%s findInputLeafs\n",myName.Data());
  
  int k=0;
  // Access DV leafs
  for(int i=0;i<ndv();i++) {
    pLeafDV[i]=getOneLeaf(chain,dvName[i],afixName);  
    pYieldDV[i]=getOneLeaf(chain,setLeafName2Yield(dvName[i]),afixName);   
    pLeafError[k++]=getOneLeaf(chain,dvName[i],"Device_Error_Code");   
  }
  // Access IV leafs
  for(int i=0;i<niv();i++){
    pLeafIV[i]=getOneLeaf(chain,ivName[i],afixName);   
    pYieldIV[i]=getOneLeaf(chain,setLeafName2Yield(ivName[i]),afixName);   
    pLeafError[k++]=getOneLeaf(chain,ivName[i],"Device_Error_Code");   
  }

  // Access Aux leafs
  pLeafAux[0]=getOneLeaf(chain,"pattern_number");
  pLeafAux[1]=getOneLeaf(chain,"yield_qwk_bcm1",afixName);
  pLeafAux[2]=getOneLeaf(chain,"yield_qwk_bcm2",afixName); // not used for cuts
  pLeafError[k++]=getOneLeaf(chain,"yield_qwk_bcm1","Device_Error_Code");   
  pLeafError[k++]=getOneLeaf(chain,"yield_qwk_bcm2","Device_Error_Code");   

  assert(nLeafError==k);
  hA[3]=new TH1F("inpDevErr",Form("device error code  !=0;channels: DV[0-%d], IV[%d-%d], BCM1,2   ",ndv()-1,ndv(),ndv()+niv()-1),nLeafError,-0.5,nLeafError-0.5);
  hA[3]->SetFillColor(kGreen);

  //...... now you can shorten names so they are easier to read & print
  for(unsigned int i=0; i<dvName.size(); i++) 
    dvName[i]=humanizeLeafName(dvName[i]);
  for(unsigned int i=0; i<ivName.size(); i++) 
    ivName[i]=humanizeLeafName(ivName[i]);



}


//========================
//========================
void 
JbLeafTransform::presetMyStat(double x1,double x2, double thr, double x3){
  hA[0]->Fill("inpTree", x1);
  hA[0]->Fill("customCut", x2);
  hA[0]->Fill(Form("bcm >%.0f#muA",thr), x3);
  //  thr=50;
  TList *Lx=hA[1]->GetListOfFunctions();
  TLine *ln=new TLine(thr,0,thr,5.e3);
  ln->SetLineColor(kRed); ln->SetLineWidth(2);
  Lx->Add(ln);
  
}

//========================
//========================
bool 
JbLeafTransform::unpackEvent(){
  hA[0]->Fill("inp", 1.);

  // test for NaN as data are accessed 

  // printf("\n-----------JbLeafTransform_%s unpackEvent\n",myName.Data());
 
  // Access DV leafs
  for(int i=0;i<ndv();i++){
    //    printf("  dv=%d p=%p\n",i,pLeafDV[i]);
    Yvec[i]=1e6*(*pLeafDV[i]);    
    // printf("  dv=%d val=%g\n",i,Yvec[i]);
    if(Yvec[i]!=Yvec[i])  return false; // corrupted event w/ NaN
  }

  // Access IV leafs
  for(int i=0;i<niv();i++){
    // printf("  dv=%d p=%p\n",i,pLeafDV[i]);
    Pvec[i]= 1e6*(*pLeafIV[i]);    
    // printf("  iv=%d val=%g\n",i,Pvec[i]);
    if(Yvec[i]!=Yvec[i])  return false; // corrupted event w/ NaN
  }

  // access Auxiliary values
  for(int i=0;i<mxAux;i++) {
    double val=*pLeafAux[i];
    //printf("  aux=%d val=%g\n",i,val);
    if(val!=val)  return false; // corrupted event w/ NaN
  }

  hA[0]->Fill("noNaN", 1.);

  int eveBad=false;
  for(int i=0; i<nLeafError;i++){
    if((*pLeafError[i])==0.) continue;
    // printf("xx i=%d\n",i);
    eveBad=true;
    hA[3]->Fill(i);
  }
  if(eveBad) return false;

  hA[0]->Fill("noDevErr", 1.);

  double pattNo=(*pLeafAux[0]);
  double bcm1_uA=(*pLeafAux[1]);
  hA[1]->Fill(bcm1_uA);
  hA[2]->Fill(pattNo,bcm1_uA);
  
  // fill yield histso
  for(int i=0;i<ndv();i++)
    hydv[i]->Fill(*pYieldDV[i]);
  
  for(int i=0;i<niv();i++)
    hyiv[i]->Fill(*pYieldIV[i]);
  

  /* arrays w/ final 15 dv & iv variables
     1-bar tube combos:    MD 1-8 (all bars)
     2-bar combos:             MD H (3+7), V (1+5), D1 (2+6), D2(4+8)
     4-bar combos:             MD C (1+3+5+7), MD X (2+4+6+8)
     all:	                               MD ALL (1+2+3+4+5+6+7+8)

     named: md1,...,8, h,v,d1,d2,c,x,all
  */


  return true;  
}


//========================
//========================
void 
JbLeafTransform::init() {
  initHistos(); 
}


//________________________________________________
//________________________________________________
void
JbLeafTransform::initHistos(){
  printf("JbLeafTransform::initHistos()\n");

  TH1 *h;
  //...... data histograms
  memset(hA,0,sizeof(hA));
  int nCase=6;
  hA[0]=h=new TH1F("myStat","blueRegression: event count",nCase,0,nCase);
  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.06);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);
  h->SetLineColor(kBlue);h->SetLineWidth(2);
  h->SetMarkerSize(2);//<-- large text

  // char key[][200]={"inp","BHT3Id","L2wId"};
  // for(int i=0;i<4;i++) h->Fill(key[i],0.); // preset the order of keys

  hA[1]=h=new TH1F("inpBcm1","BCM1 yield after cuts; current  #mu A ",128,0.,0.);
  h->SetFillColor(kBlue);
  h->SetBit(TH1::kCanRebin);

  hA[2]=h=new TProfile("inpPattNo","Current stability; Pattern_number; average BCM1 (#mu A)    ",200,0.,0.);
  h->SetFillColor(kYellow);  h->SetBit(TH1::kCanRebin); // rescalled in finish


  // hA[3] is set in findInputLeafs()

  
  //.....  yield  dv
  hydv=new TH1 *[ndv()];
  for(int i=0;i<ndv();i++) {   
    TString name=humanizeLeafName(setLeafName2Yield(dvName[i])) ;
    hydv[i]=h=new TH1D(Form("yieldDV%d",i),Form("%s, DV%d ;%s",name.Data(),i,name.Data()),128,-0.,0.);
    h->GetXaxis()->SetNdivisions(4);  h->SetBit(TH1::kCanRebin);
    h->GetXaxis()->SetTitleColor(kBlue);
  }

  //.....  yield  iv
  hyiv=new TH1 *[niv()];
  for(int i=0;i<niv();i++) {   
    TString name=humanizeLeafName(setLeafName2Yield(ivName[i])) ;
    hyiv[i]=h=new TH1D(Form("yieldIV%d",i),Form("%s, IV%d ;%s",name.Data(),i,name.Data()),128,0.,0.);
    h->GetXaxis()->SetNdivisions(4);    h->SetBit(TH1::kCanRebin);
    h->GetXaxis()->SetTitleColor(kBlue);
  }



}


//________________________________________________
//________________________________________________
void
JbLeafTransform::finish(){
  printf("::::::::::::::::JbLeafTransform::finish(%s) :::::::::::\n",myName.Data());


}

//========================
//========================
void 
JbLeafTransform::print() {

  printf("JbLeafTransform: myName='%s' , inpTree='%s'  regVarName=%s, len: dv=%d iv=%d  nLeafErr=%d, afix=%s\nDV: ",myName.Data(),inpTree.Data(),regVarName.Data(),(int)dvName.size(),(int)ivName.size(), nLeafError,afixName.Data());
  printf("  custom cut: name=%s  formula='%s'\n",cutName.Data(), cutFormula.Data());
  for(unsigned int i=0; i<dvName.size(); i++) printf("%s, ",dvName[i].Data());
  printf("\nIV: ");
  for(unsigned int i=0; i<ivName.size(); i++) printf("%s, ",ivName[i].Data());
  printf("\nCorrFac: print-end\n");
}

//========================
//========================
void 
JbLeafTransform::readConfig(const char * configFName){
  
  FILE *fp=fopen(configFName,"r");
  assert(fp);
  int ret=harvestNames(fp);
  assert(ret==2);
  fclose(fp);

  assert(ndv()>0);
  assert(niv()>0);

  // allocate memory for leaves w/ data
  pLeafDV= new Double_t * [ndv()];
  Yvec   = new Double_t   [ndv()];
  pLeafIV= new Double_t * [niv()];
  Pvec   = new Double_t   [niv()];

  // leavs w/ yields, local only
  pYieldDV= new Double_t * [ndv()];
  pYieldIV= new Double_t * [niv()];

  // leavs w/ error code
  nLeafError=niv()+ndv()+2;// '2' is for bcm1, bcm2
  pLeafError= new Double_t * [nLeafError]; 

}

//========================
//========================
int 
JbLeafTransform::harvestNames(FILE *fp) {
  assert(fp);
  enum {mx=1000};
  char buf[mx];
  int nl=0;
  int state=0; 
  while(  fgets( buf, mx, fp)) {
    nl++;
    if (strstr(buf,"#")>0) continue;
    char *x=strstr(buf, "\n"); if(x)*x=0;
    //printf("line=%d  state=%d buf=%s=\n",nl,state,buf);
    if(state==0) { // before  block of IV-DV is found
      if (strstr(buf, "customcut")){
	char name[1000], formula[10000];
	int ret=sscanf(buf+9,"%s %s",name,formula);
	printf("CUSTOM CUT line: ret=%d  name=%s= formula=%s=\n",ret,name,formula);
	cutName=name;
	cutFormula=formula;
      }
      if (strstr(buf, "regvars")==0) continue;// first search for new block
      state=1;
      regVarName=buf+8;
      continue;
    } 
    if(state==1){ // increment iv & dv name lists
      if (strstr(buf, "afix")) afixName=buf+5;
      if (strstr(buf, "iv")) ivName.push_back(buf+3);
      if (strstr(buf, "dv")) dvName.push_back(buf+3);
      if (strstr(buf, "treetype")==0) continue;
      // end this DV-IV block
      state=2;
      inpTree=buf+9;
      break;
    }
  }
  return state;
}



//=====================
TString 
JbLeafTransform::humanizeLeafName(TString longName) {
  TString name=longName;
  name.ReplaceAll("_qwk","");
  name.ReplaceAll("_md","_MD");
  name.ReplaceAll("barsum","");
  name.ReplaceAll("bars","");
  printf("Humanize '%s' --> '%s'\n", longName.Data(),name.Data());
  return name;
}

//=====================
TString 
JbLeafTransform::setLeafName2Yield(TString longName){
  TString name=longName;
  name.ReplaceAll("asym_","yield_");
  name.ReplaceAll("diff_","yield_");
  //printf("leaf2yield: '%s' \n",name.Data());
  return name;
}
