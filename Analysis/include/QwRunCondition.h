#ifndef __QwRunCondition__
#define __QwRunCondition__
/**
 *  \file   QwRunCondition.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Friday, September  3 16:18:24 EDT 2010
 */


#include <unistd.h>
#include <iostream>

#include "TList.h"
#include "TObjString.h"
#include "TSystem.h"

class QwRunCondition
{

 public:
  QwRunCondition();
  //  QwRunCondition(Int_t argc, Char_t* argv[]);
  virtual ~QwRunCondition();

 
  TList* GetCondition();
  TList *fRunConditionList;
  void SetRunCondition();
  void Set(TString in);
  /* void SetRunCondition(Int_t argc, Char_t* argv[]); */

 private:

  // void AddObjStrings();

  //  TObjString *fRevision;
  //  TObjString *fProgName;
  //  TObjString *fRunNumber;
  //  TObjString *fHostName;
  //  TObjString *fUserName;
  //  TObjString *fCreatedDate;
  //  TObjString *fRunCommand;
  //  TObjString *fPath;

  //  TSystem fSystem;
 
  ClassDef(QwRunCondition,1);

};


#endif
