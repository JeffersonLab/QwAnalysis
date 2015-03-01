// Name:   Q2_check.C
// Author: Siyuan Yang
// Email:  sxyang@email.wm.edu
// Date:   Monday Jan 9th,2012
// Version:1.3
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

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>


// below are various cut parameters. Note: need to chckec that the MD time windows are correct for all running periods!
const int md_low=-210;
const int md_high=-150;
const int multiple=18;

void Q2_check(int event_start=-1,int event_end=-1,int run=8658, bool opt=false, TString stem="Qweak_", string suffix=""){

  //cross section value - needed for simualtion files ran through qwsimtracking
  // for data will be set equal to 1
  double CrossSection = 0;


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

   TH1F* angle=new TH1F("angle","Scattering Angle",100,0,15);
   TH1F* q2=new TH1F("Q2","Q2",100,0,0.12);
   TH1F* angle_1=new TH1F("angle_1","Scattering Angle in Package 1",100,0,15);
   TH1F* angle_2=new TH1F("angle_2","Scattering Angle in Package 2",100,0,15);
   TH1F* q2_1=new TH1F("q2_1","Q2 distribution in Package 1",100,0,0.12);
   TH1F* q2_2=new TH1F("q2_2","Q2 distribution in Package 2",100,0,0.12);
   TH1F* r2_hits_1=new TH1F("r2_hits_1","Wires Hit in Region 2 for tracks in Package 1",50,0,50);
   TH1F* r2_hits_2=new TH1F("r2_hits_2","Wires Hit in Region 2 for tracks in Package 2",50,0,50);
   TH1F* md_time_1=new TH1F("md_time_1","MD hit time, Package 1",80,-400,0);
   TH1F* md_time_2=new TH1F("md_time_2","MD hit time, Package 2",80,-400,0);

   TH1F* p1_theta=new TH1F("p1_theta","Theta Direction mismatch, Package 1",200,-0.05,0.05);
   TH1F* p2_theta=new TH1F("p2_theta","Theta Direction mismatch, Package 2",200,-0.05,0.05);
   TH1F* p1_phi=new TH1F("p1_phi","Phi Direction mismatch, Package 1",200,-0.2,0.2);
   TH1F* p2_phi=new TH1F("p2_phi","Phi Direction mismatch, Package 2",200,-0.2,0.2);
   TH1F* p1_theta_p=new TH1F("p1_theta_p","Theta Position mismatch, Package 1",200,-0.01,0.01);
   TH1F* p2_theta_p=new TH1F("p2_theta_p","Theta Position mismatch, Package 2",200,-0.01,0.01);
   TH1F* p1_phi_p=new TH1F("p1_phi_p","Phi Position mismatch, Package 1",200,-0.2,0.2);
   TH1F* p2_phi_p=new TH1F("p2_phi_p","Phi Position mismatch, Package 2",200,-0.2,0.2);

   QwEvent* fEvent=0;
   QwTrack* track=0;
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

  if(maindet_branch)
    {
      TLeaf* mdp_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
      TLeaf* mdm_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
      TLeaf* mdp_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
      TLeaf* mdm_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));
    }

    double mean_thetaoff_pkg1=0,sigma_thetaoff_pkg1=1;
    double mean_thetaoff_pkg2=0,sigma_thetaoff_pkg2=1;
    double mean_phioff_pkg1=0, sigma_phioff_pkg1=1;
    double mean_phioff_pkg2=0,sigma_phioff_pkg2=1;       

    // In the section below, we plot the various track matching parameters (the differences in the angles in theta
    // and phi found in Region3 from those found from swimming the Region 2 track through the magnetic field), and
    // fit these distributions to Gaussians, to determine the means and sigma's of the distributions.

    if(opt){

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
   
    
    // Now, loop over all events
  
    for(int i=start;i<end;++i){

      if(i%100000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);

      if(trig_branch)
        {
          trig_branch->GetEntry(i);
        }

      if(maindet_branch)
      {
        maindet_branch->GetEntry(i);

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

        CrossSection = 1;

     }else
        {
          bool prompt_mdp1 = true;
          bool prompt_mdm1 = true;
          bool prompt_mdp2 = true;
          bool prompt_mdm2 = true;

         CrossSection = fEvent->fCrossSection;

        }

      double ntracks=fEvent->GetNumberOfTracks();
          
      // find the number of wires hit in Region 2 in each package
      int nhits=fEvent->GetNumberOfHits();
      int valid_hits_1=0,valid_hits_2=0;

      // Now, only look at events with valid tracks

      if (ntracks>0){
	// Loop over hits to a) get Region 2 hit multiplicity in each package and b) look for prompt MD hits in each package
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0){
	    if(hit->GetPackage()==1)
	      ++valid_hits_1;
	    else
	      ++valid_hits_2;
	  }
    if(maindet_branch)
    {
      // look for Main Detector hits in the "prompt" time window; MainDetector data is "Region=5"
       // recall that in QwHits the "plane" for MD hits is the octant number, and element = 1 for PMT_positive, =2 for PMT_negative
      if(hit->GetRegion()==5 && hit->GetPlane()==md_1 && hit->GetElement()==1 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdp1=true;
      if(hit->GetRegion()==5 && hit->GetPlane()==md_1 && hit->GetElement()==2 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdm1=true;
	    if(hit->GetRegion()==5 && hit->GetPlane()==md_2 && hit->GetElement()==1 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdp2=true;
	    if(hit->GetRegion()==5 && hit->GetPlane()==md_2 && hit->GetElement()==2 && hit->GetTimeNs()>md_low && hit->GetTimeNs()<md_high) prompt_mdm2=true;
    }
	}

	if(ntracks>0){
	track=fEvent->GetTrack(0);	// DSA: only look at first track in event

	double angle_val=fEvent->GetScatteringAngle();
	//	double angle_val=track->fTheta;

	if(track->GetPackage()==1){
	  r2_hits_1->Fill(valid_hits_1);
	  if (valid_hits_1 < multiple && prompt_mdp1 && prompt_mdm1){
	    if(track->fDirectionPhioff>pkg1_phioff_lower && track->fDirectionPhioff<pkg1_phioff_upper && track->fDirectionThetaoff>pkg1_thetaoff_lower && track->fDirectionThetaoff<pkg1_thetaoff_upper ){
	      double Q2_val=fEvent->fKinElasticWithLoss.fQ2;
	      angle_1->Fill(angle_val,CrossSection);
	      q2_1->Fill(Q2_val,CrossSection);
	      angle->Fill(angle_val,CrossSection);
	      q2->Fill(Q2_val,CrossSection);

	      p1_theta->Fill(track->fDirectionThetaoff,CrossSection);
	      p1_phi->Fill(track->fDirectionPhioff,CrossSection);
	      p1_theta_p->Fill(track->fPositionThetaoff,CrossSection);
	      p1_phi_p->Fill(track->fPositionPhioff,CrossSection);
	    }
	  }
	}
	else if(track->GetPackage()==2){
	   r2_hits_2->Fill(valid_hits_2);
	   if (valid_hits_2 < multiple && prompt_mdp2 && prompt_mdm2){
	     if(track->fDirectionPhioff>pkg2_phioff_lower && track->fDirectionPhioff<pkg2_phioff_upper && track->fDirectionThetaoff>pkg2_thetaoff_lower && track->fDirectionThetaoff<pkg2_thetaoff_upper){
	       double Q2_val=fEvent->fKinElasticWithLoss.fQ2;
	       angle_2->Fill(angle_val,CrossSection);
	       q2_2->Fill(Q2_val,CrossSection);
	       angle->Fill(angle_val,CrossSection);
	       q2->Fill(Q2_val,CrossSection);

	       p2_theta->Fill(track->fDirectionThetaoff,CrossSection);
	       p2_phi->Fill(track->fDirectionPhioff,CrossSection);
	       p2_theta_p->Fill(track->fPositionThetaoff,CrossSection);
	       p2_phi_p->Fill(track->fPositionPhioff,CrossSection);
	     }
	   }
	}
	}   
      }  //extra
    }  // end of loop over events
       

    cout << "tracks per trigger" << endl;
    cout << "pkg1: " << angle_1->GetEntries() << endl;
    cout << "pkg2: " << angle_2->GetEntries() << endl;

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
    r2_hits_1->SetTitle("Number Wires Hits, Region 2 for Package 1");
    r2_hits_1->GetXaxis()->SetTitle("Number of Wires Hit");
    c2_1->SetLogy();
    r2_hits_1->Draw();
    c2->cd(2);
    r2_hits_2->SetTitle("Number Wires Hits, Region 2 for Package 2");
    r2_hits_2->GetXaxis()->SetTitle("Number of Wires Hit");
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


    TCanvas* c=new TCanvas("c","Scattering Angle and Q^2",800,600);
    c->Divide(2,3);
    c->cd(1);
    angle->Draw();
    angle->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle->SetTitle("Scattering Angle Distribution in Package 1");

    c->cd(2);
    q2->Draw();
    q2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2->SetTitle("Q2 Distribution in Package 1");
    
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
    
    // make plots of matching parameters for all tracks used in Q2 plots
    TCanvas* c=new TCanvas("c7","Matching Parameters in Good Tracks (after cuts)",800,600);
    gStyle->SetStatW(0.45);
    gStyle->SetStatH(0.25);

    c7->Divide(4,2);
    c7->cd(1);
    p1_theta->Draw();
    c7->cd(2);
    p1_phi->Draw();
    c7->cd(3);
    p1_theta_p->Draw();
    c7->cd(4);
    p1_phi_p->Draw();
    c7->cd(5);
    p2_theta->Draw();
    c7->cd(6);
    p2_phi->Draw();
    c7->cd(7);
    p2_theta_p->Draw();
    c7->cd(8);
    p2_phi_p->Draw();


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
