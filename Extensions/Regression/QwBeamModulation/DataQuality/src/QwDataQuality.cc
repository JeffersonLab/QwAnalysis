#define QwDataQuality_cxx
#include "QwDataQuality.hh"
#include "headers.h"
#include "TPaveStats.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#ifdef QwDataQuality_cxx

QwDataQuality::QwDataQuality(TChain *tree)
{

  fTitle      = "";
  fXaxisTitle = "";
  fYaxisTitle = "";
  fFilename   = "";
  fFilename   = "output";

  fXmaximum =  5.0;
  fXminimum = -5.0;
  fYmaximum =  100;
  fYminimum =  100;
  fNbins    =  250;

  fXcanvas = 2000;
  fYcanvas = 1500;

  fFileOpen  = false;
  fStatError = false;
  fSignCorr  = true;
  fRun1      = false;
  fRun2      = false;

  fChain = tree;
  fChain->SetMakeClass(1);

}

QwDataQuality::~QwDataQuality()
{

}

TString QwDataQuality::GetTitle(){return fTitle;}
TString QwDataQuality::GetXaxisTitle(){return fXaxisTitle;}
TString QwDataQuality::GetYaxisTitle(){return fYaxisTitle;}

void QwDataQuality::SetTitle(TString title)
{
  // Set title used for plots

  fTitle = title;
  return;
}

void QwDataQuality::SetXaxisTitle(TString title)
{
  // set x-axis title

  fXaxisTitle = title;
  return;
}

void QwDataQuality::SetYaxisTitle(TString title)
{
  // set y-axis title

  fYaxisTitle = title;
  return;
}

void QwDataQuality::SetStatisticalError(Bool_t value = true)
{
  // Flag to make sure the error variable is set to rms/Sqrt(N) instead of what is native in the database.  Currently, for most leaves, I think they are the same.

  fStatError = value;
}

void QwDataQuality::SetSignCorr(Bool_t value = true)
{
  // Flag used to determine whether data is sign corrected or not.

  fSignCorr = value;
}

void QwDataQuality::SetOutputFilename(TString output = "output")
{
  // Choose the name used for the output file.

  fOutputName = output;
}

void QwDataQuality::SetRunPeriod(TString run)
{

  // Sets run period to be plotted.

  if(run.CompareTo("run1", TString::kIgnoreCase) == 0) fRun1 = true;
  if(run.CompareTo("run2", TString::kIgnoreCase) == 0) fRun2 = true;

  return;
}

void QwDataQuality::SetXmaximum(Int_t value)
{

  // Set x extrema for plots

  fXmaximum = value;
  return;
}

void QwDataQuality::SetXminimum(Int_t value)
{

  // Set x extrema for plots

  fXminimum = value;
  return;
}

void QwDataQuality::SetYmaximum(Int_t value)
{
  // Set y extrema for plots

  fYmaximum = value;
  return;
}

void QwDataQuality::SetYminimum(Int_t value)
{
  // Set y extrema for plots

  fYminimum = value;
  return;
}

void QwDataQuality::SetExtrema(std::vector <Double_t> x_value)
{

  // Loop through a given data vector and find the x(y) extrema then set the internal class variables asscotiated with each.

 Int_t nentries = x_value.size();

  Double_t max = x_value[0];
  Double_t min = x_value[0];
    
  for(Int_t i = 0; i < nentries; i++){
    if( x_value[i] > max) max = x_value[i];
    if( x_value[i] < min) min = x_value[i]; 
  }
  fXmaximum = max;
  fXminimum = min;
  fNbins = (max - min)*0.1;
    
  if(fNbins > 1e4) fNbins = 1000;

  return;
  
}
void QwDataQuality::SetExtrema(std::vector <Double_t> x_value, std::vector <Double_t> y_value)
{

  // Loop through a given data vector and find the x(y) extrema then set the internal class variables asscotiated with each.

  Int_t nentries = x_value.size();
  
  Double_t max = x_value[0];
  Double_t min = x_value[0];
  
  for(Int_t i = 0; i < nentries; i++){
    if( x_value[i] > max) max = x_value[i];
    if( x_value[i] < min) min = x_value[i]; 
  }
  fXmaximum = max;
  fXminimum = min;

  nentries = y_value.size();
  
  max = y_value[0];
  min = y_value[0];
  
  for(Int_t i = 0; i < nentries; i++){
    if( y_value[i] > max) max = y_value[i];
    if( y_value[i] < min) min = y_value[i]; 
  }
  fYmaximum = max;
  fYminimum = min;

  fNbins = (max - min)*0.1;
    
  if(fNbins > 1e4) fNbins = 1000;

  return;
}

