// This tool is based on one used in the Hall A parity experiment and has been modified for QWeak
// The aim is to determine the pedestals from apedestal scan run.
//
// Kent Paschke, Mark Dalton


void InjectorPedestals(Int_t runnum=1, TString usercut="1", Int_t scandatanorm=1000){
	if (runnum==1) {
		printf("Usage:\n\t.x InjectorPedestals.C(runnum, [usercut], [scandatanorm])\n\n");
		printf("[usercut] is an additional root cut to be passed to the fitting and plotting routines.\n");
		printf("[scandatanorm] is the factor to divide scandata1 by to get the current in uA (1000 assumed.)\n\n");
		return;
	}
	gROOT->Reset();

	TString plotdir = "plots";
	TString outputdir = "output";
 	Int_t listlength = 8;
 	TString detlist[] = {"1i02", "1i04", "1i06", "0i02", "0i05", "0i07", 
 						 "0l01", "0l02"}//, "bcm0l02"};

//	Int_t listlength = 1;
//	TString detlist[] = {"1i02"};
	
//	,"0l03", "0l04", "0l05",
// 		       "0l06", "0l07", "0l08", "0l09", "0l10", 
// 		       "0r01", "0r02", "0r05", "0r06", "0l10", 
// 		       };
		 
	gROOT->SetStyle("Plain");

	// define style here 
	// general parameters
	gStyle->SetOptDate(0);     //gStyle->SetOptTitle(0);
	gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
	gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
	gStyle->SetOptFit(1011);
  
	// canvas parameters
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameBorderSize(0);
//	gStyle->SetFrameFillColor(10);
 
	// pads parameters
	//  gStyle->SetPadColor(39); 
// 	gStyle->SetPadColor(0); 
// 	gStyle->SetPadBorderMode(0);
// 	gStyle->SetPadBorderSize(0);
	gStyle->SetPadBottomMargin(0.15);
	gStyle->SetPadRightMargin(0.01);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.05,"x");
	gStyle->SetLabelSize(0.05,"y");
	gStyle->SetTitleSize(0.06,"hxyz");
	gROOT->ForceStyle();  

	TString fname = Form("$QW_ROOTFILES/Qweak_%4d.000.root",runnum);
	TFile *f1 = new TFile(fname.Data());

	TTree* mps = (TTree *) f1->Get("Mps_Tree");
	cout << f1 << " " << mps << endl;

	TCanvas *canvas = new TCanvas("canvas","BPM Pedestal",0,0,700,800);

	// create and initialise
	Double_t arr[4];
	for(Int_t i=0;i<4;i++)   arr[i]=0;

	char outfilename[255];
	sprintf(outfilename,"%s/pedscan_run%d.txt",outputdir.Data(),runnum);
	printf("Writing output to %s\n",outfilename);

	printf("Using cut %s\n",usercut.Data());

	// vqwknum = #, start @ vqwk#
	FILE *outfile = fopen(outfilename, "w"); 
	for(Int_t i=0;i<listlength;i++){    
		//  for(Int_t i=21;i<listlength;i++){    

		cout << detlist[i] << endl;
		TString bpmNam = Form("%s",detlist[i].Data());
		printf("%s\n",bpmNam.Data());

		TString plotname = plotdir.Data();
		plotname += "/pedscan_";
		plotname += bpmNam.Data();
		plotname += Form("_run%d",runnum);
		plotname += ".png";

		if (bpmNam.Contains("bcm")) {
			plot_element_bcm(detlist[i].Data(),arr,canvas,mps,plotname,usercut,scandatanorm);
    
			TString outstr = "QWK_";
			outstr += detlist[i];
			//      outstr += Form(", %10.4g , 1.0 \n",arr[0]*nsamp);
			outstr += Form(", %10.4g , 1.0 \n",arr[0]);
			fprintf(outfile,outstr.Data());
           
		} else {
			plot_element(detlist[i].Data(),arr,canvas,mps,plotname,usercut,scandatanorm);
       
			TString outstr = "QWK_";
			outstr += detlist[i];
			//      outstr += Form("XP , %10.4g , 1.0 \n",arr[0]*nsamp);
			outstr += Form("XP , %10.4g , 1.0 \n",arr[0]);
			fprintf(outfile,outstr.Data());
      
			outstr = "QWK_";
			outstr += detlist[i];
			outstr += Form("XM , %10.4g , 1.0 \n",arr[1]);
			fprintf(outfile,outstr.Data());
      
			outstr = "QWK_";
			outstr += detlist[i];
			outstr += Form("YP , %10.4g , 1.0 \n",arr[2]);
			fprintf(outfile,outstr.Data());
      
			outstr = "QWK_";
			outstr += detlist[i];
			outstr += Form("YM , %10.4g , 1.0 \n",arr[3]);
			fprintf(outfile,outstr.Data());
      
		}

	}      
	fclose(outfile); 
	return;
}

