/**********************************************************\
* File: QwScanner.cc                                       *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Thu May 21 21:48:20 CDT 2009                             *
\**********************************************************/

#include "QwScanner.h"

// Qweak headers
#include "QwParameterFile.h"
#include "QwHistogramHelper.h"
#include "TString.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwScanner);

const Bool_t QwScanner::bStoreRawData = kTRUE;
const UInt_t QwScanner::kMaxNumberOfModulesPerROC     = 21;
//const UInt_t QwScanner::kMaxNumberOfChannelsPerModule = 32;

QwScanner::QwScanner(const TString& name)
: VQwSubsystem(name),
  VQwSubsystemParity(name),
  VQwSubsystemTracking(name)
{
  fDEBUG = 0;
  fEventTypeMask = 0xffff; // explicit because of diamond inheritance
  ClearAllBankRegistrations();

  fScaEventCounter = 0;
  fF1TDContainer = new QwF1TDContainer();
  fF1TDCDecoder  = fF1TDContainer->GetF1TDCDecoder();
  kMaxNumberOfChannelsPerModule = fF1TDCDecoder.GetTDCMaxChannels();
}


QwScanner::~QwScanner()
{
  fPMTs.clear();
  fSCAs.clear();
  fADCs.clear();

  delete fF1TDContainer;
}


void QwScanner::Copy(const VQwSubsystem *source)
{
  if (typeid(*source) == typeid(*this)) {
    VQwSubsystem::Copy(source);
    const QwScanner* input = dynamic_cast<const QwScanner*>(source);

    fSCAs = input->fSCAs;
    fADCs = input->fADCs;
    fPMTs = input->fPMTs;

    fHelicityFrequency = input->fHelicityFrequency;

    fHomePositionX = input->fHomePositionX;
    fHomePositionY = input->fHomePositionY;
    fVoltage_Offset_X = input->fVoltage_Offset_X;
    fVoltage_Offset_Y = input->fVoltage_Offset_Y;
    fChannel_Offset_X = input->fChannel_Offset_X;
    fChannel_Offset_Y = input->fChannel_Offset_Y;

    fCal_Factor_VQWK_X = input->fCal_Factor_VQWK_X;
    fCal_Factor_VQWK_Y = input->fCal_Factor_VQWK_Y;
    fCal_Factor_QDC_X = input->fCal_Factor_QDC_X;
    fCal_Factor_QDC_Y= input->fCal_Factor_QDC_Y;

    fRateMapCM = input->fRateMapCM;
    fRateMapEM = input->fRateMapEM;
  }
}


void QwScanner::ProcessOptions(QwOptions &options)
{
  //Handle command line options
}

Int_t QwScanner::LoadChannelMap(TString mapfile)
{
  Bool_t local_debug = false;
  TString varname, varvalue;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  // TString test = mapstr.GetParamFilename();
  // // char a[80];
  // // sprintf(a, "prminput/");
  // // std::cout << "index " << test.Index("prminput/") << std::endl;
  //std::cout << "QwScanner::LoadChannelMap(TString mapfile)" << mapstr.GetParamFilename() << std::endl;

  
  // fParameterFileNames.push_back(mapstr.GetParamFilename());
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
          else if (varname=="vqwk_bank")
            {
              fBankID[3] = value;
              RegisterSubbank(value);
            }
          else if (varname=="slot")
            {
              RegisterSlotNumber(value);
            }
        }
      else
        {
          //  Break this line into tokens to process it.
          TString modtype = mapstr.GetTypedNextToken<TString>();
          Int_t modnum    = mapstr.GetTypedNextToken<Int_t>();
          Int_t channum   = mapstr.GetTypedNextToken<Int_t>();
          TString dettype = mapstr.GetTypedNextToken<TString>();
          TString name    = mapstr.GetTypedNextToken<TString>();

          //  Push a new record into the element array
          if (modtype=="VQWK")
            {
              QwVQWK_Channel localchannel(name);
              fADCs.push_back(localchannel);
              fADCs_map[name] = fADCs.size()-1;
              Int_t offset = QwVQWK_Channel::GetBufferOffset(modnum,channum);
              fADCs_offset.push_back(offset);
            }

          else if (modtype=="SIS3801")
            {
              QwSIS3801D24_Channel localchannel(name);
              localchannel.SetNeedsExternalClock(kFALSE);
              fSCAs.push_back(localchannel);
              fSCAs_map[name] = fSCAs.size()-1;
              Int_t offset = QwSIS3801D24_Channel::GetBufferOffset(modnum,channum);
              fSCAs_offset.push_back(offset);
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
		  if(local_debug) {
		    //  If not, push a new record into the element array
		    if (modtype=="V792") std::cout<<"V792: ";
		    else if (modtype=="V775") std::cout<<"V775: ";
		    else if (modtype=="F1TDC") std::cout<<"F1TDC: ";
		  }
                  LinkChannelToSignal(channum, name);
                }
            }

          else
            {
	      if(local_debug) {
		std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
			  << std::endl;
	      }
            }
        }
    }
  mapstr.Close(); // Close the file (ifstream)
  if(local_debug) ReportConfiguration();
  return 0;
}

