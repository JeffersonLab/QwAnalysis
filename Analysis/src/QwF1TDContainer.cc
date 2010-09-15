
#include "QwF1TDContainer.h"

#include "QwColor.h"
#include "QwLog.h"

#include "TROOT.h"


/**
 *  \file   QwF1TDContainer.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Tuesday, September 14 23:04:36 EDT 2010
 */

ClassImp(QwF1TDC);
ClassImp(QwF1TDContainer);


//
//
//
//  QwF1TDC
//
//
//
//


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
  fF1TDC_TFO_counter   = 0;

  fF1TDC_RLF_counter   = 0;
  fF1TDC_HFO_counter   = 0;
  fF1TDC_OFO_counter   = 0;

  fReferenceSignals    = NULL;

  fReferenceSlotFlag   = false;
  fF1TDCNormResFlag    = true;
  fF1TDCSyncFlag       = true;

  fF1TDCFactor         = 0.0;

  fF1TDCIndex          = -1;
  fF1BankIndex         = -1;

  fBuffer = new UInt_t[fWordsPerBuffer];

}



QwF1TDC::QwF1TDC(const Int_t roc, const Int_t slot)
{
  fROC                 = roc;
  // fROCIndex            = -1;
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
  fF1TDC_TFO_counter   = 0;

  fF1TDC_RLF_counter   = 0;
  fF1TDC_HFO_counter   = 0;
  fF1TDC_OFO_counter   = 0;

  fReferenceSignals    = NULL;

  fReferenceSlotFlag   = false;
  fF1TDCNormResFlag    = true;
  fF1TDCSyncFlag       = true;

  fF1TDCFactor         = 0.0;

  fF1TDCIndex          = -1;
  fF1BankIndex         = -1;

  fBuffer = new UInt_t[fWordsPerBuffer];
 
}


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


Double_t 
QwF1TDC::ReferenceSignalCorrection(Double_t raw_time, Double_t ref_time)
{

  Double_t trigger_window = (Double_t) fF1TDC_trigwin;
  Double_t time_offset    = (Double_t) fF1TDC_t_offset;


  Double_t time_condition = 0.0;
  Double_t local_time_difference = 0.0;
  Double_t actual_time_difference = 0.0;

  local_time_difference = raw_time - ref_time; 
  
  if(local_time_difference == 0.0) {
    // raw_time is the same as ref_time
    actual_time_difference = local_time_difference;
  }
  else {
    time_condition = fabs(local_time_difference); 
    // maximum value is trigger_window -1, 
    // thus 17195-1 =  17194 - 0 = 17194
    if(time_condition < trigger_window) {
      // there is no ROLLEVENT within trigger window
      actual_time_difference = local_time_difference;
    }
    else {
      // there is an ROLLOVER event within trigger window
      if (local_time_difference > 0.0) {
	// ref_time is in after ROLLOVER event
	actual_time_difference =  local_time_difference - time_offset;
      }
      else {
	// we already excluded local_time_diffrence == 0 case.
	// ref_time is in before ROLLOVER event
	actual_time_difference = local_time_difference + time_offset;
      }
      
    }
  }
  return actual_time_difference;
}



void
QwF1TDC::PrintF1TDCBuffer()
{
  Int_t i = 0;
  
  printf("System %s F1TDC ROC %2d, Slot %2d TDC id %2d Bank id %2d\n", 
	 fSystemName.Data(), fROC, fSlot, fF1TDCIndex, fF1BankIndex);
  for(i=0; i<fWordsPerBuffer; i++) {
    printf("0x%x ", fBuffer[i]);
  }
  printf("\n");

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
  
  fF1TDC_TFO_counter = 0;

  fF1TDC_RLF_counter = 0;
  fF1TDC_HFO_counter = 0;
  fF1TDC_OFO_counter = 0;
  return;
}


