// *************************** //
//
// Macro to create a tree with runlet averages of corrected asymmetries,
// to be friend-able to the DB tree.
// Read averages only for runlets that have entries in the DB tree.
//
// Author: Manolis Kargiantoulakis, 10/11/13. 
//
// *************************** //



#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <utility>

using namespace std;

Int_t MakeCorrectedDBfile(
			  TString regression_id="off",
			  Int_t iQweakRun = 1,
			  TString fileExtension="", // eg, "Set11"
			  TString detectorListFileName = "/home/ek4px/lists/detectorList.txt" )
{
  TStopwatch timer;
  
  const Int_t debug=0;

  if (fileExtension!="") fileExtension.Prepend("_");
 
  
  //-> Read set of detectors

  const Int_t MaxDetectors=100;
  FILE *detectorListFile;
  TString detectorListFileName = "/home/ek4px/lists/detectorList.txt";
  detectorListFile = fopen (detectorListFileName.Data(), "r");
  if (detectorListFile == NULL) printf("Error opening file %s\n",detectorListFileName.Data());
  else printf("\n Opened %s\n",detectorListFileName.Data());
  char detector_name[MaxDetectors][255];
  Int_t nDetectors=0;
  
  if (debug) cout << " Reading set of detectors ::\n";
  while ( ! feof (detectorListFile) ) {
    if (fscanf(detectorListFile,"%s",detector_name[nDetectors]) != 1) {
      fscanf(detectorListFile,"%s",tmp);
      printf(" Warning: Not valid detector input :: %s\n", tmp);
      continue;
    }
    if (debug) printf("%i %s\n",
		      nDetectors, detector_name[nDetectors]);
    nDetectors++;
    if (nDetectors+1 >= MaxDetectors) {
      printf("Warning: Max number of detectors (%i) exceeded\n",MaxDetectors);
      break;
    }
  }
  fclose (detectorListFile);
  
  
  // Add the detector combinations
  const Int_t NCombos = 14;
  TString detector_combo[NCombos] = {"asym_qwk_md_odd", "asym_qwk_md_even", "asym_qwk_md_odd_even",
				     "asym_qwk_md_1p5_3p7", "asym_qwk_md_2p6_4p8",
				     "asym_qwk_md_dipole_h", "asym_qwk_md_dipole_v", "asym_qwk_md_barsum_avg",
				     "asym_uslumi_1p5", "asym_uslumi_3p7", "asym_uslumi_1_5", "asym_uslumi_3_7", 
				     "asym_uslumi_1p3_5p7", "asym_uslumi_barsum_avg" };

  for (int iCombo=0; iCombo<NCombos; iCombo++) {
    strcpy(detector_name[nDetectors], detector_combo[iCombo].Data());    
    if (debug) printf("%i %s\n",
		      nDetectors, detector_name[nDetectors]);
    nDetectors++;  
  }
  
  const Int_t NDet = nDetectors;
  cout << " Creating " << NDet << " corrected detector leaves \n\n";
  
  
  //-> Create the root file and tree to hold the corrected data
  TString rootFileDir = Form("/home/ek4px/rootfiles/DB/Run%d", iQweakRun);
  TString correctedRootFileName = Form("%s/corrected_tree%s.root",
				       rootFileDir.Data(), fileExtension.Data() );
  TFile *correctedRootFile = TFile::Open(correctedRootFileName.Data(),"RECREATE");
  if (correctedRootFile == NULL) {
    printf("Error: opening file %s for output\n",correctedRootFileName.Data());
    return -1;
  }
  TTree *corrected_tree = new TTree("corrected_tree", "corrected_tree" );
  
  struct MyType_t{
    Double_t value;
    Double_t err;
    Double_t rms;
    Int_t n;
  };
  
  MyType_t detector[NDet];
  Int_t run, runlet; Double_t run_number_decimal, temp;
  TBranch *branches[NDet+3];
  
  // Modify the leaf names for the corrected detectors
  TString newLeafName[NDet];
  for (int iDet=0; iDet<NDet; iDet++) {
    newLeafName[iDet] = Form("corrected_%s", detector_name[iDet] );
    if (debug)  printf (" Corrected leaf name :: %s\n", newLeafName[iDet].Data() );
    // Create the branch for the corrected tree
    branches[iDet] = corrected_tree->Branch( newLeafName[iDet].Data(),
					     &detector[iDet], "value/D:err/D:rms/D:n/I");
  }
  
  branches[NDet]   = corrected_tree->Branch("run_number", &run, "run_number/I");
  branches[NDet+1] = corrected_tree->Branch("run_number_decimal", &run_number_decimal, "run_number_decimal/D");
  branches[NDet+2] = corrected_tree->Branch("runlet", &runlet, "runlet/I");
  
  
  // -> Now read in the input DB tree
  
  TString inputRootFileName = Form("%s/%s_tree.root",
				   rootFileDir.Data(), regression_id.Data() );
  TFile *inputRootFile = TFile::Open(inputRootFileName);
  if (inputRootFile==0) {
    printf(" Did not find given input ROOT file :: \n%s",
	   rootFileName_in.Data() );
    exit(1);
  }    
  printf("Opened %s\n",inputRootFileName.Data());
  
  TTree *tree = (TTree*)gROOT->FindObject("tree");
  
  // Only enable the branches we need to read the run and runlet number
  tree->SetBranchStatus("*",0); // disable all branches
  tree->SetBranchStatus("run_number",1);				
  tree->SetBranchAddress("run_number", &run);			
  tree->SetBranchStatus("run_number_decimal",1);				
  tree->SetBranchAddress("run_number_decimal", &run_number_decimal);				


  //-> Step through the tree reading the runlets for which it is filled.
  // For these runlets read the corrected asymmetry data and fill the corrected tree.
  // ** Except, no runlet# on the DB tree. Will have to extract it from the run_number_decimal variable.
  Int_t nEntries = tree->GetEntries();
  printf("There are %i entries in the tree.\n",nEntries);

  ifstream datFile;
  string line; 
  Int_t iDet=0;

  for (Int_t iEntry=0; iEntry<nEntries; iEntry++) {
    tree->GetEntry(iEntry);

    if (iEntry%1000==0) cout << " Processing entry " << iEntry << endl;

    runlet = (int) ( (run_number_decimal - 1.*run)*20 + 0.5 );
    if (debug>1 && iEntry<20) cout << run_number_decimal << " ::  Run " << run << " , runlet " << runlet << endl;
    
    datFile.open( Form("/home/ek4px/data/correctedData_%d_%d%s.dat",
		       run, runlet,
		       fileExtension.Data() ) );
    if (!datFile)  {
      // Set value to -99999 if no corrected data is found for this runlet //
      for (iDet=0; iDet<NDet; iDet++) {
	detector[iDet].value = -99999;
	detector[iDet].err   = 0;
	detector[iDet].rms   = 0;
	detector[iDet].n     = 0;
      }
      corrected_tree->Fill();
      if (debug) cout << " No corrected data found in runlet " << run << "." << runlet << endl;
      continue;
    }

    if (debug>1) cout << " Found file for Runlet " << run << "." << runlet << endl;
    
    // Skip the 1st line.
    getline(datFile,line);
    iDet=0; 
    while(1) {
      getline(datFile,line); 
      if(!datFile.good()) break;   
      if (line[0]=='!')  continue;
      stringstream ss(line);
      ss >> detector[iDet].value >> detector[iDet].err 
	 >> detector[iDet].rms >> detector[iDet].n; 
      detector[iDet].value *= 1e6;      detector[iDet].err *= 1e6;      detector[iDet].rms *= 1e6;
      iDet++;
    }

    if (debug>1 && iEntry<20) cout << detector_name[0] << " value :: " << detector[0].value << endl;

    datFile.close(); 
   
    corrected_tree->Fill();
  }
  
  correctedRootFile->Write(0,TObject::kOverwrite);
  timer.Print();
  Double_t fillTime = timer.CpuTime();
  printf("Time: %f ms per event\n",fillTime/nEntries*1000);
  
}
