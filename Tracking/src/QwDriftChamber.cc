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
#include "QwOptions.h"
#include "QwParameterFile.h"

//const UInt_t QwDriftChamber::kMaxNumberOfTDCsPerROC = 21; 
// VME 21 slots - 2 VMEs (CPU, TI)
// in priniciple, the maximum number is 19,
// but, Qweak only uses 9 F1TDCs at ROC 9 and ROC 10.
const UInt_t QwDriftChamber::kMaxNumberOfSlotsPerROC = 21;
const Int_t  QwDriftChamber::kReferenceChannelPlaneNumber = 99;
const Int_t  QwDriftChamber::kCodaMasterPlaneNumber = 98;

// kMaxNumberOfChannelsPerTDC is used in RegisterSlotNumber() function
// before one can access the real F1TDC configuration from CODA buffer.
// Thus, it is a constant value and is independent upon the real
// F1TDC configuration generally. However, Qweak uses only the Normal 
// Resolution configuration. Thus, it is always 64 channels we uses. 
// If someone wants to use the High Resolution Mode of F1TDC, 
// it would be better to change this number to 32 by hand.
// Friday, September  3 13:31:06 EDT 2010, jhlee


bool QwDriftChamber::fPrintF1TDCConfiguration = true;

#define OK 0


QwDriftChamber::QwDriftChamber(const TString& name)
  :VQwSubsystem(name),
   VQwSubsystemTracking(name),
   fWireHits(fTDCHits)
{
  fNumberOfTDCs = 0;
  ClearAllBankRegistrations();
  InitHistogramPointers();

  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerTDC = fF1TDCDecoder.GetTDCMaxChannels();
  fF1RefContainer = new F1TDCReferenceContainer();
}

QwDriftChamber::QwDriftChamber(const TString& name, std::vector< QwHit > &fWireHits_TEMP)
  :VQwSubsystem(name),
   VQwSubsystemTracking(name),
   fWireHits(fWireHits_TEMP)
{
  fNumberOfTDCs = 0;
  ClearAllBankRegistrations();
  InitHistogramPointers();

  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerTDC = fF1TDCDecoder.GetTDCMaxChannels(); 
  fF1RefContainer = new F1TDCReferenceContainer();
}

QwDriftChamber::~QwDriftChamber()
{
  delete fF1TDContainer;
  delete fF1RefContainer;
}

void  QwDriftChamber::FillDriftDistanceToHits()
{ 
//Currently This routine is not in use the drift distance calculation is done at ProcessEvent() on each sub-class
//   std::cout << "size: " << fWireHits.size() << std::endl;

  for (std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) 
    {
      hit1->SetDriftDistance(
			     CalculateDriftDistance( hit1->GetTimeNs(), hit1->GetDetectorID() )
			     );
    }
  return;
}