void QwDataQuality::GetWeightedAverage(std::vector <Double_t> value, std::vector <Double_t> weight)
{

  // Calculate the weighted average of a data vector given the weight.  Currently this value is not stored anywhere.  The input data vector can 
  // be filled using FillDataVector()

  Double_t avg = 0;
  Double_t err = 0;

  for(Int_t i = 0; i < (Int_t)value.size(); i++){
    avg += value[i]/TMath::Power(weight[i], 2);
    err += 1/TMath::Power(weight[i], 2);
  }

  std::cout << avg/err << " +- " << TMath::Sqrt(1/err) << std::endl;

}

void QwDataQuality::PullByWien(TString dataname="diff_bcmdd12/value") 
{

  // Plots the pull plot for a given variable.  Before using sign correction shoulf be set using SetSignCorr() function.  This flag defaults to 
  // true.  It is also recommended that flags such as title, axes names, and run period be set.

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  std::vector <TF1 *> fit;
  std::vector <TH1F*> histo;

  Int_t data_set_offset = 0;

  Double_t mean = 0;
  
  if(fRun1){
    data_set_offset = 1;
  }
  else if(fRun2){
    data_set_offset = 6;
  }
  else {
    std::cout << "Unknown run period. Exiting." << std::endl;
    exit(1);
  }

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);

  canvas->Divide(1,5);
  
  for(Int_t i = 0; i < 5; i++){
    TH1F *temp = new TH1F("temp","temp", fNbins, fXminimum, fXmaximum);
    FillHistoByWien(i +  data_set_offset, dataname, temp);
    mean = temp->GetMean(1);
    ClearVectors();
    temp->Delete();

    histo.push_back(new TH1F(Form("Wien_%i", i + data_set_offset),"PullHisto", fNbins, fXminimum, fXmaximum));
    FillPullByWien(i +  data_set_offset, dataname, histo[i], mean);
    // blueHisto(histo[i]);

    canvas->cd(i + 1);
    gPad->SetGrid();
    gPad->SetLogy();

    histo[i]->SetTitle(fTitle);
    histo[i]->GetXaxis()->SetTitle(fXaxisTitle);
    histo[i]->GetYaxis()->SetTitle(fYaxisTitle);
    histo[i]->SetLineColor(1);
    histo[i]->SetFillColor(9);
    histo[i]->Draw();
    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "gaus"));
    FitGraphWithStats(histo[i], fit[i], 0.85, 0.46, 0.99, 0.99);
  }
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

  DeleteHisto(histo);
}

