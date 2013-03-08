#include "Accum.h"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TROOT.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TCint.h>
#include <TSystem.h>
#include <TChain.h>
#include <TGaxis.h>
#include <TLeaf.h>

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
  TString baseDir = TString("$QW_ROOTFILES/");
  TString fileName = baseDir + Form("Compton_Pass2b_%d.???.root", runNum);
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
    fileName = baseDir + Form("Compton_Pass2b_%d.%03d.root", runNum, fileNum);
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




//////////////////////////////////////////////////////////////////////
//This program returns the abscissa of the righttmost populated bin.//
//////////////////////////////////////////////////////////////////////
Float_t getRightmost(TH1F * hTemp0){
  Float_t maxBinEdge = 0; 
  for(Int_t i=0;i<hTemp0->GetNbinsX();i++){
    if(hTemp0->GetBinContent(i)!= 0)
      maxBinEdge = hTemp0->GetBinLowEdge(i+1);
  }
  return(maxBinEdge);
}





////////////////////////////////////////////////////////////////////
//This program returns the abscissa of the leftmost populated bin.//
////////////////////////////////////////////////////////////////////
Float_t getLeftmost(TH1F * hTemp0){
  Float_t minBinEdge = 0; 
  for(Int_t i=hTemp0->GetNbinsX(); i>0; i--){
    if(hTemp0->GetBinContent(i-1)!= 0)
      minBinEdge = hTemp0->GetBinLowEdge(i-1);
  }
  return(minBinEdge);
}




using namespace std;


void Accum::init( ComptonSession* session ) {
  VComptonMacro::init(session);
  VComptonMacro::SetName("Accum");
}

////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the results.   //
//It also stores the results of each laser wise asymmetry and differences //
//in a file called "polstats.txt".                                        //
//If "clear" is nonzero, the canvases will be deleted at the end.         //
//The inputs are run number and either 0 or 1 for whether or not the file //
//is a first100K file. The last argument is also either 
//instructs the program whether or not to delete everything upon closing.///
///////////////////////////////////////////////////////////////////////////

