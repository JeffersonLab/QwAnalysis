#include "QwF1TDContainer.h"
/**
 *  \file   QwF1TDContainer.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Tuesday, August 31 11:41:58 EDT 2010
 */
ClassImp(QwF1TDC);
ClassImp(QwF1TDContainer);


const Int_t QwF1TDC::fWordsPerBuffer = 16;
const Int_t QwF1TDC::fMaxF1TDCChannelNumber = 64;

QwF1TDC::QwF1TDC()
{
  fROC                 = -1;
  fSlot                = -1;

  fChannelNumber       = 0;

  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;
  
  fF1TDC_t_offset      = 0.0;

  fF1TDC_SEU_counter   = 0;
  fF1TDC_EMM_counter   = 0;
  fF1TDC_SYN_counter   = 0;

  fReferenceSignals    = NULL;

  fReferenceSlotFlag   = false;
  fF1TDCNormResFlag    = true;
  fF1TDCSyncFlag       = true;

  fF1TDCFactor         = 0.0;

  fF1TDCIndex          = -1;
  
  fBuffer = new UInt_t[fWordsPerBuffer];

}



QwF1TDC::QwF1TDC(const Int_t roc, const Int_t slot)
{
  fROC                 = roc;
  fSlot                = slot;

  fChannelNumber       = 0;

  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;

  fF1TDC_t_offset      = 0.0;

  fF1TDC_SEU_counter   = 0;
  fF1TDC_EMM_counter   = 0;
  fF1TDC_SYN_counter   = 0;

  fReferenceSignals    = NULL;

  fReferenceSlotFlag   = false;
  fF1TDCNormResFlag    = true;
  fF1TDCSyncFlag       = true;

  fF1TDCFactor         = 0.0;

  fF1TDCIndex          = -1;
  fBuffer = new UInt_t[fWordsPerBuffer];

}


// QwF1TDC::QwF1TDC(const QwF1TDC &f1tdc)
// {
//   fROC                 = -1;
//   fSlot                = -1;

//   fChannelNumber       = 64;

//   fF1TDC_refcnt        = 0;
//   fF1TDC_hsdiv         = 0;
//   fF1TDC_refclkdiv     = 0;
//   fF1TDC_trigwin       = 0;
//   fF1TDC_triglat       = 0;
  
//   fF1TDC_tframe_ns     = 0.0;
//   fF1TDC_full_range_ns = 0.0;
//   fF1TDC_window_ns     = 0.0;
//   fF1TDC_latency_ns    = 0.0;
//   fF1TDC_resolution_ns = 0.0;

//   fF1TDC_SEU_counter   = 0;
//   fF1TDC_EMM_counter   = 0;
//   fF1TDC_SYN_counter   = 0;

//   fReferenceSignals    = NULL;
//   fReferenceSlotFlag   = kFALSE;
//   fF1TDCNormalResolutionFlag = kTRUE;

//   fF1TDCIndex          = -1;
//   // if(fF1TDCNormalResolutionFlag) fChannelNumber = 64;
//   // else                           fChannelNumber = 32;

//   *this = f1tdc;
// };



// QwF1TDC& QwF1TDC::operator=(const QwF1TDC& f1tdc)
// {
//   if(this == & f1tdc)
//     return *this;
 
//   fROC                       = f1tdc.fROC;
//   fSlot                      = f1tdc.fSlot;

//   fChannelNumber             = f1tdc.fChannelNumber;
//   fF1TDC_refcnt              = f1tdc.fF1TDC_refcnt;
//   fF1TDC_hsdiv               = f1tdc.fF1TDC_hsdiv;
//   fF1TDC_refclkdiv           = f1tdc.fF1TDC_refclkdiv;
//   fF1TDC_trigwin             = f1tdc.fF1TDC_trigwin;
//   fF1TDC_triglat             = f1tdc.fF1TDC_triglat;
  
//   fF1TDC_tframe_ns           = f1tdc.fF1TDC_tframe_ns;
//   fF1TDC_full_range_ns       = f1tdc.fF1TDC_full_range_ns;

//   fF1TDC_window_ns           = f1tdc.fF1TDC_window_ns;
//   fF1TDC_latency_ns          = f1tdc.fF1TDC_latency_ns;
//   fF1TDC_resolution_ns       = f1tdc.fF1TDC_resolution_ns;

//   fF1TDC_SEU_counter         = f1tdc.fF1TDC_SEU_counter;
//   fF1TDC_EMM_counter         = f1tdc.fF1TDC_EMM_counter;
//   fF1TDC_SYN_counter         = f1tdc.fF1TDC_SYN_counter;

//   fF1TDCIndex                = f1tdc.fF1TDCIndex;
//   fReferenceSignals = f1tdc.fReferenceSignals;

//   fReferenceSlotFlag         = f1tdc.fReferenceSlotFlag;
//   fF1TDCNormalResolutionFlag = f1tdc.fF1TDCNormalResolutionFlag;

//   return *this;
// };



