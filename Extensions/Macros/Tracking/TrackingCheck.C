// author: Siyuan Yang
//       : College of William & Mary
//       : sxyang@email.wm.edu
//this script contains many tools(functions) to diagnose tracking analysis
//  function "efficiency" will return how many qualified events the run has; 
//  we define the qualified events as those in which every plane in the VDCs 
//     must have minimum number of hits required to perform the pattern recognition search.
// function "slope" plots the slope diagnosis information for Region 3 
// function "residual" plots the average residual of plane0 and single plane.
//
//  DSA: lots of cleanup and additions, July 2012
//     : added function "trigger"
//     : added function "r2angle"
//     : added function "diagnostics"
//  DSA: added MD tests to "efficiency", and lots more diagnostic outputs, Feb 2013
 
#include <iostream>
#include <iomanip> // for setprecision
#include <iterator>
#include <vector>
#include <algorithm>


using namespace std;

const double wirespace=0.496965;


void efficiency(int ev_start=-1,int ev_end=-1,Int_t run_number=6327, int octant1=5, int octant2=1){

  // octant1= MD sector for package 1, octant2  = MD sector for package 1 

    Int_t start=0,end=0;
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
    ofstream events_log;
    events_log.open(Form("%s/failed_events_list.txt",gSystem->Getenv ( "QW_ROOTFILES" )));
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwPartialTrack* pt=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();

    if(ev_start==-1 && ev_end==-1) 
      {start=0;end=nevent;}
        else {
	  start=ev_start;end=ev_end;}

    TBranch* branch_event=event_tree->GetBranch("events");
    TBranch* branch_trig=event_tree->GetBranch("trigscint");
    TBranch* branch_main=event_tree->GetBranch("maindet");
    branch_event->SetAddress(&fEvent);

    TLeaf* trig1=branch_trig->GetLeaf("ts1mt_f1");
    TLeaf* trig2=branch_trig->GetLeaf("ts2mt_f1");
    TLeaf* md1p=branch_main->GetLeaf(Form("md%dp_f1",octant1));  
    TLeaf* md1m=branch_main->GetLeaf(Form("md%dm_f1",octant1));  
    TLeaf* md2p=branch_main->GetLeaf(Form("md%dp_f1",octant2));  
    TLeaf* md2m=branch_main->GetLeaf(Form("md%dm_f1",octant2));  

    int qualified_events_1=0;
    int qualified_events_2=0;
    int md_triggered_events_1=0;
    int md_triggered_events_2=0;
    int md_qualified_events_1=0;
    int md_qualified_events_2=0;
    int total_r3_pt_1=0;
    int total_r3_pt_2=0;
    int total_r2_pt_1=0;
    int total_r2_pt_2=0;
    int trig_1=0;
    int trig_2=0;
    int md_r2_pt_1=0;
    int md_r3_pt_1=0;
    int md_r2_r3_pt_1=0;
    int md_vq_r2_r3_pt_1=0;
    int md_vq_r2_pt_1=0;
    int md_r2_pt_2=0;
    int md_r3_pt_2=0;
    int md_r2_r3_pt_2=0;
    int md_vq_r2_r3_pt_2=0;
    int md_vq_r2_pt_2=0;
    int track_1=0;
    int track_2=0;
    int octant_mix_1=0;
    int octant_mix_2=0;
    int vq_r3_pt_1=0;
    int vq_r3_pt_2=0;

    for(int i=start;i< end;i++){
        if(i%10000==0)
        cout << "events process so far: " << i << endl;  
        event_tree->GetEntry(i);

        int nhits=fEvent->GetNumberOfHits();
        int npts=fEvent->GetNumberOfPartialTracks();
	int ntracks=fEvent->GetNumberOfTracks();
        int a[8]={0};            //represents eight planes, in case both packages get hits
        bool has_r3_pt_1=false;
        bool has_r2_pt_1=false;
        bool has_r3_pt_2=false;
        bool has_r2_pt_2=false;
        bool has_track_1=false;
        bool has_track_2=false;
        bool qualified_1=false;
        bool qualified_2=false;

	if(ntracks>0){
	  track=fEvent->GetTrack(0);
	  if(track->GetPackage()==1)has_track_1=true;
	  if(track->GetPackage()==2)has_track_2=true;
	}
	//   Below: count the number of Partial Tracks found in Region 3
        for(int j=0;j<npts;j++){ 
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==3){
                  if(pt->GetPackage()==1){
		    total_r3_pt_1++; 
		    has_r3_pt_1=true;
		  }
                  if(pt->GetPackage()==2){
		    total_r3_pt_2++;
		    has_r3_pt_2=true;
		  }
                }
        }         
	//   Below: count the number of Partial Tracks found in Region 2
        for(int j=0;j<npts;j++){ 
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==2){
                  if(pt->GetPackage()==1){
		    total_r2_pt_1++; 
		    has_r2_pt_1=true;
		  }
                  if(pt->GetPackage()==2){
		    total_r2_pt_2++;
		    has_r2_pt_2=true;
		  }
                }
        }         
                      
	// Now check which octant fired the TS using the trigger scint Meantime TDC
	//  check for TDC hits in both MD phototubes (no tight window on timing)
	// then look for partial tracks in R2 and R3

        if(trig1->GetValue()!=0) {
	  trig_1++;
	  if(has_r2_pt_2) octant_mix_1++;
	  if(md1m->GetValue()<-1&&md1p->GetValue()<-1) {
	    md_triggered_events_1++;
	    if(has_r2_pt_1) md_r2_pt_1++;
	    if(has_r3_pt_1) md_r3_pt_1++;
	    if(has_r2_pt_1&&has_r3_pt_1)=md_r2_r3_pt_1++;
	    if(has_r2_pt_1&&has_r3_pt_1&&has_track_1)=track_1++;
	  }
	}        
	if(trig2->GetValue()!=0) {
	  trig_2++;
	  if(has_r2_pt_1) octant_mix_2++;
	  if(md2m->GetValue()<-1&&md2p->GetValue()<-1) {
	    md_triggered_events_2++;
	    if(has_r2_pt_2) md_r2_pt_2++;
	    if(has_r3_pt_2) md_r3_pt_2++;
	    if(has_r2_pt_2&&has_r3_pt_2)=md_r2_r3_pt_2++;
	    if(has_r2_pt_2&&has_r3_pt_2&&has_track_2)=track_2++;
	  }
	}

	// Count the number of events with at least 4 VDC hits in each of the 4 planes of same package
        for(int j=0;j<nhits;j++){
                hit=fEvent->GetHit(j);
                if(hit->GetRegion()==3){
                   a[(hit->GetPackage()-1)*4+hit->GetPlane()-1]++;
                }
        }
        
                if(a[0]>=4&&a[1]>=4&&a[2]>=4&&a[3]>=4){
		        qualified_1=true;
		        qualified_events_1++;
			if(has_r3_pt_1) vq_r3_pt_1++;
			if(md1m->GetValue()<-1&&md1p->GetValue()<-1) {
			  md_qualified_events_1++;
			  if(has_r2_pt_1) md_vq_r2_pt_1++;
			  if(has_r2_pt_1&&has_r3_pt_1) md_vq_r2_r3_pt_1++;
			// the next two lines for debugging track reconstruction
			                        if(!has_r3_pt_1 && has_r2_pt_1)
						  events_log << "event: " << i << "  in Package 1" <<endl;
			}
                }
                if(a[4]>=4&&a[5]>=4&&a[6]>=4&&a[7]>=4){
		        qualified_2=true;
                        qualified_events_2++;
			if(has_r3_pt_2) vq_r3_pt_2++;
			if(md2m->GetValue()<-1&&md2p->GetValue()<-1) {
			  md_qualified_events_2++;
			  if(has_r2_pt_2) md_vq_r2_pt_2++;
			  if(has_r2_pt_2&&has_r3_pt_2) md_vq_r2_r3_pt_2++;
			// the next two lines for debugging track reconstruction
			                        if(!has_r3_pt_2 && has_r2_pt_2)
						  events_log << "event: " << i << "  in Package 2" <<endl;       
			}
                }
        //
        }
    cout << "Qualified Events are those with at least 4 wires hit in all 4 VDC planes in that package" << endl;

    cout << endl << "PACKAGE 1: "  << endl << endl;

    cout << "Triggers : " << trig_1 << endl;
    cout << "MD triggered Events : " << md_triggered_events_1 << endl;
    cout << "MD triggered with R2 partial tracks : " << md_r2_pt_1 << endl;
    cout << "MD triggered with R3 partial tracks : " << md_r3_pt_1 << endl;
    cout << "MD triggered with R2 and R3 partial tracks : " << md_r2_r3_pt_1 << endl;
    cout << "VDC Qualified Events : " << qualified_events_1 << endl;
    cout << "VDC Qualified Events with R3 partial track : " << vq_r3_pt_1 << endl;
    cout << "MD.VDC Qualified Events : " << md_qualified_events_1 << endl;
    cout << "MD.VDC Qualified with R2 partial tracks : " << md_vq_r2_pt_1 << endl;
    cout << "MD.VDC Qualified with R2 and R3 partial tracks : " << md_vq_r2_r3_pt_1 << endl;
    cout << "   R3 partial track efficiency: " << (double)md_r2_r3_pt_1/md_r2_pt_1*100 << " %" <<endl;
    cout << "   R2 partial track efficiency: " << (double)md_r2_r3_pt_1/md_r3_pt_1*100 << " %" <<endl;
    cout << "   R2.R3 partial track efficiency: " << (double)md_r2_r3_pt_1/md_triggered_events_1*100 << " %" <<endl;
    cout << "   Bridged Track efficiency: " << (double)track_1/md_triggered_events_1*100 << " %" <<endl;
    cout << "   Bridging efficiency: " << (double)track_1/md_r2_r3_pt_1*100 << " %" <<endl<<endl;

    cout << "TS trigger in Package 1 but R2 partial track in Package 2: " << octant_mix_1 << endl << endl;

    cout << "PACKAGE 2: "  << endl << endl;

    cout << "Triggers : " << trig_2 << endl;
    cout << "MD triggered Events : " << md_triggered_events_2 << endl;
    cout << "MD triggered with R2 partial tracks : " << md_r2_pt_2 << endl;
    cout << "MD triggered with R3 partial tracks : " << md_r3_pt_2 << endl;
    cout << "MD triggered with R2 and R3 partial tracks : " << md_r2_r3_pt_2 << endl;
    cout << "VDC Qualified Events : " << qualified_events_2 << endl;
    cout << "VDC Qualified Events with R3 partial track : " << vq_r3_pt_2 << endl;
    cout << "MD.VDC Qualified Events : " << md_qualified_events_2 << endl;
    cout << "MD.VDC Qualified with R2 partial tracks : " << md_vq_r2_pt_2 << endl;
    cout << "MD.VDC Qualified with R2 and R3 partial tracks : " << md_vq_r2_r3_pt_2 << endl;
    cout << "   R3 partial track efficiency: " << (double)md_r2_r3_pt_2/md_r2_pt_2*100 << " %" <<endl;
    cout << "   R2 partial track efficiency: " << (double)md_r2_r3_pt_2/md_r3_pt_2*100 << " %" <<endl;
    cout << "   R2.R3 partial track efficiency: " << (double)md_r2_r3_pt_2/md_triggered_events_2*100 << " %" <<endl;
    cout << "   Bridged Track efficiency: " << (double)track_2/md_triggered_events_2*100 << " %" <<endl;
    cout << "   Bridging efficiency: " << (double)track_2/md_r2_r3_pt_2*100 << " %" <<endl<<endl;

    cout << "TS trigger in Package 2 but R2 partial track in Package 1: " << octant_mix_2 << endl << endl;


    cout << "   Trigger Purity:" <<endl;
    cout << " Package 1: MD.VDC.R2.R3/triggers:  " << (double)md_vq_r2_r3_pt_1/trig_1*100 << " %" <<endl;
    cout << " Package 2: MD.VDC.R2.R3/triggers:  " << (double)md_vq_r2_r3_pt_2/trig_2*100 << " %" <<endl;

    events_log.close();

    return;
}


