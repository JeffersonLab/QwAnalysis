#include "QwF1TDContainer.h"

ClassImp(QwF1TDC);
ClassImp(QwF1TDContainer);

QwF1TDC::QwF1TDC()
{
  this->Initialize();
}

QwF1TDC::QwF1TDC(const Int_t roc, const Int_t slot)
{
  this->Initialize();
  fROC  = roc;
  fSlot = slot;
}

void QwF1TDC::Initialize()
{
  fROC                 = -1;
  fSlot                = -1;
  fReferenceSlotFlag   = kFALSE;
  
  fF1TDC_refcnt        = 0;
  fF1TDC_hsdiv         = 0;
  fF1TDC_refclkdiv     = 0;
  fF1TDC_trigwin       = 0;
  fF1TDC_triglat       = 0;
  
  fF1TDC_tframe_ns     = 0.0;
  fF1TDC_full_range_ns = 0.0;
  fF1TDC_bin_size_ns   = 0.0;
  fF1TDC_window_ns     = 0.0;
  fF1TDC_latency_ns    = 0.0;
  fF1TDC_resolution_ns = 0.0;

  return;
}


QwF1TDC::~QwF1TDC()
{
}


// Initialize the static list of QwF1TDCs
TClonesArray *QwF1TDContainer::gQwF1TDCs = 0;
const Int_t QwF1TDContainer::gMaxCloneArray = 100;

// Constructor
QwF1TDContainer::QwF1TDContainer()
{
  // Create the static TClonesArray if not existing yet
  if (not gQwF1TDCs) gQwF1TDCs = new TClonesArray("QwF1TDC", gMaxCloneArray);
  // Set local TClonesArray to static TClonesArray and zero f1tdcs
  fQwF1TDCs     = gQwF1TDCs; fQwF1TDCs->Clear();
  fDetectorType = kTypeNull;
  fRegion       = kRegionIDNull;
  
};

// Constructor
QwF1TDContainer::QwF1TDContainer(EQwDetectorType detector_type)
{
  // Create the static TClonesArray if not existing yet
  if (not gQwF1TDCs) gQwF1TDCs = new TClonesArray("QwF1TDC", gMaxCloneArray);
  // Set local TClonesArray to static TClonesArray and zero f1tdcs
  fQwF1TDCs     = gQwF1TDCs; fQwF1TDCs->Clear();
  fDetectorType = detector_type;
  fRegion       = kRegionIDNull;
};


// Constructor
QwF1TDContainer::QwF1TDContainer(EQwDetectorType detector_type, EQwRegionID region)
{
  // Create the static TClonesArray if not existing yet
  if (not gQwF1TDCs) gQwF1TDCs = new TClonesArray("QwF1TDC", gMaxCloneArray);
  // Set local TClonesArray to static TClonesArray and zero f1tdcs
  fQwF1TDCs     = gQwF1TDCs; fQwF1TDCs->Clear();
  fDetectorType = detector_type;
  fRegion       = region;
};



// Destructor
QwF1TDContainer::~QwF1TDContainer()
{
  // Delete the static TClonesArray
  Reset();
  // Set local TClonesArray to null
  fNQwF1TDCs = 0;
};

// Clear the local TClonesArray
void QwF1TDContainer::Clear(Option_t *option)
{
  fQwF1TDCs->Clear(option);
  fNQwF1TDCs = 0;
};

void QwF1TDContainer::Delete(Option_t *option)
{
  fQwF1TDCs->Delete(option);
  fNQwF1TDCs = 0;
};

// Delete the static TClonesArray
void QwF1TDContainer::Reset(Option_t *option)
{
  delete gQwF1TDCs;
  gQwF1TDCs = 0;
};


void QwF1TDContainer::AddQwF1TDC(QwF1TDC &in)
{
  TClonesArray &hits = *fQwF1TDCs;
  new (hits[fNQwF1TDCs++]) QwF1TDC(in);
  return;
};


QwF1TDC *QwF1TDContainer::GetF1TDC (Int_t f1tdcID) const
{
  return (QwF1TDC*) fQwF1TDCs->At(f1tdcID);
}

