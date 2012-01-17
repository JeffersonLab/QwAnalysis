#include <rootClass.h>
#include "getEBeamLasCuts.C"
//#include "comptonRunConstants.h" //we don't need to re-include this
div_t div_output;
//////////////////////////////////////////////////////////////////////////
/** This calculates the Compton differential cross section asymmetry.  **/
/** The asymmetry equation below is in dimensionless units x=k/kmax    **/
/** which puts the Compton edge at x = 1.Strip # has to be converted   **/
/** to this dimensionless scale.                                       **/
/** Arguments: scattering energy in strip # and e-beam polarization    **/
/** Beam and laser parameters are set as constants.                    **/ 
//////////////////////////////////////////////////////////////////////////

TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments

vector<Int_t>cutLas;//arrays of cuts for laser
vector<Int_t>cutEB;//arrays of cuts for electron beam

///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the results
///////////////////////////////////////////////////////////////////////////
 
Int_t edetasym(Int_t runnum1, Int_t runnum2, Bool_t isFirst100k=kFALSE)
{
  Bool_t debug  = 1;
  Bool_t debug1 = 1;//print statements with line numb
  Bool_t debug2 = 1;
  Bool_t debug3 = 0;
  gStyle->SetFillColor(0);
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1); //displays the fit parameters on the respective plots
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleH(0.07);
  gStyle->SetLineWidth(2);

  time_t tStart = time(0), tEnd; 
  Bool_t  lasOn, beamOn=kFALSE, chainExists,lastTrip=kFALSE,goodCycle=kFALSE,newCycle=kTRUE;
  Int_t h = 0, l = 0, nthBeamTrip = 0, nBeamTrips = 0;
  Int_t nLasCycles, nMpsH0 = 0,nMpsH1 = 0, nMpsBkgd = 0;
  Int_t p1Ac[2][2][nSTRIPS], p2Ac[2][2][nSTRIPS],p3Ac[2][2][nSTRIPS],p4Ac[2][2][nSTRIPS];
  Int_t nMpsH0LasCyc = 0, nMpsH1LasCyc = 0, nMpsH0BkgdLasCyc = 0, nMpsH1BkgdLasCyc = 0;
  Double_t lasPow[3], lasMax, helicity, comptQ[3], Qmax;//, beamMax, bcm
  Double_t pattern_number,  event_number;
  Double_t bP1[nSTRIPS], bP2[nSTRIPS], bP3[nSTRIPS], bP4[nSTRIPS];

  std::vector< Int_t > lasOnH1; //[nCycle]
  std::vector< Int_t > lasOnH0; //[nCycle]
  std::vector< Int_t > lasOffH1; //[nCycle]
  std::vector< Int_t > lasOffH0; //[nCycle]
  std::vector< Int_t > nMpsLasOnH1;
  std::vector< Int_t > nMpsLasOnH0;
  std::vector< Int_t > nMpsLasOffH1;
  std::vector< Int_t > nMpsLasOffH0;
  std::vector< Int_t > unNormLasCycSum;
  std::vector< Int_t > secH1LasCyc;
  std::vector< Int_t > secH0LasCyc;
  std::vector< Int_t > secH1BkgdLasCyc;
  std::vector< Int_t > secH0BkgdLasCyc;
  std::vector< Double_t > normLasOnHpLasCyc;
  std::vector< Double_t > normLasOnHmLasCyc;
  std::vector< Double_t > normLasOffHpLasCyc;
  std::vector< Double_t > normLasOffHmLasCyc;

  std::vector< Double_t > BCnormLasOnHpLasCyc;
  std::vector< Double_t > BCnormLasOnHmLasCyc;
  std::vector< Double_t > BCnormLasCycDiff;
  std::vector< Double_t > BCnormLasCycSum;
  std::vector< Double_t > tNormLasCycAsym;
  std::vector< Double_t > LasCycAsymEr;

  TH1D *hAsymPlaneStr = new TH1D("hAsymPlaneStr", "Plane2SingleStripAsym", 25,-0.001,0.001);
  TH1D *hAsymErPlaneStr = new TH1D("hAsymErPlaneStr","Single Strip asymmetry error",25,-0.0001,0.0001);

  hAsymPlaneStr->SetBit(TH1::kCanRebin);
  hAsymPlaneStr->SetLineColor(kBlue);
  hAsymPlaneStr->SetTitle("Plane 2 Strip 50 Laser cycle based Asymmetry");
  hAsymPlaneStr->GetXaxis()->SetTitle("Asymmetry");
  hAsymPlaneStr->GetYaxis()->SetTitle("Counts");

  hAsymErPlaneStr->SetBit(TH1::kCanRebin);
  hAsymErPlaneStr->SetLineColor(kBlue);
  hAsymErPlaneStr->SetTitle("Plane 2 Strip 50 Laser cycle based Asymmetry Error");
  hAsymErPlaneStr->GetXaxis()->SetTitle("Asym Error");
  hAsymErPlaneStr->GetYaxis()->SetTitle("Counts");

  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetBorderSize(1);
 
  /** Open either Pass1 or the First 100K **/
  for (Int_t runnum = runnum1; runnum <= runnum2; runnum++) {
    if( isFirst100k) {
      mpsChain->Add(Form("$QW_ROOTFILES/first100k_%d.root",runnum));
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/pass2/first100k_%d.root",runnum));
    }
    else {
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Qweak Run2
    }

    if(!chainExists){//delete chains and exit if files do not exist
      cout<<"File does not exist."<<endl;
      printf("file for run # %d does not exist\n",runnum);
      delete mpsChain;
      return 0;
    }
    printf("Run numbers used for analysis are %d \n",runnum);
  }
  //printf("%d files attached to chain.\n",chainExists);! shows always only 1 file attached

  Int_t nEntries = mpsChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);

  Int_t nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, mpsChain);
  if (debug3) printf("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
  if (debug3) printf("cutEB.size:%d,cutLas.size:%d\n",cutEB.size(),cutLas.size());
  mpsChain->ResetBranchAddresses();//!? should it be here?
  nLasCycles = nLasCycBeamTrips%1000 - 1;
  ////first two digits of return value of getEBeamLasCuts
  nBeamTrips = nLasCycBeamTrips / 1000;
  ////fourth digit of return value of getEBeamLasCuts

  Qmax = mpsChain->GetMaximum("compton_charge/value");//!I should use sca_bcm6
  cout<<"Qmax = "<<Qmax <<endl;
  lasMax = mpsChain->GetMaximum("sca_laser_PowT/value");
  cout<<"lasMax="<<lasMax<<endl;
  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<endl;

  mpsChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  mpsChain->SetBranchStatus("actual_helicity",1);
  mpsChain->SetBranchStatus("sca_laser_PowT",1);
  mpsChain->SetBranchStatus("compton_charge",1);
  mpsChain->SetBranchStatus("p*Ac",1);

  mpsChain->SetBranchAddress("event_number",&event_number);
  mpsChain->SetBranchAddress("pattern_number",&pattern_number);
  mpsChain->SetBranchAddress("actual_helicity",&helicity);
  mpsChain->SetBranchAddress("sca_laser_PowT",&lasPow);
  mpsChain->SetBranchAddress("compton_charge",&comptQ);

//   mpsChain->SetBranchAddress("p1RawAc",&bP1);
  mpsChain->SetBranchAddress("p2RawAc",&bP2);
//   mpsChain->SetBranchAddress("p3RawAc",&bP3);
//   mpsChain->SetBranchAddress("p4RawAc",&bP4);
  
  Int_t mystr = 48;
  //   for(Int_t s=0; s<nSTRIPS; s++) { //!temp for speed
//     if (newCycle) {   //// Initialize the arrays
      for(Int_t s=mystr;s<=mystr;s++) {  
	for(Int_t h=0; h<2; h++) {
	  for(Int_t l=0; l<2; l++) {
	    p1Ac[h][l][s] = 0, p2Ac[h][l][s] = 0, p3Ac[h][l][s] = 0, p4Ac[h][l][s] = 0;
	  } 
	}
      }
//       cout<< "beginning of LasOn ...resetting all accum counters" << endl;
//     }

////currently v r going through each entry go on accumulating the counters, and when the beam is found off
////..we jump over to the next event of beam-on and laser-on

////instead v shd enter only those LasCycles for which we know that there isn't any beam trips till 
////..the end of this Laser cycle

  Int_t nCycle = 0; //this entire loop is currently only for one strip of one plane
  for(Int_t i=cutLas.at(nCycle); i<nEntries; i++) { //loop over laser cycle periods
    if(debug && i%25000==0) printf("%i events analyzed.\n",i);
    if(i<124) i = 124; //the first 122 events with unknown helicity shouldn't creep in
    if(i==(2*nCycle+1)) newCycle=kTRUE;
    else newCycle=kFALSE;
    if(i >= (cutLas.at(2*nCycle)) -3) { //beginning of the last laser cycle!?"-3"?
      if(i == cutLas.at(2*nCycle)) cout<<"\nLaser OFF at i="<<i<<"; Laser Cycle="<<nCycle<<endl;
      l = 0;//in case run ends with laser OFF
      if(i >= cutLas.at(2*nLasCycles+1)) l = 1;//in case run ends with laser ON!?"how can this condition get fulfilled inside the above if statement"?
    }

    if(nthBeamTrip < nBeamTrips) {
      if(i >=cutEB.at(2*nthBeamTrip) && i<=cutEB.at(2*nthBeamTrip+1)) { //identifying beam-off
	beamOn = kFALSE;//declaring beam-off
	cout<<"Beam OFF at entry "<<i <<endl;
	i = cutEB.at(2*nthBeamTrip+1);//jump to the next beamOn entry
	cout<<". Skip to beam ON at entry "<<i<<endl;
	////this is not just beamOff but also ramping beam etc hence not of use
	///!Comment:later-on I can purify this 'beamOff' period to accumulate some clean beam-off counts 
	//..which will help in background evaluation for the same run
	nthBeamTrip ++;

	if (nthBeamTrip == nBeamTrips) lastTrip=kTRUE;
 	if (debug1) printf("line:%d,Trip:%d,nCycle:%d,i:%d,cutEB.size:%d,nthTrip:%d\n",__LINE__,lastTrip,nCycle,i,cutEB.size(),nthBeamTrip);
	while (i > cutLas.at(2*nCycle+1)) {
	  nCycle=nCycle+1;//syncs the nCycle with the jumped value of 'i'
	  cout<<"jumped to a new Laser Cycle: nCycle # "<<nCycle<<" i="<<i<<endl;
	  if (debug1) printf("line %d, nCycle:%d, i:%d\n", __LINE__,nCycle,i);
	}
	if (i <= cutLas.at(2*nCycle + 1)) {
// 	  if (debug1) printf("line %d, nCycle:%d, i:%d\n", __LINE__,nCycle,i);
	  i=cutLas.at(2*nCycle + 1);//after crossing the beam-off,go to the next laser-on event
	  cout<<"Hey, looking for LasOn beginning, I jumped to entry # "<< i <<endl;
	}
	if (debug1) printf("line %d, nCycle:%d, i:%d\n", __LINE__,nCycle,i);
      }
      else beamOn = kTRUE;
    }
    
    mpsChain->GetEntry(i);

    lasOn = lasPow[0] > laserFrac *lasMax;
    h = (Int_t)helicity;

    if(l==1 && h==0 && lasOn && beamOn) { //Laser stable at On & H- & Beam On
      nMpsH0++;
      nMpsH0LasCyc++;//nMpsLasOnH0++
      //       for(Int_t s=0; s<nSTRIPS; s++) { //sum up each strip individually
      for(Int_t s=mystr; s==mystr; s++) {
	// 	p1Ac[h][1][s] += (Int_t)bP1[s];
	p2Ac[h][1][s] += (Int_t)bP2[s];
	// 	p3Ac[h][1][s] += (Int_t)bP3[s];
	// 	p4Ac[h][1][s] += (Int_t)bP4[s];
      }
    }
    else if(l==1 && h==1 && lasOn && beamOn) { //Laser stable at On & H+ & Beam On
      nMpsH1++;      
      nMpsH1LasCyc++;//nMpsLasOnH1++;
      //       for(Int_t s=0; s<nSTRIPS; s++) {	//sum up each strip individually. 
      for(Int_t s=mystr; s==mystr; s++) {
	// 	p1Ac[h][1][s] += (Int_t)bP1[s];
	p2Ac[h][1][s] += (Int_t)bP2[s];
	// 	p3Ac[h][1][s] += (Int_t)bP3[s];
	// 	p4Ac[h][1][s] += (Int_t)bP4[s];
      }
    }
    else if(l==0 && h==0 && beamOn) { //Laser stable at Off & H- & Beam On
      nMpsBkgd++;      
      nMpsH0BkgdLasCyc++;//nMpsLasOffH0++;
      //       for(Int_t s=0; s<nSTRIPS; s++) { //sum up each strip individually
      for(Int_t s=mystr; s==mystr; s++) {
	// 	p1Ac[h][0][s] += (Int_t)bP1[s];
	p2Ac[h][0][s] += (Int_t)bP2[s];
	// 	p3Ac[h][0][s] += (Int_t)bP3[s];
	// 	p4Ac[h][0][s] += (Int_t)bP4[s];
      }
    }
    else if(l==0 && h==1 && beamOn) { //Laser stable at Off & H+ & Beam On
      nMpsBkgd++;      
      nMpsH1BkgdLasCyc++;//nMpsLasOffH1++;
      //       for(Int_t s=0; s<nSTRIPS; s++) {	//sum up each strip individually. 
      for(Int_t s=mystr; s==mystr; s++) {
	// 	p1Ac[h][0][s] += (Int_t)bP1[s];
	p2Ac[h][0][s] += (Int_t)bP2[s];
	// 	p3Ac[h][0][s] += (Int_t)bP3[s];
	// 	p4Ac[h][0][s] += (Int_t)bP4[s];
      }
    }
    if(nCycle < nLasCycles) {
      if (i>=cutLas.at(2*nCycle+1)) {// && cutLas.at(2*nCycle+2)<=cutEB.at(2*nthBeamTrip)) { 
 	if (debug1) printf("line:%d,Trip:%d,nCycle:%d,i:%d,nthTrip:%d\n",__LINE__,lastTrip,nCycle,i,nthBeamTrip);
	if ((!lastTrip) ? cutLas.at(2*nCycle+2)<cutEB.at(2*nthBeamTrip) : kTRUE) {
 	if (debug1) printf("line:%d,Trip:%d,nCycle:%d,i:%d,nthTrip:%d\n",__LINE__,lastTrip,nCycle,i,nthBeamTrip);
	//(beginning of LasOn period) & end of this period not falling into a beamTrip
	//      if (newCycle) {
	  //    if we've passed last beamTrip proceed, else check if the next beamTrip doesn't happen till the end of THIS LasCyc
	  //    odd number entry crpd's to ON state
	  // 	I've to later insert a loop here for the strip # 'mystr'
	  lasOnH1.push_back(p2Ac[1][1][mystr]);
	  lasOffH1.push_back(p2Ac[1][0][mystr]);
	  if (debug2) printf("lasOnH1:%d\t lasOffH1:%d\n",lasOnH1[nCycle],lasOffH1[nCycle]);
	  lasOnH0.push_back(p2Ac[0][1][mystr]);
	  lasOffH0.push_back(p2Ac[0][0][mystr]);
	  if (debug2) printf("lasOnH0:%d\t lasOffH0:%d\n",lasOnH0[nCycle],lasOffH0[nCycle]);
	  nMpsLasOnH1.push_back(nMpsH1LasCyc);
	  nMpsLasOffH1.push_back(nMpsH1BkgdLasCyc);
	  if (debug2) printf("nMpsLasOnH1:%d\t nMpsLasOffH1:%d \n",nMpsLasOnH1[nCycle],nMpsLasOffH1[nCycle]);
	  nMpsLasOnH0.push_back(nMpsH0LasCyc);
	  nMpsLasOffH0.push_back(nMpsH0BkgdLasCyc);
	  if (debug2) printf("nMpsLasOnH0:%d\t nMpsLasOffH0:%d \n",nMpsLasOnH0[nCycle],nMpsLasOffH0[nCycle]);

	  secH0LasCyc.push_back(0), secH1LasCyc.push_back(0);
	  secH0BkgdLasCyc.push_back(0), secH1BkgdLasCyc.push_back(0);
	  normLasOnHpLasCyc.push_back(0.0), normLasOnHmLasCyc.push_back(0.0);
	  normLasOffHpLasCyc.push_back(0.0), normLasOffHmLasCyc.push_back(0.0);

	  BCnormLasOnHpLasCyc.push_back(0.0), BCnormLasOnHmLasCyc.push_back(0.0);
	  BCnormLasCycDiff.push_back(0.0), BCnormLasCycSum.push_back(0.0);

	  unNormLasCycSum.push_back(0),tNormLasCycAsym.push_back(0.0),LasCycAsymEr.push_back(0.0);
	  if(debug) printf("for nCycle:%d, i:%d, tNormLasCycAsym.size:%d\n\n",nCycle,i,tNormLasCycAsym.size());
	
	  nCycle ++;
	  cout<<"\nLaser ON at i= "<<i<< "; Laser Cycle="<<nCycle<<endl;
	  l = 1;//laser is ON
	  nMpsH0LasCyc=0,nMpsH1LasCyc=0,nMpsH0BkgdLasCyc=0,nMpsH1BkgdLasCyc=0;
	  nMpsH0=0,nMpsH1=0,nMpsBkgd=0;

	  ////since this is the beginning of a new Laser cycle, and I have already assigned the 
	  ////accumulated counts to a permanent variable reset the LasCyc counters
	  //!temp for speed	for(Int_t s=0; s<nSTRIPS; s++) {
	  for(Int_t s=mystr;s<=mystr;s++) {
	    for(Int_t h=0; h<2; h++) {
	      for(Int_t l=0; l<2; l++) {
		p1Ac[h][l][s]=0, p2Ac[h][l][s]=0, p3Ac[h][l][s]=0, p4Ac[h][l][s]=0;
	      }
	    }
	  }
	}
      }
    }
  }

  if (debug2) printf("\n  ****Entering laser Cycle Analysis**** \n");
  for (Int_t nCycle=0; nCycle <(Int_t)lasOnH1.size(); nCycle=nCycle+1) {
    if (debug2) printf("\nnCycle:%d\n",nCycle);
    if (nMpsLasOnH0[nCycle] <= 0 || nMpsLasOnH1[nCycle] <= 0) {
      printf("**number of Laser ON events in this laser nCycle:%d is ZERO**\n",nCycle); 
    }
    else if (nMpsLasOffH0[nCycle] <= 0 || nMpsLasOffH1[nCycle] <= 0) {
      printf("**number of Laser Off events in this laser nCycle:%d is ZERO**\n",nCycle); 
    }
    else {
      secH1LasCyc[nCycle] = nMpsLasOnH1[nCycle];// /FLIP_RATE;
      if (debug3)printf("secH1LasCyc :%d\n",secH1LasCyc[nCycle]);
      secH0LasCyc[nCycle] = nMpsLasOnH0[nCycle];// /FLIP_RATE;
      if (debug3)printf("secH0LasCyc :%d\n", secH0LasCyc[nCycle]);
      secH1BkgdLasCyc[nCycle] = nMpsLasOffH1[nCycle];// /FLIP_RATE;
      if (debug3)printf("secH1BkgdLasCyc :%d\n",secH1BkgdLasCyc[nCycle]);
      secH0BkgdLasCyc[nCycle] = nMpsLasOffH0[nCycle];// /FLIP_RATE;
      if (debug3)printf("secH0BkgdLasCyc :%d\n",secH0BkgdLasCyc[nCycle]);
      
      normLasOnHpLasCyc[nCycle] = (Double_t)lasOnH1[nCycle]/secH1LasCyc[nCycle];
      if(debug2)printf(" normLasOnHpLasCyc:\t%f = lasOnH1:%d / secH1LasCyc:%d\n",normLasOnHpLasCyc[nCycle],lasOnH1[nCycle],(Int_t)secH1LasCyc[nCycle]);
      normLasOffHpLasCyc[nCycle] = (Double_t)lasOffH1[nCycle]/secH1BkgdLasCyc[nCycle];
      if(debug2)printf(" normLasOffHpLasCyc:\t%f = lasOffH1:%d / secH1BkgdLasCyc:%d\n",normLasOffHpLasCyc[nCycle],lasOffH1[nCycle],(Int_t)secH1BkgdLasCyc[nCycle]);
      
      normLasOnHmLasCyc[nCycle] = (Double_t)lasOnH0[nCycle]/secH0LasCyc[nCycle];
      if(debug2)printf(" normLasOnHmLasCyc:\t%f = lasOnH0:%d / secH0LasCyc:%d\n",normLasOnHmLasCyc[nCycle],lasOnH0[nCycle],(Int_t)secH0LasCyc[nCycle]);
      normLasOffHmLasCyc[nCycle] = (Double_t)lasOffH0[nCycle]/secH0BkgdLasCyc[nCycle];
      if(debug2)printf(" normLasOffHmLasCyc:\t%f = lasOffH0:%d / secH0BkgdLasCyc:%d\n",normLasOffHmLasCyc[nCycle],lasOffH0[nCycle],(Int_t)secH0BkgdLasCyc[nCycle]);
      
      BCnormLasOnHpLasCyc[nCycle] = normLasOnHpLasCyc[nCycle] - normLasOffHpLasCyc[nCycle];
      BCnormLasOnHmLasCyc[nCycle] = normLasOnHmLasCyc[nCycle] - normLasOffHmLasCyc[nCycle];

      BCnormLasCycDiff[nCycle] = BCnormLasOnHpLasCyc[nCycle] - BCnormLasOnHmLasCyc[nCycle];
      if (debug2)printf(" BCnormLasCycDiff:\t%f\n", BCnormLasCycDiff[nCycle]);
      BCnormLasCycSum[nCycle] = BCnormLasOnHpLasCyc[nCycle] + BCnormLasOnHmLasCyc[nCycle];
      if (debug2)printf(" BCnormLasCycSum:%f\n", BCnormLasCycSum[nCycle]);
      
      unNormLasCycSum[nCycle] = lasOnH1[nCycle] + lasOnH0[nCycle];
      if (debug2) printf("Starting to evaluate asym\n");
      if (debug2) printf(" unNormLasCycSum:%d\n",unNormLasCycSum[nCycle]);
      
      if (BCnormLasCycSum[nCycle] <= 0.0)
	printf("\n\n**Warning**:normLasCycSum[nCycle=%d] is %f, strip=%d\n\n",
	       nCycle,BCnormLasCycSum[nCycle],mystr);
      else {
	tNormLasCycAsym[nCycle] = (BCnormLasCycDiff[nCycle] / BCnormLasCycSum[nCycle]);
	hAsymPlaneStr->Fill(tNormLasCycAsym[nCycle]);
	LasCycAsymEr[nCycle] = 1.0/sqrt(unNormLasCycSum[nCycle]);
	if (debug2) printf("sqrt(unNormLasCycSum[nCycle]):%f\n",sqrt(unNormLasCycSum[nCycle]));
	hAsymErPlaneStr->Fill(LasCycAsymEr[nCycle]);
	//if (debug3) printf("line %d\n", __LINE__);
	if (debug2) {
	  printf("for nCycle:%d, tNormLasCycAsym[strip:%d]= %f (stat.err:%f)\n"
		 ,nCycle,mystr,tNormLasCycAsym[nCycle],LasCycAsymEr[nCycle]);
	  printf("formed by normalized BC (%f -/+ %f) \n",
		 normLasOnHpLasCyc[nCycle],normLasOnHmLasCyc[nCycle]);
	}
      }
    }
  }
  
//   printf("nMpsH0= %i,   nMpsH1= %i,  nMpsBkgd= %i\n",nMpsH0,nMpsH1,nMpsBkgd);
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,400);
  c1->Divide(2,1);
  c1->cd(1);
  hAsymPlaneStr->Draw("H");

  hAsymPlaneStr->Fit("gaus");

  c1->cd(2);
  hAsymErPlaneStr->Draw("H");

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to complete.\n",div_output.quot,div_output.rem );

  return 1;
}
