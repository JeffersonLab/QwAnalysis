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

#include <TProfile.h> 
#include <TList.h>
#include <TLine.h>
#include <TFile.h> 
#include <TTree.h> 
#include <TChain.h> 
#include <TH2.h> 


#include <math.h>
#include <TMath.h>

#include "QwkRegBlueTransform.h"

//========================
//========================
QwkRegBlueTransform::QwkRegBlueTransform(const char *core) {
  myName=core;
  cutName="none"; cutFormula="";
  inpPath="fixPath-in-config-file";
  printf("constr of QwkRegBlueTransform=%s\n",myName.Data());
}


//========================
//========================
void 
QwkRegBlueTransform::getOneBranch(TChain *chain,TString name,QwkChanJan *qwkChan){

  // try first if it is not aliased name
  //printf(" getOneBranch chan=%s=  nals=%d...\n",name.Data(),nals());

  for(int k=0; k<nals(); k++) {
    //printf(" zz %s k=%d %s  %d\n",name.Data(),k,alsName[k].Data(),alsName[k].Contains(name+"="));
    if (!alsName[k].Contains(name+"=")) continue;
    printf("  chan=%s= is computed by alias k=%d\n",name.Data(),k);
    return ;
  }

  int ierr=chain->SetBranchAddress(name,qwkChan);
  if(ierr) printf(" failed to map chan=%s=\n",name.Data());
  assert(ierr==0);  
  //printf("  mapped chan: %s to Tree\n",name.Data());
}


//========================
//========================
void
QwkRegBlueTransform::findInputLeafs(TChain *chain){
  printf("QwkRegBlueTransform_%s findInputLeafs\n",myName.Data());
  

  int ierr=chain->SetBranchAddress("pattern_number",&pattern_number);
  assert(ierr==0);  

  for(int i=0;i<nchan();i++){
    //printf("findInputLeafs: ich=%d of %d name=%s=\n",i,nchan(),chanName[i].Data());
    getOneBranch(chain,chanName[i],&qwkChan[i]);
  }
 
 
  hA[3]=new TH1F("inpDevErr",Form("device error code  !=0;channels: see log file "),nchan(),-0.5,nchan()-0.5);
  hA[3]->SetFillColor(kGreen);



  //...... print variables in order
  printf("\n---Aliases N=%d:\n",nals());
  for(int i=0; i<nals(); i++) {
    printf("ial=%d : %s\n",i,alsName[i].Data());
    aliasVec[i].print();
  }

  printf("\n---DVs n=%d:\n",ndv());
  for(int i=0; i<ndv(); i++) {
    printf("dv=%d : %s (ich=%d),    ",i,dvName[i].Data(),dv2chan[i]);
    if(i%2) printf("\n");
  }

  printf("\n---IVs n=%d:\n",niv()); 
  for(int i=0; i<niv(); i++) {
    printf("iv=%d : %s (ich=%d),    ",i,ivName[i].Data(),iv2chan[i]);
    if(i%2) printf("\n");
  }

  printf("\n\n---all channels n=%d:\n",nchan());
  for(int i=0; i<nchan(); i++) {
    printf("ich=%d : %s,    ",i,chanName[i].Data());
    if(i%2) printf("\n");
  }
  printf("\n\n");
 
}


//========================
//========================
void 
QwkRegBlueTransform::presetMyStat(double x1,double x2, double thr, double x3){
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
QwkRegBlueTransform::unpackEvent(){
  hA[0]->Fill("inp", 1.);
  //printf("\n-----------QwkRegBlueTransform_%s unpackEvent\n",myName.Data());

  //  compute aliases
  for(int i=0;i<nals();i++) {
    aliasVec[i].compute_hw_sum(qwkChan);
  }

  // check dev-error on all leaves
  bool eveBad=false;
  for(int i=0;i<nchan();i++){
    int devErr=static_cast<int> (qwkChan[i].Device_Error_Code);
    //if(i==46 && devErr) printf("ich=%d %s  val=%f  devErr=%d  eve=%.0f\n",i,chanName[i].Data(),qwkChan[i].hw_sum,devErr, hA[0]->GetBinContent(4));
    if(devErr==0) continue;
    eveBad=true;
    hA[3]->Fill(i);

    ((TH2F*)hA[4])->Fill(Form("0x%x",devErr),chanName[i],1.);
    
  }

  if(eveBad) return false;
  hA[0]->Fill("noDevErr", 1.);

  
  // Access DV leafs
  for(int i=0;i<ndv();i++){
    int k=dv2chan[i];
    Yvec[i]=1e6*qwkChan[k].hw_sum;
    //if(i==35)
    //printf("  dv=%d val=%f\n",i,Yvec[i]);
  }

  // Access IV leafs
  for(int i=0;i<niv();i++){
    int k=iv2chan[i];
    Pvec[i]= 1e6*qwkChan[k].hw_sum;
    //printf("  iv=%d val=%g\n",i,Pvec[i]);
  }
 
  double bcm1_uA=qwkChan[0].hw_sum;
  hA[1]->Fill(bcm1_uA);
  hA[2]->Fill( pattern(),bcm1_uA);
  
  
  return true;  
}


//========================
//========================
void 
QwkRegBlueTransform::init() {
  initHistos(); 
}


//________________________________________________
//________________________________________________
void
QwkRegBlueTransform::initHistos(){
  printf("QwkRegBlueTransform::initHistos()\n");

  TH1 *h;
  //...... data histograms
  memset(hA,0,sizeof(hA));
  int nCase=5;
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

  hA[4]=h=new TH2F("devErrType","No.of patterns w/ device error code (independent incrementation); DeviceError value (hex)",1,0,0,nchan(),-0.5,nchan()-0.5);
  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.04);  h->GetXaxis()->SetTitleSize(0.05); h->SetMinimum(0.8);
  h->SetMarkerSize(2);//<-- large text

  for(int i=0;i<ndv();i++)
    ((TH2F*)hA[4])->Fill(0.,dvName[i],0.);

  for(int i=0;i<niv();i++)
    ((TH2F*)hA[4])->Fill(0.,ivName[i],0.);

  // 5...7 free
  
  assert(chanName[0].CompareTo("yield_qwk_bcm1")==0); // needed by hA[1,2]
}


