/**********************************************************\
* File: QwQuartzBar.h                                      *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWQUARTZBAR__
#define __QWQUARTZBAR__

#include <vector>
#include <TTree.h>

#include "VQwSubsystemParity.h"

#include "QwVQWK_Module.h"


class QwQuartzBar: public VQwSubsystemParity {
  /******************************************************************
   *  Class: QwQuartzBar
   *
   *
   ******************************************************************/
 public:
  QwQuartzBar(TString region_tmp);
  ~QwQuartzBar();

  /*  Member functions derived from VQwSubsystemParity. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();

  void  ConstructHistograms(TDirectory *folder){
    TString tmpstr("");
    ConstructHistograms(folder,tmpstr);
  };

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();
  Bool_t Compare(VQwSubsystem *source);


  VQwSubsystem&  operator=  ( VQwSubsystem *value);
  VQwSubsystem&  operator+= ( VQwSubsystem *value);
  VQwSubsystem&  operator-= ( VQwSubsystem *value);


  void Sum(VQwSubsystem *value1, VQwSubsystem *value2);
  void Difference(VQwSubsystem *value1,VQwSubsystem *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
  

  Bool_t IsGoodEvent(){
    Bool_t status = kTRUE;
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
	status &= fADC_Data.at(i)->IsGoodEvent();
      }
    }
    return status;
  };




 protected:
  Bool_t fDEBUG;

  

  std::vector<QwVQWK_Module*> fADC_Data;




};

#endif













