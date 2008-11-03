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

#include "VQwSubsystem.h"

#include "QwVQWK_Module.h"


class QwQuartzBar: public VQwSubsystem {
  /******************************************************************
   *  Class: QwQuartzBar
   *
   *
   ******************************************************************/
 public:
  QwQuartzBar(TString region_tmp);
  ~QwQuartzBar();

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();

  void  ConstructHistograms(TDirectory *folder){
    TString tmpstr("");
    ConstructHistograms(folder,tmpstr);
  };
  void  ConstructHistograms(TDirectory *folder, TString prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);

  QwQuartzBar& operator=  (const QwQuartzBar &value);
  QwQuartzBar& operator+= (const QwQuartzBar &value);
  QwQuartzBar& operator-= (const QwQuartzBar &value);
  void Sum(QwQuartzBar &value1, QwQuartzBar &value2);
  void Difference(QwQuartzBar &value1, QwQuartzBar &value2);
  void Ratio(QwQuartzBar &numer, QwQuartzBar &denom);
  

  Bool_t IsGoodEvent(){
    Bool_t status = kTRUE;
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
	status &= fADC_Data.at(i)->IsGoodEvent();
      }
    }
    return status;
  };


   void GetHitList(QwHitContainer & grandHitContainer){
    
  }; //empty function


 protected:
/*   void Sum(QwQuartzBar &value1, QwQuartzBar &value2); */
/*   void Difference(QwQuartzBar &value1, QwQuartzBar &value2); */
/*   void Ratio(QwQuartzBar &numer, QwQuartzBar &denom); */


 protected:
  Bool_t fDEBUG;

  

  std::vector<QwVQWK_Module*> fADC_Data;




};

#endif
