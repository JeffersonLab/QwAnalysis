//////////////////////////////////////////////////////////////////////
//
// TaMakePairFile.h
//
//  Class that compiles together many runs into a single rootfile
//
//////////////////////////////////////////////////////////////////////

#ifndef TaMakePairFile_h
#define TaMakePairFile_h

#include <multirun/TaPairSelector.h>
#include <multirun/TaVarChooser.h>
#include <src/TaFileName.hh>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TString.h>
#include <multirun/TaRunlist.h>
#include <macro/DitherAlias.h>
#include <multirun/HeDitOrganizer.h>
#include <multirun/HDitOrganizer.h>

#ifndef MAXDOUBLEDATA
#define MAXDOUBLEDATA 200
#endif

#ifndef MAXINTDATA
#define MAXINTDATA    10
#endif

#ifndef MAXDITDET
#define MAXDITDET 30
#endif

#ifndef MAXDITMON
#define MAXDITMON 10
#endif

using namespace std;

class TaMakePairFile {

 public:
  TaMakePairFile(TString rootfilename, TString chooserfilename, 
		 Bool_t ditherSwitch=kTRUE);
  virtual ~TaMakePairFile() {};
  void RunLoop();
  Bool_t SetRunList(TString listfilename);
  Bool_t SetRunList(vector <pair <UInt_t,UInt_t> > list);
  void Finish();
  void SetDBRunlist(TString filename) {fDBRunlist = new TaRunlist(filename);};
  void SetDoDit(Bool_t state=kTRUE) { doDit = state;};
  void SetBPM12Kludge() { fB12Kludge=kTRUE;};
  void GetKludge();
  void ConfigureDithering();

 private:
  TFile             *fRootFile;
  TTree             *fTree;
  TaPairSelector    *pairSelect;
  TaPairSelector    *regSelect;
  TaVarChooser      *fChooser;
  TString            fDitFilename;
  TString            fRunFilename;
  TaRunlist         *fDBRunlist;
  Bool_t             useDBRunlist;
  vector <TString>   doubleVars;
  vector <TString>   intVars;
  vector <TString>   doubleRegVars;
  vector <TString>   intRegVars;
  Double_t           doubleData[MAXDOUBLEDATA];
  Int_t              intData[MAXINTDATA];
  Double_t           doubleRegData[MAXDOUBLEDATA];
  Int_t              intRegData[MAXINTDATA];
  vector <UInt_t>    ditBPMindex;
  vector <UInt_t>    DETindex;
  vector <pair <UInt_t,UInt_t> > runlist; // first is run, second is slug

  vector <TString>   doubleDitVars;
  Double_t           doubleDitData[MAXDOUBLEDATA];
  Double_t           fSlopes[MAXDITDET][MAXDITMON];
  vector <TString>   ditMonStr;
  vector <TString>   ditDetSkel;
  vector <TString>   ditDetUD;
  UInt_t             ditMapMon[MAXDITMON];
  UInt_t             ditMapUD[MAXDITDET];
  vector <Bool_t>    ditOK;
  //  Bool_t             ditOK;
  Bool_t             doDit;
  Bool_t             fB12Kludge;
  UInt_t             targetType;

  // Some default leafs, not in the pan/redana rootfiles.
  Int_t runnumber;
  Int_t slowsign;
  Int_t slug;
  Int_t ok_Both;
  Int_t ok_Left;
  Int_t ok_Right;
  Int_t ok_regBoth, ok_regLeft, ok_regRight;
  Int_t pair_num;

  void EventLoop(Long64_t nevents);
  void MakeBranches();
  void PushToDoubleList(vector <TString>,TString,TString);
  void PushToDoubleList(vector <TString> thisvar,
			TString suffix) {
    PushToDoubleList(thisvar,suffix,""); };
  void PushToDitList(vector <TString>,TString);
  void MakeVarList();
  Bool_t isConfigured();

};

#endif
