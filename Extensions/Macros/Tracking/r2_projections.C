// Author: David Armstrong
// File: r2_projections.C
// Date: May 30 2014
// 
// Description: This file is used project the Region 2 partial tracks
//    back to three Z-locations: that of Collimator 1, Collimator 2 and the
//    nominal center of the lH2 target, for both packages.
//
//  Based on r2_projection.C, originally written by Siyuan Yang
//
//  Usage: 
//   Root> projections(15121,1,10000)
//
//  Output: 6 Canvases of plots, saved as .png files, 
//  mean and RMS written to r2_projections.txt
//
//
#include <cstring>
#include <cstdlib>
#include <sstream>

// maximum allowed value for #R2 hits 
const int multitracks=18;

double adc_pedestal[9] = {0.0, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000}; // units adc units or a.u. (arbitrary units)

//define a function
  int DetermineOctantRegion3(TTree* event_tree, int package);

void projections(int run=15121, int event_start=0,int event_end=-1,string prefix="", string suffix=""){

   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/Qweak_";
   ss << prefix ;
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());
  
   double z_coll1 = -568.17;
   double z_coll2 = -370.719;
   double z_lh2 = -656.0;


   TH1F* p1_coll1_y=new TH1F("y at coll 1","y at coll 1",200,-20,20);
   TH1F* p1_coll1_x=new TH1F("x at coll 1","x at coll 1",200,-20,20);
   TH1F* p1_coll2_y=new TH1F("y at coll 2","y at coll 2",240,-60,60);
   TH1F* p1_coll2_x=new TH1F("x at coll 2","x at coll 2",240,-60,60);
   TH1F* p1_lh2_y=new TH1F("y at lH2","y at lH2 2",64,-8,8);
   TH1F* p1_lh2_x=new TH1F("x at lH2","x at lH2 2",64,-8,8);

   TH1F* p2_coll1_y=new TH1F("y at coll 1","y at coll 1",200,-20,20);
   TH1F* p2_coll1_x=new TH1F("x at coll 1","x at coll 1",200,-20,20);
   TH1F* p2_coll2_y=new TH1F("y at coll 2","y at coll 2",240,-60,60);
   TH1F* p2_coll2_x=new TH1F("x at coll 2","x at coll 2",240,-60,60);
   TH1F* p2_lh2_y=new TH1F("y at lH2","y at lH2",64,-8,8);
   TH1F* p2_lh2_x=new TH1F("x at lH2","x at lH2",64,-8,8);

   TH1F* p1_vertex=new TH1F("vertex in Z","vertex in Z  (package 1)",200,-850,-450);
   TH2F* p1_projection_coll1=new TH2F("projection","projection",240,-20,20,240,-20,20);
   TH2F* p1_projection_coll2=new TH2F("projection","projection",240,-60,60,240,-60,60);
   TH2F* p1_projection_lh2=new TH2F("projection","projection",40,-8,8,40,-8,8);
   TH1F* p1_Chi_histo=new TH1F("chi","chi in Region 2  (package 1)",200,0,20);

   TH1F* p2_vertex=new TH1F("vertex in Z","vertex in Z  (package 2)",200,-850,-450);
   TH2F* p2_projection_coll1=new TH2F("projection","projection",240,-20,20,240,-20,20);
   TH2F* p2_projection_coll2=new TH2F("projection","projection",240,-60,60,240,-60,60);
   TH2F* p2_projection_lh2=new TH2F("projection","projection",40,-8,8,40,-8,8);
   TH1F* p2_Chi_histo=new TH1F("chi","chi in Region 2  (package 2)",200,0,20);

   QwEvent* fEvent=0;
   QwHit* hit=0;
   QwTrack* track=0;
   QwPartialTrack* pt=0;

   TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );

   Int_t nevents=event_tree->GetEntries();
   cout << "total events: " << nevents << endl;
    
   int start=(event_start==-1)? 0:event_start;
   int end=(event_end==-1)? nevents:event_end;
   event_tree->SetBranchStatus("events",1);
   TBranch* event_branch=event_tree->GetBranch("events");
   TBranch* maindet_branch=event_tree->GetBranch("maindet");
   event_branch->SetAddress(&fEvent);

   int md_1 = DetermineOctantRegion3(event_tree, 1); //octant number for package one
   int md_2 = DetermineOctantRegion3(event_tree, 2); //octant number for package two
   cout << "octant for package 1 = " << md_1 << "  octant for package 2 = " << md_2 << endl; 

    // DSA kludge
    int oct=md_2;

    TLeaf* mdp_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
    TLeaf* mdm_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
    TLeaf* mdp_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
    TLeaf* mdm_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));
   
    // set up track matching cuts
    // DSA should make these more realistic!!

    double mean_thetaoff_pkg1=0,sigma_thetaoff_pkg1=0.01;
    double mean_thetaoff_pkg2=0,sigma_thetaoff_pkg2=0.01;
    double mean_phioff_pkg1=0, sigma_phioff_pkg1=0.02;
    double mean_phioff_pkg2=0,sigma_phioff_pkg2=0.02; 
   
    double width=1000;   // essentially wide-open for now.
    //    double width=3;   // tight cut, 5% acceptance loss

    double pkg1_phioff_lower=mean_phioff_pkg1-width*sigma_phioff_pkg1;
    double pkg1_phioff_upper=mean_phioff_pkg1+width*sigma_phioff_pkg1;
    double pkg2_phioff_lower=mean_phioff_pkg2-width*sigma_phioff_pkg2;
    double pkg2_phioff_upper=mean_phioff_pkg2+width*sigma_phioff_pkg2;

    double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*sigma_thetaoff_pkg1;
    double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*sigma_thetaoff_pkg1;
    double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*sigma_thetaoff_pkg2;
    double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*sigma_thetaoff_pkg2;
  
     // Loop over events for package 1
    int pkg=1;
    for(int i=start;i<end;++i){

      if(i%1000==0) cout << "package 1 events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      double mdp_value_1=mdp_1->GetValue();
      double mdm_value_1=mdm_1->GetValue();
      
   // count number of Region 2 hits 
      int nhits=fEvent->GetNumberOfHits();
      int valid_hits=0;
      for(int j=0;j<nhits;++j){
	hit=fEvent->GetHit(j);
	if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0 && hit->GetPackage() == pkg){
	  ++valid_hits;
	}
      }

   // test if the MD is hit - using TDC hits
   // only looks at first hit in MD's multihit TDCs - not ideal logic
	
      if(mdm_value_1 <-210 || mdm_value_1 > -150 || mdp_value_1 < -210 || mdp_value_1 > -150)
	  continue;  // jump out of this event in for loop if not a prompt MD hit
	
      if(valid_hits>=multitracks) continue;     // jump out of this event in for loop if too many R2 hits

      double ntracks=fEvent->GetNumberOfTracks();
      double npts=fEvent->GetNumberOfPartialTracks();
      double chi=0;

      // now, apply track matching cuts
      // only use first bridged track
      if(ntracks>0){
      int nts=0;
      track=fEvent->GetTrack(nts);
      if(track->GetPackage()!=pkg){
	continue;
      }
      if(track->fDirectionPhioff<pkg1_phioff_lower || track->fDirectionPhioff>pkg1_phioff_upper || track->fDirectionThetaoff < pkg1_thetaoff_lower || track->fDirectionThetaoff > pkg1_thetaoff_upper) continue;

      // Now the event has passed out cuts

      for(int j=0;j<npts;++j){
        pt=fEvent->GetPartialTrack(j);
	// check that the partial track is from the correct package
	if(pt->GetRegion()!=2 || pt->GetPackage()!=pkg || pt->GetPackage()!=track->GetPackage()) continue;

	chi=pt->fChi;
	p1_Chi_histo->Fill(pt->fChi);

	double vertex_z=fEvent->GetScatteringVertexZ();
	p1_vertex->Fill(vertex_z);

	// project partial tracks to three different z-locations
	double x = pt->fOffsetX+z_coll1*pt->fSlopeX;
	double y = pt->fOffsetY+z_coll1*pt->fSlopeY;
	p1_projection_coll1->Fill(x,y); 
        p1_coll1_y->Fill(y);
        p1_coll1_x->Fill(x);

	x = pt->fOffsetX+z_coll2*pt->fSlopeX;
	y = pt->fOffsetY+z_coll2*pt->fSlopeY;
	p1_projection_coll2->Fill(x,y); 
        p1_coll2_y->Fill(y);
        p1_coll2_x->Fill(x);

	x = pt->fOffsetX+z_lh2*pt->fSlopeX;
	y = pt->fOffsetY+z_lh2*pt->fSlopeY;
	p1_projection_lh2->Fill(x,y); 
        p1_lh2_y->Fill(y);
        p1_lh2_x->Fill(x);
      }
      }
    }  // end of for loop over package 1 events
   

    // Loop over events for package 2
    pkg=2;
    for(int i=start;i<end;++i){

      if(i%1000==0) cout << "package 2 events processed so far: " << i << endl;
      //      cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      double mdp_value_2=mdp_2->GetValue();
      double mdm_value_2=mdm_2->GetValue();
      
   // count number of Region 2 hits 
      int nhits=fEvent->GetNumberOfHits();
      int valid_hits=0;
      for(int j=0;j<nhits;++j){
	hit=fEvent->GetHit(j);
	if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0 && hit->GetPackage() == pkg){
	  ++valid_hits;
	}
      }

   // test if the MD is hit - using TDC hits
   // only looks at first hit in MD's multihit TDCs - not ideal logic
	
	if(mdm_value_2 <-210 || mdm_value_2 > -150 || mdp_value_2 < -210 || mdp_value_2 > -150)
	  continue;  // jump out of this event in for loop if not a prompt MD hi

	
      if(valid_hits>=multitracks) continue;     // jump out of this event in for loop if too many R2 hits

      double ntracks=fEvent->GetNumberOfTracks();
      double npts=fEvent->GetNumberOfPartialTracks();
      double chi=0;

      // now, apply track matching cuts
      // only use first bridged track
      if(ntracks>0){
      int nts=0;
      track=fEvent->GetTrack(nts);
      if(track->GetPackage()!=pkg){
	continue;
      }

      if(track->fDirectionPhioff<pkg2_phioff_lower || track->fDirectionPhioff> pkg2_phioff_upper || track->fDirectionThetaoff < pkg2_thetaoff_lower || track->fDirectionThetaoff >  pkg2_thetaoff_upper ) continue;

      // Now the event has passed out cuts

      for(int j=0;j<npts;++j){
        pt=fEvent->GetPartialTrack(j);
	// check that the partial track is from the correct package
	if(pt->GetRegion()!=2 || pt->GetPackage()!=pkg || pt->GetPackage()!=track->GetPackage()) continue;

	chi=pt->fChi;
	p2_Chi_histo->Fill(pt->fChi);

	double vertex_z=fEvent->GetScatteringVertexZ();
	p2_vertex->Fill(vertex_z);

	// project partial tracks to three different z-locations
	double x = pt->fOffsetX+z_coll1*pt->fSlopeX;
	double y = pt->fOffsetY+z_coll1*pt->fSlopeY;
	p2_projection_coll1->Fill(x,y); 
        p2_coll1_y->Fill(y);
        p2_coll1_x->Fill(x);

	x = pt->fOffsetX+z_coll2*pt->fSlopeX;
	y = pt->fOffsetY+z_coll2*pt->fSlopeY;
	p2_projection_coll2->Fill(x,y);
        p2_coll2_y->Fill(y);
        p2_coll2_x->Fill(x);

	x = pt->fOffsetX+z_lh2*pt->fSlopeX;
	y = pt->fOffsetY+z_lh2*pt->fSlopeY;
	p2_projection_lh2->Fill(x,y); 
        p2_lh2_y->Fill(y);
        p2_lh2_x->Fill(x);
      }
      }
    }  // end of for loop over package 2 events
   

    // make plots; Package 1 first

    double PI=3.1415926;
    double px[6];
    double py[6];
    double highy,highx,middley,middlex,lowy,lowx;
    double angle=oct==8? 135:(3-oct)*45;
    //    if(pkg==2)
    angle+=180;
    double Sin=sin(angle*PI/180);
    double Cos=cos(angle*PI/180);

    // Collimator 1, package 1
    TCanvas* coll1=new TCanvas("c1","Collimator 1 projection, Package 1",1000,600);
    TPad* spad1=new TPad("spad1","spad1",.61,.51,.99,.99);

    spad1->Draw();
    spad1->cd();
    p1_coll1_y->Draw();
    coll1->cd();
    TPad* spad2=new TPad("spad2","spad2",.61,0.01,.99,.49);
    spad2->Draw();
    spad2->cd();
    p1_coll1_x->Draw();
    coll1->cd();
    TPad* spad3=new TPad("spad3","spad3",.01,.01,.59,.99);
    spad3->Draw();
    spad3->cd();
  
    gStyle->SetPalette(1);
    p1_projection_coll1->GetXaxis()->SetTitle("hit global x [cm]");
    p1_projection_coll1->GetYaxis()->SetTitle("hit global y [cm]");
    p1_projection_coll1->Draw("colz");
    
    p1_projection_coll1->SetTitle(Form("run %d: Package 1: projection to collimator 1, Z=%f",run,z_coll1));
    
    // Draw collimator 1 outline
    highy=18.46,middley=12.63,lowy=7.03;
    highx=3.65,middlex=3.65,lowx=1.91;
    px[0]=-Cos*highx+Sin*highy;
    py[0]=Sin*highx+Cos*highy;
    px[1]=Cos*highx+Sin*highy;
    py[1]=-Sin*highx+Cos*highy;
    px[2]=-Cos*lowx+Sin*lowy;
    py[2]=Sin*lowx+Cos*lowy;
    px[3]=Cos*lowx+Sin*lowy;
    py[3]=-Sin*lowx+Cos*lowy;
    px[4]=-Cos*middlex+Sin*middley;
    py[4]=Sin*middlex+Cos*middley;
    px[5]=Cos*middlex+Sin*middley;
    py[5]=-Sin*middlex+Cos*middley;
   
    TLine* t1=new TLine(px[0],py[0],px[1],py[1]);
    TLine* t2=new TLine(px[2],py[2],px[3],py[3]);
    TLine* t3=new TLine(px[0],py[0],px[4],py[4]);
    TLine* t4=new TLine(px[1],py[1],px[5],py[5]);
    TLine* t5=new TLine(px[4],py[4],px[2],py[2]);
    TLine* t6=new TLine(px[5],py[5],px[3],py[3]);
    t1->SetLineWidth(2);
    t2->SetLineWidth(2);
    t3->SetLineWidth(2);
    t4->SetLineWidth(2);
    t5->SetLineWidth(2);
    t6->SetLineWidth(2);
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");

    coll1->SaveAs(Form("%d_p1_coll1_proj.png",run));

    // Collimator 2

    TCanvas* coll2=new TCanvas("c2","Collimator 2 projection, Package 1",1000,600);
    TPad* spad4=new TPad("spad4","spad4",.61,.51,.99,.99);

    spad4->Draw();
    spad4->cd();
    p1_coll2_y->Draw();
    coll2->cd();
    TPad* spad5=new TPad("spad5","spad5",.61,0.01,.99,.49);
    spad5->Draw();
    spad5->cd();
    p1_coll2_x->Draw();
    coll2->cd();
    TPad* spad6=new TPad("spad6","spad6",.01,.01,.59,.99);
    spad6->Draw();
    spad6->cd();
  
    gStyle->SetPalette(1);
    p1_projection_coll2->GetXaxis()->SetTitle("hit global x [cm]");
    p1_projection_coll2->GetYaxis()->SetTitle("hit global y [cm]");
    p1_projection_coll2->Draw("colz");
    p1_projection_coll2->SetTitle(Form("run %d: Package 1: projection to collimator 2, Z=%f",run,z_coll2));
    
    // draw collimator 2 outline
    highy=53.7,middley=37.1,lowy=30.57;
    highx=9.2,middlex=9.2,lowx=6.5;

    px[0]=-Cos*highx+Sin*highy;
    py[0]=Sin*highx+Cos*highy;
    px[1]=Cos*highx+Sin*highy;
    py[1]=-Sin*highx+Cos*highy;
    px[2]=-Cos*lowx+Sin*lowy;
    py[2]=Sin*lowx+Cos*lowy;
    px[3]=Cos*lowx+Sin*lowy;
    py[3]=-Sin*lowx+Cos*lowy;
    px[4]=-Cos*middlex+Sin*middley;
    py[4]=Sin*middlex+Cos*middley;
    px[5]=Cos*middlex+Sin*middley;
    py[5]=-Sin*middlex+Cos*middley;
    
    *t1= TLine(px[0],py[0],px[1],py[1]);
    *t2= TLine(px[2],py[2],px[3],py[3]);
    *t3= TLine(px[0],py[0],px[4],py[4]);
    *t4= TLine(px[1],py[1],px[5],py[5]);
    *t5= TLine(px[4],py[4],px[2],py[2]);
    *t6= TLine(px[5],py[5],px[3],py[3]);

    t1->SetLineWidth(2);
    t2->SetLineWidth(2);
    t3->SetLineWidth(2);
    t4->SetLineWidth(2);
    t5->SetLineWidth(2);
    t6->SetLineWidth(2);
    
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");

    coll2->SaveAs(Form("%d_p1_coll2_proj.png",run));

    // Center (in z) of lH2 target
    TCanvas* lh2=new TCanvas("c3","Hydrogen Target Projection, Package 1",1000,600);
    TPad* spad4=new TPad("spad4","spad4",.61,.51,.99,.99);

    spad4->Draw();
    spad4->cd();
    p1_lh2_y->Draw();
    lh2->cd();
    TPad* spad5=new TPad("spad5","spad5",.61,0.01,.99,.49);
    spad5->Draw();
    spad5->cd();
    p1_lh2_x->Draw();
    lh2->cd();
    TPad* spad6=new TPad("spad6","spad6",.01,.01,.59,.99);
    spad6->Draw();
    spad6->cd();
  
    gStyle->SetPalette(1);
    p1_projection_lh2->GetXaxis()->SetTitle("hit global x [cm]");
    p1_projection_lh2->GetYaxis()->SetTitle("hit global y [cm]");
    p1_projection_lh2->Draw("colz");
    p1_projection_lh2->SetTitle(Form("run %d: Package 1:  projection to lH2 target center, Z=%f",run,z_lh2));
    
    lh2->SaveAs(Form("%d_p1_lh2_proj.png",run));

 
    // make plots; Package 2 now
    angle+=180;
    Sin=sin(angle*PI/180);
    Cos=cos(angle*PI/180);

    // Collimator 1, package 2
    TCanvas* coll1_p2=new TCanvas("c4","Collimator 1 projection, Package 2 ",1000,600);
    TPad* spad1=new TPad("spad1","spad1",.61,.51,.99,.99);

    spad1->Draw();
    spad1->cd();
    p2_coll1_y->Draw();
    coll1_p2->cd();
    TPad* spad2=new TPad("spad2","spad2",.61,0.01,.99,.49);
    spad2->Draw();
    spad2->cd();
    p2_coll1_x->Draw();
    coll1_p2->cd();
    TPad* spad3=new TPad("spad3","spad3",.01,.01,.59,.99);
    spad3->Draw();
    spad3->cd();
  
    gStyle->SetPalette(1);
    p2_projection_coll1->GetXaxis()->SetTitle("hit global x [cm]");
    p2_projection_coll1->GetYaxis()->SetTitle("hit global y [cm]");
    p2_projection_coll1->Draw("colz");
    
    p2_projection_coll1->SetTitle(Form("run %d: Package 2:  projection to collimator 1, Z=%f",run,z_coll1));

    // Draw collimator 1 outline
    highy=18.46,middley=12.63,lowy=7.03;
    highx=3.65,middlex=3.65,lowx=1.91;
    px[0]=-Cos*highx+Sin*highy;
    py[0]=Sin*highx+Cos*highy;
    px[1]=Cos*highx+Sin*highy;
    py[1]=-Sin*highx+Cos*highy;
    px[2]=-Cos*lowx+Sin*lowy;
    py[2]=Sin*lowx+Cos*lowy;
    px[3]=Cos*lowx+Sin*lowy;
    py[3]=-Sin*lowx+Cos*lowy;
    px[4]=-Cos*middlex+Sin*middley;
    py[4]=Sin*middlex+Cos*middley;
    px[5]=Cos*middlex+Sin*middley;
    py[5]=-Sin*middlex+Cos*middley;
   
    *t1= TLine(px[0],py[0],px[1],py[1]);
    *t2= TLine(px[2],py[2],px[3],py[3]);
    *t3= TLine(px[0],py[0],px[4],py[4]);
    *t4= TLine(px[1],py[1],px[5],py[5]);
    *t5= TLine(px[4],py[4],px[2],py[2]);
    *t6= TLine(px[5],py[5],px[3],py[3]);
    t1->SetLineWidth(2);
    t2->SetLineWidth(2);
    t3->SetLineWidth(2);
    t4->SetLineWidth(2);
    t5->SetLineWidth(2);
    t6->SetLineWidth(2);
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");

    coll1_p2->SaveAs(Form("%d_p2_coll1_proj.png",run));

    // Collimator 2

    TCanvas* coll2_p2=new TCanvas("c5","Collimator 2 projection, Package 2",1000,600);
    TPad* spad4=new TPad("spad4","spad4",.61,.51,.99,.99);

    spad4->Draw();
    spad4->cd();
    p2_coll2_y->Draw();
    coll2_p2->cd();
    TPad* spad5=new TPad("spad5","spad5",.61,0.01,.99,.49);
    spad5->Draw();
    spad5->cd();
    p2_coll2_x->Draw();
    coll2_p2->cd();
    TPad* spad6=new TPad("spad6","spad6",.01,.01,.59,.99);
    spad6->Draw();
    spad6->cd();
  
    gStyle->SetPalette(1);
    p2_projection_coll2->GetXaxis()->SetTitle("hit global x [cm]");
    p2_projection_coll2->GetYaxis()->SetTitle("hit global y [cm]");
    p2_projection_coll2->Draw("colz");
    p2_projection_coll2->SetTitle(Form("run %d: Package 2:  projection to collimator 2, Z=%f",run,z_coll2));
    
    // draw collimator 2 outline
    highy=53.7,middley=37.1,lowy=30.57;
    highx=9.2,middlex=9.2,lowx=6.5;

    px[0]=-Cos*highx+Sin*highy;
    py[0]=Sin*highx+Cos*highy;
    px[1]=Cos*highx+Sin*highy;
    py[1]=-Sin*highx+Cos*highy;
    px[2]=-Cos*lowx+Sin*lowy;
    py[2]=Sin*lowx+Cos*lowy;
    px[3]=Cos*lowx+Sin*lowy;
    py[3]=-Sin*lowx+Cos*lowy;
    px[4]=-Cos*middlex+Sin*middley;
    py[4]=Sin*middlex+Cos*middley;
    px[5]=Cos*middlex+Sin*middley;
    py[5]=-Sin*middlex+Cos*middley;
    
    *t1= TLine(px[0],py[0],px[1],py[1]);
    *t2= TLine(px[2],py[2],px[3],py[3]);
    *t3= TLine(px[0],py[0],px[4],py[4]);
    *t4= TLine(px[1],py[1],px[5],py[5]);
    *t5= TLine(px[4],py[4],px[2],py[2]);
    *t6= TLine(px[5],py[5],px[3],py[3]);

    t1->SetLineWidth(2);
    t2->SetLineWidth(2);
    t3->SetLineWidth(2);
    t4->SetLineWidth(2);
    t5->SetLineWidth(2);
    t6->SetLineWidth(2);
    
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");

    coll2_p2->SaveAs(Form("%d_p2_coll2_proj.png",run));

    // Center (in z) of lH2 target
    TCanvas* lh2_p2=new TCanvas("c6","Hydrogen Target Projection, Package 2",1000,600);
    TPad* spad4=new TPad("spad4","spad4",.61,.51,.99,.99);

    spad4->Draw();
    spad4->cd();
    p2_lh2_y->Draw();
    lh2_p2->cd();

    TPad* spad5=new TPad("spad5","spad5",.61,0.01,.99,.49);
    spad5->Draw();
    spad5->cd();
    p2_lh2_x->Draw();
    lh2_p2->cd();
    TPad* spad6=new TPad("spad6","spad6",.01,.01,.59,.99);
    spad6->Draw();
    spad6->cd();
  
    gStyle->SetPalette(1);
    p2_projection_lh2->GetXaxis()->SetTitle("hit global x [cm]");
    p2_projection_lh2->GetYaxis()->SetTitle("hit global y [cm]");
    p2_projection_lh2->Draw("colz");
    p2_projection_lh2->SetTitle(Form("run %d: Package 2:  projection to lH2 target center, Z=%f",run,pkg,z_lh2));
    
    lh2_p2->SaveAs(Form("%d_p2_lh2_proj.png",run));
 

    // Vertex, chi for Region 2 partial tracks
    TCanvas* vertex=new TCanvas("vertex","Region 2 Partial Tracks: z-vertex and chi",600,600);

    vertex->Divide(2,2);
    vertex->cd(1);
    gStyle->SetStatW(0.3);
    gStyle->SetStatH(0.3);
    p1_vertex->SetLineColor(2);
    p1_vertex->Draw();
    vertex->cd(2);
    p2_vertex->SetLineColor(4);
    p2_vertex->Draw();
    vertex->cd(3);
    gPad->SetLogy();
    p1_Chi_histo->SetLineColor(2);
    p1_Chi_histo->Draw();
    vertex->cd(4);
    gPad->SetLogy();
    p2_Chi_histo->SetLineColor(4);
    p2_Chi_histo->Draw();
    vertex->SaveAs(Form("%d_r2_vertex_chi.png",run));


    //  Output summary data to text files

   std::ofstream fout;
   //   fout.open(outputPrefix+"r2_projections.txt");
   fout.open("r2_projections.txt");

   fout << "Run \t pkg \twhere\t (x,y) \t Value \t\t RMS" <<endl; 

   fout << run << "\t 1 \tcoll 1\t   x\t" << p1_projection_coll1->ProjectionX()->GetMean() << "\t" << p1_projection_coll1->ProjectionX()->GetRMS() << endl;

   fout << run << "\t 1 \tcoll 1\t   y \t" << p1_projection_coll1->ProjectionY()->GetMean() << "\t" << p1_projection_coll1->ProjectionY()->GetRMS() << endl;

   fout << run << "\t 1 \tcoll 2\t   x \t" << p1_projection_coll2->ProjectionX()->GetMean() << "\t" << p1_projection_coll2->ProjectionX()->GetRMS() << endl;

   fout << run << "\t 1 \tcoll 2\t   y \t" << p1_projection_coll2->ProjectionY()->GetMean() << "\t" << p1_projection_coll2->ProjectionY()->GetRMS() << endl;

   fout << run << "\t 1 \tlH2\t   x \t" << p1_projection_lh2->ProjectionX()->GetMean() << "\t" << p1_projection_lh2->ProjectionX()->GetRMS() << endl;

   fout << run << "\t 1 \tlH2\t   y \t" << p1_projection_lh2->ProjectionY()->GetMean() << "\t" << p1_projection_lh2->ProjectionY()->GetRMS() << endl;



   fout << run << "\t 2 \tcoll 1\t   x\t" << p2_projection_coll1->ProjectionX()->GetMean() << "\t" << p2_projection_coll1->ProjectionX()->GetRMS() << endl;

   fout << run << "\t 2 \tcoll 1\t   y \t" << p2_projection_coll1->ProjectionY()->GetMean() << "\t" << p2_projection_coll1->ProjectionY()->GetRMS() << endl;

   fout << run << "\t 2 \tcoll 2\t   x \t" << p2_projection_coll2->ProjectionX()->GetMean() << "\t" << p2_projection_coll2->ProjectionX()->GetRMS() << endl;

   fout << run << "\t 2 \tcoll 2\t   y \t" << p2_projection_coll2->ProjectionY()->GetMean() << "\t" << p2_projection_coll2->ProjectionY()->GetRMS() << endl;

   fout << run << "\t 2 \tlH2\t   x \t" << p2_projection_lh2->ProjectionX()->GetMean() << "\t" << p2_projection_lh2->ProjectionX()->GetRMS() << endl;

   fout << run << "\t 2 \tlH2\t   y \t" << p2_projection_lh2->ProjectionY()->GetMean() << "\t" << p2_projection_lh2->ProjectionY()->GetRMS() << endl;


   return;
}



int DetermineOctantRegion3(TTree* event_tree, int package)
{
  for (int octant = 1; octant <= 8; octant++) 
	{
    TH1D* h_adc = new TH1D("h_adc","h_adc",128,0,4096);
    event_tree->Draw(Form("maindet.md%dm_adc+maindet.md%dp_adc>>h_adc",octant,octant),
                     Form("events.fQwTracks.fPackage==%d",package),"",1000);
    if (h_adc->GetMean() > adc_pedestal[octant] + 50.0) {
      delete h_adc;
      return octant;
    }
  }
}
