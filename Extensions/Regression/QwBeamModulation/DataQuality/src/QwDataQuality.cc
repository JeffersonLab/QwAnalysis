#define QwDataQuality_cxx
#include "QwDataQuality.hh"
#include "headers.h"
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

  fXmaximum =  2.5;
  fXminimum = -2.5;
  fYmaximum =  100;
  fYminimum =  100;
  fNbins    =  100;

  fFileOpen = false;

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
  fTitle = title;
  return;
}

void QwDataQuality::SetXaxisTitle(TString title)
{
  fXaxisTitle = title;
  return;
}

void QwDataQuality::SetYaxisTitle(TString title)
{
  fYaxisTitle = title;
  return;
}

void QwDataQuality::HistoByWien(TString dataname="diff_bcmdd12/value", Int_t data_set = 0 ) 
{
  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  Int_t data_set_offset = 0;
  
  if(data_set == 1){
    data_set_offset = 1;
  }
  else if(data_set == 2){
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

  TCanvas *canvas = new TCanvas("canvas","title", 5);
  canvas->Divide(1,5);
  
  for(Int_t i = 0; i < 5; i++){
    histo.push_back(new TH1F(Form("h%i", i + data_set),"title", fNbins, fXminimum, fXmaximum));
    FillHistoByWien(i +  data_set_offset, dataname, histo[i]);
    // blueHisto(histo[i]);

    canvas->cd(i + 1);
    histo[i]->SetTitle(fTitle);
    histo[i]->GetXaxis()->SetTitle(fXaxisTitle);
    histo[i]->GetYaxis()->SetTitle(fYaxisTitle);
    histo[i]->Draw();
    histo[i]->Fit("gaus");
  }

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s_histogram_by_wien.C", GetTitle().Data()));
}

void QwDataQuality::HistoByWien(TString dataname="diff_bcmdd12/value", Int_t data_set = 0, TString weight = "" ) {
  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  Int_t data_set_offset = 0;
  
  if(data_set == 1){
    data_set_offset = 1;
  }
  else if(data_set == 2){
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

  TCanvas *canvas = new TCanvas("canvas","title",5);
  canvas->Divide(1,5);
  
  for(Int_t i = 0; i < 5; i++){
    histo.push_back(new TH1F(Form("h%i", i + data_set),"title", fNbins, fXminimum, fXmaximum));
    FillHistoByWien(i + data_set_offset, dataname, histo[i], weight);
    // blueHisto(histo[i]);

    canvas->cd(i + 1);
    histo[i]->Draw();
    histo[i]->Fit("gaus");
  }

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

  canvas->SaveAs(Form("%s_histogram_by_wien_weighted_by_%s.C", GetTitle().Data(), weight.Data()));
}

void QwDataQuality::PlotByWien(TString dataname="diff_bcmdd12/value", Int_t data_set = 0) {
  gROOT->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  Int_t data_set_offset = 1;

  if(data_set == 1){
    data_set_offset = 1;
  }
  else if(data_set == 2){
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

  TCanvas *canvas = new TCanvas("canvas","title", 5);
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
    w[i]->Draw("ap");
    // placeAxis(title, xtitle, ytitle, canvas, w[i]);

    fit.push_back(new TF1(Form("fit%d", data_set_offset + i), "pol0"));
    // fitGraphWithStats(w[i], fit[i], 0.85, 0.66, 0.99, 0.99);

  }
  gPad->Modified();
  gPad->Update();
  canvas->SaveAs(Form("%s_%s_plotted_by_wien.C", GetBranchName(dataname).Data(), GetLeafName(dataname).Data()));

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

void QwDataQuality::GetDataByWien(Int_t wien, TString name)
{

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  if(!(fChain->GetLeaf(name))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
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

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/value", GetBranchName(name).Data()))->GetValue())  == -1e6 || wien_slug != wien )
      continue;
    else{
      fValue.push_back(sign*(fChain->GetLeaf(Form("%s/value",GetBranchName(name).Data()))->GetValue()));
      fError.push_back(fChain->GetLeaf(Form("%s/err",GetBranchName(name).Data()))->GetValue());
      fRMS.push_back(fChain->GetLeaf(Form("%s/rms",GetBranchName(name).Data()))->GetValue());
      fRunlet.push_back(fChain->GetLeaf("run_number_decimal")->GetValue());
    }
  }

  return;
}
/*
void QwDataQuality::GetDataByWien(Int_t wien, TString name, TString weight)
{

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

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
  fChain->SetBranchStatus(GetBranchName(weight), 1);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchAddress("sign_correction", &sign);

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/value", GetBranchName(name).Data()))->GetValue())  == -1e6 || wien_slug != wien )
      continue;
    else{
      fValue.push_back(sign*(fChain->GetLeaf(Form("%s.value",GetBranchName(name).Data()))->GetValue()));
      fWeight.push_back(sign*(fChain->GetLeaf(Form("%s.value",GetBranchName(weight).Data()))->GetValue()));
      fError.push_back(sign*(fChain->GetLeaf(Form("%s.error",GetBranchName(name).Data()))->GetValue()));
      fRMS.push_back(sign*(fChain->GetLeaf(Form("%s.rms",GetBranchName(name).Data()))->GetValue()));
    }
  }
}
*/
void QwDataQuality::FillHistoByWien(Int_t wien, TString name, TH1F *hist)
{

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

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
    
    if ( (fChain->GetLeaf(Form("%s/value", GetBranchName(name).Data()))->GetValue())  == -1e6 || wien_slug != wien )
      continue;
    else{
      hist->Fill(sign*(fChain->GetLeaf(name)->GetValue()));
    }
  }
}

void QwDataQuality::FillHistoByWien(Int_t wien, TString name, TH1F *hist, TString weight)
{
  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

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

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    
    if ( (fChain->GetLeaf(Form("%s/value", GetBranchName(name).Data()))->GetValue())  == -1e6 || wien_slug != wien )
      continue;
    else{
      hist->Fill(sign*(fChain->GetLeaf(name)->GetValue()), TMath::Power(1/(fChain->GetLeaf(weight)->GetValue()),2));
    }
  }
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
  fError.clear();
  fRMS.clear();
  fRunlet.clear();

  return;
}

#endif
