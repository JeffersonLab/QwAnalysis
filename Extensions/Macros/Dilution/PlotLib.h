  /*
   * JoshTreeLib.cpp
 * Author: Josh Magee <magee@jlab.org>
 * Date:   2013-07-16
 *
 * Small program to walk QwDB rootfile trees for plotting purposes
 *
 */

#ifndef _PlotLib_H_
#define _PlotLib_H_
#include <TString.h>
using namespace std;

class TGraphErrors;

void placeAxis(TString, TString, TString, TCanvas*, TH1F*);
void placeAxis(TString, TString, TString, TCanvas*, TH2F*);
void placeAxis(TString, TString, TString, TCanvas*, TGraphErrors*);
void placeAxis(TString, TString, TString, TCanvas*, TMultiGraph*);

void placeLabel(TString,float,float,float,float);
void placeLabelRed(TString,float,float,float,float);
void placeLabelBlue(TString,float,float,float,float);

void callPlot(TGraphErrors *, TString, float);

void bluePlot(TGraphErrors *, float);
void redPlot(TGraphErrors *, float);
void greenPlot(TGraphErrors *, float);
void magentaPlot(TGraphErrors *, float);
void grayPlot(TGraphErrors *, float);
void orangePlot(TGraphErrors *, float);
void cyanPlot(TGraphErrors *, float);
void blackPlot(TGraphErrors *, float);

void blueHisto(TH1F*);
void redHisto(TH2F*);

void redStats(TPaveStats*);
void blueStats(TPaveStats*);
void setup_stats(TPaveStats *, float, float, float, float);
void HorizontalLine(TLine *, float, float, float);
void VerticalLine(TLine *, float, float, float);
void HorizontalArrow(TArrow *, float, float, float);

void fitGraph(TGraphErrors *,TF1 *);
void fitGraphWithStats(TGraphErrors *,TF1 *,float, float, float, float);
void fitGraphWithStatsRed(TGraphErrors *,TF1 *,float, float, float, float);
void fitGraphWithStatsBlue(TGraphErrors *,TF1 *,float, float, float, float);
void fitHistoWithStats(TH1F *,TF1 *,float, float, float, float);
void fitHistoWithStats(TH2F *,TF1 *,float, float, float, float);


#endif