void slope(int ev_start=-1,int ev_end=-1,int run_number=6327){

  // DSA: plots slopes of the Region 3 treelines
  //   and plots difference between slopes in the front and back chambers in a package
  //   need to redo and just plot for the "correct" treelines in each partial track?

    Int_t start=0,end=0;
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
 
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwTreeLine* tl=0;
    QwPartialTrack* pt=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();
    // define the histograms needed later
    TH1F* uslope=new TH1F("abs(slope) in u direction","uslope",400,0,2);
    TH1F* vslope=new TH1F("abs(slope) in v direction","vslope",400,0,2);
    TH1F* uslope_diff=new TH1F("slope_diff in u direction","uslope_diff",200,-1,1 );
    TH1F* vslope_diff=new TH1F("slope_diff in v direction","vslope_diff",200,-1,1 );

    if(ev_start==-1 && ev_end==-1) {start=0;end=nevent;}
        else {
        start=ev_start;end=ev_end;}

    for(int i=start;i< end;i++){
        event_tree->GetEntry(i);
        if(i%10000==0) cout << "events process so far: " << i << endl;
        int ntls=fEvent->GetNumberOfTreeLines();
        int npts=fEvent->GetNumberOfPartialTracks();

        //u,v direction
        double slope[2];
        double fslope[2];
        double bslope[2];
        int reverse=1;        
        bool r3=false;    // if there's any r3 track
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==3 && r3==false){
                 r3=true;
		 if(pt->GetPackage()==1) reverse=-1;
                        else reverse=1;
                 }
        }
        
        if(r3==false) continue;

        for(int j=0;j<ntls;j++){
                
                treeline=fEvent->GetTreeLine(j);
              
                if(treeline->GetRegion()==3 && treeline->GetPlane()==0)
                slope[treeline->GetDirection()-3]=treeline->GetSlope();
                if(treeline->GetRegion()==3 && treeline->GetPlane()!=0){
                        if(treeline->GetPlane()<3)
                          fslope[treeline->GetDirection()-3]=reverse*treeline->GetSlope();
                        else
                          bslope[treeline->GetDirection()-3]=reverse*treeline->GetSlope();
                }
        }
        uslope->Fill(fabs(slope[0]));
        vslope->Fill(fabs(slope[1]));

