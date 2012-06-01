
#define QwPlotHelper_cxx
#include "../include/QwPlotHelper.hh"
#include "../include/headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TLine.h"

#ifdef QwPlotHelper_cxx

QwPlotHelper::QwPlotHelper():
  fMarkerStyleIN(20),fMarkerStyleOUT(21),fMarkerStyleNULL(22),fMarkerStyleAVG(23),
  fMarkerColorIN(4),fMarkerColorOUT(5),fMarkerColorNULL(8),fMarkerColorAVG(9)
{
  mgraph = new TMultiGraph();
  legend = new TLegend(0.1, 0.9, 0.3, 0.7);
}

QwPlotHelper::~QwPlotHelper()
{
  delete mgraph;
  delete legend;
}

TLegend *QwPlotHelper::GetLegend()
{

  return(legend);
}

TMultiGraph *QwPlotHelper::GetMultiGraph()
{

  return(mgraph);
}

void QwPlotHelper::SetColor(Color_t in, Color_t out, Color_t null, Color_t avg)
{
  fMarkerColorIN   = in;
  fMarkerColorOUT  = out;
  fMarkerColorNULL = null;
  fMarkerColorAVG  = avg;

}

void QwPlotHelper::SetStyle(Style_t in, Style_t out, Style_t null, Style_t avg)
{
  fMarkerStyleIN   = in;
  fMarkerStyleOUT  = out;
  fMarkerStyleNULL = null;

  fMarkerStyleAVG  = avg;

}

void QwPlotHelper::SetPlotTitle(TString title, TString xaxis, TString yaxis)
{

  fTitle = title;
  fTitleX = xaxis;
  fTitleY = yaxis;

}

void QwPlotHelper::InOutErrorPlotAsym(QwDataContainer &in, QwDataContainer &out, TString option)
{

  TF1 *line_in = new TF1("line_in", "[0]", 0, 1);
  TF1 *line_out = new TF1("line_out", "[0]", 0, 1);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200, 800);

  Int_t index = 0;

  canvas->cd();

  gPad->SetGridx();
  gPad->SetGridy();

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  TGraphErrors *tgraph_in = new TGraphErrors(in.fRunNumber, in.fAsym, in.fZero, in.fError);

  tgraph_in->SetMarkerColor(fMarkerColorIN);
  tgraph_in->SetMarkerSize(0.4);
  tgraph_in->SetMarkerStyle(fMarkerStyleIN);
  tgraph_in->SetTitle(Form("%s", fTitle.Data()));
  tgraph_in->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_in->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_in->GetXaxis()->SetLabelSize(0.02);
  tgraph_in->GetYaxis()->SetLabelSize(0.02);
  tgraph_in->GetXaxis()->SetTitleSize(0.02);
  tgraph_in->GetYaxis()->SetTitleSize(0.02);
  tgraph_in->GetXaxis()->SetTitleOffset(1.5);
  tgraph_in->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_in);

  //  tgraph_in->Draw("AP");
  line_in->SetLineColor(fMarkerColorIN);
  line_in->SetRange(in.fMysqlRunLower, in.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0){
    tgraph_in->Fit("line_in", "R");
    in.SetSlope(line_in->GetParameter(0));
    in.SetError(line_in->GetParError(0));
    in.SetChiNDF(line_in->GetChisquare()/line_in->GetNDF());
  }

  TGraphErrors *tgraph_out = new TGraphErrors(out.fRunNumber, out.fAsym, out.fZero, out.fError);

  tgraph_out->SetMarkerColor(fMarkerColorOUT);
  tgraph_out->SetMarkerSize(0.4);
  tgraph_out->SetMarkerStyle(fMarkerStyleOUT);
  tgraph_out->SetTitle(Form("%s", fTitle.Data()));
  tgraph_out->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_out->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_out->GetXaxis()->SetLabelSize(0.02);
  tgraph_out->GetYaxis()->SetLabelSize(0.02);
  tgraph_out->GetXaxis()->SetTitleSize(0.02);
  tgraph_out->GetYaxis()->SetTitleSize(0.02);
  tgraph_out->GetXaxis()->SetTitleOffset(1.5);
  tgraph_out->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_out);

  line_out->SetLineColor(fMarkerColorOUT);
  line_out->SetRange(out.fMysqlRunLower, out.fMysqlRunUpper);
  //  tgraph_out->Draw("AP");
  if(option.CompareTo("fit", TString::kExact) == 0){
    tgraph_out->Fit("line_out", "R");
    out.SetSlope(line_out->GetParameter(0));
    out.SetError(line_out->GetParError(0));
    out.SetChiNDF(line_out->GetChisquare()/line_in->GetNDF());
  }
  mgraph->Add(tgraph_in);
  mgraph->Add(tgraph_out);


  mgraph->Draw("ap");

  mgraph->GetXaxis()->SetLabelSize(0.02);
  mgraph->GetYaxis()->SetLabelSize(0.02);
  mgraph->GetXaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleSize(0.02);
  mgraph->GetXaxis()->SetTitleOffset(1.5);
  mgraph->GetYaxis()->SetTitleOffset(1.5);
  mgraph->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  mgraph->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  
  if(option.CompareTo("fit", TString::kExact) == 0){
    legend->SetHeader("MultiGraph Legend");
    legend->AddEntry(tgraph_in, Form("IN: %3.3e +- %3.3e", in.GetSlope(), in.GetError()),"pl");
    legend->AddEntry(tgraph_in, Form("Chi^2/NDF: %3.3e", in.GetChiNDF()),"pl");
    legend->AddEntry(tgraph_out, Form("OUT: %3.3e +- %3.3e",  out.GetSlope(), out.GetError()),"pl");
    legend->AddEntry(tgraph_out, Form("Chi^2/NDF: %3.3e", out.GetChiNDF()),"pl");
    legend->Draw();
  }
}

