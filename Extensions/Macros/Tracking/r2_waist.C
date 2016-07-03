// Author: David Armstrong
// File: r2_waist.C
// Date: May 18 2015
// 
// Description: This file is used project the Region 2 partial tracks to the Al (thin) target.
//    It loops over assumed values of the Z-location of the Al target, looks
//    at the width of the projected x and y at the Z of the target for
//    each Z via fits to Gaussians, and plots the sigmas and means vs. assumed Z.  
//   
//   Requires good, bridged full tracks; cuts on MD being hit, and the matching cuts.
//
//  Based on r2_projection.C, originally written by Siyuan Yang
//
//  Usage: 
//   Root> projections(15121,1,10000)   run, start event, end event 
//
//
//
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>

// maximum allowed value for #R2 hits 
const int multitracks=18;

double adc_pedestal[9] = {0.0, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000}; // units adc units or a.u. (arbitrary units)

double z_al[14] = {-624, -626, -628, -630, -632, -633, -634, -635, -636, -637, -638, -640, -642, -644 }; 

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
  
   std::vector<TH1D*> p1_lh2_x;//[oct]
   std::vector<TH1D*> p1_lh2_y;//[oct]
   std::vector<TH1D*> p2_lh2_x;//[oct]
   std::vector<TH1D*> p2_lh2_y;//[oct]
   p1_lh2_x.resize(14);
   p1_lh2_y.resize(14);
   p2_lh2_x.resize(14);
   p2_lh2_y.resize(14);

   Double_t p0_p1x[14];
   Double_t p0_p1y[14];
   Double_t p0_p2x[14];
   Double_t p0_p2y[14];
   Double_t p1_p1x[14];
   Double_t p1_p1y[14];
   Double_t p1_p2x[14];
   Double_t p1_p2y[14];
   Double_t p2_p1x[14];
   Double_t p2_p1y[14];
   Double_t p2_p2x[14];
   Double_t p2_p2y[14];

  for (int jj = 0; jj<p1_lh2_x.size();jj++){
     p1_lh2_x[jj] = new TH1D (Form("p1_lh2_x[%d]",jj),Form("P1: x at target for Z_Al = %f",z_al[jj]),64,-6,6);
     p1_lh2_x[jj]->GetXaxis()->SetTitle("X (cm)");
     p1_lh2_x[jj]->GetYaxis()->SetTitle("Events");

     p1_lh2_y[jj] = new TH1D (Form("p1_lh2_y[%d]",jj),Form("P1: y at target for Z_Al = %f",z_al[jj]),64,-6,6);
     p1_lh2_y[jj]->GetXaxis()->SetTitle("Y (cm)");
     p1_lh2_y[jj]->GetYaxis()->SetTitle("Events");

     p2_lh2_x[jj] = new TH1D (Form("p2_lh2_x[%d]",jj),Form("P2: x at target for Z_Al = %f",z_al[jj]),64,-6,6);
     p2_lh2_x[jj]->GetXaxis()->SetTitle("X (cm)");
     p2_lh2_x[jj]->GetYaxis()->SetTitle("Events");

     p2_lh2_y[jj] = new TH1D (Form("p2_lh2_y[%d]",jj),Form("P2: y at target for Z_Al = %f",z_al[jj]),64,-6,6);
     p2_lh2_y[jj]->GetXaxis()->SetTitle("Y (cm)");
     p2_lh2_y[jj]->GetYaxis()->SetTitle("Events");

  }

   TH1F* p1_vertex=new TH1F("vertex in Z","vertex in Z  (package 1)",200,-850,-450);
   TH1F* p1_Chi_histo=new TH1F("chi","chi in Region 2  (package 1)",200,0,20);
   TH1F* p2_vertex=new TH1F("vertex in Z","vertex in Z  (package 2)",200,-850,-450);
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

   if (md_1==0 && md_2==0){
     md_1=5;
     md_2=1;
     cout << "Warning: octant identification failed, default to 5,1" << endl;
   }

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
  


    // Loop over Z of Al target
    
    int m=0;

    //      cout << "looping over Z of Al target; loop variable = " << m << " Z_Alum = " << z_al[m] << endl; 

     // Loop over events for package 1
    int pkg=1;
    for(int i=start;i<end;++i){

      if(i%100000==0) cout << "package 1 events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      double mdp_value_1=mdp_1->GetValue();
      double mdm_value_1=mdm_1->GetValue();
      
   // count number of Region 2 hits 
   // plot Raw VDC hits also
      int nhits=fEvent->GetNumberOfHits();
      int valid_hits=0;


      for(int j=0;j<nhits;++j){
	hit=fEvent->GetHit(j);
	if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0 && hit->GetPackage() == pkg){
	  ++valid_hits;
	}
      }

      double ntracks=fEvent->GetNumberOfTracks();
      double npts=fEvent->GetNumberOfPartialTracks();
      double chi=0;

   // test if the MD is hit - using TDC hits
   // only looks at first hit in MD's multihit TDCs - not ideal logic
	
      if(mdm_value_1 <-210 || mdm_value_1 > -150 || mdp_value_1 < -210 || mdp_value_1 > -150)   continue;  // jump out of this event in for loop if not a prompt MD hit
	
      if(valid_hits>=multitracks) continue;     // jump out of this event in for loop if too many R2 hits

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

	// project partial tracks to assumed Al z-location

    for(int m=0;m<14;++m){
	double x = pt->fOffsetX+z_al[m]*pt->fSlopeX;
	double y = pt->fOffsetY+z_al[m]*pt->fSlopeY;
	//	p1_projection_lh2->Fill(x,y); 
        p1_lh2_x[m]->Fill(x);
        p1_lh2_y[m]->Fill(y);
        }
      }
      }
    }  // end of for loop over package 1 events
   

    // Loop over events for package 2
    pkg=2;
    for(int i=start;i<end;++i){

      if(i%100000==0) cout << "package 2 events processed so far: " << i << endl;
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

	// project partial tracks to assumed al z-locations

	for(int m=0;m<14;++m){
	  double x = pt->fOffsetX+z_al[m]*pt->fSlopeX;
	  double y = pt->fOffsetY+z_al[m]*pt->fSlopeY;
	  //	p2_projection_lh2->Fill(x,y); 
	  
	  p2_lh2_x[m]->Fill(x);
	  p2_lh2_y[m]->Fill(y);
	}
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


    // Projections to Al target
    TCanvas* lh21x=new TCanvas("c1","Aluminum Target Projection, x, Package 1",1200,1200);
    lh21x->Divide(4,4);
    for (jj = 0; jj<p1_lh2_x.size();jj++){
      lh21x->cd(jj+1);  
      p1_lh2_x[jj]->Draw();
      p1_lh2_x[jj]->Fit("gaus");
      TF1 *fit = p1_lh2_x[jj]->GetFunction("gaus");
      p0_p1x[jj] = fit->GetParameter(0);
      p1_p1x[jj] = fit->GetParameter(1);
      p2_p1x[jj] = fit->GetParameter(2);
    }
    lh21x->SaveAs(Form("%d_p1_x_al.png",run));

    TCanvas* lh21y=new TCanvas("c2","Aluminum Target Projection, y, Package 1",1200,1200);
    lh21y->Divide(4,4);
    for (jj = 0; jj<p1_lh2_y.size();jj++){
      lh21y->cd(jj+1);  
      p1_lh2_y[jj]->Draw();
      p1_lh2_y[jj]->Fit("gaus");
      TF1 *fit2 = p1_lh2_y[jj]->GetFunction("gaus");
      p0_p1y[jj] = fit2->GetParameter(0);
      p1_p1y[jj] = fit2->GetParameter(1);
      p2_p1y[jj] = fit2->GetParameter(2);
    }
    lh21y->SaveAs(Form("%d_p1_y_al.png",run));

    TCanvas* lh22x=new TCanvas("c3","Aluminum Target Projection, x, Package 2",1200,1200);
    lh22x->Divide(4,4);
    for (jj = 0; jj<p2_lh2_x.size();jj++){
      lh22x->cd(jj+1);  
      p2_lh2_x[jj]->Draw();
      p2_lh2_x[jj]->Fit("gaus");
      TF1 *fit3 = p2_lh2_x[jj]->GetFunction("gaus");
      p0_p2x[jj] = fit3->GetParameter(0);
      p1_p2x[jj] = fit3->GetParameter(1);
      p2_p2x[jj] = fit3->GetParameter(2);
    }
    lh21x->SaveAs(Form("%d_p2_x_al.png",run));

    TCanvas* lh22y=new TCanvas("c4","Aluminum Target Projection, y, Package 2",1200,1200);
    lh22y->Divide(4,4);
    for (jj = 0; jj<p2_lh2_y.size();jj++){
      lh22y->cd(jj+1);  
      p2_lh2_y[jj]->Draw();
      p2_lh2_y[jj]->Fit("gaus");
      TF1 *fit4 = p2_lh2_y[jj]->GetFunction("gaus");
      p0_p2y[jj] = fit4->GetParameter(0);
      p1_p2y[jj] = fit4->GetParameter(1);
      p2_p2y[jj] = fit4->GetParameter(2);
    }
    lh21y->SaveAs(Form("%d_p2_y_al.png",run));
 

    angle+=180;
    Sin=sin(angle*PI/180);
    Cos=cos(angle*PI/180);


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
    vertex->SaveAs(Form("%d_r2_vertex_chi_waist.png",run));


  // Plot fitted sigmas vs Z_al
  TCanvas *cw = new TCanvas("cw","HDC beam waist",200,10,700,900);

   cw->cd(1);
   TGraph *gr1 = new TGraph(14,z_al,p2_p1x);
   TGraph *gr2 = new TGraph(14,z_al,p2_p2x);
   TGraph *gr3 = new TGraph(14,z_al,p2_p1y);
   TGraph *gr4 = new TGraph(14,z_al,p2_p2y);

   gPad->SetLeftMargin(0.20); 
   gStyle->SetOptFit(0000);
   gStyle->SetOptStat(0000);

   gr1->GetYaxis()->SetTitleOffset(2.0); 
   gr1->SetTitle(Form("Vertex #sigma vs. tgt Z, Run %d (4% DS Al)",run));
   gStyle->SetTitleX(0.5);

   gr1->SetMarkerStyle(20);
   gr1->SetMarkerColor(2);
   gr1->SetLineColor(2);
   gr1->GetHistogram()->SetMaximum(0.5); 
   gr1->GetXaxis()->SetTitle("z-position of Al target (cm)");
   gr1->GetYaxis()->SetTitle("#sigma of vertex (cm)"); 
   gr1->GetYaxis()->SetLabelSize(0.03);
   gr1->GetXaxis()->SetLabelSize(0.035);
   //   gStyle->SetOptFit(1111);
   gr1->Draw("AP");
   gr1->Fit("pol3");
   gr1->GetFunction("pol3")->SetLineColor(2);

   double min1;
   double min2;
   double smin1;
   double smin2;
   
   // calculate the location of local minimum of fit
   TF1 *fita = gr1->GetFunction("pol3");
   double p1_x_0  = fita->GetParameter(0);
   double p1_x_1  = fita->GetParameter(1);
   double p1_x_2  = fita->GetParameter(2);
   double p1_x_3  = fita->GetParameter(3);
   /// need to check for imaginary numbers ?
   min1 = (-2.0*p1_x_2 + sqrt(4.0*p1_x_2*p1_x_2 - 12*p1_x_3*p1_x_1))/(6*p1_x_3);
   min2 = (-2.0*p1_x_2 - sqrt(4.0*p1_x_2*p1_x_2 - 12*p1_x_3*p1_x_1))/(6*p1_x_3);
   smin1 = p1_x_0 + p1_x_1*min1 + p1_x_2*min1*min1 + p1_x_3*min1*min1*min1;
   smin2 = p1_x_0 - p1_x_1*min1 + p1_x_2*min1*min1 + p1_x_3*min1*min1*min1;
   if (abs(smin2)<abs(smin1)) 
	  {
	    cout << " P1 x minimum at Z = " << min2 << " sigma min = " << smin2 << endl;
	    double p1_x_min=min2;
	    double sig_p1_x = smin2;
	  }
	 else
	   {
	     cout << " P1 x minimum at Z = " << min1 << " sigma min = " << smin1 << endl;
 	    double p1_x_min=min1;
	    double sig_p1_x = smin1;
	   }

   gr2->SetMarkerStyle(21);
   gr2->SetLineColor(4);
   gr2->SetMarkerColor(4);
   gr2->Draw("P");
   gr2->Fit("pol3");
   gr2->GetFunction("pol3")->SetLineColor(4);

   // calculate the location of local minimum of fit
   TF1 *fitb = gr2->GetFunction("pol3");
   double p2_x_0  = fitb->GetParameter(0);
   double p2_x_1  = fitb->GetParameter(1);
   double p2_x_2  = fitb->GetParameter(2);
   double p2_x_3  = fitb->GetParameter(3);
   min1 = (-2.0*p2_x_2 + sqrt(4.0*p2_x_2*p2_x_2 - 12*p2_x_3*p2_x_1))/(6*p2_x_3);
   min2 = (-2.0*p2_x_2 - sqrt(4.0*p2_x_2*p2_x_2 - 12*p2_x_3*p2_x_1))/(6*p2_x_3);
   smin1 = p2_x_0 + p2_x_1*min1 + p2_x_2*min1*min1 + p2_x_3*min1*min1*min1;
   smin2 = p2_x_0 + p2_x_1*min2 + p2_x_2*min2*min2 + p2_x_3*min2*min2*min2;
   if (abs(smin2)<abs(smin1))
	  {
	    cout << " P2 x minimum at Z = " << min2 << endl;
	    double p2_x_min=min2;
	    double sig_p2_x = smin2;
	  }
	 else
	   {
	     cout << " P2 x minimum at Z = " << min1 << endl;
	     double p2_x_min=min1;
	     double sig_p2_x = smin1;
	   }

   gr3->SetMarkerStyle(24);
   gr3->SetLineColor(2);
   gr3->SetMarkerColor(2);
   gr3->Draw("P");
   gr3->Fit("pol3");
   gr3->GetFunction("pol3")->SetLineColor(2);

   // calculate the location of local minimum of fit
   TF1 *fitc = gr3->GetFunction("pol3");
   double p1_y_0  = fitc->GetParameter(0);
   double p1_y_1  = fitc->GetParameter(1);
   double p1_y_2  = fitc->GetParameter(2);
   double p1_y_3  = fitc->GetParameter(3);
   min1 = (-2.0*p1_y_2 + sqrt(4.0*p1_y_2*p1_y_2 - 12*p1_y_3*p1_y_1))/(6*p1_y_3);
   min2 = (-2.0*p1_y_2 - sqrt(4.0*p1_y_2*p1_y_2 - 12*p1_y_3*p1_y_1))/(6*p1_y_3);
   smin1 = p1_y_0 + p1_y_1*min1 + p1_y_2*min1*min1 + p1_y_3*min1*min1*min1;
   smin2 = p1_y_0 + p1_y_1*min2 + p1_y_2*min2*min2 + p1_y_3*min2*min2*min2;
   if (abs(smin2)<abs(smin1))
	  {
	    cout << " P1 y minimum at Z = " << min2 << endl;
	    double p1_y_min=min2;
	    double sig_p1_y =smin2;
	  }
	 else
	   {
	     cout << " P1 y minimum at Z = " << min1 << endl;
	     double p1_y_min=min1;
	     double sig_p1_y =smin1;
	   }

   gr4->SetMarkerStyle(25);
   gr4->SetLineColor(4);
   gr4->SetMarkerColor(4);
   gr4->Draw("P");
   gr4->Fit("pol3");
   gr4->GetFunction("pol3")->SetLineColor(4);
   // calculate the location of local minimum of fit
   TF1 *fitd = gr4->GetFunction("pol3");
   double p2_y_0  = fitd->GetParameter(0);
   double p2_y_1  = fitd->GetParameter(1);
   double p2_y_2  = fitd->GetParameter(2);
   double p2_y_3  = fitd->GetParameter(3);
   min1 = (-2.0*p2_y_2 + sqrt(4.0*p2_y_2*p2_y_2 - 12*p2_y_3*p2_y_1))/(6*p2_y_3);
   min2 = (-2.0*p2_y_2 - sqrt(4.0*p2_y_2*p2_y_2 - 12*p2_y_3*p2_y_1))/(6*p2_y_3);
   smin1 = p2_y_0 + p2_y_1*min1 + p2_y_2*min1*min1 + p2_y_3*min1*min1*min1;
   smin2 = p2_y_0 + p2_y_1*min2 + p2_y_2*min2*min2 + p2_y_3*min2*min2*min2;
   if (abs(smin2)<abs(smin1))
	  {
	    cout << " P2 y minimum at Z = " << min2 << endl;
	    double p2_y_min=min2;
	    double sig_p2_y=smin2;
	  }
	 else
	   {
	     cout << " P2 y minimum at Z = " << min1 << endl;
	     double p2_y_min=min1;
	     double sig_p2_y=smin1;
	   }

   leg = new TLegend(0.22,0.65,0.45,0.88);
   leg->AddEntry(gr1,"Pkg. 1, #sigma_{x}","p"); 
   leg->AddEntry(gr2,"Pkg. 2, #sigma_{x}","p"); 
   leg->AddEntry(gr3,"Pkg. 1, #sigma_{y}","p"); 
   leg->AddEntry(gr4,"Pkg. 2, #sigma_{y}","p"); 
   leg->Draw();
   cw->SaveAs(Form("%d_waist_sigmas.png",run));

  // Plot fitted means vs Z_al
  TCanvas *cmx = new TCanvas("cmx","HDC waist x,y means",400,400);

   cmx->cd(1);
   TGraph *gr5 = new TGraph(14,z_al,p1_p1x);
   TGraph *gr6 = new TGraph(14,z_al,p1_p2x);

   gPad->SetLeftMargin(0.20); 
   gStyle->SetOptFit(0000);
   gStyle->SetOptStat(0000);

   gr5->GetYaxis()->SetTitleOffset(2.0); 
   gr5->SetTitle(Form("Vertex (x,y) means vs. tgt Z, Run %d (4% DS Al)",run));
   gStyle->SetTitleX(0.5
);
 
   gr5->SetMarkerStyle(20);
   gr5->SetMarkerColor(2);
   gr5->SetLineColor(2);
   gr5->GetHistogram()->SetMaximum(2.0); 
   gr5->GetHistogram()->SetMinimum(-2.0); 
   gr5->GetXaxis()->SetTitle("z-position of Al target (cm)");
   gr5->GetYaxis()->SetTitle("x or y mean of vertex (cm)"); 
   gr5->GetYaxis()->SetLabelSize(0.03);
   gr5->GetXaxis()->SetLabelSize(0.035);
   //   gStyle->SetOptFit(1111);
   gr5->Draw("AP");
   gr5->Fit("pol1");
   gr5->GetFunction("pol1")->SetLineColor(2);

   gr6->SetMarkerStyle(21);
   gr6->SetLineColor(4);
   gr6->SetMarkerColor(4);
   gr6->Draw("P");
   gr6->Fit("pol1");
   gr6->GetFunction("pol1")->SetLineColor(4);

   TF1 *fite = gr5->GetFunction("pol1");
   double p1_xm_0  = fite->GetParameter(0);
   double p1_xm_1  = fite->GetParameter(1);
   double p1_z_x0 = -p1_xm_0/p1_xm_1;
   cout << "Package 1 Z at X=0 = " << p1_z_x0 << endl;

   TF1 *fitf = gr6->GetFunction("pol1");
   double p2_xm_0  = fitf->GetParameter(0);
   double p2_xm_1  = fitf->GetParameter(1);
   double p2_z_x0 = -p2_xm_0/p2_xm_1;
   cout << "Package 2 Z at X=0 = " << p2_z_x0 << endl;


   //   TCanvas *cmy = new TCanvas("cmy","HDC  waist y means",400,400);
   //   cmy->cd(1);
   TGraph *gr7 = new TGraph(14,z_al,p1_p1y);
   TGraph *gr8 = new TGraph(14,z_al,p1_p2y);

   //   gPad->SetLeftMargin(0.20); 
   //   gStyle->SetOptFit(0000);
   //   gStyle->SetOptStat(0000);

   //   gr7->GetHistogram()->SetMaximum(0.5); 
   //   gr7->GetHistogram()->SetMinimum(-0.5); 


   gr7->SetMarkerStyle(24);
   gr7->SetLineColor(2);
   gr7->SetMarkerColor(2);
   gr7->Draw("P");
   gr7->Fit("pol1");
   gr7->GetFunction("pol1")->SetLineColor(2);

   gr8->SetMarkerStyle(25);
   gr8->SetLineColor(4);
   gr8->SetMarkerColor(4);
   gr8->Draw("P");
   gr8->Fit("pol1");
   gr8->GetFunction("pol1")->SetLineColor(4);

   leg = new TLegend(0.22,0.65,0.45,0.88);
   leg->AddEntry(gr5,"Pkg. 1, mean_{x}","p"); 
   leg->AddEntry(gr6,"Pkg. 2, mean_{x}","p"); 
   leg->AddEntry(gr7,"Pkg. 1, mean_{y}","p"); 
   leg->AddEntry(gr8,"Pkg. 2, mean_{y}","p"); 
   leg->Draw();

   TF1 *fitf = gr7->GetFunction("pol1");
   double p1_ym_0  = fitf->GetParameter(0);
   double p1_ym_1  = fitf->GetParameter(1);
   double p1_z_y0 = -p1_ym_0/p1_ym_1;
   cout << "Package 1 Z at Y=0 = " << p1_z_y0 << endl;

   TF1 *fitg = gr8->GetFunction("pol1");
   double p2_ym_0  = fitg->GetParameter(0);
   double p2_ym_1  = fitg->GetParameter(1);
   double p2_z_y0 = -p2_ym_0/p2_ym_1;
   cout << "Package 2 Z at Y=0 = " << p2_z_y0 << endl;

   cmx->SaveAs(Form("%d_waist_xy_mean.png",run));

   //   cmy->SaveAs(Form("%d_waist_means.png",run));


//  Output summary data to text files
   std::ofstream fout;
   fout.open(Form("r2_waist_%d.txt",run));

   fout << "Run: " << run << endl;
   fout << "Events analyzed = " << (end-start) << endl;
 

   fout << "Z values of minima in widths: " << endl; 
   fout << "Pack 1 x \t Pack 2 x \t Pack 1 y \t Pack 2 y "<< endl;
   fout << p1_x_min << "\t" << p2_x_min << "\t" << p1_y_min << "\t" << p2_y_min << endl;

   fout << "Sigma at minima: " << endl; 
   fout << "Pack 1 x \t Pack 2 x \t Pack 1 y \t Pack 2 y "<< endl;
   fout << sig_p1_x << "\t" << sig_p2_x << "\t" << sig_p1_y << "\t" << sig_p2_y << endl;

   fout << "Z values at zeros in x or y: " << endl; 
   fout << "Pack 1 x \t Pack 2 x \t Pack 1 y \t Pack 2 y "<< endl;
   fout << p1_z_x0 << " \t" << p2_z_x0 << " \t" << p1_z_y0 << " \t" << p2_z_y0 << endl;

   fout << "Gaussian Fit sigmas" <<endl; 
   fout << "m \t Z (cm) \ P1 (x) \t P2 (x) \t P1 (y) \t P2 (y)" <<endl; 

   for(m=0;m<14;++m){
     fout << m << "\t" << setprecision(6) << z_al[m]
 << "\t" << p2_p1x[m] << "\t" << p2_p2x[m] << "\t" << p2_p1y[m] << "\t" << p2_p2y[m] << endl;
   }

   fout << endl << "Gaussian Fit means" <<endl; 
   fout << "m \t Z (cm) \ P1 (x) \t P2 (x) \t P1 (y) \t P2 (y)" <<endl; 

   for(m=0;m<14;++m){
     fout << m << "\t" << setprecision(6) << z_al[m] << "\t" << p1_p1x[m] << "\t" << p1_p2x[m] << "\t" << p1_p1y[m] << "\t" << p1_p2y[m] << endl;
   }

   fout << "Run \t pkg \twhere\t (x,y) \t Value \t\t RMS" <<endl; 
   fout << run << "\t 1 \tlH2\t   z \t" << p1_vertex->GetMean() << "\t" << p1_vertex->GetRMS() << endl;

   fout << run << "\t 2 \tlH2\t   z \t" << p2_vertex->GetMean() << "\t" << p1_vertex->GetRMS() << endl;

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
