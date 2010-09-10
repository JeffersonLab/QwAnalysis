#ifndef __QwRunCondition__
#define __QwRunCondition__
/**
 *  \file   QwRunCondition.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Thursday, September  9 21:42:30 EDT 2010
 */


#include <unistd.h>
#include <iostream>
#include <fstream>

#include "RVersion.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include "TTimeStamp.h"

class QwRunCondition
{

 public:
  QwRunCondition(){fRunConditionList=NULL;};
  QwRunCondition(Int_t argc, Char_t* argv[]);
  virtual ~QwRunCondition();

  TList* Get();
  void SetROCFlagsFileName(TString in) {fROCFlagFileName = in;};

 private:
  
  void Set(Int_t argc, Char_t* argv[]);
  void Add(TString in);
  
  TString GetROCFlags();
  
  TList *fRunConditionList;
 
  TString fROCFlagFileName;
  static const Int_t fCharLength;

  //  ClassDef(QwRunCondition,1);
};


#endif