//         if(((fslope[0]+bslope[0])/(2*wirespace)-slope[0])>1) cout << "event: " << i << endl;
        
        uslope_diff->Fill((fslope[0]+bslope[0])/(2*wirespace)-slope[0]);
        vslope_diff->Fill((fslope[1]+bslope[1])/(2*wirespace)-slope[1]);
        }
        
        TCanvas* c=new TCanvas("slope","slope",600,600);
        c->Divide(2,2);
        c->cd(1);
        uslope->SetLineColor(2);
        uslope->SetTitle("slope in u direction");
        uslope->Draw();
        c->cd(2);
        vslope->SetLineColor(3);
        vslope->SetTitle("slope in v direction");
        vslope->Draw();
        c->cd(3);
        uslope_diff->SetLineColor(4);
        uslope_diff->SetTitle("slope difference in u direction");
        uslope_diff->Draw();
        c->cd(4);
        vslope_diff->SetLineColor(6);
        vslope_diff->SetTitle("slope difference in v direction");
        vslope_diff->Draw();
    return;
}



void residual(int run_number=6327){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
 
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwTreeLine* treeline=0;
    QwPartialTrack* pt=0;
    TH1F* dd=new TH1F("dd","dd",500,-5,5);
    TH1F* dt=new TH1F("dt","dt",700,-100,600);
    TH1F* ar_0=new TH1F("ar_0","ar_0",100,0,0.5);
    TH1F* ar_1=new TH1F("ar_1","ar_1",100,0,0.5);
    
    event_tree->Project("dt","fQwHits.fTimeNs","fQwHits.fRegion==3");
    event_tree->Project("dd","fQwHits.fDistance","fQwHits.fRegion==3");
    event_tree->Project("ar_0","fQwTreeLines.fAverageResidual","fQwTreeLines.fPlane==0&&fQwTreeLines.fRegion==3");
    event_tree->Project("ar_1","fQwTreeLines.fAverageResidual","fQwTreeLines.fPlane!=0&&fQwTreeLines.fRegion==3");

    ar_0->SetLineColor(kRed);
    ar_1->SetLineColor(kBlue);
    ar_0->SetTitle("average residual in plane 0 (R3 treeline)");
    ar_1->SetTitle("average residual in single plane (R3 treeline)");
    dd->SetTitle("Drift Distance, Region 3, all hits");
    dt->SetTitle("Drift Time, Region 3, all hits");
        
    TCanvas* c=new TCanvas("ar","drift time, distance, residuals",700,600);
    c->Divide(2,2);
    c->cd(1);
    dt->Draw();
    c->cd(2);
    dd->Draw();
    c->cd(3);
    ar_0->Draw();
    c->cd(4);
    ar_1->Draw();
    return;
}


