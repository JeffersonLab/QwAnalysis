/*------------------------------------------------------------------------*//*!

 \file QwRoot.cc

 \ingroup QwAnalysis

 \brief ROOT wrapper with Qweak functionality

*//*-------------------------------------------------------------------------*/

// Qweak headers
#include "QwRint.h"
#include <TSystem.h>
#include <TROOT.h>
#include <TString.h>
#include <QwOptions.h>

int main(int argc, char** argv)
{
  TString path = getenv_safe_TString("QWANALYSIS");
  gROOT->ProcessLine(".include " + path + "/Analysis/include");
  gROOT->ProcessLine(".include " + path + "/Parity/include");
  gROOT->ProcessLine(".include " + path + "/Parity/include");
  // Start Qw-Root command prompt
  QwRint* qwrint = new QwRint("Qweak-Root Analyzer", &argc, argv);
  qwrint->Run();
  delete qwrint;
}
