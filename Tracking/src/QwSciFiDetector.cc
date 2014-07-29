/**********************************************************\
 * File: QwSciFiDetector.cc                                *
 *                                                         *
 * Author: Jeong Han Lee                                   *
 * Date:  Sunday, January 15 17:22:07 EST 2012             *
\**********************************************************/

#include "QwSciFiDetector.h"
#include "QwParameterFile.h"

#include "boost/bind.hpp"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwSciFiDetector);

const UInt_t QwSciFiDetector::kMaxNumberOfSlotsPerROC   = 21;
const Int_t  QwSciFiDetector::kF1ReferenceChannelNumber = 99;


QwSciFiDetector::QwSciFiDetector(const TString& name)
: VQwSubsystem(name),
  VQwSubsystemTracking(name)
{
  // accept all event types to transfer to ProcessEvBuffer(roc_id .....)
  // ProcessEvBuffer(eventtype...) is defined in VQwSubsystem.h uses ProcessEvBuffer(roc_id,...)
  // 
  SetEventTypeMask(0xffff); 
  
  ClearAllBankRegistrations();

  kRegion             = "";
  fCurrentBankIndex   = 0;
  fCurrentSlot        = 0;
  fCurrentModuleIndex = 0;
  kNumberOfVMEModules = 0;
  fScalerBankIndex    = -1;

  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerF1TDC = fF1TDCDecoder.GetTDCMaxChannels(); 
  fF1RefContainer = new F1TDCReferenceContainer();
}

QwSciFiDetector::~QwSciFiDetector()
{
  fSCAs.clear();
  delete fF1TDContainer;
  delete fF1RefContainer;
};


