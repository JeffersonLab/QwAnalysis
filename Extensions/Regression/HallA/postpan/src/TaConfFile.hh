///////////////////////////////////////////////////////////////
// TaConfFile.hh
//    - Configuration File Class for ReD analysis
//

#ifndef TACONFFILE_HH
#define TACONFFILE_HH

#include <string>
#include "TString.h"
#include "Rtypes.h"
#include "TCut.h"
#include "TObject.h"
#include <vector>

#define MAXCOIL 7

using namespace std;

class TaString;

class TaConfFile : public TObject {

  ClassDef(TaConfFile,0) // config file class for ReD analysis

public:
  TaConfFile();
  virtual ~TaConfFile();
  void Read();
  void Read(TString);
  void Load();
  
  // Data access functions
  TString GetAnaType() { return analysis; }
  TString GetTreeType() {return treetype; }
  //   - Regression Analysis specific(?)
  TString GetRegressType() { return regtype; }
  TString GetMatrixChoice() { return regmatrixchoice; }
  Int_t GetRegressMinirun() { return minirunevt; }
  Bool_t MinirunUsesCuts() { return usecuts; }
  Int_t GetRegressCycles() { return cycles; }
  
  Int_t GetIVNum() { return iv.size(); }
  Int_t GetDVNum() { return dv.size(); }
  
  TString GetIV(Int_t index) { return iv[index]; }
  TString GetDV(Int_t index) { return dv[index]; }
  vector <TString> GetIVNames() { return iv; }
  vector <TString> GetDVNames() { return dv; }
  
  Int_t GetDetNum() { return detList.size();}
  Int_t GetMonNum() { return monList.size();}
  Int_t GetDitNum() { return ditList.size();}
  TString GetDet(Int_t index) { return detList[index]; }
  TString GetMon(Int_t index) { return monList[index]; }
  TString GetDit(Int_t index) { return ditList[index];}
  TString GetCurMon() {return curmon;}
  Bool_t  UseCurMon() {return fUseCurMon;}

  vector <TString> GetDetList() { return detList; }
  vector <TString> GetMonList() { return monList; }
  vector <TString> GetDitList() { return ditList;}
  
  TString GetRootAnaType() {return rootanatype; }
  TString GetCommentTag() {return commenttag; }
  vector <TString> GetPanCuts() {return pancuts; }
  vector <TString> GetRawCuts() {return rawcuts; }
  vector <TString> GetPairCuts() {return paircuts; }
  pair <TString,TString> GetCustomCut() {return customcut; }

  UInt_t GetCksum () { return fCksum; }
  Int_t GetDitLimNum() { return ditLimList.size(); }
  vector <pair <TString,Double_t> > GetDitLimList() {return ditLimList;}
  Double_t GetDitLimit(TString ditobj) { 
    for (UInt_t i=0; i<ditLimList.size(); i++)
      if ((ditLimList[i].first).Contains(ditobj)) return ditLimList[i].second;
    return 1e6;
  }
private:
  void Print();
  void ParseLine(vector <string>);
  void LoadCksum (const string);
  ifstream *fConfFile;
  string   fFileName;
  UInt_t fCksum;          // Checksum
  TString analysis;       // Type of Analysis to Run
  TString regtype;        // Type of Regression Analysis to Run
  TString regmatrixchoice;// Decision to calc coeffs/regress quantities
  TString treetype;       // Type of Tree type to Run
  Int_t minirunevt;       // Number of pairs/minirun
  Bool_t usecuts;         // True of minirun is defined by pairs passing cuts
  Int_t cycles;           // Number of linear regression cycles.
  
  vector <TString> iv;    // String Array of independent variables
  vector <TString> dv;    // String Array of dependent variables
  
  vector <TString> pancuts;  // Defined PAN Cuts
  vector <TString> rawcuts;  // Defined PAN RawTree Cuts
  vector <TString> paircuts;  // Defined PAN PairTree Cuts
  TString rootanatype;    // string name of analysis type for input root file
  TString commenttag;     // comment tag for output files (root and res)
  
  vector <TString> monList;  // String Array of independent variables (monitors)
  vector <TString> detList;  // String Array of dependent variables  (detectors)
  vector <TString> ditList;  // String Array of dithering objects (coils + cavity)
  TString curmon; // String name of monitor used for current normalization
  Bool_t fUseCurMon;  // Bool to flag use of curmon
  pair <TString, TString> customcut;
  vector <pair <TString, Double_t> > ditLimList;  // 

};

#endif
