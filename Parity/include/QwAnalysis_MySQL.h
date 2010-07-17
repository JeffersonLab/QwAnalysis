/**********************************************************\
* File: QwAnalysis_MySQL.h                                 *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWANAYSIS_MYSQL_MAIN__
#define __QWANAYSIS_MYSQL_MAIN__



#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TStopwatch.h"

#include "QwEventBuffer.h"
#include "QwHistogramHelper.h"
#include "VQwSubsystem.h"
#include "QwBeamLine.h"
#include "QwLumi.h"

#include "VQwDataElement.h"
/* #include "QwHelicity.h" */
#include "QwFakeHelicity.h"
#include "QwSubsystemArrayParity.h"
#include "QwMainCerenkovDetector.h"
#include "QwHelicityPattern.h"
#include "QwScanner.h"

#include "QwEventRing.h"

#include "QwColor.h"
#include "QwOptionsParity.h"
#include "QwDatabase.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <new>

void PrintInfo(TStopwatch& timer);


#endif
