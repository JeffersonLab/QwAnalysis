/********************************************************************/
/*                      File: NurClass.h                            */
/*                         Nuruzzaman                               */
/*                         10/08/2010                               */
/*                                                                  */
/*                   Last Edited:11/11/2011                         */
/*                                                                  */
/* This includes the different classes needed for the main files.   */
/* It also defines the different styles used in the plots.          */
/********************************************************************/

#ifndef __NURCLASS_H 
#define __NURCLASS_H

//gROOT.Reset();
#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TBox.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TString.h>
#include <TAxis.h>
#include <TCut.h>
#include <TText.h>
#include <TAttText.h>
#include <TLine.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <Rtypes.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TObject.h>
#include <TApplication.h>
#include <fstream>
#include <vector>
#include <new>
#include <TColor.h>
#include <TMath.h>
#include <math.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TPad.h>
#include <TVector.h>
#include <TSystem.h>
#include <TArrow.h>
#include <TChain.h>

//#include <QwColor.h>
//#include <ConsoleColor.h>

#define UTC (0)
#define CDT (-5)

char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
char green[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
char blue[] = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };
char magenta[] = { 0x1b, '[', '1', ';', '3', '5', 'm', 0 };
char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

TString contact = Form("%sPlease contact Nuruzzaman (nur@jlab.org) for problems and comments%s\n",blue,normal);


TString SolidLine = Form("%s#-------------------------------------------------------------------------#%s\n",magenta,normal);
TString DashedLine = Form("%s#*************************************************************************#%s\n",red,normal);


#endif
