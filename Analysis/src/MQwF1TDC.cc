/**********************************************************\
* File: MQwF1TDC.C                                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha,                *
*         J. H. Lee                                        *
* Time-stamp: <2010-04-12 12:12>                           *
\**********************************************************/

#include "MQwF1TDC.h"


const UInt_t MQwF1TDC::kF1Mask_SlotNumber          = 0xf8000000;
const UInt_t MQwF1TDC::kF1Mask_HeaderFlag          = 0x00800000;
const UInt_t MQwF1TDC::kF1Mask_ResolutionLockFlag  = 0x04000000;
const UInt_t MQwF1TDC::kF1Mask_OutputFIFOFlag      = 0x02000000;
const UInt_t MQwF1TDC::kF1Mask_HitFIFOFlag         = 0x01000000;
const UInt_t MQwF1TDC::kF1Mask_ChannelNumber       = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_Dataword            = 0x0000ffff;

const UInt_t MQwF1TDC::kF1Mask_EventNumber         = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_TriggerTime         = 0x0000ff80;
const UInt_t MQwF1TDC::kF1Mask_HeaderChannelNumber = 0x0000003f;



MQwF1TDC::MQwF1TDC(): fMinDiff(-1.0*kMaxInt), fMaxDiff(1.0*kMaxInt), 
		      fOffset(0.0), fTimeShift(0.0)
{ 
  fF1HeaderFlag         = kFALSE;
  fF1HitFIFOFlag        = kFALSE;
  fF1OutputFIFOFlag     = kFALSE;
  fF1ResolutionLockFlag = kFALSE;

  fF1SlotNumber         = 0;
  fF1ChannelNumber      = 0;
  fF1Dataword           = 0;
  fF1EventNumber        = 0;
  fF1TriggerTime        = 0;

};

MQwF1TDC::~MQwF1TDC() { };


void MQwF1TDC::DecodeTDCWord(UInt_t &word)
{
  fF1SlotNumber         = (word & kF1Mask_SlotNumber)>>27;
  fF1HeaderFlag         = ((word & kF1Mask_HeaderFlag)==0); // TRUE if the mask bit IS NOT set

  // These three flags should be TRUE if their mask bit IS set
  fF1HitFIFOFlag        = ((word & kF1Mask_HitFIFOFlag)!=0);
  fF1OutputFIFOFlag     = ((word & kF1Mask_OutputFIFOFlag)!=0); 
  fF1ResolutionLockFlag = ((word & kF1Mask_ResolutionLockFlag)!=0);
  
  if (fF1HeaderFlag){
    //  This is a header word.
    fF1ChannelNumber = (word & kF1Mask_HeaderChannelNumber);
    fF1Dataword      = 0;
    fF1EventNumber   = (word & kF1Mask_EventNumber)>>16;
    fF1TriggerTime   = (word & kF1Mask_TriggerTime)>>7;
  } 
  else {
    fF1ChannelNumber = (word & kF1Mask_ChannelNumber)>>16;
    fF1Dataword      = (word & kF1Mask_Dataword);
    fF1EventNumber   = 0;
    fF1TriggerTime   = 0;
    //std::cout << "channel: " << fF1ChannelNumber << " raw time: " << fF1Dataword << std::endl;
  }

  return;
};


// UInt_t MQwF1TDC::SubtractReference(UInt_t rawtime, UInt_t a)
// {
//
//   //UInt_t  time_diff = 25600;
//   //UInt_t  offset    = 64495;
//   int del_t = 0;
//   int real_time=(int)rawtime;
//  
// 
//   del_t = (int)a-real_time;
//  
//   if( del_t < -30000 ) {
//     real_time = a + offset - real_time  ;
//   }
//   else if( del_t > 30000) {
//     real_time = a - real_time - offset ; 
//   }
//   else real_time = a - real_time;
//   real_time=real_time+8929;
//   return real_time;                // sign int to unsign int
// }

Double_t MQwF1TDC::SubtractReference(Double_t rawtime, Double_t reftime)
{
  //  Note that this produces the opposite sign of the corrected time
  //  as compared to the UInt_t version from revision 423 shown above.
  //
  //  For Region 3, according to the UInt_t version from revision 423,
  //  the internal parameters should be:
  //      fMinDiff   = -30000
  //      fMaxDiff   =  30000
  //      fOffset    =  64495
  //      fTimeShift =   8929
  //
  Double_t real_time = rawtime - reftime;  

  if( real_time < fMinDiff ) {
    real_time += fOffset;
  }
  else if( real_time > fMaxDiff) {
    real_time -= fOffset;
  }
  real_time = real_time + fTimeShift;
  return real_time;
}


