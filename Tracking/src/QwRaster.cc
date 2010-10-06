/**********************************************************\
* File: QwRaster.cc                                       *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Tue Jun 29 00:16:14 CDT 2010                             *
\**********************************************************/

#include "QwRaster.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwRaster> theRasterFactory("QwRaster");

extern QwHistogramHelper gQwHists;
const Bool_t QwRaster::bStoreRawData = kTRUE;
const UInt_t QwRaster::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwRaster::kMaxNumberOfChannelsPerModule = 32;

QwRaster::QwRaster(TString region_tmp)
        :VQwSubsystem(region_tmp),
        VQwSubsystemTracking(region_tmp)
{

    fDEBUG = 0;
    ClearAllBankRegistrations();
};


QwRaster::~QwRaster()
{

    fPMTs.clear();
    //DeleteHistograms();
};


void QwRaster::ProcessOptions(QwOptions &options)
{
    //Handle command line options
};

Int_t QwRaster::LoadChannelMap(TString mapfile)
{
    TString varname, varvalue;
    TString modtype, dettype, name;
    Int_t modnum, channum;

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
            else if (varname=="bank")
            {
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
            modtype   = mapstr.GetNextToken(", ").c_str();
            modnum    = (atol(mapstr.GetNextToken(", ").c_str()));
            channum   = (atol(mapstr.GetNextToken(", ").c_str()));
            dettype   = mapstr.GetNextToken(", ").c_str();
            name      = mapstr.GetNextToken(", ").c_str();

            //  Push a new record into the element array

            if (modtype=="V792")
            {
                RegisterModuleType(modtype);
                fBankID[0] = fCurrentBank_ID;

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
                    LinkChannelToSignal(channum, name);
                }
            }

            else
            {
                std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
                << std::endl;
            }
        }
    }
    //
    ReportConfiguration();
    return 0;
};

Int_t QwRaster::LoadInputParameters(TString parameterfile)
{
    Bool_t ldebug=kTRUE;
    TString varname, varvalue;
    Double_t varped;
    Double_t varcal;
    TString localname;

    Int_t lineread=0;

    QwParameterFile mapstr(parameterfile.Data());  //Open the file
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
            varname = mapstr.GetNextToken(", \t").c_str();	//name of the channel
            varname.ToLower();
            varname.Remove(TString::kBoth,' ');
            varped= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
            varcal= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
            if (ldebug) std::cout<<"inputs for channel "<<varname
                <<": ped="<<varped<<", cal="<<varcal<<"\n";
        }
    }
    if (ldebug) std::cout<<" line read in the parameter file ="<<lineread<<" \n";

    ldebug=kFALSE;
    return 0;
};


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

    for (size_t i=0; i<fADC_Data.size(); i++)
    {
        if (fADC_Data.at(i) != NULL)
        {
            fADC_Data.at(i)->ClearEventData();
        }
    }

};


Int_t QwRaster::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
    Int_t index = GetSubbankIndex(roc_id,bank_id);
    if (index>=0 && num_words>0)
    {
        //  We want to process the configuration data for this ROC.
        UInt_t words_read = 0;

        if (fBankID[1]==bank_id)
        {
            for (size_t i=0; i<fADC_Data.size(); i++)
            {
                if (fADC_Data.at(i) != NULL)
                {
                    words_read += fADC_Data.at(i)->ProcessConfigBuffer(&(buffer[words_read]),
                                  num_words-words_read);
                }
            }
        }

    }
    return 0;
};


