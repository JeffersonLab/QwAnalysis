// *************************** //
//
// Macro to create a copy of the "reduced" slugfiles with dithering corrections.
// Read the monitors and slopes to use for the corrections from text files.
//
// Rewritten by Don Jones 05/21/14
// Author: Manolis Kargiantoulakis, 8/29/13. 
// Code adapted from Scott MacEwan's (?) MakeSlug.C
//
// *************************** //



#include "TFile.h"
#include "TList.h"
#include "TChain.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TGraph.h"
#include "TSystem.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <utility>
#include <sstream>

using namespace std;

Bool_t globalEXIT=0;

void sigint_handler(int sig)
{
  std::cout << "handling signal no. " << sig << "\n";
  globalEXIT=1;
}

Int_t MakeCorrectedSlugFile_beta(int slug)
{
  Int_t run_period = ((slug >= 42 && slug <=136) ? 1 : 0);
  if( slug >= 137 && slug<=321 )
    run_period = 2;
  if(run_period == 0){
    cout<<"Invalid slug number. Exiting.\n";
    return -1;
  }

  TStopwatch timer;
  const Int_t debug=0, minEntriesPerSlug = 1;
  TChain *ch = new TChain("slopes");
  ch->Add(Form("%s/../MacrocycleSlopesTree_ChiSqMin.set0.root", 
	       gSystem->Getenv("BMOD_OUT")));
  if(ch->GetEntries(Form("slug==%i",slug))<minEntriesPerSlug){
    cout<<"Too few entries in this slug. Only "<<
      ch->GetEntries(Form("slug<%i",minEntriesPerSlug))<<" entries. Exiting.\n";
    return -1;
  }
  TString listsDir = "/home/ek4px/lists/detectorLists/";
  TString detectorListFileName = Form("%sdetectorList.txt", listsDir.Data());
  TString rootFileDir = Form("/net/data2/paschkelab2/reduced_slugfiles/run%i/", 
			     run_period);
  TString rootFileName_in = rootFileDir + Form("reduced_slug%i.root", slug);
  TString rootFileName_out = rootFileDir + Form("corrected_slug%i.root", slug);

  
  TString slopeListsDir = "/net/data1/paschkedata1/bmod_out/slopelists3/";
  TString monitorListFileStem = "monitorList";
  TString monitorListFileName = listsDir+monitorListFileStem+".txt";

  //-> Read set of monitors to use for corrections

  const Int_t MaxMonitors=10;
  ifstream monitorListFile(monitorListFileName.Data());
  if (!monitorListFile.is_open()) 
    printf("Error opening file %s\n", monitorListFileName.Data());
  else printf("\n Opened %s\n", monitorListFileName.Data());
  char monitor[MaxMonitors][255];
  Int_t nMonitors=0;
  
  if (debug) cout << " Reading set of monitors to apply corrections ::\n";
  while ( !monitorListFile.eof()) {
    string line;
    monitorListFile>>monitor[nMonitors];
    printf("%i %s\n",
	   nMonitors, monitor[nMonitors]);
    nMonitors++;
    getline(monitorListFile, line);
    monitorListFile.peek();
    if (nMonitors+1 >= MaxMonitors) {
      printf("Warning: Max number of monitors (%i) exceeded\n",MaxMonitors);
      break;
    }
  }
  monitorListFile.close();
  
  cout << " " << nMonitors << " monitors will be used for corrections, \n"
       << " expecting that many slopes for each detector\n\n";
  const Int_t NMon = nMonitors;

  
  //-> Read set of detectors to be corrected
  
  const Int_t MaxDetectors=100;

  ifstream detectorListFile(detectorListFileName.Data());
  if (!detectorListFile.is_open()) 
    printf("Error opening file %s\n",detectorListFileName.Data());
  else if (debug) printf("\n Opened %s\n",detectorListFileName.Data());
  char detector[MaxDetectors][255];
  Int_t nDetectors=0;
  
  if (debug)  cout << " Reading set of detectors to be corrected ::\n";
  while (!detectorListFile.eof()) {
    string line;
    detectorListFile>>detector[nDetectors];
    if (debug)  printf("%i %s\n", nDetectors, detector[nDetectors]);
    nDetectors++;
    getline(detectorListFile,line);
    detectorListFile.peek();
    if (nDetectors+1 >= MaxDetectors) {
      printf("Warning: Max number of detectors (%i) exceeded\n",MaxDetectors);
      break;
    }
  }
  detectorListFile.close();
  
  const Int_t NDet = nDetectors;
  cout << " Correcting for " << NDet << " detectors \n\n";
  

  //Find the proper run range
  ifstream run_range_file(Form("%srun_ranges.dat", slopeListsDir.Data())); 
  if( !(run_range_file.is_open() && run_range_file.good()) ){
    cout<<"Run range file not found. Run the following command from slopeslists"
	<<" directory then rerun program:\n";
    cout<<"ls slopeList_asym_qwk_md1barsum.* |"
	<<" sed 's/slopeList_asym_qwk_md1barsum.//g'" 
	<<"|sed 's/-/  /g' |sed 's/.dat//g' > run_ranges.dat\n";
    return -1;
  }

  int run_lo, run_hi, slug_lo, slug_hi, slopes_exist = 0;
  while (!run_range_file.eof() && !slopes_exist) {
    run_range_file >>run_lo>>run_hi;
    string line;
    ifstream slopesFile(Form("%sslopeList_asym_uslumi_sum.%i-%i.dat",
			     slopeListsDir.Data(),run_lo, run_hi));
    if(!(slopesFile.is_open()&&slopesFile.good())){
      cout<<"Slopes file not found. Exiting.\n";
      printf("%sslopeList_asym_uslumi_sum.%i-%i.dat",
	     slopeListsDir.Data(),run_lo, run_hi);
      return -1;
    }
    getline(slopesFile, line);
    cout<<line<<endl;
    string find_this = "slug >= ";
    size_t pos_lslug = line.find(find_this) + find_this.length();
    size_t n_digits = line.find(" ",pos_lslug) - pos_lslug ;
    find_this = line.substr(pos_lslug, n_digits);
    slug_lo = atoi(find_this.data());
    //    cout<<find_this<<" "<<n_digits<<" "<<slug_lo<<endl;

    find_this = "slug <= ";
    size_t pos_hslug = line.find(find_this) + find_this.length();
    n_digits = line.find("\"",pos_hslug) - pos_hslug;  
    find_this = line.substr(pos_hslug, n_digits);
    slug_hi = atoi(find_this.data());
    //    cout<<find_this<<" "<<n_digits<<" "<<slug_hi<<endl;

    if(slug>=slug_lo &&slug<=slug_hi){
      cout<<"Slopes found for slug "<<slug<<".\n";
      slopes_exist = 1;
      break;
    }
    run_range_file.peek();
  }
  run_range_file.close();
  if(!slopes_exist){
    cout<<"No slopes found for slug "<<slug<<". Exiting.\n";
    return -1;
  }

  Double_t **correction_slope = new Double_t *[NDet], 
    **err_correction_slope = new Double_t *[NDet];
  for(int idet = 0; idet<NDet;++idet){
    correction_slope[idet] = new Double_t[NMon];
    err_correction_slope[idet] = new Double_t[NMon];
  }
  
 
  cout << run_lo << " - " << run_hi << endl;
  cout << endl;
  
  for (int iDet=0; iDet<NDet; iDet++) {
      
    ifstream datFile;
    string line;
      
    TString fileName = Form("slopeList_%s.%d-%d.dat",
			    detector[iDet], run_lo, run_hi );
    fileName.Prepend(slopeListsDir);
    datFile.open( fileName );
    if (!datFile) { 
      cout << fileName.Data()<<" not found.\n";
      continue;
    }
      
    if (debug>2)
      cout << " Reading slopes from file :: " << fileName << endl;
    int iMon;
    for (iMon=0; iMon<NMon;) {
      getline(datFile,line); 
      if(!datFile.good())  break; 
      if (line[0]=='!') continue;  // allow for comments in files.
      std::stringstream ss;
      ss.str(line);
      ss >> correction_slope[iDet][iMon] >> err_correction_slope[iDet][iMon];
      if (debug>2) cout << " Slope " << (iMon+1) << " :: " 
			<< correction_slope[iDet][iMon] << " +- " 
			<< err_correction_slope[iDet][iMon] << endl;
      if (iMon==0 || iMon==1 || iMon==4) { 
	// Divide position slopes by 1000 to match units in tree
	correction_slope[iDet][iMon] /= 1000.0; 
	err_correction_slope[iDet][iMon] /= 1000.0;
      }
      iMon++;
    }
    datFile.close(); 
    if (iMon!=NMon)    
      cout << "\n\n\n\t !!! Attention !!!\n"
	   << " Expected " << NMon << " lines in file, read only " << iMon << " !!\n\n\n\n";
      
  }
  
  
  
  //-> Create the root file and tree to hold the corrected data
  TFile *correctedRootFile = new TFile(rootFileName_out.Data(),"RECREATE");
  if (correctedRootFile == NULL) {
    printf("Error: opening file %s for output\n",rootFileName_out.Data());
    return -1;
  }
  TTree *corrected_tree = new TTree("corrected_tree", "Corrected slug tree");
  
  Double_t inputDetectorValue[MaxDetectors];
  Double_t inputMonitorValue[MaxMonitors];
  Double_t correctedDetectorValue[MaxDetectors];
  Int_t run;
  TBranch* branches[MaxDetectors];

  TBranch* branch_flag = corrected_tree->Branch("slopes_exist", &slopes_exist, "slopes_exist/I");

  TLeaf *inputDetectorLeaf[MaxDetectors], *inputMonitorLeaf[MaxMonitors];
  //  Int_t iEvent=0, iRun=0, iRunlet=0;

  
  // Modify the leaf names for the corrected detectors
  TString newLeafName[NDet];
  for (int iDet=0; iDet<NDet; iDet++) {
    newLeafName[iDet] = Form("corrected_%s", detector[iDet] );
    if (debug)  printf (" Corrected leaf name :: %s\n", newLeafName[iDet].Data());
    // Create the branch for the correctedslug tree
    branches[iDet] = corrected_tree->Branch(newLeafName[iDet].Data(), 
					    &correctedDetectorValue[iDet],
					    Form("%s/D",newLeafName[iDet].Data())
					    );
  }


  // -> Now read in the input "reduced" tree

  TFile *inputRootFile = TFile::Open(rootFileName_in);
  if (inputRootFile==0) {
    printf(" Did not find given input ROOT file :: \n%s",
	   rootFileName_in.Data() );
    exit(1);
  }    
  printf("Opened %s\n",rootFileName_in.Data());
  
  TTree *tree = (TTree*)gROOT->FindObject("slug");

  // Only enable the important branches
  tree->SetBranchStatus("*",0); // disable all branches
  for (int iDet=0; iDet<NDet; iDet++) {
    tree->SetBranchStatus(Form("%s",detector[iDet]), 1);	
    inputDetectorLeaf[iDet] = (TLeaf*) tree->GetLeaf(detector[iDet]);
    tree->SetBranchAddress(Form("%s",detector[iDet]), 
			   &inputDetectorValue[iDet]);	
  }
  for (int iMon=0; iMon<NMon; iMon++) {
    tree->SetBranchStatus(Form("%s",monitor[iMon]), 1);
    inputMonitorLeaf[iMon] = (TLeaf*) tree->GetLeaf(monitor[iMon]);
    tree->SetBranchAddress(Form("%s",monitor[iMon]), 
			   &inputMonitorValue[iMon]);
  }
  tree->SetBranchStatus("run",1);				
  tree->SetBranchAddress("run", &run);				


  //-> Step through the tree reading detector and monitor values.
  // Use the appropriate slopes to correct the detector values
  // and save them to the corrected tree.
  Int_t nEntries = tree->GetEntries();
  printf("There are %i entries in the tree.\n",nEntries);

  for (Int_t iEntry=0; iEntry<nEntries; iEntry++) {
    tree->GetEntry(iEntry);
  
    for (int iDet=0; iDet<NDet; iDet++) {
      if (inputDetectorLeaf[iDet]) {
	correctedDetectorValue[iDet]=inputDetectorLeaf[iDet]->GetValue();
	if (debug>1 && iEntry==0 && iDet<3) 
	  cout << " Initial value :: " << correctedDetectorValue[iDet] << endl;
	if (slopes_exist) {
	  for (int iMon=0; iMon<NMon; iMon++) { // Apply corrections
	  
// 	  cout << " \n\n Correction slope value at this point ( "
// 	       << iDet << " , " << iMon << " , " << iRange << " ) :: "
// 	       << correction_slope[iDet][iMon][iRange] << "\n\n";

	    correctedDetectorValue[iDet] -= correction_slope[iDet][iMon] 
	                                    * inputMonitorValue[iMon];
	    if (debug>1 && iEntry==0 && iDet<3) 
	      cout << " After correction " << (iMon+1) << " ( "
		   << correction_slope[iDet][iMon] << " * " 
		   << inputMonitorValue[iMon] << " ) :: "
		   << correctedDetectorValue[iDet] << endl;
	  }
	}
      }
      else
	correctedDetectorValue[iDet] = -1e6;
      
      if (debug > 1 && iEntry<10) 
	printf("%i %i ::  %10e  -->  %10e \n", iEntry, iDet,
	       inputDetectorValue[iDet], correctedDetectorValue[iDet] );
    }
    corrected_tree->Fill();
  }
  
  correctedRootFile->Write(0,TObject::kOverwrite);
  timer.Print();
  Double_t fillTime = timer.CpuTime();
  printf("Time: %f ms per event\n",fillTime/nEntries*1000);
  return 0;
}
