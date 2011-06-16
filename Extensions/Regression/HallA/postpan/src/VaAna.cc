//****************************************************************
//
//    VaAna.C - Abstract analysis class
//
//
#define DEBUG
//#define NOISY
#include "VaAna.hh"
#include "TaConfFile.hh"
#include "TaRedAna.hh"
#include "TaInput.hh"
#include <iostream>
#include "TaFileName.hh"
#include "TaOResultsFile.hh"
#include "PanTypes.hh"
#include "TaOResultsFile.hh"

ClassImp(VaAna);

VaAna::VaAna():
  fRedFile(0),
  fPairTree(0),
  fRawTree(0),
  fInput(0)
{}

VaAna::~VaAna()
{}

Int_t VaAna::Init(TaInput& input) {
#ifdef DEBUG
  cout << "VaAna::Init()" << endl;
#endif
  fInput = &input;

  TString theAnaType = fInput->GetConfig().GetAnaType();
  // Get the PAN trees

  
#ifdef DEBUG
  //  cout << "VaAna cloning pair tree" << endl;
  cout << "VaAna getting pair tree" << endl;
#endif
  fPairTree = &(fInput->GetPairTree());

  if(theAnaType == "regress") {  // regress doesn't use raw tree
  } else {
#ifdef DEBUG
  cout << "VaAna getting raw tree" << endl;
  //  cout << "VaAna cloning raw tree" << endl;
#endif
  fRawTree = &(fInput->GetRawTree());
#ifdef NOISY
  fRawTree->Print();
#endif
  } 
#ifdef NOISY
  fPairTree->Print();
#endif
  Int_t runnumber = fInput->GetRunNumber();

  // Open the ReD root file.
  TaFileName::Setup(runnumber,theAnaType.Data());

  TaFileName redname = TaFileName("root",
				  fInput->GetConfig().GetCommentTag()+fInput->GetConfig().GetTreeType());
  cout << "Root output: " << redname.String().c_str() << endl;
  fRedFile = new TFile(redname.String().c_str(),"RECREATE");
  if (fRedFile->IsOpen() == kFALSE) {
    cerr << "Could not open Output file: " << redname.String().c_str() << endl;
    return 1;
  }


  // Open the results file.
  fTxtOut = new TaOResultsFile ("redana",
				runnumber,
				theAnaType.Data(),
				fInput->GetConfig().GetCksum(),
				fInput->GetConfig().GetCommentTag());

  return 0;

}

void VaAna::Process() {

#ifdef DEBUG
  cout << "VaAna::Process()" << endl;
#endif

}


void VaAna::OutputResults() {

#ifdef DEBUG
  cout << "VaAna::OutputResults()" << endl;
#endif

}

void VaAna::End() {

#ifdef DEBUG
  cout << "VaAna::End()" << endl;
#endif
  fRedFile->Write();
  fRedFile->Close();

}