Int_t QwRaster::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
    Int_t index = GetSubbankIndex(roc_id,bank_id);

    SetDataLoaded(kTRUE);
    if (fDEBUG) std::cout << "Raster::ProcessEvBuffer:  "
        << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
        <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
        << ", num_words "<<num_words<<", index "<<index<<std::endl;

    //  This is a QADC/TDC bank
    if (bank_id==fBankID[0])
    {
        if (index>=0 && num_words>0)
        {
            //  We want to process this ROC.  Begin looping through the data.
            if (fDEBUG) std::cout << "Raster::ProcessEvBuffer:  "
                << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
                <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"<< std::endl;

            if (fDEBUG)
                std::cout<<"QwRaster::ProcessEvBuffer (trig) Data: \n";

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
                        //TODO The slot number should be set properly in DAQ
                        // using 0 for now
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

    return 0;
};


void  QwRaster::ProcessEvent()
{
    if (! HasDataLoaded()) return;

    for (size_t i=0; i<fPMTs.size(); i++)
    {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
            fPMTs.at(i).at(j).ProcessEvent();
        }
    }

    for (size_t i=0; i<fADC_Data.size(); i++)
    {
        if (fADC_Data.at(i) != NULL)
        {
            fADC_Data.at(i)->ProcessEvent();
        }
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
};


void  QwRaster::ConstructHistograms(TDirectory *folder, TString &prefix)
{

    TString basename;

    if (prefix = "")  basename = "";
    else  basename = prefix;

    if (folder != NULL) folder->cd();

    if (bStoreRawData)
    {
        for (size_t i=0; i<fPMTs.size(); i++)
        {
            for (size_t j=0; j<fPMTs.at(i).size(); j++)
                fPMTs.at(i).at(j).ConstructHistograms(folder, basename);
        }
    }

    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("raster_position_x")));
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("raster_position_y")));

    fRateMap  = new TH2D("raster_rate_map","Raster Rate Map",500,-2.5,2.5,500,-2.5,2.5);
    fRateMap->GetXaxis()->SetTitle("PositionX");
    fRateMap->GetYaxis()->SetTitle("PositionY");
    fRateMap->SetOption("colz");

    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("bpm_3h07a_pos_x")));
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("bpm_3h07a_pos_y")));
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("bpm_3h09b_pos_x")));
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("bpm_3h09b_pos_y")));

};

void  QwRaster::FillHistograms()
{

    if (! HasDataLoaded()) return;

    if (bStoreRawData)
    {
        //Fill trigger data
        for (size_t i=0; i<fPMTs.size(); i++)
        {
            for (size_t j=0; j<fPMTs.at(i).size(); j++)
            {
                fPMTs.at(i).at(j).FillHistograms();
            }
        }
    }

    for (size_t j=0; j<fHistograms1D.size();j++)
    {
        if (fHistograms1D.at(j)->GetTitle()==TString("raster_position_x"))
        {
            fHistograms1D.at(j)->Fill(fPositionX_ADC);
        }

        if (fHistograms1D.at(j)->GetTitle()==TString("raster_position_y"))
        {
            fHistograms1D.at(j)->Fill(fPositionY_ADC);
        }
        if (fHistograms1D.at(j)->GetTitle()==TString("raster_position_y"))
        {
            fHistograms1D.at(j)->Fill(fPositionY_ADC);
        }
        if (fHistograms1D.at(j)->GetTitle()==TString("bpm_3h07a_pos_x"))
        {
            fHistograms1D.at(j)->Fill(fbpm_3h07a_pos_x);
        }
        if (fHistograms1D.at(j)->GetTitle()==TString("bpm_3h07a_pos_y"))
        {
            fHistograms1D.at(j)->Fill(fbpm_3h07a_pos_y);
        }
        if (fHistograms1D.at(j)->GetTitle()==TString("bpm_3h09b_pos_x"))
        {
            fHistograms1D.at(j)->Fill(fbpm_3h09b_pos_x);
        }
        if (fHistograms1D.at(j)->GetTitle()==TString("bpm_3h09b_pos_y"))
        {
            fHistograms1D.at(j)->Fill(fbpm_3h09b_pos_y);
        }
    }

    fRateMap->Fill(fPositionX_ADC,fPositionY_ADC);
};


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

    if (bStoreRawData)
    {

      for (size_t i=0; i<fPMTs.size(); i++)
      {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          //fPMTs.at(i).at(j).ConstructBranchAndVector(tree, prefix, values);
          if (fPMTs.at(i).at(j).GetElementName()=="")
          {
            //  This channel is not used, so skip setting up the tree.
          }
          else
          {
            values.push_back(0.0);
            list += ":"+fPMTs.at(i).at(j).GetElementName()+"_raw/D";
          }
        }
      }

      fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
      tree->Branch(basename, &values[fTreeArrayIndex], list);
    }
    return;
};


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

    if (bStoreRawData)
    {
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
    }
    return;
};


