/*------------------------------------------------------------------------*//*!

 \file QwRoot.cc

 \ingroup QwAnalysis

 \brief ROOT wrapper with Qweak functionality

*//*-------------------------------------------------------------------------*/

// Qweak headers
#include "QwControl.h"

int main(int argc, char* argv[])
{
  // Start Qw-Root command prompt
  QwControl* qwctrl = new QwControl("Qweak-Root Analyzer", &argc, argv);
  qwctrl->Run();
  delete qwctrl;
}
