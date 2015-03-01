/*------------------------------------------------------------------------*//*!

  \file QwMainDetector.cc

  \author P. King
  \author P. Wang

  \date  2007-05-08 15:40

  \brief This is the main executable for the tracking analysis.

  \ingroup QwTracking
				
*//*-------------------------------------------------------------------------*/


#include "QwMainDetector.h"
#include "QwParameterFile.h"
#include "QwSubsystemArrayTracking.h"

#include "boost/bind.hpp"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwMainDetector);


const UInt_t QwMainDetector::kMaxNumberOfModulesPerROC = 21;
const Int_t  QwMainDetector::kF1ReferenceChannelNumber = 99;
const Int_t  QwMainDetector::kMaxNumberOfQwHitPlane    = 8;   //JAM 2012-03-13


QwMainDetector::QwMainDetector(const TString& name)
: VQwSubsystem(name),VQwSubsystemTracking(name)
{
  SetEventTypeMask(0xffff); 

  fDEBUG = false;
  ClearAllBankRegistrations();

  fRegion             = "";
  fCurrentBankIndex   = 0;
  fCurrentSlot        = 0;
  fCurrentModuleIndex = 0;

  fBankID[0]          = 0;
  fBankID[1]          = 0;
  fBankID[2]          = 0;

  fNumberOfModules    = 0;

  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerF1TDC = fF1TDCDecoder.GetTDCMaxChannels();
  fF1RefContainer = new F1TDCReferenceContainer();

  for (Int_t plane_idx=0; plane_idx<kMaxNumberOfQwHitPlane; plane_idx++) //JAM
  {
    fSoftwareMeantimeContainer[plane_idx] = new MeanTimeContainer();
  }

  fSoftwareMeantimeOption = false;
  fSoftwareMeantimeTimeWindowNs = 0.0;
}

QwMainDetector::~QwMainDetector()
{
  fPMTs.clear();
  fSCAs.clear();
  delete fF1TDContainer;
  delete fF1RefContainer;

  for (Int_t plane_idx=0; plane_idx<kMaxNumberOfQwHitPlane; plane_idx++)
  {
    delete fSoftwareMeantimeContainer[plane_idx];
  }

}

