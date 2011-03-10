/**
 * \file	QwComptonPhotonDetector.cc
 *
 * \brief	Implementation of the analysis of Compton photon detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonPhotonDetector class is defined as a parity subsystem that
 * contains all data modules of the photon detector (SIS3320, V775, V792).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#include "QwComptonPhotonDetector.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"


// Register this subsystem with the factory
RegisterSubsystemFactory(QwComptonPhotonDetector);


/**
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 */

void QwComptonPhotonDetector::ProcessOptions(QwOptions &options)
{
  // Handle command line options
}

Int_t QwComptonPhotonDetector::LoadChannelMap(TString mapfile)
{
  Int_t subbank = -1; // subbank index
  Int_t current_roc_id = -1; // current ROC id
  Int_t current_bank_id = -1; // current bank id

  // Open the file
  QwParameterFile mapstr(mapfile.Data());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();      // Remove everything after a comment character
    mapstr.TrimWhitespace();   // Get rid of leading and trailing whitespace
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)) {
      // This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
        current_roc_id = value;
        RegisterROCNumber(current_roc_id,0);
      } else if (varname == "bank") {
        current_bank_id = value;
        RegisterSubbank(current_bank_id);
        subbank = GetSubbankIndex(current_roc_id,current_bank_id);
      }

    } else {
      //  Break this line into tokens to process it.
      TString modtype = mapstr.GetNextToken().c_str();
      UInt_t modnum   = QwParameterFile::GetUInt(mapstr.GetNextToken().c_str());
      UInt_t channum  = QwParameterFile::GetUInt(mapstr.GetNextToken().c_str());
      TString dettype = mapstr.GetNextToken().c_str();
      TString name    = mapstr.GetNextToken().c_str();

      //  Push a new record into the element array
      if (modtype == "SIS3320") {
        // Register data channel type
        fMapping[subbank] = kSamplingADC;
        // Increase module mapping size
        if (modnum >= fSamplingADC_Mapping[subbank].size())
          fSamplingADC_Mapping[subbank].resize(modnum+1);
        // Increase channel mapping size
        if (channum >= fSamplingADC_Mapping[subbank].at(modnum).size())
          fSamplingADC_Mapping[subbank].at(modnum).resize(channum+1,-1);
        // Search whether this module/channel is already registered
        for (SamplingADC_Mapping_t::iterator iter = fSamplingADC_Mapping.begin();
            iter != fSamplingADC_Mapping.end(); iter++) {
          if (iter->first != subbank &&
              iter->second.size() > modnum &&
              iter->second.at(modnum).size() > channum &&
              iter->second.at(modnum).at(channum) >= 0) {
            QwMessage << "Connecting SIS3320 " << name << "/" << dettype
                      << std::hex
                      << " in ROC 0x" << current_roc_id << ", bank 0x" << current_bank_id
                      << std::dec
                      << " to existing SIS3320." << QwLog::endl;
            fSamplingADC_Mapping[subbank].at(modnum).at(channum) =
                iter->second.at(modnum).at(channum);
          }
        }
        // Register new SIS3320
        if (fSamplingADC_Mapping[subbank].at(modnum).at(channum) < 0) {
          QwMessage << "Registering SIS3320 " << name << "/" << dettype
                    << std::hex
                    << " in ROC 0x" << current_roc_id << ", bank 0x" << current_bank_id
                    << std::dec
                    << " at mod " << modnum << ", chan " << channum
                    << QwLog::endl;
          UInt_t index = fSamplingADC.size();
          fSamplingADC_Mapping[subbank].at(modnum).at(channum) = index;
          fSamplingADC.push_back(MQwSIS3320_Channel(channum, name));
          fSamplingADC.at(index).SetNumberOfAccumulators(6);
          fSamplingADC.at(index).InitializeChannel(channum, name);
        }

      } else if (modtype == "V792") {
        // Register data channel type
        fMapping[subbank] = kIntegratingADC;
        // Add to mapping
        if (modnum >= fMultiQDC_Mapping[subbank].size())
          fMultiQDC_Mapping[subbank].resize(modnum+1);
        if (channum >= fMultiQDC_Mapping[subbank].at(modnum).size())
          fMultiQDC_Mapping[subbank].at(modnum).resize(channum+1,-1);
        // Add scaler channel
        if (fMultiQDC_Mapping[subbank].at(modnum).at(channum) < 0) {
          QwMessage << "Registering V792 " << name
                    << std::hex
                    << " in ROC 0x" << current_roc_id << ", bank 0x" << current_bank_id
                    << std::dec
                    << " at mod " << modnum << ", chan " << channum
                    << QwLog::endl;
          UInt_t index = fMultiQDC_Channel.size();
          fMultiQDC_Mapping[subbank].at(modnum).at(channum) = index;
          fMultiQDC_Channel.push_back(QwPMT_Channel(name));
          fMultiQDC_Channel.at(index).SetModule(modnum);
          fMultiQDC_Channel.at(index).SetSubbankID(current_bank_id);
        }

      } else if (modtype == "V775") {
        // Register data channel type
        fMapping[subbank] = kIntegratingTDC;
        // Add to mapping
        if (modnum >= fMultiTDC_Mapping[subbank].size())
          fMultiTDC_Mapping[subbank].resize(modnum+1);
        if (channum >= fMultiTDC_Mapping[subbank].at(modnum).size())
          fMultiTDC_Mapping[subbank].at(modnum).resize(channum+1,-1);
        // Add scaler channel
        if (fMultiTDC_Mapping[subbank].at(modnum).at(channum) < 0) {
          QwMessage << "Registering V775 " << name
                    << std::hex
                    << " in ROC 0x" << current_roc_id << ", bank 0x" << current_bank_id
                    << std::dec
                    << " at mod " << modnum << ", chan " << channum
                    << QwLog::endl;
          UInt_t index = fMultiTDC_Channel.size();
          fMultiTDC_Mapping[subbank].at(modnum).at(channum) = index;
          fMultiTDC_Channel.push_back(QwPMT_Channel(name));
          fMultiTDC_Channel.at(index).SetModule(modnum);
          fMultiTDC_Channel.at(index).SetSubbankID(current_bank_id);
        }

      } else if (modtype == "SIS3801D24" || modtype == "SIS3801D32") {
        // Register data channel type
        fMapping[subbank] = kScaler;
        // Add to mapping
        if (modnum >= fScaler_Mapping[subbank].size())
          fScaler_Mapping[subbank].resize(modnum+1);
        if (channum >= fScaler_Mapping[subbank].at(modnum).size())
          fScaler_Mapping[subbank].at(modnum).resize(channum+1,-1);
        // Add scaler channel
        if (fScaler_Mapping[subbank].at(modnum).at(channum) < 0) {
          QwMessage << "Registering " << modtype << " " << name
                    << std::hex
                    << " in ROC 0x" << current_roc_id << ", bank 0x" << current_bank_id
                    << std::dec
                    << " at mod " << modnum << ", chan " << channum
                    << QwLog::endl;
          UInt_t index = fScaler.size();
          fScaler_Mapping[subbank].at(modnum).at(channum) = index;
          if (modtype == "SIS3801D24")
            fScaler.push_back(new QwSIS3801D24_Channel(name));
          else if (modtype == "SIS3801D32")
            fScaler.push_back(new QwSIS3801D32_Channel(name));

        }

      } // end of switch by modtype

    } // end of if for token line
  } // end of while over parameter file

  return 0;
}

