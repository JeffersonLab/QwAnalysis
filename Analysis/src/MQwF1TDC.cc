/**********************************************************\
* File: MQwF1TDC.C                                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/

#include "MQwF1TDC.h"


MQwF1TDC::MQwF1TDC() { };
MQwF1TDC::~MQwF1TDC() { };

const UInt_t MQwF1TDC::kF1Mask_SlotNumber     = 0xf8000000;
const UInt_t MQwF1TDC::kF1Mask_HeaderFlag     = 0x00800000;
const UInt_t MQwF1TDC::kF1Mask_ResolutionFlag = 0x04000000;
const UInt_t MQwF1TDC::kF1Mask_OutputFIFOFlag = 0x02000000;
const UInt_t MQwF1TDC::kF1Mask_HitFIFOFlag    = 0x01000000;
const UInt_t MQwF1TDC::kF1Mask_ChannelNumber  = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_Dataword       = 0x0000ffff;
const UInt_t MQwF1TDC::offset                 = 64495;

void MQwF1TDC::DecodeTDCWord(UInt_t &word){
  fF1SlotNumber      = (word & kF1Mask_SlotNumber)>>27;
  fF1HeaderFlag      = ((word & kF1Mask_HeaderFlag)==0);
  if (fF1HeaderFlag){
    //  THis is a header word.
    fF1ChannelNumber = 0;
    fF1Dataword      = 0;
  } else {
/* 	resolution = ((buffer[i] & kF1Mask_ResolutionFlag)!=0); */
/* 	out_fifo   = ((buffer[i] & kF1Mask_OutputFIFOFlag)!=0); */
/* 	hit_fifo   = ((buffer[i] & kF1Mask_HitFIFOFlag)!=0); */
    fF1ChannelNumber = (word & kF1Mask_ChannelNumber)>>16;
    fF1Dataword      = (word & kF1Mask_Dataword);
  }
};


UInt_t MQwF1TDC::SubtractReference(UInt_t rawtime, UInt_t a)
{

  //UInt_t  time_diff = 25600;
  //UInt_t  offset    = 64495;
  int del_t = 0;
  int real_time=(int)rawtime;
  
 
  del_t = (int)a-real_time;
  
  if( del_t < -30000 ) {
    real_time = a + offset - real_time  ;
  }
  else if( del_t > 30000) {
    real_time = a - real_time - offset ; 
  }
  else real_time = a - real_time;
  real_time=real_time+8929;
  return real_time;                // sign int to unsign int
}


