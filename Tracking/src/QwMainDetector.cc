/*------------------------------------------------------------------------*//*!

  \file QwMainDetector.cc

  \author P. King
  \author P. Wang

  \date  2007-05-08 15:40

  \brief This is the main executable for the tracking analysis.

  \ingroup QwTracking
									    *//*-------------------------------------------------------------------------*/


#include "QwMainDetector.h"

// Qweak headers
#include "QwParameterFile.h"


const UInt_t QwMainDetector::kMaxNumberOfModulesPerROC     = 21;




// Register this subsystem with the factory
RegisterSubsystemFactory(QwMainDetector);

QwMainDetector::QwMainDetector(TString region_tmp):VQwSubsystem(region_tmp),
						   VQwSubsystemTracking(region_tmp)
{
  fDEBUG = false;
  ClearAllBankRegistrations();

  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerF1TDC = fF1TDCDecoder.GetTDCMaxChannels();
}

QwMainDetector::~QwMainDetector()
{
  fPMTs.clear();

  for (size_t i = 0; i < fSCAs.size(); i++)
    delete fSCAs.at(i);
  fSCAs.clear();

  delete fF1TDContainer;
}


Int_t QwMainDetector::LoadGeometryDefinition ( TString mapfile )
{
  std::cout<<"Main Detector Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires, detectorId, region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  fDetectorInfo.clear();

  //  Int_t pkg,pln;

  DIRMODE=0;

  QwParameterFile mapstr ( mapfile.Data() );  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while ( mapstr.ReadNextLine() )
    {
      mapstr.TrimComment ( '!' );   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if ( mapstr.LineIsEmpty() )  continue;

      if ( mapstr.HasVariablePair ( "=",varname,varvalue ) )
        {
          //  This is a declaration line.  Decode it.
          varname.ToLower();
          //UInt_t value = atol(varvalue.Data());
          if ( varname=="name" )   //Beginning of detector information
            {
              DIRMODE=1;
            }
        }
      else if ( DIRMODE==1 )
        {
          //  Break this line Int_to tokens to process it.
          varvalue = ( mapstr.GetNextToken ( ", " ).c_str() );//this is the sType
          Zpos = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          rot = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) * Qw::deg );
          sp_res = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          track_res = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          slope_match = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          package = mapstr.GetNextToken ( ", " ).c_str();
          region  = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
          dType = mapstr.GetNextToken ( ", " ).c_str();
          direction  = mapstr.GetNextToken ( ", " ).c_str();
          Det_originX = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          Det_originY = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          ActiveWidthX = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          ActiveWidthY = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          ActiveWidthZ = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          WireSpace = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          FirstWire = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          W_rcos = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          W_rsin = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
          TotalWires = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
          detectorId = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
          //std::cout<<"Detector ID "<<detectorId<<" "<<varvalue<<" Package "<<package<<" Plane "<<Zpos<<" Region "<<region<<std::endl;

          if ( region==5 )
            {
            QwDetectorInfo* detector = new QwDetectorInfo();
            detector->SetDetectorInfo(dType, Zpos,
                rot, sp_res, track_res, slope_match,
                package, region, direction,
                Det_originX, Det_originY,
                ActiveWidthX, ActiveWidthY, ActiveWidthZ,
                WireSpace, FirstWire,
                W_rcos, W_rsin,
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

  QwMessage << "Qweak Geometry Loaded " << QwLog::endl;

  return 0;
}


Int_t QwMainDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum=0, channum=0, slotnum=0;

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
          //UInt_t value = atol(varvalue.Data());
          UInt_t value = QwParameterFile::GetUInt(varvalue);
          if (varname=="roc")
            {
              RegisterROCNumber(value);
            }
          else if (varname=="qdc_bank")
            {
              fBankID[0] = value;
              RegisterSubbank(value);
            }
          else if (varname=="sca_bank")
            {
              fBankID[1] = value;
              RegisterSubbank(value);
            }
          else if (varname=="f1tdc_bank")
            {
              fBankID[2] = value;
              RegisterSubbank(value);
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
          modtype   = mapstr.GetNextToken(", ").c_str();
          modnum    = (atol(mapstr.GetNextToken(", ").c_str()));
          channum   = (atol(mapstr.GetNextToken(", ").c_str()));
          dettype   = mapstr.GetNextToken(", ").c_str();
          name      = mapstr.GetNextToken(", ").c_str();

          if (name=="md_reftime_f1") {
            reftime_slotnum = slotnum;
            reftime_channum = channum;
          }

          //  Push a new record into the element array
          if (modtype=="SIS3801")
            {
              if (modnum >= (Int_t) fSCAs.size())  fSCAs.resize(modnum+1);
              if (! fSCAs.at(modnum)) fSCAs.at(modnum) = new QwSIS3801_Module();
              fSCAs.at(modnum)->SetChannel(channum, name);
            }

          else if (modtype=="V792" || modtype=="V775" || modtype=="F1TDC")
            {
              RegisterModuleType(modtype);
              //  Check to see if we've encountered this channel or name yet
              if (fModulePtrs.at(fCurrentIndex).at(channum).first != kUnknownModuleType)
                {
                  //  We've seen this channel
                }
              else if (FindSignalIndex(fCurrentType, name) >= 0)
                {
                  //  We've seen this signal
                }
              else
                {
                  //  If not, push a new record into the element array
                  if (modtype=="V792") std::cout<<"V792: ";
                  else if (modtype=="V775") std::cout<<"V775: ";
                  else if (modtype=="F1TDC") std::cout<<"F1TDC: ";
                  LinkChannelToSignal(channum, name);
                }
            }

          else
            {
              std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str() << std::endl;
            }
        }
    }
  //ReportConfiguration();
  return 0;
}


