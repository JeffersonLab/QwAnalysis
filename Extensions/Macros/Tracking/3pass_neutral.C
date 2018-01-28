//  3pass_dave.C
//
// authors: David Armstrong
//
//    Special purpose for 3-pass data; event mode runs with
//  no VDC/HDC tracks
//

//    Takes into account ADC/TDC event number mismatch:
//    For these runs (near Run 17983...) we 
//    find that, for event "i" in the event_tree:
//        maindet_branch  : TDC leafs match TDC hits in branch_event  (i.e fEvent, where I grab 
//                              multihit F1TDC data) for event "i" 
//                        : TDC leafs match ADC hits seen for event "i+6"
//        trigscint_branch: TDC leafs match TDC hits in branch_event "i"
//                        : TDC hits match ADC hits in event "i+6"
//        
//    This makes coding of cuts, etc, rather convoluted. ol
//  Usage: first you must put the rootfile which contains 
//   the data under the $QW_ROOTFILES directory and then type:
//   project_root(package, octant, runnumber,events)
//   where "package" = package number (1 or 2)
//         "octant" = main detector octant number (1 to 8)
//         "runnumber" = run number
//         "events" is number of events to analyze (default is whole run)
//   
//    can't cut on MD multiple hits, as this would bias us away from charged
//     events which typically have large MD pulse-heights which can then re-trigger TDC
//
//
//      study prompt MD "AND"s as well as "OR"s
//
//
//      is there a wide ADC gate here? hclog suggests maybe not
//      why are there large pulseheight TS ADC values but neither 
//      TS has been hit (time==0) and there is not a prompt MD hit?
//        (seen in pedestals)
//      
#include<iostream>
#include<fstream>
#include <string>
#include <TF1.h>
#include <stdio.h>
#include <stdlib.h>

TCanvas  *c;

