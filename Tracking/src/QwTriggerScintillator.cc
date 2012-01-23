/**********************************************************\
* File: QwTriggerScintillator.cc                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2009-03-07 11:50>                           *
\**********************************************************/

#include "QwTriggerScintillator.h"

// Qweak headers
#include "QwParameterFile.h"

const UInt_t QwTriggerScintillator::kMaxNumberOfModulesPerROC     = 21;


// Register this subsystem with the factory
RegisterSubsystemFactory(QwTriggerScintillator);


QwTriggerScintillator::QwTriggerScintillator(const TString& name)
: VQwSubsystem(name),VQwSubsystemTracking(name)
{
  ClearAllBankRegistrations();
  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerF1TDC = fF1TDCDecoder.GetTDCMaxChannels();
}

QwTriggerScintillator::~QwTriggerScintillator()
{
  fPMTs.clear();
  fSCAs.clear();
  delete fF1TDContainer;
}


Int_t QwTriggerScintillator::LoadGeometryDefinition ( TString mapfile )
{
  std::cout<<"Trigger Scintillator Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  Int_t  plane, TotalWires, detectorId, region, DIRMODE;
  Double_t Zpos,rot,sp_res,track_res,slope_match,Det_originX,Det_originY,
    ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin,tilt;

  fDetectorInfo.clear();

  DIRMODE=0;

  QwParameterFile mapstr ( mapfile.Data() );  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if ( mapstr.LineIsEmpty() )  continue;

    if ( mapstr.HasVariablePair ( "=",varname,varvalue ) ) {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      //UInt_t value = atol(varvalue.Data());
      if ( varname=="name" ) { //Beginning of detector information
	DIRMODE=1;
      }
    }
    else if ( DIRMODE==1 ) {
      //  Break this line Int_to tokens to process it.
      varvalue = ( mapstr.GetTypedNextToken<TString>() );//this is the sType
      Zpos = mapstr.GetTypedNextToken<Double_t>();
      rot = mapstr.GetTypedNextToken<Double_t>() * Qw::deg;
      sp_res = mapstr.GetTypedNextToken<Double_t>();
      track_res = mapstr.GetTypedNextToken<Double_t>();
      slope_match = mapstr.GetTypedNextToken<Double_t>();
      package = mapstr.GetTypedNextToken<TString>();
      region  = mapstr.GetTypedNextToken<Int_t>();
      dType = mapstr.GetTypedNextToken<TString>();
      direction  = mapstr.GetTypedNextToken<TString>();
      Det_originX = mapstr.GetTypedNextToken<Double_t>();
      Det_originY = mapstr.GetTypedNextToken<Double_t>();
      ActiveWidthX = mapstr.GetTypedNextToken<Double_t>();
      ActiveWidthY = mapstr.GetTypedNextToken<Double_t>();
      ActiveWidthZ = mapstr.GetTypedNextToken<Double_t>();
      WireSpace = mapstr.GetTypedNextToken<Double_t>();
      FirstWire = mapstr.GetTypedNextToken<Double_t>();
      W_rcos = mapstr.GetTypedNextToken<Double_t>();
      W_rsin = mapstr.GetTypedNextToken<Double_t>();
      tilt = mapstr.GetTypedNextToken<Double_t>();
      TotalWires = mapstr.GetTypedNextToken<Int_t>();
      detectorId = mapstr.GetTypedNextToken<Int_t>();
      //std::cout<<"Detector ID "<<detectorId<<" "<<varvalue<<" Package "<<package<<" Plane "<<Zpos<<" Region "<<region<<std::endl;

      if ( region==4 ) {
        QwDetectorInfo* detector = new QwDetectorInfo();
        detector->SetDetectorInfo(dType, Zpos,
            rot, sp_res, track_res, slope_match,
            package, region, direction,
            Det_originX, Det_originY,
            ActiveWidthX, ActiveWidthY, ActiveWidthZ,
            WireSpace, FirstWire,
	    W_rcos, W_rsin, tilt,
            TotalWires,
            detectorId);
        fDetectorInfo.push_back(detector);
      }
    }
  }

  QwMessage << "Loaded Qweak Geometry" << " Total Detectors in kPackageUP "
      << fDetectorInfo.in(kPackageUp).size()
      << ", "
      << "kPackagDown "
      << fDetectorInfo.in(kPackageDown).size()
      << QwLog::endl;

  QwMessage << "Sorting detector info..." << QwLog::endl;

  plane = 1;
  QwGeometry detector_info_up = fDetectorInfo.in(kPackageUp);
  for (size_t i = 0; i < detector_info_up.size(); i++)
  {
    detector_info_up.at(i)->fPlane = plane++;
    QwMessage << " kPackageUp Region " << detector_info_up.at(i)->fRegion
        << " Detector ID " << detector_info_up.at(i)->fDetectorID
        << QwLog::endl;
  }

  plane = 1;
  QwGeometry detector_info_down = fDetectorInfo.in(kPackageDown);
  for (size_t i = 0; i < detector_info_down.size(); i++)
  {
    detector_info_down.at(i)->fPlane = plane++;
    QwMessage << " kPackageDown Region " << detector_info_down.at(i)->fRegion
        << " Detector ID " << detector_info_down.at(i)->fDetectorID
        << QwLog::endl;
  }

  mapstr.Close(); // Close the file (ifstream)
  QwMessage << "Qweak Geometry Loaded " << QwLog::endl;

  return 0;
}