Int_t QwScanner::LoadInputParameters(TString parameterfile)
{
  Bool_t ldebug=kFALSE;
  TString localname;

  Int_t lineread=0;

  QwParameterFile mapstr(parameterfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine())
    {
      lineread+=1;
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;

      TString varname, varvalue;
      if (mapstr.HasVariablePair("=",varname,varvalue))
        {
          varname.ToLower();
          Double_t value = atof(varvalue.Data());
          if (varname=="helicityfrequency")
            {
              fHelicityFrequency = value;
            }
          else if (varname=="homepositionx")
            {
              fHomePositionX = value;
            }
          else if (varname=="homepositiony")
            {
              fHomePositionY = value;
            }
          else if (varname=="cal_factor_vqwk_x")
            {
              fCal_Factor_VQWK_X = value;
            }
          else if (varname=="cal_factor_vqwk_y")
            {
              fCal_Factor_VQWK_Y = value;
            }
          else if (varname=="cal_factor_qdc_x")
            {
              fCal_Factor_QDC_X = value;
            }
          else if (varname=="cal_factor_qdc_y")
            {
              fCal_Factor_QDC_Y = value;
            }
          else if (varname=="voltage_offset_x")
            {
              fVoltage_Offset_X = value;
            }
          else if (varname=="voltage_offset_y")
            {
              fVoltage_Offset_Y = value;
            }
          else if (varname=="channel_offset_x")
            {
              fChannel_Offset_X = value;
            }
          else if (varname=="channel_offset_y")
            {
              fChannel_Offset_Y = value;
            }
          if (ldebug) std::cout<<"inputs for "<<varname<<": "<<value<<"\n";
        }

      else
        {
          varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
          varname.ToLower();
          varname.Remove(TString::kBoth,' ');
          Double_t varped = mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
          Double_t varcal = mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
          if (ldebug)
            std::cout<<"inputs for channel "<<varname
            <<": ped="<<varped<<", cal="<<varcal<<"\n";
        }
    }
  if (ldebug) std::cout<<" line read in the parameter file ="<<lineread<<" \n";

  ldebug=kFALSE;
  mapstr.Close(); // Close the file (ifstream)
  return 0;
}


void  QwScanner::ClearEventData()
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
    fSCAs.at(i).ClearEventData();
  }

  for (size_t i=0; i<fADCs.size(); i++)
  {
    fADCs.at(i).ClearEventData();
  }

}


