
#include <algorithm>

void transmission_2D_compare(Int_t runnum1=1, Int_t runnum2=1, 
							 const string comment="", const string comment2="", TString usercut ="1") {
	if (runnum1==1 || runnum2==1) {
		printf("Usage:\n\t.x transmission_2D_compare.C(runnum1, runnum2, [comment1], [comment2], [cut])\n\n");
		return;
	}  
    gROOT->Reset();

	Bool_t debug = 0;

    gROOT->SetStyle("Plain");
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetTitleSize(0.06,"h");
    //    gStyle->SetLabelSize(0.05,"y");
    gROOT->ForceStyle();  
    // gStyle->SetOptStat(kFALSE);
    gStyle->SetTitleBorderSize(0);

//     TCanvas *Trans2DCompCanvas = new TCanvas("Trans2DCompCanvas","Transmission",40,0,800,800);
//     Trans2DCompCanvas->Clear();
//     Trans2DCompCanvas->Divide(4,4,0.0001,0.0001);
// 	TString bpmlist[] = {"1i02", "1i04","1i06","0i02","0i02a","0i05",
// 						"0i07","0l02","0l03","0l04","0l05","0l06","0l07","0l08",
// 						"0l09","0l10"};
// 	const Int_t numbpms=16;
// 	Double_t maxx=4;

    TCanvas *Trans2DCompCanvas = new TCanvas("Trans2DCompCanvas","Transmission",40,0,1200,600);
    Trans2DCompCanvas->Clear();
    Trans2DCompCanvas->Divide(4,2,0.0001,0.0001);
	TString bpmlist[] = {"1i02", "1i04","1i06","1i01","1i01a","0i02","0i02a","0i05"};//,"0i07","0l01"};
	const Int_t numbpms=8;
	Double_t maxx=0.08;

//    TString bpmlist[] = {"bpm1i02", "bpm1i04","bpm1i06","bpm0i01","bpm0i01A","bpm0i02","bpm0i02A","bpm0i05"};
//    const Int_t numplots=8;
//     TString bpmlist[] = {"bpm1i02", "bpm1i04","bpm1i06","bpm0i01","bpm0i01A","bpm0i02","bpm0i02A","bpm0i05",
// 			"bpm0i07","bpm0l02","bpm0l03","bpm0l04","bpm0l05","bpm0l06","bpm0l07","bpm0l08",
// 			"bpm0l09","bpm0l10","bpm0r03","bpm0r04"};
//     const Int_t numplots=20;


	Double_t maxy, maxxplot, maxyplot;
	maxy=maxx;
	maxxplot=maxyplot=maxx*1.1;


	// Open file 1
	char filename[255];
	sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum1);
	TFile *_file1 = TFile::Open(filename);
	TTree *tree1 = (TTree*)gROOT->FindObject("Hel_Tree");

	sprintf(filename,"$QW_ROOTFILES/Qweak_%i.000.root",runnum2);
	TFile *_file2 = TFile::Open(filename);
	TTree *tree2 = (TTree*)gROOT->FindObject("Hel_Tree");

