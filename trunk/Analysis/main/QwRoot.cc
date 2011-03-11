/*------------------------------------------------------------------------*//*!

 \file QwRoot.cc

 \ingroup QwAnalysis

 \brief ROOT wrapper with Qweak functionality

*//*-------------------------------------------------------------------------*/

// Qweak headers
#include "QwRint.h"

int main(int argc, char** argv)
{
  // Start Qw-Root command prompt
  QwRint* qwrint = new QwRint("Qweak-Root Analyzer", &argc, argv);
  qwrint->Run();
  delete qwrint;
}