/**
 * Load the event cuts
 * @param filename Event cuts file
 * @return Zero if successful
 */
Int_t QwComptonPhotonDetector::LoadEventCuts(TString & filename)
{
  return 0;
}

/**
 * Load the input parameters
 * @param pedestalfile Input parameters file
 * @return Zero if successful
 */
Int_t QwComptonPhotonDetector::LoadInputParameters(TString pedestalfile)
{
  // Open the file
  QwParameterFile mapstr(pedestalfile.Data());
  Bool_t found = kFALSE;
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();    // Remove everything after a comment character
    mapstr.TrimWhitespace(); // Get rid of leading and trailing spaces

    TString varname = "";
    Double_t varped = 0.0;
    Double_t varcal = 1.0;

    if (mapstr.LineIsEmpty())  continue;
    else {
      varname = mapstr.GetNextToken(", \t").c_str();  // name of the channel
      varname.ToLower();
      varname.Remove(TString::kBoth,' ');
      varped = (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
      varcal = (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
    }

    for (size_t i = 0; i < fSamplingADC.size() && !found; i++) {
      TString localname = fSamplingADC[i].GetElementName();
      localname.ToLower();
      if (localname == varname) {
        fSamplingADC[i].SetPedestal(varped);
        fSamplingADC[i].SetCalibrationFactor(varcal);
        found = kTRUE;
      }
    } // end of loop over all sampling ADC channels


    for (size_t i = 0; i < fScaler.size() && !found; i++) {
      TString localname = fScaler[i]->GetElementName();
      localname.ToLower();
      if (localname == varname) {
        fScaler[i]->SetPedestal(varped);
        fScaler[i]->SetCalibrationFactor(varcal);
        found = kTRUE;
      }
    } // end of loop over all scaler channels

  } // end of loop reading all lines of the pedestal file

  return 0;
}


/**
 * Randomize this event with the given helicity
 * @param helicity Helicity of this event (default is zero)
 */
void QwComptonPhotonDetector::RandomizeEventData(int helicity)
{
  // Randomize all MQwSIS3320 buffers
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].RandomizeEventData(helicity);

  // Randomize all MQwV775TDC buffers
  //for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
  //  fMultiTDC_Channel[i].RandomizeEventData(helicity);

  // Randomize all MQwV792ADC buffers
  //for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
  //  fMultiQDC_Channel[i].RandomizeEventData(helicity);
}


