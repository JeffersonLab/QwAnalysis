///////////////////////////////////////////////////////////////////////////////
//  File:       RevisionDifference.C
//  Author(s):  Juan Carlos Cornejo <cornejo@jlab.org>
//  ChangeLog:  * Mon Jun 27 2011 Juan Carlos Cornejo <cornejo@jlab.org>
//              - Initial working version
//  Summary:    This just scours the specified rootfile and produces 1D plots
//              of every branch found in the specified trees.. The # of Entries,
//              Mean, RMS, Min and Max are printed to the standard out.
//  Usage:      To use this script from a command line do:
//                qwroot -b -q RevisionDifference.C\(run_number,\"ROOT_File_Stem\"\) \
//                  > file_output
//              To use from within ROOT (or QwROOT) do:
//                .x RevisionDifference.C(run_number,"ROOT_FILE_Stem")
//              Where run_number is the run number and ROOT_File_Stem is the
//              prefix of the ROOT file you want to summarize, where the
//              rootfile has to be in the form of
//              ROOT_File_Stem_run_number.###.root
//
//              Then it is useful to just diff two different outputs and check
//              that no unintentional changes were made.
///////////////////////////////////////////////////////////////////////////////
// ROOT Includes
#include "TString.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TChain.h"
#include "TH1F.h"
#include "TDirectory.h"

// Standard Library Includes
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Prototypes
void FindBranchNames( TObjArray *array, std::vector<std::string> *names,
    UInt_t entries);
void ProcessBranch( TChain *chain, std::vector<std::string> names,
    UInt_t entries);
///////////////////////////////////////////////////////////////////////////////
// Global Defines
const Bool_t kDebug = kFALSE; // Turn on/off all extraneous debug output that
                              // clouds the true output.

UInt_t kPrintHeaderEvery = 50;  // Print header every n lines

// Vectors of the names of branches in both Mps tree and Helicity tree
std::vector<std::string> mpsBranches;
std::vector<std::string> helBranches;
///////////////////////////////////////////////////////////////////////////////
//
void RevisionDifference( Int_t runnumber = 12161, const char *prefix = "Qweak_")
{
  // Chain All the Trees
  TChain *mpsChain = new TChain("Mps_Tree");
  TChain *helChain = new TChain("Hel_Tree");
  TString fileName = Form("%s/%s%d.*.trees.root",getenv("QW_ROOTFILES"),
      prefix,runnumber);
  mpsChain->Add(fileName);
  helChain->Add(fileName);

  // Get a list of _ALL_ branches (for now does not go into sub-branches)
  TObjArray *mpsBranchArray = mpsChain->GetListOfBranches();
  TObjArray *helBranchArray = helChain->GetListOfBranches();

  // Check to make sure both arrays got filled
  if( !mpsBranchArray ) {
    std::cerr << "Error: Mps_Tree empty in rootfile "<<fileName.Data()<<
      ", Check run number and prefix."<<std::endl;
    return;
  }
  if( !helBranchArray ) {
    std::cerr << "Error: Hel_Tree empty in rootfile "<<fileName.Data()<<
      ", Check run number and prefix."<<std::endl;
    return;
  }

  // Get a list of entries in each branch (not necessarily the same)
  UInt_t mpsNumberOfEntries = mpsBranchArray->GetEntriesFast();
  UInt_t helNumberOfEntries = helBranchArray->GetEntriesFast();

  // Extract the names of all the branches in each Tree
  FindBranchNames(mpsBranchArray,&mpsBranches,mpsNumberOfEntries);
  FindBranchNames(helBranchArray,&helBranches,helNumberOfEntries);

  // Now process the branch
  ProcessBranch(mpsChain,mpsBranches,mpsNumberOfEntries);
  ProcessBranch(helChain,helBranches,helNumberOfEntries);

  // Relax!
  return;
}
///////////////////////////////////////////////////////////////////////////////
void FindBranchNames( TObjArray *array, std::vector<std::string> *names,
    UInt_t entries)
{
  // Get the names
  names->reserve(entries);
  for( UInt_t entry = 0; entry < entries; entry++ ) {
    if(array)
      names->push_back(((TObject*)array->At(entry))->GetName());
  }
}
///////////////////////////////////////////////////////////////////////////////
void ProcessBranch( TChain *chain, std::vector<std::string> names,
    UInt_t entries)
{
  // We create a temporary to draw on
  TCanvas *canvas = new TCanvas("canvas","All Stored Branches");

  std::cout<<"============================================================"
    <<"================================================"<<std::endl
    <<"============================================================"
    <<"================================================"<<std::endl
    <<"The following is a summary of tree: "<<chain->GetName()<<std::endl;

  // Now draw each entry and extract information to print
  TH1F *htemp;
  UInt_t hEntries;
  Double_t mean,rms,min,max;
  char compare_output[1000];
  TString draw;
  for( UInt_t entry = 0; entry < entries; entry++ ) {
    if( entry %kPrintHeaderEvery == 0 ) {
      std::cout<<"============================================================"
        <<"================================================"<<std::endl
        <<"Name                       \tEntries \t    Mean \t    RMS \t"
        <<"     Min \t     Max"<<std::endl
        <<"............................................................"
        <<"................................................"<<std::endl;
    }
    canvas->cd(entry+1);
    draw.Clear();
    draw = names[entry]+">>htemp";
    chain->Draw(draw.Data());
    htemp = (TH1F*)gDirectory->Get("htemp");
    if( htemp) {
      hEntries = htemp->GetEntries();
      mean = htemp->GetMean();
      rms = htemp->GetRMS();
      min = htemp->GetMinimum();
      max = htemp->GetMaximum();
      htemp->Clear();
    } else {
      std::cout << "Error! Lost histogram for " << names[entry] << std::endl;
    }
    sprintf(compare_output,"%-30s \t%7d \t%#6.6e \t%#6.6e \t%#6.6e \t%#6.6e",
        names[entry].c_str(),hEntries,mean,rms,min,max);
    std::cout<<compare_output<<std::endl;
  }

  // Clean up
  canvas->Clear();
  delete canvas;
}

