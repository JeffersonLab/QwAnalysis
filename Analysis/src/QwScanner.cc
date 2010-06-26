/**********************************************************\
* File: QwScanner.cc                                       *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Thu May 21 21:48:20 CDT 2009                             *
\**********************************************************/

#include "QwScanner.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwScanner> theScannerFactory("QwScanner");

extern QwHistogramHelper gQwHists;
const Bool_t QwScanner::bStoreRawData = kFALSE;
const UInt_t QwScanner::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwScanner::kMaxNumberOfChannelsPerModule = 32;
const UInt_t QwScanner::kV775Mask_SlotNumber        = 0xf8000000;  // 27-31
const UInt_t QwScanner::kV775Mask_WordType          = 0x07000000;  // 24-26
const UInt_t QwScanner::kV775Mask_ChannelNumber     = 0x001f0000;  // 16-20
const UInt_t QwScanner::kV775Mask_Dataword          = 0x00000fff;  // 0-11
const UInt_t QwScanner::kV775WordType_Datum    = 0;

QwScanner::QwScanner(TString region_tmp)
        :VQwSubsystem(region_tmp),
        VQwSubsystemTracking(region_tmp),
        VQwSubsystemParity(region_tmp) {

    fDEBUG = 0;

    myTimer = abs((Int_t) gRandom->Gaus(15,3));
    fDirectionX = fPreDirectionX = 1.0;
    fDirectionY = fPreDirectionY = 1.0;
    fMockFrontScaData = 0;
    fMockBackScaData = 0;
    fMockCoincidenceScaData = 0;

    fEvtCounter = 0;
    fTrigEvtCounter = 0;
    fSumEvtCounter = 0;

    ClearAllBankRegistrations();
};


QwScanner::~QwScanner() {

    fPMTs.clear();
    fSCAs.clear();

    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            delete fADC_Data.at(i);
            fADC_Data.at(i) = NULL;
        }
    }
    fADC_Data.clear();
    //DeleteHistograms();
};


void QwScanner::ProcessOptions(QwOptions &options){
      //Handle command line options
};

Int_t QwScanner::LoadChannelMap(TString mapfile) {
    TString varname, varvalue;
    TString modtype, dettype, name;
    Int_t modnum, channum;

    QwParameterFile mapstr(mapfile.Data());  //Open the file
    while (mapstr.ReadNextLine()) {
        mapstr.TrimComment('!');   // Remove everything after a '!' character.
        mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
        if (mapstr.LineIsEmpty())  continue;

        if (mapstr.HasVariablePair("=",varname,varvalue)) {
            //  This is a declaration line.  Decode it.
            varname.ToLower();
            //UInt_t value = atol(varvalue.Data());
            UInt_t value = QwParameterFile::GetUInt(varvalue);
            if (varname=="roc") {
                RegisterROCNumber(value);
            } else if (varname=="qdc_bank") {
                fBankID[0] = value;
                RegisterSubbank(value);
            } else if (varname=="sca_bank") {
                fBankID[1] = value;
                RegisterSubbank(value);
            } else if (varname=="f1tdc_bank") {
                fBankID[2] = value;
                RegisterSubbank(value);
            } else if (varname=="vqwk_bank") {
                fBankID[3] = value;
                RegisterSubbank(value);
            } else if (varname=="slot") {
                RegisterSlotNumber(value);
            }
        } else {
            //  Break this line into tokens to process it.
            modtype   = mapstr.GetNextToken(", ").c_str();
            modnum    = (atol(mapstr.GetNextToken(", ").c_str()));
            channum   = (atol(mapstr.GetNextToken(", ").c_str()));
            dettype   = mapstr.GetNextToken(", ").c_str();
            name      = mapstr.GetNextToken(", ").c_str();

            //  Push a new record into the element array
            if (modtype=="VQWK") {
                //std::cout<<"modnum="<<modnum<<"    "<<"fADC_Data.size="<<fADC_Data.size()<<std::endl;
                if (modnum >= (Int_t) fADC_Data.size())  fADC_Data.resize(modnum+1);
                if (! fADC_Data.at(modnum)) fADC_Data.at(modnum) = new QwVQWK_Module();
                fADC_Data.at(modnum)->SetChannel(channum, name);
            }

            else if (modtype=="SIS3801") {
                //std::cout<<"modnum="<<modnum<<"    "<<"fSCAs.size="<<fSCAs.size()<<std::endl;
                if (modnum >= (Int_t) fSCAs.size())  fSCAs.resize(modnum+1);
                if (! fSCAs.at(modnum)) fSCAs.at(modnum) = new QwSIS3801_Module();
                fSCAs.at(modnum)->SetChannel(channum, name);

            }

            else if (modtype=="V792" || modtype=="V775") {
                RegisterModuleType(modtype);
                //  Check to see if we've encountered this channel or name yet
                if (fModulePtrs.at(fCurrentIndex).at(channum).first>=0) {
                    //  We've seen this channel
                } else if (FindSignalIndex(fCurrentType, name)>=0) {
                    //  We've seen this signal
                } else {
                    //  If not, push a new record into the element array
                    if (modtype=="V792") std::cout<<"V792: ";
                    if (modtype=="V775") std::cout<<"V775: ";
                    LinkChannelToSignal(channum, name);
                }
            }

            else {
                std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
                << std::endl;
            }
        }
    }
    //
    ReportConfiguration();
    return 0;
};

Int_t QwScanner::LoadInputParameters(TString parameterfile) {
    Bool_t ldebug=kTRUE;
    TString varname, varvalue;
    Double_t varped;
    Double_t varcal;
    TString localname;

    Int_t lineread=0;

    QwParameterFile mapstr(parameterfile.Data());  //Open the file
    if (ldebug) std::cout<<"\nReading scanner parameter file: "<<parameterfile<<"\n";

    while (mapstr.ReadNextLine()) {
        lineread+=1;
        mapstr.TrimComment('!');   // Remove everything after a '!' character.
        mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
        if (mapstr.LineIsEmpty())  continue;

        if (mapstr.HasVariablePair("=",varname,varvalue)) {
            varname.ToLower();
            Double_t value = atof(varvalue.Data());
            if (varname=="helicityfrequency") {
                fHelicityFrequency = value;
            } else if (varname=="homepositionx") {
                fHomePositionX = value;
            } else if (varname=="homepositiony") {
                fHomePositionY = value;
            } else if (varname=="cal_factor_vqwk_x") {
                fCal_Factor_VQWK_X = value;
            } else if (varname=="cal_factor_vqwk_y") {
                fCal_Factor_VQWK_Y = value;
            } else if (varname=="cal_factor_qdc_x") {
                fCal_Factor_QDC_X = value;
            } else if (varname=="cal_factor_qdc_y") {
                fCal_Factor_QDC_Y = value;
            } else if (varname=="voltage_offset_x") {
                fVoltage_Offset_X = value;
                fCurrentPotentialX = fVoltage_Offset_X; // fCurrentPotentialX is uded for generating mock data
            } else if (varname=="voltage_offset_y") {
                fVoltage_Offset_Y = value;
                fCurrentPotentialY = fVoltage_Offset_Y; // fCurrentPotentialY is uded for generating mock data
            } else if (varname=="channel_offset_x") {
                fChannel_Offset_X = value;
            } else if (varname=="channel_offset_y") {
                fChannel_Offset_Y = value;
            }
            if (ldebug) std::cout<<"inputs for "<<varname<<": "<<value<<"\n";
        }

        else {
            varname = mapstr.GetNextToken(", \t").c_str();	//name of the channel
            varname.ToLower();
            varname.Remove(TString::kBoth,' ');
            varped= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
            varcal= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
            if (ldebug) std::cout<<"inputs for channel "<<varname
                <<": ped="<<varped<<", cal="<<varcal<<"\n";
            // Bool_t notfound=kTRUE;
        }
    }
    if (ldebug) std::cout<<" line read in the parameter file ="<<lineread<<" \n";

    ldebug=kFALSE;
    return 0;
};