Int_t 
QwSciFiDetector::LoadChannelMap( TString mapfile )
{
  
  Bool_t local_debug = false;

  if(local_debug) printf("\n------------- R1 LoadChannelMap %s\n\n", mapfile.Data());

  TString varname   = "";
  TString varvalue  = "";
  UInt_t  value     = 0;

  TString modtype      = "";
  Int_t   slot_number  = 0; // for SIS3801 scaler. In case of F1TDC, it is a dummy slot number.
  Int_t   chan_number  = 0; 
  Int_t   fiber_number = 0;
  TString name         = "";
  Int_t reference_counter = 0;

  EQwDetectorPackage package = kPackageNull;
  EQwDirectionID   direction = kDirectionNull;


  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine()){
    
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.

    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
       //  This is a declaration line.  Decode it.
      varname.ToLower();
      value = QwParameterFile::GetUInt(varvalue);
      if (value ==0){
	value = atol(varvalue.Data());
      }
      if (varname=="roc") {
	RegisterROCNumber(value);
      } 
      else if (varname=="bank"){
       	RegisterSubbank(value);
      }
      else if (varname=="pkg") {
       	package=(EQwDetectorPackage)value;
      }
      else if (varname=="slot") {
      	RegisterSlotNumber(value);
      } 
    }
    else {

      modtype      = mapstr.GetTypedNextToken<TString>();
      slot_number  = mapstr.GetTypedNextToken<Int_t>();
      chan_number  = mapstr.GetTypedNextToken<Int_t>();
      fiber_number = mapstr.GetTypedNextToken<Int_t>();
      name         = mapstr.GetTypedNextToken<TString>();

      if(local_debug) {
	printf("Modtype  %8s Slot_Number %4d  ChanN %4d, FiberN %4d, FiberName %s\n", modtype.Data(), slot_number, chan_number, fiber_number, name.Data());
      }
      if (modtype=="F1TDC") {

	if (fiber_number==99) {

	  fF1RefContainer -> AddF1TDCReferenceSignal(new F1TDCReferenceSignal(fCurrentBankIndex, slot_number, chan_number, name));

	  if (name=="MasterTrigger" ) {
	    //	  fReferenceChannels.push_back(std::make_pair(fCurrentModuleIndex, chan_number));
	    //	    printf("bank index %d Chan %d reference_counter %d\n", fCurrentBankIndex, chan_number, reference_counter);
	    fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentModuleIndex;
	    fReferenceChannels.at ( fCurrentBankIndex ).second = chan_number;
	    
	    fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fElement   = fCurrentBankIndex;
	    reference_counter++;
	  }
	}
	else {
	  fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fElement   = name.Atoi();
	}
	
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fRegion    = kRegionID1;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fPackage   = package;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fPlane     = fiber_number;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fDirection = direction;

      }
      else if (modtype == "SIS3801") {
	
	// We have *one* SIS3801 module, and "8" SIS3801 channels,
	// So, fSCAs size is 8

	fScalerBankIndex = fCurrentBankIndex;
	//	printf("Scaler bank index %d\n", fScalerBankIndex);
	QwSIS3801D24_Channel localchannel(name);
	localchannel.SetNeedsExternalClock(kFALSE);
	fSCAs.push_back(localchannel);
	fSCAs_map[name] = fSCAs.size()-1;
	fSCAs_offset.push_back( QwSIS3801D24_Channel::GetBufferOffset(slot_number,chan_number) );

	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fRegion    = kRegionID1;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fPackage   = package;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fPlane     = fiber_number;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fDirection = direction;
	fDetectorIDs.at(fCurrentModuleIndex).at(chan_number).fElement   = -1;

      }
      
   
      
    } 

  }
  
  if(local_debug) {
  
  
    Int_t unused_size_counter = 0;
    std::size_t i = 0;
    
    

    for( i = 0; i < fModuleIndex.size(); i++) 
      {
	for(size_t slot_size =0; slot_size < fModuleIndex.at(i).size(); slot_size++)
	  {
	    Int_t m_idx = 0;
	    m_idx = fModuleIndex.at(i).at(slot_size);
	    if(m_idx != -1 ) {
	      std::cout << "[" << i <<","<< slot_size << "] "
			<< " module index " << fModuleIndex.at(i).at(slot_size)
			<< std::endl;
	    }
	    else {
	      unused_size_counter++;
	    }
	  }
      }
    // why the bank index is always odd number?
    // 

    printf("Total unused size of fModuleIndex vector %6d\n", unused_size_counter);
    
    for(i = 0; i < fReferenceChannels.size(); i++) 
      {
	std::cout << "[" << i <<"] "
		  << " fRerenceChannel " << fReferenceChannels.at(i).first
		  << " " << fReferenceChannels.at(i).second
		  << std::endl;
      }


    // for(i = 0; i < fReferenceData.size(); i++) 
    //   {
    // 	for(std::size_t j=0; j< fReferenceData.at(i).size(); j++) 
    // 	  {
    // 	    std::cout << "[" << i << "," << j << "]"
    // 		      << " fReferenceData " << fReferenceData.at(i).at(j)
    // 		      << std::endl;
    // 	  }
    //   }
    //  fReferenceChannels.resize(fCurrentBankIndex+1);
    //    fReferenceData.resize(fCurrentBankIndex+1);

    printf("\n------------- R1 LoadChannelMap End%s\n\n", mapfile.Data());

  }

  mapstr.Close(); // Close the file (ifstream)

  ReportConfiguration(local_debug);
  return 0;
};

Int_t 
QwSciFiDetector::LoadInputParameters(TString mapfile)
{
  return 0;
};

void 
QwSciFiDetector::ClearEventData()
{ 
  SetDataLoaded(kFALSE);

  fTDCHits.clear();
  std::size_t i = 0;
  
  fF1RefContainer->ClearEventData();

  for (i=0; i<fReferenceData.size(); i++) {
    fReferenceData.at(i).clear();
  }
  for (i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ClearEventData();
  }
  return;
};

void
QwSciFiDetector::ProcessEvent()
{
  if (not HasDataLoaded()) return;

  // F1TDCs
  SubtractReferenceTimes();
  UpdateHits();


  // SIS3801 scaler
  std::size_t i = 0;
  for (i=0; i<fSCAs.size(); i++)
    {
      fSCAs.at(i).ProcessEvent();
    }
  return;
};



void  
QwSciFiDetector::FillListOfHits(QwHitContainer& hitlist)
{
  return;
};

void  
QwSciFiDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  
  for (std::size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ConstructHistograms(folder, prefix);
  }
  return;
};

void 
QwSciFiDetector::FillHistograms()
{
  if (! HasDataLoaded()) return;
  for (std::size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).FillHistograms();
  }
  
  return;
};

