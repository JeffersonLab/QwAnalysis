// Standard Lib includes
#include <iostream>

// ROOT Includes
#include "TChain.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGraph.h"
#include "TH2F.h"

// Compton Includes
#include "getChain.C"

Int_t beamline(Int_t runnum, Bool_t isFirst100k = kFALSE, Bool_t deleteOnExit = kFALSE)
{
  gROOT->Reset();

  // Get the chain
  TChain *mpsChain = getMpsChain(runnum,isFirst100k);
  if (!mpsChain) {
    std::cout << "What a scam!! We didn't even get data!\n";
    return -1;
  }

  // Prepare the canvas
  TCanvas *canvas = new TCanvas("BeamLineCanvas","Beam Line Canvas",1024,1600);
  canvas->Divide(2,4);
  canvas->SetFillColor(0);

  // Prepare the web url
  TString www = TString(getenv("QWSCRATCH")) + Form("/www/run_%d/",runnum);
  gSystem->mkdir(www,true);
  TString canvas1 = www+"beamline_plots.png";

  // Now plot all the stuff out
  canvas->cd(1);
  mpsChain->Draw("sca_bpm_3p02aX:mps_counter","compton_charge>25");
  canvas->cd(2);
  mpsChain->Draw("sca_bpm_3p02aY:mps_counter","compton_charge>25");
  canvas->cd(3);
  mpsChain->Draw("sca_bpm_3p02bX:mps_counter","compton_charge>25");
  canvas->cd(4);
  mpsChain->Draw("sca_bpm_3p02bY:mps_counter","compton_charge>25");
  canvas->cd(5);
  mpsChain->Draw("sca_bpm_3p03aX:mps_counter","compton_charge>25");
  canvas->cd(6);
  mpsChain->Draw("sca_bpm_3p03aY:mps_counter","compton_charge>25");
  canvas->cd(7);
  mpsChain->Draw("sca_bpm_3c20X:mps_counter","compton_charge>25");
  canvas->cd(8);
  mpsChain->Draw("sca_bpm_3c20Y:mps_counter","compton_charge>25");

  // Save it!
  canvas->SaveAs(canvas1);

  if ( deleteOnExit == kTRUE ) {
    delete mpsChain;
    delete canvas;
  }

  return 0;

}