void  QwScanner::ClearEventData() {
    SetDataLoaded(kFALSE);
    for (size_t i=0; i<fPMTs.size(); i++) {
        for (size_t j=0; j<fPMTs.at(i).size(); j++) {
            fPMTs.at(i).at(j).SetValue(0);
        }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
        if (fSCAs.at(i) != NULL) {
            fSCAs.at(i)->ClearEventData();
        }
    }

    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->ClearEventData();
        }
    }

};

void QwScanner::DecodeTDCWord(UInt_t &word) {

    fV775SlotNumber = (word & kV775Mask_SlotNumber)>>27;
    UInt_t wordtype = (word & kV775Mask_WordType)>>24;
    if (wordtype == kV775WordType_Datum) {
        fV775ValidFlag     = kTRUE;
        fV775ChannelNumber = (word & kV775Mask_ChannelNumber)>>16;
        /*     datavalid      = ((word & kV775Mask_DataValidBit)!=0); */
        /*     underthreshold = ((word & kV775Mask_UnderthresholdBit)!=0); */
        /*     overflow       = ((word & kV775Mask_OverflowBit)!=0); */
        fV775Dataword      = (word & kV775Mask_Dataword);
    } else {
        //  For now, don't distinguish between the header, tail word,
        //  or invalid data.
        //  Treat them all as invalid data.
        fV775ValidFlag     = kFALSE;
        fV775ChannelNumber = 0;
        fV775Dataword      = 0;
    }
};


Int_t QwScanner::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) {
    Int_t index = GetSubbankIndex(roc_id,bank_id);
    if (index>=0 && num_words>0) {
        //  We want to process the configuration data for this ROC.
        UInt_t words_read = 0;

        if(fBankID[3]==bank_id){
        for (size_t i=0; i<fADC_Data.size(); i++) {
            if (fADC_Data.at(i) != NULL) {
                words_read += fADC_Data.at(i)->ProcessConfigBuffer(&(buffer[words_read]),
                              num_words-words_read);
            }
        }
        }

        else if (fBankID[1]==bank_id){
        for (size_t i=0; i<fSCAs.size(); i++) {
            if (fSCAs.at(i) != NULL) {
                words_read += fSCAs.at(i)->ProcessConfigBuffer(&(buffer[words_read]),
                              num_words-words_read);
            }
        }
        }

    }
    return 0;
};


