// Name:   Q2_check_DSA.C
// Author: Siyuan Yang/David Armstrong
// Email:  sxyang@email.wm.edu
// Date:   Monday Jan 9th,2012
//
//
// This script is used to extract the Q2 information for both packages
// from the rootfiles. Now it has two cuts turned on, one is the MD
// cut, which requires the R3 track to hit the MD. The other is the number
// of valid hits in R2 and this is used to eliminate the cases when there
// are more than one track going through the R2 at one event. The variable
// multiple sets the threshold for that multiplicity case.
//
// It also allows cuts on the theta and phi matching parameters in Region 3
//
// Examples
// root[0] .L Extensions/Macros/Tracking/Q2_check.C
// root[1] q2_check(-1,-1,13653)
// this will draw the scattering angle and Q2 for both packages over the entire 
// range of events in Qweak_13653.

// Another Example
// root[2] q2_check(-1,10000,13733,false,"Qweak_","_v2")
// this will carry out the same task as previous example for the first 10000 events. 
// However, the name of the rootfile
// has a suffix "_v2", so the script is looking for a rootfile named as Qweak_13653_v2.root.

// notion: the package number is always consistent with package number in R2

// UPDATE on Feb 12th, 2012
// remove the oct number in the argument list. The octant number will be automatically deduced give 
// the run number by the getOctNumber function.This modification saves people's time to remeber 
// the octant number for specific run.

// DSA April 2013  (version 1.3)
//                - made the "opt" a boolean argument: decide if one wants to make histograms and fits of
//                  the "matching" parameters and use the fit results for sigmas for the cuts; added plots of parameters
//                - changed range on matching parameter histograms, to better ensure convergent Gaussian fits
//                - forced minimum values for sigmas of matching parameters (in case Gaussian fits fail)
//                - changed histogram ranges for the matching parameters for the fitting.
//                - loop over all MD hits, to seek hits in correct octant in "prompt" window
//                - define range for MD prompt as constants
//                - make histograms of R2 hit multiplicities
//                - make histograms of matching parameters for "good" tracks used in Q^2 plots
// DSA April 2013 (version 1.4) 
//                - added more diagnostic histograms      
// DSA June 2013 (version 1.5) 
//                - added alternate versions of Q2 plots
// DSA June 2014 (version 1.6) 
//                - kludge fix to phi for octant 5 (180 degrees); change
//                  histogram resolution
// DSA November 2014 (version 1.7) 
//                - add light-weighted Phi histograms
//                - add MD ADC pedestal determination
//                - save all plots as .pdf
//                - add VDC residuals plots
// DSA January 2015 (version 1.8) 
//                - count raw number of bridged tracks
//                - count number of events with a partial track in R2 and in R3
//                - count number of R2 and R3 partial tracks in bridged tracks
//               
// DSA July 16 2015 (version 1.9)
//                - check package number for track based on fBack partial track (so "reverse" runs will work)
//                - include light-weighted Q^2 and Theta plots
//
// DSA NOTE: beware, matching parameter cuts don't work here for "reverse" runs; need
// to steal code from auto_Q2_check which does this correctly.

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

// below are various cut parameters. Note: need to check that the MD time windows are correct for all running periods!
const int md_low=-210;
const int md_high=-150;
const int multiple=18;
const float gain_factor=1.0;

