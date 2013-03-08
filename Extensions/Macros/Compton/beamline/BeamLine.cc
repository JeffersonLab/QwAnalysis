#include "BeamLine.h"
#include <iostream>

#include <TCanvas.h>
#include <TChain.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TPad.h>

// Setup the useful styles
Style_t BeamLine::fStyles[3] = {4,7,8};
Color_t BeamLine::fColors[3] = { 2, 3, 4 };

void BeamLine::init(ComptonSession *session)
{
  VComptonMacro::init(session);
  VComptonMacro::SetName("BeamLine");
  gROOT->SetStyle("Modern");

  // BCM canvas setup
  fCanvasBCM = new TCanvas("BCMCanvas","BCM Canvas", 800, 1600);
  fCanvasBCM->Divide(1,3);
  fCanvasBCM->SetFillColor(0.0); // Set it to white
  for(Int_t i = 1; i < 4; i++ ) {
    fCanvasBCM->cd(i);
    gPad->Divide(1,2,0.01,0.008);
  }

  // BPM canvases setup
  TString axis[2] = {"X","Y"};
  for(Int_t a = 0; a < 2; a++ ) {
    fCanvasBPM[a] = new TCanvas(Form("BPMCanvas%s",axis[a].Data()),
          Form("BPM Canvas %s Axis",axis[a].Data()), 800, 1600);
    fCanvasBPM[a]->Divide(1,3);
    fCanvasBPM[a]->SetFillColor(0.0); // Set it to white
    for(Int_t i = 1; i < 4; i++ ) {
      fCanvasBPM[a]->cd(i);
      gPad->Divide(1,2,0.01,0.008);
    }
  }

  // Temp canvas setup
  fCanvasTemp = new TCanvas("temp","temp",800,800);
}

void BeamLine::run()
{
  std::cout << "Processing BCMs..." << std::endl;
  ProcessBCMs();
  std::cout << "Processing BPMs..." << std::endl;
  ProcessBPMs();
}


