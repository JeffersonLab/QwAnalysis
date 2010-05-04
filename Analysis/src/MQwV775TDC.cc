/**********************************************************\
* File: MQwV775TDC.cc                                      *
*                                                          *
* Author: P. M. King                                       *
*         J. H. Lee                                        *
* Time-stamp: <2010-04-12 16:11>                           *
\**********************************************************/

#include "MQwV775TDC.h"
#include <iostream>


const UInt_t MQwV775TDC::kV775Mask_SlotNumber        = 0xf8000000;  // 27-31
const UInt_t MQwV775TDC::kV775Mask_WordType          = 0x07000000;  // 24-26

const UInt_t MQwV775TDC::kV775Mask_CrateNumber       = 0x00ff0000;  // 16-23
const UInt_t MQwV775TDC::kV775Mask_HitChannels       = 0x00003f00;  //  8-13

const UInt_t MQwV775TDC::kV775Mask_EventCounter      = 0x00ffffff;  //  0-23

const UInt_t MQwV775TDC::kV775Mask_ChannelNumber     = 0x001f0000;  // 16-20
const UInt_t MQwV775TDC::kV775Mask_DataValidBit      = 0x00004000;  // 14
const UInt_t MQwV775TDC::kV775Mask_UnderthresholdBit = 0x00002000;  // 13
const UInt_t MQwV775TDC::kV775Mask_OverflowBit       = 0x00001000;  // 12
const UInt_t MQwV775TDC::kV775Mask_Dataword          = 0x00000fff;  // 0-11

const UInt_t MQwV775TDC::kV775WordType_NotValid      = 0x110;//6
const UInt_t MQwV775TDC::kV775WordType_Header        = 0x010;//2
const UInt_t MQwV775TDC::kV775WordType_Tail          = 0x100;//4;
const UInt_t MQwV775TDC::kV775WordType_Datum         = 0x000;//0;


// See page 43 at https://qweak.jlab.org/wiki/images/V775.pdf

void MQwV775TDC::DecodeTDCWord(UInt_t &word, const UInt_t roc_id)
{
  
  fV775SlotNumber = (word & kV775Mask_SlotNumber)>>27;
  UInt_t wordtype = (word & kV775Mask_WordType)>>24;
  
  if      ( wordtype == kV775WordType_Datum    ) fV775ValidFlag  = kTRUE;
  else if ( wordtype == kV775WordType_Header   ) fV775HeaderFlag = kTRUE;
  else if ( wordtype == kV775WordType_Tail     ) fV775HeaderFlag = kFALSE;
  else if ( wordtype == kV775WordType_NotValid ) fV775ValidFlag  = kFALSE;
  else    // reserved (not used)

  if (fV775ValidFlag) {
    fV775ChannelNumber = (word & kV775Mask_ChannelNumber)>>16;
    fV775Dataword      = (word & kV775Mask_Dataword);
    fV775EventNumber   = 0;
    /*     datavalid      = ((word & kV775Mask_DataValidBit)!=0); */
    /*     underthreshold = ((word & kV775Mask_UnderthresholdBit)!=0); */
    /*     overflow       = ((word & kV775Mask_OverflowBit)!=0); */
    fV775Dataword      = (word & kV775Mask_Dataword);
    fV775EventNumber   = 0;
  } else {
    //  For now, don't distinguish between the header, tail word,
    //  or invalid data.
    //  Treat them all as invalid data.
    fV775ValidFlag     = kFALSE;
    fV775ChannelNumber = 0;
    fV775Dataword      = 0;
    fV775EventNumber   = 0;
  }
  else {
    fV775ChannelNumber = 0;
    fV775Dataword      = 0;
    fV775EventNumber   = 0;
  }
  
  return;
};

// UInt_t MQwV775TDC::SubtractReference(UInt_t a, UInt_t rawtime){
//   UInt_t b=rawtime;
//   return b;
// }

Double_t MQwV775TDC::SubtractReference(Double_t rawtime, Double_t reftime)
{
  Double_t real_time = rawtime - reftime;  
  return real_time;
}



UInt_t  MQwV775TDC::GetTDCTriggerTime()
{
  UInt_t trigger_time = 512; 
  // V775TDC has no trigger time information into its data stream.
  // To keep the same function name between V775TDC and F1TDC,
  // I use this function in order to return "trigger_time" as 512.
  // The valid trigger time of F1TDC is in the range of 0 - 511,
  // thus we can distinguish whether F1TDC or V775TDC.

  return trigger_time;
}
