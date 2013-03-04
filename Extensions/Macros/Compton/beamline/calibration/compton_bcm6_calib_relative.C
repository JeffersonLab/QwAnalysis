///////////////////////////////////////////////////////////////////////////////
//  File: compton_bcm_calib.C
//  Author(s):  Juan Carlos Cornejo <cornejo@jlab.org>
//  Summary:    Compton scaler BCM calibration using a 4mhz clock and bcm1
//  Revisions:
//    * Tue Jun 26 2012 Juan Carlos Cornejo <cornejo@jlab.org>
//    - Fixed percent residual plots, and added scaler error code cuts
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
  Double_t bcm1;
  Double_t bcm1Error;
  bool operator<(const BCMData &a)
  {
    return bcm1 < a.bcm1;
  }
  // Constructor
  BCMData(Double_t v,Double_t ve,Double_t u,Double_t ue)
  {
    value = v;
    valueError = ve;
    bcm1 = u;
    bcm1Error = ue;
  }
};

void compton_bcm6_calib_relative(Int_t runNum,Double_t lowI=-1.00,Double_t highI=200.0)
{
  // This analysis will calibrate BCM17 (known as BCM6) relative to the other
  // two BCM's. This is because we lost the bcm1 signal at some point

  // Define the beam cuts
  TCut beamCuts = Form("(bcm1>=%f&&bcm1<=%f)",lowI,highI);
  TCut sanityCuts = "(sca_4mhz.raw<5e3&&sca_4mhz.raw>2000)";

  // Define scaler error flag cuts
  TCut errorFlagCuts[4] = {
    "1","1","1","1"
/*    "sca_bcm1.Device_Error_Code!=0",
    "sca_bcm2.Device_Error_Code!=0",
    "sca_bcm6.Device_Error_Code!=0",
    "sca_bcm1.Device_Error_Code!=0",
    */
  };

  // Open up the rootfile
  TChain *chain = new TChain("Mps_Tree");
  std::cout << "Opened " << chain->Add(Form("$QW_ROOTFILES/Compton_BCMCalib_%d.root",runNum))
    << " rootfiles." << std::endl;

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
    //std::cout << "Off s: " << start << " \te: " << end << std::endl;
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
    //std::cout << "On s: " << start << " \te: " << end << std::endl;
  }

  /////////////////////////////////////////////////////////////////////////////
  // The first step is to calibrate the BCM1
  chain->SetAlias("bcm1_raw","sca_bcm1.raw*4e6/sca_4mhz.raw");
  TCanvas *bcm1Canvas = new TCanvas("bcm1Canvas","bcm1Canvas",1200,800);
  bcm1Canvas->Divide(2,2);
  bcm1Canvas->cd(1);
  /*TH2F *hBCM1Raw = */(void) new TH2F("hBCM1Raw",
      "BCM1 Frequency vs Mps Counter;Mps Counter;Frequency (Hz)",
      1111,0,chain->GetEntries(),1111,2e5,6e5);
  TH2F *hBCM1RawGoodOn = new TH2F("hBCM1RawGoodOn",
      "BCM1 Good Frequencies vs Mps Counter;Mps Counter;Frequency (Hz)",
      1111,0,chain->GetEntries(),1111,2e5,6e5);
  TH2F *hBCM1RawGoodOff = new TH2F("hBCM1RawGoodOff",
      "BCM1 Good Frequencies vs Mps Counter;Mps Counter;Frequency (Hz)",
      1111,0,chain->GetEntries(),1111,2e5,6e5);
  TH1F *hBCM1RawPedestal = new TH1F("hBCM1RawPedestal",
      "BCM1 Raw Pedestal;Frequency (Hz)",
      1111,2.3e5,2.7e5);
  hBCM1RawGoodOn->SetMarkerColor(3);
  hBCM1RawGoodOff->SetMarkerColor(2);
  /*TH2F *hBCM1Calibrated =*/(void) new TH2F("hBCM1Calibrated",
      "BCM1 Calibrated Current vs Mps Counter;Mps Counter;BCM1 (#muA)",
      1111,0,chain->GetEntries(),1111,-5.,200.);
  chain->Draw("bcm1_raw:mps_counter>>hBCM1Raw",errorFlagCuts[3]&&sanityCuts);
  bcm1Canvas->cd(2);
  TCut beamOnCuts = 0;
  for(Int_t i = 0; i < (Int_t)beamOnCut.size(); i++ ) {
    beamOnCuts = beamOnCuts||beamOnCut[i];
  }
  chain->Draw("bcm1_raw:mps_counter>>hBCM1RawGoodOn",
      beamOnCuts&&errorFlagCuts[3]&&sanityCuts);
  TCut beamOffCuts = 0;
  for(Int_t i = 0; i < (Int_t)beamOffCut.size(); i++ ) {
    beamOffCuts = beamOffCuts||beamOffCut[i];
  }
  chain->Draw("bcm1_raw:mps_counter>>hBCM1RawGoodOff",
      beamOffCuts&&errorFlagCuts[3]&&sanityCuts,"SAME");

  bcm1Canvas->cd(3);
  hBCM1RawPedestal->SetStats(kTRUE);
  chain->Draw("bcm1_raw>>hBCM1RawPedestal",
      beamOffCuts&&errorFlagCuts[3]&&sanityCuts);
  TSpectrum *sBCM1 = new TSpectrum(2); // 20 max peaks to return
  Int_t bcm1PeaksFound = 0;
  bcm1PeaksFound = sBCM1->Search(hBCM1RawPedestal,3,"noMarkov",0.05);
  delete hBCM1RawPedestal;
  hBCM1RawPedestal = new TH1F("hBCM1RawPedestal",
      "BCM1 Raw Pedestal;Frequency (Hz)",
      1111,sBCM1->GetPositionX()[0]*0.9,sBCM1->GetPositionX()[0]*1.1);
  chain->Draw("bcm1_raw>>hBCM1RawPedestal",
      beamOffCuts&&errorFlagCuts[3]&&sanityCuts);