Int_t QwScanner::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
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
      
      if(local_debug) {
	std::cout << "-----------------------------------------------------" << std::endl;
	
	std::cout << "QwScanner : " 
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


Int_t QwScanner::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (fDEBUG)
    std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
    << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
    <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
    << ", num_words "<<num_words<<", index "<<index<<std::endl;

  //  This is a VQWK bank
  if (bank_id==fBankID[3]) {

    if (index>=0 && num_words>0) {
      SetDataLoaded(kTRUE);

      UInt_t words_read = 0;
      for (size_t i=0; i<fADCs.size(); i++) {
        words_read += fADCs.at(i).ProcessEvBuffer(&(buffer[fADCs_offset.at(i)]),
                                                  num_words-fADCs_offset.at(i));
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
      //   Int_t tmp_last_chan = 65535; // for removing the multiple hits....
      
      bank_index = GetSubbankIndex(roc_id, bank_id);
      
      if (bank_index>=0 && num_words>0) {
	//  We want to process this ROC.  Begin looping through the data.
	SetDataLoaded(kTRUE);
	
	
	if (temp_print_flag ) {
	  std::cout << "QwScanner::ProcessEvBuffer:  "
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
		// if(tdc_chan_number != tmp_last_chan)
		//   {
		tdc_data = fF1TDCDecoder.GetTDCData();
		FillRawWord(bank_index, tdc_slot_number, tdc_chan_number, tdc_data);
		// //		  Check if this is reference time data
		// if (tdc_slot_number == reftime_slotnum && tdc_chan_number == reftime_channum)
		//   reftime = fF1TDCDecoder.GetTDCData();
		// tmp_last_chan = tdc_chan_number;
		// }
	      }
	      catch (std::exception& e) {
		std::cerr << "Standard exception from QwScanner::FillRawWord: "
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
      //     SetDataLoaded(kTRUE);
      //     if (fDEBUG)
      //       std::cout << "QwScanner::ProcessEvBuffer:  "
      //       << "Begin processing F1TDC Bank 0x"<<std::hex<<bank_id<<std::dec<<std::endl;

      //     Int_t tdc_slot_number = 0;
      //     Int_t tdc_chan_number = 0;

      //     Bool_t data_integrity_flag = false;
      //     Bool_t temp_print_flag     = false;

      //     data_integrity_flag = fF1TDCDecoder.CheckDataIntegrity(roc_id, buffer, num_words);

      //     if (data_integrity_flag)
      //       {
      //         //;
      //         for (UInt_t i=0; i<num_words ; i++)
      //           {

      //             fF1TDCDecoder.DecodeTDCWord(buffer[i], roc_id);

      //             tdc_slot_number = fF1TDCDecoder.GetTDCSlotNumber();
      //             tdc_chan_number = fF1TDCDecoder.GetTDCChannelNumber();

      //             if ( tdc_slot_number == 31)
      //               {
      //                 //  This is a custom word which is not defined in
      //                 //  the F1TDC, so we can use it as a marker for
      //                 //  other data; it may be useful for something.
      //               }

      //             // Each subsystem has its own interesting slot(s), thus
      //             // here, if this slot isn't in its slot(s) (subsystem map file)
      //             // we skip this buffer to do the further process
      //             if (! IsSlotRegistered(index, tdc_slot_number) ) continue;

      //             if ( fF1TDCDecoder.IsValidDataword() )
      //               {
      //                 try
      //                   {
      //                     FillRawWord(index, tdc_slot_number, tdc_chan_number, fF1TDCDecoder.GetTDCData());
      //                     fF1TDCDecoder.PrintTDCData(temp_print_flag);
      //                   }
      //                 catch (std::exception& e)
      //                   {
      //                     std::cerr << "Standard exception from QwScanner::FillRawTDCWord: "
      //                     << e.what() << std::endl;
      //                     std::cerr << "   Parameters:  index=="  << index
      //                     << "; GetF1SlotNumber()=="    << tdc_slot_number
      //                     << "; GetF1ChannelNumber()==" << tdc_chan_number
      //                     << "; GetF1Data()=="          << fF1TDCDecoder.GetTDCData()
      //                     << std::endl;
      //                   }
      //               }
      //           }
      //       }//; if(data_integrity_flag)
      //   }
    }


  // This is a SCA bank
  if (bank_id==fBankID[1]) {

    // Check if scaler buffer contains more than one event
    if (buffer[0]/32!=1) return 0;

    if (index>=0 && num_words>0) {
      SetDataLoaded(kTRUE);
      fScaEventCounter++;
      //  This is a SCA bank We want to process this ROC.  Begin looping through the data.

      UInt_t words_read = 0;
      for (size_t i=0; i<fSCAs.size(); i++) {
        words_read += fSCAs.at(i).ProcessEvBuffer(&(buffer[fSCAs_offset.at(i)]),
                                                  num_words-fSCAs_offset.at(i));
      }
    }
  }


  //  This is a QADC/TDC bank
  if (bank_id==fBankID[0])
    {
      if (index>=0 && num_words>0)
        {
          SetDataLoaded(kTRUE);
          //  We want to process this ROC.  Begin looping through the data.
          if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
            << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
            <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"<< std::endl;


          if (fDEBUG)
            std::cout<<"QwScanner::ProcessEvBuffer (trig) Data: \n";

          for (size_t i=0; i<num_words ; i++)
            {
              //  Decode this word as a V775TDC word.
              fQDCTDC.DecodeTDCWord(buffer[i]);

              if (! IsSlotRegistered(index, fQDCTDC.GetTDCSlotNumber())) continue;

              if (fQDCTDC.IsValidDataword())
                {
                  // This is a V775 TDC data word
                  if (fDEBUG)
                    {
                      std::cout<<"This is a valid QDC/TDC data word. Index="<<index
                      <<" slot="<<fQDCTDC.GetTDCSlotNumber()<<" Ch="<<fQDCTDC.GetTDCChannelNumber()
                      <<" Data="<<fQDCTDC.GetTDCData()<<"\n";
                    }

                  try
                    {
                      // The slot number should be set properly in DAQ
                      // using 0 if it is not set
                      FillRawWord(index,fQDCTDC.GetTDCSlotNumber(),fQDCTDC.GetTDCChannelNumber(),
                                  fQDCTDC.GetTDCData());
                      //FillRawWord(index,0,GetTDCChannelNumber(),GetTDCData());
                    }
                  catch (std::exception& e)
                    {
                      std::cerr << "Standard exception from FocalPlaneScanner::FillRawTDCWord: "
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

  return 0;
}


void  QwScanner::ProcessEvent()
{
  if (! HasDataLoaded()) return;
  //std::cout<<"Scanner Events will be processed here."<<std::endl;

  TString elementname = "";
  Double_t rawtime = 0.0;
  Double_t reftime = 0.0;

  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).ProcessEvent();
          if (fPMTs.at(i).at(j).GetElementName() == TString("ref_t_f1"))
            reftime = fPMTs.at(i).at(j).GetValue();
        }
    }
  
  Int_t bank_index = 0;
  Int_t slot_num   = 0;	  
  Double_t newdata = 0.0;

  // F1TDC reference time subtraction
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          elementname = fPMTs.at(i).at(j).GetElementName();
          if (elementname.EndsWith("f1") && elementname != TString("ref_t_f1"))
            {
              rawtime = fPMTs.at(i).at(j).GetValue();

              // only subtract reftime if channel value is nonzero
              if (rawtime!=0)
                {
		  bank_index = fPMTs.at(i).at(j).GetSubbankID();
		  slot_num   = fPMTs.at(i).at(j).GetModule();
		  newdata    = fF1TDContainer->ReferenceSignalCorrection(rawtime, reftime, bank_index, slot_num);
                  fPMTs.at(i).at(j).SetValue(newdata);
                }
            }
        }
    }

  for (size_t i=0; i<fADCs.size(); i++)
  {
    fADCs.at(i).ProcessEvent();
  }

  for (size_t i=0; i<fSCAs.size(); i++)
  {
    fSCAs.at(i).ProcessEvent();
  }

  //Fill trigger data
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {

          TString element_name = fPMTs.at(i).at(j).GetElementName();
          Double_t tmpvalue = fPMTs.at(i).at(j).GetValue();

          if (element_name==TString("front_adc"))
            fFrontADC = tmpvalue;
          else if (element_name==TString("back__adc"))
            fBackADC = tmpvalue;
          else if (element_name==TString("front_f1"))
            fFrontTDC = tmpvalue;
          else if (element_name==TString("back__f1"))
            fBackTDC = tmpvalue;
          // TODO jpan: replace the position determination with interplation table
          else if (element_name==TString("pos_x_adc"))
            {
              if (tmpvalue>0)
                {
                  fPositionX_ADC = tmpvalue;
                  fPositionX_ADC = (fPositionX_ADC-fChannel_Offset_X)*fCal_Factor_QDC_X + fHomePositionX;

                  if (fScaEventCounter>1)
                    {
                      fMeanPositionX_ADC = 0.5*(fMeanPositionX_ADC + fPositionX_ADC);
                    }
                  else
                    {
                      fMeanPositionX_ADC = fPositionX_ADC;
                    }
                }
            }
          else if (element_name==TString("pos_y_adc"))
            {
              if (tmpvalue>0)
                {
                  fPositionY_ADC = tmpvalue;
                  fPositionY_ADC = (fPositionY_ADC-fChannel_Offset_Y)*fCal_Factor_QDC_Y + fHomePositionY;

                  if (fScaEventCounter>1)
                    {
                      fMeanPositionY_ADC = 0.5*(fMeanPositionY_ADC + fPositionY_ADC);
                    }
                  else
                    {
                      fMeanPositionY_ADC = fPositionY_ADC;
                    }
                }
            }

          /*            std::cout<<"fPositionX_ADC="<<fPositionX_ADC<<", fPositionY_ADC="<<fPositionY_ADC<<"\n";
                      std::cout<<"fMeanPositionX_ADC="<<fMeanPositionX_ADC
                               <<",fMeanPositionY_ADC="<<fMeanPositionY_ADC<<"\n";*/
        }
    }

  // Fill position data
  for (size_t i=0; i<fADCs.size(); i++)
  {
    // TODO replace the position determination with interplation table
    const double volts_per_bit = (20./(1<<18));
    double num_samples;
    fPowSupply_VQWK = fADCs.at(fADCs_map["power_vqwk"]).GetRawValue();
    //std::cout<<"fPowSupply_VQWK_HardSum = "<<fPowSupply_VQWK;
    num_samples = fADCs.at(fADCs_map["power_vqwk"]).GetNumberOfSamples();
    //std::cout<<", num_samples = "<<num_samples;
    fPowSupply_VQWK = fPowSupply_VQWK * volts_per_bit / num_samples;
    //std::cout<<" fPowSupply_VQWK = "<<fPowSupply_VQWK<<std::endl<<std::endl;


    fPositionX_VQWK = fADCs.at(fADCs_map["pos_x_vqwk"]).GetRawValue();
    //std::cout<<"fPositionX_VQWK_HardSum = "<<fPositionX_VQWK;
    num_samples = fADCs.at(fADCs_map["pos_x_vqwk"]).GetNumberOfSamples();
    //std::cout<<", num_samples = "<<num_samples;
    fPositionX_VQWK = fPositionX_VQWK * volts_per_bit / num_samples;
    //std::cout<<"  fPositionX = "<<fPositionX_VQWK<<"  [V]";
    fPositionX_VQWK = (fPositionX_VQWK-fVoltage_Offset_X)*fCal_Factor_VQWK_X + fHomePositionX;
    //std::cout<<"  fPositionX = "<<fPositionX_VQWK<<std::endl<<std::endl;

    //fPositionY_VQWK = fADC_Data.at(i)->GetChannel(TString("pos_y_vqwk"))->GetAverageVolts();
    fPositionY_VQWK = fADCs.at(fADCs_map["pos_y_vqwk"]).GetRawValue();
    //std::cout<<"fPositionY_VQWK_HardSum = "<<fPositionY_VQWK;
    num_samples = fADCs.at(fADCs_map["pos_y_vqwk"]).GetNumberOfSamples();
    //std::cout<<", num_samples = "<<num_samples;
    fPositionY_VQWK = fPositionY_VQWK * volts_per_bit / num_samples;
    //std::cout<<"  fPositionY = "<<fPositionY_VQWK<<"  [V]";
    fPositionY_VQWK = (fPositionY_VQWK-fVoltage_Offset_Y)*fCal_Factor_VQWK_Y + fHomePositionY;
    //std::cout<<"  fPositionY = "<<fPositionY_VQWK<<std::endl<<std::endl;
  }

  // Fill scaler data
  for (size_t i=0; i<fSCAs.size(); i++)
  {
    fCoincidenceSCA = fHelicityFrequency*(fSCAs.at(fSCAs_map["coinc_sca"]).GetValue());
    fFrontSCA = fHelicityFrequency*(fSCAs.at(fSCAs_map["front_sca"]).GetValue());
    fBackSCA = fHelicityFrequency*(fSCAs.at(fSCAs_map["back__sca"]).GetValue());
  }

}


void  QwScanner::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  // TODO (wdc) disabled due to restriction imposed by memory mapped file
  // Also changes to ConstructHistograms() calls below.
  //TDirectory* scannerfolder = folder->mkdir("scanner");

  if (folder != NULL) folder->cd();

  TString basename = prefix + "scanner_";
  if (bStoreRawData)
    {
      for (size_t i=0; i<fPMTs.size(); i++)
        {
          for (size_t j=0; j<fPMTs.at(i).size(); j++)
            fPMTs.at(i).at(j).ConstructHistograms(folder,basename);
        }

      for (size_t i=0; i<fSCAs.size(); i++)
        {
          fSCAs.at(i).ConstructHistograms(folder,basename);
        }

      for (size_t i=0; i<fADCs.size(); i++)
        {
          fADCs.at(i).ConstructHistograms(folder,basename);
        }
    }


  fHistograms.push_back( gQwHists.Construct1DHist(TString("scanner_vqwk_power")));
  fHistograms.push_back( gQwHists.Construct1DHist(TString("scanner_position_x")));
  fHistograms.push_back( gQwHists.Construct1DHist(TString("scanner_position_y")));
  fHistograms.push_back( gQwHists.Construct1DHist(TString("scanner_ref_posi_x")));
  fHistograms.push_back( gQwHists.Construct1DHist(TString("scanner_ref_posi_y")));

  // fHistograms.push_back( gQwHists.Construct2DHist(TString("scanner_rate_map")));

  //TProfile2D(const char* name, const char* title,
  // Int_t nbinsx, Double_t xlow, Double_t xup,
  // Int_t nbinsy, Double_t ylow, Double_t yup,
  // Option_t* option = "")

  fRateMapCM  = new TProfile2D("scanner_rate_map_cm",
      "Scanner Rate Map (Current Mode)",110,-55.0,55.0,40,-360.0,-320.0);
  fRateMapCM->GetXaxis()->SetTitle("PositionX [cm]");
  fRateMapCM->GetYaxis()->SetTitle("PositionY [cm]");
  fRateMapCM->SetOption("colz");

  fRateMapEM  = new TProfile2D("scanner_rate_map_em",
      "Scanner Rate Map (Event Mode)",110,-55.0,55.0,40,-360.0,-320.0);
  fRateMapEM->GetXaxis()->SetTitle("PositionX [cm]");
  fRateMapEM->GetYaxis()->SetTitle("PositionY [cm]");
  fRateMapEM->SetOption("colz");

  // fParameterFileNamesHist = new TH1F("scanner_parameter_files", "scanner_parameter_files", 10,0,10);
  // fParameterFileNamesHist -> SetBit(TH1::kCanRebin);
  // fParameterFileNamesHist -> SetStats(0);
  // for (std::size_t i=0; i< fParameterFileNames.size(); i++) {
  // 	fParameterFileNamesHist -> Fill(fParameterFileNames[i].Data(), 1);
  // }
  // const char* opt = "TEXT";
  // fParameterFileNamesHist->SetMarkerSize(1.4);
  // fParameterFileNamesHist->SetOption(opt);
  // fParameterFileNamesHist->LabelsDeflate("X");
  // fParameterFileNamesHist->LabelsOption("avu", "X");
}

void  QwScanner::FillHistograms()
{
  if (! HasDataLoaded()) return;

  if (bStoreRawData)
    {
      // Fill trigger data
      for (size_t i=0; i<fPMTs.size(); i++)
        {
          for (size_t j=0; j<fPMTs.at(i).size(); j++)
            {
              fPMTs.at(i).at(j).FillHistograms();
            }
        }

      // Fill scaler data
      for (size_t i=0; i<fSCAs.size(); i++)
        {
              fSCAs.at(i).FillHistograms();
        }

      // Fill position data
      for (size_t i=0; i<fADCs.size(); i++)
        {
          fADCs.at(i).FillHistograms();
        }

    }

  for (size_t j=0; j<fHistograms.size();j++)
    {

      if (fHistograms.at(j)->GetTitle()==TString("scanner_position_x"))
        {
          fHistograms.at(j)->Fill(fPositionX_VQWK);
        }

      if (fHistograms.at(j)->GetTitle()==TString("scanner_position_y"))
        {
          fHistograms.at(j)->Fill(fPositionY_VQWK);
        }

      if (fHistograms.at(j)->GetTitle()==TString("scanner_ref_posi_x"))
        {
          fHistograms.at(j)->Fill(fPositionX_ADC);
        }

      if (fHistograms.at(j)->GetTitle()==TString("scanner_ref_posi_y"))
        {
          fHistograms.at(j)->Fill(fPositionY_ADC);
        }
    }

  //Fill rate map
//   Double_t rate;
//   for (size_t j=0; j<fHistograms.size();j++)
//     {
//       if (fHistograms.at(j)->GetTitle()==TString("scanner_rate_map"))
//         {
//           Int_t checkvalidity = 1;
//           Double_t prevalue = get_value( fHistograms.at(j), fPositionX_ADC, fPositionY_ADC, checkvalidity);
//           if (checkvalidity!=0)
//             {
//               rate = (prevalue + fCoincidenceSCA)*0.5;  //average value for this bin
//
//               fHistograms.at(j)->SetBinContent((Int_t) fPositionX_ADC, (Int_t)fPositionY_ADC,rate);
//               Int_t xbin = fHistograms.at(j)->GetXaxis()->FindBin( fPositionX_ADC );
//               Int_t ybin = fHistograms.at(j)->GetYaxis()->FindBin( fPositionY_ADC );
//               fHistograms.at(j)->SetBinContent( fHistograms.at(j)->GetBin( xbin, ybin ), rate);
//             }
//         }
//     }

  fRateMapCM->Fill(fPositionX_VQWK,fPositionY_VQWK,fCoincidenceSCA,1);

  if ( fScaEventCounter>1 && fCoincidenceSCA>0)
    {
      //std::cout<<"Fill histo: "<<fMeanPositionX_ADC<<", "<<fMeanPositionY_ADC<<", "<<fCoincidenceSCA<<"\n";
      fRateMapEM->Fill(fMeanPositionX_ADC,fMeanPositionY_ADC,fCoincidenceSCA,1);
    } 

  
}


void  QwScanner::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  fTreeArrayIndex = values.size();

  values.push_back(0.0);
  TString list = prefix + "PowSupply_VQWK/D";
  values.push_back(0.0);
  list += ":" + prefix  + "PositionX_VQWK/D";
  values.push_back(0.0);
  list += ":" + prefix  + "PositionY_VQWK/D";
  values.push_back(0.0);
  list += ":" + prefix  + "FrontSCA/D";
  values.push_back(0.0);
  list += ":" + prefix  + "BackSCA/D";
  values.push_back(0.0);
  list += ":" + prefix  + "CoincidenceSCA/D";
  values.push_back(0.0);
  list += ":" + prefix  + "FrontADC/D";
  values.push_back(0.0);
  list += ":" + prefix  + "BackADC/D";
  values.push_back(0.0);
  list += ":" + prefix  + "FrontTDC/D";
  values.push_back(0.0);
  list += ":" + prefix  + "BackTDC/D";
  values.push_back(0.0);
  //list += ":" + prefix  + "PowSupply_QDC/D";
  //values.push_back(0.0);
  list += ":" + prefix  + "PositionX_QDC/D";
  values.push_back(0.0);
  list += ":" + prefix  + "PositionY_QDC/D";

  if (bStoreRawData) {
    for (size_t i=0; i<fPMTs.size(); i++) {
      for (size_t j=0; j<fPMTs.at(i).size(); j++) {
        if (fPMTs.at(i).at(j).GetElementName() != "") {
          values.push_back(0.0);
          list += ":" + fPMTs.at(i).at(j).GetElementName() + "_raw/D";
        }
      }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
      if (fSCAs.at(i).GetElementName() != "") {
        values.push_back(0.0);
        list += ":" + fSCAs.at(i).GetElementName() + "_raw/D";
      }
    }

    for (size_t i=0; i<fADCs.size(); i++) {
      TString channelname = fADCs.at(i).GetElementName();
      channelname.ToLower();
      if ( (channelname == "")
        || (channelname == "empty")
        || (channelname == "spare")) {}
      else {
        values.push_back(0.0);
        list += ":" + fADCs.at(i).GetElementName() + "_raw/D";
      }
    }
  }

  fTreeArrayNumEntries = values.size() - fTreeArrayIndex;

  TString basename = "scanner";
  if (gQwHists.MatchDeviceParamsFromList(basename.Data()))
    tree->Branch(basename, &values[fTreeArrayIndex], list);
}


void  QwScanner::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  Int_t index = fTreeArrayIndex;
  values[index++] = fPowSupply_VQWK;
  values[index++] = fPositionX_VQWK;
  values[index++] = fPositionY_VQWK;
  values[index++] = fFrontSCA;
  values[index++] = fBackSCA;
  values[index++] = fCoincidenceSCA;
  values[index++] = fFrontADC;
  values[index++] = fBackADC;
  values[index++] = fFrontTDC;
  values[index++] = fBackTDC;
//    values[index++] = fPowSupply_ADC;
  values[index++] = fPositionX_ADC;
  values[index++] = fPositionY_ADC;

  if (bStoreRawData) {

    // Fill trigger values
    for (size_t i=0; i<fPMTs.size(); i++) {
      for (size_t j=0; j<fPMTs.at(i).size(); j++) {
        if (fPMTs.at(i).at(j).GetElementName() != "") {
          values[index++] = fPMTs.at(i).at(j).GetValue();
        }
      }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
      if (fSCAs.at(i).GetElementName() != "") {
        values[index++] = fSCAs.at(i).GetValue();
      }
    }

    // Fill sumvalues
    for (size_t i=0; i<fADCs.size(); i++) {
      TString channelname = fADCs.at(i).GetElementName();
      channelname.ToLower();
      if ( (channelname =="")
        || (channelname.BeginsWith("empty"))
        || (channelname.BeginsWith("spare")) ) { }
      else {
        //values[index++] = fADC_Data.at(i)->fChannels.at(j).GetValue();
        values[index++] = fADCs.at(i).GetAverageVolts();
      }
    }
  }
}