void  
QwSciFiDetector::DeleteHistograms()
{

  return;
};



void 
QwSciFiDetector::ConstructBranchAndVector(TTree *tree, 
					  TString& prefix, 
					  std::vector<Double_t> &values)
{
  fTreeArrayIndex = values.size();

  TString basename;
  if (prefix=="") basename = "scifiber";
  else            basename = prefix;

  TString list = "";

  for (std::size_t i=0; i<fSCAs.size(); i++)
    {
      if (fSCAs.at(i).GetElementName() != "") {
	values.push_back(0.0);
	list += ":" + fSCAs.at(i).GetElementName() + "/D";
      }
    }
  
  if (list[0]==':') {
    list = list(1,list.Length()-1);
  }
  
  fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
  tree->Branch(basename, &values[fTreeArrayIndex], list);
  return;
}

void
QwSciFiDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  Int_t index = fTreeArrayIndex;

  for (std::size_t i=0; i<fSCAs.size(); i++) 
    {
      if (fSCAs.at(i).GetElementName() != "") {
	values[index] = fSCAs.at(i).GetValue();
	index++;
      }
    }
  return;
}




void  
QwSciFiDetector::ReportConfiguration(Bool_t verbose)
{

  // Bank index 1 : ROC4  F1TDC
  // Bank index 3 : ROC9  F1TDCs
  // Bank index 5 : ROC11 SIS3801 


  if(verbose) {
    std::size_t i       = 0;
    std::size_t j       = 0;

    Int_t roc_num       = 0;
    Int_t bank_flag     = 0;
    Int_t bank_index    = 0;
    Int_t module_index  = 0;

    UInt_t slot_id      = 0;
    UInt_t vme_slot_num = 0;
    
    std::cout << "QwSciFiDetector Region : " 
	      << this->GetSubsystemName()
	      << "::ReportConfiguration fDetectorIDs.size() " 
	      << fDetectorIDs.size() << std::endl;


    for ( i=0; i<fROC_IDs.size(); i++ ) 
      {

	roc_num = fROC_IDs.at(i);

	for ( j=0; j<fBank_IDs.at(i).size(); j++ ) 
	  {
	    bank_flag = fBank_IDs.at(i).at(j);
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

		module_index = GetModuleIndex(bank_index, slot_id);
	  
		vme_slot_num = slot_id;
	  
		std::cout << "    "
			  << "Slot [id, VME num] [" 
			  << std::setw(2) << slot_id
			  << ","
			  << std::setw(2) << vme_slot_num
			  << "]";
		if ( module_index == -1 ) {
		  std::cout << "    "
			    << "Unused in R1 SciFiDetector" 
			    << std::endl;
		}
		else {
		  std::cout << "    "
			    << "Module index " 
			    << module_index << std::endl;
		}
	      }
	  }
      }
    
    for( size_t midx = 0; midx < fDetectorIDs.size(); midx++ )
      {
	for (size_t chan = 0 ; chan< fDetectorIDs.at(midx).size(); chan++)
	  {
	    std::cout << "[" << midx <<","<< chan << "] "
		      << " detectorID " << fDetectorIDs.at(midx).at(chan)
		      << std::endl;
	  }
      }
  }
  return;
}







