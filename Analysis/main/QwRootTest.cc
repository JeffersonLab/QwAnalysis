/*------------------------------------------------------------------------*//*!

 \file QwRoot.cc

 \ingroup QwAnalysis

 \brief ROOT wrapper with Qweak functionality

*//*-------------------------------------------------------------------------*/

// Qweak headers
#include "QwRoot.h"

int main(int argc, char* argv[])
{
  // Start Qw-Root command prompt
  QwRoot* qwroot = new QwRoot("Qweak-Root Analyzer", &argc, argv);
  qwroot->Run();
  delete qwroot;
}