void
QwF1TDC::PrintErrorCounter()
{
  std::cout << "System " << this->GetF1SystemName()
	    << " QwF1TDC object at " << this
	    << *this
	    << " "
	    << " OFO " << this->GetOFO()
	    << " RLF " << this->GetRLF()
	    << " TFO " << this->GetTFO()
	    << " EMM " << this->GetEMM()
	    << " SEU " << this->GetSEU()
	    << " SYN " << this->GetSYN()
	    << " HFO " << this->GetHFO()
	    << std::endl;
  return;
}



TString
QwF1TDC::GetErrorCounter()
{
  TString error_counter;

  error_counter =  Form("Error Counter F1TDC ROC%2d", fROC);
  error_counter += Form(" Slot%2d", fSlot);
  error_counter += Form(" Index%2d", fF1TDCIndex);
  error_counter += " OFO : ";
  error_counter += this->GetOFO();
  error_counter += " RLF : ";
  error_counter += this->GetRLF();
  error_counter += " TFO : ";
  error_counter += this->GetTFO();
  error_counter += " EMM : ";
  error_counter += this->GetEMM();
  error_counter += " SEU : ";
  error_counter +=  this->GetSEU();
  error_counter += " SYN : ";
  error_counter += this->GetSYN();
  error_counter += " HFO : ";
  error_counter += this->GetHFO();

  return error_counter;
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
  os << " Bank idx ";
  os << std::setw(2) << f1tdc.fF1BankIndex;

  return os;
}




//----------------------------------
//
//
//
//  QwF1TDContainer
//
//
//
//
//----------------------------------




QwF1TDContainer::QwF1TDContainer()
{
  fQwF1TDCList = new TObjArray();

  fQwF1TDCList -> Clear();
  fQwF1TDCList -> SetOwner(kTRUE);
  fNQwF1TDCs = 0;
  fDetectorType = kTypeNull;
  fRegion       = kRegionIDNull;

  fLocalDebug = false;
  
};


QwF1TDContainer::~QwF1TDContainer()
{

  PrintErrorSummary();
  if(fQwF1TDCList) delete fQwF1TDCList; fQwF1TDCList = NULL;
};



void 
QwF1TDContainer::AddQwF1TDC(QwF1TDC *in)
{
  Int_t pos = 0;

  pos = fQwF1TDCList -> AddAtFree(in);
  if(fLocalDebug) {
    std::cout << "AddQwF1TDC at pos " 
	      << pos 
	      << std::endl;
  }

  fNQwF1TDCs++;
  return;
};


QwF1TDC *
QwF1TDContainer::GetF1TDC(Int_t roc, Int_t slot)
{
  Int_t roc_num  = 0;
  Int_t slot_num = 0;
  
  //  Int_t unique_id = 0;


  QwF1TDC* F1 = NULL;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;


  while ( (obj = next()) )
    {
      
      F1 = (QwF1TDC*) obj;
      //      unique_id = F1->GetUniqueID();
      roc_num  = F1->GetROCNumber();
      slot_num = F1->GetSlotNumber();
      if((roc_num == roc) && (slot_num == slot) ) {
	if(fLocalDebug) {
	  std::cout << "System " << F1->GetF1SystemName()
		    << " QwF1TDContainer::GetF1TDC F1TDC address at" << F1 << std::endl;
	}
	return F1;
      }
    }

  return NULL;
}


QwF1TDC *
QwF1TDContainer::GetF1TDCwithIndex(Int_t tdc_index)
{
  Int_t f1_idx = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL; 
  QwF1TDC* F1  = NULL;

  while ( (obj = next()) )
    {
      F1     = (QwF1TDC*) obj;
      f1_idx = F1->GetF1TDCIndex();
      if( f1_idx == tdc_index ) { 
	if(fLocalDebug) {
	  std::cout << "System " << F1->GetF1SystemName()
		    << " QwF1TDContainer::GetF1TDCIndex F1TDC address at" << F1 << std::endl;
	}
	return F1;
      }
    }

  return NULL;
}

