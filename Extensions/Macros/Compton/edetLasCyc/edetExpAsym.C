////Don Jones seeded the code and Juan Carlos helped significantly in the development of this code
#include <rootClass.h>
#include "getEBeamLasCuts.C"
#include "maskedStrips.C"

///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////
 
Int_t edetExpAsym(Int_t runnum, Float_t stripAsym[nPlanes][nStrips], Float_t stripAsymEr[nPlanes][nStrips], Float_t stripAsymRMS[nPlanes][nStrips], Bool_t isFirst100k=kFALSE)
{
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Bool_t debug = 1, debug1 = 0, debug2 = 0;
  Bool_t beamOn =kFALSE;//lasOn,
  Int_t chainExists = 0,goodCycles=0;
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nMpsB1H1L1=0, nMpsB1H0L1=0;
  Int_t nMpsB1H1L0L=0, nMpsB1H0L0L=0, nMpsB1H1L0R=0, nMpsB1H0L0R=0;
  Int_t nMpsB1H1L0=0,nMpsB1H0L0=0;
  Int_t entry=0; ///integer assitant in reading entry from a file
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Int_t AccumB1H0L1[nPlanes][nStrips],AccumB1H1L1[nPlanes][nStrips];
  Int_t AccumB1H0L0L[nPlanes][nStrips],AccumB1H1L0L[nPlanes][nStrips],AccumB1H0L0R[nPlanes][nStrips],AccumB1H1L0R[nPlanes][nStrips] ;
 //Int_t AccumB0H0L0[nPlanes][nStrips],AccumB0H0L1[nPlanes][nStrips],AccumB0H1L0[nPlanes][nStrips],AccumB0H1L1[nPlanes][nStrips];
  Double_t comptQH1L1=0.0, comptQH0L1=0.0;
  Double_t comptQH1L0L=0.0, comptQH0L0R=0.0;
  Double_t comptQH1L0R=0.0, comptQH0L0L=0.0;
  //Double_t lasPowB1H1=0.0, lasPowB1H0=0.0; //, lasPowB0H1, lasPowB0H0;
  //Int_t unNormAcB1L0[nPlanes][nStrips],unNormAcB1L1[nPlanes][nStrips];
  //Int_t unNormAcB1H1[nPlanes][nStrips],unNormAcB1H0[nPlanes][nStrips];
  //Double_t unNormQL0;//,unNormQL1,unNormQH0,unNormQH1;
  Double_t lasPow[3], helicity, bcm[3];
  Double_t pattern_number, event_number;
  Double_t bRawAccum[nPlanes][nStrips];//qNormAcB1H1L0LLasCyc
  //  Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsymRMS[nPlanes][nStrips] ;
  Float_t laserOnOffRatioH1=0.0, laserOnOffRatioH0=0.0;
  Float_t qNormAcB1H1L1LasCyc[nPlanes][nStrips], qNormAcB1H0L1LasCyc[nPlanes][nStrips];
  Float_t qNormAcB1H0L0RLasCyc[nPlanes][nStrips], qNormAcB1H1L0LLasCyc[nPlanes][nStrips];
  Float_t qNormAcB1H0L0LLasCyc[nPlanes][nStrips], qNormAcB1H1L0RLasCyc[nPlanes][nStrips];
  Float_t BCqNormAcB1H1L1LasCyc[nPlanes][nStrips], BCqNormAcB1H0L1LasCyc[nPlanes][nStrips];//Background Corrected
  Float_t BCqNormLasCycSum[nPlanes][nStrips], BCqNormLasCycDiff[nPlanes][nStrips];
  Float_t weightedMeanNrAsym[nPlanes][nStrips],weightedMeanDrAsym[nPlanes][nStrips];
  Float_t qNormLasCycAsym[nPlanes][nStrips], LasCycAsymEr[nPlanes][nStrips],LasCycAsymErSqr[nPlanes][nStrips];
  Float_t errB1H1L1[nPlanes][nStrips],errB1H0L1[nPlanes][nStrips];
  Float_t errB1H1L0L[nPlanes][nStrips],errB1H0L0L[nPlanes][nStrips],errB1H1L0R[nPlanes][nStrips],errB1H0L0R[nPlanes][nStrips];
  TString readEntry;
  TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam

  ofstream outfileExpAsymP, checkStat,checkStat2,checkStat3 ;
  ifstream infileLas, infileBeam;

  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);
  
  ///following variables are not to be reset every laser-cycle hence lets initialize with zero
  for(Int_t p = startPlane; p <endPlane; p++) {      	
    for(Int_t s =startStrip; s <endStrip; s++) {
      //if (maskedStrips(p,s)) continue;    
      weightedMeanNrAsym[p][s]=0,weightedMeanDrAsym[p][s]=0;
    }
  }

  /// Open either Pass1 or the First 100K
  if( isFirst100k) {
    chainExists = mpsChain->Add(Form("$QW_ROOTFILES/first100k_%d.root",runnum));
  }
  else {
    chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Run2
    //chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for myQwAnalyisis output
    printf("Attached %d files to chain for Run # %d\n",chainExists,runnum);
  }
  if(!chainExists){//delete chains and exit if files do not exist
    cout<<"\n***Error: File for run "<<runnum<<" does not exist***\n"<<endl;
    delete mpsChain;
    return -1;
  }
  
  infileLas.open(Form("analOut/r%d_cutLas.txt",runnum));
  infileBeam.open(Form("analOut/r%d_cutBeam.txt",runnum));
    
  if (infileLas.is_open() && infileBeam.is_open()) {
    cout<<"Found the cutLas and cutEB file"<<endl;
    while (infileLas.good()) {
      infileLas >> readEntry; //read the contents of the line in a string first
      if (readEntry.IsDigit()) { //check if the string is a combination of numbers of not
	entry = readEntry.Atoi(); //if string is a combination of numbers get the corresponding Integer of this string
	if (debug2) printf("cutLas[%d]=%d\n",(Int_t)cutLas.size(),entry);
	cutLas.push_back(entry);
      }
      //else cout<<"check cutLas file for "<<runnum<<endl;///this always happens at the end of file read
    }
    infileLas.close();
    nLasCycles = (cutLas.size() - 2)/2;
    
    while (infileBeam.good()) {
      infileBeam >> readEntry;
      if (readEntry.IsDigit()) {
	entry = readEntry.Atoi();
	if (debug2) printf("cutEB[%d]=%d\n",(Int_t)cutEB.size(),entry);
	cutEB.push_back(entry);
      }
      //else cout<<"check cutEB file for "<<runnum<<endl;
    }
    infileBeam.close();
    nBeamTrips = (cutEB.size())/2;
  }
  else {
    cout << "\n*****:Atleast one of the Cut files missing *****\n"<<endl;
    cout<<"          hence executing the cut function"<<endl;
    Int_t nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, mpsChain,runnum);
    if (debug) printf("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
    nLasCycles = nLasCycBeamTrips%1000 - 1;
    ////first two digits of return value of getEBeamLasCuts
    nBeamTrips = nLasCycBeamTrips / 1000;
    ////fourth digit of return value of getEBeamLasCuts
  }
  
  if (debug) printf("cutEB.size:%d,cutLas.size:%d\n",cutEB.size(),cutLas.size());

  Int_t nEntries = mpsChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);

  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<endl;
  
  mpsChain->ResetBranchAddresses();//!? should it be here?
  mpsChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  mpsChain->SetBranchStatus("actual_helicity",1);
  mpsChain->SetBranchStatus("sca_laser_PowT",1);
  mpsChain->SetBranchStatus("sca_bcm6",1);
  mpsChain->SetBranchStatus("p*Ac",1);

  mpsChain->SetBranchAddress("event_number",&event_number);
  mpsChain->SetBranchAddress("pattern_number",&pattern_number);
  mpsChain->SetBranchAddress("actual_helicity",&helicity);
  mpsChain->SetBranchAddress("sca_laser_PowT",&lasPow);
  mpsChain->SetBranchAddress("sca_bcm6",&bcm);

  for(Int_t p = 0; p <nPlanes; p++) {      
    mpsChain->SetBranchAddress(Form("p%dRawAc",p+1),&bRawAccum[p]);
  }//the branch for each plane is named from 1 to 4

  if (debug2) {    
    checkStat.open(Form("check_r%d.txt",runnum));
    checkStat2.open(Form("check2_r%d.txt",runnum));
    checkStat3.open(Form("check3_r%d.txt",runnum));
  }

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ///since this is the beginning of a new Laser cycle, and all Laser cycle based variables 
    ///..are already assigned to a permanent variable reset the LasCyc based variables
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0L= 0, nMpsB1H0L0L= 0, nMpsB1H1L0R= 0, nMpsB1H0L0R= 0;
    nMpsB1H1L0= 0, nMpsB1H0L0= 0, missedLasEntries=0; 
    comptQH1L1= 0.0, comptQH0L1= 0.0, comptQH1L0L= 0.0, comptQH0L0L= 0.0, nMpsB1H1L0R= 0, nMpsB1H0L0R= 0;
    laserOnOffRatioH1= 0.0, laserOnOffRatioH0= 0.0;
    //lasPowB1H1= 0.0, lasPowB1H0= 0.0;
    for(Int_t p = 0; p <nPlanes; p++) {      
      for(Int_t s = 0; s <nStrips; s++) {
	AccumB1H0L0L[p][s] =0,AccumB1H0L0R[p][s] =0, AccumB1H1L0L[p][s] =0, AccumB1H1L0R[p][s] =0;
	AccumB1H0L1[p][s] =0, AccumB1H1L1[p][s] =0;
	qNormAcB1H1L1LasCyc[p][s]= 0.0, qNormAcB1H0L1LasCyc[p][s]= 0.0; 
	qNormAcB1H0L0RLasCyc[p][s]= 0.0, qNormAcB1H1L0LLasCyc[p][s]= 0.0;
	qNormAcB1H0L0LLasCyc[p][s]= 0.0, qNormAcB1H1L0RLasCyc[p][s]= 0.0;
	BCqNormAcB1H1L1LasCyc[p][s]= 0.0, BCqNormAcB1H0L1LasCyc[p][s]= 0.0; 
	BCqNormLasCycSum[p][s]= 0.0, BCqNormLasCycDiff[p][s]= 0.0;
	qNormLasCycAsym[p][s]= 0.0, LasCycAsymEr[p][s]= 0.0,LasCycAsymErSqr[p][s]= 0.0;
	errB1H1L1[p][s]=0.0,errB1H0L1[p][s]=0.0;
	errB1H1L0L[p][s]=0.0,errB1H0L0L[p][s]=0.0,errB1H1L0R[p][s]=0.0,errB1H0L0R[p][s]=0.0;
	stripAsym[p][s]= 0.0,stripAsymEr[p][s]= 0.0,stripAsymRMS[p][s]= 0.0;
      }
    }

    if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
    else if(nthBeamTrip < nBeamTrips) {  ///yes, we do have beamtrip(s)
      if(nthBeamTrip==0) { // haven't encountered a trip yet(special case of first trip)
	if(cutLas.at(2*nCycle+3)<cutEB.at(0)) beamOn = kTRUE; ///no beam trip till the end of THIS laser cycle
	else {                    ///there is a beam trip during this laser cycle
	  beamOn = kFALSE;
	  nthBeamTrip++;          ///encountered a beam trip
	}
      }
      else if(cutLas.at(2*nCycle+1)<cutEB.at(2*nthBeamTrip-1)) beamOn=kFALSE;///continuation of the previous nthBeamTrip
      else if(cutLas.at(2*nCycle+3)<cutEB.at(2*nthBeamTrip)) beamOn=kTRUE;
      else { ///encountered "another" beam trip	
	beamOn = kFALSE;  
	nthBeamTrip++;          
      }
    }
    else if(nthBeamTrip == nBeamTrips) { ///encountered the last beamTrip     
      if (cutLas.at(2*nCycle+1) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE; ///current laser Cycle begins after the beamTrip recovered
      else beamOn = kFALSE;
    }   
    else cout<<"\n***Error ... Something drastically wrong in BeamTrip evaluation***\n"<<endl;
      
    if(debug) cout<<"Will analyze from entry # "<<cutLas.at(2*nCycle)<<" to entry # "<<cutLas.at(2*nCycle+3)<<endl;

    for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+3); i++) { 
      //loop over laser cycle periods taking one LasOn state and two adjacent laserOff states
      if(debug && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;

      if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<minLasPow) l= -1; ///left laser off zone
      else if((i > cutLas.at(2*nCycle+2)) && lasPow[0]<minLasPow) l =0; ///right laser off zone
      else if((i >=cutLas.at(2*nCycle+1)) && (i <=cutLas.at(2*nCycle+2))) l =1;///laser on zone
      ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currents assign it(may change!)
      else missedLasEntries++;

      mpsChain->GetEntry(i);
      h = (Int_t)helicity;

      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	// 	if (h ==1 || h ==0) {  ////to avoid the h=-9999 that appears in beginning of every runlet
	if (h ==0 && l ==0) {
	  nMpsB1H0L0R++;
	  comptQH0L0R += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L0R[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	    }
	  }
	}
	else if (h ==0 && l ==-1) {
	  nMpsB1H0L0L++;
	  comptQH0L0L += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L0L[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	    }
	  }
	}
	else if (h ==0 && l==1) {////the elseif statement helps avoid overhead in each entry
	  nMpsB1H0L1++;
	  comptQH0L1 += bcm[0];
	  //lasPowB1H0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L1[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	    }	  
	  }
	}
	else if (h ==1 && l==0) {
	  nMpsB1H1L0R++;
	  comptQH1L0R += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L0R[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0];// /lasPow[0];
	    }
	  }
	}
	else if (h ==1 && l==-1) {
	  nMpsB1H1L0L++;
	  comptQH1L0L += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L0L[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0];// /lasPow[0];
	    }
	  }
	}
	else if (h ==1 && l==1) {
	  nMpsB1H1L1++;
	  comptQH1L1 += bcm[0];
	  //lasPowB1H1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L1[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0];// /lasPow[0];
	    }
	  }
	}
      }
    }///for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+3); i++)
    if(debug) cout<<"Had to skip "<<missedLasEntries<<" entries in this laser cycle"<<endl;

    //after having filled the above vectors based on laser and beam periods, find asymmetry
    if (beamOn) {
      goodCycles++;
      nMpsB1H1L0 = nMpsB1H1L0L + nMpsB1H1L0R; ///total laserOff time for this laserCycle
      laserOnOffRatioH1 = nMpsB1H1L1/nMpsB1H1L0;

      nMpsB1H0L0 = nMpsB1H0L0L + nMpsB1H0L0R; ///total laserOff time for this laserCycle
      laserOnOffRatioH0 = nMpsB1H0L1/nMpsB1H0L0;

      if (debug1) cout<<"the Laser Cycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
      if (nMpsB1H0L1<= 0 || nMpsB1H1L1<= 0 || nMpsB1H0L0L<= 0 || nMpsB1H1L0L<= 0|| nMpsB1H0L0R<= 0 || nMpsB1H1L0R<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0L:%d, nMpsB1H1L0L:%d, nMpsB1H0L0R:%d, nMpsB1H1L0R:%d**\n",
	       nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0L,nMpsB1H1L0L,nMpsB1H0L0R,nMpsB1H1L0R);
      else if (comptQH0L1<= 0 || comptQH1L1<= 0 || comptQH0L0L<= 0 || comptQH1L0L<= 0 || comptQH0L0R<= 0 || comptQH1L0R<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check comptQH0L1:%f,comptQH1L1:%f, comptQH0L0R:%f, comptQH1L0R:%f**\n",
	       nCycle,comptQH0L1,comptQH1L1,comptQH0L0R,comptQH1L0R);
      else {
	if(debug2) {
	  printf("comptQB1H1L1:%f\tcomptQB1H0L1:%f\tcomptQB1H1L0R:%f\tcomptQB1H0L0R:%f\n",
		 comptQH1L1,comptQH0L1,comptQH1L0R,comptQH0L0R);
	  printf("nMpsB1H1L1:%d\tnMpsB1H0L1:%d\tnMpsB1H1L0R:%d\tnMpsB1H0L0R:%d\n",
		 nMpsB1H1L1,nMpsB1H0L1,nMpsB1H1L0R,nMpsB1H0L0R);
	}
	for (Int_t p =startPlane; p <endPlane; p++) {	  	  
	  for (Int_t s =startStrip; s <endStrip; s++) {	  
	    if (maskedStrips(p,s)) continue;
	    qNormAcB1H1L1LasCyc[p][s]  = AccumB1H1L1[p][s]  /comptQH1L1;
	    qNormAcB1H0L1LasCyc[p][s]  = AccumB1H0L1[p][s]  /comptQH0L1;
	    qNormAcB1H1L0LLasCyc[p][s] = AccumB1H1L0L[p][s] /comptQH1L0L;
	    qNormAcB1H0L0LLasCyc[p][s] = AccumB1H0L0L[p][s] /comptQH0L0L;
	    qNormAcB1H1L0RLasCyc[p][s] = AccumB1H1L0R[p][s] /comptQH1L0R;
	    qNormAcB1H0L0RLasCyc[p][s] = AccumB1H0L0R[p][s] /comptQH0L0R;

	    if(debug2) printf("AccumB1H1L1[%d][%d]: %d\tAccumB1H0L1: %d\tAccumB1H1L0R: %d\tAccumB1H0L0R: %d\n",
			     p,s,AccumB1H1L1[p][s],AccumB1H0L1[p][s],AccumB1H1L0R[p][s],AccumB1H0L0R[p][s]);

	    BCqNormAcB1H1L1LasCyc[p][s]=qNormAcB1H1L1LasCyc[p][s]-laserOnOffRatioH1*(qNormAcB1H1L0LLasCyc[p][s]+qNormAcB1H1L0RLasCyc[p][s]);
	    BCqNormAcB1H0L1LasCyc[p][s]=qNormAcB1H0L1LasCyc[p][s]-laserOnOffRatioH0*(qNormAcB1H0L0LLasCyc[p][s]+qNormAcB1H0L0RLasCyc[p][s]); 

	    BCqNormLasCycDiff[p][s] = (BCqNormAcB1H1L1LasCyc[p][s] - BCqNormAcB1H0L1LasCyc[p][s]);
	    BCqNormLasCycSum[p][s]  = (BCqNormAcB1H1L1LasCyc[p][s] + BCqNormAcB1H0L1LasCyc[p][s]);
	    if (BCqNormLasCycSum[p][s]  <= (Float_t)0.0 && s<=Cedge) {
	      printf("\n**Warning**:BCqNormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCqNormLasCycSum[p][s],nCycle);
	      printf("note: AccumB1H1L1:%d, AccumB1H1L0R:%d, AccumB1H0L1:%d, AccumB1H0L0R:%d\n"
		     ,AccumB1H1L1[p][s],AccumB1H1L0R[p][s],AccumB1H0L1[p][s],AccumB1H0L0R[p][s]);
	      printf("and comptQH1L1:%f, comptQH1L0R:%f, comptQH0L1:%f, comptQH0L0R:%f",
		     comptQH1L1,comptQH1L0R,comptQH0L1,comptQH0L0R);
	    }
	    else {
	      qNormLasCycAsym[p][s] = (BCqNormLasCycDiff[p][s] / BCqNormLasCycSum[p][s]);
	      
	      ///Evaluation of error on asymmetry
	      errB1H1L1[p][s] =(1-qNormLasCycAsym[p][s])/(comptQH1L1 *BCqNormLasCycSum[p][s]);
	      errB1H0L1[p][s] =(1+qNormLasCycAsym[p][s])/(comptQH0L1 *BCqNormLasCycSum[p][s]);
	      errB1H1L0L[p][s]=(1-qNormLasCycAsym[p][s])/(comptQH1L0L*BCqNormLasCycSum[p][s]);
	      errB1H0L0L[p][s]=(1+qNormLasCycAsym[p][s])/(comptQH0L0L*BCqNormLasCycSum[p][s]);
	      errB1H1L0R[p][s]=(1-qNormLasCycAsym[p][s])/(comptQH1L0R*BCqNormLasCycSum[p][s]);
	      errB1H0L0R[p][s]=(1+qNormLasCycAsym[p][s])/(comptQH0L0R*BCqNormLasCycSum[p][s]);

	      LasCycAsymErSqr[p][s] =(pow(errB1H1L1[p][s],2))*(AccumB1H1L1[p][s])+ (pow(errB1H0L1[p][s],2))*(AccumB1H0L1[p][s])
		+ (pow(errB1H1L0R[p][s],2))*(AccumB1H1L0R[p][s])+ (pow(errB1H0L0R[p][s],2))*(AccumB1H0L0R[p][s])
		+ (pow(errB1H1L0L[p][s],2))*(AccumB1H1L0L[p][s])+ (pow(errB1H0L0L[p][s],2))*(AccumB1H0L0L[p][s]);

	      if(LasCycAsymErSqr[p][s]!=(Float_t)0.0)
		weightedMeanNrAsym[p][s] += qNormLasCycAsym[p][s]/LasCycAsymErSqr[p][s]; ///Numerator eqn 4.17(Bevington)
	      else cout<<"check plane "<<p<<" strip "<<s<<" . It gives zero Asym Er"<<endl;
	      weightedMeanDrAsym[p][s] += 1.0/LasCycAsymErSqr[p][s]; ///Denominator eqn 4.17(Bevington)
	    }
	    if (debug2) {
// 	      checkStat<<nCycle<<" "<<p+1<<" "<<s+1<<"  "<<qNormLasCycAsym[p][s]<<"\t"<<LasCycAsymErSqr[p][s]<<"\t"<<weightedMeanDrAsym[p][s]<<endl;
// 	      checkStat2<<nCycle<<" "<<p+1<<" "<<s+1<<"  "<<errB1H1L1[p][s]<<"\t"<<errB1H0L1[p][s]<<"\t"<<errB1H1L0R[p][s]<<"\t"<<errB1H0L0R[p][s]<<endl;
// 	      checkStat3<<nCycle<<" "<<p+1<<" "<<s+1<<"  "<<pow(errB1H1L1[p][s],2)<<"\t"<<pow(errB1H0L1[p][s],2)<<"\t"<<pow(errB1H1L0[p][s],2)<<"\t"<<pow(errB1H0L0[p][s],2)<<endl;
	      printf("qNormLasCycAsym[p%d][s%d]= %f (stat.err:%f)\n",p,s,qNormLasCycAsym[p][s],LasCycAsymErSqr[p][s]);
	      printf("formed by normalized BC (%f -/+ %f) \n",qNormAcB1H1L1LasCyc[p][s],qNormAcB1H0L1LasCyc[p][s]);
	    }
	  }
	}///for (Int_t p =startPlane; p <endPlane; p++) {
      }///sanity check of being non-zero for filled laser cycle variables
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip(nthBeamTrip:"<<nthBeamTrip<<"), hence skipping"<<endl;
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
  if (debug2) {
    checkStat.close(), checkStat2.close(), checkStat3.close();
  }
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (maskedStrips(p,s)) continue;
      if(weightedMeanDrAsym[p][s]==0.0) cout<<"stand. deviation in weighted Mean Asym is ZERO for p"<<p<<" s"<<s<<"\n"<<endl;
      else if(weightedMeanNrAsym[p][s]==0.0) cout<<"asym for strip "<<s<<" in plane "<<p<<" is zero"<<endl;
      else stripAsym[p][s]= weightedMeanNrAsym[p][s]/weightedMeanDrAsym[p][s];
      stripAsymEr[p][s] = TMath::Sqrt(1/weightedMeanDrAsym[p][s]);
      stripAsymRMS[p][s] = stripAsymEr[p][s];//!!!temp fix, since I'm not histogramming anymore, so I don't have an RMS from a distribution
      if(debug2) printf("stripAsym[%d][%d]:%f  stripAsymEr:%f\n",p,s,stripAsym[p][s],stripAsymEr[p][s]);
    }
  }
  
  for(Int_t p = startPlane; p < endPlane; p++) {
    outfileExpAsymP.open(Form("analOut/r%d_expAsymP%d.txt",runnum,p+1));
    //outfileExpAsymP<<"strip\texpAsym\tasymEr\tasymRMS"<<endl; ///If I want a header for the following text
    cout<<Form("analOut/r%d_expAsymP%d.txt",runnum,p+1)<<" file created"<<endl;
    for (Int_t s =startStrip; s <endStrip;s++) {    
      if (maskedStrips(p,s)) continue;
      outfileExpAsymP<<Form("%2.0f\t%f\t%f\t%f\t%f\n",(Float_t)s+1,stripAsym[p][s],stripAsymEr[p][s],weightedMeanNrAsym[p][s],weightedMeanDrAsym[p][s]);
    }
    outfileExpAsymP.close();
    cout<<Form("analOut/r%d_expAsymP%d.txt",runnum,p+1)<<" filled and closed"<<endl;
  }
  delete mpsChain;
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate edetExpAsym.\n",div_output.quot,div_output.rem );  
  return goodCycles;//the function returns the number of used Laser cycles
}

