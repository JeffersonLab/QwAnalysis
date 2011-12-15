#include "VQwHardwareChannel.h"

// Qweak database headers
#include "QwLog.h"
#include "QwDBInterface.h"
#include "QwParameterFile.h"

VQwHardwareChannel::VQwHardwareChannel():
  fNumberOfDataWords(0),
  fNumberOfSubElements(0), fDataToSave(kRaw),
  fErrorFlag(0), fDefErrorFlag(0)
{
}

VQwHardwareChannel::~VQwHardwareChannel()
{
}

UInt_t VQwHardwareChannel::GetEventcutErrorFlag()
{
  // return the error flag
  if (((fErrorFlag & kGlobalCut) == kGlobalCut) && fDeviceErrorCode>0){
    // we care only about global cuts
    return fErrorFlag;
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
  fErrorFlag=errorflag;
  fDefErrorFlag=errorflag;
  fStability=stability;
  SetSingleEventCuts(min,max);
  QwDebug << "Set single event cuts for " << GetElementName() << ": "
	  << "errorflag == 0x" << std::hex << errorflag << std::dec
	  << ", global? " << (fErrorFlag & kGlobalCut) << QwLog::endl;
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

void VQwHardwareChannel::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist){
  if (GetElementName()!=""){
    TString devicename;
    devicename=GetElementName();
    devicename.ToLower();
    if (modulelist.HasValue(devicename)){
      ConstructBranch(tree,prefix);
    }
  }
}
