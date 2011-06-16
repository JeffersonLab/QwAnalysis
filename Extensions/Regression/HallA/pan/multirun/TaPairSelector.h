//////////////////////////////////////////////////////////////////////
//
// TaPairSelector.h
//
// Class similar to the ROOT TSelector class.
//  Utility to loop through TTree/TChain branch entries
//  for use in creating pair summary rootfiles
//
//////////////////////////////////////////////////////////////////////

#ifndef TaPairSelector_h
#define TaPairSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <multirun/TaVarChooser.h>
#include <TString.h>
#include <iostream>
#include <vector>

#ifndef MAXDOUBLEDATA
#define MAXDOUBLEDATA 200
#endif

#ifndef MAXINTDATA
#define MAXINTDATA    10
#endif

using namespace std;

class TaPairSelector {

 public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent;
   TaVarChooser   *fChooser;  // Currently only use the chooser for
                              // BPM saturation cuts... but keep it
                              // here for future improvements.

   Int_t           ok_cut;
   Bool_t          isPanTree;
   Bool_t          doBPMSat_cuts;
   Double_t        evt_bpm4amx[2];
   Double_t        evt_bpm4bmx[2];
   Double_t        evt_bpm12mx[2];
   /*    Double_t        evt_bmwcyc[2]; */ // Removed until someone tells
                                           // me to put it back!


   Double_t doubleData[MAXDOUBLEDATA];
   Int_t    intData[MAXINTDATA];
   Bool_t   doubleIsGood[MAXDOUBLEDATA];
   Bool_t   intIsGood[MAXINTDATA];

   TaPairSelector(TTree *tree,
		  vector <TString> doublevars,
		  vector <TString> intvars, 
		  TaVarChooser& chooser);
   virtual ~TaPairSelector();
   Long64_t   GetEntry(Long64_t entry);
   Int_t      LoadTree(Long64_t entry);
   void       Init(TTree *tree,
		   vector <TString> doublevars,
		   vector <TString> intvars);
   void       Loop();
   Bool_t     Notify();
   void       Show(Long64_t entry = -1);
   Bool_t     ProcessCut();
   Bool_t     ProcessLoad(Long64_t entry);
   void       SetChooser(TaVarChooser& varchooser) {fChooser = &varchooser;};
   Double_t   GetDoubleData(UInt_t index) 
   {return doubleData[index];};

   ClassDef(TaPairSelector,0);
};

#endif

