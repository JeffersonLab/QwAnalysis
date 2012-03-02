// Author : Jeong Han Lee
//          jhlee@jlab.org
//
// Date   : Thursday, February  9 15:47:41 EST 2012
// 
//         .L F1TDCMultiHits.C
//         ShowMultiHits(1,1) // for MD1p and TS1p
//         ShowMultiHits(1,2) // for MD1m and TS1m
//         ShowMultiHits(5,2) // for MD1m and TS1m
//
//          0.1 : Friday, January 27 15:26:27 EST 2012
//
//          0.2 : Sunday, February 12 00:16:16 EST 2012,
//                added ShowMTMultiHits
//                      in order to understand a software meantime
//                      histograms in TS and MD F1TDC signals.
//
//                ShowMTMultiHits(1,1,-1, -1, 102, 0,0, filename);
//          0.3 : Tuesday, February 14 16:35:36 EST 2012
//                modified ShowMTMultiHits
//   




void
Style() 
{
  gStyle->SetHistMinimumZero(kTRUE);
  gStyle->SetPalette(1,0);
  gROOT->ForceStyle();
  return;
}

TCanvas * 
ShowMultiHits(Int_t md_plane, Int_t md_element,   
	      Int_t ts_plane, Int_t ts_element,
	      Int_t event_number=-1, 
	      Int_t nbin=225, 
	      TString filename="14955.root" )
{

  Style();

  TString path = gSystem->Getenv("QW_ROOTFILES");
  
  if (path.IsNull()) { 
    printf("$QW_ROOTFILES is not defined, please define them first\n");
    return NULL;
  }

  path += "/"; //  just in case...
  path += filename;
  
  TFile afile(path);
  if (afile.IsZombie()) { 
      printf("Error opening file\n"); 
      return NULL;
  }

  TCanvas *c1 = new TCanvas("c1","stacked hists",100,100,1080,800);

  TString name ="";
  //  TString tube ="";
  //  TString cut = "";

  THStack *hs[2];


  
  
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
  


  TTree*   event_tree  = (TTree*) afile.Get("event_tree");
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

  afile.Close();


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



TCanvas * 
ShowMTMultiHits(Int_t md_plane, Int_t ts_plane, 
		Int_t event_number_start=-1, 
		Int_t event_number_end=-1,
		Bool_t ts_debug = false, Bool_t md_debug = false,
		TString filename="14955.root",
		Int_t nbin=102, 
		Int_t time_shift_ns=0)
{

  Style();
  Bool_t verbose = true;
  // Bool_t ts_debug = false;
  // Bool_t md_debug = true;

  TString path = gSystem->Getenv("QW_ROOTFILES");
  
  if (path.IsNull()) { 
    printf("$QW_ROOTFILES is not defined, please define them first\n");
    return NULL;
  }

  path += "/"; //  just in case...
  path += filename;
  
  TFile afile(path);
  if (afile.IsZombie()) { 
      printf("Error opening file\n"); 
      return NULL;
  }

  TString output_filename="MTMultiHits";
 
  output_filename += "md";
  output_filename += md_plane;
					       
  output_filename += "ts";
  output_filename += ts_plane;
  
  output_filename += "_";
  output_filename += filename;

   TCanvas *c1 = new TCanvas("c1","stacked hists",100,100,1080,800);

  TString name ="";
  

  TObjArray HistList(0);

  THStack *mt_hs[6];

  
  mt_hs[0] = new THStack(Form("MD%dsMTHitStack", md_plane),
		      Form("MD%dsMT F1TDC Multi-Hit Contribution Histogram", md_plane)
		      );
  mt_hs[1] = new THStack(Form("TS%dsMTHitStack", ts_plane),
		      Form("TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
		      );
  mt_hs[2] = new THStack(Form("TS%dhMTHitStack", ts_plane),
		      Form("TS%dhMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
		      );

  mt_hs[3] = new THStack(Form("dMD%dpmHitStack", md_plane),
			 Form("MD%p - MD%dm F1TDC Multi-Hit Contribution Histogram", md_plane, md_plane)
		      );
  mt_hs[4] = new THStack(Form("dTS%dpmHitStack", ts_plane),
			 Form("TS%dp - TS%dm F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_plane)
		      );
  mt_hs[5] = new THStack(Form("dTS%dMTsHitStack", ts_plane),
			 Form("TS%dhMT -TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_plane)
			 );
  
  
  for(Int_t idx=0; idx<6; idx++)
    {
      HistList.Add(mt_hs[idx]);
    }
  
  TH1D* mdhist[7];
  TH1D* tshist[7];
  TH1D* tsmthist[7];

  TH1D* dmdhist[7];
  TH1D* dtshist[7];
  TH1D* dtsmthist[7];

  //
  for(Int_t idx=0; idx <7;idx++)
    {// 2200, 1100, 550
      mdhist[idx]   = new TH1D(Form("MDsMT_hitid_%d", idx), Form("MD sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
      tshist[idx]   = new TH1D(Form("TSsMT_hitid_%d", idx), Form("TS sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
      tsmthist[idx] = new TH1D(Form("TShMT_hitid_%d", idx), Form("TS hMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
   
      dmdhist[idx]   = new TH1D(Form("dMDpm_hitid_%d", idx), Form("MDp - MDm  HitId %d", idx),    nbin, -2000+time_shift_ns, 2000+time_shift_ns);
      dtshist[idx]   = new TH1D(Form("dTSpm_hitid_%d", idx), Form("TSp - TSm  HitId %d", idx),    nbin, -2000+time_shift_ns, 2000+time_shift_ns);
      dtsmthist[idx] = new TH1D(Form("dTSMT_hitid_%d", idx), Form("TShMT - TSsMT HitId %d", idx), nbin, -2000+time_shift_ns, 2000+time_shift_ns);

      HistList.Add(mdhist[idx]);
      HistList.Add(tshist[idx]);
      HistList.Add(tsmthist[idx]);

      HistList.Add(dmdhist[idx]);
      HistList.Add(dtshist[idx]);
      HistList.Add(dtsmthist[idx]);
      
    } 
  


  TTree*   event_tree  = (TTree*) afile.Get("event_tree");
  TBranch* ev_branch   = event_tree->GetBranch("events");
  QwEvent* qw_event    = 0; 
  QwHit*   qwhit       = 0;

  Long64_t num_entries = event_tree->GetEntries();
  //  Long64_t ini_event   = 0;
  Long64_t localEntry  = 0;
  Int_t    nhit        = 0;

  ev_branch -> SetAddress(&qw_event);
 

  if (event_number_start ==-1) {
    event_number_start = 0;
  }
  if (event_number_end != -1) {
    num_entries = event_number_end;
  }

  Double_t ini = 0.0;
  for (Long64_t i=event_number_start; i<num_entries; i++)
    {//;
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
      
      if(i % 1000 == 0) {
	printf(" Total events %d events process so far : %d\n", num_entries, i);
      }
      
      //    printf("\n");
      nhit = qw_event->GetNumberOfHits();
      
      

      Double_t tsp[7]  = {ini};
      Double_t tsm[7]  = {ini};
      Double_t tsmt[7] = {ini};

      Double_t mdp[7]  = {ini};
      Double_t mdm[7]  = {ini};

      Int_t fregion    = 0;
      Int_t fplane     = 0;
      Int_t felement   = 0;
      Int_t fhitnumber = 0;

      Double_t timens  = 0.0;
      
      Int_t ts_max_hit = 0;
      Int_t md_max_hit = 0;
    
      for(Int_t hit_idx=0; hit_idx<nhit; hit_idx++) 
	{//;;
	  qwhit = qw_event->GetHit(hit_idx);

	  fregion    = qwhit->GetRegion();

	  if (fregion == 4 ) { 
	    fplane     = qwhit->GetPlane();
	    if (fplane != ts_plane)   continue;
	    else {
	      felement   = qwhit->GetElement();
	      fhitnumber = qwhit->GetHitNumber(); 
	      
	      if (fhitnumber > ts_max_hit ) ts_max_hit = fhitnumber;
	      timens =  qwhit->GetTimeNs() +time_shift_ns;
	      if     (felement == 1) tsp[fhitnumber] = timens;
	      else if(felement == 2) tsm[fhitnumber] = timens;
	      else if(felement == 0) {
		tsmt[fhitnumber] = timens;
		tsmthist[fhitnumber] -> Fill(timens);
	      }
	    }
	  }
	  else if (fregion == 5 ) {
	    fplane     = qwhit->GetPlane();
	    if (fplane != md_plane)   continue;
	    else {
	      felement   = qwhit->GetElement();
	      fhitnumber = qwhit->GetHitNumber(); 
	      timens =  qwhit->GetTimeNs() +time_shift_ns;
	      if(fhitnumber > md_max_hit ) md_max_hit = fhitnumber;

	      if     (felement == 1) mdp[fhitnumber] = timens;
	      else if(felement == 2) mdm[fhitnumber] = timens;
	    }
	    
	  }
	  else {
	    continue;
	  }
	}//;;

      if (md_debug || ts_debug) printf("\n");

      Int_t ts_max_hit_range = ts_max_hit+1;

      for(Int_t idx=0; idx<ts_max_hit_range; idx++) 
	{//;;
	  Double_t software_mt_ts = 0.0;
	  Double_t software_del_ts = 0.0;
	  if(ts_debug) {
	    printf(">>TS>> NHits%4d in Event %8d: HitOrder[%d,%d] TSp%+9.2f TSm%+9.2f", 
		   nhit, i, ts_max_hit, idx, tsp[idx], tsm[idx]);
	  }
	  if (tsp[idx]!=ini && tsm[idx]!=ini) {
	    software_mt_ts  = 0.5*(tsp[idx]+tsm[idx]);
	    software_del_ts = tsp[idx] - tsm[idx];
	    if (ts_debug) printf(" dTS%+9.2f TSsMT %+10.2f", software_del_ts, software_mt_ts);
	    tshist[idx]  -> Fill(software_mt_ts);
	    dtshist[idx] -> Fill(software_del_ts);
	  }
	  else {
	    software_mt_ts = 0.0;
	    if (ts_debug) printf(" >>---------- >>--------------");
	  }

	 
	  if(software_mt_ts!=0.0) {
	    dtsmthist[idx] -> Fill(tsmt[idx]-software_mt_ts);
	    if (ts_debug )  {
	      printf(" TSMT %+8.2f dTSMT %+8.2f >>\n", tsmt[idx], tsmt[idx]-software_mt_ts);
	    }
	  }
	  else {
	    if (ts_debug )  {
	      printf(" TSMT %+8.2f >>------------ >>\n", tsmt[idx]);
	    }
	  }
	

	  
	}
    

      Int_t md_max_hit_range = md_max_hit+1;
      for(Int_t idx=0; idx<md_max_hit_range; idx++) 
	{//;;
	  Double_t software_mt_md  = 0.0;
	  Double_t software_del_md = 0.0;
	  if (md_debug) {
	    printf("<<MD<< NHits%4d in Event %8d: HitOrder[%d,%d] MDp%+9.2f MDm%+9.2f", 
		   nhit, i,md_max_hit,  idx, mdp[idx], mdm[idx]);
	  }
	  if (mdp[idx]!=ini && mdm[idx]!=ini) {
	    //	    printf("event %18d id %d mdp %+8.2f mdm%+8.2f mdsmt ", i, idx, mdp[idx], mdm[idx]);
	    software_mt_md  = 0.5*(mdp[idx]+mdm[idx]);
	    software_del_md = mdp[idx]-mdm[idx];

	    mdhist[idx]  -> Fill(software_mt_md);
	    dmdhist[idx] -> Fill(software_del_md);
	    if (md_debug) printf(" dMD%+9.2f MDsMT %+10.2f", software_del_md, software_mt_md);
	  }
	  else {
	    if(md_debug)  printf(" <<---------- <<--------------");

	  }
	  if (md_debug) printf(" <<----------- <<------------ <<\n");
	  
	}//;;


      //	}//;; for(Int_t hit_idx=0; 
    }//; Long64_t i=0; i<num_entries....
  
  for(Int_t idx=0; idx <7;idx++)
    {
      mdhist[idx] -> SetDirectory(0);
      mdhist[idx] -> SetMarkerStyle(21);
      mt_hs[0]->Add(mdhist[idx]);   
   
      tshist[idx] -> SetDirectory(0);
      tshist[idx] -> SetMarkerStyle(21);
      mt_hs[1]->Add(tshist[idx]);

      tsmthist[idx] -> SetDirectory(0);
      tsmthist[idx] -> SetMarkerStyle(21);
      mt_hs[2]->Add(tsmthist[idx]);

      dmdhist[idx] -> SetDirectory(0);
      dmdhist[idx] -> SetMarkerStyle(21);
      mt_hs[3]->Add(dmdhist[idx]);   
   
      dtshist[idx] -> SetDirectory(0);
      dtshist[idx] -> SetMarkerStyle(21);
      mt_hs[4]->Add(dtshist[idx]);

      dtsmthist[idx] -> SetDirectory(0);
      dtsmthist[idx] -> SetMarkerStyle(21);
      mt_hs[5]->Add(dtsmthist[idx]);

    }


  afile.Close();


  // Int_t color[7]= {2,3,4,6,7,5,43};

  // for (Int_t idx = 0; i<7; i++) 
  //   {
  //     //      color[idx] = gROOT->GetColor(int_color[idx]);

      // mdhist[idx] -> SetFillColor((Color_t) color[idx]);
      // mdhist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
      // tshist[idx] -> SetFillColor((Color_t) color[idx]);
      // tshist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
      // tsmthist[idx] -> SetFillColor((Color_t) color[idx]);
      // tsmthist[idx] -> SetMarkerColor((Color_t) color[idx]);

      // dmdhist[idx] -> SetFillColor((Color_t) color[idx]);
      // dmdhist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
      // dtshist[idx] -> SetFillColor((Color_t) color[idx]);
      // dtshist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
      // dtsmthist[idx] -> SetFillColor((Color_t) color[idx]);
      // dtsmthist[idx] -> SetMarkerColor((Color_t) color[idx]);
    
  //    }


  mdhist[0] -> SetFillColor(2);
  mdhist[0] -> SetMarkerColor(2);
  
  tshist[0] -> SetFillColor(2);
  tshist[0] -> SetMarkerColor(2);
  
  tsmthist[0] -> SetFillColor(2);
  tsmthist[0] -> SetMarkerColor(2);
  
  dmdhist[0] -> SetFillColor(2);
  dmdhist[0] -> SetMarkerColor(2);
  
  dtshist[0] -> SetFillColor(2);
  dtshist[0] -> SetMarkerColor(2);
  
  dtsmthist[0] -> SetFillColor(2);
  dtsmthist[0] -> SetMarkerColor(2);


  mdhist[1] -> SetFillColor(3);
  mdhist[1] -> SetMarkerColor(3);
  
  tshist[1] -> SetFillColor(3);
  tshist[1] -> SetMarkerColor(3);
  
  tsmthist[1] -> SetFillColor(3);
  tsmthist[1] -> SetMarkerColor(3);
  
  dmdhist[1] -> SetFillColor(3);
  dmdhist[1] -> SetMarkerColor(3);
  
  dtshist[1] -> SetFillColor(3);
  dtshist[1] -> SetMarkerColor(3);
  
  dtsmthist[1] -> SetFillColor(3);
  dtsmthist[1] -> SetMarkerColor(3);



  mdhist[2] -> SetFillColor(4);
  mdhist[2] -> SetMarkerColor(4);
  
  tshist[2] -> SetFillColor(4);
  tshist[2] -> SetMarkerColor(4);
  
  tsmthist[2] -> SetFillColor(4);
  tsmthist[2] -> SetMarkerColor(4);
  
  dmdhist[2] -> SetFillColor(4);
  dmdhist[2] -> SetMarkerColor(4);
  
  dtshist[2] -> SetFillColor(4);
  dtshist[2] -> SetMarkerColor(4);
  
  dtsmthist[2] -> SetFillColor(4);
  dtsmthist[2] -> SetMarkerColor(4);




  mdhist[3] -> SetFillColor(6);
  mdhist[3] -> SetMarkerColor(6);
  
  tshist[3] -> SetFillColor(6);
  tshist[3] -> SetMarkerColor(6);
  
  tsmthist[3] -> SetFillColor(6);
  tsmthist[3] -> SetMarkerColor(6);
  
  dmdhist[3] -> SetFillColor(6);
  dmdhist[3] -> SetMarkerColor(6);
  
  dtshist[3] -> SetFillColor(6);
  dtshist[3] -> SetMarkerColor(6);
  
  dtsmthist[3] -> SetFillColor(6);
  dtsmthist[3] -> SetMarkerColor(6);




  mdhist[4] -> SetFillColor(7);
  mdhist[4] -> SetMarkerColor(7);
  
  tshist[4] -> SetFillColor(7);
  tshist[4] -> SetMarkerColor(7);
  
  tsmthist[4] -> SetFillColor(7);
  tsmthist[4] -> SetMarkerColor(7);
  
  dmdhist[4] -> SetFillColor(7);
  dmdhist[4] -> SetMarkerColor(7);
  
  dtshist[4] -> SetFillColor(7);
  dtshist[4] -> SetMarkerColor(7);
  
  dtsmthist[4] -> SetFillColor(7);
  dtsmthist[4] -> SetMarkerColor(7);




  mdhist[5] -> SetFillColor(5);
  mdhist[5] -> SetMarkerColor(5);
  
  tshist[5] -> SetFillColor(5);
  tshist[5] -> SetMarkerColor(5);
  
  tsmthist[5] -> SetFillColor(5);
  tsmthist[5] -> SetMarkerColor(5);
  
  dmdhist[5] -> SetFillColor(5);
  dmdhist[5] -> SetMarkerColor(5);
  
  dtshist[5] -> SetFillColor(5);
  dtshist[5] -> SetMarkerColor(5);
  
  dtsmthist[5] -> SetFillColor(5);
  dtsmthist[5] -> SetMarkerColor(5);






  mdhist[6] -> SetFillColor(43);
  mdhist[6] -> SetMarkerColor(43);
  
  tshist[6] -> SetFillColor(43);
  tshist[6] -> SetMarkerColor(43);
  
  tsmthist[6] -> SetFillColor(43);
  tsmthist[6] -> SetMarkerColor(43);
  
  dmdhist[6] -> SetFillColor(43);
  dmdhist[6] -> SetMarkerColor(43);
  
  dtshist[6] -> SetFillColor(43);
  dtshist[6] -> SetMarkerColor(43);
  
  dtsmthist[6] -> SetFillColor(43);
  dtsmthist[6] -> SetMarkerColor(43);




  TLegend *leg = NULL;
  
  c1->Divide(3,2);
  if(mt_hs[0]) {
    c1->cd(1);
 
    gPad->SetGridy();
    gPad->SetLogy();
    mt_hs[0] -> Draw("goff");
    mt_hs[0] -> GetXaxis() -> SetTitle("Time[ns]");
    mt_hs[0] -> Draw();
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
    mt_hs[0]->Draw("nostack");
    leg->SetHeader("Non Stack Histogram");
    leg->Draw();
    gPad->Update();

  }

  if(mt_hs[1]) {
    c1->cd(2);
    gPad->SetGridy();
    gPad->SetLogy();
    mt_hs[1] -> Draw("goff");
    mt_hs[1] -> GetXaxis() -> SetTitle("Time[ns]");
    mt_hs[1] -> Draw();
    TLegend *leg2 = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    leg2->SetHeader("Stack Histogram");
    leg2->SetNColumns(2);
    leg2->AddEntry(tshist[0],"1st Hits","f");
    leg2->AddEntry(tshist[1],"2nd Hits","f");
    leg2->AddEntry(tshist[2],"3rd Hits","f");
    leg2->AddEntry(tshist[3],"4th Hits","f"); 
    leg2->AddEntry(tshist[4],"5th Hits","f");
    leg2->AddEntry(tshist[5],"6th Hits","f");
    leg2->AddEntry(tshist[6],"7th Hits","f");
    leg2->Draw();
    gPad->Update();

    c1->cd(5);
    gPad->SetGridy();
    gPad->SetLogy();
    mt_hs[1]->Draw("nostack");
    leg2->SetHeader("Non Stack Histogram");
    leg2->Draw();
    gPad->Update();
    c1-> Modified();
    c1-> Update();

  }
  
  if(mt_hs[2]) {
    c1->cd(3);
    gPad->SetGridy();
    gPad->SetLogy();
    mt_hs[2] -> Draw("goff");
    mt_hs[2] -> GetXaxis() -> SetTitle("Time[ns]");
    mt_hs[2] -> Draw();
    TLegend *leg3 = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    leg3->SetHeader("Stack Histogram");
    leg3->SetNColumns(2);
    leg3->AddEntry(tshist[0],"1st Hits","f");
    leg3->AddEntry(tshist[1],"2nd Hits","f");
    leg3->AddEntry(tshist[2],"3rd Hits","f");
    leg3->AddEntry(tshist[3],"4th Hits","f"); 
    leg3->AddEntry(tshist[4],"5th Hits","f");
    leg3->AddEntry(tshist[5],"6th Hits","f");
    leg3->AddEntry(tshist[6],"7th Hits","f");
    leg3->Draw();
    gPad->Update();

    c1->cd(6);
    gPad->SetGridy();
    gPad->SetLogy();
    mt_hs[2]->Draw("nostack");
    leg3->SetHeader("Non Stack Histogram");
    leg3->Draw();
    gPad->Update();

  }
  c1-> Modified();
  c1-> Update();


  TFile f(Form("%s", output_filename.Data()),"recreate");
  //  printf("2\n");
  HistList.Write();


  return c1;
};
