/**********************************************************\
* File: QwAnalysis.h                                       *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-22 15:40>                           *
\**********************************************************/

#ifndef __QWMAIN__
#define __QWMAIN__



//#define NDetMax 1010 //for track reconstruction libraries
#define NEventMax 1000

#include <iostream>
#include <fstream>
#include <vector>
#include <new>

#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TStopwatch.h>


#include "Det.h" // for NDetMax discrepancy 
#include "QwCommandLine.h"

//#include "QwEventBuffer.h"
#include "QwASCIIEventBuffer.h"

#include "QwSubsystemArrayTracking.h"

//  Raster position subsystem?
//  Beamline diagnostic subsystem?
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"

#include "QwScanner.h"
//#include "QwMainCerenkovDetector.h"
#include "QwVQWK_Module.h"
#include "QwVQWK_Channel.h"

#include "QwHistogramHelper.h"


//from QTR
#include "globals.h"
#include "QwHit.h"






//for debugging purposes I'm wrting grand hit list and sub hit lists to files - Rakitha (10/23/2008)
const char FILE_NAME[] = "grandhits_output.txt";
const char FILE_NAME2[] = "subhits_output.txt";
FILE *out_file; 
FILE *out_file2;







void PrintInfo(TStopwatch& timer, UInt_t run);
void SaveHits(QwHitContainer &); //for debugging purposes - Rakitha (10/23/2008)
void SaveSubList(QwHitContainer &);//for debugging purposes - Rakitha (10/23/2008)

//this will add braches and fill vector for each hit
void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values,Int_t size);
void  FillTreeVector(std::vector<Float_t> &values,QwHitContainer &hitlist);




#endif