void QwDataQuality::HistoByWien(TString dataname="diff_bcmdd12/value") 
{

  // Plots the histograms of a given variable.  Before using sign correction shoulf be set using SetSignCorr() function.  This flag defaults to 
  // true.  It is also recommended that flags such as title, axes names, and run period be set.

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  std::vector <TF1 *> fit;

  Int_t data_set_offset = 0;
  
  if(fRun1){
    data_set_offset = 1;
  }
  else if(fRun2){
    data_set_offset = 6;
  }
  else {
    std::cout << "Unknown run period. Exiting." << std::endl;
    exit(1);
  }

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  std::vector <TH1F*> histo;

  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);
  canvas->Divide(1,5);
  
  for(Int_t i = 0; i < 5; i++){
    histo.push_back(new TH1F(Form("Wien_%i", i + data_set_offset),"WienHisto", fNbins, fXminimum, fXmaximum));
    FillHistoByWien(i +  data_set_offset, dataname, histo[i]);
    // blueHisto(histo[i]);

    canvas->cd(i + 1);
    gPad->SetGrid();
    gPad->SetLogy();

    histo[i]->SetTitle(fTitle);
    histo[i]->GetXaxis()->SetTitle(fXaxisTitle);
    histo[i]->GetYaxis()->SetTitle(fYaxisTitle);

    histo[i]->SetLineColor(1);
    histo[i]->SetFillColor(9);
    histo[i]->Draw();
    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "gaus"));
    FitGraphWithStats(histo[i], fit[i], 0.85, 0.46, 0.99, 0.99);
  }
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

  DeleteHisto(histo);
}

void QwDataQuality::HistoByWien(TString dataname="diff_bcmdd12/value", TString weight = "" ) {

  // Plots the weighted histograms of a given variable.  Before using sign correction shoulf be set using SetSignCorr() function.  This flag defaults to 
  // true.  It is also recommended that flags such as title, axes names, and run period be set.  

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  std::vector <TF1 *> fit;

  Int_t data_set_offset = 0;
  
  if(fRun1){
    data_set_offset = 1;
  }
  else if(fRun2){
    data_set_offset = 6;
  }
  else {
    std::cout << "Unknown run period. Exiting." << std::endl;
    exit(1);
  }

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  std::vector <TH1F*> histo;

  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);
  canvas->Divide(1,5);
  
  for(Int_t i = 0; i < 5; i++){
    histo.push_back(new TH1F(Form("Wien_%i", i + data_set_offset),"WienHisto", fNbins, fXminimum, fXmaximum));
    FillHistoByWien(i + data_set_offset, dataname, histo[i], weight);
    // blueHisto(histo[i]);

    canvas->cd(i + 1);
    gPad->SetGrid();
    gPad->SetLogy();

    histo[i]->GetXaxis()->SetTitle(fXaxisTitle);
    histo[i]->GetYaxis()->SetTitle(fYaxisTitle);
    histo[i]->SetLineColor(1);
    histo[i]->SetFillColor(9);
    histo[i]->Draw();
    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "gaus"));
    FitGraphWithStats(histo[i], fit[i], 0.85, 0.46, 0.99, 0.99);
  }

  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

  DeleteHisto(histo);

}

void QwDataQuality::HistoByWien(std::vector <Double_t> value) 
{

  // Plots the histograms of a data vector.  Before using sign correction shoulf be set using SetSignCorr() function.  This flag defaults to 
  // true.  It is also recommended that flags such as title, axes names, and run period be set.
  //
  // Data vector can be filled using FillDataVector().
  

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  std::vector <TF1 *> fit;

  Int_t data_set_offset = 0;

  if(fRun1){
    data_set_offset = 1;
  }
  else if(fRun2){
    data_set_offset = 6;
  }
  else {
    std::cout << "Unknown run period. Exiting." << std::endl;
    exit(1);
  }
  
  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }
  Int_t events = (Int_t)fChain->GetEntries();  

  Double_t wien_slug;

  std::vector <TH1F*> histo;

  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);
  canvas->Divide(1,5);
  
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  
  for(Int_t i = 0; i < 5; i++){
    histo.push_back(new TH1F(Form("Wien_%i", i + data_set_offset),"WienHisto", fNbins, fXminimum, fXmaximum));
    for(Int_t j = 0; j < events; j++){
      fChain->GetEntry(j);
      histo[i]->Fill(value[j]);
    }
        
    canvas->cd(i + 1);
    gPad->SetGrid();
    gPad->SetLogy();
    
    histo[i]->SetTitle(fTitle);
    histo[i]->GetXaxis()->SetTitle(fXaxisTitle);
    histo[i]->GetYaxis()->SetTitle(fYaxisTitle);
    histo[i]->SetLineColor(1);
    histo[i]->SetFillColor(9);
    histo[i]->Draw();
    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "gaus"));
    FitGraphWithStats(histo[i], fit[i], 0.85, 0.46, 0.99, 0.99);
  }
  gPad->Modified();
  gPad->Update();
  
  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

  DeleteHisto(histo);

}