Int_t QwTriggerScintillator::LoadChannelMap(TString mapfile){

  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum=0, channum=0, slotnum=0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname=="roc") {
	RegisterROCNumber(value);
      } else if (varname=="qdc_bank") {
	  RegisterSubbank(value);
          fBankID[0] = value;
      } else if (varname=="sca_bank") {
          fBankID[1] = value;
          RegisterSubbank(value);
      } else if (varname=="f1tdc_bank") {
	  RegisterSubbank(value);
          fBankID[2] = value;
      } else if (varname=="slot") {
          RegisterSlotNumber(value);
          slotnum = value;
      } else if (varname=="module") {
	  RegisterModuleType(varvalue);
      }
    } else {
        //  Break this line into tokens to process it.
        modtype   = mapstr.GetTypedNextToken<TString>();
        modnum    = mapstr.GetTypedNextToken<Int_t>();
        channum   = mapstr.GetTypedNextToken<Int_t>();
        dettype   = mapstr.GetTypedNextToken<TString>();
        name      = mapstr.GetTypedNextToken<TString>();

        // Check for the reference time channel
        if (name=="ts_reftime_f1") {
          reftime_slotnum = slotnum;
          reftime_channum = channum;
        }

        //  Push a new record into the element array
        if (modtype=="SIS3801") {
          QwSIS3801D24_Channel localchannel(name);
          localchannel.SetNeedsExternalClock(kFALSE);
          fSCAs.push_back(localchannel);
          fSCAs_map[name] = fSCAs.size()-1;
          Int_t offset = QwSIS3801D24_Channel::GetBufferOffset(modnum,channum);
          fSCAs_offset.push_back(offset);
        } else if (modtype=="V792" || modtype=="V775" || modtype=="F1TDC") {
            RegisterModuleType(modtype);
            //  Check to see if we've encountered this channel or name yet
            if (fModulePtrs.at(fCurrentIndex).at(channum).first != kUnknownModuleType) {
              //  We've seen this channel
            } else if (FindSignalIndex(fCurrentType, name)>=0) {
                //  We've seen this signal
            } else {
              //  If not, push a new record into the element array
              LinkChannelToSignal(channum, name);
            }
        } else {
            std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str() << std::endl;
        }
      }
  }

  mapstr.Close(); // Close the file (ifstream)
  return 0;
}


void  QwTriggerScintillator::ClearEventData(){
  SetDataLoaded(kFALSE);
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).SetValue(0);
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ClearEventData();
  }
}

