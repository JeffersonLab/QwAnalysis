/**********************************************************\
 * File: QwDriftChamber.C                                   *
 *                                                          *
 * Author: P. M. King, Rakitha Beminiwattha                 *
 *         J. H. Lee                                        *
 * Time-stamp: Wednesday, July 28 15:52:26 EDT 2010         *
\**********************************************************/

#include "QwDriftChamber.h"

#include "QwLog.h"
#include "QwColor.h"
#include "QwParameterFile.h"

const UInt_t QwDriftChamber::kMaxNumberOfTDCsPerROC = 21;
const Int_t QwDriftChamber::kReferenceChannelPlaneNumber = 99;


// OK, fDEBUG, fNumberOfTDCs
QwDriftChamber::QwDriftChamber(TString region_tmp,std::vector< QwHit > &fWireHits_TEMP)
  :VQwSubsystem(region_tmp),
   VQwSubsystemTracking(region_tmp),
   fWireHits(fWireHits_TEMP)
{
  OK            = 0;
  fDEBUG        = kFALSE;
  fNumberOfTDCs = 0;
  ClearAllBankRegistrations();
  InitHistogramPointers();

  kMaxNumberOfChannelsPerTDC = fF1TDC.GetTDCMaxChannels();  

  fF1TDContainer     = NULL;
  fF1TDCBadDataCount = 0;

};

QwDriftChamber::QwDriftChamber(TString region_tmp)
  :VQwSubsystemTracking(region_tmp),fWireHits(fTDCHits)
{
  OK            = 0;
  fDEBUG        = kFALSE;
  fNumberOfTDCs = 0;
  ClearAllBankRegistrations();
  InitHistogramPointers();

  kMaxNumberOfChannelsPerTDC = fF1TDC.GetTDCMaxChannels(); 

  fF1TDContainer     = NULL;
  fF1TDCBadDataCount = 0;

};




void  QwDriftChamber::FillDriftDistanceToHits()
{ //Currently This routine is not in use the drift distance calculation is done at ProcessEvent() on each sub-class
  for (std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) {
    
    if (hit1->GetTime()<0) continue;
    hit1->SetDriftDistance(CalculateDriftDistance(hit1->GetTime(),hit1->GetDetectorID()));
    
  }
  return;
};


Int_t QwDriftChamber::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  Int_t  index           = 0;
  Int_t  tdc_slot_number = 0;
  Int_t  tdc_chan_number = 0;
  UInt_t tdc_data        = 0;

  Bool_t data_integrity_flag = false;
  Bool_t temp_print_flag     = false;

  index = GetSubbankIndex(roc_id, bank_id);
  //  fF1TDCBadDataCount = 0;

 

  
  if (index>=0 && num_words>0) {
    //  We want to process this ROC.  Begin looping through the data.
    SetDataLoaded(kTRUE);


    if (temp_print_flag ) {
      std::cout << "QwDriftChamber::ProcessEvBuffer:  "
		<< "Begin processing ROC" 
		<< std::setw(2)
		<< roc_id 
		<< " bank id " 
		<< bank_id 
		<< " Subbbank Index "
		<< index
		<< " Region "
		<< GetSubsystemName()
		<< std::endl;
    }
  

    data_integrity_flag = fF1TDC.CheckDataIntegrity(roc_id, buffer, num_words);
    
    if (data_integrity_flag) {
      
      for (UInt_t i=0; i<num_words ; i++) {
	
	//  Decode this word as a F1TDC word.
	fF1TDC.DecodeTDCWord(buffer[i], roc_id); // MQwF1TDC or MQwV775TDC

	// For MQwF1TDC,   roc_id is needed to print out some warning messages.
	// For MQwV775TDC, roc_id isn't necessary, thus I set roc_id=0 in
	//                 MQwV775TDC.h  (Mon May  3 12:32:06 EDT 2010 jhlee)

	tdc_slot_number = fF1TDC.GetTDCSlotNumber();
	tdc_chan_number = fF1TDC.GetTDCChannelNumber();

	if ( tdc_slot_number == 31) {
	  //  This is a custom word which is not defined in
	  //  the F1TDC, so we can use it as a marker for
	  //  other data; it may be useful for something.
	}
	
	// Each subsystem has its own interesting slot(s), thus
	// here, if this slot isn't in its slot(s) (subsystem map file)
	// we skip this buffer to do the further process

	if (not IsSlotRegistered(index, tdc_slot_number) ) continue;

	if(temp_print_flag) std::cout << fF1TDC << std::endl;

	if ( fF1TDC.IsValidDataword() ) {//;;
	  // if F1TDC has a valid slot, resolution locked, and data word
	  try {
	    tdc_data = fF1TDC.GetTDCData();
	    if (tdc_data) {
	      // Only care when data is and not zero.
	      FillRawTDCWord(index, tdc_slot_number, tdc_chan_number, tdc_data);
	    }
	    else {
	      // I saw TDC raw time = 0, when SEU exists.
	      // Thus, I skip such a case. And this is a temp solution.
	    }
	  }
	  catch (std::exception& e) {
	    std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: "
		      << e.what() << std::endl;
	    std::cerr << "   Parameters:  index=="<<index
		      << "; GetF1SlotNumber()=="<< tdc_slot_number
		      << "; GetF1ChannelNumber()=="<<tdc_chan_number
		      << "; GetF1Data()=="<<tdc_data
		      << std::endl;
	    Int_t tdcindex = GetTDCIndex(index, tdc_slot_number);
	    std::cerr << "   GetTDCIndex()=="<<tdcindex
		      << "; fTDCPtrs.at(tdcindex).size()=="
		      << fTDCPtrs.at(tdcindex).size()
		      << "; fTDCPtrs.at(tdcindex).at(chan).fPlane=="
		      << fTDCPtrs.at(tdcindex).at(tdc_chan_number).fPlane
		      << "; fTDCPtrs.at(tdcindex).at(chan).fElement=="
		      << fTDCPtrs.at(tdcindex).at(tdc_chan_number).fElement
		      << std::endl;
	  }
	}//;;
      } // for (UInt_t i=0; i<num_words ; i++) {
    }
    else {
      // TODO...
      // must have some functions to access this error counter 
      //
      fF1TDCBadDataCount++;
    }
  }

  // if(temp_print_flag) {
  //   std::cout << "Total Bad TDC data count " 
  // 	      << fF1TDCBadDataCount 
  // 	      << std::endl;
  // }

  return OK;
};



