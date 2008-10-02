/**********************************************************\
* File: QwAnalysis.h                                       *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-22 15:40>                           *
\**********************************************************/

#ifndef __QWMAIN__
#define __QWMAIN__



#define NDetMax 1010 //for track reconstruction libraries

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

#include "QwSubsystemArray.h"


#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwMainDetector.h"

#include "QwHistogramHelper.h"










///  Globally defined instance of the QwHistogramHelper class.
QwHistogramHelper gQwHists;

void PrintInfo(TStopwatch& timer, Int_t run);


#endif
