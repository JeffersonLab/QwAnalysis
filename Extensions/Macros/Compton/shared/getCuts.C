#include <cstdlib>
#include <vector>
#include <iostream>
#include "TTree.h"
#include "TH1F.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TChain.h"
#include "TMath.h"
#include "TROOT.h"
#include "TCint.h"
#include "TSystem.h"

/////////////////////////////////////////////////////////////////////////////
//*** This function cycles through the laser power entries, creating an ***//
//*** array of values for starting and ending points of the laser off   ***//
//*** periods. Even indices 0,2,4... are beginning points and odd       ***//
//*** indices 1,3,5... are endpoints. Laser off entries are those that  ***//
//*** have a value of 1 or less.The function returns the number of      ***//
//*** entries in the array. It also records electron beam off periods   ***//
//*** in a similar fashion.                                             ***//
//*** Files are analyzed runlet by runlet.                              ***//
///////////////////////////////////////////////////////////////////////////// 



Int_t getCuts(std::vector<Int_t> &cutL, std::vector<Int_t> &cutE, 
	      Int_t runNum, Double_t lasLlimit = 5000.0)
{
  const Int_t WAIT_N_ENTRIES = 2000;//# of quartets to wait after beam trip
  Bool_t flipperIsUp = kFALSE, isABeamTrip = kFALSE;
  Bool_t rampIsDone = kTRUE, prevTripDone = kTRUE;
  Int_t numFiles, nEntTotal;//number of runlets and net entries
  Int_t n = 0, m = 0, o = 0, p = 0, q = 0, nEntries = 0;
  Int_t z = 0, fileNum = 0;
  Int_t minEntries = 2000; //Laser must be off for at least this many
  Double_t las, bcm, ent = 0;



  //Get number of runlets and total entries then 
  //reset chain and attach one runlet at a time
  TChain *ch = new TChain("Hel_Tree");
  TString baseDir = TString("/net/cdaq/cdaql8data/compton/rootfiles/");
  TString fileName = baseDir + Form("Compton_Pass1_%d.???.root", runNum);
  numFiles = ch->Add(fileName);
  nEntTotal = ch->GetEntries(); 
  std::cout<<numFiles<<" runlets in run "<<runNum<<" with a total of "<<nEntTotal<<" entries."<<std::endl;

 
  //Get maximum current
  ch->Draw("yield_sca_bcm6>>h(100,0,200)","","goff");
  TH1F *h = (TH1F*)gDirectory->Get("h");
  Double_t beamMax = h->GetBinLowEdge(h->FindLastBinAbove(100));
  Double_t beamOnVal = 0.9 * beamMax;
  printf("Ebeam considered On if above %f.\n", beamOnVal);
  delete h;


  while(fileNum<numFiles){

    ch->Reset();
    fileName = baseDir + Form("Compton_Pass1_%d.%03d.root", runNum, fileNum);
    ch->Add(fileName);

    TBranch *bBCM = (TBranch*)ch->GetBranch("yield_sca_bcm6");
    TBranch *bLas = (TBranch*)ch->GetBranch("yield_sca_laser_PowT");
    TLeaf *lBCM = (TLeaf*)bBCM->GetLeaf("value");
    TLeaf *lLas = (TLeaf*)bLas->GetLeaf("value");

    nEntries = bBCM->GetEntries();
    ent += nEntries;
    std::cout<<"Runlet "<<fileNum<<" attached to chain with "<<nEntries<<" entries."<<std::endl;
    std::cout<<"Entry = "<<z<<std::endl;     


    for(Int_t i=0; i<nEntries;i++){
      bBCM->GetEntry(i);
      bLas->GetEntry(i);
      bcm = lBCM->GetValue();
      las = lLas->GetValue();

      //find laser off periods and record start and finish entries
      if(n==minEntries){
	cutL.push_back(z-minEntries);
	printf("cutL[%d]=%d\n",m,cutL.back());
	flipperIsUp = kTRUE;
	m++;
      }


      //if laser ever goes above set limit for off restart counter
      if(las<=lasLlimit)n++;
      else n=0;
      if(flipperIsUp){
	if(n == 0 || i == (nEntries-1) ){
	  cutL.push_back(z-1);

	  printf("cutL[%d]=%d\n",m,cutL.back());
	  m++;
	  flipperIsUp = kFALSE;
	}
      }


 
      rampIsDone = bcm > beamOnVal;//beam above 95% maximum
      if(i<1000) isABeamTrip = bcm < beamOnVal;//allow for runs that start during ramp
      else  isABeamTrip = bcm <= 0.1 * beamMax;


      //find and record electron beam off periods
      if(isABeamTrip && prevTripDone){
        //to make sure it is a beam trip not a problem with acquisition
	//insist > 10 in a row meet the isABeamTrip criterion
	q++;
	if(q>10){
	  q = 0;
	  o++;
	  cutE.push_back(TMath::Max((z-15), 0));
	  prevTripDone = kFALSE;
	}
      }



      if(!prevTripDone && rampIsDone){
	p++;
	//wait a few seconds for beam to stabilize then record the final entry
	if(p>=WAIT_N_ENTRIES){
	  p = 0;
	  TH1F *h = new TH1F("h", "h", 300, -10, 200);
	  for(Int_t a=i; a<i+WAIT_N_ENTRIES; a++)
	    {
	      bBCM->GetEntry(a);
	      bcm = lBCM->GetValue();
	      h->Fill(bcm);
	    }
	  if(h->GetRMS()<5.0)//trip is over only if beam has stabilized
	    {
	      o++;
	      cutE.push_back(z);
	      prevTripDone = kTRUE;
	    }
	  else prevTripDone = kFALSE;

	  delete h;
	}
      }
      z++;
    }
    fileNum++;
  }

  //if the run ends with a beam trip, record last entry in cutE vector
  if(o%2!=0){
    cutE.push_back(nEntTotal - 1);
    o++;
  }
  //print the beam trip cuts
  
  for(Int_t i=0;i<o;i++){
    printf("cutE[%i]=%i\n",i,cutE.at(i));
  }
  

  return o*500+(m-1)/2;//nEntries for both arrays is encoded into return value
}


