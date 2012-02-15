/**********************************************************\
* File: QwRaster.cc                                       *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Tue Jun 29 00:16:14 CDT 2010                             *
\**********************************************************/

#include "QwRaster.h"

// ROOT headers
#include "TStyle.h"

// Qweak headers
#include "QwHistogramHelper.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwRaster);

const UInt_t QwRaster::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwRaster::kMaxNumberOfChannelsPerModule = 32;

QwRaster::QwRaster(const TString& name)
: VQwSubsystem(name),
  VQwSubsystemTracking(name)
{
    ClearAllBankRegistrations();
}


QwRaster::~QwRaster()
{
  fPMTs.clear();
  fSCAs.clear();
}


void QwRaster::ProcessOptions(QwOptions &options)
{
    //Handle command line options
}

Int_t QwRaster::LoadChannelMap(TString mapfile)
{
    TString varname, varvalue;
    TString modtype, dettype, name;
    Int_t modnum = 0, channum = 0;

    QwParameterFile mapstr(mapfile.Data());  //Open the file
    fDetectorMaps.insert(mapstr.GetParamFileNameContents());

    while (mapstr.ReadNextLine())
    {
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
          } else if (varname=="slot") {
              RegisterSlotNumber(value);
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

            //  Push a new record into the element array
            if (modtype=="SIS3801") {
              QwSIS3801D24_Channel localchannel(name);
              localchannel.SetNeedsExternalClock(kFALSE);
              fSCAs.push_back(localchannel);
              fSCAs_map[name] = fSCAs.size()-1;
              Int_t offset = QwSIS3801D24_Channel::GetBufferOffset(modnum,channum);
              fSCAs_offset.push_back(offset);
            } else if (modtype=="V792" || modtype=="V775") {
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
  //ReportConfiguration();
  return 0;
}

Int_t QwRaster::LoadInputParameters(TString parameterfile)
{
    Bool_t ldebug=kFALSE;
    TString varname, varvalue;
    Double_t varped;
    Double_t varcal;
    TString localname;

    Int_t lineread=0;

    QwParameterFile mapstr(parameterfile.Data());  //Open the file
    fDetectorMaps.insert(mapstr.GetParamFileNameContents());

    if (ldebug) std::cout<<"\nReading raster parameter file: "<<parameterfile<<"\n";

    while (mapstr.ReadNextLine())
    {
        lineread+=1;
        mapstr.TrimComment('!');   // Remove everything after a '!' character.
        mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
        if (mapstr.LineIsEmpty())  continue;

        if (mapstr.HasVariablePair("=",varname,varvalue))
        {
            varname.ToLower();
            Double_t value = atof(varvalue.Data());
            if (varname=="position_offset_x")
            {
                fPositionOffsetX = value;
            }
            else if (varname=="position_offset_y")
            {
                fPositionOffsetY = value;
            }
            else if (varname=="cal_factor_qdc_x")
            {
                fCal_Factor_QDC_X = value;
            }
            else if (varname=="cal_factor_qdc_y")
            {
                fCal_Factor_QDC_Y = value;
            }
            else if (varname=="channel_offset_x")
            {
                fChannel_Offset_X = value;
            }
            else if (varname=="channel_offset_y")
            {
                fChannel_Offset_Y = value;
            }
            else if (varname=="bpm_3h07a_slope")
            {
                fbpm_3h07a_slope = value;
            }
            else if (varname=="bpm_3h07a_intercept")
            {
                fbpm_3h07a_intercept = value;
            }
            else if (varname=="bpm_3h09b_slope")
            {
                fbpm_3h09b_slope = value;
            }
            else if (varname=="bpm_3h09b_intercept")
            {
                fbpm_3h09b_intercept = value;
            }

            if (ldebug) std::cout<<"inputs for "<<varname<<": "<<value<<"\n";
        }

        else
        {
            varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
            varname.ToLower();
            varname.Remove(TString::kBoth,' ');
            varped= mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
            varcal= mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
            if (ldebug) std::cout<<"inputs for channel "<<varname
                <<": ped="<<varped<<", cal="<<varcal<<"\n";
        }
    }
    if (ldebug) std::cout<<" line read in the parameter file ="<<lineread<<" \n";

    ldebug=kFALSE;
    mapstr.Close(); // Close the file (ifstream)
    return 0;
}


void  QwRaster::ClearEventData()
{
    SetDataLoaded(kFALSE);

    for (size_t i=0; i<fPMTs.size(); i++)
    {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
            fPMTs.at(i).at(j).SetValue(0);
        }
    }

    /*for (size_t i=0; i<fADC_Data.size(); i++)
    {
        if (fADC_Data.at(i) != NULL)
        {
            fADC_Data.at(i)->ClearEventData();
        }
    }*/

    for (size_t i=0; i<fSCAs.size(); i++) {
      fSCAs.at(i).ClearEventData();
    }

}


Int_t QwRaster::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
    return 0;
}


