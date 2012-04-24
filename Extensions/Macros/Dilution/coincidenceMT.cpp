//Script to plot the relevant adc spectra for a given set of dilution runs
//Written by Josh Magee, thanks to Josh Hoskins. 2012-02-27


void coincidencePlot( Int_t runNum ) {
  gROOT -> Reset();
  gROOT -> SetStyle("Plain");
  /*
    TString cuts  = "fQwHits.fElement==3 && fQwHits.fTimeNs!=0";
    TString mdcut = "fQwHits.fRegion==5 && fQwHits.fPlane==1";
    TString tscut = "fQwHits.fRegion==4 && fQwHits.fPlane==1";
  */
  TString filename = Form("Qweak_%i.root",runNum);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cout <<"Error opening ROOTFILE " << file->GetName() <<endl;
    return;
  }
  else {
    std::cout <<"Successfully opened ROOTFILE " <<file->GetName() <<endl;
  }
/*
  TH2F* correlateHisto[7];
  for (Int_t n=0; n<7; n++) {
    correlateHisto[n] = new TH2F(Form("correlateHisto[%i]",n), Form("Hit #%i",n), 1600,-300,1300,1600,-300,1300);
  }

  TH1F* tsMinusMdHisto[7];
  for (Int_t n=0; n<7; n++) {
    tsMinusMdHisto[n] = new TH1F(Form("tsMinusMdHisto[%i]",n), Form("Hit #%i",n), 51,-19.5,30.5);
  }
*/

  TH1F* tsSoftwareMT = new TH1F("tsSoftwareMT","TS Software Meantime",1800,-400,1400);
  TH1F* mdSoftwareMT = new TH1F("mdSoftwareMT","MD Software Meantime",1800,-400,1400);
  TH1F* mdCuts       = new TH1F("mdCuts","Main Detector MT with TS Cut",1800,-400,1400);


  TTree*   event_tree  = (TTree*) file ->Get("event_tree");
  TBranch* ev_branch   = event_tree->GetBranch("events");
  QwEvent* qwevent     = 0;
  QwHit*   qwhit       = 0;

  Long64_t num_entries = event_tree->GetEntries();
  Long64_t localEntry  = 0;
  Int_t    nhit        = 0;

  Int_t fregion = 0;
  Int_t felement = 0;
  Int_t fplane = 0;

  ev_branch -> SetAddress(&qwevent);
 

  for (Long64_t i=0; i<num_entries; i++)
    {//;
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
      
      if(i % 1000 == 0) {
	printf(" Total events %d events process so far : %d\n", num_entries, i);
      }
      
      nhit = qwevent->GetNumberOfHits();
      //    printf("\n");
      Int_t md_mt = -2000;
      Int_t ts_mt = -2000;

      Int_t mdarray[7];
      Int_t tsarray[7];

      Double_t tsMinusMd[7];


      Int_t ts_hitnum = 0;
      Int_t md_hitnum = 0;
      Int_t hitnumber = 0;

      for (Int_t j=0; j<7; j++) {
        mdarray[j]=0;
        tsarray[j]=0;
        tsMinusMd[j]=0;
      }

      for (Int_t hit=0; hit<nhit; hit++)
	{//;;;
	  qwhit = qwevent->GetHit(hit);
	  fregion = qwhit -> GetRegion();
	  felement = qwhit -> GetElement();
	  fplane = qwhit -> GetPlane();
    hitnumber = qwhit -> GetHitNumber();

	  if (fregion==4 && fplane==2 && felement==3)  {
//   tsarray[ts_hitnum] = qwhit -> GetTimeNs();
     tsarray[hitnumber] = qwhit -> GetTimeNs();
            ts_hitnum++;
//	    std::cout <<"Event: " <<i <<"\tTS hit#: " <<ts_hitnum <<endl;
	  }
	  if (fregion==5 && fplane==1 && felement==3) {
//	    mdarray[md_hitnum] = qwhit -> GetTimeNs();
	    mdarray[hitnumber] = qwhit -> GetTimeNs();
 	    md_hitnum++;
//	    std::cout <<"Event: " <<i <<"\tMD hit#: " <<md_hitnum <<endl;
	  }
	} //;;; //end loop through hits

//	if (num_entries % 100000 ) { std::cout <<"ts_mt: " <<ts_mt <<"\tmd_mt " <<md_mt <<"\ttshit: " <<tshit <<"\tmdhit: " <<mdhit<<endl; }

    for (Int_t k=0; k<7; k++) {
        if(tsarray[k]!=0) tsSoftwareMT->Fill(tsarray[k]);
        if(mdarray[k]!=0) mdSoftwareMT->Fill(mdarray[k]);
        if(tsarray[k]!=0 && tsarray[k]>-184 && tsarray[k]<-178 && mdarray[k]!=0) mdCuts[k]->Fill(mdarray[k]);
//      if(tsarray[k]!=0 && mdarray[k]!=0) correlateHisto[k]->Fill(tsarray[k],mdarray[k]);
//      if(tsarray[k]!=0 && mdarray[k]!=0) tsMinusMdHisto[k]->Fill(tsarray[k]-mdarray[k]);
    }

    }//; end loop through num_entries

  TCanvas *tsSoftwareCanvas = new TCanvas("tsSoftwareCanvas","title");
  tsSoftwareMT->SetTitle("Trigger Scintillator Software Meantime");
  tsSoftwareMT->GetXaxis()->SetTitle("Time (ns)");
  gPad->SetLogy();
  tsSoftwareMT->Draw();

  TCanvas *mdSoftwareCanvas = new TCanvas("mdSoftwareCanvas","title");
  mdSoftwareMT->SetTitle("Main Detector Software Meantime");
  mdSoftwareMT->GetXaxis()->SetTitle("Time (ns)");
  gPad->SetLogy();
  mdSoftwareMT->Draw();
  mdSoftwareMT->SetLineColor(2);
  mdCuts->SetLineColor(4);
  mdCuts->Draw("same");