void bridge(int ev_start=-1,int ev_end=-1,int run_number=5148){

  // DSA - plots slope in Phi direction for R2 vs slope in Phi for R3
  ///       partial tracks 

    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwPartialTrack* pt=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();

    //    event_tree->Project("dt","fQwHits.fTimeNs","fQwHits.fRegion==3");  
           
    TH2F* histo=new TH2F("r2 vs. r3","r2 phi vs. r3 phi ",200,-0.5,0.5,200,-0.5,0.5);

    int nevents=event_tree->GetEntries();
    if(ev_start==-1 && ev_end==-1) 
      {start=0;end=nevent;}
        else {start=ev_start;end=ev_end;}

    for(int i=start;i< end;i++){
        if(nevents/10000==0) cout << "events process so far: " << i << std::endl;
        event_tree->GetEntry(i);
        int npts=fEvent->GetNumberOfPartialTracks();
        double r2=0,r3=0;
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
		//		cout << j << endl;
                if(pt->GetRegion()==2){
                        r2=pt->fSlopeY/pt->fSlopeX;
			//			cout << "r2 slope " <<r2 << endl;
		}
                else if(pt->GetRegion()==3){
                        r3=pt->fSlopeY/pt->fSlopeX;
			//			cout << " r3 slope " << r3 << endl;
		}
        }
        if(r2 !=0 && r3!=0)
                  histo->Fill(r2,r3);
    }
    TCanvas* c=new TCanvas("R2 vs R3 slope","R2 vs. R3 phi slope",400,400);
    histo->Draw();
}