Int_t QwRaster::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
    Int_t index = 0;

    index = GetSubbankIndex(roc_id,bank_id);

    //  This is a QADC/TDC bank
    if (bank_id==fBankID[0])
    {
        if (index>=0 && num_words>0)
        {
            //  We want to process this ROC.  Begin looping through the data.
            SetDataLoaded(kTRUE);
            for (size_t i=0; i<num_words ; i++)
            {
                //  Decode this word as a V775TDC word.
                fQDCTDC.DecodeTDCWord(buffer[i]);

                if (! IsSlotRegistered(index, fQDCTDC.GetTDCSlotNumber())) continue;

                if (fQDCTDC.IsValidDataword())
                {
                    // This is a V775 TDC data word
                    try
                    {
                        FillRawWord(index,fQDCTDC.GetTDCSlotNumber(),fQDCTDC.GetTDCChannelNumber(),
                                          fQDCTDC.GetTDCData());
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "Standard exception from Raster::FillRawTDCWord: "
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

    return 0;
}


void  QwRaster::ProcessEvent()
{
    if (! HasDataLoaded()) return;

    for (size_t i=0; i<fPMTs.size(); i++) {
      for (size_t j=0; j<fPMTs.at(i).size(); j++) {
        fPMTs.at(i).at(j).ProcessEvent();
      }
    }

    /*for (size_t i=0; i<fADC_Data.size(); i++)
    {
        if (fADC_Data.at(i) != NULL)
        {
            fADC_Data.at(i)->ProcessEvent();
        }
    }*/

    for (size_t i=0; i<fSCAs.size(); i++) {
      fSCAs.at(i).ProcessEvent();
    }

    //Fill trigger data
    for (size_t i=0; i<fPMTs.size(); i++)
    {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {

            TString element_name = fPMTs.at(i).at(j).GetElementName();
            if (element_name==TString("raster_posx_adc"))
            {
                fPositionX_ADC = fPMTs.at(i).at(j).GetValue();
                fPositionX_ADC = (fPositionX_ADC-fChannel_Offset_X)*fCal_Factor_QDC_X + fPositionOffsetX;
            }
            else if (element_name==TString("raster_posy_adc"))
            {
                fPositionY_ADC = fPMTs.at(i).at(j).GetValue();
                fPositionY_ADC = (fPositionY_ADC-fChannel_Offset_Y)*fCal_Factor_QDC_Y + fPositionOffsetY;
            }

            else if (element_name==TString("bpm_3h07a_xp_adc"))
            {
                fbpm_3h07a_xp = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h07a_xm_adc"))
            {
                fbpm_3h07a_xm = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h07a_yp_adc"))
            {
                fbpm_3h07a_yp = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h07a_ym_adc"))
            {
                fbpm_3h07a_ym = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h09b_xp_adc"))
            {
                fbpm_3h09b_xp = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h09b_xm_adc"))
            {
                fbpm_3h09b_xm = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h09b_yp_adc"))
            {
                fbpm_3h09b_yp = fPMTs.at(i).at(j).GetValue();
            }
            else if (element_name==TString("bpm_3h09b_ym_adc"))
            {
                fbpm_3h09b_ym = fPMTs.at(i).at(j).GetValue();
            }
        }
     }
     
     fbpm_3h07a_pos_x = fbpm_3h07a_slope*(fbpm_3h07a_xp-fbpm_3h07a_xm)/
                        (fbpm_3h07a_xp+fbpm_3h07a_xm)+fbpm_3h07a_intercept;
     fbpm_3h07a_pos_y = fbpm_3h07a_slope*(fbpm_3h07a_yp-fbpm_3h07a_ym)/
                        (fbpm_3h07a_yp+fbpm_3h07a_ym)+fbpm_3h07a_intercept;
     fbpm_3h09b_pos_x = fbpm_3h09b_slope*(fbpm_3h09b_xp-fbpm_3h09b_xm)/
                        (fbpm_3h09b_xp+fbpm_3h09b_xm)+fbpm_3h09b_intercept;
     fbpm_3h09b_pos_y = fbpm_3h09b_slope*(fbpm_3h09b_yp-fbpm_3h09b_ym)/
                        (fbpm_3h09b_yp+fbpm_3h09b_ym)+fbpm_3h09b_intercept;

}


void  QwRaster::ConstructHistograms(TDirectory *folder, TString &prefix)
{
    TString basename;

    if (prefix == "")  basename = "";
    else  basename = prefix;

    if (folder != NULL) folder->cd();

    for (size_t i=0; i<fPMTs.size(); i++)
    {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
            fPMTs.at(i).at(j).ConstructHistograms(folder, basename);
    }
    
    for (size_t i=0; i<fSCAs.size(); i++) {
      fSCAs.at(i).ConstructHistograms(folder, prefix);
    }

    fHistograms.push_back( gQwHists.Construct1DHist(TString("raster_position_x")));
    fHistograms.push_back( gQwHists.Construct1DHist(TString("raster_position_y")));
  
    fRateMap  = new TH2D("raster_rate_map","Raster Rate Map",125,0,0,125,0,0);
    
    fRateMap->GetXaxis()->SetTitle(" X [mm]");
    fRateMap->GetYaxis()->SetTitle(" Y [mm]");

    gStyle     -> SetPalette(1);
    fRateMap->SetOption("colz");

    fHistograms.push_back( gQwHists.Construct1DHist(TString("bpm_3h07a_pos_x")));
    fHistograms.push_back( gQwHists.Construct1DHist(TString("bpm_3h07a_pos_y")));
    fHistograms.push_back( gQwHists.Construct1DHist(TString("bpm_3h09b_pos_x")));
    fHistograms.push_back( gQwHists.Construct1DHist(TString("bpm_3h09b_pos_y")));

}

void  QwRaster::FillHistograms()
{

    if (! HasDataLoaded()) return;

    for (size_t i=0; i<fPMTs.size(); i++)
    {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
            fPMTs.at(i).at(j).FillHistograms();
        }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
      fSCAs.at(i).FillHistograms();
   }
   
    // FR fudge factor is 3.2, by Dave Mack
    // See hclog ttps://hallcweb.jlab.org/hclog/1010_archive/101012134143.html
    // I inverse x axis and multiply the FR fudge factor to x and y values
    // in order to see the unit is mm in the FR plot
    // Tuesday, October 12 17:11:52 EDT 2010, jhlee

    Double_t fudge_factor = 3.2;
    
    Double_t raster_x_mm = 0.0;
    Double_t raster_y_mm = 0.0;

    raster_x_mm = - fudge_factor*fPositionX_ADC;
    raster_y_mm =   fudge_factor*fPositionY_ADC;

    for (size_t j=0; j<fHistograms.size();j++)
    {
      if (fHistograms.at(j)->GetTitle()==TString("raster_position_x"))
        {
	  fHistograms.at(j)->Fill(raster_x_mm);
        }
      if (fHistograms.at(j)->GetTitle()==TString("raster_position_y"))
        {
	  fHistograms.at(j)->Fill(raster_y_mm);
        }
      if (fHistograms.at(j)->GetTitle()==TString("bpm_3h07a_pos_x"))
        {
	  fHistograms.at(j)->Fill(fbpm_3h07a_pos_x);
        }
      if (fHistograms.at(j)->GetTitle()==TString("bpm_3h07a_pos_y"))
        {
	  fHistograms.at(j)->Fill(fbpm_3h07a_pos_y);
        }
      if (fHistograms.at(j)->GetTitle()==TString("bpm_3h09b_pos_x"))
        {
	  fHistograms.at(j)->Fill(fbpm_3h09b_pos_x);
        }
      if (fHistograms.at(j)->GetTitle()==TString("bpm_3h09b_pos_y"))
        {
	  fHistograms.at(j)->Fill(fbpm_3h09b_pos_y);
        }
    }
    
    fRateMap->Fill(raster_x_mm, raster_y_mm);

}


void  QwRaster::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
    fTreeArrayIndex = values.size();

    TString basename;
    if (prefix=="") basename = "beamline";
    else basename = prefix;

    values.push_back(0.0);
    TString list = "PositionX_ADC/D";
    values.push_back(0.0);
    list += ":PositionY_ADC/D";
    list += ":bpm_3h07a_pos_x/D";
    list += ":bpm_3h07a_pos_y/D";
    list += ":bpm_3h09b_pos_x/D";
    list += ":bpm_3h09b_pos_y/D";

    for (size_t i=0; i<fPMTs.size(); i++) {
      for (size_t j=0; j<fPMTs.at(i).size(); j++) {
        TString element_name = fPMTs.at(i).at(j).GetElementName();
        if (element_name=="") {
          //  This channel is not used, so skip setting up the tree.
        } else {
            values.push_back(0.0);
            list += ":"+element_name+"_raw/D";
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


void  QwRaster::FillTreeVector(std::vector<Double_t> &values) const
{
    if (! HasDataLoaded()) return;

    Int_t index = fTreeArrayIndex;
    values[index++] = fPositionX_ADC;
    values[index++] = fPositionY_ADC;
    values[index++] = fbpm_3h07a_pos_x;
    values[index++] = fbpm_3h07a_pos_y;
    values[index++] = fbpm_3h09b_pos_x;
    values[index++] = fbpm_3h09b_pos_y;

    //fill trigvalues
    for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
      {
        if (fPMTs.at(i).at(j).GetElementName()=="") {}
        else
        {
          values[index++] = fPMTs.at(i).at(j).GetValue();
        }
      }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
      if (fSCAs.at(i).GetElementName()!="") {
        values[index] = fSCAs.at(i).GetValue();
        index++;
      }
    }
}


void  QwRaster::ReportConfiguration()
{
    /*std::cout << "Configuration of raster:"<< std::endl;
    for (size_t i = 0; i<fROC_IDs.size(); i++)
    {
        for (size_t j=0; j<fBank_IDs.at(i).size(); j++)
        {

            Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
            std::cout << "ROC " << fROC_IDs.at(i)
            << ", subbank 0x" << std::hex<<fBank_IDs.at(i).at(j)<<std::dec
            << ":  subbank index==" << ind << std::endl;

                for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++)
                {
                    Int_t QadcTdcindex = GetModuleIndex(ind,k);
                    if (QadcTdcindex != -1)
                    {
                        std::cout << "    Slot "<<k<<"  Module#"<<QadcTdcindex<< std::endl;
                    }
                }
        }
    }*/

}
//ReportConfiguration()

void QwRaster::ClearAllBankRegistrations()
{
    VQwSubsystem::ClearAllBankRegistrations();
    fModuleIndex.clear();
    fModulePtrs.clear();
    fModuleTypes.clear();
    fNumberOfModules = 0;
}

Int_t QwRaster::RegisterROCNumber(const UInt_t roc_id)
{
    VQwSubsystem::RegisterROCNumber(roc_id, 0);
    fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
    std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
    fModuleIndex.push_back(tmpvec);
    return fCurrentBankIndex;
}

Int_t QwRaster::RegisterSubbank(const UInt_t bank_id)
{
    Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
    fCurrentBankIndex++;
    std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
    fModuleIndex.push_back(tmpvec);
    //std::cout<<"RASTER Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
    return stat;
}


Int_t QwRaster::RegisterSlotNumber(UInt_t slot_id)
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
        std::cerr << "QwRaster::RegisterSlotNumber:  Slot number "
        << slot_id << " is larger than the number of slots per ROC, "
        << kMaxNumberOfModulesPerROC << std::endl;
    }
    return fCurrentIndex;
}

EQwModuleType QwRaster::RegisterModuleType(TString moduletype)
{
    moduletype.ToUpper();

    //  Check to see if we've already registered a type for the current slot,
    //  if so, throw an error...

    if (moduletype=="V792"){
      fCurrentType = kV792_ADC;
    } else if (moduletype=="SIS3801") {
      fCurrentType = kSIS3801;
    }
    fModuleTypes.at(fCurrentIndex) = fCurrentType;
    if ((Int_t) fPMTs.size() <= fCurrentType){
      fPMTs.resize(fCurrentType+1);
    }
    return fCurrentType;
}


Int_t QwRaster::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
    fPMTs.at(fCurrentType).push_back(QwPMT_Channel(name));
    fModulePtrs.at(fCurrentIndex).at(chan).first  = fCurrentType;
    fModulePtrs.at(fCurrentIndex).at(chan).second =
    fPMTs.at(fCurrentType).size() -1;
    return 0;
}

void QwRaster::FillRawWord(Int_t bank_index,
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
        }
    }
}


Int_t QwRaster::GetModuleIndex(size_t bank_index, size_t slot_num) const
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


Int_t QwRaster::FindSignalIndex(const EQwModuleType modtype, const TString &name) const
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

void QwRaster::PrintInfo()
{
    std::cout << "QwRaster: " << fSystemName << std::endl;

    for (size_t i=0; i<fPMTs.size(); i++)
    {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
            fPMTs.at(i).at(j).PrintInfo();
        }
    }

    return;
}