Int_t QwMainDetector::LoadChannelMap(TString mapfile)
{

  TString varname  = "";
  TString varvalue = "";
  UInt_t  value     = 0;

  TString modtype  = "";
  TString dettype  = "";
  TString name     = "";

  Int_t   modnum   = 0;
  Int_t   channum  = 0;
  Int_t   slotnum  = 0;

  Int_t   ts_chan_num_to_plane    = 0;
  Int_t   ts_chan_type_to_element = 0;


  // package 
  // u -> u
  // d -> d
  // 
  // plane 
  // MD1 -> 1
  // MD2 -> 2
  // MD3 -> 3
  // MD4 -> 4
  // MD5 -> 5
  // MD6 -> 6
  // MD7 -> 7
  // MD8 -> 8
  //
  // element
  // p   -> 1
  // m   -> 2
  //
  
  
  Bool_t local_debug = false;
  Int_t reference_counter = 0;

  EQwDetectorPackage package = kPackageNull;
  EQwDirectionID   direction = kDirectionNull;


  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine())
    {
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;

      if (mapstr.HasVariablePair("=",varname,varvalue)) 
        {
          //  This is a declaration line.  Decode it.
          varname.ToLower();
          value = QwParameterFile::GetUInt(varvalue);
          if (varname=="roc")
            {
              RegisterROCNumber(value);
            }
          else if (varname=="qdc_bank")
            {
	      RegisterSubbank(value);
	      fBankID[0] = value;
            }
          else if (varname=="sca_bank")
            {
	      RegisterSubbank(value);
	      fBankID[1] = value;
            }
          else if (varname=="f1tdc_bank")
            {
	      RegisterSubbank(value);
	      fBankID[2] = value;
            }
          else if (varname=="slot")
            {
              RegisterSlotNumber(value);
              slotnum=value;
            }
        }
      else
        {
          //  Break this line into tokens to process it.
          modtype   = mapstr.GetTypedNextToken<TString>();
          modnum    = mapstr.GetTypedNextToken<Int_t>();
          channum   = mapstr.GetTypedNextToken<Int_t>();
          dettype   = mapstr.GetTypedNextToken<TString>();
          name      = mapstr.GetTypedNextToken<TString>();

	  if (local_debug) printf("%8s, %d, %d %s, %s\n", modtype.Data(), modnum, channum, dettype.Data(), name.Data());
	  
          //  Push a new record into the element array
          if (modtype=="SIS3801") {

	    QwSIS3801D24_Channel localchannel(name);
	    localchannel.SetNeedsExternalClock(kFALSE);
	    fSCAs.push_back(localchannel);
	    fSCAs_map[name] = fSCAs.size()-1;
	    fSCAs_offset.push_back(QwSIS3801D24_Channel::GetBufferOffset(modnum,channum));

	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fRegion    = kRegionIDCer;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPackage   = package;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPlane     = 0;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fDirection = direction;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fElement   = 0;
	  }
          else if (modtype=="V792" ) {// || modtype=="V775" || modtype=="F1TDC")
	    RegisterModuleType(modtype);
	    //  Check to see if we've encountered this channel or name yet
	    if (fModulePtrs.at(fCurrentModuleIndex).at(channum).first != kUnknownModuleType) {
	      //  We've seen this channel
	    }
	    else if (FindSignalIndex(fCurrentType, name) >= 0){
	      //  We've seen this signal
	    }
	    else {
	      //  If not, push a new record into the element array
                  // if (modtype=="V792") std::cout<<"V792: ";
                  // else if (modtype=="V775") std::cout<<"V775: ";
                  // else if (modtype=="F1TDC") std::cout<<"F1TDC: ";
	      LinkChannelToSignal(channum, name);
	    }
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fRegion    = kRegionIDCer;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPackage   = package;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPlane     = 0;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fDirection = direction;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fElement   = 0;
	  }	
	  else if( modtype=="F1TDC") {

	    RegisterModuleType(modtype);
	    //  Check to see if we've encountered this channel or name yet
	    if (fModulePtrs.at(fCurrentModuleIndex).at(channum).first != kUnknownModuleType) {
	      //  We've seen this channel
	    } else if (FindSignalIndex(fCurrentType, name)>=0) {
	      //  We've seen this signal
	    } else {
	      //  If not, push a new record into the element array
	      LinkChannelToSignal(channum, name);
	    }

	  

	    if (name=="md_reftime_f1") {

	      fF1RefContainer -> AddF1TDCReferenceSignal(new F1TDCReferenceSignal(fCurrentBankIndex, slotnum, channum, "MasterTrigger"));

	      fRefTime_SlotNum = slotnum;
	      fRefTime_ChanNum = channum;
	      reference_counter++;
	      //  printf("bank index %d Chan %d reference_counter %d\n", fCurrentBankIndex, channum, reference_counter);
	      fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentModuleIndex;
	      fReferenceChannels.at ( fCurrentBankIndex ).second = channum;
	    
	      package = kPackageNull;

	      fDetectorIDs.at(fCurrentModuleIndex).at(channum).fElement = fCurrentBankIndex;
	      fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPlane   = kF1ReferenceChannelNumber;
	   
	    }
	    else {
	      if (name=="md1m_f1") {
		ts_chan_num_to_plane    = 1;
		ts_chan_type_to_element = 2;
		package                 = kPackage1;
	      }
	      else if (name=="md2m_f1") {
		ts_chan_num_to_plane    = 2;
		ts_chan_type_to_element = 2;
		package                 = kPackage2;
	      }
	      else if (name=="md3m_f1") {
		ts_chan_num_to_plane    = 3;
		ts_chan_type_to_element = 2;
		package                 = kPackage1;
	      }
	      else if (name=="md4m_f1") {
		ts_chan_num_to_plane    = 4;
		ts_chan_type_to_element = 2;
		package                 = kPackage2;
	      }
	      else if (name=="md5m_f1") {
		ts_chan_num_to_plane    = 5;
		ts_chan_type_to_element = 2;
		package                 = kPackage1;
	      }
	      else if (name=="md6m_f1") {
		ts_chan_num_to_plane    = 6;
		ts_chan_type_to_element = 2;
		package                 = kPackage2;
	      }
	      else if (name=="md7m_f1") {
		ts_chan_num_to_plane    = 7;
		ts_chan_type_to_element = 2;
		package                 = kPackage1;
	      }
	      else if (name=="md8m_f1") {
		ts_chan_num_to_plane    = 8;
		ts_chan_type_to_element = 2;
		package                 = kPackage2;
	      }
	      else if (name=="md1p_f1") {
		ts_chan_num_to_plane    = 1;
		ts_chan_type_to_element = 1;
		package                 = kPackage1;
	      }
	      else if (name=="md2p_f1") {
		ts_chan_num_to_plane    = 2;
		ts_chan_type_to_element = 1;
		package                 = kPackage2;
	      }
	      else if (name=="md3p_f1") {
		ts_chan_num_to_plane    = 3;
		ts_chan_type_to_element = 1;
		package                 = kPackage1;
	      }
	      else if (name=="md4p_f1") {
		ts_chan_num_to_plane    = 4;
		ts_chan_type_to_element = 1;
		package                 = kPackage2;
	      }
	      else if (name=="md5p_f1") {
		ts_chan_num_to_plane    = 5;
		ts_chan_type_to_element = 1;
		package                 = kPackage1;
	      }
	      else if (name=="md6p_f1") {
		ts_chan_num_to_plane    = 6;
		ts_chan_type_to_element = 1;
		package                 = kPackage2;
	      }
	      else if (name=="md7p_f1") {
		ts_chan_num_to_plane    = 7;
		ts_chan_type_to_element = 1;
		package                 = kPackage1;
	      }
	      else if (name=="md8p_f1") {
		ts_chan_num_to_plane    = 8;
		ts_chan_type_to_element = 1;
		package                 = kPackage2;
	      }
	      else {
		ts_chan_num_to_plane    = -1;
		ts_chan_type_to_element = -1;	      
		package                 = kPackageNull;
	      }
	    

	      fDetectorIDs.at(fCurrentModuleIndex).at(channum).fElement   = ts_chan_type_to_element;
	      fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPlane     = ts_chan_num_to_plane;
	
	    }
	    
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fRegion    = kRegionIDCer;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fPackage   = package;
	    fDetectorIDs.at(fCurrentModuleIndex).at(channum).fDirection = direction;
	
	  }
          else {
	    std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str() << std::endl;
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
			<< " module index " << m_idx
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
    
    printf("\n------------- MD LoadChannelMap End%s\n\n", mapfile.Data());

  }

  ReportConfiguration(local_debug);

  mapstr.Close(); // Close the file (ifstream)
  //ReportConfiguration();
  return 0;
}


void  QwMainDetector::ClearEventData()
{
  SetDataLoaded(kFALSE);


  fTDCHits.clear();
  std::size_t i = 0;
  
  fF1RefContainer->ClearEventData();

  //Added by JAM 2012-03-13
  for (Int_t plane_idx=0; plane_idx<kMaxNumberOfQwHitPlane; plane_idx++)
  {
    fSoftwareMeantimeContainer[plane_idx] -> ClearEventData();
  }


  for (i=0; i<fReferenceData.size(); i++) {
    fReferenceData.at(i).clear();
  }

  for (size_t i=0; i<fPMTs.size(); i++) {
    for (size_t j=0; j<fPMTs.at(i).size(); j++) {
      fPMTs.at(i).at(j).SetValue(0);
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ClearEventData();
  }
  return;
}

//JAM stopped here

Int_t QwMainDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

 
  if( bank_id==fBankID[2] ) {

    TString subsystem_name;

    Int_t bank_index    = 0;
    Int_t tdc_index     = 0;
    UInt_t slot_id      = 0;
    UInt_t vme_slot_num = 0;

    Bool_t local_debug  = false;

    QwF1TDC *local_f1tdc = NULL;
   
    bank_index = GetSubbankIndex(roc_id, bank_id);

    if(bank_index >=0) {
      if(local_debug) {
	std::cout << "fF1TDContainer " << fF1TDContainer
		  <<" local_f1tdc    " << local_f1tdc << "\n";
      }
      subsystem_name = this->GetSubsystemName();
      fF1TDContainer -> SetSystemName(subsystem_name);
      fF1RefContainer-> SetSystemName(subsystem_name);

      for (Int_t plane_idx=0; plane_idx<kMaxNumberOfQwHitPlane; plane_idx++)
      {
        fSoftwareMeantimeContainer[plane_idx]->SetSystemName(subsystem_name);
        fSoftwareMeantimeContainer[plane_idx]->SetPlane(plane_idx+1);
      } //JAM

      if(local_debug) std::cout << "-----------------------------------------------------" << std::endl;
      
      std::cout << "QwMainDetector : " 
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
      for ( slot_id=0; slot_id<kMaxNumberOfModulesPerROC; slot_id++ ) { 
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

	    if(local_f1tdc) delete local_f1tdc; local_f1tdc = 0;

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

	    if(local_debug) {
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
      
	if(local_debug) std::cout << std::endl;
      }
  
      fF1TDCResolutionNS = fF1TDContainer->DoneF1TDCsConfiguration();

      if(local_debug) {
	fF1TDContainer->Print();
	std::cout << "-----------------------------------------------------" << std::endl;
      }
    }
  }
  return 0;
}



Int_t QwMainDetector::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  
  Int_t index = 0;

  index = GetSubbankIndex(roc_id,bank_id);

  //This is a QDC bank
  if (bank_id == fBankID[0]) {
    Bool_t fDEBUG = false;
    if (fDEBUG)
      std::cout << "QwMainDetector::QwMainDetector:  "
		<< "Begin processing ROC" << roc_id <<", Bank "<<bank_id
		<<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
		<< ", num_words "<<num_words<<", index "<<index<<std::endl;

    if (index>=0 && num_words>0)
      {
	SetDataLoaded(kTRUE);
	//  We want to process this ROC.  Begin looping through the data.
	for (size_t i=0; i<num_words ; i++)
	  {
	    //  Decode this word as a V775TDC word.
	    fQDCTDC.DecodeTDCWord(buffer[i]);

	    if (! IsSlotRegistered(index, fQDCTDC.GetTDCSlotNumber())) continue;
	    // std::cout<<"Slot registered as "<<fQDCTDC.GetTDCSlotNumber()<<"\n";

	    if (fQDCTDC.IsValidDataword()) {
	      //	      if (true)
	      //		{
		  // This is a V775 TDC data
	      if (fDEBUG)
		{
		  std::cout<<"This is a valid QDC/TDC data word. Index="<<index
			   <<" slot="<<fQDCTDC.GetTDCSlotNumber()<<" Ch="<<fQDCTDC.GetTDCChannelNumber()
			   <<" Data="<<fQDCTDC.GetTDCData()<<"\n";
		}
	    try
	      {
		FillRawWord(index,fQDCTDC.GetTDCSlotNumber(),fQDCTDC.GetTDCChannelNumber(),
			    fQDCTDC.GetTDCData());
	      }
	    catch (std::exception& e)
	      {
		std::cerr << "Standard exception from QwMainDetector::FillRawTDCWord: "
			  << e.what() << std::endl;
		Int_t chan = fQDCTDC.GetTDCChannelNumber();
		std::cerr << "   Parameters:  index=="<<index
			  << "; GetV775SlotNumber()=="<<fQDCTDC.GetTDCSlotNumber()
			  << "; GetV775ChannelNumber()=="<<chan
			  << "; GetV775Data()=="<<fQDCTDC.GetTDCData()
			  << std::endl;
		Int_t modindex = GetModuleIndex(index, fQDCTDC.GetTDCSlotNumber());
		std::cerr << "   GetModuleIndex()=="<<modindex
			  << "; fModulePtrs.at(modindex).size()=="
			  << fModulePtrs.at(modindex).size()
			  << "; fModulePtrs.at(modindex).at(chan).first {module type}=="
			  << fModulePtrs.at(modindex).at(chan).first
			  << "; fModulePtrs.at(modindex).at(chan).second {signal index}=="
			  << fModulePtrs.at(modindex).at(chan).second
			  << std::endl;
	      }
	    }
	  }
      }
  }
  else if (bank_id==fBankID[1]) { // This is a SCA bank
     // Check if scaler buffer contains more than one event
    if (buffer[0]/32!=1) return 0;
    
    if (index>=0 && num_words>0) {
      SetDataLoaded(kTRUE);
      UInt_t words_read = 0;
      for (size_t i=0; i<fSCAs.size(); i++) {
	words_read += fSCAs.at(i).ProcessEvBuffer(&(buffer[fSCAs_offset.at(i)]),
						  num_words-fSCAs_offset.at(i));
      }
    }
  }
  
  else if (bank_id==fBankID[2])  {  // This is a F1TDC bank

    // reset the reference time
    fRefTime = 0.0;
    
    Bool_t local_debug_f1 = false;

    Int_t  bank_index      = 0;
    UInt_t tdc_data        = 0;


    Int_t  dummy_slot_number = 0;
    Int_t  dummy_chan_number = 0;

    Bool_t data_integrity_flag = false;
    Bool_t temp_print_flag     = false;

    UInt_t hit_counter = 0;

    bank_index = GetSubbankIndex(roc_id, bank_id);
      
    if (bank_index>=0 && num_words>0) {
      //  We want to process this ROC.  Begin looping through the data.
      SetDataLoaded(kTRUE);
	
	
      if (local_debug_f1 ) {
      	std::cout << "\nQwMainDetector::ProcessEvBuffer:  "
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

	dummy_slot_number = 0;
	dummy_chan_number = 0;
	hit_counter       = 0;
	for (UInt_t i=0; i<num_words ; i++) {
	    
	  //  Decode this word as a F1TDC word.
	  fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id); // MQwF1TDC or MQwV775TDC
	    
	  // For MQwF1TDC,   roc_id is needed to print out some warning messages.
	  	    
	  Int_t tdc_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
	  Int_t tdc_chan_number = fF1TDCDecoder.GetTDCChannelNumber();
	  //Int_t tdcindex = GetModuleIndex(bank_index, tdc_slot_number);
	    
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

	    try {
		
	      tdc_data = fF1TDCDecoder.GetTDCData();
	      if (dummy_slot_number == tdc_slot_number && dummy_chan_number == tdc_chan_number) {  
		hit_counter++;
	      } 
	      else {
		hit_counter = 0;
	      }
	      
	      if(local_debug_f1) {
		printf("MD::ProcessEvBuffer: [%4d] hit counter %d, bank_index %2d slot_number [%2d,%2d] chan [%2d,%2d] data %10d %10.2f\n", i, hit_counter,
		       bank_index, tdc_slot_number, fRefTime_SlotNum, tdc_chan_number, fRefTime_ChanNum,tdc_data, fRefTime);
	      }
	      
	      if (hit_counter == 0) {
	      	FillRawWord   (bank_index, tdc_slot_number, tdc_chan_number, tdc_data);
	      	if ( IsF1ReferenceChannel(tdc_slot_number,tdc_chan_number) ) {
	      	  fRefTime = (Double_t) tdc_data;
	      	}
	      }
	      
	      FillRawTDCWord(bank_index, tdc_slot_number, tdc_chan_number, tdc_data);
	      
	      dummy_slot_number = tdc_slot_number;
	      dummy_chan_number = tdc_chan_number;
	      
	
	    }
	    catch (std::exception& e) {
	      std::cerr << "Standard exception from QwMainDetector::FillRawWord: "
			<< e.what() << std::endl;
	      std::cerr << "   Parameters:  index==" <<bank_index
			<< "; GetF1SlotNumber()=="   <<tdc_slot_number
			<< "; GetF1ChannelNumber()=="<<tdc_chan_number
			<< "; GetF1Data()=="         <<tdc_data
			<< std::endl;
	    }
	  }//;;
	} // for (UInt_t i=0; i<num_words ; i++) {
      }
    }
  }

  return 0;
}