QwF1TDC::~QwF1TDC()
{
  delete [] fBuffer;
  delete [] fReferenceSignals;
  //delete [] fF1TDCReferenceChannels;
}

void
QwF1TDC::SetF1TDCBuffer(UInt_t *buffer, UInt_t num_words)
{
  UInt_t i = 0;
  Int_t j  = 0;
  UInt_t start_index = 0;

  for(i=0; i<num_words; i++ ) {
    if(buffer[i] == (UInt_t) fSlot) start_index = i;
  }


  // reassign buffer with 16 bits trim, because f1 configuration register is 16 bits

  for(j=0; j<fWordsPerBuffer; j++) {
    // exclude the first buffer, because it contains "slot" number.
    fBuffer[j] = buffer[start_index+1+j] & 0xFFFF; 
  }


  // decide Normal or High Resolution Mode 
  if ( fBuffer[1] & 0x8000 ) {
    fF1TDCFactor = 0.5;
    fF1TDCNormResFlag = false;
  }
  else {
    fF1TDCFactor = 1.0;
    fF1TDCNormResFlag = true;
  }
  

  // check whether Synchronous mode or not
  if( fBuffer[15] & 0x4 ) {
    fF1TDCSyncFlag = true;
  } 
  else {
    fF1TDCSyncFlag = false;
  }
  
  // decide max channel number of F1TDC according to "resolution mode"

  fChannelNumber   = (Int_t) (fF1TDCFactor*fMaxF1TDCChannelNumber);

  // get refcnt and calculate tframe_ns
  fF1TDC_refcnt    = (fBuffer[7]>>6) & 0x1FF;
  fF1TDC_tframe_ns = (Double_t)(25 * (fF1TDC_refcnt +2 ));


  // calculate refclkdiv
  UInt_t exponent = 0;
  exponent = ( fBuffer[10] >> 8 ) & 0x7;
  
  fF1TDC_refclkdiv = 1;
  for(UInt_t ii = 0; ii < exponent; ii++) fF1TDC_refclkdiv = 2 * fF1TDC_refclkdiv;

  // get hsdiv
  fF1TDC_hsdiv = fBuffer[10] & 0xFF;

  // calculate resolution_ns or bin_size_ns and full_range_ns
  Double_t f1tdc_internal_reference_clock_ns = 25.0; // 40MHz
  fF1TDC_resolution_ns = fF1TDCFactor * (f1tdc_internal_reference_clock_ns/152.0) * ( (Double_t) fF1TDC_refclkdiv )/( (Double_t) fF1TDC_hsdiv );
  fF1TDC_full_range_ns = 65536.0 * fF1TDC_resolution_ns;

  // get trigwin and triglat
  fF1TDC_trigwin    = fBuffer[8] & 0xFFFF;
  fF1TDC_triglat    = fBuffer[9] & 0xFFFF; 	

  // calculate trigger window_ns and trigger latency_ns
  fF1TDC_window_ns  = ((Double_t)fF1TDC_trigwin) * fF1TDC_resolution_ns / fF1TDCFactor;
  fF1TDC_latency_ns = ((Double_t)fF1TDC_triglat) * fF1TDC_resolution_ns / fF1TDCFactor;

  // calculate the rollover counter (t_offset) if synchronoues mode is used
  if(fF1TDCSyncFlag) {
    fF1TDC_t_offset = fF1TDC_tframe_ns / fF1TDC_resolution_ns;
  }
  else {
    fF1TDC_t_offset = 65536.0; // no offset (rollover)
  }

  return;
}

void
QwF1TDC::PrintF1TDCBuffer()
{
  Int_t i = 0;
  
  printf("\nF1TDC ROC %2d, Slot %2d TDC index %d\n", fROC, fSlot, fF1TDCIndex);
  for(i=0; i<fWordsPerBuffer; i++) {
    std::cout << "0x" << std::hex << fBuffer[i] << " ";
  }
  std::cout << std::endl;

  this->PrintF1TDCConfigure();

  return;
};


void
QwF1TDC::PrintF1TDCConfigure()
{
  if(IsNormResolution()) printf("Normal Resolution mode ");
  else                   printf("High Resolution mMode ");
  
  printf(" with the total channel number %d\n", fChannelNumber);

  if(IsSyncMode())       {
    printf("Synchronous mode ");
    printf(" with the rollover (t_offset) counter %8.1f\n", fF1TDC_t_offset);
  }
  else {
    printf("Non Synchronouse mode \n");
    printf(" with the full range %8.1f\n", fF1TDC_t_offset);
  }
  
  printf("refcnt    = %5d, tframe(ns) = %5.4lf\n", fF1TDC_refcnt, fF1TDC_tframe_ns);
  printf("refclkdiv = %5d, hsdiv   = %6d, bin_size(ns) = %10.4f, full_range(ns) = %8.2f\n",
	 fF1TDC_refclkdiv, fF1TDC_hsdiv, fF1TDC_resolution_ns, fF1TDC_full_range_ns);
  printf("trigwin   = %5d, triglat = %6d, window  (ns) = %10.4f, latency   (ns) = %8.2f\n",
	 fF1TDC_trigwin, fF1TDC_triglat, fF1TDC_window_ns, fF1TDC_latency_ns);
  
  printf("\n");

  return;
};