/**
 * Encode this event into a CODA buffer
 * @param buffer Buffer to append data to
 */
void QwComptonPhotonDetector::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].EncodeEventData(elements);

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (elements.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(elements.size() + 1);	// subbank size
    subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
		// subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
		// bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
    buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
    buffer.insert(buffer.end(), elements.begin(), elements.end());
  }
}


/**
 * Process the event buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
Int_t QwComptonPhotonDetector::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  UInt_t words_read = 0;

  // Get the subbank index (or -1 when no match)
  Int_t subbank = GetSubbankIndex(roc_id, bank_id);

  if (subbank >= 0 && num_words > 0) {

    //  We want to process this ROC.  Begin looping through the data.

    switch (fMapping[subbank]) {

      // Sampling ADCs
      case kSamplingADC:
      {
        for (size_t modnum = 0; modnum < fSamplingADC_Mapping[subbank].size(); modnum++) {
          for (size_t channum = 0; channum < fSamplingADC_Mapping[subbank].at(modnum).size(); channum++) {
            Int_t index = fSamplingADC_Mapping[subbank].at(modnum).at(channum);
            if (index >= 0) {
              words_read += fSamplingADC[index].ProcessEvBuffer(&(buffer[words_read]), num_words - words_read);
            }
          }
        }
        break;
      }

      // Scalers
      case kScaler:
      {
        // Read header word
        //UInt_t num_events = buffer[words_read];
        words_read++;
        // TODO Multiscaler functionality

        // Read scalers
        for (size_t modnum = 0; modnum < fScaler_Mapping[subbank].size(); modnum++) {
          for (size_t channum = 0; channum < fScaler_Mapping[subbank].at(modnum).size(); channum++) {
            Int_t index = fScaler_Mapping[subbank].at(modnum).at(channum);
            if (index >= 0) {
              words_read += fScaler[index]->ProcessEvBuffer(&(buffer[words_read]), num_words - words_read);
            }
          }
        }
        words_read = num_words;
        break;
      }

      // Integrating ADCs and TDCs
      case kIntegratingADC:
      case kIntegratingTDC:
      {
        // Read header word
        UInt_t header = buffer[words_read++];
        if ((header & 0xbad00dc0) == 0xbad00dc0)
          break; // Bad ADC/TDC

        // Parse number of events
        UInt_t num_events = 0;
        if ((header & 0xda000dc0) == 0xda000dc0)
          num_events = (buffer[0] & 0x00FF0000) >> 16;
        // TODO Multihit functionality

        // Loop over buffered events
        for (size_t i = words_read; i < num_words; i++) {

          // Decode this word as a V775 TDC / V792 QDC word
          DecodeTDCWord(buffer[i]);

          // Is this a valid V775 TDC / V792 QDC data word?
          if (! IsValidDataword()) continue;

          // Is there an QDC channel registered for this slot and channel?
          for (size_t modnum = 0; modnum < fMultiQDC_Mapping[subbank].size(); modnum++) {
            UInt_t channum = GetTDCChannelNumber();
            if (channum < fMultiQDC_Mapping[subbank].at(modnum).size()) {
              Int_t index = fMultiQDC_Mapping[subbank].at(modnum).at(channum);
              if (index >= 0) {
                fMultiQDC_Channel.at(index).SetValue(GetTDCData());
                QwDebug << "QDC " << std::hex << subbank << " "
                        << modnum << "," << channum << ": " << std::dec
                        << GetTDCData() << QwLog::endl;
              }
            }
          }

          // Is there an TDC channel registered for this slot and channel?
          for (size_t modnum = 0; modnum < fMultiTDC_Mapping[subbank].size(); modnum++) {
            UInt_t channum = GetTDCChannelNumber();
            if (channum < fMultiTDC_Mapping[subbank].at(channum).size()) {
              Int_t index = fMultiTDC_Mapping[subbank].at(modnum).at(channum);
              if (index >= 0) {
                fMultiTDC_Channel.at(index).SetValue(GetTDCData());
                QwDebug << "TDC " << std::hex << subbank << " "
                        << modnum << "," << channum << ": " << std::dec
                        << GetTDCData() << QwLog::endl;
              }
            }
          }

        } // end of loop over buffered events

        words_read = num_words;
        break;
      }

      // Unknown data channel type
      case kUnknown:
      default:
      {
        QwError << "QwComptonPhotonDetector: Unknown data channel type" << QwLog::endl;
        break;
      }
    }

    // Check for leftover words
    if (num_words != words_read) {
      QwError << "QwComptonPhotonDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize"
              << std::hex
              << " in ROC " << roc_id << ", bank " << bank_id << "."
              << std::dec
              << QwLog::endl;
    }
  }

  return words_read;
}


/**
 * Process the single event cuts
 * @return
 */