void  QwMainDetector::ProcessEvent()
{
  if (! HasDataLoaded()) return;

  TString elementname = "";

  Double_t rawtime_arb_unit       = 0.0;
  Double_t corrected_time_arb_unit = 0.0;
  Double_t time_ns                 = 0.0;


  Int_t bank_index = 0;
  Int_t slot_num   = 0;

  for (size_t i=0; i<fPMTs.size(); i++) 
  {
    for (size_t j=0; j<fPMTs.at(i).size(); j++)
    {
      elementname = fPMTs.at(i).at(j).GetElementName();


      if (elementname.EndsWith("f1") ) {
        rawtime_arb_unit = fPMTs.at(i).at(j).GetValue(); // returns Double_t

        if ( rawtime_arb_unit not_eq 0.0) {

          if ( not elementname.Contains("reftime") ) {
            bank_index              = fPMTs.at(i).at(j).GetSubbankID();
            slot_num                = fPMTs.at(i).at(j).GetModule();
            corrected_time_arb_unit = fF1TDContainer->ReferenceSignalCorrection(rawtime_arb_unit, fRefTime, bank_index, slot_num);
            time_ns                 = fF1TDContainer->ReturnTimeCalibration(corrected_time_arb_unit);
            fPMTs.at(i).at(j).SetValue(time_ns);
            // printf("TS::ProcessBuffer:  bank_index %2d slot_number [%2d,%2d] chan [%2d,%2d] data %10f %10.2f, %10.2f\n", 
            //        bank_index, slot_num, fRefTime_SlotNum, tdc_chan_number, fRefTime_ChanNum, rawtime_arb_unit, fRefTime, corrected_time_arb_unit);
          } 
          else {
            // we save the referennce raw time
            fPMTs.at(i).at(j).SetValue(rawtime_arb_unit);
          } 
        }
        else {
          fPMTs.at(i).at(j).SetValue(rawtime_arb_unit); // zero
        }

      }
      else {
        fPMTs.at(i).at(j).ProcessEvent();
      }

    }//;;
  }//;

  // F1TDCs of  QwHit
  SubtractReferenceTimes();
  UpdateHits();
  AddSoftwareMeantimeToHits(fSoftwareMeantimeOption); //JAM

  for (size_t i=0; i<fSCAs.size(); i++)
  {
    fSCAs.at(i).ProcessEvent();
  }
  return;
};