void BeamLine::ProcessBCMs()
{
  Int_t bcms[3] = {1,2,6};
  // Average values
  std::vector<Double_t> c[2][3][2]; // Two trees, three bcms, two states
  std::vector<Double_t> cE[2][3][2]; // Two trees, three bcms, two states
  // Differences
  std::vector<Double_t> dC[2][3][2];
  std::vector<Double_t> dCE[2][3][2];
  // Cycle numbers
  std::vector<Double_t> n[2];
  std::vector<Double_t> nE[2];

  TString states[2] = {"On","Off"};
  TCut laserCuts[2];

  // Get some limits for the graphs I'll make later
  Double_t low = 0.0;
  Double_t high = 1.0;
  if(cycles(1).NumberOfCycles(0)>0) {
    low = cycles(1).start[0];
    high = cycles(1).end[cycles(1).NumberOfCycles(0)-1];
  }

  // Alright, we are now ready to process these monitors
  fCanvasTemp->cd(0);
  Double_t tV = 0.0; // A temporary value
  Double_t tE = 0.0; // A temporary error
  Int_t tN = 0; // A temporary entries
  Double_t avg[3] = {0.0,0.0,0.0};
  Double_t avgE[3] = {0.0,0.0,0.0};
  Double_t avgD[3] = {0.0,0.0,0.0};
  Double_t avgDE[3] = {0.0,0.0,0.0};
  for(Int_t state  = 0; state < 2; state++ ) {
    for(Int_t cycle = 0; cycle < cycles(1).NumberOfCycles(state); cycle++ ) {
      n[state].push_back(Double_t(cycle)-(state?0.:0.5) );
      nE[state].push_back(0.0);
      TCut cut;
      if(state) { // On
        cut = cycles(1).GetCutOn(cycle);
      } else { // Off
        cut = cycles(1).GetCutOff(cycle);
      }
      laserCuts[state] = laserCuts[state]||cut;
      for(Int_t bcm = 0; bcm < 3; bcm++ ) {
        // Unique names for our histogram
        TString histName = Form("h1BCM%d_%d_%d",bcms[bcm],cycle,state);
        TString histDName = Form("h1DBCM%d_%d_%d",bcms[bcm],cycle,state);

        // Draw the currents
        chain(1)->Draw(Form("%ssca_bcm%d>>%s",prefix(1).Data(),bcms[bcm],
              histName.Data()),cut);
        GetH1ValErr(histName.Data(),&tN,&tV,&tE);
        c[1][bcm][state].push_back(tV);
        cE[1][bcm][state].push_back(tE);
        avg[bcm]+=tV/(tE*tE);
        avgE[bcm]+=1./(tE*tE);
        DBStoreBeam(1,Form("bcm%d",bcms[bcm]),
            "Current",cycle,state,tN,tV,tE);

        // Draw the differences
        chain(1)->Draw(Form("diff_sca_bcm%d>>%s",bcms[bcm],
              histDName.Data()),cut);
        GetH1ValErr(histDName.Data(),&tN,&tV,&tE);
        dC[1][bcm][state].push_back(tV);
        dCE[1][bcm][state].push_back(tE);
        avgD[bcm]+=tV/(tE*tE);
        avgDE[bcm]+=1./(tE*tE);
        DBStoreBeam(1,Form("bcm%d",bcms[bcm]),
            "DiffCurrent",cycle,state,tN,tV,tE);

        // Be nice and ask root to delete these old histograms
        gDirectory->Delete(histName.Data());
        gDirectory->Delete(histDName.Data());
      }
    }
  }

  for(Int_t bcm = 0; bcm <3; bcm++ ) {
    avg[bcm] = avg[bcm]/avgE[bcm];
    avgD[bcm] = avgD[bcm]/avgDE[bcm];
    avgE[bcm]=TMath::Sqrt(1.0/avgE[bcm]);
    avgDE[bcm]=TMath::Sqrt(1.0/avgDE[bcm]);
  }


  TH2F *hBCMS[2][3];
  TH2F *hBCMSD[3];
  TGraphErrors *graphBCMSD[2][3];
  // While we are here plot the BCM vs time plots
  for(Int_t state = 0; state < 2; state++ ) {
    for(Int_t bcm = 0; bcm < 3; bcm++ ) {
      TString ops = "";
      if(state !=0 ) {
        ops = "SAME";
      }
      fCanvasBCM->cd(bcm+1);
      gPad->cd(1);
      TString histName = Form("h2BCM%d_%d",bcms[bcm],state);
      hBCMS[state][bcm] = new TH2F(histName.Data(),
          Form("BCM%d Across Laser Patterns;Laser Pattern Number; BCM%d (#muA)",
            bcms[bcm],bcms[bcm]),
          chain(1)->GetEntries()*.25,-1.0,cycles(1).NumberOfCycles(),
          400,0.,200.);
      H2SetStyle(hBCMS[state][bcm],fColors[2-state]);
      chain(1)->Draw(Form("%ssca_bcm%d:%s>>%s",prefix(1).Data(),bcms[bcm],
            scaling("pat_counter",-0.5,cycles(1).NumberOfCycles(1)-.5,low,high),
            histName.Data()),laserCuts[state],ops);

      // Well, we are here, might as well be done with the differences too!
      fCanvasBCM->cd(bcm+1);
      gPad->cd(2);
      if(state==0) {
        hBCMSD[bcm] = new TH2F(Form("h2DBCM%d",bcms[bcm]),
            ";;Differences (#muA)",100,-1.0,cycles(1).NumberOfCycles(),
            100,avgD[bcm]-30*avgDE[bcm],avgD[bcm]+30*avgDE[bcm]);
        H2SetStyle(hBCMSD[bcm],0);
        hBCMSD[bcm]->Draw();
      }
      graphBCMSD[state][bcm] = new TGraphErrors(cycles(1).NumberOfCycles(state),
          n[state].data(),dC[1][bcm][state].data(),
          nE[state].data(),dCE[1][bcm][state].data());
      graphBCMSD[state][bcm]->SetMarkerStyle(fStyles[1]);
      graphBCMSD[state][bcm]->SetMarkerSize(3.5);
      graphBCMSD[state][bcm]->SetMarkerColor(fColors[2-state]);
      graphBCMSD[state][bcm]->Draw("PSAME");
    }
  }

  SaveToWeb(fCanvasBCM,"bcms");

  // Cleanup before leaving
  for(Int_t type = 0; type < 2; type++ ) {
    for(Int_t bcm = 0; bcm < 3; bcm++ ) {
      for(Int_t state = 0; state < 2; state++ ) {
        c[type][bcm][state].clear();
        cE[type][bcm][state].clear();

        dC[type][bcm][state].clear();
        dCE[type][bcm][state].clear();

        if(hBCMS[state][bcm]) {
          //hBCMS[state][bcm]->Reset();
          delete hBCMS[state][bcm];
          hBCMS[state][bcm] = 0;
        }
        if(graphBCMSD[state][bcm]) {
          delete graphBCMSD[state][bcm];
          graphBCMSD[state][bcm] = 0;
        }


      }
      if(hBCMSD[bcm]) {
        //hBCMSD[state]->Reset();
        delete hBCMSD[bcm];
        hBCMSD[bcm] = 0;
      }
    }
  }
  for(Int_t state = 0 ; state < 2; state++ ) {
    n[state].clear();
    nE[state].clear();
  }

}

