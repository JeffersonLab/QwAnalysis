/**********************************************************\
* File: QwQuartzBar.h                                      *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

///
/// \ingroup QwAnalysis_ADC

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
  QwQuartzBar(TString region_tmp):VQwSubsystemParity(region_tmp) { };
  ~QwQuartzBar() {
    DeleteHistograms();
  };

  /*  Member functions derived from VQwSubsystemParity. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString  filename);
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings 
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Bool_t CheckRunningAverages(Bool_t ); //check the running averages of sub systems and passing argument decide print AVG or not.

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();

  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ConstructHistograms(TDirectory *folder){
    TString tmpstr("");
    ConstructHistograms(folder,tmpstr);
  };

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  FillDB(QwDatabase *db, TString type) {return;};

  QwVQWK_Channel* GetChannel(const TString name);

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();
  Bool_t Compare(VQwSubsystem *source);


  VQwSubsystem&  operator=  ( VQwSubsystem *value);
  VQwSubsystem&  operator+= ( VQwSubsystem *value);
  VQwSubsystem&  operator-= ( VQwSubsystem *value);


  void Sum(VQwSubsystem *value1, VQwSubsystem *value2);
  void Difference(VQwSubsystem *value1,VQwSubsystem *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);


  Bool_t ApplyHWChecks(){//Check for harware errors in the devices
    Bool_t status = kTRUE;
    for (size_t i=0; i<fADC_Data.size(); i++){
      status &= fADC_Data.at(i).ApplyHWChecks();
    }
    return status;
  };




 protected:
  Bool_t fDEBUG;



  std::vector<QwVQWK_Module> fADC_Data;




};

#endif













