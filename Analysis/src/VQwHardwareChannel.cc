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


VQwHardwareChannel::~VQwHardwareChannel()
{
}


void VQwHardwareChannel::Copy(const VQwDataElement *source){
  const VQwHardwareChannel* lsource = 
    dynamic_cast<const VQwHardwareChannel*>(source);
  if (lsource == NULL){
    TString loc="VQwHardwareChannel::Copy for "
      +this->GetElementName()+" failed with input "
      +source->GetElementName();
    throw(std::invalid_argument(loc.Data()));
  } else {
    Copy(*lsource);
  }
}

void VQwHardwareChannel::Copy(const VQwHardwareChannel& source){
  VQwDataElement::Copy(source);
  fNumberOfDataWords   = source.fNumberOfDataWords;
  fNumberOfSubElements = source.fNumberOfSubElements;
  fDataToSave          = source.fDataToSave;
  fTreeArrayIndex      = source.fTreeArrayIndex;
  fTreeArrayNumEntries = source.fTreeArrayNumEntries;
  fPedestal            = source.fPedestal;
  fCalibrationFactor   = source.fCalibrationFactor;
  kFoundPedestal       = source.kFoundPedestal;
  kFoundGain           = source.kFoundGain;
  bEVENTCUTMODE        = source.bEVENTCUTMODE;
  fULimit              = source.fULimit;
  fLLimit              = source.fLLimit;
  fStability           = source.fStability;
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
