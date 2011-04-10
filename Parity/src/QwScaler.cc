//
// C++ Implementation: QwScaler
//
// Description: used to collect and process the information from the scaler channel

#include "QwScaler.h"

// Qweak headers
#include "QwParameterFile.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwScaler);


void QwScaler::DefineOptions(QwOptions &options)
{
  // Define command line options
}

void QwScaler::ProcessOptions(QwOptions &options)
{
  // Handle command line options
}


/**
 * Constructor
 */
QwScaler::QwScaler(TString region_tmp)
: VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp)
{
  // Nothing, really
}

/**
 * Destructor
 *
 * Delete histograms and clean up scaler channel objects
 */
QwScaler::~QwScaler()
{
  // Delete histograms
  DeleteHistograms();

  // Delete scalers
  for (size_t i = 0; i < fScaler.size(); i++) {
    delete fScaler.at(i);
  }
  fScaler.clear();
}


/**
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 */
Int_t QwScaler::LoadChannelMap(TString mapfile)
{
  Int_t current_roc_id = -1;    // current ROC id
  Int_t current_bank_id = -1;   // current bank id

  // Normalization channel (register default token "1")
  const TString default_norm_chan = "1";
  fNames[default_norm_chan] = -1;
  Name_to_Scaler_Map_t::iterator current_norm_chan = fNames.find(default_norm_chan);
  // Map with normalizations
  std::vector< Name_to_Scaler_Map_t::iterator > normalization;

  // Number of words accounted for
  Int_t wordsofar = 0;
  
  // Open the file
  QwParameterFile mapstr(mapfile.Data());
  fDetectorMapsNames.push_back(mapstr.GetParamFilename());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();       // Remove everything after a comment character.
    mapstr.TrimWhitespace();    // Get rid of leading and trailing whitespace
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      // This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
        current_roc_id = value;
        RegisterROCNumber(current_roc_id,0);
      } else if (varname == "bank") {
        current_bank_id = value;
        RegisterSubbank(current_bank_id);
      } else if (varname == "norm") {
        if (fNames.count(varvalue) == 0) {
          // assign a temporarily pointer
          fNames[varvalue] = -1;
        }
        current_norm_chan = fNames.find(varvalue);
      }

    } else {

      //  Break this line into tokens to process it.
      TString modtype = mapstr.GetNextToken().c_str();
      UInt_t  modnum  = QwParameterFile::GetUInt(mapstr.GetNextToken().c_str());
      UInt_t  channum = QwParameterFile::GetUInt(mapstr.GetNextToken().c_str());
      TString dettype = mapstr.GetNextToken().c_str();
      TString keyword = mapstr.GetNextToken().c_str();
      modtype.ToUpper();
      dettype.ToUpper();

      if (modtype == "SIS3801D24" || modtype == "SIS3801D32") {
        wordsofar += 1;
      } else if (modtype == "SIS7200") {
        wordsofar += 1;
      } else {
        QwError << "Unrecognized module type " << modtype << QwLog::endl;
        continue;
      }

      // Register data channel type
      Int_t subbank = GetSubbankIndex(current_roc_id,current_bank_id);
      if (modnum >= fSubbank_Mapping[subbank].size())
        fSubbank_Mapping[subbank].resize(modnum+1);
      if (channum >= fSubbank_Mapping[subbank].at(modnum).size())
        fSubbank_Mapping[subbank].at(modnum).resize(channum+1,-1);
      // Add scaler channel
      if (fSubbank_Mapping[subbank].at(modnum).at(channum) < 0) {
        QwMessage << "Registering " << modtype << " " << keyword
                  << std::hex
                  << " in ROC 0x" << current_roc_id << ", bank 0x" << current_bank_id
                  << std::dec
                  << " at mod " << modnum << ", chan " << channum
                  << QwLog::endl;
        size_t index = fScaler.size();
        VQwScaler_Channel* scaler = 0;
        if (modtype == "SIS3801D24")
          scaler = new QwSIS3801D24_Channel(keyword);
        else if (modtype == "SIS3801D32")
          scaler = new QwSIS3801D32_Channel(keyword);
        else {
          QwError << "Unrecognized module type " << modtype << QwLog::endl;
          continue;
        }
        // Register keyword to scaler channel
        fNames[keyword] = index;
        // Store scaler
        fScaler.push_back(scaler);
        fModuleChannel_Mapping[std::pair<Int_t,Int_t>(modnum,channum)] = index;
        fSubbank_Mapping[subbank].at(modnum).at(channum) = index;
        // Store current normalization
        normalization.push_back(current_norm_chan);
      }
    }
  }

  // Check for names without channels after all channels were read
  for (Name_to_Scaler_Map_t::iterator iter = fNames.begin(); iter != fNames.end(); iter++) {
    if (iter->second == -1 && iter->first != default_norm_chan) {
      QwError << "Normalization channel " << iter->first << " not found!" << QwLog::endl;
    }
  }

  // Fill normalization structure
  fNorm.resize(fScaler.size());
  for (size_t i = 0; i < fScaler.size(); i++) {
    Int_t index = normalization.at(i)->second;
    if (index < 0) fNorm.at(i) = 0;
    else fNorm.at(i) = fScaler.at(index);
  }

  return 0;
}