//  Double_t bcm1Pedestal = hBCM1RawPedestal->GetMean();
//  Double_t bcm1PedestalError = hBCM1RawPedestal->GetRMS()/
//    TMath::Sqrt(hBCM1RawPedestal->GetEntries());
  Double_t bcm1Pedestal = 250436;
  Double_t bcm1PedestalError = 0.25;

  std::cout << "BCM1 pedestal is (" << bcm1Pedestal
    << " +/- " << bcm1PedestalError << ") Hz" << std::endl;

  // Now define the calibrated bcm1 alias
  // Note: Dave G. claims the usual systematic uncertainty on the
  // bcm1 is about +/- 0.2uA
  chain->SetAlias("bcm1",Form("(bcm1_raw-%f)*0.000563251",bcm1Pedestal));
  bcm1Canvas->cd(4);
  chain->Draw("bcm1:mps_counter>>hBCM1Calibrated",
      (beamOffCuts||beamOnCuts)&&"(bcm1>=-1)"&&errorFlagCuts[3]&&sanityCuts);
  bcm1Canvas->SaveAs(Form("www/bcm1_calib_%d.png",runNum));

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
  Double_t *bcm1[3];
  Double_t *bcm1Err[3];
  Double_t *bcmResP[3];
  Double_t *bcmResC[3];
  Double_t *bcmResPErr[3];
  Double_t *bcmResCErr[3];
  Double_t bcmPed[3];
  Double_t bcmCal[3];

  std::vector<BCMData> data[3];
  std::vector<BCMData> dataResC[3];
  Int_t bcms[3] = {1,2,6};
  Int_t bcm = 0;
  // Only do BCM 6 calibration
  for(Int_t i = 2; i < 3; i++ ) {
    bcm = bcms[i];
    chain->SetAlias(Form("bcm%d_raw",bcm),
        Form("sca_bcm%d.raw*4e6/sca_4mhz.raw",bcm));
    for(Int_t j = 0; j < (Int_t)beamOnCut.size(); j++ ) {
      bcmTempCanvas->cd(1);
      chain->Draw(Form("bcm%d_raw>>hBCM%d_%d",bcm,bcm,j),
        beamOnCut[j]&&beamCuts&&errorFlagCuts[i]&&errorFlagCuts[3]&&sanityCuts);
      bcmTempCanvas->cd(2);
      chain->Draw(Form("bcm1>>hBCM1%d_%d",bcm,j),
        beamOnCut[j]&&beamCuts&&errorFlagCuts[i]&&errorFlagCuts[3]&&sanityCuts);
      TH1F *hB = (TH1F*)gDirectory->Get(Form("hBCM%d_%d",bcm,j));
      TH1F *hU = (TH1F*)gDirectory->Get(Form("hBCM1%d_%d",bcm,j));
      if(hB->GetEntries()>0&&hU->GetEntries()>0) {
        data[i].push_back(BCMData(
              hB->GetMean(),
              hB->GetRMS()/TMath::Sqrt(hB->GetEntries()),
              hU->GetMean(),
              // Add the 0.2uA systematic uncertainty in quadrature to the bcm1
              // statistical uncertainty
              TMath::Sqrt(0.04+TMath::Power(hU->GetRMS(),2)/hU->GetEntries())
              ));
        //bcmTempCanvas->SaveAs(Form("www/temp_bcm%d_%d_%d.png",bcm,j,runNum));
      }
    }

    Int_t numData = data[i].size();
    bcmRaw[i] = new Double_t[numData];
    bcmRawErr[i] = new Double_t[numData];
    bcmResP[i] = new Double_t[numData];
    bcmResC[i] = new Double_t[numData];
    bcmResPErr[i] = new Double_t[numData];
    bcmResCErr[i] = new Double_t[numData];
    bcm1[i] = new Double_t[numData];
    bcm1Err[i] = new Double_t[numData];
    for(Int_t k = 0; k < numData; k++ ) {
      bcmRaw[i][k] = data[i][k].value;
      bcmRawErr[i][k] = data[i][k].valueError;
      bcm1[i][k] = data[i][k].bcm1;
      bcm1Err[i][k] = data[i][k].bcm1Error;
    }
    // Alright, now make a graph out of them and extract those pedestals and
    // calibration factors!
    bcmCanvas[i] = new TCanvas(Form("bcmCanvas%d",bcm),"BCM Calibration Canvas",600,1200);
    bcmCanvas[i]->Divide(1,3);
    bcmCanvas[i]->cd(1);
    bcmGraphRaw[i] = new TGraphErrors(numData,bcm1[i],bcmRaw[i],bcm1Err[i],
        bcmRawErr[i]);
    bcmGraphRaw[i]->SetTitle(
        Form("BCM %d Raw Frequencies vs BCM1 Current (#muA)",bcm));
    bcmGraphRaw[i]->GetYaxis()->SetTitle(Form("BCM %d Raw Frequencies (Hz)",
          bcm));
    bcmGraphRaw[i]->GetXaxis()->SetTitle("bcm1 (#muA)");
    bcmGraphRaw[i]->Draw("AP");
    bcmGraphRaw[i]->Fit("pol1");
    TF1 *f = (TF1*)bcmGraphRaw[i]->GetFunction("pol1");
    bcmPed[i] = f->GetParameter(0);
    bcmCal[i] = 1/f->GetParameter(1);
    chain->SetAlias(Form("bcm%d",bcm),
        Form("(bcm%d_raw-%f)*%f",bcm,bcmPed[i],bcmCal[i]));
    for(Int_t j = 0; j < (Int_t)beamOnCut.size(); j++ ) {
      bcmTempCanvas->cd(1);
      chain->Draw(Form("bcm%d-bcm1>>hBCMRC%d_%d",bcm,bcm,j),
        beamOnCut[j]&&beamCuts&&errorFlagCuts[i]&&errorFlagCuts[3]&&sanityCuts);
      bcmTempCanvas->cd(2);
      TH1F *hC = (TH1F*)gDirectory->Get(Form("hBCMRC%d_%d",bcm,j));
      if(hC->GetEntries()>0) { //&&hP->GetEntries()>0) {
        dataResC[i].push_back(BCMData(
              hC->GetMean(),
              // Add the 0.2uA systematic uncertainty in quadrature to the bcm1
              // statistical uncertainty
              TMath::Sqrt(0.04+TMath::Power(
              hC->GetRMS()/TMath::Sqrt(hC->GetEntries()),2.)),0.,0.));
      }
    }
    numData = dataResC[i].size();
    for(Int_t k = 0; k < numData; k++ ) {
      bcmResC[i][k] = dataResC[i][k].value;
      bcmResCErr[i][k] = dataResC[i][k].valueError;
      bcmResP[i][k] = 100*bcmResC[i][k]/bcm1[i][k];
      bcmResPErr[i][k] = 100*dataResC[i][k].valueError/bcm1[i][k];
    }
    bcmCanvas[i]->cd(2);
    bcmHResC[i] = new TH2F(Form("hResC%d",bcm),
        Form("BCM %d Fit Residuals;BCM1 (#muA);Residual (#muA)",
          bcm),1001,0.,200.,1008,-1.0,1.0);
    bcmGraphResC[i] = new TGraphErrors(numData,bcm1[i],bcmResC[i],bcm1Err[i],
        bcmResCErr[i]);
    bcmHResC[i]->SetStats(kFALSE);
    bcmHResC[i]->Draw();
    bcmGraphResC[i]->Draw("PS");

    bcmCanvas[i]->cd(3);
    bcmHResP[i] = new TH2F(Form("hResP%d",bcm),
          Form("BCM %d Fit Residuals;BCM1 (#muA);Percent",
          bcm),1001,0.,200.,1008,-2.5,2.5);
    bcmHResP[i]->Draw();
    bcmGraphResP[i] = new TGraphErrors(numData,bcm1[i],bcmResP[i],bcm1Err[i],
        bcmResPErr[i]);
    bcmHResP[i]->SetStats(kFALSE);
    bcmGraphResP[i]->Draw("PS");
    bcmCanvas[i]->SaveAs(Form("www/bcm%d_calib_%d.png",bcm,runNum));
  }

  ofstream out;
  out.open(Form("bcm_calibration_%d.map",runNum));
  out << "sca_bcm1,        " << bcm1Pedestal << ",        0.0025000"
    << std::endl;
  for(Int_t i = 2; i < 3; i++ ) {
    out << "sca_bcm" << bcms[i] << ",        " << bcmPed[i] << ",        "
      << bcmCal[i] << std::endl;
    std::cout << "sca_bcm" << bcms[i] << ",        " << bcmPed[i] << ",        "
      << bcmCal[i] << std::endl;
  }
//  std::cout << "sca_bcm1,        " << bcm1Pedestal << ",        0.0025000"
//    << std::endl;

  // We don't need no stinking pointers!!
  delete chain;
  delete bcm1Canvas;
  delete hBCM1RawGoodOn;
  delete hBCM1RawGoodOff;
  delete hBCM1RawPedestal;
  delete sBCM1;
  delete bcmTempCanvas;
  for(Int_t i = 2; i < 3; i++ ) {
    delete bcmCanvas[i];
    delete bcmGraphRaw[i];
    delete bcmHResC[i];
    delete bcmGraphResC[i];
    delete bcmHResP[i];
    delete bcmGraphResP[i];
  }
}