void QwDataQuality::PlotByWien(TString dataname="diff_bcmdd12/value") {
  // Plots a given variable versus runlet for each wien.  Before using sign correction shoulf be set using SetSignCorr() function.  This flag defaults to 
  // true.  It is also recommended that flags such as title, axes names, and run period be set.

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  Int_t data_set_offset = 1;

  if(fRun1){
    data_set_offset = 1;
  }
  else if(fRun2){
    data_set_offset = 6;
  }
  else {
    std::cout << "Unknown run period. Exiting." << std::endl;
    exit(1);
  }

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  std::vector <TGraphErrors *> w;
  std::vector <TF1 *> fit;

  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);
  canvas->Divide(1,5);

  for(Int_t i = 0; i < 5; i++){
    GetDataByWien(i + data_set_offset, dataname);

    if(GetLeafName(dataname).CompareTo("value", TString::kExact) == 0){
      w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fValue.data(), 0, fError.data()));
      ClearVectors();
    }
    else if(GetLeafName(dataname).CompareTo("rms", TString::kExact) == 0){
      w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fRMS.data(), 0, 0));
      ClearVectors();
    }
    else if(GetLeafName(dataname).CompareTo("err", TString::kExact) == 0){
      w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fError.data(), 0, 0));
      ClearVectors();
    }
    else{
      std::cerr << "Unknown data type passed to QwDataQuality::PlotByWien.  Exiting." << std::endl;
      exit(1);
    }

    canvas->cd(i + 1);
    gPad->SetGrid();
    w[i]->SetTitle(fTitle);
    w[i]->GetXaxis()->SetTitle(fXaxisTitle);
    w[i]->GetYaxis()->SetTitle(fYaxisTitle);
    w[i]->SetMarkerColor(9);
    w[i]->SetMarkerStyle(20);
    w[i]->SetLineColor(9);
    w[i]->SetLineWidth(2);
    w[i]->Draw("ap");
    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "pol0"));
    FitGraphWithStats(w[i], fit[i], 0.85, 0.66, 0.99, 0.99);

  }
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

} 

void QwDataQuality::PlotByWien(TString dataname="diff_bcmdd12/value", TString weight = "diff_bcmdd12/err") {

  // Plots a given variable and weight versus runlet for each wien.  Before using sign correction shoulf be set using SetSignCorr() function.  This flag defaults to 
  // true.  It is also recommended that flags such as title, axes names, and run period be set.

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  Int_t data_set_offset = 1;

  if(fRun1){
    data_set_offset = 1;
  }
  else if(fRun2){
    data_set_offset = 6;
  }
  else {
    std::cout << "Unknown run period. Exiting." << std::endl;
    exit(1);
  }

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  std::vector <TGraphErrors *> w;
  std::vector <TF1 *> fit;

  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);
  canvas->Divide(1,5);

  for(Int_t i = 0; i < 5; i++){
    GetDataByWien(i + data_set_offset, dataname, weight);

    if(GetLeafName(dataname).CompareTo("value", TString::kExact) == 0){

      // Fill error with '0' for the moment till I add the proper perscription for a weighted error
      // w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fValue.data(), 0, fError.data()));

      w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fValue.data(), 0, 0));
      ClearVectors();
    }
    else if(GetLeafName(dataname).CompareTo("rms", TString::kExact) == 0){
      w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fRMS.data(), 0, 0));
      ClearVectors();
    }
    else if(GetLeafName(dataname).CompareTo("err", TString::kExact) == 0){
      w.push_back(new TGraphErrors(fRunlet.size(), fRunlet.data(), fError.data(), 0, 0));
      ClearVectors();
    }
    else{
      std::cerr << "Unknown data type passed to QwDataQuality::PlotByWien.  Exiting." << std::endl;
      exit(1);
    }

    canvas->cd(i + 1);
    gPad->SetGrid();

    w[i]->SetTitle(fTitle);
    w[i]->GetXaxis()->SetTitle(fXaxisTitle);
    w[i]->GetYaxis()->SetTitle(fYaxisTitle);
    w[i]->SetMarkerColor(1);
    w[i]->SetMarkerStyle(20);
    w[i]->SetLineColor(9);
    w[i]->SetLineWidth(2);
    w[i]->Draw("ap");
    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "pol0"));
    FitGraphWithStats(w[i], fit[i], 0.85, 0.66, 0.99, 0.99);

  }
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

}

