/**
 * \file	QwComptonElectronDetector.cc
 *
 * \brief	Implementation of the analysis of Compton electron detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonElectronDetector class is defined as a parity subsystem that
 * contains all data modules of the electron detector (V1495, ...).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#include "QwComptonElectronDetector.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"

//*****************************************************************
Int_t QwComptonElectronDetector::LoadChannelMap(TString mapfile)
{
  return 0;
};

//*****************************************************************
Int_t QwComptonElectronDetector::LoadEventCuts(TString & filename)
{
  return 0;
};

//*****************************************************************
Int_t QwComptonElectronDetector::LoadInputParameters(TString pedestalfile)
{
  return 0;
}

//*****************************************************************
void QwComptonElectronDetector::RandomizeEventData(int helicity)
{
}

//*****************************************************************
void QwComptonElectronDetector::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

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
Int_t QwComptonElectronDetector::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
};

//*****************************************************************
Bool_t QwComptonElectronDetector::SingleEventCuts()
{
  QwOut << "QwComptonElectronDetector::SingleEventCuts()" << QwLog::endl;
  return IsGoodEvent();
};

//*****************************************************************
void  QwComptonElectronDetector::ProcessEvent()
{
  return;
};

//*****************************************************************
Int_t QwComptonElectronDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
};

//*****************************************************************
Bool_t QwComptonElectronDetector::IsGoodEvent()
{
  Bool_t test = kTRUE;

  if (!test) std::cerr << "This is not a good event!" << std::endl;
  return test;
}

//*****************************************************************
void QwComptonElectronDetector::ClearEventData()
{
  return;
};

//*****************************************************************
VQwSubsystem&  QwComptonElectronDetector::operator=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    // TODO: low-level assignment operations
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    // TODO: low-level addition-assignment operations
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator-=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    // TODO: low-level subtraction-assignment operations
  }
  return *this;
};

void  QwComptonElectronDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
};

void  QwComptonElectronDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
};

void QwComptonElectronDetector::Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwComptonElectronDetector* innumer = dynamic_cast<QwComptonElectronDetector*> (numer);
    QwComptonElectronDetector* indenom = dynamic_cast<QwComptonElectronDetector*> (denom);
    // TODO: low-level ratio operations
  }
  return;
};

void QwComptonElectronDetector::Scale(Double_t factor)
{
  // TODO: low-level scaling operations
  return;
};

Bool_t QwComptonElectronDetector::Compare(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;
  } else {

    // TODO: low-level comparison operations

  }
  return result;
}

//*****************************************************************
void  QwComptonElectronDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  return;
};

void  QwComptonElectronDetector::DeleteHistograms()
{
  return;
};

void  QwComptonElectronDetector::FillHistograms()
{
  return;
};



void QwComptonElectronDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  return;
};

void QwComptonElectronDetector::FillTreeVector(std::vector<Double_t> &values)
{
  return;
};


//*****************************************************************
void  QwComptonElectronDetector::Print()
{
  VQwSubsystemParity::Print();

  return;
}

//*****************************************************************
void  QwComptonElectronDetector::Copy(VQwSubsystem *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      VQwSubsystem::Copy(source);
      QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (source);

      // TODO: low-level copy operations

    } else {
      TString loc = "Standard exception from QwComptonElectronDetector::Copy = "
             + source->GetSubsystemName() + " "
             + this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return;
}


VQwSubsystem*  QwComptonElectronDetector::Copy()
{

  QwComptonElectronDetector* TheCopy = new QwComptonElectronDetector(this->GetSubsystemName() + " Copy");
  TheCopy->Copy(this);
  return TheCopy;
}
