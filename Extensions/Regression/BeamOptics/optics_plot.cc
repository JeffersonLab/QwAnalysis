#include "headers.h"
#include "beammod.hh"

const Int_t fNMonitors = 24;
const Int_t fCycles = 5;

Int_t main(Int_t argc, Char_t *argv[])
{
  //  TApplication suuperApp("App", &argc, argv);

  TMultiGraph *multi_graph = new TMultiGraph();

  TString filename;
  TString cut;
  TString file_stem = "QwPass1_";
  TString pattern_name[5] = {"X", "Y", "E", "XP", "YP"};
  TString ramp_cut = "((ramp.block0 + ramp.block3) - (ramp.block1 + ramp.block2)) > -50 && ((ramp.block0 + ramp.block3) - (ramp.block1 + ramp.block2)) < 50";
  TString monitor[fNMonitors] = {"qwk_target","qwk_bpm3h09b","qwk_bpm3h09","qwk_bpm3h08","qwk_bpm3h07c","qwk_bpm3h07b",
				 "qwk_bpm3h07a","qwk_bpm3h04","qwk_bpm3h02","qwk_bpm3c21","qwk_bpm3c20","qwk_bpm3p03a",
				 "qwk_bpm3p02b","qwk_bpm3p02a","qwk_bpm3c19","qwk_bpm3c18","qwk_bpm3c17","qwk_bpm3c16",
				 "qwk_bpm3c14","qwk_bpm3c12","qwk_bpm3c11","qwk_bpm3c08","qwk_bpm3c07a","qwk_bpm3c07"};
  TString coord[2] = {"X","Y"};
  TString flag;
  TString scratch_dir;

  Bool_t fVerbose = false;

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1200,600);

  Int_t run_number;
  Int_t counter = 0;

  Double_t amplitude[fNMonitors];
  Double_t error[fNMonitors];
  Double_t phase[fNMonitors];
  Double_t bpm_z[fNMonitors];
  Double_t zeros[fNMonitors];

  TChain *chain = new TChain("Mps_Tree");

  TF1 *sine = new TF1("sine", "[0]+[1]*sin(( 1.571e-3)*x +[2])", 0, 4000);

  std::fstream monitor_position;

  run_number = atoi(argv[1]);

  if(argv[2] != NULL){
    flag = argv[2];
    if(flag.CompareTo("--bpms", TString::kExact))
       fVerbose = true;
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

  if(gSystem->OpenDirectory(Form("%s/plots", scratch_dir.Data())) == 0)
    gSystem->Exec(Form("mkdir %s/plots", scratch_dir.Data()));

  // Loop over cycles
  for(Int_t k = 0; k < 2; k ++){
    for(Int_t j = 0; j < fCycles; j++){

      cut = Form("ErrorFlag == 0x4018080 && bm_pattern_number == %i && %s", j + 11, ramp_cut.Data());
      std::cout << cut << std::endl;
      
      for(Int_t i = 0; i < fNMonitors; i++){
	canvas->cd();
	TH2F* histo = new TH2F("histo", "histo", 400, 0., 4000., 1000, -1., 1.);
	
	chain->Draw(Form("%s%s:ramp>>histo", monitor[i].Data(), coord[k].Data()), cut, "prof");
	histo = (TH2F *)gDirectory->Get("histo");
	sine->SetParameters(histo->GetMean(), 0.150, 0.1);
	sine->SetParLimits(2, 0, TMath::PiOver4() );
	histo->Fit("sine","R");    
	amplitude[i] = sine->GetParameter(1);
	error[i] = sine->GetParError(1);
	phase[i] = sine->GetParameter(2);

	if(fVerbose){
	  canvas->SaveAs(Form("%s/plots/histo_%i.png", scratch_dir.Data(), i));
	  if(gSystem->OpenDirectory(Form("%s/plots/hist_%i", scratch_dir.Data(), run_number)) == 0){
	    std::cout << "Making histogram directory." << std::endl;
	    gSystem->Exec(Form("mkdir %s/plots/hist_%i", scratch_dir.Data(), run_number));
	  }

	}

	histo->Delete();
      }
      for(Int_t i = 0; i < fNMonitors; i++) printf("%s\t%-5.5e  %-5.5e\t%-5.5e\n", monitor[i].Data(), amplitude[i], error[i], phase[i]);
      
      
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
      
      baseline->SetMarkerStyle(7);
      baseline->SetMarkerColor(1);
      baseline->SetLineStyle(1);
      fit_optics->SetMarkerStyle(24);
      fit_optics->SetMarkerColor(2);
      fit_optics->SetLineColor(0);    
      fit_optics->SetLineStyle(3);    
      
      multi_graph->Add(baseline);
      multi_graph->Add(fit_optics);
      multi_graph->SetMaximum( 0.3 );
      multi_graph->SetMinimum(-0.3 );
      
      multi_graph->Draw("apl");
      multi_graph->GetXaxis()->SetTitle("Z beamline position(cm)");
      multi_graph->GetYaxis()->SetTitle(Form( "BMod Position Offset %s (mm)  mod_type:%s", coord[k].Data(), pattern_name[j].Data()) );    
      canvas->Update();
      
      monitor_position.close();

      if(gSystem->OpenDirectory(Form("%s/plots/run_%i", scratch_dir.Data(), run_number)) == 0){
	std::cout << "Making output directory." << std::endl;
	gSystem->Exec(Form("mkdir %s/plots/run_%i", scratch_dir.Data(), run_number));
      }
      
      canvas->SaveAs(Form("plots/run_%i/coord_%s_pattern_%s.png", run_number, coord[k].Data(), pattern_name[j].Data()) );
      
      baseline->Delete();
      fit_optics->Delete();
      multi_graph->Clear();
      canvas->Clear();
    }
  }
  // End of Cycles loop

  std::cout << "Analysis finished." << std::endl;
  
  //  suuperApp.Run();
  return 0;
}