void QwDataQuality::PlotCorrelation(std::vector <Double_t> x, std::vector <Double_t> y, std::vector <Double_t> x_err, std::vector <Double_t> y_err) 
{
  // This function simply plots 2 variables versus each other.  I would like to add more functionality to this.  Data vectors can be built using FillDataVector()
  // and then passed to this function.  Axis extrema are not automatically determined yet so SetExtrema() functions above should be used if the default values 
  // are not sufficient. 

  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }


  TCanvas *canvas = new TCanvas("canvas","title", fYcanvas, fXcanvas);

  TGraphErrors *graph = new TGraphErrors(x.size(), x.data(), y.data(), x_err.data(), y_err.data());

    canvas->cd();
    gPad->SetGrid();

    graph->SetTitle(fTitle);
    graph->GetXaxis()->SetTitle(fXaxisTitle);
    graph->GetYaxis()->SetTitle(fYaxisTitle);
    graph->SetMarkerColor(1);
    graph->SetMarkerStyle(20);
    graph->SetLineColor(9);
    graph->SetLineWidth(2);
    graph->Draw("ap");
    TF1 *linear= new TF1("linear", "pol1");
    FitGraphWithStats(graph, linear, 0.85, 0.66, 0.99, 0.99);

  
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s.C", fOutputName.Data()));
  canvas->Close();

}

TString QwDataQuality::GetBranchName(TString name)
{

  std::string s_name(name.Data());

  Int_t index = s_name.find_first_of("/");

  TString b_name = s_name.substr(0, index).c_str();

  return b_name;

}

TString QwDataQuality::GetLeafName(TString name)
{

  std::string s_name(name.Data());
  
  Int_t index = s_name.find_first_of("/");
  
  TString l_name = s_name.substr(index + 1, s_name.length() - index).c_str();
  
  return l_name;

}

Double_t QwDataQuality::GetValue(Int_t index)
{
  // Return a given value from the interanl data vectors filled using FillDataVector().  This should not be used unless the FillDataVector funstion has been run.
  // A flag should be added to assure this is the case.

  return(fValue[index]);
}

Double_t QwDataQuality::GetRMS(Int_t index)
{
  // Return a given value from the interanl data vectors filled using FillDataVector().  This should not be used unless the FillDataVector funstion has been run.
  // A flag should be added to assure this is the case.

  return(fRMS[index]);
}

Double_t QwDataQuality::GetError(Int_t index)
{
  // Return a given value from the interanl data vectors filled using FillDataVector().  This should not be used unless the FillDataVector funstion has been run.
  // A flag should be added to assure this is the case.
  return(fError[index]);
}

