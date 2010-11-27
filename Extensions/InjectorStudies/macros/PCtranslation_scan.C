// 2010-07-01 Mark Dalton 
// Initial code taken from tools used in Hall A parity programs.
// 

void plot_element(TPad *p1, TPad* p2, char *devnam, TString lcut, Int_t dset);

void PCtranslation_scan(Int_t runnum=1, TString direction="0", Int_t decimals=0, TString device="1i02", TString usercut ="1") {
	if (runnum==1 || direction=="0") {
		printf("Usage:\n\t.x PCtranslation_scan.C(runnum,direction,[numberdecimals],[device],[cut])\n\n");
		printf("The scandata will be multiplied by 10^[numdecimals]\n");
		return;
	}
	gROOT->Reset();

	TString plotdir = "plots";
	TString outputdir = "output";

    TString gtitle;
    gtitle = "PC translation scan ";
	gtitle += direction;
	gtitle += ", Run ";
    gtitle += runnum;

// 	// define style here 
// 	// general parameters
// 	gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
// 	gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
// 	gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
  
// 	// canvas parameters
// 	gStyle->SetFrameBorderMode(0);
// 	gStyle->SetFrameBorderSize(0);
// 	gStyle->SetFrameFillColor(10);
// 	// pads parameters
// 	//  gStyle->SetPadColor(39); 
// 	gStyle->SetPadColor(0); 
// 	gStyle->SetPadBorderMode(0);
// 	gStyle->SetPadBorderSize(0);
// 	gStyle->SetPadBottomMargin(0.18);
// 	gStyle->SetPadRightMargin(0.05);
// 	gStyle->SetPadLeftMargin(0.15);
// 	gStyle->SetLabelSize(0.05,"x");
// 	gStyle->SetLabelSize(0.05,"y");
// 	gROOT->ForceStyle();  
	gROOT->SetStyle("Plain");

	// Open the file
	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum);
	TFile *_file0 = TFile::Open(filename);
	TTree *p = (TTree*)gROOT->FindObject("Hel_Tree");
	TTree *r = (TTree*)gROOT->FindObject("Mps_Tree");
    TString cut = "cleandata&&" + usercut;

	Double_t correction=pow(10,decimals);
	printf("correction is %.0f\n",correction);

	const Int_t numbpms=6;
	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05"};
	Double_t Dx1_const[numbpms],Dx2_const[numbpms],Dx3_const[numbpms],Dx4_const[numbpms];
	Double_t Dy1_const[numbpms],Dy2_const[numbpms],Dy3_const[numbpms],Dy4_const[numbpms];
	Double_t Aq1_const[numbpms],Aq2_const[numbpms],Aq3_const[numbpms],Aq4_const[numbpms];
	Double_t Dx1_err[numbpms],Dx2_err[numbpms],Dx3_err[numbpms],Dx4_err[numbpms];
	Double_t Dy1_err[numbpms],Dy2_err[numbpms],Dy3_err[numbpms],Dy4_err[numbpms];
	Double_t Aq1_err[numbpms],Aq2_err[numbpms],Aq3_err[numbpms],Aq4_err[numbpms];

	char outfilename[255];
	sprintf(outfilename,"%s/PCtransscan_run%d.txt",outputdir.Data(),runnum);
	printf("Writing output to %s\n",outfilename);
	FILE *outfile = fopen(outfilename, "w"); 

	TCanvas *a1 = new TCanvas("a1", "Scan", 0,0,700,700);

	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		TString device = bpmlist[bpmcount-1];
		plotcommand = "diff_qwk_"+device+"X.hw_sum*1000*";
		plotcommand += Form("%.0f",correction);
		plotcommand += ":scandata1>>histo";
		cout << plotcommand << endl;
		Int_t successes = p->Draw(plotcommand.Data(),cut.Data(),"prof,goff");
		if (successes<=0) {
			printf("No events pass cuts\n");
			return;
		}
		TH1 *histo = (TH1*)gDirectory->Get("histo");
		histo->Fit("pol1");
		TF1 *fDx1 = histo->GetFunction("pol1");
		Dx1_const[bpmcount-1]=fDx1->GetParameter(1);
		Dx1_err[bpmcount-1]=fDx1->GetParError(1);
		plotcommand = "diff_qwk_"+device+"Y.hw_sum*1000*";
		plotcommand += correction;
		plotcommand += ":scandata1>>histo";
		cout << plotcommand << endl;
		Int_t successes = p->Draw(plotcommand.Data(),cut.Data(),"prof,goff");
		if (successes<=0) {
			printf("No events pass cuts\n");
			return;
		}
		TH1 *histo = (TH1*)gDirectory->Get("histo");
		histo->Fit("pol1");
		TF1 *fDy1 = histo->GetFunction("pol1");
		Dy1_const[bpmcount-1]=fDy1->GetParameter(1);
		Dy1_err[bpmcount-1]=fDy1->GetParError(1);

		fprintf(outfile,"%6s  d(Dx)/d%s:  %8.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dx1_const[bpmcount-1],Dx1_err[bpmcount-1]);
		fprintf(outfile,"%6s  d(Dy)/d%s:  %8.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dy1_const[bpmcount-1],Dy1_err[bpmcount-1]);
  

// 	a1->Draw();
// 	a1->Clear();
// 	a1->Update();;

	TPad* a1_p[7];

	TPad* a1_p[0]  = new TPad( "a1_1", "a1_1",0.00,0.61,0.75,0.92);    
	TPad* a1_p[1]  = new TPad( "a1_7", "a1_7",0.75,0.61,1.00,0.92);    

	TPad* a1_p[2]  = new TPad( "a1_1", "a1_1",0.00,0.31,0.75,0.60);    
	TPad* a1_p[3]  = new TPad( "a1_7", "a1_7",0.75,0.31,1.00,0.60);    

	TPad* a1_p[4]  = new TPad( "a1_1", "a1_1",0.00,0.01,0.75,0.30);    
	TPad* a1_p[5]  = new TPad( "a1_7", "a1_7",0.75,0.01,1.00,0.30);    

	TPad* a1_title = new TPad("a1_title","a1_title",0.0,0.92,1.0,1.0);    

	a1->cd();
	for (Int_t i =0; i<6; i++) a1_p[i]->Draw();
	a1_title->Draw();

	a1_title->cd();
	TPaveText* pt2 = new TPaveText(0.04,0.04,0.96,0.50,"brNDC");
	pt2->SetBorderSize(0);pt2->SetFillColor(10);
	pt2->SetTextFont(20);
	pt2->SetTextSize(0.55);
	pt2->SetTextAlign(22);
	text = pt2->AddText(0.5,0.9,gtitle.Data());
	pt2->Draw();
	pt2->SetTextAlign(22);

	plot_element(a1_p[0],a1_p[1],"qwk_"+device+"_EffectiveCharge.hw_sum",correction,usercut,runnum);
	plot_element(a1_p[2],a1_p[3],"qwk_"+device+"X.hw_sum",correction,usercut,runnum);
	plot_element(a1_p[4],a1_p[5],"qwk_"+device+"Y.hw_sum",correction,usercut,runnum);

	a1->cd();
	a1->Update();

    TString plotname = plotdir;
    plotname += "/pctranscan_run";
//    plotname += runnum;
    plotname += Form("%i_%s.png",runnum,device.Data());
//	plotname += ".png";
	a1->Print(plotname.Data());
	printf("\n    \n");

	}
	fclose(outfile);
	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		printf("%6s  d(Dx)/d%s:  %8.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dx1_const[bpmcount-1],Dx1_err[bpmcount-1]);
		printf("%6s  d(Dy)/d%s:  %8.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dy1_const[bpmcount-1],Dy1_err[bpmcount-1]);
	}  
}

void plot_element(TPad *p1, TPad* p2, char *devnam, Double_t correction, TString localcut, Int_t runnum) {

	TString *bpmNam = new TString(devnam);

	Int_t ifnd = 0;
	TString tmpname;
	TString plotcommand;

	Int_t icurRun=0;
	char buff[170];

    // make cut
    TString cut = "cleandata&&" + localcut;

	// Open the file
	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum);
	TFile *_file0 = TFile::Open(filename);
	TTree *p = (TTree*)gROOT->FindObject("Hel_Tree");
	TTree *r = (TTree*)gROOT->FindObject("Mps_Tree");

    // plot desired  Aq
    tmpname = bpmNam->Data();
    TString titpre = "asym_";
    TString titsum = " (ppm) vs Scandata1";
    //    tmpname += "1sum";
    if (tmpname.Contains("X") || tmpname.Contains("Y")) {
		plotcommand = "diff_";
		plotcommand += tmpname.Data();
		plotcommand += "*1000";
		titpre = "diff_";
		titsum = " (nm) vs Scandata1";
    } else {
		if (tmpname.Contains("EffectiveCharge")) {
			plotcommand = "asym_";
			plotcommand += tmpname.Data();
			plotcommand += "*1000000";
		} else {
			printf("problems\n");
		}
	}

	plotcommand += ":scandata1>>hAq";
	cout << plotcommand << endl;
	p1->cd();
	
	Int_t successes = p->Draw(plotcommand.Data(),cut.Data(),"prof");
	if (successes<=0) {
		printf("No events pass cuts\n");
		return;
	}
//	p->Project("hAq",plotcommand.Data(),cut.Data(),"prof");
	// fit Aq linearity
	TH1 *hAq = (TH1*)gDirectory->Get("hAq");
	hAq->Fit("pol1");
	TF1 *f1 = hAq->GetFunction("pol1");
	f1->SetLineColor(2);
	f1->SetLineWidth(1);

	hAq->SetMarkerColor(12);
	hAq->SetMarkerSize(1.0);
	hAq->SetMarkerStyle(22);
	hAq->GetXaxis()->CenterTitle();
	hAq->GetXaxis()->SetTitleOffset(1.2);
	hAq->GetXaxis()->SetTitleSize(0.06);
	hAq->Draw();
	TString tit = titpre.Data();
	tit += tmpname.Data();
	tit += titsum;
	hAq->GetXaxis()->SetTitle(tit.Data());

	p2->cd();
	char linetxt[50];
	pt = new TPaveText(0.0,0.3,0.9,0.7,"brNDC");
	pt->SetBorderSize(1);pt->SetFillColor(10);
	pt->SetTextAlign(12);pt->SetTextFont(22);
    if (tmpname.Contains("X")) {
		sprintf(linetxt,"D_{x}(0) = %7.1f",f1->GetParameter(0));
		pt->AddText(linetxt);
		sprintf(linetxt,"slope = %7.4f", f1->GetParameter(1));
		pt->AddText(linetxt);
		//sprintf(linetxt,"zero at = %7.2f", f1->GetParameter(1));
		//pt->AddText(linetxt);
	} else {
		if (tmpname.Contains("Y")) {
			sprintf(linetxt,"D_{y}(0) = %7.2f",f1->GetParameter(0));
			pt->AddText(linetxt);
			sprintf(linetxt,"slope = %7.4f", f1->GetParameter(1));
			pt->AddText(linetxt);
			//sprintf(linetxt,"zero at = %7.4f", f1->GetParameter(1));
			//pt->AddText(linetxt);
		} else {
			if (tmpname.Contains("EffectiveCharge")) {
				sprintf(linetxt,"A_{q}(0) = %7.2f",f1->GetParameter(0));
				pt->AddText(linetxt);
				sprintf(linetxt,"slope = %7.2f", f1->GetParameter(1));
				pt->AddText(linetxt);
				//sprintf(linetxt,"zero at = %7.4f", f1->GetParameter(1));
				//pt->AddText(linetxt);
			} else {
				printf("problems\n");
			}
		}
	}
	pt->Draw();
}

/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