void QwMainDetector::DefineOptions ( QwOptions& options )
{
  options.AddOptions()("enable-md-software-meantime",
		       po::value<Bool_t>()->default_bool_value(true),
		       "Create Software meantime for MD in QwHits" 
		       );
  options.AddOptions()("set-md-software-meantime-timewindow",
		       po::value<Double_t>()->default_value(50.0),
		       "TimeWindow (ns) for MD Software meantime"
		       );//max window is 2000 ns

  return;
};

void QwMainDetector::ProcessOptions ( QwOptions& options )
{
  fSoftwareMeantimeOption       = options.GetValue<Bool_t>   ( "enable-md-software-meantime" );
  fSoftwareMeantimeTimeWindowNs = options.GetValue<Double_t> ( "set-md-software-meantime-timewindow" );
  return;
};



void  QwMainDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).ConstructHistograms(folder, prefix);
        }
    }

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      fSCAs.at(i).ConstructHistograms(folder, prefix);
    }

}

void  QwMainDetector::FillHistograms()
{
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
	  fPMTs.at(i).at(j).FillHistograms();
        }
    }

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      fSCAs.at(i).FillHistograms();
    }
}


void  QwMainDetector::ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t> &values)
{
  fTreeArrayIndex = values.size();

  TString basename;
  if (prefix=="") basename = "maindet";
  else            basename = prefix;

  TString element_name = "";

  TString list = "";

  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
	  element_name = fPMTs.at(i).at(j).GetElementName();
	  if (element_name!="") {
	    values.push_back(0.0);
	    list += ":" + element_name + "/D";
	    //std::cout<<"Added to list: "<<fPMTs.at(i).at(j).GetElementName()<<"\n"<<std::endl;
	  }
        }
    }

  element_name = "";

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      element_name = fSCAs.at(i).GetElementName();
      if (element_name != "") {
	values.push_back(0.0);
	list += ":" + element_name + "/D";
      }
    }

  if (list[0]==':') {
    list = list(1,list.Length()-1);
  }

  fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
  tree->Branch(basename, &values[fTreeArrayIndex], list);
  // std::cout<<list<<"\n";
  return;
}


