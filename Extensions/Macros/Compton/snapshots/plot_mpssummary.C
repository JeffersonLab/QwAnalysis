#include <iostream>
#include <algorithm>

#include "TCut.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TIterator.h"
#include "TCollection.h"
#include "TChainElement.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TDirectory.h"
#include "THStack.h"
#include "TF1.h"
#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"

#include "globals.C"
#include "plotting.C"
#include "stepthru_mps.C"

// Declarations
void plot_mpssummary_worker(Int_t runnumber = 1, 
	Bool_t do_mps = 1, Bool_t do_samples_correl = 1, Bool_t do_samples_pedestal = 1,  
	Bool_t do_samples_pedcorr = 1,  Bool_t do_pedcorrection = 1, Bool_t do_samples = 1,
	Bool_t do_samples_asymmetry = 1,  Bool_t do_samples_responsefunc = 1, Bool_t do_waveforms = 1);



// *********************
// **** This is the main
// 

void plot_mpssummary_DEVEL(Int_t runnumber = 1, 
					 Bool_t do_mps=1, Bool_t do_samples_correl=1, Bool_t do_samples_pedestal=1,  
					 Bool_t do_samples_pedcorr=1,  Bool_t do_pedcorrection=1, Bool_t do_samples=1,
					 Bool_t do_samples_asymmetry=1,  Bool_t do_samples_responsefunc=1, Bool_t do_waveforms=1) 
{
	if (runnumber==1) {
		printf("\n\nUseage:\n\t	plot_mpssummary( runnumber, [do_mps], [do_samples_correl], [do_samples_pedestal], [do_samples_pedcorr],\n"); 
		printf("\t\t\t[do_pedcorrection], [do_samples], [do_samples_asymmetry], [do_samples_responsefunc], [do_waveforms])\n\n\n");
		return;
	}
	plot_mpssummary_worker(runnumber, do_mps, do_samples_correl, do_samples_pedestal, do_samples_pedcorr, 
						   do_pedcorrection, do_samples, do_samples_asymmetry, do_samples_responsefunc, do_waveforms);
}

void plot_mpssummary(Int_t runnumber = 1, 
					 Bool_t do_mps=1, Bool_t do_samples_correl=1, Bool_t do_samples_pedestal=1,  
					 Bool_t do_samples_pedcorr=1,  Bool_t do_pedcorrection=1, Bool_t do_samples=1,
					 Bool_t do_samples_asymmetry=1,  Bool_t do_samples_responsefunc=1, Bool_t do_waveforms=1) 
{
	if (runnumber==1) {
		printf("\n\nUseage:\n\t	plot_mpssummary(runnumber, [do_mps], [do_samples_correl], [do_samples_pedestal], [do_samples_pedcorr],\n"); 
		printf("\t\t\t[do_pedcorrection], [do_samples], [do_samples_asymmetry], [do_samples_responsefunc], [do_waveforms])\n\n\n");
		return;
	}
	plot_mpssummary_worker(runnumber, do_mps, do_samples_correl, do_samples_pedestal, do_samples_pedcorr, 
						   do_pedcorrection, do_samples, do_samples_asymmetry, do_waveforms);
}

void plot_mpssummary_worker(Int_t runnumber,
	Bool_t do_mps, Bool_t do_samples_correl, Bool_t do_samples_pedestal,  
	Bool_t do_samples_pedcorr,  Bool_t do_pedcorrection, Bool_t do_samples,
	Bool_t do_samples_asymmetry,  Bool_t do_samples_responsefunc, Bool_t do_waveforms)
{

	printf("%i  do_mps,\n%i  do_samples_correl,\n%i  do_samples_pedestal,\n%i  do_samples_pedcorr,\n%i   do_pedcorrection,\n%i  do_samples,\n%i  do_samples_asymmetry,\n%i  do_samples_responsefunc,\n%i  do_waveforms\n",
		   do_mps,  do_samples_correl,  do_samples_pedestal,  
		   do_samples_pedcorr,   do_pedcorrection,  do_samples,
		   do_samples_asymmetry,  do_samples_responsefunc, do_waveforms);

	printf("Attempting to open the plotting routinges\n");
	gROOT->ProcessLine(".L plotting.C");

	char rundir[255], outtextfilename_ped[255], outtextfilename_width[255];
	sprintf(rundir,"$QWSCRATCH/www");
	sprintf(outtextfilename_ped,"%s/run_%i/pedestalfit.txt", rundir, runnumber);
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);
	// **** Really important number for pedestal correction
	Double_t numsamples = 190; //256


	Int_t yieldrebinnum = 10;
	Int_t intrebinnum = 10;
	// *************************** 
	// ***** Set up for the canvas
	// *************************** 
	Int_t canvaswidth,canvasheight,canvasxoff,xpads,ypads;
	if (gROOT->IsBatch()) {
		canvasxoff=0;  canvaswidth=1000;  canvasheight=800; xpads=3; ypads=6; // Batch mode
	} else {
		canvasxoff=100;  canvaswidth=800;  canvasheight=600; xpads=6; ypads=3; // Interactive vnc
		//canvasxoff=200;  canvaswidth=1000;  canvasheight=800; xpads=3; ypads=6; // Interactive ccomptl1
	}
	if (0) {
		printf("\nKill me now!!!!!\n");
		sleep(10);
	}
	TString filename = Form("summary/Compton_NewSummary_%i.root",runnumber);
	printf("opening %s for input\n",filename.Data());
	TFile *file0 = TFile::Open(filename.Data());
	if (!file0) {
		printf("bye!\n\n");
		return;
	}

	Int_t intmin, intmax, subintmin, subintmax ,maxmin, maxmax;
	intmin = 100; intmax = 550; subintmin = 300; subintmax = 550;
	if (runnumber <= 21136) {
		intmin = 350; intmax = 650; subintmin = 350; subintmax = 650; maxmin=250; maxmax=400;
	}
