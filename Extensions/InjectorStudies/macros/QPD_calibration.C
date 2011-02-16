
void QPD_calibration(Int_t runnum=1, TString direction="0", TString title="BPM differences", 
					 TString usercut="1", TString rootfilesdir="~/users/buddhini/rootfiles") {
	if (runnum==1 || direction=="0") {
		printf("\nUsage:\n\t");
		printf(".x QPD_calibration(runnum, direction, [title], [usercut], [rootfilesdir])\n\n");
		printf("runnum\t\t is the run number.\n");
		printf("direction\t\t is the scan direction.\n");
		printf("[title]\t\t= \"BPM differences\"\tis the title of the plot.\n");
		printf("[usercut]\t= 1\t\t\tis an additional root cut to be passed to the fitting and plotting routines.\n");
		printf("[rootfilesdir]\t= 1\t\t\tis the directory where the root files are.\n\n");
		return;
	}

	gROOT->Reset();
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(kFALSE);
 	gStyle->SetOptFit(1);
// 	gStyle->SetStatW(0.3);
// 	gStyle->SetStatH(0.2);
	gStyle->SetTitleSize(0.06,"xy");
	gStyle->SetTitleSize(0.08,"h");
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetPadTopMargin(0.10);
	gStyle->SetPadBottomMargin(0.15);
	gStyle->SetLabelSize(0.055, "xy");
	gStyle->SetTitleOffset(1.1, "X");
	gStyle->SetTitleOffset(1.3, "Y");

	TString plotdir = "plots";
	TString outputdir = "output";

	char scandataname[255];
	sprintf(scandataname,"scandata1");

	// Open the file and load trees
	char filename[255];
	sprintf(filename,"%s/Qweak_%i.000.root",rootfilesdir.Data(),runnum);
	TFile *_file = TFile::Open(filename);
	TTree *tree = (TTree*)gROOT->FindObject("Mps_Tree");
	TString cut = "cleandata&&(" + usercut + ")";

	char outfilename[255];
	sprintf(outfilename,"%s/QPD_transscan_run%d.txt",outputdir.Data(),runnum);
	printf("Writing output to %s\n",outfilename);
	FILE *outfile = fopen(outfilename, "w"); 

	char histname[255], varname[255], plotcom[255];
	TH1F *padhistos[4];
	TString padlist[4]={"TR","TL","BR","BL"};
	Double_t pad_slope[4], pad_slope_err[4];

	// First draw and fit the individual pads
	TCanvas *padcanvas = new TCanvas("padcanvas","QPD pad signals",0,0,800,600);
	padcanvas->Divide(2,2,0.0001,0.0001);
	for (Int_t i=1; i<=4; i++) {
		padcanvas->cd(i);
		sprintf(histname, "hist%s",padlist[i-1].Data());
		sprintf(varname,  "qwk_qpd%s.hw_sum",padlist[i-1].Data());
		sprintf(plotcom,  "%s:%s>>%s",varname,scandataname,histname);
		printf("%s\t%s\t%s\n",histname,varname,plotcom);
		Int_t successes = tree->Draw(plotcom,cut,"prof");
		if (successes<=0) {
			printf("No events pass cuts\n");
			return;
		}
		padhistos[i-1] = (TH1F*)gPad->GetPrimitive(histname);
		padhistos[i-1]->Fit("pol1");
		TF1 *fline = padhistos[i-1]->GetFunction("pol1");
		padhistos[i-1]->SetMarkerStyle(20);
		pad_slope[i-1] = fline->GetParameter(1);
		pad_slope_err[i-1]   = fline->GetParError(1);
		padhistos[i-1]->SetTitle(Form("QPD pad %s;QPD position %s  (mils);Amplitude",
									  padlist[i-1].Data(),direction.Data()));
		padhistos[i-1]->GetYaxis()->CenterTitle();
		padhistos[i-1]->GetXaxis()->CenterTitle();
		fprintf(outfile,"pad %s  d/d%s:  %8.3f +- %5.3f  (ADC channels/mil)\n", padlist[i-1].Data(),direction.Data(),
				pad_slope[i-1],pad_slope_err[i-1]);
	}
	padcanvas->Print(Form("%s/QPD_transcan_run%i_pads.png",plotdir.Data(),runnum));	
	//fclose(outfile);

	TH1F *poshistos[4];
	TString poslist[4]={"X","Y","RelX","RelY"};
	Double_t pos_slope[4], pos_slope_err[4];
	// Now draw and fit the positions
	TCanvas *poscanvas = new TCanvas("poscanvas","QPD positions",0,0,800,600);
	poscanvas->Divide(2,2,0.0001,0.0001);
	for (Int_t i=1; i<=4; i++) {
		poscanvas->cd(i);
		sprintf(histname, "hist%s",poslist[i-1].Data());
		sprintf(varname,  "qwk_qpd%s.hw_sum",poslist[i-1].Data());
		sprintf(plotcom,  "%s:%s>>%s",varname,scandataname,histname);
		printf("%s\t%s\t%s\n",histname,varname,plotcom);
		Int_t successes = tree->Draw(plotcom,cut,"prof");
		if (successes<=0) {
			printf("No events pass cuts\n");
			return;
		}
		poshistos[i-1] = (TH1F*)gPad->GetPrimitive(histname);
		poshistos[i-1]->Fit("pol1");
		TF1 *fline = poshistos[i-1]->GetFunction("pol1");
		poshistos[i-1]->SetMarkerStyle(20);
		pos_slope[i-1] = fline->GetParameter(1);
		pos_slope_err[i-1]   = fline->GetParError(1);
		poshistos[i-1]->SetTitle(Form("QPD pos %s;QPD position %s  (mils);Amplitude",
									  poslist[i-1].Data(),direction.Data()));
		poshistos[i-1]->GetYaxis()->CenterTitle();
		poshistos[i-1]->GetXaxis()->CenterTitle();
		fprintf(outfile,"pos %4s  d/d%s:  %8.3f +- %5.3f  (distance/mil)\n", poslist[i-1].Data(),direction.Data(),
				pos_slope[i-1],pos_slope_err[i-1]);
	}
	poscanvas->Print(Form("%s/QPD_transcan_run%i_pos.png",plotdir.Data(),runnum));	
	fclose(outfile);
}



/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
