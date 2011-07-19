///////////////////////////////////////////////////////////////
// TaRedAna.C
//    - Class that handles all analyses
//      
//  
//#define NOISY
//#define DEBUG

#include "Rtypes.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <iostream>

#include "TaRedAna.hh"
#include "TaRegression.hh"
#include "TaDitAna.hh"

ClassImp(TaRedAna);

// Constructors/destructors/operators                                         
TaRedAna::TaRedAna():
  fAnalysis(0),
  fConfig(0),
  fInput(0)
{}

TaRedAna::TaRedAna(TString usrConfFileName):
  fAnalysis(0),
  fConfig(0),
  fInput(0)
{
  fConfFileName = usrConfFileName;
}

TaRedAna::~TaRedAna() {}

Int_t TaRedAna::Init (Int_t runnumber)
{
#ifdef DEBUG
  cout << "TaRedAna::Init()" << endl;
#endif

  if(fConfFileName.IsNull()) {
    fInput = new TaInput();
  } else {
    fInput = new TaInput(fConfFileName);
  }
  if (fInput->Init(runnumber) != 0)
    return 1;

  return CommonInit();
}

Int_t TaRedAna::Init (string runfile)
{
#ifdef DEBUG
  cout << "TaRedAna::Init()" << endl;
#endif

  fInput = new TaInput();
  if (fInput->Init(runfile) != 0) 
    return 1;

  return CommonInit();
}

Int_t TaRedAna::CommonInit ()
{
  TString theAnaType = fInput->GetConfig().GetAnaType();

  if(theAnaType == "regress") {
    fAnalysis = new TaRegression;
    fAnalysis->Init(*fInput);

  } else if(theAnaType == "dither") {

    fAnalysis = new TaDitAna();
    fAnalysis->Init(*fInput);

  } else {
    cout << "TaRedAna::Init() ERROR: " << theAnaType << "not recognized."
	 << endl;
    return 1;
  }
 
  return 0;
}

void TaRedAna::Process () 
{
#ifdef DEBUG
  cout << "TaRedAna::Process()" << endl;
#endif
  fAnalysis->Process();
}

void TaRedAna::End ()
{
#ifdef DEBUG
  cout << "TaRedAna::End()" << endl;
#endif
  fAnalysis->End();
  //  delete fPanFile;
  //  delete fConfig;
  delete fAnalysis;

}

