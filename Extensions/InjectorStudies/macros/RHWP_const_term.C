// This macro compares two RHWP scans, with PITA offset 0 and PITA offset != 0
// and based on the fit functions, plots the PITA slope as a function of RHWP
// angle for asym and position differences. Based on these slopes, it also
// plots the amount of voltage necessary to zero out the charge asym, and the 
// corresponding position differences at these voltages. 
//
// Mark Dalton, Kent Paschke -- for PREX injector studies

void RHWP_const_term(Int_t runnum1=1, Int_t runnum2=1, Int_t runnum3=1, Int_t runnum4=1, Int_t PITA=1, TString usercut ="1") 
{
	if (runnum1==1 || runnum2==1 || runnum3==1 || runnum4==1 || PITA==1) {
		printf("Usage:\n\t.x RHWPscan.C(runnum1, runnum2, runnum3, runnum4, PITAoffset, [cut])\n\n");
		printf("Where: runnum1 is IN\n       runnum2 is OUT\n       runnum2 is IN PITA offset\n       runnum2 is OUT PITA offset\n");

		printf("\nThis script will analyse the constant terms from four RHWP scans ans determine the");
		printf("size of overall correction required with PC translation.\n");
		return;
	}

	TString outputdir = "output";

    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(kFALSE);
    gStyle->SetTitleBorderSize(0);

	Float_t titlesize=0.06;

	// Open file 1
	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum1);
	TFile *_file1 = TFile::Open(filename);
	TTree *p1 = (TTree*)gROOT->FindObject("Hel_Tree");

    TString cut = "cleandata&&" + usercut;