// 	if (runnumber == 21136) {
// 		intmin = 80; intmax = 650; subintmin = 160; subintmax = 650; maxmin=180; maxmax=330;
// 	}
	if (runnumber >= 21137) {
		intmin = 110; intmax = 650; subintmin = 200; subintmax = 650; maxmin=140; maxmax=330;
	}
	if (runnumber >= 21139) {
		intmin = 300; intmax = 1150; subintmin = 350; subintmax = 1150; maxmin=190; maxmax=330;
	}
	if (runnumber >= 21140) {
		intmin = 550; intmax = 1150; subintmin = 600; subintmax = 1150; maxmin=300; maxmax=330;
	}
	if (runnumber >= 21142) {
		intmin = 500; intmax = 1000; subintmin = 550; subintmax = 1000; maxmin=330; maxmax=350;
	}
	if (runnumber >= 21143) {
		intmin = 450; intmax = 1000; subintmin = 450; subintmax = 1000; maxmin=330; maxmax=350;
	}	
	if (runnumber == 21144) {
		intmin = 100; intmax = 1000; subintmin = 170; subintmax = 1000; maxmin=150; maxmax=330;
	}	
	if (runnumber == 21145) {
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=330; maxmax=350;
	}	
	if (runnumber == 21146 || runnumber == 21147) {
		intmin = 300; intmax = 1000; subintmin = 300; subintmax = 1000; maxmin=250; maxmax=350;
	}
	if (runnumber >= 21148) {
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=250; maxmax=350;
	}	
	if (runnumber >= 21317) {
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=270; maxmax=350;
	}
	if (runnumber >= 21339) { // Start with electron triggers, 1900 V
		intmin = 400; intmax = 1000; subintmin = 450; subintmax = 1000; maxmin=270; maxmax=350;
	}	
	if (runnumber >= 21391) { // Go to 1800 V
		intmin = 250; intmax = 1000; subintmin = 300; subintmax = 1000; maxmin=200; maxmax=350;
	}	
 	if (runnumber == 21393) { // 1900 V
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=270; maxmax=350;
	}  
	if (runnumber >= 21452) { // Go to 1900 V
		intmin = 350; intmax = 1000; subintmin = 400; subintmax = 1000; maxmin=270; maxmax=350;
	}
	if (runnumber >= 21608) { // PbW04
		intmin = 200; intmax = 1000; subintmin = 360; subintmax = 1000; maxmin=80; maxmax=260;
	}
	if (runnumber == 21619) { // PbW04
		intmin = 360; intmax = 1000; subintmin = 400; subintmax = 1000; maxmin=130; maxmax=260;
	}
	if (runnumber >= 21660) { // PbW04
		intmin = 150; intmax = 1000; subintmin = 200; subintmax = 1000; maxmin=140; maxmax=388;
	}
	if (runnumber >= 21987) { // BGO
		intmin = 150; intmax = 1000; subintmin = 200; subintmax = 1000; maxmin=140; maxmax=388;
	}
	printf("Using intmin %i   intmax %i   subintmin %i   subintmax %i   maxmin %i   maxmax %i \n",
		   intmin, intmax, subintmin, subintmax, maxmin, maxmax);

	TLatex text;
	text.SetNDC(kTRUE);
	text.SetTextFont(42);
	text.SetTextAlign(22);
	text.SetTextSize(0.04);
	Float_t textspace=0.06;