Int_t QwScanner::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) {
    Int_t index = GetSubbankIndex(roc_id,bank_id);

    SetDataLoaded(kTRUE);
    fEvtCounter++;
    if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
        << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
        <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
        << ", num_words "<<num_words<<", index "<<index<<std::endl;

    fSumFlag = 0;
    fTrigFlag = 0;

    //  This is a VQWK bank
    if (bank_id==fBankID[3]) {

        fSumEvtCounter++;
        fSumFlag = 5;

        if (index>=0 && num_words>0) {

            //  This is a VQWK bank We want to process this ROC.  Begin looping through the data.
            if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer: Processing Bank "<<bank_id
                <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
                << " fADC_Data.size() = "<<fADC_Data.size()<<std::endl;
            UInt_t words_read = 0;
            for (size_t i=0; i<fADC_Data.size(); i++) {
                if (fADC_Data.at(i) != NULL) {
                    words_read += fADC_Data.at(i)->ProcessEvBuffer(&(buffer[words_read]),
                                  num_words-words_read);
                    if (fDEBUG) {
                        std::cout<<"QwScanner::ProcessEvBuffer(VQWK): "<<words_read<<" words_read, "
                        <<num_words<<" num_words"<<" Data: ";
                        for (UInt_t j=(words_read-48); j<words_read; j++)
                            std::cout<<"     "<<std::hex<<buffer[j]<<std::dec;
                        std::cout<<std::endl;
                    }
                }
               else {
                    words_read += 48; // skip 48 data words (8 channel x 6 words/ch)
               }
            }

            if (fDEBUG & (num_words != words_read)) {
                std::cerr << "QwScanner::ProcessEvBuffer(VQWK):  There were "
                << num_words-words_read
                << " leftover words after decoding everything we recognize."
                << std::endl;
            }
        }
    }


    // This is a SCA bank
    if (bank_id==fBankID[1]) {

        if (index>=0 && num_words>0) {

            //  This is a SCA bank We want to process this ROC.  Begin looping through the data.
            if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer: Processing Bank "<<bank_id
                <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
                << " fSCAs.size() = "<<fSCAs.size()<<std::endl;

            UInt_t words_read = 0;
            for (size_t i=0; i<fSCAs.size(); i++) {
                words_read++; // skip header word
                if (fSCAs.at(i) != NULL) {
                    words_read += fSCAs.at(i)->ProcessEvBuffer(&(buffer[words_read]),
                                  num_words-words_read);

                    if (fDEBUG) {
                        std::cout<<"QwScanner::ProcessEvBuffer(SCA): "<<words_read<<" words_read, "
                        <<num_words<<" num_words"<<" Data: \n";
                        for (UInt_t j=0; j<words_read; j++){
                            std::cout<<"     "<<std::hex<<buffer[j]<<std::dec;
                            std::cout<<"(ch="<<((buffer[j]>>24) & 0x1f)
                                     <<" data="<<(buffer[j] & 0xffffff)<<")\n";
                        }

                    }

                }
                else {
                    words_read += 32; // skip a block of data for a single module
                }
            }

            if (fDEBUG & (num_words != words_read)) {
                std::cerr << "QwScanner::ProcessEvBuffer(SCA):  There were "
                << num_words-words_read
                << " leftover words after decoding everything we recognize."
                << std::endl;
            }
        }
    }


    //  This is a QADC/TDC bank
    if (bank_id==fBankID[0]) {

        fTrigEvtCounter++;
        fTrigFlag = 5;

        if (index>=0 && num_words>0) {
            //  We want to process this ROC.  Begin looping through the data.
            if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
                << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
                <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"<< std::endl;


            if (fDEBUG)
                std::cout<<"QwScanner::ProcessEvBuffer (trig) Data: \n";

            for (size_t i=0; i<num_words ; i++) {
                //  Decode this word as a V775TDC word.
                DecodeTDCWord(buffer[i]);

                if (! IsSlotRegistered(index, GetTDCSlotNumber())) continue;

                if (IsValidDataword()) {
                    // This is a V775 TDC data word
                  if (fDEBUG) {
                    std::cout<<"This is a valid QDC/TDC data word. Index="<<index
                             <<" slot="<<GetTDCSlotNumber()<<" Ch="<<GetTDCChannelNumber()
                             <<" Data="<<GetTDCData()<<"\n";
                    }

                    try {
                        //TODO The slot number should be set properly in DAQ
                        // using 0 for now
                        FillRawWord(index,GetTDCSlotNumber(),GetTDCChannelNumber(),GetTDCData());
                        //FillRawWord(index,0,GetTDCChannelNumber(),GetTDCData());

                    } catch (std::exception& e) {
                        std::cerr << "Standard exception from FocalPlaneScanner::FillRawTDCWord: "
                        << e.what() << std::endl;
                        Int_t chan = GetTDCChannelNumber();
                        std::cerr << "   Parameters:  index=="<<index
                        << "; GetV775SlotNumber()=="<<GetTDCSlotNumber()
                        << "; GetV775ChannelNumber()=="<<chan
                        << "; GetV775Data()=="<<GetTDCData()
                        << std::endl;
                        Int_t modindex = GetModuleIndex(index, GetTDCSlotNumber());
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


void  QwScanner::ProcessEvent() {
    if (! HasDataLoaded()) return;
    //std::cout<<"Scanner Events will be processed here."<<std::endl;

    for (size_t i=0; i<fPMTs.size(); i++) {
        for (size_t j=0; j<fPMTs.at(i).size(); j++) {
            fPMTs.at(i).at(j).ProcessEvent();
        }
    }

    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->ProcessEvent();
        }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
        if (fSCAs.at(i) != NULL) {
            fSCAs.at(i)->ProcessEvent();
        }
    }

    //Fill trigger data
    for (size_t i=0; i<fPMTs.size(); i++) {
        for (size_t j=0; j<fPMTs.at(i).size(); j++) {

            TString element_name = fPMTs.at(i).at(j).GetElementName();
            if (element_name==TString("front_adc"))
                fFrontADC = fPMTs.at(i).at(j).GetValue();
            else if (element_name==TString("back__adc"))
                fBackADC = fPMTs.at(i).at(j).GetValue();
            else if (element_name==TString("front_tdc"))
                fFrontTDC = fPMTs.at(i).at(j).GetValue();
            else if (element_name==TString("back__tdc"))
                fBackTDC = fPMTs.at(i).at(j).GetValue();
            // TODO jpan: replace the position determination with interplation table
            else if (element_name==TString("pos_x_adc"))
               {
                 fPositionX_ADC = fPMTs.at(i).at(j).GetValue();
                 fPositionX_ADC = (fPositionX_ADC-fChannel_Offset_X)*fCal_Factor_QDC_X + fHomePositionX;
               }
            else if (element_name==TString("pos_y_adc"))
               {
                 fPositionY_ADC = fPMTs.at(i).at(j).GetValue();
                 fPositionY_ADC = (fPositionY_ADC-fChannel_Offset_Y)*fCal_Factor_QDC_Y + fHomePositionY;
               }
        }
    }

    //Fill position data
    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {

            // TODO replace the position determination with interplation table
            fPowSupply_VQWK = fADC_Data.at(i)->GetChannel(TString("power_vqwk"))->GetAverageVolts();

            fPositionX_VQWK = fADC_Data.at(i)->GetChannel(TString("pos_x_vqwk"))->GetAverageVolts();
            fPositionX_VQWK = (fPositionX_VQWK-fVoltage_Offset_X)*fCal_Factor_VQWK_X + fHomePositionX;

            fPositionY_VQWK = fADC_Data.at(i)->GetChannel(TString("pos_y_vqwk"))->GetAverageVolts();
            fPositionY_VQWK = (fPositionY_VQWK-fVoltage_Offset_Y)*fCal_Factor_VQWK_Y + fHomePositionY;
        }
    }

    //Fill scaler data
    for (size_t i=0; i<fSCAs.size(); i++) {
        if (fSCAs.at(i) != NULL) {
            fCoincidenceSCA = fHelicityFrequency*(fSCAs.at(i)->GetChannel(TString("coinc_sca"))->GetValue());
            fFrontSCA = fHelicityFrequency*(fSCAs.at(i)->GetChannel(TString("front_sca"))->GetValue());
            fBackSCA = fHelicityFrequency*(fSCAs.at(i)->GetChannel(TString("back__sca"))->GetValue());
        }
    }

};


void  QwScanner::ConstructHistograms(TDirectory *folder, TString &prefix) {

    TString pat1 = "asym";
    TString pat2 = "yield";
    TString basename;

    if (prefix.BeginsWith(pat1)) {   }    //construct histograms in hel_histo folder if need
    else if (prefix.BeginsWith(pat2)) {    }

    else {
        if (prefix = "")  basename = "scanner_";
        else  basename = prefix;

        if (folder != NULL) folder->cd();

        // TODO (wdc) disabled due to restriction imposed by memory mapped file
        // Also changes to ConstructHistograms() calls below.
        //TDirectory* scannerfolder = folder->mkdir("scanner");

        if (bStoreRawData)
        {
            for (size_t i=0; i<fPMTs.size(); i++) {
                for (size_t j=0; j<fPMTs.at(i).size(); j++)
                    fPMTs.at(i).at(j).ConstructHistograms(folder, basename);
            }

            for (size_t i=0; i<fSCAs.size(); i++) {
                if (fSCAs.at(i) != NULL) {
                    fSCAs.at(i)->ConstructHistograms(folder, basename);
                }
            }

//             for (size_t i=0; i<fADC_Data.size(); i++) {
//                 if (fADC_Data.at(i) != NULL)
//                     fADC_Data.at(i)->ConstructHistograms(folder, basename);
//            }
        }

        //fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_vqwk_power")));
        fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_front_adc")));
        fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_back__adc")));
        fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_position_x")));
        fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_position_y")));
        fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_ref_posi_x")));
        fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_ref_posi_y")));

        fHistograms2D.push_back( gQwHists.Construct2DHist(TString("scanner_rate_map")));

        //TProfile2D(const char* name, const char* title,
        // Int_t nbinsx, Double_t xlow, Double_t xup,
        // Int_t nbinsy, Double_t ylow, Double_t yup,
        // Option_t* option = "")
        fRateMap  = new TProfile2D("scanner_rate_map_profile",
                                   "Scanner Rate Map Profile",110,-55,55,40,-355,-315);

    }
};

void  QwScanner::FillHistograms() {

    if (! HasDataLoaded()) return;

    if (bStoreRawData)
    {
        //Fill trigger data
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                fPMTs.at(i).at(j).FillHistograms();
            }
        }

        //Fill scaler data
        for (size_t i=0; i<fSCAs.size(); i++) {
            if (fSCAs.at(i) != NULL) {
                fSCAs.at(i)->FillHistograms();
            }
        }

        //Fill position data
//         for (size_t i=0; i<fADC_Data.size(); i++) {
//             if (fADC_Data.at(i) != NULL) {
//                 fADC_Data.at(i)->FillHistograms();
//             }
//         }

    }

    for (size_t j=0; j<fHistograms1D.size();j++) {

        if (fHistograms1D.at(j)->GetTitle()==TString("scanner_front_adc")) {
            fHistograms1D.at(j)->Fill(fFrontADC);
        }

        if (fHistograms1D.at(j)->GetTitle()==TString("scanner_back__adc")) {
            fHistograms1D.at(j)->Fill(fBackADC);
        }

        if (fHistograms1D.at(j)->GetTitle()==TString("scanner_position_x")) {
            fHistograms1D.at(j)->Fill(fPositionX_VQWK);
        }

        if (fHistograms1D.at(j)->GetTitle()==TString("scanner_position_y")) {
            fHistograms1D.at(j)->Fill(fPositionY_VQWK);
        }

        if (fHistograms1D.at(j)->GetTitle()==TString("scanner_ref_posi_x")) {
            fHistograms1D.at(j)->Fill(fPositionX_ADC);
        }

        if (fHistograms1D.at(j)->GetTitle()==TString("scanner_ref_posi_y")) {
            fHistograms1D.at(j)->Fill(fPositionY_ADC);
        }
    }

    //Fill rate map
    Double_t rate = fCoincidenceSCA;
    for (size_t j=0; j<fHistograms2D.size();j++) {
        if (fHistograms2D.at(j)->GetTitle()==TString("scanner_rate_map")) {
            Int_t checkvalidity = 1;
            Double_t prevalue = get_value( fHistograms2D.at(j), fPositionX_VQWK, fPositionY_VQWK, checkvalidity);
            if (checkvalidity!=0) {
                if (prevalue>0) {
                    rate = (prevalue + rate)*0.5;  //average value for this bin
                }
                fHistograms2D.at(j)->SetBinContent((Int_t) fPositionX_VQWK, (Int_t)fPositionY_VQWK,rate);
                Int_t xbin = fHistograms2D.at(j)->GetXaxis()->FindBin( fPositionX_VQWK );
                Int_t ybin = fHistograms2D.at(j)->GetYaxis()->FindBin( fPositionY_VQWK );
                fHistograms2D.at(j)->SetBinContent( fHistograms2D.at(j)->GetBin( xbin, ybin ), rate);
            }
        }
    }

    fRateMap->Fill(fPositionX_VQWK,fPositionY_VQWK,fCoincidenceSCA,1);

};


void  QwScanner::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values) {

    ConstructBranchAndVector(tree, prefix);
};