void vertex(int ev_start=-1, int ev_end=-1,  int run=5148, int package=1, double dz=-500){

  // DSA: not sure exactly what is being plotted here...

    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    event_tree->SetBranchAddress ( "events",&fEvent );

    Int_t nevent=event_tree->GetEntries();

    if(ev_start==-1 && ev_end==-1) 
      {start=0;end=nevent;}
        else {
	  start=ev_start;end=ev_end;}
    
    TH2F* histo=new TH2F(Form("project to z=%f",dz),Form("project to z=%f",dz),400,-50,50,400,-50,50);
    TH1F* hx=new TH1F("vertex x","vertex x",300,-700,-400);
    TH1F* hy=new TH1F("vertex y","vertex y",300,-700,-400);
    TCanvas* c=new TCanvas("c","c",600,600);
    c->Divide(2,2);

    TBranch* branch_event=event_tree->GetBranch("events");
    TBranch* branch_trig=event_tree->GetBranch("trigscint");
    TBranch* branch_main=event_tree->GetBranch("maindet");
    branch_event->SetAddress(&fEvent);

    int nevents=event_tree->GetEntries();

    for(int i=start;i<end;i++){
        if(i%10000==0) cout << "events process so far: " << i << std::endl;
        branch_event->GetEntry(i);
        int npts=fEvent->GetNumberOfPartialTracks();
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                double zx=0,zy=0,x=0,y=0;
                if(pt->GetRegion()==2 && pt->GetPackage()==package){
                    zx=-pt->fOffsetX/pt->fSlopeX;
                    zy=-pt->fOffsetY/pt->fSlopeY;
                    x=pt->fSlopeX*dz+pt->fOffsetX;
                    y=pt->fSlopeY*dz+pt->fOffsetY;
		    if(zx!=0 && zy!=0){
		      hx->Fill(zx);
		      hy->Fill(zy);
		      histo->Fill(x,y);
		    }
		}
	    }
          }

         gStyle->SetPalette(1);
         c->cd(1);
         histo->Draw("colz");
         c->cd(2);
         hx->Draw();
         c->cd(3);
         hy->Draw();
}


