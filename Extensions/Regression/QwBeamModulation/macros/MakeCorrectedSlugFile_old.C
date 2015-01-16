// *************************** //
//
// Macro to create a copy of the "reduced" slugfiles with dithering corrections.
// Read the monitors and slopes to use for the corrections from text files.
//
// Author: Manolis Kargiantoulakis, 8/29/13. 
// Code adapted from Scott MacEwan's (?) MakeSlug.C
//
// *************************** //



#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TSystemDirectory.h"
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

Int_t MakeCorrectedSlugFile(int slug =42, Int_t run_period = 1)
{
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
  

  // -> Read the correction slopes for each run range
  
  std::vector<int> run_low, run_hi;
  
  TSystemDirectory dir(slopeListsDir, slopeListsDir);
  TList *files = dir.GetListOfFiles();
  TString fileStem = Form("slopeList_%s", detector[0]);
  fileStem.Append(".");
  Int_t index = fileStem.Length();
  
  if (files) {
    TSystemFile *file;
    TString fileName;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fileName = file->GetName();
      cout << " Reading slopes from " << fileName.Data() << endl;
      cout << " Reading slopes from 2 " << file->GetName() << endl;

      Int_t digits = 5;
      if (!file->IsDirectory()
	  && fileName.BeginsWith(fileStem)
	  && fileName.EndsWith(".dat")  ) {
	if (debug>1) cout << " Reading slopes from " << fileName.Data() << endl;
	TString run_str( fileName(index,digits) );
	if (!run_str.IsAlnum() ) digits--; // 4-digit run number
	run_low.push_back( run_str.Atoi() ); // works even if last digit is not a number
	//	cout << " Low run = " << run_low.at(nRuns) << endl;
	run_str = fileName(index+digits+1,digits+1);
	run_hi.push_back( run_str.Atoi() ); // works even if last digit is not a number
	//	cout << " High run = " << run_hi.at(nRuns) << endl;
      }
    }
  }

  const Int_t NRanges = run_low.size();
  Double_t correction_slope[NDet][NMon][NRanges], err_correction_slope[NDet][NMon][NRanges];
  
  // -> Sort the low runs of each range.
  // The runs should be sorted "alphabetically" from the file iterator, which means that  // [unfortunately they're not]
  // runs below 10000 are actually coming later, but otherwise are in order.
  // So only need to compare the last and first elements.
//   while(1) { // iterate until the last element is bigger than the first
//     if (run_low[NRanges-1] <= run_low[0]) {
//       run_low.insert( run_low.begin(), run_low[NRanges-1] );
//       run_hi.insert( run_hi.begin(), run_hi[NRanges-1] );
//       run_low.pop_back();
//       run_hi.pop_back();
//     }
//     else break;
//   }
  // Turns out this doesn't work, but it's ok: 
  // All the ranges are checked when a run is found outside of the current range.

  if (debug) {
    cout <<  " Reading correction slopes from " << NRanges << " run ranges :: \n";  
    for (int iRange=0; iRange<NRanges; iRange++)
      cout << run_low[iRange] << " - " << run_hi[iRange] << endl;
    cout << endl;
  }
  
  for (int iRange=0; iRange<NRanges; iRange++) {
    for (int iDet=0; iDet<NDet; iDet++) {
      
      ifstream datFile;
      string line;
      
      TString fileName = Form("slopeList_%s.%d-%d.dat",
		      detector[iDet], run_low[iRange], run_hi[iRange] );
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
	ss >> correction_slope[iDet][iMon][iRange] >> err_correction_slope[iDet][iMon][iRange];
	if (debug>2) cout << " Slope " << (iMon+1) << " :: " 
			  << correction_slope[iDet][iMon][iRange] << " +- " 
			  << err_correction_slope[iDet][iMon][iRange] << endl;
 	if (iMon==0 || iMon==1 || iMon==4) { // Divide position slopes by 1000 to match units in tree
 	  correction_slope[iDet][iMon][iRange] /= 1000; err_correction_slope[iDet][iMon][iRange] /= 1000;
 	}
	iMon++;
      }
      datFile.close(); 
      if (iMon!=NMon)    
	cout << "\n\n\n\t !!! Attention !!!\n"
	     << " Expected " << NMon << " lines in file, read only " << iMon << " !!\n\n\n\n";
      
    }
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

  Int_t slopes_exist;
  TBranch* branch_flag = corrected_tree->Branch("slopes_exist", &slopes_exist, "slopes_exist/I");
  Int_t previous_NotFound_run = 0;

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
  Int_t iRange=0;
  slopes_exist=1;

  for (Int_t iEntry=0; iEntry<nEntries; iEntry++) {
    tree->GetEntry(iEntry);

    if ((run<run_low[iRange] || run>run_hi[iRange]) && run!=previous_NotFound_run) {
      slopes_exist=0;
      for (iRange=0; iRange<NRanges; iRange++)
	if (run>=run_low[iRange] && run<=run_hi[iRange]) {
	  if (debug)
	    cout << "\n Run " << run << " in range " << iRange << " :: " 
		 << run_low[iRange] << " - " << run_hi[iRange] << endl << endl;
	  slopes_exist=1; break;
	}
    }
    if (! slopes_exist && run!=previous_NotFound_run) {
      cout //<< "\n\n\n\t !!! Attention !!!\n"
	<< "\n Run " << run << " not found in any range of correction slopes !!\n\n";
      previous_NotFound_run=run;
      //      exit(1);
    }
    
    for (int iDet=0; iDet<NDet; iDet++) {
      if (inputDetectorLeaf[iDet]) {
	correctedDetectorValue[iDet]=inputDetectorLeaf[iDet]->GetValue();
	if (debug>1 && iEntry==0 && iDet<3) cout << " Initial value :: " << correctedDetectorValue[iDet] << endl;
	if (slopes_exist) {
	  for (int iMon=0; iMon<NMon; iMon++) { // Apply corrections
	  
// 	  cout << " \n\n Correction slope value at this point ( "
// 	       << iDet << " , " << iMon << " , " << iRange << " ) :: "
// 	       << correction_slope[iDet][iMon][iRange] << "\n\n";

	    correctedDetectorValue[iDet] -= correction_slope[iDet][iMon][iRange] * inputMonitorValue[iMon];
	    if (debug>1 && iEntry==0 && iDet<3) 
	      cout << " After correction " << (iMon+1) << " ( "
		   << correction_slope[iDet][iMon][iRange] << " * " 
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