void BeamLine::ProcessBPMs()
{
  TString bpms[3] = {"3p02a","3p02b","3p03a"};
  TString axisName[2] = {"X","Y"};

  // Average values
  std::vector<Double_t> p[2][3][2][2]; // 2 trees, 3 bpms, 2 axis and states,
  std::vector<Double_t> pE[2][3][2][2];
  // Differences
  std::vector<Double_t> dP[2][3][2][2];
  std::vector<Double_t> dPE[2][3][2][2];
  // Cycle numbers
  std::vector<Double_t> n[2];
  std::vector<Double_t> nE[2];

  TString states[2] = {"On","Off"};
  TCut laserCuts[2];

  TH2F *hBPMS[2][3][2];
  TH2F *hBPMSD[3][2];
  TGraphErrors *graphBPMSD[2][3][2];

  // Get some limits for the graphs I'll make later
  Double_t low = 0.0;
  Double_t high = 1.0;
  if(cycles(1).NumberOfCycles(0)>0) {
    low = cycles(1).start[0];
    high = cycles(1).end[cycles(1).NumberOfCycles(0)-1];
  }

  // Alright, we are now ready to process these monitors
  fCanvasTemp->cd(0);
  Double_t tV = 0.0; // A temporary value
  Double_t tE = 0.0; // A temporary error
  Int_t tN = 0.0; // A temporary entries
  for(Int_t axis = 0; axis < 2; axis++ ) {
    Double_t avg[3] = {0.0,0.0,0.0};
    Double_t avgE[3] = {0.0,0.0,0.0};
    Double_t avgD[3] = {0.0,0.0,0.0};
    Double_t avgDE[3] = {0.0,0.0,0.0};
    for(Int_t state  = 0; state < 2; state++ ) {
      for(Int_t cycle = 0; cycle < cycles(1).NumberOfCycles(state); cycle++ ) {
        n[state].push_back(Double_t(cycle)-(state?0.0:0.5) );
        nE[state].push_back(0.0);
        TCut cut;
        if(state) { // On
          cut = cycles(1).GetCutOn(cycle);
        } else { // Off
          cut = cycles(1).GetCutOff(cycle);
        }
        if(axis == 0 ) // Don't need to repeat for every axis!
          laserCuts[state] = laserCuts[state]||cut;

        for(Int_t bpm = 0; bpm < 3; bpm++ ) {
          // Unique names for our histogram
          TString histName = Form("h1BPM%s%s_%d_%d",bpms[bpm].Data(),
              axisName[axis].Data(),cycle,state);
          TString histDName = Form("h1DBPM%s%s_%d_%d",bpms[bpm].Data(),
              axisName[axis].Data(),cycle,state);

          // Draw the positions
          chain(1)->Draw(Form("%ssca_bpm_%s%s>>%s",prefix(1).Data(),
                bpms[bpm].Data(),axisName[axis].Data(),
                histName.Data()),cut);
          GetH1ValErr(histName.Data(),&tN,&tV,&tE);
          p[1][bpm][axis][state].push_back(tV);
          pE[1][bpm][axis][state].push_back(tE);
          avg[bpm]+=tV/(tE*tE);
          avgE[bpm]+=1./(tE*tE);
          DBStoreBeam(1,Form("%s%s",bpms[bpm].Data(),axisName[axis].Data()),
              "Distance",cycle,state,tN,tV,tE);

          // Draw the differences
          chain(1)->Draw(Form("diff_sca_bpm_%s%s>>%s",bpms[bpm].Data(),
                axisName[axis].Data(),histDName.Data()),cut);
          GetH1ValErr(histDName.Data(),&tN,&tV,&tE);
          dP[1][bpm][axis][state].push_back(tV);
          dPE[1][bpm][axis][state].push_back(tE);
          avgD[bpm]+=tV/(tE*tE);
          avgDE[bpm]+=1/(tE*tE);
          DBStoreBeam(1,Form("%s%s",bpms[bpm].Data(),axisName[axis].Data()),
            "DiffDistance",cycle,state,tN,tV,tE);

          // Cleanup these temporary histograms
          gDirectory->Delete(histName.Data());
          gDirectory->Delete(histDName.Data());
        }
      }
    }
    for(Int_t bpm = 0; bpm <3; bpm++ ) {
      avg[bpm] = avg[bpm]/avgE[bpm];
      avgD[bpm] = avgD[bpm]/avgDE[bpm];
      avgE[bpm]=TMath::Sqrt(1.0/avgE[bpm]);
      avgDE[bpm]=TMath::Sqrt(1.0/avgDE[bpm]);
    }

    // While we are here plot the BPM vs time plots
    for(Int_t state = 0; state < 2; state++ ) {
      for(Int_t bpm = 0; bpm < 3; bpm++ ) {
        TString ops = "";
        if(state !=0 ) {
          ops = "SAME";
        }
        fCanvasBPM[axis]->cd(bpm+1);
        gPad->cd(1);
        TString histName = Form("h2BPM%s%s_%d",bpms[bpm].Data(),
            axisName[axis].Data(),state);
        hBPMS[state][bpm][axis] = new TH2F(histName.Data(),
            Form("BPM %s%s Across Laser Patterns;Laser Pattern Number;"
              "BPM%s%s (mm)",bpms[bpm].Data(),
              axisName[axis].Data(),bpms[bpm].Data(),axisName[axis].Data()),
            chain(1)->GetEntries()*.25,-1.0,cycles(1).NumberOfCycles(1),
            400,avg[bpm]-1.5,avg[bpm]+1.5);
        H2SetStyle(hBPMS[state][bpm][axis],fColors[2-state]);
        chain(1)->Draw(Form("%ssca_bpm_%s%s:%s>>%s",
              prefix(1).Data(),bpms[bpm].Data(),axisName[axis].Data(),
              scaling("pat_counter",-0.5,
                cycles(1).NumberOfCycles(1)-.5,low,high),
              histName.Data()),laserCuts[state],ops);

        // Well, we are here, might as well be done with the differences too!
        fCanvasBPM[axis]->cd(bpm+1);
        gPad->cd(2);
        if(state==0) {
          hBPMSD[bpm][axis] = new TH2F(Form("h2DBPM%s%s",bpms[bpm].Data(),
                axisName[axis].Data()),
              ";;Differences (mm)",100,-1.0,cycles(1).NumberOfCycles(1),
              100,avgD[bpm]-30*avgDE[bpm],avgD[bpm]+30*avgDE[bpm]);
          H2SetStyle(hBPMSD[bpm][axis],0);
          hBPMSD[bpm][axis]->Draw();
        }
        graphBPMSD[state][bpm][axis] = new TGraphErrors(
            cycles(1).NumberOfCycles(state),
            n[state].data(),dP[1][bpm][axis][state].data(),
            nE[state].data(),dPE[1][bpm][axis][state].data());
        graphBPMSD[state][bpm][axis]->SetMarkerStyle(fStyles[1]);
        graphBPMSD[state][bpm][axis]->SetMarkerSize(3.5);
        graphBPMSD[state][bpm][axis]->SetMarkerColor(fColors[2-state]);
        graphBPMSD[state][bpm][axis]->Draw("PSAME");
      }
    }

    SaveToWeb(fCanvasBPM[axis],Form("bpms%s",axisName[axis].Data()));

    for(Int_t type = 0; type < 2; type++ ) {
      for(Int_t bpm = 0; bpm < 3; bpm++ ) {
        for(Int_t state = 0; state < 2; state++) {
          p[type][bpm][axis][state].clear();
          pE[type][bpm][axis][state].clear();
          dP[type][bpm][axis][state].clear();
          dPE[type][bpm][axis][state].clear();

        }
        if(hBPMS[type][bpm][axis]) {
          delete hBPMS[type][bpm][axis];
          hBPMS[type][bpm][axis] = 0;
        }
        if(graphBPMSD[type][bpm][axis]) {
          delete graphBPMSD[type][bpm][axis];
          graphBPMSD[type][bpm][axis] = 0;
        }
        if(hBPMSD[bpm][axis]) {
          delete hBPMSD[bpm][axis];
          hBPMSD[bpm][axis] = 0;
        }
      }
    }

    for(Int_t state = 0; state < 2; state++ ) {
      n[state].clear();
      nE[state].clear();
    }
  }

}


