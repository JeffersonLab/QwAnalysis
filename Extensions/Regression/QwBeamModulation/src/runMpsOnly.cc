#include "headers.h"
#include "QwMpsOnly.hh"
#include <iostream>

Int_t main(Int_t argc, Char_t *argv[])
{
  Int_t minEvents = 10000;
  TString filename;
  TChain *mps_tree = new TChain("mps_slug");
  QwMpsOnly *mps_only = new QwMpsOnly(mps_tree);
  mps_only->SetOutput((char*)gSystem->Getenv("BMOD_OUT"));
  if(!gSystem->OpenDirectory(mps_only->GetOutput())){
    mps_only->PrintError("Cannot open output directory.\n");
    mps_only->PrintError("Directory needs to be set as env variable and contain:\n\t slopes/ \n\t regression/ \n\t diagnostics/ \n\t rootfiles/"); 
    exit(1);
  }
  mps_only->GetOptions(argc, argv);
  std::cout<<"Beginning to process run "<< mps_only->run_number<<std::endl;

  std::cout<<"fRunNum: "<<mps_only->fRunNumberSet<<" "<< mps_only->run_number
	   <<std::endl;

  if( !(mps_only->fRunNumberSet ) ){
    std::cout<<"fRunNum: "<<mps_only->fRunNumberSet<<" "<<
      mps_only->run_number<<std::endl;
    mps_only->PrintError("Error Loading:  no run number specified");
    //   std::cout<<mps_only->fRunNumberSet<<std::endl;
    exit(1);
  }

  std::cout<<mps_only->fOmitCoil.size()<<" coils omitted from analysis.\n";
  //load segments in chronological order
  int nFiles = 0;
  filename = Form("%s_%d_0:*.root", mps_only->fFileStem.Data(), 
		  mps_only->run_number);
  std::cout<<"File: "<<filename.Data()<<std::endl;
  if(mps_only->LoadRootFile(filename, mps_tree, 0)){
    mps_only->SetFileName(filename);
    ++nFiles;
  }

  filename = Form("%s_%d_5*.root", mps_only->fFileStem.Data(), 
		  mps_only->run_number);
  std::cout<<"File: "<<filename.Data()<<std::endl;
  if(mps_only->LoadRootFile(filename, mps_tree, 0)){
    mps_only->SetFileName(filename);
    ++nFiles;
  }

  filename = Form("%s_%d_1*.root", mps_only->fFileStem.Data(), 
		  mps_only->run_number);
  std::cout<<"File: "<<filename.Data()<<std::endl;
  if(mps_only->LoadRootFile(filename, mps_tree, 0)){
    mps_only->SetFileName(filename);
    ++nFiles;
  }

  filename = Form("%s_%d_2*.root", mps_only->fFileStem.Data(), 
		  mps_only->run_number);
  std::cout<<"File: "<<filename.Data()<<std::endl;
  if(mps_only->LoadRootFile(filename, mps_tree, 0)){
    mps_only->SetFileName(filename);
    ++nFiles;
  }

  filename = Form("%s_%d_3*.root", mps_only->fFileStem.Data(), 
		  mps_only->run_number);
  std::cout<<"File: "<<filename.Data()<<std::endl;
  if(mps_only->LoadRootFile(filename, mps_tree, 0)){
    mps_only->SetFileName(filename);
    ++nFiles;
  }

  filename = Form("%s_%d_ful*.root", mps_only->fFileStem.Data(), 
		  mps_only->run_number);
  std::cout<<"File: "<<filename.Data()<<std::endl;
  if(mps_only->LoadRootFile(filename, mps_tree, 0)){
    mps_only->SetFileName(filename);
    ++nFiles;
  }
  std::cout<< mps_tree->GetNtrees()<<" files added to chain."<<std::endl;


  if(mps_only->fMakeFriendTree){
    std::cout << "Creating friend tree with new monitors and ramp_filled leaves."
	      << std::endl;
    if(mps_only->MakeFriendTree(1)){
      std::cout<<"Failed to make friend tree. Exiting.\n";
      return -1;
    }
    std::cout<<"Friend tree successfully created.\n";
  }

  if(mps_only->AddFriendTree()){
    std::cout<<"Failed to add friend tree. Exiting.\n";
     return -1;
  } 
  std::cout<<"Friend tree successfully added.\n";


  std::cout<<"Entries: "<<mps_tree->GetEntries()<<std::endl;
  if(mps_tree->GetEntries() < minEvents){
    std::cout<<mps_tree->GetEntries()<<" entries. Exiting program."<<
      " Too few entries.\n";
    return -1;
  }

  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;
  mps_only->ReadConfig("");

  std::cout << "Finished scanning data -- building relevant data vectors" << std::endl;
  mps_only->BuildDetectorData();
  mps_only->BuildMonitorData();
  if(mps_only->fCutNonlinearRegions)
    if(mps_only->FindRampExcludedRegions()) 
      return -1;

  mps_only->BuildCoilData();
  mps_only->BuildDetectorSlopeVector();
  mps_only->BuildDetectorResidualVector();
  mps_only->BuildMonitorSlopeVector();
  mps_only->BuildDetectorAvSlope();
  mps_only->BuildMonitorAvSlope();

  //Open file for writing coil sensitivities by microcycle
  gSystem->Exec("umask 002");
  mps_only->coil_sens.open(Form("%s/coil_sensitivities/coil_sens_%i%s.%s.dat", 
				mps_only->GetOutput().Data(),mps_only->run_number,
				( mps_only->fChiSquareMinimization ? 
				  "_ChiSqMin" : ""),
				mps_only->fSetStem.Data()), fstream::out);


  std::cout << "Starting to pilfer the data" << std::endl;
  if(!mps_only->coil_sens.is_open()){
    std::cout<<"Coil sensitivities file(s) not opened. Exiting.\n";
    return -1;
  }
  if(mps_only->PilferData()<minEvents){
    std::cout<<"Too few good events. Exiting program for run "<<
      mps_only->run_number<<"\n"; 
    return -1;
  }

  mps_only->coil_sens.close();
  mps_only->macrocycle_coeffs.close();
  if(mps_only->CalculateWeightedSlope(1)==-1){
    std::cout<<"Error in calculating slopes. One or more 0 entries. Exiting\n";
    return -1;
  }
  mps_only->MatrixFill(1);
  
  std::cout << "Closing Mps_Tree" << std::endl;
  delete mps_tree;
  mps_only->Clean();


  // The second half isn't ready yet. So exit.
  exit(1);

  std::cout << "Creating Slug(let) chain now." << std::endl;
  TChain *hel_tree = new TChain("slug");
  mps_only->Init(hel_tree);

  filename = Form("sluglet%d_*root", mps_only->run_number);
  mps_only->LoadRootFile(filename, hel_tree, true);
  mps_only->SetFileName(filename);

  mps_only->SetupHelBranchAddress();
  mps_only->ReadConfig("hel");

  std::cout << "Calculating Corrections" << std::endl;
  mps_only->ComputeAsymmetryCorrections();

  return 0;

}
