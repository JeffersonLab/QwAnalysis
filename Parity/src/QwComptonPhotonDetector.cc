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
#include "QwHistogramHelper.h"


// Register this subsystem with the factory
QwSubsystemFactory<QwComptonPhotonDetector>
  theComptonPhotonDetectorFactory("QwComptonPhotonDetector");


/**
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 */

void QwComptonPhotonDetector::ProcessOptions(QwOptions &options){
}; //Handle command line options

Int_t QwComptonPhotonDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  UInt_t modnum, channum;

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
        if (modnum >= fSamplingADC_Mapping.size())
          fSamplingADC_Mapping.resize(modnum+1);
        if (channum >= fSamplingADC_Mapping.at(modnum).size())
          fSamplingADC_Mapping.at(modnum).resize(channum+1,-1);
        if (fSamplingADC_Mapping.at(modnum).at(channum) < 0) {
          UInt_t index = fSamplingADC.size();
          fSamplingADC_Mapping.at(modnum).at(channum) = index;
          fSamplingADC.push_back(MQwSIS3320_Channel());
          fSamplingADC.at(index).InitializeChannel(channum, name);
          fSamplingADC.at(index).SetNumberOfAccumulators(6);
        }
      } else if (modtype == "V775") {
        // not yet implemented
      } else if (modtype == "V792") {
        // not yet implemented
      } // end of switch (modtype)

    } // end of if for token line
  } // end of while over parameter file

  return 0;
};

/**
 * Load the event cuts
 * @param filename Event cuts file
 * @return Zero if successful
 */
Int_t QwComptonPhotonDetector::LoadEventCuts(TString & filename)
{
  return 0;
};

/**
 * Load the input parameters
 * @param pedestalfile Input parameters file
 * @return Zero if successful
 */
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
  //for (size_t i = 0; i < fIntegratingTDC.size(); i++)
  //  fIntegratingTDC[i].RandomizeEventData(helicity);

  // Randomize all MQwV792ADC buffers
  //for (size_t i = 0; i < fIntegratingADC.size(); i++)
  //  fIntegratingADC[i].RandomizeEventData(helicity);
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
      QwError << "QwComptonPhotonDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
  }

  return words_read;
};


/**
 * Process the single event cuts
 * @return
 */
Bool_t QwComptonPhotonDetector::SingleEventCuts()
{
  QwDebug << "QwComptonPhotonDetector::SingleEventCuts()" << QwLog::endl;
  return IsGoodEvent();
};


/**
 * Process this event
 */
void  QwComptonPhotonDetector::ProcessEvent()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].ProcessEvent();
  //for(size_t i = 0; i < fIntegratingTDC.size(); i++)
  //  fIntegratingTDC[i].ProcessEvent();
  //for(size_t i = 0; i < fIntegratingADC.size(); i++)
  //  fIntegratingADC[i].ProcessEvent();

  return;
};


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
};


/**
 * Check whether this is a good event
 * @return True if the event is good
 */
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


/**
 * Clear the event data in this subsystem
 */
void QwComptonPhotonDetector::ClearEventData()
{
  for(size_t i=0;i<fSamplingADC.size();i++)
    fSamplingADC[i].ClearEventData();
  //for(size_t i=0;i<fIntegratingTDC.size();i++)
  //  fIntegratingTDC[i].ClearEventData();
  //for(size_t i=0;i<fIntegratingADC.size();i++)
  //  fIntegratingADC[i].ClearEventData();

  return;
};


/**
 * Assignment operator
 * @param value Right-hand side
 * @return Left-hand side
 */
VQwSubsystem&  QwComptonPhotonDetector::operator=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*>(value);
    for (size_t i = 0; i < input->fSamplingADC.size(); i++)
      this->fSamplingADC[i] = input->fSamplingADC[i];
    for (size_t i = 0; i < input->fIntegratingTDC.size();i++)
      this->fIntegratingTDC[i] = input->fIntegratingTDC[i];
    for (size_t i = 0; i < input->fIntegratingADC.size();i++)
      this->fIntegratingADC[i] = input->fIntegratingADC[i];
  }
  return *this;
};

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
    //for (size_t i = 0; i < input->fIntegratingTDC.size(); i++)
    //  this->fIntegratingTDC[i] += input->fIntegratingTDC[i];
    //for (size_t i = 0; i < input->fIntegratingADC.size(); i++)
    //  this->fIntegratingADC[i] += input->fIntegratingADC[i];
  }
  return *this;
};

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
    //for (size_t i = 0; i < input->fIntegratingTDC.size(); i++)
    //  this->fIntegratingTDC[i] -= input->fIntegratingTDC[i];
    //for (size_t i = 0; i < input->fIntegratingADC.size(); i++)
    //  this->fIntegratingADC[i] -= input->fIntegratingADC[i];
  }
  return *this;
};

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
};

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
};

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
    //for (size_t i = 0; i < innumer->fIntegratingTDC.size(); i++)
    //  this->fIntegratingTDC[i].Ratio(innumer->fIntegratingTDC[i], indenom->fIntegratingTDC[i]);
    //for (size_t i = 0; i < innumer->fIntegratingADC.size(); i++)
    //  this->fIntegratingADC[i].Ratio(innumer->fIntegratingADC[i], indenom->fIntegratingADC[i]);
  }
  return;
};

/**
 * Scale the photon detector
 * @param factor Scale factor
 */