void  QwScanner::ReportConfiguration()
{
  std::cout << "Configuration of the focal plane scanner:"<< std::endl;
  for (size_t i = 0; i<fROC_IDs.size(); i++)
    {
      for (size_t j=0; j<fBank_IDs.at(i).size(); j++)
        {

          Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
          std::cout << "ROC " << fROC_IDs.at(i)
          << ", subbank 0x" << std::hex<<fBank_IDs.at(i).at(j)<<std::dec
          << ":  subbank index==" << ind << std::endl;

          if (fBank_IDs.at(i).at(j)==fBankID[0])
            {
              for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++)
                {
                  Int_t QadcTdcindex = GetModuleIndex(ind,k);
                  if (QadcTdcindex != -1)
                    {
                      std::cout << "    Slot "<<k<<"  Module#"<<QadcTdcindex<< std::endl;
                    }
                }
            }
          else if (fBank_IDs.at(i).at(j)==fBankID[1])
            {
              std::cout << "    Number of SIS3801 Scaler:  " << fSCAs.size() << std::endl;
            }
          else if (fBank_IDs.at(i).at(j)==fBankID[2])
            {
              std::cout << "    F1TDC added." << std::endl;
            }
          else if (fBank_IDs.at(i).at(j)==fBankID[3])
            {
              std::cout << "    Number of TRIUMF ADC:  " << fADCs.size() << std::endl;
            }
        }
    }

}

