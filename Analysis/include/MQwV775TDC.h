/**********************************************************\
* File: MQwV775TDC.h                                       *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __MQwV775TDC__
#define __MQwV775TDC__

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
  void DecodeV775Word(UInt_t &word);

  Bool_t IsV775ValidData(){return fV775ValidFlag;};

  UInt_t GetV775SlotNumber(){return fV775SlotNumber;};
  UInt_t GetV775ChannelNumber(){return fV775ChannelNumber;};
  UInt_t GetV775Data(){return fV775Dataword;};
  
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

const UInt_t MQwV775TDC::kV775Mask_SlotNumber        = 0xf8000000;  // 27-31
const UInt_t MQwV775TDC::kV775Mask_WordType          = 0x07000000;  // 24-26

const UInt_t MQwV775TDC::kV775Mask_CrateNumber       = 0x00ff0000;  // 16-23
const UInt_t MQwV775TDC::kV775Mask_HitChannels       = 0x00003f00;  //  8-13

const UInt_t MQwV775TDC::kV775Mask_EventCounter      = 0x00FFFFFF;  //  0-23

const UInt_t MQwV775TDC::kV775Mask_ChannelNumber     = 0x001f0000;  // 16-20
const UInt_t MQwV775TDC::kV775Mask_DataValidBit      = 0x00004000;  // 14
const UInt_t MQwV775TDC::kV775Mask_UnderthresholdBit = 0x00002000;  // 13
const UInt_t MQwV775TDC::kV775Mask_OverflowBit       = 0x00001000;  // 12
const UInt_t MQwV775TDC::kV775Mask_Dataword          = 0x00000fff;  // 0-11

const UInt_t MQwV775TDC::kV775WordType_NotValid = 6;
const UInt_t MQwV775TDC::kV775WordType_Header   = 2;
const UInt_t MQwV775TDC::kV775WordType_Tail     = 4;
const UInt_t MQwV775TDC::kV775WordType_Datum    = 0;



void MQwV775TDC::DecodeV775Word(UInt_t &word){
  fV775SlotNumber = (word & kV775Mask_SlotNumber)>>27;
  UInt_t wordtype = (word & kV775Mask_WordType)>>24;
  if (wordtype == kV775WordType_Datum){
    fV775ValidFlag     = kTRUE;
    fV775ChannelNumber = (word & kV775Mask_ChannelNumber)>>16;
    /*     datavalid      = ((word & kV775Mask_DataValidBit)!=0); */
    /*     underthreshold = ((word & kV775Mask_UnderthresholdBit)!=0); */
    /*     overflow       = ((word & kV775Mask_OverflowBit)!=0); */
    fV775Dataword      = (word & kV775Mask_Dataword);
  } else {
    //  For now, don't distinguish between the header, tail word,
    //  or invalid data.
    //  Treat them all as invalid data.
    fV775ValidFlag     = kFALSE;
    fV775ChannelNumber = 0;
    fV775Dataword      = 0;
  }
};

#endif
