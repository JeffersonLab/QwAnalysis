//
// Usage: qwsubcycle --run <run number> --file-stem mps_only --set-stem < database flag for modulation set ex:on_beammod_4> --pattern-number <pattern number 11-15>
//
#include "../include/headers.h"
#include "../include/QwDataFile.hh"
#include "../include/QwDataContainer.hh"
#include "../include/QwModulation.hh"
#include "TLeaf.h"
#include "TProfile.h"
#include "TLegend.h"
#include <vector>
#include <fstream>
#include <iostream>

const Int_t fMaxModEvents = 3932; 
const Int_t fSubCycle = 983;

Int_t fNMonitor;

std::vector < std::vector <TProfile *> > fMonitorSubCycle;

void FillSubCycleProfile(std::vector < std::vector <Double_t> > &, std::vector <Double_t> &);

Int_t main(Int_t argc, Char_t *argv[])
{

  enum sens_t {xsens, xpsens, esens, ysens, ypsens};

  TString pattern_name[5] = {"X", "Y","E", "XP", "YP"};

  TLegend *legend;

  TChain *mps_tree = new TChain("mps_slug");

  QwModulation *modulation = new QwModulation(mps_tree);
  QwDataContainer *data = new QwDataContainer();

  data->ConnectDB();

  modulation->GetOptions(argv);
  data->GetOptions(argv);

  if( !(modulation->fRunNumberSet) ){
    modulation->PrintError("Error Loading:  no run number specified");
    exit(1);
  }
  else
    data->fRunRange = true;

  data->SetRunRange(modulation->run_number, modulation->run_number);

  TString directory = gSystem->Getenv("QW_ROOTFILES");

  if(!gSystem->OpenDirectory(directory)){
    modulation->PrintError("Cannot open rootfiles directory.\n");
    exit(1);
  }

  TString output = gSystem->Getenv("QWSCRATCH");

  if(!gSystem->OpenDirectory(output)){
    modulation->PrintError("Cannot open scratch directory.\n");
    exit(1);
  }

  TString filename = Form("%s_%d*root", modulation->fFileStem.Data(), modulation->run_number);
  std::cout << "Filename: " <<  Form("%s/%s", directory.Data(), filename.Data() ) << std::endl;
  if( !(mps_tree->Add(Form("%s/%s", directory.Data(), filename.Data() ))) ){
    std::cerr << "Failure loading file." << std::endl;
    exit(1);
  }

  std::ofstream sine_data;
  std::ofstream cosine_data;

  std::cout << "Number of entries: " << mps_tree->GetEntries() << std::endl;

  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;

  Double_t ErrorFlag;
  Double_t ramp_hw_sum;
  Double_t bm_pattern_number;

  TBranch *b_ErrorFlag;
  TBranch *b_ramp;
  TBranch *b_bm_pattern_number;

  modulation->ReadConfig();

  fNMonitor = modulation->fNMonitor;

  modulation->fNumberEvents = mps_tree->GetEntries();

  mps_tree->SetBranchStatus("*", 0);     
  mps_tree->SetBranchStatus("ErrorFlag", 1);   
  mps_tree->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
  mps_tree->SetBranchStatus("ramp", 1);   
  mps_tree->SetBranchStatus("ramp_block0", 1);   
  mps_tree->SetBranchStatus("ramp_block1", 1);   
  mps_tree->SetBranchStatus("ramp_block2", 1);   
  mps_tree->SetBranchStatus("ramp_block3", 1);   
  mps_tree->SetBranchStatus("bm_pattern_number", 1);   
  mps_tree->SetBranchAddress("bm_pattern_number", &bm_pattern_number, &b_bm_pattern_number);   
  mps_tree->SetBranchAddress("ramp", &ramp_hw_sum, &b_ramp);


  for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
    mps_tree->SetBranchStatus(Form("%s", modulation->MonitorList[mon].Data()), 1);   
  }

  std::cout << "Number of entries: " << modulation->fNumberEvents << std::endl;
  if(modulation->fNumberEvents <= 0){
    modulation->PrintError("There are no entries in this rootfile."); 
    exit(1);
  }

  Int_t events = 0;

  std::vector < std::vector <Double_t> > fDataVectorMonitor;
  std::vector <Double_t> fDataVectorRamp;
  
  fMonitorSubCycle.resize(modulation->fNMonitor);

  for(Int_t mon = 0; mon < (modulation->fNMonitor); mon++){
    for(Int_t i = 0; i < 4; i++){
      fMonitorSubCycle[mon].push_back(new TProfile(Form("profile%d%d", mon, i), Form("profile%d%d", mon, i), 100, 0, 360));
    }
  }
  
  std::cout << "Cutting on pattern number: " 
	    << modulation->fPatternNumber << std::endl;

  std::cout << "Number of events: " << modulation->fNumberEvents << std::endl;

  Int_t counter = 0;
  
  for(Int_t i = 0; i < (modulation->fNumberEvents); i++){
    mps_tree->GetEntry(i);
    
    if(ErrorFlag == 0x4018080 && bm_pattern_number == (modulation->fPatternNumber)){
      fDataVectorMonitor.resize(modulation->fNMonitor);
      do {
	if((modulation->CheckRampLinearity("")) == 0 && ErrorFlag == 0x4018080 && ramp_hw_sum > 10 && bm_pattern_number == (modulation->fPatternNumber)){
	  fDataVectorRamp.push_back(mps_tree->GetLeaf("ramp")->GetValue());
	  for(Int_t mon = 0; mon < (modulation->fNMonitor); mon++){
	    fDataVectorMonitor[mon].push_back(mps_tree->GetLeaf(modulation->MonitorList[mon].Data())->GetValue());
	  }
	    counter++;	  
	}
	i++;
	mps_tree->GetEntry(i);
      }while(ErrorFlag == 0x4018080 && bm_pattern_number == (modulation->fPatternNumber) && i < (modulation->fNumberEvents));
      std::cout << "Modulation cycle contains: " << counter << " events." << std::endl; 
      if(counter >= 0.9*fMaxModEvents){
	std::cout << "Filling subcycle profiles." << std::endl;
	FillSubCycleProfile(fDataVectorMonitor, fDataVectorRamp);
      }
      counter = 0;
      fDataVectorMonitor.clear();
      fDataVectorRamp.clear();
    }
  }

  //
  // Now we need to make the composite plots
  // 

 
  TString corrected;

  TF1 *composite = new TF1("composite", "[0] + [1]*sin(0.0174533*x + [2]) + [3]*cos(0.0174533*x + [4])", 10., 350.);

  for(Int_t mon = 0; mon < (modulation->fNMonitor); mon++){
    corrected = modulation->MonitorList[mon].Data();

    std::cout << "Plotting: " << corrected << std::endl;
    
    sine_data.open(Form("%s/sine_fit_data_%s_%s.dat", output.Data(), modulation->MonitorList[mon].Data(), 
			pattern_name[(modulation->fPatternNumber) - 11].Data()), std::ios::out | std::ios::app);
    cosine_data.open(Form("%s/cosine_fit_data_%s_%s.dat", output.Data(), modulation->MonitorList[mon].Data(), 
			  pattern_name[(modulation->fPatternNumber) - 11].Data()), std::ios::out | std::ios::app);
    
    TCanvas *canvas = new TCanvas("canvas", "canvas", 1000, 700);
    
    gStyle->SetOptFit(0);
    gStyle->SetOptStat(0);
    
    gPad->SetGridx();
    gPad->SetGridy();

    for(Int_t cycle = 0; cycle < 4; cycle++){
      
      
      //
      // Because I didn't want to rename all the calls below
      //
      
      TProfile *profile = fMonitorSubCycle[mon][cycle];

      std::cout << "Events in profile: " << profile->GetEntries() << " " << fMonitorSubCycle[mon][cycle]->GetEntries()  << std::endl;
      
      canvas->cd();

      if(!(profile->GetEntries())){
	modulation->PrintError(Form("No entries in the profile plot. Monitor %d  Cycle: %d", mon, cycle));
	exit(1);
      }
      profile->Draw();

      composite->SetParameter(0, profile->GetMean(2));
      composite->SetParameter(2, TMath::PiOver2());
      composite->SetParameter(4, TMath::PiOver2());
      
      if(!(modulation->fPhaseConfig)){
	TString analysis = gSystem->Getenv("QWANALYSIS");
	modulation->ReadPhaseConfig(Form("%s/Extensions/Regression/QwBeamModulation/config/phase_set3.config", analysis.Data()));
      }
      
      composite->FixParameter(2, modulation->phase[(modulation->fPatternNumber) - 11]);
      composite->FixParameter(4, modulation->phase[(modulation->fPatternNumber) - 11]);
      
      composite->SetLineColor(6);
      
      profile->GetXaxis()->SetLabelSize(0.02);
      profile->GetYaxis()->SetLabelSize(0.02);
      profile->GetXaxis()->SetTitleSize(0.02);
      profile->GetYaxis()->SetTitleOffset(2.0);
      profile->GetYaxis()->SetTitleSize(0.02);
      
      profile->SetTitle(Form("%s Corrected Monitor Response: %s Modulation", 
			     modulation->MonitorList.front().Data(), pattern_name[(modulation->fPatternNumber) - 11].Data()));
      profile->GetXaxis()->SetTitle("degrees");
      profile->GetYaxis()->SetTitle("Amplitude(mm or um)");
      
      profile->Fit("composite", "R B+");
      
      TF1 *sine     = new TF1("sine", "[0] + [1]*sin(0.0174533*x + [2])", 10., 350.);
      TF1 *cosine   = new TF1("cosine", "[0] + [1]*cos(0.0174533*x + [2])", 10., 350.);
      
      
      sine->SetLineColor(2);
      cosine->SetLineColor(4);
      
      sine->FixParameter(0, composite->GetParameter(0));
      sine->FixParameter(1, composite->GetParameter(1));
      sine->FixParameter(2, composite->GetParameter(2));
      
      cosine->FixParameter(0, composite->GetParameter(0));
      cosine->FixParameter(1, composite->GetParameter(3));
      cosine->FixParameter(2, composite->GetParameter(4));  
      
      profile->Fit("sine", "R B+ Q");
      profile->Fit("cosine", "R B+ Q");
      
      Double_t sine_amplitude   = composite->GetParameter(1);
      Double_t cosine_amplitude = composite->GetParameter(3);
      
      Double_t sine_error = TMath::Power((composite->GetParError(1))/(composite->GetParameter(1)), 2);
      sine_error += TMath::Power((composite->GetParError(0))/(composite->GetParameter(0)), 2); 
      sine_error = TMath::Sqrt(sine_error)*TMath::Abs(((composite->GetParameter(1))/(composite->GetParameter(0))));
      
      Double_t cosine_error = TMath::Power((composite->GetParError(3))/(composite->GetParameter(3)), 2);
      cosine_error += TMath::Power((composite->GetParError(0))/(composite->GetParameter(0)), 2); 
      cosine_error = TMath::Sqrt(cosine_error)*TMath::Abs(((composite->GetParameter(3))/(composite->GetParameter(0))));
      
      legend = new TLegend(0.1, 0.8, 0.45, 0.92);
      legend->AddEntry(sine, Form("Sine: %4.4f +- %4.4f ppm", sine_amplitude, sine_error), "l");
      legend->AddEntry(cosine,  Form("Cosine: %4.4f +- %4.4f ppm", cosine_amplitude, cosine_error), "l");
      legend->Draw();
      
      canvas->Update();
      
      profile->GetYaxis()->SetRangeUser( (composite->GetParameter(0)) - 2*(composite->GetParameter(1)),
					 (composite->GetParameter(0)) - 2*(composite->GetParameter(1)));
      
      sine_data << Form("%.d.%d", modulation->run_number, cycle) << " " 
		<< sine_amplitude << " "
		<< sine_error
		<< std::endl;
      cosine_data << Form("%.d.%d", modulation->run_number, cycle) << " " 
		  << cosine_amplitude << " "
		  << cosine_error
		  << std::endl;
      canvas->SaveAs(Form("monitor_response_%s_%s_cycle_%d.C", modulation->MonitorList[mon].Data(), 
			  pattern_name[(modulation->fPatternNumber) - 11].Data(), cycle));
      canvas->Clear();
    }
    
    sine_data.close();
    cosine_data.close();
  }
  
  std::cout << "Finished." << std::endl;
  
  return 0;
}


