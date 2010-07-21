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
QwSubsystemFactory<QwMainDetector> theMainDetectorFactory("QwMainDetector");

QwMainDetector::QwMainDetector(TString region_tmp):VQwSubsystem(region_tmp),
    VQwSubsystemTracking(region_tmp)
{
  fDEBUG = false;
  ClearAllBankRegistrations();
};

QwMainDetector::~QwMainDetector()
{
  DeleteHistograms();
  fPMTs.clear();
  fSCAs.clear();
};


Int_t QwMainDetector::LoadGeometryDefinition ( TString mapfile )
{
  std::cout<<"Main Detector Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires, detectorId, region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo temp_Detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize ( kNumPackages );
  //  Int_t pkg,pln;

  DIRMODE=0;

  QwParameterFile mapstr ( mapfile.Data() );  //Open the file

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
              temp_Detector.SetDetectorInfo ( dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId );


              if ( package == "u" )
                fDetectorInfo.at ( kPackageUp ).push_back ( temp_Detector );
              else if ( package == "d" )
                fDetectorInfo.at ( kPackageDown ).push_back ( temp_Detector );
            }
        }
    }

  std::cout<<"Loaded Qweak Geometry"<<" Total Detectors in pkg_d 1 "<<fDetectorInfo.at ( kPackageUp ).size() << " pkg_d 2 "<<fDetectorInfo.at ( kPackageDown ).size() <<std::endl;

  std::cout << "Sorting detector info..." << std::endl;
  plane = 1;
  std::sort ( fDetectorInfo.at ( kPackageUp ).begin(),
              fDetectorInfo.at ( kPackageUp ).end() );

  UInt_t i = 0;
  for ( i = 0; i < fDetectorInfo.at ( kPackageUp ).size(); i++ )
    {
      fDetectorInfo.at ( kPackageUp ).at ( i ).fPlane = plane++;
      std::cout<<" Region "<<fDetectorInfo.at ( kPackageUp ).at ( i ).fRegion<<" Detector ID "<<fDetectorInfo.at ( kPackageUp ).at ( i ).fDetectorID << std::endl;
    }

  plane = 1;
  std::sort ( fDetectorInfo.at ( kPackageDown ).begin(),
              fDetectorInfo.at ( kPackageDown ).end() );
  for ( i = 0; i < fDetectorInfo.at ( kPackageDown ).size(); i++ )
    {
      fDetectorInfo.at ( kPackageDown ).at ( i ).fPlane = plane++;
      std::cout<<" Region "<<fDetectorInfo.at ( kPackageDown ).at ( i ).fRegion<<" Detector ID " << fDetectorInfo.at ( kPackageDown ).at ( i ).fDetectorID << std::endl;
    }

  std::cout<<"Qweak Geometry Loaded "<<std::endl;

  return 0;
}


Int_t QwMainDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum=0, channum=0, slotnum=0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
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
              if (fModulePtrs.at(fCurrentIndex).at(channum).first>=0)
                {
                  //  We've seen this channel
                }
              else if (FindSignalIndex(fCurrentType, name)>=0)
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
              std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
              << std::endl;
            }
        }
      if (name=="MD_reftime_f1") {
            reftime_slotnum = slotnum;
            reftime_channum = channum;
      }
    }
  //ReportConfiguration();
  return 0;
};


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
};

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
  return 0;
};



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
      if (index>=0 && num_words>0)
        {
	  SetDataLoaded(kTRUE);
          if (fDEBUG) std::cout << "QwScanner::ProcessEvBuffer:  "
            << "Begin processing F1TDC Bank "<<bank_id<< std::endl;

          Int_t tdc_slot_number = 0;
          Int_t tdc_chan_number = 0;
          Int_t tmp_last_chan = 65535;

	  Bool_t data_integrity_flag = false;
	  Bool_t temp_print_flag     = false;

	  data_integrity_flag = fF1TDC.CheckDataIntegrity(roc_id, buffer, num_words);

	  if (data_integrity_flag)
	    {//;
	      for (UInt_t i=0; i<num_words ; i++)
		{

		  fF1TDC.DecodeTDCWord(buffer[i], roc_id);

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
		  if (! IsSlotRegistered(index, tdc_slot_number) ) continue;

		  if ( fF1TDC.IsValidDataword() )
		    {
		      try {
		    	if(tdc_chan_number != tmp_last_chan)
		    	{
			    FillRawWord(index, tdc_slot_number, tdc_chan_number, fF1TDC.GetTDCData());

			    fF1TDC.PrintTDCData(temp_print_flag);
			    if (tdc_slot_number == reftime_slotnum && tdc_chan_number == reftime_channum)
			      reftime = fF1TDC.GetTDCData();
			    tmp_last_chan = tdc_chan_number;

		    	}
		      }
		      catch (std::exception& e) {
			std::cerr << "Standard exception from QwMainDetector::FillRawTDCWord: "
				  << e.what() << std::endl;
			std::cerr << "   Parameters:  index=="  << index
				  << "; GetF1SlotNumber()=="    << tdc_slot_number
				  << "; GetF1ChannelNumber()==" << tdc_chan_number
				  << "; GetF1Data()=="          << fF1TDC.GetTDCData()
				  << std::endl;
		      }
		    }
		   }
	    }//; if(data_integrity_flag)
      }
    }


  // This is a SCA bank
  else if (bank_id==fBankID[1])
    {

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
};


