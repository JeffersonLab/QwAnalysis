//Script to calculate the mdallbars, +/- scalers given a run number and list of cuts.
//Written by J. Magee, 2011-10-27

#include <math.h>
#include <stdlib.h> 
#include <iostream>

#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>

using namespace std;
using std::vector;

void sca_analysis(int runnumber, int eventLow=0, int eventHigh=4000000) {
  //Create a TApplication. This is required for anything using the ROOT GUI.
  gROOT->Reset();
  gROOT->SetStyle("Modern");

  //define number of md/ts channels
  const int num_md = 8;
  const int num_ts = 2;

  //create arrays for histogram means and errors
  float mdall_mean[num_md];
  float mdp_mean[num_md];
  float mdm_mean[num_md];

  float mdall_error[num_md];
  float mdp_error[num_md];
  float mdm_error[num_md];

  float tsall_mean[num_ts];
  float tsp_mean[num_ts];
  float tsm_mean[num_ts];

  float tsall_error[num_ts];
  float tsp_error[num_ts];
  float tsm_error[num_ts];

  //create array of pointers to histogrmas for md all bars, md_plus, and md_minus
  TH1F *mdall[num_md];
  TH1F *mdp[num_md];
  TH1F *mdm[num_md];

  //create histograms for these quantities
  for(int j=0; j<num_md; j++) {
    mdall[j] = new TH1F(Form("mdall[%i]",j),Form("MD All Octant Bar %i",j+1),2400,0,2400);
    mdp[j]   = new TH1F(Form("mdp[%i]",j),Form("MD Plus Octant %i",j+1),2400,0,2400);
    mdm[j]   = new TH1F(Form("mdm[%i]",j),Form("MD Neg  Octant %i",j+1),2400,0,2400);

    mdall[j]->SetFillColor(kRed+3);
    mdp[j]->SetFillColor(kRed+3);
    mdm[j]->SetFillColor(kRed+3);
  }

  //create array of pointers to histogrmas for ts all bars, ts_plus, and ts_minus
  TH1F *tsall[num_md];
  TH1F *tsp[num_md];
  TH1F *tsm[num_md];

  for(int j=0; j<num_ts; j++) {
    tsall[j] = new TH1F(Form("tsall[%i]",j),Form("TS Mean time %i",j),2400,0,2400);
    tsp[j] = new TH1F(Form("tsp[%i]",j),Form("TS Plus %i",j),2400,0,2400);
    tsm[j] = new TH1F(Form("tsm[%i]",j),Form("TS Neg  %i",j),2400,0,2400);

    tsall[j]->SetFillColor(kBlue+3);
    tsp[j]->SetFillColor(kBlue+3);
    tsm[j]->SetFillColor(kBlue+3);
  }

  //Open rootfile
  //TFile *rootfile1 = TFile::Open("Qweak_18531.root");
  //TFile *rootfile1 = TFile::Open("Qweak_10709.root");
  TFile *rootfile1 = TFile::Open(Form("Qweak_%i.root",runnumber));

  if ( !rootfile1->IsOpen() ) {
    printf("Rootfile isn't open! Exiting!\n");
    exit(1);
  } else {
    printf("Rootfile is open!\n");
  }

 //grab the tree from the rootfile
  TTree *tree = (TTree*) rootfile1->Get("event_tree");
  tree->SetBranchStatus("*",0); //disable all branches
  tree->SetBranchStatus("maindet",1); //enable the 1 branch we care about
  tree->SetBranchStatus("trigscint",1); //enable the 1 branch we care about
  TBranch *maindet   = tree->GetBranch("maindet");
  TBranch *trigscint = tree->GetBranch("trigscint");

  //create arrays of pointers to mdall, mdp, and mdm leaves
  TLeaf *lMdall[num_md];
  TLeaf *lMdp[num_md];
  TLeaf *lMdm[num_md];

  for (int j=0; j<num_md; j++) {
    lMdall[j] = (TLeaf*) maindet->GetLeaf(Form("md%ibar_sca",j+1));
    lMdp[j]   = (TLeaf*) maindet->GetLeaf(Form("md%ip_sca",j+1));
    lMdm[j]   = (TLeaf*) maindet->GetLeaf(Form("md%im_sca",j+1));
  }

  //create arrays of pointers to tsall, tsp, and tsm leaves
  TLeaf *lTsall[num_ts];
  TLeaf *lTsp[num_ts];
  TLeaf *lTsm[num_ts];

  for (int j=0; j<num_ts; j++) {
    lTsall[j] = (TLeaf*) trigscint->GetLeaf(Form("ts%imt_sca",j+1));
    lTsp[j]   = (TLeaf*) trigscint->GetLeaf(Form("ts%ip_sca",j+1));
    lTsm[j]   = (TLeaf*) trigscint->GetLeaf(Form("ts%im_sca",j+1));
  }

  long n_entries = tree->GetEntries();
  for (long j=eventLow; j<n_entries; j++) {
    //process events only within desired event number range
    if (!(j>=eventLow && j<=eventHigh)) {continue;}
    tree      -> GetEntry(j);
    maindet   -> GetEntry(j);
    trigscint -> GetEntry(j);

    if(j % 100000 == 0) {
      printf(" Total events %ld. events process so far: %ld\n", n_entries, j);
    }

    for (int j=0; j<num_md; j++) {
      if (lMdall[j]->GetValue() !=0)
        mdall[j] -> Fill( .01*lMdall[j]->GetValue() );
      if (lMdp[j]->GetValue() !=0)
        mdp[j]   -> Fill( .01*lMdp[j]->GetValue() );
      if (lMdm[j]->GetValue() !=0)
        mdm[j]   -> Fill( .01*lMdm[j]->GetValue() );
    }

    for (int j=0; j<num_ts; j++) {
      if (lTsall[j]->GetValue() !=0)
        tsall[j] -> Fill( .01*lTsall[j]->GetValue() );
      if (lTsp[j]->GetValue() !=0)
        tsp[j]   -> Fill( .01*lTsp[j]->GetValue() );
      if (lTsm[j]->GetValue() !=0)
        tsm[j]   -> Fill( .01*lTsm[j]->GetValue() );
    }

  } //end of loop through n_entries

  const Int_t md_pad_position[num_md] = {
    4, 1, 2, 3, 6, 9, 8, 7
  };

  TCanvas *canvas_mdall = new TCanvas("canvas_mdall","title");
  TCanvas *canvas_mdp   = new TCanvas("canvas_mdp","title");
  TCanvas *canvas_mdm   = new TCanvas("canvas_mdm","title");
  canvas_mdall->Divide(3,3);
  canvas_mdp->Divide(3,3);
  canvas_mdm->Divide(3,3);
  for(int j=0; j<num_md;j++) {
    canvas_mdall->cd(md_pad_position[j]);
    mdall[j]->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();
    mdall_mean[j]  = mdall[j]->GetMean();
    mdall_error[j] = double(
        mdall[j]->GetRMS() / sqrt( mdall[j]->GetEntries() ) );

    canvas_mdp->cd(md_pad_position[j]);
    mdp[j]->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();
    mdp_mean[j]  = mdp[j]->GetMean();
    mdp_error[j] = double(
        mdp[j]->GetRMS() / sqrt(mdp[j]->GetEntries() ) );

    canvas_mdm->cd(md_pad_position[j]);
    mdm[j]->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();
    mdm_mean[j]  = mdm[j]->GetMean();
    mdm_error[j] = double(
      mdm[j]->GetRMS() / sqrt(mdm[j]->GetEntries() ) );
  }

  const Int_t ts_pad_position[6] = {
    1, 3, 5, 2, 4, 6
  };

  int k=0;
  TCanvas *canvas_ts   = new TCanvas("canvas_ts","title");
  canvas_ts->Divide(2,3);
  for(int j=0; j<6;j++) {
    canvas_ts->cd(ts_pad_position[j]);

    if (j==0 || j==1) {
      k=j;
      tsp[k]->Draw();
      gPad->SetLogy();
      gPad->Modified();
      gPad->Update();
      tsp_mean[k]  = tsp[j]->GetMean();
      tsp_error[k] = double(
          tsp[k]->GetRMS() / sqrt(tsp[j]->GetEntries() ) );
    }

    if (j==2 || j==3) {
      k=j-2;
      tsm[k]->Draw();
      gPad->SetLogy();
      gPad->Modified();
      gPad->Update();
      tsm_mean[k]  = tsm[k]->GetMean();
      tsm_error[k] = double(
          tsm[k]->GetRMS() / sqrt(tsm[k]->GetEntries() ) );
    }

    if (j==4 || j==5) {
      k=j-4;
      tsall[k]->Draw();
      gPad->SetLogy();
      gPad->Modified();
      gPad->Update();
      tsall_mean[k]  = tsall[k]->GetMean();
      tsall_error[k] = double(
          tsall[k]->GetRMS() / sqrt( tsall[k]->GetEntries() ) );
    }

  }

//raw output for MD and TS in kHz
  std::cout <<"\t\tThe Main Detector Scaler Data (kHz)\n";
  std::cout <<"Octant \tAllBars Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_md; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
    mdall_mean[j], mdall_error[j],
    mdm_mean[j]  , mdm_error[j],
    mdp_mean[j]  , mdp_error[j] );
  }

  std::cout <<"\n\t\tThe Trigger Scintillator Scaler Data (kHz)\n";
  std::cout <<"Package Meantime Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_ts; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j,
    tsall_mean[j], tsall_error[j],
    tsm_mean[j]  , tsm_error[j],
    tsp_mean[j]  , tsp_error[j] );
  }