/******************************************************
!Querries:
* tried to return 'goodCycles' integer but the program didn't do that
*	//!?what if the run starts with a beamTrip
******************************************************/

/******************************************************
!Further modifications:
* evolve a mechanism for making the laser-off events equivalent to laser-on in time
* plot qNormAcB1H1L0LasCyc and qNormAcB1H0L0LasCyc against
* ..strip numbers to watch the helicity correlated beam-background
* evaluate asymmetry per pattern_number and see its behaviour
* ensure efficient evaluation of beamtrips
* check consistency of cut on laserPow and beamtrip
* add color code to the error and warning messages
******************************************************/

/******************************************************
Comments:
* B1: beam on; H1: helicity plus; L1: laser on; and vice versa
* qNormXXX: charge normalized quantity; tNormXXX: time normalized quantity
* BC: Backgraound Corrected.
* each Laser cycle consitutes of one laser on and one laser off period.
* ..it begins with a Laser On period and ends with the (just)next laser off period.
* While checking if we want to consider a laser cycle as bad, we check till 
* ..the beginning of the next laser cycle on purpose to make sure that the 
* ..beam was indeed ON during both laser On as well as Off periods
* nCycle refers to the present/current/ongoing laser-cycle
* nthBeamTrip (on the contrary) refers to the upcoming beamTrip
* the code counts from '0' but at all human interface, I add '+1' hence human counting
******************************************************/