QwF1TDC *
QwF1TDContainer::GetF1TDCwithBankIndexSLOT(Int_t bank_index, Int_t slot)
{
  Int_t bank_idx = 0;
  Int_t slot_num = 0;
  
  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;
  QwF1TDC* F1  = NULL;

  while ( (obj = next()) )
    {
      F1       = (QwF1TDC*) obj;
      bank_idx = F1->GetF1BankIndex();
      slot_num = F1->GetSlotNumber();
      if((bank_idx == bank_index) && (slot_num == slot) ) {
	if(fLocalDebug) {
	  std::cout << "System " << F1->GetF1SystemName()
		    << " QwF1TDContainer::GetF1TDC F1TDC address at" << F1 << std::endl;
	}
	return F1;
      }
    }

  return NULL;
};


void
QwF1TDContainer::AddSYN(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);
  
  if(F1) {
    F1->AddSYN();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddSYN : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}

void
QwF1TDContainer::AddEMM(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddEMM();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddEMM : " << PrintNoF1TDC(roc,slot)  << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddSEU(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddSEU();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddSEU : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddTFO(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddTFO();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddTFO : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}



void
QwF1TDContainer::AddRLF(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddRLF();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddRLF : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}


void
QwF1TDContainer::AddHFO(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddHFO();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddHFO : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}

void
QwF1TDContainer::AddOFO(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);

  if(F1) {
    F1->AddOFO();
    if(fLocalDebug) F1->PrintErrorCounter();
  }
  else {
    if(fLocalDebug) std::cout << "QwF1TDContainer::AddOFO : " << PrintNoF1TDC(roc,slot) << std::endl;
  }
  return;
}

Double_t
QwF1TDContainer::ReferenceSignalCorrection(
					   Double_t raw_time, 
					   Double_t ref_time, 
					   Int_t bank_index, 
					   Int_t slot_num
					   )
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDCwithBankIndexSLOT(bank_index, slot_num);

  if(F1) {
    return F1->ReferenceSignalCorrection(raw_time, ref_time);
  }
  else {
    //   printf("There is no F1TDC with Bank index %2d slot %2d\n", bank_index, slot_num);
    return -1.0;
  }
};


void 
QwF1TDContainer::SetSystemName(const TString name)
{
  // Types are defined in QwType.h
  if(fSystemName.IsNull()) {
    fSystemName = name;
    if(fSystemName == "R2") {
      fDetectorType = kTypeDriftHDC;
      fRegion       = kRegionID2;
    }
    else if(fSystemName == "R3") {
      fDetectorType = kTypeDriftVDC;
      fRegion       = kRegionID3;
    }
    else {
      //others?
    }
  }
  else {
    std::cout << "QwF1TDContainer::SetSystemName " 
	      << fSystemName 
	      << " is already registered."
	      << std::endl;
  }

  return;
};

void
QwF1TDContainer::Print()
{

  Int_t size = 0; 
  size = this->GetSize();

  std::cout << "\nQwF1TDContainer::Print() "
	    << " QwF1TDContainer in System : "  
	    <<  this->GetSystemName()
	    << ", DetectorType " 
	    << this->GetDetectorType()
	    << ", RegionType "   
	    << this->GetRegion()
	    << ", How many F1TDCs are : " 
	    << size
	    << std::endl;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj;
  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      std::cout << "F1TDC object " << F1 << std::endl;
      F1 -> PrintF1TDCBuffer();
    }

  return;
}


TString
QwF1TDContainer::PrintNoF1TDC(Int_t roc, Int_t slot)
{
  TString tmp = Form("There is no F1TDC with ROC%2d and SLOT%2d in System %s\n", 
		     roc, slot, GetSystemName().Data());
  return tmp;

};

TString
QwF1TDContainer::PrintNoF1TDC(Int_t tdc_index)
{
  TString tmp = Form("There is no F1TDC with TDCINDEX%2d in System %s\n", 
		     tdc_index, GetSystemName().Data());

  return tmp;
};