void QwDriftChamber::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();
  std::size_t i = 0;
  for ( i=0; i<fTDC_Index.size(); i++){
    fTDC_Index.at(i).clear();
  }
  fTDC_Index.clear();

  fTDCPtrs.clear();
  fWireData.clear();
  fNumberOfTDCs = 0;
  return;
}

Int_t QwDriftChamber::RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id)
{
  Int_t status = 0;
  status = VQwSubsystemTracking::RegisterROCNumber(roc_id, bank_id);
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return status;
};


Int_t QwDriftChamber::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex = GetSubbankIndex(VQwSubsystem::fCurrentROC_ID, bank_id);//subbank id is directly related to the ROC
  
  if (fReferenceChannels.size()<=fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  std::cout<< "RegisterSubbank()" 
	   <<" ROC " << (VQwSubsystem::fCurrentROC_ID)
	   <<" Subbank "<<bank_id
	   <<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
};


Int_t QwDriftChamber::RegisterSlotNumber(UInt_t slot_id)
{
  if (slot_id<kMaxNumberOfTDCsPerROC) {
    if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fTDC_Index.size()) {
      fTDCPtrs.resize(fNumberOfTDCs+1);
      fTDCPtrs.at(fNumberOfTDCs).resize(kMaxNumberOfChannelsPerTDC);
      fNumberOfTDCs = fTDCPtrs.size();
      fTDC_Index.at(fCurrentBankIndex).at(slot_id) = fNumberOfTDCs-1;
      fCurrentSlot     = slot_id;
      fCurrentTDCIndex = fNumberOfTDCs-1;
    }
  } 
  else {
    std::cerr << "QwDriftChamber::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfTDCsPerROC << std::endl;
  }
  return fCurrentTDCIndex;
    
};

Int_t QwDriftChamber::GetTDCIndex(size_t bank_index, size_t slot_num) const 
{
  Int_t tdcindex = -1;
  if (bank_index>=0 && bank_index<fTDC_Index.size()) {
    if (slot_num>=0 && slot_num<fTDC_Index.at(bank_index).size()) {
      tdcindex = fTDC_Index.at(bank_index).at(slot_num);
    }
  }
  return tdcindex;
};


Int_t QwDriftChamber::LinkReferenceChannel (const UInt_t chan,const Int_t plane, const Int_t wire )
{
  fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentTDCIndex;
  fReferenceChannels.at ( fCurrentBankIndex ).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPackage = kPackageNull; //Should be OK?
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPlane   = plane;
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fElement = fCurrentBankIndex;
  return OK;
};