void
QwF1TDC::ResetCounters()
{
  fF1TDC_SEU_counter = 0;
  fF1TDC_SYN_counter = 0;
  fF1TDC_EMM_counter = 0;
  return;
}



// void 
// SetRefernceSignals(Int_t chan, Double_t val)
// {
//   if( IsReFerenceSlot() ) 
//     {
//       fF1TDCReferenceChannels[chan] = val;
//     }
//   return;
// }


std::ostream& operator<< (std::ostream& os, const QwF1TDC &f1tdc)
{
  
  os << " ROC ";
  os << std::setw(2) << f1tdc.fROC;
  os << " Slot ";
  os << std::setw(2) << f1tdc.fSlot;
  os << " TDC idx ";
  os << std::setw(2) << f1tdc.fF1TDCIndex;

  return os;
}



QwF1TDContainer::QwF1TDContainer()
{
  fQwF1TDCs = new TObjArray();

  fQwF1TDCs->Clear();
  fNQwF1TDCs = 0;
  fDetectorType = kTypeNull;
  fRegion       = kRegionIDNull;
  
};


QwF1TDContainer::~QwF1TDContainer()
{
  if(fQwF1TDCs) delete fQwF1TDCs; fQwF1TDCs = NULL;
};



void 
QwF1TDContainer::AddQwF1TDC(QwF1TDC *in)
{
  Int_t pos = 0;
  pos = fQwF1TDCs -> AddAtFree(in);
  // std::cout << "AddQwF1TDC at pos " 
  // 	    << pos 
  // 	    << std::endl;
  fNQwF1TDCs++;
  return;
};


QwF1TDC *
QwF1TDContainer::GetF1TDC (Int_t f1tdcID) const
{

  return (QwF1TDC*) fQwF1TDCs->At(f1tdcID);
}

void 
QwF1TDContainer::SetSystemName(const TString name)
{
  // Types are defined in QwType.h
  if(fSubsystemName.IsNull()) {
    fSubsystemName = name;
    if(fSubsystemName == "R2") {
      fDetectorType = kTypeDriftHDC;
      fRegion       = kRegionID2;
    }
    else if(fSubsystemName == "R3") {
      fDetectorType = kTypeDriftVDC;
      fRegion       = kRegionID3;
    }
  }
  else {
    std::cout << "QwF1TDContainer::SetSystemName " 
	      << fSubsystemName 
	      << " is already registered."
	      << std::endl;
  }

  return;
};

void
QwF1TDContainer::Print()
{
  Int_t i =0;
  Int_t size = this->Size();

  std::cout << "\nQwF1TDContainer in Subsystem : " 
	    <<  this->GetSystemName()
	    << ", DetectorType " << this->GetDetectorType()
	    << ", RegionType "   << this->GetRegion()
	    << ", How many F1TDCs are : " << size
	    << std::endl;
  for ( i=0; i < size; i++ ) {
    ((QwF1TDC*) GetF1TDC(i))->PrintF1TDCBuffer();
  }

  return;
}


Double_t
QwF1TDContainer::GetF1TDCResolution()
{

  // F1TDC resolution must be the same
  // among VME crates and among F1TDC boards
  // We cannot change it on each F1TDC board.
  // Thus, this function return one value of them.
  // Wednesday, September  1 16:52:05 EDT 2010, jhlee

  Int_t size = this->Size();
  Int_t i  =  0;
  
  Double_t old_r = 0.0;
  Double_t new_r = 0.0;
    
  for ( i=0; i < size; i++ ) {
    new_r = ((QwF1TDC*) GetF1TDC(i))->GetF1TDC_resolution();
    if(i not_eq  0) 
      if(old_r not_eq new_r) {
	std::cout << "NEVER to see this message."
		  << "If one see this, F1TDC configurations are corrupted!\n";
	return 0.0;
      }
    //  printf("resolution %lf %lf\n", old_r, new_r);
    old_r = new_r;
  }
  

  return old_r;
}



std::ostream& operator<< (std::ostream& os, const QwF1TDContainer &container)
{
  
  Int_t size = 0;
  Int_t i = 0;
  size = container.fNQwF1TDCs;
  
  os << "\nQwF1TDContainer in Subsystem : " ;
  os <<  container.fSubsystemName;
  os << ", DetectorType " << container.fDetectorType;
  os << ", RegionType "   << container.fRegion;
  os << ", How many F1TDCs are : "  ;
  os << size ;
  os << "\n";

  for ( i=0; i < size; i++ ) {
    os << std::setw(2) << i;
    os << ":";
    os << container.GetF1TDC(i);
    os << *container.GetF1TDC(i);
    os << "\n";
  };
  return os;
}