//     TCanvas *RHWPcanvas = new TCanvas("RHWPcanvas","Optimisation",40,0,1200,940);
//     RHWPcanvas->Divide(4,3,0.0001,0.0001); 
	
    // plot A_Q

    TString tmpname;
    TString plotcommand, plotcommand2;

	const Int_t numbpms=6;
	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05"};
	Double_t Dx1_const[numbpms],Dx2_const[numbpms],Dx3_const[numbpms],Dx4_const[numbpms];
	Double_t Dy1_const[numbpms],Dy2_const[numbpms],Dy3_const[numbpms],Dy4_const[numbpms];
	Double_t Aq1_const[numbpms],Aq2_const[numbpms],Aq3_const[numbpms],Aq4_const[numbpms];
	Double_t Dx1_err[numbpms],Dx2_err[numbpms],Dx3_err[numbpms],Dx4_err[numbpms];
	Double_t Dy1_err[numbpms],Dy2_err[numbpms],Dy3_err[numbpms],Dy4_err[numbpms];
	Double_t Aq1_err[numbpms],Aq2_err[numbpms],Aq3_err[numbpms],Aq4_err[numbpms];

	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {

		TString device = bpmlist[bpmcount-1];
		TString dettype = "qwk_" + TString(device);

		plotcommand = "asym_";
		plotcommand += dettype;
		plotcommand += "_EffectiveCharge.hw_sum";
		plotcommand += "*1000000";
		plotcommand += ":scandata1/50.0>>hAq1";

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

		Int_t nbins = hAq1->GetNbinsX();
		Float_t xmin =  hAq1->GetBinLowEdge(1);
		Float_t xmax =  hAq1->GetBinLowEdge(nbins+1);
		printf("%i  %f  %f\n",nbins,xmin,xmax);

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDx1";

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

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDy1";
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
	
		sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum2);
		TFile *_file2 = TFile::Open(filename);
		TTree *p2 = (TTree*)gROOT->FindObject("Hel_Tree");

		plotcommand = "asym_";
		plotcommand += dettype;
		plotcommand += "_EffectiveCharge.hw_sum";
		plotcommand += "*1000000";
		plotcommand += ":scandata1/50.0>>hAq2";
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

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDx2";
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

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDy2";
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


		sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum3);
		TFile *_file3 = TFile::Open(filename);
		TTree *p3 = (TTree*)gROOT->FindObject("Hel_Tree");

		plotcommand = "asym_";
		plotcommand += dettype;
		plotcommand += "_EffectiveCharge.hw_sum";
		plotcommand += "*1000000";
		plotcommand += ":scandata1/50.0>>hAq3";
		//cout << plotcommand << endl;
		p3->Draw(plotcommand.Data(),cut.Data(),"prof");
		hAq3= (TH1F*)gPad->GetPrimitive("hAq3");
		sprintf(title,"Charge Asymmetry, PITA=%i, run %i",PITA,runnum3);
		hAq3->SetTitle(title);
		hAq3->SetMarkerStyle(7);
		hAq3->SetMarkerColor(kBlue);
		hAq3->SetLineColor(kBlue);
		TF1 *fAq3 = new TF1("fAq3","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		fAq3->SetLineColor(2);
		fAq3->SetLineWidth(1);
		hAq3->Fit("fAq3");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDx3";
		p3->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDx3= (TH1F*)gPad->GetPrimitive("hDx3");
		sprintf(title,"X Pos Diff, PITA=%i, run %i",PITA,runnum3);
		hDx3->SetTitle(title);
		hDx3->SetMarkerStyle(7);
		hDx3->SetMarkerColor(kBlue);
		hDx3->SetLineColor(kBlue);
		TF1 *fDx3 = new TF1("fDx3","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		fDx3->SetLineColor(2);
		fDx3->SetLineWidth(1);
		hDx3->Fit("fDx3");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDy3";
		p3->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDy3= (TH1F*)gPad->GetPrimitive("hDy3");
		sprintf(title,"Y Pos Diff, PITA=%i, run %i",PITA,runnum3);
		hDy3->SetTitle(title);
		hDy3->SetMarkerStyle(7);
		hDy3->SetMarkerColor(kBlue);
		hDy3->SetLineColor(kBlue);
		TF1 *fDy3 = new TF1("fDy3","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		fDy3->SetLineColor(2);
		fDy3->SetLineWidth(1);
		hDy3->Fit("fDy3");

		sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum4);
		TFile *_file4 = TFile::Open(filename);
		TTree *p4 = (TTree*)gROOT->FindObject("Hel_Tree");

		plotcommand = "asym_";
		plotcommand += dettype;
		plotcommand += "_EffectiveCharge.hw_sum";
		plotcommand += "*1000000";
		plotcommand += ":scandata1/50.0>>hAq4";
		//cout << plotcommand << endl;
		p4->Draw(plotcommand.Data(),cut.Data(),"prof");
		hAq4= (TH1F*)gPad->GetPrimitive("hAq4");
		sprintf(title,"Charge Asymmetry, PITA=%i, run %i",PITA,runnum4);
		hAq4->SetTitle(title);
		hAq4->SetMarkerStyle(7);
		hAq4->SetMarkerColor(kBlue);
		hAq4->SetLineColor(kBlue);
		TF1 *fAq4 = new TF1("fAq4","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		fAq4->SetLineColor(2);
		fAq4->SetLineWidth(1);
		hAq4->Fit("fAq4");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDx4";
		p4->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDx4= (TH1F*)gPad->GetPrimitive("hDx4");
		sprintf(title,"X Pos Diff, PITA=%i, run %i",PITA,runnum4);
		hDx4->SetTitle(title);
		hDx4->SetMarkerStyle(7);
		hDx4->SetMarkerColor(kBlue);
		hDx4->SetLineColor(kBlue);
		TF1 *fDx4 = new TF1("fDx4","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		fDx4->SetLineColor(2);
		fDx4->SetLineWidth(1);
		hDx4->Fit("fDx4");

		plotcommand = "diff_";
		plotcommand += dettype;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000:scandata1/50.0>>hDy4";
		p4->Draw(plotcommand.Data(),cut.Data(),"prof");
		hDy4= (TH1F*)gPad->GetPrimitive("hDy4");
		sprintf(title,"Y Pos Diff, PITA=%i, run %i",PITA,runnum4);
		hDy4->SetTitle(title);
		hDy4->SetMarkerStyle(7);
		hDy4->SetMarkerColor(kBlue);
		hDy4->SetLineColor(kBlue);
		TF1 *fDy4 = new TF1("fDy4","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0,180.0);
		fDy4->SetLineColor(2);
		fDy4->SetLineWidth(1);
		hDy4->Fit("fDy4");
		
		Aq1_const[bpmcount-1]=fAq1->GetParameter(0);
		Aq1_err[bpmcount-1]=fAq1->GetParError(0);
		Aq2_const[bpmcount-1]=fAq2->GetParameter(0);
		Aq2_err[bpmcount-1]=fAq2->GetParError(0);
		Aq3_const[bpmcount-1]=fAq3->GetParameter(0);
		Aq3_err[bpmcount-1]=fAq3->GetParError(0);
		Aq4_const[bpmcount-1]=fAq4->GetParameter(0);
		Aq4_err[bpmcount-1]=fAq4->GetParError(0);

		Dx1_const[bpmcount-1]=fDx1->GetParameter(0);
		Dx1_err[bpmcount-1]=fDx1->GetParError(0);
		Dx2_const[bpmcount-1]=fDx2->GetParameter(0);
		Dx2_err[bpmcount-1]=fDx2->GetParError(0);
		Dx3_const[bpmcount-1]=fDx3->GetParameter(0);
		Dx3_err[bpmcount-1]=fDx3->GetParError(0);
		Dx4_const[bpmcount-1]=fDx4->GetParameter(0);
		Dx4_err[bpmcount-1]=fDx4->GetParError(0);

		Dy1_const[bpmcount-1]=fDy1->GetParameter(0);
		Dy1_err[bpmcount-1]=fDy1->GetParError(0);
		Dy2_const[bpmcount-1]=fDy2->GetParameter(0);
		Dy2_err[bpmcount-1]=fDy2->GetParError(0);
		Dy3_const[bpmcount-1]=fDy3->GetParameter(0);
		Dy3_err[bpmcount-1]=fDy3->GetParError(0);
		Dy4_const[bpmcount-1]=fDy4->GetParameter(0);
		Dy4_err[bpmcount-1]=fDy4->GetParError(0);

// 		printf("%s  %7.1f +- %.1f  %7.1f +- %.1f  %7.1f +- %.1f   %7.1f +- %.1f \n",device.Data(),fAq1->GetParameter(0),fAq1->GetParError(0),
// 			   fAq2->GetParameter(0),fAq2->GetParError(0),fAq3->GetParameter(0),fAq3->GetParError(0),
// 			   fAq4->GetParameter(0),fAq4->GetParError(0));
// 		printf("%s  %7.3f +- %5.3f  %7.3f +- %5.3f  %7.3f +- %5.3f   %7.3f +- %5.3f \n",device.Data(),fDx1->GetParameter(0),fDx1->GetParError(0),
// 			   fDx2->GetParameter(0),fDx2->GetParError(0),fDx3->GetParameter(0),fDx3->GetParError(0),
// 			   fDx4->GetParameter(0),fDx4->GetParError(0));
// 		printf("%s  %7.3f +- %5.3f  %7.3f +- %5.3f  %7.3f +- %5.3f   %7.3f +- %5.3f \n",device.Data(),fDy1->GetParameter(0),fDy1->GetParError(0),
// 			   fDy2->GetParameter(0),fDy2->GetParError(0),fDy3->GetParameter(0),fDy3->GetParError(0),
// 			   fDy4->GetParameter(0),fDy4->GetParError(0));
	

	}
	//     TString psnam = "plots/RHWP_optimize_";
	//     psnam += runnum1;
	//     psnam += "_";
	//     psnam += runnum2;
	//     psnam += "_";
	//     psnam += device;
	//     psnam += ".png";
	//     //    psnam += ".gif";
	//     RHWPcanvas->Print(psnam.Data());
	

	printf("\n    %14i run %14i run %14i run %14i run\n",runnum1,runnum2,runnum3,runnum4);
	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		printf("%6s  Aq:  %7.1f +- %.1f    %7.1f +- %.1f    %7.1f +- %.1f     %7.1f +- %.1f \n", bpmlist[bpmcount-1].Data(),
			   Aq1_const[bpmcount-1],Aq1_err[bpmcount-1],
			   Aq2_const[bpmcount-1],Aq2_err[bpmcount-1],
			   Aq3_const[bpmcount-1],Aq3_err[bpmcount-1],
			   Aq4_const[bpmcount-1],Aq4_err[bpmcount-1]);
		printf("%6s  Dx:  %7.3f +- %5.3f  %7.3f +- %5.3f  %7.3f +- %5.3f   %7.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),
			   Dx1_const[bpmcount-1],Dx1_err[bpmcount-1],
			   Dx2_const[bpmcount-1],Dx2_err[bpmcount-1],
			   Dx3_const[bpmcount-1],Dx3_err[bpmcount-1],
			   Dx4_const[bpmcount-1],Dx4_err[bpmcount-1]);
		printf("%6s  Dy:  %7.3f +- %5.3f  %7.3f +- %5.3f  %7.3f +- %5.3f   %7.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),
			   Dy1_const[bpmcount-1],Dy1_err[bpmcount-1],
			   Dy2_const[bpmcount-1],Dy2_err[bpmcount-1],
			   Dy3_const[bpmcount-1],Dy3_err[bpmcount-1],
			   Dy4_const[bpmcount-1],Dy4_err[bpmcount-1] );
	}
	printf("\n        %18s %18s %18s %18s\n","ave in","ave out","center","half width");
	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		printf("%6s  Dx:  %7.3f +- %5.3f  %7.3f +- %5.3f  %7.3f +- %5.3f   %7.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),
			   (Dx1_const[bpmcount-1] + Dx3_const[bpmcount-1])/2,Dx1_err[bpmcount-1],
			   (Dx2_const[bpmcount-1] + Dx4_const[bpmcount-1])/2,Dx2_err[bpmcount-1],
			   (Dx1_const[bpmcount-1] + Dx2_const[bpmcount-1] + Dx3_const[bpmcount-1] + Dx4_const[bpmcount-1])/4,Dx1_err[bpmcount-1],
			   (Dx1_const[bpmcount-1] + Dx3_const[bpmcount-1] - Dx2_const[bpmcount-1] - Dx4_const[bpmcount-1])/4,Dx1_err[bpmcount-1]);

		printf("%6s  Dy:  %7.3f +- %5.3f  %7.3f +- %5.3f  %7.3f +- %5.3f   %7.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),
			   (Dy1_const[bpmcount-1] + Dy3_const[bpmcount-1])/2,Dy1_err[bpmcount-1],
			   (Dy2_const[bpmcount-1] + Dy4_const[bpmcount-1])/2,Dy2_err[bpmcount-1],
			   (Dy1_const[bpmcount-1] + Dy2_const[bpmcount-1] + Dy3_const[bpmcount-1] + Dy4_const[bpmcount-1])/4,Dy1_err[bpmcount-1],
			   (Dy1_const[bpmcount-1] + Dy3_const[bpmcount-1] - Dy2_const[bpmcount-1] - Dy4_const[bpmcount-1])/4,Dy1_err[bpmcount-1]);
	}
	printf("Correction required from PC offset.\n");

	char outfilename[255];
	sprintf(outfilename,"%s/RHWP_const_%d_%d_%d_%d.txt",outputdir.Data(),runnum1,runnum2,runnum3,runnum4);
	printf("Writing output to %s\n",outfilename);
	FILE *outfile = fopen(outfilename, "w"); 

	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		printf("%6s  Dx:  %7.3f +- %5.3f\n", bpmlist[bpmcount-1].Data(),
			   (Dx1_const[bpmcount-1] + Dx2_const[bpmcount-1] + Dx3_const[bpmcount-1] + Dx4_const[bpmcount-1])/4,Dx1_err[bpmcount-1]);
		printf("%6s  Dy:  %7.3f +- %5.3f\n", bpmlist[bpmcount-1].Data(),
			   (Dy1_const[bpmcount-1] + Dy2_const[bpmcount-1] + Dy3_const[bpmcount-1] + Dy4_const[bpmcount-1])/4,Dy1_err[bpmcount-1]);
		fprintf(outfile,"%6s  Dx:  %7.3f +- %5.3f\n", bpmlist[bpmcount-1].Data(),
			   (Dx1_const[bpmcount-1] + Dx2_const[bpmcount-1] + Dx3_const[bpmcount-1] + Dx4_const[bpmcount-1])/4,Dx1_err[bpmcount-1]);
		fprintf(outfile,"%6s  Dy:  %7.3f +- %5.3f\n", bpmlist[bpmcount-1].Data(),
			   (Dy1_const[bpmcount-1] + Dy2_const[bpmcount-1] + Dy3_const[bpmcount-1] + Dy4_const[bpmcount-1])/4,Dy1_err[bpmcount-1]);
	}
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
