//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer PostPan
//
//           TaDitAna.hh
//
// Author:  K.D. Paschke
//
////////////////////////////////////////////////////////////////////////
//
//    Central object in the correction of asymmetry using the
//    dithering technique.  
//
////////////////////////////////////////////////////////////////////////

#ifndef TADITANA_HH
#define TADITANA_HH

#include "TFile.h"
#include "TObjArray.h"
#include "TString.h"
#include "TTree.h"
#include "VaAna.hh"
#include <vector>

#include "TaDitCyc.hh"

using namespace std;

class TaRootDitDef : public TObject {
  // Class to define variables within the reg Tree 
  // This is then stored in the root file
public:
  TaRootDitDef() {}
  ~TaRootDitDef() {}
  void PutDetector(TString name) { DetNames.push_back(name); }
  void PutMonitor(TString name) { MonNames.push_back(name); }
  void PutCoil(TString name) { CoilNames.push_back(name); }
  TString GetDetectorName(Int_t index) {
    return DetNames[index];
  }
  TString GetMonitorName(Int_t index) {
    return MonNames[index];
  }
  TString GetCoilName(Int_t index) {
    return CoilNames[index];
  }
  Int_t GetDetectorIndex(TString name) {
    for (UInt_t i=0; i<DetNames.size(); i++) {
      if (name == DetNames[i]) return i;
    }
    return -1;
  }
  Int_t GetMonitorIndex(TString name) {
    for (UInt_t i=0; i<MonNames.size(); i++) {
      if (name == MonNames[i]) return i;
    }
    return -1;
  }
  Int_t GetCoilIndex(TString name) {
    for (UInt_t i=0; i<CoilNames.size(); i++) {
      if (name == CoilNames[i]) return i;
    }
    return -1;
  }
  Int_t GetNumDetectors() { return DetNames.size(); }
  Int_t GetNumMonitors() { return MonNames.size(); }
  Int_t GetNumCoils() { return CoilNames.size(); }


  void Print() {
    //    cout << "TaRootDitDef::Print()" << endl;
    cout  << endl;
    for (UInt_t i=0; i<CoilNames.size(); i++) {
      cout << "Coil Index " << i << ": " << CoilNames[i] << endl;
    }
    cout  << endl;
    for (UInt_t i=0; i<MonNames.size(); i++) {
      cout << "Monitor Index " << i << ": " << MonNames[i] << endl;
    }
    cout  << endl;
    for (UInt_t i=0; i<DetNames.size(); i++) {
      cout << "Detector Index " << i << ": " << DetNames[i] << endl;
    }
    cout  << endl;
  }
  void Usage() {
    // Prints out some usage info
    cout << "This object serves as your guide to the ditslps tree." << endl;
    cout << endl 
	 << "TString TaRootDitDef::GetCoilName(Int_t index) -" << endl
	 << "  retrieves the Name of the Modulation Object "
	 << "stored at index.";
    cout << endl 
	 << "TString TaRootDitDef::GetMonitorName(Int_t index) -" << endl
	 << "  retrieves the Name of the Monitor "
	 << "stored at index.";
    cout << endl 
	 << "TString TaRootDitDef::GetDetectorName(Int_t index) -" << endl
	 << "  retrieves the Name of the Detector "
	 << "stored at index.";
    cout << endl << endl
	 << "Int_t TaRootDitDef::GetCoilIndex(TString name) -" << endl
	 << "  retrieves the index of the Modulation Object "
	 << "stored with name.";
    cout << endl << endl
	 << "Int_t TaRootDitDef::GetMonitorIndex(TString name) -" << endl
	 << "  retrieves the index of the Monitor "
	 << "stored with name.";
    cout << endl << endl
	 << "Int_t TaRootDitDef::GetDetectorIndex(TString name) -" << endl
	 << "  retrieves the index of the Detector "
	 << "stored with name.";
    cout << endl << endl;
  }
private:
  vector <TString> DetNames;
  vector <TString> MonNames;
  vector <TString> CoilNames;

  ClassDef(TaRootDitDef,1) // Class to define variables within reg tree
    
};


class TaDitAna : public VaAna{
      
  ClassDef(TaDitAna,0) // dithering analysis class

  private:

  // Data members
  static const Int_t fgNCoils=8;
  static const Int_t fgMxNDet=20;
  static const Int_t fgMxNMon=10;

 public:
  Int_t fNumCyc;       // number of supercycles in input data file.
  TaDitCyc* fProtoCyc; // prototype supercycle object
  TObjArray fDitCyc;   // array of TaDitCyc, one for each supercycle
  TaDitCyc* avgCyc;    // grand average TaDitCyc, merged from all others


  // Vectors for Monitor, Detector, and coil names
  vector <TString> DetList;  // Det Names.
  vector <TString> MonList;  // Mon Names.
  vector <TString> DitList;  // Dit Names.
  TString CurMon;            // current monitor name
  Bool_t fUseCurMon;            // Bool to flag use of current monitor
  Int_t DitIndex[fgNCoils];  // Index used in raw data to represent each coil
  Int_t DitMap[fgNCoils];    // pointer to used-coil list, 
                             // indexed by raw-data representation
  Double_t DitLimit[fgNCoils]; // limits for each coil, indexed by used coil

  // Vectors for cuts
  vector <TString> RwPanCuts;                 // Pan Cuts defined from Raw leafs

  Int_t fNDet;       // number of detectors used
  Int_t fNDit;       // number of coils used
  Int_t fNMon;       // number of monitors used
  Int_t fNRwCuts;    // number of cuts applied to raw tree leaves
  Int_t fNPairCuts;    // number of cuts applied to pair tree leaves


  // Vectors for Pair Tree variables
  vector <TString> DetVarList;  // Det asym Names.
  vector <TString> MonVarList;  // Mon diff Names.

  vector <TString> PrPanCuts; // Pan Cuts defined from Pair leafs

  TaDitCyc * curCyc;

  TaRootDitDef *rootdef;

  // Private methods

  //public:

  // Constructors/destructors/operators
  TaDitAna();
  virtual ~TaDitAna();

  // Major methods
  Int_t Init (TaInput&);         // Initialization with config file
  void Process ();               //  sets this mutha in motion
  void ComputeDitCoeffs ();      // calc dit coeffs and slopes from each S-C
  void OutputResults();         // generate text and rootfile output

  Bool_t LoadModData();
  Bool_t OutputSlopes();    // output slopes for each minirun
  Bool_t FillSlopesTree();  // put slopes for each minirun into tree
  Bool_t CorrectPairTree();  // fill ditpair tree with corrected asyms for all dets
  void PrintCycResults(Int_t);   // print slopes/coeffs from indicated cycle

  // Modifier Methods

  // Data access methods

  // Constants

  // Static members


};

#endif
