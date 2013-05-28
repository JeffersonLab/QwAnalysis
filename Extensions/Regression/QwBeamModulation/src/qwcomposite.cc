//
// Usage: qwcomposite --run <run number> --file-stem mps_only --set-stem < database flag for modulation set ex:on_beammod_4> --pattern-number <pattern number 11-15>
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

Int_t main(Int_t argc, Char_t *argv[])
{

  //
  // For some reason TApplication fucks up the input argv's
  // don't need it anyway outside of testing though...
  //
  //   TApplication theApp("App", &argc, argv);

  enum sens_t {xsens, xpsens, esens, ysens, ypsens};

  TString pattern_name[5] = {"X", "Y","E", "XP", "YP"};

  TLegend *legend;

  TChain *mps_tree = new TChain("mps_slug");

  QwModulation *modulation = new QwModulation(mps_tree);
  QwDataContainer *data = new QwDataContainer();
  data->ConnectDB();

//   modulation->output = gSystem->Getenv("BMOD_OUT");
//   if(!gSystem->OpenDirectory(modulation->output)){
//     modulation->PrintError("Cannot open output directory.\n");
//     modulation->PrintError("Directory needs to be set as env variable and contain:\n\t slopes/ \n\t regression/ \n\t diagnostics/ \n\t rootfiles/"); 
//     exit(1);
//   }

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
  data->GetDBSensitivities(modulation->DetectorList.front().Data(), "", "sens_all");

  for(Int_t i = 0; i < 5; i++){
    std::cout << "Sensitvities: " << i 
	      << " : " << data->fSensitivity[i][0]
	      << std::endl;
  }

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


  for(Int_t det = 0; det < modulation->fNDetector; det++){
    mps_tree->SetBranchStatus(Form("%s", modulation->DetectorList[det].Data()), 1);   
  }
  for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
    mps_tree->SetBranchStatus(Form("%s", modulation->MonitorList[mon].Data()), 1);   
  }

  std::cout << "Number of entries: " << modulation->fNumberEvents << std::endl;
  if(modulation->fNumberEvents <= 0){
    modulation->PrintError("There are no entries in this rootfile."); 
    exit(1);
  }

  Int_t events = 0;

  std::vector <Double_t> fMonitorMean(modulation->fNMonitor);
  std::vector <Double_t> fDetectorMean(modulation->fNDetector);

  std::cout << "Cutting on pattern number: " 
	    << modulation->fPatternNumber << std::endl;

  for(Int_t i = 0; i < (modulation->fNumberEvents); i++){
    mps_tree->GetEntry(i);

    if((modulation->CheckRampLinearity("")) != 0 && ErrorFlag == 0x4018080 && ramp_hw_sum > 10 && bm_pattern_number == (modulation->fPatternNumber)){
      for(Int_t mon = 0; mon < (modulation->fNMonitor); mon++){
	if(i == 0) fMonitorMean[mon] = 0;      
	fMonitorMean[mon] += mps_tree->GetLeaf(modulation->MonitorList[mon].Data())->GetValue(); 
      }
      for(Int_t det = 0; det < (modulation->fNDetector); det++){
	if(i == 0) fDetectorMean[det] = 0;      
	fDetectorMean[det] += mps_tree->GetLeaf(modulation->DetectorList[det].Data())->GetValue(); 
      }
      events++;      
    }

  }
  if(events == 0){
    modulation->PrintError("No good events were found.");
    exit(1);
  }
  for(Int_t mon = 0; mon < (modulation->fNMonitor); mon++){
    fMonitorMean[mon] /= events;
    std::cout << "<Monitor>: "
 	      << fMonitorMean[mon]
 	      << std::endl;
  }
  for(Int_t det = 0; det < (modulation->fNDetector); det++){
    fDetectorMean[det] /= events;
    std::cout << "<Detector>: "
 	      << fDetectorMean.front()
 	      << std::endl;
  }

  //
  // Now that we have the means we need to make the composite plots
  // 
  TString corrected;

  TProfile *profile;

  TF1 *composite = new TF1("composite", "[0] + [1]*sin(0.0174533*x + [2]) + [3]*cos(0.0174533*x + [4])", 10., 350.);


  //
  // Energy sensitivities come out of database function as ppm/ppm need to convert back.
  //

  Double_t key[5] = {1.e6, 1., 4100., 1.e6, 1.};

  corrected = modulation->DetectorList.front().Data();
