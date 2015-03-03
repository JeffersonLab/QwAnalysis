
#define Neutral_Pedestal_cxx
#include "Neutral_Pedestal.h"
#include <TH2.h>
#include <TCut.h>
#include <TStyle.h>
#include <TCanvas.h>

void Neutral_Pedestal::Loop(Int_t cleanData)
{
//   In a ROOT session, you can do:
//      Root > .L Neutral_Pedestal.C
//      Root > Neutral_Pedestal t(13718,5,1);
//      Root > t.Loop(1);

  if (fChain == 0) return;

  // TDC Cuts
  Double_t MD_lo=-195;			// Main Detector Mean Time High threshold
  Double_t MD_hi=-178;			// MD MT Low threshold
  Double_t MD_rand_lo = -215;		// MD MT Low threshold for randoms
  Double_t MD_rand_hi = -198;		// " " High " " "
  Double_t TS_lo=-220;			// Trigger Scintillator MT High threshold
  Double_t TS_hi=-150;			// TS MT Low threshold
  Double_t MD_bar=20;			// Bars are 40 ns long so the difference of the TDCs should be between -20 and 20
  Double_t TS_bar=20;

  // Trigger Scintillator ADC Software Threshold
  Double_t TS_ADC_Average_lo = 250;
  Double_t TS_ADC_Individual_lo = 150;

  //////////
  // Cuts //
  //////////
  
  TCut EventType = "CodaEventType == 4";
  TCut CleanData = Form("Coda_CleanData == %i", cleanData);
 
  Int_t nentries = fChain->GetEntries();
  std::cout << nentries << std::endl;

  ////////////////
  // Histograms //
  ////////////////

  TH1F* hMD_ADC_Avg[8];
  TH1F* hMD_ADC_m[8];
  TH1F* hMD_ADC_p[8];
  TH1F* hMD_ADC_Avg_Sum;
  TH1F* hMD_ADC_m_Sum;
  TH1F* hMD_ADC_p_Sum;
  for(Int_t i=0; i<8; i++) {
    hMD_ADC_Avg[i] = new TH1F(Form("hMD_ADC_Avg_%i",i+1),Form("hMD_ADC_Avg_%i",i+1),500,0,500);
    hMD_ADC_m[i] = new TH1F(Form("hMD_ADC_m_%i",i+1),Form("hMD_ADC_m_%i",i+1),500,0,500);
    hMD_ADC_p[i] = new TH1F(Form("hMD_ADC_p_%i",i+1),Form("hMD_ADC_p_%i",i+1),500,0,500);
  }
  hMD_ADC_Avg_Sum = new TH1F("hMD_ADC_Avg_Sum","hMD_ADC_Avg_Sum",500,0,500);
  hMD_ADC_m_Sum = new TH1F("hMD_ADC_m_Sum","hMD_ADC_m_Sum",500,0,500);
  hMD_ADC_p_Sum = new TH1F("hMD_ADC_p_Sum","hMD_ADC_p_Sum",500,0,500);

  TCanvas* cAvg = new TCanvas("cAvg","cAvg",900,900); 
  cAvg->Divide(3,3);
  cAvg->cd(4);
  fChain->Draw("(maindet.md1m_adc+maindet.md1p_adc)/2.0 >> hMD_ADC_Avg_1", EventType && CleanData, "");
  cAvg->cd(1);
  fChain->Draw("(maindet.md2m_adc+maindet.md2p_adc)/2.0 >> hMD_ADC_Avg_2", EventType && CleanData, "");
  cAvg->cd(2);
  fChain->Draw("(maindet.md3m_adc+maindet.md3p_adc)/2.0 >> hMD_ADC_Avg_3", EventType && CleanData, "");
  cAvg->cd(3);
  fChain->Draw("(maindet.md4m_adc+maindet.md4p_adc)/2.0 >> hMD_ADC_Avg_4", EventType && CleanData, "");
  cAvg->cd(6);
  fChain->Draw("(maindet.md5m_adc+maindet.md5p_adc)/2.0 >> hMD_ADC_Avg_5", EventType && CleanData, "");
  cAvg->cd(9);
  fChain->Draw("(maindet.md6m_adc+maindet.md6p_adc)/2.0 >> hMD_ADC_Avg_6", EventType && CleanData, "");
  cAvg->cd(8);
  fChain->Draw("(maindet.md7m_adc+maindet.md7p_adc)/2.0 >> hMD_ADC_Avg_7", EventType && CleanData, "");
  cAvg->cd(7);
  fChain->Draw("(maindet.md8m_adc+maindet.md8p_adc)/2.0 >> hMD_ADC_Avg_8", EventType && CleanData, "");
  cAvg->cd(5);
  fChain->Draw("(maindet.md1m_adc+maindet.md1p_adc + maindet.md2m_adc+maindet.md2p_adc + maindet.md3m_adc+maindet.md3p_adc + maindet.md4m_adc+maindet.md4p_adc + maindet.md5m_adc+maindet.md5p_adc + maindet.md6m_adc+maindet.md6p_adc + maindet.md7m_adc+maindet.md7p_adc + maindet.md8m_adc+maindet.md8p_adc)/16.0 >> hMD_ADC_Avg_Sum", EventType && CleanData, "");

  TCanvas* cMinus = new TCanvas("cMinus","cMinus",900,900);
  cMinus->Divide(3,3);
  cMinus->cd(4);
  fChain->Draw("maindet.md1m_adc >> hMD_ADC_m_1", EventType && CleanData, "");
  cMinus->cd(1);
  fChain->Draw("maindet.md2m_adc >> hMD_ADC_m_2", EventType && CleanData, "");
  cMinus->cd(2);
  fChain->Draw("maindet.md3m_adc >> hMD_ADC_m_3", EventType && CleanData, "");
  cMinus->cd(3);
  fChain->Draw("maindet.md4m_adc >> hMD_ADC_m_4", EventType && CleanData, "");
  cMinus->cd(6);
  fChain->Draw("maindet.md5m_adc >> hMD_ADC_m_5", EventType && CleanData, "");
  cMinus->cd(9);
  fChain->Draw("maindet.md6m_adc >> hMD_ADC_m_6", EventType && CleanData, "");
  cMinus->cd(8);
  fChain->Draw("maindet.md7m_adc >> hMD_ADC_m_7", EventType && CleanData, "");
  cMinus->cd(7);
  fChain->Draw("maindet.md8m_adc >> hMD_ADC_m_8", EventType && CleanData, "");
  cMinus->cd(5);
  fChain->Draw("(maindet.md1m_adc + maindet.md2m_adc + maindet.md3m_adc + maindet.md4m_adc + maindet.md5m_adc + maindet.md6m_adc + maindet.md7m_adc + maindet.md8m_adc)/8.0 >> hMD_ADC_m_Sum", EventType && CleanData, "");

  TCanvas* cPlus = new TCanvas("cPlus","cPlus",900,900);
  cPlus->Divide(3,3);
  cPlus->cd(4);
  fChain->Draw("maindet.md1p_adc >> hMD_ADC_p_1", EventType && CleanData, "");
  cPlus->cd(1);
  fChain->Draw("maindet.md2p_adc >> hMD_ADC_p_2", EventType && CleanData, "");
  cPlus->cd(2);
  fChain->Draw("maindet.md3p_adc >> hMD_ADC_p_3", EventType && CleanData, "");
  cPlus->cd(3);
  fChain->Draw("maindet.md4p_adc >> hMD_ADC_p_4", EventType && CleanData, "");
  cPlus->cd(6);
  fChain->Draw("maindet.md5p_adc >> hMD_ADC_p_5", EventType && CleanData, "");
  cPlus->cd(9);
  fChain->Draw("maindet.md6p_adc >> hMD_ADC_p_6", EventType && CleanData, "");
  cPlus->cd(8);
  fChain->Draw("maindet.md7p_adc >> hMD_ADC_p_7", EventType && CleanData, "");
  cPlus->cd(7);
  fChain->Draw("maindet.md8p_adc >> hMD_ADC_p_8", EventType && CleanData, "");
  cPlus->cd(5);
  fChain->Draw("(maindet.md1p_adc + maindet.md2p_adc + maindet.md3p_adc + maindet.md4p_adc + maindet.md5p_adc + maindet.md6p_adc + maindet.md7p_adc + maindet.md8p_adc)/8.0 >> hMD_ADC_p_Sum", EventType && CleanData, "");

  for(Int_t i=0; i<8; i++) {
    std::cout << i+1 << " " << hMD_ADC_Avg[i]->GetEntries() << " " << hMD_ADC_Avg[i]->GetMean() << " " << hMD_ADC_Avg[i]->GetRMS() << " " << hMD_ADC_m[i]->GetEntries() << " " << hMD_ADC_m[i]->GetMean() << " " << hMD_ADC_m[i]->GetRMS() << " " << hMD_ADC_p[i]->GetEntries() << " " << hMD_ADC_p[i]->GetMean() << " " << hMD_ADC_p[i]->GetRMS() << std::endl;
  }
  std::cout << "Average " << hMD_ADC_Avg_Sum->GetEntries() << " " << hMD_ADC_Avg_Sum->GetMean() << " " << hMD_ADC_Avg_Sum->GetRMS() << " " << hMD_ADC_m_Sum->GetEntries() << " " << hMD_ADC_m_Sum->GetMean() << " " << hMD_ADC_m_Sum->GetRMS() << " " << hMD_ADC_p_Sum->GetEntries() << " " << hMD_ADC_p_Sum->GetMean() << " " << hMD_ADC_p_Sum->GetRMS() << std::endl;
}
