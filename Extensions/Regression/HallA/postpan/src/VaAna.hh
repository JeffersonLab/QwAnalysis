//****************************************************************
//
//    VaAna.hh - Abstract analysis class
//
//

#ifndef VAANA_HH
#define VAANA_HH

//#include "TString.h"
//#include "Rtypes.h"
#include "TTree.h"
#include "TFile.h"
#include "TObject.h"
//#include "TQObject.h"
//#include "RQ_OBJECT.h"

class TaConfFile;
//class TFile;
//class TTree;
class TaInput;
class TaOResultsFile;
class TaFileName;

class VaAna : public TObject{
  //  RQ_OBJECT()
  ClassDef(VaAna,0)

protected:

  //  TaConfFile* fConfig;
  TFile*      fRedFile;
  TTree*      fPairTree;
  TTree*      fRawTree;
  TaInput*    fInput;

 
public:
  VaAna();
  virtual ~VaAna();
  virtual Int_t Init (TaInput&);
  virtual void Process();
  virtual void End();
  virtual void OutputResults();

  TaOResultsFile* fTxtOut;


};

#endif