void QwPlotHelper::InOutErrorPlotAsym(QwDataContainer &raw_in, QwDataContainer &raw_out, QwDataContainer &corr_in, QwDataContainer &corr_out,  TString option)
{

  TF1 *line_in = new TF1("line_in", "[0]", 0, 1);
  TF1 *line_out = new TF1("line_out", "[0]", 0, 1);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200, 800);

  Int_t index = 0;

  canvas->cd();

  gPad->SetGridx();
  gPad->SetGridy();

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  TGraphErrors *tgraph_raw_in = new TGraphErrors(raw_in.fRunNumber, raw_in.fAsym, raw_in.fZero, raw_in.fError);

  tgraph_raw_in->SetMarkerColor(4);
  tgraph_raw_in->SetMarkerSize(0.4);
  tgraph_raw_in->SetMarkerStyle(20);
  tgraph_raw_in->SetTitle(Form("%s", fTitle.Data()));
  tgraph_raw_in->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_raw_in->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_raw_in->GetXaxis()->SetLabelSize(0.02);
  tgraph_raw_in->GetYaxis()->SetLabelSize(0.02);
  tgraph_raw_in->GetXaxis()->SetTitleSize(0.02);
  tgraph_raw_in->GetYaxis()->SetTitleSize(0.02);
  tgraph_raw_in->GetXaxis()->SetTitleOffset(1.5);
  tgraph_raw_in->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_raw_in);

  //  tgraph_raw_in->Draw("AP");
  line_in->SetLineColor(4);
  line_in->SetRange(raw_in.fMysqlRunLower, raw_in.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0){
    tgraph_raw_in->Fit("line_in", "R");
    raw_in.SetSlope(line_in->GetParameter(0));
    raw_in.SetError(line_in->GetParError(0));
    raw_in.SetChiNDF(line_in->GetChisquare()/line_in->GetNDF());
  }

  TGraphErrors *tgraph_raw_out = new TGraphErrors(raw_out.fRunNumber, raw_out.fAsym, raw_out.fZero, raw_out.fError);

  tgraph_raw_out->SetMarkerColor(5);
  tgraph_raw_out->SetMarkerSize(0.4);
  tgraph_raw_out->SetMarkerStyle(21);
  tgraph_raw_out->SetTitle(Form("%s", fTitle.Data()));
  tgraph_raw_out->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_raw_out->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_raw_out->GetXaxis()->SetLabelSize(0.02);
  tgraph_raw_out->GetYaxis()->SetLabelSize(0.02);
  tgraph_raw_out->GetXaxis()->SetTitleSize(0.02);
  tgraph_raw_out->GetYaxis()->SetTitleSize(0.02);
  tgraph_raw_out->GetXaxis()->SetTitleOffset(1.5);
  tgraph_raw_out->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_raw_out);

  line_out->SetLineColor(5);
  line_out->SetRange(raw_out.fMysqlRunLower, raw_out.fMysqlRunUpper);
  //  tgraph_raw_out->Draw("AP");
  if(option.CompareTo("fit", TString::kExact) == 0){
    tgraph_raw_out->Fit("line_out", "R");
    raw_out.SetSlope(line_out->GetParameter(0));
    raw_out.SetError(line_out->GetParError(0));
    raw_out.SetChiNDF(line_out->GetChisquare()/line_in->GetNDF());
  }
  mgraph->Add(tgraph_raw_in);
  mgraph->Add(tgraph_raw_out);


  TGraphErrors *tgraph_corr_in = new TGraphErrors(corr_in.fRunNumber, corr_in.fAsym, corr_in.fZero, corr_in.fError);

  tgraph_corr_in->SetMarkerColor(8);
  tgraph_corr_in->SetMarkerSize(0.4);
  tgraph_corr_in->SetMarkerStyle(22);
  tgraph_corr_in->SetTitle(Form("%s", fTitle.Data()));
  tgraph_corr_in->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_corr_in->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_corr_in->GetXaxis()->SetLabelSize(0.02);
  tgraph_corr_in->GetYaxis()->SetLabelSize(0.02);
  tgraph_corr_in->GetXaxis()->SetTitleSize(0.02);
  tgraph_corr_in->GetYaxis()->SetTitleSize(0.02);
  tgraph_corr_in->GetXaxis()->SetTitleOffset(1.5);
  tgraph_corr_in->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_corr_in);

  //  tgraph_corr_in->Draw("AP");
  line_in->SetLineColor(8);
  line_in->SetRange(corr_in.fMysqlRunLower, corr_in.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0){
    tgraph_corr_in->Fit("line_in", "R");
    corr_in.SetSlope(line_in->GetParameter(0));
    corr_in.SetError(line_in->GetParError(0));
    corr_in.SetChiNDF(line_in->GetChisquare()/line_in->GetNDF());
  }

  TGraphErrors *tgraph_corr_out = new TGraphErrors(corr_out.fRunNumber, corr_out.fAsym, corr_out.fZero, corr_out.fError);

  tgraph_corr_out->SetMarkerColor(9);
  tgraph_corr_out->SetMarkerSize(0.4);
  tgraph_corr_out->SetMarkerStyle(23);
  tgraph_corr_out->SetTitle(Form("%s", fTitle.Data()));
  tgraph_corr_out->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_corr_out->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_corr_out->GetXaxis()->SetLabelSize(0.02);
  tgraph_corr_out->GetYaxis()->SetLabelSize(0.02);
  tgraph_corr_out->GetXaxis()->SetTitleSize(0.02);
  tgraph_corr_out->GetYaxis()->SetTitleSize(0.02);
  tgraph_corr_out->GetXaxis()->SetTitleOffset(1.5);
  tgraph_corr_out->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_corr_out);

  line_out->SetLineColor(9);
  line_out->SetRange(corr_out.fMysqlRunLower, corr_out.fMysqlRunUpper);
  //  tgraph_corr_out->Draw("AP");
  if(option.CompareTo("fit", TString::kExact) == 0){
    tgraph_corr_out->Fit("line_out", "R");
    corr_out.SetSlope(line_out->GetParameter(0));
    corr_out.SetError(line_out->GetParError(0));
    corr_out.SetChiNDF(line_out->GetChisquare()/line_in->GetNDF());
  }
  mgraph->Add(tgraph_corr_in);
  mgraph->Add(tgraph_corr_out);

  mgraph->Draw("ap");

  mgraph->GetXaxis()->SetLabelSize(0.02);
  mgraph->GetYaxis()->SetLabelSize(0.02);
  mgraph->GetXaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleSize(0.02);
  mgraph->GetXaxis()->SetTitleOffset(1.5);
  mgraph->GetYaxis()->SetTitleOffset(1.5);
  mgraph->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  mgraph->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  
  if(option.CompareTo("fit", TString::kExact) == 0){
    legend->SetHeader("MultiGraph Legend");
    
    legend->AddEntry(tgraph_corr_in, Form("IN: %3.3e +- %3.3e", corr_in.GetSlope(), corr_in.GetError()),"pl");
    legend->AddEntry(tgraph_corr_in, Form("Chi^2/NDF: %3.3e", corr_in.GetChiNDF()),"pl");
    legend->AddEntry(tgraph_corr_out, Form("OUT: %3.3e +- %3.3e",  corr_out.GetSlope(), corr_out.GetError()),"pl");
    legend->AddEntry(tgraph_corr_out, Form("Chi^2/NDF: %3.3e", corr_out.GetChiNDF()),"pl");
    
    legend->AddEntry(tgraph_raw_in, Form("IN: %3.3e +- %3.3e", raw_in.GetSlope(), raw_in.GetError()),"pl");
    legend->AddEntry(tgraph_raw_in, Form("Chi^2/NDF: %3.3e", raw_in.GetChiNDF()),"pl");
    legend->AddEntry(tgraph_raw_out, Form("OUT: %3.3e +- %3.3e",  raw_out.GetSlope(), raw_out.GetError()),"pl");
    legend->AddEntry(tgraph_raw_out, Form("Chi^2/NDF: %3.3e", raw_out.GetChiNDF()),"pl");
    
    legend->Draw();
  }
}

