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

void QwDataQuality::HistoByWien(TString dataname="diff_bcmdd12", Int_t data_set = 0 ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
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

  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);
  
  for(Int_t i = 0; i < 5; i++){
    histo.push_back(new TH1F(Form("h%i", i + data_set),"title", fNbins, fXminimum, fXmaximum));
    FillHistoByWien(i, dataname, histo[i], "qwk_bcmdd12.value");
    // blueHisto(histo[i]);

    canvas->cd(i + 1);
    histo[i]->Draw();
    histo[i]->Fit("gaus");
  }

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();
}

TString QwDataQuality::GetBranchName(TString name)
{

  std::string s_name(name.Data());

  Int_t index = s_name.find_first_of(".");

  TString b_name = s_name.substr(0, index).c_str();

  return b_name;

}

TString QwDataQuality::GetLeafName(TString name)
{

  std::string s_name(name.Data());
  
  Int_t index = s_name.find_first_of(".");
  
  TString l_name = s_name.substr(index + 1, s_name.length() - index).c_str();
  
  return l_name;

}

void QwDataQuality::FillHistoByWien(Int_t wien, TString name, TH1F *hist, TString weight)
{

  Int_t nevents = (Int_t) fChain->GetEntries();
  Int_t wien_slug;
  Int_t slug;
  Int_t sign;

  Double_t value[4];
  Double_t w[4];

  Bool_t fSelfWeighted = false;

  TBranch *b_value;
  TBranch *b_weight;

  if(!(fChain->GetBranch(GetBranchName(name)))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", name.Data()) << std::endl;
    exit(1);
  }
  if(!(fChain->GetBranch(GetBranchName(weight)))){
    std::cout << Form("Cannot find : %s in tree.  Exiting.", weight.Data()) << std::endl;
    exit(1);
  }

  if(name.CompareTo(weight, TString::kExact) == 0){
    fSelfWeighted = true;
    std::cout << "Self Weighted" << std::endl;
  }

  fChain->ResetBranchAddresses();
  fChain->SetBranchStatus("*", 0);

  fChain->SetBranchAddress("slug", &slug);
  fChain->SetBranchStatus("slug", 1);

  fChain->SetBranchAddress("wien_slug", &wien_slug);
  fChain->SetBranchStatus("wien_slug", 1);

  fChain->SetBranchAddress("sign_correction", &sign);
  fChain->SetBranchStatus("sign_correction", 1);

  // fChain->SetBranchAddress(name, &value, &b_value);
  // fChain->SetBranchStatus(name, 1);

  // if(!fSelfWeighted){
  //   fChain->SetBranchAddress(weight, &w, &b_weight);
  //   fChain->SetBranchStatus(weight, 1);
  // }

  for( Int_t i = 0; i < nevents; i++ ) {
    fChain->GetEntry(i);
    std::cout << "Leaf value:" << fChain->GetLeaf(name)->GetValue() << std::endl;
    
    // if (  value[0]  == -1e6 || wien_slug != wien )
    //   continue;
    // std::cout << TMath::Power(1/value[1],2) << std::endl;
    // if(!fSelfWeighted){
    //   hist->Fill(sign*value[0], TMath::Power(1/value[1],2));
    // }
    // else{
    //   hist->Fill(sign*value[0], TMath::Power(1/w[1],2));
    // }
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
}

#endif
