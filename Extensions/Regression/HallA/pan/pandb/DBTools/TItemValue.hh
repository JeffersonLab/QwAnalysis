
#ifndef __TITEMVALUE__
#define  __TITEMVALUE__

#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip.h>

// ROOT includes

#include "TROOT.h"
#include "TObject.h"
#include "TString.h"



class TItemValue{
  
public:
  
  Int_t    fItemValueId;
  Int_t    fItemId;
  Int_t    fRun;
  TString  fAuthor;
  TString  fTime;
  TString  fComment;
  
  
  TItemValue();
  
  TItemValue(Int_t fpItemValueId, Int_t fItemId, Int_t fpRun,
	     const char *fpAuthor,
	     const char *fpTime,
	     const char *fpComment);
  
  virtual ~TItemValue();
  
  void Set(Int_t fpItemValueId, Int_t fpItemId, Int_t fpRun,
	   const char *fpAuthor,
	   const char *fpTime,
	   const char *fpComment);
  
  //  void  Print();
  
  //
  // This routines return lists of variables 
  // to include in QUERIES
  //
  
  //  const char *Value_List();
  //  const char *Equal_List();

  
  ClassDef(TItemValue,1) // Class for DB Item Value 
    
    };

#endif
