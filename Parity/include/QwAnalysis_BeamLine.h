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

#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TStopwatch.h"

#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHistogramHelper.h"
#include "VQwSubsystem.h"
#include "QwBeamLine.h"
#include "QwLumi.h"

#include "VQwDataElement.h"
#include "QwHelicity.h"
#include "QwSubsystemArrayParity.h"
#include "QwHelicityPattern.h"
#include "QwScanner.h"

#include "QwEventRing.h"

void PrintInfo(TStopwatch& timer);


#endif
