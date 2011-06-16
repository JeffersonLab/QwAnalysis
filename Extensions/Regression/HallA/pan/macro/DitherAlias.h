//////////////////////////////////////////////////////////////////////
//
// DitherAlias.macro
//   Bryan Moffit - August 2005
//
//  A class to create aliases to a provided tree containing
//   corrections to detectors using dithering slopes
//


#ifndef DitherAlias_h
#define DitherAlias_h


#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <TString.h>
#include <TTree.h>
#include "RedCoefs.macro"
#include "src/TaFileName.hh"

using namespace std;

class DitherAlias
{

 public:
  DitherAlias (TTree *tree, UInt_t run);  
  DitherAlias (TTree *tree, const char *filename);
  DitherAlias (const char *filename);
  vector<string> GetMonNames();
  vector<Double_t> GetCorSlopes(TString);
  virtual ~DitherAlias();
  void MakeAliases_12xKludge(TTree *tree);
  void MakeIndividualAliases(TTree *tree);

 private:
  void Load();
  void MakeAliases();

  TTree *fTree;
  UInt_t fRun;
  const char *fDitResFile;
  vector <string> fMonName;
  vector <string> fDetName;
  map <string, vector<Double_t> > fSlope;

  ClassDef(DitherAlias,0);
};

#endif