void FillSubCycleProfile(std::vector <std::vector <Double_t> > &response, std::vector <Double_t> &ramp){

  Int_t fNumEvents = ramp.size();
  Int_t fCycleIndex = 0;

  for(Int_t i = fCycleIndex; i < (fSubCycle + fCycleIndex); i++){
    for(Int_t mon = 0; mon < (fNMonitor); mon++){
      fMonitorSubCycle[mon][0]->Fill(ramp[i], response[mon][i]);
    }
  }
  fCycleIndex += fSubCycle;

  for(Int_t i = fCycleIndex; i < (fSubCycle + fCycleIndex); i++){
    for(Int_t mon = 0; mon < (fNMonitor); mon++){
      fMonitorSubCycle[mon][1]->Fill(ramp[i], response[mon][i]);
    }
  }
  fCycleIndex += fSubCycle;

  for(Int_t i = fCycleIndex; i < (fSubCycle + fCycleIndex); i++){
    for(Int_t mon = 0; mon < (fNMonitor); mon++){
      fMonitorSubCycle[mon][2]->Fill(ramp[i], response[mon][i]);
    }
  }
  fCycleIndex += fSubCycle;

  for(Int_t i = fCycleIndex; i < fNumEvents; i++){
    for(Int_t mon = 0; mon < (fNMonitor); mon++){
      fMonitorSubCycle[mon][3]->Fill(ramp[i], response[mon][i]);
    }
  }

  return;
}
