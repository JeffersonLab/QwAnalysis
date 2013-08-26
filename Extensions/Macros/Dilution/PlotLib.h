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

void placeAxis(TString, TString, TString, TCanvas*, TGraphErrors*);
void placeAxis(TString, TString, TString, TCanvas*, TH1F*);

void placeLabel(TString,float,float,float,float);

void bluePlot(TGraphErrors *, float);
void redPlot(TGraphErrors *, float);
void greenPlot(TGraphErrors *, float);
void magentaPlot(TGraphErrors *, float);

void blueHisto(TH1F*);

void setup_stats(TPaveStats *, float, float, float, float);
void HorizontalLine(TLine *, float, float, float);
void VerticalLine(TLine *, float, float, float);
void HorizontalArrow(TArrow *, float, float, float);

void fitGraph(TGraphErrors *,TF1 *);
void fitGraphWithStats(TGraphErrors *,TF1 *,float, float, float, float);
void fitHistoWithStats(TH1F *,TF1 *,float, float, float, float);

#endif