void QwDataQuality::SetValue(std::vector <Double_t> value)
{
  // Set a given value from the interanl data vectors filled using FillDataVector().  This should not be used unless the FillDataVector funstion has been run.
  // A flag should be added to assure this is the case.

  Int_t nevents;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }
  // if(!fDataFilled){
  //   std::cerr << "Data vectors not yet filled." << std::endl;
  //   exit(1);
  // }

  nevents = (Int_t)fChain->GetEntries();
  
  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    fValue[i] = value[i];
  }
  
  return;
}

void QwDataQuality::SetError(std::vector <Double_t> error)
{
  // Return a given value from the interanl data vectors filled using FillDataVector().  This should not be used unless the FillDataVector funstion has been run.
  // A flag should be added to assure this is the case.

  Int_t nevents;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }
  // if(!fDataFilled){
  //   std::cerr << "Data vectors not yet filled." << std::endl;
  //   exit(1);
  // }

  nevents = (Int_t)fChain->GetEntries();
  
  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    fError[i] = error[i];
  }
  
  return;
}

void QwDataQuality::SetRMS(std::vector <Double_t> rms)
{
  // Return a given value from the interanl data vectors filled using FillDataVector().  This should not be used unless the FillDataVector funstion has been run.
  // A flag should be added to assure this is the case.

  Int_t nevents;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }
  // if(!fDataFilled){
  //   std::cerr << "Data vectors not yet filled." << std::endl;
  //   exit(1);
  // }

  nevents = (Int_t)fChain->GetEntries();
  
  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    fRMS[i] = rms[i];
  }
  
  return;
}

void QwDataQuality::GetDataByWien(Int_t wien, TString name = "diff_bcmdd12/value")
{
  //
  // Fill class data vectors with branch values.  There is no check on whether these have been previously filled so they need to be cleared using ClearVectors()
  // before another branch can be grabbed.
  //

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchStatus("slug", 1);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchStatus("sign_correction", 1);
  fChain->SetBranchStatus("run_number_decimal", 1);
  fChain->SetBranchStatus(GetBranchName(name), 1);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchAddress("sign_correction", &sign);

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue())  == 0 || wien_slug != wien )
      continue;
    else{
      if(fSignCorr) fValue.push_back(sign*(fChain->GetLeaf(Form("%s/value",GetBranchName(name).Data()))->GetValue()));
      else fValue.push_back((fChain->GetLeaf(Form("%s/value",GetBranchName(name).Data()))->GetValue()));
      fError.push_back(fChain->GetLeaf(Form("%s/err",GetBranchName(name).Data()))->GetValue());
      fRMS.push_back(fChain->GetLeaf(Form("%s/rms",GetBranchName(name).Data()))->GetValue());
      fN.push_back(fChain->GetLeaf(Form("%s/n",GetBranchName(name).Data()))->GetValue());
      fRunlet.push_back(fChain->GetLeaf("run_number_decimal")->GetValue());

      if(fStatError) fError.back() = fRMS.back()/TMath::Sqrt(fN.back());
    }
  }

  return;
}

void QwDataQuality::GetDataByWien(Int_t wien, TString name = "diff_bcmdd12/value", TString weight = "diff_bcmdd12/err")
{

  //
  // Fill class data vectors with weighted branch values.  There is no check on whether these have been previously filled so they need to be cleared using ClearVectors()
  // before another branch can be grabbed.
  //

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  Double_t weighted_value;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchStatus("slug", 1);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchStatus("sign_correction", 1);
  fChain->SetBranchStatus("run_number_decimal", 1);
  fChain->SetBranchStatus(GetBranchName(name), 1);
  fChain->SetBranchStatus(GetBranchName(weight), 1);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchAddress("sign_correction", &sign);

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }
  if(!(fChain->GetLeaf(weight))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", weight.Data()) << std::endl;
    exit(1);
  }

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue())  == 0 || wien_slug != wien )
      continue;
    else{
      if(fSignCorr) weighted_value = sign*(fChain->GetLeaf(Form("%s/value",GetBranchName(name).Data()))->GetValue());
      else weighted_value = (fChain->GetLeaf(Form("%s/value",GetBranchName(name).Data()))->GetValue());
      weighted_value /= (TMath::Power(fChain->GetLeaf(Form("%s",GetBranchName(weight).Data()))->GetValue(), 2));
      fValue.push_back(weighted_value);

      // Not sure how to handle the error and rms here : /

      fError.push_back(fChain->GetLeaf(Form("%s/err",GetBranchName(name).Data()))->GetValue());
      fRMS.push_back(fChain->GetLeaf(Form("%s/rms",GetBranchName(name).Data()))->GetValue());
      fRunlet.push_back(fChain->GetLeaf("run_number_decimal")->GetValue());
    }
  }

  return;
}