Bool_t QwScanner::Compare(VQwSubsystem* value)
{
  if (typeid(*value) != typeid(*this)) {
    QwError << "QwScanner::Compare: "
            << "value is " << typeid(*value).name() << ", "
            << "but this is " << typeid(*this).name() << "!"
            << QwLog::endl;
    return kFALSE;
  } else {
    QwScanner* input = dynamic_cast<QwScanner*>(value);
    if (input->fSCAs.size() != fSCAs.size() ||
        input->fPMTs.size() != fPMTs.size() ||
        input->fADCs.size() != fADCs.size()) {
      QwError << "QwScanner::Compare: "
              << "value is " << input->fSCAs.size() << "," << input->fPMTs.size() << "," << input->fADCs.size() << ", "
              << "but this is " << fSCAs.size() << "," << fPMTs.size() << "," << fADCs.size() << "!"
              << QwLog::endl;
      return kFALSE;
    } else {
      return kTRUE;
    }
  }
}


VQwSubsystem& QwScanner::operator=(VQwSubsystem* value)
{
  if (Compare(value)) {
    VQwSubsystem::operator=(value);
    QwScanner* input = dynamic_cast<QwScanner*>(value);
    for (size_t i = 0; i < fPMTs.size(); i++)
      for (size_t j = 0; j < fPMTs.at(i).size(); j++)
        fPMTs.at(i).at(j) = input->fPMTs.at(i).at(j);
    for (size_t i = 0; i < fADCs.size(); i++)
      fADCs.at(i) = input->fADCs.at(i);
    for (size_t i = 0; i < fSCAs.size(); i++)
      fSCAs.at(i) = input->fSCAs.at(i);

    fScaEventCounter = input->fScaEventCounter;

    fPowSupply_VQWK = input->fPowSupply_VQWK;
    fPositionX_VQWK = input->fPositionX_VQWK;
    fPositionY_VQWK = input->fPositionY_VQWK;
    fFrontSCA = input->fFrontSCA;
    fBackSCA = input->fBackSCA;
    fCoincidenceSCA = input->fCoincidenceSCA;

    fFrontADC = input->fFrontADC;
    fFrontTDC = input->fFrontTDC;
    fBackADC = input->fBackADC;
    fBackTDC = input->fBackTDC;
    fPositionX_ADC = input->fPositionX_ADC;
    fPositionY_ADC = input->fPositionY_ADC;

    fMeanPositionX_ADC = input->fMeanPositionX_ADC;
    fMeanPositionY_ADC = input->fMeanPositionY_ADC;

    for (size_t i = 0; i < 4; i++)
      fBankID[i] = input->fBankID[i];

  } else {
    QwError << "QwScanner::operator=:  Problems!!!"
            << QwLog::endl;
  }
  return *this;
}


