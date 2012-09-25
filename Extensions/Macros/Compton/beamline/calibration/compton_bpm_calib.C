///////////////////////////////////////////////////////////////////////////////
//  File: compton_bpm_calib.C
//  Author(s):  Juan Carlos Cornejo <cornejo@jlab.org>
//  Summary:    Compton scaler BPM calibration using a calibrated unser
//  Revisions:
//    * Tue Jun 26 2012 Juan Carlos Cornejo <cornejo@jlab.org>
//    - Initial script rewrite
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

void compton_bpm_calib(Int_t runNum,Double_t unserPed= 0.,
    Double_t unserCal = 2.5e-3)
{
  // Open up the rootfile
  TChain *chain = new TChain("Mps_Tree");
  chain->Add(Form("$QW_ROOTFILES/Compton_BPMCalib_%d.root",runNum));

  // Set the alias to the corrected unser current
  chain->SetAlias("unser",
      Form("((sca_unser.raw*4e6/sca_4mhz.raw-%f)*%f)",unserPed,unserCal));

  std::vector<Double_t> goodBeam[2];
  std::ifstream fileGoodBeam;
  std::vector<TCut> cutsGoodBeam;
  fileGoodBeam.open(Form("bpm_goodregion_%d.dat",runNum),std::ifstream::in);
  if(!fileGoodBeam.is_open()) {
    std::cerr << "Cannot open file bpm_goodregion_" << runNum
      << ".dat for reading! Exiting!" << std::endl;
    return;
  }
  Double_t start = 0;
  Double_t end;
  TCut cutGoodBeam;
  while(!fileGoodBeam.eof()) {
    fileGoodBeam >> start;
    fileGoodBeam >> end;
    goodBeam[0].push_back(start);
    goodBeam[1].push_back(end);
    cutsGoodBeam.push_back(Form("mps_counter>=%f&&mps_counter<=%f",start,end));
    cutGoodBeam = cutGoodBeam||cutsGoodBeam[cutsGoodBeam.size()-1];
  }

  // Display the UNSER to show good beam periods
  TCanvas *canvasUnser = new TCanvas("canvasUnser","Unser Canvas",600,800);
  canvasUnser->Divide(1,2);
    (void)new TH2F("hUnserAll","Unser vs Mps Counter; Mps Counter; Unser (#muA)",
      1123,0,chain->GetEntries(),1123,-1.,200.);
    (void)new TH2F("hUnserGood",
        "Good Unser Currents vs Mps Counter; Mps Counter; Unser (#muA)",
      1123,0,chain->GetEntries(),1123,-1.,200.);
  canvasUnser->cd(1);
  chain->Draw("unser:mps_counter>>hUnserAll");
  canvasUnser->cd(2);
  chain->Draw("unser:mps_counter>>hUnserGood",cutGoodBeam);

  // Get the unser values for each good time period
  std::vector<Double_t> unser;
  std::vector<Double_t> unserError;
  unser.resize(goodBeam[0].size());
  unserError.resize(goodBeam[0].size());

  TCanvas *canvasTemp = new TCanvas("canvasTemp","canvasBPMTemp",600,400);
  canvasTemp->cd(0);
  for(UInt_t i = 0; i < unser.size(); i++ ) {
    chain->Draw(Form("unser>>hUnser%d",i),cutsGoodBeam[i]);
    TH1F *h = (TH1F*)gDirectory->Get(Form("hUnser%d",i));
    if(h&&h->GetEntries()>0) {
      unser[i] = h->GetMean();
      unserError[i] = 0*TMath::Sqrt(
          // Add the standard unser 0.2 uA systematic error"
          0.04+TMath::Power(h->GetRMS(),2.)/h->GetEntries());
    }
  }

  // Now finally calibrate the compton BPMS individually
  ofstream out;
  out.open(Form("bpm_calibration_%d.map",runNum));

  TString bpmName[3] = {"3p02a","3p02b","3p03a"};
  TString antenna[4] = {"XP","XM","YP","YM"};
  TCanvas *canvasBPM[3];
  std::vector<Double_t> bpmValue[3][4];
  std::vector<Double_t> bpmValueError[3][4];
  std::vector<Double_t> bpmFitRes[3][4];
  std::vector<Double_t> bpmFitResError[3][4];
  Double_t bpmPed[3][4];
  Double_t bpmCal[3][4];
  TGraphErrors *graphBPM[3][4];
  TGraphErrors *graphBPMRes[3][4];
  for(Int_t bpm = 0; bpm < 3; bpm++ ) {
    canvasBPM[bpm] = new TCanvas(Form("canvasBPM%d",bpm),"canvas bpm",2400,800);
    canvasBPM[bpm]->Divide(4,2);
    for(Int_t a = 0; a < 4; a++ ) {

      // Extract noninal Raw bpm values
      for(UInt_t j = 0; j < unser.size(); j++) {
        canvasTemp->cd(0);
        chain->SetAlias(Form("sca_bpm_%s%s_freq",bpmName[bpm].Data(),
              antenna[a].Data()),
        Form("sca_bpm_%s%s.raw",
              bpmName[bpm].Data(),antenna[a].Data()));
        chain->Draw(Form("sca_bpm_%s%s_freq>>bpm%d_%s%d",
              bpmName[bpm].Data(),antenna[a].Data(),bpm,antenna[a].Data(),j),
            cutsGoodBeam[j]);
        TH1F *h =(TH1F*)gDirectory->Get(Form("bpm%d_%s%d",bpm,antenna[a].Data(),
              j));
        if(h&&h->GetEntries()>0) {
          bpmValue[bpm][a].push_back(h->GetMean());
          bpmValueError[bpm][a].push_back(h->GetRMS()/
            TMath::Sqrt(h->GetEntries()));
        }
      }

      // Make a graph of Raw BPM Frequency vs Unser Current and fit
      graphBPM[bpm][a] = new TGraphErrors(unser.size(),unser.data(),
          bpmValue[bpm][a].data(),
          unserError.data(),bpmValueError[bpm][a].data());
      graphBPM[bpm][a]->SetTitle(
          Form("BPM %s (%s) Raw Frequencies vs Unser current",bpmName[bpm].Data(),
            antenna[a].Data()));
      graphBPM[bpm][a]->GetXaxis()->SetTitle("unser (#muA)");
      graphBPM[bpm][a]->GetYaxis()->SetTitle(Form("Raw %s (Hz)",antenna[a].Data()));
      canvasBPM[bpm]->cd(a+1);
      TF1 *f = new TF1(Form("f%d",a),"pol1",100,200);
      // Fit, but ignore Xerror bars because otherwise ROOT becomes completely
      // incompetent when it tries to make a fit.
      //graphBPM[bpm][a]->Fit(f,"EX0");
      graphBPM[bpm][a]->Fit(f);
      graphBPM[bpm][a]->Draw("AP");
      bpmPed[bpm][a] = f->GetParameter(0);
      bpmCal[bpm][a] = 1/f->GetParameter(1);

      // Compute the residuals in terms of percentages
      for(UInt_t j = 0; j < unser.size(); j++) {
        canvasTemp->cd(0);
        chain->Draw(
            Form("100*(sca_bpm_%s%s_freq-(unser/%f+%f))/sca_bpm_%s%s_freq>>Rbpm%d_%s%d",
              bpmName[bpm].Data(),antenna[a].Data(),bpmCal[bpm][a],
              bpmPed[bpm][a],bpmName[bpm].Data(),antenna[a].Data(),bpm,
              antenna[a].Data(),j),
            cutsGoodBeam[j]);
        TH1F *h =(TH1F*)gDirectory->Get(Form("Rbpm%d_%s%d",bpm,antenna[a].Data(),
              j));
        if(h&&h->GetEntries()>0) {
          bpmFitRes[bpm][a].push_back(h->GetMean());
          bpmFitResError[bpm][a].push_back(TMath::Sqrt(
            h->GetRMS()*h->GetRMS()/h->GetEntries()+
            0*TMath::Power(((0.02/bpmCal[bpm][a])+bpmPed[bpm][a])/bpmValue[bpm][a][j],2)));
        }
      }

      // And make a graph of the residuals vs unser current
      graphBPMRes[bpm][a] = new TGraphErrors(unser.size(),unser.data(),
          bpmFitRes[bpm][a].data(),
          unserError.data(),bpmFitResError[bpm][a].data());
      graphBPMRes[bpm][a]->SetTitle(
          Form("BPM %s (%s)  Residuals vs Unser current",bpmName[bpm].Data(),
            antenna[a].Data()));
      graphBPMRes[bpm][a]->GetXaxis()->SetTitle("unser (#muA)");
      graphBPMRes[bpm][a]->GetYaxis()->SetTitle(Form("%s Fit Residual (%%)",
            antenna[a].Data()));
      canvasBPM[bpm]->cd(a+5);
      graphBPMRes[bpm][a]->Draw("AP");
      out << "sca_bpm_"  << bpmName[bpm] << antenna[a] << ",\t" <<
        bpmPed[bpm][a] << ",\t" << bpmCal[bpm][a] << ",\t1.0,\t0.0" << std::endl;
      canvasBPM[bpm]->SaveAs(Form("www/%s_calib_%d.png",bpmName[bpm].Data(),runNum));
    }
  }
}