void  QwMainDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  Int_t index = fTreeArrayIndex;
  for (size_t i=0; i<fPMTs.size(); i++) {
    for (size_t j=0; j<fPMTs.at(i).size(); j++) {
      if (fPMTs.at(i).at(j).GetElementName()!="") {
	values[index] = fPMTs.at(i).at(j).GetValue();
	index++;
      }
    }
  }
  
  for (size_t i=0; i<fSCAs.size(); i++) {
    if (fSCAs.at(i).GetElementName() != "") {
      values[index] = fSCAs.at(i).GetValue();
      index++;
    }
  }

  return;
}


QwMainDetector& QwMainDetector::operator=  (const QwMainDetector &value)
{
  if (fPMTs.size() == value.fPMTs.size())
    {
      for (size_t i=0; i<fPMTs.size(); i++)
        {
          for (size_t j=0; j<fPMTs.at(i).size(); j++)
            {
              fPMTs.at(i).at(j) = value.fPMTs.at(i).at(j);
            }
        }
    }
  else
    {
      std::cerr << "QwMainDetector::operator=:  Problems!!!"
		<< std::endl;
    }
  return *this;
}




void QwMainDetector::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();
  fModuleIndex.clear();
  fModulePtrs.clear();
  fModuleTypes.clear();

  fDetectorIDs.clear();
  fTDCHits.clear();

  fNumberOfModules = 0;
}

Int_t QwMainDetector::RegisterROCNumber(const UInt_t roc_id)
{
  Int_t status = 0;
  status = VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return status;
}

Int_t QwMainDetector::RegisterSubbank(const UInt_t bank_id)
{
  Int_t status         = VQwSubsystem::RegisterSubbank(bank_id);
  Int_t current_roc_id = VQwSubsystem::fCurrentROC_ID;

  std::size_t reference_size = fReferenceChannels.size();

  fCurrentBankIndex = GetSubbankIndex(current_roc_id, bank_id);

  if ( (Int_t) reference_size <= fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }

  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);

  std::cout<< "QwMainDetector::RegisterSubbank()" 
	   <<" ROC "            << current_roc_id
	   <<" Subbank "        << bank_id
	   <<" with BankIndex " << fCurrentBankIndex
	   << std::endl;
  return status;
}


