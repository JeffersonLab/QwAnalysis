/**********************************************************\
* File: QwAnalysis.h                                       *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWMAIN__
#define __QWMAIN__


#include <iostream>
#include <fstream>
#include <vector>
#include <new>

#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TStopwatch.h>

#include "QwCommandLine.h"

#include "QwEventBuffer.h"

#include "QwHistogramHelper.h"

#include "QwSubsystemArray.h"
#include "QwQuartzBar.h"

///  Globally defined instance of the QwHistogramHelper class.
QwHistogramHelper gQwHists;


void PrintInfo(TStopwatch& timer, Int_t run);

#endif
