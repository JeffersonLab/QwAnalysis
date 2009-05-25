/*------------------------------------------------------------------------*//*!

 \file QwControl.cc

 \brief Program which starts a dataserver, an analyzer and a Qw-Root prompt

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/

#include "QwControl.h"

int main (int argc, char** argv)
{
  // Start Qw-Root command prompt
  QwControl* qwctrl = new QwControl ("Qweak-Root Analyzer", &argc, argv);
  qwctrl->StartDataserver();
  qwctrl->StartAnalyzer();
  qwctrl->Run();
  delete qwctrl;
}
