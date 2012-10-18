//This script finds the mean and RMS of the main detector scaler banks quickly.
//It is an improvement of the previous version, which was slow: it looped
//through all events for each histogram. This one loops through once.
//Written by Josh Magee
//magee@jlab.org
//October 18, 2012

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <QwEvent.h>
#include <QwHit.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TChain.h>

void scalers( Int_t runNum, Int_t lowcut = 0, Int_t highcut = 4e6, TString mycut = "") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");

  gStyle->SetOptStat("neMmRr");
  gStyle->SetStatW(0.32);

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

  TTree* event_tree  = (TTree*) file->Get("event_tree");
  TBranch* ev_branch = event_tree->GetBranch("events");

  QwEvent* qwevent   = 0;

  Long_t num_entries = event_tree->GetEntries();
  Lont_t localEntry  = 0;

  ev_branch -> SetAddress(&qwevent);

  for (Long_t i=0; i<num_entries; i++)
  {
    if (!(i>=lowcut && i<=highcut)) {continue;}
    if (!CodaEventType==1) {continue;}

    localEntry = event_tree->LoadTree(i);
    event_tree->GetEntry(localEntry);

    //fill histograms
    for (j=1; j<=8; j++) {
      if (!mdArray[j]!=0) mdHist[j]->Fill(mdarray[j]);
      if (!mdmArray[j]!=0) mdmHist[j]->Fill(mdmArray[j]);
      if (!mdpArray[j]!=0) mdpHist[j]->Fill(mdpArray[j]);
    } //end of loop through main detectors

    for (j=1; j<=6; j++) {
      if (!tsArray[j]!=0) tsHist[j]->Fill(tsarray[j]);
      if (!tsmArray[j]!=0) tsmHist[j]->Fill(tsmArray[j]);
      if (!tspArray[j]!=0) tspHist[j]->Fill(tspArray[j]);
    } //end of loop through main detectors

    if (i % 1000 == 0 ) {
      printf(" Events processed so far : %ld out of %ld total events\n", i, num_entries);
    }

  } // end for loop through entries

  //plot histograms





} //end scaler function