void  QwScanner::ConstructBranchAndVector(TTree *tree, TString &prefix) {

    TString pat1 = "asym";
    TString pat2 = "yield";
    TString basename;

    if (prefix.BeginsWith(pat1)) {}

    else if (prefix.BeginsWith(pat2)) {
        //tree->Branch("scanner_posx",&fPositionX_VQWK,"scanner_posx/D");
        //tree->Branch("scanner_posy",&fPositionY_VQWK,"scanner_posy/D");
        //tree->Branch("scanner_rate",&fCoincidenceSCA,"scanner_rate/D");
    }

    else {

        if (prefix=="") basename = "scanner";
        else basename = prefix;

        fScannerVector.reserve(6000);

        fScannerVector.push_back(0.0);
        TString list = "EvtCounter/D";
        fScannerVector.push_back(0.0);
        list += ":TrigEvtCounter/D";
        fScannerVector.push_back(0.0);
        list += ":SumEvtCounter/D";
        fScannerVector.push_back(0.0);
        list += ":SumFlag/D";
        fScannerVector.push_back(0.0);
        list += ":TrigFlag/D";
        fScannerVector.push_back(0.0);
        list += ":PowSupply_VQWK/D";
        fScannerVector.push_back(0.0);
        list += ":PositionX_VQWK/D";
        fScannerVector.push_back(0.0);
        list += ":PositionY_VQWK/D";
        fScannerVector.push_back(0.0);
        list += ":FrontSCA/D";
        fScannerVector.push_back(0.0);
        list += ":BackSCA/D";
        fScannerVector.push_back(0.0);
        list += ":CoincidenceSCA/D";
        fScannerVector.push_back(0.0);
        list += ":FrontADC/D";
        fScannerVector.push_back(0.0);
        list += ":BackADC/D";
        fScannerVector.push_back(0.0);
        list += ":FrontTDC/D";
        fScannerVector.push_back(0.0);
        list += ":BackTDC/D";
        fScannerVector.push_back(0.0);
//         list += ":PowSupply_ADC/D";
//         fScannerVector.push_back(0.0);
        list += ":PositionX_ADC/D";
        fScannerVector.push_back(0.0);
        list += ":PositionY_ADC/D";

        if (bStoreRawData) {

            for (size_t i=0; i<fPMTs.size(); i++) {
                for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                    //fPMTs.at(i).at(j).ConstructBranchAndVector(tree, prefix, fScannerVector);
                    if (fPMTs.at(i).at(j).GetElementName()=="") {
                        //  This channel is not used, so skip setting up the tree.
                    } else {
                        fScannerVector.push_back(0.0);
                        list += ":"+fPMTs.at(i).at(j).GetElementName()+"_raw/D";
                    }
                }
            }

            for (size_t i=0; i<fSCAs.size(); i++) {
                if (fSCAs.at(i) != NULL) {

                    for (size_t j=0; j<fSCAs.at(i)->fChannels.size(); j++) {
                        if (fSCAs.at(i)->fChannels.at(j).GetElementName()=="") {} else {
                            fScannerVector.push_back(0.0);
                            list += ":"+fSCAs.at(i)->fChannels.at(j).GetElementName()+"_raw/D";
                        }
                    }
                }
            }


            for (size_t i=0; i<fADC_Data.size(); i++) {
                if (fADC_Data.at(i) != NULL) {

                    for (size_t j=0; j<fADC_Data.at(i)->fChannels.size(); j++) {
                        TString channelname = fADC_Data.at(i)->fChannels.at(j).GetElementName();
                        channelname.ToLower();
                        if ( (channelname =="") || (channelname =="empty") || (channelname =="spare")) {} 
                        else {
                            fScannerVector.push_back(0.0);
                            list += ":"+fADC_Data.at(i)->fChannels.at(j).GetElementName()+"_raw/D";
                        }
                    }

                }
            }

        }

        //fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
        tree->Branch(basename, &fScannerVector[0], list);

    }
    return;
};