Bool_t QwComptonPhotonDetector::SingleEventCuts()
{
  QwDebug << "QwComptonPhotonDetector::SingleEventCuts()" << QwLog::endl;
  return IsGoodEvent();
}


/**
 * Process this event
 */
void  QwComptonPhotonDetector::ProcessEvent()
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].ProcessEvent();
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].ProcessEvent();
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].ProcessEvent();
}


/**
 * Process the configuration buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
Int_t QwComptonPhotonDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
}


/**
 * Check whether this is a good event
 * @return True if the event is good
 */
Bool_t QwComptonPhotonDetector::IsGoodEvent()
{
  Bool_t test = kTRUE;

  for(size_t i=0;i<fSamplingADC.size();i++)
    test &= fSamplingADC[i].IsGoodEvent();
//  for(size_t i=0;i<fMultiTDC_Channel.size();i++)
//    test &= fMultiTDC_Channel[i].IsGoodEvent();
//  for(size_t i=0;i<fMultiQDC_Channel.size();i++)
//    test &= fMultiQDC_Channel[i].IsGoodEvent();

  if (!test) std::cerr << "This is not a good event!" << std::endl;
  return test;
}


/**
 * Clear the event data in this subsystem
 */
void QwComptonPhotonDetector::ClearEventData()
{
  // Clear all sampling ADC channels
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].ClearEventData();

  // Clear all TDC and QDC channels
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].ClearEventData();
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].ClearEventData();

  // Clear all scaler channels
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i]->ClearEventData();
}

