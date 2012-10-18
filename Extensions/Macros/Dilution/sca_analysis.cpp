//Script to calculate the mdallbars, +/- scalers given a run number and list of cuts.
//Written by J. Magee, 2011-10-27

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TMath.h>

void sca_analysis( Int_t runNum, Int_t lowcut, Int_t highcut, Double_t bcm_minimum = 0.0, TString mycut = "") {
  gROOT->Reset();
  gROOT->SetStyle("Plain");

  //Set bcm calibration (good as of Oct 2011)
//  const TString bcm1 = "((sca_bcm1*1e-3)-250.34)/45.75";  //run1 1 Hz mps
//  const TString bcm2 = "((sca_bcm2*1e-3)-250.4)/44.92";  //run1 1 Hz mps
  const TString bcm1 = "((sca_bcm1*1e-2)-250.34)/45.75";  //run1
  const TString bcm2 = "((sca_bcm2*1e-2)-250.4)/44.92";  //run1
//  const TString bcm1 = "((sca_bcm1*1e-2)-249.7)/43.46"; //run2
//  const TString bcm2 = "((sca_bcm2*1e-2)-249.9)/41.95"; //run2
//  const TString bcm = bcm1;
  const TString bcm = bcm2;
  Int_t bcmoffset = 100*bcm_minimum;
// const TString bcm = "sca_bcm2>0"; 

  TString cuts = mycut.IsNull() ? mycut : "&&"+mycut;

  //populate arrays of allbar, +/- info

  const TString mdall [8] = {
    "md1bar_sca", "md2bar_sca", "md3bar_sca",
    "md4bar_sca", "md5bar_sca", "md6bar_sca",
    "md7bar_sca", "md8bar_sca"
  };

  const TString mdp [8] = {
    "md1p_sca", "md2p_sca", "md3p_sca",
    "md4p_sca", "md5p_sca", "md6p_sca",
    "md7p_sca", "md8p_sca"
  };

  const TString mdm [8] = {
    "md1m_sca", "md2m_sca", "md3m_sca",
    "md4m_sca", "md5m_sca", "md6m_sca",
    "md7m_sca", "md8m_sca"
  };


  const TString trigscint [6] = {
    "ts1mt_sca", "ts1m_sca", "ts1p_sca",
    "ts2mt_sca", "ts2m_sca", "ts2p_sca"
      //     "ts1mt_sca", "ts2mt_sca",
      //    "ts1m_sca", "ts2m_sca",
      //    "ts1p_sca", "ts2p_sca"
  };

  Double_t pArray [8];
  Double_t mArray [8];
  Double_t aArray [8];
  
  Double_t pArrayError [8];
  Double_t mArrayError [8];
  Double_t aArrayError [8];

  for (Int_t i=0; i<8; i++) {
    pArray[i] = 0;
    mArray[i] = 0;
    aArray[i] = 0;
    pArrayError[i] = 0;
    mArrayError[i] = 0;
    aArrayError[i] = 0;
  }

  Double_t tsArray[6];
  Double_t tsArrayError[6];
  for (Int_t i=0; i<6; i++) {
    tsArray[i]=0;
    tsArrayError[i]=0;
  }


  const Int_t canvas [8] = {
    4, 1, 2, 3, 6, 9, 8, 7
  };

  const Int_t canvas_ts [6] = {
    1, 3, 5, 2, 4, 6
  };

  Char_t filename[100];

  //open the filename
//  sprintf(filename, "Qweak_%i.root", runNum);
//  sprintf(filename, "QweakMay11_%i.root", runNum);
  sprintf(filename, "QweakMarchGas11_%i.root", runNum);
//  sprintf(filename, "QweakNew_%i.root", runNum);
//  sprintf(filename, "Qweak_%i.000.trees.root", runNum);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    return;
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *event_tree = (TTree*) file->Get("event_tree");

  gStyle->SetOptStat("neMmRr");
  gStyle->SetStatW(0.32);

//diagnostic plots
  std::cout <<"Drawing diagnostic plots...\n";

  TCanvas *diagnostics = new TCanvas("diagnostics");
  diagnostics->Divide(2,2);

  diagnostics->cd(1);
  gPad->SetLogy();
  event_tree->Draw("sca_4MHz/4e6>>clock",Form("%s", cuts.Data()));

  diagnostics->cd(2);
  gPad->SetLogy();
  event_tree->Draw(Form("%s>>h1", bcm1.Data()),Form("%s>0 && CodaEventNumber>%i && CodaEventNumber<%i %s", bcm1.Data(), lowcut, highcut, cuts.Data()));
  h1->Fit("gaus");
  gPad->Modified();

  diagnostics->cd(3);
  gPad->SetLogy();
  event_tree->Draw(Form("%s>>h2", bcm2.Data()),Form("%s>0 && CodaEventNumber>%i && CodaEventNumber<%i  %s", bcm2.Data(), lowcut, highcut, cuts.Data()));
  h2->Fit("gaus");
  gPad->Modified();

  diagnostics->cd(4);
  gPad->SetLogy();
  event_tree->Draw(Form("%s>>h3", bcm1.Data()),Form("%s>%i/100 && CodaEventNumber>%i && CodaEventNumber<%i %s", bcm1.Data(), bcmoffset, lowcut, highcut, cuts.Data()));
  h3->Fit("gaus");
  gPad->Modified();

//diagnostic plots end

  //Let's just start with the allbars data
  std::cout <<"**THE MDALLBARS DATA**\n\n";
  TCanvas *md_can = new TCanvas("md_can", "MD All Bars", 1400, 1100);
  md_can -> Divide(3,3);
  for (Int_t j=0; j < 8; j++) {

    md_can->cd(canvas[j]);
    gPad -> SetLogy();
    event_tree->Draw(Form("1e-2*%s>>mdall_%i",mdall[j].Data(),j+1),Form("%s!=0 && CodaEventNumber>%i && CodaEventNumber<%i && %s>%i/100  %s",mdall[j].Data(), lowcut, highcut,bcm.Data(), bcmoffset, cuts.Data()));
    TH1F *htmp = (TH1F*) gPad->GetPrimitive(Form("mdall_%i",j+1));
    std::cout <<mdall[j].Data() <<" " <<htmp->GetMean() <<" " <<htmp->GetRMS() <<std::endl;
    aArray[j] = htmp->GetMean();
    aArrayError[j] = double(htmp->GetRMS()/sqrt( htmp->GetEntries() ) );
//    htmp->Fit("gaus");
    std::cout <<"\n";
    gPad->Modified();
    gPad->Update();
//    fit = htmp->GetFunction("gaus");
//    aArray[j] = fit->GetParameter(1);
//    aArrayError[j] = fit->GetParError(1);
  } //end mdallbars for loop

  //mdp plots here
  std::cout <<"**THE MDP DATA**\n\n";
  TCanvas *mdp_can = new TCanvas("mdp_can", "MDp tubes", 1400, 1100);
  mdp_can -> Divide(3,3);
  for (Int_t j=0; j < 8; j++) {

    mdp_can->cd(canvas[j]);
    gPad -> SetLogy();
    event_tree->Draw(Form("((1e-2)*%s)>>mdp_%i",mdp[j].Data(),j+1),Form("%s!=0 && CodaEventNumber>%i && CodaEventNumber<%i && %s>%i/100 %s",mdp[j].Data(), lowcut, highcut,bcm.Data(), bcmoffset, cuts.Data()));
    TH1F *htmp = (TH1F*) gPad->GetPrimitive(Form("mdp_%i",j+1));
    std::cout <<mdp[j].Data() <<" " <<htmp->GetMean() <<" " <<htmp->GetRMS() <<std::endl;
    pArray[j] = htmp->GetMean();
    pArrayError[j] = double(htmp->GetRMS()/sqrt( htmp->GetEntries() ) );
//    htmp->Fit("gaus");
    std::cout <<"\n";
    gPad->Modified();
    gPad->Update();
//    fit = htmp->GetFunction("gaus");
//    pArray[j] = fit->GetParameter(1);
//    pArrayError[j] = fit->GetParError(1);
   } //end mdp for loop


  //mdm plots here
  std::cout <<"**THE MDM DATA**\n\n";
  TCanvas *mdm_can = new TCanvas("mdm_can", "MDm  tubes", 1400, 1100);
  mdm_can -> Divide(3,3);
  for (Int_t j=0; j < 8; j++) {

    mdm_can->cd(canvas[j]);
    gPad -> SetLogy();
    event_tree->Draw(Form("((1e-2)*%s)>>mdm_%i",mdm[j].Data(),j+1),Form("%s!=0 && CodaEventNumber>%i && CodaEventNumber<%i && %s>%i/100 %s",mdm[j].Data(), lowcut, highcut, bcm.Data(), bcmoffset, cuts.Data()));
    TH1F *htmp = (TH1F*) gPad->GetPrimitive(Form("mdm_%i",j+1));
    std::cout <<mdm[j].Data() <<" " <<htmp->GetMean() <<" " <<htmp->GetRMS() <<std::endl;
    mArray[j] = htmp->GetMean();
    mArrayError[j] = double(htmp->GetRMS()/sqrt( htmp->GetEntries() ) );
//    htmp->Fit("gaus");
    std::cout <<"\n";
    gPad->Modified();
    gPad->Update();
//    fit = htmp->GetFunction("gaus");
//    mArray[j] = fit->GetParameter(1);
//    mArrayError[j] = fit->GetParError(1);
   } //end mdm for loop

  //ts plots here
  std::cout <<"**The TS DATA**\n\n";
  TCanvas *ts_can = new TCanvas("ts_can","Trigger Scintillator Scalers",1200,1200);
  ts_can -> Divide(2,3);

  for (Int_t j=0; j<6; j++) {
    ts_can->cd(canvas_ts[j]);
    gPad -> SetLogy();
    event_tree->Draw(Form("((1e-2)*%s)>>%s",trigscint[j].Data(),trigscint[j].Data()),Form("%s!=0 && CodaEventNumber>%i && CodaEventNumber<%i && %s>%i/100  %s",trigscint[j].Data(), lowcut, highcut, bcm.Data(), bcmoffset, cuts.Data()));
    TH1F *htmp = (TH1F*) gPad->GetPrimitive(Form("%s",trigscint[j].Data()));
    std::cout <<Form("%s",trigscint[j].Data()) <<" " <<htmp->GetMean() <<" " <<htmp->GetRMS() << std::endl;
    tsArray[j] = htmp->GetMean();
    tsArrayError[j] = double(htmp->GetRMS()/sqrt( htmp->GetEntries() ) );
//    htmp->Fit("gaus");
    std::cout <<"\n";
    gPad->Modified();
    gPad->Update();
//    fits = htmp->GetFunction("gaus");
//    tsArray[j] = fits->GetParameter(1);
//    tsArrayError[j] = fits->GetParError(1);
  } //end trigger scintillator loop

  for (Int_t i=0; i<8; i++) {
    std::cout <<aArray[i] <<", " <<aArrayError[i] <<", " <<mArray[i] <<", " <<mArrayError[i] <<", " <<pArray[i] <<", " <<pArrayError[i] <<endl;
  }

  for (Int_t i=1; i<7; i++) {
    std::cout <<tsArray[i-1] <<", " <<tsArrayError[i-1] <<", ";
   if (i%3==0)  std::cout <<endl;
  }

} // end sca_analysis function

