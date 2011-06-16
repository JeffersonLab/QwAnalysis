///////////////////////////////////////////////////////////////
// TaRedAna.hh
//    - Class that handles all analyses
//      
//  

#ifndef TAREDANA_HH
#define TAREDANA_HH


#include "Rtypes.h"
#include "TTree.h"
#include "TObject.h"
#include <string>
#include "VaAna.hh"
#include "TaConfFile.hh"
#include "TaInput.hh"

//  class TaConfFile;
//  class VaAna;
//  class TaInput;

class TaRedAna {

  ClassDef(TaRedAna,0) // regression or dithering analysis

public:

  TaRedAna ();
  TaRedAna (TString);

  virtual ~TaRedAna ();

  Int_t Init (Int_t);
  Int_t Init (string);
  Int_t CommonInit ();
  void Process ();
  void End ();

  //Data Access Functions
  //  const TaConfFile& GetConfigFile() const { return *fConfig; }


  VaAna*      fAnalysis;
private:
  TaConfFile* fConfig;
  TaInput*    fInput;
  TString     fConfFileName;
};

#endif
