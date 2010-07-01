
/**********************************************************\
* File: MQwF1TDC.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __MQwF1TDC__
#define __MQwF1TDC__

#include "Rtypes.h"
#include <cstdlib>
#include <iostream>

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

// protected:
  /*
  void DecodeF1Word(UInt_t &word);
  

  Bool_t IsAF1Headerword(){return fF1HeaderFlag;};

  UInt_t GetF1SlotNumber(){return fF1SlotNumber;};
  UInt_t GetF1ChannelNumber(){return fF1ChannelNumber;};
  UInt_t GetF1Data(){return fF1Dataword;};
  */
  void DecodeTDCWord(UInt_t &word, const UInt_t roc_id);

  Bool_t IsValidDataword();
  Bool_t IsHeaderword() {return fF1HeaderFlag;};
 
  UInt_t GetTDCSlotNumber()        {return fF1SlotNumber;};
  UInt_t GetTDCChannelNumber()     {return fF1ChannelNumber;};
  UInt_t GetTDCData()              {return fF1Dataword;};
  
 
  UInt_t GetTDCEventNumber()       {return GetTDCHeaderEventNumber();};
  UInt_t GetTDCTriggerTime()       {return GetTDCHeaderTriggerTime();};
  
  void SetReferenceParameters(Double_t mindiff, Double_t maxdiff,
			      Double_t offset, Double_t shift){
    fMinDiff   = mindiff;
    fMaxDiff   = maxdiff;
    fOffset    = offset;
    fTimeShift = shift;
  };
  
  Double_t SubtractReference(Double_t rawtime, Double_t reftime);
  
  Bool_t CheckDataIntegrity(UInt_t ref_event_number, UInt_t ref_trigger_time);

  // Two print functions are used for a debugging purpose temporarily 
  void   PrintTDCHeader(Bool_t flag);
  void   PrintTDCData(Bool_t flag);
  

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

  Bool_t fF1HeaderFlag;              // true(1) if word is 0 (header) and false(0) if word is 1 (data)
  Bool_t fF1HitFIFOFlag;             // true(1) if word is 1 
  Bool_t fF1OutputFIFOFlag;          // true(1) if word is 1
  Bool_t fF1ResolutionLockFlag;      // true(1) if word is 1

  UInt_t fF1SlotNumber;
  UInt_t fF1ChannelNumber;
  UInt_t fF1Dataword;

  Bool_t fF1HeaderTrigFIFOFlag;
  UInt_t fF1HeaderEventNumber;
  UInt_t fF1HeaderTriggerTime;
  Bool_t fF1HeaderXorSetupFlag;
 
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
  
  //  These variables are used in the SubtractReference routine.
  Double_t fMinDiff;     ///< Low edge of acceptable range of F1TDC channel time/reference time difference
  Double_t fMaxDiff;     ///< High edge of acceptable range of F1TDC channel time/reference time difference

  Double_t fOffset;      ///< Correction to move F1TDC channel time/reference time difference back into acceptable range.
                         ///  It will depend on the exact F1TDC configuration.

  Double_t fTimeShift;   ///< "Small" correction to all reference-time-subtracted times;
                         ///  NOTE:  this would preferrably be done by a different function than 
                         ///  MQwF1TDC::SubtractReference, but R3 has this correction designed in for now.

  Bool_t fF1FirstWordFlag;
/*   UInt_t fheader_old_event_number; */
/*   UInt_t fheader_old_trigger_time; */


 
  void   PrintHitFIFOStatus(const UInt_t roc_id);
  void   PrintOutputFIFOStatus(const UInt_t roc_id);
  void   PrintResolutionLockStatus(const UInt_t roc_id);

 

  UInt_t GetTDCHeaderEventNumber() {return fF1HeaderEventNumber;};
  UInt_t GetTDCHeaderTriggerTime() {return fF1HeaderTriggerTime;};


  //Bool_t IsValidDataSlot()         {return fF1ValidDataSlotFlag;};
  // at the moment we don't use the following functions to check
  // Xor setup and TrigFIFO status (Mon May  3 15:27:21 EDT 2010, jhlee)
  Bool_t IsHeaderXorSetup()        {return fF1HeaderXorSetupFlag;};
  Bool_t IsHeaderTrigFIFO()        {return fF1HeaderTrigFIFOFlag;};

};



#endif