/**
 * Assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem&  QwComptonPhotonDetector::operator=  (VQwSubsystem *value)
{
  if (CompareADC(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(value);
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] = input->fSamplingADC[i];
  }

  if (CompareTDC(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(value);
    for (size_t i = 0; i < input->fMultiTDC_Channel.size();i++)
      this->fMultiTDC_Channel[i] = input->fMultiTDC_Channel[i];
  }

  if (CompareQDC(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(value);
    for (size_t i = 0; i < input->fMultiQDC_Channel.size();i++)
      this->fMultiQDC_Channel[i] = input->fMultiQDC_Channel[i];
  }

  if (CompareScaler(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(value);
    for (size_t i = 0; i < fScaler.size(); i++)
      *(this->fScaler[i]) = *(input->fScaler[i]);
  }

  return *this;
}

/**
 * Addition-assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem&  QwComptonPhotonDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(value);
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] += input->fSamplingADC[i];
    for (size_t i = 0; i < input->fMultiTDC_Channel.size(); i++)
      this->fMultiTDC_Channel[i] += input->fMultiTDC_Channel[i];
    for (size_t i = 0; i < input->fMultiQDC_Channel.size(); i++)
      this->fMultiQDC_Channel[i] += input->fMultiQDC_Channel[i];
    for (size_t i = 0; i < fScaler.size(); i++)
      *(this->fScaler[i]) += *(input->fScaler[i]);
  }
  return *this;
}

/**
 * Subtraction-assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem&  QwComptonPhotonDetector::operator-=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (value);
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] -= input->fSamplingADC[i];
    for (size_t i = 0; i < input->fMultiTDC_Channel.size(); i++)
      this->fMultiTDC_Channel[i] -= input->fMultiTDC_Channel[i];
    for (size_t i = 0; i < input->fMultiQDC_Channel.size(); i++)
      this->fMultiQDC_Channel[i] -= input->fMultiQDC_Channel[i];
    for (size_t i = 0; i < fScaler.size(); i++)
      *(this->fScaler[i]) -= *(input->fScaler[i]);
  }
  return *this;
}

/**
 * Summation
 * @param value1 First value
 * @param value2 Second value
 */
void  QwComptonPhotonDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
}

/**
 * Difference
 * @param value1 First value
 * @param value2 Second value
 */
void  QwComptonPhotonDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
}

/**
 * Determine the ratio of two photon detectors
 * @param numer Numerator
 * @param denom Denominator
 */
void QwComptonPhotonDetector::Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwComptonPhotonDetector* innumer = dynamic_cast<QwComptonPhotonDetector*> (numer);
    QwComptonPhotonDetector* indenom = dynamic_cast<QwComptonPhotonDetector*> (denom);
    for (size_t i = 0; i < innumer->fSamplingADC.size(); i++)
      this->fSamplingADC[i].Ratio(innumer->fSamplingADC[i], indenom->fSamplingADC[i]);
    for (size_t i = 0; i < innumer->fMultiTDC_Channel.size(); i++)
      this->fMultiTDC_Channel[i].Ratio(innumer->fMultiTDC_Channel[i], indenom->fMultiTDC_Channel[i]);
    for (size_t i = 0; i < innumer->fMultiQDC_Channel.size(); i++)
      this->fMultiQDC_Channel[i].Ratio(innumer->fMultiQDC_Channel[i], indenom->fMultiQDC_Channel[i]);
    for (size_t i = 0; i < fScaler.size(); i++)
      this->fScaler[i]->Ratio(*(innumer->fScaler[i]),*(indenom->fScaler[i]));
  }
}

/**
 * Scale the photon detector
 * @param factor Scale factor
 */
void QwComptonPhotonDetector::Scale(Double_t factor)
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].Scale(factor);
  //for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
  //  fMultiTDC_Channel[i].Scale(factor);
  //for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
  //  fMultiQDC_Channel[i].Scale(factor);
  for (size_t i = 0; i < fScaler.size(); i++)
    this->fScaler[i]->Scale(factor);
}