/*
//now give the lifetime, accidentals, and uncorrelated pulse rates
  //discriminator width is 40 ns, convert to seconds
  const int disc = 174*pow(10,-9);
  //coincidence width is 79 ns
  const int coin = 79*pow(10,-9);
  std::cout <<"\n The livetime, accidentals, and uncorrelated pulses "
    <<"assuming dead time constant of 174 ns, coincidence width of 79ns.";
  std::cout <<"\t\tThe Main Detector\n";
  std::cout <<"\t\tLivetime \t\tAccidentals(kHz)\n";
  std::cout <<"Octant \tAllBars Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_md; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
    1-disc*1000*mdall_mean[j], //livetime
    //uncertainty on livetime
    mdall_mean[j]*1000*disc*pow(pow(mdall_error[j]/mdall_mean[j],2) ,0.5),
    mdm_mean[j]  ,mdm_error[j],
    mdp_mean[j]  ,mdp_error[j] );
  }

  std::cout <<"\t\tThe Trigger Scintillator\n";
  std::cout <<"\t\tLivetime \t\tAccidentals(kHz)\n";
  std::cout <<"Octant \tAllBars Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_ts; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j,
    1-disc*1000*tsall_mean[j], tsall_error[j],
    tsm_mean[j]  , tsm_error[j],
    tsp_mean[j]  , tsp_error[j] );
  }


//Give correction sizes in percent
  std::cout <<"\t\tThe Main Detector Scaler Data (kHz)\n";
  std::cout <<"Octant \tAllBars Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_md; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
    mdall_mean[j], mdall_error[j],
    mdm_mean[j]  , mdm_error[j],
    mdp_mean[j]  , mdp_error[j] );
  }

  std::cout <<"\n\t\tThe Trigger Scintillator Scaler Data (kHz)\n";
  std::cout <<"Package Meantime Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_ts; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j,
    tsall_mean[j], tsall_error[j],
    tsm_mean[j]  , tsm_error[j],
    tsp_mean[j]  , tsp_error[j] );
  }


//Give corrected rates
  std::cout <<"\t\tThe Main Detector Scaler Data (kHz)\n";
  std::cout <<"Octant \tAllBars Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_md; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j+1,
    mdall_mean[j], mdall_error[j],
    mdm_mean[j]  , mdm_error[j],
    mdp_mean[j]  , mdp_error[j] );
  }

  std::cout <<"\n\t\tThe Trigger Scintillator Scaler Data (kHz)\n";
  std::cout <<"Package Meantime Error \tNeg \tError \tPos \tError\n";
  for(int j=0; j<num_ts; j++) {
  printf("%i \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f \t%-.2f\n", j,
    tsall_mean[j], tsall_error[j],
    tsm_mean[j]  , tsm_error[j],
    tsp_mean[j]  , tsp_error[j] );
  }
  */
}



