/********************************************************************/
/*                      File: NurClass.h                            */
/*                         Nuruzzaman                               */
/*                         10/08/2010                               */
/*                                                                  */
/*                   Last Edited:08/03/2012                         */
/*                                                                  */
/* This includes the different classes needed for the main files.   */
/* It also defines the different styles used in the plots.          */
/********************************************************************/

#ifndef __NURCLASS_H 
#define __NURCLASS_H

#include <TROOT.h>
#include <TFile.h>
#include <iomanip>
#include <TRandom.h>
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
#include <TLatex.h>
#include <TPaveStats.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include "TObjString.h"
#include <sstream>
#include <string>


#define UTC (0)
#define CDT (-5)

char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
char green[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
char blue[] = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };
char magenta[] = { 0x1b, '[', '1', ';', '3', '5', 'm', 0 };
char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

const Double_t gPadCoord1(0.01);
const Double_t gPadCoord2(0.93);
const Double_t gPadCoord3(0.94);
const Double_t gPadCoord4(0.99);

TString email_list("qweak_autoanalysis@jlab.org");
TString user_name_hclog("qwanalysis");

TString gPlotDir(".");
TString gRunlistDir(".");

TString contact = Form("%sPlease contact Nuruzzaman (nur@jlab.org) for problems and comments%s\n",blue,normal);
TString contact_html = "Please contact Nuruzzaman (nur@jlab.org) for problems and comments";

TString SolidLine = Form("%s#-------------------------------------------------------------------------#%s\n",magenta,normal);
TString DashedLine = Form("%s#*************************************************************************#%s\n",red,normal);

TString start_analysis = "";
TString stop_analysis = "";


Int_t    gRunNumber;
TString  gTarget;
TString  gCurrent;
TString  gRasterString;



TH1* GetHist(TString name){
  return (TH1*)(gDirectory->Get(name));
}


TString     RunLabel(){
  return Form("Run %6d",gRunNumber);
}

TString     CanvasTitle(TString name){
  //  Creates a label like "Run  12345: <name>"
  TString tmp(RunLabel() + ":");
  tmp += name;
  return tmp;
}
TString     Heading(TString name){
  //  Creates a label like "Run  12345: <name> for <target setting>, <current setting>, <raster setting>"
  TString tmp(CanvasTitle(name));
  tmp += " for ";
  tmp += gTarget + ", " + gCurrent + ", " + gRasterString;
  return tmp;
}

TText* DrawCanvasHeader(TString name){
  TText* tmp = new TText(0.012,0.4,Heading(name));
  tmp->SetTextSize(0.3);
  tmp->Draw();
  return tmp;
}



TString     MakeSubject(){
  TString tmp("Analysis: ");
  tmp += RunLabel() + " - Plots and Summary Table for 100k Events.";
  return tmp;
}

TString     WrapParameter(TString param, TString value){
  TString tmp("--");
  tmp += param + "=\"" + value + "\" ";
  return tmp;
}
TString     WrapAttachment(TString filename){
  TString tmp("--attachment=\"");
  tmp += filename + "\" ";
  return tmp;
}

Int_t gPadCounter(1);
TPad* NewFramedPad(Color_t outcolor, Color_t incolor, TString heading){
  TString basename = Form("pad%d",gPadCounter++);
  TPad *outer = new TPad(basename+"0",basename+"0",gPadCoord1,gPadCoord2,gPadCoord4,gPadCoord4);
  TPad *inner = new TPad(basename+"1",basename+"1",gPadCoord1,gPadCoord1,gPadCoord4,gPadCoord3);
  outer->SetFillColor(outcolor);
  inner->SetFillColor(incolor);
  outer->Draw();
  inner->Draw();
  outer->cd();
  DrawCanvasHeader(heading);
  gPad->Update();
  inner->cd();
  return inner;
}


#endif
