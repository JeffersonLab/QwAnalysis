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

QwPlotHelper::QwPlotHelper()
{

}

QwPlotHelper::~QwPlotHelper()
{

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

  TLegend *legend = new TLegend(0.1, 0.9, 0.3, 0.8);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200, 800);

  TMultiGraph *mgraph = new TMultiGraph();

  canvas->cd();

  gPad->SetGridx();
  gPad->SetGridy();

  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  TGraphErrors *tgraph_in = new TGraphErrors(in.fRunNumber, in.fCorrAsym, in.fZero, in.fCorrError);

  tgraph_in->SetMarkerColor(4);
  tgraph_in->SetMarkerSize(0.4);
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
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_in->Fit("line_in", "R");

  TGraphErrors *tgraph_out = new TGraphErrors(out.fRunNumber, out.fCorrAsym, out.fZero, out.fCorrError);

  tgraph_out->SetMarkerColor(6);
  tgraph_out->SetMarkerSize(0.4);
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

  line_out->SetLineColor(6);
  line_out->SetRange(out.fMysqlRunLower, out.fMysqlRunUpper);
  //  tgraph_out->Draw("AP");
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_out->Fit("line_out", "R");

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
    legend->AddEntry(tgraph_in, Form("IN: %3.3e +- %3.3e", line_in->GetParameter(0), line_in->GetParError(0)),"pl");
    legend->AddEntry(tgraph_in, Form("Chi^2/NDF: %3.3e", line_in->GetChisquare()/line_in->GetNDF()),"pl");
    legend->AddEntry(tgraph_out, Form("OUT: %3.3e +- %3.3e",  line_out->GetParameter(0), line_out->GetParError(0)),"pl");
    legend->AddEntry(tgraph_out, Form("Chi^2/NDF: %3.3e", line_out->GetChisquare()/line_out->GetNDF()),"pl");
    legend->Draw();
  }
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

  TMultiGraph *mgraph = new TMultiGraph();

  TLegend *legend = new TLegend(0.1, 0.9, 0.3, 0.8);

  zero.Zero();

  in.CalculateAverage();
  out.CalculateAverage();

  ihwp_in[0] = in.fAvCorrAsym[0];
  ihwp_out[0] = out.fAvCorrAsym[0];
  ihwp_null[0] = 0.5*(in.fAvCorrAsym[0] + out.fAvCorrAsym[0]);
  ihwp_avg[0] = 0.5*(in.fAvCorrAsym[0] - out.fAvCorrAsym[0]);

  ihwp_in_error[0] = in.fAvCorrError[0];
  ihwp_out_error[0] = out.fAvCorrError[0];
  ihwp_null_error[0] = 0.5*TMath::Sqrt((TMath::Power(in.fAvCorrError[0], 2) + TMath::Power(out.fAvCorrError[0], 2)));

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
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_in->Fit("line", "R");


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
  if(option.CompareTo("fit", TString::kExact) == 0) tgraph_out->Fit("line", "R");

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

  legend->SetHeader("MultiGraph Legend");
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