void QwPlotHelper::InOutErrorPlotAvAsym(QwDataContainer &raw_in, QwDataContainer &raw_out, QwDataContainer &corr_in, QwDataContainer &corr_out, TString option)
{
  

  TF1 *line = new TF1("line", "[0]", 0, 1);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200, 800);

  TVectorD axis(1);
  TVectorD ihwp_corr_in(1);
  TVectorD ihwp_corr_out(1);
  TVectorD ihwp_raw_in(1);
  TVectorD ihwp_raw_out(1);

  TVectorD ihwp_corr_null(1);
  TVectorD ihwp_raw_null(1);
  TVectorD ihwp_corr_avg(1);
  TVectorD ihwp_raw_avg(1);

  TVectorD ihwp_corr_in_error(1);
  TVectorD ihwp_corr_out_error(1);
  TVectorD ihwp_raw_in_error(1);
  TVectorD ihwp_raw_out_error(1);

  TVectorD ihwp_null_error(1);

  TVectorD zero(1);

  //  TLegend *legend = new TLegend(0.1, 0.9, 0.3, 0.7);

  zero.Zero();

  corr_in.CalculateAverage();
  corr_out.CalculateAverage();
  raw_in.CalculateAverage();
  raw_out.CalculateAverage();

  ihwp_corr_in[0] = corr_in.fAvAsym[0];
  ihwp_corr_out[0] = corr_out.fAvAsym[0];
  ihwp_raw_in[0] = raw_in.fAvAsym[0];
  ihwp_raw_out[0] = raw_out.fAvAsym[0];

  ihwp_corr_null[0] = 0.5*(corr_in.fAvAsym[0] + corr_out.fAvAsym[0]);
  ihwp_corr_avg[0] = 0.5*(corr_in.fAvAsym[0] - corr_out.fAvAsym[0]);

  ihwp_corr_null[0] = 0.5*(corr_in.fAvAsym[0] + corr_out.fAvAsym[0]);
  ihwp_raw_null[0] = 0.5*(raw_in.fAvAsym[0] + raw_out.fAvAsym[0]);

  ihwp_raw_avg[0] = 0.5*(raw_in.fAvAsym[0] - raw_out.fAvAsym[0]);

  ihwp_corr_in_error[0] = corr_in.fAvError[0];
  ihwp_corr_out_error[0] = corr_out.fAvError[0];
  ihwp_null_error[0] = 0.5*TMath::Sqrt((TMath::Power(corr_in.fAvError[0], 2) + TMath::Power(corr_out.fAvError[0], 2)));

  ihwp_raw_in_error[0] = raw_in.fAvError[0];
  ihwp_raw_out_error[0] = raw_out.fAvError[0];
  ihwp_null_error[0] = 0.5*TMath::Sqrt((TMath::Power(raw_in.fAvError[0], 2) + TMath::Power(raw_out.fAvError[0], 2)));

  canvas->cd();

  gPad->SetGridy();

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  //
  // Build graphs
  //

  axis[0] = 1;

  TGraphErrors *tgraph_corr_in = new TGraphErrors(axis, ihwp_corr_in, zero, ihwp_corr_in_error);

  tgraph_corr_in->SetMarkerColor(4);
  tgraph_corr_in->SetMarkerSize(1.5);
  tgraph_corr_in->SetMarkerStyle(20);

  TGraphErrors *tgraph_raw_in = new TGraphErrors(axis, ihwp_raw_in, zero, ihwp_raw_in_error);

  tgraph_raw_in->SetMarkerColor(4);
  tgraph_raw_in->SetMarkerSize(1.5);
  tgraph_raw_in->SetMarkerStyle(24);

  axis[0] = 2;

  TGraphErrors *tgraph_corr_out = new TGraphErrors(axis, ihwp_corr_out, zero, ihwp_corr_out_error);

  tgraph_corr_out->SetMarkerColor(5);
  tgraph_corr_out->SetMarkerSize(1.5);
  tgraph_corr_out->SetMarkerStyle(21);

  TGraphErrors *tgraph_raw_out = new TGraphErrors(axis, ihwp_raw_out, zero, ihwp_raw_out_error);

  tgraph_raw_out->SetMarkerColor(5);
  tgraph_raw_out->SetMarkerSize(1.5);
  tgraph_raw_out->SetMarkerStyle(25);

  axis[0] = 3;

  TGraphErrors *tgraph_corr_null = new TGraphErrors(axis, ihwp_corr_null, zero, ihwp_null_error);

  tgraph_corr_null->SetMarkerColor(8);
  tgraph_corr_null->SetMarkerSize(1.5);
  tgraph_corr_null->SetMarkerStyle(22);

  TGraphErrors *tgraph_raw_null = new TGraphErrors(axis, ihwp_raw_null, zero, ihwp_null_error);

  tgraph_raw_null->SetMarkerColor(8);
  tgraph_raw_null->SetMarkerSize(1.5);
  tgraph_raw_null->SetMarkerStyle(26);

  //  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_raw_null->Fit("line", "R");

  axis[0] = 4;

  TGraphErrors *tgraph_corr_avg = new TGraphErrors(axis, ihwp_corr_avg, zero, ihwp_null_error);

  tgraph_corr_avg->SetMarkerColor(9);
  tgraph_corr_avg->SetMarkerSize(1.5);
  tgraph_corr_avg->SetMarkerStyle(23);

  TGraphErrors *tgraph_raw_avg = new TGraphErrors(axis, ihwp_raw_avg, zero, ihwp_null_error);

  tgraph_raw_avg->SetMarkerColor(9);
  tgraph_raw_avg->SetMarkerSize(1.5);
  tgraph_raw_avg->SetMarkerStyle(32);


  //  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_corr_avg->Fit("line", "R");

  //
  // End building graphs..
  //


  //
  // This is a stupid way to do this but the damned TLine, like much of ROOT is useless...
  //

  TVectorD zero_line(4);
  TVectorD zero_error(4);
  TVectorD zero_axis(4);

  for(Int_t i = 0; i < 4; i++){
    zero_line[i] = 0;
    zero_error[i] = 0;
    zero_axis[i] = i + 1;
  }

  TGraphErrors *zero_plot = new TGraphErrors(zero_axis, zero_line, zero_error, zero_error);

  line->SetLineStyle(1);
  line->SetLineWidth(2);
  zero_plot->Fit("line");

  mgraph->Add(tgraph_corr_in);
  mgraph->Add(tgraph_corr_out);
  mgraph->Add(tgraph_corr_null);
  mgraph->Add(tgraph_corr_avg);
  mgraph->Add(zero_plot);

  mgraph->Add(tgraph_raw_in);
  mgraph->Add(tgraph_raw_out);
  mgraph->Add(tgraph_raw_null);
  mgraph->Add(tgraph_raw_avg);
  mgraph->Add(zero_plot);

  mgraph->Draw("ap");

  mgraph->GetYaxis()->SetTitle("Asymmetry(ppm)");
  mgraph->GetXaxis()->SetLabelSize(0.02);
  mgraph->GetYaxis()->SetLabelSize(0.02);
  mgraph->GetXaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleSize(0.02);
  mgraph->GetXaxis()->SetTitleOffset(1.5);
  mgraph->GetYaxis()->SetTitleOffset(1.5);
  mgraph->GetXaxis()->SetNdivisions(4);

  mgraph->GetXaxis()->SetBinLabel(5, "IN");
  mgraph->GetXaxis()->SetBinLabel(35, "OUT");
  mgraph->GetXaxis()->SetBinLabel(66, "<IN+OUT>");
  mgraph->GetXaxis()->SetBinLabel(97, "AVG(IN,-OUT)");

  //  legend->SetHeader("MultiGraph Legend");
  legend->AddEntry(tgraph_corr_in, Form("Corr IN: %3.3e +- %3.3e", ihwp_corr_in[0], ihwp_corr_in_error[0]),"p");
  legend->AddEntry(tgraph_corr_out, Form("Corr OUT: %3.3e +- %3.3e",  ihwp_corr_out[0], ihwp_corr_out_error[0]),"p");
  legend->AddEntry(tgraph_corr_null, Form("Corr NULL: %3.3e +- %3.3e",  ihwp_corr_null[0], ihwp_null_error[0]),"p");
  legend->AddEntry(tgraph_corr_avg, Form("Corr AVG(IN,-OUT): %3.3e +- %3.3e",  ihwp_corr_avg[0], ihwp_null_error[0]),"p");

  legend->AddEntry(tgraph_raw_in, Form("Raw IN: %3.3e +- %3.3e", ihwp_raw_in[0], ihwp_raw_in_error[0]),"p");
  legend->AddEntry(tgraph_raw_out, Form("Raw OUT: %3.3e +- %3.3e",  ihwp_raw_out[0], ihwp_raw_out_error[0]),"p");
  legend->AddEntry(tgraph_raw_null, Form("Raw NULL: %3.3e +- %3.3e",  ihwp_raw_null[0], ihwp_null_error[0]),"p");
  legend->AddEntry(tgraph_raw_avg, Form("Raw AVG(IN,-OUT): %3.3e +- %3.3e",  ihwp_raw_avg[0], ihwp_null_error[0]),"p");
  legend->Draw();

}

