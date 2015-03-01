/*************************************************
 * Courtesy: Don Jones
 * modified by: Amrendra
 * thanks Juan Carlos
 ************************************************ */
#ifndef __getEBeamLasCuts_F
#define __getEBeamLasCuts_F

#include "rootClass.h"
#include "comptonRunConstants.h"

/////////////////////////////////////////////////////////////////////////////
//*** This function cycles through the laser power entries, creating an ***//
//*** array of values for starting and ending points of the laser off   ***//
//*** periods. Even indices 0,2,4... are beginning points and odd       ***//
//*** indices 1,3,5... are endpoints.The function returns the number of ***//
//*** entries in the array. It also records electron beam off periods   ***//
//*** in a similar fashion. It requires the Hel_Tree to be used.        ***//
///////////////////////////////////////////////////////////////////////////// 

Int_t getEBeamLasCuts(std::vector<Int_t> &cutL, std::vector<Int_t> &cutE, TChain *chain, Int_t runnum)
{
  filePre = Form(filePrefix,runnum,runnum);
  const Bool_t debug = 0;
  chain->ResetBranchAddresses();
  Int_t nEntries = chain->GetEntries();
  Double_t laser = 0.0, bcm = 0.0, patNum=0.0;

  TH1D *hBeam = new TH1D("hBeam","dummy",100,0,200);//typical value of maximum beam current
  TH1D *hLaser = new TH1D("hLaser","dummy",1000,0,250000);//typical value of maximum laser power

  chain->Draw("yield_sca_bcm6.value>>hBeam","","goff");
  //chain->Draw("yield_sca_bcm6.value>>hBeam");
  hBeam = (TH1D*)gDirectory->Get("hBeam");  
  //beamMax = hBeam->GetMaximum();
  beamMean = hBeam->GetMean();
  beamMeanEr = hBeam->GetMeanError();
  beamRMS = hBeam->GetRMS();
  beamMax = hBeam->GetBinLowEdge(hBeam->FindLastBinAbove(100));//to avoid extraneous values
  //cout<<"beamMax(bcm6): "<<beamMax<<",\t beamMean: "<<beamMean<<",\t beamRMS: "<<beamRMS<<endl;
  beamTripLimit = beamFrac * beamMax;

  chain->Draw("yield_sca_laser_PowT.value>>hLaser","","goff");
  hLaser = (TH1D*)gDirectory->Get("hLaser");  
  laserMax = hLaser->GetBinLowEdge(hLaser->FindLastBinAbove(100));//to avoid extraneous values
  cout<<"laserMax = "<<laserMax<<endl;

  Int_t n = 0, m = 0, o = 0, p = 0, q = 0;
  Bool_t flipperIsUp = kFALSE, isABeamTrip = kFALSE;
  Bool_t rampIsDone = kTRUE, prevTripDone = kTRUE;
  ofstream fOut;
  TString file;

  TBranch *bLaser;
  TBranch *bBCM;
  TLeaf *lLaser;
  TLeaf *lBCM;

  chain->SetBranchStatus("*",0);//turn off all branches
  chain->SetBranchStatus("yield_sca_laser_PowT",1);//turn on cavity pow branch
  chain->SetBranchStatus("yield_sca_bcm6",1);//turn on bcm branch
  chain->SetBranchStatus("pattern_number",1);
  chain->SetBranchAddress("pattern_number",&patNum);

  chain->SetAutoDelete(kTRUE);
  printf("Ebeam considered On if above %f.\n", beamFrac*beamMax);

  for(Int_t index=0; index<nEntries;index++) {
    chain->GetEntry(index);
    bLaser = bBCM = 0;
    bLaser = (TBranch*)chain->GetBranch("yield_sca_laser_PowT");
    lLaser = lBCM = 0;
    lLaser = (TLeaf*)bLaser->GetLeaf("value");
    laser = lLaser->GetValue();
    bBCM = (TBranch*)chain->GetBranch("yield_sca_bcm6");
    lBCM = (TLeaf*)bBCM->GetLeaf("value");
    bcm = lBCM->GetValue();

    ////find laser off periods and record start and finish entries
    // if(laser<=laserFrac*laserMax) n++;
    // else if((laser<maxLasPow)&&(laser>=0.0)) n=0; 
    if(laser<=laserFrac*laserMax) n++;///laser is off for n consecutive entries
    else n=0; ///laser On begins 

    if (n==minEntries) { ///laser has been off for minEntries/240 seconds continuously, hence consider it a valid laseroff
      cutL.push_back(index-minEntries+1);//!the +1 is needed to take care of the fact that C++ counts "index" from 0, while 'minEntries' is compared only when 'n' goes all the way from 1 to minEntries.
      printf("cutL[%d]=%d, laserPow falls to %d @index:%d & pat#:%d\n",m,cutL.back(),(Int_t)laser, index-minEntries+1, (int)patNum-minEntries);//print begin of lasOff entry
      if(debug) printf("laserPow here:%d, index:%d\n",(Int_t)laser,index);
      flipperIsUp = kTRUE; ///laserOff state begins
      m++; ///cutLas array's even number index (corresponding to a laserOff)
    }
    if(flipperIsUp){ ///if the laser is known to be off check ...
      if(n == 0 || index == nEntries-1) { ///if laser On has just begun OR the end of run has been reached
        cutL.push_back(index); ///record this as the end of laserOn cycle
        printf("cutL[%d]=%d, laserPow rises to %d @index:%d & pat#:%d\n",m,cutL.back(),(Int_t)laser,index, (int)patNum);
        m++; ///cutLas array's odd number index (corresponding to a laserOn)
        flipperIsUp = kFALSE; ///laserOff state ends
      }
    }
    ///&&&&&&&&&&&&&&&&&&&& beam cut off process &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    ///find and record electron beam off periods
    //rampIsDone = (bcm> (beamFrac*beamMax) && (bcm <200.0));
    //isABeamTrip = (bcm<= (beamFrac*beamMax) && (bcm >0.0));
    rampIsDone = ((bcm> beamTripLimit) && (bcm <beamMaxEver));
    isABeamTrip = ((bcm<= beamTripLimit) && (bcm >0.0));

    if(isABeamTrip && prevTripDone) {
      //to make sure it is a beam trip not a problem with acquisition
      //insist > avoidDAQEr in a row meet the isABeamTrip criterion
      q++;
      if(q>avoidDAQEr) { ///beam is found off for over 10 consecutive entries (~ 40ms) 
        q = 0;
        o++; ///cutE array's even number index (corresponding to a beamTrip)
        if (index >= (PREV_N_ENTRIES+avoidDAQEr)) { //to protect the beamTrip that may have occured in the first 2s of the run
          cutE.push_back(index-(PREV_N_ENTRIES+avoidDAQEr)); ///register the entry# ~ 2s before this instance as a beam-trip
          printf("%scutE[%d]=%i,   bcm:%3.2f   index:%d\n%s",red, (o)/2, cutE.back(),bcm,index,normal);
          //printf("ch1 bcm:%3.2f, index:%d\n",bcm,index);
        }
        else {
          cutE.push_back(index-avoidDAQEr);///note: index = q -1
          printf("%scutE[]=%i,   bcm:%3.2f   index:%d\n%s",blue,cutE.back(),bcm,index,normal);
          //printf("ch2 bcm:%3.2f, index:%d\n",bcm,index);
        }
        prevTripDone = kFALSE; ///register that this beamTrip has been recorded
      }
    }///if(isABeamTrip && prevTripDone)

    if(!prevTripDone && rampIsDone){
      p++;      
      if(p>=WAIT_N_ENTRIES) { //wait ~2s for beam to stabilize in this state
        o++; ///cutE array's odd number index (corresponding to a beam recovery)
        p = 0; 
        cutE.push_back(index); ///register that the trip is recovered. 
        printf("%scutE[%d]=%i,   bcm:%3.2f   index:%d\n%s",green, (o+1)/2, cutE.back(), bcm, index,normal);
        //printf("ch3 bcm:%3.2f, o:%d  index:%d\n",bcm,o,index);
        prevTripDone = kTRUE; 
      }
    }
  }///for(Int_t index=0; index<nEntries;index++)

  //if the run ends with a beam trip, record last entry in cutE vector
  if(o%2!=0){
    cutE.push_back(nEntries-1);
    printf("%scutE[]=%i,   bcm:%3.2f\n%s",green,cutE.back(),bcm,normal);
    o++;
  }
  //print the beam trip cuts
  //printf("going to write beam cut file\n");
  file = Form("%s/%s/%scutBeam.txt",pPath, txt,filePre.Data());
  fOut.open(file);
  if(fOut.is_open()) {
    cout<<Form("%s/%s/%scutBeam.txt",pPath, txt,filePre.Data())<<" file created"<<endl;
    for(Int_t i=0;i<o;i++) {
      //if(debug) printf("cutE[%i]=%i\n",i,cutE.at(i));
      fOut << cutE.at(i) <<endl;
    }
    fOut.close();
  } else cout<<red<<"couldn't open "<<file<<normal<<endl;

  //printf("going to write cutL file\n");
  file = Form("%s/%s/%scutLas.txt",pPath, txt,filePre.Data());
  fOut.open(file);
  if(fOut.is_open()) {
    cout<<file<<" created"<<endl;
    for(Int_t i=0;i<m;i++) {
      //if(debug) printf("cutL[%i]=%i\n",i,cutL.at(i));
      fOut << cutL.at(i) <<endl;
    }
    fOut.close();
  } else cout<<red<<"couldn't open "<<file<<normal<<endl;

  if(debug) {
    cout<<blue<<"\nrunnum\tbeamMax\tbeamRMS\tbeamMean\tbeamMeanEr\tbeamTrips\tlasMax\tnLasCyc\tnEntries"<<endl;
    cout<<Form("%d\t%.2f\t%f\t%f\t%f\t%.0f\t%.2f\t%.0f\t%.0f",runnum,beamMax,beamRMS,beamMean,beamMeanEr,((Float_t)cutE.size()-2.0)/2,laserMax,((Float_t)cutL.size()-2.0)/2,(Float_t)nEntries)<<normal<<endl;
  }

  file = Form("%s/%s/%sinfoBeamLas.txt",pPath, txt,filePre.Data());
  fOut.open(file);
  if(fOut.is_open()) {
    cout<<file<<" created\n"<<endl;
    fOut<<"runnum\tbeamMax\tbeamRMS\tbeamMean\tbeamMeanEr\tnBeamTrips\tlasMax\tnLasCycles\tnEntries"<<endl;
    fOut<<runnum<<"\t"<<beamMax<<"\t"<<beamRMS<<"\t"<<beamMean<<"\t"<<beamMeanEr<<"\t"<<(cutE.size())/2<<"\t"<<laserMax<<"\t"<<(cutL.size()-2)/2<<"\t"<<nEntries<<endl;
    fOut.close();
  } else cout<<red<<"couldn't open "<<file<<normal<<endl;

  return o*500+m/2;//nEntries for both arrays is encoded into return value
}
#endif

/* Comments**************************
 * the entry numbers in cutLas and cutEB demarcate the periods when the the laser
 *..and beam were ON and Good. This is unlike the previous version where the 
 *..demarcation was for beam Off and laser-off.
 * we throw away about 2 seconds of data before the beamTrip is found
 * the laser on state doesn't have a check for how long it stays ON
 ************************************/
