////Don Jones seeded the code and Juan Carlos helped significantly in the development of this code
#include <rootClass.h>
#include "getEBeamLasCuts.C"
#include "maskedStrips.C"

div_t div_output;

TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments
vector<Int_t>cutLas;//arrays of cuts for laser
vector<Int_t>cutEB;//arrays of cuts for electron beam

///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////
 
Int_t edetExpAsym(Int_t runnum1, Int_t runnum2, Float_t stripAsym[nPlanes][nStrips], Float_t stripAsymEr[nPlanes][nStrips], Float_t stripAsymRMS[nPlanes][nStrips], Bool_t isFirst100k=kFALSE)
{
  time_t tStart = time(0), tEnd; 
  Bool_t debug = 1, debug1 = 0, debug2 = 0;
  Bool_t lasOn, beamOn =kFALSE;
  Int_t chainExists = 0,goodCycles=0;
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nMpsB1H1L1, nMpsB1H0L1, nMpsB1H1L0, nMpsB1H0L0;
  Int_t entry;
  Float_t AccumB1H0L0[nPlanes][nStrips],AccumB1H0L1[nPlanes][nStrips],AccumB1H1L0[nPlanes][nStrips],AccumB1H1L1[nPlanes][nStrips];
  Double_t comptQH1L1, comptQH0L1, comptQH1L0, comptQH0L0;
  Double_t lasPowB1H1, lasPowB1H0; //, lasPowB0H1, lasPowB0H0;
  //Double_t unNormLasCycSum[nPlanes][nStrips];
  Double_t lasPow[3], lasMax, helicity, bcm[3];
  Double_t pattern_number, event_number;
  Double_t bRawAccum[nPlanes][nStrips];
  //  Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsymRMS[nPlanes][nStrips] ;
  Float_t normAcB1H1L1LasCyc[nPlanes][nStrips], normAcB1H0L1LasCyc[nPlanes][nStrips];
  Float_t normAcB1H0L0LasCyc[nPlanes][nStrips], normAcB1H1L0LasCyc[nPlanes][nStrips];
  Float_t BCnormAcB1H1L1LasCyc[nPlanes][nStrips], BCnormAcB1H0L1LasCyc[nPlanes][nStrips];//Background Corrected
  Float_t BCnormLasCycSum[nPlanes][nStrips], BCnormLasCycDiff[nPlanes][nStrips];
  Double_t qNormLasCycAsym[nPlanes][nStrips], LasCycAsymEr[nPlanes][nStrips];
  Float_t errB1H1L1[nPlanes][nStrips],errB1H0L1[nPlanes][nStrips],errB1H1L0[nPlanes][nStrips],errB1H0L0[nPlanes][nStrips];
  TString readEntry;
  TH1D *h1[10], *h2[10]; //!temp test histograms
  char hName[nPlanes][120],hNameEr[nPlanes][120];
  std::vector<std::vector<TH1D> > hAsymPS;
  std::vector<std::vector<TH1D> > hAsymErPS;
  ofstream outfileExpAsymP; 
  ifstream infileLas, infileBeam;

  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);

  for (Int_t p =0; p <nPlanes; p++) {   
    if (p >= (Int_t) hAsymPS.size()) {
      hAsymPS.resize(p+1);
      hAsymErPS.resize(p+1);
    }
    for (Int_t s =0; s<nStrips; s++) { 
      sprintf(hName[p],"asymPlane%d_Str%d",p+1,s+1);
      hAsymPS[p].push_back(TH1D(hName[p],"Plane strip",25,-0.001,0.001));
      sprintf(hNameEr[p],"asymErPlane%dStr%d",p+1,s+1);
      hAsymErPS[p].push_back(TH1D(hNameEr[p],"Single Strip asym stat.error",25,-0.001,0.001));

      hAsymPS[p][s].SetBit(TH1::kCanRebin);
      hAsymPS[p][s].SetTitle(Form("Plane %d Strip %d Asymmetry",p+1,s+1));
      hAsymPS[p][s].GetXaxis()->SetTitle("Asymmetry");
      hAsymPS[p][s].GetYaxis()->SetTitle("Counts");

      hAsymErPS[p][s].SetBit(TH1::kCanRebin);
      hAsymErPS[p][s].SetTitle(Form("Plane %d Strip %d Asymmetry Error",p+1,s+1));
      hAsymErPS[p][s].GetXaxis()->SetTitle("Asym Error");
      hAsymErPS[p][s].GetYaxis()->SetTitle("Counts");
    }
  }

  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetBorderSize(1);
  
  /** Open either Pass1 or the First 100K **/
  for (Int_t runnum = runnum1; runnum <= runnum2; runnum++) {
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

    infileLas.open(Form("r%d_cutLas.txt",runnum));
    infileBeam.open(Form("r%d_cutBeam.txt",runnum));
    
    if (infileLas.is_open() && infileBeam.is_open()) {
      cout<<"Found the cutLas and cutEB file"<<endl;
      while (infileLas.good()) {
	infileLas >> readEntry; //read the contents of the line in a string first
	if (readEntry.IsDigit()) { //check if the string is a combination of numbers of not
	  entry = readEntry.Atoi(); //if string is a combination of numbers get the corresponding Integer of this string
	  if (debug) printf("cutLas[%d]=%d\n",(Int_t)cutLas.size(),entry);
	  cutLas.push_back(entry);
	}
	else cout<<"check cutLas file for "<<runnum<<endl;
      }
      infileLas.close();
      nLasCycles = (cutLas.size() - 2)/2;
    
      while (infileBeam.good()) {
	infileBeam >> readEntry;
	if (readEntry.IsDigit()) {
	  entry = readEntry.Atoi();
	  if (debug) printf("cutEB[%d]=%d\n",(Int_t)cutEB.size(),entry);
	  cutEB.push_back(entry);
	}
	else cout<<"check cutEB file for "<<runnum<<endl;
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
  }
  Int_t nEntries = mpsChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);

  mpsChain->ResetBranchAddresses();//!? should it be here?
  
  lasMax = mpsChain->GetMaximum("sca_laser_PowT/value");
  cout<<"lasMax="<<lasMax<<endl;
  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<endl;
  
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

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ///since this is the beginning of a new Laser cycle, and all Laser cycle based variables 
    ///..are already assigned to a permanent variable reset the LasCyc based variables
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0= 0, nMpsB1H0L0= 0;
    comptQH1L1= 0.0, comptQH0L1= 0.0, comptQH1L0= 0.0, comptQH0L0= 0.0;
    lasPowB1H1= 0.0, lasPowB1H0= 0.0;
    for(Int_t p = 0; p <nPlanes; p++) {      
      for(Int_t s = 0; s <nStrips; s++) {
	AccumB1H0L0[p][s] =0.0, AccumB1H0L1[p][s] =0.0, AccumB1H1L0[p][s] =0.0, AccumB1H1L1[p][s] =0.0;
	normAcB1H1L1LasCyc[p][s]= 0.0, normAcB1H0L1LasCyc[p][s]= 0.0; 	//unNormLasCycSum[p][s]= 0.0;
	normAcB1H0L0LasCyc[p][s]= 0.0, normAcB1H1L0LasCyc[p][s]= 0.0;
	BCnormAcB1H1L1LasCyc[p][s]= 0.0, BCnormAcB1H0L1LasCyc[p][s]= 0.0; 
	errB1H1L1[p][s]=0.0,errB1H0L1[p][s]=0.0,errB1H1L0[p][s]=0.0,errB1H0L0[p][s]=0.0;

	BCnormLasCycSum[p][s]= 0.0, BCnormLasCycDiff[p][s]= 0.0;
	qNormLasCycAsym[p][s]= 0.0, LasCycAsymEr[p][s]= 0.0;
	stripAsym[p][s]= 0.0,stripAsymEr[p][s]= 0.0,stripAsymRMS[p][s]= 0.0;
      }
    }

    if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
    else if(nthBeamTrip < nBeamTrips) {  ///finite number of beamtrip(s)
      if(cutLas.at(2*nCycle+3)<cutEB.at(2*nthBeamTrip))	beamOn = kTRUE; ///no beam trip till the end of THIS laser cycle
      else {                    ///there is a beam trip during this laser cycle
	beamOn = kFALSE;
	nthBeamTrip++;          ///encountered a beam trip
      }
    }
    else if(nthBeamTrip == nBeamTrips) { ///encountered the last beamTrip     
      if (cutLas.at(2*nCycle+1) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE; ///current laser Cycle begins after the beamTrip recovered
      else beamOn = kFALSE;
    }
    else cout<<"\n***Error ... Something drastically worng in BeamTrip evaluation***\n"<<endl;
      
    for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++) { 
      //loop over laser cycle periods from one LasOn state upto just before beginning of next LasOn
      if(debug && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;
      if ((i >= cutLas.at(2*nCycle+1)) && (i < cutLas.at(2*nCycle+2))) l=1;
      if (i >= cutLas.at(2*nCycle+2)) l=0;

      mpsChain->GetEntry(i);
      lasOn = lasPow[0] > laserFrac *lasMax;//!this part should be directly done in getEBeamLasCuts.C
      h = (Int_t)helicity;
      if ((l==1) && lasOn) l=1; //making the laser On requirement more strict

      /**********
       * (a)currently this method appears a little inefficient since we not processing beamOff data
       * why do we even enter this laser-cycle which is not going to do anything;
       * but later on, we are going to use use the beam off part in someway and this would make sense then
       **********/
      
      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	// 	if (h ==1 || h ==0) {  ////to avoid the h=-9999 that appears in beginning of every runlet
	if (h ==0 && l ==0) {
	  nMpsB1H0L0++;
	  comptQH0L0 += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L0[p][s] += bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	    }
	  }
	}
	else if (h ==0 && l==1) {////the elseif statement helps avoid overhead in each entry
	  nMpsB1H0L1++;
	  comptQH0L1 += bcm[0];
	  lasPowB1H0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L1[p][s] += bRawAccum[p][s];// / bcm[0]; // /lasPow[0];	    
	    }	  
	  }
	}
	else if (h ==1 && l==0) {
	  nMpsB1H1L0++;
	  comptQH1L0 += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L0[p][s] += bRawAccum[p][s];// / bcm[0];// /lasPow[0];	    
	    }
	  }
	}
	else if (h ==1 && l==1) {
	  nMpsB1H1L1++;
	  comptQH1L1 += bcm[0];
	  lasPowB1H1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L1[p][s] += bRawAccum[p][s];// / bcm[0];// /lasPow[0];	    
	    }
	  }
	}
      }
    }///for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++)
    
    //after having filled the above vectors based on laser and beam periods, its time to calculate
    if (beamOn) {
      goodCycles++;
      if (debug1) cout<<"the Laser Cycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
      if (nMpsB1H0L1<= 0 || nMpsB1H1L1<= 0 || nMpsB1H0L0<= 0 || nMpsB1H1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0:%d, nMpsB1H1L0:%d**\n",
	       nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0,nMpsB1H1L0);
      else if (comptQH0L1<= 0 || comptQH1L1<= 0 || comptQH0L0<= 0 || comptQH1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check comptQH0L1:%f,comptQH1L1:%f, comptQH0L0:%f, comptQH1L0:%f**\n",
	       nCycle,comptQH0L1,comptQH1L1,comptQH0L0,comptQH1L0);
      else {
	for (Int_t p =startPlane; p <endPlane; p++) {	  	  
	  for (Int_t s =startStrip; s <endStrip; s++) {	  
	    if (maskedStrips(p,s)) continue;
	    normAcB1H1L1LasCyc[p][s] = AccumB1H1L1[p][s] /comptQH1L1;
	    normAcB1H1L0LasCyc[p][s] = AccumB1H1L0[p][s] /comptQH1L0;;// 
	    normAcB1H0L1LasCyc[p][s] = AccumB1H0L1[p][s] /comptQH0L1;
	    normAcB1H0L0LasCyc[p][s] = AccumB1H0L0[p][s] /comptQH0L0;

	    if(debug2) {
	      printf(" normAcB1H1L1LasCyc[%d][%d]:\t%f = AccumB1H1L1:%f / comptQH1L1:%f\n"
			     ,p,s,normAcB1H1L1LasCyc[p][s],AccumB1H1L1[p][s], comptQH1L1 );
	      printf(" normAcB1H1L0LasCyc[%d][%d]:\t%f = AccumB1H1L0:%f / comptQH1L0:%f\n"
			     ,p,s,normAcB1H1L0LasCyc[p][s],AccumB1H1L0[p][s], comptQH1L0 );
	      printf(" normAcB1H0L1LasCyc[%d][%d]:\t%f = AccumB1H0L1:%f / comptQH0L1:%f\n"
			     ,p,s,normAcB1H0L1LasCyc[p][s],AccumB1H0L1[p][s], comptQH0L1 );
	      printf(" normAcB1H0L0LasCyc[%d][%d]:\t%f = AccumB1H0L0:%f / comptQH0L0:%f\n"
			     ,p,s,normAcB1H0L0LasCyc[p][s],AccumB1H0L0[p][s], comptQH0L0 );
	    }
	    //unNormLasCycSum[p][s] = AccumB1H1L1[p][s] + AccumB1H0L1[p][s];//total counts during Laser ON

	    BCnormAcB1H1L1LasCyc[p][s] = normAcB1H1L1LasCyc[p][s] - normAcB1H1L0LasCyc[p][s]; //* comptQH1L1/comptQH1L0;
	    BCnormAcB1H0L1LasCyc[p][s] = normAcB1H0L1LasCyc[p][s] - normAcB1H0L0LasCyc[p][s]; //* comptQH0L1/comptQH0L0;

	    BCnormLasCycDiff[p][s] = (BCnormAcB1H1L1LasCyc[p][s] - BCnormAcB1H0L1LasCyc[p][s]);
	    BCnormLasCycSum[p][s] = (BCnormAcB1H1L1LasCyc[p][s]  + BCnormAcB1H0L1LasCyc[p][s]);
	    if (BCnormLasCycSum[p][s]  <= 0.0) {
	      if(debug) printf("\n**Warning**:BCnormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCnormLasCycSum[p][s],nCycle);
	      if(debug) printf("note: AccumB1H1L1:%f, AccumB1H1L0:%f, AccumB1H0L1:%f, AccumB1H0L0:%f\n"
			       ,AccumB1H1L1[p][s],AccumB1H1L0[p][s],AccumB1H0L1[p][s],AccumB1H0L0[p][s]);
	      if(debug) printf("and comptQH1L1:%f, comptQH1L0:%f, comptQH0L1:%f, comptQH0L0:%f",comptQH1L1,comptQH1L0,comptQH0L1,comptQH0L0);
	    }
	    else {
	      qNormLasCycAsym[p][s] = (BCnormLasCycDiff[p][s] / BCnormLasCycSum[p][s]);
	      
	      ///Evaluation of error on asymmetry
	      errB1H1L1[p][s]=((1-qNormLasCycAsym[p][s])/(comptQH1L1*BCnormLasCycSum[p][s]))*sqrt(AccumB1H1L1[p][s]);
	      errB1H0L1[p][s]=((1+qNormLasCycAsym[p][s])/(comptQH0L1*BCnormLasCycSum[p][s]))*sqrt(AccumB1H0L1[p][s]);
	      errB1H1L0[p][s]=((1-qNormLasCycAsym[p][s])/(comptQH1L0*BCnormLasCycSum[p][s]))*sqrt(AccumB1H1L0[p][s]);
	      errB1H0L0[p][s]=((1+qNormLasCycAsym[p][s])/(comptQH0L0*BCnormLasCycSum[p][s]))*sqrt(AccumB1H0L0[p][s]);

	      LasCycAsymEr[p][s] = sqrt(pow(errB1H1L1[p][s],2)+pow(errB1H0L1[p][s],2)+pow(errB1H1L0[p][s],2)+pow(errB1H0L0[p][s],2));
	      hAsymPS[p][s].Fill(qNormLasCycAsym[p][s]);
	      hAsymErPS[p][s].Fill(LasCycAsymEr[p][s]);	    
	    }
	    //LasCycAsymEr[p][s]  = 1.0/sqrt(unNormLasCycSum[p][s]);

	    if (debug1) {
	      printf("for nCycle:%d, qNormLasCycAsym[p%d][s%d]= %f (stat.err:%f)\n",nCycle,p,s,qNormLasCycAsym[p][s] ,LasCycAsymEr[p][s]);
	      printf("formed by normalized BC (%f -/+ %f) \n",normAcB1H1L1LasCyc[p][s],normAcB1H0L1LasCyc[p][s]);
	    }
	  }
	}///for (Int_t p =startPlane; p <endPlane; p++) {
      }
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip, hence skipping"<<endl;
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
  
  TCanvas *cmystrAsym = new TCanvas("cmystrAsym","Asymmetry in strip",10,10,1500,1100);
  cmystrAsym->Divide(2,4);
  Int_t n = 0;
  for(Int_t s =mystr; s >(mystr-4); s--) {
    cmystrAsym->cd(1+2*n);
    h1[s] = (TH1D*)hAsymPS[0][s].Clone();    
    //h1[s]->Draw();
    h1[s]->Fit("gaus");//the fit function automatically draws it too

    cmystrAsym->cd(2+2*n);
    h2[s] = (TH1D*)hAsymErPS[0][s].Clone(); //,"","goff");
    h2[s]->Draw();
    cmystrAsym->Update();
    n++;
  }
  cmystrAsym->SaveAs(Form("AsymStrip%d.png",mystr));
  
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (maskedStrips(p,s)) continue;
      stripAsym[p][s] = hAsymPS[p][s].GetMean();
      stripAsymRMS[p][s] = hAsymPS[p][s].GetRMS();      
      stripAsymEr[p][s] = ( hAsymErPS[p][s].GetMean() ) / sqrt(goodCycles);/// 1/sqrt(N)
    }
  }

  for (Int_t runnum = runnum1; runnum <= runnum2; runnum++) {
    for(Int_t p = startPlane; p < endPlane; p++) {
      outfileExpAsymP.open(Form("r%d_expAsymP%d.txt",runnum,p+1));
      //outfileExpAsymP<<"strip\texpAsym\tasymEr\tasymRMS"<<endl; ///If I want a header for the following text
      cout<<Form("r%d_expAsymP%d.txt",runnum,p+1)<<" file created"<<endl;
      for (Int_t s =startStrip; s <endStrip;s++) {    
	if (maskedStrips(p,s)) continue;
	//	outfileExpAsymP<<s+1<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<"\t"<<stripAsymRMS[p][s]<<endl;
	outfileExpAsymP<<Form("%d\t%f\t%f\t%f\n",s+1,stripAsym[p][s],stripAsymEr[p][s],stripAsymRMS[p][s]);
      }
      outfileExpAsymP.close();
      cout<<Form("r%d_expAsymP%d.txt",runnum,p+1)<< " filled and closed"<<endl;
    }
  }
  
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate edetExpAsym.\n",div_output.quot,div_output.rem );  
  return goodCycles;//the function returns the number of used Laser cycles
}

/******************************************************
!Querries:
* why does a repeat execution of the code causes crash of root-session
* how to properly delete the new TGraphErrors and the new TCanvas and the new TLine 
* ..created in the code
*	//!?what if the run starts with a beamTrip
******************************************************/

/******************************************************
!Further modifications:
* Adapt to handle multiple run numbers
* evaluate asymmetry per pattern and see its pattern
* ensure efficient evaluation of beamtrips
* check consistency of cut on laserPow and beamtrip
******************************************************/
