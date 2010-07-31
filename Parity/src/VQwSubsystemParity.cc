#include "VQwSubsystemParity.h"

#include "QwCorrelationMonitor.h"


/// Optional correlation monitor file
Int_t VQwSubsystemParity::LoadCorrelationMonitor(TString mapfile) {
  TString prFix = mapfile;
  QwMessage << "Loading correlation matrix from " << mapfile << QwLog::endl;
  fCorrelationMonitor = new  QwCorrelationMonitor;
  fCorrelationMonitor->SetParams( prFix,200,0.41);
  fCorrelationMonitor->AddVariableList(mapfile);
  fCorrelationMonitor->AccessChannels(this);
};
