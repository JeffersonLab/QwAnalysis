#ifndef __QwRunCondition__
#define __QwRunCondition__
/**
 *  \file   QwRunCondition.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Sunday, September  5 01:12:26 EDT 2010
 */


#include <unistd.h>
#include <iostream>
#include <fstream>


#include "TList.h"
#include "TObjString.h"

class QwRunCondition
{

 public:
  QwRunCondition(){fRunConditionList=NULL;};
  QwRunCondition(Int_t argc, Char_t* argv[]);
  virtual ~QwRunCondition();

  TList* Get();
  
  void SetROCFlagsFileName(TString in) {fROCFlagFileName = in;};

 private:
  
  void SetRunCondition(Int_t argc, Char_t* argv[]);

  void Set(TString in);
  
  TString GetSvnRevision();
  TString GetROCFlags();
  

  TList *fRunConditionList;
  static const Int_t fCharLength;
  TString fROCFlagFileName;

  ClassDef(QwRunCondition,1);
};


#endif