/**
 * Compare two QwComptonPhotonDetector objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwComptonPhotonDetector::Compare(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (value);
    if (input->fSamplingADC.size() != fSamplingADC.size()) {
      result = kFALSE;
    } else {
      if (input->fMultiTDC_Channel.size() != fMultiTDC_Channel.size()) {
        result = kFALSE;
      } else {
        if (input->fMultiQDC_Channel.size() != fMultiQDC_Channel.size()) {
          result = kFALSE;
        }
      }
    }
  }
  return result;
}

/**
 * Compare two QwComptonPhotonDetector ADC objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwComptonPhotonDetector::CompareADC(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (value);
    if (input->fSamplingADC.size() != fSamplingADC.size()) {
      result = kFALSE;
    }
  }
  return result;
}

/**
 * Compare two QwComptonPhotonDetector TDC objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwComptonPhotonDetector::CompareTDC(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (value);
    if (input->fMultiTDC_Channel.size() != fMultiTDC_Channel.size()) {
      result = kFALSE;
    }
  }
  return result;
}

/**
 * Compare two QwComptonPhotonDetector QDC objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwComptonPhotonDetector::CompareQDC(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (value);
    if (input->fMultiQDC_Channel.size() != fMultiQDC_Channel.size()) {
      result = kFALSE;
    }
  }
  return result;
}

/**
 * Compare two QwComptonPhotonDetector scaler objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwComptonPhotonDetector::CompareScaler(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (value);
    if (input->fScaler.size() != fScaler.size()) {
      result = kFALSE;
    }
  }
  return result;
}


/**
 * Construct the histograms
 * @param folder Folder in which the histograms will be created
 * @param prefix Prefix with information about the type of histogram
 */
void  QwComptonPhotonDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].ConstructHistograms(folder, prefix);
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].ConstructHistograms(folder,prefix);
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].ConstructHistograms(folder,prefix);
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i]->ConstructHistograms(folder,prefix);
}

/**
 * Fill the histograms with data
 */
void  QwComptonPhotonDetector::FillHistograms()
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].FillHistograms();
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].FillHistograms();
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].FillHistograms();
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i]->FillHistograms();
}

/**
 * Delete the histograms
 */
void  QwComptonPhotonDetector::DeleteHistograms()
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].DeleteHistograms();
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].DeleteHistograms();
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].DeleteHistograms();
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i]->DeleteHistograms();
}

/**
 * Construct the tree
 * @param folder Folder in which the tree will be created
 * @param prefix Prefix with information about the type of histogram
 */
void  QwComptonPhotonDetector::ConstructTree(TDirectory *folder, TString &prefix)
{
  folder->cd();
  fTree = new TTree("ComptonPhoton", "Compton Photon Detector");
  fTree->Branch("nevents",&fTree_fNEvents,"nevents/I");
}

/**
 * Delete the tree
 */
void  QwComptonPhotonDetector::DeleteTree()
{
  delete fTree;
}

/**
 * Fill the tree with data
 */
void  QwComptonPhotonDetector::FillTree()
{
  for (size_t i = 0; i < fSamplingADC.size(); i++) {
    fTree_fNEvents = fSamplingADC[i].GetNumberOfEvents();
    fTree->Fill();
  }
}

/**
 * Construct the tree
 * @param tree Pointer to the tree in which the branches will be created
 * @param prefix Prefix with information about the type of histogram
 * @param values
 */
void QwComptonPhotonDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].ConstructBranchAndVector(tree, prefix, values);
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].ConstructBranchAndVector(tree, prefix, values);
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].ConstructBranchAndVector(tree, prefix, values);
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i]->ConstructBranchAndVector(tree, prefix, values);
}

/**
 * Fill the tree with data
 * @param values
 */
void QwComptonPhotonDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].FillTreeVector(values);
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].FillTreeVector(values);
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].FillTreeVector(values);
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i]->FillTreeVector(values);
}

/**
 * Print the value for the subcomponents
 */