void QwPlotHelper::InOutErrorPlotAvAsym(QwDataContainer &in, QwDataContainer &out, TString option)
{
  

  TF1 *line = new TF1("line", "[0]", 0, 1);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200, 800);

  TVectorD axis(1);
  TVectorD ihwp_in(1);
  TVectorD ihwp_out(1);
  TVectorD ihwp_null(1);
  TVectorD ihwp_avg(1);

  TVectorD ihwp_in_error(1);
  TVectorD ihwp_out_error(1);
  TVectorD ihwp_null_error(1);

  TVectorD zero(1);

  //  TLegend *legend = new TLegend(0.1, 0.9, 0.3, 0.8);

  zero.Zero();

  in.CalculateAverage();
  out.CalculateAverage();

  ihwp_in[0] = in.fAvAsym[0];
  ihwp_out[0] = out.fAvAsym[0];
  ihwp_null[0] = 0.5*(in.fAvAsym[0] + out.fAvAsym[0]);
  ihwp_avg[0] = 0.5*(in.fAvAsym[0] - out.fAvAsym[0]);

  ihwp_in_error[0] = in.fAvError[0];
  ihwp_out_error[0] = out.fAvError[0];
  ihwp_null_error[0] = 0.5*TMath::Sqrt((TMath::Power(in.fAvError[0], 2) + TMath::Power(out.fAvError[0], 2)));

  canvas->cd();

  gPad->SetGridy();

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  axis[0] = 1;

  TGraphErrors *tgraph_in = new TGraphErrors(axis, ihwp_in, zero, ihwp_in_error);

  tgraph_in->SetMarkerColor(4);
  tgraph_in->SetMarkerSize(1.5);
  tgraph_in->SetMarkerStyle(20);
 //  ScaleTGraph(tgraph_in);
 //  tgraph_in->Draw("AP");