void  QwMainDetector::ClearEventData()
{
  SetDataLoaded(kFALSE);
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).SetValue(0);
        }
    }

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      if (fSCAs.at(i) != NULL)
        {
          fSCAs.at(i)->ClearEventData();
        }
    }
}

Int_t QwMainDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t index = GetSubbankIndex(roc_id,bank_id);
  if (index>=0 && num_words>0)
    {
      //  We want to process the configuration data for this ROC.
      UInt_t words_read = 0;

      if (fBankID[1]==bank_id)
        {
          for (size_t i=0; i<fSCAs.size(); i++)
            {
              if (fSCAs.at(i) != NULL)
                {
                  words_read += fSCAs.at(i)->ProcessConfigBuffer(&(buffer[words_read]),
								 num_words-words_read);
                }
            }
        }

    }

  // Sorry, I don't know how to combine the below F1TDContainer routine with the above code,
  // thus I seperated them. It is a big ugly, but at least it works fine with less time
  // consuming....
  // Tuesday, September 28 14:26:48 EDT 2010, jhlee

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
  Int_t index = GetSubbankIndex(roc_id,bank_id);


  //This is a QDC bank
  if (bank_id == fBankID[0])
    {
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

              if (fQDCTDC.IsValidDataword())
                if (true)
                  {
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

  // This is a F1TDC bank
  else if (bank_id==fBankID[2])
    {

      Int_t  bank_index      = 0;
      Int_t  tdc_slot_number = 0;
      Int_t  tdc_chan_number = 0;
      UInt_t tdc_data        = 0;
      
      Bool_t data_integrity_flag = false;
      Bool_t temp_print_flag     = false;
      Int_t tdcindex = 0;
      Int_t tmp_last_chan = 65535; // for removing the multiple hits....

      bank_index = GetSubbankIndex(roc_id, bank_id);
      
      if (bank_index>=0 && num_words>0) {
	//  We want to process this ROC.  Begin looping through the data.
	SetDataLoaded(kTRUE);
	
	
	if (temp_print_flag ) {
	  std::cout << "QwMainDetector::ProcessEvBuffer:  "
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
		    if (tdc_slot_number == reftime_slotnum && tdc_chan_number == reftime_channum)
		      reftime = fF1TDCDecoder.GetTDCData();
		    tmp_last_chan = tdc_chan_number;
		  }
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
      
      // if (index>=0 && num_words>0)
      //   {
      // 	  SetDataLoaded(kTRUE);
      //     if (fDEBUG) std::cout << "QwScanner::ProcessEvBuffer:  "
      //       << "Begin processing F1TDC Bank "<<bank_id<< std::endl;

      //     Int_t tdc_slot_number = 0;
      //     Int_t tdc_chan_number = 0;
      //     Int_t tmp_last_chan = 65535;

      // 	  Bool_t data_integrity_flag = false;
      // 	  Bool_t temp_print_flag     = false;

      // 	  data_integrity_flag = fF1TDCDecoder.CheckDataIntegrity(roc_id, buffer, num_words);

      // 	  if (data_integrity_flag)
      // 	    {//;
      // 	      for (UInt_t i=0; i<num_words ; i++)
      // 		{

      // 		  fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id);

      // 		  tdc_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
      // 		  tdc_chan_number = fF1TDCDecoder.GetTDCChannelNumber();

      // 		  if ( tdc_slot_number == 31) {
      // 		    //  This is a custom word which is not defined in
      // 		    //  the F1TDC, so we can use it as a marker for
      // 		    //  other data; it may be useful for something.
      // 		  }

      // 		  // Each subsystem has its own interesting slot(s), thus
      // 		  // here, if this slot isn't in its slot(s) (subsystem map file)
      // 		  // we skip this buffer to do the further process
      // 		  if (! IsSlotRegistered(index, tdc_slot_number) ) continue;

      // 		  if ( fF1TDCDecoder.IsValidDataword() )
      // 		    {
      // 		      try {
      // 		    	if(tdc_chan_number != tmp_last_chan)
      // 		    	{
      // 			    FillRawWord(index, tdc_slot_number, tdc_chan_number, fF1TDCDecoder.GetTDCData());

      // 			    fF1TDCDecoder.PrintTDCData(temp_print_flag);
      // 			    if (tdc_slot_number == reftime_slotnum && tdc_chan_number == reftime_channum)
      // 			      reftime = fF1TDCDecoder.GetTDCData();
      // 			    tmp_last_chan = tdc_chan_number;

      // 		    	}
      // 		      }
      // 		      catch (std::exception& e) {
      // 			std::cerr << "Standard exception from QwMainDetector::FillRawTDCWord: "
      // 				  << e.what() << std::endl;
      // 			std::cerr << "   Parameters:  index=="  << index
      // 				  << "; GetF1SlotNumber()=="    << tdc_slot_number
      // 				  << "; GetF1ChannelNumber()==" << tdc_chan_number
      // 				  << "; GetF1Data()=="          << fF1TDCDecoder.GetTDCData()
      // 				  << std::endl;
      // 		      }
      // 		    }
      // 		   }
      // 	    }//; if(data_integrity_flag)
      // }
    }


  // This is a SCA bank
  else if (bank_id==fBankID[1])
    {
      // Check if scaler buffer contains more than one event
      if (buffer[0]/32!=1) return 0;

      if (index>=0 && num_words>0)
        {
	  SetDataLoaded(kTRUE);
          UInt_t words_read = 0;
          for (size_t i=0; i<fSCAs.size(); i++)
            {
              words_read++; // skip header word
              if (fSCAs.at(i) != NULL)
                {
                  words_read += fSCAs.at(i)->ProcessEvBuffer(&(buffer[words_read]),
							     num_words-words_read);
                }
              else
                {
                  words_read += 32; // skip a block of data for a single module
                }
            }
        }
    }

  return 0;
}


void  QwMainDetector::ProcessEvent()
{
  if (! HasDataLoaded()) return;

  TString elementname = "";
  Double_t rawtime = 0.0;
  Double_t newdata = 0.0;

  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ProcessEvent();
      elementname = fPMTs.at(i).at(j).GetElementName();
      rawtime = fPMTs.at(i).at(j).GetValue();
      if (elementname.EndsWith("f1") && rawtime!=0) {
	Int_t bank_index = fPMTs.at(i).at(j).GetSubbankID();
	Int_t slot_num   = fPMTs.at(i).at(j).GetModule();
	newdata = fF1TDContainer->ReferenceSignalCorrection(rawtime, reftime, bank_index, slot_num);
	//	Double_t newdata = fF1TDCDecoder.ActualTimeDifference(rawtime, reftime);
	fPMTs.at(i).at(j).SetValue(newdata);
      }
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->ProcessEvent();
    }
  }
}


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
      if (fSCAs.at(i) != NULL)
        {
          fSCAs.at(i)->ConstructHistograms(folder, prefix);
        }
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
      if (fSCAs.at(i) != NULL)
        {
          fSCAs.at(i)->FillHistograms();
        }
    }
}


