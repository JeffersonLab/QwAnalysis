//////////////////////////////////////////////////////////////////////
//
// TaVarChooser.h
//
//  Class that reads in a file, and provides utilities to determine
//   which variables are to be included in the pair-summary 
//   rootfiles
//
//////////////////////////////////////////////////////////////////////

#ifndef TaVarChooser_h
#define TaVarChooser_h

#include <TString.h>
#include <vector>

using namespace std;

class TaVarChooser {

 public :
  TaVarChooser(TString filename);
  virtual ~TaVarChooser() {};
  vector <TString> GetBatteries() {return opt_batteries;};
  vector <TString> GetDitBPMs() {return opt_ditbpms;};
  vector <TString> GetBPMs() {return opt_bpms;};
  Bool_t GetHallBPMSat() {return opt_hallbpms_sat;};
  vector <TString> GetBCMs() {return opt_bcms;};
  UInt_t GetFLumi() {return opt_flumi;};
  UInt_t GetBLumi() {return opt_blumi;};
  UInt_t GetHe4Detectors() {return opt_he4detectors;};
  UInt_t GetLH2Detectors() {return opt_lh2detectors;};
  UInt_t GetBMW() {return opt_bmw_words;};
  void   Print();

 private :
  void ParseLine(vector <string>);
  ifstream *fVarFile;
  Bool_t isRead;
  vector <TString> opt_batteries;
/*   vector <TString> opt_cavities; */
  vector <TString> opt_ditbpms;
  vector <TString> opt_bpms;
  UInt_t opt_hallbpms_sat;
  vector <TString> opt_bcms;
  UInt_t opt_flumi;
  UInt_t opt_blumi;
  UInt_t opt_he4detectors;
  UInt_t opt_lh2detectors;
  UInt_t opt_bmw_words;

};

#endif