Int_t QwScaler::LoadInputParameters(TString)
{
  return 0;
}

/**
 * Clear the event data in this subsystem
 */
void QwScaler::ClearEventData()
{
  // Clear all scaler channels
  for (size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->ClearEventData();
  }
  // Reset good event count
  fGoodEventCount = 0;
}

/**
 * Make a copy of this subsystem, including all its subcomponents
 * @param source Original version
 */
void  QwScaler::Copy(VQwSubsystem *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      VQwSubsystem::Copy(source);
      QwScaler* input = dynamic_cast<QwScaler*>(source);

      fScaler.resize(input->fScaler.size());
      for (size_t i = 0; i < fScaler.size(); i++) {
        VQwScaler_Channel* scaler = 0;
        if ((scaler = dynamic_cast<QwSIS3801D24_Channel*>(input->fScaler.at(i)))) {
          fScaler.at(i) = new QwSIS3801D24_Channel();
        } else if ((scaler = dynamic_cast<QwSIS3801D32_Channel*>(input->fScaler.at(i)))) {
          fScaler.at(i) = new QwSIS3801D32_Channel();
        }
        fScaler.at(i)->Copy(scaler);
      }

    } else {
      TString loc = "Standard exception from QwScaler::Copy = "
             + source->GetSubsystemName() + " "
             + this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }

  } catch (std::exception& e) {
    QwError << e.what() << QwLog::endl;
  }
}


/**
 * Make a copy of this subsystem
 * @return Copy of this subsystem
 */
VQwSubsystem*  QwScaler::Copy()
{
  QwScaler* copy = new QwScaler(this->GetSubsystemName() + " Copy");
  copy->Copy(this);
  return copy;
}