/*
  TCanvas *mdCutsCanvas = new TCanvas("mdCutsCanvas","title");
  mdCuts->SetTitle("Main Detector Meantime with TS Coincidence Condition");
  mdCuts->GetXaxis()->SetTitle("Time (ns)");
  gPad->SetLogy();
  mdCuts->Draw();

  TCanvas* canvases[3];
  for (Int_t n=1; n<3; n++) {
    canvases[n] = new TCanvas(Form("c%i",n),"title");
    correlateHisto[n] -> SetTitle(Form("TS meantime vs. MD meantime for Hit #%i",n));
    correlateHisto[n] -> GetXaxis() -> SetTitle("MD Meantime (ns)");
    correlateHisto[n] -> GetYaxis() -> SetTitle("TS Meantime (ns)");
    correlateHisto[n] -> Draw("colz");
//    correlateHisto[n] -> Draw("prof");
  }

  TCanvas* minusCanvases[3];
  for (Int_t n=1; n<3; n++) {
    minusCanvases[n] = new TCanvas(Form("d%i",n),"title");
    tsMinusMdHisto[n] -> SetTitle(Form("TS meantime -  MD meantime for Hit #%i",n));
    tsMinusMdHisto[n] -> GetXaxis() -> SetTitle("Time (ns)");
    tsMinusMdHisto[n] -> SetLineColor(4);
    tsMinusMdHisto[n] -> SetFillColor(9);
     tsMinusMdHisto[n] -> Draw();
  }
*/

  for (Int_t n=4; n<=7; n++) {
    if (tsarray[n]!=0) std::cout <<"The TS has " << n <<"hits: make sure to plot them!\n";
    if (mdarray[n]!=0) std::cout <<"The MD has " << n <<"hits: make sure to plot them!\n";
  }

//  TCanvas *c4 = new TCanvas("c4","TS - MD MT");
//  tsMinusMdHisto[0] -> Draw();

} //end function

