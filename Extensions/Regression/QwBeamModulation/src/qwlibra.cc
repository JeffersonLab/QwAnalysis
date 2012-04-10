#include "headers.h"
#include "QwDiagnostic.hh"
#include "QwData.hh"
#include "TSystem.h"
#include "TStyle.h"

Int_t main(Int_t argc, Char_t *argv[])
{

  //  TApplication theApp("App", &argc, argv);

  TString filename;
  TString correction_cut;

  TChain *mod_tree = new TChain("Mod_Tree");
  
  TCanvas *canvas0 = new TCanvas("c0", "canvas0", 3500, 3500);
  TCanvas *canvas1 = new TCanvas("c1", "canvas1", 3500, 3500);
  TCanvas *canvas2 = new TCanvas("c2", "canvas2", 1000, 1000);
  TCanvas *canvas3 = new TCanvas("c3", "canvas3", 1500, 1500);
  TCanvas *canvas4 = new TCanvas("c4", "canvas4", 1500, 1500);
  TCanvas *canvas5 = new TCanvas("c5", "canvas5", 1500, 1500);
  TCanvas *canvas6 = new TCanvas("c6", "canvas6", 500, 500);
  TCanvas *canvas7 = new TCanvas("c7", "canvas7", 500, 500);
  TCanvas *canvas8 = new TCanvas("c8", "canvas8", 500, 500);

  TCut cut_mod[5];
  TCut cut_nat[5];

  TF1 *linear = new TF1("linear", "[0] + [1]*x", -1.0, 1.0);

  Int_t n = 1;
//   Int_t pNum[5] = {11, 14, 13, 12, 15};
  Int_t pNum[5] = {0, 1, 2, 3, 4};
  Int_t entries;
  Int_t fResolution = 300;

  Double_t meanx;
  Double_t meany;
  Double_t delta;
  Double_t range[5] = {0.12, 4e-6, 0.12, 0.12, 4e-6};
  Double_t yield_range[5] = {0.5, 0.02e-3, 0.5, 0.3, 0.02e-3};
  Double_t fRange[5] = {0.15, 0.0035e-3, 0.3, 0.12, 0.003e-3};
  Double_t fRangeDiff[5] = {0.12, 3.2e-6, 0.14, 0.12, 2.2e-6};
//   Double_t corr_range[5] = {50.e-3, 50.6e-3, 50.e-3, 50.e-3, 50.e-3};
//   Double_t corr_range[5] = {0.1, 0.1, 0.1, 0.1, 0.1};
  Double_t corr_range[5] = {1., 1., 1., 1., 1.};

  QwDiagnostic *modulation = new QwDiagnostic(mod_tree);

  char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
  char other[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
  char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

  gStyle->SetOptFit(01011);
  gStyle->SetOptStat("nemm");

  if(argv[1] == NULL){
    std::cout << red << "No run number specified :: exiting" 
	      << normal << std::endl;
    exit(1);
  }

  modulation->run_number = atoi(argv[1]);
  filename = Form("bmod_tree_%i.root", modulation->run_number);

  std::cout << "Generating diagnostics for run." << std::endl;
  modulation->LoadRootFile(filename, mod_tree);
  modulation->SetFileName(filename);
    
  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;
  modulation->ReadConfig(modulation);

  gSystem->Exec(Form("rm -rf %s_diagnostic", filename.Data()));
  gSystem->Exec(Form("mkdir %s_diagnostic", filename.Data()));

  // ******************************************************************** //
  // Build a proper cut string for the corrections plots.  This should    //
  // match what is used to fill the corrected asymmetry in the analyzer.  //
  // ******************************************************************** //

  correction_cut = "ErrorFlag == 0";

  for(Int_t i = 0; i < (Int_t)(modulation->fNDetector); i++)
    correction_cut = Form("%s &&  asym_%s_Device_Error_Code == 0", correction_cut.Data(), modulation->DetectorList[i].Data());
  for(Int_t i = 0; i < (Int_t)(modulation->fNMonitor); i++)
    correction_cut = Form("%s &&  diff_%s_Device_Error_Code == 0", correction_cut.Data(), modulation->MonitorList[i].Data());

  std::cout << red << correction_cut << normal << std::endl;

  // ******************************************************************** //

  for(Int_t i = 0; i < (modulation->GetModNumber()); i++){
    modulation->SensHistogram.push_back(std::vector <TProfile *>());
    modulation->SensHistogramCorr.push_back(std::vector <TProfile *>());
  }

  std::cout << "Making histograms for Sensitivity Histograms." << std::endl;
  for(Int_t j = 0; j < modulation->fNMonitor; j++){
    modulation->DiffHistogram.push_back(new TH1F(Form("hist_diff_%s", modulation->MonitorList[j].Data()),
						 Form("hist_diff_%s", modulation->MonitorList[j].Data()), 1000, -range[j], range[j] ));
  }
  for(Int_t j = 0; j < modulation->fNDetector; j++){
    modulation->AsymHistogram.push_back(new TH1F(Form("hist_asym_%s", modulation->DetectorList[j].Data()),
						 Form("hist_asym_%s", modulation->DetectorList[j].Data()), 2000, -1.e6, 1.e6 ));
    modulation->RawAsymHistogram.push_back(new TH1F(Form("hist_raw_asym_%s", modulation->DetectorList[j].Data()),
						    Form("hist_raw_asym_%s", modulation->DetectorList[j].Data()), 2000, -1.e6, 1.e6 ));
  }

  for(Int_t i = 0; i < modulation->fNDetector; i++){

      modulation->TotalCorrectionHistogram.push_back(new TH1F(Form("hist_corr_%s", modulation->DetectorList[i].Data()),
							      Form("hist_corr_%s", modulation->DetectorList[i].Data()),
							      10000, -100.e-3, 100.e-3 ));
    for(Int_t j = 0; j < modulation->fNMonitor; j++){
      modulation->CorrectionHistogram.push_back(new TH1F(Form("hist_corr_%s_%s", modulation->DetectorList[i].Data(), modulation->MonitorList[j].Data()),
							 Form("hist_corr_%s_%s", modulation->DetectorList[i].Data(), modulation->MonitorList[j].Data()),
							 50000, -corr_range[j], corr_range[j] ));
    }
  }
  canvas2->Divide(1,modulation->fNMonitor);
  canvas4->Divide(1,modulation->GetModNumber());
  canvas5->Divide(1,modulation->fNMonitor);

  //********************************************
  //  Set cuts for slope correction plots
  //********************************************

  for(Int_t i = 0; i < modulation->GetModNumber(); i++)
    cut_mod[i] = Form("(ErrorFlag==0x4018080) && yield_bm_pattern_number==%i || yield_bm_pattern_number==%i", (pNum[i] + 11), pNum[i]);
  for(Int_t i = 0; i < modulation->GetModNumber(); i++)
    cut_nat[i] = Form(" (ErrorFlag == 0) ", pNum[i]);

  std::cout << "Making Slope Correction Plots" << std::endl;

  for(Int_t k = 0; k < modulation->fNDetector; k++){
    canvas0->Divide(modulation->fNMonitor, modulation->GetModNumber());
    canvas1->Divide(modulation->fNMonitor, modulation->GetModNumber());

    for(Int_t i = 0; i < modulation->fNMonitor; i++){
      for(Int_t j = 0; j < (modulation->GetModNumber()); j++){
	canvas0->cd(n);
	
	gPad->SetGridx();
	gPad->SetGridy();
	
	modulation->Slope[k][i].push_back(new QwData() );
	modulation->SlopeCorr[k][i].push_back(new QwData() );

	//*************************************
	// Getting limits from temp histogram
	//*************************************

	TH2F *temp = new TH2F("temp", "temp", fResolution, -1.0, 1.0, fResolution, -1.0, 1.0);    
	mod_tree->Draw(Form("asym_%s:diff_%s>>temp", modulation->DetectorList[k].Data(), 
			    modulation->MonitorList[i].Data()), cut_mod[j], "prof");

	temp = (TH2F *)gDirectory->Get("temp");
	meanx = temp->GetMean(1);
	meany = temp->GetMean(2);
	temp->Delete();

	// ************************************
	// Building TProfiles
	// ************************************

	modulation->SensHistogram[i].push_back(new TProfile(Form("hist_%i_%s_%s", j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()),
							    Form("hist_%i_%s_%s", j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()), 
							    fResolution, meanx - fRangeDiff[i], meanx + fRangeDiff[i], -1.0, 1.0) );      
	modulation->SensHistogramCorr[i].push_back(new TProfile(Form("corr_hist_%i_%s_%s", j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()),
								Form("corr_hist_%i_%s_%s", j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()), 
								fResolution, meanx - fRangeDiff[i], meanx + fRangeDiff[i], -1.0, 1.0) );

	// ************************************
	// Making TProfile plots
	// ************************************
	modulation->SensHistogram[i][j]->SetErrorOption("i");	
	modulation->SensHistogram[i][j]->SetLineColor(12);	
	mod_tree->Draw(Form("asym_%s:diff_%s>>hist_%i_%s_%s", modulation->DetectorList[k].Data(), 
			    modulation->MonitorList[i].Data(), j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()), cut_mod[j], "");
	
	std::cout << other << Form("asym_%s:diff_%s>>hist_%i_%s_%s", modulation->DetectorList[k].Data(), 
				   modulation->MonitorList[i].Data(), j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()) << " " <<  cut_mod[j] << normal << std::endl;
	modulation->SensHistogram[i][j] = (TProfile *)gDirectory->Get(Form("hist_%i_%s_%s", j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data() ));
 	modulation->SensHistogram[i][j]->SetAxisRange(-0.0005, 0.0005, "Y");
 	for(Int_t m = 1; m < fResolution; m++){
 	  entries = (Int_t)(modulation->SensHistogram[i][j])->GetBinEntries(m);

 	  if(entries < 10 && entries > 0){
	    (modulation->SensHistogram[i][j])->SetBinEntries(m,0.0);
	  }
	}	

	delta = 0.35*(TMath::Abs(meanx - (modulation->SensHistogram[i][j]->GetXaxis()->GetXmax())));
	linear->SetRange(meanx - delta, meanx + delta);
	linear->SetLineColor(2);
	modulation->SensHistogram[i][j]->GetYaxis()->SetTitle(Form("%s Asymmetry", modulation->DetectorList[k].Data()));
	modulation->SensHistogram[i][j]->GetXaxis()->SetTitle(Form("%s", modulation->MonitorList[i].Data()) );
	modulation->SensHistogram[i][j]->SetTitle( Form("%s vs %s -- uncorrected", modulation->DetectorList[k].Data(), 
	 						modulation->MonitorList[i].Data()) );
	modulation->SensHistogram[i][j]->Draw("");

	modulation->SensHistogram[i][j]->Fit("linear","R");
	modulation->Slope[k][i][j]->slope = linear->GetParameter(1);
	modulation->Slope[k][i][j]->error = linear->GetParError(1);
	modulation->Slope[k][i][j]->ChiSquare = linear->GetChisquare();
	modulation->Slope[k][i][j]->NDF = linear->GetNDF();
	modulation->Slope[k][i][j]->ChiSquareNDF = modulation->Slope[k][i][j]->ChiSquare/modulation->Slope[k][i][j]->NDF;
	canvas0->Update();
	canvas0->Modified();


	//**********************************************
	// Getting limits from temp histogram : part 2
	//**********************************************

	canvas1->cd(n);
	gPad->SetGridx();
	gPad->SetGridy();

	TH2F *temp2 = new TH2F("temp2", "temp2", fResolution, -1.0, 1.0, fResolution, -1.0, 1.0);    

	mod_tree->Draw(Form("corr_asym_%s:diff_%s>>temp2", modulation->DetectorList[k].Data(), 
			    modulation->MonitorList[i].Data()), cut_mod[j], "prof");

	temp2 = (TH2F *)gDirectory->Get("temp2");
	meanx = temp2->GetMean(1);
	meany = temp2->GetMean(2);
	temp2->Delete();

	modulation->SensHistogramCorr[i][j]->SetErrorOption("i");	
	modulation->SensHistogramCorr[i][j]->SetLineColor(12);	
	
	mod_tree->Draw(Form("corr_asym_%s:diff_%s>>corr_hist_%i_%s_%s", modulation->DetectorList[k].Data(), 
			    modulation->MonitorList[i].Data(), j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()), cut_mod[j], "prof");
	
	std::cout << other << Form("corr_asym_%s:diff_%s>>corr_hist_%i_%s_%s", modulation->DetectorList[k].Data(), modulation->MonitorList[i].Data(), j, 
				   modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data()) << " " << cut_mod[j] << normal << std::endl;

	modulation->SensHistogramCorr[i][j] = (TProfile *)gDirectory->Get(Form("corr_hist_%i_%s_%s", j, modulation->MonitorList[i].Data(), modulation->DetectorList[k].Data() ));
	modulation->SensHistogramCorr[i][j]->SetAxisRange(-0.0005, 0.0005, "Y");

 	for(Int_t m = 1; m < fResolution; m++){
 	  entries = (Int_t)(modulation->SensHistogramCorr[i][j])->GetBinEntries(m);

 	  if(entries < 10 && entries > 0){
	    (modulation->SensHistogramCorr[i][j])->SetBinEntries(m,0.0);
	  }
	}	
	
	delta = 0.35*(TMath::Abs(meanx - (modulation->SensHistogramCorr[i][j]->GetXaxis()->GetXmax())));
	linear->SetRange(meanx - delta, meanx + delta);
	linear->SetLineColor(2);
	modulation->SensHistogramCorr[i][j]->GetYaxis()->SetTitle(Form("%s Corrected", modulation->DetectorList[k].Data()));
	modulation->SensHistogramCorr[i][j]->GetXaxis()->SetTitle(Form("%s", modulation->MonitorList[i].Data()) );
	modulation->SensHistogramCorr[i][j]->SetTitle(Form("%s vs %s -- corrected", modulation->DetectorList[k].Data(), 
							   modulation->MonitorList[i].Data()) );
	modulation->SensHistogramCorr[i][j]->Draw("");
	modulation->SensHistogramCorr[i][j]->Fit("linear","R");
	modulation->SlopeCorr[k][i][j]->slope = linear->GetParameter(1);
	modulation->SlopeCorr[k][i][j]->error = linear->GetParError(1);
	modulation->SlopeCorr[k][i][j]->ChiSquare = linear->GetChisquare();;
	modulation->SlopeCorr[k][i][j]->NDF = linear->GetNDF();
	modulation->SlopeCorr[k][i][j]->ChiSquareNDF = modulation->SlopeCorr[k][i][j]->ChiSquare/modulation->SlopeCorr[k][i][j]->NDF;
	canvas1->Update();
	canvas1->Modified();
	n++;
							      }
      }
    canvas0->SaveAs( Form("%s_diagnostic/uncorrected_%s_sens_slopes.pdf", filename.Data(), modulation->DetectorList[k].Data()) );
    canvas1->SaveAs( Form("%s_diagnostic/corrected_%s_sens_slopes.pdf",   filename.Data(), modulation->DetectorList[k].Data()) );

    canvas0->Clear();
    canvas1->Clear();
    n = 1;
  }

  for(Int_t i = 0; i < modulation->GetModNumber(); i++){
    modulation->DiffSlope.push_back(new QwData() );    
    
    canvas2->cd(i + 1);
    mod_tree->Draw(Form("diff_%s>>hist_diff_%s", modulation->MonitorList[i].Data(), modulation->MonitorList[i].Data()), "ErrorFlag == 0");
    modulation->DiffHistogram[i] = (TH1F *)gDirectory->Get(Form("hist_diff_%s", modulation->MonitorList[i].Data()));
    modulation->DiffSlope[i]->slope = modulation->DiffHistogram[i]->GetMean();
    modulation->DiffSlope[i]->error = modulation->DiffHistogram[i]->GetRMS()/(TMath::Sqrt(modulation->DiffHistogram[i]->GetEntries()));
    modulation->DiffSlope[i]->entries = (Int_t)(modulation->DiffHistogram[i])->GetEntries();
    modulation->DiffHistogram[i]->GetYaxis()->SetTitle("Counts");
    modulation->DiffHistogram[i]->GetXaxis()->SetTitle("position/angle difference");
    canvas2->Update();
    canvas2->Modified();
  }
  
  gStyle->SetOptStat(1111);

  for(Int_t k = 0; k < modulation->fNDetector; k++){
    canvas3->Divide(1,modulation->fNMonitor);
    
    for(Int_t i = 0; i < modulation->GetModNumber(); i++){
      modulation->Correction[k].push_back(new QwData() );    
     
      canvas3->cd(i + 1);
      mod_tree->Draw(Form("corr_asym_%s_diff_%s>>hist_corr_%s_%s", modulation->DetectorList[k].Data(), modulation->MonitorList[i].Data(),
			  modulation->DetectorList[k].Data(), modulation->MonitorList[i].Data()), correction_cut);
      modulation->CorrectionHistogram[i] = (TH1F *)gDirectory->Get(Form("hist_corr_%s_%s", modulation->DetectorList[k].Data(), 
									modulation->MonitorList[i].Data()));
      modulation->Correction[k][i]->slope = modulation->CorrectionHistogram[i]->GetMean();
      modulation->Correction[k][i]->error = modulation->CorrectionHistogram[i]->GetRMS()/(TMath::Sqrt(modulation->CorrectionHistogram[i]->GetEntries()));
      modulation->Correction[k][i]->entries = (Int_t)(modulation->CorrectionHistogram[i])->GetEntries();
      modulation->CorrectionHistogram[i]->SetTitle("Correction to Asymmetry for each Monitor");
      modulation->CorrectionHistogram[i]->GetYaxis()->SetTitle("Counts");
      modulation->CorrectionHistogram[i]->GetXaxis()->SetTitle("correction position/angle difference");
      // Try to refit the histogram to include the enormous widths of the lumis
      Double_t rms = modulation->CorrectionHistogram[i]->GetRMS();
      modulation->CorrectionHistogram[i]->SetAxisRange(-10*rms, 10*rms, "X");
      modulation->CorrectionHistogram[i]->Draw();
      canvas3->Update();
      canvas3->Modified();

    }
    canvas3->SaveAs( Form("%s_diagnostic/corrections_%s.pdf", filename.Data(), modulation->DetectorList[k].Data()) );
    canvas3->Clear();

    // Get total correction to each detector

      modulation->TotalCorrection.push_back(new QwData() );    
     
      canvas8->cd();
      mod_tree->Draw(Form("correction_%s>>hist_corr_%s", modulation->DetectorList[k].Data(),
			  modulation->DetectorList[k].Data()), correction_cut);
      modulation->TotalCorrectionHistogram[k] = (TH1F *)gDirectory->Get(Form("hist_corr_%s", modulation->DetectorList[k].Data()));
      modulation->TotalCorrection[k]->slope = modulation->TotalCorrectionHistogram[k]->GetMean();
      modulation->TotalCorrection[k]->error = modulation->TotalCorrectionHistogram[k]->GetRMS()/(TMath::Sqrt(modulation->TotalCorrectionHistogram[k]->GetEntries()));
      modulation->TotalCorrection[k]->entries = (Int_t)(modulation->TotalCorrectionHistogram[k])->GetEntries();
      modulation->TotalCorrectionHistogram[k]->SetTitle(Form("Total Correction to Asymmetry for %s", modulation->DetectorList[k].Data()));
      modulation->TotalCorrectionHistogram[k]->GetYaxis()->SetTitle("Counts");
      modulation->TotalCorrectionHistogram[k]->GetXaxis()->SetTitle("correction position/angle difference");

      Double_t totalrms = modulation->TotalCorrectionHistogram[k]->GetRMS();
      modulation->TotalCorrectionHistogram[k]->SetAxisRange(-10*totalrms, 10*totalrms, "X");
      (modulation->TotalCorrectionHistogram[k]->GetXaxis())->SetLabelSize(0.02);
      modulation->TotalCorrectionHistogram[k]->Draw();
      canvas8->Update();
      canvas8->Modified();

      canvas8->SaveAs( Form("%s_diagnostic/total_correction_%s.pdf", filename.Data(), modulation->DetectorList[k].Data()) );
      canvas8->Clear();
  }

  //**********************************************************************
  //
  //  Plots of detector sensitivity to natural motion after corrections
  //
  //**********************************************************************
  
  for(Int_t i = 0; i < modulation->fNDetector; i++){
    for(Int_t j = 0; j < modulation->fNMonitor; j++){
      modulation->NatSlope[i].push_back(new QwData() );    

      canvas5->cd(j + 1);

      TH2F *temp = new TH2F("temp", "temp", fResolution, -1.0, 1.0, fResolution, -1.0, 1.0);    
      mod_tree->Draw(Form("corr_asym_%s:diff_%s>>temp", modulation->DetectorList[i].Data(), 
			  modulation->MonitorList[j].Data()), cut_nat[j], "prof");

	temp = (TH2F *)gDirectory->Get("temp");
	meanx = temp->GetMean(1);
	meany = temp->GetMean(2);
	temp->Delete();

	//*********************************
	//  Make the TProfiles
	//*********************************

	modulation->NaturalSensitivity.push_back(new TProfile(Form("sens_hist_%s_%s", modulation->DetectorList[i].Data(), modulation->MonitorList[j].Data()),
							      Form("sens_hist_%s_%s", modulation->DetectorList[i].Data(), modulation->MonitorList[j].Data()),
							      fResolution, meanx - fRangeDiff[i], meanx + fRangeDiff[i], -1.0, 1.0));

	modulation->NaturalSensitivity[j]->SetErrorOption("i");	
	modulation->NaturalSensitivity[j]->SetLineColor(12);	
	
	//*********************************
	//  Make the TProfile plots
	//*********************************

	mod_tree->Draw(Form("corr_asym_%s:diff_%s>>sens_hist_%s_%s", modulation->DetectorList[i].Data(), 
			    modulation->MonitorList[j].Data(),  modulation->MonitorList[j].Data(), modulation->DetectorList[i].Data()), cut_nat[j], "prof");
	
	std::cout << other << Form("corr_asym_%s:diff_%s>>sens_hist_%s_%s", modulation->DetectorList[i].Data(), modulation->MonitorList[j].Data(), 
				   modulation->MonitorList[j].Data(), modulation->DetectorList[i].Data()) << " " << cut_mod[j] << normal << std::endl;

	modulation->NaturalSensitivity[j] = (TProfile *)gDirectory->Get(Form("sens_hist_%s_%s", modulation->MonitorList[j].Data(), modulation->DetectorList[i].Data() ));
	modulation->NaturalSensitivity[j]->SetAxisRange(-0.0005, 0.0005, "Y");

 	for(Int_t m = 1; m < fResolution; m++){
 	  entries = (Int_t)(modulation->NaturalSensitivity[j])->GetBinEntries(m);

 	  if(entries < 10 && entries > 0){
	    (modulation->NaturalSensitivity[j])->SetBinEntries(m,0.0);
	  }
	}	
	
	delta = 0.35*(TMath::Abs(meanx - (modulation->NaturalSensitivity[j]->GetXaxis()->GetXmax())));
	linear->SetRange(meanx - delta, meanx + delta);
	linear->SetLineColor(2);
	modulation->NaturalSensitivity[j]->GetYaxis()->SetTitle(Form("%s Asymmetry", modulation->DetectorList[i].Data()));
	modulation->NaturalSensitivity[j]->GetXaxis()->SetTitle(Form("%s", modulation->MonitorList[j].Data()) );
	modulation->NaturalSensitivity[j]->SetTitle(Form("%s vs %s Correct Natural Motion", modulation->DetectorList[i].Data(), 
							    modulation->MonitorList[j].Data()) );
	modulation->NaturalSensitivity[j]->Draw("");
	modulation->NaturalSensitivity[j]->Fit("linear","R");
	modulation->NatSlope[i][j]->slope = linear->GetParameter(1);
	modulation->NatSlope[i][j]->error = linear->GetParError(1);
	modulation->NatSlope[i][j]->ChiSquare = linear->GetChisquare();;
	modulation->NatSlope[i][j]->NDF = linear->GetNDF();
	modulation->NatSlope[i][j]->ChiSquareNDF = modulation->NatSlope[i][j]->ChiSquare/modulation->NatSlope[i][j]->NDF;
	canvas5->Update();
	canvas5->Modified();
	n++;

    }
    canvas5->SaveAs( Form("%s_diagnostic/correction_natural_motion_%s.pdf", filename.Data(), modulation->DetectorList[i].Data()) );
    canvas5->Clear();
  }

  //  End detector sensitivity to natural motion


  for(Int_t i = 0; i < modulation->GetModNumber(); i++){
    modulation->ChargeAsym.push_back(new QwData() );
    
    canvas4->cd(i + 1);
    gPad->SetGridx();
    gPad->SetGridy();

    //  Clean up the bins for proflile plots

    TH2F *temp3 = new TH2F("temp3", "temp3", 150, -1.0, 1.0, 150, 160.0, 180.0);    
    mod_tree->Draw(Form("yield_qwk_charge:yield_%s>>temp3", modulation->MonitorList[i].Data()), 
		   Form("(ErrorFlag == 0x4018080) && yield_bm_pattern_number == %i || yield_bm_pattern_number == %i", (pNum[i]+11), pNum[i]),"prof");
    temp3 = (TH2F *)gDirectory->Get("temp3");
    meanx = temp3->GetMean(1);
    meany = temp3->GetMean(2);
    temp3->Delete();

    //***************************************
    //  Create TProfile and fill
    //***************************************
    
    modulation->ChargeAsymHistogram.push_back(new TProfile(Form("prof_charge_asym_%i", pNum[i]), Form("prof_charge_asym_%i", pNum[i]), 
							   150, (meanx - yield_range[i]), (meanx + yield_range[i]), 0.0, 180.0));

    (modulation->ChargeAsymHistogram[i])->SetErrorOption("i");    
    mod_tree->Draw(Form("yield_qwk_charge:yield_%s>>prof_charge_asym_%i", modulation->MonitorList[i].Data(), pNum[i]), 
 		   Form("(ErrorFlag == 0x4018080) && yield_bm_pattern_number == %i || yield_bm_pattern_number == %i", (pNum[i]+11), pNum[i]),"");
    
    modulation->ChargeAsymHistogram[i] = (TProfile *)gDirectory->Get(Form("prof_charge_asym_%i", pNum[i]));
    modulation->ChargeAsymHistogram[i]->SetAxisRange(meany - 2.0, meany + 2.0 , "Y");

    //***************************************
    
    linear->SetRange(meanx - fRange[i], meanx + fRange[i]);
    
    linear->SetLineColor(2);
    modulation->ChargeAsymHistogram[i]->GetYaxis()->SetTitle("Charge");
    modulation->ChargeAsymHistogram[i]->GetXaxis()->SetTitle(Form("yield_%s", modulation->MonitorList[i].Data()) );
    modulation->ChargeAsymHistogram[i]->SetTitle("Charge vs Monitor Yield");
    modulation->ChargeAsymHistogram[i]->Draw("");
    
    modulation->ChargeAsymHistogram[i]->Fit("linear","R");
    modulation->ChargeAsym[i]->slope = linear->GetParameter(1);
    modulation->ChargeAsym[i]->error = linear->GetParError(1);
    modulation->ChargeAsym[i]->ChiSquare = linear->GetChisquare();;
    modulation->ChargeAsym[i]->NDF = linear->GetNDF();
    modulation->ChargeAsym[i]->ChiSquareNDF = modulation->ChargeAsym[i]->ChiSquare/modulation->ChargeAsym[i]->NDF;
    canvas4->Update();
    canvas4->Modified();
    
  }

  canvas2->SaveAs( Form("%s_diagnostic/position_differences.pdf", filename.Data()) );
  canvas4->SaveAs( Form("%s_diagnostic/charge_asym.pdf", filename.Data()) );

  //**********************************************
  // Mean Asymmetry and Width
  //**********************************************

  for(Int_t i = 0; i < modulation->fNDetector; i++){

    canvas6->cd();
    mod_tree->Draw(Form("1e6*corr_asym_%s>>hist_asym_%s", modulation->DetectorList[i].Data(), modulation->DetectorList[i].Data()), "ErrorFlag == 0");
    modulation->AsymHistogram[i] = (TH1F *)gDirectory->Get(Form("hist_asym_%s", modulation->DetectorList[i].Data()));
    modulation->AsymMean.push_back(modulation->AsymHistogram[i]->GetMean());
    modulation->AsymRMS.push_back(modulation->AsymHistogram[i]->GetRMS());
    modulation->AsymEntries.push_back((modulation->CorrectionHistogram[i])->GetEntries());
    modulation->AsymHistogram[i]->GetYaxis()->SetTitle("Counts");
    modulation->AsymHistogram[i]->GetXaxis()->SetTitle("Asymmetry (ppm)");
    canvas6->Update();
    canvas6->Modified();

    canvas6->SaveAs( Form("%s_diagnostic/corrected_asym_%s.pdf", filename.Data(), modulation->DetectorList[i].Data()) );
    canvas6->Clear();
  }
  
  for(Int_t i = 0; i < modulation->fNDetector; i++){

    canvas7->cd();
    mod_tree->Draw(Form("1e6*asym_%s>>hist_raw_asym_%s", modulation->DetectorList[i].Data(), modulation->DetectorList[i].Data()), "ErrorFlag == 0");
    modulation->RawAsymHistogram[i] = (TH1F *)gDirectory->Get(Form("hist_raw_asym_%s", modulation->DetectorList[i].Data()));
    modulation->RawAsymMean.push_back(modulation->RawAsymHistogram[i]->GetMean());
    modulation->RawAsymRMS.push_back(modulation->RawAsymHistogram[i]->GetRMS());
    modulation->RawAsymHistogram[i]->GetYaxis()->SetTitle("Counts");
    modulation->RawAsymHistogram[i]->GetXaxis()->SetTitle("Asymmetry (ppm)");
    canvas7->Update();
    canvas7->Modified();

    canvas7->SaveAs( Form("%s_diagnostic/uncorrected_asym_%s.pdf", filename.Data(), modulation->DetectorList[i].Data()) );
    canvas7->Clear();

  }
  
  std::cout << other << "Writing results to output files" << normal << std::endl;
  modulation->Write();
  
  std::cout << "Done" << std::endl;
    
  //  theApp.Run();
  
  return 0;
    
}