void  QwMainDetector::ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t> &values)
{
  fTreeArrayIndex = values.size();

  TString basename;
  if (prefix=="") basename = "maindet";
  else basename = prefix;

  TString list = "";

  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          if (fPMTs.at(i).at(j).GetElementName()=="")
            {
              //  skip empty channel
            }
          else
            {
              values.push_back(0.0);
	      list += ":"+fPMTs.at(i).at(j).GetElementName()+"/D";
	      //std::cout<<"Added to list: "<<fPMTs.at(i).at(j).GetElementName()<<"\n"<<std::endl;
            }
        }
    }

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      if (fSCAs.at(i) != NULL)
        {

          for (size_t j=0; j<fSCAs.at(i)->fChannels.size(); j++)
            {
              if (fSCAs.at(i)->fChannels.at(j).GetElementName()=="") {}
              else
                {
                  values.push_back(0.0);
		  list += ":"+fSCAs.at(i)->fChannels.at(j).GetElementName()+"/D";
                }
            }
        }
    }

  if (':' == list[0])
    list = list(1,list.Length()-1);

  fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
  tree->Branch(basename, &values[fTreeArrayIndex], list);
  // std::cout<<list<<"\n";
  return;
}


void  QwMainDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  Int_t index = fTreeArrayIndex;

  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          if (fPMTs.at(i).at(j).GetElementName()=="") {}
          else
            {
              values[index] = fPMTs.at(i).at(j).GetValue();
              // std::cout<<"Fill data "<<values[index]<<" to index "
              // <<index<<" ch_name "<<fPMTs.at(i).at(j).GetElementName()<<"\n";
              index++;
            }
        }
    }

  if (fDEBUG)
    {
      std::cout<<"Main detector tree vector: "<<"\n";
      index = 1;
      for (size_t i=0; i<fPMTs.size(); i++)
        {
          for (size_t j=0; j<fPMTs.at(i).size(); j++)
            {
              std::cout<<  values[index]<<"\t";
              index++;
            }
        }
      std::cout<<"\n";
    }

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      if (fSCAs.at(i) != NULL)
        {
          for (size_t j=0; j<fSCAs.at(i)->fChannels.size(); j++)
            {
              if (fSCAs.at(i)->fChannels.at(j).GetElementName()=="") {}
              else
                {
                  values[index] = fSCAs.at(i)->fChannels.at(j).GetValue();
		  index++;
                }
            }
        }
      else
        {
          if (fDEBUG)
            std::cerr<<"QwMainDetector::FillTreeVector:  "<<"fSCA_Data.at("<<i<<") is NULL"<<std::endl;
        }
    }

}