void plot_element(char *devnam, Double_t *arr, TCanvas *canvas, TTree *mps, 
				  TString lplotname, TString localcut, Int_t scandatanorm) 
{
  
	TString bpmNam = Form(devnam);

	// name canvas
	TString cnam = "BPM Pedestal, qwk_";
	cnam += bpmNam.Data();
	canvas->SetTitle(cnam.Data());

	// make cut
//	TString scut = "cleandata&&scandata1<180000&&scandata1>1&&(" + localcut + ")"; // scandata1 should be reasonable
//	TString scut = "cleandata==1&&scandata1>20" + localcut; // scandata1 should be reasonable
	TString scut = "cleandata&&scandata1<180000&&scandata1>1&&(" + localcut + ")";

	Double_t xpint,xmint,ypint,ymint;
	Double_t xpslp,xmslp,ypslp,ymslp;
	Double_t expint,exmint,eypint,eymint;
	Double_t expslp,exmslp,eypslp,eymslp;
	char linetxt[50];
	TString plotcommand;

	canvas->Clear();
	canvas->Divide(2,4);

	if( !(bpmNam.Contains("bcm")) ){
    
		char linetxt[50];
       
		// fit xp wire linearity
		plotcommand = Form("qwk_%sXP.hw_sum_raw",bpmNam.Data());
		plotcommand += ":scandata1/";
    	plotcommand += scandatanorm;

		canvas->cd(1);
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *hxp = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());    
		hxp->Fit("pol1");
		TF1 *f1 = hxp->GetFunction("pol1");
		f1->SetLineColor(kBlue);
		f1->SetLineWidth(1);
		TString tit = Form("XP vs. Current;Current  (#muA);qwk_%sXP", bpmNam.Data());
		hxp->SetTitle(tit.Data());
		hxp->Draw("ap");
		hxp->GetXaxis()->CenterTitle();
		hxp->GetXaxis()->SetTitleOffset(1.2);
		hxp->GetXaxis()->SetTitleSize(0.06); 

		xpint = f1->GetParameter(0);
		xpslp = f1->GetParameter(1);
		expint = f1->GetParError(0);
		expslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += Form("XP.hw_sum_raw / num_samples - scandata1/%i*",scandatanorm);
		plotcommand += xpslp;
		plotcommand += " - ";
		plotcommand += xpint;
 		plotcommand += Form(":scandata1/%i", scandatanorm);
    
		canvas->cd(2);
		//printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *Rxp = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());  
		TString tit = Form("Residual;Current  (#muA);qwk_%sXP", bpmNam.Data());
		Rxp->SetTitle(tit.Data());
		Rxp->Draw("ap");
		Rxp->GetXaxis()->CenterTitle();
		Rxp->GetXaxis()->SetTitleOffset(1.2);
		Rxp->GetXaxis()->SetTitleSize(0.06);     
		canvas->Update();

		// fit xm wire linearity
		canvas->cd(3);

		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += "XM.hw_sum_raw / num_samples:scandata1/";
    	plotcommand +=  scandatanorm;

		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		//printf("%s\n%s\n",plotcommand.Data(),scut.Data());    
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *hxm = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());    
		hxm->Fit("pol1");
		TF1 *f1 = hxm->GetFunction("pol1");
		f1->SetLineColor(kBlue);
		f1->SetLineWidth(1);
		TString tit = Form("XM vs. Current;Current  (#muA);qwk_%sXM", bpmNam.Data());
		hxm->SetTitle(tit.Data());
		hxm->Draw("ap");
		hxm->GetXaxis()->CenterTitle();
		hxm->GetXaxis()->SetTitleOffset(1.2);
		hxm->GetXaxis()->SetTitleSize(0.06); 

		xmint = f1->GetParameter(0);
		xmslp = f1->GetParameter(1);
		expint = f1->GetParError(0);
		expslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += Form("XM.hw_sum_raw / num_samples - scandata1/%i*",scandatanorm);
		plotcommand += xmslp;
		plotcommand += " - ";
		plotcommand += xmint;
		plotcommand += Form(":scandata1/%i",scandatanorm);
      
		canvas->cd(4);
		//printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *Rxm = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());	
		TString tit = Form("Residual;Current  (#muA);qwk_%sXM", bpmNam.Data());
		Rxm->SetTitle(tit.Data());
		Rxm->Draw("ap");	
		Rxm->GetXaxis()->CenterTitle();
		Rxm->GetXaxis()->SetTitleOffset(1.2);
		Rxm->GetXaxis()->SetTitleSize(0.06);
		canvas->Update();

		canvas->cd(5);

		// fit yp wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += "YP.hw_sum_raw / num_samples:scandata1/";
    	plotcommand +=  scandatanorm;

		//printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *hyp = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());    
		hyp->Fit("pol1");
		TF1 *f1 = hyp->GetFunction("pol1");
		f1->SetLineColor(kBlue);
		f1->SetLineWidth(1);
		TString tit = Form("YP vs. Current;Current  (#muA);qwk_%sYP", bpmNam.Data());
		hyp->SetTitle(tit.Data());
		hyp->Draw("ap");
		hyp->GetXaxis()->CenterTitle();
		hyp->GetXaxis()->SetTitleOffset(1.2);
		hyp->GetXaxis()->SetTitleSize(0.06); 

		ypint = f1->GetParameter(0);
		ypslp = f1->GetParameter(1);
		eypint = f1->GetParError(0);
		eypslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += Form("YP.hw_sum_raw / num_samples - scandata1/%i*",scandatanorm);
		plotcommand += ypslp;
		plotcommand += " - ";
		plotcommand += ypint;
		plotcommand += Form(":scandata1/%i", scandatanorm);
     
		canvas->cd(6);
		//printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *Ryp = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());
		TString tit = Form("Residual;Current  (#muA);qwk_%sYP", bpmNam.Data());
		Ryp->SetTitle(tit.Data());
		Ryp->Draw("ap");
		Ryp->GetXaxis()->CenterTitle();
		Ryp->GetXaxis()->SetTitleOffset(1.2);
		Ryp->GetXaxis()->SetTitleSize(0.06);
		canvas->Update();
    
		canvas->cd(7);
		// fit ym wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += "YM.hw_sum_raw / num_samples:scandata1/";
    	plotcommand +=  scandatanorm;

    	//printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *hym = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());    
		hym->Fit("pol1");
		TF1 *f1 = hym->GetFunction("pol1");
		f1->SetLineColor(kBlue);
		f1->SetLineWidth(1);
		TString tit = Form("YM vs. Current;Current  (#muA);qwk_%sYM", bpmNam.Data());
		hym->SetTitle(tit.Data());
		hym->Draw("ap");
		hym->GetXaxis()->CenterTitle();
		hym->GetXaxis()->SetTitleOffset(1.2);
		hym->GetXaxis()->SetTitleSize(0.06); 

		ymint = f1->GetParameter(0);
		ymslp = f1->GetParameter(1);
		eymint = f1->GetParError(0);
		eymslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += Form("YM.hw_sum_raw / num_samples - scandata1/%i*",scandatanorm);
		plotcommand += ymslp;
		plotcommand += " - ";
		plotcommand += ymint;
		plotcommand += Form(":scandata1/%i",scandatanorm);
      
		canvas->cd(8);
		//printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		printf("Mps_Tree->Draw(\"%s\",\"%s\")\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *Rym = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());
		TString tit = Form("Residual;Current  (#muA);qwk_%sYM", bpmNam.Data());
		Rym->SetTitle(tit.Data());
		Rym->Draw("ap");
		Rym->GetXaxis()->CenterTitle();
		Rym->GetXaxis()->SetTitleOffset(1.2);
		Rym->GetXaxis()->SetTitleSize(0.06);
		canvas->Update();

