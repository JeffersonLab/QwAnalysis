
/**********************************************************\
* File: MQwF1TDC.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __MQwF1TDC__
#define __MQwF1TDC__

#include "Rtypes.h"
#include "QwTypes.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>

///
/// \ingroup QwAnalysis
class MQwF1TDC{
  /******************************************************************
   *  Class: MQwF1TDC
   *         Mix-in class containing decoding functions for the F1TDC
   *         32-bit dataword.
   *         The functions in this class will decode a single word
   *         of F1TDC data and provide the components of the word
   *         through member functions.
   ******************************************************************/
 
  
 public:
  MQwF1TDC();
  ~MQwF1TDC();

  friend std::ostream& operator<<(std::ostream& stream, const MQwF1TDC f1tdc);

  void DecodeTDCWord(UInt_t &word, const UInt_t roc_id);

  Bool_t IsValidDataword();
  Bool_t IsHeaderword()            {return fF1HeaderFlag;};
 
  UInt_t GetTDCSlotNumber()        {return fF1SlotNumber;};
  UInt_t GetTDCChannelNumber()     {return fF1ChannelNumber;};
  UInt_t GetTDCData()              {return fF1Dataword;};
  UInt_t GetTDCMaxChannels()       {return fF1MaxChannelsPerModule;};
  
 
  UInt_t GetTDCEventNumber()       {return GetTDCHeaderEventNumber();};
  UInt_t GetTDCTriggerTime()       {return GetTDCHeaderTriggerTime();};
  
  /* void SetReferenceParameters(Double_t mindiff, Double_t maxdiff, */
  /* 			      Double_t offset, Double_t shift){ */
  /*   fMinDiff   = mindiff; */
  /*   fMaxDiff   = maxdiff; */
  /*   fOffset    = offset; */
  /*   fTimeShift = shift; */
  /* }; */
  
  /* Double_t SubtractReference(Double_t rawtime, Double_t reftime); */
  Double_t ActualTimeDifference(Double_t raw_time, Double_t ref_time);
  Bool_t CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words);
  void   PrintTDCHeader(Bool_t flag);
  void   PrintTDCData(Bool_t flag);
  void   Print(Bool_t flag);
  

 private:

  static const UInt_t kF1Mask_SlotNumber;
  static const UInt_t kF1Mask_HeaderFlag;
  static const UInt_t kF1Mask_ResolutionLockFlag;
  static const UInt_t kF1Mask_OutputFIFOFlag;
  static const UInt_t kF1Mask_HitFIFOFlag;
  static const UInt_t kF1Mask_ChannelNumber;
  static const UInt_t kF1Mask_Dataword;

  static const UInt_t kF1Mask_HeaderTrigFIFOFlag;
  static const UInt_t kF1Mask_HeaderEventNumber;
  static const UInt_t kF1Mask_HeaderTriggerTime;
  static const UInt_t kF1Mask_HeaderXorSetupFlag;
  static const UInt_t kF1Mask_HeaderChannelNumber;

  
  
  //  static const UInt_t offset;
  UInt_t fF1ROCNumber;

  Bool_t fF1HeaderFlag;              // true(1) if word is 0 (header) and false(0) 
                                     // if word is 1 (data)
  Bool_t fF1HitFIFOFlag;             // true(1) if word is 1 
  Bool_t fF1OutputFIFOFlag;          // true(1) if word is 1
  Bool_t fF1ResolutionLockFlag;      // true(1) if word is 1

  UInt_t fF1SlotNumber;
  UInt_t fF1ChannelNumber;
  UInt_t fF1Dataword;
  UInt_t fF1MaxChannelsPerModule;

  Bool_t fF1HeaderTrigFIFOFlag;
  UInt_t fF1HeaderEventNumber;
  UInt_t fF1HeaderTriggerTime;
  Bool_t fF1HeaderXorSetupFlag;
 
  
  Bool_t fF1OverFlowEntryFlag;
  Bool_t fF1ValidDataSlotFlag;  

  // Slot 1 - 21 indicates valid data
  // Slot 0  is the tag for a "filler" word. This is a non-valid data word that is
  //         inserted to make the block of data output from the module consist of
  //         an "even" number of words.
  // Slot 30 : the module will return 30 when the data is not valid
  // 
  //           Tempoararily comment out, because I have no idea how
  //           to check this inside MQwF1TDC so we don't interrupt 
  //           MQwV775TDC. I think "IsSlotRegistered()" can do instead of this
  //                   

  void   PrintHitFIFOStatus(const UInt_t roc_id);
  void   PrintOutputFIFOStatus(const UInt_t roc_id);
  void   PrintResolutionLockStatus(const UInt_t roc_id);

 

  UInt_t GetTDCHeaderEventNumber() {return fF1HeaderEventNumber;};
  UInt_t GetTDCHeaderTriggerTime() {return fF1HeaderTriggerTime;};


  //Bool_t IsValidDataSlot()         {return fF1ValidDataSlotFlag;};
 
  Bool_t IsHeaderXorSetup()        {return fF1HeaderXorSetupFlag;};
  Bool_t IsNotHeaderTrigFIFO()     {return !fF1HeaderTrigFIFOFlag;};

};

#endif