void  QwRaster::DeleteHistograms()
{
    if (bStoreRawData)
    {
        for (size_t i=0; i<fPMTs.size(); i++)
        {
            for (size_t j=0; j<fPMTs.at(i).size(); j++)
            {
                if (fPMTs.at(i).at(j).GetElementName()=="") {}
                else  fPMTs.at(i).at(j).DeleteHistograms();
            }
        }

    }

    // Delete all histograms in the list
    for (size_t i = 0; i < fHistograms1D.size(); i++) {
        if (fHistograms1D.at(i) != NULL) {
            delete fHistograms1D.at(i);
            fHistograms1D.at(i) = NULL;
        }
    }
    // Then clear the list
    fHistograms1D.clear();

    // Delete all histograms in the list
    for (size_t i = 0; i < fHistograms2D.size(); i++) {
        if (fHistograms2D.at(i) != NULL) {
            delete fHistograms2D.at(i);
            fHistograms2D.at(i) = NULL;
        }
    }
    // Then clear the list
    fHistograms2D.clear();

};

void  QwRaster::ReportConfiguration()
{
    std::cout << "Configuration of raster:"<< std::endl;
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
    }

}
; //ReportConfiguration()

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
};

Int_t QwRaster::RegisterSubbank(const UInt_t bank_id)
{
    Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
    fCurrentBankIndex++;
    std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
    fModuleIndex.push_back(tmpvec);
    //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
    return stat;
};


Int_t QwRaster::RegisterSlotNumber(UInt_t slot_id)
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
};

const QwRaster::EModuleType QwRaster::RegisterModuleType(TString moduletype)
{
    moduletype.ToUpper();

    //  Check to see if we've already registered a type for the current slot,
    //  if so, throw an error...

    if (moduletype=="V792")
    {
        fCurrentType = V792_ADC;
        fModuleTypes.at(fCurrentIndex) = fCurrentType;
        if ((Int_t) fPMTs.size()<=fCurrentType)
        {
            fPMTs.resize(fCurrentType+1);
        }
    }

    return fCurrentType;
};


Int_t QwRaster::LinkChannelToSignal(const UInt_t chan, const TString &name)
{
    size_t index = fCurrentType;
    if (index == 0 || index == 1)
    {
        fPMTs.at(index).push_back(QwPMT_Channel(name));
        fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
        fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(index).size() -1;
    }
    std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
    return 0;
};

void QwRaster::FillRawWord(Int_t bank_index,
                            Int_t slot_num,
                            Int_t chan, UInt_t data)
{
    Int_t modindex = GetModuleIndex(bank_index,slot_num);

    // std::cout<<"modtype="<<EModuleType(fModulePtrs.at(modindex).at(chan).first)
    // <<"  chanindex="<<fModulePtrs.at(modindex).at(chan).second<<"  data="<<data<<"\n";

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


Int_t QwRaster::GetModuleIndex(size_t bank_index, size_t slot_num) const
{
    Int_t modindex = -1;
    if (bank_index>=0 && bank_index<fModuleIndex.size())
    {
        if (slot_num>=0 && slot_num<fModuleIndex.at(bank_index).size())
        {
            modindex = fModuleIndex.at(bank_index).at(slot_num);
        }
    }
    return modindex;
};


Int_t QwRaster::FindSignalIndex(const QwRaster::EModuleType modtype, const TString &name) const
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