VQwSubsystem& QwScanner::operator+=(VQwSubsystem* value)
{
  if (Compare(value)) {
    QwScanner* input = dynamic_cast<QwScanner*>(value);
    // operator+= not defined for tracking-type data element QwPMT_Channel
    //for (size_t i = 0; i < fPMTs.size(); i++)
    //  for (size_t j = 0; j < fPMTs.at(i).size(); j++)
    //    fPMTs.at(i).at(j) += input->fPMTs.at(i).at(j);
    for (size_t i = 0; i < fADCs.size(); i++)
      fADCs.at(i) += input->fADCs.at(i);
    for (size_t i = 0; i < fSCAs.size(); i++)
      fSCAs.at(i) += input->fSCAs.at(i);

    fFrontSCA += input->fFrontSCA;
    fBackSCA += input->fBackSCA;
    fCoincidenceSCA += input->fCoincidenceSCA;

  } else {
    QwError << "QwScanner::operator+=:  Problems!!!"
            << QwLog::endl;
  }
  return *this;
}


VQwSubsystem& QwScanner::operator-=(VQwSubsystem* value)
{
  if (Compare(value)) {
    QwScanner* input = dynamic_cast<QwScanner*>(value);
    // operator-= not defined for tracking-type data element QwPMT_Channel
    //for (size_t i = 0; i < fPMTs.size(); i++)
    //  for (size_t j = 0; j < fPMTs.at(i).size(); j++)
    //    fPMTs.at(i).at(j) -= input->fPMTs.at(i).at(j);
    for (size_t i = 0; i < fADCs.size(); i++)
      fADCs.at(i) -= input->fADCs.at(i);
    for (size_t i = 0; i < fSCAs.size(); i++)
      fSCAs.at(i) -= input->fSCAs.at(i);

    fFrontSCA -= input->fFrontSCA;
    fBackSCA -= input->fBackSCA;
    fCoincidenceSCA -= input->fCoincidenceSCA;

  } else {
    QwError << "QwScanner::operator-=:  Problems!!!"
            << QwLog::endl;
  }
  return *this;
}