void  QwScanner::FillTreeVector(std::vector<Double_t> &values) {
    FillTreeVector();
};

void  QwScanner::FillTreeVector() {
    if (! HasDataLoaded()) return;

    Int_t index = 0;
    fScannerVector[index++] = fEvtCounter;
    fScannerVector[index++] = fTrigEvtCounter;
    fScannerVector[index++] = fSumEvtCounter;
    fScannerVector[index++] = fSumFlag;
    fScannerVector[index++] = fTrigFlag;
    fScannerVector[index++] = fPowSupply_VQWK;
    fScannerVector[index++] = fPositionX_VQWK;
    fScannerVector[index++] = fPositionY_VQWK;
    fScannerVector[index++] = fFrontSCA;
    fScannerVector[index++] = fBackSCA;
    fScannerVector[index++] = fCoincidenceSCA;
    fScannerVector[index++] = fFrontADC;
    fScannerVector[index++] = fBackADC;
    fScannerVector[index++] = fFrontTDC;
    fScannerVector[index++] = fBackTDC;
//    fScannerVector[index++] = fPowSupply_ADC;
    fScannerVector[index++] = fPositionX_ADC;
    fScannerVector[index++] = fPositionY_ADC;

    if (bStoreRawData) {

        //fill trigvalues
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                if (fPMTs.at(i).at(j).GetElementName()=="") {} else {
                    fScannerVector[index++] = fPMTs.at(i).at(j).GetValue();
                }
            }
        }

        for (size_t i=0; i<fSCAs.size(); i++) {
            if (fSCAs.at(i) != NULL) {
                for (size_t j=0; j<fSCAs.at(i)->fChannels.size(); j++) {
                    if (fSCAs.at(i)->fChannels.at(j).GetElementName()=="") {} else {
                        fScannerVector[index++] = fSCAs.at(i)->fChannels.at(j).GetValue();
                    }
                }
            } else {
                if(fDEBUG)
                  std::cerr<<"QwScanner::FillTreeVector:  "<<"fSCA_Data.at("<<i<<") is NULL"<<std::endl;
            }
        }

        //fill sumvalues
        for (size_t i=0; i<fADC_Data.size(); i++) {
            if (fADC_Data.at(i) != NULL) {

                for (size_t j=0; j<fADC_Data.at(i)->fChannels.size(); j++) {
                    TString channelname = fADC_Data.at(i)->fChannels.at(j).GetElementName();
                    channelname.ToLower();
                    if ( (channelname =="")
                            || (channelname.BeginsWith("empty"))
                            || (channelname.BeginsWith("spare")) ) {

                    } else {
                        //fScannerVector[index++] = fADC_Data.at(i)->fChannels.at(j).GetHardwareSum();
                        fScannerVector[index++] = fADC_Data.at(i)->fChannels.at(j).GetAverageVolts();
                    }
                }

            } else {
                if(fDEBUG)
                std::cerr<<"QwScanner::FillTreeVector:  "<<"fADC_Data.at(" <<i<<") is NULL"<< std::endl;
            }
        }


    }
    return;
};


void  QwScanner::DeleteHistograms() {

    if (bStoreRawData)
    {
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                if (fPMTs.at(i).at(j).GetElementName()=="") {} else  fPMTs.at(i).at(j).DeleteHistograms();
            }
        }

        for (size_t i=0; i<fSCAs.size(); i++) {
            if (fSCAs.at(i) != NULL) {
                fSCAs.at(i)->DeleteHistograms();
            }
        }

//         for (size_t i=0; i<fADC_Data.size(); i++) {
//             if (fADC_Data.at(i) != NULL) {
//                 fADC_Data.at(i)->DeleteHistograms();
//             }
//         }
    }

    for (size_t i=0; i<fHistograms1D.size(); i++) {
      if (fHistograms1D.at(i) != NULL) {
	delete fHistograms1D.at(i);
	fHistograms1D.at(i) =  NULL;
      }
    }

    for (size_t i=0; i<fHistograms2D.size(); i++) {
      if (fHistograms2D.at(i) != NULL) {
	delete fHistograms2D.at(i);
	fHistograms2D.at(i) =  NULL;
      }
    }

};

void  QwScanner::ReportConfiguration() {
    std::cout << "Configuration of the focal plane scanner:"<< std::endl;
    for (size_t i = 0; i<fROC_IDs.size(); i++) {
        for (size_t j=0; j<fBank_IDs.at(i).size(); j++) {

            Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
            std::cout << "ROC " << fROC_IDs.at(i)
            << ", subbank " << fBank_IDs.at(i).at(j)
            << ":  subbank index==" << ind
            << std::endl;

            if (fBank_IDs.at(i).at(j)==fBankID[0]) {
                for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++) {
                    Int_t QadcTdcindex = GetModuleIndex(ind,k);
                    if (QadcTdcindex != -1) {
                        std::cout << "    Slot " << k;
                        std::cout << "  Module#" << QadcTdcindex << std::endl;
                    }
                }
            } else if (fBank_IDs.at(i).at(j)==fBankID[1]) {
                std::cout << "    Number of SIS3801 Scaler:  " << fSCAs.size() << std::endl;
            } else if (fBank_IDs.at(i).at(j)==fBankID[2]) {
                std::cout << "    F1TDC added." << std::endl;
            } else if (fBank_IDs.at(i).at(j)==fBankID[3]) {
                std::cout << "    Number of TRIUMF ADC:  " << fADC_Data.size() << std::endl;
            }
        }
    }

}
; //ReportConfiguration()

Bool_t  QwScanner::Compare(QwScanner &value) {

    return kTRUE;
}


QwScanner& QwScanner::operator=  (QwScanner &value) {
    if (fPMTs.size() == value.fPMTs.size()) {
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                fPMTs.at(i).at(j) = value.fPMTs.at(i).at(j);
            }
        }
    }
