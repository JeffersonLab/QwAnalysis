#define TaPairSelector_cxx
//////////////////////////////////////////////////////////////////////
//
// TaPairSelector.C
//
// Class similar to the ROOT TSelector class.
//  Utility to loop through TTree/TChain branch entries
//  for use in creating pair summary rootfiles
//
//////////////////////////////////////////////////////////////////////

#include "TaPairSelector.h"

TaPairSelector::TaPairSelector(TTree *tree,
			       vector <TString> doublevars,
			       vector <TString> intvars,
			       TaVarChooser& chooser) 
{
  if (tree !=0) {
    SetChooser(chooser);
    Init(tree,doublevars,intvars);
  }
}

TaPairSelector::~TaPairSelector()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Long64_t TaPairSelector::GetEntry(Long64_t entry)
{
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

Int_t TaPairSelector::LoadTree(Long64_t entry)
{
  if(!fChain) return -5;
  Int_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (fChain->IsA() != TChain::Class()) return centry;
  TChain *chain = (TChain*)fChain;
  if (chain->GetTreeNumber() != fCurrent) {
    fCurrent = chain->GetTreeNumber();
    Notify();
  }
  return centry;
}

void TaPairSelector::Init(TTree *tree, 
			  vector <TString> doublevars,
			  vector <TString> intvars)
{
  // Initialization of the TaPairSelector.
  //  For the provided tree, setup the branch address for 
  //  each of the indicated doublevars and intvars.

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   TString treename = tree->GetName();

   printf("TaPairSelector::Init  found %s tree\n",treename.Data());

   if(treename == "P" || treename == "M") isPanTree=kTRUE;
   else isPanTree=kFALSE;

   if(doublevars.size()>MAXDOUBLEDATA) {
     cerr << "Error... doublevars.size > MAXDOUBLEDATA" << endl;
     return;
   }
   if(intvars.size()>MAXINTDATA) {
     cerr << "Error... intvars.size > MAXINTDATA" << endl;
     return;
   }

   for(UInt_t i=0; i<doublevars.size(); i++) {
     TLeaf* tl = fChain->GetLeaf(doublevars[i]);
     if (tl) {
       fChain->SetBranchAddress(doublevars[i],&doubleData[i]);
       doubleIsGood[i]=kTRUE;
     } else {
       cout << "\n **** No data found for " << doublevars[i] << endl;
       doubleData[i]=-1e-6;
       doubleIsGood[i]=kFALSE;
     }
   }
   
   for(UInt_t i=0; i<intvars.size(); i++) {
     TLeaf* tl = fChain->GetLeaf(intvars[i]);
     if (tl) {
       fChain->SetBranchAddress(intvars[i],&intData[i]);
       intIsGood[i]=kTRUE;
     } else {
       cout << "/n **** No data found for " << intvars[i] << endl;
       intData[i]= -1000000;
       intIsGood[i]=kFALSE;
     }
   }
   if(isPanTree) {
     fChain->SetBranchAddress("ok_cut",&ok_cut);
     /*      fChain->SetBranchAddress("evt_bmwcyc",evt_bmwcyc); */
     if(fChooser->GetHallBPMSat()) {
       doBPMSat_cuts=kTRUE;
       fChain->SetBranchAddress("evt_bpm4amx",evt_bpm4amx);
       fChain->SetBranchAddress("evt_bpm4bmx",evt_bpm4bmx);
       fChain->SetBranchAddress("evt_bpm12mx",evt_bpm12mx);
     } else
       doBPMSat_cuts=kFALSE;
   }
   Notify();

   fChain->SetBranchStatus("*",0);  // disable all branches
   for(UInt_t i=0; i<doublevars.size(); i++) {
     if (doubleIsGood[i]) {
       fChain->SetBranchStatus(doublevars[i],1);
     }
   }
   for(UInt_t i=0; i<intvars.size(); i++) {
     if (intIsGood[i]) {
       fChain->SetBranchStatus(intvars[i],1);
     }
   } 
   if(isPanTree) {
     fChain->SetBranchStatus("ok_cut",1);
     if(doBPMSat_cuts) {
       fChain->SetBranchStatus("evt_bpm4amx",1);
       fChain->SetBranchStatus("evt_bpm4bmx",1);
       fChain->SetBranchStatus("evt_bpm12mx",1);
     }
     fChain->SetBranchStatus("evt_bmwcyc",1);
   }
}

Bool_t TaPairSelector::Notify()
{
  cout << "Processing file: "
       << fChain->GetCurrentFile()->GetName() << endl;

   return kTRUE;
}

void TaPairSelector::Show(Long64_t entry)
{
  if (!fChain) return;
  fChain->Show(entry);
}

Bool_t TaPairSelector::ProcessCut()
{
  // Process the cuts for this event.

  // PAN Cuts
  if(isPanTree) {
    if(!ok_cut) return kFALSE;

    // BPM saturation cuts
    if(doBPMSat_cuts) {
      if(evt_bpm12mx[0]>=60000 || evt_bpm12mx[1]>=60000) return kFALSE;
      if(evt_bpm4amx[0]>=60000 || evt_bpm4amx[1]>=60000) return kFALSE;  
      if(evt_bpm4bmx[0]>=60000 || evt_bpm4bmx[1]>=60000) return kFALSE;  
    }
//   if(evt_bmwcyc[0]!=0 || evt_bmwcyc[1]!=0) return kFALSE;  

  }
  return kTRUE;

}

Bool_t TaPairSelector::ProcessLoad(Long64_t entry) 
{
  // Check the leafs for this event.

  fChain->GetEntry(entry);
  //cout << entry << " ";
  return kTRUE;
}

void TaPairSelector::Loop() 
{

  if (fChain == 0) return;
  
  Long64_t nentries = Long64_t(fChain->GetEntriesFast());
  
  Int_t nbytes = 0, nb = 0;
  for (Int_t jentry=0; jentry<nentries;jentry++) {
    Int_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;
  }
  
}

