#include<vector>
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"

/////////////////////////////////////////////////////////////////////////////
//*** This function cycles through the laser power entries, creating an ***//
//*** array of values for starting and ending points of the laser off   ***//
//*** periods. Even indices 0,2,4... are beginning points and odd       ***//
//*** indices 1,3,5... are endpoints. Laser off entries are those that  ***//
//*** have a value of 1 or less.The function returns the number of      ***//
//*** entries in the array.                                             ***//
///////////////////////////////////////////////////////////////////////////// 

Int_t cutOnLaser(std::vector<Int_t> &cut1, TTree *tree){
  //FILE *file = fopen("f.txt","w");
  Double_t myvar;
  const Double_t LOWLIMIT = 20.0;
  Int_t n = 0, m = 0, numEntries = 0;
  Bool_t flag = kFALSE;
  Int_t minEntries = 3000; //Laser must be off for at least this many
                           //consecutive entries to be considered off
  tree->SetBranchStatus("*",0);//turn off all branches
  tree->SetBranchStatus("yield_sca_laser_PowT",1);//turn on only this branch
  tree->SetBranchAddress("yield_sca_laser_PowT",&myvar);
  numEntries = tree->GetEntries();
  for(Int_t i=0; i<numEntries;i++){
    tree->GetEntry(i);
    //if(i%60==0)fprintf(file,"%d,\n",myvar);
    //else fprintf(file,"%d ",myvar);
    if(n==minEntries){
      cut1.push_back(i-minEntries);
      printf("cut[%d]=%d\n",m,cut1.back());
      flag = kTRUE;
      m++;
    }
    if(myvar<=LOWLIMIT)n++;
    else n=0;
    if(flag){
      if(n == 0 || i == numEntries-1 ){
        cut1.push_back(i-1);
	printf("cut[%d]=%d\n",m,cut1.back());
        m++;
        flag = kFALSE;
      }
    }
  }
  tree->SetBranchStatus("*",1);//turn on all branches again
  return m;
}

///////////////////////////////////////////////
//Same as above except takes TChain as input.//
///////////////////////////////////////////////
Int_t cutOnLaser(std::vector<Int_t> &cut1, TChain *chain){
  //FILE *file = fopen("f.txt","w");
  const Double_t LOWLIMIT = 20.0;
  Double_t myvar;
  Int_t n = 0, m = 0, numEntries = 0;
  Bool_t flag = kFALSE;
  Int_t minEntries = 3000; //Laser must be off for at least this many
                           //consecutive entries to be considered off.
  chain->SetBranchStatus("*",0);//turn off all branches
  chain->SetBranchStatus("yield_sca_laser_PowT",1);//turn on only this branch
  chain->SetBranchAddress("yield_sca_laser_PowT",&myvar);
  numEntries = chain->GetEntries();
  printf("Chain has %d entries.\n",numEntries);
  for(Int_t i=0; i<numEntries;i++){
    chain->GetEntry(i);
    //if(i%60==0)fprintf(file,"%d,\n",myvar);
    //else fprintf(file,"%d ",myvar);
    if(n==minEntries){
      cut1.push_back(i-minEntries);
      printf("cut[%d]=%d\n",m,cut1.back());
      flag = kTRUE;
      m++;
    }
    if(myvar<=LOWLIMIT)n++;
    else n=0;
    if(flag){
      if(n == 0 || i == numEntries-1 ){
        cut1.push_back(i-1);
	printf("cut[%d]=%d\n",m,cut1.back());
        m++;
        flag = kFALSE;
      }
    }
  }
  chain->SetBranchStatus("*",1);//turn on all branches again
  return m;
}

