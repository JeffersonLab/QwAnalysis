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
#include "QwHistogramHelper.h"
#include <stdexcept>


//*****************************************************************
Int_t QwComptonPhotonDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum, channum;

  QwParameterFile mapstr(mapfile.Data());  // Open the file
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing whitespace (spaces or tabs).
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      // This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
        RegisterROCNumber(value,0);
      } else if (varname == "bank") {
        RegisterSubbank(value);
      }
    } else {
      //  Break this line into tokens to process it.
      modtype   = mapstr.GetNextToken(", \t").c_str();
      modnum    = (atol(mapstr.GetNextToken(", \t").c_str()));
      channum   = (atol(mapstr.GetNextToken(", \t").c_str()));
      dettype   = mapstr.GetNextToken(", \t").c_str();
      name      = mapstr.GetNextToken(", \t").c_str();

      //  Push a new record into the element array
      if (modtype == "SIS3320") {
        if (modnum >= (Int_t) fSamplingADC.size())
          fSamplingADC.push_back(MQwSIS3320_Channel());
        fSamplingADC.at(modnum).InitializeChannel(channum, name);
        fSamplingADC.at(modnum).SetNumberOfAccumulators(6);
      } else if (modtype == "V775") {
        // not yet implemented
      } else if (modtype == "V792") {
        // not yet implemented
      } // end of switch (modtype)

    } // end of if for token line
  } // end of while over parameter file

  return 0;
};

//*****************************************************************
Int_t QwComptonPhotonDetector::LoadEventCuts(TString & filename)
{
  return 0;
};

//*****************************************************************
Int_t QwComptonPhotonDetector::LoadInputParameters(TString pedestalfile)
{
  TString varname;
  Double_t varped;
  Double_t varcal;
  Bool_t notfound = kTRUE;

  // Open the file
  QwParameterFile mapstr(pedestalfile.Data());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    else {
      varname = mapstr.GetNextToken(", \t").c_str();  // name of the channel
      varname.ToLower();
      varname.Remove(TString::kBoth,' ');
      varped = (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
      varcal = (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
    }

    for (size_t i = 0; i < fSamplingADC.size() && notfound; i++) {
      TString localname = fSamplingADC[i].GetElementName();
      localname.ToLower();
      if (localname == varname) {
        fSamplingADC[i].SetPedestal(varped);
        fSamplingADC[i].SetCalibrationFactor(varcal);
        notfound = kFALSE;
      }

    } // end of loop over all sampling ADC channels

  } // end of loop reading all lines of the pedestal file

  return 0;
}

//*****************************************************************
void QwComptonPhotonDetector::RandomizeEventData(int helicity)
{
  // Randomize all MQwSIS3320 buffers
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].RandomizeEventData(helicity);

  // Randomize all MQwV775TDC buffers
  for (size_t i = 0; i < fIntegratingTDC.size(); i++)
    fIntegratingTDC[i].RandomizeEventData(helicity);

  // Randomize all MQwV792ADC buffers
  for (size_t i = 0; i < fIntegratingADC.size(); i++)
    fIntegratingADC[i].RandomizeEventData(helicity);
}

//*****************************************************************
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

//*****************************************************************
Int_t QwComptonPhotonDetector::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  UInt_t words_read = 0;

  // Get the subbank index (or -1 when no match)
  Int_t index = GetSubbankIndex(roc_id, bank_id);
  if (index >= 0 && num_words > 0) {

    //  We want to process this ROC.  Begin looping through the data.
    for (size_t i = 0; i < fSamplingADC.size(); i++) {
      words_read += fSamplingADC[i].ProcessEvBuffer(&(buffer[words_read]), num_words-words_read);
    }
    for (size_t i = 0; i < fIntegratingTDC.size(); i++) {
      //words_read += fIntegratingTDC[i].ProcessEvBuffer(&(buffer[words_read]), num_words-words_read);
    }
    for (size_t i = 0; i < fIntegratingADC.size(); i++) {
      //words_read += fIntegratingADC[i].ProcessEvBuffer(&(buffer[words_read]), num_words-words_read);
    }
    if (num_words != words_read) {
      std::cerr << "QwComptonPhotonDetector::ProcessEvBuffer:  There were "
		<< num_words-words_read
		<< " leftover words after decoding everything we recognize."
		<< std::endl;
    }
  }

  return words_read;
};

//*****************************************************************
Bool_t QwComptonPhotonDetector::SingleEventCuts()
{
  std::cout << "QwComptonPhotonDetector::SingleEventCuts()" << std::endl;
  return IsGoodEvent();
};