void BeamLine::GetH1ValErr(const char* hist, Int_t *n,
    Double_t *val, Double_t *err)
{
  TH1F *h = (TH1F*)gDirectory->Get(hist);
  if(h) {
    *val = h->GetMean();
    *n = h->GetEntries();
    if(*n<=0) { // Make error huge!
      *err = 1.0e6;
    } else {
      *err = h->GetRMS()/TMath::Sqrt(h->GetEntries());
    }
  }
}

void BeamLine::H2SetStyle(TH2F *hist, Int_t color)
{
  hist->SetTitleOffset(0.4,"Y");
  hist->SetTitleOffset(0.5,"X");
  hist->SetTitleSize(0.07,"Y");
  hist->SetTitleSize(0.07,"X");
  hist->SetTitleSize(0.1,"");
  hist->SetMarkerColor(color);
  hist->SetStats(0);

}


BeamLine::~BeamLine()
{
  if (fCanvasTemp)
    delete fCanvasTemp;
  if (fCanvasBCM)
    delete fCanvasBCM;
  if(fCanvasBPM[0])
    delete fCanvasBPM[0];
  if(fCanvasBPM[1])
    delete fCanvasBPM[1];
}

// Define functions with c symbols (create/destroy instances)
extern "C" BeamLine* create()
{
  return new BeamLine();
}

extern "C" void destroy(BeamLine* beamline)
{
  delete beamline;
}
