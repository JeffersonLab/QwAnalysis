//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : April 29th, 2010
//*****************************************************************************************************//
//correlation_plots.h

#ifndef __CORRELATIONPLOTS_H 
#define __CORRELATIONPLOTS_H

#include <vector>
#include <iomanip>
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TObject.h>
#include "correlations.cc"

void main_detector_correlations(TString  device_list[]); 
void plot_correlations(TTree* helltree,TString run_number, TString det2);
void get_fit_parameters( );
void fit_parameter_variation(TString device_list[]);  

#endif