/**
 * Process the configuration buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
Int_t QwScaler::ProcessConfigurationBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
}

/**
 * Process the event buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
Int_t QwScaler::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  // TODO fix :-)

  UInt_t words_read = 0;

  // Get the subbank index (or -1 when no match)
  Int_t subbank = GetSubbankIndex(roc_id, bank_id);

  if (subbank >= 0 && num_words > 0) {

    // Read header word
    //UInt_t num_events = buffer[words_read];
    words_read++;
    // TODO Multiscaler functionality

    // Read scalers
    for (size_t modnum = 0; modnum < fSubbank_Mapping[subbank].size(); modnum++) {
      for (size_t channum = 0; channum < fSubbank_Mapping[subbank].at(modnum).size(); channum++) {
        Int_t index = fSubbank_Mapping[subbank].at(modnum).at(channum);
        if (index >= 0) {
          words_read += fScaler[index]->ProcessEvBuffer(&(buffer[words_read]), num_words - words_read);
        }
      }
    }
    words_read = num_words;

    // Check for leftover words
    if (num_words != words_read) {
      QwError << "QwScaler: There were "
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

void QwScaler::ProcessEvent()
{
  // Process the event
  for (size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->ProcessEvent();
  }

  // Normalization
  for (size_t i = 0; i < fScaler.size(); i++) {
    if (fNorm.at(i)) {
      fScaler.at(i)->Normalize(*(fNorm.at(i)));
    }
  }
}

void QwScaler::ConstructHistograms(TDirectory* folder, TString& prefix)
{
  for(size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->ConstructHistograms(folder, prefix);
  }
}

void QwScaler::FillHistograms()
{
  for(size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->FillHistograms();
  }
}

void QwScaler::DeleteHistograms()
{
  for(size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->DeleteHistograms();
  }
}

void QwScaler::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->ConstructBranchAndVector(tree, prefix, values);
  }
}

void QwScaler::FillTreeVector(std::vector<Double_t> &values) const
{
  for(size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->FillTreeVector(values);
  }
}

/**
 * Assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem& QwScaler::operator=(VQwSubsystem *value)
{
  if (Compare(value)) {
    VQwSubsystem::operator=(value);
    QwScaler* input = dynamic_cast<QwScaler*>(value);
    for (size_t i = 0; i < fScaler.size(); i++) {
      *(fScaler.at(i)) = *(input->fScaler.at(i));
    }
  }
  return *this; 
}

/**
 * Addition-assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem& QwScaler::operator+=(VQwSubsystem *value)
{
  if (Compare(value)) {
    QwScaler* input = dynamic_cast<QwScaler*>(value);
    for (size_t i = 0; i < fScaler.size(); i++) {
      *(fScaler.at(i)) += *(input->fScaler.at(i));
    }
  }
  return *this;
}

/**
 * Subtraction-assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem& QwScaler::operator-=(VQwSubsystem *value)
{
  if (Compare(value)) {
    QwScaler* input = dynamic_cast<QwScaler *> (value);
    for (size_t i = 0; i < fScaler.size(); i++) {
      *(fScaler.at(i)) -= *(input->fScaler.at(i));
    }
  }
  return *this;
}

/**
 * Summation
 * @param value1 First value
 * @param value2 Second value
 */
void QwScaler::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
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
void QwScaler::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
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
void QwScaler::Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwScaler* innumer = dynamic_cast<QwScaler*>(numer);
    QwScaler* indenom = dynamic_cast<QwScaler*>(denom);
    for (size_t i = 0; i < fScaler.size(); i++) {
      fScaler.at(i)->Ratio(*(innumer->fScaler.at(i)), *(indenom->fScaler.at(i)));
    }
  }
}

/**
 * Scale the photon detector
 * @param factor Scale factor
 */
void QwScaler::Scale(Double_t factor)
{
  for (size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->Scale(factor);
  }
}

/**
 * Accumulate the running sum
 */
void QwScaler::AccumulateRunningSum(VQwSubsystem* value)
{
  if (Compare(value)) {
    fGoodEventCount++;
    *this  += value;
  }
}

/**
 * Normalize the running sum
 */
void QwScaler::CalculateRunningAverage()
{
  if (fGoodEventCount <= 0) {
    Scale(0);
  } else {
    Scale(1.0/fGoodEventCount);
  }
}

Int_t QwScaler::LoadEventCuts(TString filename)
{
  return 0;
}

Bool_t QwScaler::ApplySingleEventCuts()
{
  return true;
}

Int_t QwScaler::GetEventcutErrorCounters()
{
  return 0;
}

UInt_t QwScaler::GetEventcutErrorFlag()
{
  return 0;
}

Int_t QwScaler::GetChannelIndex(TString channelName, UInt_t module_number)
{
  return 0;
}


/**
 * Compare two scaler objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwScaler::Compare(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwScaler* input = dynamic_cast<QwScaler*> (value);
    if (input->fScaler.size() != fScaler.size()) {
      result = kFALSE;
    }
  }
  return result;
}

/**
 * Print some debugging output for the subcomponents
 */
void QwScaler::PrintInfo() const
{
  VQwSubsystemParity::PrintInfo();

  QwOut << " there are " << fScaler.size() << " scaler channels" << QwLog::endl;

  for (size_t i = 0; i < fScaler.size(); i++) {
    QwOut << " scaler " << i << ": ";
    fScaler.at(i)->PrintInfo();
  }
}

/**
 * Print the value for the subcomponents
 */
void QwScaler::PrintValue() const
{
  for(size_t i = 0; i < fScaler.size(); i++) {
    fScaler.at(i)->PrintValue();
  }
}