void q2_check(int event_start=-1,int event_end=-1,int run=8658, bool opt=false, TString stem="Qweak_", string suffix=""){

   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/";
   //ss << folder << "/tracking/";
   ss << stem.Data();
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());
   TString outputPrefix(Form("Q2_check_run%d_",run));

   TH1F* angle=new TH1F("angle","Scattering Angle",100,0,15);
   TH1F* q2=new TH1F("Q2","Q2",100,0,0.2);
   TH1F* angle_1=new TH1F("angle_1","Scattering Angle in Package 1",100,0,15);
   TH1F* angle_2=new TH1F("angle_2","Scattering Angle in Package 2",100,0,15);
   TH1F* q2_1=new TH1F("q2_1","Q2 distribution in Package 1",100,0,0.2);
   TH1F* q2_2=new TH1F("q2_2","Q2 distribution in Package 2",100,0,0.2);

   TH1F* angle_1_lw=new TH1F("angle_1","Scattering Angle in Package 1, light-weighted",100,0,15);
   TH1F* angle_2_lw=new TH1F("angle_2","Scattering Angle in Package 2, light-weighted",100,0,15);
   TH1F* q2_1_lw=new TH1F("q2_1","Q2 distribution in Package 1, light-weighted",100,0,0.2);
   TH1F* q2_2_lw=new TH1F("q2_2","Q2 distribution in Package 2,light-weighted",100,0,0.2);

   TH1F* q2_1_test=new TH1F("q2_1_test","Q2 Package 1, VDC wires cut",100,0,0.2);
   TH1F* q2_1_test2=new TH1F("q2_1_test2","Q2 Package 1, VDC bad wires ",100,0,.2);

   TH1F* q2_1_v0=new TH1F("q2_1_v0","Q2_v0 (with eloss) in Package 1",100,0,0.2);
   TH1F* q2_1_v3=new TH1F("q2_1_v3","Q2_v3 (with eloss) in Package 1",100,-.3,0.2);
   TH1F* q2_v0=new TH1F("q2_v0","Q2_v0 (generic with eloss) both Packages",100,0,0.2);
   TH1F* q2_v4=new TH1F("q2_v4","Q2_v4 (generic no eloss) both Packages",100,0,0.2);
   TH1F* q2_v5=new TH1F("q2_v5","Q2_v5 (elastic no eloss) both Packages",100,0,0.2);

   TH1F* q2_2_v0=new TH1F("q2_2_v0","Q2_v0 (with eloss) in Package 2",100,0,0.2);
   TH1F* q2_2_v3=new TH1F("q2_2_v3","Q2_v3 (with eloss) in Package 2",100,-.3,0.2);

   TH1F* eprime_1=new TH1F("eprime_1","E' in Package 1",120,0.6,1.2);
   TH1F* eloss_1=new TH1F("eloss_1","E (with eloss) in Package 1",100,1.0,1.2);
   TH1F* e_0=new TH1F("e_0","E_0 incident beam energy",100,1.0,1.2);

   TH1F* eprime_2=new TH1F("eprime_2","E' in Package 2",120,0.6,1.2);
   TH1F* eloss_2=new TH1F("eloss_2","E (with eloss) in Package 2",100,1.0,1.2);

   TH1F* chi_1=new TH1F("chi_1","Chi distribution in Package 1",100,0,10);
   TH1F* chi_2=new TH1F("chi_2","Chi distribution in Package 2",100,0,10);

   TH1F* chi_vdc_1=new TH1F("chi_vdc_1","VDC partial track chi, Package 1",100,0,5);
   TH1F* chi_vdc_2=new TH1F("chi_vdc_2","VDC partial track chi, Package 2",100,0,5);

   TH1F* chi_hdc_1=new TH1F("chi_hdc_1","HDC chi distribution in Package 1",100,0,10);
   TH1F* chi_hdc_2=new TH1F("chi_hdc_2","HDC chi distribution in Package 2",100,0,10);

   TH1F* momentum_1=new TH1F("momentum_1","Momentum in Package 1",100,0,1500);
   TH1F* momentum_2=new TH1F("momentum_2","Momentum in Package 2",100,0,1500);

   TH1F* phi_1=new TH1F("phi_1","Phi in Package 1",400,-200,200);
   TH1F* phi_2=new TH1F("phi_2","Phi in Package 2",400,-200,200);

   TH1F* phi_1p=new TH1F("phi_1p","Phi in Package 1, weighted by MD+",400,-200,200);
   TH1F* phi_1m=new TH1F("phi_1m","Phi in Package 1, weighted by MD-",400,-200,200);
   TH1F* phi_2p=new TH1F("phi_2p","Phi in Package 2, weighted by MD+",400,-200,200);
   TH1F* phi_2m=new TH1F("phi_2m","Phi in Package 2, weighted by MD-",400,-200,200);

   TH1F* r2_hits_1=new TH1F("r2_hits_1","Wires Hit in Region 2 for tracks in Package 1",50,0,50);
   TH1F* r2_hits_2=new TH1F("r2_hits_2","Wires Hit in Region 2 for tracks in Package 2",50,0,50);
   TH1F* md_time_1=new TH1F("md_time_1","MD hit time, Package 1",80,-400,0);
   TH1F* md_time_2=new TH1F("md_time_2","MD hit time, Package 2",80,-400,0);

   TH1F* p1_theta=new TH1F("p1_theta","Theta Direction mismatch, Package 1",200,-0.05,0.05);
   TH1F* p2_theta=new TH1F("p2_theta","Theta Direction mismatch, Package 2",200,-0.05,0.05);
   TH1F* p1_phi=new TH1F("p1_phi","Phi Direction mismatch, Package 1",200,-0.2,0.2);
   TH1F* p2_phi=new TH1F("p2_phi","Phi Direction mismatch, Package 2",200,-0.2,0.2);
   TH1F* p1_theta_p=new TH1F("p1_theta_p","Theta Position mismatch, Package 1",200,-0.002,0.002);
   TH1F* p2_theta_p=new TH1F("p2_theta_p","Theta Position mismatch, Package 2",200,-0.002,0.002);
   TH1F* p1_phi_p=new TH1F("p1_phi_p","Phi Position mismatch, Package 1",200,-0.2,0.2);
   TH1F* p2_phi_p=new TH1F("p2_phi_p","Phi Position mismatch, Package 2",200,-0.2,0.2);

   TH1F* n_tracks_1=new TH1F("n_tracks_1","Number of tracks per event, Package 1",10,0,10);
   TH1F* n_tracks_2=new TH1F("n_tracks_2","Number of tracks per event, Package 2",10,0,10);

   TH1D* adcpped_1 = new TH1D("ADCPPED_1", "ADCP pedestal, Package 1", 50, 0, 400);
   TH1D* adcmped_1 = new TH1D("ADCMPED_1", "ADCM pedestal, Package 1", 50, 0, 400);
   TH1D* adcpped_2 = new TH1D("ADCPPED_2", "ADCP pedestal, Package 2", 50, 0, 400);
   TH1D* adcmped_2 = new TH1D("ADCMPED_2", "ADCM pedestal, Package 2", 50, 0, 400);

   TH1D* adcp_1 = new TH1D("ADCP_1", "ADCP, ped subtracted, Package 1", 50, 0, 4000);
   TH1D* adcm_1 = new TH1D("ADCM_1", "ADCM, ped subtracted, Package 1", 50, 0, 4000);
   TH1D* adcp_2 = new TH1D("ADCP_2", "ADCP, ped subtracted, Package 2", 50, 0, 4000);
   TH1D* adcm_2 = new TH1D("ADCM_2", "ADCM, ped subtracted, Package 2", 50, 0, 4000);
   TH1D* adc_ratio_1 = new TH1D("ADC_ratio_1", "ln(ADCP/ADCM), ped subtracted, Package 1", 100, -5, 5);
   TH1D* adc_ratio_2 = new TH1D("ADC_ratio_2", "ln(ADCP/ADCM), ped subtracted, Package 2", 100, -5, 5);
   
   TH1D* R1 = new TH1D ("R1","Pkg 1: weighted chi, VDC partial tracks",100,0,5);
   TH1D* R2 = new TH1D ("R2","Pkg 2: weighted chi, VDC partial tracks",100,0,5);

   h_wires_hit_1= new TH1F("h_wires_hit_1", "Vader wire hits in tracks  V-Plane ", 280, 0.5, 280.5);
   h_wires_hit_2= new TH1F("h_wires_hit_2", "Vader wire hits in tracks U-Plane ", 280, 0.5, 280.5);
   h_wires_hit_3= new TH1F("h_wires_hit_3", "Leia wire hits in tracks V-Plane ", 280, 0.5, 280.5);
   h_wires_hit_4= new TH1F("h_wires_hit_4", "Leia wire hits in tracks U-Plane ", 280, 0.5, 280.5);
   h_wires_hit_5= new TH1F("h_wires_hit_5", "Yoda wire hits in tracks  V-Plane ", 280, 0.5, 280.5);
   h_wires_hit_6= new TH1F("h_wires_hit_6", "Yoda wire hits in tracks U-Plane ", 280, 0.5, 280.5);
   h_wires_hit_7= new TH1F("h_wires_hit_7", "Han wire hits in tracks V-Plane ", 280, 0.5, 280.5);
   h_wires_hit_8= new TH1F("h_wires_hit_8", "Han wire hits in tracks U-Plane ", 280, 0.5, 280.5);

   h_vdc_residual_signed_1= new TH1F("h_vdc_residual_signed_1", "VDC Package 1 ", 200, -2, 2);
   h_vdc_residual_1= new TH1F("h_vdc_residual_1", "Package 1 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_1_1= new TH1F("h_vdc_residual_1_1", "Package 1 Plane 1 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_1_2= new TH1F("h_vdc_residual_1_2", "Package 1 Plane 2 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_1_3= new TH1F("h_vdc_residual_1_3", "Package 1 Plane 3 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_1_4= new TH1F("h_vdc_residual_1_4", "Package 1 Plane 4 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_signed_2= new TH1F("h_vdc_residual_signed_2", "VDC Package 2 ", 200, -2, 2);
   h_vdc_residual_2= new TH1F("h_vdc_residual_2", "Package 2 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_2_1= new TH1F("h_vdc_residual_2_1", "Package 2 Plane 1 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_2_2= new TH1F("h_vdc_residual_2_2", "Package 2 Plane 2 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_2_3= new TH1F("h_vdc_residual_2_3", "Package 2 Plane 3 VDC wire residual ", 200, 0.0, 0.2);
   h_vdc_residual_2_4= new TH1F("h_vdc_residual_2_4", "Package 2 Plane 4 VDC wire residual ", 200, 0.0, 0.2);

   QwEvent* fEvent=0;
   QwTrack* track=0;
   QwKinematics* kinematic=0;
   TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
   
   //try to get the oct number from the run number
   int oct=getOctNumber(event_tree);
   
   Int_t nevents=event_tree->GetEntries();
   cout << "total events: " << nevents << endl;
   int start=(event_start==-1)? 0:event_start;
   int end=(event_end==-1)? nevents:event_end;

    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    TBranch* maindet_branch=event_tree->GetBranch("maindet");
    TBranch* trig_branch=event_tree->GetBranch("trigscint");
    event_branch->SetAddress(&fEvent);
    int md_1=(oct+4)%8;
    int md_2=oct;
    
    TLeaf* mdp_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
    TLeaf* mdm_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
    TLeaf* mdp_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
    TLeaf* mdm_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));

    TLeaf* mdadcp_1=maindet_branch->GetLeaf(Form("md%dp_adc",md_1));
    TLeaf* mdadcm_1=maindet_branch->GetLeaf(Form("md%dm_adc",md_1));
    TLeaf* mdadcp_2=maindet_branch->GetLeaf(Form("md%dp_adc",md_2));
    TLeaf* mdadcm_2=maindet_branch->GetLeaf(Form("md%dm_adc",md_2));

    double mean_thetaoff_pkg1=0,sigma_thetaoff_pkg1=1;
    double mean_thetaoff_pkg2=0,sigma_thetaoff_pkg2=1;
    double mean_phioff_pkg1=0, sigma_phioff_pkg1=1;
    double mean_phioff_pkg2=0,sigma_phioff_pkg2=1;       

    Double_t md_adc_p_1_ped = 220;
    Double_t md_adc_m_1_ped = 220;
    Double_t md_adc_p_2_ped = 220;
    Double_t md_adc_m_2_ped = 220;

    // In the section below, we plot the various track matching parameters (the differences in the angles in theta
    // and phi found in Region3 from those found from swimming the Region 2 track through the magnetic field), and
    // fit these distributions to Gaussians, to determine the means and sigma's of the distributions.

    if(opt){

      cout << "opt is true, so matching parameters means and sigmas determined from data" << endl;

      TH1F* pkg1_theta=new TH1F("theta1","theta1",500,-0.1,0.1);
      TH1F* pkg2_theta=new TH1F("theta2","theta2",500,-0.1,0.1);
      TH1F* pkg1_phi=new TH1F("phi1","phi1",500,-1,1);
      TH1F* pkg2_phi=new TH1F("phi2","phi2",500,-1,1);

      TF1* f2=new TF1("f2","gaus",-1,1);
      f2->SetParameters(1,0.0,0.01);
      TF1* f1=new TF1("f1","gaus",-1,1);
      f1->SetParameters(1,0.0,0.01);
      TF1* f3=new TF1("f3","gaus",-1,1);
      f1->SetParameters(1,0.0,0.01);
      TF1* f4=new TF1("f4","gaus",-1,1);
      f1->SetParameters(1,0.0,0.01);

      event_tree->Project("theta1","events.fQwTracks.fDirectionThetaoff","events.fQwTracks.fPackage==1");
      pkg1_theta->Fit("f1","");
      event_tree->Project("theta2","events.fQwTracks.fDirectionThetaoff","events.fQwTracks.fPackage==2");
      pkg2_theta->Fit("f2","");
      event_tree->Project("phi1","events.fQwTracks.fDirectionPhioff","events.fQwTracks.fPackage==1");
      pkg1_phi->Fit("f3","");
      event_tree->Project("phi2","events.fQwTracks.fDirectionPhioff","events.fQwTracks.fPackage==2");
      pkg2_phi->Fit("f4","");
      
      mean_thetaoff_pkg1=f1->GetParameter(1);
      sigma_thetaoff_pkg1=f1->GetParameter(2);
      cout << "Package 1 thetaoff mean = " << mean_thetaoff_pkg1 << "   sigma  = " << sigma_thetaoff_pkg1 << endl;

      mean_thetaoff_pkg2=f2->GetParameter(1);
      sigma_thetaoff_pkg2=f2->GetParameter(2);
      cout << "Package 2 thetaoff mean = " << mean_thetaoff_pkg2 << "   sigma  = " << sigma_thetaoff_pkg2 << endl;

      mean_phioff_pkg1=f3->GetParameter(1);
      sigma_phioff_pkg1=f3->GetParameter(2);
      cout << "Package 1 phioff mean = " << mean_phioff_pkg1 << "   sigma  = " << sigma_phioff_pkg1 << endl;

      mean_phioff_pkg2=f4->GetParameter(1);
      sigma_phioff_pkg2=f4->GetParameter(2);
      cout << "Package 2 phioff mean = " << mean_phioff_pkg2 << "   sigma  = " << sigma_phioff_pkg2 << endl;
      
      TCanvas* c8=new TCanvas("c8","Track Matching Parameters",800,600);
      c8->Divide(2,2);
      c8->cd(1);
      c8_1->SetLogy();
      theta1->Draw();
      c8->cd(2);
      c8_2->SetLogy();
      theta2->Draw();
      c8->cd(3);
      c8_3->SetLogy();
      phi1->Draw();
      c8->cd(4);
      c8_4->SetLogy();
      phi2->Draw();
    }

    //  set minimum values for the sigmas of matching parameters, in case "opt" was chosen but Gaussian fit fails
    //  these are in units of radians. Empirically chosen minimum values

    if (sigma_phioff_pkg1 < 0.01) sigma_phioff_pkg1=0.01;
    if (sigma_phioff_pkg2 < 0.01) sigma_phioff_pkg2=0.01;
    if (sigma_thetaoff_pkg1 < 0.001) sigma_thetaoff_pkg1=0.001;
    if (sigma_thetaoff_pkg2 < 0.001) sigma_thetaoff_pkg2=0.001;


    // Below is width of cuts in sigma.. should probably be something like 3
    double width=1000;

    double pkg1_phioff_lower=mean_phioff_pkg1-width*sigma_phioff_pkg1;
    double pkg1_phioff_upper=mean_phioff_pkg1+width*sigma_phioff_pkg1;
    double pkg2_phioff_lower=mean_phioff_pkg2-width*sigma_phioff_pkg2;
    double pkg2_phioff_upper=mean_phioff_pkg2+width*sigma_phioff_pkg2;

    double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*sigma_thetaoff_pkg1;
    double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*sigma_thetaoff_pkg1;
    double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*sigma_thetaoff_pkg2;
    double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*sigma_thetaoff_pkg2;
   
    

  // this bit is to get quick ADC pedestal estimates
    for (int i=0;i<10000;i++)
      {
	maindet_branch->GetEntry(i);

	Double_t pdataplus1 = mdp_1->GetValue();
	Double_t pdataminus1 = mdm_1->GetValue();
	Double_t pdataplus2 = mdp_2->GetValue();
	Double_t pdataminus2 = mdm_2->GetValue();
	Double_t padcp1 = mdadcp_1->GetValue();
	Double_t padcm1 = mdadcm_1->GetValue();
	Double_t padcp2 = mdadcp_2->GetValue();
	Double_t padcm2 = mdadcm_2->GetValue();
	
	if(pdataplus1 == 0 && pdataminus1 == 0)
	  {
	    adcpped_1->Fill(padcp1);
	    adcmped_1->Fill(padcm1);
	  }
	if(pdataplus2 == 0 && pdataminus2 == 0)
	  {
	    adcpped_2->Fill(padcp2);
	    adcmped_2->Fill(padcm2);
	  }
	//	}
      }
    md_adc_p_1_ped = adcpped_1->GetMean();
    md_adc_m_1_ped = adcmped_1->GetMean();
    md_adc_p_2_ped = adcpped_2->GetMean();
    md_adc_m_2_ped = adcmped_2->GetMean();


  
    int n_pt_r3_p1 = 0;
    int n_pt_r3_p2 = 0;
    int n_pt_r2_p1 = 0;
    int n_pt_r2_p2 = 0;
    int n_pt_r2andr3_p1 = 0;
    int n_pt_r2andr3_p2 = 0;

    // Now, loop over all events

    for(int i=start;i<end;++i){

      if(i%100000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      trig_branch->GetEntry(i);
       
      bool prompt_mdp1 = false;
      bool prompt_mdm1 = false;
      bool prompt_mdp2 = false;
      bool prompt_mdm2 = false;

      // Look at first hits in the appropriate Main Detector channels...
      double mdp_value_1=mdp_1->GetValue();
      double mdm_value_1=mdm_1->GetValue();
      double mdp_value_2=mdp_2->GetValue();
      double mdm_value_2=mdm_2->GetValue();
      md_time_1->Fill(mdp_value_1);
      md_time_1->Fill(mdm_value_1);
      md_time_2->Fill(mdp_value_2);
      md_time_2->Fill(mdm_value_2);

          
      // find the number of wires hit in Region 2 in each package
      int nhits=fEvent->GetNumberOfHits();
      int valid_hits_1=0,valid_hits_2=0;

      // Counting number of events with partial tracks in each region
      bool has_r3_pt_p1=false;
      bool has_r2_pt_p1=false;
      bool has_r3_pt_p2=false;
      bool has_r2_pt_p2=false;
 
      int npartials=fEvent->GetNumberOfPartialTracks();
      for(int j=0;j<npartials;j++){ 
	        const QwPartialTrack* pt = fEvent->GetPartialTrack(j);
                pt=fEvent->GetPartialTrack(j);
                if(pt->GetRegion()==3){
                  if(pt->GetPackage()==1){
		    n_pt_r3_p1++;
		    has_r3_pt_p1=true;
                  }
                  if(pt->GetPackage()==2){
		    n_pt_r3_p2++;
		    has_r3_pt_p2=true;
                  }
                }
                if(pt->GetRegion()==2){
                  if(pt->GetPackage()==1){
		    n_pt_r2_p1++;
		    has_r2_pt_p1=true;
                  }
                  if(pt->GetPackage()==2){
		    n_pt_r2_p2++;
		    has_r2_pt_p2=true;
                  }
                }
      }
      if (has_r2_pt_p1 && has_r3_pt_p1) n_pt_r2andr3_p1++;
      if (has_r2_pt_p2 && has_r3_pt_p2) n_pt_r2andr3_p2++;

      // Now, only look at events with valid tracks

      double ntracks=fEvent->GetNumberOfTracks();

      if (ntracks>0){

	// Loop over hits to a) get Region 2 hit multiplicity in each package and b) look for prompt MD hits in each package
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  //	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0){
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0){
	    if(hit->GetPackage()==1)
	      ++valid_hits_1;
	    else
	      ++valid_hits_2;
	  }
	  // look for Main Detector hits in the "prompt" time window; MainDetector data is "Region=5"
          // recall that in QwHits the "plane" for MD hits is the octant number, and element = 1 for PMT_positive, =2 for PMT_negative
	  if(hit->GetRegion()==5 && hit->GetPlane()==md_1 && hit->GetElement()==1 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdp1=true;
	  if(hit->GetRegion()==5 && hit->GetPlane()==md_1 && hit->GetElement()==2 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdm1=true;
	  if(hit->GetRegion()==5 && hit->GetPlane()==md_2 && hit->GetElement()==1 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdp2=true;
	  if(hit->GetRegion()==5 && hit->GetPlane()==md_2 && hit->GetElement()==2 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdm2=true;
	}

	if(ntracks>0){
	track=fEvent->GetTrack(0);	// DSA: only look at first track in event

	double angle_val=fEvent->GetScatteringAngle();

	if(track->fBack->GetPackage()==1){
	  n_tracks_1->Fill(ntracks);

	  r2_hits_1->Fill(valid_hits_1);
	  if (valid_hits_1 < multiple && prompt_mdp1 && prompt_mdm1){
	    if(track->fDirectionPhioff>pkg1_phioff_lower && track->fDirectionPhioff<pkg1_phioff_upper && track->fDirectionThetaoff>pkg1_thetaoff_lower && track->fDirectionThetaoff<pkg1_thetaoff_upper ){
	      double Q2_val=fEvent->fKinElasticWithLoss.fQ2;
	      angle_1->Fill(angle_val);
	      q2_1->Fill(Q2_val);
	      angle->Fill(angle_val);
	      q2->Fill(Q2_val);
	      chi_1->Fill(track->fChi);
	      momentum_1->Fill(track->fMomentum);

	      // DSA Kludge: for octant 5 (180 degrees) move by 180's so not at ends of histogram
	      double temp = 57.2958*track->fFront->GetMomentumDirectionPhi();
	      if (temp < -155) temp=180+temp;
	      if (temp > 155) temp=temp-180;
	      //	      phi_1->Fill(57.2958*track->fFront->GetMomentumDirectionPhi());
	      phi_1->Fill(temp);
	      
	      double  adc_1p = mdadcp_1->GetValue();
	      double  adc_1m = mdadcm_1->GetValue();

 	      // Do light-weighted versions
	      angle_1_lw->Fill(angle_val,abs(adc_1p - md_adc_p_1_ped + adc_1m - md_adc_m_1_ped));
	      adcp_1->Fill(adc_1p - md_adc_p_1_ped);
	      adcm_1->Fill(adc_1m - md_adc_m_1_ped);
	      adc_ratio_1->Fill(log((adc_1p - md_adc_p_1_ped)/(adc_1m - md_adc_m_1_ped+.00001)));

              q2_1_lw->Fill(Q2_val,abs(gain_factor*(adc_1p - md_adc_p_1_ped) + adc_1m - md_adc_m_1_ped));

	      phi_1p->Fill(temp,abs(adc_1p-md_adc_p_1_ped));
	      phi_1m->Fill(temp,abs(adc_1m-md_adc_m_1_ped));
	      
	      double Q2_from_nu_1 = fEvent->fKinWithLoss.fNu*2.0*(0.938272);
	      double Q2_zero_1 = fEvent->fKinWithLoss.fQ2; 
	      double Q2_noloss_1 = fEvent->fKin.fQ2;
	      double Q2_noloss_elastic_1 = fEvent->fKinElastic.fQ2;
	      q2_1_v3->Fill(Q2_from_nu_1);
	      q2_1_v0->Fill(Q2_zero_1);
	      q2_v0->Fill(Q2_zero_1);
	      q2_v4->Fill(Q2_noloss_1);
	      q2_v5->Fill(Q2_noloss_elastic_1);

	      eprime_1->Fill(fEvent->fKin.fPp);
	      eloss_1->Fill(fEvent->fKinWithLoss.fP0);
	      e_0->Fill(fEvent->fKin.fP0);
	      p1_theta->Fill(track->fDirectionThetaoff);
	      p1_phi->Fill(track->fDirectionPhioff);
	      p1_theta_p->Fill(track->fPositionThetaoff);
	      p1_phi_p->Fill(track->fPositionPhioff);

	      int icrap=0;

      	      // now, histogram information related to the VDCs for these good tracks
	      int nPTs = track->GetNumberOfPartialTracks();
	      for (int b = 0; b < nPTs; b++)
		{
		  const QwPartialTrack* fPt = track->GetPartialTrack(b);
		  if(fPt->GetRegion()==2){

		    double chi_hdc = fPt->fChi;
		    chi_hdc_1->Fill(chi_hdc);
		  }
		  if(fPt->GetRegion()==3){

		    double chi_vdc = fPt->fChi;
		    chi_vdc_1->Fill(chi_vdc);

 		    double chiweight = fPt->GetChiWeight();

		    R1->Fill(chiweight);
		    int nTLs = fPt->GetNumberOfTreeLines();

		    for (int a = 0; a < nTLs; a++ )
		      {
			const QwTreeLine* const_treeline = fPt->GetTreeLine(a);
			int nhits = const_treeline->GetNumberOfHits();

			for(int j=0;j<nhits;j++)
			  {
			    const QwHit* hit = const_treeline->GetHit(j);
				int itemp = hit->GetElement();

				if (hit->GetPlane()==4 && itemp>60 && itemp<70) icrap = 1;

	                        float vdc_resid = hit->GetTreeLineResidual();
				// the below is a kludge to fake signed residuals, in mm
				if (itemp%2==0)
				  h_vdc_residual_signed_1->Fill(10.*vdc_resid);
				else
				  h_vdc_residual_signed_1->Fill(-10.0*vdc_resid);
				// end of kludge

				h_vdc_residual_1->Fill(vdc_resid);
				if (hit->GetPlane()==1){
				  h_wires_hit_1->Fill(itemp);
				  h_vdc_residual_1_1->Fill(vdc_resid);
				}
				if (hit->GetPlane()==2){
				  h_wires_hit_2->Fill(itemp);
				  h_vdc_residual_1_2->Fill(vdc_resid);
				}
				if (hit->GetPlane()==3){
				  h_wires_hit_3->Fill(itemp);
				  h_vdc_residual_1_3->Fill(vdc_resid);
				}
				if (hit->GetPlane()==4){
				  h_wires_hit_4->Fill(itemp);
				  h_vdc_residual_1_4->Fill(vdc_resid);
				}

			  } // loop over hits
		      } // loop over Treelines
		  }  // check if is Region 3 PT
		} // loop over all PTs
	      if(icrap==0) q2_1_test->Fill(Q2_val);
	      if(icrap==1) q2_1_test2->Fill(Q2_val);
      	    } // good track
	  }
	}
	else if(track->fBack->GetPackage()==2){
	   n_tracks_2->Fill(ntracks);
	   r2_hits_2->Fill(valid_hits_2);
	   if (valid_hits_2 < multiple && prompt_mdp2 && prompt_mdm2){
	     if(track->fDirectionPhioff>pkg2_phioff_lower && track->fDirectionPhioff<pkg2_phioff_upper && track->fDirectionThetaoff>pkg2_thetaoff_lower && track->fDirectionThetaoff<pkg2_thetaoff_upper){
	       double Q2_val=fEvent->fKinElasticWithLoss.fQ2;
	       angle_2->Fill(angle_val);
	       q2_2->Fill(Q2_val);
	       angle->Fill(angle_val);
	       q2->Fill(Q2_val);
 	       chi_2->Fill(track->fChi);
	       momentum_2->Fill(track->fMomentum);

	       double temp = 57.2958*track->fFront->GetMomentumDirectionPhi();
 	       phi_2->Fill(temp);

	       double  adc_2p = mdadcp_2->GetValue();
	       double  adc_2m = mdadcm_2->GetValue();
	       phi_2p->Fill(temp,abs(adc_2p-md_adc_p_2_ped));
	       phi_2m->Fill(temp,abs(adc_2m-md_adc_m_2_ped));

 	       // Do light-weighted versions
	       angle_2_lw->Fill(angle_val,abs(adc_2p - md_adc_p_2_ped + adc_2m - md_adc_m_2_ped));
	       q2_2_lw->Fill(Q2_val,abs(gain_factor*(adc_2p - md_adc_p_2_ped) + adc_2m - md_adc_m_2_ped));
	       adcp_2->Fill(adc_2p - md_adc_p_2_ped);
	       adcm_2->Fill(adc_2m - md_adc_m_2_ped);
	       adc_ratio_2->Fill(log((adc_2p - md_adc_p_2_ped)/(adc_2m - md_adc_m_2_ped+.00001)));

	       eprime_2->Fill(fEvent->fKin.fPp);
	       eloss_2->Fill(fEvent->fKinWithLoss.fP0);
	       e_0->Fill(fEvent->fKin.fP0);
	       double delta_e_2 = 1.15575 - fEvent->fKinWithLoss.fP0;   // get pre-scattering energy loss	       
	       double post_loss = 0.0191 - delta_e_2;   // estimate post-scattering loss
               double Q2_from_nu_2 = 2.0*(0.938272)*(fEvent->fKinWithLoss.fP0 - (fEvent->fKin.fPp+post_loss));

	       double Q2_zero_2 = fEvent->fKinWithLoss.fQ2; 
	       double Q2_noloss_2 = fEvent->fKin.fQ2;
	       double Q2_noloss_elastic_2 = fEvent->fKinElastic.fQ2;
	       
	       q2_2_v3->Fill(Q2_from_nu_2);
	       q2_2_v0->Fill(Q2_zero_2);
	       q2_v0->Fill(Q2_zero_2);
	       q2_v4->Fill(Q2_noloss_2);
	       q2_v5->Fill(Q2_noloss_elastic_2);

	       p2_theta->Fill(track->fDirectionThetaoff);
	       p2_phi->Fill(track->fDirectionPhioff);
	       p2_theta_p->Fill(track->fPositionThetaoff);
	       p2_phi_p->Fill(track->fPositionPhioff);

      	      // now, histogram information related to the VDCs for these good tracks
	      int nPTs = track->GetNumberOfPartialTracks();
	      for (int b = 0; b < nPTs; b++)
		{
		  const QwPartialTrack* fPt = track->GetPartialTrack(b);
		  if(fPt->GetRegion()==2){

		    double chi_hdc = fPt->fChi;
		    chi_hdc_2->Fill(chi_hdc);
		  }
		  if(fPt->GetRegion()==3){
		    double chi_vdc = fPt->fChi;
		    chi_vdc_2->Fill(chi_vdc);
		    double chiweight = fPt->GetChiWeight();

		    R2->Fill(chiweight);
		    int nTLs = fPt->GetNumberOfTreeLines();

		    for (int a = 0; a < nTLs; a++ )
		      {
			const QwTreeLine* const_treeline = fPt->GetTreeLine(a);
			int nhits = const_treeline->GetNumberOfHits();

			for(int j=0;j<nhits;j++)
			  {
			    const QwHit* hit = const_treeline->GetHit(j);
				int itemp = hit->GetElement();

	                        float vdc_resid = hit->GetTreeLineResidual();

				// the below is a kludge to fake signed residuals, in mm
				if (itemp%2==0)
				  h_vdc_residual_signed_2->Fill(10.*vdc_resid);
				else
				  h_vdc_residual_signed_2->Fill(-10.0*vdc_resid);
				// end of kludge
				h_vdc_residual_2->Fill(vdc_resid);
				if (hit->GetPlane()==1){
				  h_wires_hit_5->Fill(itemp);
				  h_vdc_residual_2_1->Fill(vdc_resid);
				}
				if (hit->GetPlane()==2){
				  h_wires_hit_6->Fill(itemp);
				  h_vdc_residual_2_2->Fill(vdc_resid);
				}
				if (hit->GetPlane()==3){
				  h_wires_hit_7->Fill(itemp);
				  h_vdc_residual_2_3->Fill(vdc_resid);
				}
				if (hit->GetPlane()==4){
				  h_wires_hit_8->Fill(itemp);
				  h_vdc_residual_2_4->Fill(vdc_resid);
				}
			  } // loop over hits
		      } // loop over Treelines
		  }  // check if is Region 3 PT
		} // loop over all PTs
	     } // good tracks
	   }
	}
	}   
      }  //extra
    }  // end of loop over events
       

    // Now, do all the output to plots and cout

    cout << "Events with Region 2 Partial Tracks, package 1:  " << n_pt_r2_p1 << endl;
    cout << "Events with Region 2 Partial Tracks, package 2:  " << n_pt_r2_p2 << endl;
    cout << "Events with Region 3 Partial Tracks, package 1:  " << n_pt_r3_p1 << endl;
    cout << "Events with Region 3 Partial Tracks, package 2:  " << n_pt_r3_p2 << endl;
    cout << "Events with partials tracks in both R2 and R3, package 1: " << n_pt_r2andr3_p1 << endl;
    cout << "Events with partials tracks in both R2 and R3, package 2: " << n_pt_r2andr3_p2 << endl;
    cout << "Events with Bridged Tracks:" << endl;
    cout << "pkg1: " << n_tracks_1->GetEntries() << endl;
    cout << "pkg2: " << n_tracks_2->GetEntries() << endl;
    cout << "Events with Good Bridged Tracks:" << endl;
    cout << "pkg1: " << angle_1->GetEntries() << endl;
    cout << "pkg2: " << angle_2->GetEntries() << endl;

    std::ofstream fout;
    fout.open(outputPrefix+"track_counting.txt");
    fout << "Events with Region 2 Partial Tracks, package 1:  " << n_pt_r2_p1 << endl;
    fout << "Events with Region 2 Partial Tracks, package 2:  " << n_pt_r2_p2 << endl;
    fout << "Events with Region 3 Partial Tracks, package 1:  " << n_pt_r3_p1 << endl;
    fout << "Events with Region 3 Partial Tracks, package 2:  " << n_pt_r3_p2 << endl;
    fout << "Events with partials tracks in both R2 and R3, package 1: " << n_pt_r2andr3_p1 << endl;
    fout << "Events with partials tracks in both R2 and R3, package 2: " << n_pt_r2andr3_p2 << endl;
    fout << "Events with Bridged Tracks:" << endl;
    fout << "pkg1: " << n_tracks_1->GetEntries() << endl;
    fout << "pkg2: " << n_tracks_2->GetEntries() << endl;
    fout << "Events with Good Bridged Tracks:" << endl;
    fout << "pkg1: " << angle_1->GetEntries() << endl;
    fout << "pkg2: " << angle_2->GetEntries() << endl;


    fout << "q2: " << endl;
    if(q2->GetEntries()) {
      fout << "all: " <<  setprecision(5) << 1000*q2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2->GetRMS()/sqrt(q2->GetEntries()) << endl;
    }
    if(q2_1->GetEntries()!=0) {
      fout << "pkg1: " <<  setprecision(5) << 1000*q2_1->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_1->GetRMS()/sqrt(q2_1->GetEntries()) << endl;
    }
    if(q2_2->GetEntries()!=0) {
      fout << "pkg2: " <<  setprecision(5) << 1000*q2_2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_2->GetRMS()/sqrt(q2_2->GetEntries()) << endl;
    }
    if(q2_1_lw->GetEntries()!=0) {
      fout << "lw pkg1: " <<  setprecision(5) << 1000*q2_1_lw->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_1_lw->GetRMS()/sqrt(q2_1_lw->GetEntries()) << endl;
    }
    if(q2_2->GetEntries()!=0) {
      fout << "lw pkg2: " <<  setprecision(5) << 1000*q2_2_lw->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_2_lw->GetRMS()/sqrt(q2_2_lw->GetEntries()) << endl;
    }

    fout << "Theta: " << endl;
    if(angle_1->GetEntries()) {
      fout << "pkg1: " <<  setprecision(5) << angle_1->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << angle_1->GetRMS()/sqrt(angle_1->GetEntries()) << endl;
    }
    if(angle_2->GetEntries()) {
      fout << "pkg2: " <<  setprecision(5) << angle_2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << angle_2->GetRMS()/sqrt(angle_2->GetEntries()) << endl;
    }
    if(angle_1_lw->GetEntries()) {
      fout << "lw pkg1: " <<  setprecision(5) << angle_1_lw->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << angle_1_lw->GetRMS()/sqrt(angle_1_lw->GetEntries()) << endl;
    }
    if(angle_2_lw->GetEntries()) {
      fout << "lw pkg2: " <<  setprecision(5) << angle_2_lw->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << angle_2_lw->GetRMS()/sqrt(angle_2_lw->GetEntries()) << endl;
    }

    fout.close();

    cout << "scattering angle: " << endl;
    if(angle->GetEntries()) {
      cout << "all: " << setprecision(5) << angle->GetMean() << " error RMS/sqrt(N): " << setprecision(4) << angle->GetRMS()/sqrt(angle->GetEntries()) << endl; 
    }
    if(angle_1->GetEntries()!=0) {
      cout << "pkg1: " << setprecision(5) << angle_1->GetMean() << " error RMS/sqrt(N): " << setprecision(4) << angle_1->GetRMS()/sqrt(angle_1->GetEntries()) << endl;
    }
    if(angle_2->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << angle_2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << angle_2->GetRMS()/sqrt(angle_2->GetEntries()) << endl;
    }

    cout << "q2: " << endl;
    if(q2->GetEntries()) {
      cout << "all: " <<  setprecision(5) << 1000*q2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2->GetRMS()/sqrt(q2->GetEntries()) << endl;
    }
    if(q2_1->GetEntries()!=0) {
      cout << "pkg1: " <<  setprecision(5) << 1000*q2_1->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_1->GetRMS()/sqrt(q2_1->GetEntries()) << endl;
    }
    if(q2_2->GetEntries()!=0) {
      cout << "pkg2: " <<  setprecision(5) << 1000*q2_2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_2->GetRMS()/sqrt(q2_2->GetEntries()) << endl;
    }
    
    TCanvas* c2=new TCanvas("c2","Region 2 Wire Hits and MD time hit times",700,500);
    c2->Divide(2,2);
    c2->cd(1);
    r2_hits_1->SetTitle("Number of valid Hits, Region 2 for Package 1");
    r2_hits_1->GetXaxis()->SetTitle("Number of Hits");
    c2_1->SetLogy();
    r2_hits_1->Draw();
    c2->cd(2);
    r2_hits_2->SetTitle("Number of valid Hits, Region 2 for Package 2");
    r2_hits_2->GetXaxis()->SetTitle("Number of Hits");
    c2_2->SetLogy();
    r2_hits_2->Draw();
    c2->cd(3);
    md_time_1->SetTitle("Main Detector Time, Package 1");
    md_time_1->GetXaxis()->SetTitle("Time (ns)");
    c2_3->SetLogy();
    md_time_1->Draw();
    c2->cd(4);
    md_time_2->SetTitle("Main Detector Time, Package 2");
    md_time_2->GetXaxis()->SetTitle("Time (ns)");
    c2_4->SetLogy();
    md_time_2->Draw();
    c2->SaveAs(outputPrefix+"r2hits_MDtimes.pdf");


    TCanvas* cv=new TCanvas("cv","Q^2 different versions ",800,600);
    cv->Divide(2,2);
    cv->cd(1);
    q2_1_v0->Draw();
    q2_1_v0->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1_v0->SetTitle("Q2 version 0 in Package 1");
    cv->cd(2);
    q2_1_v3->Draw();
    q2_1_v3->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1_v3->SetTitle("Q2 version 3 in Package 1");
    cv->cd(3);
    q2_2_v0->Draw();
    q2_2_v0->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_2_v0->SetTitle("Q2 version 0 in Package 2");
    cv->cd(4);
    q2_2_v3->Draw();
    q2_2_v3->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_2_v3->SetTitle("Q2 version 3 in Package 2");
    cv->SaveAs(outputPrefix+"Q2_versions.pdf");

    TCanvas* cw=new TCanvas("cw","Q^2 more versions ",800,600);
    cw->Divide(2,2);
    cw->cd(1);
    q2_v4->Draw();
    q2_v4->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_v4->SetTitle("Q2 version 4 generic no loss both Packages");
    cw->cd(2);
    q2_v5->Draw();
    q2_v5->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_v5->SetTitle("Q2 version 5 elastic no loss both Packages");
    cw->cd(3);
    q2->Draw();
    q2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2->SetTitle("Q2 v1: elastic with eloss both Packages ");
    cw->cd(4);
    q2_v0->Draw();
    q2_v0->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_v0->SetTitle("Q2 v0: generic with eloss both Packages ");
    cw->SaveAs(outputPrefix+"Q2_more.pdf");

    TCanvas* ce=new TCanvas("ce","E, Eprime  ",800,600);
    ce->Divide(2,3);
    ce->cd(1);
    eprime_1->Draw();
    eprime_1->GetXaxis()->SetTitle("Eprime (GeV)");
    eprime_1->SetTitle("Eprime in Package 1");
    ce->cd(2);
    eloss_1->Draw();
    eloss_1->GetXaxis()->SetTitle("Eprime (GeV");
    eloss_1->SetTitle("E with Eloss in Package 1");
    ce->cd(3);
    eprime_2->Draw();
    eprime_2->GetXaxis()->SetTitle("Eprime (GeV)");
    eprime_2->SetTitle("Eprime in Package 2");
    ce->cd(4);
    eloss_2->Draw();
    eloss_2->GetXaxis()->SetTitle("Eprime (GeV)");
    eloss_2->SetTitle("E with Eloss in Package 2");
    ce->cd(5);
    gStyle->SetStatFormat("7.7g");
    e_0->Draw(); 
    e_0->GetXaxis()->SetTitle("E0 (GeV)");
    e_0->SetTitle("E_0 incident beam energy ");
    ce->SaveAs(outputPrefix+"Eprimes.pdf");


    TCanvas* c=new TCanvas("c","Scattering Angle and Q^2",800,600);
    c->Divide(2,3);
    c->cd(1);
    angle->Draw();
    angle->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle->SetTitle("Scattering Angle Distribution");

    c->cd(2);
    q2->Draw();
    q2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2->SetTitle("Q2 Distribution ");
    
    c->cd(3);
    angle_1->Draw();
    angle_1->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle_1->SetTitle("Scattering Angle Distribution in Package 1");

    c->cd(4);
    q2_1->Draw();
    q2_1->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1->SetTitle("Q2 Distribution in Package 1");
    
    c->cd(5);
    angle_2->Draw();
    angle_2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    angle_2->SetTitle("Scattering Angle Distribution in Package 2");

    c->cd(6);
    q2_2->Draw();
    q2_2->GetXaxis()->SetTitle("Scattering Angle: degree");
    q2_2->SetTitle("Q2 Distribution in Package 2");
    c->SaveAs(outputPrefix+"Q2_and_Theta.pdf");


    TCanvas* cadc=new TCanvas("cadc","MD ADCs, pedestal subtracted, light-weighted",500,500);

    cadc->Divide(2,3);
    gStyle->SetStatW(0.4);                
    gStyle->SetStatH(0.3);                
    cadc->cd(1);
    adcp_1->SetLineColor(2);
    adcp_1->Draw();
    cadc->cd(2);
    adcm_1->SetLineColor(2);
    adcm_1->Draw();
    cadc->cd(3);
    adcp_2->SetLineColor(4);
    adcp_2->Draw();
    cadc->cd(4);
    adcm_2->SetLineColor(4);
    adcm_2->Draw();
    cadc->cd(5);
    adc_ratio_1->SetLineColor(2);
    adc_ratio_1->Draw();
    cadc->cd(6);
    adc_ratio_2->SetLineColor(4);
    adc_ratio_2->Draw();
    cadc->SaveAs(outputPrefix+"MD_ADCs_ped_subtracted.pdf");

    TCanvas* c1=new TCanvas("c1","Scattering Angle and Q^2, light-weighted",800,600);

    c1->Divide(2,2);
    c1->cd(1);
    angle_1_lw->SetLineColor(2);
    angle_1_lw->Draw();
    angle_1_lw->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle_1_lw->SetTitle("light-weighted Scattering Angle Distribution in Package 1");

    c1->cd(2);
    q2_1_lw->SetLineColor(2);
    q2_1_lw->Draw();
    q2_1_lw->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1_lw->SetTitle("light-weighted Q2 Distribution in Package 1");
    
    c1->cd(3);
    angle_2_lw->SetLineColor(3);
    angle_2_lw->Draw();
    angle_2_lw->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    angle_2_lw->SetTitle("light-weighted Scattering Angle Distribution in Package 2");

    c1->cd(4);
    q2_2_lw->SetLineColor(3);
    q2_2_lw->Draw();
    q2_2_lw->GetXaxis()->SetTitle("Scattering Angle: degree");
    q2_2_lw->SetTitle("light-weighted Q2 Distribution in Package 2");
    c1->SaveAs(outputPrefix+"Q2_and_Theta_lightweighted.pdf");
    
    // make plots of matching parameters for all tracks used in Q2 plots
    TCanvas* c7=new TCanvas("c7","Matching Parameters in Good Tracks (after cuts)",800,600);
    gStyle->SetStatW(0.45);
    gStyle->SetStatH(0.25);

    c7->Divide(4,2);
    c7->cd(1);
    c7_1->SetLogy();
    p1_theta->Draw();
    c7->cd(2);
    c7_2->SetLogy();
    p1_phi->Draw();
    c7->cd(3);
    c7_3->SetLogy();
    p1_theta_p->Draw();
    c7->cd(4);
    c7_4->SetLogy();
    p1_phi_p->Draw();
    c7->cd(5);
    c7_5->SetLogy();
    p2_theta->Draw();
    c7->cd(6);
    c7_6->SetLogy();
    p2_phi->Draw();
    c7->cd(7);
    c7_7->SetLogy();
    p2_theta_p->Draw();
    c7->cd(8);
    c7_8->SetLogy();
    p2_phi_p->Draw();
    c7->SaveAs(outputPrefix+"MatchingParameters.pdf");

    TCanvas* c4=new TCanvas("c4","Ntracks, Chi, momentum, phi for Good Tracks (after cuts)",800,600);

    c4->Divide(4,2);
    c4->cd(1); 
    c4_1->SetLogy();
    n_tracks_1->Draw();
    c4->cd(5);
    c4_5->SetLogy();
    n_tracks_2->Draw();
    c4->cd(2);
    chi_1->Draw();
    c4->cd(6);
    chi_2->Draw();
    c4->cd(3);
    c4_3->SetLogy();
    momentum_1->Draw();
    c4->cd(7);
    c4_7->SetLogy();
    momentum_2->Draw();
    c4->cd(4);
    //    c4_4->SetLogy();
    phi_1->Draw();
    c4->cd(8);
    //    c4_8->SetLogy();
    phi_2->Draw();
    c4->SaveAs(outputPrefix+"Ntracks_Chi_Phi.pdf");

    TCanvas* c5=new TCanvas("c5","Phi from HDC, weighted by MDs",800,600);

    c5->Divide(2,2);
    c5->cd(1);
    phi_1p->Draw();
    c5->cd(2);
    phi_1m->Draw();
    c5->cd(3);
    phi_2p->Draw();
    c5->cd(4);
    phi_2m->Draw();
    c5->SaveAs(outputPrefix+"LightWeighted_Phi.pdf");

    TCanvas* c6=new TCanvas("c6","MD ADC pedestals",400,400);
    c6->Divide(2,2);
    c6->cd(1);
    adcpped_1->Draw();
    c6->cd(2);
    adcmped_1->Draw();
    c6->cd(3);
    adcpped_2->Draw();
    c6->cd(4);
    adcmped_2->Draw();
    c6->SaveAs(outputPrefix+"MDC_ADC_pedestals.pdf");

    TCanvas *wires_c = new TCanvas("wires_c","Vader and Leia (Package 1)    Wires Hit",450,450);
    h_wires_hit_1 -> SetLineColor(2);
    h_wires_hit_1 -> GetXaxis()-> SetTitle("Wire #    Plane 1");
    h_wires_hit_2 -> SetLineColor(2);
    h_wires_hit_2 -> GetXaxis()-> SetTitle("Wire #    Plane 2");
    h_wires_hit_3 -> SetLineColor(4);
    h_wires_hit_3 -> GetXaxis()-> SetTitle("Wire #    Plane 3");
    h_wires_hit_4 -> SetLineColor(4);
    h_wires_hit_4 -> GetXaxis()-> SetTitle("Wire #    Plane 4");
    
    gStyle->SetOptStat(1111);
    wires_c->Clear();
    wires_c->Divide(2,2);
    wires_c->cd(1);
    h_wires_hit_1 -> Draw();
    wires_c->cd(2);
    h_wires_hit_2 -> Draw();
    wires_c->cd(3);
    h_wires_hit_3 -> Draw();
    wires_c->cd(4);
    h_wires_hit_4 -> Draw();
    wires_c->SaveAs(outputPrefix+"wires_package1.pdf");

    TCanvas *wires_c2 = new TCanvas("wires_c2","Yoda and Han (Package 2)    Wires Hit",500,0,450,450);
    h_wires_hit_5 -> SetLineColor(3);
    h_wires_hit_5 -> GetXaxis()-> SetTitle("Wire #    Plane 1");
    h_wires_hit_6 -> SetLineColor(3);
    h_wires_hit_6 -> GetXaxis()-> SetTitle("Wire #    Plane 2");
    h_wires_hit_7 -> SetLineColor(6);
    h_wires_hit_7 -> GetXaxis()-> SetTitle("Wire #    Plane 3");
    h_wires_hit_8 -> SetLineColor(6);
    h_wires_hit_8 -> GetXaxis()-> SetTitle("Wire #    Plane 4");
    
    gStyle->SetOptStat(1111);
    wires_c2->Clear();
    wires_c2->Divide(2,2);
    wires_c2->cd(1);
    h_wires_hit_5 -> Draw();
    wires_c2->cd(2);
    h_wires_hit_6 -> Draw();
    wires_c2->cd(3);
    h_wires_hit_7 -> Draw();
    wires_c2->cd(4);
    h_wires_hit_8 -> Draw();
    wires_c2->SaveAs(outputPrefix+"wires_package2.pdf");

    TCanvas *resid_c = new TCanvas("resid_c","VDC wire residuals",-20,0,450,450);
    resid_c->Divide(2,2);
    resid_c->cd(1);
    h_vdc_residual_1->Draw();
    resid_c->cd(2);
    h_vdc_residual_2->Draw();
    resid_c->cd(3);
    R1->Draw();
    resid_c->cd(4);
    R2->Draw();
    resid_c->SaveAs(outputPrefix+"VDC_residuals.pdf");

    TCanvas *resid_plane_c = new TCanvas("resid_plane_c","VDC wire residuals by plane",-20,0,800,800);
    resid_plane_c->Divide(4,2);
    resid_plane_c->cd(1);
    h_vdc_residual_1_1->Draw();
    resid_plane_c->cd(2);
    h_vdc_residual_1_2->Draw();
    resid_plane_c->cd(3);
    h_vdc_residual_1_3->Draw();
    resid_plane_c->cd(4);
    h_vdc_residual_1_4->Draw();
    resid_plane_c->cd(5);
    h_vdc_residual_2_1->Draw();
    resid_plane_c->cd(6);
    h_vdc_residual_2_2->Draw();
    resid_plane_c->cd(7);
    h_vdc_residual_2_3->Draw();
    resid_plane_c->cd(8);
    h_vdc_residual_2_4->Draw();
    resid_plane_c->SaveAs(outputPrefix+"VDC_residuals_by_plane.pdf");

    TCanvas *resid_signed_c = new TCanvas("resid_signed_c"," VDC signed residuals ",-20,0,400,400);
    //    gStyle->SetOptStat(1);
    resid_signed_c->Divide(2,3);
    resid_signed_c->cd(1);
    h_vdc_residual_signed_1->Draw();
    resid_signed_c->cd(2);
    h_vdc_residual_signed_2->Draw();
    resid_signed_c->cd(3);
    chi_vdc_1->Draw();
    resid_signed_c->cd(4);
    chi_vdc_2->Draw();
    resid_signed_c->cd(5);
    chi_hdc_1->Draw();
    resid_signed_c->cd(6);
    chi_hdc_2->Draw();
    resid_signed_c->SaveAs(outputPrefix+"VDC_residuals_signed.pdf");

    TCanvas* crap=new TCanvas("crap","Scattering Angle and Q^2",800,800);
    crap->Divide(1,3);
    crap->cd(1);
    q2_1->Draw();
    q2_1->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1->SetTitle("Q2 package 1");
    crap->cd(2);
    q2_1_test->Draw();
    q2_1_test->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1_test->SetTitle("Q2 package 1, cut out VDC wires");
    crap->cd(3);
    q2_1_test2->Draw();
    q2_1_test2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1_test2->SetTitle("Q2 package 1, cut region VDC wires");

    return;
 }


//Function- taking the run number fiqure out which octant is package 2 for region 2, which is what q2_check 
//needs to get the right values

int getOctNumber(TTree* event_tree){

//get a histogram (called h) of the octant cutting on region 3 and pacakge 2 - only one octant will be return - the one we want

event_tree->Draw("events.fQwPartialTracks.fOctant>>h","events.fQwPartialTracks.fRegion==3&&events.fQwPartialTracks.fPackage==2");

//event_tree->Draw("events.fQwPartialTracks.fOctant>>h","events.fQwPartialTracks.fRegion==2&&events.fQwPartialTracks.fPackage==2");

//get the mean of histgram h made above, this returns a double value that is the truncated to integer which is the region 3 pacakge 2 octant number

int j = int (h->GetMean());

return (j);

}



void sim(Int_t run=8658,std::string element="fQ2"){

  TDirectory * where=gDirectory;
  TFile sim_file(Form("%s/qweak_sim.root",gSystem->Getenv("QW_ROOTFILES")));
  TFile data_file(Form("%s/Qweak_%d_base_cut_v1.root",gSystem->Getenv("QW_ROOTFILES"),run));
  TFile data_file_1(Form("%s/Qweak_%d_base_cut_v2.root",gSystem->Getenv("QW_ROOTFILES"),run));

  TTree* sim_tree=(TTree*)sim_file.Get("h8013");
  TTree* data_tree=(TTree*)data_file.Get("event_tree");
  TTree* data_tree_1=(TTree*)data_file_1.Get("event_tree");
  where->cd();
  TH1F* sim_h=new TH1F("sim","sim",100,4,14);
  TH1F* data_h=new TH1F("data","data",100,4,14);
  TH1F* data_h_1=new TH1F("data_1","data",100,4,14);
  string sim_string;
  if(element=="fQ2")
    sim_string="q__2";
  else if(element=="fScatteringAngle")
    sim_string="theta_o";

  sim_tree->Project("sim",Form("%s",sim_string.c_str()));
  
  data_tree->Project("data",Form("events.fQwTracks.%s",element.c_str()),"events.fQwTracks.fScatteringAngle>4 && events.fQwTracks.fScatteringAngle<12");
  data_tree_1->Project("data_1",Form("events.fQwTracks.%s",element.c_str()),"events.fQwTracks.fScatteringAngle>4 && events.fQwTracks.fScatteringAngle<12");
  sim_h->SetLineColor(kRed);
  sim_h->Draw();
  sim_h->SetTitle("sim(Red) vs real data: method1(Blue) vs method2(Cyan)");
  sim_h->GetXaxis()->SetTitle(Form("x axis:%s",element.c_str()));
  data_h->SetLineColor(kBlue);
  data_h->Scale(sim_h->GetEntries()/data_h->GetEntries());
  data_h->Draw("same");
  //data_h_1->SetLineColor(6);
  //data_h_1->Scale(sim_h->GetEntries()/data_h_1->GetEntries());
  //data_h_1->Draw("same");

  return;

}