//  else if (Compare(value)) {
//    QwScanner* input= (QwScanner &)value;
//    for(size_t i=0;i<input->fADC_Data.size();i++){
//      *(QwVQWK_Module*)fADC_Data.at(i) = *(QwVQWK_Module*)(input->fADC_Data.at(i));
//    }
//  }
    else {
        std::cerr << "QwScanner::operator=:  Problems!!!"
        << std::endl;
    }
    return *this;
};


QwScanner& QwScanner::operator+=  ( QwScanner &value) {
    if (fPMTs.size() == value.fPMTs.size()) {
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                fPMTs.at(i).at(j) += value.fPMTs.at(i).at(j);
            }
        }
    }
//  else if(Compare(value))
//  {
//    QwScanner* input= (QwScanner &)value ;
//    for(size_t i=0;i<input->fADC_Data.size();i++){
//      *(QwVQWK_Module*)fADC_Data.at(i) += *(QwVQWK_Module*)(input->fADC_Data.at(i));
//      }
//   }
    else {
        std::cerr << "QwScanner::operator=:  Problems!!!"
        << std::endl;
    }
    return *this;
};


QwScanner& QwScanner::operator-=  ( QwScanner &value) {
    if (fPMTs.size() == value.fPMTs.size()) {
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                fPMTs.at(i).at(j) -= value.fPMTs.at(i).at(j);
            }
        }
    }
//  else if(Compare(value))
//  {
//    QwScanner* input= (QwScanner &)value ;
//    for(size_t i=0;i<input->fADC_Data.size();i++){
//      *(QwVQWK_Module*)fADC_Data.at(i) -= *(QwVQWK_Module*)(input->fADC_Data.at(i));
//      }
//   }
    else {
        std::cerr << "QwScanner::operator=:  Problems!!!"
        << std::endl;
    }
    return *this;
};


void QwScanner::ClearAllBankRegistrations() {
    VQwSubsystem::ClearAllBankRegistrations();
    fModuleIndex.clear();
    fModulePtrs.clear();
    fModuleTypes.clear();
    fNumberOfModules = 0;
}

Int_t QwScanner::RegisterROCNumber(const UInt_t roc_id) {
    VQwSubsystem::RegisterROCNumber(roc_id, 0);
    fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
    std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
    fModuleIndex.push_back(tmpvec);
    return fCurrentBankIndex;
};

Int_t QwScanner::RegisterSubbank(const UInt_t bank_id) {
    Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
    fCurrentBankIndex++;
    std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
    fModuleIndex.push_back(tmpvec);
    //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
    return stat;
};


Int_t QwScanner::RegisterSlotNumber(UInt_t slot_id) {

    std::pair<Int_t, Int_t> tmppair;
    tmppair.first  = -1;
    tmppair.second = -1;
    if (slot_id<kMaxNumberOfModulesPerROC) {
        if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fModuleIndex.size()) {
            fModuleTypes.resize(fNumberOfModules+1);
            fModulePtrs.resize(fNumberOfModules+1);
            fModulePtrs.at(fNumberOfModules).resize(kMaxNumberOfChannelsPerModule,
                                                    tmppair);
            fNumberOfModules = fModulePtrs.size();
            fModuleIndex.at(fCurrentBankIndex).at(slot_id) = fNumberOfModules-1;
            fCurrentSlot     = slot_id;
            fCurrentIndex = fNumberOfModules-1;
        }
    } else {
        std::cerr << "QwScanner::RegisterSlotNumber:  Slot number "
        << slot_id << " is larger than the number of slots per ROC, "
        << kMaxNumberOfModulesPerROC << std::endl;
    }
    return fCurrentIndex;
};

const QwScanner::EModuleType QwScanner::RegisterModuleType(TString moduletype) {
    moduletype.ToUpper();

    //  Check to see if we've already registered a type for the current slot,
    //  if so, throw an error...

    if (moduletype=="V792") {
        fCurrentType = V792_ADC;
        fModuleTypes.at(fCurrentIndex) = fCurrentType;
        if ((Int_t) fPMTs.size()<=fCurrentType) {
            fPMTs.resize(fCurrentType+1);
        }
    }

    else if (moduletype=="V775") {
        fCurrentType = V775_TDC;
        fModuleTypes.at(fCurrentIndex) = fCurrentType;
        if ((Int_t) fPMTs.size()<=fCurrentType) {
            fPMTs.resize(fCurrentType+1);
        }

    }

    else if (moduletype=="SIS3801") {



    }

    return fCurrentType;
};


Int_t QwScanner::LinkChannelToSignal(const UInt_t chan, const TString &name) {
    size_t index = fCurrentType;
    if (index == 0 || index == 1) {
        fPMTs.at(index).push_back(QwPMT_Channel(name));
        fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
        fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(index).size() -1;
    } else if (index ==2) {
        std::cout<<"scaler module has not been implemented yet."<<std::endl;
    }
    std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
    return 0;
};

void QwScanner::FillRawWord(Int_t bank_index,
                            Int_t slot_num,
                            Int_t chan, UInt_t data) {
    Int_t modindex = GetModuleIndex(bank_index,slot_num);

    // std::cout<<"modtype="<<EModuleType(fModulePtrs.at(modindex).at(chan).first)
    // <<"  chanindex="<<fModulePtrs.at(modindex).at(chan).second<<"  data="<<data<<"\n";

    if (modindex != -1) {
        EModuleType modtype = EModuleType(fModulePtrs.at(modindex).at(chan).first);
        Int_t chanindex     = fModulePtrs.at(modindex).at(chan).second;

        if (modtype == EMPTY || chanindex == -1) {
            //  This channel is not connected to anything.
            //  Do nothing.
        } else {
            fPMTs.at(modtype).at(chanindex).SetValue(data);
        }
    };
};


Int_t QwScanner::GetModuleIndex(size_t bank_index, size_t slot_num) const {
    Int_t modindex = -1;
    if (bank_index>=0 && bank_index<fModuleIndex.size()) {
        if (slot_num>=0 && slot_num<fModuleIndex.at(bank_index).size()) {
            modindex = fModuleIndex.at(bank_index).at(slot_num);
        }
    }
    return modindex;
};


Int_t QwScanner::FindSignalIndex(const QwScanner::EModuleType modtype, const TString &name) const {
    size_t index = modtype;
    Int_t chanindex = -1;
    for (size_t chan=0; chan<fPMTs.at(index).size(); chan++) {
        if (name == fPMTs.at(index).at(chan).GetElementName()) {
            chanindex = chan;
            break;
        }
    }
    return chanindex;
};


/********************************************************/
void QwScanner::SetTriggerEventData(Double_t TrigEventValue) {
    for (size_t i=0; i<fPMTs.size(); i++) {
        for (size_t j=0; j<fPMTs.at(i).size(); j++) {
            fPMTs.at(i).at(j).SetValue(TrigEventValue);
        }
    }
}

