// This macro compares two RHWP scans, with PITA offset 0 and PITA offset != 0
// and based on the fit functions, plots the PITA slope as a function of RHWP
// angle for asym and position differences. Based on these slopes, it also
// plots the amount of voltage necessary to zero out the charge asym, and the 
// corresponding position differences at these voltages. 
//
// Mark Dalton, Kent Paschke -- for PREX injector studies

void RHWP_optimise(Int_t runnum1=1, Int_t runnum2=1, Int_t PITA=1, TString device="1i02", TString usercut ="1") 
{
	if (runnum1==1 || runnum2==1 || PITA==1) {
		printf("Usage:\n\t.x RHWPscan.C(runnum1, runnum2, PITAoffset, [device], [cut])\n\n");
		return;
	}  
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(kFALSE);
    gStyle->SetTitleBorderSize(0);

	Float_t titlesize=0.06;

	TString dettype = "qwk_"+device;

	// Open file 1
	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/qwinjector_%i.000.root",runnum1);
	TFile *_file1 = TFile::Open(filename);
	TTree *p1 = (TTree*)gROOT->FindObject("Hel_Tree");

	if(!_file1) exit(1);


    TCanvas *RHWPcanvas = new TCanvas("RHWPcanvas","Optimisation",40,0,1200,940);
    RHWPcanvas->Divide(4,3,0.0001,0.0001);   
    // plot A_Q

    TString tmpname;
    TString plotcommand, plotcommand2;

    RHWPcanvas->cd(1);
	plotcommand = "asym_";
	plotcommand += dettype;
	plotcommand += "_EffectiveCharge.hw_sum";
	plotcommand += "*1000000";
    plotcommand += ":scandata1/50.0>>hAq1";

    TString cut = Form("ErrorFlag == 0 && asym_%s_EffectiveCharge.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

    p1->Draw(plotcommand.Data(),cut.Data(),"prof");
    hAq1= (TH1F*)gPad->GetPrimitive("hAq1");
    char title[256];
    sprintf(title,"Charge Asymmetry, PITA=0, %s, run %i",dettype.Data(),runnum1);
    hAq1->SetTitle(title);
    hAq1->SetBarWidth(0.);
    hAq1->SetTitleSize(12);
    hAq1->SetMarkerStyle(7);
    hAq1->SetMarkerColor(kBlue);
    hAq1->SetLineColor(kBlue);
    TF1 *fAq1 = new TF1("fAq1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
    fAq1->SetLineColor(2);
    fAq1->SetLineWidth(1);
    hAq1->Fit("fAq1");
    hAq1->DrawCopy("p");
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    Int_t nbins = hAq1->GetNbinsX();
    Float_t xmin =  hAq1->GetBinLowEdge(1);
    Float_t xmax =  hAq1->GetBinLowEdge(nbins+1);
    printf("%i  %f  %f\n",nbins,xmin,xmax);

    RHWPcanvas->cd(5);
    plotcommand = "diff_";
    plotcommand += dettype;
    plotcommand += "X.hw_sum";
    plotcommand += "*1000:scandata1/50.0>>hDx1";

	cut = Form("ErrorFlag == 0 && diff_%sX.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

    p1->Draw(plotcommand.Data(),cut.Data(),"prof");
    hDx1= (TH1F*)gPad->GetPrimitive("hDx1");
    sprintf(title,"X Pos Difference, PITA=0, run %i",runnum1);
    hDx1->SetTitle(title);
    hDx1->SetMarkerStyle(7);
    hDx1->SetMarkerColor(kBlue);
    hDx1->SetLineColor(kBlue);
    TF1 *fDx1 = new TF1("fDx1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
    fDx1->SetLineColor(2);
    fDx1->SetLineWidth(1);
    hDx1->Fit("fDx1");
    hDx1->DrawCopy("p");
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(9);
    plotcommand = "diff_";
    plotcommand += dettype;
    plotcommand += "Y.hw_sum";
    plotcommand += "*1000:scandata1/50.0>>hDy1";

	cut = Form("ErrorFlag == 0 && diff_%sY.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());

    p1->Draw(plotcommand.Data(),cut.Data(),"prof");
    hDy1= (TH1F*)gPad->GetPrimitive("hDy1");
    sprintf(title,"Y Pos Difference, PITA=0, run %i",runnum1);
    hDy1->SetTitle(title);
    hDy1->SetMarkerStyle(7);
    hDy1->SetMarkerColor(kBlue);
    hDy1->SetLineColor(kBlue);
    TF1 *fDy1 = new TF1("fDy1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
    fDy1->SetLineColor(2);
    fDy1->SetLineWidth(1);
    hDy1->Fit("fDy1");
    hDy1->DrawCopy("p");
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

	
	sprintf(filename,"$QW_ROOTFILES/qwinjector_%i.000.root",runnum2);
	TFile *_file2 = TFile::Open(filename);
	TTree *p2 = (TTree*)gROOT->FindObject("Hel_Tree");

    RHWPcanvas->cd(2);
	plotcommand = "asym_";
	plotcommand += dettype;
	plotcommand += "_EffectiveCharge.hw_sum";
	plotcommand += "*1000000";
    plotcommand += ":scandata1/50.0>>hAq2";

	cut = Form("ErrorFlag == 0 && asym_%s_EffectiveCharge.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());
    //cout << plotcommand << endl;
    p2->Draw(plotcommand.Data(),cut.Data(),"prof");
    hAq2= (TH1F*)gPad->GetPrimitive("hAq2");
    sprintf(title,"Charge Asymmetry, PITA=%i, run %i",PITA,runnum2);
    hAq2->SetTitle(title);
    hAq2->SetMarkerStyle(7);
    hAq2->SetMarkerColor(kBlue);
    hAq2->SetLineColor(kBlue);
    TF1 *fAq2 = new TF1("fAq2","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
    fAq2->SetLineColor(2);
    fAq2->SetLineWidth(1);
    hAq2->Fit("fAq2");
    hAq2->DrawCopy("p");
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(6);
    plotcommand = "diff_";
    plotcommand += dettype;
    plotcommand += "X.hw_sum";
    plotcommand += "*1000:scandata1/50.0>>hDx2";
	cut = Form("ErrorFlag == 0 && diff_%sX.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());
    p2->Draw(plotcommand.Data(),cut.Data(),"prof");
    hDx2= (TH1F*)gPad->GetPrimitive("hDx2");
    sprintf(title,"X Pos Diff, PITA=%i, run %i",PITA,runnum2);
    hDx2->SetTitle(title);
    hDx2->SetMarkerStyle(7);
    hDx2->SetMarkerColor(kBlue);
    hDx2->SetLineColor(kBlue);
    TF1 *fDx2 = new TF1("fDx2","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
    fDx2->SetLineColor(2);
    fDx2->SetLineWidth(1);
    hDx2->Fit("fDx2");
    hDx2->DrawCopy("p");
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(10);
    plotcommand = "diff_";
    plotcommand += dettype;
    plotcommand += "Y.hw_sum";
    plotcommand += "*1000:scandata1/50.0>>hDy2";
	cut = Form("ErrorFlag == 0 && diff_%sY.Device_Error_Code == 0 && cleandata && %s",dettype.Data(),usercut.Data());
	
    p2->Draw(plotcommand.Data(),cut.Data(),"prof");
    hDy2= (TH1F*)gPad->GetPrimitive("hDy2");
    sprintf(title,"Y Pos Diff, PITA=%i, run %i",PITA,runnum2);
    hDy2->SetTitle(title);
    hDy2->SetMarkerStyle(7);
    hDy2->SetMarkerColor(kBlue);
    hDy2->SetLineColor(kBlue);
    TF1 *fDy2 = new TF1("fDy2","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
    fDy2->SetLineColor(2);
    fDy2->SetLineWidth(1);
    hDy2->Fit("fDy2");
    hDy2->DrawCopy("p");
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(3);
    TF1 *fpitaslope = new TF1("fpitaslope","(fAq2-fAq1)/[0]",0,180.0);
    //Double_t dPITA = PITA*1.0;
    fpitaslope->SetParameter(0,PITA);
    fpitaslope->SetTitle("Charge Asym slope");
    fpitaslope->SetLineWidth(1);
    fpitaslope->DrawCopy();
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(7);
    TF1 *fpitaslopex = new TF1("fpitaslopex","(fDx2-fDx1)/[0]",0,180.0);
    fpitaslopex->SetParameter(0,PITA);
    fpitaslopex->SetTitle("X poss diff slope");
    fpitaslopex->SetLineWidth(1);
    fpitaslopex->DrawCopy();
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(11);
    TF1 *fpitaslopey = new TF1("fpitaslopey","(fDy2-fDy1)/[0]",0,180.0);
    fpitaslopey->SetParameter(0,PITA);
    fpitaslopey->SetTitle("Y poss diff slope");
    fpitaslopey->SetLineWidth(1);
    fpitaslopey->DrawCopy();
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

    RHWPcanvas->cd(4);
    TF1 *fcorrection = new TF1("fcorrection","-(fAq1/fpitaslope)",0,180.0);
    fcorrection->SetTitle("Charge asym zero correction");
    fcorrection->SetLineWidth(1);
    fcorrection->SetNpx(10000);
    fcorrection->SetMaximum(200);
    fcorrection->SetMinimum(-200);
    fcorrection->DrawCopy();
    TLine *t1 = new TLine(0,0.0,180,0.0);
    t1->Draw();

//     RHWPcanvas->cd(8);
//     TF1 *fresultx = new TF1("resultx","fDx1+fcorrection*fpitaslopex",0,180.0);
//     fresultx->SetTitle("X poss diff (Result of charge asym corr)");
//     fresultx->SetLineWidth(1);
//     fresultx->SetNpx(10000);
//     fresultx->SetMaximum(0.9);
//     fresultx->SetMinimum(-0.9);
//     fresultx->DrawCopy();
//     TLine *t1 = new TLine(0,0.0,180,0.0);
//     t1->Draw();

//     RHWPcanvas->cd(12);
//     TF1 *fresulty = new TF1("resulty","fDy1+fcorrection*fpitaslopey",0,180.0);
//     fresulty->SetTitle("Y poss diff (Result of charge asym corr)");
//     fresulty->SetLineWidth(1);
//     fresulty->SetNpx(10000);
//     fresulty->SetMaximum(0.9);
//     fresulty->SetMinimum(-0.9);
//     fresulty->DrawCopy();
//     TLine *t1 = new TLine(0,0.0,180,0.0);
//     t1->Draw();

	RHWPcanvas->cd(8);
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


	RHWPcanvas->cd(12);
	TF1 *fresulty = new TF1("resulty","fDy1+fcorrection*fpitaslopey",0,180.0);
	sprintf(title,"BPM %s, Y poss diff",device.Data());
	fresulty->SetTitle(title);
// 		fresulty->SetLineWidth(1);
	fresulty->SetNpx(10000);
// 		fresulty->SetMaximum(0.9);
// 		fresulty->SetMinimum(-0.9);
// 		fresulty->DrawCopy();

	TGraph *gresulty = new TGraph(fresulty);
	gresulty->GetHistogram()->SetTitleSize(titlesize);
	gresulty->SetLineWidth(1);
	gresulty->SetLineColor(2);
	gresulty->SetMaximum(0.9);
	gresulty->SetMinimum(-0.9);
	gresulty->Draw("al");
	TLine *t1 = new TLine(0,0.0,180,0.0);
	t1->Draw();

    TString psnam = "plots/RHWP_optimize_";
    psnam += runnum1;
    psnam += "_";
    psnam += runnum2;
    psnam += "_";
    psnam += device;
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
