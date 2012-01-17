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

Int_t getEBeamLasCuts(std::vector<Int_t> &cutL, std::vector<Int_t> &cutE, TChain *chain)
{
  chain->ResetBranchAddresses();
  //  Char_t textf[255],textwrite[255];
  Int_t nEntries = chain->GetEntries();
  Double_t laser = 0, bcm = 0 , comptQ = 0;
  Double_t beamMax = chain->GetMaximum("sca_bcm6/value");
  Double_t Qmax = chain->GetMaximum("compton_charge/value");
  Int_t nLasCycBeamTrips;
  Int_t n = 0, m = 0, o = 0, p = 0, q = 0;
  Bool_t flipperIsUp = kFALSE, isABeamTrip = kFALSE;
  Bool_t rampIsDone = kTRUE, prevTripDone = kTRUE;
  Int_t minEntries = 2000; //Laser must be off for at least this many
  //consecutive entries to be considered off.
  //   sprintf(textf,"%d_output.txt",runnum);
  //   ofstream outfile(Form("%s",textf));
  //   printf("%s file created\n",textf);
  if (beamMax-Qmax >=5) {
    printf("Warning: notice the difference in bcm6 and average of bcm 1&2\n");
    printf("             beamMax: %f, Qmax: %f\n",beamMax,Qmax);
  }
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
  printf("Ebeam considered On if above %f.\n", beamFrac * Qmax);

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
      printf("cutL[%d]=%d\n",m,cutL.back());
      flipperIsUp = kTRUE;
      m++;
    }
    if(laser<=20*1000) n++;
    else n=0;
    if(flipperIsUp){
      if(n == 0 || index == nEntries-1 ){
        cutL.push_back(index-1);
        printf("cutL[%d]=%d\n",m,cutL.back());
        //sprintf(textwrite,"%d %d",m,cutL.back());
        //	outfile <<textwrite<<endl;
        m++;
        flipperIsUp = kFALSE;
      }
    }
    //    find and record electron beam off periods
//     rampIsDone = bcm > beamFrac * beamMax;
//     isABeamTrip = bcm <= (1.0 - beamFrac) *beamMax;
    rampIsDone = comptQ > 0.2 * Qmax;//!review this condition
    isABeamTrip = comptQ <= 0.1*Qmax;//!review this condition

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
    printf("cutE[%i]=%i\n",i,cutE.at(i));
  }

  chain->SetBranchStatus("*",1);//turn on all branches again
  nLasCycBeamTrips = o*500+m/2;
  return o*500+m/2;//nEntries for both arrays is encoded into return value
}

/* !A couple of inconsistenties exit as of now:
.. 1. the beam trip evaluation is neither what is claimed nor what it should be
.. 2. 
..*/