const Double_t
QwF1TDContainer::GetF1TDCResolution()
{

  // F1TDC resolution must be the same
  // among VME crates and among F1TDC boards
  // We cannot change it on each F1TDC board.
  // Thus, this function return one value of them.
  // Wednesday, September  1 16:52:05 EDT 2010, jhlee

  Double_t old_r = 0.0;
  Double_t new_r = 0.0;
  Int_t cnt      = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj;

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      new_r = F1->GetF1TDC_resolution();
      //   printf("cnt %d, new %f , old %f\n", cnt, new_r, old_r);
      if(cnt not_eq 0) {
	if(old_r not_eq new_r) {
	  std::cout << "NEVER see this message."
		    << "If one can see this, F1TDC configurations are corrupted!\n";
	  return 0.0;
	}

      }
      old_r = new_r;
      cnt++;
    }

  return old_r;
};



const Int_t
QwF1TDContainer::GetF1TDCChannelNumber()
{

  // F1TDC max channel number must be the same
  // among VME crates and among F1TDC boards
  // We cannot change it on each F1TDC board.
  // Thus, this function return one value of them. (32 or 64)
  // Friday, September  3 13:09:01 EDT 2010, jhlee

  Int_t old_c = 0;
  Int_t new_c = 0;
  Int_t cnt   = 0;

  TObjArrayIter next(fQwF1TDCList);
  TObject* obj;

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      new_c = F1->GetChannelNumber();
      //   printf("cnt %d, new %d , old %d\n", cnt, new_c, old_c);
      if(cnt not_eq 0) {
	if(old_c not_eq new_c) {
	  std::cout << "NEVER see this message."
		    << "If one can see this, F1TDC configurations are corrupted!\n";
	  return 0;
	}

      }
      old_c = new_c;
      cnt++;
    }
  
  return old_c;
};



void
QwF1TDContainer::PrintErrorSummary()
{
  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;
  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      F1 -> PrintErrorCounter();
    }

  return;
};


TList *
QwF1TDContainer::GetErrorSummary()
{

  TList *error_list = new TList;
  error_list->SetOwner(true);
  
  TObjArrayIter next(fQwF1TDCList);
  TObject* obj = NULL;

  while ( (obj = next()) )
    {
      QwF1TDC* F1 = (QwF1TDC*) obj;
      TString error = F1->GetErrorCounter();
      error_list -> Add(new TObjString(error));
    }

  return error_list;
};





