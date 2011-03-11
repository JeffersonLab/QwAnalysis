


void transmission_2D(Int_t runnumber=1) 
{
	if (runnumber==1) {
		printf("Usage:\n\t.x transmission_2D.C(runnum, listfilename)\n\n");
		return;
	}
	gROOT->Reset();
	gROOT->SetStyle("Plain");
	Bool_t debug=0;
	Bool_t setsize=1;
	Double_t maxx=0.05;

	Double_t maxy, maxxplot, maxyplot;
	maxy=maxx;
	maxxplot=maxyplot=maxx*1.1;

	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetTitleSize(0.06,"h");

	gROOT->ForceStyle();  

	gStyle->SetTitleBorderSize(0);
	//TString bpmlist[] = {"bpm1I02", "bpm1I04","bpm1I06","bpm0I01","bpm0I01A","bpm0I02","bpm0I02A","bpm0I05"};
// 	TString bpmlist[] = {"1i02", "1i04","1i06","0i01","0i01A","0i02","0i02A","0i05",
// 						"0i07","0l02","0l03","0l04","0l05","0l06","0l07","0l08",
// 						"0l09","0l10","0r03","0r04"};
// 	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05",
// 						"0i07","0l02 ","0l03","0l04","0l05","0l06","0l07","0l08",
// 						"0l09","0l10"};
// 	const Int_t numplots=16;
	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05",
						 "0i07","0l02 "};
// 	,"0l03","0l04","0l05","0l06","0l07","0l08",
// 						"0l09","0l10"};
	const Int_t numplots=8;

	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnumber);
	printf("Opening file: %s\n",filename);
	TFile *_file0 = TFile::Open(filename);

	TTree *tree;
	tree = (TTree*)gROOT->FindObject("Hel_Tree");

	TString plotname, histname, title;

	TCanvas *Transcanvas = new TCanvas("Transcanvas","Transmission",40,0,1200,600);
	Transcanvas->Clear();
	Transcanvas->Divide(4,2,0.0001,0.0001);
	TH1F *hx[numplots], *hy[numplots];
	Double_t xpos[numplots],xerr[numplots],ypos[numplots],yerr[numplots];
	Double_t xposall[numplots],xerrall[numplots],yposall[numplots],yerrall[numplots];
	TGraphErrors *graph[numplots];

	Double_t sizex[2], sizey[2];
	sizex[0]=-maxx;sizex[1]=maxx; sizey[0]=-maxy;sizey[1]=maxy;
	TGraph *graphsize = new TGraph(2,sizex,sizey);
	TLine *t1 = new TLine(0,-maxyplot,0,maxyplot);
	TLine *t2 = new TLine(-maxxplot,0,maxxplot,0);

	Transcanvas->cd();
	xpos[1]=0;
	xerr[1]=0;
	ypos[1]=0;
	yerr[1]=0;

	for (Int_t i=1; i<=numplots; i++) {
		Transcanvas->cd(i);

		histname = "hx";
		histname += i;
		plotname = "diff_qwk_";
		plotname += bpmlist[i-1] + "X.hw_sum*1000";
		plotname += ">>" + histname;
		if (debug) printf("%i  %s   %s\n",i, plotname.Data(), histname.Data() );
		tree->Draw(plotname,"1","goff");
		hx[i-1] = (TH1F*)gROOT->FindObject(histname);
		xposall[i-1] = xpos[0] = hx[i-1]->GetMean();
		xerrall[i-1] = xerr[0] = hx[i-1]->GetRMS()/sqrt(hx[i-1]->GetEntries()-1);
		printf("%6s X, mean: %8.4f +- %.4f um\n",bpmlist[i-1].Data(),xpos[0], xerr[0]);

		histname = "hy";
		histname += i;
		plotname = "diff_qwk_";
		plotname += bpmlist[i-1] + "Y.hw_sum*1000";
		plotname += ">>" + histname;
		if (debug) printf("%i  %s   %s\n",i, plotname.Data(), histname.Data() );
		tree->Draw(plotname,"1","goff");
		hy[i-1] = (TH1F*)gROOT->FindObject(histname);
		yposall[i-1] = ypos[0] = hy[i-1]->GetMean();
		yerrall[i-1] = yerr[0] = hy[i-1]->GetRMS()/sqrt(hy[i-1]->GetEntries()-1);
		printf("%6s Y, mean: %8.4f +- %.4f um\n",bpmlist[i-1].Data(),ypos[0], yerr[0]);
//		printf("%s Y, mean: %f, error: %f\n",bpmlist[i-1].Data(), ypos[0], yerr[0]);
	
		graph[i-1] = new TGraphErrors(2,xpos,ypos,xerr,yerr);
		graph[i-1]->SetMarkerStyle(20);
		graph[i-1]->SetMarkerColor(kBlue);
		graph[i-1]->SetLineColor(kBlue);
		title = bpmlist[i-1];
		title += "  (run ";
		title += runnumber;
		title += ");x position diff   (#mu m);y position diff   (#mu m)";
		graphsize->SetTitle(title.Data());
		if (setsize) graphsize->DrawClone("ap");
		t1->DrawLine(-maxxplot,0,maxxplot,0);
		t2->DrawLine(0,-maxyplot,0,maxyplot);
		graph[i-1]->Draw("ple,same");
	}


	TGraphErrors *graphall = new TGraphErrors(numplots,xposall,yposall,xerrall,yerrall);
	Int_t xmin, xmax, ymin, ymax;
	xmin = graphall->GetXaxis()->GetBinLowEdge(0);
	xmax = 0;//graphall->GetXaxis()->GetBinHighEdge(graphall->GetXaxis()->GetNbins());
	ymin = graphall->GetYaxis()->GetBinLowEdge(0);
	ymax = 0;//graphall->GetYaxis()->GetBinHighEdge(graphall->GetYaxis()->GetNbins());
	//    printf("%.4f  %.4f;  %.4f  %.4f\n",xmin,xmax,ymin,ymax);

	TCanvas *Transcanvas1 = new TCanvas("Transcanvas1","Transmission",240,400,400,400);
	Transcanvas1->Clear();
	graphall->SetMarkerStyle(20);
	graphall->SetMarkerColor(kBlue);
	graphall->SetLineColor(kBlue);
	title = "Transmission (run ";
	title += runnumber;
	title += ");x position diff   (#mu m);y position diff   (#mu m)";
	graphsize->SetTitle(title.Data());
	graphsize->DrawClone("ap");
	graphall->DrawClone("pl");
	t1->DrawLine(-maxxplot,0,maxxplot,0);
	t2->DrawLine(0,-maxyplot,0,maxyplot);

}



/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