void QwComptonPhotonDetector::Scale(Double_t factor)
{
  for (size_t i = 0; i < fSamplingADC.size(); i++)
    fSamplingADC[i].Scale(factor);
  //for (size_t i = 0; i < fIntegratingTDC.size(); i++)
  //  fIntegratingTDC[i].Scale(factor);
  //for (size_t i = 0; i < fIntegratingADC.size(); i++)
  //  fIntegratingADC[i].Scale(factor);

  return;
};

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
  //for(size_t i = 0; i < fIntegratingTDC.size(); i++)
  //    fIntegratingTDC[i].ConstructHistograms(folder,prefix);
  //for(size_t i = 0; i < fIntegratingADC.size(); i++)
  //    fIntegratingADC[i].ConstructHistograms(folder,prefix);

  return;
};

/**
 * Delete the histograms
 */
void  QwComptonPhotonDetector::DeleteHistograms()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].DeleteHistograms();
  //for(size_t i = 0; i < fIntegratingTDC.size(); i++)
  //    fIntegratingTDC[i].DeleteHistograms();
  //for(size_t i = 0; i < fIntegratingADC.size(); i++)
  //    fIntegratingADC[i].DeleteHistograms();

  return;
};

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
  return;
};

/**
 * Delete the tree
 */
void  QwComptonPhotonDetector::DeleteTree()
{
  delete fTree;
  return;
};

/**
 * Fill the tree with data
 */
void  QwComptonPhotonDetector::FillTree()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++) {
    fTree_fNEvents = fSamplingADC[i].GetNumberOfEvents();
    fTree->Fill();
  }

  return;
};

/**
 * Fill the histograms with data
 */
void  QwComptonPhotonDetector::FillHistograms()
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].FillHistograms();
  //for(size_t i = 0; i < fIntegratingTDC.size(); i++)
  //    fIntegratingTDC[i].FillHistograms();
  //for(size_t i = 0; i < fIntegratingADC.size(); i++)
  //    fIntegratingADC[i].FillHistograms();

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
  //for(size_t i = 0; i < fIntegratingTDC.size(); i++)
  //    fIntegratingTDC[i].ConstructBranchAndVector(tree, prefix, values);
  //for(size_t i = 0; i < fIntegratingADC.size(); i++)
  //    fIntegratingADC[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

/**
 * Fill the tree with data
 * @param values
 */
void QwComptonPhotonDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  for(size_t i = 0; i < fSamplingADC.size(); i++)
      fSamplingADC[i].FillTreeVector(values);
  //for(size_t i = 0; i < fIntegratingTDC.size(); i++)
  //    fIntegratingTDC[i].FillTreeVector(values);
  //for(size_t i = 0; i < fIntegratingADC.size(); i++)
  //    fIntegratingADC[i].FillTreeVector(values);

  return;
};

/**
 * Print the value for the subcomponents
 */
void  QwComptonPhotonDetector::PrintValue() const
{
  for (size_t i = 0; i < fSamplingADC.size(); i++) {
    fSamplingADC[i].PrintValue();
  }
  for (size_t i = 0; i < fIntegratingTDC.size(); i++) {
    //fIntegratingTDC[i].PrintValue();
  }
  for (size_t i = 0; i < fIntegratingADC.size(); i++) {
    //fIntegratingADC[i].PrintValue();
  }

  return;
}

/**
 * Print some debugging output for the subcomponents
 */
void  QwComptonPhotonDetector::PrintInfo() const
{
  VQwSubsystemParity::PrintInfo();

  QwOut << " there are " << fSamplingADC.size() << " sampling ADCs" << QwLog::endl;
  QwOut << " there are " << fIntegratingTDC.size() << " integrating TDCs" << QwLog::endl;
  QwOut << " there are " << fIntegratingADC.size() << " integrating ADCs" << QwLog::endl;

  for (size_t i = 0; i < fSamplingADC.size(); i++) {
    QwOut << " sampling ADC " << i << ":";
    fSamplingADC[i].PrintInfo();
  }
  for (size_t i = 0; i < fIntegratingTDC.size(); i++) {
    QwOut << " integrating TDC " << i << ":";
    //fIntegratingTDC[i].PrintInfo();
  }
  for (size_t i = 0; i < fIntegratingADC.size(); i++) {
    QwOut << " integrating ADC " << i << ":";
    //fIntegratingADC[i].PrintInfo();
  }

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
      VQwSubsystem::Copy(source);
      QwComptonPhotonDetector* input = dynamic_cast<QwComptonPhotonDetector*> (source);
      this->fSamplingADC.resize(input->fSamplingADC.size());
      for (size_t i = 0; i < this->fSamplingADC.size(); i++)
        this->fSamplingADC[i].Copy(&(input->fSamplingADC[i]));
      //this->fIntegratingTDC.resize(input->fIntegratingTDC.size());
      //for (size_t i = 0; i < this->fIntegratingTDC.size(); i++)
      //  this->fIntegratingTDC[i].Copy(&(input->fIntegratingTDC[i]));
      //this->fIntegratingADC.resize(input->fIntegratingADC.size());
      //for (size_t i = 0; i < this->fIntegratingADC.size(); i++)
      //  this->fIntegratingADC[i].Copy(&(input->fIntegratingADC[i]));

    } else {
      TString loc = "Standard exception from QwComptonPhotonDetector::Copy = "
             + source->GetSubsystemName() + " "
             + this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }

  } catch (std::exception& e) {
    QwError << e.what() << QwLog::endl;
  }

  return;
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
};