//________________________________________________
//________________________________________________
void
QwkRegBlueTransform::finish(){
  printf("::::::::::::::::QwkRegBlueTransform::finish(%s) :::::::::::\n",myName.Data());

  int highBin=hA[3]->GetMaximumBin();

  double cnt=hA[3]->GetBinContent(highBin);
  TString name=chanName[highBin-1];

  double frac=-1;
  double nBcmCut=hA[0]->GetBinContent(4);
  if(nBcmCut>1) frac=cnt/nBcmCut;
  printf("#highest error: bin=%d  name=%s  count=%.0f of %.0f  frac=%.2f\n",highBin,name.Data(),cnt,nBcmCut,frac);

}

//========================
//========================
void 
QwkRegBlueTransform::print() {

  printf("QwkRegBlueTransform: myName='%s' , inpTree='%s'  regVarName=%s, len: dv=%d iv=%d  nChanCount=%d\nDV: ",myName.Data(),inpTree.Data(),regVarName.Data(),(int)dvName.size(),(int)ivName.size(), nchan());
  printf("  custom cut: name=%s  formula='%s'\n",cutName.Data(), cutFormula.Data());
  return;
}
//========================
//========================
int
QwkRegBlueTransform::findAddChan(TString name){
  for(int k=0; k<nchan(); k++) {
    if (chanName[k].CompareTo(name)!=0) continue;
    return k;
  }

  // name was not found, add it
  chanName.push_back(name);
  return nchan()-1;
  
}

//========================
//========================
void 
QwkRegBlueTransform::readConfig(const char * configFName){
  

  findAddChan("yield_qwk_bcm1"); // add more hardcoded leaves here

  FILE *fp=fopen(configFName,"r");
  printf(" reads in =%s=\n",configFName);
  assert(fp);
  int ret=harvestNames(fp);
  assert(ret==2);
  fclose(fp);

  assert(ndv()>0);
  assert(niv()>0);

  // inclusive add aliases
  for(int i=0;i<nals();i++){
    TString x=alsName[i];
    //printf("parse alias: %s\n",x.Data());
    QwkRegBlueAlias als(x);
    int kout=findAddChan(als.outName);
    int kin1=findAddChan(als.inpName1);
    int kin2=findAddChan(als.inpName2);
    als.mappChannels(kout, kin1, kin2);
    aliasVec.push_back(als);
    als.print();
  }

  //  inclusive add DVs
  for(int i=0;i<ndv();i++){
    int k=findAddChan(dvName[i]);
    dv2chan.push_back(k);
  }

  
  //  inclusive add IVs
  for(int i=0;i<niv();i++){
    int k=findAddChan(ivName[i]);
    iv2chan.push_back(k);
  }

  qwkChan=new QwkChanJan [nchan()];
  // allocate memory for exportable data
  Yvec   = new Double_t   [ndv()];
  Pvec   = new Double_t   [niv()];
}

//========================
//========================
int 
QwkRegBlueTransform::harvestNames(FILE *fp) {
  assert(fp);
  enum {mx=1000};
  char buf[mx];
  int nl=0;
  int state=0;
  char name[1000];

  while(  fgets( buf, mx, fp)) {
    nl++;
    if (strstr(buf,"#")>0) continue;
    char *x=strstr(buf, "\n"); if(x)*x=0;
    //printf("line=%d  state=%d buf=%s=\n",nl,state,buf);
    if(state==0) { // before  block of IV-DV is found
      if (strstr(buf, "customcut")){
	char  formula[10000];
	int ret=sscanf(buf+9,"%s %s",name,formula);
	printf("CUSTOM CUT line: ret=%d  name=%s= formula=%s=\n",ret,name,formula);
	cutName=name;
	cutFormula=formula;
	assert(ret==2);
	continue;
      }
      if (strstr(buf, "inpPath")){
	int ret=sscanf(buf+7,"%s ",name); 
	inpPath=name;
	printf("input tree path=%s\n",inpPath.Data());
	assert(ret==1);
	continue;
      }
      if (strstr(buf, "alias")){
	int ret=sscanf(buf+6,"%s ",name);
	alsName.push_back(name);
	printf("define %d alias => %s\n",nals(),name);
	assert(ret==1);
	continue;
      }
      if (strstr(buf, "regvars")==0) continue;// first search for new block
      state=1;
      regVarName=buf+8;
      continue;
    } 
    if(state==1){ // increment iv & dv name lists
      int ret=sscanf(buf+3,"%s ",name); assert(ret==1);
      if (strstr(buf, "iv")) ivName.push_back(name);
      if (strstr(buf, "dv")) dvName.push_back(name);
      if (strstr(buf, "treetype")==0) continue;
      // end this DV-IV block
      state=2;
      inpTree=buf+9;
      break;
    }
  }
  //printf("harvestNames stat=%d\n",state);
  return state;
}


// not used .....
//=====================
TString 
QwkRegBlueTransform::setLeafName2Yield(TString longName){
  TString name=longName;
  name.ReplaceAll("asym_","yield_");
  name.ReplaceAll("diff_","yield_");
  //printf("  leaf2yield: '%s'-> '%s' \n\n",longName.Data(),name.Data());
  return name;
  assert(2==3);
}
