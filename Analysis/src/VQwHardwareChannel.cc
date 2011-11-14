#include "VQwHardwareChannel.h"

// Qweak database headers
#include "QwLog.h"
#include "QwDBInterface.h"

VQwHardwareChannel::VQwHardwareChannel():
  fNumberOfDataWords(0),
  fNumberOfSubElements(0), fDataToSave(kRaw)
{
  fULimit = 0.0;
  fLLimit = 0.0;
  fErrorFlag = 0;
  fErrorConfigFlag = 0;
}

VQwHardwareChannel::VQwHardwareChannel(const VQwHardwareChannel& value)
  :VQwDataElement(value),
   fNumberOfDataWords(value.fNumberOfDataWords),
   fNumberOfSubElements(value.fNumberOfSubElements),
   fDataToSave(value.fDataToSave),
   fTreeArrayIndex(value.fTreeArrayIndex),
   fTreeArrayNumEntries(value.fTreeArrayNumEntries),
   fPedestal(value.fPedestal),
   fCalibrationFactor(value.fCalibrationFactor),
   kFoundPedestal(value.kFoundPedestal),
   kFoundGain(value.kFoundGain),
   bEVENTCUTMODE(value.bEVENTCUTMODE),
   fULimit(value.fULimit),
   fLLimit(value.fLLimit),
   fStability(value.fStability)
{
}

void VQwHardwareChannel::Copy(const VQwDataElement *source)
{
  if (typeid(*source) == typeid(*this)) {
    VQwDataElement::Copy(source);
    const VQwHardwareChannel* input =
        dynamic_cast<const VQwHardwareChannel*>(source);

    fNumberOfDataWords   = input->fNumberOfDataWords;
    fNumberOfSubElements = input->fNumberOfSubElements;
    fDataToSave          = input->fDataToSave;
    fTreeArrayIndex      = input->fTreeArrayIndex;
    fTreeArrayNumEntries = input->fTreeArrayNumEntries;
    fPedestal            = input->fPedestal;
    fCalibrationFactor   = input->fCalibrationFactor;
    kFoundPedestal       = input->kFoundPedestal;
    kFoundGain           = input->kFoundGain;
    bEVENTCUTMODE        = input->bEVENTCUTMODE;
    fULimit              = input->fULimit;
    fLLimit              = input->fLLimit;
    fStability           = input->fStability;

  } else {
    TString loc="VQwHardwareChannel::Copy for "
      +this->GetElementName()+" failed with input "
      +source->GetElementName();
    throw(std::invalid_argument(loc.Data()));
  }
}

UInt_t VQwHardwareChannel::GetEventcutErrorFlag()
{
  // return the error flag
  //first condition check for global/local status and second condition check to see non-zero HW error codes
  if (((fErrorConfigFlag & kGlobalCut) == kGlobalCut) && (fErrorFlag)>0){
    // we care only about global cuts
    //std::cout<<"fErrorFlag "<<(fErrorFlag & kGlobalCut)<<std::endl;
    return fErrorFlag+fErrorConfigFlag;//pass the error codes and configuration codes
  }
  return 0;
};

void VQwHardwareChannel::SetSingleEventCuts(Double_t min, Double_t max)
{
  fULimit=max;
  fLLimit=min;
}

void VQwHardwareChannel::SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability)
{
  fErrorConfigFlag=errorflag;
  fStability=stability;
  SetSingleEventCuts(min,max);
  QwWarning << "Set single event cuts for " << GetElementName() << ": "
	    << "Config-error-flag == 0x" << std::hex << errorflag << std::dec
	    << ", global? " << ((fErrorConfigFlag & kGlobalCut)==kGlobalCut) << ", stability? " << ((fErrorConfigFlag & kStabilityCut)==kStabilityCut) << QwLog::endl;
}


void VQwHardwareChannel::AddEntriesToList(std::vector<QwDBInterface> &row_list)
{
    QwDBInterface row;
    TString name    = GetElementName();
    UInt_t  entries = GetGoodEventCount();
    //  Loop over subelements and build the list.
    for(UInt_t subelement=0; 
	subelement<GetNumberOfSubelements();
	subelement++) {
      row.Reset();
      row.SetDetectorName(name);
      row.SetSubblock(subelement);
      row.SetN(entries);
      row.SetValue(GetValue(subelement));
      row.SetError(GetValueError(subelement));
      row_list.push_back(row);
    }
  }
