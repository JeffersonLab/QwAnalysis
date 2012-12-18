#include "headers.h"
#include "beammod.hh"

const Int_t fNMonitors = 24;
const Int_t fCycles = 5;

Double_t GetAmplitudeSign(Double_t, Double_t, Double_t, Double_t);

Int_t main(Int_t argc, Char_t *argv[])
{
  //  TApplication suuperApp("App", &argc, argv);

  TMultiGraph *multi_graph = new TMultiGraph();
  TMultiGraph *phase_graph = new TMultiGraph();

  TString filename;
  TString cut;
  TString file_stem = "QwPass5_";

  TString ramp_cut = "((ramp.block0 + ramp.block3) - (ramp.block1 + ramp.block2)) > -50 && ((ramp.block0 + ramp.block3) - (ramp.block1 + ramp.block2)) < 50";
  TString monitor[fNMonitors] = {"qwk_target","qwk_bpm3h09b","qwk_bpm3h09","qwk_bpm3h08","qwk_bpm3h07c","qwk_bpm3h07b",
				 "qwk_bpm3h07a","qwk_bpm3h04","qwk_bpm3h02","qwk_bpm3c21","qwk_bpm3c20","qwk_bpm3p03a",
				 "qwk_bpm3p02b","qwk_bpm3p02a","qwk_bpm3c19","qwk_bpm3c18","qwk_bpm3c17","qwk_bpm3c16",
				 "qwk_bpm3c14","qwk_bpm3c12","qwk_bpm3c11","qwk_bpm3c08","qwk_bpm3c07a","qwk_bpm3c07"};
  TString coord[2] = {"X","Y"};
  TString flag;
  TString scratch_dir;
  TString pattern_name[5] = {"X", "Y", "E", "XP", "YP"};
  TString pattern_name_single[5] = {"X1", "Y1", "E", "X2", "Y2"};

  Bool_t fVerbose = false;
  Bool_t fSingle = false;

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200,600);
  TCanvas *phase_c = new TCanvas("phase_c", "phase_c", 1200,600);

  Int_t run_number;
  Int_t counter = 0;
  Int_t argvn = 0;

  Double_t amplitude[fNMonitors];
  Double_t error[fNMonitors];
  Double_t phase[fNMonitors];
  Double_t phase_error[fNMonitors];
  Double_t bpm_z[fNMonitors];
  Double_t zeros[fNMonitors];
  Double_t mean = 0;
  Double_t upper = 350;
  Double_t lower = 10;

  TChain *chain = new TChain("Mps_Tree");

  TF1 *sine = new TF1("sine", "[0]+[1]*sin(0.01745*x +[2])", lower, upper);

  std::fstream monitor_position;

  run_number = atoi(argv[1]);

  gStyle->SetOptFit(0111);
  gStyle->SetOptStat(1111);

  do{
    flag = argv[argvn];

    if(flag.CompareTo("--bpms", TString::kExact) == 0){
      fVerbose = true;
      flag.Clear();
      std::cout << "Saving bpms option set to 'true'" <<std::endl;
    }
    if(flag.CompareTo("--single", TString::kExact) == 0){
      fSingle = true;
      flag.Clear();
      std::cout << "Single Coil option set to 'true'" <<std::endl;
    }
    if(flag.CompareTo("--help", TString::kExact) == 0){
      std::cout << "Usage:./optics_plot <run number> <options>\n" 
		<< "\tOptions:\n" 
		<< "\t\t--bpms :\tAllows you to save histograms of bpm response plots.\n\n"
		<< "\t\t--single :\tAllows you to process single coil modulation assuming pattern numbers: 0-5"
		<< std::endl;
      flag.Clear();
      exit(1);
    }
    argvn++;
  } while(argv[argvn] != NULL);
  
  if(fSingle){
    for(Int_t i = 0; i < 5; i++) pattern_name[i] = pattern_name_single[i];
  }
     
  filename = Form("%s%d.0*.trees.root", file_stem.Data(), run_number);

  if(argv[1] == NULL){
    std::cout << "No run number specified.  Exiting." << endl;
    exit(1);
  }
  LoadRootFile(filename, chain, run_number);

  scratch_dir = gSystem->Getenv("QWSCRATCH");
  std::cout << "Scratch directory set to be:\t" << scratch_dir << std::endl;

  if( (gSystem->OpenDirectory(scratch_dir)) == 0){
    std::cout << "$QWSCRATCH needs to be set." << std::endl;
    exit(1);
  }

  if( (gSystem->OpenDirectory( gSystem->Getenv("QW_ROOTFILES") )) == 0){
    std::cout << "$QW_ROOTFILES needs to be set." << std::endl;
    exit(1);
  }

  if(gSystem->OpenDirectory(Form("%s/plots", scratch_dir.Data())) == 0)
    gSystem->Exec(Form("mkdir %s/plots", scratch_dir.Data()));

  // Loop over cycles
  for(Int_t k = 0; k < 2; k ++){
//   for(Int_t k = 0; k < 1; k ++){
    for(Int_t j = 0; j < fCycles; j++){
//     for(Int_t j = 0; j < 1; j++){

      if(fSingle) 
	cut = Form("ErrorFlag == 0x4018080 && bm_pattern_number == %i && %s", j, ramp_cut.Data());
      else 
	cut = Form("ErrorFlag == 0x4018080 && bm_pattern_number == %i && %s", j + 11, ramp_cut.Data());

      std::cout << cut << std::endl;
      
      for(Int_t i = 0; i < fNMonitors; i++){
	canvas->cd();

	TH2F* histo = new TH2F("histo", "histo", 400, 0., 400., 1000, -2., 2.);
	
	chain->Draw(Form("%s%s:ramp>>histo", monitor[i].Data(), coord[k].Data()), cut, "prof");
	histo = (TH2F *)gDirectory->Get("histo");
	sine->SetParameters(histo->GetMean(), 0.150, TMath::Pi());
// 	sine->SetParLimits(2, 0., TMath::Pi()*0.5 );
	sine->SetParLimits(2, 0., TMath::Pi()*2 );

	mean = histo->GetMean(2);
 	histo->GetYaxis()->SetRangeUser(mean - 0.3, mean + 0.3);
	histo->Fit("sine","R B");    
 
	amplitude[i] = TMath::Abs(sine->GetParameter(1));
	phase[i] = (sine->GetParameter(2))*(180/TMath::Pi());

	// try some thing new //

	if(phase[i] >= 180){
	  phase[i] -= 180;
// 	  amplitude[i] = -amplitude[i];
	}

	amplitude[i] *= GetAmplitudeSign(sine->Derivative(lower), 
					 sine->Derivative2(lower), 
					 sine->Derivative3(lower), 
					 sine->GetParameter(lower));
	// end //

	error[i] = sine->GetParError(1);
	phase_error[i] = (sine->GetParError(2))*(180/TMath::Pi());
	printf("\n%s\t%-5.5e +- %-5.5e\n", monitor[i].Data(), phase[i], phase_error[i]);
	histo->Draw();
	canvas->Update();

	if(fVerbose == true){
	  std::cout << "Check to see if histogram directory exist." << std::endl;
	  if( (gSystem->OpenDirectory(Form("%s/plots/hist_%i", scratch_dir.Data(), run_number)))  == 0){
	    std::cout << " ##### Making histogram directory. #####" << std::endl;
	    gSystem->Exec(Form("mkdir %s/plots/hist_%i", scratch_dir.Data(), run_number));
	  }

	  std::cout << "Trying to write histogram....." << std::endl;
	  canvas->SaveAs(Form("%s/plots/hist_%i/hist_%s%s_%i.png", scratch_dir.Data(), run_number, monitor[i].Data(), pattern_name[j].Data(), j));
	}

	histo->Delete();
      }
      for(Int_t i = 0; i < fNMonitors; i++) printf("%s\t%-5.5e\t%-5.5e\t%-5.5e\t%-5.5e\n", monitor[i].Data(), amplitude[i], error[i], phase[i], phase_error[i]);
      
      
      monitor_position.open("config/z.txt", fstream::in);
      if( !monitor_position.good() ){
	std::cout << "Problem opening file." << std::endl;
	exit(1);
      }
      
      while(!monitor_position.eof()){
	if(counter >= fNMonitors) exit(1);
	
	monitor_position >> bpm_z[counter] >> zeros[counter];
	counter++;
      }
      counter = 0;
      canvas->cd();
      TGraphErrors *baseline = new TGraphErrors( fNMonitors, bpm_z, zeros, zeros, zeros);
      
      TGraphErrors *fit_optics = new TGraphErrors( fNMonitors, bpm_z, amplitude, zeros, error);
      
      gPad->SetGridx();
      gPad->SetGridy();
      
      baseline->SetMarkerStyle(6);
      baseline->SetMarkerColor(1);
      baseline->SetLineStyle(3);
      fit_optics->SetMarkerStyle(24);
      fit_optics->SetMarkerColor(2);
      fit_optics->SetLineColor(2);    
      fit_optics->SetLineStyle(1);    
      
      multi_graph->Add(baseline);
      multi_graph->Add(fit_optics);
      multi_graph->SetMaximum( 0.5 );
      multi_graph->SetMinimum(-0.5 );
      
      multi_graph->Draw("ap");
      multi_graph->GetXaxis()->SetTitle("Z beamline position(cm)");
      multi_graph->GetYaxis()->SetTitle(Form( "BMod Phase Offset %s (mm)  mod_type:%s", coord[k].Data(), pattern_name[j].Data()) );    
      canvas->Update();
      
      //
      // Phase Multi-graph
      //

      phase_c->cd();
      TGraphErrors *phase_optics = new TGraphErrors( fNMonitors, bpm_z, phase, zeros, phase_error);
      
      gPad->SetGridx();
      gPad->SetGridy();
      
      baseline->SetMarkerStyle(6);
      baseline->SetMarkerColor(1);
      baseline->SetLineStyle(3);
      phase_optics->SetMarkerStyle(24);
      phase_optics->SetMarkerColor(2);
      phase_optics->SetLineColor(2);    
      phase_optics->SetLineStyle(1);    
      
      phase_graph->Add(baseline);
      phase_graph->Add(phase_optics);
      phase_graph->SetMaximum( 160. );
      phase_graph->SetMinimum(-10. );
      
      phase_graph->Draw("ap");
      phase_graph->GetXaxis()->SetTitle("Z beamline position(cm)");
      phase_graph->GetYaxis()->SetTitle(Form( "BMod Position Offset %s (deg)  mod_type:%s", coord[k].Data(), pattern_name[j].Data()) );    
      canvas->Update();


      //
      //
      //
      monitor_position.close();

      if(gSystem->OpenDirectory(Form("%s/plots/run_%i", scratch_dir.Data(), run_number)) == 0){
	std::cout << "Making output directory." << std::endl;
	gSystem->Exec(Form("mkdir %s/plots/run_%i", scratch_dir.Data(), run_number));
      }
      
      canvas->SaveAs(Form("plots/run_%i/coord_%s_pattern_%s.png", run_number, coord[k].Data(), pattern_name[j].Data()) );
      phase_c->SaveAs(Form("plots/run_%i/coord_%s_pattern_%s_phase.png", run_number, coord[k].Data(), pattern_name[j].Data()) );      

      baseline->Delete();
      fit_optics->Delete();
      phase_optics->Delete();
      multi_graph->Clear();
      phase_graph->Clear();
      canvas->Clear();
    }
  }
  // End of Cycles loop

  std::cout << "Analysis finished." << std::endl;
  
  //  suuperApp.Run();
  return 0;
}

Double_t GetAmplitudeSign(Double_t d1, Double_t d2, Double_t d3, Double_t fmean)
{

  Double_t sign = 0.0;

  if(d1 > 0.0 && d2 < 0.0)          sign =  1.0;
  else if(d1 == 0.0 && fmean > 0.0) sign =  1.0;
  else if(d1 < 0.0 && d2 < 0.0)     sign =  1.0;

  else if(d1 < 0.0 && d3 < 0)       sign =  1.0;

  else if(d1 < 0.0 && d2 > 0.0)     sign = -1.0;
  else if(d1 == 0.0 && fmean < 0.0) sign = -1.0;
  else if(d1 > 0.0 && d2 > 0.0)     sign = -1.0;

  else if(d1 < 0.0 && d3 > 0)       sign = -1.0;

  else
    sign = 1.0;

  return(sign);

}
