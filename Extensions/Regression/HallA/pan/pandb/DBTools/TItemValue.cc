
#include "TItemValue.hh"

ClassImp(TItemValue)
  
  
  TItemValue::TItemValue(){
  // default constructor
  //
}

TItemValue::~TItemValue(){
  // default destructure
  //
}

TItemValue::TItemValue(Int_t fpItemValueId, Int_t fpItemId, Int_t fpRun, 
		       const char *fpAuthor, const char *fpTime, const char *fpComment){
  // Constructor Sets Variables 
  //
  fItemValueId =  fpItemValueId;
  fItemId = fpItemId;
  fRun = fpRun;
  fAuthor = fpAuthor; 
  fTime = fpTime;
  fComment = fpComment;  
}

void TItemValue::Set(Int_t fpItemValueId, Int_t fpItemId, Int_t fpRun, 
		     const char *fpAuthor, const char *fpTime, const char *fpComment){
  // Sets Variables 
  //
  
  fItemValueId = fpItemValueId;
  fItemId = fpItemId;
  fRun = fpRun;
  fAuthor = fpAuthor; 
  fTime = fpTime;
  fComment = fpComment;
}

