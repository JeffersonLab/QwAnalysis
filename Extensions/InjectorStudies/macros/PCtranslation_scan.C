// 2010-07-01 Mark Dalton 
// Initial code taken from tools used in Hall A parity programs.
// 7-3-2011 Buddhini Added ErrorFlag and Device _Error_Code cuts. Changed rootfiles stem to "qwinjector"





void plot_element(TTree *p, char *devnam, Int_t scale, TString localcut, Int_t runnum);
void plot_element(TTree *p, TPad *p1, char *devnam, Double_t scale, TString localcut, Int_t runnum, TString scandata);



void PCtranslation_scan(Int_t runnum=1, TString direction="0", Int_t scaleby=1, TString device="1i02", TString usercut ="1") {

	const Int_t numbpms=6;


	Double_t Dx1_const[numbpms],Dx2_const[numbpms],Dx3_const[numbpms],Dx4_const[numbpms];
	Double_t Dy1_const[numbpms],Dy2_const[numbpms],Dy3_const[numbpms],Dy4_const[numbpms];
	Double_t Aq1_const[numbpms],Aq2_const[numbpms],Aq3_const[numbpms],Aq4_const[numbpms];
	Double_t Dx1_err[numbpms],Dx2_err[numbpms],Dx3_err[numbpms],Dx4_err[numbpms];
	Double_t Dy1_err[numbpms],Dy2_err[numbpms],Dy3_err[numbpms],Dy4_err[numbpms];
	Double_t Aq1_err[numbpms],Aq2_err[numbpms],Aq3_err[numbpms],Aq4_err[numbpms];


	if (runnum==1 || direction=="0") {
		printf("Usage:\n\t.x PCtranslation_scan.C(runnum,direction,[scandata scale],[device],[cut])\n\n");
		printf("The scandata will be multiplied by 10^[numdecimals]\n");
		return;
	}
	gROOT->Reset();

	TString scandata;
	if (direction.Contains("X")) scandata = "scandata1";
	if (direction.Contains("Y")) scandata = "scandata2";


	TString plotdir = "output";
	TString outputdir = "output";

    TString gtitle;
    gtitle = "PC translation scan ";
	gtitle += direction;
	gtitle += ", Run ";
    gtitle += runnum;

	// define style here 
	// general parameters
	gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
	gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
	gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
	gStyle->SetOptFit(1); 
	// canvas parameters
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameBorderSize(0);
	gStyle->SetFrameFillColor(10);
	// pads parameters
	//  gStyle->SetPadColor(39); 
	gStyle->SetPadColor(0); 
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadBorderSize(0);
	gStyle->SetPadTopMargin(0.05);
	gStyle->SetPadBottomMargin(0.2);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadLeftMargin(0.15);
 	gStyle->SetLabelSize(0.04,"x");
	gStyle->SetLabelSize(0.04,"y");
	gROOT->ForceStyle();  

	// Open the file
	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/qwinjector_%i.000.root",runnum);
	TFile *_file0 = TFile::Open(filename);
	if(!_file0) exit(1);
	TTree *p = (TTree*)gROOT->FindObject("Hel_Tree");
	TTree *r = (TTree*)gROOT->FindObject("Mps_Tree");
	TString cut;
	TString plotcommand;
	Double_t scale = 1.0/scaleby;

	printf("scale scandata by is %.0f\n",scaleby);

	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05"};

	char outfilename[255];
	sprintf(outfilename,"%s/PCtransscan_run%d.txt",outputdir.Data(),runnum);
	printf("Writing output to %s\n",outfilename);
	FILE *outfile = fopen(outfilename, "w"); 

	TCanvas *a1 = new TCanvas("a1", "PC translation X results", 0,0,1000,1000);
	TCanvas *a2 = new TCanvas("a2", "PC translation Y results", 0,0,1000,1000);
	TCanvas *a3 = new TCanvas("a3", "PC translation Aq results", 0,0,1000,1000);

	TPad* a2_p  = new TPad( "a2_p", "a2_p",0.00,0.00,1.00,1.00);    	
	TPad* a2_title = new TPad("a2_title","a2_title",0.0,0.00,1.0,0.92);    

	TPaveText* pt2 = new TPaveText(0.04,0.04,0.96,0.50,"brNDC");
	pt2->SetBorderSize(0);pt2->SetFillColor(10);
	pt2->SetTextFont(20);
	pt2->SetTextSize(0.55);
	pt2->SetTextAlign(22);
	text = pt2->AddText(0.5,0.9,gtitle.Data());



	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		TString device = bpmlist[bpmcount-1];

// Draw X positions
		plotcommand = Form("diff_qwk_%sX.hw_sum*1000:%s*%f>>histo",device.Data(),scandata.Data(),scale);
		cut = Form("ErrorFlag == 0 && diff_qwk_%sX.Device_Error_Code == 0 && cleandata && %s",device.Data(),usercut.Data());
		std::cout << "command : "<<plotcommand << std::endl;
		std::cout << "cut : "<<cut << std::endl;

		p->Draw(plotcommand,cut,"prof,goff");
		TH1 *histo = (TH1*)gDirectory->Get("histo");

		if (histo == NULL) {
			printf("No events pass cuts\n");
			return;
		} else {
			if(!histo) exit(1);
			histo->Fit("pol1");
			TF1 *fDx1 = histo->GetFunction("pol1");
			if(fDx1 == NULL)  exit(1);
			Dx1_const[bpmcount-1]=fDx1->GetParameter(1);
			Dx1_err[bpmcount-1]=fDx1->GetParError(1);
		}

// Draw Y positions
		plotcommand = Form("diff_qwk_%sY.hw_sum*1000:%s*%f>>histo",device.Data(),scandata.Data(),scale);
		cut = Form("ErrorFlag == 0 && diff_qwk_%sY.Device_Error_Code == 0 && cleandata && %s",device.Data(),usercut.Data());

		std::cout << "command : "<<plotcommand << std::endl;
		std::cout << "cut : "<<cut << std::endl;
		
		p->Draw(plotcommand,cut,"prof,goff");
		TH1 *histo = (TH1*)gDirectory->Get("histo");

		if (histo == NULL) {
			printf("No events pass cuts\n");
			return;
		} else {
			histo->Fit("pol1");
			TF1 *fDy1 = histo->GetFunction("pol1");
			if(fDy1 == NULL) exit(1);
			Dy1_const[bpmcount-1]=fDy1->GetParameter(1);
			Dy1_err[bpmcount-1]=fDy1->GetParError(1);
		}


		fprintf(outfile,"%6s  d(Dx)/d%s:  %8.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dx1_const[bpmcount-1],Dx1_err[bpmcount-1]);
		fprintf(outfile,"%6s  d(Dy)/d%s:  %8.3f +- %5.3f \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dy1_const[bpmcount-1],Dy1_err[bpmcount-1]);
  
	
		printf("\n    \n");

	}

	a1->Draw();
	a2->Draw();
	a3->Draw();
	a1->Clear();
	a2->Clear();
	a3->Clear();
	a1->Divide(3,2);
	a2->Divide(3,2);
	a3->Divide(3,2);


	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		TString device = bpmlist[bpmcount-1];


		std::cout<<"#####################"<<std::endl;
		std::cout<<"On to plotting "<<device<<std::endl;
		std::cout<<"#####################"<<std::endl;

		plot_element(p,(TPad*)a3->cd(bpmcount),"qwk_"+device+"_EffectiveCharge",scale,usercut,runnum,scandata);
		a3->Modified();		
		a3->Update();	
		plot_element(p,(TPad*)a1->cd(bpmcount),"qwk_"+device+"X",scale,usercut,runnum,scandata);
		a1->Modified();		
		a1->Update();
		plot_element(p,(TPad*)a2->cd(bpmcount),"qwk_"+device+"Y",scale,usercut,runnum,scandata);
		a2->Modified();		
		a2->Update();

	}

	a1->Print(Form("output/pc_trans_scan_%i_X.png",runnum));
	a2->Print(Form("output/pc_trans_scan_%i_Y.png",runnum));
	a3->Print(Form("output/pc_trans_scan_%i_Aq.png",runnum));

	

	fclose(outfile);
	for (Int_t bpmcount=1; bpmcount<=numbpms; bpmcount++) {
		printf("%6s  d(Dx)/d%s: %2.3e +- %2.3e \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dx1_const[bpmcount-1],Dx1_err[bpmcount-1]);
		printf("%6s  d(Dy)/d%s: %2.3e +- %2.3e \n", bpmlist[bpmcount-1].Data(),direction.Data(),
			   Dy1_const[bpmcount-1],Dy1_err[bpmcount-1]);
	} 
	
}

void plot_element(TTree *p, TPad *p1, char *devnam, Double_t scale, TString localcut, Int_t runnum, TString scandata) {

	TString *bpmNam = new TString(devnam);


	Int_t ifnd = 0;
	TString tmpname;
	TString plotcommand;

	Int_t icurRun=0;
	char buff[170];

    // for cut
    TString cut;
	TString tit;

    // plot desired  Aq
    tmpname = bpmNam->Data();
 
    if (tmpname.Contains("X") || tmpname.Contains("Y")) {
		plotcommand = Form("diff_%s.hw_sum*1e3:%s*%f>>hAq",tmpname.Data(),scandata.Data(),scale);
		tit = Form("diff_%s (#mum)",tmpname.Data());
		cut = Form("ErrorFlag == 0 && diff_%s.Device_Error_Code == 0 && cleandata && %s",tmpname.Data(),localcut.Data());
    } else {
		if (tmpname.Contains("EffectiveCharge")) {
			plotcommand = Form("asym_%s.hw_sum*1e6:%s*%f>>hAq",tmpname.Data(),scandata.Data(),scale);
			cut = Form("ErrorFlag == 0 && asym_%s.Device_Error_Code == 0 && cleandata && %s",tmpname.Data(),localcut.Data());			
			tit = Form("asym_%s (ppm)",tmpname.Data());		
	} else 
		{
			printf("problems\n");
		}
	}

	std::cout << "command : "<<plotcommand << std::endl;
	std::cout << "cut :"<<cut << std::endl;

	p1->cd();
	
	p->Draw(plotcommand.Data(),cut.Data(),"prof");
	TH1 *hAq = (TH1*)gDirectory->Get("hAq");

	if (hAq == NULL) {
		printf("No events pass cuts\n");
		return;
	}
	// fit Aq linearity
	hAq->Fit("pol1");
	TF1 *f1 = hAq->GetFunction("pol1");
	if(f1 == NULL) exit(1);

	f1->SetLineColor(2);
	f1->SetLineWidth(1);

	hAq->SetMarkerColor(12);
	hAq->SetMarkerSize(1.0);
	hAq->SetMarkerStyle(22);
	hAq->GetXaxis()->CenterTitle();
	hAq->GetXaxis()->SetTitleOffset(1.2);
	hAq->GetXaxis()->SetTitleSize(0.04);
	hAq->GetYaxis()->SetTitleOffset(1.6);
	hAq->GetYaxis()->SetTitleSize(0.04);


	hAq->GetYaxis()->SetTitle(tit);
	hAq->GetXaxis()->SetTitle(Form("run %i : scandata*%2.3f (mils)",runnum,scale));

	hAq->DrawCopy();


// 	char linetxt[50];
// 	pt = new TPaveText(0.0,0.3,0.9,0.7,"brNDC");
// 	pt->SetBorderSize(1);pt->SetFillColor(10);
// 	pt->SetTextAlign(12);pt->SetTextFont(22);
//     if (tmpname.Contains("X")) {
// 		sprintf(linetxt,"D_{x}(0) = %7.1f",f1->GetParameter(0));
// 		pt->AddText(linetxt);
// 		sprintf(linetxt,"slope = %5.3f", f1->GetParameter(1));
// 		pt->AddText(linetxt);
// 	} else {
// 		if (tmpname.Contains("Y")) {
// 			sprintf(linetxt,"D_{y}(0) = %7.2f",f1->GetParameter(0));
// 			pt->AddText(linetxt);
// 			sprintf(linetxt,"slope = %5.3f", f1->GetParameter(1));
// 			pt->AddText(linetxt);
// 		} else {
// 			if (tmpname.Contains("EffectiveCharge")) {
// 				sprintf(linetxt,"A_{q}(0) = %7.2f",f1->GetParameter(0));
// 				pt->AddText(linetxt);
// 				sprintf(linetxt,"slope = %e", f1->GetParameter(1));
// 				pt->AddText(linetxt);
// 			} else {
// 				printf("problems\n");
// 			}
// 		}
// 	}
// 	pt->Draw();
}

/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