//   line->SetLineColor(4);
//   line->SetRange(in.fMysqlRunLower, in.fMysqlRunUpper);
//   if(option.CompareTo("fit", TString::kExact) == 0){
//     tgraph_in->Fit("line", "R");
//     in.SetSlope(line_in->GetParameter(0));
//     in.SetError(line_in->GetParError(0));
//     in.SetChiNDF(line_in->GetChisquare()/line_in->GetNDF());
//   }

  axis[0] = 2;

  TGraphErrors *tgraph_out = new TGraphErrors(axis, ihwp_out, zero, ihwp_out_error);

  tgraph_out->SetMarkerColor(5);
  tgraph_out->SetMarkerSize(1.5);
  tgraph_out->SetMarkerStyle(21);
//   tgraph_out->SetTitle(Form("%s", fTitle.Data()));
 //  ScaleTGraph(tgraph_out);
 //  tgraph_out->Draw("AP");
//   line->SetLineColor(4);
//   line->SetRange(in.fMysqlRunLower, in.fMysqlRunUpper);
//   if(option.CompareTo("fit", TString::kExact) == 0){
//     tgraph_out->Fit("line", "R");
//     out.SetSlope(line_out->GetParameter(0));
//     out.SetError(line_out->GetParError(0));
//     out.SetChiNDF(line_out->GetChisquare()/line_in->GetNDF());
//   }

  axis[0] = 3;

  TGraphErrors *tgraph_null = new TGraphErrors(axis, ihwp_null, zero, ihwp_null_error);

  tgraph_null->SetMarkerColor(8);
  tgraph_null->SetMarkerSize(1.5);
  tgraph_null->SetMarkerStyle(22);
 //  ScaleTGraph(tgraph_null);
 //  tgraph_null->Draw("AP");