//	TString cut = "cleandata&&" + usercut;
	TString cut = usercut+"&&pattern_number>10";

    TString plotcommand, histname, title;

    TH1F *hx[numbpms], *hy[numbpms];
    Double_t xpos[numbpms],xerr[numbpms],ypos[numbpms],yerr[numbpms];
    Double_t xposall[numbpms],xerrall[numbpms],yposall[numbpms],yerrall[numbpms];
    TGraphErrors *graph[numbpms];

    TH1F *hx2[numbpms], *hy2[numbpms];
    Double_t xpos2[numbpms],xerr2[numbpms],ypos2[numbpms],yerr2[numbpms];
    Double_t xposall2[numbpms],xerrall2[numbpms],yposall2[numbpms],yerrall2[numbpms];
    TGraphErrors *graph2[numbpms];


    Double_t sizex[2], sizey[2];
    sizex[0]=-maxx;sizex[1]=maxx; sizey[0]=-maxy;sizey[1]=maxy;
    TGraph *graphsize = new TGraph(2,sizex,sizey);
    TLine *t1 = new TLine(0,-maxyplot,0,maxyplot);
    TLine *t2 = new TLine(-maxxplot,0,maxxplot,0);

    Trans2DCompCanvas->cd();
    xpos[1]=0;
    xerr[1]=0;
    ypos[1]=0;
    yerr[1]=0;
    xpos2[1]=0;
    xerr2[1]=0;
    ypos2[1]=0;
    yerr2[1]=0;

    for (Int_t i=1; i<=numbpms; i++) {
        Trans2DCompCanvas->cd(i);

		TString detname = "qwk_";
		detname += bpmlist[i-1];

		histname = "hx";
		histname += i;
		plotcommand = "diff_";
		plotcommand += detname;
		plotcommand += "X.hw_sum";
		plotcommand += "*1000>>";
		plotcommand += histname;

		if (debug) printf("%i  %s   %s\n",i, plotcommand.Data(), histname.Data() );
		tree1->Draw(plotcommand,cut.Data(),"goff");
		hx[i-1] = (TH1F*)gROOT->FindObject(histname);
		xposall[i-1] = xpos[0] = hx[i-1]->GetMean();
		xerrall[i-1] = xerr[0] = hx[i-1]->GetRMS()/sqrt(hx[i-1]->GetEntries()-1);
		tree2->Draw(plotcommand,cut.Data(),"goff");
		hx2[i-1] = (TH1F*)gROOT->FindObject(histname);
		xposall2[i-1] = xpos2[0] = hx2[i-1]->GetMean();
		xerrall2[i-1] = xerr2[0] = hx2[i-1]->GetRMS()/sqrt(hx2[i-1]->GetEntries()-1);

		histname = "hy";
		histname += i;
		plotcommand = "diff_";
		plotcommand += detname;
		plotcommand += "Y.hw_sum";
		plotcommand += "*1000>>";
		plotcommand += histname;

		if (debug) printf("%i  %s   %s\n",i, plotcommand.Data(), histname.Data() );
		tree1->Draw(plotcommand,cut.Data(),"goff");
		hy[i-1] = (TH1F*)gROOT->FindObject(histname);
		yposall[i-1] = ypos[0] = hy[i-1]->GetMean();
		yerrall[i-1] = yerr[0] = hy[i-1]->GetRMS()/sqrt(hy[i-1]->GetEntries()-1);
		tree2->Draw(plotcommand,cut.Data(),"goff");
		hy2[i-1] = (TH1F*)gROOT->FindObject(histname);
		yposall2[i-1] = ypos2[0] = hy2[i-1]->GetMean();
		yerrall2[i-1] = yerr2[0] = hy2[i-1]->GetRMS()/sqrt(hy[i-1]->GetEntries()-1);
		printf("%6s X, mean: %8.4f +- %.4f        %8.4f +- %.4f        %8.4f +- %.4f\n", 
			   bpmlist[i-1].Data(), xpos[0], xerr[0], xpos2[0], xerr2[0], (xpos[0]+ypos2[0])/2, max(yerr[0],yerr2[0]));	
		printf("%6s Y, mean: %8.4f +- %.4f        %8.4f +- %.4f        %8.4f +- %.4f\n", 
			   bpmlist[i-1].Data(), ypos[0], yerr[0], ypos2[0], yerr2[0], (ypos[0]+ypos2[0])/2, max(yerr[0],yerr2[0]));	

		graph[i-1] = new TGraphErrors(2,xpos,ypos,xerr,yerr);
		graph[i-1]->SetMarkerStyle(20);
		graph[i-1]->SetMarkerColor(kBlue);
		graph[i-1]->SetLineColor(kBlue);
		title = bpmlist[i-1];
		title += "   ";

		title += ";x position diff   (#mu m);y position diff   (#mu m)";
		graphsize->SetTitle(title.Data());
		graphsize->DrawClone("ap");
		t1->DrawLine(-maxxplot,0,maxxplot,0);
		t2->DrawLine(0,-maxyplot,0,maxyplot);
		graph[i-1]->Draw("pl,same");


		graph2[i-1] = new TGraphErrors(2,xpos2,ypos2,xerr2,yerr2);
		graph2[i-1]->SetMarkerStyle(20);
		graph2[i-1]->SetMarkerColor(kRed);
		graph2[i-1]->SetLineColor(kRed);
		graph2[i-1]->Draw("pl,same");

		if (i==1) {
			TLegend *legend = new TLegend(0.33,0.78,0.99,0.99);
			title = "run ";
			title += runnum1;
			title += " " + comment;
			if (debug) cout << title << endl;
			legend->AddEntry(graph[i-1],title.Data(),"p");
			title = "run ";
			title += runnum2;
			title += " " + comment2;
			if (debug) cout << title << endl;
			legend->AddEntry(graph2[i-1],title.Data(),"p");
			legend->Draw();
		}
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
