// Author : Jeong Han Lee
//          jhlee@jlab.org
//
// Date   : Thursday, February  9 15:47:41 EST 2012
// 
//         .L F1TDCMultiHits.C
//         ShowMultiHits(1,1) // for MD1p and TS1p
//         ShowMultiHits(1,2) // for MD1m and TS1m
//         ShowMultiHits(5,2) // for MD1m and TS1m
//          0.0.1 : Friday, January 27 15:26:27 EST 2012
//                  
// 
gStyle->SetHistMinimumZero(true);

TCanvas * 
ShowMultiHits(Int_t md_plane, Int_t md_element,   
	      Int_t ts_plane, Int_t ts_element,
	      Int_t event_number=-1, 
	      Int_t nbin=225, 
	      TString filename="14955.root" )
{

  TString path = gSystem->Getenv("QW_ROOTFILES");
  
  if (path.IsNull()) { 
    printf("$QW_ROOTFILES is not defined, please define them first\n");
    return NULL;
  }

  path += "/"; //  just in case...
  path += filename;
  
  TFile *afile = new TFile(path);
  if (afile->IsZombie()) { 
      printf("Error opening file\n"); 
      return NULL;
  }

  TCanvas *c1 = new TCanvas("c1","stacked hists",100,100,1080,800);

  TString name ="";
  //  TString tube ="";
  //  TString cut = "";

  THStack *hs[2];

  
  // if(region ==1 ){
  //   name="SciFi";
  // }
  // else if(region==2) {
  //   name="HDC";
  // }
  // else if(region==3) {
  //   name="VDC";
  // }
  // else
  // if(region == 4) {
  //   name = "TS";
  // }
  // else if (region==5) {
  //   name = "MD";
  // }
  // else if (region==6) {
  //   name = "Scanner";
  // }
  // else {
  //   printf("Region %d is not defined ....\n", region);
  //   delete c1;
  //   return NULL;
  // }
  
  //  name += plane;
  
  // if (element == 1) {
  //   tube = "p";
  // }
  // else if( element == 2) {
  //   tube = "m";
  // }
  // else {
  //   delete c1;
  //   return NULL;
  // }
 

  
  
  hs[0] = new THStack(Form("MD%dElement%dHitStack", md_plane, md_element),
		      Form("MD%d Element %d F1TDC Multi-Hit Contribution Histogram", md_plane, md_element)
		      );
  hs[1] = new THStack(Form("TS%dElement%dHitStack", ts_plane, ts_element),
		      Form("TS%d Element %d F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_element)
		      );
  

  TH1D* mdhist[7];
  TH1D* tshist[7];
  
  for(Int_t idx=0; idx <7;idx++)
    {// 2200, 1100, 550
      mdhist[idx] = new TH1D(Form("MD_hitid%d", idx), Form("MD HitId %d", idx), nbin, -900, 1300);
      tshist[idx] = new TH1D(Form("TS_hitid%d", idx), Form("TS HitId %d", idx), nbin, -900, 1300);
      //  SummaryQ2Mean [j]-> SetXTitle("Run Number");
      //      mdhist[idx] -> SetXTitle("Relative Time[ns]");
    }
  


  TTree*   event_tree  = (TTree*) afile->Get("event_tree");
  TBranch* ev_branch   = event_tree->GetBranch("events");
  QwEvent* qw_event    = 0; 
  QwHit*   qwhit       = 0;

  Long64_t num_entries = event_tree->GetEntries();
  Long64_t localEntry  = 0;
  Int_t    nhit        = 0;

  ev_branch -> SetAddress(&qw_event);
 
  if(event_number != -1) num_entries = event_number;

  for (Long64_t i=0; i<num_entries; i++)
    {//;
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
      
      if(i % 10000 == 0) {
	std::cout <<" Total events " << num_entries
		  << " events process so far: " << i
		  << std::endl;
      }
      
      nhit = qw_event->GetNumberOfHits();
      
      for(Int_t hit_idx=0; hit_idx<nhit; hit_idx++) 
	{//;;
	  qwhit = qw_event->GetHit(hit_idx);

	  Int_t fregion    = qwhit->GetRegion();
	  Int_t fplane     = qwhit->GetPlane();
	  Int_t felement   = qwhit->GetElement();
	  Int_t fhitnumber = 0; 
	  Double_t timens =  0.0; 

	    // std::cout << hit_idx << " " 
	    // 	    << " plane " << fplane 
	    // 	    << " element " << felement
	    // 	    << " Hit num " << fhitnumber
	    // 	    << " time " << qwhit->GetTimeNs()
	    // 	    << std::endl;

	  if (fregion == 4 && fplane==ts_plane && felement==ts_element ) {
	    fhitnumber = qwhit->GetHitNumber();
	    timens     = qwhit->GetTimeNs();
	    tshist[fhitnumber] -> Fill(timens);
	  }
	  else if (fregion == 5 && fplane==md_plane && felement==md_element ) {
	    fhitnumber = qwhit->GetHitNumber();
	    timens     = qwhit->GetTimeNs();
	    mdhist[fhitnumber] -> Fill(timens);
	  }

	}//;; for(Int_t hit_idx=0; 
    }//; Long64_t i=0; i<num_entries....
  
  for(Int_t idx=0; idx <7;idx++)
    {
      mdhist[idx] -> SetDirectory(0);
      mdhist[idx] -> SetMarkerStyle(21);
      hs[0]->Add(mdhist[idx]);      
      tshist[idx] -> SetDirectory(0);
      tshist[idx] -> SetMarkerStyle(21);
      hs[1]->Add(tshist[idx]);
    }

  delete afile;


  mdhist[0] -> SetFillColor(2);
  mdhist[0] -> SetMarkerColor(2);

  mdhist[1] -> SetFillColor(3);
  mdhist[1] -> SetMarkerColor(3);

  mdhist[2] -> SetFillColor(4);
  mdhist[2] -> SetMarkerColor(4);

  mdhist[3] -> SetFillColor(6); 
  mdhist[3] -> SetMarkerColor(6); 

  mdhist[4] -> SetFillColor(7); 
  mdhist[4] -> SetMarkerColor(7); 

  mdhist[5] -> SetFillColor(5); 
  mdhist[5] -> SetMarkerColor(5); 

  mdhist[6] -> SetFillColor(43); 
  mdhist[6] -> SetMarkerColor(43); 


  tshist[0] -> SetFillColor(2);
  tshist[0] -> SetMarkerColor(2);

  tshist[1] -> SetFillColor(3);
  tshist[1] -> SetMarkerColor(3);

  tshist[2] -> SetFillColor(4);
  tshist[2] -> SetMarkerColor(4);

  tshist[3] -> SetFillColor(6); 
  tshist[3] -> SetMarkerColor(6); 

  tshist[4] -> SetFillColor(7); 
  tshist[4] -> SetMarkerColor(7); 

  tshist[5] -> SetFillColor(5); 
  tshist[5] -> SetMarkerColor(5); 

  tshist[6] -> SetFillColor(43); 
  tshist[6] -> SetMarkerColor(43); 

  
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetGridy();
  gPad->SetLogy();
  hs[0] -> Draw("goff");
  hs[0] -> GetXaxis() -> SetTitle("Time[ns]");
  hs[0] -> Draw();
  TLegend *leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");

  leg->SetNColumns(2);
  leg->SetBorderSize(1);
  leg->SetTextFont(62);
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetFillStyle(1001);

  leg->SetHeader("Stack Histogram");
  leg->AddEntry(mdhist[0],"1st Hits","f");
  leg->AddEntry(mdhist[1],"2nd Hits","f");
  leg->AddEntry(mdhist[2],"3rd Hits","f");
  leg->AddEntry(mdhist[3],"4th Hits","f"); 
  leg->AddEntry(mdhist[4],"5th Hits","f");
  leg->AddEntry(mdhist[5],"6th Hits","f");
  leg->AddEntry(mdhist[6],"7th Hits","f");

  leg->Draw();
  gPad->Update();

  c1->cd(2);
  gPad->SetGridy();
  gPad->SetLogy();
  hs[0]->Draw("nostack");
  leg->SetHeader("Non Stack Histogram");
  leg->Draw();
  gPad->Update();

  c1->cd(3);
  gPad->SetGridy();
  gPad->SetLogy();
  hs[1] -> Draw("goff");
  hs[1] -> GetXaxis() -> SetTitle("Time[ns]");
  hs[1] -> Draw();
  leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
  leg->SetHeader("Stack Histogram");
 leg->SetNColumns(2);
  leg->AddEntry(tshist[0],"1st Hits","f");
  leg->AddEntry(tshist[1],"2nd Hits","f");
  leg->AddEntry(tshist[2],"3rd Hits","f");
  leg->AddEntry(tshist[3],"4th Hits","f"); 
  leg->AddEntry(tshist[4],"5th Hits","f");
  leg->AddEntry(tshist[5],"6th Hits","f");
  leg->AddEntry(tshist[6],"7th Hits","f");
  leg->Draw();
  gPad->Update();

  c1->cd(4);
  gPad->SetGridy();
  gPad->SetLogy();
  hs[1]->Draw("nostack");
  leg->SetHeader("Non Stack Histogram");
  leg->Draw();
  gPad->Update();
  c1-> Modified();
  c1-> Update();

  return c1;
};


// nbin is the same as Draw();

TCanvas * 
ShowMTMultiHits(Int_t md_plane, Int_t ts_plane, 
		Int_t event_number=-1, 
		Int_t nbin=102, 
		TString filename="14955.root",
		Int_t time_shift_ns=0)
{
  
  TString path = gSystem->Getenv("QW_ROOTFILES");
  
  if (path.IsNull()) { 
    printf("$QW_ROOTFILES is not defined, please define them first\n");
    return NULL;
  }

  path += "/"; //  just in case...
  path += filename;
  
  TFile *afile = new TFile(path);
  if (afile->IsZombie()) { 
      printf("Error opening file\n"); 
      return NULL;
  }

  TCanvas *c1 = new TCanvas("c1","stacked hists",100,100,1080,800);

  TString name ="";
  

  THStack *hs[3];

  
  hs[0] = new THStack(Form("MD%dsMTHitStack", md_plane),
		      Form("MD%dsMT F1TDC Multi-Hit Contribution Histogram", md_plane)
		      );
  hs[1] = new THStack(Form("TS%dsMTHitStack", ts_plane),
		      Form("TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
		      );
  hs[2] = new THStack(Form("TS%dhMTHitStack", ts_plane),
		      Form("TS%dhMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
		      );
  

  TH1D* mdhist[7];
  TH1D* tshist[7];
  TH1D* tsmthist[7];
  
  //
  for(Int_t idx=0; idx <7;idx++)
    {// 2200, 1100, 550
      mdhist[idx]   = new TH1D(Form("MDsMT_hitid%d", idx), Form("MD sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
      tshist[idx]   = new TH1D(Form("TSsMT_hitid%d", idx), Form("TS sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
      tsmthist[idx] = new TH1D(Form("TShMT_hitid%d", idx), Form("TS hMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
    }
  


  TTree*   event_tree  = (TTree*) afile->Get("event_tree");
  TBranch* ev_branch   = event_tree->GetBranch("events");
  QwEvent* qw_event    = 0; 
  QwHit*   qwhit       = 0;

  Long64_t num_entries = event_tree->GetEntries();
  Long64_t localEntry  = 0;
  Int_t    nhit        = 0;

  ev_branch -> SetAddress(&qw_event);
 
  if(event_number != -1) num_entries = event_number;

  for (Long64_t i=0; i<num_entries; i++)
    {//;
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
      
      if(i % 10000 == 0) {
	std::cout <<" Total events " << num_entries
		  << " events process so far: " << i
		  << std::endl;
      }
      
      //    printf("\n");
      nhit = qw_event->GetNumberOfHits();
      
      Double_t tsp[7] = {0.0};
      Double_t tsm[7] = {0.0};
      Double_t tsmt[7] = {0.0};

      Double_t mdp[7] = {0.0};
      Double_t mdm[7] = {0.0};

      for(Int_t hit_idx=0; hit_idx<nhit; hit_idx++) 
	{//;;
	  qwhit = qw_event->GetHit(hit_idx);

	  Int_t fregion    = qwhit->GetRegion();
	  Int_t fplane     = qwhit->GetPlane();
	  Int_t felement   = qwhit->GetElement();
	  Int_t package    = (Int_t) qwhit-> GetPackage();

	  Int_t fhitnumber = 0; 
	  Double_t timens =  0.0; 
	  fhitnumber = qwhit->GetHitNumber();
	  timens     = qwhit->GetTimeNs() +time_shift_ns;
	  if (fregion == 4 ) {
	    // if(verbose) {
	    //   std::cout << " i " << i <<" : " 
	    //   	      << hit_idx << " " 
	    //   	      << " Region " << fregion
	    //   	      << " package " << package
	    //   	      << " plane " << fplane 
	    //   	      << " element " << felement
	    //   	      << " Hit num " << fhitnumber
	    //   	      << " time " << timens
	    //   	      << std::endl;
	    // }
	    if (felement == 1 ) {
	      tsp[fhitnumber] = timens;
	    }
	    else if(felement == 2) {
	      tsm[fhitnumber] = timens;
	    }
	    else if(felement == 0) {
	      tsmt[fhitnumber] = timens;
	      tsmthist[fhitnumber] -> Fill(timens);
	    }
	  }
	  else if ( fregion == 5 ) {
	    // if(verbose) {
	    //   std::cout << " i " << i <<" : " 
	    // 		<< hit_idx << " " 
	    // 		<< " Region " << fregion
	    // 		<< " package " << package
	    // 		<< " plane " << fplane 
	    // 		<< " element " << felement
	    // 		<< " Hit num " << fhitnumber
	    // 		<< " time " << timens
	    // 		<< std::endl;
	    // }
	    if (felement == 1 ) {
	      mdp[fhitnumber] = timens;
	    }
	    else if(felement == 2) {
	      mdm[fhitnumber] = timens;
	    }
	  }
	}//;;

      for(Int_t hit_id=0; hit_id<7; hit_id++) 
	{//;;
	  if (tsp[hit_id]!=0.0 && tsm[hit_id]!=0.0) {
	    Double_t software_mt_ts = 0.5*(tsp[hit_id]+tsm[hit_id]);
	    // if(verbose) {
	    //   printf("id %d tsp %+8.2f tsm%+8.2f tssmt %+8.2f  tsmt %+8.2f\n", hit_id, tsp[hit_id], tsm[hit_id], software_mt_ts, tsmt[hit_id]);
	    // }
	    tshist[hit_id] -> Fill(software_mt_ts);
	  }
	  Double_t software_mt_md = 0.0;

	  printf("event %d id %d mdp %+8.2f mdm%+8.2f mdsmt ", i, hit_id, mdp[hit_id], mdm[hit_id]);
	  if (mdp[hit_id]!=0.0 && mdm[hit_id]!=0.0) {
	    //	    printf("event %18d id %d mdp %+8.2f mdm%+8.2f mdsmt ", i, hit_id, mdp[hit_id], mdm[hit_id]);
	    software_mt_md = 0.5*(mdp[hit_id]+mdm[hit_id]);
	    printf(" %+8.2f", software_mt_md);

	    mdhist[hit_id] -> Fill(software_mt_md);
	  }
	  printf("\n");
	  
	}//;;


      //	}//;; for(Int_t hit_idx=0; 
    }//; Long64_t i=0; i<num_entries....
  
  for(Int_t idx=0; idx <7;idx++)
    {
      mdhist[idx] -> SetDirectory(0);
      mdhist[idx] -> SetMarkerStyle(21);
      hs[0]->Add(mdhist[idx]);   
   
      tshist[idx] -> SetDirectory(0);
      tshist[idx] -> SetMarkerStyle(21);
      hs[1]->Add(tshist[idx]);

      tsmthist[idx] -> SetDirectory(0);
      tsmthist[idx] -> SetMarkerStyle(21);
      hs[2]->Add(tsmthist[idx]);
    }

  delete afile;


  mdhist[0] -> SetFillColor(2);
  mdhist[0] -> SetMarkerColor(2);

  mdhist[1] -> SetFillColor(3);
  mdhist[1] -> SetMarkerColor(3);

  mdhist[2] -> SetFillColor(4);
  mdhist[2] -> SetMarkerColor(4);

  mdhist[3] -> SetFillColor(6); 
  mdhist[3] -> SetMarkerColor(6); 

  mdhist[4] -> SetFillColor(7); 
  mdhist[4] -> SetMarkerColor(7); 

  mdhist[5] -> SetFillColor(5); 
  mdhist[5] -> SetMarkerColor(5); 

  mdhist[6] -> SetFillColor(43); 
  mdhist[6] -> SetMarkerColor(43); 


  tshist[0] -> SetFillColor(2);
  tshist[0] -> SetMarkerColor(2);

  tshist[1] -> SetFillColor(3);
  tshist[1] -> SetMarkerColor(3);

  tshist[2] -> SetFillColor(4);
  tshist[2] -> SetMarkerColor(4);

  tshist[3] -> SetFillColor(6); 
  tshist[3] -> SetMarkerColor(6); 

  tshist[4] -> SetFillColor(7); 
  tshist[4] -> SetMarkerColor(7); 

  tshist[5] -> SetFillColor(5); 
  tshist[5] -> SetMarkerColor(5); 

  tshist[6] -> SetFillColor(43); 
  tshist[6] -> SetMarkerColor(43); 


  tsmthist[0] -> SetFillColor(2);
  tsmthist[0] -> SetMarkerColor(2);

  tsmthist[1] -> SetFillColor(3);
  tsmthist[1] -> SetMarkerColor(3);

  tsmthist[2] -> SetFillColor(4);
  tsmthist[2] -> SetMarkerColor(4);

  tsmthist[3] -> SetFillColor(6); 
  tsmthist[3] -> SetMarkerColor(6); 

  tsmthist[4] -> SetFillColor(7); 
  tsmthist[4] -> SetMarkerColor(7); 

  tsmthist[5] -> SetFillColor(5); 
  tsmthist[5] -> SetMarkerColor(5); 

  tsmthist[6] -> SetFillColor(43); 
  tsmthist[6] -> SetMarkerColor(43); 

  TLegend *leg = NULL;
  
  c1->Divide(3,2);
  if(hs[0]) {
    c1->cd(1);
 
    gPad->SetGridy();
    gPad->SetLogy();
    hs[0] -> Draw("goff");
    hs[0] -> GetXaxis() -> SetTitle("Time[ns]");
    hs[0] -> Draw();
    leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    
    leg->SetNColumns(2);
    leg->SetBorderSize(1);
    leg->SetTextFont(62);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(1001);
    
    leg->SetHeader("Stack Histogram");
    leg->AddEntry(mdhist[0],"1st Hits","f");
    leg->AddEntry(mdhist[1],"2nd Hits","f");
    leg->AddEntry(mdhist[2],"3rd Hits","f");
    leg->AddEntry(mdhist[3],"4th Hits","f"); 
    leg->AddEntry(mdhist[4],"5th Hits","f");
    leg->AddEntry(mdhist[5],"6th Hits","f");
    leg->AddEntry(mdhist[6],"7th Hits","f");
    leg->Draw();
    gPad->Update();
 
 
    c1->cd(4);
    gPad->SetGridy();
    gPad->SetLogy();
    hs[0]->Draw("nostack");
    leg->SetHeader("Non Stack Histogram");
    leg->Draw();
    gPad->Update();

  }

  if(hs[1]) {
    c1->cd(2);
    gPad->SetGridy();
    gPad->SetLogy();
    hs[1] -> Draw("goff");
    hs[1] -> GetXaxis() -> SetTitle("Time[ns]");
    hs[1] -> Draw();
    leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    leg->SetHeader("Stack Histogram");
    leg->SetNColumns(2);
    leg->AddEntry(tshist[0],"1st Hits","f");
    leg->AddEntry(tshist[1],"2nd Hits","f");
    leg->AddEntry(tshist[2],"3rd Hits","f");
    leg->AddEntry(tshist[3],"4th Hits","f"); 
    leg->AddEntry(tshist[4],"5th Hits","f");
    leg->AddEntry(tshist[5],"6th Hits","f");
    leg->AddEntry(tshist[6],"7th Hits","f");
    leg->Draw();
    gPad->Update();

    c1->cd(5);
    gPad->SetGridy();
    gPad->SetLogy();
    hs[1]->Draw("nostack");
    leg->SetHeader("Non Stack Histogram");
    leg->Draw();
    gPad->Update();
    c1-> Modified();
    c1-> Update();

  }
  
  if(hs[2]) {
    c1->cd(3);
    gPad->SetGridy();
    gPad->SetLogy();
    hs[2] -> Draw("goff");
    hs[2] -> GetXaxis() -> SetTitle("Time[ns]");
    hs[2] -> Draw();
    leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    leg->SetHeader("Stack Histogram");
    leg->SetNColumns(2);
    leg->AddEntry(tshist[0],"1st Hits","f");
    leg->AddEntry(tshist[1],"2nd Hits","f");
    leg->AddEntry(tshist[2],"3rd Hits","f");
    leg->AddEntry(tshist[3],"4th Hits","f"); 
    leg->AddEntry(tshist[4],"5th Hits","f");
    leg->AddEntry(tshist[5],"6th Hits","f");
    leg->AddEntry(tshist[6],"7th Hits","f");
    leg->Draw();
    gPad->Update();

    c1->cd(6);
    gPad->SetGridy();
    gPad->SetLogy();
    hs[2]->Draw("nostack");
    leg->SetHeader("Non Stack Histogram");
    leg->Draw();
    gPad->Update();

  }
  c1-> Modified();
  c1-> Update();

  return c1;
};