Bool_t 
QwF1TDContainer::CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words)
{

  // only check the three possibilities.
  // 1) Trigger Time mismatch -> Sync Issue
  // 2) Event number mismatch 
  // 3) Xor bit changed -> Single Event Upset


  // three counter flags
  Bool_t hit_fifo_overflow_flag    = kFALSE;
  Bool_t output_fifo_overflow_flag = kFALSE;
  Bool_t chip_resolution_lock_flag = kTRUE;

  // real DataIntegrity Check
  Bool_t event_ok_flag              = kFALSE;
  Bool_t trig_time_ok_flag          = kFALSE;
  Bool_t trig_fifo_ok_flag          = kFALSE;

  // data integrity flags
  Bool_t data_integrity_flag        = kFALSE;
  Bool_t data_integrity_unlock_flag = kTRUE; // unlock first


  // SEU
  Bool_t xor_setup_flag     = kFALSE;
  
  Int_t slot_number          = 0;
  UInt_t reference_trig_time = 0;
  UInt_t reference_event_num = 0;

  const Int_t valid_trigger_time_offset = 1;
  
  for (UInt_t i=0; i<num_words ; i++) 
    {

      fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id); 
      slot_number = fF1TDCDecoder.GetTDCSlotNumber();

      // We use the multiblock data transfer for F1TDC, thus
      // we must get the event number and the trigger time from the first buffer
      // (buffer[0]), and these valuse can be used to check "data" integrity
      // over all F1TDCs

      if ( i==0 ) {
	if ( fF1TDCDecoder.IsHeaderword() ) {
	  reference_event_num = fF1TDCDecoder.GetTDCEventNumber();
	  reference_trig_time = fF1TDCDecoder.GetTDCTriggerTime();
	  
	  trig_fifo_ok_flag = fF1TDCDecoder.IsNotHeaderTrigFIFO();
	  if(not trig_fifo_ok_flag) this -> AddTFO(roc_id, slot_number);
	  
	}
	else {
	  std::cout << "The first word of F1TDC must be header word. "
		    << "Check CODA stream and QwF1TDContainer::CheckDataIntegrity fucntion."
		    << std::endl;
	  return false;
	}
      }

      // Check F1TDC status is inside a subsystem
      if(fF1TDCDecoder.IsValidDataSlot()) {
	if( CheckRegisteredF1(roc_id, slot_number) ) {
      
	// Check F1TDC slot, provided by buffer[i], is valid (1<=slot<=21)


	  if(fLocalDebug) {
	    std::cout << GetSystemName()
		      << " i" << std::setw(3) << i << fF1TDCDecoder << std::endl;
	  }

	  // Both header and data words have HFO, OFO, and RLF
	  hit_fifo_overflow_flag    = fF1TDCDecoder.IsHitFIFOOverFlow();
	  output_fifo_overflow_flag = fF1TDCDecoder.IsOutputFIFOOverFlow();
	  chip_resolution_lock_flag = fF1TDCDecoder.IsResolutionLock();
	  
	
	  if(hit_fifo_overflow_flag) {
	    this->AddHFO(roc_id, slot_number);
	    // one event creates the maximum 9 HFOs
	    // but the number is within the range of 1 - 9

	    if(fLocalDebug) {
	      std::cout << "There is the Hit FIFO Overflow on the F1TDC board at"
			<< " Ch "   << fF1TDCDecoder.GetTDCChannelNumber()
			<< " ROC "  << roc_id
			<< " Slot " << slot_number <<"\n";
	      std::cout << std::endl;
	    }
	  }
	  
	  if(output_fifo_overflow_flag)  {
	    this->AddOFO(roc_id, slot_number);
	    if(fLocalDebug) {
	      std::cout << "There is the Output FIFO Overflow on the F1TDC board at"
			<< " Ch "   << fF1TDCDecoder.GetTDCChannelNumber()
			<< " ROC "  << roc_id
			<< " Slot " << slot_number <<"\n";
	      std::cout << std::endl;
	    }
	  }
	  
	  if(not chip_resolution_lock_flag) {
	    this->AddRLF(roc_id, slot_number);
	    if(fLocalDebug) {
	      std::cout << "There is the Resolution Lock Failed on the F1TDC board at"
			<< " Ch "   << fF1TDCDecoder.GetTDCChannelNumber()
			<< " ROC "  << roc_id
			<< " Slot " << slot_number <<"\n";
	      std::cout << std::endl;
	    }
	  }

	  // check only header word

	  if ( fF1TDCDecoder.IsHeaderword() ) {
	    
	    
	    xor_setup_flag    = fF1TDCDecoder.IsHeaderXorSetup();
	    trig_fifo_ok_flag = fF1TDCDecoder.IsNotHeaderTrigFIFO();
	    event_ok_flag     = ( reference_event_num==fF1TDCDecoder.GetTDCHeaderEventNumber() );
	    trig_time_ok_flag = abs( reference_trig_time-fF1TDCDecoder.GetTDCHeaderTriggerTime() ) <= valid_trigger_time_offset;
	 
	    
	    // If SEU exists, Xor Setup Register bit is changing from 1 to 0, I think.
	    // Thus, it sets the trigger time 0 and the event number 0.
	    // For example, 
	    // Ch  8 Xor 0 tOF 0(hitOF,outOF,resLK)(001) ROC  9 Slot 10 EvtN 0 TriT   0
	    // And it is the source of the trigger time and the event number differences
	    // within the same ROC. In the CheckDataIntegrity routine, I decided
	    // to skip such a event. 
	    // Sunday, August  8 03:42:48 EDT 2010, jhlee
	  

	    // if no SEU, check Trigger FIFO Overflow, Trigger Time, and Event Number

	    if (xor_setup_flag) {

	      if(not trig_fifo_ok_flag) {
		this -> AddTFO(roc_id, slot_number);
		if(fLocalDebug) {
		  std::cout << "There is the Trigger FIFO overflow at"
			    << " Ch "   << fF1TDCDecoder.GetTDCChannelNumber()
			    << " ROC "  << roc_id
			    << " Slot " << slot_number <<"\n";
		  std::cout << std::endl;
		}
	      }

	      // Trigger Time difference of up to 1 count among the chips is acceptable
	      // For the Trigger Time, this assumes that an external SYNC_RESET signal has
	      // been successfully applied at the start of the run

	      if (not trig_time_ok_flag) {
		this->AddSYN(roc_id, slot_number);
		if(fLocalDebug) {
		  std::cout << "There is the no SYNC_RESET on the F1TDC board at"
			    << " Ch "   << fF1TDCDecoder.GetTDCChannelNumber()
			    << " ROC "  << roc_id
			    << " Slot " << slot_number <<"\n";
		  std::cout << std::endl;
		}
	      
	      
	      }
	      // Any difference in the Event Number among the chips indicates a serious error
	      // that requires a reset of the board.
	    
	      if (not event_ok_flag) {
		this->AddEMM(roc_id, slot_number);
		if(fLocalDebug) {
		  std::cout << "There is the Event Number Mismatch issue on the F1TDC board at"
			    << " ROC "  << roc_id
			    << " Slot " << slot_number << "\n";
		  std::cout << std::endl;
		}
	      }
	    

	      if(data_integrity_unlock_flag) { 
		// check data_integrity_flag = false once (not initial value),
		// if so, we skip the further process to check
		// data integrity, because it is alreay invalid
		// data we have.

		data_integrity_flag = (event_ok_flag) && (trig_time_ok_flag) && (trig_fifo_ok_flag) ;
		if(data_integrity_flag) data_integrity_unlock_flag = true;    // true  -> not lock -> check it again
		else                    data_integrity_unlock_flag = false;   // false -> lock  -> no check it again -> return false;
	      }
	    } // if(xor_setup_flag) {
	 
	 
	    // ignore SEU first, then we count them how frequently they are,
	    // because there is no way to seperate SEU and the data integrity.
	    // data integrity is valid within all buffer, but SEU is valid 
	    // for only when it is (header and dataword)

	    else {
	      this->AddSEU(roc_id, slot_number);
	  
	      if (fLocalDebug) {
		std::cout << "There is the Single Event Upset (SEU) on the F1TDC board at"
			  << " ROC "  << roc_id
			  << " Slot " << slot_number << "\n";
		std::cout << std::endl;
	      }
	    }
	  
	  } //   if ( fF1TDCDecoder.IsHeaderword() ) {
	  else {
	    // dataword
	    // nothing to check .....
	    //	    if(!fF1TDCDecoder.IsOverFlowEntry()) fF1TDCDecoder.PrintTDCData(fLocalDebug);
	  }//;;
	  
	} // if( CheckRegisteredF1(roc_id, slot_number) ) {

      }//if(fF1TDCDecoder.IsValidDataSlot()) {

  }//for (UInt_t i=0; i<num_words ; i++) {
  
  
  return (data_integrity_flag); 
};


void 
QwF1TDContainer::WriteErrorSummary()
{
  TSeqCollection *file_list = gROOT->GetListOfFiles();

  for(Int_t i=0; i<file_list->GetSize(); i++) 
    {
      TFile * file = (TFile*) file_list->At(i);
      file -> WriteObject(
			  this->GetErrorSummary(),
			  Form("%s : F1TDC Error Summary", GetSystemName().Data())
			  );
    }
  
  return;
};
  


Bool_t 
QwF1TDContainer::CheckRegisteredF1(Int_t roc, Int_t slot)
{
  QwF1TDC* F1 = NULL;
  F1 = this->GetF1TDC(roc, slot);
  
  if(F1) return true;
  else   return false;
};
