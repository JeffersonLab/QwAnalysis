// 2010-07-01 Mark Dalton 
// Initial code taken from tools used in Hall A parity programs.
// 

void plot_element(TPad *p1, TPad* p2, char *devnam, TString lcut, Int_t dset);

void PITA_scan(Int_t runnumber=1, Int_t ihwp=-1, TString device="1i02", TString usercut ="1", TString intitle="PITA") {
	if (runnumber==1) {
		printf("Usage:\n\t.x PITA_scan.C(runnum,  ihwp,[device],[cut],[title])\n");
		printf("OR\t.x PITA_scan.C(filename,ihwp,[device],[cut],[title])\n\n");
//		printf("Usage:\n\t.x PITA_scan.C(runnum,ihwp,[device],[cut], [title])\n\n");
		printf("defaults:\n\t[device] = \"1i02\"\n\t[cut]    = \"1\"\n\t[title]  = \"PITA\"\n\n");
		return;
	}
	TString infilename = Form("$QW_ROOTFILES/qwinjector_%i.000.root",runnumber);
	PITA_scan(infilename, ihwp, device, usercut, intitle, runnumber);
}

void PITA_scan(TString infilename = "1", Int_t ihwp=-1, TString device="1i02", 
			   TString usercut ="1", TString intitle="PITA", Int_t runnumber=1) {
	if (infilename=="1") {
		printf("Usage:\n\t.x PITA_scan.C(runnum,ihwp,[device],[cut], [title])\n");
		printf("OR\t.x PITA_scan.C(filename,ihwp,[device],[cut], [title])\n\n");
		printf("defaults:\n\t[device] = \"1i02\"\n\t[cut]    = \"1\"\n\t[title]  = \"PITA\"\n\n");
		return;
	}

	gROOT->Reset();
	while (ihwp!=0 && ihwp!=1) {
		printf("Please enter the IHWP position: (1) IN (0) OUT\n");
		cin >> ihwp;
	}

    TString scantype;
    TString gtitle;
    TString plotname;
	TString ihwppos;

	if (ihwp==0) {
		ihwppos="OUT";
	} else {
		if (ihwp==1) {
			ihwppos="IN";	
		} else {
			return;
		}
	}
	gtitle = intitle + " Scan, IHWP ";
	gtitle += ihwppos;
	if (runnumber!=1) {
		gtitle += ", Run ";
		gtitle += runnumber;
	} else {
		gtitle += infilename;
	}

    plotname = "pitascan_run";
    plotname += runnumber;
    
	// define style here 
	// general parameters
	gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
	gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
	gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
  
	// canvas parameters
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameBorderSize(0);
	gStyle->SetFrameFillColor(10);
	// pads parameters
	//  gStyle->SetPadColor(39); 
	gStyle->SetPadColor(0); 
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadBorderSize(0);
	gStyle->SetPadBottomMargin(0.18);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.05,"x");
	gStyle->SetLabelSize(0.05,"y");
	gROOT->ForceStyle();  

	TPad* a1_p[7];

	TCanvas *a1 = new TCanvas("a1", "Scan", 0,0,700,700);
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

	plot_element(a1_p[0],a1_p[1],"qwk_"+device+"_EffectiveCharge",usercut,infilename);
	
	plot_element(a1_p[2],a1_p[3],"qwk_"+device+"X",usercut,infilename);
	
	plot_element(a1_p[4],a1_p[5],"qwk_"+device+"Y",usercut,infilename);

	a1->cd();
	a1->Update();
	TString psnam = "output/";
	psnam += plotname.Data();
	psnam += ".png";
	a1->Print(psnam.Data());
  
}

void plot_element(TPad *p1, TPad* p2, char *devnam, TString localcut, TString infilename) {

	TString *bpmNam = new TString(devnam);

	Int_t ifnd = 0;
	TString tmpname;
	TString plotcommand;

	Int_t icurRun=0;
	char buff[170];

	// Open the file
	TFile *_file0 = TFile::Open(infilename.Data());
	TTree *p = (TTree*)gROOT->FindObject("Hel_Tree");
	TTree *r = (TTree*)gROOT->FindObject("Mps_Tree");


	TString cut;

    // plot desired  Aq
    tmpname = bpmNam->Data();
    TString titpre = "asym_";
    TString titsum = " (ppm) vs Scandata1";
    //    tmpname += "1sum";
    if (tmpname.Contains("X") || tmpname.Contains("Y")) {
		plotcommand = "diff_";
		plotcommand += tmpname.Data();
		plotcommand += ".hw_sum*1000";
		titpre = "diff_";
		titsum = " (#mum) vs Scandata1";

		// make cut
		cut =Form("ErrorFlag==0 && cleandata && diff_%s.Device_Error_Code == 0",tmpname.Data());

    } else {
		if (tmpname.Contains("EffectiveCharge")) {
			plotcommand = "asym_";
			plotcommand += tmpname.Data();
			plotcommand += ".hw_sum*1000000";

			// make cut
			cut = "ErrorFlag==0 && cleandata && asym_"+tmpname+".Device_Error_Code == 0 &&"+ localcut;

		} else {
			printf("problems\n");
		}
	}

	plotcommand += ":scandata1>>hAq";


	printf("%s\n%s\n",plotcommand.Data(),cut.Data());
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
		sprintf(linetxt,"zero at = %7.2f", -(f1->GetParameter(0)/f1->GetParameter(1)));
		pt->AddText(linetxt);
	} else {
		if (tmpname.Contains("Y")) {
			sprintf(linetxt,"D_{y}(0) = %7.2f",f1->GetParameter(0));
			pt->AddText(linetxt);
			sprintf(linetxt,"slope = %7.4f", f1->GetParameter(1));
			pt->AddText(linetxt);
			sprintf(linetxt,"zero at = %7.2f", -(f1->GetParameter(0)/f1->GetParameter(1)));
			pt->AddText(linetxt);
			//sprintf(linetxt,"zero at = %7.4f", f1->GetParameter(1));
			//pt->AddText(linetxt);
		} else {
			if (tmpname.Contains("EffectiveCharge")) {
				sprintf(linetxt,"A_{q}(0) = %7.2f",f1->GetParameter(0));
				pt->AddText(linetxt);
				sprintf(linetxt,"slope = %7.4f", f1->GetParameter(1));
				pt->AddText(linetxt);
				sprintf(linetxt,"zero at = %7.2f", -(f1->GetParameter(0)/f1->GetParameter(1)));
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