void  QwMainDetector::DeleteHistograms()
{
  fF1TDContainer->PrintErrorSummary();
  fF1TDContainer->WriteErrorSummary();
  std::size_t i = 0;
  std::size_t j = 0;

  for ( i=0; i<fPMTs.size(); i++)
    {
      for ( j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).DeleteHistograms();
        }
    }

  for ( i=0; i<fSCAs.size(); i++)
    {
      if (fSCAs.at(i) != NULL)
        {
          fSCAs.at(i)->DeleteHistograms();
        }
    }


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
  fNumberOfModules = 0;
}

Int_t QwMainDetector::RegisterROCNumber(const UInt_t roc_id)
{
  VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return fCurrentBankIndex;
}

Int_t QwMainDetector::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex++;
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
}


Int_t QwMainDetector::RegisterSlotNumber(UInt_t slot_id)
{
  std::pair<EQwModuleType, Int_t> tmppair;
  tmppair.first  = kUnknownModuleType;
  tmppair.second = -1;
  if (slot_id<kMaxNumberOfModulesPerROC)
    {
      // fCurrentBankIndex is unsigned int and always positive
      if (/* fCurrentBankIndex >= 0 && */ fCurrentBankIndex <= fModuleIndex.size())
        {
          fModuleTypes.resize(fNumberOfModules+1);
          fModulePtrs.resize(fNumberOfModules+1);
          fModulePtrs.at(fNumberOfModules).resize(fF1TDCDecoder.GetTDCMaxChannels(),
                                                  tmppair);
          fNumberOfModules = fModulePtrs.size();
          fModuleIndex.at(fCurrentBankIndex).at(slot_id) = fNumberOfModules-1;
          fCurrentSlot     = slot_id;
          fCurrentIndex = fNumberOfModules-1;
        }
    }
  else
    {
      std::cerr << "QwMainDetector::RegisterSlotNumber:  Slot number "
		<< slot_id << " is larger than the number of slots per ROC, "
		<< kMaxNumberOfModulesPerROC << std::endl;
    }
  return fCurrentIndex;
}