Int_t QwTriggerScintillator::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  if( bank_id==fBankID[2] ) {

    TString subsystem_name;

    Int_t bank_index    = 0;
    Int_t tdc_index     = 0;
    UInt_t slot_id      = 0;
    UInt_t vme_slot_num = 0;

    Bool_t local_debug  = true;

    QwF1TDC *local_f1tdc = NULL;
   
    bank_index = GetSubbankIndex(roc_id, bank_id);

    if(bank_index >=0) {
      if(local_debug) {
	std::cout << "fF1TDContainer " << fF1TDContainer
		  <<" local_f1tdc    " << local_f1tdc << "\n";
      }
      subsystem_name = this->GetSubsystemName();
      fF1TDContainer -> SetSystemName(subsystem_name);
      
      if(local_debug) std::cout << "-----------------------------------------------------" << std::endl;
      
      std::cout << "QwTriggerScintillator : " 
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



Int_t QwTriggerScintillator::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) 
{
  Int_t index = 0;

  index = GetSubbankIndex(roc_id,bank_id);


  if (bank_id==fBankID[0]) { // V792 or V775
    if (index>=0 && num_words>0) {
      //  We want to process this ROC.  Begin looping through the data.
      SetDataLoaded(kTRUE);
      for(size_t i=0; i<num_words ; i++) {
        //  Decode this word as a V775TDC word.
        fQDCTDC.DecodeTDCWord(buffer[i]);

        if (! IsSlotRegistered(index, fQDCTDC.GetTDCSlotNumber())) continue;

        if (fQDCTDC.IsValidDataword()) {
	  // This is a V775 TDC data word
	  try {
	    FillRawWord(index,fQDCTDC.GetTDCSlotNumber(),fQDCTDC.GetTDCChannelNumber(),
			fQDCTDC.GetTDCData());
	  }
	  catch (std::exception& e) {
	    std::cerr << "Standard exception from QwTriggerScintillator::FillRawWord: "
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

  else if (bank_id==fBankID[1]) { // SIS Scalar

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

  else if (bank_id==fBankID[2]) { // F1TDC
   // reset the refrence time 
    reftime = 0.0;

    Bool_t local_debug_f1 = false;

    Int_t  bank_index      = 0;
    Int_t  tdc_slot_number = 0;
    Int_t  tdc_chan_number = 0;
    UInt_t tdc_data        = 0;
      
    Bool_t data_integrity_flag = false;
    Bool_t temp_print_flag     = false;
    Int_t tdcindex = 0;
    Int_t tmp_last_chan = 65535;

    bank_index = GetSubbankIndex(roc_id, bank_id);
      
    if (bank_index>=0 && num_words>0) {
      //  We want to process this ROC.  Begin looping through the data.
      SetDataLoaded(kTRUE);
	
	
      //
      // CheckDataIntegrity() do "counter" whatever errors in each F1TDC 
      // and check whether data is OK or not.
	
      data_integrity_flag = fF1TDContainer->CheckDataIntegrity(roc_id, buffer, num_words);
      // if it is false (TFO, EMM, and SYN), the whole buffer is excluded for
      // the further process, because of multiblock data transfer.
	
      if (data_integrity_flag) {
	  
	for (UInt_t i=0; i<num_words ; i++) {
	    
	  //  Decode this word as a F1TDC word.
	  fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id); 
	    
	  // For MQwF1TDC,   roc_id is needed to print out some warning messages.
	  	    
	  tdc_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
	  tdc_chan_number = fF1TDCDecoder.GetTDCChannelNumber();
	  tdcindex        = GetModuleIndex(bank_index, tdc_slot_number);
	    
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
	      if(tdc_chan_number != tmp_last_chan)
		{
		  tdc_data = fF1TDCDecoder.GetTDCData();
		  FillRawWord(bank_index, tdc_slot_number, tdc_chan_number, tdc_data);
		  //		  Check if this is reference time data
		  if ( IsF1ReferenceChannel(tdc_slot_number,tdc_chan_number) ) {
		    reftime = (Double_t) tdc_data;
		  }
		  if(local_debug_f1) {
		    printf("TS::ProcessEvBuffer: bank_index %2d slot_number [%2d,%2d] chan [%2d,%2d] data %10d %10.2f\n",
			   bank_index, tdc_slot_number, reftime_slotnum, tdc_chan_number, reftime_channum,tdc_data, reftime);
		  }

		  tmp_last_chan = tdc_chan_number;
		}
	    }
	    catch (std::exception& e) {
	      std::cerr << "Standard exception from QwTriggerScintillator::FillRawWord: "
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
      
    // if (index>=0 && num_words>0) {
    //   SetDataLoaded(kTRUE);
    //   if (fDEBUG) std::cout << "QwTriggerScintillator::ProcessEvBuffer:  "
    // 			    << "Begin processing F1TDC Bank "<<bank_id<< std::endl;

    //   Int_t tdc_slot_number = 0;
    //   Int_t tdc_chan_number = 0;
    //   Int_t tmp_last_chan = 65535;

    //   Bool_t data_integrity_flag = false;
    //   Bool_t temp_print_flag     = false;

    //   data_integrity_flag = fF1TDCDecoder.CheckDataIntegrity(roc_id, buffer, num_words);

    //   if (data_integrity_flag) {
    //     for (UInt_t i=0; i<num_words ; i++) {
    //       fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id);
    // 	  tdc_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
    // 	  tdc_chan_number = fF1TDCDecoder.GetTDCChannelNumber();

    // 	  if ( tdc_slot_number == 31) {
    //         //  This is a custom word which is not defined in
    //         //  the F1TDC, so we can use it as a marker for
    //         //  other data; it may be useful for something.
    //       }

    // 	  // Each subsystem has its own interesting slot(s), thus
    // 	  // here, if this slot isn't in its slot(s) (subsystem map file)
    // 	  // we skip this buffer to do the further process

    //       if (! IsSlotRegistered(index, tdc_slot_number) ) continue;
    // 	  if ( fF1TDCDecoder.IsValidDataword() )
    // 	    {
    // 	      try
    // 		{
    // 		  if(tdc_chan_number != tmp_last_chan)
    // 		    {
    // 		      FillRawWord(index, tdc_slot_number, tdc_chan_number, fF1TDCDecoder.GetTDCData());
    // 		      fF1TDCDecoder.PrintTDCData(temp_print_flag);

    // 		      // Check if this is reference time data
    // 		      if (tdc_slot_number == reftime_slotnum && tdc_chan_number == reftime_channum)
    //         		reftime = fF1TDCDecoder.GetTDCData();
    // 		      tmp_last_chan = tdc_chan_number;
    // 		    }
    // 		}
    // 	      catch (std::exception& e)
    // 		{
    // 		  std::cerr << "Standard exception from QwTriggerScintillator::FillRawWord: "
    // 			    << e.what() << std::endl;
    // 		  std::cerr << "   Parameters:  index=="  <<index
    // 			    << "; GetF1SlotNumber()=="    << tdc_slot_number
    // 			    << "; GetF1ChannelNumber()==" <<tdc_chan_number
    // 			    << "; GetF1Data()=="          <<fF1TDCDecoder.GetTDCData()
    // 			    << std::endl;
    // 		}
    // 	    }
    // 	}
    //   }
    // }
  }
  return 0;
}


void  QwTriggerScintillator::ProcessEvent()
{
  if (! HasDataLoaded()) return;

  TString elementname = "";
  Double_t rawtime = 0.0;
  Double_t corrected_time = 0.0;
  
  Int_t bank_index = 0;
  Int_t slot_num   = 0;

  for (size_t i=0; i<fPMTs.size(); i++) 
    {//;
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
	{//;;
	  fPMTs.at(i).at(j).ProcessEvent();
	  elementname = fPMTs.at(i).at(j).GetElementName();

	  // Check whether the element is "reftime" 
	  if ( not elementname.Contains("reftime") ) {
	    rawtime = fPMTs.at(i).at(j).GetValue();

	    if (elementname.EndsWith("f1") && rawtime!=0.0) {

	      bank_index = fPMTs.at(i).at(j).GetSubbankID();
	      slot_num   = fPMTs.at(i).at(j).GetModule();
	      // if the reference time signal is recorded by (a) channel(s) of F1TDC(s),
	      // we correct them. And if not, we set them to zero. (jhlee)

	      if ( reftime!=0.0 ) {
		corrected_time = fF1TDContainer->ReferenceSignalCorrection(rawtime, reftime, bank_index, slot_num);
	      }
	      else {
		corrected_time = 0.0;
	      }
	      fPMTs.at(i).at(j).SetValue(corrected_time);
	    } // if (elementname.EndsWith("f1") && rawtime!=0.0) {
	  } 
	  // if ( not elementname.Contains("reftime") ) {
	  //
	  // we keep the raw reference time information in an output ROOT file. 
	  //
	}//;;
    }//;

  
  for (size_t i=0; i<fSCAs.size(); i++)
    {
      fSCAs.at(i).ProcessEvent();
    }
  return;
};


void  QwTriggerScintillator::ConstructHistograms(TDirectory *folder, TString &prefix){

  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructHistograms(folder, prefix);
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).ConstructHistograms(folder, prefix);
  }

}

void  QwTriggerScintillator::FillHistograms(){
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).FillHistograms();
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++) {
    fSCAs.at(i).FillHistograms();
  }

}

void QwTriggerScintillator::ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t> &values)
{
  fTreeArrayIndex = values.size();

  TString basename;
  if (prefix=="") basename = "trigscint";
  else basename = prefix;

  TString list = "";
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      TString element_name = fPMTs.at(i).at(j).GetElementName();
      if (element_name=="") {
        // This channel is not used, so skip setting up the tree.
      } else {
          values.push_back(0.0);
          list += ":"+element_name+"/D";
      }
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++){
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

void  QwTriggerScintillator::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  Int_t index = fTreeArrayIndex;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      if (fPMTs.at(i).at(j).GetElementName()=="") {}
      else {
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

}


QwTriggerScintillator& QwTriggerScintillator::operator=  (const QwTriggerScintillator &value){
  if (fPMTs.size() == value.fPMTs.size()){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
	fPMTs.at(i).at(j) = value.fPMTs.at(i).at(j);
      }
    }
  } else {
    std::cerr << "QwTriggerScintillator::operator=:  Problems!!!"
	      << std::endl;
  }
  return *this;
}




void QwTriggerScintillator::ClearAllBankRegistrations(){
  VQwSubsystemTracking::ClearAllBankRegistrations();
  fModuleIndex.clear();
  fModulePtrs.clear();
  fModuleTypes.clear();
  fNumberOfModules = 0;
}

Int_t QwTriggerScintillator::RegisterROCNumber(const UInt_t roc_id){
  VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return fCurrentBankIndex;
}

Int_t QwTriggerScintillator::RegisterSubbank(const UInt_t bank_id){
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex++;
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
}

Int_t QwTriggerScintillator::RegisterSlotNumber(UInt_t slot_id)
{
  std::pair<EQwModuleType, Int_t> tmppair;
  tmppair.first  = kUnknownModuleType;
  tmppair.second = -1;
  if (slot_id < kMaxNumberOfModulesPerROC){
    // fCurrentBankIndex is unsigned int and always positive
    if (/* fCurrentBankIndex >= 0 && */ fCurrentBankIndex <= fModuleIndex.size()) {
      fModuleTypes.resize(fNumberOfModules+1);
      fModulePtrs.resize(fNumberOfModules+1);
      fModulePtrs.at(fNumberOfModules).resize(fF1TDCDecoder.GetTDCMaxChannels(),
					      tmppair);
      fNumberOfModules = fModulePtrs.size();
      fModuleIndex.at(fCurrentBankIndex).at(slot_id) = fNumberOfModules-1;
      fCurrentSlot     = slot_id;
      fCurrentIndex = fNumberOfModules-1;
    }
  } else {
    std::cerr << "QwTriggerScintillator::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfModulesPerROC << std::endl;
  }
  return fCurrentIndex;
}

EQwModuleType QwTriggerScintillator::RegisterModuleType(TString moduletype)
{
  moduletype.ToUpper();

  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792"){
    fCurrentType = kV792_ADC;
  } else if (moduletype=="V775"){
    fCurrentType = kV775_TDC;
  } else if (moduletype=="F1TDC") {
    fCurrentType = kF1TDC;
  } else if (moduletype=="SIS3801") {
    fCurrentType = kSIS3801;
  }
  fModuleTypes.at(fCurrentIndex) = fCurrentType;

  if ((Int_t) fPMTs.size() <= fCurrentType){
    fPMTs.resize(fCurrentType+1);
  }
  return fCurrentType;
}


Int_t QwTriggerScintillator::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
  fPMTs.at(fCurrentType).push_back(QwPMT_Channel(name));
  fModulePtrs.at(fCurrentIndex).at(chan).first  = fCurrentType;
  fModulePtrs.at(fCurrentIndex).at(chan).second =
    fPMTs.at(fCurrentType).size() -1;

  return 0;

}

void QwTriggerScintillator::FillRawWord(Int_t bank_index,
				 Int_t slot_num,
				 Int_t chan, UInt_t data){
  Int_t modindex = GetModuleIndex(bank_index,slot_num);
  if (modindex != -1){
    EQwModuleType modtype = fModulePtrs.at(modindex).at(chan).first;
    Int_t chanindex       = fModulePtrs.at(modindex).at(chan).second;
    if (modtype == kUnknownModuleType || chanindex == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
    } else {
      fPMTs.at(modtype).at(chanindex).SetValue(data);
      fPMTs.at(modtype).at(chanindex).SetSubbankID(bank_index);
      fPMTs.at(modtype).at(chanindex).SetModule(slot_num);
    }
  }
}


Int_t QwTriggerScintillator::GetModuleIndex(size_t bank_index, size_t slot_num) const {
  Int_t modindex = -1;
  // bank_index and slot_num are unsigned int and always positive
  if (/* bank_index >= 0 && */ bank_index < fModuleIndex.size()){
    if (/* slot_num >= 0 && */ slot_num < fModuleIndex.at(bank_index).size()){
      modindex = fModuleIndex.at(bank_index).at(slot_num);
    }
  }
  return modindex;
}


Int_t QwTriggerScintillator::FindSignalIndex(const EQwModuleType modtype, const TString &name) const
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


void QwTriggerScintillator::FillHardwareErrorSummary()
{
  fF1TDContainer->PrintErrorSummary();
  fF1TDContainer->WriteErrorSummary();
  //  fF1TDContainer->WriteErrorSummaryToDedicatedRootFile(rootfile);
  return;
};