void QwScanner::SetPositionEventData(Double_t* PositionEvBuf, UInt_t sequencenumber) {
    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->SetEventData(PositionEvBuf,sequencenumber);
        }
    }
    return;
};


/********************************************************/
void QwScanner::SetPedestal(Double_t pedestal) {
    fPedestal=pedestal;

    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->SetPedestal(fPedestal);
        }
    }

    return;
};

void QwScanner::SetCalibrationFactor(Double_t calib) {
    fCalibration=calib;
    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->SetCalibrationFactor(fCalibration);
        }
    }

    return;
};

/********************************************************/
void  QwScanner::InitializeChannel(TString name, TString datatosave) {
    SetPedestal(0.);
    SetCalibrationFactor(1.);
    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->InitializeChannel(name,datatosave);
        }
    }

    return;
};
/********************************************************/

void QwScanner::SetRandomEventParameters(Double_t mean, Double_t sigma) {
    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->SetRandomEventParameters(mean, sigma);
        }
    }

    return;
};


void QwScanner::RandomizeEventData(int helicity) {
    for (size_t i=0; i<fPMTs.size(); i++) {
        for (size_t j=0; j<fPMTs.at(i).size(); j++) {

            //jpan: It is not strict to use i,j as slot number and channel number,
            // but it is only for generating mock data anyway.
            fPMTs.at(i).at(j).RandomizeEventData(helicity,i,j);
        }
    }

    for (size_t i=0; i<fSCAs.size(); i++) {
        if (fSCAs.at(i) != NULL) {
            fSCAs.at(i)->RandomizeEventData(helicity);
        }
    }

    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->RandomizeEventData(helicity);
        }
    }
    return;
};