// 	gStyle->SetPadRightMargin(0.02);
// 	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.06,"hxyz");
	gStyle->SetTitleSize(0.06,"hxyz");
	//gStyle->SetTitleXOffset(0.7);

	TCanvas *waveforms_laserON;
	TPad *c_waveforms_laserON;
	TVirtualPad *padp;
	if (do_waveforms) {
		waveforms_laserON = new TCanvas("waveforms_laserON", Form("waveforms %i",runnumber), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_laserON->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 7: Laser ON snapshots", runnumber));
		c_waveforms_laserON = new TPad("c_waveforms_laserON","c_waveforms_laserON",0,0,1,1.0-textspace);
		c_waveforms_laserON->Draw();
		c_waveforms_laserON->Divide(5,4,0.001,0.001);
		for (Int_t i=1; i<=20; i++) {
			padp = c_waveforms_laserON->cd(i);
			TGraph *graph1;
//			graph1 = 
			gDirectory->GetObject(Form("laserON%02i",i-1),graph1);
			//cout << graph1 << endl;
			if (graph1) {
				graph1->Draw("al");
				padp->SetMargin(0.15,0.02,0.1,0.1);
				graph1->GetXaxis()->SetLabelSize(0.07);
				graph1->GetYaxis()->SetLabelSize(0.07);
				graph1->GetXaxis()->SetTitleSize(0.07);
				graph1->GetYaxis()->SetTitleSize(0.07);
				//graph1->SetTitleSize(0.07);
			}
		}
		waveforms_laserON->Print(Form("%s/run_%i/waveforms_laserON_%i.png",rundir,runnumber,runnumber));
	}

	TCanvas *waveforms_laserOFF;
	TPad *c_waveforms_laserOFF;    
	if (do_waveforms) {
		waveforms_laserOFF = new TCanvas("waveforms_laserOFF", Form("waveforms %i",runnumber), 
										 canvasxoff,0,canvaswidth,canvasheight);
		waveforms_laserOFF->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 8: Laser OFF snapshots", runnumber));
		c_waveforms_laserOFF = new TPad("c_waveforms_laserOFF","c_waveforms_laserOFF",0,0,1,1.0-textspace);
		c_waveforms_laserOFF->Draw();
		c_waveforms_laserOFF->Divide(5,4,0.001,0.001);
		for (Int_t i=1; i<=20; i++) {
			padp = c_waveforms_laserOFF->cd(i);
			TGraph *graph2 = (TGraph*) gDirectory->Get(Form("laserOFF%02i",i-1));
			if (graph2) {
				//graph2->SetTitleSize(0.07);
				graph2->Draw("al");
				graph2->GetXaxis()->SetLabelSize(0.07);
				graph2->GetYaxis()->SetLabelSize(0.07);
				graph2->GetXaxis()->SetTitleSize(0.07);
				graph2->GetYaxis()->SetTitleSize(0.07);
				//graph2->SetTitleSize(0.07);
				padp->SetMargin(0.15,0.02,0.1,0.1);
			}
		}
		waveforms_laserOFF->Print(Form("%s/run_%i/waveforms_laserOFF_%i.png",rundir,runnumber,runnumber));
	}

	TCanvas *waveforms_beamOFF;
	TPad *c_waveforms_beamOFF;    
	if (do_waveforms) {
		waveforms_beamOFF = new TCanvas("waveforms_beamOFF", Form("waveforms %i",runnumber), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_beamOFF->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 9: beam OFF snapshots", runnumber));
		c_waveforms_beamOFF = new TPad("c_waveforms_beamOFF","c_waveforms_beamOFF",0,0,1,1.0-textspace);
		c_waveforms_beamOFF->Draw();
		c_waveforms_beamOFF->Divide(5,4,0.001,0.001);
		for (Int_t i=1; i<=20; i++) {
			padp = c_waveforms_beamOFF->cd(i);
			TGraph *graph3 = (TGraph*) gDirectory->Get(Form("beamOFF%02i",i-1));
			if (graph3) {
				graph3->Draw("al");
				graph3->GetXaxis()->SetLabelSize(0.07);
				graph3->GetYaxis()->SetLabelSize(0.07);
				graph3->GetXaxis()->SetTitleSize(0.07);
				graph3->GetYaxis()->SetTitleSize(0.07);
				//graph3->SetTitleSize(0.07);
				padp->SetMargin(0.15,0.02,0.1,0.1);
			}
		}
		waveforms_beamOFF->Print(Form("%s/run_%i/waveforms_beamOFF_%i.png",rundir,runnumber,runnumber));
	}



	TCanvas *mps;
	TPad *c_mps;
	if (do_mps) {
		mps = new TCanvas("mps", Form("mps %i",runnumber),canvasxoff,0,canvaswidth,canvasheight);
		mps->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 1: MPS tree general",runnumber));
		c_mps = new TPad("c_mps","c_mps",0,0,1,1.0-textspace);
		c_mps->Draw();
		c_mps->Divide(1,5,0.001,0.001);
		c_mps->cd(1);
		projectALLtypeTH1(lasertime,0,"",0,1,1);
		c_mps->cd(2);
		projectALLtypeTH1(bcmtime,0,"",0,1,1);
		c_mps->cd(3);
		projectALLtypeTH1(accum0time,0,"",0,1,1);
		c_mps->cd(4);
		projectALLtypeTH1(samp_pedVStime,0,"",0,1,1);
		c_mps->cd(5);
		snaphist->DrawCopy();
		//cerr << "time 3\n" << endl;
		gPad->SetMargin(0.06,0.02,0.1,0.1);
		mps->Print(Form("%s/run_%i/mps_summary_%i.png", rundir,runnumber,runnumber));
	}

	TCanvas *samples_correl;
	TPad *c_samples_correl;
	if (do_samples_correl) {
		samples_correl = new TCanvas("samples_correl", Form("samples_correl %i",runnumber),
									 canvasxoff,0,canvaswidth,canvasheight);
		samples_correl->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 2: Snapshot general",runnumber));
		c_samples_correl = new TPad("c_samples_correl","c_samples_correl",0,0,1,1.0-textspace);
		c_samples_correl->Draw();
		c_samples_correl->Divide(3,3,0.001,0.001);
		c_samples_correl->cd(1);
		drawscalesubbytypeTH1(samp_max, 1, 2, maxmin, maxmax, 1);
		c_samples_correl->cd(2);
		drawALLtypeTH1(samp_min,0,"",1);
		c_samples_correl->cd(3);
		drawALLtypeTH1(samp_pedave,1,"",1);
		c_samples_correl->cd(4);
		drawALLtypeTH1(samp_maxpos,0,"",1);
		c_samples_correl->cd(5);
		drawALLtypeTH1(samp_minpos,0,"",1);
		c_samples_correl->cd(6);
		drawALLtypeTH1(samp_pedprepeakave,1,"",1);
		c_samples_correl->cd(7);
		sprintf(outtextfilename_width,"%s/run_%i/width_pairdiff.txt", rundir, runnumber);
		drawALLtypeTH1(accum0_diff_pair,1,"",1,outtextfilename_width); 
		c_samples_correl->cd(8);
		sprintf(outtextfilename_width,"%s/run_%i/width_quaddiff.txt", rundir, runnumber);
		drawALLtypeTH1(accum0_diff_quad,1,"",1,outtextfilename_width);
		c_samples_correl->cd(9);
		sprintf(outtextfilename_width,"%s/run_%i/width_32diff.txt", rundir, runnumber);
		drawALLtypeTH1(accum0_diff_32,1,"",1,outtextfilename_width);
		//drawALLtypeTH1(accum0_asym_32);
		//    draw3TH1gen(samp_bcm17,samp_bcm1,samp_bcm2,1,"bcm 17","bcm 1","bcm 2",kRed,kGreen,kBlue,"bcms");
//		drawALLtypeTH1(accum0,1,"",1);
		//drawscalesub1D(samp_laserOFF_max, samp_laserON_max, 160, 280, 0);
		samples_correl->Print(Form("%s/run_%i/samples_correl_%i.png",rundir,runnumber,runnumber));
	}

	TCanvas *samples_pedestal;
	TPad *c_samples_pedestal;
	if (do_samples_pedestal) {
		samples_pedestal = new TCanvas("samples_pedestal",
									   Form("samples_pedestal %i",runnumber),
									   canvasxoff,0,canvaswidth,canvasheight);
		samples_pedestal->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 3: Snapshot pedestal analysis",runnumber));
		c_samples_pedestal = new TPad("c_samples_pedestal","c_samples_pedestal",0,0,1,1.0-textspace);
		c_samples_pedestal->Draw();
		c_samples_pedestal->Divide(4,4,0.001,0.001);
		c_samples_pedestal->cd(1);
		draw1typeTH2(samp_maxVSmaxpos,1,0,"Laser ON");
		c_samples_pedestal->cd(2);
		draw1typeTH2(samp_maxVSmaxpos,2,0,"Laser OFF");
		c_samples_pedestal->cd(3);
		draw1typeTH2(samp_maxVSmaxpos,3,0,"Beam OFF");
		c_samples_pedestal->cd(4);
		draw1typeTH2(samp_maxVSmaxpos,4,0,"laser flash");

		c_samples_pedestal->cd(5);
		draw1typeTH2(samp_minVSminpos,1,0,"Laser ON");
		c_samples_pedestal->cd(6);
		draw1typeTH2(samp_minVSminpos,2,0,"Laser OFF");
		c_samples_pedestal->cd(7);
		draw1typeTH2(samp_minVSminpos,3,0,"Beam OFF");
		c_samples_pedestal->cd(8);
		draw1typeTH2(samp_minVSminpos,4,0,"laser flash");

		c_samples_pedestal->cd(9);
		draw1typeTH2(samp_pedVStime,1,0,"Laser ON");
		c_samples_pedestal->cd(10);
		draw1typeTH2(samp_pedVStime,2,0,"Laser OFF");
		c_samples_pedestal->cd(11);
		draw1typeTH2(samp_pedVStime,3,0,"Beam OFF");
		c_samples_pedestal->cd(12);
		draw1typeTH2(samp_pedVStime,4,0,"laser flash");

		c_samples_pedestal->cd(13);
		draw1typeTH2(samp_pedVSpedpp,1,0,"Laser ON");
		c_samples_pedestal->cd(14);
		draw1typeTH2(samp_pedVSpedpp,2,0,"Laser OFF");
		c_samples_pedestal->cd(15);
		draw1typeTH2(samp_pedVSpedpp,3,0,"Beam OFF");
		c_samples_pedestal->cd(16);
		draw1typeTH2(samp_pedVSpedpp,4,0,"laser flash");

		samples_pedestal->Print(Form("%s/run_%i/samples_ped_%i.png",
									 rundir,runnumber,runnumber));
	}

	TCanvas *samples_pedcorr;
	TPad *c_samples_pedcorr;
	if (do_samples_pedcorr) {
		samples_pedcorr = new TCanvas("samples_pedcorr", Form("samples_pedcorr %i",runnumber),
									  canvasxoff,0,canvaswidth,canvasheight);
		samples_pedcorr->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 4: Pedestal correction analysis",runnumber));
		c_samples_pedcorr = new TPad("c_samples_pedcorr","c_samples_pedcorr",0,0,1,1.0-textspace);
		c_samples_pedcorr->Draw();
		c_samples_pedcorr->Divide(3,3,0.001,0.001);
		c_samples_pedcorr->cd(1);
		draw1typeTH2(samp_pedVSmin,1,0,"Laser ON");
		c_samples_pedcorr->cd(2);
		draw1typeTH2(samp_pedVSmin,2,0,"Laser OFF");
		c_samples_pedcorr->cd(3);
		draw1typeTH2(samp_pedVSmin,3,0,"Beam OFF");
		c_samples_pedcorr->cd(4);
		draw1typeTH2(samp_pedVSyield,1,0,"Laser ON");
		c_samples_pedcorr->cd(5);
		draw1typeTH2(samp_pedVSyield,2,0,"Laser OFF");
		c_samples_pedcorr->cd(6);
		draw1typeTH2(samp_pedVSyield,3,0,"Beam OFF");
 		c_samples_pedcorr->cd(7);
		projectALLtypeTH1(samp_pedVSyield);
 		c_samples_pedcorr->cd(8);
		projectALLtypeTH1andfit(samp_pedVSyield,1,"",0,10,outtextfilename_ped);  //rebin here
		samples_pedcorr->Print(Form("%s/run_%i/samples_pedcorr_%i.png",
									rundir,runnumber,runnumber));
		system(Form("cat %s",outtextfilename_ped));
	}

	// ******************
	// **** Here correct for pedestal shift
	// ******************
	TH2D* samp_yieldVSsubint_type[numtypes];
	if (do_pedcorrection) {
		TH3F *samp_yieldVSsubint_pedcorr = samp_yieldVSsubint->Clone("samp_yieldVSsubint_pedcorr");
		samp_yieldVSsubint_pedcorr->Reset();
		samp_yieldVSsubint_pedcorr->SetTitle("above, pedestal corrected");
// 		TH2F *samp_laserON_yieldVSsubint_hel0_pedcorr = samp_laserON_yieldVSsubint_hel0
// 			->Clone("samp_laserON_yieldVSsubint_hel0_pedcorr");
// 		samp_laserON_yieldVSsubint_hel0_pedcorr->Reset();
// 		samp_laserON_yieldVSsubint_hel0_pedcorr->SetTitle("hel 0, pedestal corrected");
// 		TH2F *samp_laserON_yieldVSsubint_hel1_pedcorr = samp_laserON_yieldVSsubint_hel1
// 			->Clone("samp_laserON_yieldVSsubint_hel1_pedcorr");
// 		samp_laserON_yieldVSsubint_hel1_pedcorr->Reset();
// 		samp_laserON_yieldVSsubint_hel1_pedcorr->SetTitle("hel 1, pedestal corrected");
		TStopwatch timer;

		Double_t entries[numtypes];
		Double_t binCenter, binWidth, bincont, binerr, newybin;
		char name[numtypes][255];
		sprintf(name,"%s",samp_yieldVSsubint->GetName());
		Int_t xbins = samp_yieldVSsubint->GetNbinsX();
		Int_t ybins = samp_yieldVSsubint->GetNbinsY();
		TAxis *xaxis = samp_yieldVSsubint->GetXaxis();
		binWidth = xaxis->GetBinWidth(1);

		Double_t laseron_ped, laseroff_ped, beamoff_ped, laseron_pedcorr, laseroff_pedcorr, beamoff_pedcorr;
		Int_t laseron_bincorr, laseroff_bincorr, beamoff_bincorr;

		for (Int_t typestep = 1; typestep <= numtypes; typestep++) {
			// **** first decide if we should analyse this type
			samp_yieldVSsubint->GetZaxis()->SetRange(typestep,typestep);
			samp_yieldVSsubint_type[typestep-1] = (TH2D*)samp_yieldVSsubint->Project3D(Form("yx%i",typestep));
			entries[typestep-1] = samp_yieldVSsubint_type[typestep-1]->GetEntries();
			printf("type %i has %i events\n",typestep,entries[typestep-1]);
			if (entries[typestep-1]>0) {
				Int_t xbinmin = samp_yieldVSsubint_type[typestep-1]->FindFirstBinAbove(0, 1);
				Int_t xbinmax = samp_yieldVSsubint_type[typestep-1]->FindLastBinAbove(0, 1);
				Int_t ybinmin = samp_yieldVSsubint_type[typestep-1]->FindFirstBinAbove(0, 2);
				Int_t ybinmax = samp_yieldVSsubint_type[typestep-1]->FindLastBinAbove(0, 2);
				printf("Working in a box of %i bins from %i to %i and %i to %i\n",
					   (xbinmax-xbinmin)*(ybinmax-ybinmin),xbinmin, xbinmax, ybinmin, ybinmax);
				TAxis *yaxis = samp_yieldVSsubint_type[typestep-1]->GetYaxis();
				for (Int_t ybincount = ybinmin; ybincount <= ybinmax; ybincount++) {
					binCenter = yaxis->GetBinCenter(ybincount);
					laseron_ped = fitpol1[0]->Eval(binCenter);
					//laseron_ped = fitpol1[typestep-1]->Eval(binCenter); // NB change back
					laseron_pedcorr = numsamples * laseron_ped;
					laseron_bincorr = laseron_pedcorr/binWidth;
					//	printf("bin %i pedestal for %9.4g is %.2f correction is %.0f in bins  %.0f\n",
					//     ybincount, binCenter, laseron_ped, laseron_pedcorr, laseron_bincorr);
					for (Int_t xbincount = xbinmin; xbincount <= xbinmax; xbincount++) {
						// apply pedestal correction for laser ON
						Int_t newxbin = xbincount-laseron_bincorr;
						if (newxbin > 0) {
							bincont = samp_yieldVSsubint_type[typestep-1]->GetBinContent(xbincount,ybincount);
							binerr = samp_yieldVSsubint_type[typestep-1]->GetBinError(xbincount,ybincount);
							if (bincont>0){
								samp_yieldVSsubint_pedcorr->SetBinContent(newxbin,ybincount,typestep,bincont);
								samp_yieldVSsubint_pedcorr->SetBinError(newxbin,ybincount,typestep,binerr);
							} 
// 							bincont = samp_laserON_yieldVSsubint_hel0->GetBinContent(xbincount,ybincount);
// 							binerr = samp_laserON_yieldVSsubint_hel0->GetBinError(xbincount,ybincount);
// 							if (bincont>0){
// 								samp_laserON_yieldVSsubint_hel0_pedcorr->SetBinContent(newxbin,ybincount,bincont);
// 								samp_laserON_yieldVSsubint_hel0_pedcorr->SetBinError(newxbin,ybincount,binerr);
// 							} 
// 							bincont = samp_laserON_yieldVSsubint_hel1->GetBinContent(xbincount,ybincount);
// 							binerr = samp_laserON_yieldVSsubint_hel1->GetBinError(xbincount,ybincount);
// 							if (bincont>0){
// 								samp_laserON_yieldVSsubint_hel1_pedcorr->SetBinContent(newxbin,ybincount,bincont);
// 								samp_laserON_yieldVSsubint_hel1_pedcorr->SetBinError(newxbin,ybincount,binerr);
// 							} 
						}
					}
				}
			}
			Double_t realtime = timer.CpuTime();
			Double_t cputime = timer.RealTime();
			printf ("Type %i took %.2f seconds (%.2f s of cpu)\n",typestep,realtime,cputime);
			timer.Start(1);
		}
	}


	TCanvas *samples;
	TPad *c_samples;
	if (do_samples) {
		if (!do_pedcorrection) {
			printf("Need to calculatepedestals to plot this stuff\n");
		} else {
			samples = new TCanvas("samples", Form("samples %i",runnumber), canvasxoff,0,canvaswidth,canvasheight);
			samples->cd();
			text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 5: Pedestal corrected integral analysis", runnumber));
			c_samples = new TPad("c_samples","c_samples",0,0,1,1.0-textspace);
			c_samples->Draw();
			c_samples->Divide(3,4,0.001,0.001);
			c_samples->cd(1);
			draw1typeTH2(samp_yieldVSsubint,1,0,"Laser ON");
			c_samples->cd(2);
			draw1typeTH2(samp_yieldVSsubint,2,0,"Laser OFF");
			c_samples->cd(3);
			draw1typeTH2(samp_yieldVSsubint,3,0,"Beam OFF");
			
// 		TH1F *samp_pedcorr_subintproj = (TH1F*)samp_yieldVSsubint_pedcorr->
// 			ProjectionX("samp_yieldVSsubint_pedcorr_proj",1,-1,"e");
// 		samp_pedcorr_subintproj->SetTitle("integral, pedestal corrected;integral");
			
// 		TH1F *samp_laserON_hel0_pedcorr_subintproj = (TH1F*)samp_laserON_yieldVSsubint_hel0_pedcorr->
// 			ProjectionX("samp_laserON_yieldVSsubint_hel0_pedcorr_proj",1,-1,"e");
// 		TH1F *samp_laserON_hel1_pedcorr_subintproj = (TH1F*)samp_laserON_yieldVSsubint_hel1_pedcorr->
// 			ProjectionX("samp_laserON_yieldVSsubint_hel1_pedcorr_proj",1,-1,"e"); 
// 		samp_laserON_hel0_pedcorr_subintproj->SetTitle("integral, hel 0, pedestal corrected;integral");
// 		samp_laserON_hel1_pedcorr_subintproj->SetTitle("integral, hel 1, pedestal corrected;integral");
			
			c_samples->cd(4);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,1,0,"Laser ON, ped. corr.");
			c_samples->cd(5);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,2,0,"Laser OFF, ped. corr.");
			c_samples->cd(6);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,3,0,"Beam OFF, ped. corr.");
			
			c_samples->cd(7);
			drawscalesubbytypeTH1(samp_integral, 1, 2, intmin, intmax, 1);
			c_samples->cd(8);
			drawscalesubbytypeTH1(samp_subintegral, 1, 2, intmin, intmax, 1);
			c_samples->cd(9);			
// 		samp_yieldVSsubint_pedcorr->GetXaxis()->SetRange();
// 		samp_yieldVSsubint_pedcorr->GetYaxis()->SetRange();
			samp_yieldVSsubint_pedcorr->GetZaxis()->SetRange();
			TH2F *samp_subint_pedcorr  = (TH2F*)samp_yieldVSsubint_pedcorr->Project3D("zx");
			samp_subint_pedcorr->SetTitle("Integral, (max pos cut, peak cut, ped. corr.)");
			//samp_subint_pedcorr->Draw("colz");
			drawscalesubbytypeTH1(samp_subint_pedcorr, 1, 2, subintmin, subintmax, 1);
	
			c_samples->cd(10);
			drawscalesubbytypeTH1(samp_integral, 1, 2, intmin, intmax, 0);
			c_samples->cd(11);
			drawscalesubbytypeTH1(samp_subintegral, 1, 2, intmin, intmax, 0);
			c_samples->cd(12);
			drawscalesubbytypeTH1(samp_subint_pedcorr, 1, 2, subintmin, subintmax, 0);	

//     c_samples->cd(10);
//     samp_laserON_yieldVSsubint_hel0_pedcorr->Draw("colz");
//     samp_laserON_yieldVSsubint_hel0_pedcorr->Draw("colz");
//     c_samples->cd(11);
//     samp_laserON_yieldVSsubint_hel1_pedcorr->Draw("colz");
//     c_samples->cd(12);
//     samp_laserON_hel0_pedcorr_subintproj->Rebin(10);
//     samp_laserON_hel0_pedcorr_subintproj->Draw();
//     //    samp_laserON_hel1_pedcorr_subintproj-
			samples->Print(Form("%s/run_%i/samples_summary_%i.png",rundir,runnumber,runnumber));
		}
	}




 	TCanvas *samples_asymmetry;
 	TPad *c_samples_asymmetry;
 	if (do_samples_asymmetry) {
		samples_asymmetry = new TCanvas("samples_asymmetry", Form("samples_asymmetry %i",runnumber),
										canvasxoff,0,canvaswidth,canvasheight);
 		samples_asymmetry->cd();
 		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 6: Asymmetry analysis",runnumber));
		c_samples_asymmetry = new TPad("c_samples_asymmetry","c_samples_asymmetry",0,0,1,1.0-textspace);
		c_samples_asymmetry->Draw();
		c_samples_asymmetry->Divide(3,3,0.001,0.001);
		c_samples_asymmetry->cd(1);
		drawscalesubbytypeTH1(samp_max_hel0, 1, 2, maxmin, maxmax, 1);
		c_samples_asymmetry->cd(2);
		drawscalesubbytypeTH1(samp_max_hel1, 1, 2, maxmin, maxmax, 1);

		TH1F *samp_max_hel0_laserON = (TH1F*)samp_max_hel0->ProjectionX("samp_max_hel0_laserON",1,1,"e");
		TH1F *samp_max_hel1_laserON = (TH1F*)samp_max_hel1->ProjectionX("samp_max_hel1_laserON",1,1,"e"); 
		samp_max_hel0_laserON->SetTitle("max, laser ON, hel 0;max");
		samp_max_hel1_laserON->SetTitle("max, laser ON, hel 1;max");
  
 		TH1F *samp_max_laserOFF = (TH1F*)samp_max->ProjectionX("samp_max_laserOFF",2,2,"e"); 
		samp_max_laserOFF->SetTitle("max, laser OFF,;max");

 		Double_t intOFF = samp_max_laserOFF->Integral(maxmin, maxmax);
 		Double_t intON_hel0 = samp_max_hel0_laserON->Integral(maxmin, maxmax);
 		Double_t intON_hel1 = samp_max_hel1_laserON->Integral(maxmin, maxmax);
 		Double_t scalefac = (intON_hel0+intON_hel1)/(2*intOFF);
 		printf("intON_hel0=%3g  intON_hel1=%3g  intOFF=%3g  scalefac=%3g\n",intON_hel0, intON_hel1, intOFF, scalefac);

 		samp_max_hel0_laserON->Rebin(intrebinnum);
 		samp_max_hel1_laserON->Rebin(intrebinnum);
 		samp_max_laserOFF->Rebin(intrebinnum);

 		c_samples_asymmetry->cd(4);
		TH2F *samp_max_hel0_rebin = (TH2F*)samp_max_hel0->Clone("samp_max_hel0_rebin");
 		samp_max_hel0_rebin->RebinX(intrebinnum);
 		TH1F* samp_max_hel0_rebin_sub = drawscalesubbytypeTH1(samp_max_hel0_rebin, 1, 2, (maxmin+1)/intrebinnum, (maxmax+1)/intrebinnum, 1);
 		c_samples_asymmetry->cd(5);
		TH2F *samp_max_hel1_rebin = (TH2F*)samp_max_hel1->Clone("samp_max_hel1_rebin");
 		samp_max_hel1_rebin->RebinX(intrebinnum);
 		TH1F* samp_max_hel1_rebin_sub = drawscalesubbytypeTH1(samp_max_hel1_rebin, 1, 2, (maxmin+1)/intrebinnum, (maxmax+1)/intrebinnum, 1);



// 		c_samples_asymmetry->cd(4);
// 		samp_max_hel0_laserON->DrawCopy();
// 		c_samples_asymmetry->cd(5);
// 		samp_max_hel1_laserON->DrawCopy();
// 		c_samples_asymmetry->cd(6);
// 		samp_max_laserOFF->DrawCopy();
//  		TH1F* samp_max_hel0_laserON_bksub = samp_max_hel0_laserON->Clone("samp_max_hel0_laserON_bksub");
//  		TH1F* samp_max_hel1_laserON_bksub = samp_max_hel1_laserON->Clone("samp_max_hel1_laserON_bksub");
//  		samp_max_laserOFF->Scale(scalefac);
//  		samp_max_hel0_laserON_bksub->Add(samp_max_laserOFF,-1);
//  		samp_max_hel1_laserON_bksub->Add(samp_max_laserOFF,-1);

		c_samples_asymmetry->cd(7);
		samp_max_hel0_rebin_sub->DrawCopy("e");
		c_samples_asymmetry->cd(8);
		samp_max_hel1_rebin_sub->DrawCopy("e");
		c_samples_asymmetry->cd(9);
 		TH1F *asym_bksub = samp_max_hel1_rebin_sub->GetAsymmetry(samp_max_hel0_rebin_sub);
 		asym_bksub->SetMaximum(min(asym_bksub->GetMaximum(),0.06));
 		asym_bksub->SetMinimum(max(asym_bksub->GetMinimum(),-0.06));
 		asym_bksub->SetTitle("Asymmetry (background subtracted)");
		asym_bksub->SetLineColor(kRed);
 		asym_bksub->Draw();

		samples_asymmetry->Print(Form("%s/run_%i/samples_asymmetry_%i.png",rundir,runnumber,runnumber));
	}
}



/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
 
