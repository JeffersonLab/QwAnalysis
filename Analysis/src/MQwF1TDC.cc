/**********************************************************\
* File: MQwF1TDC.C                                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha,                *
*         J. H. Lee                                        *
* Time-stamp: <2010-04-12 12:12>                           *
\**********************************************************/

#include "MQwF1TDC.h"
#include "QwColor.h"
#include "QwLog.h"


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
  fF1ROCNumber          = 0;
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
  fF1FirstWordFlag      = kFALSE;

};

MQwF1TDC::~MQwF1TDC() { };


void MQwF1TDC::DecodeTDCWord(UInt_t &word, const UInt_t roc_id)
{
  fF1ROCNumber  = roc_id;
  fF1SlotNumber = (word & kF1Mask_SlotNumber)>>27;

  if( fF1SlotNumber>=1 && fF1SlotNumber<=21 ) fF1ValidDataSlotFlag = kTRUE;
  else                                        fF1ValidDataSlotFlag = kFALSE;

  if(fF1ValidDataSlotFlag) {

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

    // Do we need to check Hit and Output FIFO? (jhlee Fri Apr 30 15:37:59 EDT 2010)
    // if there is a resolution lock fail, the warning message will be printed,
    // But it will be recovered by F1TDC itself, thus we treat an event with "resolution
    // lock failed as an invalid event.
    
    //    PrintHitFIFOStatus(roc_id);
    //    PrintOutputFIFOStatus(roc_id);
    //    PrintResolutionLockStatus(roc_id);
  }
  else {
    
  }


  return;
};


void MQwF1TDC::PrintTDCHeader(Bool_t flag)
{
  if(flag) {
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



void MQwF1TDC::PrintResolutionLockStatus(const UInt_t roc_id)
{
  if(!fF1ResolutionLockFlag) {
    QwWarning << "F1TDC board RESOULTION LOCK FAIL at Ch " 
	      << GetTDCChannelNumber() << " ROC " << roc_id
	      << " Slot " << GetTDCSlotNumber() << QwLog::endl;
  }
  return;
};



void MQwF1TDC::PrintHitFIFOStatus(const UInt_t roc_id)
{
  if(fF1HitFIFOFlag) {
    QwWarning << "F1TDC board HIT FIFO FULL at Ch "
              << GetTDCChannelNumber() << " ROC " << roc_id
              << " Slot " << GetTDCSlotNumber() << QwLog::endl;
  }
  return;
};



void MQwF1TDC::PrintOutputFIFOStatus(const UInt_t roc_id)
{
  if(fF1OutputFIFOFlag) {
    QwWarning << "F1TDC board OUTPUT FIFO FULL at Ch "
              << GetTDCChannelNumber() << " ROC " << roc_id
              << " Slot " << GetTDCSlotNumber() << QwLog::endl;
  }
  return;
};


Bool_t MQwF1TDC::IsValidDataword()
{
  // fF1ValidDataSlotFlag = TRUE,
  // fF1ResolutionFlag    = TRUE, and 
  // fF1HeaderFlag        = FALSE, then it is a data word.

  if( fF1ValidDataSlotFlag && fF1ResolutionLockFlag && !fF1HeaderFlag )
    return kTRUE;
  else                                             
    return kFALSE; 
};



Bool_t MQwF1TDC::CheckDataIntegrity(UInt_t ref_event_number, UInt_t ref_trigger_time)
{
  const Int_t valid_trigger_time_offset = 1;
  
  Bool_t event_ok_flag     = kFALSE;
  Bool_t trig_time_ok_flag = kFALSE;

  event_ok_flag     = (ref_event_number == fF1HeaderEventNumber);
  trig_time_ok_flag = abs(ref_trigger_time - fF1HeaderTriggerTime) <= valid_trigger_time_offset;
  

  // Trigger Time difference of up to 1 count among the chips is acceptable
  // For the Trigger Time, this assumes that an external SYNC_RESET signal has
  // been successfully applied at the start of the run
  
  if(! trig_time_ok_flag ) {
    QwWarning  << QwColor(Qw::kBlue)
	       << "There are SYNC_RESET issue on the F1TDC board at Ch "<<  fF1ChannelNumber
	       << " ROC " << fF1ROCNumber << " Slot " << fF1SlotNumber <<"\n";
    QwWarning  << QwColor(Qw::kBlue)
	       << "This event is excluded from the ROOT data stream.\n";
    //    QwWarning  << QwColor(Qw::kBlue)
    //	       << "Please contact (a) Qweak DAQ expert(s) as soon as possible.\n";
    QwWarning  << QwLog::endl;
  }

  // Any difference in the Event Number among the chips indicates a serious error
  // that requires a reset of the board.

  if( !event_ok_flag ) {
    QwWarning << QwColor(Qw::kRed)
	      << "There are Event Number Mismatch issue on the F1TDC board at ROC "<<  fF1ROCNumber
	      << " Slot " << fF1SlotNumber << "\n";
    QwWarning << QwColor(Qw::kRed) 
	      << "This event is excluded from the ROOT data stream.\n";
    //    QwWarning << QwColor(Qw::kRed) 
    //	      << "Please contact (a) Qweak DAQ expert(s) as soon as possible.\n";
    QwWarning << QwLog::endl;
    
  }
  return (event_ok_flag && trig_time_ok_flag);

};

