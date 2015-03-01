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

void placeAxis(TString title, TString xaxis, TString yaxis, TCanvas *canvas, TGraphErrors *graph) {
  float size=0.05;
  float titlesize=0.07;
  float labelsize = 0.06;
  graph->SetTitle(title);
  graph->GetXaxis()->SetTitle(xaxis);
  graph->GetYaxis()->SetTitle(yaxis);

  canvas->Modified();
  canvas->Update();

  graph->GetYaxis()->SetTitleOffset(0.55);
  graph->GetXaxis()->SetTitleOffset(0.80);
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

void placeAxis(TString title, TString xaxis, TString yaxis, TCanvas *canvas, TMultiGraph *multi) {
  float size=0.07;
  float titlesize=0.09;
  float labelsize = 0.07;
  multi->SetTitle(title);
  multi->GetXaxis()->SetTitle(xaxis);
  multi->GetYaxis()->SetTitle(yaxis);

  canvas->Modified();
  canvas->Update();

  multi->GetYaxis()->SetTitleOffset(0.50);
  multi->GetXaxis()->SetTitleOffset(0.80);
  multi->GetYaxis()->SetTitleSize(size);
  multi->GetXaxis()->SetTitleSize(size);
  multi->GetYaxis()->SetLabelSize(labelsize);
  multi->GetXaxis()->SetLabelSize(labelsize);

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
 subtitle->SetTextSize(0.60);
 //  subtitle->SetTextColor(46);
 subtitle->Draw();
}

void placeLabelRed(TString text, float x1, float y1, float x2, float y2) {
 TPaveLabel *subtitle = new TPaveLabel(x1,y1,x2,y2,text,"NDC");
 subtitle->SetBorderSize(0);
 subtitle->SetFillColor(0);
 subtitle->SetTextSize(0.75);
 subtitle->SetTextColor(46);
// subtitle->SetTextColor(kRed+3);
 subtitle->Draw();
}


void placeLabelBlue(TString text, float x1, float y1, float x2, float y2) {
 TPaveLabel *subtitle = new TPaveLabel(x1,y1,x2,y2,text,"NDC");
 subtitle->SetBorderSize(0);
 subtitle->SetFillColor(0);
 subtitle->SetTextSize(0.75);
 subtitle->SetTextColor(9);
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
  fit->SetLineWidth(3);
  graph->Fit(fit,"sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats * stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  setup_stats(stats,x1,y1,x2,y2);
  stats->Draw("sames");
}

void fitGraphWithStatsRed(TGraphErrors *graph, TF1* fit,float x1, float y1, float x2, float y2) {
  fit->SetLineColor(kRed+2);
  fit->SetLineStyle(9);
  fit->SetLineWidth(3);
  graph->Fit(fit,"sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats * stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  setup_stats(stats,x1,y1,x2,y2);
  redStats(stats);
  stats->Draw("sames");
}


void fitGraphWithStatsBlue(TGraphErrors *graph, TF1* fit,float x1, float y1, float x2, float y2) {
  fit->SetLineColor(kBlue+2);
  fit->SetLineStyle(2);
  fit->SetLineWidth(3);
  graph->Fit(fit,"sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats * stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  setup_stats(stats,x1,y1,x2,y2);
  blueStats(stats);
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

void fitHistoWithStats(TH2F* hist, TF1* fit,float x1, float y1, float x2, float y2) {
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  hist->Fit(fit,"sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats * stats = (TPaveStats*) hist->GetListOfFunctions()->FindObject("stats");
  setup_stats(stats,x1,y1,x2,y2);
  stats->Draw("sames");
}

void callPlot(TGraphErrors *plot, TString color, float size) {
  color.ToLower();
  if (color == "blue")
    bluePlot(plot, size);
  else if (color == "red")
    redPlot(plot,size);
  else if (color == "green")
    greenPlot(plot,size);
  else if (color == "magenta")
    magentaPlot(plot,size);
  else if (color == "gray")
    grayPlot(plot,size);
  else if (color == "orange")
    orangePlot(plot,size);
  else if (color == "cyan")
    cyanPlot(plot,size);
  else if (color == "black")
    blackPlot(plot,size);
  else {
    std::cout <<"You need to select a color! Blue is selected for you!\n";
    bluePlot(plot,size);
  }
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

void grayPlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(kGray+3);
  plot->SetMarkerStyle(29);
  plot->SetLineColor(kGray+3);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void orangePlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(kOrange+10);
  plot->SetMarkerStyle(21);
  plot->SetLineColor(kOrange+10);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void cyanPlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(kCyan+4);
  plot->SetMarkerStyle(32);
  plot->SetLineColor(kCyan+4);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void blackPlot(TGraphErrors *plot, float size ) {
  plot->SetMarkerColor(kBlack);
  plot->SetMarkerStyle(33);
  plot->SetLineColor(kBlack);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}

void blueHisto(TH1F *hist) {
  hist->SetLineColor(kBlue+3);
  hist->SetFillColor(9);
}

void redHisto(TH2F *hist) {
  hist->SetLineColor(kRed+3);
  hist->SetFillColor(kRed+3);
  hist->SetMarkerColor(kRed+3);
  hist->SetMarkerStyle(2);
}

void setup_stats(TPaveStats *stats, float x1, float y1, float x2, float y2) {
  stats->SetFillColor(0);
  stats->SetX1NDC(x1);
  stats->SetY1NDC(y1);
  stats->SetX2NDC(x2);
  stats->SetY2NDC(y2);
}

void redStats(TPaveStats *stats) {
  stats->SetTextColor(46);
}

void blueStats(TPaveStats *stats) {
  stats->SetTextColor(9);
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