Int_t QwMainDetector::RegisterSlotNumber(UInt_t slot_id)
{
  //  printf("QwMainDetector::RegisterSlotNumber\n");
  std::pair<EQwModuleType, Int_t> tmppair;
  tmppair.first  = kUnknownModuleType;
  tmppair.second = -1;

  if (slot_id<kMaxNumberOfModulesPerROC) {

    if ( fCurrentBankIndex <= (Int_t) fModuleIndex.size()) {

      fModuleTypes.resize(fNumberOfModules+1);
      fModulePtrs.resize(fNumberOfModules+1);
      fModulePtrs.at(fNumberOfModules).resize(kMaxNumberOfChannelsPerF1TDC,
					      tmppair);
      //    fNumberOfModules = fModulePtrs.size();

      fDetectorIDs.resize(fNumberOfModules+1);
      fDetectorIDs.at(fNumberOfModules).resize(kMaxNumberOfChannelsPerF1TDC);
         // reassign kNumberOfModules after resize it
      fNumberOfModules = (Int_t) fDetectorIDs.size();
      fCurrentModuleIndex = fNumberOfModules-1;
      fCurrentSlot        = slot_id;
      fModuleIndex.at(fCurrentBankIndex).at(fCurrentSlot) = fCurrentModuleIndex;
    }
  }
  else {
    std::cerr << "QwMainDetector::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfModulesPerROC << std::endl;
  }
  return fCurrentModuleIndex;
}

EQwModuleType QwMainDetector::RegisterModuleType(TString moduletype)
{
  moduletype.ToUpper();

  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792") {
    fCurrentType = kV792_ADC;
  }
  else if (moduletype=="V775") {
    fCurrentType = kV775_TDC;
  }
  else if (moduletype=="F1TDC") {
    fCurrentType = kF1TDC;
  }
  else if (moduletype=="SIS3801") {
    fCurrentType = kSIS3801;
  }

  fModuleTypes.at(fCurrentModuleIndex) = fCurrentType;
  
  if ((Int_t) fPMTs.size()<=fCurrentType) {
    fPMTs.resize(fCurrentType+1);
  }

  return fCurrentType;
}


Int_t QwMainDetector::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
  fPMTs.at(fCurrentType).push_back(QwPMT_Channel(name));
  fModulePtrs.at(fCurrentModuleIndex).at(chan).first  = fCurrentType;
  fModulePtrs.at(fCurrentModuleIndex).at(chan).second = fPMTs.at(fCurrentType).size()-1;

  return 0;
}

void QwMainDetector::FillRawWord(Int_t bank_index,
                                 Int_t slot_num,
                                 Int_t chan, UInt_t data)
{
  Int_t modindex = GetModuleIndex(bank_index,slot_num);

  if (modindex != -1)
    {
      EQwModuleType modtype = fModulePtrs.at(modindex).at(chan).first;
      Int_t chanindex       = fModulePtrs.at(modindex).at(chan).second;

      if (modtype == kUnknownModuleType || chanindex == -1)
        {
          //  This channel is not connected to anything.
          //  Do nothing.
        }
      else
        {
    	  fPMTs.at(modtype).at(chanindex).SetValue(data);
	  fPMTs.at(modtype).at(chanindex).SetSubbankID(bank_index);
	  fPMTs.at(modtype).at(chanindex).SetModule(slot_num);
        }
    }
}


void  QwMainDetector::FillRawTDCWord (Int_t bank_index, 
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

    EQwDirectionID direction   = kDirectionNull;

    TString name         = "";
 

    fF1RefContainer->SetReferenceSignal(bank_index, slot_num, chan, data, local_debug);
  
    // MD1(8) plane 1(8)
    Int_t plane   = fDetectorIDs.at(tdcindex).at(chan).fPlane;
    // p is element 1, and m is element 2
    Int_t element = fDetectorIDs.at(tdcindex).at(chan).fElement;
    Int_t octant  = fDetectorIDs.at(tdcindex).at(chan).fOctant;
    EQwDetectorPackage package = fDetectorIDs.at(tdcindex).at(chan).fPackage;

  
    if(local_debug) {
      printf("tdcindx %d bank_idx %d, slot %d, plane %d, element %d, package %d\n",
	     tdcindex, 
	     bank_index, slot_num, (Int_t) plane, (Int_t) element, (Int_t) package);
    }

    if (plane == -1 or element == -1){
      //  This channel is not connected to anything. Do nothing.
    }
    else if (plane == kF1ReferenceChannelNumber){
      fReferenceData.at(element).push_back(data);
      // we assign the reference time into fReferenceData according the module index
      // See LocalChannelMap
      // if(local_debug) {
      // 	std::cout << "At QwMainDetector::FillRawTDCWord " 
      // 		  << " bank index " << bank_index
      // 		  << " slot num " << slot_num
      // 		  << " chan num " << chan
      // 		  << " hitcnt " << 0
      // 		  << " plane  " << plane
      // 		  << " wire   " << element
      // 		  << " package " << package 
      // 		  << " diection " << direction
      // 		  << " data " << data
      // 		  << " fTDCHits.size() " <<  0
      // 		  << std::endl;
      // }
    }
    else {
      direction = fDetectorIDs.at(tdcindex).at(chan).fDirection;

      hitcnt = std::count_if(fTDCHits.begin(), fTDCHits.end(), 
			     boost::bind(
					 &QwHit::WireMatches, _1, kRegionIDCer, boost::ref(package), boost::ref(plane), boost::ref(element)
					 ) 
			     );
   
      if(local_debug) {
	std::cout << "At QwMainDetector::FillRawTDCWord " 
		  << " bank index " << bank_index
		  << " slot num " << slot_num
		  << " chan num " << chan
		  << " hitcnt " << hitcnt
		  << " plane  " << plane
		  << " wire   " << element
		  << " package " << package 
		  << " diection " << direction
		  << " data " << data
		  << " fTDCHits.size() " <<  fTDCHits.size() 
		  << std::endl;
      }

      fTDCHits.push_back(
    			 QwHit(
    			       bank_index, 
    			       slot_num, 
    			       chan, 
                               hitcnt, 
                               kRegionIDCer, 
                               package, 
                               octant,
                               plane,
                               direction, 
                               element, 
    			       data
    			       )
    			 );

      
    }

    //  }
  } // (tdcindex not_eq -1) {
  
  return;
}