void Accum::run()
{

  gROOT->SetStyle("Plain");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetFrameFillColor(-1);
  gStyle->SetHistFillColor(0);
  gStyle->SetHistFillStyle(0);
  gStyle->SetTitleFillColor(-1);
  gStyle->SetFillColor(-1);
  gStyle->SetTitleStyle(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetTitleBorderSize(0);
  gROOT->ForceStyle();

  // Create a chain
  Bool_t drawForDebug = kFALSE;//draw graphs to show results of electron beam and laser cuts
  TChain *helChain = chain(1);
  if(helChain == 0)
  {
    printf("Exiting program.\n");
    return;
  }
  TChain *mpsChain = chain(2);
  
  const Double_t LOW_LSR_LMT = 3000.0; // laser unlocked below this
  const Double_t MINCUR = 2.0; // bcm6 values
  const Int_t MIN_ENTRIES = 2000;//min # of entries to use a laserwise cycle
  const Int_t NCUTS = 500;//initialized size of TCut arrays
  const Double_t LSCALE = 0.7;//used to scale laser power on yield graph.
  const char* BCM = "yield_sca_bcm6";


  //TString www = Form("/u/group/hallc/www/hallcweb/html/compton/photonsummary"
  //		     "/run2/pass1/run_%d/",runnum());
  //TString www = Form("/home/cornejo/public_html/pass2_test/run_%d",runnum());
  //TString www = TString(getenv("QWSCRATCH")) + Form("/www/run_%d/",runnum());
  //gSystem->mkdir(www,true);
  //gSystem->mkdir(www,true);
  //TString canvas1 = www + "accum0_plots.png";
  //TString canvas2 = www + "accum0_asymm.png";
  //TString canvas3 = www + "accum0_yieldvspatnum.png";
  //TString canvas4;
  Bool_t beamTripInLasCycle = kFALSE;


  // File for storing stats.
  //TString stats =  www + "accum0_stats.txt";
  //FILE *polstats = fopen(stats.Data(),"w");

 
  //////////////////////////////////////////////////////////
  //***Find entries where laser is off and beam is off.***//
  //***These are returned as vectors by "getCuts.C"    ***//
  //////////////////////////////////////////////////////////
 
  std::vector<Int_t> cutLas;//vector for laser power cut
  std::vector<Int_t> cutEB;//vector for electron beam trip cut
  Int_t startEntry, endEntry, nCuts, nLasCycles; 
  Int_t nBeamTrips = getCuts(cutLas,cutEB, runnum(), LOW_LSR_LMT);
  if(helChain->GetEntries()==0){
    printf("No entries in chain. Exiting program.\n");
    return;
  }


  helChain->ResetBranchAddresses();//resets branch address
  nLasCycles = nBeamTrips%1000;//first two digits of return value of getCuts
  nBeamTrips = (nBeamTrips -  nLasCycles)/1000;//thousands place of return value of getCuts
  nCuts = cutLas.size();//number of entries in laser cycles vector
  printf("nLasCycles=%d,   nBeamTrips=%d,   nCuts=%d\n",nLasCycles, nBeamTrips, nCuts);

 
  if(nCuts>NCUTS-2){//Stop execution if # of cuts exceeds initialized array.
    printf("Number of cuts exceeds program limits.");
    printf("Need to increase NCUTS value in analyzeAccum0.C.\n");
    cutLas.vector::~vector();
    cutEB.vector::~vector();
    return;
  }


  helChain->Draw("yield_sca_laser_PowT>>hL(100,0,2.2e5)","","goff");
  TH1F *hL = (TH1F*)gDirectory->Get("hL");
  Double_t hBinX = hL->GetBinLowEdge(hL->FindLastBinAbove(100)+1);
  Double_t lBinX = hL->GetBinLowEdge(hL->FindFirstBinAbove(100));
  Double_t diffX = hBinX - lBinX;
  std::cout<<"hBinX="<<hBinX<<"   "<<"lBinX="<<lBinX<<std::endl;

  delete hL;
  helChain->Draw(Form("yield_sca_laser_PowT>>hTemp(100,%f,%f)",lBinX,hBinX),
		 Form("yield_sca_laser_PowT>%f",0.2 * hBinX),"goff");
  TH1D *hTemp = (TH1D*)gDirectory->Get("hTemp");
  Double_t cutval1 = hTemp->GetMean()*0.9;
  Double_t cutval2 = hTemp->GetMean()*2.0;
  std::cout << "Laser power cuts: " << cutval1 << "\t" << cutval2
            << "  \t" << hTemp->GetMean() << std::endl;
  delete hTemp;
 
  Double_t yldAbvBkgd[NCUTS], yldAbvBkgdErr[NCUTS];
  Double_t asymm[NCUTS],asymmErr[NCUTS];
  Double_t diffOn[NCUTS], diffOnRMS[NCUTS], diffOnErr[NCUTS];
  Double_t diffOff[NCUTS], diffOffRMS[NCUTS], diffOffErr[NCUTS];
  helChain->Draw("yield_sca_bcm6.value>>hB(500)", "yield_sca_bcm6.value<200","goff");
  TH1F *hB = (TH1F*)gDirectory->Get("hB");
  Double_t maxCur = hB->GetBinLowEdge(hB->FindLastBinAbove(1000)+1);
  Double_t minCur = hB->GetBinLowEdge(hB->FindFirstBinAbove(50)-1);
  delete hB;

  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>hTemp(100)","","goff");
  hTemp = (TH1D*)gDirectory->Get("hTemp");
  Double_t maxYield = hTemp->GetBinLowEdge(hTemp->FindLastBinAbove(100)+1);
  std::cout<<"maxCur="<<maxCur<<std::endl;
  std::cout<<"maxYield="<<maxYield<<std::endl;
  Double_t cutCur = maxCur * 0.9;
  std::cout << "BCM current cut is " << cutCur << " uA." << std::endl;
  delete hTemp;

  //////////////////////////////////
  //***Define the desired cuts.***//
  //////////////////////////////////

  TCut bcmCut;
  if(nBeamTrips != 0)
  {
    TString bcmCutStr =  Form("(!(Entry$>%i&&Entry$<%i))",cutEB.at(0),cutEB.at(1));
   
    for(Int_t i=1; i<nBeamTrips;i++)
    {
      bcmCutStr += Form("&&(!(Entry$>%i&&Entry$<%i))",cutEB.at(2*i),cutEB.at(2*i+1));
    }
    bcmCut = TCut(bcmCutStr);
  }
  bcmCut += Form("yield_sca_bcm6<%f", 200.0);
  std::cout<<"Beam off cut is: "<<std::endl;
  bcmCut.Print();


  TCut beamOffCut = Form("%s<=%f", BCM, MINCUR);
  TCut pmtCut = "yield_sca_laser_photon.value > 200";//PMT On
                                                    

  TCut cut1, cut2, cut3, cut4, cutTemp, cutTemp1, cutTemp2, cutTemp3;
  //  TCut bModCut = "yield_rate0x118<10";

  //laser and PMT on
  TCut lasOnCut = pmtCut && Form("yield_sca_laser_PowT.value>%f",cutval1)
                            && Form("yield_sca_laser_PowT.value<%f",cutval2);
  //laser and electron beam 
  TCut lasONCut = bcmCut && Form("yield_sca_laser_PowT.value>%f",cutval1)
                         && Form("yield_sca_laser_PowT.value<%f",cutval2);
  TCut lasOnCutL = Form("yield_sca_laser_PowT.value>%f",cutval1);
  TCut lasOnCutH = Form("yield_sca_laser_PowT.value<%f",cutval2);
  //unused data between laser on and off
  TCut unusedCut = Form("yield_sca_laser_PowT.value<=%f", cutval1);
  unusedCut +=  Form("yield_sca_laser_PowT.value>=%f", LOW_LSR_LMT/2.0);
  unusedCut  = unusedCut || Form("yield_sca_laser_PowT.value>=%f", cutval2); 

  //If flipper mirror not working flashing occurs even when cavity unlocked. These flashes 
  //make it more difficult to identify laser off times. Therefore, the LOW_LSR_LMT has been 
  //set high to allow for these times. However, only data below one half of this value is
  //accepted as good unlocked data for determining background.

  TCut lasOffCut =  pmtCut && //laser unlocked and PMT on
                    Form("yield_sca_laser_PowT.value<=%f",LOW_LSR_LMT /2.0);
  TCut lasOffCut2 = Form("yield_sca_laser_PowT.value<=%f",LOW_LSR_LMT/2.0 );//all unlocked times
  TCut lasOFFCut = "";//only laser OFF periods i.e. when flipper is up
  std::vector<TCut> lasWiseCut, bkgCut, asymCut;
  std::vector<Double_t> bkgMean, bkgWidth, bkgErr;

  TCanvas *ctemp = new TCanvas("ctemp","ctemp",0,0,700,500);

  for(Int_t i=0; i<nCuts;i++){//print out the cuts to make sure this program
    printf("cutLas.at[%d]=%d\n",i, cutLas.at(i));//has access to them.
  }

  Int_t btCnt = 0;//counter for beam trip array indices
  for(Int_t i=0; i*2<nCuts-2; i++)
  {
    Bool_t enoughBkgData = kFALSE, enoughLasOnData = kFALSE;
    std::cout<<"i="<<i<<std::endl;

    cut1 = (Form("Entry$>%d",cutLas.at(2*i)));
    cut2 = (Form("Entry$<%d",cutLas.at(2*i+1)));
    cut3 = (Form("Entry$>%d",cutLas.at(2*i+2)));
    cut4 = (Form("Entry$<%d",cutLas.at(2*i+3))); 

    //Beam power cut has been implemented as a cut in time. Here is it is combined with 
    //the laser cycle cuts in time. The combined cuts must work for the cases where 
    //1. there are multiple beam cuts in one laser cycle off or on period 
    //2. there is a laser change from on to off or vice versa during a beam trip
    //3. a beam trip period overlaps one or more entire laser off or on periods
    //4. there are no beam trips

    //Background cut for the i'th laser cycle consists of the Off periods surrounding
    //the i'th On period. The background cuts are stored in cutTemp1 and cutTemp3.
    //The laser On cuts are stored in cutTemp2. For the first On cycle find the background
    //cuts for both Off periods, but after that just shift the value previously found as 
    //cutTemp3 to cutTemp1.
    

    //1a). Deal with first laser Off cycle
    if(i == 0){

      if(nBeamTrips != 0){
	//in case first beam trip over before first good laser cycle starts
	if(cutEB.at(2*btCnt+1)<cutLas.at(0) && btCnt<nBeamTrips-1)btCnt++;
      }

      startEntry = cutLas.at(2*i);
      endEntry = cutLas.at(2*i+1);//set endEntry to end of On cycle and only change
                                  //endEntry value if there are intervening beam trip(s)
      cutTemp1 = Form("Entry$>%d&&Entry$<%d", startEntry, endEntry);
      cutTemp1.Print();

      //To be a problematic beam trip it must fit into one of the following categories:
      //-- start before and end after  the current cycle
      //-- start during and end after  the current cycle
      //-- start before and end during the current cycle
      //-- start during and end during the current cycle
      if(nBeamTrips != 0){
	beamTripInLasCycle = 
	  (cutEB.at(2*btCnt) <= cutLas.at(2*i) && cutEB.at(2*btCnt+1) >= cutLas.at(2*i+1)) ||
	  (cutEB.at(2*btCnt) >= cutLas.at(2*i) && cutEB.at(2*btCnt) <= cutLas.at(2*i+1) && 
	   cutEB.at(2*btCnt+1) >= cutLas.at(2*i+1)) ||
	  (cutEB.at(2*btCnt) <= cutLas.at(2*i) &&  cutEB.at(2*btCnt+1) >= cutLas.at(2*i) && 
	   cutEB.at(2*btCnt+1) <= cutLas.at(2*i+1)) ||
	  (cutEB.at(2*btCnt) >= cutLas.at(2*i) && cutEB.at(2*btCnt) <= cutLas.at(2*i+1)) &&  
	  (cutEB.at(2*btCnt+1) >= cutLas.at(2*i) && cutEB.at(2*btCnt+1) <= cutLas.at(2*i+1));
      }else beamTripInLasCycle = kFALSE;

      while(beamTripInLasCycle)
      {
	startEntry = max(cutEB.at(2*btCnt), cutLas.at(2*i));
	endEntry =  min(cutEB.at(2*btCnt+1), cutLas.at(2*i+1));
	cutTemp1 = cutTemp1 && Form("!(Entry$>%d&&Entry$<%d)",startEntry, endEntry);

	if(cutEB.at(2*btCnt+1) < cutLas.at(2*i+1))//if end of beam trip in this las cycle
	{
	  if(btCnt != nBeamTrips-1)
	  {
	    btCnt++;//advance to next beam trip
	  }
	  else beamTripInLasCycle = kFALSE;//otherwise at end of last beam trip

	  if(cutEB.at(2*btCnt)> cutLas.at(2*i+1)) beamTripInLasCycle = kFALSE;
	}
	else 
	{
	  beamTripInLasCycle = kFALSE;
	}
      }
    }
    else
    {
      cutTemp1 = cutTemp3;
    }
    printf("btCnt=%d\n", btCnt);
    std::cout<<"cutTemp1="<<cutTemp1<<"\n"<<std::endl;	
    //1b). Shift previous cutTemp3 to current cutTemp1 
    //     i.e. deal with laser Off period before current laser On period


    //2). Deal with laser On period
    startEntry = cutLas.at(2*i+1);
    endEntry = cutLas.at(2*i+2);//set endEntry to end of On cycle and only change
                                //endEntry value if there are intervening beam trip(s)
    cutTemp2 = Form("Entry$>%d&&Entry$<%d", startEntry, endEntry);

    //To be a problematic beam trip it must fit into one of the following categories:
    //-- start before and end after  the current cycle
    //-- start during and end after  the current cycle
    //-- start before and end during the current cycle
    //-- start during and end during the current cycle
    if(nBeamTrips != 0){
      beamTripInLasCycle = 
	(cutEB.at(2*btCnt) <= cutLas.at(2*i+1) && cutEB.at(2*btCnt+1) >= cutLas.at(2*i+2)) ||
	(cutEB.at(2*btCnt) >= cutLas.at(2*i+1) && cutEB.at(2*btCnt) <= cutLas.at(2*i+2) && 
	 cutEB.at(2*btCnt+1) >= cutLas.at(2*i+2)) ||
	(cutEB.at(2*btCnt) <= cutLas.at(2*i+1) &&  cutEB.at(2*btCnt+1) >= cutLas.at(2*i+1) && 
	 cutEB.at(2*btCnt+1) <= cutLas.at(2*i+2)) ||
	(cutEB.at(2*btCnt) >= cutLas.at(2*i+1) && cutEB.at(2*btCnt) <= cutLas.at(2*i+2)) &&  
	(cutEB.at(2*btCnt+1) >= cutLas.at(2*i+1) && cutEB.at(2*btCnt+1) <= cutLas.at(2*i+2));
    }
    else beamTripInLasCycle = kFALSE;

    while(beamTripInLasCycle)
    {
      startEntry = max(cutEB.at(2*btCnt), cutLas.at(2*i+1));
      endEntry =  min(cutEB.at(2*btCnt+1), cutLas.at(2*i+2));
      cutTemp2 = cutTemp2 && Form("!(Entry$>%d&&Entry$<%d)",startEntry, endEntry);
      
      if(cutEB.at(2*btCnt+1)<cutLas.at(2*i+2))//if end of current beam trip in this las cycle
      {
	if(btCnt != nBeamTrips-1)
	{
	  btCnt++;//advance to next beam trip
	}
	else beamTripInLasCycle = kFALSE;//otherwise at end of last beam trip

	if(cutEB.at(2*btCnt)> cutLas.at(2*i+2)) beamTripInLasCycle = kFALSE;
      }
      else 
      {
	beamTripInLasCycle = kFALSE;
      }
    }
    printf("btCnt=%d\n", btCnt);
   
    
    //3). Deal with laser Off period after current On period
    startEntry = cutLas.at(2*i+2);
    endEntry = cutLas.at(2*i+3);//set endEntry to end of On cycle and only change
                                //endEntry value if there are intervening beam trip(s)
    cutTemp3 = Form("Entry$>%d&&Entry$<%d", startEntry, endEntry);


    //To be a problematic beam trip it must fit into one of the following categories:
    //-- start before and end after  the current cycle
    //-- start during and end after  the current cycle
    //-- start before and end during the current cycle
    //-- start during and end during the current cycle
    if(nBeamTrips != 0){
      beamTripInLasCycle = 
	(cutEB.at(2*btCnt) <= cutLas.at(2*i+2) && cutEB.at(2*btCnt+1) >= cutLas.at(2*i+3)) ||
	(cutEB.at(2*btCnt) >= cutLas.at(2*i+2) && cutEB.at(2*btCnt) <= cutLas.at(2*i+3) && 
	 cutEB.at(2*btCnt+1) >= cutLas.at(2*i+3)) ||
	(cutEB.at(2*btCnt) <= cutLas.at(2*i+2) &&  cutEB.at(2*btCnt+1) >= cutLas.at(2*i+2) && 
	 cutEB.at(2*btCnt+1) <= cutLas.at(2*i+3)) ||
	(cutEB.at(2*btCnt) >= cutLas.at(2*i+2) && cutEB.at(2*btCnt) <= cutLas.at(2*i+3)) &&  
	(cutEB.at(2*btCnt+1) >= cutLas.at(2*i+2) && cutEB.at(2*btCnt+1) <= cutLas.at(2*i+3));
    } 
    else beamTripInLasCycle = kFALSE;
   
    while(beamTripInLasCycle)
    {
      startEntry = max(cutEB.at(2*btCnt), cutLas.at(2*i+2));
      endEntry =  min(cutEB.at(2*btCnt+1), cutLas.at(2*i+3));
      cutTemp3 = cutTemp3 && Form("!(Entry$>%d&&Entry$<%d)",startEntry, endEntry);
      
      if(cutEB.at(2*btCnt+1) < cutLas.at(2*i+3))//if end of current beam trip in this las cycle
      {
	if(btCnt != nBeamTrips-1)
	{
	  btCnt++;//advance to next beam trip
	}
	else beamTripInLasCycle = kFALSE;//otherwise at end of last beam trip

	if(cutEB.at(2*btCnt)> cutLas.at(2*i+3)) beamTripInLasCycle = kFALSE;
      }
      else 
      {
	beamTripInLasCycle = kFALSE;
      }
    }
    cutTemp = cutTemp1 || cutTemp3;
    lasWiseCut.push_back(pmtCut && cut1 && cut4);
    bkgCut.push_back(lasOffCut && cutTemp);//laser unlocked and PMT on and e-beam On
    std::cout<<"bkgCut="<<bkgCut[i]<<"\n"<<std::endl;
    std::cout<<"cutTemp="<<cutTemp<<"\n"<<std::endl;
    asymCut.push_back(lasOnCut && cutTemp2);//laser locked and PMT on and e-beam On
    std::cout<<"asymCut="<<asymCut[i]<<"\n"<<std::endl;
    helChain->Draw("yield_fadc_compton_accum0.hw_sum>>h(100)",bkgCut[i],"goff");
    // gPad->Update();
    TH1D *h = (TH1D*)gDirectory->Get("h");
    Double_t mean = h->GetMean(), width = h->GetRMS(), nEnt = h->GetEntries(), err;
    enoughBkgData = nEnt > MIN_ENTRIES;
    if(enoughBkgData)
    {
      err = width / sqrt((Double_t)nEnt);
    }
    else 
    {
      mean = -9999;
      width = -9999;
      err = -9999;
    }
    delete h;
    bkgMean.push_back(mean);
    bkgWidth.push_back(width);
    bkgErr.push_back(err);
    std::cout << "bkgMean["  << i << "]=" << bkgMean[i]  << ",  "
	      << "bkgWidth[" << i << "]=" << bkgWidth[i] <<",  "
	      << "bkgErr[" << i << "]=" <<bkgErr[i]<<",  "
	      << "nEntries=" << nEnt << std::endl;
    helChain->Draw("yield_fadc_compton_accum0.hw_sum>>ha(100)", asymCut[i],"goff");
    TH1D *ha = (TH1D*)gDirectory->Get("ha");
    nEnt = (Double_t)ha->GetEntries();
    enoughLasOnData = nEnt > MIN_ENTRIES;
    if(enoughLasOnData && enoughBkgData){
      yldAbvBkgd[i] = ha->GetMean() - bkgMean[i];
      yldAbvBkgdErr[i] = sqrt(pow(bkgErr[i],2) + //add errors of bkgd and yld
			      pow(ha->GetRMS(),2) / nEnt);
      std::cout<<"yldAbvBkgd["<<i<<"]="<<yldAbvBkgd[i]<<" (+/-) "
	  <<yldAbvBkgdErr[i]<<", n_entries="<<nEnt<<std::endl;
    }else{
      yldAbvBkgd[i] = -9999;
      yldAbvBkgdErr[i] = -9999;
    }
    if(i==0)
      lasOFFCut = (cut1 && cut2) || (cut3 && cut4);
    else 
      lasOFFCut = lasOFFCut ||(cut3 && cut4);
    if(2*i==nCuts-3) 
      delete ha;
    //draw laser cycle data for debugging
    TH2D *hBkgd, *hLocked, *hLasPow, *hBeamCur;
    if(drawForDebug)
    {
      Double_t hbin = (Double_t)cutLas.at(2*i+3) + 1000.0;
      Double_t lbin = (Double_t)cutLas.at(2*i) - 1000.0;
      helChain->Draw(Form("yield_fadc_compton_accum0.hw_sum*%e:Entry$>>hBkgd(300,%e,%e,"
			  "300,-5,100)",90./maxYield, lbin, hbin), bkgCut.at(i), "goff");
      hBkgd=(TH2D*)gDirectory->Get("hBkgd");
      hBkgd->SetMarkerColor(kBlue);
      hBkgd->SetLineColor(kBlue);
      helChain->Draw(Form("yield_fadc_compton_accum0.hw_sum*%e:Entry$>>hLocked(300,%e,%e,"
			  "300,-5,100)",90./maxYield,lbin,hbin), asymCut.at(i) , "goff");
      hLocked=(TH2D*)gDirectory->Get("hLocked");
      hLocked->SetMarkerColor(kRed);
      hLocked->SetLineColor(kRed);
      hBkgd->Draw();
      hLocked->Draw("same");
      helChain->Draw(Form("yield_sca_laser_PowT*%e:Entry$>>hLasPow(300,%e,%e,"
			  "300,-5,100)",50./cutval1, lbin, hbin),"","goff");
      hLasPow=(TH2D*)gDirectory->Get("hLasPow");
      hLasPow->SetMarkerColor(kGreen);
      hLasPow->SetLineColor(kGreen);
      hLasPow->Draw("same");
      helChain->Draw(Form("%s*%e:Entry$>>hBeamCur(300,,,300)", BCM, 80.0/maxCur),
		     "","same");
      hBeamCur = (TH2D*)gDirectory->Get("hbeamCur");
      TLegend *leg = new TLegend(0.86,0.6,0.98,0.75);
      //legend1->SetHeader("Some histograms");
      leg->AddEntry(hBkgd,"Background","l");
      leg->AddEntry(hLocked,"Locked Data","l");
      leg->AddEntry(hLasPow,"Laser Power","l");
      leg->AddEntry(hBeamCur,"Beam Curent","l");
      leg->SetFillColor(0);
      leg->SetShadowColor(0);
      leg->Draw();
      //canvas4 = Form("~/users/jonesdc/compton_runs/misc/lasCycle%i.png", i);
      // (jc2): Disabled this printout for now. I'm sure it must have
      // been only a testing thing
      //ctemp->Print(canvas4);
      // (jc2): Done with hLocked?
      delete hLocked;
    }
    printf("btCnt=%d\n", btCnt);

  }
  delete ctemp;

  //  lasOFFCut = lasOFFCut && bcmCut && pmtCut;//laser off and PMT on and e-beam on
 

  ////////////////////////////////////
  //***Create canvases for plots.***//
  ////////////////////////////////////
  TCanvas *c1 = new TCanvas("c1","c1",0,0,1000,950);
  c1->SetFillColor(0);
  c1->Divide(2,3);
  TCanvas *c3 = new TCanvas("c3","c3",0,0,950,750);
  c3->Divide(2,2);
  c1->cd(1);


  ////////////////////////////////////////
  //***Draw Current vs Pattern Number***//
  ////////////////////////////////////////
  hBinX = helChain->GetMaximum("pattern_number");
  lBinX = helChain->GetMinimum("pattern_number");
  diffX = hBinX - lBinX;
  hBinX += diffX*0.1;
  lBinX -= diffX*0.1;
  Double_t hBinY, lBinY, diffY;
  diffY = maxCur - minCur;
  hBinY = maxCur + 0.1 * diffY;
  lBinY = minCur - 0.1 * diffY; 

  TH2F *hbcm = new TH2F("hbcm", "hbcm",300,lBinX,hBinX,300,lBinY,hBinY);
  hbcm->GetXaxis()->SetTitle("Pattern Number");
  hbcm->GetYaxis()->SetTitle("BCM Yield");
  hbcm->SetMarkerColor(kGray+2);
  hbcm->SetLineColor(kGray+2);
  hbcm->SetTitle("BCM Yield vs. Pattern Number");
  TH2F *hbcmOn = new TH2F("hbcmOn", "hbcmOn",300,lBinX,hBinX,300,lBinY,hBinY);
  hbcmOn->SetMarkerColor(kRed);
  hbcmOn->SetLineColor(kRed);
  helChain->Draw(Form("%s:pattern_number>>hbcm", BCM));
  helChain->Draw(Form("%s:pattern_number>>hbcmOn", BCM),
		 bcmCut,"sames");
  gPad->Update();

  if(hbcm->GetEntries()==0){
    std::cout << "No useful events in run " << runnum() << "." << std::endl;
    delete hbcm;
    delete hbcmOn;
    delete c1;
    return;
  }

 
  ////////////////////////////////////////////////////////
  //***Draw accumulator 0 yield with different colors***//
  //***for Laser On and Laser Off states.            ***//
  ////////////////////////////////////////////////////////
  c1->cd(2);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>h(100)","","goff");
  TH1F *h = (TH1F*)gDirectory->Get("h");
  if(h->GetEntries()==0){
    printf("\n\nNo useful events in Run %d.\n\n\n",runnum());
    //hbcm->TH2F::~TH2F();
    delete hbcm;
    //h->TH1F::~TH1F();
    delete h;
    //c1->TCanvas::Destructor();
    delete c1;
    cutLas.clear();
    cutLas.clear();
    return;
  }

  hBinY = getRightmost(h);
  lBinY = getLeftmost(h);
  diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  
  Double_t hBinYtmp = h->GetMean()+3*h->GetRMS();
  if(hBinY>hBinYtmp){
    hBinY = hBinYtmp;
  }
  Double_t lBinYtmp = h->GetMean()-3*h->GetRMS();
  if(lBinY<lBinYtmp){
    lBinY = lBinYtmp;
  }
  
  delete h;

  TH1F *hYieldOn = new TH1F("hYieldOn", "hYieldOn", 300, lBinY, hBinY);
  TH1F *hYieldOff = new TH1F("hYieldOff", "hYieldOff", 300, lBinY, hBinY);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>hYieldOn",
		   lasONCut && pmtCut,"goff");
  hYieldOn->SetLineColor(kGreen);
  hYieldOn->GetXaxis()->SetTitle("Accumulator_0 Yields");
  hYieldOff->GetXaxis()->SetTitle("Accumulator_0 Yields");
  hYieldOn->SetTitle("Accumulator_0 Yields");
  hYieldOff->SetTitle("Accumulator_0 Yields");
  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>hYieldOff",
		 lasOFFCut && pmtCut && bcmCut,"goff");
  hYieldOff->SetLineColor(kBlue);
  if(hYieldOn->GetMaximum()>hYieldOff->GetMaximum()){
    gStyle->SetOptStat(1111);
    hYieldOn->Draw();
    gPad->Update();

    TPaveStats *stOn = (TPaveStats*)gPad->GetPrimitive("stats");
    stOn->SetY1NDC(0.67); //new y start position
    stOn->SetY2NDC(0.84); //new y end position
    hYieldOff->Draw("sames");
    gPad->Update();
    }else{
    gStyle->SetOptStat(1111);
    hYieldOff->Draw();
    gPad->Update();
    TPaveStats *stOff = (TPaveStats*)gPad->GetPrimitive("stats");
    stOff->SetY1NDC(0.67); //new y start position
    stOff->SetY2NDC(0.84); //new y end position
    hYieldOn->Draw("sames");
    gPad->Update();
  }

  TLegend *legend1 = new TLegend(0.86,0.55,0.98,0.67);
  //legend1->SetHeader("Some histograms");
  legend1->AddEntry(hYieldOn,"Laser On","l");
  legend1->AddEntry(hYieldOff,"Laser Off","l");
  legend1->SetFillColor(0);
  legend1->SetShadowColor(-1);
  legend1->Draw();
  gPad->Update();
  
  /////////////////////////////////////////////////////////
  //***Draw accumulator 0 yield vs pattern number with***//
  //***different colors for laser On and Off states.  ***//
  //***Superimpose scaled laser power on plot as well.***//
  ////////////////////////////////////////////////////////
  c1->cd(3);
  gStyle->SetOptStat(111);
  helChain->Draw("yield_sca_laser_PowT>>hPowT(100)");
  TH1F *hPowT = (TH1F*)gDirectory->Get("hPowT");


  Double_t maxPowT = hPowT->GetBinLowEdge(hPowT->FindLastBinAbove(100)+1);
  delete hPowT;

  //std::cout<<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<std::endl;
  diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  //  std::cout<<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<std::endl;
  Float_t scale = LSCALE*(Float_t)(hBinY-lBinY)/(Float_t)maxPowT;
  // printf("maxPowT=%f\nscale=%f\n",maxPowT,scale);
  TH2F *hPow = new TH2F("hPow","hPow", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  //data when cavity is neither locked nor unlocked is unused
  TH2F *hPUnsd = new TH2F("hPUnsd","hPUnsd",2000,lBinX,hBinX,2000,lBinY,hBinY);
  TH2F *hPLow = new TH2F("hPLow","hPLow",2000,lBinX, hBinX,2000, lBinY, hBinY);
  TH2F *hOn = new TH2F("hOn","hOn", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hOff = new TH2F("hOff","hOff", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hBeamOff = new TH2F("hBeamOff","hBeamOff", 2000, lBinX, hBinX, 
			                            2000, lBinY, hBinY);
  hPow->SetMarkerColor(kRed);
  hPow->SetLineColor(kRed);
  hPUnsd->SetMarkerColor(kRed+3);
  hPUnsd->SetLineColor(kRed+3);
  hPLow->SetMarkerColor(kRed-8);
  hPLow->SetLineColor(kRed-8);
  hOn->SetMarkerColor(kGreen);
  hOn->SetLineColor(kGreen);
  hOn->GetXaxis()->SetTitle("Pattern Number");
  hOn->GetYaxis()->SetTitle("Accumulator 0 Values");
  hOn->SetTitle("Accumulator 0 vs. Pattern Number");
  hOff->SetMarkerColor(kBlue);
  hOff->SetLineColor(kBlue);
  hPow->SetMarkerColor(kRed);
  hPow->SetLineColor(kRed);
  hPUnsd->SetMarkerColor(kRed+3);
  hPUnsd->SetLineColor(kRed+3);
  hPLow->SetMarkerColor(kRed-8);
  hPLow->SetLineColor(kRed-8);
  hBeamOff->SetMarkerColor(kGray+1);
  hBeamOff->SetLineColor(kGray+1);


  helChain->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>hOn",
		   lasONCut && pmtCut);
  gPad->Update();
  helChain->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>hBeamOff",
		 Form("%s<%f", BCM, cutCur), "same");
  helChain->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>hOff",
		 lasOFFCut && pmtCut && bcmCut,"sames");
  gPad->Update();
  TPaveStats *st1 = (TPaveStats*)gPad->GetPrimitive("stats");
  st1->SetX1NDC(0.58); //new x start position
  st1->SetX2NDC(0.78); //new x end position
  st1->Draw();
  gPad->Update();
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hPUnsd",
		      scale,lBinY+diffY*0.05),unusedCut,"same");
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hPow",
		      scale,lBinY+diffY*0.05),lasOnCutL && lasOnCutH, "same");
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hPLow",
		    scale,lBinY+diffY*0.05),lasOffCut2,"same");


  TLegend *legend2 = new TLegend(0.83,0.64,0.98,0.835);
  legend2->AddEntry(hPow,"Laser Power","l");
  legend2->AddEntry(hOff,"Yield Las Off","l");
  legend2->AddEntry(hOn,"Yield Las On","l");
  legend2->AddEntry(hBeamOff,"Yield Beam Off", "l");
  legend2->Draw();

  //Change to third canvas for plots.
  c3->cd(1);

  //Draw plot of Scalars vs pattern number.
  gStyle->SetOptStat(111);
  helChain->Draw("yield_sca_laser_photon>>h2","","goff");
  TH1F *h2 = (TH1F*)gDirectory->Get("h2"); 
  hBinY = 1.1 * h2->GetBinLowEdge(h2->FindLastBinAbove(100)+1);
  lBinY = h2->GetBinLowEdge(h2->FindFirstBinAbove(100));
  diffY = hBinY - lBinY;
  lBinY -= 0.1*diffY;
  lBinY = TMath::Max(lBinY,-10.0);
  delete h2;
  TH2F *hOn2 = new TH2F("hOn2","hOn2", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hOff2 = new TH2F("hOff2","hOff2", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hOff3 = new TH2F("hOff3","hOff3", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  hOff3->SetMarkerColor(kGray+2);
  hOff3->SetLineColor(kGray+2);
  helChain->Draw("yield_sca_laser_photon:pattern_number>>hOn2"
	         ,lasOnCutL && lasOnCutH && bcmCut,"goff");
  helChain->Draw("yield_sca_laser_photon:pattern_number>>hOff2"
	         ,lasOffCut2 && bcmCut,"goff");
  hOff2->SetMarkerColor(kBlue);
  hOff2->SetLineColor(kBlue);
  hOn2->SetMarkerColor(kGreen);
  hOn2->SetLineColor(kGreen);
  hOn2->SetTitle("Photon Detector Scalar");
  hOn2->GetXaxis()->SetTitle("Pattern Number");
  hOn2->GetYaxis()->SetTitle("Photon Detector Scalar");
  hOn2->Draw();
  gPad->Update();
  TPaveStats *st2 = (TPaveStats*)gPad->GetPrimitive("stats");
  st2->SetX1NDC(0.58); //new x start position
  st2->SetX2NDC(0.78); //new x end position
  st2->Draw();
  hOff2->Draw("sames");
  helChain->Draw("yield_sca_laser_photon:pattern_number>>hOff3",
		 Form("%s<%e", BCM, 1.05 * cutCur), "same");

  TLegend *legend3 = new TLegend(0.81,0.68,0.98,0.835);
  legend3->AddEntry(hOff2,"Laser Off","l");
  legend3->AddEntry(hOn2,"Laser On","l");
  legend3->AddEntry(hOff3,"E-beam Off","l");
  legend3->Draw();
  gPad->Update();


  ////////////////////////////////////////////////////////////////////////
  //***Draw Beam Positions for Laser On and Off using Downstream BPM.***//
  ////////////////////////////////////////////////////////////////////////

  c3->cd(2);
  gStyle->SetOptStat(1111);
  helChain->Draw("0.5*yield_sca_bpm_3p02aY+"
		 "0.5*yield_sca_bpm_3p02bY:pattern_number>>hPosAvgY(200,,,200)",
		 "", "goff");
  TH2F * hPosAvgY = (TH2F*)gDirectory->Get("hPosAvgY");
  hPosAvgY->SetMarkerColor(kRed);
  hPosAvgY->SetLineColor(kRed);
  hPosAvgY ->SetTitle("Average Compton Y Position vs Pattern Number");
  //  hPosAvgY ->GetYaxis()->SetTitle("0.5(3P02A.YPOS+3P02B.YPOS) (mm)");
  hPosAvgY ->GetXaxis()->SetTitle("Pattern Number");
  hPosAvgY->Draw();
  TPaveText *pt1 = new TPaveText(0.12,0.82,0.48,0.88, "NDC"); // NDC sets coords
                                              // relative to pad dimensions
  pt1->SetTextSize(0.048); 
  pt1->SetBorderSize(0);
  pt1->SetTextAlign(12);
  pt1->AddText("0.5(3P02A.YPOS+3P02B.YPOS) (mm)");
  pt1->SetFillColor(-1);
  pt1->SetShadowColor(-1);
  pt1->Draw();
 
  c3->cd(3);
  helChain->Draw("yield_sca_bpm_3p02bX:pattern_number>>hPosX(200,,,200)", "", "goff");
  TH2F * hPosX = (TH2F*)gDirectory->Get("hPosX");
  hPosX->SetMarkerColor(kBlue);
  hPosX->SetLineColor(kBlue);
  hPosX ->SetTitle("Downstream BPM X Position vs Pattern Number");
  //  hPosX ->GetYaxis()->SetTitle("IPM3P02B.XPOS (mm)");
  hPosX ->GetXaxis()->SetTitle("Pattern Number");
  hPosX->Draw();
  TPaveText *pt2 = new TPaveText(0.12,0.82,0.48,0.88, "NDC"); // NDC sets coords
                                              // relative to pad dimensions
  pt2->SetTextSize(0.048); 
  pt2->SetBorderSize(0);
  pt2->SetTextAlign(12);
  pt2->AddText("IPM3P02B.XPOS (mm)");
  pt2->SetFillColor(-1);
  pt2->SetShadowColor(-1);
  pt2->Draw();

  c3->cd(4);
  helChain->Draw("yield_sca_bpm_3p02bY:pattern_number>>hPosY(200,,,200)", "", "goff");
  TH2F * hPosY = (TH2F*)gDirectory->Get("hPosY");
  hPosY->SetMarkerColor(kRed);
  hPosY->SetLineColor(kRed);
  hPosY ->SetTitle("Downstream BPM Y Position vs Pattern Number");
  //  hPosY ->GetYaxis()->SetTitle("IPM3P02B.YPOS (mm)");
  hPosY ->GetXaxis()->SetTitle("Pattern Number");
  hPosY->Draw();
  TPaveText *pt3 = new TPaveText(0.12,0.82,0.48,0.88, "NDC"); // NDC sets coords
                                              // relative to pad dimensions
  pt3->SetTextSize(0.048); 
  pt3->SetBorderSize(0);
  pt3->SetTextAlign(12);
  pt3->AddText("IPM3P02B.YPOS (mm)");
  pt3->SetFillColor(-1);
  pt3->SetShadowColor(-1);
  pt3->Draw();

  gPad->Update();

  //////////////////////////////////////////////////////////////
  //***Draw Accumulator 0 Differences for Laser On and Off.***//
  //////////////////////////////////////////////////////////////
  c1->cd(4);
  gStyle->SetOptStat(111111);
  helChain->Draw("diff_fadc_compton_accum0.hw_sum*2>>h",
		 Form("yield_sca_laser_PowT>%e", cutval1),"goff");
  h = (TH1F*)gDirectory->Get("h");
  hBinX =   h->GetMean() + h->GetRMS()*3;
  lBinX = h->GetMean() - h->GetRMS()*3;
  //std::cout<<"\nhBinx="<<hBinX<<",  lBinX="<<lBinX<<"\n"<<std::endl;
  delete h;
  helChain->Draw(Form("diff_fadc_compton_accum0.hw_sum*2>>h(100,%e,%e)",
		      lBinX, hBinX), Form("yield_sca_laser_PowT>%e", 
					 cutval1),"goff");
  h = (TH1F*)gDirectory->Get("h");
  hBinX =  h->GetMean() + h->GetRMS()*3;
  lBinX =  h->GetMean() - h->GetRMS()*3;
  delete h;

  helChain->Draw(Form("diff_fadc_compton_accum0.hw_sum*2>>h(100,%e,%e)",
		      lBinX, hBinX), Form("yield_sca_laser_PowT>%e", 
					 cutval1),"goff");
  h = (TH1F*)gDirectory->Get("h");
  if((h->GetBinContent(0)+h->GetBinContent(101))>(Int_t)(0.0001*h->GetEntries()))
  {
    hBinX =  h->GetMean() + h->GetRMS()*7;
    lBinX =  h->GetMean() - h->GetRMS()*7;
    h->TH1F::~TH1F();
  }
  delete h;
  //std::cout<<"\nhBinx="<<hBinX<<",  lBinX="<<lBinX<<"\n"<<std::endl;
  TH1F *hDiffOn = new TH1F("hDiffOn","hDiffOn",300, lBinX, hBinX);
  TH1F *hDiffOff = new TH1F("hDiffOff", "hDiffOff", 300, lBinX, hBinX); 
  TH1D *hTempOn = new TH1D("hTempOn","hTempOn",300,lBinX,hBinX);
  TH1D *hTempOff = new TH1D("hTempOff", "hTempOff",300, lBinX, hBinX);
  TH1D *hTemp0 = new TH1D("hTemp0","hTemp0",300,lBinX,hBinX);
  hDiffOn->StatOverflows(kTRUE);
  hDiffOff->StatOverflows(kTRUE);
  hTempOn->StatOverflows(kTRUE);
  hTempOff->StatOverflows(kTRUE);
  hTemp0->StatOverflows(kTRUE);
  Int_t m = 0, n = 0, p = 0, numEnt = 0;
  Int_t enoughEntries[NCUTS];//cut quality flag.0 if cut not usable.1 if good.
  for(Int_t i=0; i*2<nCuts-2; i++){
    helChain->Draw("diff_fadc_compton_accum0.hw_sum*2>>hTempOn",
		   asymCut[i],"goff");
    //asymCut[i].Print();
    numEnt =(Int_t)hTempOn->GetEntries();
    if(numEnt>MIN_ENTRIES){
      enoughEntries[i] = 1;
      m++;
      std::cout<<"Cycle "<<i<<" for laser On is good."<<std::endl;
    }else{
      std::cout<<"Cycle "<<i<<" for laser On is NOT good."<<std::endl;
      enoughEntries[i] = 0;
    }
    cut1 = Form("Entry$>=%d",cutLas.at(2*i)); 
    cut2 = Form("Entry$<=%d",cutLas.at(2*i+1));
    cut3 = Form("Entry$>=%d",cutLas.at(2*i+2));
    cut4 = Form("Entry$<=%d",cutLas.at(2*i+3));
    helChain->Draw("diff_fadc_compton_accum0.hw_sum*2>>hTempOff",
		   cut1 && cut2 && bkgCut[i],"goff");
    numEnt = (Int_t)hTempOff->GetEntries();
    helChain->Draw("diff_fadc_compton_accum0.hw_sum*2>>hTempOff",
		   cut3 && cut4 && bkgCut[i],"goff");
    numEnt = TMath::Min(numEnt,(Int_t)hTempOff->GetEntries());
    if(numEnt>MIN_ENTRIES){
      n++;
      std::cout<<"Cycle "<<i<<" for laser Off is good."<<std::endl;
    }else{
      std::cout<<"Cycle "<<i<<" for laser Off is NOT good."<<std::endl;
      enoughEntries[i] = 0;
    }
    if(enoughEntries[i] == 1){
      if(i==0){//add first off cycle if it is good.
	hDiffOn->Add(hTempOn);
	diffOn[p] = hTempOn->GetMean();
	diffOnRMS[p] = hTempOn->GetRMS();
	diffOnErr[p] = diffOnRMS[p] / sqrt(hTempOn->GetEntries());
	hDiffOff->Add(hTempOff);
	hTemp0->Add(hTempOff);
	helChain->Draw("diff_fadc_compton_accum0.hw_sum*2>>hTempOff",
		       cut1 && cut2 && bkgCut[i],"goff");
	hDiffOff->Add(hTempOff);
	diffOff[p] = hTempOff->GetMean();
	diffOffRMS[p] = hTempOff->GetRMS();
	diffOffErr[p] = diffOffRMS[p] / sqrt(hTempOff->GetEntries());
	p++;
      }else{//add all the rest of the on and off cycles.
	hDiffOn->Add(hTempOn);
	diffOn[p] = hTempOn->GetMean();
	diffOnRMS[p] = hTempOn->GetRMS();
	diffOnErr[p] = diffOnRMS[p] / sqrt(hTempOn->GetEntries());
	hDiffOff->Add(hTempOff);
	hTempOff->Add(hTemp0);
	diffOff[p] = hTempOff->GetMean();
	diffOffRMS[p] = hTempOff->GetRMS();
	diffOffErr[p] = diffOffRMS[p] / sqrt(hTempOff->GetEntries());
	hTemp0->Add(hTemp0, hTempOff, -1, 1);//hTemp only holds previous off data
	p++;
      }
    }
    hTempOn->Reset();
    hTempOff->Reset();
   }
  std::cout<<m<<" good cycles for laser On."<<std::endl;
  std::cout<<n<<" good cycles for laser Off."<<std::endl;
  std::cout<<"On && Off = "<<p<<" good cycles in total."<<std::endl;
  hDiffOn->GetXaxis()->SetTitle("Accumulator 0 Differences");
  hDiffOn->SetTitle("Laser On Accumulator 0 Differences (x2)");
  hDiffOn->SetLineColor(kGreen);
  hDiffOn->Draw();
  gPad->Update();
  c1->cd(5);
  hDiffOff->GetXaxis()->SetTitle("Accumulator 0 Differences");
  hDiffOff->SetTitle("Laser Off Accumulator 0 Differences (x2)");
  hDiffOff->SetLineColor(kBlue);
  hDiffOff->Draw();
  gPad->Update();

  ////////////////////////////////////
  //***Draw Accumulator Asymmetry***//
  ////////////////////////////////////

  c1->cd(6);
  gStyle->SetOptStat(11111);
  TH1D *hAsymm = new TH1D("hAsymm","hAsymm",300,-3,3);
  hAsymm->StatOverflows(kTRUE);
  Double_t sumMeanAsymm=0, sumReciprocalVar=0, sumWeighted=0;
  Double_t meanPosX;
  Double_t meanPosY;

  Double_t mean[NCUTS];
  Double_t errorX[NCUTS], errorY[NCUTS], cutNum[NCUTS];
  hAsymm->GetXaxis()->SetTitle("Accumulator_0 Asymmetry");
  hAsymm->SetTitle("Accumulator_0 Asymmetry (Difference/YieldAboveBackground)");
  n = 0;
  numEnt = 0;
//  fprintf(polstats,"#CutNum\t Asymm\t\t AsymmErr \tNumEnt\t YieldAbvBkgd  "
//	  " YAbvBkgdErr  \t Background\t BkgdErr \t AvgLasPow\t ScaLasOn"
//	  " \t ScaLasOff\t MeanPosX\t MeanPosY\t DiffLasOn\t DiffLasOnErr\t"
//	  "DiffLasOnRMS\t DiffLasOff\t DiffLasOffErr\t DiffLasOffRMS\n"); 
  //TCanvas *ctemp = new TCanvas("ctemp","ctemp",0,0,600,400);
  for(Int_t i=0; i*2<nCuts-2; i++){
    TH1D *hTempD = new TH1D("hTempD","hTempD",300,-3,3);
    hTempD->StatOverflows(kTRUE);
    helChain->Draw(Form("diff_fadc_compton_accum0.hw_sum/%e>>hTempD",
        		yldAbvBkgd[i]),asymCut[i], "goff");
    hTempD = (TH1D*)gDirectory->Get("hTempD");
    //helChain->Draw(Form("diff_fadc_compton_accum0.hw_sum/"
    //		"(yield_fadc_compton_accum0.hw_sum-%e)>>hTempD",
    //		bkgMean[i]),asymCut[i],"goff");
    //asymCut[i].Print();
    //ctemp->Print(Form("wwwdon/run_%i/hist%i.png",runnum(),i));
    numEnt = (Int_t)hTempD->GetEntries();
    if(numEnt > MIN_ENTRIES){
      asymm[i] = hTempD->GetMean();
      asymmErr[i] = sqrt(pow(hTempD->GetRMS(),2) / (Double_t)numEnt + 
			 pow(yldAbvBkgdErr[i]/yldAbvBkgd[i] * hTempD->GetMean(),2));
      //  asymmErr[i] =  hTempD->GetRMS()/sqrt( (Double_t)numEnt);
    }
    else
    {
      asymm[i] = -9999;
      asymmErr[i] = -9999;
    }

    if(numEnt>MIN_ENTRIES && enoughEntries[i]==1)
    {
      if(yldAbvBkgd[i] < -1)//0 entry laser On cycles should not be included
	if(yldAbvBkgd[i] == -9999)
	  printf("\n\nSCREAM!!! Bug in program!\n\n");
	else 
	  printf("yldAbvBkgd[%d]=%f\n",i, yldAbvBkgd[i]);
      mean[n] = asymm[i];
      sumMeanAsymm += mean[n];
      sumReciprocalVar += pow(asymmErr[i],-2);
      sumWeighted += mean[n] * pow(asymmErr[i],-2);
      helChain->Draw("yield_sca_laser_photon>>hScaOn",
		     lasOnCut && lasWiseCut[i], "goff");
      TH1D *hScaOn=(TH1D*)gDirectory->Get("hScaOn");
      Double_t scaLasOnRate = hScaOn->GetMean();
      delete hScaOn;

      helChain->Draw("yield_sca_laser_photon>>hScaOff", lasWiseCut[i] && lasOFFCut && bcmCut,
		     "goff");
      TH1D *hScaOff = (TH1D*)gDirectory->Get("hScaOff");

      Double_t scaLasOffRate = hScaOff->GetMean();
      delete hScaOff;

      helChain->Draw("yield_sca_laser_PowT.value>>hLasOn", lasWiseCut[i] && lasONCut,
		     "goff");

      TH1D *hLasOn=(TH1D*)gDirectory->Get("hLasOn");

      Double_t lasAvgPow = hLasOn->GetMean();

      delete hLasOn;


      helChain->Draw("yield_sca_bpm_3p02bY>>hPosYtmp", lasWiseCut[i] &&
		     Form("%s>1", BCM), "goff");


      TH1D *hPosYtmp = (TH1D*)gDirectory->Get("hPosYtmp");

      meanPosY = hPosYtmp->GetMean();

      delete hPosYtmp;

      helChain->Draw("yield_sca_bpm_3p02bX>>hPosXtmp", lasWiseCut[i] &&
		      Form("%s>1", BCM), "goff");
      TH1D *hPosXtmp = (TH1D*)gDirectory->Get("hPosXtmp");
      meanPosX = hPosXtmp->GetMean();
      delete hPosXtmp;

      // Save to the Database (jc2)
      //printf("%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t"
	    //           "%12.5e\t%12.5e\t%12.5e\t%12.5e\t%12.5e\t%12.5e\t%9.2e"
	    //  "\t%9.2e\t%12.5e\t%9.2e\t%9.2e\n",
	    //  i,asymm[i], asymmErr[i], numEnt,
	    //  yldAbvBkgd[i], yldAbvBkgdErr[i],bkgMean[i], bkgErr[i],lasAvgPow,
	    //  scaLasOnRate,scaLasOffRate, meanPosX, meanPosY, diffOn[n], 
	    //  diffOnErr[n], diffOnRMS[n],diffOff[n], diffOffErr[n], diffOffRMS[n]);
      DBStorePhoton(1,"accum0","Asymmetry",i,2,numEnt,asymm[i],asymmErr[i]);
      DBStorePhoton(1,"accum0","BackgroundSubtracted",i,
          2,numEnt,yldAbvBkgd[i],yldAbvBkgdErr[i]);
      DBStorePhoton(1,"accum0","Background",i,2,numEnt,bkgMean[i],
          bkgErr[i]);
      DBStorePhoton(1,"accum0","LaserPower",i,2,numEnt,lasAvgPow,0.0);
      DBStorePhoton(1,"accum0","DiffBackgroundSubtracted",i,2,numEnt,
          diffOn[n],diffOnErr[n]);
      DBStorePhoton(1,"accum0","DiffBackground",i,2,numEnt,
          diffOff[n],diffOffErr[n]);
      hAsymm->Add(hTempD);
      cutNum[n] = i;
      errorY[n] = asymmErr[i];
      errorX[n] = 0;

      //      std::cout<<"errorY="<<errorY[n]<<std::endl;
      n++;
      std::cout<<"Cycle "<<i<<" good. Added to hAsymm."<<std::endl;

    }else{
      std::cout<<"Cycle "<<i<<" NOT good. NOT added to hAsymm."<<std::endl;
    }
    delete hTempD;
  }
  c1->cd(6);
  std::cout<<"Total of "<<n<<" good cuts added to hAsymm."<<std::endl;
  Double_t meanAsymm = sumWeighted/sumReciprocalVar;
  Double_t meanError = 1.0/TMath::Sqrt(sumReciprocalVar);
  printf("The weighted mean asymmetry is: %11.4e +/- %7.1e\n",
	 meanAsymm,meanError);
  // hAsymm->SetLineColor(kViolet+2);
  gStyle->SetOptStat(111111);
  hAsymm->Draw();

  TPaveText *pt = new TPaveText(0.12,0.74,0.38,0.88, "NDC"); // NDC sets coords
                                              // relative to pad dimensions
  pt->SetTextColor(kRed);
  pt->SetTextSize(0.048); 
  pt->SetBorderSize(0);
  pt->SetTextAlign(12);
  pt->AddText(Form("Mean asymmetry:\n"));
  pt->AddText(Form(" %0.2f\% \t (+/- %0.2f\%)",
                   meanAsymm*100,meanError*100," "));
  pt->Draw();
  pt->SetFillColor(-1);
  pt->SetShadowColor(-1);
  gPad->Update();

  //fclose(polstats);

  TCanvas *c2 = new TCanvas("c2","c2",900,0,530,380);
  TGraphErrors *gr = new TGraphErrors(n,cutNum,mean,errorX,errorY);
  // (jc2): Why do we create a new one when we just ignore it later?
  TPaveStats *st4;// = new TPaveStats();

  if(n!=0){
  c2->SetLeftMargin(0.14);
  c2->SetRightMargin(0.06);
  gr->GetXaxis()->SetTitle("Laser Cycle Number");
  gr->GetYaxis()->SetTitle("Accumulator_0 Asymmetry");
  gr->GetYaxis()->SetTitleOffset(1.7);
  gr->SetTitle("Accum_0 Asymmetry vs. Laser Cycle Number");
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerStyle(21);
  gStyle->SetOptFit(1111);
  gr->Fit("pol0");
  gr->Draw("AP");
  gPad->Update();
  st4 = (TPaveStats*)gr->GetListOfFunctions()->FindObject("stats");
  st4->SetX1NDC(0.70); //new x start position
  st4->SetX2NDC(0.99); //new x end position 
  st4->Draw();
  gPad->Update();
  SaveToWeb(c2,"asymm");
  //std::cout<<canvas1<<std::endl;
}
  //Save canvases to file.
  std::cout<<"Saving to web directory."<<std::endl;

  SaveToWeb(c1,"plots");
  SaveToWeb(c3,"yieldvspatnum");
  //std::cout<<canvas1<<std::endl;
  //std::cout<<canvas3<<std::endl;
  printf("\n\n\nRun %d completed.\n\n\n",runnum());
  //  cutLas.clear();
  if (kTRUE){
    delete hBeamOff;
    delete hbcm;
    if(hbcmOn)
      delete hbcmOn;
    delete hPosAvgY;
    delete hYieldOn;
    delete hYieldOff;
    delete hPow;
    delete hPosX;
    delete hPosY;
    delete hPLow;
    delete hPUnsd;
    delete hOn;
    delete hOff;
    delete hOff2;
    delete hOff3;
    delete hDiffOn;
    delete hDiffOff;
    delete hAsymm;
    delete pt;
    delete pt1;
    delete pt2;
    delete pt3;
    delete gr;
    delete legend1;
    delete legend2;
    delete legend3;
    delete c1;
    delete c2;
    delete c3;
    if(hTempOn)
      delete hTempOn;
    if(hTempOff)
      delete hTempOff;
    if(hTemp0)
      delete hTemp0;

    helChain = 0; // Should not be deleted because they are not ours to control
    mpsChain = 0;

    // At this point, we also seem to have created a lot of histograms
    // that we left in Root's main Directory. Let's clean those up too.
    gDirectory->Delete("hLocked");

    /*
    hBeamOff->TH2F::~TH2F();
    hbcm->TH2F::~TH2F();
    hYieldOn->TH1F::~TH1F();
    hYieldOff->TH1F::~TH1F();
    hPow->TH2F::~TH2F();
    hPosX->TH2F::~TH2F();
    hPosY->TH2F::~TH2F();
    hPLow->TH2F::~TH2F();
    hPUnsd->TH2F::~TH2F();
    hOn->TH2F::~TH2F();
    hOff->TH2F::~TH2F();
    hDiffOn->TH1F::~TH1F();
    hDiffOff->TH1F::~TH1F();
    hAsymm->TH1D::~TH1D();
    pt->TPaveText::~TPaveText();
    st1->TPaveStats::~TPaveStats();
    st2->TPaveStats::~TPaveStats();
    st4->TPaveStats::~TPaveStats();
    pt->TPaveText::~TPaveText();
    gr->TGraphErrors::~TGraphErrors();
    helChain->TChain::~TChain();
    mpsChain->TChain::~TChain();
    legend1->TLegend::~TLegend();
    legend2->TLegend::~TLegend();
    legend3->TLegend::~TLegend();
    c1->TCanvas::Destructor();
    c2->TCanvas::Destructor();
    c3->TCanvas::Destructor();
    */

    /*    gDirectory->Delete("hbcm");
    gDirectory->Delete("hPow");
    gDirectory->Delete("hPLow");
    gDirectory->Delete("hPUnsd");
    gDirectory->Delete("hOn");
    gDirectory->Delete("hOff");
    gDirectory->Delete("hDiffOn");
    gDirectory->Delete("hDiffOff");
    gDirectory->Delete("hAsymm");
    gDirectory->Delete("hTempOn");
    gDirectory->Delete("hTempOff");
    gDirectory->Delete("hTempD");
    gDirectory->Delete("hYieldOn");
    gDirectory->Delete("hYieldOff");
    gDirectory->Delete("st");
    gDirectory->Delete("st2");
    gDirectory->Delete("legend1");
    gDirectory->Delete("legend2");
    gDirectory->Delete("pt");
    gDirectory->Delete("gr");
    gDirectory->Clear();
    */
  }
}

// Define functions with c symbols (create/destroy instances) 
extern "C" Accum* create()
{
  return new Accum();
}


extern "C" void destroy( Accum *accum )
{
  delete accum;
}
