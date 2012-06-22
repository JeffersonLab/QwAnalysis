///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

// ROOT Includes
#include <TROOT.h>
#include <TStyle.h>
#include <TChain.h>
#include <TProfile.h>
#include <TSpectrum.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TMath.h>
#include <TCut.h>
#include <TGraphErrors.h>

// Standard library includes
#include <fstream>

////////////////////////////////////////////////////////////////////////////////
// BCM Data structure
struct BCMData {
  Double_t value;
  Double_t valueError;
  Double_t unser;
  Double_t unserError;
  bool operator<(const BCMData &a)
  {
    return unser < a.unser;
  }
  // Constructor
  BCMData(Double_t v,Double_t ve,Double_t u,Double_t ue)
  {
    value = v;
    valueError = ve;
    unser = u;
    unserError = ue;
  }
};

void compton_bcm_calib(Int_t runNum,Double_t lowI=5.00,Double_t highI=180.0)
{
  // Define the beam cuts
  TCut beamCuts = Form("(unser>=%f&&unser<=%f)",lowI,highI);

  // Fit and stat parameters
  //gStyle->SetOptFit(1111);
  //gStyle->SetOptStat(0000000);
  //gStyle->SetStatH(0.1);
  //gStyle->SetStatW(0.2);

  // Open up the rootfile
  TChain *chain = new TChain("Mps_Tree");
  chain->Add(Form("$QW_ROOTFILES/Compton_BCMCalib_%d.root",runNum));

  // Read in beam ON/OFF
  std::vector<Double_t> goodBeamOn[2];
  std::vector<Double_t> goodBeamOff[2];
  std::vector<TCut> beamOffCut;
  std::vector<TCut> beamOnCut;
  std::ifstream fileBeamOff;
  std::ifstream fileBeamOn;
  fileBeamOff.open(Form("bcm_beamoff_%d.dat",runNum), ifstream::in);
  fileBeamOn.open(Form("bcm_beamon_%d.dat",runNum), ifstream::in);
  if(!fileBeamOff.is_open()||!fileBeamOn.is_open()) {
    std::cout << "Error, cannot read from bcm_beam{on,off}_" << runNum << ".dat"
      << std::endl;
    return;
  }

  Double_t start = 0;
  Double_t end;
  while(!fileBeamOff.eof()) {
    fileBeamOff >> start;
    fileBeamOff >> end;
    goodBeamOff[0].push_back(start);
    goodBeamOff[1].push_back(end);
    beamOffCut.push_back(Form("mps_counter>=%f&&mps_counter<=%f",start,end));
  }
  while(!fileBeamOn.eof()) {
    fileBeamOn >> start;
    fileBeamOn >> end;
    goodBeamOn[0].push_back(start);
    goodBeamOn[1].push_back(end);
    beamOnCut.push_back(Form("(mps_counter>=%f&&mps_counter<=%f)",start,end));
  }

  /////////////////////////////////////////////////////////////////////////////
  // The first step is to calibrate the Unser
  chain->SetAlias("unser_raw","sca_unser.raw*4e6/sca_4mhz.raw");
  TCanvas *unserCanvas = new TCanvas("unserCanvas","unserCanvas",1200,800);
  unserCanvas->Divide(2,2);
  unserCanvas->cd(1);
  TH2F *hUnserRaw = new TH2F("hUnserRaw",
      "Unser Frequency vs Mps Counter;Mps Counter;Frequency (Hz)",
      1111,0,chain->GetEntries(),1111,3e5,5e5);
  (void)hUnserRaw; // Remove annoying unsed variable warning
  TH2F *hUnserRawGoodOn = new TH2F("hUnserRawGoodOn",
      "Unser Good Frequencies vs Mps Counter;Mps Counter;Frequency (Hz)",
      1111,0,chain->GetEntries(),1111,3e5,5e5);
  TH2F *hUnserRawGoodOff = new TH2F("hUnserRawGoodOff",
      "Unser Good Frequencies vs Mps Counter;Mps Counter;Frequency (Hz)",
      1111,0,chain->GetEntries(),1111,3e5,5e5);
  TH1F *hUnserRawPedestal = new TH1F("hUnserRawPedestal",
      "Unser Raw Pedestal;Frequency (Hz)",
      1111,3e5,5e5);
  hUnserRawGoodOn->SetMarkerColor(3);
  hUnserRawGoodOff->SetMarkerColor(2);
  TH2F *hUnserCalibrated = new TH2F("hUnserCalibrated",
      "Unser Calibrated Current vs Mps Counter;Mps Counter;Unser (#muA)",
      1111,0,chain->GetEntries(),1111,-5.,200.);
  chain->Draw("unser_raw:mps_counter>>hUnserRaw");
  unserCanvas->cd(2);
  TCut beamOnCuts = 0;
  for(Int_t i = 0; i < (Int_t)beamOnCut.size(); i++ ) {
    beamOnCuts = beamOnCuts||beamOnCut[i];
  }
  chain->Draw("unser_raw:mps_counter>>hUnserRawGoodOn",beamOnCuts);
  TCut beamOffCuts = 0;
  for(Int_t i = 0; i < (Int_t)beamOffCut.size(); i++ ) {
    beamOffCuts = beamOffCuts||beamOffCut[i];
  }
  chain->Draw("unser_raw:mps_counter>>hUnserRawGoodOff",
      beamOffCuts,"SAME");

  unserCanvas->cd(3);
  hUnserRawPedestal->SetStats(kTRUE);
  chain->Draw("unser_raw>>hUnserRawPedestal",beamOffCuts);
  TSpectrum *sUnser = new TSpectrum(2); // 20 max peaks to return
  Int_t unserPeaksFound = 0;
  unserPeaksFound = sUnser->Search(hUnserRawPedestal,3,"noMarkov",0.05);
  delete hUnserRawPedestal;
  hUnserRawPedestal = new TH1F("hUnserRawPedestal",
      "Unser Raw Pedestal;Frequency (Hz)",
      1111,sUnser->GetPositionX()[0]*0.9,sUnser->GetPositionX()[0]*1.1);
  chain->Draw("unser_raw>>hUnserRawPedestal",beamOffCuts);
  Double_t unserPedestal = hUnserRawPedestal->GetMean();
  Double_t unserPedestalError = hUnserRawPedestal->GetRMS()/
    TMath::Sqrt(hUnserRawPedestal->GetEntries());

  std::cout << "Unser pedestal is (" << unserPedestal
    << " +/- " << unserPedestalError << ") Hz" << std::endl;

  // Now define the calibrated unser alias
  // Note: Dave G. claims the usual systematic uncertainty on the
  // unser is about +/- 0.2uA
  chain->SetAlias("unser",Form("(unser_raw-%f)*2.5e-3",unserPedestal));
  unserCanvas->cd(4);
  chain->Draw("unser:mps_counter",beamOffCuts||beamOnCuts);
  unserCanvas->SaveAs(Form("www/unser_calib_%d.png",runNum));

  /////////////////////////////////////////////////////////////////////////////
  // Finally, we can calibrate the three compton bcm's individually
  TCanvas *bcmTempCanvas = new TCanvas("bcmTempCanvas");
  bcmTempCanvas->Divide(1,2);
  TCanvas *bcmCanvas[3];
  TGraphErrors *bcmGraphRaw[3];
  TGraphErrors *bcmGraphResC[3];
  TGraphErrors *bcmGraphResP[3];
  TH2F* bcmHResC[3];
  TH2F* bcmHResP[3];

  Double_t *bcmRaw[3];
  Double_t *bcmRawErr[3];
  Double_t *unser[3];
  Double_t *unserErr[3];
  Double_t *bcmResP[3];
  Double_t *bcmResC[3];
  Double_t *bcmResPErr[3];
  Double_t *bcmResCErr[3];
  Double_t bcmPed[3];
  Double_t bcmCal[3];

  std::vector<BCMData> data[3];
  std::vector<BCMData> dataResC[3];
  std::vector<BCMData> dataResP[3];
  Int_t bcms[3] = {1,2,6};
  Int_t bcm = 0;
  for(Int_t i = 0; i < 3; i++ ) {
    bcm = bcms[i];
    chain->SetAlias(Form("bcm%d_raw",bcm),
        Form("sca_bcm%d.raw*4e6/sca_4mhz.raw",bcm));
    for(Int_t j = 0; j < (Int_t)beamOnCut.size(); j++ ) {
      bcmTempCanvas->cd(1);
      chain->Draw(Form("bcm%d_raw>>hBCM%d_%d",bcm,bcm,j),
        beamOnCut[j]&&beamCuts);
      bcmTempCanvas->cd(2);
      chain->Draw(Form("unser>>hUnser%d_%d",bcm,j),
        beamOnCut[j]&&beamCuts);
      TH1F *hB = (TH1F*)gDirectory->Get(Form("hBCM%d_%d",bcm,j));
      TH1F *hU = (TH1F*)gDirectory->Get(Form("hUnser%d_%d",bcm,j));
      if(hB->GetEntries()>0&&hU->GetEntries()>0) {
        data[i].push_back(BCMData(
              hB->GetMean(),
              hB->GetRMS()/TMath::Sqrt(hB->GetEntries()),
              hU->GetMean(),
              // Add the 0.2uA systematic uncertainty in quadrature to the unser
              // statistical uncertainty
              TMath::Sqrt(0.04+TMath::Power(hU->GetRMS(),2)/hU->GetEntries())
              ));
      }
    }

    Int_t numData = data[i].size();
    bcmRaw[i] = new Double_t[numData];
    bcmRawErr[i] = new Double_t[numData];
    bcmResP[i] = new Double_t[numData];
    bcmResC[i] = new Double_t[numData];
    bcmResPErr[i] = new Double_t[numData];
    bcmResCErr[i] = new Double_t[numData];
    unser[i] = new Double_t[numData];
    unserErr[i] = new Double_t[numData];
    for(Int_t k = 0; k < numData; k++ ) {
      bcmRaw[i][k] = data[i][k].value;
      bcmRawErr[i][k] = data[i][k].valueError;
      unser[i][k] = data[i][k].unser;
      unserErr[i][k] = data[i][k].unserError;
    }
    // Alright, now make a graph out of them and extract those pedestals and
    // calibration factors!
    bcmCanvas[i] = new TCanvas(Form("bcmCanvas%d",bcm),"BCM Calibration Canvas",600,12000);
    bcmCanvas[i]->Divide(1,3);
    bcmCanvas[i]->cd(1);
    bcmGraphRaw[i] = new TGraphErrors(numData,unser[i],bcmRaw[i],unserErr[i],
        bcmRawErr[i]);
    bcmGraphRaw[i]->SetTitle(
        Form("BCM %d Raw Frequencies vs Unser Current (#muA)",bcm));
    bcmGraphRaw[i]->GetYaxis()->SetTitle(Form("BCM %d Raw Frequencies (Hz)",
          bcm));
    bcmGraphRaw[i]->GetXaxis()->SetTitle("unser (#muA)");
    bcmGraphRaw[i]->Draw("AP");
    bcmGraphRaw[i]->Fit("pol1");
    TF1 *f = (TF1*)bcmGraphRaw[i]->GetFunction("pol1");
    bcmPed[i] = f->GetParameter(0);
    bcmCal[i] = 1/f->GetParameter(1);
    chain->SetAlias(Form("bcm%d",bcm),
        Form("(bcm%d_raw-%f)*%f",bcm,bcmPed[i],bcmCal[i]));
    for(Int_t j = 0; j < (Int_t)beamOnCut.size(); j++ ) {
      bcmTempCanvas->cd(1);
      chain->Draw(Form("bcm%d-unser>>hBCMRC%d_%d",bcm,bcm,j),
        beamOnCut[j]&&beamCuts);
      chain->Draw(Form("100*(bcm%d_raw-(unser/%f+%f))/bcm%d_raw>>hBCMRP%d_%d",
            bcm,bcmCal[i],bcmPed[i],bcm,bcm,j),
        beamOnCut[j]&&beamCuts);
      bcmTempCanvas->cd(2);
      TH1F *hC = (TH1F*)gDirectory->Get(Form("hBCMRC%d_%d",bcm,j));
      TH1F *hP = (TH1F*)gDirectory->Get(Form("hBCMRP%d_%d",bcm,j));
      if(hC->GetEntries()>0&&hP->GetEntries()>0) {
        dataResC[i].push_back(BCMData(
              hC->GetMean(),
              hC->GetRMS()/TMath::Sqrt(hC->GetEntries()),0.,0.));
        dataResP[i].push_back(BCMData(
              hP->GetMean(),
              hP->GetRMS()/TMath::Sqrt(hP->GetEntries()),0.,0.));
      }
    }
    numData = dataResC[i].size();
    for(Int_t k = 0; k < numData; k++ ) {
      bcmResC[i][k] = dataResC[i][k].value;
      bcmResCErr[i][k] = dataResC[i][k].valueError;
      bcmResP[i][k] = dataResP[i][k].value;
      bcmResPErr[i][k] = dataResP[i][k].valueError;
    }
    bcmCanvas[i]->cd(2);
    bcmHResC[i] = new TH2F(Form("hResC%d",bcm),
        Form("BCM %d Fit Residuals;Unser (#muA);Residual (#muA)",
          bcm),1001,0.,200.,1008,-0.5,0.5);
    bcmGraphResC[i] = new TGraphErrors(numData,unser[i],bcmResC[i],unserErr[i],
        bcmResCErr[i]);
    bcmHResC[i]->SetStats(kFALSE);
    bcmHResC[i]->Draw();
    bcmGraphResC[i]->Draw("PS");

    bcmCanvas[i]->cd(3);
    bcmHResP[i] = new TH2F(Form("hResP%d",bcm),
          Form("BCM %d Fit Residuals;Unser (#muA);Percent",
          bcm),1001,0.,200.,1008,-0.5,0.5);
    bcmHResP[i]->Draw();
    bcmGraphResP[i] = new TGraphErrors(numData,unser[i],bcmResP[i],unserErr[i],
        bcmResPErr[i]);
    bcmHResP[i]->SetStats(kFALSE);
    bcmGraphResP[i]->Draw("PS");
    bcmCanvas[i]->SaveAs(Form("www/bcm%d_calib_%d.png",bcm,runNum));
  }

  ofstream out;
  out.open(Form("bcm_calibration_%d.map",runNum));
  for(Int_t i = 0; i < 3; i++ ) {
    out << "sca_bcm" << bcms[i] << "        " << bcmPed[i] << "        "
      << bcmCal[i] << std::endl;

    std::cout << "sca_bcm" << bcms[i] << "        " << bcmPed[i] << "        "
      << bcmCal[i] << std::endl;
  }

  // We no longer need the temp canvas
  delete bcmTempCanvas;

  if(chain)
    delete chain;
}