void  QwMainDetector::SubtractReferenceTimes()
{


  UInt_t   bank_index        = 0;
  Double_t raw_time_arb_unit = 0.0;
  Double_t ref_time_arb_unit = 0.0;
  Double_t time_arb_unit     = 0.0;
 // EQwDetectorPackage package = kPackageNull;  // Region 1 has only Package 2, so kPackageDown

  Bool_t local_debug = false;
  Int_t slot_num = 0;

  TString reference_name1 = "MasterTrigger";
  //  TString reference_name2 = "CopyMasterTrigger";


  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
    {

      bank_index        = hit -> GetSubbankID();
      slot_num          = hit -> GetModule();
      raw_time_arb_unit = (Double_t) hit -> GetRawTime();
      ref_time_arb_unit = fF1RefContainer -> GetReferenceTimeAU(bank_index, reference_name1);
      //
      // if there is no reference time due to a channel error, try to use a copy of mater trigger
      // 
      // if(ref_time_arb_unit==0.0) {
      // 	ref_time_arb_unit =  fF1RefContainer->GetReferenceTimeAU(bank_index, reference_name2);
      // }
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




void QwMainDetector::UpdateHits()
{
 
  // std::vector... is weird
  // I use a plane number to assign a f1tdc channel. Channel 99 is the F1TDC reference channel
  // , becaue I see plan is 0 in qweak_new.geo file and I think, nowhere plane is used in analyzer
  // But, I am wrong. This double std::vector fDetectorInfo uses plane number as second index,
  // So I had the notorious following error
  //  Terminate called after throwing an instance of 'std::out_of_range'
  //   what():  vector::_M_range_check
  // Abort
  //
  //
  // I don't care about QwDetectorInfo for other tracking subsystems except VDC/HDC,
  // I disable to fill QwDetectorInfo into QwHit

  // Thursday, January 26 10:30:09 EST 2012, jhlee


  // EQwDetectorPackage package = kPackageNull;
  // //  Int_t              plane   = 0;

  // QwDetectorID       local_id;
  // QwDetectorInfo  *local_info;


  // Int_t plane = 0;
  // Int_t element = 0;
  // Int_t hitnumber = 0;
  // Double_t timens = 0.0;
  // TString output = "";


  for(std::vector<QwHit>::iterator iter=fTDCHits.begin(); iter!=fTDCHits.end(); ++iter)
    {
      // local_id   = iter->GetDetectorID();
      // package    = local_id.fPackage;
      // //      plane      = local_id.fPlane;

   
      // local_info = fDetectorInfo.in(package).at(0); 
      // //      local_info = fDetectorInfo.in(package).at(plane); 

      // iter->SetDetectorInfo(local_info);
      iter->ApplyTimeCalibration(fF1TDCResolutionNS); // Fill fTimeRes and fTimeNs in QwHit


    }


  return;
}


//JAM
void QwMainDetector::AddSoftwareMeantimeToHits(Bool_t option)
{
  //if option is false, end this function
  if( !option ) return;
  // if fTDCHits size is zero, end this function
  if ( fTDCHits.size() ==0 ) return;

  Bool_t    local_debug = false; //local debug

  Long64_t             ev_num    = 0;
  Int_t                plane     = 0;
  Int_t                element   = 0;
  Int_t                octant    = 0;
  Int_t                hitnumber = 0;
  Double_t             timens   = 0.0;


  Int_t                bank_index  = 0;
  Int_t                slot_num    = 0;
  Int_t                chan_num    = 0;
  EQwDetectorPackage   package     = kPackageNull;
  EQwDirectionID       direction   = kDirectionNull;
  EQwRegionID          region      = kRegionIDCer;

  MeanTime*            md_mt_time = NULL;

  Int_t               v_plane_idx = 0;

  for( std::vector<QwHit>::iterator iter=fTDCHits.begin(); iter!=fTDCHits.end(); ++iter)
  {
    plane = iter->GetPlane();
    if( plane ==1 || plane == 2 || plane == 3 ||  plane ==4 || plane == 5 ||  plane ==6 || plane == 7 || plane == 8 )
      {
        v_plane_idx = plane-1;
        element     = iter->GetElement();
        hitnumber   = iter->GetHitNumber();
        timens      = iter->GetTimeNs();
	//	printf("plane %d element %d hitnumber %d time %12.2lf\n", plane, element, hitnumber, timens);
        fSoftwareMeantimeContainer[v_plane_idx] ->  Add(element, hitnumber, timens);
      } //end for
  } //end for loop JAM2 




  // Reset bank_index, slot_num, and chan_num to -9,-99,-999
  // because we don't have these numbers for software meantime
  //  and we don't need these numbers in otfer to access software meantime from QwHit

  ev_num      = VQwSubsystem::GetParent()->GetCodaEventNumber();
  bank_index  = -9;
  slot_num    = -99;
  chan_num    = -999;

  for (v_plane_idx=0; v_plane_idx<kMaxNumberOfQwHitPlane; v_plane_idx++)
  {
    if(v_plane_idx%2 ==0) {
      package = kPackage1;
      } // end if
    else if  (v_plane_idx%2 ==1) {
      package = kPackage2;
    } //end else if
    else {
      break;
    } //end else

    fSoftwareMeantimeContainer[v_plane_idx] -> SetEventId(ev_num);
    fSoftwareMeantimeContainer[v_plane_idx] -> SetTimeWindow(fSoftwareMeantimeTimeWindowNs);
    fSoftwareMeantimeContainer[v_plane_idx] -> ProcessMeanTime();
    plane = fSoftwareMeantimeContainer[v_plane_idx] -> GetPlane();

    md_mt_time = NULL;

    for (Int_t v_smt_idx=0; v_smt_idx < fSoftwareMeantimeContainer[v_plane_idx]->SoftwareMTSize(); v_smt_idx++)
    {
      md_mt_time =  fSoftwareMeantimeContainer[v_plane_idx]->GetMeanTimeObject(v_smt_idx); //Note: this should be md_mt_time
      md_mt_time  -> Print(local_debug);

      QwHit software_meantime_hit(bank_index, slot_num, chan_num, v_smt_idx, 
                                  region, package, octant, plane, direction,
                                  md_mt_time->GetSoftwareMeantimeHitElement()
                                  );
      software_meantime_hit.SetTimens(md_mt_time->GetMeanTime());
      fTDCHits.push_back(software_meantime_hit);
      
      QwHit software_positive_hit(bank_index, slot_num, chan_num, v_smt_idx, 
                                  region, package, octant, plane, direction,
                                  md_mt_time->GetSoftwarePositiveHitElement()
                                  );
      software_positive_hit.SetTimens(md_mt_time->GetPositiveValue());
      software_positive_hit.SetHitNumberR(md_mt_time->GetPositiveHitId());
      fTDCHits.push_back(software_positive_hit);
      
      QwHit software_negative_hit(bank_index, slot_num, chan_num, v_smt_idx, 
                                  region, package, octant, plane, direction,
                                  md_mt_time->GetSoftwareNegativeHitElement()
                                  );
      software_negative_hit.SetTimens(md_mt_time->GetNegativeValue());
      software_negative_hit.SetHitNumberR(md_mt_time->GetNegativeHitId());
      fTDCHits.push_back(software_negative_hit);
      
      QwHit software_subtract_hit(bank_index, slot_num, chan_num, v_smt_idx, 
                                  region, package, octant, plane, direction,
                                  md_mt_time->GetSoftwareSubtractHitElement()
                                  );
      software_subtract_hit.SetTimens(md_mt_time->GetSubtractTime());
      fTDCHits.push_back(software_subtract_hit);
      
    }//end internal for "v_smt_idx"

  } //end external for "v_plane_idx"


  if(local_debug) {

    TString output = "";

    for(std::vector<QwHit>::iterator iter=fTDCHits.begin(); iter!=fTDCHits.end(); ++iter)
    {

      bank_index    = iter->GetSubbankID();
      slot_num      = iter->GetModule();
      chan_num      = iter->GetChannel();
      package       = iter->GetPackage();
      plane         = iter->GetPlane();
      element       = iter->GetElement();
      hitnumber     = iter->GetHitNumber();
      timens        = iter->GetTimeNs();

      output += GetSubsystemName();
      output += Form(" Bank ID %+2d", bank_index);
      output += Form(" Slot %+3d", slot_num);
      output += Form(" Chan %+4d", chan_num);
      output += Form(" Package %2d", (Int_t) package);
      output += Form(" Plane %2d", plane);
      output += " Element ";
      output += element;
      output += " Hit ";
      output += hitnumber;
      output += Form(" TimeNs %+10.2f\n", timens);

    } //end for iter

    std::cout <<"Size fTDCHits " << fTDCHits.size()
      << " Event Number " <<ev_num << "\n" << output <<std::endl;
  } //end local_debug

  return;
} //end SoftwareMeantimeToHits member function




Int_t QwMainDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const
{
  Int_t modindex = -1;

  if ( bank_index < fModuleIndex.size()) {
    if ( slot_num < fModuleIndex.at(bank_index).size()) {
      modindex = fModuleIndex.at(bank_index).at(slot_num);
    }
  }
  return modindex;
}


Int_t QwMainDetector::FindSignalIndex(const EQwModuleType modtype, const TString &name) const
{
  Int_t chanindex = -1;
  if (modtype < (Int_t) fPMTs.size()) {
    for (size_t chan = 0; chan < fPMTs.at(modtype).size(); chan++) {
      if (name == fPMTs.at(modtype).at(chan).GetElementName()) {
	chanindex = chan;
	break;
      }
    }
  }
  return chanindex;
}

void
QwMainDetector::FillHardwareErrorSummary()
{
  fF1RefContainer -> PrintCounters();
  fF1TDContainer  -> PrintErrorSummary();
  fF1TDContainer  -> WriteErrorSummary();
  return;
};



void  QwMainDetector::ReportConfiguration(Bool_t verbose)
{

  if(verbose) {
    std::size_t i       = 0;
    std::size_t j       = 0;

    Int_t roc_num       = 0;
    Int_t bank_flag     = 0;
    Int_t bank_index    = 0;
    Int_t module_index  = 0;

    UInt_t slot_id      = 0;
    UInt_t vme_slot_num = 0;
    
    std::cout << " QwMainDetector : " 
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
	
	    for ( slot_id=2; slot_id<kMaxNumberOfModulesPerROC; slot_id++ ) 
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
			    << "Unused in "
			    << GetSubsystemName()
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