void QwScanner::ClearAllBankRegistrations()
{
  VQwSubsystem::ClearAllBankRegistrations();
  fModuleIndex.clear();
  fModulePtrs.clear();
  fModuleTypes.clear();
  fNumberOfModules = 0;
}

Int_t QwScanner::RegisterROCNumber(const UInt_t roc_id)
{
  VQwSubsystem::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return fCurrentBankIndex;
}

Int_t QwScanner::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex++;
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
}


Int_t QwScanner::RegisterSlotNumber(UInt_t slot_id)
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
          fModulePtrs.at(fNumberOfModules).resize(kMaxNumberOfChannelsPerModule,
                                                  tmppair);
          fNumberOfModules = fModulePtrs.size();
          fModuleIndex.at(fCurrentBankIndex).at(slot_id) = fNumberOfModules-1;
          fCurrentSlot     = slot_id;
          fCurrentIndex = fNumberOfModules-1;
        }
    }
  else
    {
      std::cerr << "QwScanner::RegisterSlotNumber:  Slot number "
      << slot_id << " is larger than the number of slots per ROC, "
      << kMaxNumberOfModulesPerROC << std::endl;
    }
  return fCurrentIndex;
}

EQwModuleType QwScanner::RegisterModuleType(TString moduletype)
{
  moduletype.ToUpper();

  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792")
    {
      fCurrentType = kV792_ADC;
      fModuleTypes.at(fCurrentIndex) = fCurrentType;
      if ( (Int_t) fPMTs.size() <= fCurrentType)
        {
          fPMTs.resize(fCurrentType+1);
        }
    }

  else if (moduletype=="V775")
    {
      fCurrentType = kV775_TDC;
      fModuleTypes.at(fCurrentIndex) = fCurrentType;
      if ( (Int_t) fPMTs.size() <= fCurrentType)
        {
          fPMTs.resize(fCurrentType+1);
        }
    }

  else if (moduletype=="F1TDC")
    {
      fCurrentType = kF1TDC;
      fModuleTypes.at(fCurrentIndex) = fCurrentType;
      if ( (Int_t) fPMTs.size() <= fCurrentType)
        {
          fPMTs.resize(fCurrentType+1);
        }
    }

  else if (moduletype=="SIS3801")
    {

    }

  return fCurrentType;
}


