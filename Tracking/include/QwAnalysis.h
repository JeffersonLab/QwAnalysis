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

#include "QwSubsystemArrayTracking.h"


#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwMainDetector.h"

#include "QwHistogramHelper.h"



//for debugging purposes I'm wrting grand hit list and sub hit lists to files - Rakitha (10/23/2008)
const char FILE_NAME[] = "grandhits_output.txt";
const char FILE_NAME2[] = "subhits_output.txt";
FILE *out_file; 
 FILE *out_file2;






///  Globally defined instance of the QwHistogramHelper class.
QwHistogramHelper gQwHists;

void PrintInfo(TStopwatch& timer, Int_t run);
void SaveHits(QwHitContainer &); //for debugging purposes - Rakitha (10/23/2008)
void SaveSubList(QwHitContainer &);//for debugging purposes - Rakitha (10/23/2008)

#endif
