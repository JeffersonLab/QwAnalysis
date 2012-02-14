/*************************************************
 * Courtesy: Don Jones
 * modified by: Amrendra
 ************************************************ */
#include <rootClass.h>
#include "comptonRunConstants.h"

/////////////////////////////////////////////////////////////////////////////
//*** This function cycles through the laser power entries, creating an ***//
//*** array of values for starting and ending points of the laser off   ***//
//*** periods. Even indices 0,2,4... are beginning points and odd       ***//
//*** indices 1,3,5... are endpoints. Laser off entries are those that  ***//
//*** have a value of 1 or less.The function returns the number of      ***//
//*** entries in the array. It also records electron beam off periods   ***//
//*** in a similar fashion. It requires the Mps_Tree to be used.        ***//
///////////////////////////////////////////////////////////////////////////// 

Int_t getEBeamLasCuts(std::vector<Int_t> &cutL, std::vector<Int_t> &cutE, TChain *chain, Int_t runnum)
{
  chain->ResetBranchAddresses();
  Char_t textf[255],textwrite[255];
  Int_t nEntries = chain->GetEntries();
  Double_t laser = 0, bcm = 0 , comptQ = 0;
  Double_t beamMax, laserMax;

  TH1D *hBeam = new TH1D("hBeam","dummy",100,0,220);//typical value of maximum beam current
  TH1D *hLaser = new TH1D("hLaser","dummy",1000,0,180000);//typical value of maximum laser power

  chain->Draw("sca_bcm6.value>>hBeam","","goff");
  hBeam = (TH1D*)gDirectory->Get("hBeam");  
  beamMax = hBeam->GetBinLowEdge(hBeam->FindLastBinAbove(100));//to avoid extraneous values
  cout<<"beamMax(bcm6) "<<beamMax<<endl;

  chain->Draw("sca_laser_PowT.value>>hLaser","","goff");
  hLaser = (TH1D*)gDirectory->Get("hLaser");  
  laserMax = hLaser->GetBinLowEdge(hLaser->FindLastBinAbove(100));//to avoid extraneous values
  cout<<"laserMax = "<<laserMax<<endl;

  Int_t nLasCycBeamTrips;
  Int_t n = 0, m = 0, o = 0, p = 0, q = 0;
  Bool_t flipperIsUp = kFALSE, isABeamTrip = kFALSE;
  Bool_t rampIsDone = kTRUE, prevTripDone = kTRUE;
  sprintf(textf,"analOut/r%d_cutLas.txt",runnum);
  ofstream outfileLas(Form("%s",textf));
  printf("%s file created\n",textf);

  sprintf(textf,"analOut/r%d_cutBeam.txt",runnum);
  ofstream outfileBeam(Form("%s",textf));
  printf("%s file created\n",textf);

  TBranch *bLaser;
  TBranch *bBCM;
  TBranch *bCharge;
  TLeaf *lLaser;
  TLeaf *lBCM;
  TLeaf *lCharge;
  chain->SetBranchStatus("*",0);//turn off all branches
  chain->SetBranchStatus("sca_laser_PowT",1);//turn on cavity pow branch
  chain->SetBranchStatus("sca_bcm6",1);//turn on bcm branch
  chain->SetBranchStatus("compton_charge",1);//turn on charge branch
  chain->SetAutoDelete(kTRUE);
  printf("Ebeam considered On if above %f.\n", beamFrac*beamMax);

  for(Int_t index=0; index<nEntries;index++) {
    chain->GetEntry(index);
    bLaser = bBCM = bCharge = 0;
    lLaser = lBCM = lCharge = 0;
    bLaser = (TBranch*)chain->GetBranch("sca_laser_PowT");
    bBCM = (TBranch*)chain->GetBranch("sca_bcm6");
    bCharge = (TBranch*)chain->GetBranch("compton_charge");
    lLaser = (TLeaf*)bLaser->GetLeaf("value");
    lBCM = (TLeaf*)bBCM->GetLeaf("value");
    lCharge = (TLeaf*)bCharge->GetLeaf("value");
    laser = lLaser->GetValue();
    bcm = lBCM->GetValue();
    comptQ = lCharge->GetValue();

    ////find laser off periods and record start and finish entries
    if (n==minEntries) {
      cutL.push_back(index-minEntries);
      //printf("cutL[%d]=%d\n",m,cutL.back());///print begin of laser off entry
      flipperIsUp = kTRUE;
      m++;
    }
    if(laser<=laserFrac*laserMax) n++;
    else n=0;

    if(flipperIsUp){
      if(n == 0 || index == nEntries-1 ) {
        cutL.push_back(index-1);
	//printf("cutL[%d]=%d\n",m,cutL.back());///print end of laser off entry
        m++;
        flipperIsUp = kFALSE;
      }
    }
    //    find and record electron beam off periods
    rampIsDone = (bcm> (beamFrac*beamMax));
    isABeamTrip = (bcm<= (beamFrac*beamMax));

    if(isABeamTrip && prevTripDone) {
      //to make sure it is a beam trip not a problem with acquisition
      //insist > 10 in a row meet the isABeamTrip criterion
      q++;
      if(q>10) {
        q = 0;
        o++;
        cutE.push_back(index);
        prevTripDone = kFALSE;
      }
    }

    if(!prevTripDone && rampIsDone){
      p++;
      //wait a few seconds for beam to stabilize then record the final entry
      if(p>=WAIT_N_ENTRIES){
        o++;
        p = 0;
        cutE.push_back(index);
        prevTripDone = kTRUE;
      }
    }
  }

  //if the run ends with a beam trip, record last entry in cutE vector
  if(o%2!=0){
    cutE.push_back(nEntries - 1);
    o++;
  }
  //print the beam trip cuts

  for(Int_t i=0;i<o;i++) {
    //printf("cutE[%i]=%i\n",i,cutE.at(i));
    sprintf(textwrite,"%d",cutE.at(i));
    outfileBeam <<textwrite<<endl;
  }

  for(Int_t i=0;i<m;i++) {
    sprintf(textwrite,"%d",cutL.at(i));
    outfileLas <<textwrite<<endl;
  }

//   chain->SetBranchStatus("*",1);//turn on all branches again
  nLasCycBeamTrips = o*500+m/2;
  return o*500+m/2;//nEntries for both arrays is encoded into return value
}

/* !A couple of inconsistenties exit as of now:
.. 1. the beam trip evaluation is neither what is claimed nor what it should be
.. 2. 
..*/
