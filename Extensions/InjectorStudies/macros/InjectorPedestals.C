// This tool is based on one used in the Hall A parity experiment and has been modified for QWeak
// The aim is to determine the pedestals from apedestal scan run.
//
// Kent Paschke, Mark Dalton


void InjectorPedestals(Int_t runnum=1){
	if (runnum==1) {
		printf("Usage:\n\t.x InjectorPedestals.C(runnum)\n\n");
		return;
	}
	gROOT->Reset();

	TString plotdir = "plots";
	TString outputdir = "output";
	Int_t listlength = 9;
	TString detlist[] = {"1i02", "1i04", "1i06", "0i02", "0i05", "0i07", 
						 "0l01", "0l02", "bcm0l02"};
//	,"0l03", "0l04", "0l05",
// 		       "0l06", "0l07", "0l08", "0l09", "0l10", 
// 		       "0r01", "0r02", "0r05", "0r06", "0l10", 
// 		       };
		 
	gROOT->SetStyle("Plain");

	// define style here 
	// general parameters
	gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
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
	gStyle->SetPadBottomMargin(0.18);
	gStyle->SetPadRightMargin(0.01);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.05,"x");
	gStyle->SetLabelSize(0.05,"y");
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

	FILE *outfile = fopen(outfilename, "w"); 
	// vqwknum = #, start @ vqwk#

	for(Int_t i=0;i<listlength;i++){    
		//  for(Int_t i=21;i<listlength;i++){    

		cout << detlist[i] << endl;
		TString* bpmNam = new TString(detlist[i].Data());

		TString plotname = plotdir.Data();
		plotname += "/pedscan_";
		plotname += bpmNam->Data();
		plotname += ".png";

		if (bpmNam->Contains("bcm")) {
			cout << "WTF  " << bpmNam->Data() << endl;
			plot_element_bcm(detlist[i].Data(),arr,canvas,mps,plotname);

    
			TString outstr = "QWK_";
			outstr += detlist[i];
			//      outstr += Form(", %10.4g , 1.0 \n",arr[0]*nsamp);
			outstr += Form(", %10.4g , 1.0 \n",arr[0]);
			fprintf(outfile,outstr.Data());
           
		} else {
			plot_element(detlist[i].Data(),arr,canvas,mps,plotname);
      
     
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

void plot_element(char *devnam, Double_t *arr, TCanvas *canvas, TTree *mps, TString lplotname) 
{
  
	TString *bpmNam = new TString(devnam);

	// name canvas
	TString cnam = "BPM Pedestal, qwk_";
	cnam += bpmNam->Data();
	canvas->SetTitle(cnam.Data());

	// make cut
	TString scut = "cleandata==1";

	Double_t xpint,xmint,ypint,ymint;
	Double_t xpslp,xmslp,ypslp,ymslp;
	Double_t expint,exmint,eypint,eymint;
	Double_t expslp,exmslp,eypslp,eymslp;
	char linetxt[50];
	TString plotcommand;

	canvas->Clear();
	canvas->Divide(2,4);

	if( !(bpmNam->Contains("bcm")) ){
    
		char linetxt[50];
       
		// fit xp wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "XP.hw_sum_raw / num_samples:scandata1/10>>hxp";
    
		canvas->cd(1);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof,goff");
		//TGraph *hxp = new TGraph(r->GetSelectedRows(),r->GetV2(),r->GetV1());    
		hxp->Fit("pol1");
		TF1 *f1 = hxp->GetFunction("pol1");
		hxp->SetMarkerStyle(20);
		hxp->SetMarkerColor(kBlue);
		// hxp->Add(f1,-1);
		hxp->GetXaxis()->CenterTitle();
		hxp->GetXaxis()->SetTitleOffset(1.2);
		hxp->GetXaxis()->SetTitleSize(0.06);
		hxp->Draw("pe,goff");


		TString tit = "qwk_";
		tit += bpmNam->Data();
		tit += ", XP vs. Current";
		hxp->GetXaxis()->SetTitle(tit.Data());
//     TString funcstr = Form("Xp = %8.2f +  %6.1f * I", 
// 			    f1->GetParameter(0), f1->GetParameter(1));
//     pt = new TPaveText(0.1,0.88,0.9,0.99,"brNDC");
//     pt->SetBorderSize(1);pt->SetFillColor(10);
//     pt->SetTextAlign(12);pt->SetTextFont(22);
//     pt->AddText(funcstr.Data());
//     pt->Draw();

		xpint = f1->GetParameter(0);
		xpslp = f1->GetParameter(1);
		expint = f1->GetParError(0);
		expslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "XP.hw_sum_raw / num_samples - scandata1/10*";
		plotcommand += xpslp;
		plotcommand += " - ";
		plotcommand += xpint;
		plotcommand += ":scandata1/10>>Rxp";
      
		canvas->cd(2);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof");
		TProfile * Rxp = (TProfile*)gROOT->FindObject("Rxp");
		Rxp->GetXaxis()->CenterTitle();
		Rxp->GetXaxis()->SetTitleOffset(1.2);
		Rxp->GetXaxis()->SetTitleSize(0.06);
		TString tit = "residual:qwk_ ";
		tit += bpmNam->Data();
		tit += ",  XP vs. Current";
		Rxp->GetXaxis()->SetTitle(tit.Data());
		Rxp->Draw();
    
		// fit xm wire linearity
		canvas->cd(3);

		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "XM.hw_sum_raw / num_samples:scandata1/10>>hxm";
    
		mps->Draw(plotcommand.Data(),scut.Data(),"prof,goff");
		//TGraph *hxm = new TGraph(r->GetSelectedRows(),r->GetV2(),r->GetV1());    
		hxm->Fit("pol1");
		TF1 *f1 = hxm->GetFunction("pol1");
		hxm->Draw("p");
		hxm->SetMarkerStyle(20);
		hxm->SetMarkerColor(kBlue);
		xmint = f1->GetParameter(0);
		xmslp = f1->GetParameter(1);
		exmint = f1->GetParError(0);
		exmslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "XM.hw_sum_raw / num_samples - scandata1/10*";
		plotcommand += xmslp;
		plotcommand += " - ";
		plotcommand += xmint;
		plotcommand += ":scandata1/10>>Rxm";
      
		canvas->cd(4);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof");
		TProfile * Rxm = (TProfile*)gROOT->FindObject("Rxm");
		Rxm->GetXaxis()->CenterTitle();
		Rxm->GetXaxis()->SetTitleOffset(1.2);
		Rxm->GetXaxis()->SetTitleSize(0.06);
		TString tit = "residual:qwk_ ";
		tit += bpmNam->Data();
		tit += ",  xm vs. Current";
		Rxm->GetXaxis()->SetTitle(tit.Data());
		Rxm->Draw();
		canvas->cd(5);

		// fit yp wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "YP.hw_sum_raw / num_samples:scandata1/10>>hyp";
    
		mps->Draw(plotcommand.Data(),scut.Data(),"prof,goff");
		//TGraph *hyp = new TGraph(r->GetSelectedRows(),r->GetV2(),r->GetV1());    
		hyp->Fit("pol1");
		TF1 *f1 = hyp->GetFunction("pol1");
		hyp->Draw("p");
		hyp->SetMarkerStyle(20);
		hyp->SetMarkerColor(kBlue);
		ypint = f1->GetParameter(0);
		ypslp = f1->GetParameter(1);
		eypint = f1->GetParError(0);
		eypslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "YP.hw_sum_raw / num_samples - scandata1/10*";
		plotcommand += ypslp;
		plotcommand += " - ";
		plotcommand += ypint;
		plotcommand += ":scandata1/10>>Ryp";
      
		canvas->cd(6);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof");
		TProfile * Ryp = (TProfile*)gROOT->FindObject("Ryp");
		Ryp->GetXaxis()->CenterTitle();
		Ryp->GetXaxis()->SetTitleOffset(1.2);
		Ryp->GetXaxis()->SetTitleSize(0.06);
		TString tit = "residual:qwk_ ";
		tit += bpmNam->Data();
		tit += ",  yp vs. Current";
		Ryp->GetXaxis()->SetTitle(tit.Data());
		Ryp->Draw();
    
		canvas->cd(7);
		// fit ym wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "YM.hw_sum_raw / num_samples:scandata1/10>>hym";
    
		mps->Draw(plotcommand.Data(),scut.Data(),"prof,goff");
		//TGraph *hym = new TGraph(r->GetSelectedRows(),r->GetV2(),r->GetV1());    
		hym->Fit("pol1");
		TF1 *f1 = hym->GetFunction("pol1");
		hym->Draw("p");
		hym->SetMarkerStyle(20);
		hym->SetMarkerColor(kBlue);
		ymint = f1->GetParameter(0);
		ymslp = f1->GetParameter(1);
		eymint = f1->GetParError(0);
		eymslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += "YM.hw_sum_raw / num_samples - scandata1/10*";
		plotcommand += ymslp;
		plotcommand += " - ";
		plotcommand += ymint;
		plotcommand += ":scandata1/10>>Rym";
      
		canvas->cd(8);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof");
		TProfile * Rym = (TProfile*)gROOT->FindObject("Rym");
		Rym->GetXaxis()->CenterTitle();
		Rym->GetXaxis()->SetTitleOffset(1.2);
		Rym->GetXaxis()->SetTitleSize(0.06);
		TString tit = "residual:qwk_ ";
		tit += bpmNam->Data();
		tit += ",  ym vs. Current";
		Rym->GetXaxis()->SetTitle(tit.Data());
		Rym->Draw();

//     lplotname = bpmNam->Data();
//     lplotname += "ym.png";
		//c1->Print(lplotname);
//     c1->Close();

		canvas->Print(lplotname);
		canvas->cd();

		printf("%.1f\n",xpint);     arr[0]=xpint;
		printf("%.1f\n",xmint);     arr[1]=xmint;
		printf("%.1f\n",ypint);     arr[2]=ypint;
		printf("%.1f\n",ymint);     arr[3]=ymint;
	}
  
}

void plot_element_bcm(char *devnam, Double_t *arr, TCanvas *canvas, TTree *mps, TString lplotname) 
{
  

	Double_t qint,qslp,eqint,eqslp;

	TString *bpmNam = new TString(devnam);


	// name canvas
	TString cnam = "BCM Pedestal, qwk_";
	cnam += bpmNam->Data();
	canvas->SetTitle(cnam.Data());

// make cut
	TString scut = "cleandata==1";

	Double_t xpint,xmint,ypint,ymint;
	Double_t xpslp,xmslp,ypslp,ymslp;
	Double_t expint,exmint,eypint,eymint;
	Double_t expslp,exmslp,eypslp,eymslp;
	char linetxt[50];
	TString plotcommand;

	canvas->Clear();
	canvas->Divide(1,2);

	if( (bpmNam->Contains("bcm")) ){
    
		char linetxt[50];
       
		// fit xp wire linearity
		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += ".hw_sum_raw / num_samples:scandata1/10>>hq";
    
		canvas->cd(1);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof,goff");
		//TGraph *hq = new TGraph(r->GetSelectedRows(),r->GetV2(),r->GetV1());    
		hq->Fit("pol1");
		TF1 *f1 = hq->GetFunction("pol1");
		hq->SetMarkerStyle(20);
		hq->SetMarkerColor(kBlue);
		// hq->Add(f1,-1);
		hq->GetXaxis()->CenterTitle();
		hq->GetXaxis()->SetTitleOffset(1.2);
		hq->GetXaxis()->SetTitleSize(0.06);
		hq->Draw("pe,goff");


		TString tit = "qwk_";
		tit += bpmNam->Data();
		tit += " vs. Current";
		hq->GetXaxis()->SetTitle(tit.Data());

		qint = f1->GetParameter(0);
		qslp = f1->GetParameter(1);
		eqint = f1->GetParError(0);
		eqslp = f1->GetParError(1);

		plotcommand = "qwk_";
		plotcommand += bpmNam->Data();
		plotcommand += ".hw_sum_raw / num_samples - scandata1/10*";
		plotcommand += qslp;
		plotcommand += " - ";
		plotcommand += qint;
		plotcommand += ":scandata1/10>>Rq";
      
		canvas->cd(2);
		mps->Draw(plotcommand.Data(),scut.Data(),"prof");
		TProfile * Rq = (TProfile*)gROOT->FindObject("Rq");
		Rq->GetXaxis()->CenterTitle();
		Rq->GetXaxis()->SetTitleOffset(1.2);
		Rq->GetXaxis()->SetTitleSize(0.06);
		TString tit = "residual:qwk_ ";
		tit += bpmNam->Data();
		tit += ",  q vs. Current";
		Rq->GetXaxis()->SetTitle(tit.Data());
		Rq->Draw();
    

		canvas->cd();
// 		TString lplotname = "qwk_";
// 		lplotname += bpmNam->Data();
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