void trigger(int ev_start=-1, int ev_end=-1,  int run=5148, int package=1, int octant=5){

  // DSA: built from the efficiency stuff in Siyuan's "vertex" function
  //      no longer so useful, since more functionality is in "efficiency" function.

    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    event_tree->SetBranchAddress ( "events",&fEvent );

    Int_t nevent=event_tree->GetEntries();

    if(ev_start==-1 && ev_end==-1) 
      {start=0;end=nevent;}
        else {
	  start=ev_start;end=ev_end;}
    
    TBranch* branch_event=event_tree->GetBranch("events");
    TBranch* branch_trig=event_tree->GetBranch("trigscint");
    TBranch* branch_main=event_tree->GetBranch("maindet");
    branch_event->SetAddress(&fEvent);

    TLeaf* trig=branch_trig->GetLeaf(Form("ts%dmt_f1",package));
    TLeaf* main=branch_main->GetLeaf(Form("md%dm_f1",octant));  // arbitrarily use MD=negative PMT for now

    int nevents=event_tree->GetEntries();
    int counts=0;
    int counts2=0;
    int counts3=0;
    int counts23=0;
    int trig_counts=0;
    int total_counts=0;
    int only_r3=0,only_r2=0,r23=0;
    for(int i=start;i<end;i++){
        if(i%10000==0) cout << "events process so far: " << i << std::endl;
        branch_event->GetEntry(i);
        branch_trig->GetEntry(i);
        branch_main->GetEntry(i);
        int value=trig->GetValue();
        int npts=fEvent->GetNumberOfPartialTracks();
        bool trig_fired=false,main_fired=false;
        if(trig->GetValue()!=0){
                trig_fired=true;
                trig_counts++;
                }
        if(main->GetValue()!=0)
                main_fired=true;
        if(main->GetValue()!=0 && trig->GetValue()!=0)
	        total_counts++;   

        bool r2=false,r3=false;
        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==2 && pt->GetPackage()==package)r2=true;		
		else if(pt->GetRegion()==3 && pt->GetPackage()==package) r3=true;
	}
        if(r2==true && r3==true && trig_fired==true && main_fired==true) counts++;    
        if(r2==true && r3==true && trig_fired==true) counts23++;    
        if(r2==true && trig_fired==true) counts2++;    
        if(r3==true && trig_fired==true) counts3++;    
    }

    std::cout << "Package " << package << ", only trig fired: " << trig_counts << std::endl;
    std::cout << "Package " << package << ", both trig and main fired: " << total_counts 
	      << "  " << (double)total_counts/(double)trig_counts*100 << " %" << std::endl;
    std::cout << "Package " << package << ", " << counts2  << " of TS events have r2 tracks: " 
         << (double)counts2/(double)trig_counts*100 << " %" << std::endl;
    std::cout << "Package " << package << ", " << counts3  << " of TS events have r3 tracks: " 
         << (double)counts3/(double)trig_counts*100 << " %" << std::endl;
    std::cout << "Package " << package << ", " << counts23  << " of TS events have both r2 and r3 tracks: " 
         << (double)counts23/(double)trig_counts*100 << " %" << std::endl;
    std::cout << "Package " << package << ", " << counts  << " of TS.MD events have both r2 and r3 tracks: " 
         << (double)counts/(double)trig_counts*100 << " %" << std::endl;
}