Int_t QwDriftChamber::ProcessEvBuffer(const UInt_t roc_id, 
				      const UInt_t bank_id, 
				      UInt_t* buffer, 
				      UInt_t num_words)
{

  Int_t  bank_index      = 0;
  UInt_t tdc_data        = 0;

  Bool_t data_integrity_flag = false;
  Bool_t temp_print_flag     = false;
  //  Int_t tdcindex = 0;

  bank_index = GetSubbankIndex(roc_id, bank_id);

 

  
  if (bank_index>=0 && num_words>0) {
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
		<< bank_index
		<< " Region "
		<< GetSubsystemName()
		<< std::endl;
    }
    
    //
    // CheckDataIntegrity() do "counter" whatever errors in each F1TDC 
    // and check whether data is OK or not.

    data_integrity_flag = fF1TDContainer->CheckDataIntegrity(roc_id, buffer, num_words);
    // if it is false (TFO, EMM, and SYN), the whole buffer is excluded for
    // the further process, because of multiblock data transfer.

    if (data_integrity_flag) {
      
      for (UInt_t i=0; i<num_words ; i++) {
	
	//  Decode this word as a F1TDC word.
	fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id); // MQwF1TDC or MQwV775TDC

	// For MQwF1TDC,   roc_id is needed to print out some warning messages.
	// For MQwV775TDC, roc_id isn't necessary, thus I set roc_id=0 in
	//                 MQwV775TDC.h  (Mon May  3 12:32:06 EDT 2010 jhlee)

	Int_t tdc_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
	Int_t tdc_chan_number = fF1TDCDecoder.GetTDCChannelNumber();
	Int_t tdcindex        = GetTDCIndex(bank_index, tdc_slot_number);
	
	if ( tdc_slot_number == 31) {
	  //  This is a custom word which is not defined in
	  //  the F1TDC, so we can use it as a marker for
	  //  other data; it may be useful for something.
	}
	
	// Each subsystem has its own interesting slot(s), thus
	// here, if this slot isn't in its slot(s) (subsystem map file)
	// we skip this buffer to do the further process

	if (not IsSlotRegistered(bank_index, tdc_slot_number) ) continue;

	if(temp_print_flag) std::cout << fF1TDCDecoder << std::endl;

	if ( fF1TDCDecoder.IsValidDataword() ) {//;;
	  // if decoded F1TDC data has a valid slot, resolution locked, data word, no overflow (0xFFFF), and no fake data
	  // try get time and fill it into raw QwHit.. FillRawTDCWord function is in each subsystem.

	  try {
	    tdc_data = fF1TDCDecoder.GetTDCData();
	    FillRawTDCWord(bank_index, tdc_slot_number, tdc_chan_number, tdc_data);
	  }
	  catch (std::exception& e) {
	    std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: "
		      << e.what() << std::endl;
	    std::cerr << "   Parameters:  index==" <<bank_index
		      << "; GetF1SlotNumber()=="   <<tdc_slot_number
		      << "; GetF1ChannelNumber()=="<<tdc_chan_number
		      << "; GetF1Data()=="         <<tdc_data
		      << std::endl;
	    // Int_t tdcindex = GetTDCIndex(bank_index, tdc_slot_number);
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
 
  }

  // fF1TDContainer-> PrintErrorSummary();
  return OK;
}



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
  std::vector<Int_t> tmpvec(kMaxNumberOfSlotsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  // std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return status;
}


Int_t QwDriftChamber::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex = GetSubbankIndex(VQwSubsystem::fCurrentROC_ID, bank_id);//subbank id is directly related to the ROC
  if (fReferenceChannels.size()<=fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
    fReferenceMaster.resize(fCurrentBankIndex+1);
  }
  std::vector<Int_t> tmpvec(kMaxNumberOfSlotsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  std::cout<< "RegisterSubbank()" 
	   <<" ROC " << (VQwSubsystem::fCurrentROC_ID)
	   <<" Subbank "<<bank_id
	   <<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
}


Int_t QwDriftChamber::RegisterSlotNumber(UInt_t slot_id)
{
  if (slot_id<kMaxNumberOfSlotsPerROC) {
    // fCurrentBankIndex is unsigned int and always positive
    if (/* fCurrentBankIndex >= 0 && */ fCurrentBankIndex <= fTDC_Index.size()) {
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
	      << kMaxNumberOfSlotsPerROC << std::endl;
  }
  return fCurrentTDCIndex;
    
}

Int_t QwDriftChamber::GetTDCIndex(size_t bank_index, size_t slot_num) const 
{
  Int_t tdcindex = -1;
  // bank_index and slot_num are unsigned int and always positive
  if (/* bank_index >= 0 && */ bank_index < fTDC_Index.size()) {
    if (/* slot_num >= 0 && */ slot_num < fTDC_Index.at(bank_index).size()) {
      tdcindex = fTDC_Index.at(bank_index).at(slot_num);
    }
  }
  return tdcindex;
}


Int_t QwDriftChamber::LinkReferenceChannel (const UInt_t chan,const Int_t plane, const Int_t wire )
{
  fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentTDCIndex;
  fReferenceChannels.at ( fCurrentBankIndex ).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPackage = kPackageNull; //Should be OK?
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPlane   = plane;
  fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fElement = fCurrentBankIndex;
  return OK;
}




void  QwDriftChamber::ReportConfiguration()
{
  std::size_t i    = 0;
  std::size_t j    = 0;

  Int_t roc_num    = 0;
  Int_t bank_flag  = 0;
  Int_t bank_index = 0;
  Int_t tdc_index  = 0;

  UInt_t slot_id   = 0;
  UInt_t vme_slot_num = 0;
    
  std::cout << "QwDriftChamber Region : " 
	    << this->GetSubsystemName()
	    << "::ReportConfiguration fTDCPtrs.size() " 
	    << fTDCPtrs.size() << std::endl;


  for ( i=0; i<fROC_IDs.size(); i++ ) 
    {

      roc_num = fROC_IDs.at(i);

      for ( j=0; j<fBank_IDs.at(i).size(); j++ ) 
	{
	
	  bank_flag    = fBank_IDs.at(i).at(j);
	  if(bank_flag == 0) continue; 
	  // must be uncommented if one doesn't define "bank_flag" in a CRL file
	  // but, now we use "bank_flag" in our crl files, thus skip to print
	  // unnecessary things on a screen
	  // Monday, August 30 14:45:34 EDT 2010, jhlee

	  bank_index = GetSubbankIndex(roc_num, bank_flag);
	
	  std::cout << "ROC [index, Num][" 
		    << i
		    << ","
		    << std::setw(2) << roc_num
		    << "]"
		    << " Bank [index,id]["
		    <<  bank_index
		    << ","
		    << bank_flag
		    << "]"
		    << std::endl;
	
	  for ( slot_id=2; slot_id<kMaxNumberOfSlotsPerROC; slot_id++ ) 
	    { 
	      // slot id starts from 2, because 0 and 1 are used for CPU and TI.
	      // Tuesday, August 31 10:57:07 EDT 2010, jhlee

	      tdc_index = GetTDCIndex(bank_index, slot_id);
	  
	      vme_slot_num = slot_id;
	  
	      std::cout << "    "
			<< "Slot [id, VME num] [" 
			<< std::setw(2) << slot_id
			<< ","
			<< std::setw(2) << vme_slot_num
			<< "]";
	      if ( tdc_index == -1 ) {
		std::cout << "    "
			  << "Unused in VDC" 
			  << std::endl;
	      }
	      else {
		std::cout << "    "
			  << "F1TDC index " 
			  << tdc_index << std::endl;
	      }
	    }
	}
    }
    
  return;
}







void QwDriftChamber::PrintConfigurationBuffer(UInt_t *buffer,UInt_t num_words)
{
  UInt_t ipt = 0;
  UInt_t j = 0;
  UInt_t k = 0;
  
  for ( j=0; j<(num_words/5); j++ ) 
    {
      printf ( "buffer[%5d] = 0x:", ipt );
      for ( k=j; k<j+5; k++ ) {
	printf ( "%12x", buffer[ipt++] );
      }
      printf ( "\n" );
    }
  
  if ( ipt<num_words ) {
    printf ( "buffer[%5d] = 0x:", ipt );
    for ( k=ipt; k<num_words; k++ ) 
      {
	printf ( "%12x", buffer[ipt++] );
      }
    printf ( "\n" );
  }
  printf ( "\n" );
  
  return;
}



Int_t QwDriftChamber::ProcessConfigurationBuffer (const UInt_t roc_id, 
						  const UInt_t bank_id, 
						  UInt_t* buffer, 
						  UInt_t num_words)
{

  TString subsystem_name;

  Int_t bank_index    = 0;
  Int_t tdc_index     = 0;
  UInt_t slot_id      = 0;
  UInt_t vme_slot_num = 0;

  QwF1TDC *local_f1tdc = NULL;
   
  bank_index = GetSubbankIndex(roc_id, bank_id);
  
  if(bank_index >=0) {
    if(fPrintF1TDCConfiguration) {
      std::cout << "fF1TDContainer " << fF1TDContainer
		<<" local_f1tdc    " << local_f1tdc << "\n";
    }
    subsystem_name = this->GetSubsystemName();
    fF1TDContainer -> SetSystemName(subsystem_name);
    fF1RefContainer-> SetSystemName(subsystem_name);

    if(fPrintF1TDCConfiguration) {
      std::cout << "-----------------------------------------------------" << std::endl;
  
      std::cout << "\nQwDriftChamber : " 
		<< subsystem_name
		<< ", "
		<< "ProcessConfigurationBuffer"
		<< std::endl;
      std::cout << "ROC " 
		<< std::setw(2) << roc_id
		<< " Bank [index,id]["
		<<  bank_index
		<< ","
		<< bank_id
		<< "]"
		<< std::endl;
    }
    for ( slot_id=0; slot_id<kMaxNumberOfSlotsPerROC; slot_id++ ) 
      { 
	// slot id starts from 2, because 0 is one offset (1) difference between QwAnalyzer and VME definition, 
	// and 1 and 2 are used for CPU and TI. Tuesday, August 31 10:57:07 EDT 2010, jhlee
	
	tdc_index    = GetTDCIndex(bank_index, slot_id);
	vme_slot_num = slot_id;
	if(fPrintF1TDCConfiguration) {
	  std::cout << "    "
		    << "Slot [id, VME num] [" 
		    << std::setw(2) << slot_id
		    << ","
		    << std::setw(2) << vme_slot_num
		    << "]";
	  std::cout << "    ";
	}
	local_f1tdc = NULL;

	if(slot_id > 2) { // save time
	
	  if (tdc_index not_eq -1) {

	    if(local_f1tdc) delete local_f1tdc; local_f1tdc = NULL;

	    local_f1tdc = new QwF1TDC(roc_id, vme_slot_num);

	    local_f1tdc->SetF1BankIndex(bank_index);
	    local_f1tdc->SetF1TDCIndex(tdc_index);
	    local_f1tdc->SetF1TDCBuffer(buffer, num_words);
	    local_f1tdc->SetF1SystemName(subsystem_name);

	    fF1TDContainer->AddQwF1TDC(local_f1tdc);
	    if(fPrintF1TDCConfiguration) {
	      std::cout << "F1TDC index " 
			<< std::setw(2) 
			<< tdc_index
			<< std::setw(16) 
			<< " local_f1tdc " 
			<< *local_f1tdc
			<< " at " 
			<< local_f1tdc;
	    }
	  }
	  else {
	    if(fPrintF1TDCConfiguration){
	      std::cout << "Unused in "  
			<< std::setw(4) 
			<< subsystem_name	
			<< std::setw(16) 
			<< " local_f1tdc  at " 
			<< local_f1tdc;
	    }
	  }
		
	}
	else { // slot_id == only 0, 1, & 2
	  if(fPrintF1TDCConfiguration) {
	    if      (slot_id == 0) std::cout << "         ";
	    else if (slot_id == 1) std::cout << "MVME CPU ";
	    else                   std::cout << "Trigger Interface"; // slot_id == 2;
	  }
	}
	if(fPrintF1TDCConfiguration) {
	  std::cout << std::endl;
	}
      }
    
    fF1TDCResolutionNS = fF1TDContainer->DoneF1TDCsConfiguration();

    if(fPrintF1TDCConfiguration) fF1TDContainer->Print();
    std::cout << "-----------------------------------------------------" << std::endl;

    
    // kMaxNumberOfChannelsPerTDC = fF1TDContainer->GetF1TDCChannelNumber();

    return OK;
  }
  else {
    ///    local_f1tdc = 0;
    return -1;
  }
}


void
QwDriftChamber::FillHardwareErrorSummary()
{
  fF1RefContainer -> PrintCounters();
  fF1TDContainer  -> PrintErrorSummary();
  fF1TDContainer  -> WriteErrorSummary();
  return;
};