/********************************************************/
void QwScanner::EncodeEventData(std::vector<UInt_t> &buffer) {
    std::vector<UInt_t> SumBuffer;
    std::vector<UInt_t> ScaBuffer;
    std::vector<UInt_t> TrigBuffer;

    std::vector<UInt_t> LocalSumBuffer;
    std::vector<UInt_t> LocalScaBuffer;
    std::vector<UInt_t> LocalTrigBuffer;
    std::vector<UInt_t> subbankheader;
    std::vector<UInt_t> rocheader;

    myTimer-=1;  //timer counting down

    if (myTimer>0) {

    //mock data for trigger events
        for (size_t i=0; i<fPMTs.size(); i++) {
            for (size_t j=0; j<fPMTs.at(i).size(); j++) {
                fPMTs.at(i).at(j).EncodeEventData(LocalTrigBuffer);

                //assume a trimming level for the adc
                Double_t front_adc_trimmer = 1200.0;
                Double_t back_adc_trimmer = 1100.0;

                Double_t front_adc_value = 0.0;
                Double_t back_adc_value = 0.0;

                if (fPMTs.at(i).at(j).GetElementName()==TString("front_adc")) {
                    front_adc_value = UInt_t(fPMTs.at(i).at(j).GetValue())&0xFFF;
                    if (front_adc_value>front_adc_trimmer) {
                        fMockFrontScaData ++;
                    }
                }

                if (fPMTs.at(i).at(j).GetElementName()==TString("back__adc")) {
                    back_adc_value = UInt_t(fPMTs.at(i).at(j).GetValue())&0xFFF;
                    if (back_adc_value>back_adc_trimmer) {
                        fMockBackScaData ++;
                    }
                }

                if (  (back_adc_value>back_adc_trimmer) && (back_adc_value>back_adc_trimmer) ) {
                    fMockCoincidenceScaData ++;
                }
            }
        }
    }

    else { //(myTimer ==0)

    //mock data for scaler events
    //replace mock data for ch0, ch1, ch2
        for (size_t i=0; i<fSCAs.size(); i++) {
            if (fSCAs.at(i) != NULL) {
                fSCAs.at(i)->GetChannel(TString("front_sca"))->SetEventData(fMockFrontScaData);
                fSCAs.at(i)->GetChannel(TString("back__sca"))->SetEventData(fMockBackScaData);
                fSCAs.at(i)->GetChannel(TString("coinc_sca"))->SetEventData(fMockCoincidenceScaData);
                fSCAs.at(i)->EncodeEventData(LocalScaBuffer);
            }
        }

        fMockFrontScaData = 0;
        fMockBackScaData = 0;
        fMockCoincidenceScaData = 0;

//mock data for position events
        for (size_t i=0; i<fADC_Data.size(); i++) {
            if (fADC_Data.at(i) != NULL) {
                fADC_Data.at(i)->EncodeEventData(LocalSumBuffer);
            }
        }

//replace mock data for ch1, ch2, ch3
        Double_t kVQWK_VoltsPerBit = 76.29e-6;
        UInt_t fNumberOfSamples = 16680;
        UInt_t fSequenceNumber = 255;
        UInt_t localbuf[6];

//ch1: DC +9.6 +/-0.2 V power supply
        Double_t mean = 9.6;  //units: V
        Double_t sigma = 0.0001;
        Double_t pValue;
        localbuf[4] = 0;
        for (size_t i = 0; i < 4; i++) {
            pValue = gRandom->Gaus(mean,sigma);
            localbuf[i] = (UInt_t) (pValue/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
            localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
        }
        localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                      | (fSequenceNumber  << 8  & 0x0000FF00);

        for (size_t i = 0; i < 6; i++) {
            LocalSumBuffer[i] =localbuf[i];
        }

//ch2: X motion, DC 2-9.5V, linear motion, back and forth
// speed = 5cm/s, 0.02cm per helicity state (assume 4 ms duration)
// Cal_FactorX = 5.333 cm/V, ==>> step size = 0.02cm / 5.333cm/V = 0.00375V,
// so the voltage in each block will increase ~ 0.00375/4 = 0.0009375V
        mean = 0.00375;  //units: V
        sigma = 0.0000001;
        if (fCurrentPotentialX>9.5) fDirectionX = -1.0;
        if (fCurrentPotentialX<2.0) fDirectionX = 1.0;

        localbuf[4] = 0;
        for (size_t i = 0; i < 4; i++) {
            pValue = 0.25*gRandom->Gaus(mean,sigma);
            fCurrentPotentialX+=pValue*fDirectionX;
            localbuf[i] = (UInt_t) (fCurrentPotentialX/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
            localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
        }
        localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                      | (fSequenceNumber  << 8  & 0x0000FF00);

        for (size_t i = 0; i < 6; i++) {
            LocalSumBuffer[6+i] =localbuf[i];
        }

//ch3: Y motion, stepping motion with 200 steps, increase from 2 to 9.5V,
// moving when X-motion changing direction, Cal_FactorY = 26.667 cm/V,
// each step = (9.5-2)V/200=0.0375V
        mean = 0.0375;  //units: V
        sigma = 0.0000001;
        if (fCurrentPotentialY>9.5) fDirectionY = -1.0;
        if (fCurrentPotentialY<2.0) fDirectionY = 1.0;

        localbuf[4] = 0;
        if (fPreDirectionX != fDirectionX) {
            fPreDirectionX = fDirectionX;
            for (size_t i = 0; i < 4; i++) {
                pValue = 0.25*gRandom->Gaus(mean,sigma);
                fCurrentPotentialY+=pValue*fDirectionY;
                localbuf[i] = (UInt_t) (fCurrentPotentialY/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
                localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
            }
        } else {
            for (size_t i = 0; i < 4; i++) {
                fCurrentPotentialY=gRandom->Gaus(fCurrentPotentialY,sigma);
                localbuf[i] = (UInt_t) (fCurrentPotentialY/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
                localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
            }
        }
        localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                      | (fSequenceNumber  << 8  & 0x0000FF00);

        for (size_t i = 0; i < 6; i++) {
            LocalSumBuffer[12+i] =localbuf[i];
        }

        myTimer = abs((Int_t) gRandom->Gaus(15,3));
    } //end of if(myTimer ==0)

    // If there is element data, generate the subbank header
    if (LocalSumBuffer.size() > 0) {

        // Form CODA subbank header
        subbankheader.clear();
        subbankheader.push_back(LocalSumBuffer.size() + 1);    // subbank size
        subbankheader.push_back((fBankID[3] << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

        // Form CODA bank/roc header
        rocheader.clear();
        rocheader.push_back(subbankheader.size() + LocalSumBuffer.size() + 1);    // bank/roc size
        rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

        // Add bank header, subbank header and element data to output buffer
        SumBuffer.insert(SumBuffer.end(), rocheader.begin(), rocheader.end());
        SumBuffer.insert(SumBuffer.end(), subbankheader.begin(), subbankheader.end());
        SumBuffer.insert(SumBuffer.end(), LocalSumBuffer.begin(), LocalSumBuffer.end());
    }


    if (LocalScaBuffer.size() > 0) {

        // Form CODA subbank header
        subbankheader.clear();
        subbankheader.push_back(LocalScaBuffer.size() + 1);    // subbank size
        subbankheader.push_back((fBankID[1] << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

        // Form CODA bank/roc header
        rocheader.clear();
        rocheader.push_back(subbankheader.size() + LocalScaBuffer.size() + 1);    // bank/roc size
        rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

        // Add bank header, subbank header and element data to output buffer
        ScaBuffer.insert(ScaBuffer.end(), rocheader.begin(), rocheader.end());
        ScaBuffer.insert(ScaBuffer.end(), subbankheader.begin(), subbankheader.end());
        ScaBuffer.insert(ScaBuffer.end(), LocalScaBuffer.begin(), LocalScaBuffer.end());
    }


    if (LocalTrigBuffer.size() > 0) {

        // Form CODA subbank header
        subbankheader.clear();
        subbankheader.push_back(LocalTrigBuffer.size() + 1);    //subbank size
//    subbankheader.push_back((2101 << 16) | (0x01 << 8) | (1 & 0xff));
        subbankheader.push_back((fBankID[0] << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

        // Form CODA bank/roc header
        rocheader.clear();
        rocheader.push_back(subbankheader.size() + LocalTrigBuffer.size() + 1);    // bank/roc size
        rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

        // Add bank header, subbank header and element data to output buffer
        TrigBuffer.insert(TrigBuffer.end(), rocheader.begin(), rocheader.end());
        TrigBuffer.insert(TrigBuffer.end(), subbankheader.begin(), subbankheader.end());
        TrigBuffer.insert(TrigBuffer.end(), LocalTrigBuffer.begin(), LocalTrigBuffer.end());
    }

    //Copy elements from TrigBuffer, ScaBuffer and SumBuffer into buffer
    for (UInt_t i=0; i<TrigBuffer.size(); i++)
        buffer.push_back(TrigBuffer.at(i));

    for (UInt_t i=0; i<ScaBuffer.size(); i++)
        buffer.push_back(ScaBuffer.at(i));

    for (UInt_t i=0; i<SumBuffer.size(); i++)
        buffer.push_back(SumBuffer.at(i));

    // Print buffer
    int kDebug = 0;

    if (kDebug) {
        std::cout << std::endl << "SumBuffer: ";
        for (size_t i = 0; i < SumBuffer.size(); i++)
            std::cout << std::hex << SumBuffer.at(i) << " ";
        std::cout << std::dec << std::endl;
    }
    if (kDebug) {
        std::cout << std::endl << "ScaBuffer: ";
        for (size_t i = 0; i < ScaBuffer.size(); i++)
            std::cout << std::hex << ScaBuffer.at(i) << " ";
        std::cout << std::dec << std::endl;
    }
    if (kDebug) {
        std::cout << std::endl << "TrigBuffer: ";
        for (size_t i = 0; i < TrigBuffer.size(); i++)
            std::cout << std::hex << TrigBuffer.at(i) << " ";
        std::cout << std::dec << std::endl;
    }

};
/********************************************************/

void QwScanner::PrintInfo() {
    std::cout << "QwScanner: " << fSystemName << std::endl;

    //fTriumf_ADC.PrintInfo();
    for (size_t i=0; i<fADC_Data.size(); i++) {
        if (fADC_Data.at(i) != NULL) {
            fADC_Data.at(i)->PrintInfo();
        }
    }

    for (size_t i=0; i<fPMTs.size(); i++) {
        for (size_t j=0; j<fPMTs.at(i).size(); j++) {
            fPMTs.at(i).at(j).PrintInfo();
        }
    }

    return;
}

//scanner analysis utilities
Double_t QwScanner::get_value( TH2* h, Double_t x, Double_t y, Int_t& checkvalidity) {
    if (checkvalidity) {
        bool x_ok = ( h->GetXaxis()->GetXmin() < x && x < h->GetXaxis()->GetXmax() );
        bool y_ok = ( h->GetYaxis()->GetXmin() < y && y < h->GetYaxis()->GetXmax() );

        if (! ( x_ok && y_ok)) {
//             if (!x_ok){
//                 std::cerr << "x value " << x << " out of range ["<< h->GetXaxis()->GetXmin()
//                           <<","<< h->GetXaxis()->GetXmax() << "]" << std::endl;
//             }
//             if (!y_ok){
//                 std::cerr << "y value " << y << " out of range ["<< h->GetYaxis()->GetXmin()
//                           <<","<< h->GetYaxis()->GetXmax() << "]" << std::endl;
//             }
            checkvalidity=0;
            return -1e20;
        }
    }

    const int xbin = h->GetXaxis()->FindBin( x );
    const int ybin = h->GetYaxis()->FindBin( y );

    return h->GetBinContent( h->GetBin( xbin, ybin ));
};