//   for(Int_t mon = 0; mon < (modulation->fNMonitor); mon++){
//     corrected = Form("%s-(%s-%e)*(%e)*(%e)/%e", corrected.Data(), modulation->MonitorList[mon].Data(), 
//      		     fMonitorMean[mon], data->fSensitivity[mon][0], fDetectorMean.front(), key[mon]);
//   }
  
  std::cout << "Plotting: " << corrected << std::endl;

  sine_data.open(Form("%s/sine_fit_data_%s_%s.dat", output.Data(), modulation->DetectorList.front().Data(), 
		      pattern_name[(modulation->fPatternNumber) - 11].Data()), std::ios::out | std::ios::app);
  cosine_data.open(Form("%s/cosine_fit_data_%s_%s.dat", output.Data(), modulation->DetectorList.front().Data(), 
			pattern_name[(modulation->fPatternNumber) - 11].Data()), std::ios::out | std::ios::app);
  
  TCanvas *canvas = new TCanvas("canvas", "canvas", 1000, 700);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);

  gPad->SetGridx();
  gPad->SetGridy();

  canvas->cd();
  profile = new TProfile("profile", "profile", 100, 10, 340);
  mps_tree->Draw(Form("(%s):ramp>>profile", corrected.Data()), 
		 Form("ErrorFlag == 0x4018080 && ramp > 10 && bm_pattern_number == %d", modulation->fPatternNumber));
  (TProfile *)gDirectory->Get("profile");
  if(!(profile->GetEntries())){
    modulation->PrintError("No entries in the profile plot.");
    exit(1);
  }

  composite->SetParameter(0, profile->GetMean(2));
  composite->SetParameter(2, TMath::PiOver2());
  composite->SetParameter(4, TMath::PiOver2());

  if(!(modulation->fPhaseConfig)){
    TString analysis = gSystem->Getenv("QWANALYSIS");
    modulation->ReadPhaseConfig(Form("%s/Extensions/Regression/QwBeamModulation/config/phase_set4.config", analysis.Data()));
  }
     
  composite->FixParameter(2, modulation->phase[(modulation->fPatternNumber) - 11]);
  composite->FixParameter(4, modulation->phase[(modulation->fPatternNumber) - 11]);

  composite->SetLineColor(6);

  profile->GetXaxis()->SetLabelSize(0.02);
  profile->GetYaxis()->SetLabelSize(0.02);
  profile->GetXaxis()->SetTitleSize(0.02);
  profile->GetYaxis()->SetTitleOffset(2.0);
  profile->GetYaxis()->SetTitleSize(0.02);

  profile->SetTitle(Form("%s Corrected Yield Response: %s Modulation", modulation->DetectorList.front().Data(), pattern_name[(modulation->fPatternNumber) - 11].Data()));
  profile->GetXaxis()->SetTitle("degrees");
  profile->GetYaxis()->SetTitle("Amplitude(mV/uA)");

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


  Double_t sine_amplitude = (composite->GetParameter(1))/(fDetectorMean.front());
  Double_t cosine_amplitude = (composite->GetParameter(3))/(fDetectorMean.front());

  Double_t sine_error = TMath::Power((composite->GetParError(1))/(composite->GetParameter(1)), 2);
  sine_error += TMath::Power((composite->GetParError(0))/(composite->GetParameter(0)), 2); 
  sine_error = TMath::Sqrt(sine_error)*TMath::Abs(((composite->GetParameter(1))/(composite->GetParameter(0))));

  Double_t cosine_error = TMath::Power((composite->GetParError(3))/(composite->GetParameter(3)), 2);
  cosine_error += TMath::Power((composite->GetParError(0))/(composite->GetParameter(0)), 2); 
  cosine_error = TMath::Sqrt(cosine_error)*TMath::Abs(((composite->GetParameter(3))/(composite->GetParameter(0))));

  legend = new TLegend(0.1, 0.8, 0.45, 0.92);
  legend->AddEntry(sine, Form("Sine: %3.3f +- %3.3f ppm", 1e6*sine_amplitude, 1e6*sine_error), "l");
  legend->AddEntry(cosine,  Form("Cosine: %3.3f +- %3.3f ppm", 1e6*cosine_amplitude, 1e6*cosine_error), "l");
  legend->Draw();

  canvas->Update();

  profile->GetYaxis()->SetRangeUser( (composite->GetParameter(0)) - 2*(composite->GetParameter(1)),
				     (composite->GetParameter(0)) - 2*(composite->GetParameter(1)));
  
  sine_data << modulation->run_number << " " 
	    << 1e6*sine_amplitude << " "
	    << 1e6*sine_error
	    << std::endl;
  cosine_data << modulation->run_number << " " 
	      << 1e6*cosine_amplitude << " "
	      << 1e6*cosine_error
	      << std::endl;

  sine_data.close();
  cosine_data.close();
  
  canvas->SaveAs(Form("%s/composite_%d_%s_%s.C", output.Data(), modulation->run_number, 
		      modulation->DetectorList.front().Data(), pattern_name[(modulation->fPatternNumber) - 11].Data()));
  
  //   theApp.Run();
  
  std::cout << "Finished." << std::endl;
  
  return 0;
}