void 
QwSciFiDetector::PrintConfigurationBuffer(UInt_t *buffer, UInt_t num_words)
{
  UInt_t ipt = 0;
  UInt_t j   = 0;
  UInt_t k   = 0;
  
  for ( j=0; j<(num_words/5); j++ ) 
    {
      printf ( "buffer[%5d] = 0x:", ipt );
      for ( k=j; k<j+5; k++ ) 
	{
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




Int_t 
QwSciFiDetector::ProcessConfigurationBuffer(const UInt_t roc_id, 
					    const UInt_t bank_id, 
					    UInt_t* buffer, 
					    UInt_t num_words)
{

  TString subsystem_name = "";

  Int_t bank_index     = 0;
  Int_t tdc_index      = 0;
  UInt_t slot_id       = 0;
  UInt_t vme_slot_num  = 0;

  Bool_t local_debug   = false;
  QwF1TDC *local_f1tdc = NULL;
   
  bank_index = GetSubbankIndex(roc_id, bank_id);
  
  if(bank_index >=0) {
    if(local_debug) {
      std::cout << "fF1TDContainer " << fF1TDContainer
		<<" local_f1tdc    " << local_f1tdc << "\n";
    }
    subsystem_name = this->GetSubsystemName();
    fF1TDContainer -> SetSystemName(subsystem_name);

    if(local_debug) {
      std::cout << "-----------------------------------------------------" << std::endl;
  
      std::cout << "\nQwSciFiDetector : " 
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
	
	tdc_index    = GetModuleIndex(bank_index, slot_id);
	vme_slot_num = slot_id;
	if(local_debug) {
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
	    if(local_debug) { 
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
	    if(local_debug){
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
	  if(local_debug) {
	    if      (slot_id == 0) std::cout << "         ";
	    else if (slot_id == 1) std::cout << "MVME CPU ";
	    else                   std::cout << "Trigger Interface"; // slot_id == 2;
	  }
	}
	if(local_debug) {
	  std::cout << std::endl;
	}
      }
  
    fF1TDCResolutionNS = fF1TDContainer->DoneF1TDCsConfiguration();

    if(local_debug) fF1TDContainer->Print();
    std::cout << "-----------------------------------------------------" << std::endl;

    
    // kMaxNumberOfChannelsPerTDC = fF1TDContainer->GetF1TDCChannelNumber();

    return 0;
  }
  else {
   if(local_debug) {
     printf("\nQwSciFiDetector::ProcessConfigurationBuffer: \n");
     printf("Bank index is not defined with ROC id %d and Bank id %d. Thus, this is not F1TDC data. Skip this bank\n\n", roc_id, bank_id);
    }
 
    return -1;
  }
}



// ProcessEvBuffer is called in QwEventBuffer.cc backtrace....
// 0 ProcessEvBuffer(event_type, roc_id, bank_id, buffer, num_words) in QwEventBuffer
// 1 ProcessEvBuffer(event_type, roc_id, bank_id, buffer, num_words) in QwSubsystemArray
// 2 ProcessEvBuffer(event_type, roc_id, bank_id, buffer, num_words) in VQwSubsystem.h
// 3 ProcessEvBuffer(roc_id, bank_id, buffer, num_words) in ProcessEvBuffer(event_type,...) in VQwSubsystem.h
// * public Constructor of VQwSubsystemTracking.h uses SetEventTypeMask() in order to filter out
//   0x1 (parity event, number 1 physics event). So I remove it by using SetEventTypeMask(0xffff)
//   in the public Constructor of QwSciFiDetector.
// 
//  Wednesday, January 18 13:34:29 EST 2012, jhlee


Int_t 
QwSciFiDetector::ProcessEvBuffer(const UInt_t roc_id, 
				 const UInt_t bank_id, 
				 UInt_t* buffer, 
				 UInt_t num_words)
{

  Int_t  bank_index             = 0;
  Int_t  vme_module_slot_number = 0;
  Int_t  vme_module_chan_number = 0;
  UInt_t vme_module_data        = 0;
  Int_t  module_index           = 0;

  Bool_t data_integrity_flag    = false;
  Bool_t temp_print_flag        = false;

  bank_index = GetSubbankIndex(roc_id, bank_id);
  
  if (bank_index>=0 && num_words>0) {
    //  We want to process this ROC.  Begin looping through the data.
    SetDataLoaded(kTRUE);

    if ( temp_print_flag ) {
      std::cout << "\nQwSciFiDetector::ProcessEvBuffer:  "
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
    
    if( bank_index != fScalerBankIndex) {
    //
    // CheckDataIntegrity() do "counter" whatever errors in each F1TDC 
    // and check whether data is OK or not.

      data_integrity_flag = fF1TDContainer->CheckDataIntegrity(roc_id, buffer, num_words);
    // if it is false (TFO, EMM, and SYN), the whole buffer is excluded for
    // the further process, because of multiblock data transfer.

      if (data_integrity_flag) {
	
	//	printf("\n");
	for (UInt_t i=0; i<num_words ; i++) {
	  
	  //  Decode this word as a F1TDC word.
	  fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id); 
	  
	  vme_module_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
	  vme_module_chan_number = fF1TDCDecoder.GetTDCChannelNumber();
	  module_index           = GetModuleIndex(bank_index, vme_module_slot_number);
	  
	  if ( vme_module_slot_number == 31) {
	    //  This is a custom word which is not defined in
	    //  the F1TDC, so we can use it as a marker for
	    //  other data; it may be useful for something.
	  }
	  
	  // Each subsystem has its own interesting slot(s), thus
	  // here, if this slot isn't in its slot(s) (subsystem map file)
	  // we skip this buffer to do the further process
	  
	  if (not IsSlotRegistered(bank_index, vme_module_slot_number) ) continue;
	  
	  if(temp_print_flag) std::cout << fF1TDCDecoder << std::endl;
	  
	  if ( fF1TDCDecoder.IsValidDataword() ) {//;;
	    // if decoded F1TDC data has a valid slot, resolution locked, data word, no overflow (0xFFFF), and no fake data
	    // try get time and fill it into raw QwHit.. FillRawTDCWord function is in each subsystem.
	    
	    try {
	      vme_module_data = fF1TDCDecoder.GetTDCData();
	      FillRawTDCWord(bank_index, vme_module_slot_number, vme_module_chan_number, vme_module_data);

	    }
	    catch (std::exception& e) {
	      std::cerr << "Standard exception from QwSciFiDetector::FillRawTDCWord: "
			<< e.what() << std::endl;
	      std::cerr << "   Parameters:  index==" <<bank_index
			<< "; Slot    Number=="   <<vme_module_slot_number
			<< "; Channel Number=="<<vme_module_chan_number
			<< "; Data=="         <<vme_module_data
			<< std::endl;
	      
	      std::cerr << " Module Index=="<<module_index
			<< "; fDetectorIDs.at(module_index).size()=="
			<< fDetectorIDs.at(module_index).size()
			<< "; fDetectorIDs.at(module_index).at(chan).fPlane=="
			<< fDetectorIDs.at(module_index).at(vme_module_chan_number).fPlane
			<< "; fDetectorIDs.at(module_index).at(chan).fElement=="
			<< fDetectorIDs.at(module_index).at(vme_module_chan_number).fElement
			<< std::endl;
	    }
	  }//;;
	} // for (UInt_t i=0; i<num_words ; i++) {
      }
 
    }
    else {
 
      // printf("Scaler %d\n", (Int_t)fSCAs.size());
      // Check if scaler buffer contains more than one event, and if so, reject it to decode.
      // 
      if (buffer[0]/32!=1) {
	if(temp_print_flag) printf("This buffer contains more than one event, thus, we reject them\n"); // Why? jhlee
	return 0;
      }

      UInt_t words_read = 0;
      for (std::size_t k=0; k<fSCAs.size(); k++) {
	words_read += fSCAs.at(k).ProcessEvBuffer(&(buffer[fSCAs_offset.at(k)]), num_words-fSCAs_offset.at(k));
	if(temp_print_flag) printf("k %d words_read %d\n", (Int_t)k, words_read);
      }
      
    }
  }
  
  // // fF1TDContainer-> PrintErrorSummary();
  return 0;
}



void  
QwSciFiDetector::FillRawTDCWord (Int_t bank_index, 
				 Int_t slot_num, 
				 Int_t chan, 
				 UInt_t data)
{
  Bool_t local_debug = false;
  Int_t tdcindex     =  0;

  tdcindex = GetModuleIndex(bank_index,slot_num);

  // I think, it is not necessary, but is just "safe" double check routine.
  if (tdcindex not_eq -1) {

    Int_t hitcnt  = 0;

    EQwDetectorPackage package = kPackageNull;
    EQwDirectionID direction   = kDirectionNull;

    Int_t   octant  = 0;
    Int_t   plane   = 0; // fiber number (1357, 2,4,6,8)  for the fibers,  and  99 for the reference channel
    Int_t   element = 0; // fiber orient type (0 (both), 1(a), 2(b)) for the fibers, and  the module index for the reference channel
    TString name         = "";
 
    //    F1TDCReferenceSignal *f1_ref_signal;
    
    fF1RefContainer->SetReferenceSignal(bank_index, slot_num, chan, data, local_debug);

    // if( f1_ref_signal ) {
    //   f1_ref_signal -> SetRefTimeAU (data);
    //   //  std::cout << *f1_ref_signal << std::endl;
    // }
    
    plane   = fDetectorIDs.at(tdcindex).at(chan).fPlane;
    element = fDetectorIDs.at(tdcindex).at(chan).fElement;
    package = fDetectorIDs.at(tdcindex).at(chan).fPackage;
    octant  = fDetectorIDs.at(tdcindex).at(chan).fOctant;
    if(local_debug) {
      printf("bank_idx %d, slot %d, plane %d, element %d, package %d\n",
             bank_index, slot_num, (Int_t) plane, (Int_t) element, (Int_t) package);
    }

    if (plane == -1 or element == -1){
      //  This channel is not connected to anything. Do nothing.
    }
    else if (plane == kF1ReferenceChannelNumber){
      fReferenceData.at(element).push_back(data);
      // we assign the reference time into fReferenceData according the module index
      // See LocalChannelMap
 
    }
    else {
      direction = fDetectorIDs.at(tdcindex).at(chan).fDirection;

      hitcnt = std::count_if(fTDCHits.begin(), fTDCHits.end(), 
			     boost::bind(
					 &QwHit::WireMatches, _1, kRegionID1, boost::ref(package), boost::ref(plane), boost::ref(element)
					 ) 
			     );
   
      fTDCHits.push_back(
    			 QwHit(
    			       bank_index, 
    			       slot_num, 
    			       chan, 
                               hitcnt, 
                               kRegionID1, 
                               package, 
                               octant,
                               plane,
                               direction, 
                               element, 
    			       data
    			       )
    			 );

      if(local_debug) {
	std::cout << "At QwSciFiDetector::FillRawTDCWord " 
		  << " bank index " << bank_index
		  << " slot num " << slot_num
		  << " chan num " << chan
		  << " hitcnt " << hitcnt
		  << " plane  " << plane
		  << " wire   " << element
		  << " package " << package 
		  << " diection " << direction
		  << " fTDCHits.size() " <<  fTDCHits.size() 
		  << std::endl;
      }

    }

  } // (tdcindex not_eq -1) {
  
  return;
}



void
QwSciFiDetector::FillHardwareErrorSummary()
{
  fF1RefContainer-> PrintCounters();
  fF1TDContainer -> PrintErrorSummary();
  fF1TDContainer -> WriteErrorSummary();
  return;
};




// protected
void 
QwSciFiDetector::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();

  std::size_t i = 0;

  for ( i=0; i<fModuleIndex.size(); i++)
    {
      fModuleIndex.at(i).clear();
    }
  
  fModuleIndex.clear();
  fDetectorIDs.clear();
  fTDCHits.clear();
  kNumberOfVMEModules = 0;
  return;
}

Int_t 
QwSciFiDetector::RegisterROCNumber(const UInt_t roc_id)
{
  Int_t status = 0;
  status = VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfSlotsPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  //std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return status;
}


Int_t 
QwSciFiDetector::RegisterSubbank(const UInt_t bank_id)
{

  Int_t status         = VQwSubsystem::RegisterSubbank(bank_id);
  Int_t current_roc_id = VQwSubsystem::fCurrentROC_ID;

  std::size_t reference_size = fReferenceChannels.size();
 
  // from here, just expand some vectors in QwSciFiDetector...
  //
  fCurrentBankIndex = GetSubbankIndex(current_roc_id, bank_id);
  //subbank id is directly related to the ROC

  if ( (Int_t) reference_size <= fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }

  // Why do we need to expand this vector size
  // even if most of them are empty? --- jhlee

  std::vector<Int_t> tmpvec(kMaxNumberOfSlotsPerROC,-1);

  fModuleIndex.push_back(tmpvec);

  std::cout<< "QwSciFiDetector::RegisterSubbank()" 
	   <<" ROC "            << current_roc_id
	   <<" Subbank "        << bank_id
	   <<" with BankIndex " << fCurrentBankIndex
	   << std::endl;

  return status;
}


Int_t 
QwSciFiDetector::RegisterSlotNumber(UInt_t slot_id)
{
  if (slot_id<kMaxNumberOfSlotsPerROC) {
    
    if (fCurrentBankIndex <= (Int_t) fModuleIndex.size()) {
 
      fDetectorIDs.resize(kNumberOfVMEModules+1);
      fDetectorIDs.at(kNumberOfVMEModules).resize(kMaxNumberOfChannelsPerF1TDC);

      // reassign kNumberOfVMEModules after resize it

      kNumberOfVMEModules = (Int_t) fDetectorIDs.size();
      fCurrentModuleIndex = kNumberOfVMEModules-1;
      fCurrentSlot        = slot_id;
      fModuleIndex.at(fCurrentBankIndex).at(fCurrentSlot) = fCurrentModuleIndex;
    }
  } 
  else {
    std::cout << "QwSciFiDetector::RegisterSlotNumber:  Slot number "
	     << slot_id << " is larger than the number of slots per ROC, "
	     << kMaxNumberOfSlotsPerROC << std::endl;
  }
  return fCurrentModuleIndex;
    
}

Int_t 
QwSciFiDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const 
{
  Int_t module_index = -1;
  if ( bank_index < fModuleIndex.size() ) {
    if ( slot_num < fModuleIndex.at(bank_index).size() ) {
      module_index = fModuleIndex.at(bank_index).at(slot_num);
    }
  }
  return module_index;
}




void  
QwSciFiDetector::SubtractReferenceTimes()
{

  UInt_t   bank_index        = 0;
  Double_t raw_time_arb_unit = 0.0;
  Double_t ref_time_arb_unit = 0.0;
  Double_t time_arb_unit     = 0.0;
 // EQwDetectorPackage package = kPackageNull;  // Region 1 has only Package 2, so kPackageDown

  Bool_t local_debug = false;
  Int_t slot_num = 0;

  TString reference_name1 = "MasterTrigger";
  TString reference_name2 = "CopyMasterTrigger";


  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
    {

      bank_index        = hit -> GetSubbankID();
      slot_num          = hit -> GetModule();
      raw_time_arb_unit = (Double_t) hit -> GetRawTime();
      ref_time_arb_unit = fF1RefContainer -> GetReferenceTimeAU(bank_index, reference_name1);
      //
      // if there is no reference time due to a channel error, try to use a copy of mater trigger
      // 
      if(ref_time_arb_unit==0.0) {
	ref_time_arb_unit =  fF1RefContainer->GetReferenceTimeAU(bank_index, reference_name2);
      }
      // second time, it returns 0.0, we simply ignore this event .... 
      // set time zero. ReferenceSignalCorrection() will return zero, and increase RFM counter...
      //
      time_arb_unit = fF1TDContainer->ReferenceSignalCorrection(raw_time_arb_unit, ref_time_arb_unit, bank_index, slot_num);

      hit -> SetTime(time_arb_unit); 
      hit -> SetRawRefTime((UInt_t) ref_time_arb_unit);

      if(local_debug) {
	QwMessage << this->GetSubsystemName()
		  << " BankIndex " << std::setw(2) << bank_index
		  << " Slot "      << std::setw(2) << slot_num
		  << " RawTime : " << std::setw(6) << raw_time_arb_unit
		  << " RefTime : " << std::setw(6) << ref_time_arb_unit
		  << " time : "    << std::setw(6) << time_arb_unit
		  << std::endl;
	
      }
    }

  return;
  
}


// void  
// QwSciFiDetector::SubtractReferenceTimes()
// {
  
//   std::vector<Double_t> reftimes;
//   std::vector<Bool_t>   refchecked;
//   std::vector<Bool_t>   refokay;
//   Bool_t allrefsokay;


//   std::size_t ref_size = 0;
//   std::size_t i = 0;
//   std::size_t j = 0;

//   ref_size = fReferenceData.size();

//   reftimes.resize  ( ref_size );
//   refchecked.resize( ref_size );
//   refokay.resize   ( ref_size );

//   for ( i=0; i<ref_size; i++ ) {
//     reftimes.at(i)   = 0.0;
//     refchecked.at(i) = kFALSE;
//     refokay.at(i)    = kFALSE;
//   }

//   allrefsokay = kTRUE;



//   UInt_t   bank_index        = 0;
//   Double_t raw_time_arb_unit = 0.0;
//   Double_t ref_time_arb_unit = 0.0;
//   Double_t time_arb_unit       = 0.0;

//   Bool_t local_debug = false;

//   for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
//     {
//       //  Only try to check the reference time for a bank if there is at least one
//       //  non-reference hit in the bank.
//       bank_index = hit->GetSubbankID();

//       // //     if(local_debug) printf("QwHit :: bank index %d\n", bank_index);

//       // if ( not refchecked.at(bank_index) ) {
	
//       // 	if ( fReferenceData.at(bank_index).empty() ) {
//       // 	  std::cout << "QwSciFiDetector::SubtractReferenceTimes:  Subbank ID "
//       // 	     	    << bank_index << " is missing a reference time." << std::endl;
//       // 	  refokay.at(bank_index) = kFALSE;
//       // 	  allrefsokay            = kFALSE;
//       // 	}
//       // 	else {
//       // 	  if(fReferenceData.at(bank_index).size() not_eq 1) {
//       // 	    std::cout << "Multiple hits are recorded in the reference channel, we use the first hit signal as the refererence signal." << std::endl;
//       // 	  }
//       // 	  reftimes.at(bank_index) = fReferenceData.at(bank_index).at(0);
//       // 	  refokay.at(bank_index)  = kTRUE;
//       // 	}

//       // 	if (refokay.at(bank_index)){
//       // 	  for ( j=0; j<fReferenceData.at(bank_index).size(); j++ ) 
//       // 	    {
//       // 	      // printf("Reference time %f fReferenceData.at(%d).at(%d) %f\n", 
//       // 	      // 	     reftimes.at(bank_index), (Int_t) bank_index, (Int_t) j, fReferenceData.at(bank_index).at(j));
//       // 	      fReferenceData.at(bank_index).at(j) -= reftimes.at(bank_index);
//       // 	      // printf("Reference time %f fReferenceData.at(%d).at(%d) %f\n", 
//       // 	      // 	     reftimes.at(bank_index), (Int_t) bank_index, (Int_t) j, fReferenceData.at(bank_index).at(j));
//       // 	    }
//       // 	}

//       // 	refchecked.at(bank_index) = kTRUE;
//       // }
      
//       // if ( refokay.at(bank_index) ) {
//    	Int_t slot_num    = hit -> GetModule();
//       	raw_time_arb_unit = (Double_t) hit -> GetRawTime();
//       	ref_time_arb_unit = (Double_t) reftimes.at(bank_index);

//       	time_arb_unit = fF1TDContainer->ReferenceSignalCorrection(raw_time_arb_unit, ref_time_arb_unit, bank_index, slot_num);

//       	hit -> SetTime(time_arb_unit); 
// 	hit -> SetRawRefTime((UInt_t) ref_time_arb_unit);

//       	if(local_debug) {
//       	  QwMessage << this->GetSubsystemName()
//       		    << " BankIndex " << std::setw(2) << bank_index
//       		    << " Slot "      << std::setw(2) << slot_num
//       		    << " RawTime : " << std::setw(6) << raw_time_arb_unit
//       		    << " RefTime : " << std::setw(6) << ref_time_arb_unit
//       		    << " time : "    << std::setw(6) << time_arb_unit
//       		    << std::endl;
	  
//       	}
//     }

  
//   // bank_index = 0;
  
//   // if ( not allrefsokay ) {
//   //   std::vector<QwHit> tmp_hits;
//   //   tmp_hits.clear();
//   //   for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
//   //     {
//   // 	bank_index = hit->GetSubbankID();
//   // 	if ( refokay.at(bank_index) ) tmp_hits.push_back(*hit);
//   //     }
//   //   //    std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
//   //   fTDCHits.clear();
//   //   fTDCHits = tmp_hits;
//   //   //     std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
//   // }
  
//   return;
// }



void 
QwSciFiDetector::UpdateHits()
{
 
 

  for(std::vector<QwHit>::iterator iter=fTDCHits.begin(); iter!=fTDCHits.end(); ++iter)
    {
  
      iter->ApplyTimeCalibration(fF1TDCResolutionNS); // Fill fTimeRes and fTimeNs in QwHit

    }

  return;
}

