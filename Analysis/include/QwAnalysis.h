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
#include "Rtypes.h"    // ROOT header, defines ints, floats, .. in a platform independent way

#include "TROOT.h"
#include "TFile.h"
#include "TStopwatch.h"


#include "TQwEventBuffer.h"

#include "VQwSubsystem.h"
#include "TQwDriftChamber.h"



void PrintInfo(TStopwatch& timer);


#endif
