/**********************************************************\
* File: QwHelicity.h                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwHELICITY__
#define __QwHELICITY__

#include <vector>
#include <TTree.h>

#include "VQwSubsystem.h"

//#include "QwBeamLine.h"




/*****************************************************************
*  Class: 
******************************************************************/
class QwHelicity : public VQwSubsystem{
  /////
 public:
  
  QwHelicity(TString region_tmp):VQwSubsystem(region_tmp)
    {      
      fHelicityReported=0;
      fHelicityActual=0;
      fQuartetNumber=-1;
      fEventNumber=-1;
    };

  ~QwHelicity() 
    {
      DeleteHistograms();
    };
   /* derived from VQwSubsystem */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintDetectorID();

  void  ClearEventData();
  void  ProcessEvent();
 

  void SetHelicityReported(Int_t reported);
  void SetHelicityActual(Int_t actual);
  void SetQuartetNumber(Long_t quartet);
  void SetEventnumber(Long_t event);
  void SetAll(Int_t reported,Int_t actual,Long_t quartet,Long_t event);
  Int_t GetHelicityReported();
  Int_t GetHelicityActual();
  Long_t GetQuartetNumber();
  Long_t GetEventnumber();
  
  Bool_t MatchQuartetNumber(Long_t quartet);
  Bool_t MatchEventNumber(Long_t event);
  Bool_t MatchActualHelicity(Int_t actual);
  
  
  QwHelicity& operator=  (const QwHelicity &value);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);


/////
 protected:

 Int_t fHelicityReported;
 Int_t fHelicityActual;
 Long_t fQuartetNumber;
 Long_t fEventNumber;



 static const Bool_t kDEBUG;

};


const Bool_t QwHelicity::kDEBUG = kFALSE;


#endif