void QwDataQuality::FillPullByWien(Int_t wien, TString name, TH1F *hist, Double_t mean = 1)
{

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  Double_t difference;
  Double_t weight;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchStatus("slug", 1);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchStatus("sign_correction", 1);
  fChain->SetBranchStatus(GetBranchName(name), 1);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchAddress("sign_correction", &sign);

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue()) == 0 || wien_slug != wien )
      continue;
    else{
      difference = sign*(fChain->GetLeaf(name)->GetValue() - mean);
      weight = 1/(fChain->GetLeaf(Form("%s/err", GetBranchName(name).Data()))->GetValue());
      hist->Fill(difference*weight);
    }
  }

  return;
}

void QwDataQuality::FillHistoByWien(Int_t wien, TString name, TH1F *hist)
{

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchStatus("slug", 1);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchStatus("sign_correction", 1);
  fChain->SetBranchStatus(GetBranchName(name), 1);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchAddress("sign_correction", &sign);

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue()) == 0 || wien_slug != wien )
      continue;
    else{
      if(fSignCorr == true){
	hist->Fill(sign*(fChain->GetLeaf(name)->GetValue()));
	// std::cout << "Adding sign correction. " << fSignCorr << std::endl;
      }
      else{
	hist->Fill((fChain->GetLeaf(name)->GetValue()));
	// std::cout << "NOT Adding sign correction." << std::endl;
      }
    }
  }
}

void QwDataQuality::FillHistoByWien(Int_t wien, TString name, TH1F *hist, TString weight)
{
  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }
  if(!(fChain->GetLeaf(weight))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", weight.Data()) << std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchStatus("slug", 1);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchStatus("sign_correction", 1);
  fChain->SetBranchStatus(GetBranchName(name), 1);
  fChain->SetBranchStatus(GetBranchName(weight), 1);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchAddress("sign_correction", &sign);

  Double_t sum = 0 ;


  std::cout << "Sum: " << sum << std::endl;

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue()) == 0 || wien_slug != wien )
      continue;
    else{
      if(fSignCorr)
	hist->Fill(sign*(fChain->GetLeaf(name)->GetValue()), 1/TMath::Power((fChain->GetLeaf(weight)->GetValue()),2));
      else
	hist->Fill((fChain->GetLeaf(name)->GetValue()), 1/TMath::Power((fChain->GetLeaf(weight)->GetValue()),2));
    }
  }
}

void QwDataQuality::CalculateCorrelation(TString x, TString y)
{
  // This is no implemented yet.

  return;
}

void QwDataQuality::FillDataVector(std::vector <Double_t > &data, TString name ="asym_qwk_mdallbars/value")
{

  // Fill input data vector with leaf of your choice.

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t sign;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus(GetBranchName(name), 1);
  fChain->SetBranchStatus("sign_correction", 1);

  fChain->SetBranchAddress("sign_correction", &sign);


  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }


  for(Int_t i = 0; i < nevents; i++){
    fChain->GetEntry(i);
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue()) == 0 )
      continue;
    else{
      if(fSignCorr){
	data.push_back(sign*(fChain->GetLeaf(name)->GetValue()));
	std::cout << sign << std::endl;
      }
      else {
	data.push_back((fChain->GetLeaf(name)->GetValue()));
      }
    }
  }

  ClearVectors();
  return;

}

