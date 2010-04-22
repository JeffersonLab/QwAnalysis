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


const UInt_t MQwF1TDC::kF1Mask_HeaderTrigFIFOFlag  = 0x00400000;
const UInt_t MQwF1TDC::kF1Mask_HeaderEventNumber   = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_HeaderTriggerTime   = 0x0000ff80;
const UInt_t MQwF1TDC::kF1Mask_HeaderXorSetupFlag  = 0x00000040;
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

  fF1HeaderTrigFIFOFlag = kFALSE;
  fF1HeaderEventNumber  = 0;
  fF1HeaderTriggerTime  = 0;
  fF1HeaderXorSetupFlag = kFALSE;

  fF1ValidDataSlotFlag  = kFALSE;

};

MQwF1TDC::~MQwF1TDC() { };


void MQwF1TDC::DecodeTDCWord(UInt_t &word)
{
  fF1SlotNumber         = (word & kF1Mask_SlotNumber)>>27;

  if( fF1SlotNumber>=1 && fF1SlotNumber<=21 ) fF1ValidDataSlotFlag = kTRUE;
  else                                        fF1ValidDataSlotFlag = kFALSE;

  // Should we need to decode "word" even if the data is invaid?
  // Thu Apr 22 11:41:16 EDT 2010 (jhlee)
  //  if(fF1ValidDataSlotFlag) {

  fF1HeaderFlag         = ((word & kF1Mask_HeaderFlag)==0); // TRUE if the mask bit IS NOT set

  // These three flags should be TRUE if their mask bit IS set
  fF1HitFIFOFlag        = ((word & kF1Mask_HitFIFOFlag       )!=0);
  fF1OutputFIFOFlag     = ((word & kF1Mask_OutputFIFOFlag    )!=0); 
  fF1ResolutionLockFlag = ((word & kF1Mask_ResolutionLockFlag)!=0);
  
  if (fF1HeaderFlag){
    //  This is a header word.
    fF1Dataword           = 0;
    fF1HeaderTrigFIFOFlag = ((word & kF1Mask_HeaderTrigFIFOFlag)!=0);
    fF1HeaderEventNumber  = ( word & kF1Mask_HeaderEventNumber )>>16;
    fF1HeaderTriggerTime  = ( word & kF1Mask_HeaderTriggerTime )>>7;
    fF1HeaderXorSetupFlag = ((word & kF1Mask_HeaderXorSetupFlag)!=0);
    fF1ChannelNumber      = ( word & kF1Mask_HeaderChannelNumber );
  } 
  else {
    // This is a data word.
    fF1ChannelNumber = (word & kF1Mask_ChannelNumber)>>16;
    fF1Dataword      = (word & kF1Mask_Dataword);
    fF1HeaderEventNumber   = 0;
    fF1HeaderTriggerTime   = 0;
    //std::cout << "channel: " << fF1ChannelNumber << " raw time: " << fF1Dataword << std::endl;
  }

  //}

  return;
};


void MQwF1TDC::PrintTDCHeader(Bool_t flag)
{
  if(flag)
    {
      printf(">>>>>>>>> H/T  : Ch %2d, Xor %1d, trOvF %1d, Chip(hitOvF,outOvF,resLock)(%1d%1d%1d) SlotID %2d Event # %d Trig Time %d\n",
	     fF1ChannelNumber, fF1HeaderXorSetupFlag, fF1HeaderTrigFIFOFlag, fF1HitFIFOFlag, fF1OutputFIFOFlag,  fF1ResolutionLockFlag,
	     fF1SlotNumber, fF1HeaderEventNumber, fF1HeaderTriggerTime
	     );
    }
  return;
};


void MQwF1TDC::PrintTDCData(Bool_t flag)
{
  if(flag)
    {
      printf(">>>>>>>>> DATA : Ch ");
      printf("%2d", fF1ChannelNumber);
      printf("%18s", "");
      printf("Chip(hitOvF,outOvF,resLock)(%1d%1d%1d) SlotID %2d",
	     fF1HitFIFOFlag, fF1OutputFIFOFlag,  fF1ResolutionLockFlag,
	     fF1SlotNumber);
      printf(", Raw time");
      printf("%12d\n", fF1Dataword);
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


