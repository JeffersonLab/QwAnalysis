// This macro compares two RHWP scans, with PITA offset 0 and PITA offset != 0
// and based on the fit functions, plots the PITA slope as a function of RHWP
// angle for asym and position differences. Based on these slopes, it also
// plots the amount of voltage necessary to zero out the charge asym, and the 
// corresponding position differences at these voltages. 
//
// Mark Dalton -- for QWeak injector studies

void RHWP_optimise_multibpm(Int_t runnum1=1, Int_t runnum2=1, Int_t PITA=1, TString usercut ="1") 
{
	if (runnum1==1 || runnum2==1 || PITA==1) {
		printf("Usage:\n\t.x RHWP_optimise_multibpm.C(runnum1, runnum2, PITAoffset, [device], [cut])\n\n");
		return;
	}  
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(kFALSE);
    gStyle->SetTitleBorderSize(0);
	char fileprefix[255];
	sprintf(fileprefix,"$QW_ROOTFILES/qwinjector");

	// Open file 1
	char filename[255];
	sprintf(filename,"%s_%i.000.root",fileprefix,runnum1);
	printf("Opening %s\n",filename);
	TFile *_file1 = TFile::Open(filename);
	TTree *tree1 = (TTree*)gROOT->FindObject("Hel_Tree");
	sprintf(filename,"%s_%i.000.root",fileprefix,runnum2);
	printf("Opening %s\n",filename);
	TFile *_file2 = TFile::Open(filename);
	TTree *tree2 = (TTree*)gROOT->FindObject("Hel_Tree");

	if(!_file1 || !_file2) exit(1);



	TCanvas *RHWPcanvas = new TCanvas("RHWPcanvas","Optimisation",40,80,1200,800);
	RHWPcanvas->Divide(3,5,0.0001,0.0001);   

// 	const Int_t numbpms=4;
// 	char bpmlist[numbpms][30]={"1i02","1i04","0i02","0l01"};

// 	const Int_t numbpms=16;
// 	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05",
// 						"0i07","0l02","0l03","0l04","0l05","0l06","0l07","0l08",
// 						"0l09","0l10"};

	const Int_t numbpms=6;
	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05"};
	TH1F *histresulty[numbpms], *histresultx[numbpms];

	Float_t titlesize=0.06;

	char title[255];

	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {

		TString device = bpmlist[bpmcount-1];
		TString dettype = "qwk_" + TString(device);

		// plot A_Q

		TString tmpname;
		TString plotcommand, plotcommand2;

		RHWPcanvas->cd(bpmcount);
		plotcommand = "asym_";
		plotcommand += dettype;
		plotcommand += "_EffectiveCharge.hw_sum";
		plotcommand += "*1000000";
		plotcommand += ":scandata1/50.0>>hAq1";
		TString cut = Form("ErrorFlag == 0 && asym_%s_EffectiveCharge.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());


		tree1->Draw(plotcommand.Data(),cut.Data(),"prof");
		hAq1= (TH1F*)gPad->GetPrimitive("hAq1");
		TF1 *fAq1 = new TF1("fAq1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		hAq1->Fit("fAq1");

		char title[256];
		Int_t nbins = hAq1->GetNbinsX();
		Float_t xmin =  hAq1->GetBinLowEdge(1);
		Float_t xmax =  hAq1->GetBinLowEdge(nbins+1);
		printf("nbins %i  xmin %f  xmax %f\n",nbins,xmin,xmax);

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDx1";

		cut = Form("ErrorFlag == 0 && diff_%sX.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

		tree1->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDx1= (TH1F*)gPad->GetPrimitive("hDx1");
		TF1 *fDx1 = new TF1("fDx1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		hDx1->Fit("fDx1");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDy1";

		cut = Form("ErrorFlag == 0 && diff_%sY.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());
		
		tree1->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDy1= (TH1F*)gPad->GetPrimitive("hDy1");
		TF1 *fDy1 = new TF1("fDy1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		hDy1->Fit("fDy1");

		plotcommand = "asym_";
		plotcommand += dettype;
		plotcommand += "_EffectiveCharge.hw_sum";
		plotcommand += "*1000000";
		plotcommand += ":scandata1/50.0>>hAq2";
		cut = Form("ErrorFlag == 0 && asym_%s_EffectiveCharge.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

		tree2->Draw(plotcommand.Data(),cut.Data(),"prof");

		TF1 *fAq2 = new TF1("fAq2","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		hAq2->Fit("fAq2");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDx2";
		cut = Form("ErrorFlag == 0 && diff_%sX.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

		tree2->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDx2= (TH1F*)gPad->GetPrimitive("hDx2");
		TF1 *fDx2 = new TF1("fDx2","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		hDx2->Fit("fDx2");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDy2";
		cut = Form("ErrorFlag == 0 && diff_%sY.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

		tree2->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDy2= (TH1F*)gPad->GetPrimitive("hDy2");
		TF1 *fDy2 = new TF1("fDy2","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		hDy2->Fit("fDy2");

		TF1 *fpitaslope = new TF1("fpitaslope","(fAq2-fAq1)/[0]",0,180.0);
		//Double_t dPITA = PITA*1.0;
		fpitaslope->SetParameter(0,PITA);

		TF1 *fpitaslopex = new TF1("fpitaslopex","(fDx2-fDx1)/[0]",0,180.0);
		fpitaslopex->SetParameter(0,PITA);

		TF1 *fpitaslopey = new TF1("fpitaslopey","(fDy2-fDy1)/[0]",0,180.0);
		fpitaslopey->SetParameter(0,PITA);

		TF1 *fcorrection = new TF1("fcorrection","-(fAq1/fpitaslope)",0,180.0);

		//RHWPcanvas->cd(bpmcount);
		gPad->Clear();
		TF1 *fresultx = new TF1("resultx","fDx1+fcorrection*fpitaslopex",0,180.0);
		sprintf(title,"BPM %s, X poss diff",device.Data());
		fresultx->SetTitle(title);
// 		fresultx->SetLineWidth(1);
 		fresultx->SetNpx(10000);
// 		fresultx->SetMaximum(0.9);
// 		fresultx->SetMinimum(-0.9);
// 		fresultx->DrawCopy();
// 		TLine *t1 = new TLine(0,0.0,180,0.0);
// 		t1->Draw();
		histresultx[bpmcount-1] = (TH1F*)(fresultx->GetHistogram());

		// Make TGraph
		TGraph *gresultx = new TGraph(fresultx);
		gresultx->GetHistogram()->SetTitleSize(titlesize);
		gresultx->SetLineWidth(1);
		gresultx->SetLineColor(2);
		gresultx->SetMaximum(0.9);
		gresultx->SetMinimum(-0.9);
		gresultx->Draw("al");
		TLine *t1 = new TLine(0,0.0,180,0.0);
		t1->Draw();


		RHWPcanvas->cd(numbpms+bpmcount);
		TF1 *fresulty = new TF1("resulty","fDy1+fcorrection*fpitaslopey",0,180.0);
		sprintf(title,"BPM %s, Y poss diff",device.Data());
		fresulty->SetTitle(title);
// 		fresulty->SetLineWidth(1);
 		fresulty->SetNpx(10000);
// 		fresulty->SetMaximum(0.9);
// 		fresulty->SetMinimum(-0.9);
// 		fresulty->DrawCopy();
		histresulty[bpmcount-1] = (TH1F*)(fresulty->GetHistogram());

		TGraph *gresulty = new TGraph(fresulty);
		gresulty->GetHistogram()->SetTitleSize(titlesize);
		gresulty->SetLineWidth(1);
		gresulty->SetLineColor(2);
		gresulty->SetMaximum(0.9);
		gresulty->SetMinimum(-0.9);
		gresulty->Draw("al");
		TLine *t1 = new TLine(0,0.0,180,0.0);
		t1->Draw();

	}
// 	TH1F *quadsumx = new TH1F((TH1F*)histresultx[0]);
// 	TH1F *quadsumy = new TH1F((TH1F*)histresulty[0]);
// 	TH1F *quadsumtot = new TH1F((TH1F*)histresulty[0]);
	TH1F *quadsumx = histresulty[0]->Clone();
	quadsumx->SetTitle("Figure of merit X;angle   (degrees);HCPD quadrature sum");
	quadsumx->SetLineWidth(1);
	quadsumx->SetLineColor(kBlue);
	TH1F *quadsumy = histresulty[0]->Clone();
	quadsumy->SetTitle("Figure of merit Y;angle   (degrees);HCPD quadrature sum");
	quadsumy->SetLineWidth(1);
	quadsumy->SetLineColor(kBlue);
	TH1F *quadsumtot = histresulty[0]->Clone();
	quadsumtot->SetTitle("Figure of merit both;angle   (degrees);HCPD quadrature sum");
	quadsumtot->SetLineWidth(1);
	quadsumtot->SetLineColor(kBlue);
	Double_t xval, yval, xval2sum, yval2sum, totval;
	for (int i=1; i<=quadsumx->GetNbinsX(); i++) {
		xval2sum = yval2sum =0;
		for (Int_t bpmcount=0; bpmcount<numbpms; bpmcount++) {
			xval2sum = xval2sum + histresultx[bpmcount]->GetBinContent(i) * histresultx[bpmcount]->GetBinContent(i);
			yval2sum = yval2sum + histresulty[bpmcount]->GetBinContent(i) * histresulty[bpmcount]->GetBinContent(i);
		}
		totval = sqrt(xval2sum + yval2sum);
		xval = sqrt(xval2sum);
		yval = sqrt(yval2sum);
		quadsumx->SetBinContent(i,xval);
		quadsumy->SetBinContent(i,yval);
		quadsumtot->SetBinContent(i,totval);
	}

	RHWPcanvas->cd(2*bpmcount+1);
	quadsumx->SetMinimum(0);
	quadsumx->SetMaximum(1);
	quadsumx->Draw();
	RHWPcanvas->cd(2*bpmcount+2);
	quadsumy->SetMinimum(0);
	quadsumy->SetMaximum(1);
	quadsumy->Draw();
	RHWPcanvas->cd(2*bpmcount+3);
	quadsumtot->SetMinimum(0);
	quadsumtot->SetMaximum(1);
	quadsumtot->Draw();

	TString psnam = "output/RHWP_optimize_multibpm_";
	psnam += runnum1;
	psnam += "_";
	psnam += runnum2;
	psnam += ".png";
	//    psnam += ".gif";
	RHWPcanvas->Print(psnam.Data());
}


/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