//     lplotname = bpmNam.Data();
//     lplotname += "ym.png";
		//c1->Print(lplotname);
//     c1->Close();

		canvas->Update();
		canvas->Print(lplotname);
		canvas->cd();

		printf("%.1f\n",xpint);     arr[0]=xpint;
		printf("%.1f\n",xmint);     arr[1]=xmint;
		printf("%.1f\n",ypint);     arr[2]=ypint;
		printf("%.1f\n",ymint);     arr[3]=ymint;
	}
  
}

void plot_element_bcm(char *devnam, Double_t *arr, TCanvas *canvas, TTree *mps, 
					  TString lplotname, TString localcut, Int_t scandatanorm) 
{
  

	Double_t qint,qslp,eqint,eqslp;

	TString *bpmNam = new TString(devnam);


	// name canvas
	TString cnam = "BCM Pedestal, qwk_";
	cnam += bpmNam.Data();
	canvas->SetTitle(cnam.Data());

// make cut
	TString scut = "cleandata&&scandata1<180000&&scandata1>1&&(" + localcut + ")";

	Double_t xpint,xmint,ypint,ymint;
	Double_t xpslp,xmslp,ypslp,ymslp;
	Double_t expint,exmint,eypint,eymint;
	Double_t expslp,exmslp,eypslp,eymslp;
	char linetxt[50];
	TString plotcommand;

	canvas->Clear();
	canvas->Divide(1,2);

	if( (bpmNam.Contains("bcm")) ){
    
		char linetxt[50];

		// fit xp wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += ".hw_sum_raw / num_samples:scandata1/";
    	plotcommand +=  scandatanorm;
    
		canvas->cd(1);
    	printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *hq = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());    
		hq->Fit("pol1");
		TF1 *f1 = hq->GetFunction("pol1");
		f1->SetLineColor(kBlue);
		f1->SetLineWidth(1);
		TString tit = Form("qwk_", bpmNam.Data());
		hq->SetTitle(tit.Data());
		hq->Draw("ap");
		hq->GetXaxis()->CenterTitle();
		hq->GetXaxis()->SetTitleOffset(1.2);
		hq->GetXaxis()->SetTitleSize(0.06); 

		qint = f1->GetParameter(0);
		qslp = f1->GetParameter(1);
		eqint = f1->GetParError(0);
		eqslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam.Data();
		plotcommand += Form(".hw_sum_raw / num_samples - scandata1/%i*",scandatanorm);
		plotcommand += qslp;
		plotcommand += " - ";
		plotcommand += qint;
		plotcommand += Form(":scandata1/%i",scandatanorm);
      
		canvas->cd(2);
		printf("%s\n%s\n",plotcommand.Data(),scut.Data());
		mps->Draw(plotcommand.Data(),scut.Data(),"goff");
		TGraph *Rq = new TGraph(mps->GetSelectedRows(),mps->GetV2(),mps->GetV1());
		TString tit = Form("Residual:qwk_", bpmNam.Data());
		Rq->SetTitle(tit.Data());
		Rq->Draw("ap");
		Rq->GetXaxis()->CenterTitle();
		Rq->GetXaxis()->SetTitleOffset(1.2);
		Rq->GetXaxis()->SetTitleSize(0.06);
		canvas->Update();

		canvas->cd();
// 		TString lplotname = "qwk_";
// 		lplotname += bpmNam.Data();
// 		lplotname += ".png"
		canvas->Print(lplotname);

		printf("%.1f\n",qint);     arr[0]=qint;
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
