/**********************************************************\
* File: MQwV775TDC.h                                       *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __MQwV775TDC__
#define __MQwV775TDC__

#include "Rtypes.h"

///
/// \ingroup QwAnalysis
class MQwV775TDC{
  /******************************************************************
   *  Class: MQwV775TDC
   *         Mix-in class containing decoding functions for the V775TDC
   *         32-bit dataword.
   *         The functions in this class will decode a single word
   *         of V775TDC data and provide the components of the word
   *         through member functions.
   *
   *         The decoding of the CAEN V792 ADC is nearly identical;
   *         the 792 does not use the "DataValidBit", so it is
   *         disabled in this version to allow this class to work
   *         for both the ADC and TDC.
   ******************************************************************/
 public:
  MQwV775TDC() { };
  ~MQwV775TDC() { };

 protected:
  //void DecodeV775Word(UInt_t &word);
  void DecodeTDCWord(UInt_t &word);
  /*
  Bool_t IsV775ValidData(){return fV775ValidFlag;};

  UInt_t GetV775SlotNumber(){return fV775SlotNumber;};
  UInt_t GetV775ChannelNumber(){return fV775ChannelNumber;};
  UInt_t GetV775Data(){return fV775Dataword;};
  */
  Bool_t IsValidDataword(){return fV775ValidFlag;};
  //Bool_t IsATDCHeaderword(){return kFALSE;};
  UInt_t GetTDCSlotNumber(){return fV775SlotNumber;};
  UInt_t GetTDCChannelNumber(){return fV775ChannelNumber;};
  UInt_t GetTDCData(){return fV775Dataword;};
  
 private:
  static const UInt_t kV775Mask_SlotNumber;
  static const UInt_t kV775Mask_WordType;

  static const UInt_t kV775Mask_CrateNumber;
  static const UInt_t kV775Mask_HitChannels;

  static const UInt_t kV775Mask_EventCounter;

  static const UInt_t kV775Mask_ChannelNumber;
  static const UInt_t kV775Mask_DataValidBit;
  static const UInt_t kV775Mask_UnderthresholdBit;
  static const UInt_t kV775Mask_OverflowBit;
  static const UInt_t kV775Mask_Dataword;

  static const UInt_t kV775WordType_NotValid;
  static const UInt_t kV775WordType_Header;
  static const UInt_t kV775WordType_Tail;
  static const UInt_t kV775WordType_Datum;

  Bool_t fV775ValidFlag;
  UInt_t fV775SlotNumber;
  UInt_t fV775ChannelNumber;
  UInt_t fV775Dataword;

};

#endif
