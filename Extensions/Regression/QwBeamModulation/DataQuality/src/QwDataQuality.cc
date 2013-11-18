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

#endif
