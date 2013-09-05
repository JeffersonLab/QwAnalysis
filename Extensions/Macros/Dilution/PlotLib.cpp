/*
 * JoshTreeLib.cpp
 * Author: Josh Magee <magee@jlab.org>
 * Date:   2013-07-16
 *
 * Small program to walk QwDB rootfile trees for plotting purposes
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TLine.h>
#include <TArrow.h>
#include <TPaveLabel.h>
#include <TApplication.h>

#include "PlotLib.h"

using namespace std;


void placeAxis(TString title, TString xaxis, TString yaxis, TCanvas *canvas, TGraphErrors *graph) {
  float size=0.07;
  float titlesize=0.09;
  float labelsize = 0.07;
  graph->SetTitle(title);
  graph->GetXaxis()->SetTitle(xaxis);
  graph->GetYaxis()->SetTitle(yaxis);

  canvas->Modified();
  canvas->Update();

  graph->GetYaxis()->SetTitleOffset(0.25);
  graph->GetXaxis()->SetTitleOffset(0.75);
  graph->GetYaxis()->SetTitleSize(size);
  graph->GetXaxis()->SetTitleSize(size);
  graph->GetYaxis()->SetLabelSize(labelsize);
  graph->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *tit = (TPaveText*) gPad->GetPrimitive("title");
  tit->SetTextSize(titlesize);
  //gStyle->SetTitleSize(titlesize);

  gPad->SetGrid();

  canvas->Modified();
  canvas->Update();

}

void placeAxis(TString title, TString xaxis, TString yaxis, TCanvas *canvas, TH1F* hist) {
  float size=0.07;
  float titlesize=0.09;
  float labelsize = 0.07;
  hist->SetTitle(title);
  hist->GetXaxis()->SetTitle(xaxis);
  hist->GetYaxis()->SetTitle(yaxis);

  canvas->Modified();
  canvas->Update();

  hist->GetYaxis()->SetTitleOffset(0.35);
  hist->GetXaxis()->SetTitleOffset(0.75);
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetYaxis()->SetLabelSize(labelsize);
  hist->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *tit = (TPaveText*) gPad->GetPrimitive("title");
  tit->SetTextSize(titlesize);
  //gStyle->SetTitleSize(titlesize);

  gPad->SetGrid();

  canvas->Modified();
  canvas->Update();

}

void placeAxis(TString title, TString xaxis, TString yaxis, TCanvas *canvas, TH2F* hist) {
  float size=0.07;
  float titlesize=0.09;
  float labelsize = 0.07;
  hist->SetTitle(title);
  hist->GetXaxis()->SetTitle(xaxis);
  hist->GetYaxis()->SetTitle(yaxis);

  canvas->Modified();
  canvas->Update();

  hist->GetYaxis()->SetTitleOffset(0.35);
  hist->GetXaxis()->SetTitleOffset(0.75);
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetYaxis()->SetLabelSize(labelsize);
  hist->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *tit = (TPaveText*) gPad->GetPrimitive("title");
  tit->SetTextSize(titlesize);
  //gStyle->SetTitleSize(titlesize);

  gPad->SetGrid();

  canvas->Modified();
  canvas->Update();

}

void placeLabel(TString text, float x1, float y1, float x2, float y2) {
 TPaveLabel *subtitle = new TPaveLabel(x1,y1,x2,y2,text,"NDC");
 subtitle->SetBorderSize(0);
 subtitle->SetFillColor(0);
 subtitle->SetTextSize(0.05);
 //  subtitle->SetTextColor(46);
  subtitle->Draw();
}

void fitGraph(TGraphErrors *graph, TF1* fit) {
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  graph->Fit(fit,"sames");
}

void fitGraphWithStats(TGraphErrors *graph, TF1* fit,float x1, float y1, float x2, float y2) {
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  graph->Fit(fit,"sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats * stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  setup_stats(stats,x1,y1,x2,y2);
  stats->Draw("sames");
}


void fitHistoWithStats(TH1F* hist, TF1* fit,float x1, float y1, float x2, float y2) {
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  hist->Fit(fit,"sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats * stats = (TPaveStats*) hist->GetListOfFunctions()->FindObject("stats");
  setup_stats(stats,x1,y1,x2,y2);
  stats->Draw("sames");
}

void bluePlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(9);
  plot->SetMarkerStyle(20);
  plot->SetLineColor(9);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void redPlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(46);
  plot->SetMarkerStyle(25);
  plot->SetLineColor(46);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void greenPlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(kGreen+3);
  plot->SetMarkerStyle(22);
  plot->SetLineColor(kGreen+3);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void magentaPlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(kMagenta-1);
  plot->SetMarkerStyle(27);
  plot->SetLineColor(kMagenta-1);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void blueHisto(TH1F *hist) {
  hist->SetLineColor(kBlue+3);
  hist->SetFillColor(9);
}

void redHisto(TH2F *hist) {
  hist->SetLineColor(kRed+3);
  hist->SetFillColor(9);
}

void setup_stats(TPaveStats *stats, float x1, float y1, float x2, float y2) {
  stats->SetFillColor(0);
  stats->SetX1NDC(x1);
  stats->SetY1NDC(y1);
  stats->SetX2NDC(x2);
  stats->SetY2NDC(y2);
}

void HorizontalLine(TLine *line, float x1, float x2, float y) {
  line->SetX1(x1);
  line->SetX2(x2);
  line->SetY1(y);
  line->SetY2(y);

  line->SetLineColor(12);
  line->SetLineWidth(2);
  line->SetLineStyle(7);
}

void VerticalLine(TLine *line, float x, float y1, float y2) {
  line->SetX1(x);
  line->SetX2(x);
  line->SetY1(y1);
  line->SetY2(y2);

  line->SetLineColor(12);
  line->SetLineWidth(2);
  line->SetLineStyle(7);

  line->Draw();
}


void HorizontalArrow(TArrow *arr, float x1, float x2, float y) {
  arr->SetX1(x1);
  arr->SetX2(x2);
  arr->SetY1(y);
  arr->SetY2(y);

  arr->SetLineColor(12);
  arr->SetLineWidth(2);
  arr->SetLineStyle(1);

  arr->SetOption("<|>");
  arr->SetArrowSize(.0175);

  arr->Draw();

//  arr->DrawArrow(x1,y,x2,y,);
}