void  QwMainDetector::ProcessEvent()
{
  if (! HasDataLoaded()) return;

  TString elementname = "";
  Double_t rawtime = 0.0;


  for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
          fPMTs.at(i).at(j).ProcessEvent();
          elementname = fPMTs.at(i).at(j).GetElementName();
          rawtime = fPMTs.at(i).at(j).GetValue();
          if (elementname.EndsWith("f1") && rawtime!=0) {
                  Double_t newdata = fF1TDC.ActualTimeDifference(rawtime, reftime);
                  fPMTs.at(i).at(j).SetValue(newdata);
          }
      }
  }

  for (size_t i=0; i<fSCAs.size(); i++){
      if (fSCAs.at(i) != NULL){
          fSCAs.at(i)->ProcessEvent();
      }
    }
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
      if (fSCAs.at(i) != NULL)
        {
          fSCAs.at(i)->ConstructHistograms(folder, prefix);
        }
    }

};

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
};


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
};


void  QwMainDetector::FillTreeVector(std::vector<Double_t> &values)
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

};


void  QwMainDetector::DeleteHistograms()
{
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).DeleteHistograms();
        }
    }

  for (size_t i=0; i<fSCAs.size(); i++)
    {
      if (fSCAs.at(i) != NULL)
        {
          fSCAs.at(i)->DeleteHistograms();
        }
    }
};


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
};




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
};

Int_t QwMainDetector::RegisterSubbank(const UInt_t bank_id)
{
  Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
  fCurrentBankIndex++;
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
  return stat;
};


Int_t QwMainDetector::RegisterSlotNumber(UInt_t slot_id)
{
  std::pair<Int_t, Int_t> tmppair;
  tmppair.first  = -1;
  tmppair.second = -1;
  if (slot_id<kMaxNumberOfModulesPerROC)
    {
      if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fModuleIndex.size())
        {
          fModuleTypes.resize(fNumberOfModules+1);
          fModulePtrs.resize(fNumberOfModules+1);
          fModulePtrs.at(fNumberOfModules).resize(fF1TDC.GetTDCMaxChannels(),
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
};

const QwMainDetector::EModuleType QwMainDetector::RegisterModuleType(TString moduletype)
{
  moduletype.ToUpper();

  std::cout<<"moduletype="<<moduletype<<"\n";
  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792")
    {
      fCurrentType = V792_ADC;
    }
  else if (moduletype=="V775")
    {
      fCurrentType = V775_TDC;
    }
  fModuleTypes.at(fCurrentIndex) = fCurrentType;
  if ((Int_t)fPMTs.size()<=fCurrentType)
    {
      fPMTs.resize(fCurrentType+1);
    }
  return fCurrentType;
};


Int_t QwMainDetector::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
  size_t index = fCurrentType;
  if (index == 0 || index == 1)
    {
      fPMTs.at(index).push_back(QwPMT_Channel(name));
      fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
      fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(index).size() -1;
    }
  else if (index ==2)
    {
      std::cout<<"scaler module has not been implemented yet."<<std::endl;
    }
  std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
  return 0;
};

void QwMainDetector::FillRawWord(Int_t bank_index,
                                 Int_t slot_num,
                                 Int_t chan, UInt_t data)
{
  Int_t modindex = GetModuleIndex(bank_index,slot_num);

  if (modindex != -1)
    {
      EModuleType modtype = EModuleType(fModulePtrs.at(modindex).at(chan).first);
      Int_t chanindex     = fModulePtrs.at(modindex).at(chan).second;

      if (modtype == EMPTY || chanindex == -1)
        {
          //  This channel is not connected to anything.
          //  Do nothing.
        }
      else
        {
    	  fPMTs.at(modtype).at(chanindex).SetValue(data);
        }
    };
};


Int_t QwMainDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const
  {
    Int_t modindex = -1;
    //std::cout<<"bank_index="<<bank_index<<" fModuleIndex.size()="<<fModuleIndex.size()<<"\n";

    if (bank_index>=0 && bank_index<fModuleIndex.size())
      {
        if (slot_num>=0 && slot_num<fModuleIndex.at(bank_index).size())
          {
            modindex = fModuleIndex.at(bank_index).at(slot_num);
          }
      }
    return modindex;
  };


Int_t QwMainDetector::FindSignalIndex(const QwMainDetector::EModuleType modtype, const TString &name) const
  {
    size_t index = modtype;
    Int_t chanindex = -1;
    for (size_t chan=0; chan<fPMTs.at(index).size(); chan++)
      {
        if (name == fPMTs.at(index).at(chan).GetElementName())
          {
            chanindex = chan;
            break;
          }
      }
    return chanindex;
  };

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
; //ReportConfiguration()
