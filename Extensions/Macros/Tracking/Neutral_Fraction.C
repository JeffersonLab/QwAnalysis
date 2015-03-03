
#define Neutral_Fraction_cxx
#include "Neutral_Fraction.h"
#include <TH2.h>
#include <TCut.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>

void Neutral_Fraction::Loop(Int_t cleanData)
{
//   In a ROOT session, you can do:
//      Root > .L Neutral_Fraction.C
//      Root > Neutral_Fraction t(13718,5,1);
//      Root > t.Loop(1);

  if (fChain == 0) return;

  // TDC Cuts
  Double_t MD_lo=-195;			// Main Detector Mean Time High threshold
  Double_t MD_hi=-177;			// MD MT Low threshold
  Double_t MD_rand_lo = -215;		// MD MT Low threshold for randoms
  Double_t MD_rand_hi = -198;		// " " High " " "
  Double_t TS_lo=-220;			// Trigger Scintillator MT High threshold
  Double_t TS_hi=-150;			// TS MT Low threshold
  Double_t MD_bar=20;			// Bars are 40 ns long so the difference of the TDCs should be between -20 and 20
  Double_t TS_bar=20;

  // Trigger Scintillator ADC Software Threshold
  Double_t TS_ADC_Average_lo = 250;
  Double_t TS_ADC_Individual_lo = 150;

  Double_t MD_ADC_PED[8] = {217.7617, 187.8818, 231.3385, 222.4068, 220.5731, 217.6809, 243.2163, 216.6158};
  Double_t MD_ADC_PED_RMS[8] = {8.9288, 5.9984, 6.7054, 6.0123, 8.5503, 6.2634, 6.5034, 6.6540};

  //////////
  // Cuts //
  //////////
  
  TCut EventType = "CodaEventType == 4";
  TCut CleanData = Form("Coda_CleanData == %i", cleanData);

  // TS ADC avg Cuts
  TCut TS_ADC_Cut_1 = "(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0>0";
  TCut TS_ADC_Cut_2 = "(trigscint.ts2m_adc+trigscint.ts2p_adc)/2.0>0";
  TCut Charge_TS_ADC_Average_Cut_1 = Form("(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0>%f",TS_ADC_Average_lo);	// For Charge Events
  TCut Charge_TS_ADC_Average_Cut_2 = Form("(trigscint.ts2m_adc+trigscint.ts2p_adc)/2.0>%f",TS_ADC_Average_lo);
  
  // TS ADC individual cuts
  TCut Charge_TS_ADC_Individual_Cut_1 = Form("trigscint.ts1m_adc>%f || trigscint.ts1m_adc>%f", TS_ADC_Individual_lo,TS_ADC_Individual_lo);
  TCut Charge_TS_ADC_Individual_Cut_2 = Form("trigscint.ts2m_adc>%f || trigscint.ts2m_adc>%f", TS_ADC_Individual_lo,TS_ADC_Individual_lo);

  // TS TDC
  TCut TS_TDC_Cut_1 = "trigscint.ts1m_f1!=0 && trigscint.ts1p_f1!=0";
  TCut TS_TDC_Cut_2 = "trigscint.ts2m_f1!=0 && trigscint.ts2p_f1!=0";
  TCut TS_MT_Cut_1 = Form("%f<((trigscint.ts1m_f1+trigscint.ts1p_f1)/2.0) && ((trigscint.ts1m_f1+trigscint.ts1p_f1)/2.0)<%f",TS_lo,TS_hi);			// Mean Time
  TCut TS_MT_Cut_2 = Form("%f<((trigscint.ts2m_f1+trigscint.ts2p_f1)/2.0) && ((trigscint.ts2m_f1+trigscint.ts2p_f1)/2.0)<%f",TS_lo,TS_hi);
  TCut TS_DIFF_Cut_1 = Form("TMath::Abs(trigscint.ts1m_f1-trigscint.ts1p_f1)<%f",TS_bar);
  TCut TS_DIFF_Cut_2 = Form("TMath::Abs(trigscint.ts2m_f1-trigscint.ts2p_f1)<%f",TS_bar);

  // MD ADC
  TCut MD_ADC_Cut_1 = Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0>0",TS_Octant[0],TS_Octant[0]);
  TCut MD_ADC_Cut_2 = Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0>0",TS_Octant[1],TS_Octant[1]);
  
  // MD TDC
  TCut MD_TDC_Cut_1 = Form("maindet.md%im_f1!=0 && maindet.md%ip_f1!=0",TS_Octant[0],TS_Octant[0]);
  TCut MD_TDC_Cut_2 = Form("maindet.md%im_f1!=0 && maindet.md%ip_f1!=0",TS_Octant[1],TS_Octant[1]);
  TCut MD_MT_Cut_1 = Form("%f<((maindet.md%im_f1+maindet.md%ip_f1)/2.0) && ((maindet.md%im_f1+maindet.md%ip_f1)/2.0)<%f",MD_lo,TS_Octant[0],TS_Octant[0],TS_Octant[0],TS_Octant[0],MD_hi);
  TCut MD_MT_Cut_2 = Form("%f<((maindet.md%im_f1+maindet.md%ip_f1)/2.0) && ((maindet.md%im_f1+maindet.md%ip_f1)/2.0)<%f",MD_lo,TS_Octant[1],TS_Octant[1],TS_Octant[1],TS_Octant[1],MD_hi);
  TCut MD_DIFF_Cut_1 = Form("TMath::Abs(maindet.md%im_f1-maindet.md%ip_f1)<%f",TS_Octant[0],TS_Octant[0],MD_bar);
  TCut MD_DIFF_Cut_2 = Form("TMath::Abs(maindet.md%im_f1-maindet.md%ip_f1)<%f",TS_Octant[1],TS_Octant[1],MD_bar);
  TCut MD_MT_Rand_Cut_1 = Form("-400.0<((maindet.md%im_f1+maindet.md%ip_f1)/2.0) && ((maindet.md%im_f1+maindet.md%ip_f1)/2.0)<0",TS_Octant[0],TS_Octant[0],TS_Octant[0],TS_Octant[0]);
  TCut MD_MT_Rand_Cut_2 = Form("-400.0<((maindet.md%im_f1+maindet.md%ip_f1)/2.0) && ((maindet.md%im_f1+maindet.md%ip_f1)/2.0)<0",TS_Octant[1],TS_Octant[1],TS_Octant[1],TS_Octant[1]);

  // Good MD && Good TS Events
  TCut Good_MD_1 = MD_MT_Cut_1 && MD_DIFF_Cut_1 && MD_TDC_Cut_1 && EventType && CleanData;
  TCut Good_MD_2 = MD_MT_Cut_2 && MD_DIFF_Cut_2 && MD_TDC_Cut_2 && EventType && CleanData;
  TCut Rand_MD_1 = MD_MT_Rand_Cut_1 && !MD_MT_Cut_1 && MD_DIFF_Cut_1 && MD_TDC_Cut_1 && EventType && CleanData;
  TCut Rand_MD_2 = MD_MT_Rand_Cut_2 && !MD_MT_Cut_2 && MD_DIFF_Cut_2 && MD_TDC_Cut_2 && EventType && CleanData;
  TCut Good_TS_1 = Charge_TS_ADC_Average_Cut_1;
  TCut Good_TS_2 = Charge_TS_ADC_Average_Cut_2;
  
  //Int_t nentries = fChain->GetEntries();
  //std::cout << nentries << std::endl;

  ////////////////
  // Histograms //
  ////////////////

  Int_t nBins = 350; 			// # of bins in ADC Histograms
  
  // Important Histograms
  TH1F* hMD_Ped[2];
  hMD_Ped[0] = new TH1F(Form("hMD_Ped_%i",TS_Octant[0]),Form("hMD_Ped_%i",TS_Octant[0]),500,0,500);
  hMD_Ped[1] = new TH1F(Form("hMD_Ped_%i",TS_Octant[1]),Form("hMD_Ped_%i",TS_Octant[1]),500,0,500);
  TH1F* hMD_ADC_Good[2];
  TH1F* hMD_ADC_Charge[2];
  TH1F* hMD_ADC_Neutral[2];
  TH1F* hMD_ADC_Good_Raw[2];
  TH1F* hMD_ADC_Charge_Raw[2];
  TH1F* hMD_ADC_Neutral_Raw[2];
  TH1F* hMD_ADC_Good_Rand[2];
  TH1F* hMD_ADC_Charge_Rand[2];
  TH1F* hMD_ADC_Neutral_Rand[2];
  TH1F* hMD_ADC_Good_Correct[2];
  TH1F* hMD_ADC_Charge_Correct[2];
  TH1F* hMD_ADC_Neutral_Correct[2];
  for(Int_t i=0; i<2; i++) {
    hMD_ADC_Good[i] = new TH1F(Form("hMD_ADC_Good_%i",TS_Octant[i]),Form("hMD_ADC_Good_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Charge[i] = new TH1F(Form("hMD_ADC_Charge_%i",TS_Octant[i]),Form("hMD_ADC_Charge_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Neutral[i] = new TH1F(Form("hMD_ADC_Neutral_%i",TS_Octant[i]),Form("hMD_ADC_Neutral_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Good_Raw[i] = new TH1F(Form("hMD_ADC_Good_Raw_%i",TS_Octant[i]),Form("hMD_ADC_Good_Raw_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Charge_Raw[i] = new TH1F(Form("hMD_ADC_Charge_Raw_%i",TS_Octant[i]),Form("hMD_ADC_Charge_Raw_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Neutral_Raw[i] = new TH1F(Form("hMD_ADC_Neutral_Raw_%i",TS_Octant[i]),Form("hMD_ADC_Neutral_Raw_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Good_Rand[i] = new TH1F(Form("hMD_ADC_Good_Rand_%i",TS_Octant[i]),Form("hMD_ADC_Good_Rand_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Charge_Rand[i] = new TH1F(Form("hMD_ADC_Charge_Rand_%i",TS_Octant[i]),Form("hMD_ADC_Charge_Rand_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Neutral_Rand[i] = new TH1F(Form("hMD_ADC_Neutral_Rand_%i",TS_Octant[i]),Form("hMD_ADC_Neutral_Rand_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Good_Correct[i] = new TH1F(Form("hMD_ADC_Good_Correct_%i",TS_Octant[i]),Form("hMD_ADC_Good_Correct_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Charge_Correct[i] = new TH1F(Form("hMD_ADC_Charge_Correct_%i",TS_Octant[i]),Form("hMD_ADC_Charge_Correct_%i",TS_Octant[i]),nBins,0,3500);
    hMD_ADC_Neutral_Correct[i] = new TH1F(Form("hMD_ADC_Neutral_Correct_%i",TS_Octant[i]),Form("hMD_ADC_Neutral_Correct_%i",TS_Octant[i]),nBins,0,3500);
  }

  /* Nice Plots
  TH1F* hTS_ADC[4];
  TH1F* hMD_ADC[4];
  for(Int_t i=0; i<4; i++) {
    hTS_ADC[i] = new TH1F(Form("hTS_ADC_%i",i+1),Form("hTS_ADC_%i",i+1),nBins,0,3500);
    hMD_ADC[i] = new TH1F(Form("hMD_ADC_%i",i+1),Form("hMD_ADC_%i",i+1),nBins,0,3500);
  }
  

  // Diagnostic Histograms
  // 1D Histograms
  TH1F* hMD_MT_Good = new TH1F("hMD_MT_Good","hMD_MT_Good",400,-400,0);
  TH1F* hMD_MT_Rand = new TH1F("hMD_MT_Rand","hMD_MT_Rand",400,-400,0);
  TH1F* hTS_ADC_Raw[2];
  TH1F* hMD_ADC_Raw[2];
  TH1F* hTS_ADC_Good[2];
  TH1F* hTS_MT_Raw[2];
  TH1F* hMD_MT_Raw[2];
  TH1F* hTS_DIFF_Raw[2];
  TH1F* hMD_DIFF_Raw[2];
  // 2D Histograms
  TH2F* hTS_MT_DIFF[2];
  TH2F* hMD_MT_DIFF[2];
  TH2F* hTS_ADC_MT[2];
  TH2F* hMD_ADC_MT[2];
  TH2F* hTS_ADC_LR[2];
  TH2F* hMD_ADC_LR[2];
  for(Int_t i=0; i<2; i++) {
    hTS_ADC_Raw[i] = new TH1F(Form("hTS_ADC_Raw_%i",i+1),Form("hTS_ADC_Raw_%i",i+1),nBins,0,3500);
    hMD_ADC_Raw[i] = new TH1F(Form("hMD_ADC_Raw_%i",TS_Octant[i]),Form("hMD_ADC_Raw_%i",TS_Octant[i]),nBins,0,3500);
    hTS_ADC_Good[i] = new TH1F(Form("hTS_ADC_Good_%i",i+1),Form("hTS_ADC_Good_%i",i+1),500,0,5000);
    hTS_MT_Raw[i] = new TH1F(Form("hTS_MT_Raw_%i",i+1),Form("hTS_MT_Raw_%i",i+1),210,-800,1300);
    hMD_MT_Raw[i] = new TH1F(Form("hMD_MT_Raw_%i",TS_Octant[i]),Form("hMD_MT_Raw_%i",TS_Octant[i]),210,-800,1300);
    hTS_DIFF_Raw[i] = new TH1F(Form("hTS_DIFF_Raw_%i",i+1),Form("hTS_DIFF_Raw_%i",i+1),440,-2200,2200);
    hMD_DIFF_Raw[i] = new TH1F(Form("hMD_DIFF_Raw_%i",TS_Octant[i]),Form("hMD_DIFF_Raw_%i",TS_Octant[i]),440,-2200,2200);
    hTS_MT_DIFF[i] = new TH2F(Form("hTS_MT_DIFF_%i",i+1),Form("hTS_MT_DIFF_%i",i+1),440,-2200,2200,210,-800,1300);
    hMD_MT_DIFF[i] = new TH2F(Form("hMD_MT_DIFF_%i",TS_Octant[i]),Form("hMD_MT_DIFF_%i",TS_Octant[i]),440,-2200,2200,210,-800,1300);
    hTS_ADC_MT[i] = new TH2F(Form("hTS_ADC_MT_%i",i+1),Form("hTS_ADC_MT_%i",i+1),210,-800,1300,500,0,5000);
    hMD_ADC_MT[i] = new TH2F(Form("hMD_ADC_MT_%i",TS_Octant[i]),Form("hMD_ADC_MT_%i",TS_Octant[i]),210,-800,1300,500,0,5000);
    hTS_ADC_LR[i] = new TH2F(Form("hTS_ADC_LR_%i",i+1),Form("hTS_ADC_LR_%i",i+1),500,0,5000,500,0,5000);
    hMD_ADC_LR[i] = new TH2F(Form("hMD_ADC_LR_%i",TS_Octant[i]),Form("hMD_ADC_LR_%i",TS_Octant[i]),500,0,5000,500,0,5000);
  }
    
  TH1F* hTS_ADC[3];
  TH1F* hMD_ADC[3];
  TH2F* hTS_PMT[3];
  TH2F* hMD_PMT[3];
  for(Int_t i=0; i<3; i++) {
    hTS_ADC[i] = new TH1F(Form("hTS_ADC_%i",i+1),Form("hTS_ADC_%i",i+1),500,0,5000);
    hMD_ADC[i] = new TH1F(Form("hMD_ADC_%i",i+1),Form("hMD_ADC_%i",i+1),500,0,5000);
    hTS_PMT[i] = new TH2F(Form("hTS_PMT_%i",i+1),Form("hTS_PMT_%i",i+1),500,0,5000,500,0,5000);
    hMD_PMT[i] = new TH2F(Form("hMD_PMT_%i",i+1),Form("hMD_PMT_%i",i+1),500,0,5000,500,0,5000);
  }
  */

  ///////////////////////
  // IMPORTANT DATAAAA //
  ///////////////////////

  // MD ADC Pedestals
  TCanvas* cMD_Ped = new TCanvas("cMD_Ped","cMD_Ped",800,400);
  cMD_Ped->Divide(2,1);
  cMD_Ped->cd(1);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)2.0 >> hMD_Ped_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]),Good_MD_2 && !Good_MD_1,"");
  cMD_Ped->cd(2);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)2.0 >> hMD_Ped_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]),Good_MD_1 && !Good_MD_2,"");
  cMD_Ped->Print(Form("Pedestal_%i.png",run));

  Double_t Pedestal[2];
  Double_t PedestalErr[2];
  Pedestal[0] = hMD_Ped[0]->GetMean();   PedestalErr[0] = hMD_Ped[0]->GetMeanError();
  Pedestal[1] = hMD_Ped[1]->GetMean();   PedestalErr[1] = hMD_Ped[1]->GetMeanError();

  // MD ADC Raw Plots (no pedestal subtraction)
  TCanvas* cMD_ADC_Raw = new TCanvas("cMD_ADC_Raw","cMD_ADC_Raw",1200,800);
  cMD_ADC_Raw->Divide(3,2);
  cMD_ADC_Raw->cd(1);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_Good_Raw_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]),Good_MD_1,"");
  cMD_ADC_Raw->cd(2);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_Charge_Raw_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]),Good_MD_1 && Good_TS_1,"");
  cMD_ADC_Raw->cd(3);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_Neutral_Raw_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]),Good_MD_1 && !Good_TS_1,"");
  cMD_ADC_Raw->cd(4);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_Good_Raw_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]),Good_MD_2,"");
  cMD_ADC_Raw->cd(5);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_Charge_Raw_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]),Good_MD_2 && Good_TS_2,"");
  cMD_ADC_Raw->cd(6);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_Neutral_Raw_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]),Good_MD_2 && !Good_TS_2,"");


  std::cout << run << " " << TS_Octant[0] << " " << hMD_Ped[0]->GetEntries() << " " << Pedestal[0] << " " << PedestalErr[0] << " " << hMD_Ped[0]->GetRMS() << " " << hMD_ADC_Good_Raw[0]->GetEntries() << " " << hMD_ADC_Good_Raw[0]->GetMean() << " " << hMD_ADC_Good_Raw[0]->GetRMS() << " " << hMD_ADC_Neutral_Raw[0]->GetEntries() << " " << hMD_ADC_Neutral_Raw[0]->GetMean() << " " << hMD_ADC_Neutral_Raw[0]->GetRMS() << " " 
                   << " " << TS_Octant[1] << " " << hMD_Ped[1]->GetEntries() << " " << Pedestal[1] << " " << PedestalErr[1] << " " << hMD_Ped[1]->GetRMS() << " " << hMD_ADC_Good_Raw[1]->GetEntries() << " " << hMD_ADC_Good_Raw[1]->GetMean() << " " << hMD_ADC_Good_Raw[1]->GetRMS() << " " << hMD_ADC_Neutral_Raw[1]->GetEntries() << " " << hMD_ADC_Neutral_Raw[1]->GetMean() << " " << hMD_ADC_Neutral_Raw[1]->GetRMS() << std::endl;

  // MD ADC Plots
  TCanvas* cMD_ADC = new TCanvas("cMD_ADC","cMD_ADC",1200,800);
  cMD_ADC->Divide(3,2);
  cMD_ADC->cd(1);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Good_%i",TS_Octant[0],TS_Octant[0],Pedestal[0],TS_Octant[0]),Good_MD_1,"");
  cMD_ADC->cd(2);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Charge_%i",TS_Octant[0],TS_Octant[0],Pedestal[0],TS_Octant[0]),Good_MD_1 && Good_TS_1,"");
  cMD_ADC->cd(3);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Neutral_%i",TS_Octant[0],TS_Octant[0],Pedestal[0],TS_Octant[0]),Good_MD_1 && !Good_TS_1,"");
  cMD_ADC->cd(4);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Good_%i",TS_Octant[1],TS_Octant[1],Pedestal[1],TS_Octant[1]),Good_MD_2,"");
  cMD_ADC->cd(5);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Charge_%i",TS_Octant[1],TS_Octant[1],Pedestal[1],TS_Octant[1]),Good_MD_2 && Good_TS_2,"");
  cMD_ADC->cd(6);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Neutral_%i",TS_Octant[1],TS_Octant[1],Pedestal[1],TS_Octant[1]),Good_MD_2 && !Good_TS_2,"");

  // MD RAND ADC Plots
  TCanvas* cMD_RAND = new TCanvas("cMD_RAND","cMD_RAND",1200,800);
  cMD_RAND->Divide(3,2);
  cMD_RAND->cd(1);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Good_Rand_%i",TS_Octant[0],TS_Octant[0],Pedestal[0],TS_Octant[0]), Rand_MD_1,"");
  cMD_RAND->cd(2);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Charge_Rand_%i",TS_Octant[0],TS_Octant[0],Pedestal[0],TS_Octant[0]), Rand_MD_1 && Good_TS_1 ,"");
  cMD_RAND->cd(3);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Neutral_Rand_%i",TS_Octant[0],TS_Octant[0],Pedestal[0],TS_Octant[0]), Rand_MD_1 && !Good_TS_1 ,"");
  cMD_RAND->cd(4);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Good_Rand_%i",TS_Octant[1],TS_Octant[1],Pedestal[1],TS_Octant[1]), Rand_MD_2,"");
  cMD_RAND->cd(5);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Charge_Rand_%i",TS_Octant[1],TS_Octant[1],Pedestal[1],TS_Octant[1]), Rand_MD_2 && Good_TS_2 ,"");
  cMD_RAND->cd(6);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Neutral_Rand_%i",TS_Octant[1],TS_Octant[1],Pedestal[1],TS_Octant[1]), Rand_MD_2 && !Good_TS_2 ,"");

  //Fill the Corrected histograms
  Double_t Random_Scale_Factor = 18.0/383.0;
  for(Int_t i=1; i<=nBins; i++) {
    hMD_ADC_Good_Correct[0]->SetBinContent(i, hMD_ADC_Good[0]->GetBinContent(i) - Random_Scale_Factor*hMD_ADC_Good_Rand[0]->GetBinContent(i));
    hMD_ADC_Good_Correct[1]->SetBinContent(i, hMD_ADC_Good[1]->GetBinContent(i) - Random_Scale_Factor*hMD_ADC_Good_Rand[1]->GetBinContent(i));
    hMD_ADC_Charge_Correct[0]->SetBinContent(i, hMD_ADC_Charge[0]->GetBinContent(i) - Random_Scale_Factor*hMD_ADC_Charge_Rand[0]->GetBinContent(i));
    hMD_ADC_Charge_Correct[1]->SetBinContent(i, hMD_ADC_Charge[1]->GetBinContent(i) - Random_Scale_Factor*hMD_ADC_Charge_Rand[1]->GetBinContent(i));
    hMD_ADC_Neutral_Correct[0]->SetBinContent(i, hMD_ADC_Neutral[0]->GetBinContent(i) - Random_Scale_Factor*hMD_ADC_Neutral_Rand[0]->GetBinContent(i));
    hMD_ADC_Neutral_Correct[1]->SetBinContent(i, hMD_ADC_Neutral[1]->GetBinContent(i) - Random_Scale_Factor*hMD_ADC_Neutral_Rand[1]->GetBinContent(i));
    if( hMD_ADC_Good_Correct[0]->GetBinContent(i)<0 ) hMD_ADC_Good_Correct[0]->SetBinContent(i,0.0);
    if( hMD_ADC_Good_Correct[1]->GetBinContent(i)<0 ) hMD_ADC_Good_Correct[1]->SetBinContent(i,0.0);
    if( hMD_ADC_Charge_Correct[0]->GetBinContent(i)<0 ) hMD_ADC_Charge_Correct[0]->SetBinContent(i,0.0);
    if( hMD_ADC_Charge_Correct[1]->GetBinContent(i)<0 ) hMD_ADC_Charge_Correct[1]->SetBinContent(i,0.0);
    if( hMD_ADC_Neutral_Correct[0]->GetBinContent(i)<0 ) hMD_ADC_Neutral_Correct[0]->SetBinContent(i,0.0);
    if( hMD_ADC_Neutral_Correct[1]->GetBinContent(i)<0 ) hMD_ADC_Neutral_Correct[1]->SetBinContent(i,0.0);
  }

  std::cout << run << " " << TS_Octant[0] << " " << hMD_ADC_Good_Correct[0]->GetEntries() << " " << hMD_ADC_Good_Correct[0]->GetMean() << " " << hMD_ADC_Good_Correct[0]->GetMeanError() << " " << hMD_ADC_Neutral_Correct[0]->GetEntries() << " " << hMD_ADC_Neutral_Correct[0]->GetMean() << " " << hMD_ADC_Neutral_Correct[0]->GetMeanError() << " "
                   << " " << TS_Octant[1] << " " << hMD_ADC_Good_Correct[1]->GetEntries() << " " << hMD_ADC_Good_Correct[1]->GetMean() << " " << hMD_ADC_Good_Correct[1]->GetMeanError() << " " << hMD_ADC_Neutral_Correct[1]->GetEntries() << " " << hMD_ADC_Neutral_Correct[1]->GetMean() << " " << hMD_ADC_Neutral_Correct[1]->GetMeanError() << std::endl;

  TCanvas* cMD_CORRECT = new TCanvas("cMD_CORRECT","cMD_CORRECT",1200,800);
  cMD_CORRECT->Divide(3,2);
  cMD_CORRECT->cd(1);
  hMD_ADC_Good_Correct[0]->Draw();
  cMD_CORRECT->cd(2);
  hMD_ADC_Charge_Correct[0]->Draw();
  cMD_CORRECT->cd(3);
  hMD_ADC_Neutral_Correct[0]->Draw();
  cMD_CORRECT->cd(4);
  hMD_ADC_Good_Correct[1]->Draw();
  cMD_CORRECT->cd(5);
  hMD_ADC_Charge_Correct[1]->Draw();
  cMD_CORRECT->cd(6);
  hMD_ADC_Neutral_Correct[1]->Draw();
  
  ////////////////////////////
  // Calculate the Dilution //
  ////////////////////////////

  Double_t f_n[2];
  Double_t df_n[2];
  
  /* Corrected Rakitha Method (Average ADC times number of hits minus pedestal and random hits)
  Double_t nNeut[2];
  Double_t nNeut_Rand[2];
  Double_t nTot[2];
  Double_t nTot_Rand[2];
  Double_t adcMeanNeut[2];
  Double_t adcRMSNeut[2];
  Double_t adcMeanTot[2];
  Double_t adcRMSTot[2];
  
  nNeut[0] = hMD_ADC_Neutral_Raw[0]->GetEntries();
  nNeut[1] = hMD_ADC_Neutral_Raw[1]->GetEntries();
  nNeut_Rand[0] = Random_Scale_Factor*hMD_ADC_Neutral_Rand[0]->GetEntries();
  nNeut_Rand[1] = Random_Scale_Factor*hMD_ADC_Neutral_Rand[1]->GetEntries();
  nTot[0] = hMD_ADC_Good_Raw[0]->GetEntries();
  nTot[1] = hMD_ADC_Good_Raw[1]->GetEntries();
  nTot_Rand[0] = Random_Scale_Factor*hMD_ADC_Good_Rand[0]->GetEntries();
  nTot_Rand[1] = Random_Scale_Factor*hMD_ADC_Good_Rand[1]->GetEntries(); 
  adcMeanNeut[0] = hMD_ADC_Neutral_Raw[0]->GetMean();
  adcMeanNeut[1] = hMD_ADC_Neutral_Raw[1]->GetMean();
  adcRMSNeut[0] = hMD_ADC_Neutral_Raw[0]->GetMeanError();
  adcRMSNeut[1] = hMD_ADC_Neutral_Raw[1]->GetMeanError();
  adcMeanTot[0] = hMD_ADC_Good_Raw[0]->GetMean();
  adcMeanTot[1] = hMD_ADC_Good_Raw[1]->GetMean();
  adcRMSTot[0] = hMD_ADC_Good_Raw[0]->GetMeanError(); 
  adcRMSTot[1] = hMD_ADC_Good_Raw[1]->GetMeanError();
  Pedestal[0] = MD_ADC_PED[TS_Octant[0]-1];
  Pedestal[1] = MD_ADC_PED[TS_Octant[1]-1];
  PedestalErr[0] = MD_ADC_PED_RMS[TS_Octant[0]-1];
  PedestalErr[1] = MD_ADC_PED_RMS[TS_Octant[1]-1];

  f_n[0] = ((nNeut[0]-nNeut_Rand[0])*(adcMeanNeut[0]-Pedestal[0]))/((nTot[0]-nTot_Rand[0])*(adcMeanTot[0]-Pedestal[0]));
  f_n[1] = ((nNeut[1]-nNeut_Rand[1])*(adcMeanNeut[1]-Pedestal[1]))/((nTot[1]-nTot_Rand[1])*(adcMeanTot[1]-Pedestal[1]));

  std::cout << nNeut[0] << " " << nNeut_Rand[0] << " " << adcMeanNeut[0] << " " << Pedestal[0] << " " << nTot[0] << " " << nTot_Rand[0] << " " << adcMeanTot[0] << std::endl;

  df_n[0] += (nNeut[0]+nNeut_Rand[0])/((nNeut[0]-nNeut_Rand[0])*(nNeut[0]-nNeut_Rand[0]));
  df_n[0] += (nTot[0]+nTot_Rand[0])/((nTot[0]-nTot_Rand[0])*(nTot[0]-nTot_Rand[0]));
  df_n[0] += (adcRMSNeut[0]*adcRMSNeut[0] + PedestalErr[0]*PedestalErr[0])/((adcRMSNeut[0]-PedestalErr[0])*(adcRMSNeut[0]-PedestalErr[0]));
  df_n[0] += (adcRMSTot[0]*adcRMSTot[0] + PedestalErr[0]*PedestalErr[0])/((adcRMSTot[0]-PedestalErr[0])*(adcRMSTot[0]-PedestalErr[0]));
  df_n[0] = TMath::Sqrt(df_n[0]);
  df_n[0] = f_n[0]*df_n[0];

  df_n[1] += (nNeut[1]+nNeut_Rand[1])/((nNeut[1]-nNeut_Rand[1])*(nNeut[1]-nNeut_Rand[1]));
  df_n[1] += (nTot[1]+nTot_Rand[1])/((nTot[1]-nTot_Rand[1])*(nTot[1]-nTot_Rand[1]));
  df_n[1] += (adcRMSNeut[1]*adcRMSNeut[1] + PedestalErr[1]*PedestalErr[1])/((adcRMSNeut[1]-PedestalErr[1])*(adcRMSNeut[1]-PedestalErr[1]));
  df_n[1] += (adcRMSTot[1]*adcRMSTot[1] + PedestalErr[1]*PedestalErr[1])/((adcRMSTot[1]-PedestalErr[1])*(adcRMSTot[1]-PedestalErr[1]));
  df_n[1] = TMath::Sqrt(df_n[1]);
  df_n[1] = f_n[1]*df_n[1];
  
  std::cout << "Rakitha: " << run << " " << TS_Octant[0] << " " << f_n[0] << " " << df_n[0] << " " << run << " " << TS_Octant[1] << " " << f_n[1] << " " << df_n[1] << std::endl;  
  */

  // Dalton Method (light-weighted neutrals over light-weighted total)
  Double_t Sum_NL_neut[2];
  Double_t Sum_NL_tot[2];
  Double_t Error_Sum_NL_neut[2];
  Double_t Error_Sum_NL_tot[2];
  for(Int_t i=0; i<=nBins; i++) {

    Double_t N_neut_i = 0;
    Double_t N_tot_i = 0;
    Double_t L_i = hMD_ADC_Good_Correct[0]->GetBinCenter(i);
    Double_t dL_i = 0;

    // TS 1 Octant 
    N_neut_i = hMD_ADC_Neutral_Correct[0]->GetBinContent(i);
    Sum_NL_neut[0] += N_neut_i*L_i; 
    N_tot_i = hMD_ADC_Good_Correct[0]->GetBinContent(i);
    Sum_NL_tot[0] += N_tot_i*L_i;
    Error_Sum_NL_neut[0] += N_neut_i*N_neut_i*dL_i + N_neut_i*L_i*L_i;
    Error_Sum_NL_tot[0] += N_tot_i*N_tot_i*dL_i + N_tot_i*L_i*L_i;
    
    // TS 2 Octant 
    N_neut_i = hMD_ADC_Neutral_Correct[1]->GetBinContent(i);
    Sum_NL_neut[1] += N_neut_i*L_i; 
    N_tot_i = hMD_ADC_Good_Correct[1]->GetBinContent(i);
    Sum_NL_tot[1] += N_tot_i*L_i;
    Error_Sum_NL_neut[1] += N_neut_i*N_neut_i*dL_i + N_neut_i*L_i*L_i;
    Error_Sum_NL_tot[1] += N_tot_i*N_tot_i*dL_i + N_tot_i*L_i*L_i;

  }

  // Calculate total dilution (Dalton Method)
  f_n[0] = Sum_NL_neut[0]/Sum_NL_tot[0];
  df_n[0] = f_n[0]*f_n[0]*( Error_Sum_NL_neut[0]/(Sum_NL_neut[0]*Sum_NL_neut[0]) + Error_Sum_NL_tot[0]/(Sum_NL_tot[0]*Sum_NL_tot[0]) );
  df_n[0] = TMath::Sqrt(df_n[0]);
  f_n[1] = Sum_NL_neut[1]/Sum_NL_tot[1];
  df_n[1] = f_n[1]*f_n[1]*( Error_Sum_NL_neut[1]/(Sum_NL_neut[1]*Sum_NL_neut[1]) + Error_Sum_NL_tot[1]/(Sum_NL_tot[1]*Sum_NL_tot[1]) );
  df_n[1] = TMath::Sqrt(df_n[1]);

  //std::cout << run << " " << TS_Octant[0] << " " << f_n[0] << " " << df_n[0] << " " << run << " " << TS_Octant[1] << " " << f_n[1] << " " << df_n[1] << std::endl;  
  std::cout << run << " " << TS_Octant[0] << " " << Sum_NL_neut[0] << " " << TMath::Sqrt(Error_Sum_NL_neut[0]) << " " << Sum_NL_tot[0] << " " << TMath::Sqrt(Error_Sum_NL_tot[0]) << " " << f_n[0] << " " << df_n[0] << " "
                          << TS_Octant[1] << " " << Sum_NL_neut[1] << " " << TMath::Sqrt(Error_Sum_NL_neut[1]) << " " << Sum_NL_tot[1] << " " << TMath::Sqrt(Error_Sum_NL_tot[1]) << " " << f_n[1] << " " << df_n[1] << std::endl;

  /////////////////
  // DIAGNOSTICS //
  /////////////////
  /*
  gStyle->SetOptStat(0);
  
  // Nice Pictures of Mean Time
  TCanvas* cMeanTime = new TCanvas("cMeanTime","cMeanTime",800,800);
  fChain->Draw(Form("(maindet.md%im_f1+maindet.md%ip_f1)/2.0 >> hMD_MT_Good",TS_Octant[0],TS_Octant[0]),Good_MD_1,"");
  hMD_MT_Good->SetLineColor(36);
  hMD_MT_Good->SetFillColor(36);
  fChain->Draw(Form("(maindet.md%im_f1+maindet.md%ip_f1)/2.0 >> hMD_MT_Rand",TS_Octant[0],TS_Octant[0]),Rand_MD_1,"SAME");
  hMD_MT_Rand->SetLineColor(46);
  hMD_MT_Rand->SetFillColor(46);
  //hMD_MT_Good->Draw();
  //hMD_MT_Rand->Draw("SAME");
  
  //Nice Pictures of Rakitha method
  TCanvas* cRakithaPlots = new TCanvas("cRakithaPlots","cRakithaPlots",800,800);
  hMD_ADC_Good_Raw[0]->SetLineColor(36);
  hMD_ADC_Good_Raw[0]->SetFillColor(36);
  hMD_ADC_Neutral_Raw[0]->SetLineColor(42);
  hMD_ADC_Neutral_Raw[0]->SetFillColor(42);  
  hMD_ADC_Good_Raw[0]->Draw();
  hMD_ADC_Neutral_Raw[0]->Draw("SAME");
 
  //Nice Pictures of Randoms
  TCanvas* cRandomPlots = new TCanvas("cRandomPlots","cRandomPlots",800,800);
  hMD_ADC_Good_Rand[0]->SetLineColor(46);
  hMD_ADC_Good_Rand[0]->SetFillColor(46);
  hMD_ADC_Neutral_Rand[0]->SetLineColor(26);
  hMD_ADC_Neutral_Rand[0]->SetFillColor(26);
  hMD_ADC_Good_Rand[0]->Draw();
  hMD_ADC_Neutral_Rand[0]->Draw("SAME");
  
  //Nice Pictures of Corrected
  TCanvas* cCorrectPlots = new TCanvas("cCorrectPlots","cCorrectPlots",800,800);
  hMD_ADC_Good_Correct[0]->SetLineColor(36);
  hMD_ADC_Good_Correct[0]->SetFillColor(36);
  hMD_ADC_Neutral_Correct[0]->SetLineColor(42);
  hMD_ADC_Neutral_Correct[0]->SetFillColor(42);
  hMD_ADC_Good_Correct[0]->Draw();
  hMD_ADC_Neutral_Correct[0]->Draw("SAME");

  // Raw ADC Avgs.
  TCanvas* cRAW = new TCanvas("cRAW","cRAW",800,800);
  cRAW->Divide(2,2);
  cRAW->cd(1);
  fChain->Draw("(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0 >> hTS_ADC_Raw_1",TS_ADC_Cut_1 && EventType && CleanData,"");
  cRAW->cd(2);
  fChain->Draw("(trigscint.ts2m_adc+trigscint.ts2p_adc)/2.0 >> hTS_ADC_Raw_2",TS_ADC_Cut_2 && EventType && CleanData,"");
  cRAW->cd(3);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Raw_%i",TS_Octant[0],TS_Octant[0],MD_ADC_PED[TS_Octant[0]-1],TS_Octant[0]),MD_ADC_Cut_1 && EventType && CleanData,"");
  cRAW->cd(4);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0-%f >> hMD_ADC_Raw_%i",TS_Octant[1],TS_Octant[1],MD_ADC_PED[TS_Octant[1]-1],TS_Octant[1]),MD_ADC_Cut_2 && EventType && CleanData,"");

  // Raw PMT Plots
  TCanvas* cPMT = new TCanvas("cPMT","cRAW",1600,800);
  cPMT->Divide(4,2);
  cPMT->cd(1);
  fChain->Draw("trigscint.ts1m_adc >> hTS_ADC_1", TS_ADC_Cut_1 && EventType && CleanData, "");
  cPMT->cd(2);
  fChain->Draw("trigscint.ts1p_adc >> hTS_ADC_2", TS_ADC_Cut_1 && EventType && CleanData, "");
  cPMT->cd(3);
  fChain->Draw("trigscint.ts2m_adc >> hTS_ADC_3", TS_ADC_Cut_2 && EventType && CleanData, "");
  cPMT->cd(4);
  fChain->Draw("trigscint.ts2p_adc >> hTS_ADC_4", TS_ADC_Cut_2 && EventType && CleanData, "");
  cPMT->cd(5);
  fChain->Draw(Form("maindet.md%im_adc >> hMD_ADC_1",TS_Octant[0]), MD_ADC_Cut_1 && EventType && CleanData, "");
  cPMT->cd(6);
  fChain->Draw(Form("maindet.md%ip_adc >> hMD_ADC_2",TS_Octant[0]), MD_ADC_Cut_1 && EventType && CleanData, "");
  cPMT->cd(7);
  fChain->Draw(Form("maindet.md%im_adc >> hMD_ADC_3",TS_Octant[1]), MD_ADC_Cut_2 && EventType && CleanData, "");
  cPMT->cd(8);
  fChain->Draw(Form("maindet.md%ip_adc >> hMD_ADC_4",TS_Octant[1]), MD_ADC_Cut_2 && EventType && CleanData, "");

  // Raw MT plots
  TCanvas* cMT_RAW = new TCanvas("cMT_RAW","cMT_RAW",800,800);
  cMT_RAW->Divide(2,2);
  cMT_RAW->cd(1);
  fChain->Draw("(trigscint.ts1m_f1+trigscint.ts1p_f1)/2.0 >> hTS_MT_Raw_1", TS_TDC_Cut_1 && EventType && CleanData, "");
  cMT_RAW->cd(2);
  fChain->Draw("(trigscint.ts2m_f1+trigscint.ts2p_f1)/2.0 >> hTS_MT_Raw_2", TS_TDC_Cut_2 && EventType && CleanData, "");
  cMT_RAW->cd(3);
  fChain->Draw(Form("(maindet.md%im_f1+maindet.md%ip_f1)/2.0 >> hMD_MT_Raw_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]), MD_TDC_Cut_1 && EventType && CleanData, "");
  cMT_RAW->cd(4);
  fChain->Draw(Form("(maindet.md%im_f1+maindet.md%ip_f1)/2.0 >> hMD_MT_Raw_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]), MD_TDC_Cut_2 && EventType && CleanData, "");

  // Raw DIFF plots
  TCanvas* cDIFF_RAW = new TCanvas("cDIFF_RAW","cDIFF_RAW",800,800);
  cDIFF_RAW->Divide(2,2);
  cDIFF_RAW->cd(1);
  fChain->Draw("(trigscint.ts1m_f1-trigscint.ts1p_f1) >> hTS_DIFF_Raw_1", TS_TDC_Cut_1 && EventType && CleanData, "");
  cDIFF_RAW->cd(2);
  fChain->Draw("(trigscint.ts2m_f1-trigscint.ts2p_f1) >> hTS_DIFF_Raw_2", TS_TDC_Cut_2 && EventType && CleanData, "");
  cDIFF_RAW->cd(3);
  fChain->Draw(Form("(maindet.md%im_f1-maindet.md%ip_f1) >> hMD_DIFF_Raw_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]), MD_TDC_Cut_1 && EventType && CleanData, "");
  cDIFF_RAW->cd(4);
  fChain->Draw(Form("(maindet.md%im_f1-maindet.md%ip_f1) >> hMD_DIFF_Raw_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]), MD_TDC_Cut_2 && EventType && CleanData, "");

  // MeanTime vs. Difference/2
  TCanvas* cMT_DIFF = new TCanvas("cMT_DIFF","cMT_DIFF",800,800);
  cMT_DIFF->Divide(2,2);
  cMT_DIFF->cd(1);
  fChain->Draw("(trigscint.ts1m_f1+trigscint.ts1p_f1)/2.0:(trigscint.ts1m_f1-trigscint.ts1p_f1) >> hTS_MT_DIFF_1",TS_TDC_Cut_1 && EventType && CleanData,"COL");
  cMT_DIFF->cd(2);
  fChain->Draw("(trigscint.ts2m_f1+trigscint.ts2p_f1)/2.0:(trigscint.ts2m_f1-trigscint.ts2p_f1) >> hTS_MT_DIFF_2",TS_TDC_Cut_2 && EventType && CleanData,"COL");
  cMT_DIFF->cd(3);
  fChain->Draw(Form("(maindet.md%im_f1+maindet.md%ip_f1)/2.0:(maindet.md%im_f1-maindet.md%ip_f1) >> hMD_MT_DIFF_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0],TS_Octant[0],TS_Octant[0]),MD_TDC_Cut_1 && EventType && CleanData,"COL");
  cMT_DIFF->cd(4);
  fChain->Draw(Form("(maindet.md%im_f1+maindet.md%ip_f1)/2.0:(maindet.md%im_f1-maindet.md%ip_f1) >> hMD_MT_DIFF_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1],TS_Octant[1],TS_Octant[1]),MD_TDC_Cut_2 && EventType && CleanData,"COL");
 
  // PMT ADC avg.
  TCanvas* cADC_MT = new TCanvas("cADC_MT","cADC_MT",800,800);
  cADC_MT->Divide(2,2);
  cADC_MT->cd(1);
  fChain->Draw("(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0:(trigscint.ts1m_f1+trigscint.ts1p_f1)/2.0 >> hTS_ADC_MT_1",TS_TDC_Cut_1 && TS_ADC_Cut_1 && EventType && CleanData,"COL");
  cADC_MT->cd(2);
  fChain->Draw("(trigscint.ts2m_adc+trigscint.ts2p_adc)/2.0:(trigscint.ts2m_f1+trigscint.ts2p_f1)/2.0 >> hTS_ADC_MT_2",TS_TDC_Cut_2 && TS_ADC_Cut_2 && EventType && CleanData,"COL");
  cADC_MT->cd(3);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0:(maindet.md%im_f1+maindet.md%ip_f1)/2.0 >> hMD_ADC_MT_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0],TS_Octant[0],TS_Octant[0]),MD_TDC_Cut_1 && MD_ADC_Cut_1 && EventType && CleanData,"COL");
  cADC_MT->cd(4);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0:(maindet.md%im_f1+maindet.md%ip_f1)/2.0 >> hMD_ADC_MT_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1],TS_Octant[1],TS_Octant[1]),MD_TDC_Cut_2 && MD_ADC_Cut_2 && EventType && CleanData,"COL");

  // PMT+ vs PMT-
  TCanvas* cADC_LR = new TCanvas("cADC_LR","cADC_LR",800,800);
  cADC_LR->Divide(2,2);
  cADC_LR->cd(1);
  fChain->Draw("trigscint.ts1m_adc:trigscint.ts1p_adc >> hTS_ADC_LR_1",TS_TDC_Cut_1 && EventType && CleanData,"COL");
  cADC_LR->cd(2);
  fChain->Draw("trigscint.ts2m_adc:trigscint.ts2p_adc >> hTS_ADC_LR_2",TS_TDC_Cut_2 && EventType && CleanData,"COL");
  cADC_LR->cd(3);
  fChain->Draw(Form("maindet.md%im_adc:maindet.md%ip_adc >> hMD_ADC_LR_%i",TS_Octant[0],TS_Octant[0],TS_Octant[0]),MD_TDC_Cut_1 && EventType && CleanData,"COL");
  cADC_LR->cd(4);
  fChain->Draw(Form("maindet.md%im_adc:maindet.md%ip_adc >> hMD_ADC_LR_%i",TS_Octant[1],TS_Octant[1],TS_Octant[1]),MD_TDC_Cut_2 && EventType && CleanData,"COL");
  
  // PMT Plots
  TCanvas* cPMT = new TCanvas("cPMT","cPMT",900,1000);
  cPMT->Divide(3,4);
  cPMT->cd(1);
  fChain->Draw("trigscint.ts1m_adc:trigscint.ts1p_adc >> hTS_PMT_1",TS_TDC_Cut_1 && EventType && CleanData,"COL");
  cPMT->cd(2);
  fChain->Draw("trigscint.ts1m_adc:trigscint.ts1p_adc >> hTS_PMT_2",MD_MT_Cut_2 && MD_DIFF_Cut_2 && TS_TDC_Cut_1 && EventType && CleanData,"COL");
  cPMT->cd(3); 
  fChain->Draw("trigscint.ts1m_adc:trigscint.ts1p_adc >> hTS_PMT_3",TS_MT_Cut_1 && TS_DIFF_Cut_1 && TS_TDC_Cut_1 && EventType && CleanData,"COL");
  cPMT->cd(4);
  fChain->Draw(Form("maindet.md%im_adc:maindet.md%ip_adc >> hMD_PMT_1",TS_Octant[0],TS_Octant[0]),MD_TDC_Cut_1 && EventType && CleanData,"COL");
  cPMT->cd(5);
  fChain->Draw(Form("maindet.md%im_adc:maindet.md%ip_adc >> hMD_PMT_2",TS_Octant[0],TS_Octant[0],TS_Octant[0]),MD_MT_Cut_2 && MD_DIFF_Cut_2 && MD_TDC_Cut_1 && EventType && CleanData,"COL");
  cPMT->cd(6);
  fChain->Draw(Form("maindet.md%im_adc:maindet.md%ip_adc >> hMD_PMT_3",TS_Octant[0],TS_Octant[0],TS_Octant[0]),MD_MT_Cut_1 && MD_DIFF_Cut_1 && MD_TDC_Cut_1  && EventType && CleanData,"COL");
  cPMT->cd(7);
  fChain->Draw("(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0 >> hTS_ADC_1",TS_TDC_Cut_1 && EventType && CleanData,"");
  cPMT->cd(8);
  fChain->Draw("(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0 >> hTS_ADC_2",MD_MT_Cut_2 && MD_DIFF_Cut_2 && TS_TDC_Cut_1 && EventType && CleanData,"");
  cPMT->cd(9);
  fChain->Draw("(trigscint.ts1m_adc+trigscint.ts1p_adc)/2.0 >> hTS_ADC_3",TS_MT_Cut_1 && TS_DIFF_Cut_1 && TS_TDC_Cut_1 && EventType && CleanData,"");
  cPMT->cd(10);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_1",TS_Octant[0],TS_Octant[0]),MD_TDC_Cut_1 && EventType && CleanData,"");
  cPMT->cd(11);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_2",TS_Octant[0],TS_Octant[0]),MD_MT_Cut_2 && MD_DIFF_Cut_2 && MD_TDC_Cut_1 && EventType && CleanData,"");
  cPMT->cd(12);
  fChain->Draw(Form("(maindet.md%im_adc+maindet.md%ip_adc)/2.0 >> hMD_ADC_3",TS_Octant[0],TS_Octant[0]),MD_MT_Cut_1 && MD_DIFF_Cut_1 && MD_TDC_Cut_1 && EventType && CleanData,"");
  */

  //////////////
  // PRINTING //
  //////////////

  /*
  std::cout << run << " ";
  std::cout << TS_Octant[0] << " ";
  std::cout << hMD_ADC_Good[0]->GetEntries() << " " << MD_Good_Rand_1 << " ";
  std::cout << hMD_ADC_Good[0]->GetMean() << " " << hMD_ADC_Good[0]->GetRMS() << " ";
  std::cout << hMD_ADC_Charge[0]->GetEntries() << " " << MD_Charge_Rand_1 << " ";
  std::cout << hMD_ADC_Charge[0]->GetMean() << " " << hMD_ADC_Charge[0]->GetRMS() << " ";
  std::cout << hMD_ADC_Neutral[0]->GetEntries() << " " << MD_Neutral_Rand_1 << " ";
  std::cout << hMD_ADC_Neutral[0]->GetMean() << " " << hMD_ADC_Neutral[0]->GetRMS() << " ";
  
  std::cout << TS_Octant[1] << " "; 
  std::cout << hMD_ADC_Good[1]->GetEntries() << " " << MD_Good_Rand_2 << " ";
  std::cout << hMD_ADC_Good[1]->GetMean() << " " << hMD_ADC_Good[1]->GetRMS() << " ";
  std::cout << hMD_ADC_Charge[1]->GetEntries() << " " << MD_Charge_Rand_2 << " ";
  std::cout << hMD_ADC_Charge[1]->GetMean() << " " << hMD_ADC_Charge[1]->GetRMS() << " ";
  std::cout << hMD_ADC_Neutral[1]->GetEntries() << " " << MD_Neutral_Rand_2 << " ";
  std::cout << hMD_ADC_Neutral[1]->GetMean() << " " << hMD_ADC_Neutral[1]->GetRMS() << " ";
  std::cout << "\n";
  */

}