EQwModuleType QwMainDetector::RegisterModuleType(TString moduletype)
{
  moduletype.ToUpper();

  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792")
    {
      fCurrentType = kV792_ADC;
    }
  else if (moduletype=="V775")
    {
      fCurrentType = kV775_TDC;
    }
  else if (moduletype=="F1TDC")
    {
      fCurrentType = kF1TDC;
    }

  fModuleTypes.at(fCurrentIndex) = fCurrentType;
  if ((Int_t) fPMTs.size()<=fCurrentType)
    {
      fPMTs.resize(fCurrentType+1);
    }
  return fCurrentType;
}


Int_t QwMainDetector::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
  if (fCurrentType == kV775_TDC || fCurrentType == kV792_ADC || fCurrentType == kF1TDC)
    {
      fPMTs.at(fCurrentType).push_back(QwPMT_Channel(name));
      fModulePtrs.at(fCurrentIndex).at(chan).first  = fCurrentType;
      fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(fCurrentType).size()-1;
    }
  else if (fCurrentType == kSIS3801)
    {
      std::cout<<"scaler module has not been implemented yet."<<std::endl;
    }
  std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
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


Int_t QwMainDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const
{
  Int_t modindex = -1;
  //std::cout<<"bank_index="<<bank_index<<" fModuleIndex.size()="<<fModuleIndex.size()<<"\n";

  // bank_index and slot_num are unsigned int and always positive
  if (/* bank_index >= 0 && */ bank_index < fModuleIndex.size())
    {
      if (/* slot_num >= 0 && */ slot_num < fModuleIndex.at(bank_index).size())
	{
	  modindex = fModuleIndex.at(bank_index).at(slot_num);
	}
    }
  return modindex;
}


Int_t QwMainDetector::FindSignalIndex(const EQwModuleType modtype, const TString &name) const
{
  Int_t chanindex = -1;
  if (modtype < (Int_t) fPMTs.size())
    {
      for (size_t chan = 0; chan < fPMTs.at(modtype).size(); chan++)
	{
	  if (name == fPMTs.at(modtype).at(chan).GetElementName())
	    {
	      chanindex = chan;
	      break;
	    }
	}
    }
  return chanindex;
}

void  QwMainDetector::ReportConfiguration()
{
  std::cout << "Configuration of main detector:"<< std::endl;
  for (size_t i = 0; i<fROC_IDs.size(); i++)
    {
      for (size_t j=0; j<fBank_IDs.at(i).size(); j++)
        {

          Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
          std::cout << "ROC " << fROC_IDs.at(i)
		    << ", subbank 0x"<<std::hex << fBank_IDs.at(i).at(j)<<std::dec
		    << ":  subbank index==" << ind << std::endl;

          if (fBank_IDs.at(i).at(j)==fBankID[0])
            {
              for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++)
                {
                  Int_t QadcTdcindex = GetModuleIndex(ind,k);
                  if (QadcTdcindex != -1)
                    {
                      std::cout << "    Slot " << k;
                      std::cout << "  Module#" << QadcTdcindex << std::endl;
                    }
                }
            }
          else if (fBank_IDs.at(i).at(j)==fBankID[2])
            {

            }
          else if (fBank_IDs.at(i).at(j)==fBankID[1])
            {
              // std::cout << "    Number of SIS3801 Scaler:  " << fSCAs.size() << std::endl;
            }
          else if (fBank_IDs.at(i).at(j)==fBankID[3])
            {
              // std::cout << "    Number of TRIUMF ADC:  " << fADC_Data.size() << std::endl;
            }
        }
    }
}
