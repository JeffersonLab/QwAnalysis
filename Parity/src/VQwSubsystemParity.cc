#include "VQwSubsystemParity.h"

#include "QwCorrelationMonitor.h"


/// Optional correlation monitor file
Int_t VQwSubsystemParity::LoadCorrelationMonitor(TString mapfile) {
  QwMessage << "Loading correlation matrix from " << mapfile << QwLog::endl;
  fCorrelationMonitor = new QwCorrelationMonitor;
  TString prefix = GetSubsystemName() + "_";
  prefix.ToLower(); prefix.ReplaceAll(' ','_');
  fCorrelationMonitor->SetParams(prefix,200,0.41);
  fCorrelationMonitor->AddVariableList(mapfile);
  fCorrelationMonitor->AccessChannels(this);
};