//*****************************************************************
void  QwComptonPhotonDetector::ProcessEvent()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].ProcessEvent();
  for(size_t i = 0; i < fIntegratingTDC.size(); i++)
    fIntegratingTDC[i].ProcessEvent();
  for(size_t i = 0; i < fIntegratingADC.size(); i++)
    fIntegratingADC[i].ProcessEvent();

  return;
};

//*****************************************************************
Int_t QwComptonPhotonDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
};

//*****************************************************************
Bool_t QwComptonPhotonDetector::IsGoodEvent()
{
  Bool_t test = kTRUE;

  for(size_t i=0;i<fSamplingADC.size();i++)
    test &= fSamplingADC[i].IsGoodEvent();
//  for(size_t i=0;i<fIntegratingTDC.size();i++)
//    test &= fIntegratingTDC[i].IsGoodEvent();
//  for(size_t i=0;i<fIntegratingADC.size();i++)
//    test &= fIntegratingADC[i].IsGoodEvent();

  if (!test) std::cerr << "This is not a good event!" << std::endl;
  return test;
}

//*****************************************************************
void QwComptonPhotonDetector::ClearEventData()
{
  for(size_t i=0;i<fSamplingADC.size();i++)
    fSamplingADC[i].ClearEventData();
  for(size_t i=0;i<fIntegratingTDC.size();i++)
    fIntegratingTDC[i].ClearEventData();
  for(size_t i=0;i<fIntegratingADC.size();i++)
    fIntegratingADC[i].ClearEventData();

  return;
};

//*****************************************************************
VQwSubsystem&  QwComptonPhotonDetector::operator=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonPhotonDetector* input = (QwComptonPhotonDetector*) value;
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] = input->fSamplingADC[i];
    for (size_t i = 0; i < input->fIntegratingTDC.size();i++)
      this->fIntegratingTDC[i] = input->fIntegratingTDC[i];
    for (size_t i = 0; i < input->fIntegratingADC.size();i++)
      this->fIntegratingADC[i] = input->fIntegratingADC[i];
  }
  return *this;
};

VQwSubsystem&  QwComptonPhotonDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonPhotonDetector* input = (QwComptonPhotonDetector*) value;
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] += input->fSamplingADC[i];
    for (size_t i = 0; i < input->fIntegratingTDC.size(); i++)
      this->fIntegratingTDC[i] += input->fIntegratingTDC[i];
    for (size_t i = 0; i < input->fIntegratingADC.size(); i++)
      this->fIntegratingADC[i] += input->fIntegratingADC[i];
  }
  return *this;
};

VQwSubsystem&  QwComptonPhotonDetector::operator-=  (VQwSubsystem *value)
{

  if (Compare(value)) {
    QwComptonPhotonDetector* input= (QwComptonPhotonDetector*) value;
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] -= input->fSamplingADC[i];
    for (size_t i = 0; i < input->fIntegratingTDC.size(); i++)
      this->fIntegratingTDC[i] -= input->fIntegratingTDC[i];
    for (size_t i = 0; i < input->fIntegratingADC.size(); i++)
      this->fIntegratingADC[i] -= input->fIntegratingADC[i];
  }
  return *this;
};

void  QwComptonPhotonDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this =  value1;
    *this += value2;
  }
};

void  QwComptonPhotonDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this =  value1;
    *this -= value2;
  }
};

/**
 * Determine the ratio of two photon detectors
 * @param numer
 * @param denom
 */
void QwComptonPhotonDetector::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwComptonPhotonDetector* innumer = (QwComptonPhotonDetector*) numer;
    QwComptonPhotonDetector* indenom = (QwComptonPhotonDetector*) denom;
    for (size_t i = 0; i < innumer->fSamplingADC.size(); i++)
      this->fSamplingADC[i].Ratio(innumer->fSamplingADC[i], indenom->fSamplingADC[i]);
    for (size_t i = 0; i < innumer->fIntegratingTDC.size(); i++)
      this->fIntegratingTDC[i].Ratio(innumer->fIntegratingTDC[i], indenom->fIntegratingTDC[i]);
    for (size_t i = 0; i < innumer->fIntegratingADC.size(); i++)
      this->fIntegratingADC[i].Ratio(innumer->fIntegratingADC[i], indenom->fIntegratingADC[i]);
  }
  return;
};

/**
 * Scale the photon detector
 * @param factor
 */
void QwComptonPhotonDetector::Scale(Double_t factor)
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].Scale(factor);
  for (size_t i = 0; i < fIntegratingTDC.size(); i++)
    fIntegratingTDC[i].Scale(factor);
  for (size_t i = 0; i < fIntegratingADC.size(); i++)
    fIntegratingADC[i].Scale(factor);

  return;
};