void r2angle(int ev_start=-1, int ev_end=-1,  int run=5148, int package=1, int octant=5, int output=0){

  // DSA: look at angles from Region 2 partial tracks
  //    : output==1 puts (lots!!) of output to screen (choose only for a few events).

    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    QwTreeLine* treeline=0;
    QwTrack* track=0;
    event_tree->SetBranchAddress ( "events",&fEvent );

    Int_t nevent=event_tree->GetEntries();

    if(ev_start==-1 && ev_end==-1) 
      {start=0;end=nevent;}
    else { start=ev_start; end=ev_end;}

    TBranch* branch_event=event_tree->GetBranch("events");

    branch_event->SetAddress(&fEvent);

    int nevents=event_tree->GetEntries();

    double slopex, slopey, chi, slopexr3, alone, ntreelines; 

    TH1F* r2slopex=new TH1F("R2 Partial Track Theta","r2slopex",100,0,15);
    TH1F* r2slopey=new TH1F("R2 Partial Track Phi","r2slopey",100,-180,180);
    TH1F* r2chi=new TH1F("R2 Partial Track Chi","r2chi",100,0,30);
    TH1F* r23slopex=new TH1F("R2 Partial Track Theta with R3","r23slopex",100,0,15);

    for(int i=start;i<end;i++){
        bool r3_pt=false;

        if(i%1000==0) cout << "events processed so far: " << i << std::endl;
        branch_event->GetEntry(i);

	int ntracks=fEvent->GetNumberOfTracks();
        int npts=fEvent->GetNumberOfPartialTracks();
        int ntrees=fEvent->GetNumberOfTreeLines();
	int region=0, nhits=0;
	int ntree=0;
	
	if (npts>0 && output==1) cout << endl << "   There are " << npts << "  partial tracks in event " << i << endl;

        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
		ntree=pt->GetNumberOfTreeLines();
		region=pt->GetRegion();
		if (region==2){
		  if (output==1) cout << "Package  " << pt->GetPackage() << "  R2: " << ntree << " trees in partial track " << j << " in event " << i << endl;
		for(int k=0;k<ntree;k++){
 		  treeline=pt->GetTreeLine(k);
		  nhits=treeline->GetNumberOfHits();
		  if (output==1)  cout << "R2: in treeline " << k << "  there are  " << nhits << " hits in partial track " << j << " in event " << i << endl;
		}
		}
		if (region==3){
		  if (output==1)  cout << "Package  " << pt->GetPackage() << "  R3: " << ntree << " trees in partial track " << j << " in event " << i << endl;
		for(int k=0;k<ntree;k++){
 		  treeline=pt->GetTreeLine(k);
		  nhits=treeline->GetNumberOfHits();
		  if (output==1)  cout << "R3: in treeline " << k << "  there are  " << nhits << " hits in partial track " << j << " in event " << i << endl;
		}
		}
		if(pt->GetRegion()==3 && pt->GetPackage()==package){r3_pt=true;}
                if(pt->GetRegion()==2 && pt->GetPackage()==package){
		  slopex = pt->GetMomentumDirectionTheta();
		  r2slopex->Fill(slopex*57.29578);
		  slopey = pt->GetMomentumDirectionPhi();
		  r2slopey->Fill(slopey*57.29578);
		  chi = pt->GetChiWeight();
		  r2chi->Fill(chi);
		}
	}


	for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==2 && pt->GetPackage()==package && r3_pt==true){
		    slopexr3 = pt->GetMomentumDirectionTheta();
		    r23slopex->Fill(slopexr3*57.29578);
		}
	  }
    }

    //  DSA: the below is example of how to dump histograms to a new file...
    //    TFile f("dave.root","new");
    //    r2slopex->Write();

	TCanvas* c1=new TCanvas("c1","R2 theta",600,600);
        c1->Divide(2,2);
        c1->cd(1);
        r2slopex->SetLineColor(2);
        r2slopex->SetTitle("R2 Partial Track theta");
        r2slopex->Draw();
	if(r2slopex->GetEntries()) {
	  cout << "theta: " 
	       << setprecision(4) << r2slopex->GetMean() << " error RMS/sqrt(N): " << setprecision(2) << 
	          r2slopex->GetRMS()/sqrt(r2slopex->GetEntries()) << endl; 
	}

        c1->cd(2);
	c1_2->SetLogy();
        r2slopey->SetLineColor(3);
        r2slopey->SetTitle("R2 Partial Track phi");
        r2slopey->Draw();
	if(r2slopey->GetEntries()) {
	   cout << "phi: " << setprecision(4) << r2slopey->GetMean() << " error RMS/sqrt(N): " << setprecision(2) 
		<< r2slopey->GetRMS()/sqrt(r2slopey->GetEntries()) << endl; 
	}

        c1->cd(3);
        r2chi->SetLineColor(4);
        r2chi->SetTitle("R2 Partial Track Chi");
        r2chi->Draw();
	if(r2chi->GetEntries()) {
	    cout << "chi: " << setprecision(4) << r2chi->GetMean() << " error RMS/sqrt(N): " << setprecision(2)
		 << r2chi->GetRMS()/sqrt(r2chi->GetEntries()) << endl; 
	}  

	c1->cd(4);
        r23slopex->SetLineColor(6);
        r23slopex->SetTitle("R2 Partial Track theta, with R3 PT");
        r23slopex->Draw();
	if(r23slopex->GetEntries()) {
	    cout << "theta: " << setprecision(4) << r23slopex->GetMean() << " error RMS/sqrt(N): " << setprecision(2) 
		 << r23slopex->GetRMS()/sqrt(r23slopex->GetEntries()) << endl; 
	}
}