void project_root(int run_number=6327,int start_events=0,int max_events=-1,TString file_suffix="Qweak_",string command="MD_")
{

  gROOT->SetBatch(1);  // don't write canvasses to screen


  TString outputPrefix(Form("3pass_run%d_",run_number));
  TString runPrefix(Form("run %d_",run_number));
  TString file_name = "";
  file_name += gSystem->Getenv ( "QW_ROOTFILES" );
  file_name +="/";
  file_name += file_suffix;
  file_name += run_number;
  file_name +=".root";
  
  TFile *file = new TFile ( file_name );
  cout << " rootfile = " << file_name << endl;

  TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );

  QwEvent* fEvent=0;
  QwPartialTrack* pt=0;
  QwTreeLine* tl=0;
  // QwHit* hits=0;

  TH1D* trigger = new TH1D("trigger", "Octant that triggered", 10, 0, 10);
  TH1D* trigger_m = new TH1D("trigger_m", "Octants that triggered", 10, 0, 10);
  TH1D* md1padc = new TH1D("md1padc", "md1+ adc randoms", 200, 0, 1000);
  TH1D* md1madc = new TH1D("md1madc", "md1- adc randoms", 200, 0, 1000);
  TH1D* md5padc = new TH1D("md5padc", "md5+ adc randoms", 200, 0, 1000);
  TH1D* md5madc = new TH1D("md5madc", "md5- adc randoms", 200, 0, 1000);

  TH1D* ts1padc = new TH1D("ts1padc", "TS1+ adc", 200, 0, 1000);
  TH1D* ts1madc = new TH1D("ts1madc", "TS1- adc", 200, 0, 1000);
  TH1D* ts2padc = new TH1D("ts2padc", "TS2+ adc", 200, 0, 1000);
  TH1D* ts2madc = new TH1D("ts2madc", "TS2- adc", 200, 0, 1000);

  TH1D* ts1sum_p = new TH1D("ts1sum_p", "TS1 sum adc prompt MD5", 200, 0, 2000);
  TH1D* ts2sum_p = new TH1D("ts2sum_p", "TS2 sum adc prompt MD1", 200, 0, 2000);
  TH1D* ts1sum_pp = new TH1D("ts1sum_pp", "TS1 sum prompt MD5 pre/post", 200, 0, 2000);
  TH1D* ts2sum_pp = new TH1D("ts2sum_pp", "TS2 sum prompt MD1 pre/post", 200, 0, 2000);
  TH1D* ts1sum_ped = new TH1D("ts1sum_ped", "TS1 sum adc pedestal", 200, 0, 400);
  TH1D* ts2sum_ped = new TH1D("ts2sum_ped", "TS2 sum adc pedestal MD1", 200, 0, 400);

  TH1D* ts1padc_p = new TH1D("ts1padc_p", "TS1+ adc prompt", 200, 0, 1000);
  TH1D* ts1madc_p = new TH1D("ts1madc_p", "TS1- adc prompt", 200, 0, 1000);
  TH1D* ts2padc_p = new TH1D("ts2padc_p", "TS2+ adc prompt", 200, 0, 1000);
  TH1D* ts2madc_p = new TH1D("ts2madc_p", "TS2- adc prompt", 200, 0, 1000);

  TH1D* ts1padc_ped = new TH1D("ts1padc_ped", "TS1+ adc pedestal", 200, 0, 400);
  TH1D* ts1madc_ped = new TH1D("ts1madc_ped", "TS1- adc pedestal", 200, 0, 400);
  TH1D* ts2padc_ped = new TH1D("ts2padc_ped", "TS2+ adc pedestal", 200, 0, 400);
  TH1D* ts2madc_ped = new TH1D("ts2madc_ped", "TS2- adc pedestal", 200, 0, 400);

  TH1D* md1padc_p = new TH1D("md1padc_p", "md1+ adc data prompt", 200, 0, 1000);
  TH1D* md1madc_p = new TH1D("md1madc_p", "md1- adc data prompt", 200, 0, 1000);
  TH1D* md5padc_p = new TH1D("md5padc_p", "md5+ adc data prompt", 200, 0, 1000);
  TH1D* md5madc_p = new TH1D("md5madc_p", "md5- adc data prompt", 200, 0, 1000);

  TH1D* md1pskip = new TH1D("md1pskip", "md1+ adc data skip", 200, 0, 1000);
  TH1D* md1mskip = new TH1D("md1mskip", "md1- adc data skip", 200, 0, 1000);
  TH1D* md5pskip = new TH1D("md5pskip", "md5+ adc data skip", 200, 0, 1000);
  TH1D* md5mskip = new TH1D("md5mskip", "md5- adc data skip", 200, 0, 1000);

  TH1D* md2ptdc = new TH1D("md2ptdc", "MD 2 + TDC all hits", 220, -900, 1300);
  TH1D* md2mtdc = new TH1D("md2mtdc", "MD 2 - TDC all hits", 220, -900, 1300);
  TH1D* md3ptdc = new TH1D("md3ptdc", "MD 3 + TDC all hits", 220, -900, 1300);
  TH1D* md3mtdc = new TH1D("md3mtdc", "MD 3 - TDC all hits", 220, -900, 1300);
  TH1D* md4ptdc = new TH1D("md4ptdc", "MD 4 + TDC all hits", 220, -900, 1300);
  TH1D* md4mtdc = new TH1D("md4mtdc", "MD 4 - TDC all hits", 220, -900, 1300);
  TH1D* md6ptdc = new TH1D("md6ptdc", "MD 6 + TDC all hits", 220, -900, 1300);
  TH1D* md6mtdc = new TH1D("md6mtdc", "MD 6 - TDC all hits", 220, -900, 1300);
  TH1D* md7ptdc = new TH1D("md7ptdc", "MD 7 + TDC all hits", 220, -900, 1300);
  TH1D* md7mtdc = new TH1D("md7mtdc", "MD 7 - TDC all hits", 220, -900, 1300);
  TH1D* md8ptdc = new TH1D("md8ptdc", "MD 8 + TDC all hits", 220, -900, 1300);
  TH1D* md8mtdc = new TH1D("md8mtdc", "MD 8 - TDC all hits", 220, -900, 1300);

  TH1D* md1ptdc = new TH1D("md1ptdc", "MD 1 + TDC all hits", 220, -900, 1300);
  TH1D* md1mtdc = new TH1D("md1mtdc", "MD 1 - TDC all hits", 220, -900, 1300);
  TH1D* md5ptdc = new TH1D("md5ptdc", "MD 5 + TDC all hits", 220, -900, 1300);
  TH1D* md5mtdc = new TH1D("md5mtdc", "MD 5 - TDC all hits", 220, -900, 1300);

  TH1D* md1ptdc_t = new TH1D("md1ptdc_t", "MD 1 + TDC all hits", 150, -250, -100);
  TH1D* md1mtdc_t = new TH1D("md1mtdc_t", "MD 1 - TDC all hits", 150, -250, -100);
  TH1D* md5ptdc_t = new TH1D("md5ptdc_t", "MD 5 + TDC all hits", 150, -250, -100);
  TH1D* md5mtdc_t = new TH1D("md5mtdc_t", "MD 5 - TDC all hits", 150, -250, -100);

  TH1D* md1_diff = new TH1D("md1_diff", "(MD1+)-(MD1-) time all hits", 100, -100, 100);

  TH1D* ts1ptdc = new TH1D("ts1ptdc", "TS 1 + TDC first hit", 1100, -900, 1300);
  TH1D* ts1mtdc = new TH1D("ts1mtdc", "TS 1 - TDC first hit", 1100, -900, 1300);
  TH1D* ts2ptdc = new TH1D("ts2ptdc", "TS 2 + TDC first hit", 1100, -900, 1300);
  TH1D* ts2mtdc = new TH1D("ts2mtdc", "TS 2 - TDC first hit", 1100, -900, 1300);

  TH1D* hts1p_t = new TH1D("hts1p_t", "TS 1 + TDC all hits", 1100, -900, 1300);
  TH1D* hts1m_t = new TH1D("hts1m_t", "TS 1 - TDC all hits", 1100, -900, 1300);
  TH1D* hts1mt_t = new TH1D("hts1mt_t", "TS 1 MT TDC all hits", 1100, -900, 1300);
  TH1D* hts2p_t = new TH1D("hts2p_t", "TS 2 + TDC all hits", 1100, -900, 1300);
  TH1D* hts2m_t = new TH1D("hts2m_t", "TS 2 - TDC all hits", 1100, -900, 1300);
  TH1D* hts2mt_t = new TH1D("hts2mt_t", "TS 2 MT TDC all hits", 1100, -900, 1300);

  TH1D* hts1p_tn = new TH1D("hts1p_tn", "TS 1 + TDC all hits", 150, -250, -100);
  TH1D* hts1m_tn = new TH1D("hts1m_tn", "TS 1 - TDC all hits", 150, -250, -100);
  TH1D* hts1mt_tn = new TH1D("hts1mt_tn", "TS 1 MT TDC all hits", 150, -230, -80);
  TH1D* hts2p_tn = new TH1D("hts2p_tn", "TS 2 + TDC all hits", 150, -250, -100);
  TH1D* hts2m_tn = new TH1D("hts2m_tn", "TS 2 - TDC all hits", 150, -250, -100);
    TH1D* hts2mt_tn = new TH1D("hts2mt_t", "TS 2 MT TDC all hits", 150, -230, -80);

  TH1D* hts1mt_l = new TH1D("hts1mt_l", "TS1 MT, large TS ADC", 1100, -900, 1300);
  TH1D* hts2mt_l = new TH1D("hts1mt_l", "TS2 MT, large TS ADC", 1100, -900, 1300);
  TH1D* hts1mt_lm = new TH1D("hts1mt_lm", "TS1 MT, large MD5 ADC", 1100, -900, 1300);
  TH1D* hts2mt_lm = new TH1D("hts1mt_lm", "TS2 MT, large MD1 ADC", 1100, -900, 1300);

  TH1D* ts1mt = new TH1D("ts1mt", "TS1 MT first hit", 1100, -900, 1300);
  TH1D* ts2mt = new TH1D("ts2mt", "TS2 MT first hit", 1100, -900, 1300);

  TH1D* ts1mt_p = new TH1D("ts1mt_p", "TS1 MT first hit prompt MD 5", 1100, -900, 1300);
  TH1D* ts2mt_p = new TH1D("ts2mt_p", "TS2 MT first hit prompt MD 1", 1100, -900, 1300);

  TH1D* md1adc_sum = new TH1D("md1adc_sum", "md 1 summed ADC spectrum", 400, 0, 4000);
  TH1D* md5adc_sum = new TH1D("md5adc_sum", "md 5 summed ADC spectrum", 400, 0, 4000);

  TH1D* md3adc_sum = new TH1D("md3adc_sum", "md31 summed ADC spectrum", 400, 0, 4000);

  TH1D* md1adc_charged = new TH1D("md1adc_charged", "md 1 sum charged", 400, 0, 4000);
  TH1D* md5adc_charged = new TH1D("md5adc_charged", "md 5 sum charged", 400, 0, 4000);
  TH1D* md1adc_neutral = new TH1D("md1adc_neutral", "md 1 sum neutral", 400, 0, 4000);
  TH1D* md5adc_neutral = new TH1D("md5adc_neutral", "md 5 sum neutral", 400, 0, 4000);

  TH1D* md1adc_charged_t = new TH1D("md1adc_charged_t", "md 1 sum charged, TS TDC cut", 400, 0, 4000);
  TH1D* md5adc_charged_t = new TH1D("md5adc_charged_t", "md 5 sum charged, TS TDC cut", 400, 0, 4000);
  TH1D* md1adc_neutral_t = new TH1D("md1adc_neutral_t", "md 1 sum neutral, TS TDC cut", 400, 0, 4000);
  TH1D* md5adc_neutral_t = new TH1D("md5adc_neutral_t", "md 5 sum neutral, TS TDC cut", 400, 0, 4000);

  TH1D* md1adc_charged_pp = new TH1D("md1adc_charged_pp", "md 1 sum charged pre/post", 400, 0, 4000);
  TH1D* md5adc_charged_pp = new TH1D("md5adc_charged_pp", "md 5 sum charged pre/post", 400, 0, 4000);
  TH1D* md1adc_neutral_pp = new TH1D("md1adc_neutral_pp", "md 1 sum neutral pre/post", 400, 0, 4000);
  TH1D* md5adc_neutral_pp = new TH1D("md5adc_neutral_pp", "md 5 sum neutral pre/post", 400, 0, 4000);

  TH1D* md5adc_pedestal = new TH1D("md5adc_pedestal", "md 5 sum pedestal", 200, 0, 800);
  TH1D* md1adc_pedestal = new TH1D("md1adc_pedestal", "md 1 sum pedestal", 200, 0, 800);



  TH1D* nts1mt = new TH1D("nts1mt", "Hits/Event TS1 meantimer", 10, 0, 10);
  TH1D* nts2mt = new TH1D("nts2mt", "Hits/Event TS2 meantimer", 10, 0, 10);
  TH1D* nts1p = new TH1D("nts1p", "Hits/Event TS1 +PMT", 10, 0, 10);
  TH1D* nts2p = new TH1D("nts2p", "Hits/Event TS2 +PMT", 10, 0, 10);
  TH1D* nts1m = new TH1D("nts1m", "Hits/Event TS1 -PMT", 10, 0, 10);
  TH1D* nts2m = new TH1D("nts2m", "Hits/Event TS2 -PMT", 10, 0, 10);

  TH1D* nmd1m = new TH1D("nmd1m", "Hits/Event MD1 -PMT", 10, 0, 10);
  TH1D* nmd1p = new TH1D("nmd1p", "Hits/Event MD1 +PMT", 10, 0, 10);
  TH1D* nmd5m = new TH1D("nmd5m", "Hits/Event MD5 -PMT", 10, 0, 10);
  TH1D* nmd5p = new TH1D("nmd5p", "Hits/Event MD5 +PMT", 10, 0, 10);


  TBranch* branch_event=event_tree->GetBranch("events");
  TBranch* branch     = event_tree->GetBranch("maindet");
  TBranch* branch_ts  = event_tree->GetBranch("trigscint");
  branch_event->SetAddress(&fEvent);

  TLeaf* md1p=branch->GetLeaf(Form("md%dp_adc",1));
  TLeaf* md1m=branch->GetLeaf(Form("md%dm_adc",1));
  TLeaf* md5p=branch->GetLeaf(Form("md%dp_adc",5));
  TLeaf* md5m=branch->GetLeaf(Form("md%dm_adc",5));

  TLeaf* md3p=branch->GetLeaf(Form("md%dp_adc",3));
  TLeaf* md3m=branch->GetLeaf(Form("md%dm_adc",3));

  TLeaf* ts1p=branch_ts->GetLeaf(Form("ts%dp_adc",1));
  TLeaf* ts1m=branch_ts->GetLeaf(Form("ts%dm_adc",1));
  TLeaf* ts2p=branch_ts->GetLeaf(Form("ts%dp_adc",2));
  TLeaf* ts2m=branch_ts->GetLeaf(Form("ts%dm_adc",2));

  TLeaf* md1p_t=branch->GetLeaf(Form("md%dp_f1",1));
  TLeaf* md1m_t=branch->GetLeaf(Form("md%dm_f1",1));
  TLeaf* md5p_t=branch->GetLeaf(Form("md%dp_f1",5));
  TLeaf* md5m_t=branch->GetLeaf(Form("md%dm_f1",5));
  TLeaf* ts1p_t=branch_ts->GetLeaf(Form("ts%dp_f1",1));
  TLeaf* ts1m_t=branch_ts->GetLeaf(Form("ts%dm_f1",1));
  TLeaf* ts2p_t=branch_ts->GetLeaf(Form("ts%dp_f1",2));
  TLeaf* ts2m_t=branch_ts->GetLeaf(Form("ts%dm_f1",2));

  TLeaf* md2p_t=branch->GetLeaf(Form("md%dp_f1",2));
  TLeaf* md2m_t=branch->GetLeaf(Form("md%dm_f1",2));
  TLeaf* md3p_t=branch->GetLeaf(Form("md%dp_f1",3));
  TLeaf* md3m_t=branch->GetLeaf(Form("md%dm_f1",3));
  TLeaf* md4p_t=branch->GetLeaf(Form("md%dp_f1",4));
  TLeaf* md4m_t=branch->GetLeaf(Form("md%dm_f1",4));
  TLeaf* md6p_t=branch->GetLeaf(Form("md%dp_f1",6));
  TLeaf* md6m_t=branch->GetLeaf(Form("md%dm_f1",6));
  TLeaf* md7p_t=branch->GetLeaf(Form("md%dp_f1",7));
  TLeaf* md7m_t=branch->GetLeaf(Form("md%dm_f1",7));
  TLeaf* md8p_t=branch->GetLeaf(Form("md%dp_f1",8));
  TLeaf* md8m_t=branch->GetLeaf(Form("md%dm_f1",8));

  TLeaf* ts1mt_t=branch_ts->GetLeaf(Form("ts%dmt_f1",1));
  TLeaf* ts2mt_t=branch_ts->GetLeaf(Form("ts%dmt_f1",2));

  QwTrack* track=0;

  Int_t nevents=event_tree->GetEntries();
  cout << "Number of Triggers  = " << nevents << endl; 
  if (max_events == -1)max_events = nevents; 


  for (int i=start_events;i<max_events;i++)
    {
      bool prompt_md1 = false;
      bool prompt_md3 = false;
      bool prompt_md5 = false;
      bool pedestal_ts1 = false;
      bool pedestal_ts2 = false;
      bool charged_ts1 = false;
      bool charged_ts2 = false;
      bool neutral_ts1 = false;
      bool neutral_ts2 = false;
      bool pre_ts1 = false;
      bool pre_ts2 = false;
      bool post_ts1 = false;
      bool post_ts2 = false;
      bool prompt_ts1 = false;
      bool prompt_ts2 = false;
      bool early_hit_md1 = false;
      bool early_hit_md5 = false;
      bool md1_hit = false;
      bool md2_hit = false;
      bool md3_hit = false;
      bool md4_hit = false;
      bool md5_hit = false;
      bool md6_hit = false;
      bool md7_hit = false;
      bool md8_hit = false;
      int n_trigger = 0;

      if(i%10000==0) cout << "events processed so far: " << i << std::endl;
      branch_event->GetEntry(i);
      branch->GetEntry(i);
      branch_ts->GetEntry(i);

// look at all hits in multihit F1 TDCs 
// Region ==4 is the TrigScint
//     plane = 1 or 2, meaning package 1 or 2
//     element = 0 is meantimer
//     element = 1 is + PMT
//     element = 2 is - PMT
// Region ==5 is the MainDet
//     plane = MD octant number (1 through 8)
//     element = 0 is meantimer
//     element = 1 is + PMT
//     element = 2 is - PMT
     
      int nhit_ts1_mt = 0;
      int nhit_ts1_p = 0;
      int nhit_ts1_m = 0;
      int nhit_ts2_mt = 0;
      int nhit_ts2_p = 0;
      int nhit_ts2_m = 0;

      int nhit_md1_p = 0;
      int nhit_md1_m = 0;
      int nhit_md5_p = 0;
      int nhit_md5_m = 0;

      Double_t md1_p_time =0;
      Double_t md1_m_time =0;
      bool Any_TS1_hit = false;
      bool Any_TS2_hit = false;
      
      // loop over all hits in event
      int nhits = fEvent->GetNumberOfHits();
      for(int j=0;j<nhits;++j){
	hit=fEvent->GetHit(j);
	if (hit->GetRegion()==4){
	  if (hit->GetPlane()==1){
	    Any_TS1_hit = true;
	    if (hit->GetElement()==0) nhit_ts1_mt++;
	    if (hit->GetElement()==0) hts1mt_t->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==0) hts1mt_tn->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==1) nhit_ts1_p++;
	    if (hit->GetElement()==1) hts1p_t->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==1) hts1p_tn->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==2) nhit_ts1_m++;
	    if (hit->GetElement()==2) hts1m_t->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==2) hts1m_tn->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==0 && (hit->GetTimeNs()<-170) && (hit->GetTimeNs()>-400)) pre_ts1 = true;
	    if (hit->GetElement()==0 && (hit->GetTimeNs()>-115) && (hit->GetTimeNs()<0)) post_ts1 = true;
	    if (hit->GetElement()==0 && (hit->GetTimeNs()<-125) && (hit->GetTimeNs()>-170)) prompt_ts1 = true;
	  }
	  if (hit->GetPlane()==2){
	    Any_TS2_hit = true;
	    if (hit->GetElement()==0) nhit_ts2_mt++;
	    if (hit->GetElement()==0) hts2mt_t->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==0) hts2mt_tn->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==1) nhit_ts2_p++;
	    if (hit->GetElement()==1) hts2p_t->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==1) hts2p_tn->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==2) nhit_ts2_m++;
	    if (hit->GetElement()==2) hts2m_t->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==2) hts2m_tn->Fill(hit->GetTimeNs());
	    if (hit->GetElement()==0 && (hit->GetTimeNs()<-175) && (hit->GetTimeNs()>-400)) pre_ts2 = true;
	    if (hit->GetElement()==0 && (hit->GetTimeNs()>-122) && (hit->GetTimeNs()<0)) post_ts2 = true;
	    if (hit->GetElement()==0 && (hit->GetTimeNs()<-120) && (hit->GetTimeNs()>-165)) prompt_ts2 = true;
	  }
	}

	if (hit->GetRegion()==5){

	  if (hit->GetPlane()==2 && hit->GetElement()==1) 
	    md2ptdc->Fill(hit->GetTimeNs());
	  if (hit->GetPlane()==2 && hit->GetElement()==2) 
	    md2mtdc->Fill(hit->GetTimeNs());

	  if (hit->GetPlane()==3 && hit->GetElement()==1) 
	    md3ptdc->Fill(hit->GetTimeNs());
	  if (hit->GetPlane()==3 && hit->GetElement()==2) 
	    md3mtdc->Fill(hit->GetTimeNs());

	  if (hit->GetPlane()==4 && hit->GetElement()==1) 
	    md4ptdc->Fill(hit->GetTimeNs());
	  if (hit->GetPlane()==4 && hit->GetElement()==2) 
	    md4mtdc->Fill(hit->GetTimeNs());

	  if (hit->GetPlane()==6 && hit->GetElement()==1) 
	    md6ptdc->Fill(hit->GetTimeNs());
	  if (hit->GetPlane()==6 && hit->GetElement()==2) 
	    md6mtdc->Fill(hit->GetTimeNs());

	  if (hit->GetPlane()==7 && hit->GetElement()==1) 
	    md7ptdc->Fill(hit->GetTimeNs());
	  if (hit->GetPlane()==7 && hit->GetElement()==2) 
	    md7mtdc->Fill(hit->GetTimeNs());

	  if (hit->GetPlane()==8 && hit->GetElement()==1) 
	    md8ptdc->Fill(hit->GetTimeNs());
	  if (hit->GetPlane()==8 && hit->GetElement()==2) 
	    md8mtdc->Fill(hit->GetTimeNs());

	  if (hit->GetPlane()==1 && hit->GetElement()==1) 
	    { nhit_md1_p++;
	      md1ptdc->Fill(hit->GetTimeNs());
	      md1ptdc_t->Fill(hit->GetTimeNs());
	      if (hit->GetTimeNs() < -230) early_hit_md1=true; 
	      md1_p_time = hit->GetTimeNs();
	    } 
	  if (hit->GetPlane()==1 && hit->GetElement()==2) 
	    { nhit_md1_m++;
	      md1mtdc->Fill(hit->GetTimeNs());
	      md1mtdc_t->Fill(hit->GetTimeNs());
	      if (hit->GetTimeNs() < -230) early_hit_md1=true; 
	      md1_m_time = hit->GetTimeNs();
	    } 
	  if (hit->GetPlane()==5 && hit->GetElement()==1) 
	    { nhit_md5_p++;
	      md5ptdc->Fill(hit->GetTimeNs());
	      md5ptdc_t->Fill(hit->GetTimeNs());
	      if (hit->GetTimeNs() < -230) early_hit_md5=true; 
	    } 
	  if (hit->GetPlane()==5 && hit->GetElement()==2) 
	    { nhit_md5_m++;
	      md5mtdc->Fill(hit->GetTimeNs());
	      md5mtdc_t->Fill(hit->GetTimeNs());
	      if (hit->GetTimeNs() < -230) early_hit_md5=true; 
	    } 
  	}

	// diagnostic outputs here
	//		if(hit->GetRegion()==5)
		  //		  cout << " Region 5 event " << i << "  hit value= " <<  hit->GetTimeNs() << "  plane = " << hit->GetPlane() << " element = " << hit->GetElement() <<
		  //		    "  hit number = " <<	hit->GetHitNumber() << endl ;
	//	if(hit->GetRegion()==4)
	//	  if(hit->GetPlane()==1 ){
	    //	  cout << " Region 4 event " << i << "  hit value= " <<  hit->GetTimeNs() << "  plane = " << hit->GetPlane() << " element = " << hit->GetElement() << 
	    //	    "  hit number = " <<  hit->GetHitNumber() << endl;
	//	  }
      }
      if((md1_p_time !=0) && (md1_m_time !=0)) md1_diff->Fill(md1_p_time - md1_m_time);

      // fill multipicity histograms
      nts1mt->Fill(nhit_ts1_mt);
      nts1p->Fill(nhit_ts1_p);
      nts1m->Fill(nhit_ts1_m);
      nts2mt->Fill(nhit_ts2_mt);
      nts2p->Fill(nhit_ts2_p);
      nts2m->Fill(nhit_ts2_m);
      nmd1p->Fill(nhit_md1_p);
      nmd1m->Fill(nhit_md1_m);
      nmd5p->Fill(nhit_md5_p);
      nmd5m->Fill(nhit_md5_m);

      // How many prompts are in other 6 octants?
      // just use the single leaf (first hits) for now.

      if ((md2p_t->GetValue() >-230) && (md2p_t->GetValue() < -170) 
	  &&(md2m_t->GetValue() > -230) && (md2m_t->GetValue() < -170)) 
	{
	  //	cout << " md2 prompt hit, event = " << i << endl;
	md2_hit = true;
	trigger->Fill(2);
	n_trigger++;
	}

      if ((md3p_t->GetValue() >-230) && (md3p_t->GetValue() < -170)
	  &&(md3m_t->GetValue() > -230) && (md3m_t->GetValue() < -170)) 
	{
	  //	cout << " md3 prompt hit, event = " << i << endl;
	md3_hit = true;
	trigger->Fill(3);
	n_trigger++;
	}

      if ((md4p_t->GetValue() >-230) && (md4p_t->GetValue() < -170)
	  &&(md4m_t->GetValue() > -230) && (md4m_t->GetValue() < -170)) 
	{
	  //	cout << " md4 prompt hit, event = " << i << endl;
	md4_hit = true;
	trigger->Fill(4);
	n_trigger++;
	}
      if ((md6p_t->GetValue() >-230) && (md6p_t->GetValue() < -170) 
	  &&(md6m_t->GetValue() > -230) && (md6m_t->GetValue() < -170)) 
	{
	  //	cout << " md6 prompt hit, event = " << i << endl;
	md6_hit = true;
	trigger->Fill(6);
	n_trigger++;
	}
      if ((md7p_t->GetValue() >-230) && (md7p_t->GetValue() < -170) 
	  &&(md7m_t->GetValue() > -230) && (md7m_t->GetValue() < -170)) 
	{
	  //	cout << " md7 prompt hit, event = " << i << endl;
	md7_hit = true;
	trigger->Fill(7);
	n_trigger++;
	}
      if ((md8p_t->GetValue() >-230) && (md8p_t->GetValue() < -170)
	  &&(md8m_t->GetValue() > -230) && (md8m_t->GetValue() < -170)) 
	{
	  //	cout << " md8 prompt hit, event = " << i << endl;
	md8_hit = true;
	trigger->Fill(7);
	n_trigger++;
	}
      if ((md1p_t->GetValue() >-230) && (md1p_t->GetValue() < -170)
	  &&(md1m_t->GetValue() > -230) && (md1m_t->GetValue() < -170)) 
	{
	  //	cout << " md1 prompt hit, event = " << i << endl;
	md1_hit = true;
	trigger->Fill(1);
	n_trigger++;
	}
      if ((md5p_t->GetValue() >-230) && (md5p_t->GetValue() < -170)
	  &&(md5m_t->GetValue() > -230) && (md5m_t->GetValue() < -170)) 
	{
	  //	cout << " md5 prompt hit, event = " << i << endl;
	md5_hit = true;
	trigger->Fill(5);
	n_trigger++;
	}
      
      //      cout << "Event = " << i << "  Trigger multiplicity = " << n_trigger << endl;
      trigger_m->Fill(n_trigger);

      // this bit only looks at first hits in TDC channels
	  Double_t p1dataplus = md1p_t->GetValue();
 	  Double_t p1dataminus = md1m_t->GetValue();

	  Double_t p3dataplus = md3p_t->GetValue();
 	  Double_t p3dataminus = md3m_t->GetValue();

	  Double_t ts1p_v = ts1p_t->GetValue();
	  if (ts1p_v !=0 )ts1ptdc->Fill(ts1p_v); 
	  Double_t ts1m_v = ts1m_t->GetValue();
	  if (ts1m_v !=0 )ts1mtdc->Fill(ts1m_v); 
	  Double_t ts2p_v = ts2p_t->GetValue();
	  if (ts2p_v !=0 )ts2ptdc->Fill(ts2p_v); 
	  Double_t ts2m_v = ts2m_t->GetValue();
	  if (ts2m_v !=0 )ts2mtdc->Fill(ts2m_v); 

	  Double_t ts1mt_v = ts1mt_t->GetValue();
	  if (ts1mt_v != 0) ts1mt->Fill(ts1mt_v); 
	  Double_t ts2mt_v = ts2mt_t->GetValue();
	  if (ts2mt_v != 0) ts2mt->Fill(ts2mt_v); 
	  if (ts1p_v == 0 && ts2p_v==0){
	    /*
	      // some diagnostic outputs here
	  cout << " event " << i << "  md1+ first hit tdc = " << p1dataplus << endl; 
	  cout << " event " << i << "  md1- first hit tdc = " << p1dataminus << endl; 
	  cout << " event " << i << "  ts1p_v = " << ts1p_v << endl; 
	  cout << " event " << i << "  ts1m_v = " << ts1m_v << endl; 
	  cout << " event " << i << "  ts2p_v = " << ts2p_v << endl; 
	  cout << " event " << i << "  ts2m_v = " << ts2m_v << endl; 
	  cout << "----------------------------------------------------------" << endl;
	    */
	  }
	  Double_t p1adcp = md1p->GetValue();
	  md1padc->Fill(p1adcp); 
	  Double_t p1adcm = md1m->GetValue();
	  md1madc->Fill(p1adcm); 

	  Double_t p5dataplus = md5p_t->GetValue();
	  Double_t p5dataminus = md5m_t->GetValue();
	  Double_t p5adcp = md5p->GetValue();
	  md5padc->Fill(p5adcp); 
	  Double_t p5adcm = md5m->GetValue();
	  md5madc->Fill(p5adcm); 

	  if(ts1p_v==0 && ts1m_v==0 && p5dataplus ==0 && p5dataminus ==0) pedestal_ts1=true;
	  if(ts2p_v==0 && ts2m_v==0 && p1dataplus ==0 && p1dataminus==0) pedestal_ts2=true;

	  //  	  if ((p1dataplus !=0) && (p1dataminus !=0)) {  
 	  if ((p3dataplus !=0) && (p3dataminus !=0)) {  
	    if (p3dataplus > -230 && p3dataplus < -170 && p3dataminus > -230 && p3dataminus < -170) prompt_md3 = true; 
	  }

 	  if ((p1dataplus !=0) && (p1dataminus !=0)) {  
	    if (p1dataplus > -230 && p1dataplus < -170 && p1dataminus > -230 && p1dataminus < -170) prompt_md1 = true; 
	    //	    md1ptdc->Fill(p1dataplus);
	    //	    md1mtdc->Fill(p1dataminus);
	    if (prompt_md1){
	      if(ts2mt_v != 0) ts2mt_p->Fill(ts2mt_v); 

	      //	      cout << " Event " << i << "  md1 plus time = " << p1dataplus << endl;
	      //	      cout << " Event " << i << "  md1 minus time = " << p1dataminus << endl;
	      //	      cout << " Event " << i << "  md1 plus adc = " << p1adcp << endl;
	      //	      cout << " Event " << i << "  md1 minus adc = " << p1adcm << endl;
	    }
	  }

	  if ((p5dataplus !=0) && (p5dataminus !=0)) {  
	    if (p5dataplus > -230 && p5dataplus < -170 && p5dataminus > -230 && p5dataminus < -170) prompt_md5 = true; 
	    md5ptdc->Fill(p5dataplus);
	    md5mtdc->Fill(p5dataminus);

	    if (prompt_md5){
	      if(ts1mt_v != 0) ts1mt_p->Fill(ts1mt_v); 
	      //    	      cout << " Event " << i << "  md5 plus time = " << p5dataplus << endl;
	      //	      cout << " Event " << i << "  md5 minus time = " << p5dataminus << endl;
	      //	      cout << " Event " << i << "  md5 plus adc = " << p5adcp << endl;
	      //	      cout << " Event " << i << "  md5 minus adc = " << p5adcm << endl;
	    }
	  }

	  // Here is where ADC/TDC event number mismatch for 3-pass data 
	  // is accounted for - skip 6 events ahead to get correct ADC
	  // values for this event
	  branch->GetEntry(i+6);
	  branch_ts->GetEntry(i+6);
	  Double_t ts1p_v = ts1p->GetValue();
	  ts1padc->Fill(ts1p_v); 
	  Double_t ts1m_v = ts1m->GetValue();
	  ts1madc->Fill(ts1m_v); 
	  Double_t ts2p_v = ts2p->GetValue();
	  ts2padc->Fill(ts2p_v); 
	  Double_t ts2m_v = ts2m->GetValue();
	  ts2madc->Fill(ts2m_v); 

	  Double_t p1adcp_skip = md1p->GetValue();
	  Double_t p1adcm_skip = md1m->GetValue();
	  Double_t p5adcp_skip = md5p->GetValue();
	  Double_t p5adcm_skip = md5m->GetValue();
  
	  Double_t p3adcp_skip = md3p->GetValue();
	  Double_t p3adcm_skip = md3m->GetValue();

	  // Fill TS time spectrum for large pulses in TS ADC and MD ADC

	  for(int j=0;j<nhits;++j){
	    hit=fEvent->GetHit(j);
	    if (hit->GetRegion()==4){
	      if (hit->GetPlane()==1){
		if ((hit->GetElement()==0) && ((ts1p_v+ts1m_v)>250) ) hts1mt_l->Fill(hit->GetTimeNs());
		if ((hit->GetElement()==0) && ((p5adcp_skip+p5adcm_skip)>500) ) hts1mt_lm->Fill(hit->GetTimeNs());
	      }
	      if (hit->GetPlane()==2){
		if ((hit->GetElement()==0) && ((ts2p_v+ts2m_v)>250) ) hts2mt_l->Fill(hit->GetTimeNs());
		if ((hit->GetElement()==0) && ((p1adcp_skip+p1adcm_skip)>500) ) hts2mt_lm->Fill(hit->GetTimeNs());
	      }
	    }
	  }


	  if (pedestal_ts1 && (!Any_TS1_hit)){
	    ts1padc_ped->Fill(ts1p_v); 
	    ts1madc_ped->Fill(ts1m_v); 
	    ts1sum_ped->Fill(ts1p_v+ts1m_v); 
	    if (!prompt_md5){
	      md5adc_pedestal->Fill(p5adcp_skip+p5adcm_skip); 
	    }
	  }
	  if (pedestal_ts2 && (!Any_TS2_hit)){
	    ts2padc_ped->Fill(ts2p_v); 
	    ts2madc_ped->Fill(ts2m_v); 
	    ts2sum_ped->Fill(ts2p_v+ts2m_v); 
	    if (!prompt_md1){
	      md1adc_pedestal->Fill(p1adcp_skip+p1adcm_skip); 
	    }
	  }
	    if ((p1dataplus !=0) || (p1dataminus !=0)) {  
	      //	      if(prompt_md1 && (nhit_md1_m==1)&& (nhit_md1_p==1)){
	      if(prompt_md1 && (!early_hit_md1)){
		//	            cout << "  Event " << i << "  forward skip md1 plus adc = " << p1adcp_skip << endl;
		//		    cout << "  Event " << i << "  forward skip md1 minus adc = " << p1adcm_skip << endl;
		ts2padc_p->Fill(ts2p_v); 
		ts2madc_p->Fill(ts2m_v); 
		ts2sum_p->Fill(ts2m_v+ts2p_v); 
		if (!pre_ts2 && !post_ts2){
		  ts2sum_pp->Fill(ts2m_v+ts2p_v); 
		}
		if ((ts2m_v+ts2p_v)> 300) charged_ts2=true;
		if ((ts2m_v+ts2p_v)< 300) neutral_ts2=true;
		md1padc_p->Fill(p1adcp_skip); 
	        md1madc_p->Fill(p1adcm_skip);
		md1adc_sum->Fill(p1adcp_skip+p1adcm_skip); 
		if(charged_ts2)md1adc_charged->Fill(p1adcp_skip+p1adcm_skip); 
		if(neutral_ts2)md1adc_neutral->Fill(p1adcp_skip+p1adcm_skip); 
		if (!pre_ts2 && !post_ts2){
		  if(charged_ts2)md1adc_charged_pp->Fill(p1adcp_skip+p1adcm_skip); 
		  if(neutral_ts2)md1adc_neutral_pp->Fill(p1adcp_skip+p1adcm_skip); 
		  if(prompt_ts2)md1adc_charged_t->Fill(p1adcp_skip+p1adcm_skip); 
		  if(!prompt_ts2)md1adc_neutral_t->Fill(p1adcp_skip+p1adcm_skip); 
		}
	      }
	    }
	    if (prompt_md3) md3adc_sum->Fill(p3adcp_skip+p3adcm_skip); 
	    if ((p5dataplus !=0) || (p5dataminus !=0)) {  
      
	      //	      if(prompt_md5 && (nhit_md5_m==1)&& (nhit_md5_p==1)){
	      if(prompt_md5 && (!early_hit_md5)){
		//	            cout << "  Event " << i << "  forward skip md5 plus adc = " << p5adcp_skip << endl;
		//		    cout << "  Event " << i << "  forward skip md5 minus adc = " << p5adcm_skip << endl;
		ts1padc_p->Fill(ts1p_v); 
		ts1madc_p->Fill(ts1m_v); 
		ts1sum_p->Fill(ts1m_v+ts1p_v); 
		if (!pre_ts1 && !post_ts1){
		  //		if (!pre_ts1){
		  ts1sum_pp->Fill(ts1m_v+ts1p_v); 
		}
		if ((ts1m_v+ts1p_v)> 300) charged_ts1=true;
		if ((ts1m_v+ts1p_v)< 300) neutral_ts1=true;
		md5padc_p->Fill(p5adcp_skip); 
	        md5madc_p->Fill(p5adcm_skip);
		md5adc_sum->Fill(p5adcp_skip+p5adcm_skip); 
		if(charged_ts1)md5adc_charged->Fill(p5adcp_skip+p5adcm_skip); 
		if(neutral_ts1)md5adc_neutral->Fill(p5adcp_skip+p5adcm_skip); 
		if (!pre_ts1 && !post_ts1){
		  if(charged_ts1)md5adc_charged_pp->Fill(p5adcp_skip+p5adcm_skip); 
		  if(neutral_ts1)md5adc_neutral_pp->Fill(p5adcp_skip+p5adcm_skip); 
		  if(prompt_ts1)md5adc_charged_t->Fill(p5adcp_skip+p5adcm_skip); 
		  if(!prompt_ts1)md5adc_neutral_t->Fill(p5adcp_skip+p5adcm_skip); 
		}
	      }
	    }
	  }
  

  TFile f(Form("3pass_%d.root",run_number),"new");
  md1adc_sum->Write();
  md5adc_sum->Write();
  md3adc_sum->Write();
  md1adc_charged->Write();
  md1adc_neutral->Write();
  md5adc_charged->Write();
  md5adc_neutral->Write();

  c2 = new TCanvas("c2","MD ADCs randoms",10, 10, 600,600);
  c2->Divide(2,2);
  gStyle->SetStatH(0.3);
  c2->cd(1);
  md1padc->Draw();
  c2->cd(2);
  md1madc->Draw();
  c2->cd(3);
  md5padc->Draw();
  c2->cd(4);
  md5madc->Draw();
  c2->SaveAs(outputPrefix+"MD_ADCs.pdf");


  c3 = new TCanvas("c3","MD corrected ADCs",10, 10, 700,700);
  c3->Divide(1,2);
  gStyle->SetStatH(0.3);
  c3->cd(1);
  md1adc_sum->Draw();
  c3->cd(2);
  md5adc_sum->Draw();
  c3->SaveAs(outputPrefix+"MD_corrected_ADCs.pdf");

  c3a = new TCanvas("c3a","MD charged and neutral ADCs",10, 10, 700,700);
  c3a->Divide(2,3);
  gStyle->SetStatH(0.3);
  c3a->cd(1);
  md1adc_charged->Draw();
  c3a->cd(2);
  md5adc_charged->Draw();
  c3a->cd(3);
  md1adc_neutral->Draw();
  c3a->cd(4);
  md5adc_neutral->Draw();
  c3a->cd(5);
  md1adc_pedestal->Draw();
  c3a->cd(6);
  md5adc_pedestal->Draw();
  c3a->SaveAs(outputPrefix+"MD_charged_neutral_ADCs.pdf");

  c3d = new TCanvas("c3d","MD charged, neutral, pedestal ADCs",10, 10, 700,700);
  c3d->Divide(1,2);
  gStyle->SetStatH(0.3);
  c3d->cd(1);
  md1adc_charged_pp->Draw();
  md1adc_neutral_pp->SetLineColor(2);
  md1adc_neutral_pp->Draw("SAME");
  md1adc_pedestal->SetLineColor(4);
  md1adc_pedestal->Draw("SAME");
  c3d->cd(2);
  md5adc_charged_pp->Draw();
  md5adc_neutral_pp->SetLineColor(2);
  md5adc_neutral_pp->Draw("SAME");
  md5adc_pedestal->SetLineColor(4);
  md5adc_pedestal->Draw("SAME");
  c3d->SaveAs(outputPrefix+"MD_overlay_all_ADCs.pdf");

  c3e = new TCanvas("c3e","MD charged, neutral, pedestal ADCs",10, 10, 700,700);
  c3e->Divide(1,2);
  gStyle->SetStatH(0.3);
  c3e->cd(1);
  TH1D *h3 = (TH1D*)md1adc_charged_pp->Clone("md1");
  md1->Draw();
  md1->GetXaxis()->SetRangeUser(300,1000);
  md1->Draw();
  md1adc_neutral_pp->SetLineColor(2);
  md1adc_neutral_pp->Draw("SAME");
  md1adc_pedestal->SetLineColor(4);
  md1adc_pedestal->Draw("SAME");
  c3e->cd(2);
  TH1D *h3 = (TH1D*)md5adc_charged_pp->Clone("md5");
  md5->Draw();
  md5->GetXaxis()->SetRangeUser(300,1000);
  md5->Draw();
  md5adc_charged_pp->Draw();
  md5adc_charged_pp->GetXaxis()->SetRangeUser(300,1000);
  md5adc_charged_pp->Draw();
  md5adc_neutral_pp->SetLineColor(2);
  md5adc_neutral_pp->Draw("SAME");
  md5adc_pedestal->SetLineColor(4);
  md5adc_pedestal->Draw("SAME");
  c3e->SaveAs(outputPrefix+"MD_overlay_ADCs.pdf");


  c3b = new TCanvas("c3b","MD charged and neutral pre/post ADCs",10, 10, 700,700);
  c3b->Divide(2,3);
  gStyle->SetStatH(0.3);
  c3b->cd(1);
  md1adc_charged_pp->Draw();
  c3b->cd(2);
  md5adc_charged_pp->Draw();
  c3b->cd(3);
  md1adc_neutral_pp->Draw();
  c3b->cd(4);
  md5adc_neutral_pp->Draw();
  c3b->cd(5);
  md1adc_pedestal->Draw();
  c3b->cd(6);
  md5adc_pedestal->Draw();
  c3b->SaveAs(outputPrefix+"MD_charged_neutral_pp_ADCs.pdf");

  c3f = new TCanvas("c3f","MD charged and neutral pre/post ADCs, TS TDC cut",10, 10, 700,700);
  c3f->Divide(2,3);
  gStyle->SetStatH(0.3);
  c3f->cd(1);
  md1adc_charged_t->Draw();
  c3f->cd(2);
  md5adc_charged_t->Draw();
  c3f->cd(3);
  md1adc_neutral_t->Draw();
  c3f->cd(4);
  md5adc_neutral_t->Draw();
  c3f->cd(5);
  md1adc_pedestal->Draw();
  c3f->cd(6);
  md5adc_pedestal->Draw();
  c3f->SaveAs(outputPrefix+"MD_charged_neutral_tdc_ADCs.pdf");

  c4 = new TCanvas("c4","MD TDCs all hits",10, 10, 600,600);
  c4->Divide(2,2);
  gStyle->SetStatH(0.3);
  c4->cd(1);
  md1ptdc->Draw();
  c4->cd(2);
  md1mtdc->Draw();
  c4->cd(3);
  md5ptdc->Draw();
  c4->cd(4);
  md5mtdc->Draw();
  c4->SaveAs(outputPrefix+"MD_TDCs_all.pdf");

  c4b = new TCanvas("c4b","other MD TDCs all hits",10, 10, 900,900);
  c4b->Divide(4,3);
  c4b->cd(1);
  md2ptdc->Draw();
  c4b->cd(2);
  md2mtdc->Draw();
  c4b->cd(3);
  md3ptdc->Draw();
  c4b->cd(4);
  md3mtdc->Draw();
  c4b->cd(5);
  md4ptdc->Draw();
  c4b->cd(6);
  md4mtdc->Draw();
  c4b->cd(7);
  md6ptdc->Draw();
  c4b->cd(8);
  md6mtdc->Draw();
  c4b->cd(9);
  md7ptdc->Draw();
  c4b->cd(10);
  md7mtdc->Draw();
  c4b->cd(11);
  md8ptdc->Draw();
  c4b->cd(12);
  md8mtdc->Draw();
  c4b->SaveAs(outputPrefix+"MD_TDCs_other.pdf");

  c4c = new TCanvas("c4c","other MD TDCs late randoms",10, 10, 900,900);
  c4c->Divide(4,4);

  gStyle->SetStatH(0.3);
  std::ofstream fout;
  fout.open(outputPrefix+"randoms.dat");
  fout << "Run " << run_number << endl;
  fout << "TriggersAnalyzed = " << max_events << endl;
  c4c->cd(1);
  md1ptdc->Draw();
  md1ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md1p singles rate (kHz) = " << (1000./max_events)*(md1ptdc->Integral(91,191)) << endl;
  fout << "md1p singles rate (kHz) = " << (1000./max_events)*(md1ptdc->Integral(91,191)) << endl;
  c4c->cd(2);
  md1mtdc->Draw();
  md1mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md1m singles rate (kHz) = " << (1000./max_events)*md1mtdc->Integral(91,191) << endl;
  fout << "md1m singles rate (kHz) = " << (1000./max_events)*md1mtdc->Integral(91,191) << endl;
  c4c->cd(3);
  md2ptdc->Draw();
  md2ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md2p singles rate (kHz) = " << (1000./max_events)*md2ptdc->Integral(91,191) << endl;
  fout << "md2p singles rate (kHz) = " << (1000./max_events)*md2ptdc->Integral(91,191) << endl;
  c4c->cd(4);
  md2mtdc->Draw();
  md2mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md2m singles rate (kHz) = " << (1000./max_events)*md2mtdc->Integral(91,191) << endl;
  fout << "md2m singles rate (kHz) = " << (1000./max_events)*md2mtdc->Integral(91,191) << endl;
  c4c->cd(5);
  md3ptdc->Draw();
  md3ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md3p singles rate (kHz) = " << (1000./max_events)*md3ptdc->Integral(91,191) << endl;
  fout << "md3p singles rate (kHz) = " << (1000./max_events)*md3ptdc->Integral(91,191) << endl;
  c4c->cd(6);
  md3mtdc->Draw();
  md3mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md3m singles rate (kHz) = " << (1000./max_events)*md3mtdc->Integral(91,191) << endl;
  fout << "md3m singles rate (kHz) = " << (1000./max_events)*md3mtdc->Integral(91,191) << endl;
  c4c->cd(7);
  md4ptdc->Draw();
  md4ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md4p singles rate (kHz) = " << (1000./max_events)*md4ptdc->Integral(91,191) << endl;
  fout << "md4p singles rate (kHz) = " << (1000./max_events)*md4ptdc->Integral(91,191) << endl;
  c4c->cd(8);
  md4mtdc->Draw();
  md4mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md4m singles rate (kHz) = " << (1000./max_events)*md4mtdc->Integral(91,191) << endl;
  fout << "md4m singles rate (kHz) = " << (1000./max_events)*md4mtdc->Integral(91,191) << endl;
  c4c->cd(9);
  md5ptdc->Draw();
  md5ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md5p singles rate (kHz) = " << (1000./max_events)*md5ptdc->Integral(91,191) << endl;
  fout << "md5p singles rate (kHz) = " << (1000./max_events)*md5ptdc->Integral(91,191) << endl;
  c4c->cd(10);
  md5mtdc->Draw();
  md5mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md5m singles rate (kHz) = " << (1000./max_events)*md5mtdc->Integral(91,191) << endl;
  fout << "md5m singles rate (kHz) = " << (1000./max_events)*md5mtdc->Integral(91,191) << endl;
  c4c->cd(11);
  md6ptdc->Draw();
  md6ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md6p singles rate (kHz) = " << (1000./max_events)*md6ptdc->Integral(91,191) << endl;
  fout << "md6p singles rate (kHz) = " << (1000./max_events)*md6ptdc->Integral(91,191) << endl;
  c4c->cd(12);
  md6mtdc->Draw();
  md6mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md6m singles rate (kHz) = " << (1000./max_events)*md6mtdc->Integral(91,191) << endl;
  fout << "md6m singles rate (kHz) = " << (1000./max_events)*md6mtdc->Integral(91,191) << endl;
  c4c->cd(13);
  md7ptdc->Draw();
  md7ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md7p singles rate (kHz) = " << (1000./max_events)*md7ptdc->Integral(91,191) << endl;
  fout << "md7p singles rate (kHz) = " << (1000./max_events)*md7ptdc->Integral(91,191) << endl;
  c4c->cd(14);
  md7mtdc->Draw();
  md7mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md7m singles rate (kHz) = " << (1000./max_events)*md7mtdc->Integral(91,191) << endl;
  fout << "md7m singles rate (kHz) = " << (1000./max_events)*md7mtdc->Integral(91,191) << endl;
  c4c->cd(15);
  md8ptdc->Draw();
  md8ptdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md8p singles rate (kHz) = " << (1000./max_events)*md8ptdc->Integral(91,191) << endl;
  fout << "md8p singles rate (kHz) = " << (1000./max_events)*md8ptdc->Integral(91,191) << endl;
  c4c->cd(16);
  md8mtdc->Draw();
  md8mtdc->GetXaxis()->SetRangeUser(0,1200);
  cout << "md8m singles rate (kHz) = " << (1000./max_events)*md8mtdc->Integral(91,191) << endl;
  fout << "md8m singles rate (kHz) = " << (1000./max_events)*md8mtdc->Integral(91,191) << endl;

  c4c->SaveAs(outputPrefix+"MD_TDCs_randoms.pdf");


  c4a = new TCanvas("c4a","MD TDCs all hits",10, 10, 600,600);
  c4a->Divide(2,2);
  gStyle->SetStatH(0.3);
  c4a->cd(1);
  md1ptdc_t->Draw();
  c4a->cd(2);
  md1mtdc_t->Draw();
  c4a->cd(3);
  md5ptdc_t->Draw();
  c4a->cd(4);
  md5mtdc_t->Draw();
  c4a->SaveAs(outputPrefix+"MD_TDCs_all_zoom.pdf");

  c5 = new TCanvas("c5","MD ADCs prompt",10, 10, 600,600);
  c5->Divide(2,2);
  gStyle->SetStatH(0.3);
  c5->cd(1);
  md1padc_p->Draw();
  c5->cd(2);
  md1madc_p->Draw();
  c5->cd(3);
  md5padc_p->Draw();
  c5->cd(4);
  md5madc_p->Draw();
  c5->SaveAs(outputPrefix+"MD_ADCs_prompt.pdf");

  c6 = new TCanvas("c6","TS ADCs ",10, 10, 600,600);
  c6->Divide(2,2);
  gStyle->SetStatH(0.3);
  c6->cd(1);
  ts2padc->Draw();
  c6->cd(2);
  ts2madc->Draw();
  c6->cd(3);
  ts1padc->Draw();
  c6->cd(4);
  ts1madc->Draw();
  c6->SaveAs(outputPrefix+"TS_ADCs.pdf");

  c7 = new TCanvas("c7","TS ADCs prompt",10, 10, 600,600);
  c7->Divide(2,2);
  gStyle->SetStatH(0.3);
  c7->cd(1);
  ts2padc_p->Draw();
  c7->cd(2);
  ts2madc_p->Draw();
  c7->cd(3);
  ts1padc_p->Draw();
  c7->cd(4);
  ts1madc_p->Draw();
  c7->SaveAs(outputPrefix+"TS_ADCs_prompt.pdf");

  c9 = new TCanvas("c9","TS ADCs pedestals",10, 10, 600,600);
  c9->Divide(3,2);
  gStyle->SetStatH(0.3);
  c9->cd(1);
  ts2padc_ped->Draw();
  c9->cd(2); 
  ts2madc_ped->Draw();
  c9->cd(3);
  ts2sum_ped->Draw();
  c9->cd(4);
  ts1padc_ped->Draw();
  c9->cd(5);
  ts1madc_ped->Draw();
  c9->cd(6);
  ts1sum_ped->Draw();
  c9->SaveAs(outputPrefix+"TS_ADCs_pedestals.pdf");


  c8 = new TCanvas("c8","TS TDCs first hits",10, 10, 600,600);
  c8->Divide(2,2);
  gStyle->SetStatH(0.3);
  c8->cd(1);
  ts2ptdc->Draw();
  c8->cd(2);
  ts2mtdc->Draw();
  c8->cd(3);
  ts1ptdc->Draw();
  c8->cd(4);
  ts1mtdc->Draw();
  c8->SaveAs(outputPrefix+"TS_TDCs_firsts.pdf");


  c11 = new TCanvas("c11","TS MT TDCs first hits",10, 10, 600,600);
  c11->Divide(2,2);
  gStyle->SetStatH(0.3);
  c11->cd(1);
  ts2mt->Draw();
  c11->cd(2);
  ts1mt->Draw();
  c11->cd(3);
  ts2mt_p->Draw();
  c11->cd(4);
  ts1mt_p->Draw();
  c11->SaveAs(outputPrefix+"TS__MT_TDCs_firsts.pdf");

  c10 = new TCanvas("c10","TS TDCs nhits",10, 10, 600,600);
  c10->Divide(3,4);
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);
  c10->cd(1);
  nts1mt->Draw();
  c10->cd(2);
  nts1m->Draw();
  c10->cd(3);
  nts1p->Draw();
  c10->cd(4);
  nts2mt->Draw();
  c10->cd(5);
  nts2m->Draw();
  c10->cd(6);
  nts2p->Draw();
  c10->cd(8);
  nmd1m->Draw();
  c10->cd(9);
  nmd1p->Draw();
  c10->cd(11);
  nmd5m->Draw();
  c10->cd(12);
  nmd5p->Draw();
  c10->SaveAs(outputPrefix+"TS_nhits.pdf");


  c12 = new TCanvas("c12","TS TDCs all hits",10, 10, 600,600);
  c12->Divide(3,2);
  gStyle->SetStatH(0.3);
  c12->cd(1);
  hts2m_t->Draw();
  c12->cd(2);
  hts2p_t->Draw();
  c12->cd(3);
  hts2mt_t->Draw();
  c12->cd(4);
  hts1p_t->Draw();
  c12->cd(5);
  hts1m_t->Draw();
  c12->cd(6);
  hts1mt_t->Draw();
  c12->SaveAs(outputPrefix+"TS_TDCs_all.pdf");

  c13 = new TCanvas("c13","TS TDCs all hits zoom",10, 10, 600,600);
  c13->Divide(3,2);
  gStyle->SetStatH(0.3);
  c13->cd(1);
  hts2m_tn->Draw();
  c13->cd(2);
  hts2p_tn->Draw();
  c13->cd(3);
  hts2mt_tn->Draw();
  c13->cd(4);
  hts1p_tn->Draw();
  c13->cd(5);
  hts1m_tn->Draw();
  c13->cd(6);
  hts1mt_tn->Draw();
  c13->SaveAs(outputPrefix+"TS_TDCs_all_zoom.pdf");

  c14 = new TCanvas("c14","MD diff",10, 10, 400,400);
  c14->cd();
  md1_diff->Draw();
  c14->SaveAs(outputPrefix+"MD1_diff.pdf");

  c18 = new TCanvas("c18","MD diff",10, 10, 400,400);
  c18->cd();
  md3adc_sum->Draw();
  c18->SaveAs(outputPrefix+"MD3_sum.pdf");

  c14a = new TCanvas("c14a","Trigger",10, 10, 300,300);
  c14a->Divide(1,2);
  c14a->cd(1);
  trigger->Draw();
  c14a->cd(2);
  gPad->SetLogy();
  trigger_m->Draw();
  c14a->SaveAs(outputPrefix+"trigger.pdf");

  c15 = new TCanvas("c15","TS TDCs large pulse",10, 10, 600,600);
  c15->Divide(2,2);
  gStyle->SetStatH(0.3);
  c15->cd(1);
  hts1mt_l->Draw();
  c15->cd(2);
  hts2mt_l->Draw();
  c15->cd(3);
  hts1mt_lm->Draw();
  c15->cd(4);
  hts2mt_lm->Draw();
  c15->SaveAs(outputPrefix+"TS_TDCs_large.pdf");

  c16 = new TCanvas("c16","TS sum ADC prompt MD",10, 10, 600,600);
  c16->Divide(2,2);
  gStyle->SetStatH(0.3);
  c16->cd(1);
  ts2sum_p->Draw();
  c16->cd(2);
  ts1sum_p->Draw();
  c16->cd(3);
  ts2sum_pp->Draw();
  c16->cd(4);
  ts1sum_pp->Draw();
  c16->SaveAs(outputPrefix+"TS_sum_ADCs_promptMD.pdf");

  c17 = new TCanvas("c17","TS sum ADC prompt MD zoom",10, 10, 600,800);
  c17->Divide(2,3);
  gStyle->SetStatH(0.3);
  c17->cd(1);
  ts2sum_p->Draw();
  c17->cd(2);
  ts1sum_p->Draw();
  c17->cd(3);
  gPad->SetLogy();
  ts2sum_p->Draw();
  c17->cd(4);
  gPad->SetLogy();
  ts1sum_p->Draw();
  c17->cd(5);
  TH1D *h2 = (TH1D*)ts2sum_pp->Clone("ts2");
  TH1D *h1 = (TH1D*)ts1sum_pp->Clone("ts1");
  ts2->GetXaxis()->SetRangeUser(1,300);
  ts2->Draw();
  cout << "TS2 no hits prompt MD1 = " << ts2->Integral() << endl;
  cout << "all prompt MD1 = " << ts2sum_pp->Integral() << endl;
  c17->cd(6);
  ts1->GetXaxis()->SetRangeUser(1,300);
  ts1->Draw();
  cout << "TS1 no hits prompt MD5 = " << ts1->Integral() << endl;
  cout << "all prompt MD5 = " << ts1sum_pp->Integral() << endl;
  c17->SaveAs(outputPrefix+"TS_sum_ADCs_promptMD_zoom.pdf");
			   
  /*
  pedestal_mean = fopen("pedestal_mean.txt", "a");
  if (pedestal_mean == NULL)
    {
      cout << "pedestal_mean did not open" << endl;
    }

  fprintf(pedestal_mean, "%d \t %d \t %d \t %f \t %f \t %f \t %d\n", run_number, package, md_number, adcpmean, adcmmean, effmean, trial_total);

  fclose(pedestal_mean);
  */
 
  return;
};


