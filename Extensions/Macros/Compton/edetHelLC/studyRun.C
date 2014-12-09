#include "rootClass.h"
#include "comptonRunConstants.h"
#include "vectorMinMax.C"

Int_t studyRun(Int_t runnum = 24519) {
  const Bool_t bBeam = 1;
  const Bool_t bYieldBPM = 1;
  const Bool_t bDiffBPM = 1;
  const Bool_t bEnergy = 0;
  const Bool_t bLaser = 1;
  gStyle->SetTextFont(132);
  gStyle->SetTextSize(0.08);
  gStyle->SetLabelFont(132,"xyz");
  gStyle->SetLabelSize(0.05,"xyz");
  gStyle->SetTitleSize(0.05,"xyz");
  gStyle->SetTitleOffset(1.3,"xyz");
  gStyle->SetOptTitle(0);
  gStyle->SetPadRightMargin(0.09);//0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.13);
  //gROOT->SetStyle("publication");
  filePre = Form("r%d/study_%d_",runnum,runnum);
  TChain *helChain = new TChain("Hel_Tree");
  Int_t chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));
  cout<<"Attached "<<chainExists<<" files to helicity chain for Run # "<<runnum<<endl;
  if(!chainExists){/// exit if rootfiles do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
    return -1;
  }

  Int_t nEntries = helChain->GetEntries();
  cout<<"The helicity chain has "<<nEntries<<" entries"<<endl;

  Double_t patNum;
  Double_t bcm[3], lasPow[3];
  std::vector<Double_t> bcm6, pattern, laserPow;
  helChain->SetBranchStatus("*",0);
  helChain->SetBranchStatus("pattern_number",1);
  helChain->SetBranchStatus("yield_sca_bcm6",1);
  helChain->SetBranchStatus("yield_sca_laser_PowT",1);
  //helChain->SetBranchStatus("",1);
  helChain->SetBranchAddress(Form("pattern_number"), &patNum);
  helChain->SetBranchAddress(Form("yield_sca_bcm6"), &bcm);
  helChain->SetBranchAddress(Form("yield_sca_laser_PowT"), &lasPow);
  TH1D *h1 = new TH1D("h1","h1",100,0,1);
  h1->SetBit(TH1::kCanRebin);

  if(bBeam) {
    TCanvas *cBeam = new TCanvas("cBeam",Form("beam for run %d",runnum), 0,0,900,500);///x,y cordinates of left top point; width, height
    TPad *pad = new TPad("pad","",0,0,1,1);
    //pad->SetGrid();
    pad->Draw();
    pad->cd();

    for(int i=0; i<nEntries; i++) {
      helChain->GetEntry(i);
      bcm6.push_back(bcm[0]);
      pattern.push_back(patNum/240);///scaling to make it time (s)
      //pattern.push_back(patNum);///scaling to make it time (s)
      laserPow.push_back(lasPow[0]);
    }

    TGraph *gr1 = new TGraph((Int_t)pattern.size(), pattern.data(), laserPow.data());
    gr1->SetMarkerStyle(kDot);
    gr1->SetMarkerColor(kGreen);
    gr1->Draw("AP");
    gr1->GetYaxis()->SetTitle("laser power");
    gr1->GetXaxis()->SetTitle("time (s)");
    //gr1->GetXaxis()->SetLimits(2509.0,2600.0);
    cBeam->Update();

    //create a transparent pad drawn on top of the main pad
    cBeam->cd();
    TPad *overlay = new TPad("overlay","",0,0,1,1);
    overlay->SetFillStyle(4000);
    overlay->SetFillColor(0);
    overlay->SetFrameFillStyle(0);
    overlay->Draw("FA");
    overlay->cd();
    TGraph *gr2 = new TGraph((Int_t)pattern.size(), pattern.data(), bcm6.data());
    gr2->SetMarkerStyle(kDot);
    gr2->SetMarkerColor(kRed);
    gr2->SetLineColor(kRed);

    Double_t xmin = pad->GetUxmin();
    Double_t xmax = pad->GetUxmax();
    Double_t ymin = 0;
    Double_t ymax = 0;//5;//190;
    vectorMinMax(bcm6, ymin, ymax);
    cout<<blue<<"for run "<<runnum<<" min and max current are "<<ymin<<" and "<<ymax<<" respectively"<<normal<<endl;
    if (ymax>200) ymax = 190.0;
    if (ymin<0) ymin = 0.0;
    ymax = ymax + 0.1*ymax;
    //cout<<"the min is: "<<*min_element(bcm6.begin(), bcm6.end())<<endl;
    //cout<<"the max is: "<<*max_element(bcm6.begin(), bcm6.end())<<endl;
    //cout<<"the min is: "<<*min_element(laserPow.begin(), laserPow.end())<<endl;
    //cout<<"the max is: "<<*max_element(laserPow.begin(), laserPow.end())<<endl;

    TH1F *hframe = overlay->DrawFrame(xmin,ymin,xmax,ymax);
    hframe->GetXaxis()->SetLabelOffset(99);
    hframe->GetYaxis()->SetTickLength(0);
    hframe->GetYaxis()->SetLabelOffset(99);
    gr2->Draw("P");
    //hframe->GetYaxis()->SetTitle("beam (#mu A)");///!doesn't work

    //Draw an axis on the right side
    TGaxis *axis = new TGaxis(xmax,ymin,xmax, ymax,ymin,ymax,510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->SetTitle("beam current (#muA)");
    axis->Draw();

    gSystem->mkdir(Form("%s/%s/r%d",pPath,www,runnum));
    cBeam->SaveAs(Form("%s/%s/%sBeamLas.png",pPath,www,filePre.Data()));
  }

  if(bYieldBPM) {
    //Double_t bpm_3p02aX[2],bpm_3p02aY[2],bpm_3p02bX[2],bpm_3p02bY[2],bpm_3p03aX[2],bpm_3p03aY[2],bpm_3c20X[2];
    helChain->SetBranchStatus("yield_sca_bpm_3p02aY*",1); 
    helChain->SetBranchStatus("yield_sca_bpm_3p02aX*",1); 
    helChain->SetBranchStatus("yield_sca_bpm_3p02bY*",1); 
    helChain->SetBranchStatus("yield_sca_bpm_3p02bX*",1); 
    helChain->SetBranchStatus("yield_sca_bpm_3p03aY*",1); 
    helChain->SetBranchStatus("yield_sca_bpm_3p03aX*",1);
    helChain->SetBranchStatus("yield_sca_bmod_ramp*",1);

    TCanvas *cYieldBPM = new TCanvas("cYieldBPM",Form("bpm yield for run %d",runnum),0,0,1200,900);
    cYieldBPM->Divide(3,2);// # of col,# of row
    cYieldBPM->cd(1);
    helChain->Draw("yield_sca_bpm_3p02aY:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    cYieldBPM->cd(2);
    helChain->Draw("yield_sca_bpm_3p02bY:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    cYieldBPM->cd(3);
    helChain->Draw("yield_sca_bpm_3p03aY:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    cYieldBPM->cd(4);
    helChain->Draw("yield_sca_bpm_3p02aX:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    cYieldBPM->cd(5);
    helChain->Draw("yield_sca_bpm_3p02bX:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    cYieldBPM->cd(6);
    helChain->Draw("yield_sca_bpm_3p03aX:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");

    cYieldBPM->Update();
    cYieldBPM->SaveAs(Form("%s/%s/%sBeamYieldBPM.png",pPath,www,filePre.Data()));
  }

  if(bDiffBPM) {
    //Double_t bpm_3p02aX[2],bpm_3p02aY[2],bpm_3p02bX[2],bpm_3p02bY[2],bpm_3p03aX[2],bpm_3p03aY[2],bpm_3c20X[2];
    helChain->SetBranchStatus("yield_sca_4mhz*",1);
    helChain->SetBranchStatus("diff_sca_bpm_3p02aY*",1); 
    helChain->SetBranchStatus("diff_sca_bpm_3p02aX*",1); 
    helChain->SetBranchStatus("diff_sca_bpm_3p02bY*",1); 
    helChain->SetBranchStatus("diff_sca_bpm_3p02bX*",1); 
    helChain->SetBranchStatus("diff_sca_bpm_3p03aY*",1); 
    helChain->SetBranchStatus("diff_sca_bpm_3p03aX*",1);
    helChain->SetBranchStatus("yield_sca_bmod_ramp*",1);
    //helChain->SetBranchAddress("diff_sca_bpm_3p02aX",&bpm_3p02aX);
    //helChain->SetBranchAddress("diff_sca_bpm_3p02aY",&bpm_3p02aY);
    //helChain->SetBranchAddress("diff_sca_bpm_3p02bX",&bpm_3p02bX);
    //helChain->SetBranchAddress("diff_sca_bpm_3p02bY",&bpm_3p02bY);
    //helChain->SetBranchAddress("diff_sca_bpm_3p03aX",&bpm_3p03aX);
    //helChain->SetBranchAddress("diff_sca_bpm_3p03aY",&bpm_3p03aY);

    TCanvas *cStability = new TCanvas("cStability",Form("stability parameters for run %d",runnum),0,0,1200,900);
    cStability->Divide(3,2);// # of col,# of row
    //cStability->cd(1);
    //helChain->Draw("diff_sca_bpm_3p02aY:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02aY)<0.00120");
    //cStability->cd(2);
    //helChain->Draw("diff_sca_bpm_3p02bY:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02bY)<0.050");
    //cStability->cd(3);
    //helChain->Draw("diff_sca_bpm_3p03aY:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p03aY)<0.050");
    //cStability->cd(4);
    //helChain->Draw("diff_sca_bpm_3p02aX:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02aX)<0.00120");
    //cStability->cd(5);
    //helChain->Draw("diff_sca_bpm_3p02bX:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02bX)<0.050");
    //cStability->cd(6);
    //helChain->Draw("diff_sca_bpm_3p03aX:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p03aX)<0.050");
    cStability->cd(1);
    helChain->Draw("diff_sca_bpm_3p02aY:pattern_number/240","yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02aY)<0.06");
    cStability->cd(2);
    helChain->Draw("diff_sca_bpm_3p02bY:pattern_number/240","yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02bY)<0.060");
    cStability->cd(3);
    helChain->Draw("diff_sca_bpm_3p03aY:pattern_number/240","yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p03aY)<0.060");
    cStability->cd(4);
    helChain->Draw("diff_sca_bpm_3p02aX:pattern_number/240","yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02aX)<0.06");
    cStability->cd(5);
    helChain->Draw("diff_sca_bpm_3p02bX:pattern_number/240","yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p02bX)<0.060");
    cStability->cd(6);
    helChain->Draw("diff_sca_bpm_3p03aX:pattern_number/240","yield_sca_bmod_ramp<100 && TMath::Abs(diff_sca_bpm_3p03aX)<0.060");

    cStability->Update();
    cStability->SaveAs(Form("%s/%s/%sBeamStability.png",pPath,www,filePre.Data()));
  }

  if(bEnergy) {
    helChain->SetBranchStatus("diff_sca_bpm_3c20Y*",1);
    //helChain->SetBranchStatus("diff_sca_bpm_3c20X*",1);
    helChain->SetBranchStatus("diff_sca_bmod_ramp*",1);
    ////helChain->SetBranchAddress("diff_sca_bpm_3c20X",&bpm_3c20X);
    TCanvas *cEn = new TCanvas("cEn",Form("energy parameters for run %d",runnum),0,0,800,500);
    cEn->Divide(2,1);// # of col,# of row
    cEn->cd(1);
    //helChain->Draw("diff_sca_bpm_3c20Y:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    helChain->Draw("diff_sca_bpm_3c20Y:pattern_number/240","yield_sca_bcm6>2 && yield_sca_bmod_ramp<100");
    cEn->cd(2);
    //helChain->Draw("diff_sca_bmod_ramp:pattern_number/240","yield_sca_bcm6>20 && yield_sca_bmod_ramp<100");
    helChain->Draw("diff_sca_bmod_ramp:pattern_number/240","yield_sca_bcm6>2 && yield_sca_bmod_ramp<100");
    cEn->SaveAs(Form("%s/%s/%sEnergyStability.png",pPath,www,filePre.Data()));
  }

  if(bLaser) {
    TCanvas *cPow = new TCanvas("cPow",Form("laser power for run %d",runnum),0,0,1000,500);///x,y cordinates of left top point; width, height of canvas
    cPow->Divide(2,1);
    cPow->cd(1);
    TH1D *hLaser = new TH1D("hLaser","hLaser",100,0,1);
    hLaser->SetBit(TH1::kCanRebin);

    //Double_t lasOnCut = 112000.0;///cannot use a variable inside the cut below
    helChain->Draw("yield_sca_laser_PowT>>hLaser","yield_sca_laser_PowT>=112000","goff");    
    //helChain->Draw("yield_sca_laser_PowT.raw>>hLaser","yield_sca_laser_PowT.raw>110.0","goff");    
    //helChain->Draw("yield_sca_laser_PowT/1000>>hLaser","","goff");    
    //cPow->GetPad(1)->SetLogx();
    cPow->GetPad(1)->SetLogy();
    hLaser->Draw();
    hLaser->GetXaxis()->SetTitle("laser on power");
    cPow->GetPad(1)->Update();///forces the generation of 'stats' box
    TPaveStats *ps1 = (TPaveStats*)hLaser->GetListOfFunctions()->FindObject("stats");
    ps1->SetX1NDC(0.20); ps1->SetX2NDC(0.45);
    ps1->SetFillStyle(0);
    ps1->SetTextColor(kBlue);
    cPow->GetPad(1)->Modified();

    cPow->cd(2);
    TH1D *h2 = new TH1D("h2","h2",100,0,1);
    h2->SetBit(TH1::kCanRebin);
    helChain->Draw("yield_sca_laser_PowT>>hLaser","yield_sca_laser_PowT<112000","goff");    
    //helChain->Draw("yield_sca_laser_PowT.raw>>h2","yield_sca_laser_PowT.raw<2.1","goff");    
    //helChain->Draw("yield_sca_laser_PowT>>h2","","goff");
    cPow->SetLogx();
    cPow->GetPad(2)->SetLogy();
    h2->GetXaxis()->SetTitle("laser off power");
    h2->Draw();
    h2->GetXaxis()->SetNdivisions(506, kTRUE);
    cPow->GetPad(2)->Update();///forces the generation of 'stats' box
    TPaveStats *ps2 = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
    ps2->SetX1NDC(0.2); ps2->SetX2NDC(0.45);
    ps2->SetTextColor(kBlack);
    ps2->SetFillStyle(0);
    cPow->GetPad(2)->Modified();

    cPow->SaveAs(Form("%s/%s/%slasPower.png",pPath,www,filePre.Data()));
  }

  gSystem->Exec(Form("%s/make_erun_page.sh %d",pPath, runnum));
  return nEntries;
}
