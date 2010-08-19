#include "QwF1TDContainer.h"

ClassImp(QwF1TDC);
ClassImp(QwF1TDContainer);

QwF1TDC::QwF1TDC()
{
  fROC                 = -1;
  fSlot                = -1;
  fReferenceSlotFlag   = kFALSE;
  fF1TDCNormalResolutionFlag = kTRUE;
  fChannelNumber       = 64;
  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  //  fF1TDC_bin_size_ns   = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;

  fReferenceSignals = NULL;

  // if(fF1TDCNormalResolutionFlag) fChannelNumber = 64;
  // else                           fChannelNumber = 32;

}

QwF1TDC::QwF1TDC(const Int_t roc, const Int_t slot)
{
  fROC  = roc;
  fSlot = slot;
  fReferenceSlotFlag   = kFALSE;
  fF1TDCNormalResolutionFlag = kTRUE;
  fChannelNumber       = 64;
  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  //  fF1TDC_bin_size_ns   = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;

  fReferenceSignals = NULL;


  // Int_t SLOTNUM = 22; // not use 0, use 1-21
  // Int_t CHANNUM = 64; //    0-63
  // fF1TDCReferenceSignals = new TArrayD[SLOTNUM];
  // for (Int_t i=0;i<SLOTNUM;i++) fValues[i].Set(CHANNUM);
}


QwF1TDC::~QwF1TDC()
{
  delete [] fReferenceSignals;
  //delete [] fF1TDCReferenceChannels;
}


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

TClonesArray *QwF1TDContainer::gQwF1TDCs    = NULL;
const Int_t QwF1TDContainer::gMaxCloneArray = 14; 

// the maximum number of F1TDC modulesthat each subsystem can have.
// For Region 2 has 14
// For Region 3 has  6
// For other    has  2
// Monday, August  2 10:56:04 EDT 2010, jhlee

QwF1TDContainer::QwF1TDContainer()
{
  if (not gQwF1TDCs) gQwF1TDCs = new TClonesArray("QwF1TDC", gMaxCloneArray);

  fQwF1TDCs     = gQwF1TDCs; fQwF1TDCs->Clear();
  fDetectorType = kTypeNull;
  fRegion       = kRegionIDNull;
  
};

QwF1TDContainer::QwF1TDContainer(EQwDetectorType detector_type)
{
  if (not gQwF1TDCs) gQwF1TDCs = new TClonesArray("QwF1TDC", gMaxCloneArray);

  fQwF1TDCs     = gQwF1TDCs; fQwF1TDCs->Clear();
  fDetectorType = detector_type;
  fRegion       = kRegionIDNull;
};


QwF1TDContainer::QwF1TDContainer(EQwDetectorType detector_type, EQwRegionID region)
{
  if (not gQwF1TDCs) gQwF1TDCs = new TClonesArray("QwF1TDC", gMaxCloneArray);

  fQwF1TDCs     = gQwF1TDCs; fQwF1TDCs->Clear();
  fDetectorType = detector_type;
  fRegion       = region;
};



QwF1TDContainer::~QwF1TDContainer()
{
  Reset();
  fNQwF1TDCs = 0;
};

void 
QwF1TDContainer::Clear(Option_t *option)
{
  fQwF1TDCs->Clear(option);
  fNQwF1TDCs = 0;
  return;
};

void 
QwF1TDContainer::Delete(Option_t *option)
{
  fQwF1TDCs->Delete(option);
  fNQwF1TDCs = 0;
  return;
};

void 
QwF1TDContainer::Reset(Option_t *option)
{
  delete gQwF1TDCs;
  gQwF1TDCs = NULL;
  return;
};


void 
QwF1TDContainer::AddQwF1TDC(QwF1TDC &in)
{
  TClonesArray &f1tdcs = *fQwF1TDCs;
  new (f1tdcs[fNQwF1TDCs++]) QwF1TDC(in);
  return;
};


QwF1TDC *
QwF1TDContainer::GetF1TDC (Int_t f1tdcID) const
{
  return (QwF1TDC*) fQwF1TDCs->At(f1tdcID);
}