//   line->SetLineColor(4);
//   line->SetRange(in.fMysqlRunLower, in.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_null->Fit("line", "R");

  axis[0] = 4;

  TGraphErrors *tgraph_avg = new TGraphErrors(axis, ihwp_avg, zero, ihwp_null_error);

  tgraph_avg->SetMarkerColor(9);
  tgraph_avg->SetMarkerSize(1.5);
  tgraph_avg->SetMarkerStyle(23);
 //  ScaleTGraph(tgraph_avg);
 //  tgraph_avg->Draw("AP");
//   line->SetLineColor(4);
//   line->SetRange(in.fMysqlRunLower, in.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_avg->Fit("line", "R");

  //
  // This is a stupid way to do this but the damned TLine, like much of ROOT is useless...
  //

  TVectorD zero_line(4);
  TVectorD zero_error(4);
  TVectorD zero_axis(4);

  for(Int_t i = 0; i < 4; i++){
    zero_line[i] = 0;
    zero_error[i] = 0;
    zero_axis[i] = i + 1;
  }

  TGraphErrors *zero_plot = new TGraphErrors(zero_axis, zero_line, zero_error, zero_error);

  line->SetLineStyle(1);
  line->SetLineWidth(2);
  zero_plot->Fit("line");

  mgraph->Add(tgraph_in);
  mgraph->Add(tgraph_out);
  mgraph->Add(tgraph_null);
  mgraph->Add(tgraph_avg);
  mgraph->Add(zero_plot);
  mgraph->Draw("ap");

  mgraph->GetYaxis()->SetTitle("Asymmetry(ppm)");
  mgraph->GetXaxis()->SetLabelSize(0.02);
  mgraph->GetYaxis()->SetLabelSize(0.02);
  mgraph->GetXaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleSize(0.02);
  mgraph->GetXaxis()->SetTitleOffset(1.5);
  mgraph->GetYaxis()->SetTitleOffset(1.5);
  mgraph->GetXaxis()->SetNdivisions(4);

  mgraph->GetXaxis()->SetBinLabel(5, "IN");
  mgraph->GetXaxis()->SetBinLabel(35, "OUT");
  mgraph->GetXaxis()->SetBinLabel(66, "<IN+OUT>");
  mgraph->GetXaxis()->SetBinLabel(97, "AVG(IN,-OUT)");

  //  legend->SetHeader("MultiGraph Legend");
  legend->AddEntry(tgraph_in, Form("IN: %3.3e +- %3.3e", ihwp_in[0], ihwp_in_error[0]),"p");
  legend->AddEntry(tgraph_out, Form("OUT: %3.3e +- %3.3e",  ihwp_out[0], ihwp_out_error[0]),"p");
  legend->AddEntry(tgraph_null, Form("NULL: %3.3e +- %3.3e",  ihwp_null[0], ihwp_null_error[0]),"p");
  legend->AddEntry(tgraph_avg, Form("AVG(IN,-OUT): %3.3e +- %3.3e",  ihwp_avg[0], ihwp_null_error[0]),"p");
  legend->Draw();

}

