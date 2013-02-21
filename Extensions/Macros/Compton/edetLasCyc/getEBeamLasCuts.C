/*************************************************
 * Courtesy: Don Jones
 * modified by: Amrendra
 * thanks Juan Carlos
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
  filePrefix= Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug = 1;
  chain->ResetBranchAddresses();
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

  ofstream outfileLas,outfileBeam,infoBeamLas;
  outfileLas.open(Form("%s/%s/%scutLas.txt",pPath,webDirectory,filePrefix.Data()));
  if(outfileLas.is_open())cout<<Form("%s/%s/%scutLas.txt",pPath,webDirectory,filePrefix.Data())<<" file created\n"<<endl;

  outfileBeam.open(Form("%s/%s/%scutBeam.txt",pPath,webDirectory,filePrefix.Data()));
  if(outfileBeam.is_open())cout<<Form("%s/%s/%scutBeam.txt",pPath,webDirectory,filePrefix.Data())<<" file created\n"<<endl;

  infoBeamLas.open(Form("%s/%s/%sinfoBeamLas.txt",pPath,webDirectory,filePrefix.Data()));
  if(infoBeamLas.is_open())cout<<Form("%s/%s/%sinfoBeamLas.txt",pPath,webDirectory,filePrefix.Data())<<" file created\n"<<endl;
  infoBeamLas<<";runnum\tbeamMax\tnBeamTrips\tlasMax\tnLasCycles\tnEntries"<<endl;

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
    if(laser<=laserFrac*laserMax) n++;///laser is off for n consecutive entries
    else if((laser<maxLasPow)&&(laser>=0.0)) n=0; ///laser On begins ///the additional if ensures that the laser readback is not non-sensical

    if (n==minEntries) { ///laser has been off for minEntries/960 seconds continuously, hence consider it a valid laseroff
      cutL.push_back(index-minEntries+1);//!the +1 is needed to take care of the fact that C++ counts "index" from 0, while 'minEntries' is compared only when 'n' goes all the way from 1 to minEntries.
      if(debug) printf("cutL[%d]=%d\n",m,cutL.back());///print begin of laser off entry
      flipperIsUp = kTRUE; ///laserOff state begins
      m++; ///cutLas array's even number index (corresponding to a laserOff)
    }
    if(flipperIsUp){ ///if the laser is known to be off check ...
      if(n == 0 || index == nEntries-1) { ///if laser On has just begun OR the end of run has been reached
        cutL.push_back(index); ///record this as the end of laserOn cycle
	if(debug) printf("cutL[%d]=%d\n",m,cutL.back());///print end of laser off entry
        m++; ///cutLas array's odd number index (corresponding to a laserOn)
        flipperIsUp = kFALSE; ///laserOff state ends
      }
    }
    ///    find and record electron beam off periods
    // rampIsDone = (bcm> (beamFrac*beamMax) && (bcm <200.0));
    // isABeamTrip = (bcm<= (beamFrac*beamMax) && (bcm >0.0));
    rampIsDone = (bcm> 50.0 && (bcm <200.0));//!?this is to make the comparision equivalent to that by Vladas
    isABeamTrip = (bcm<= 20.0 && (bcm >0.0));

    if(isABeamTrip && prevTripDone) {
      //to make sure it is a beam trip not a problem with acquisition
      //insist > 100 in a row meet the isABeamTrip criterion
      q++;
      if(q>=100) { ///beam is found off for over 100 consecutive entries (~ 100ms) 
        q = 0;
        o++; ///cutE array's even number index (corresponding to a beamTrip)
	if (index >= (PREV_N_ENTRIES+100)) 
	  cutE.push_back(index-(PREV_N_ENTRIES + 100)); ///register the entry# ~ 5s before this instance as a beam-trip
        else cutE.push_back(index);
        prevTripDone = kFALSE; ///register that the trip is not recovered yet
      }
    }

    if(!prevTripDone && rampIsDone){
      p++;      
      if(p>=WAIT_N_ENTRIES) { //wait ~10s for beam to stabilize in this state
        o++; ///cutE array's odd number index (corresponding to a beam recovery)
        p = 0; 
        cutE.push_back(index); ///register that the trip is recovered. 
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
  if(debug) printf("going to write beam cut file\n");
  for(Int_t i=0;i<o;i++) {
    if(debug) printf("cutE[%i]=%i\n",i,cutE.at(i));
    outfileBeam << cutE.at(i) <<endl;
  }

  if(debug) printf("going to write cutL file\n");
  for(Int_t i=0;i<m;i++) {
    //if(debug) printf("cutL[%i]=%i\n",i,cutL.at(i));
    outfileLas << cutL.at(i) <<endl;
  }

  infoBeamLas<<Form("%5.0f\t%.2f\t%.0f\t%.2f\t%f.0\t%.0f\n",(Float_t)runnum,beamMax,(Float_t)o,laserMax,((Float_t)cutL.size()-2.0)/2,(Float_t)nEntries);
  outfileLas.close();
  outfileBeam.close();
  infoBeamLas.close();
  nLasCycBeamTrips = o*500+m/2;
  return o*500+m/2;//nEntries for both arrays is encoded into return value
}

/* Comments**************************
 * !The output file instead of going to the run's edet-subdirectory should be in the run 
 * ..directory and should be used as a generic file used by both compton componets
 * the entry numbers in cutLas and cutEB demarcate the periods when the the laser
 *..and beam were ON and Good. This is unlike the previous version where the 
 *..demarcation was for beam Off and laser-off.
 * we throw away about 2 seconds of data before the beamTrip is found
 * the laser on state doesn't have a check for how long it stays ON?
 ************************************/
