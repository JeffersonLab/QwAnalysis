
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

 protected:
  /*
  void DecodeF1Word(UInt_t &word);
  

  Bool_t IsAF1Headerword(){return fF1HeaderFlag;};

  UInt_t GetF1SlotNumber(){return fF1SlotNumber;};
  UInt_t GetF1ChannelNumber(){return fF1ChannelNumber;};
  UInt_t GetF1Data(){return fF1Dataword;};
  */
  void DecodeTDCWord(UInt_t &word);

  Bool_t IsValidDataword(){return fF1HeaderFlag;};
  UInt_t GetTDCSlotNumber(){return fF1SlotNumber;};
  UInt_t GetTDCChannelNumber(){return fF1ChannelNumber;};
  UInt_t GetTDCData(){return fF1Dataword;};

  void SetReferenceParameters(Double_t mindiff, Double_t maxdiff,
			      Double_t offset, Double_t shift){
    fMinDiff   = mindiff;
    fMaxDiff   = maxdiff;
    fOffset    = offset;
    fTimeShift = shift;
  };

  //  UInt_t SubtractReference(UInt_t rawtime, UInt_t a);
  Double_t SubtractReference(Double_t rawtime, Double_t reftime);


 private:
  static const UInt_t kF1Mask_SlotNumber;
  static const UInt_t kF1Mask_HeaderFlag;
  static const UInt_t kF1Mask_ResolutionFlag;
  static const UInt_t kF1Mask_OutputFIFOFlag;
  static const UInt_t kF1Mask_HitFIFOFlag;
  static const UInt_t kF1Mask_ChannelNumber;
  static const UInt_t kF1Mask_Dataword;
  //  static const UInt_t offset;

  Bool_t fF1HeaderFlag;
  UInt_t fF1SlotNumber;
  UInt_t fF1ChannelNumber;
  UInt_t fF1Dataword;

  //  These variables are used in the SubtractReference routine.
  Double_t fMinDiff;     ///< Low edge of acceptable range of F1TDC channel time/reference time difference
  Double_t fMaxDiff;     ///< High edge of acceptable range of F1TDC channel time/reference time difference

  Double_t fOffset;      ///< Correction to move F1TDC channel time/reference time difference back into acceptable range.
                         ///  It will depend on the exact F1TDC configuration.

  Double_t fTimeShift;   ///< "Small" correction to all reference-time-subtracted times;
                         ///  NOTE:  this would preferrably be done by a different function than 
                         ///  MQwF1TDC::SubtractReference, but R3 has this correction designed in for now.

};



#endif