void QwPlotHelper::InOutErrorPlotSens(QwDataContainer &in, QwDataContainer &out, Int_t type, TString option)
{
  

  TF1 *line_in = new TF1("line_in", "[0]", 0, 1);
  TF1 *line_out = new TF1("line_out", "[0]", 0, 1);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200, 800);

  TMultiGraph *mgraph = new TMultiGraph();

  canvas->cd();

  gPad->SetGridx();
  gPad->SetGridy();

  gStyle->SetOptFit(1110);


  TGraphErrors *tgraph_in = new TGraphErrors(in.fRunNumber, in.fSensitivity[type], in.fZero, in.fZero);

  tgraph_in->SetMarkerColor(4);
  tgraph_in->SetMarkerSize(0.2);
  tgraph_in->SetMarkerStyle(21);
  tgraph_in->SetTitle(Form("%s", fTitle.Data()));
  tgraph_in->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_in->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_in->GetXaxis()->SetLabelSize(0.02);
  tgraph_in->GetYaxis()->SetLabelSize(0.02);
  tgraph_in->GetXaxis()->SetTitleSize(0.02);
  tgraph_in->GetYaxis()->SetTitleSize(0.02);
  tgraph_in->GetXaxis()->SetTitleOffset(1.5);
  tgraph_in->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_in);
  //  tgraph_in->Draw("AP");
  line_in->SetLineColor(4);
  line_in->SetRange(in.fMysqlRunLower, in.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_in->Fit("line_in", "");

  TGraphErrors *tgraph_out = new TGraphErrors(out.fRunNumber, out.fSensitivity[type], out.fZero, out.fZero);

  tgraph_out->SetMarkerColor(6);
  tgraph_out->SetMarkerSize(0.2);
  tgraph_out->SetMarkerStyle(21);
  tgraph_out->SetTitle(Form("%s", fTitle.Data()));
  tgraph_out->GetXaxis()->SetTitle(Form("%s", fTitleX.Data()));
  tgraph_out->GetYaxis()->SetTitle(Form("%s", fTitleY.Data()));
  tgraph_out->GetXaxis()->SetLabelSize(0.02);
  tgraph_out->GetYaxis()->SetLabelSize(0.02);
  tgraph_out->GetXaxis()->SetTitleSize(0.02);
  tgraph_out->GetYaxis()->SetTitleSize(0.02);
  tgraph_out->GetXaxis()->SetTitleOffset(1.5);
  tgraph_out->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(tgraph_out);
  //  tgraph_out->Draw("AP");
  line_out->SetLineColor(6);
  line_out->SetRange(out.fMysqlRunLower, out.fMysqlRunUpper);
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_out->Fit("line_out", "");

  mgraph->Add(tgraph_in);
  mgraph->Add(tgraph_out);
  mgraph->Draw("ap");

  mgraph->GetXaxis()->SetLabelSize(0.02);
  mgraph->GetYaxis()->SetLabelSize(0.02);
  mgraph->GetXaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleSize(0.02);
  mgraph->GetXaxis()->SetTitleOffset(1.5);
  mgraph->GetYaxis()->SetTitleOffset(1.5);
  mgraph->GetYaxis()->SetRangeUser(-0.1, 0.1);
}

void QwPlotHelper::ScaleTGraph(TGraphErrors *tgraph)
{

  Double_t max = GetMaximum(tgraph->GetY());
  Double_t min = GetMinimum(tgraph->GetY());

  if(tgraph == NULL) return;

  if( max < 0){
    max -= 0.6*max;
  } else{
    max += 0.6*max;
  }   
  if( min < 0){
    min += 0.6*min;
  } else{
    min -= 0.6*min;
  }   

  tgraph->GetYaxis()->SetRangeUser(min, max);
  return;
}

Double_t QwPlotHelper::GetMaximum(Double_t *array)
{
  Double_t temp = array[0];

  Int_t nlines = sizeof(array)/sizeof(Double_t);

  for(Int_t i = 0; i < nlines; i++){
    if(array[i] > temp) temp = array[i];
  }
  return(temp);
}

Double_t QwPlotHelper::GetMinimum(Double_t *array)
{
  Double_t temp = array[0];

  Int_t nlines = sizeof(array)/sizeof(Double_t);

  for(Int_t i = 0; i < nlines; i++){
    if(array[i] < temp) temp = array[i];
  }
  return(temp);
}

#endif
