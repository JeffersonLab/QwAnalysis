///////////////////////////////////////
// SlugAverages.C
// Martin McHugh
// 2013-10-04
// This code is designed to find 
// slopesTree.root and construct the 
// averages for all runs included.
// 
// CURRENTLY DOES NOT SUPPORT 
// USER-SPECIFIED CUTS. NEEDS TO BE 
// HARD-CODED.
//
// Must have environment variable 
// $QW_ROOTFILES point to directory 
// containing slopesTree.root.
//
// To run:
// 1) Use Root 
// 2) root [] .x SlugAverages.C(fMakeSlopes)
//
///////////////////////////////////////

gROOT->Reset();
#include <TChain.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

void SlugAverages(Int_t fMakePlots = 0){

  const Int_t nDET = 17;
  const Int_t nMOD = 5;
  const Int_t nCOIL = 10;
  
  char* DetName[nDET];
  DetName[0] = "mdallbars";
  DetName[1] = "md1barsum";
  DetName[2] = "md2barsum";
  DetName[3] = "md3barsum";
  DetName[4] = "md4barsum";
  DetName[5] = "md5barsum";
  DetName[6] = "md6barsum";
  DetName[7] = "md7barsum";
  DetName[8] = "md8barsum";
  DetName[9] = "md9barsum";
  DetName[10] = "pmtltg";
  DetName[11] = "pmtonl";
  DetName[12] = "uslumi_sum";
  DetName[13] = "uslumi1_sum";
  DetName[14] = "uslumi3_sum";
  DetName[15] = "uslumi5_sum";
  DetName[16] = "uslumi7_sum";
//  DetName[17] = "dslumi1";
//  DetName[18] = "dslumi2";
//  DetName[19] = "dslumi3";
//  DetName[20] = "dslumi4";
//  DetName[21] = "dslumi5";
//  DetName[22] = "dslumi6";
//  DetName[23] = "dslumi7";
//  DetName[24] = "dslumi8";
//  DetName[25] = "bcm1";
//  DetName[26] = "bcm2";
//  DetName[27] = "bcm5";
//  DetName[28] = "bcm6";
  
  char* ModName[nMOD];
  ModName[0] = "targetX";
  ModName[1] = "targetY";
  ModName[2] = "bpm3c12X";
  ModName[3] = "targetXSlope";
  ModName[4] = "targetYSlope";

  char* CoilName[nCOIL];
  CoilName[0] = "Coil0";		// X sine
  CoilName[1] = "Coil1";		// Y sine
  CoilName[2] = "Coil2";		// E sine
  CoilName[3] = "Coil3";		// XP sine
  CoilName[4] = "Coil4";		// YP sine
  CoilName[5] = "Coil5";		// X cosine
  CoilName[6] = "Coil6";		// Y cosine
  CoilName[7] = "Coil7";		// E cosine
  CoilName[8] = "Coil8";		// XP cosine
  CoilName[9] = "Coil9";		// YP cosine

  // Open slopesTree.root
  TString file_directory = gSystem->Getenv("QW_ROOTFILES");
  TFile* file = new TFile(Form("%s/slopesTree.root", file_directory.Data()));
  TTree* tree = (TTree*) file->Get("slopes");
  
  const Int_t nRuns = tree->GetEntries();

  // Determine which entries begin and end slugs
  vector < Double_t > slug;
  vector < Int_t > slug_start;
  vector < Int_t > slug_end;
  Double_t old_slug = 0;
  Double_t new_slug = 0;
  tree->SetBranchAddress("slug", &new_slug);  
  for(Int_t k=0; k<nRuns; k++) {
    tree->GetEntry(k);
    if(old_slug!=new_slug) {
      old_slug = new_slug;
      slug.push_back(old_slug);
      slug_start.push_back(k);
      if (k!=0) slug_end.push_back(k-1);
    }
  }
  slug_end.push_back(nRuns-1);
  
  const Int_t nSlugs = slug.size();

  ////////////////////////
  // Calculate Averages //
  ////////////////////////
  
  vector <Double_t> good_slug;
  vector < vector < vector < Double_t > > > avg;
  vector < vector < vector < Double_t > > > err;
  avg.resize(nDET);
  err.resize(nDET);
  for(Int_t i=0; i<nDET; i++) {
    avg[i].resize(nMOD);
    err[i].resize(nMOD);
  }
  
  Double_t value;
  Double_t error;  
  for(Int_t i=0; i<nDET; i++) {
    for(Int_t j=0; j<nMOD; j++) {
      tree->SetBranchAddress(Form("%s_%s",DetName[i],ModName[j]), &value);
      tree->SetBranchAddress(Form("%s_%s_err",DetName[i],ModName[j]), &error);
      for(Int_t n=0; n<nSlugs; n++) {
        
        // Cuts hard coded in
        if( slug[n]<=175|| (200<=slug[n]&&slug[n]<=289) || 303<=slug[n] ) {
          Double_t mean = 0;
          Double_t sigma = 0;
        
          for (Int_t k=slug_start[n]; k<=slug_end[n]; k++) {
            tree->GetEntry(k);
            mean += value/(error*error);
            sigma += 1/(error*error);
          }
          mean = mean/sigma;
          sigma = TMath::Sqrt(1/sigma);
        
          avg[i][j].push_back(mean);
          err[i][j].push_back(sigma);
          if(i==0 && j==0)
            good_slug.push_back(slug[n]);
          
        } //end if(cuts)
          
      }
      tree->ResetBranchAddresses();
    }
  }
  
  const Int_t nGood = good_slug.size();
  cout << endl << "Slug Averages Calculated for " << nGood << " slugs." << endl << endl;
  
  //////////////
  // PLOTTING //
  //////////////
    
  if(fMakePlots) {
    
    TCanvas* can[nDET];
    
    for(Int_t i=0; i<nDET; i++) {
      can[i] = new TCanvas(Form("c%i", i), "canvas", 1500, 1000);
      can[i]->Divide(3,2);
      TGraphErrors* graph[nMOD];
      
      for(Int_t j=0; j<nMOD; j++) {
        
        Double_t x[nGood];
        Double_t y[nGood];
        Double_t y_err[nGood];
        
        for(Int_t n=0; n<nGood; n++) {
          x[n] = good_slug[n];
          y[n] = avg[i][j][n];
          y_err[n] = err[i][j][n];
        }
        
        can[i]->cd(j+1);
	graph[j] = new TGraphErrors(nGood, x, y, 0, y_err);        
        graph[j]->SetTitle(Form("%s_%s",DetName[i],ModName[j]));
        graph[j]->GetXaxis()->SetTitle("Slug");
        graph[j]->GetYaxis()->SetTitle("Slope");
        graph[j]->SetMarkerStyle(8);
        graph[j]->SetMarkerSize(0.4);
        graph[j]->Draw("AP");
        can[i]->Update();      
      }
      
      can[i]->Print(Form("%s.png",DetName[i]));  
    }
  }
    
  return;
}


