#include <assert.h>
#include <cstdio>
#include <iostream>
#include <TTree.h>

#include "QwkRegBlueTree.h"

//========================
//========================
QwkRegBlueTree::QwkRegBlueTree(std::vector < TString > dvName) {
  mxY=dvName.size();
  printf("constr of QwkRegBlueTree mxY=%d\n",mxY);
  regDvValue=new Double_t [mxY];
  mTree = new TTree("reg","Qweak regressed DVs");
  mTree->Branch("pattern_number",&pattern_number,"pattern_number/D");
  mTree->Branch("regGlobErrorCode",&regGlobErrorCode,"regGlobErrorCode/I");

  for(int ip=0;ip<mxY;ip++) {
    TString name="reg_"+dvName[ip];
    mTree->Branch(name,regDvValue+ip,name+"/D");
    //printf("add %d xx=%s=\n",ip,name.Data());
  };
};

//========================
//========================
void
QwkRegBlueTree::FillEmpty(double patternNo) {
  pattern_number=patternNo;
  regGlobErrorCode=-1;
  for(int ip=0;ip<mxY;ip++)regDvValue[ip]=-1;
  mTree->Fill(); //..... save event in to tree
}

//========================
//========================
void
QwkRegBlueTree::Fill(double patternNo, double *Y) {
  pattern_number=patternNo;
  regGlobErrorCode=0;
  for(int ip=0;ip<mxY;ip++)regDvValue[ip]=Y[ip]/1.e6;
  mTree->Fill(); //..... save event in to tree
}


//________________________________________________
//________________________________________________
void
QwkRegBlueTree::finish(){
  printf("::::::::::::::::QwkRegBlueTree::finish() :::::::::::\n");

  mTree->Print();
  mTree->Show(7777);// event id startting from 0

}