void diagnostics(int ev_start=-1, int ev_end=-1,  int run=5148){

  // DSA: print basic event-by-event diagnostics to cout
  //      starts with treelines

    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    QwTreeLine* tl=0;
    QwHit* hit=0;
    QwTrack* track=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();

    if(ev_start==-1 && ev_end==-1) 
      {start=0;end=nevent;}
    else { start=ev_start; end=ev_end;}

    TBranch* branch_event=event_tree->GetBranch("events");
    branch_event->SetAddress(&fEvent);
    int nevents=event_tree->GetEntries();

    for(int i=start;i<end;i++){

        if(i%1000==0) cout << "events processed so far: " << i << std::endl;
        branch_event->GetEntry(i);

	int ntracks=fEvent->GetNumberOfTracks();
        int npts=fEvent->GetNumberOfPartialTracks();
        int ntrees=fEvent->GetNumberOfTreeLines();
	bool iused=0, ivalid=0;
	int region=0;
	int package=0;
	float tlslope=0; 
	float tlchi=0; 
	float tlresidual=0;
	if (ntrees>0) cout << endl <<  " event " << i << " has " << ntrees << " treelines" << endl;
        for(int j=0;j<ntrees;j++){
	  if(j==0) cout << endl; 

                tl=fEvent->GetTreeLine(j);
		//		iused=tl->IsUsed();
		//		ivalid=tl->IsValid();
		region = tl->GetRegion();
		ntlhits=tl->GetNumberOfHits();
                cout << endl << " event " << i << " Package " << tl->GetPackage() << "  treeline " << j << " Region " << region << " nhits " << ntlhits << endl;  
		tlslope=tl->GetSlope();
		tlchi=tl->GetChi();
		tlresidual=tl->GetAverageResidual();
                cout << "   treeline slope " << tlslope << " Chi " << tlchi << "  Avg Residual " << tlresidual << endl;  
		for(int l=0;l<ntlhits;l++){
		  hit=tl->GetHit(l);
		  cout << "Package " << hit->GetPackage() << "  plane  " << hit->GetPlane() << " wire " << hit->GetElement() << " time " << hit->GetTimeNs() << " drift dist " << hit->GetDriftDistance() << endl;
		}
	}

        for(int j=0;j<npts;j++){
                pt=fEvent->GetPartialTrack(j);
		cout << " event " << i << " partial track number " << j << " is in  Region " << pt->GetRegion() << " Package " << pt->GetPackage() << endl; 
	}

	for(int k=0;k<ntracks;k++){
	  track=fEvent->GetTrack(k);
	  package=track->GetPackage();
	  cout << " event " << i << " Track in package = " << package << " track number " << k << endl;
	}	  
    }
}
