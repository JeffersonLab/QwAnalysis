/**********************************************************\
* File: MQwF1TDC.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __MQwF1TDC__
#define __MQwF1TDC__

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
  MQwF1TDC() { };
  ~MQwF1TDC() { };

 protected:
  void DecodeF1Word(UInt_t &word);

  Bool_t IsAF1Headerword(){return fF1HeaderFlag;};

  UInt_t GetF1SlotNumber(){return fF1SlotNumber;};
  UInt_t GetF1ChannelNumber(){return fF1ChannelNumber;};
  UInt_t GetF1Data(){return fF1Dataword;};
  

 private:
  static const UInt_t kF1Mask_SlotNumber;
  static const UInt_t kF1Mask_HeaderFlag;
  static const UInt_t kF1Mask_ResolutionFlag;
  static const UInt_t kF1Mask_OutputFIFOFlag;
  static const UInt_t kF1Mask_HitFIFOFlag;
  static const UInt_t kF1Mask_ChannelNumber;
  static const UInt_t kF1Mask_Dataword;

  Bool_t fF1HeaderFlag;
  UInt_t fF1SlotNumber;
  UInt_t fF1ChannelNumber;
  UInt_t fF1Dataword;

};

const UInt_t MQwF1TDC::kF1Mask_SlotNumber     = 0xf8000000;
const UInt_t MQwF1TDC::kF1Mask_HeaderFlag     = 0x00800000;
const UInt_t MQwF1TDC::kF1Mask_ResolutionFlag = 0x04000000;
const UInt_t MQwF1TDC::kF1Mask_OutputFIFOFlag = 0x02000000;
const UInt_t MQwF1TDC::kF1Mask_HitFIFOFlag    = 0x01000000;
const UInt_t MQwF1TDC::kF1Mask_ChannelNumber  = 0x003f0000;
const UInt_t MQwF1TDC::kF1Mask_Dataword       = 0x0000ffff;

void MQwF1TDC::DecodeF1Word(UInt_t &word){
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

#endif