Int_t QwScanner::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
  Bool_t local_debug = false;
  if (fCurrentType == kV775_TDC || fCurrentType == kV792_ADC || fCurrentType == kF1TDC)
    {
      fPMTs.at(fCurrentType).push_back(QwPMT_Channel(name));
      fModulePtrs.at(fCurrentIndex).at(chan).first  = fCurrentType;
      fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(fCurrentType).size() -1;
    }
  else if (fCurrentType == kSIS3801)
    {
      if(local_debug) std::cout<<"scaler module has not been implemented yet."<<std::endl;
    }
  if (local_debug) 
    std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
  return 0;
}

void QwScanner::FillRawWord(Int_t bank_index,
                            Int_t slot_num,
                            Int_t chan, UInt_t data)
{
  Int_t modindex = GetModuleIndex(bank_index,slot_num);

  // std::cout<<"modtype="<< fModulePtrs.at(modindex).at(chan).first
  // <<"  chanindex="<<fModulePtrs.at(modindex).at(chan).second<<"  data="<<data<<"\n";

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


Int_t QwScanner::GetModuleIndex(size_t bank_index, size_t slot_num) const
  {
    Int_t modindex = -1;
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


Int_t QwScanner::FindSignalIndex(const EQwModuleType modtype, const TString &name) const
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


void QwScanner::SetPedestal(Double_t pedestal)
{
  for (size_t i=0; i<fADCs.size(); i++)
    fADCs.at(i).SetPedestal(pedestal);
}

void QwScanner::SetCalibrationFactor(Double_t calib)
{
  for (size_t i=0; i<fADCs.size(); i++)
    fADCs.at(i).SetCalibrationFactor(calib);
}

/********************************************************/
void  QwScanner::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);

  for (size_t i=0; i<fADCs.size(); i++)
    fADCs.at(i).InitializeChannel(name,datatosave);
}


void QwScanner::PrintInfo() const
{
  std::cout << "QwScanner: " << fSystemName << std::endl;

  for (size_t i=0; i<fADCs.size(); i++)
    fADCs.at(i).PrintInfo();

  for (size_t i=0; i<fPMTs.size(); i++)
    for (size_t j=0; j<fPMTs.at(i).size(); j++)
      fPMTs.at(i).at(j).PrintInfo();
}

// //scanner analysis utilities
// Double_t QwScanner::get_value( TH2* h, Double_t x, Double_t y, Int_t& checkvalidity)
// {
//   if (checkvalidity)
//     {
//       bool x_ok = ( h->GetXaxis()->GetXmin() < x && x < h->GetXaxis()->GetXmax() );
//       bool y_ok = ( h->GetYaxis()->GetXmin() < y && y < h->GetYaxis()->GetXmax() );
//
//       if (! ( x_ok && y_ok))
//         {
// //             if (!x_ok){
// //                 std::cerr << "x value " << x << " out of range ["<< h->GetXaxis()->GetXmin()
// //                           <<","<< h->GetXaxis()->GetXmax() << "]" << std::endl;
// //             }
// //             if (!y_ok){
// //                 std::cerr << "y value " << y << " out of range ["<< h->GetYaxis()->GetXmin()
// //                           <<","<< h->GetYaxis()->GetXmax() << "]" << std::endl;
// //             }
//           checkvalidity=0;
//           return -1e20;
//         }
//     }
//
//   const int xbin = h->GetXaxis()->FindBin( x );
//   const int ybin = h->GetYaxis()->FindBin( y );
//
//   return h->GetBinContent( h->GetBin( xbin, ybin ));
// }




void QwScanner::FillHardwareErrorSummary()
{
  fF1TDContainer->PrintErrorSummary();
  fF1TDContainer->WriteErrorSummary();
  return;
};
