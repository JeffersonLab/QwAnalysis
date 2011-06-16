//****************************************************************
//
//  TaInput.hh
//     -  Handles the input files for ReD Analysis
//
#ifndef TAINPUT_HH
#define TAINPUT_HH

#include "Rtypes.h"
#include "TObject.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

//  class TFile;
//  class TaConfFile;
//  class TTree;

class TaInput : public TObject {
  ClassDef(TaInput,0) // input file class for ReD analysis

public:
  TaInput();
  TaInput(TString);
  virtual ~TaInput();
  Int_t Init(Int_t);
  Int_t Init(string);
  Int_t CommonInit();

  //Data Access Functions
  TaConfFile& GetConfig() const { return *fConfig; };
  TFile& GetPanFile() const { return *fPanFile; };
  Int_t GetRunNumber() { return fRunNumber; };
  TTree& GetRawTree() const { return *fRawTree; };
  TTree& GetPairTree() const { return *fPairTree; };

private:
  TFile*       fPanFile;
  TTree*       fRawTree;
  TTree*       fPairTree;
  TaConfFile*  fConfig;
  Int_t        fRunNumber;
  TString      fConfFileName;
  TString      fTreeName;


};

#endif