void  QwComptonPhotonDetector::PrintValue() const
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].PrintValue();
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++)
    fMultiTDC_Channel[i].PrintValue();
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++)
    fMultiQDC_Channel[i].PrintValue();
}

/**
 * Print some debugging output for the subcomponents
 */
void  QwComptonPhotonDetector::PrintInfo() const
{
  VQwSubsystemParity::PrintInfo();

  QwOut << " there are " << fSamplingADC.size() << " sampling ADCs" << QwLog::endl;
  QwOut << " there are " << fMultiTDC_Channel.size() << " integrating TDCs" << QwLog::endl;
  QwOut << " there are " << fMultiQDC_Channel.size() << " integrating ADCs" << QwLog::endl;

  for (size_t i = 0; i < fSamplingADC.size(); i++) {
    QwOut << " sampling ADC " << i << ":";
    fSamplingADC[i].PrintInfo();
  }
  for (size_t i = 0; i < fMultiTDC_Channel.size(); i++) {
    QwOut << " integrating TDC " << i << ":";
    fMultiTDC_Channel[i].PrintInfo();
  }
  for (size_t i = 0; i < fMultiQDC_Channel.size(); i++) {
    QwOut << " integrating ADC " << i << ":";
    fMultiQDC_Channel[i].PrintInfo();
  }
}

/**
 * Make a copy of this photon detector, including all its subcomponents
 * @param source Original version
 */
void  QwComptonPhotonDetector::Copy(VQwSubsystem *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      VQwSubsystem::Copy(source);
      QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(source);

      this->fSamplingADC.resize(input->fSamplingADC.size());
      for (size_t i = 0; i < this->fSamplingADC.size(); i++)
        this->fSamplingADC[i].Copy(&(input->fSamplingADC[i]));

      //this->fMultiTDC_Channel.resize(input->fMultiTDC_Channel.size());
      //for (size_t i = 0; i < this->fMultiTDC_Channel.size(); i++)
      //  this->fMultiTDC_Channel[i].Copy(&(input->fMultiTDC_Channel[i]));

      //this->fMultiQDC_Channel.resize(input->fMultiQDC_Channel.size());
      //for (size_t i = 0; i < this->fMultiQDC_Channel.size(); i++)
      //  this->fMultiQDC_Channel[i].Copy(&(input->fMultiQDC_Channel[i]));

      this->fScaler.resize(input->fScaler.size());
      for (size_t i = 0; i < this->fScaler.size(); i++) {
        VQwScaler_Channel* scaler = 0;
        if ((scaler = dynamic_cast<QwSIS3801D24_Channel*>(input->fScaler[i]))) {
          this->fScaler[i] = new QwSIS3801D24_Channel();
        } else if ((scaler = dynamic_cast<QwSIS3801D32_Channel*>(input->fScaler[i]))) {
          this->fScaler[i] = new QwSIS3801D32_Channel();
        }
        this->fScaler[i]->Copy(scaler);
      }

    } else {
      TString loc = "Standard exception from QwComptonPhotonDetector::Copy = "
             + source->GetSubsystemName() + " "
             + this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }

  } catch (std::exception& e) {
    QwError << e.what() << QwLog::endl;
  }
}


/**
 * Make a copy of this photon detector
 * @return Copy of this photon detector
 */
VQwSubsystem*  QwComptonPhotonDetector::Copy()
{
  QwComptonPhotonDetector* copy = new QwComptonPhotonDetector(this->GetSubsystemName() + " Copy");
  copy->Copy(this);
  return copy;
}

/**
 * Get the SIS3320 channel for this photon detector
 * @param name Name of the SIS3320 channel
 * @return Pointer to the SIS3320 channel
 */
MQwSIS3320_Channel* QwComptonPhotonDetector::GetSIS3320Channel(const TString name)
{
  if (! fSamplingADC.empty()) {
    for (std::vector<MQwSIS3320_Channel>::iterator adc = fSamplingADC.begin(); adc != fSamplingADC.end(); ++adc) {
      if (adc->GetElementName() == name) {
        return &(*adc);
      }
    }
  }
  return 0;
}