/**
 * Compare two QwComptonPhotonDetector objects
 * @param value Object to compare with
 * @return kTRUE if the object is equal
 */
Bool_t QwComptonPhotonDetector::Compare(VQwSubsystem *value)
{
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;

  } else {
    QwComptonPhotonDetector* input = (QwComptonPhotonDetector*) value;
    if (input->fSamplingADC.size() != fSamplingADC.size()) {
      result = kFALSE;
    } else {
      if (input->fIntegratingTDC.size() != fIntegratingTDC.size()) {
        result = kFALSE;
      } else {
        if (input->fIntegratingADC.size() != fIntegratingADC.size()) {
          result = kFALSE;
        }
      }
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
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].ConstructHistograms(folder, prefix);
  for(size_t i = 0; i < fIntegratingTDC.size(); i++)
      fIntegratingTDC[i].ConstructHistograms(folder,prefix);
  for(size_t i = 0; i < fIntegratingADC.size(); i++)
      fIntegratingADC[i].ConstructHistograms(folder,prefix);

  return;
};

/**
 * Delete the histograms
 */
void  QwComptonPhotonDetector::DeleteHistograms()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].DeleteHistograms();
  for(size_t i = 0; i < fIntegratingTDC.size(); i++)
      fIntegratingTDC[i].DeleteHistograms();
  for(size_t i = 0; i < fIntegratingADC.size(); i++)
      fIntegratingADC[i].DeleteHistograms();

  return;
};

/**
 * Fill the histograms with data
 */
void  QwComptonPhotonDetector::FillHistograms()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].FillHistograms();
  for(size_t i = 0; i < fIntegratingTDC.size(); i++)
      fIntegratingTDC[i].FillHistograms();
  for(size_t i = 0; i < fIntegratingADC.size(); i++)
      fIntegratingADC[i].FillHistograms();

  return;
};

/**
 * Construct the tree
 * @param tree Pointer to the tree in which the branches will be created
 * @param prefix Prefix with information about the type of histogram
 * @param values
 */
void QwComptonPhotonDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fIntegratingTDC.size(); i++)
      fIntegratingTDC[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fIntegratingADC.size(); i++)
      fIntegratingADC[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

/**
 * Fill the tree with data
 * @param values
 */
void QwComptonPhotonDetector::FillTreeVector(std::vector<Double_t> &values)
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].FillTreeVector(values);
  for(size_t i = 0; i < fIntegratingTDC.size(); i++)
      fIntegratingTDC[i].FillTreeVector(values);
  for(size_t i = 0; i < fIntegratingADC.size(); i++)
      fIntegratingADC[i].FillTreeVector(values);

  return;
};

/**
 * Print some debugging output for the subcomponents
 */
void  QwComptonPhotonDetector::Print()
{
  VQwSubsystemParity::Print();

  std::cout << "there are " << fSamplingADC.size() << " sampling ADCs" << std::endl;
  std::cout << "there are " << fIntegratingTDC.size() << " integrating TDCs" << std::endl;
  std::cout << "there are " << fIntegratingADC.size() << " integrating ADCs" << std::endl;

  return;
}

/**
 * Make a copy of this photon detector, including all its subcomponents
 * @param source Original version
 */
void  QwComptonPhotonDetector::Copy(VQwSubsystem *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      QwComptonPhotonDetector* input = ((QwComptonPhotonDetector*) source);
      this->fSamplingADC.resize(input->fSamplingADC.size());
      for (size_t i = 0; i < this->fSamplingADC.size(); i++)
        this->fSamplingADC[i].Copy(&(input->fSamplingADC[i]));
      this->fIntegratingTDC.resize(input->fIntegratingTDC.size());
      for (size_t i = 0; i < this->fIntegratingTDC.size(); i++)
        this->fIntegratingTDC[i].Copy(&(input->fIntegratingTDC[i]));
      this->fIntegratingADC.resize(input->fIntegratingADC.size());
      for (size_t i = 0; i < this->fIntegratingADC.size(); i++)
        this->fIntegratingADC[i].Copy(&(input->fIntegratingADC[i]));

    } else {
      TString loc = "Standard exception from QwComptonPhotonDetector::Copy = "
             + source->GetSubsystemName() + " "
             + this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return;
}


/**
 * Make a copy of this photon detector
 * @return Copy of this photon detector
 */
VQwSubsystem*  QwComptonPhotonDetector::Copy()
{
  QwComptonPhotonDetector* TheCopy = new QwComptonPhotonDetector(this->GetSubsystemName() + " Copy");
  TheCopy->Copy(this);
  return TheCopy;
}

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
};