void QwDataQuality::FillDataVector(std::vector <Double_t > &data, TString name ="asym_qwk_mdallbars/value", Int_t wien = 1)
{

  // Fill input data vector with weighted leaf of your choice.

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t sign;

  if(!fFileOpen){
    std::cerr << "Rootfile not yet opened.  Exiting." <<std::endl;
    exit(1);
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus(GetBranchName(name), 1);
  fChain->SetBranchStatus("wien_slug", 1);
  fChain->SetBranchStatus("sign_correction", 1);

  fChain->SetBranchAddress("sign_correction", &sign);

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }


  for(Int_t i = 0; i < nevents; i++){
    fChain->GetEntry(i);
    if ( (fChain->GetLeaf(Form("%s/n", GetBranchName(name).Data()))->GetValue()) == 0 || (fChain->GetLeaf("wien_slug")->GetValue()) != wien)
      continue;
    else{
      if(fSignCorr){
	data.push_back(sign*(fChain->GetLeaf(name)->GetValue()));
      }
      else {
	data.push_back((fChain->GetLeaf(name)->GetValue()));
      }
    }
  }

  ClearVectors();
  return;

}

Bool_t QwDataQuality::FileSearch(TString filename, TChain *chain)
{

  TString file_directory;
  Bool_t c_status = false;

  file_directory = gSystem->Getenv("QW_ROOTFILES");
  
  c_status = fChain->Add(Form("%s/%s",file_directory.Data(), filename.Data()));
  std::cout << "Trying to open :: "
	    << Form("%s/%s",file_directory.Data(), filename.Data())
	    << std::endl;


  TString chain_name = fChain->GetName();
  TObjArray *fileElements = fChain->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chain_element = NULL;
  
  while((chain_element = (TChainElement*)next())){
    std::cout << "Adding :: "
	      << filename
	      << " to data chain"
	      << std::endl;
    
  }
  
  return c_status;
  
}

void QwDataQuality::LoadRootFile(TString filename, TChain *tree)
{
  Bool_t found = FileSearch(filename, tree);
  
  if(!found){
    std::cerr << "Unable to locate requested file :: "
	      << filename
	      << std::endl;
    exit(1);
    
  }
  fFileOpen = true;
}

void QwDataQuality::ClearVectors()
{

  fValue.clear();
  fWeight.clear();
  fError.clear();
  fRMS.clear();
  fN.clear();
  fRunlet.clear();

  return;
}

void QwDataQuality::DeleteHisto(TH1F *histo)
{
  histo->Delete();
  return;
}

void QwDataQuality::DeleteHisto(std::vector <TH1F *> histo)
{

  for(Int_t i = 0; i < (Int_t)histo.size(); i++){
    histo[i]->Delete();
  }
  return;
}

void QwDataQuality::FitGraphWithStats(TGraphErrors *graph, TF1* fit, Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  graph->Fit(fit,"sames");
  
  gPad->Modified();
  gPad->Update();
  
  TPaveStats * stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  stats->SetFillColor(0);
  stats->SetX1NDC(x1);
  stats->SetY1NDC(y1);
  stats->SetX2NDC(x2);
  stats->SetY2NDC(y2);
  stats->Draw("sames");
}

void QwDataQuality::FitGraphWithStats(TH1F *graph, TF1* fit, Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  graph->Fit(fit,"sames");
  
  gPad->Modified();
  gPad->Update();
  
  TPaveStats * stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  stats->SetFillColor(0);
  stats->SetX1NDC(x1);
  stats->SetY1NDC(y1);
  stats->SetX2NDC(x2);
  stats->SetY2NDC(y2);
  stats->Draw("sames");
}

#endif
