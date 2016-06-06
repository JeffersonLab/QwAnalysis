// Name:   Q2_check.C
// Author: Siyuan Yang
// Email:  sxyang@email.wm.edu
// Date:   Monday Jan 9th,2012
// Version:1.3
//
//
// This script is used to extract the Q2 information for both packages
// from the rootfiles. 

//  Now it has two filters turned on, one is the MD
// cut, which requires the R3 track to hit the MD. The other is the number
// of valid hits in R2 and this is used to eliminate the cases when there
// are more than one track going through the R2 at one event. The variable
// multiple sets the threshold for that multiplicity cases.
//
//
// Examples
// root[0] .L Extensions/Macros/Tracking/Q2_check.C
// root[1] q2_check(-1,-1,13653)
// this will draw the scattering angle and Q2 for both packages over the entire 
// range of events in Qweak_13653.
//
// notion: the package number is always consistent with package number in R3
//
// UPDATE on Feb 12th, 2012
// remove the oct number in the argument list. The octant number will be automatically deduced give 
// the run number by the getOctNumber function. This modification saves people's time to remeber 
// the octant number for specific run.
// Modifed: 06-29-2012 by Valerie Gray
//
// Modified Feb 28 2014 by DSA (version 1.3)
//   - Q2 is now fKin.KenElasticWithLoss.fQ2, i.e. Q^2_1 (with eloss); was Q^2_0
//   - loop over all MD TDC hits, and accept track if both PMTs  for given octant
//     fired in the prompt time window for at least one of the hits
//   - only uses first bridged track in the event
//   - format improvements/additions for text output file
//   - choose logy scale for Q2 and theta plots, to look for tails/screwy events
//   - redo "track matching parameter" cuts; the original version used fits to gaussians, but
//     even with the initial values and ranges of histograms chosen carefully, they sometimes
//     lead to crazy fits; see replace by simple GetMean and GetRMS calls. 
//     Add careful check on "package label" for matching parameters in case of "reverse runs"; 
//       package for matching variable checks follows the Region 3 package. 
//   - made the Draw commands for the track matching parameters use the first 500K events, in case
//     of runs where beam was off at start (in these cases, the means were zero, which might not
//     be correct for tight values of "width" parameter)
//   - added text output for matching parameters, and changed to a 3 sigma cut
//   - deleted some unused variables
//
// Modified 50-06-2014
// Added in some more text file output so that I can make the ROOT tree :)
// Also added the sig digets function

#include "auto_shared_includes.h"

#include "TSystem.h"
#include "TF1.h"
#include "TLeaf.h"

#include "QwEvent.h"
#include "QwTrack.h"
#include "QwHit.h"

#include <fstream>
#include <iostream>
#include <iomanip>

/***********************************
set up the significant figures right - &plusmn is for html,
  change to +/- if not useing,
  or in this case \t for a tab space in the outputfile
Use this by calling: value_with_error(a,da)
***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
  << std::setprecision((size_t) (std::log10((fabs(a))/(da)) - std::log10(fabs(a)) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)

// Threshold for defining the maximum number of allowed hits per event per package
const int multiple=18;
//const int multiple=15;

// below are various cut parameters. Note: need to check that the MD time windows are correct for all running periods!
int md_low=-210;
int md_high=-150;

int getOctNumber(TChain* event_tree){

//get a histogram (called h) of the octant cutting on region 3 and pacakge 2 - only one octant will be return - the one we want

TH1F* h = new TH1F("h","Region 3, Package 2 Octant number",9,-0.5,8.5);

event_tree->Draw("events.fQwPartialTracks.fOctant>>h","events.fQwPartialTracks.fRegion==3&&events.fQwPartialTracks.fPackage==2","GOFF",100000);

//get the mean of histgram h made above, this returns a double value that is the trunkated to interger which is the region 3 pacakge 2 octant number

int j = int (h->GetMean());

delete h;

return (j);
}


void auto_Q2_check(Int_t runnum, Bool_t isFirst100K = kFALSE, int event_start=-1, int event_end=-1, TString stem="Qweak_", TString suffix=""){


  // changed the outputPrefix so that it is compatble with both Root and writing to a file by setting the environment properly 
//   TString outputPrefix(Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/%d_",runnum,runnum));

   TString outputPrefix = Form(
       TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
       runnum, runnum);

   // Create and load the chain
   TChain *event_tree = new TChain("event_tree");
   event_tree->Add(Form("$QW_ROOTFILES/%s%d%s.root",stem.Data(),runnum,suffix.Data()));

   // try to get the oct number from the run number
   int oct=getOctNumber(event_tree);
   if (oct == 0 || oct > 8) {
     cout << "octant not valid!" << endl;
     oct = 1;
   }

   // Configure root
   gStyle->SetPalette(1);

   // Define our histograms
   TH1F* angle=new TH1F("Scattering Angle","ScatteringAngle",100,0,15);
   TH1F* q2=new TH1F("Q2","Q2",100,0,0.12);
   TH1F* q2_0=new TH1F("Q2_0","Q2_0",100,0,0.12);
   TH1F* angle_1=new TH1F("Scattering Angle in Package 1","ScatteringAngle in Package 1",100,0,15);
   TH1F* angle_2=new TH1F("Scattering Angle in Package 2","ScatteringAngle in Package 2",100,0,15);
   TH1F* q2_1=new TH1F("Q2 distribution in Package 1","Q2 distribution in Package 1",100,0,0.12);
   TH1F* q2_2=new TH1F("Q2 distribution in Package 2","Q2 distribution in Package 2",100,0,0.12);
   TH1F* q2_0_1=new TH1F("Q2_0 distribution in Package 1","Q2_0 distribution in Package 1",100,0,0.12);
   TH1F* q2_0_2=new TH1F("Q2_0 distribution in Package 2","Q2_0 distribution in Package 2",100,0,0.12);

   TH1F* p1_theta=new TH1F("p1_theta","Theta Direction mismatch, Package 1",200,-0.05,0.05);
   TH1F* p2_theta=new TH1F("p2_theta","Theta Direction mismatch, Package 2",200,-0.05,0.05);
   TH1F* p1_phi=new TH1F("p1_phi","Phi Direction mismatch, Package 1",200,-0.2,0.2);
   TH1F* p2_phi=new TH1F("p2_phi","Phi Direction mismatch, Package 2",200,-0.2,0.2);
   TH1F* p1_theta_p=new TH1F("p1_theta_p","Theta Position mismatch, Package 1",200,-0.01,0.002);
   TH1F* p2_theta_p=new TH1F("p2_theta_p","Theta Position mismatch, Package 2",200,-0.01,0.002);
   TH1F* p1_phi_p=new TH1F("p1_phi_p","Phi Position mismatch, Package 1",200,-0.2,0.2);
   TH1F* p2_phi_p=new TH1F("p2_phi_p","Phi Position mismatch, Package 2",200,-0.2,0.2);

   TH1F* eprime_1=new TH1F("eprime_1","E' in Package 1",120,0.6,1.2);
   TH1F* eprime_2=new TH1F("eprime_2","E' in Package 2",120,0.6,1.2);

   QwEvent* fEvent=0;

   // How many events are in this rootfile?
   Int_t nevents=event_tree->GetEntries();
   cout << "total events: " << nevents << endl;
   int start=(event_start==-1)? 0:event_start;
   int end=(event_end==-1)? nevents:event_end;

   
    // Now get a pointer to the branches so that we can loop through the tree
    TBranch* event_branch     = event_tree->GetBranch("events");
    TBranch* maindet_branch   = event_tree->GetBranch("maindet");
    TBranch* trigscint_branch = event_tree->GetBranch("trigscint");
    if (event_branch)     event_tree->SetBranchStatus("events",1);
    if (maindet_branch)   event_tree->SetBranchStatus("maindet",1);
    if (trigscint_branch) event_tree->SetBranchStatus("trigscint",1);

    event_branch->SetAddress(&fEvent);

    // disable md_low and md_high boundaries for simulation
    if (! maindet_branch) {
      md_low = -100;
      md_high = +100;
    }

    //get the octant numbers
    int md_1=(oct+4)%8; // package 1
    int md_2=oct;       // package 2

    // Temporary histograms to store projection angles
      TH1F* pkg1_theta=new TH1F("pkg1_theta","pkg1_theta",500,-0.1,0.1);
      TH1F* pkg2_theta=new TH1F("pkg2_theta","pkg2_theta",500,-0.1,0.1);
      TH1F* pkg1_phi=new TH1F("pkg1_phi","pkg1_phi",500,-0.2,0.2);
      TH1F* pkg2_phi=new TH1F("pkg2_phi","pkg2_phi",500,-0.2,0.2);

      //  DSA: decide that Gaussian fits still sometimes wonky, so just use simple mean/rms of histogram

      event_tree->Draw("events.fQwTracks.fDirectionThetaoff >> pkg1_theta","events.fQwTracks.fPackage==1","GOFF",50000);
      double mean_thetaoff_pkg1  = pkg1_theta->GetMean();
      double sigma_thetaoff_pkg1 = pkg1_theta->GetRMS();

      event_tree->Draw("events.fQwTracks.fDirectionThetaoff >> pkg2_theta","events.fQwTracks.fPackage==2","GOFF",50000);
      double mean_thetaoff_pkg2  = pkg2_theta->GetMean();
      double sigma_thetaoff_pkg2 = pkg2_theta->GetRMS();

      event_tree->Draw("events.fQwTracks.fDirectionPhioff >> pkg1_phi","events.fQwTracks.fPackage==1","GOFF",50000);
      double mean_phioff_pkg1  = pkg1_phi->GetMean();
      double sigma_phioff_pkg1 = pkg1_phi->GetRMS();

      event_tree->Draw("events.fQwTracks.fDirectionPhioff >> pkg2_phi","events.fQwTracks.fPackage==2","GOFF",50000);
      double mean_phioff_pkg2  = pkg2_phi->GetMean();
      double sigma_phioff_pkg2 = pkg2_phi->GetRMS();



      // set minimum values for sigmas of matching parameters, in case Gaussian fits fails with crazy sigmas
      // these are in units of radians. Empirically chosen minimum values.
    if (sigma_phioff_pkg1 < 0.01) sigma_phioff_pkg1=0.01;
    if (sigma_phioff_pkg2 < 0.01) sigma_phioff_pkg2=0.01;
    if (sigma_thetaoff_pkg1 < 0.001) sigma_thetaoff_pkg1=0.001;
    if (sigma_thetaoff_pkg2 < 0.001) sigma_thetaoff_pkg2=0.001;

     // Set the lower and upper limits of: +-width*sigma around mean
    //     double width=1000;
     double width=3.0;
     double pkg1_phioff_lower=mean_phioff_pkg1-width*sigma_phioff_pkg1;
     double pkg1_phioff_upper=mean_phioff_pkg1+width*sigma_phioff_pkg1;
     double pkg2_phioff_lower=mean_phioff_pkg2-width*sigma_phioff_pkg2;
     double pkg2_phioff_upper=mean_phioff_pkg2+width*sigma_phioff_pkg2;

     double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*sigma_thetaoff_pkg1;
     double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*sigma_thetaoff_pkg1;
     double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*sigma_thetaoff_pkg2;
     double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*sigma_thetaoff_pkg2;

     double thetaoff_lower;
     double thetaoff_upper;
     double phioff_lower;
     double phioff_upper;

     int n_raw_track_1=0;
     int n_raw_track_2=0;

     int n_good_track_1=0;
     int n_good_track_2=0;

     for(int i = start; i < end; ++i) {

      // Announce every 1000'th entry
      if(i%10000==0)
	cout << "events processed so far: " << i << endl;

      if (event_branch)     event_branch->GetEntry(i);
      if (maindet_branch)   maindet_branch->GetEntry(i);
      if (trigscint_branch) trigscint_branch->GetEntry(i);

      // weight of this event, or 1 if the weight is not set (for data)
      double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

      bool prompt_mdp1 = false;
      bool prompt_mdm1 = false;
      bool prompt_mdp2 = false;
      bool prompt_mdm2 = false;

      double ntracks=fEvent->GetNumberOfTracks();

      int nhits=fEvent->GetNumberOfHits();
      int valid_hits_1=0,valid_hits_2=0;

      // extract Q2 and Scattering Angle for all tracks in this event
      for(int j=0;j<nhits;++j){
          // Get pointer to j'th hit in entry i
	  const QwHit* hit=fEvent->GetHit(j);
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0){
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

	  // Until QwSimTracking is fixed to write out the correct plane numbers for the MD, the previous code will never have hits, so we bypass it
	  if (! maindet_branch && hit->GetRegion()==5) {
            prompt_mdp1 = true;
	    prompt_mdm1 = true;
            prompt_mdp2 = true;
	    prompt_mdm2 = true;
 	  }

	}

	if(ntracks > 0)
	{
        // Get only the first track - only one we are looking at
	const QwTrack* track=fEvent->GetTrack(0);

	// Count raw number of bridged tracks in each package 
	if(track->fBack->GetPackage()==1) n_raw_track_1++;
	if(track->fBack->GetPackage()==2) n_raw_track_2++;

        // Check if it is a valid track for the corresponding package
	if(track->fBack->GetPackage()==1 && valid_hits_1 < multiple && prompt_mdm1 && prompt_mdp1){
	  p1_theta->Fill(track->fDirectionThetaoff,w);
	  p1_phi->Fill(track->fDirectionPhioff,w);
	  p1_theta_p->Fill(track->fPositionThetaoff,w);
	  p1_phi_p->Fill(track->fPositionPhioff,w);
	  // since track matching results are stored based on package number of Region 2 (front)
	  //  may need to switch which limits we use if we are in a "reverse" run configuration
	  if(track->fFront->GetPackage()==2){
	    phioff_lower=pkg2_phioff_lower;
	    phioff_upper=pkg2_phioff_upper;
	    thetaoff_lower=pkg2_thetaoff_lower;
	    thetaoff_upper=pkg2_thetaoff_upper;
	  }
	  else{
	    phioff_lower=pkg1_phioff_lower;
	    phioff_upper=pkg1_phioff_upper;
	    thetaoff_lower=pkg1_thetaoff_lower;
	    thetaoff_upper=pkg1_thetaoff_upper;
	  }

	  if(track->fDirectionPhioff>phioff_lower && track->fDirectionPhioff<phioff_upper 
	     && track->fDirectionThetaoff>thetaoff_lower && track->fDirectionThetaoff<thetaoff_upper ){
  
  	  n_good_track_1++;


	  //Valid for package 1 Q^2 and angles, fill the histograms
	  angle_1->Fill(fEvent->fScatteringAngle,w);
	  q2_0_1->Fill(fEvent->fKin.fQ2,w);
	  //	  q2_1->Fill(fEvent->fKin.fQ2);
	  q2_1->Fill(fEvent->fKinElasticWithLoss.fQ2,w);
	  angle->Fill(fEvent->fScatteringAngle,w);
	  //	  q2->Fill(fEvent->fKin.fQ2);
	  q2_0->Fill(fEvent->fKin.fQ2,w);
	  q2->Fill(fEvent->fKinElasticWithLoss.fQ2,w);
	  eprime_1->Fill(fEvent->fKin.fPp,w);
	   }
	}
	  else if(track->fBack->GetPackage()==2 && valid_hits_2 < multiple && prompt_mdm2 && prompt_mdp2){
 
	    p2_theta->Fill(track->fDirectionThetaoff,w);
	    p2_phi->Fill(track->fDirectionPhioff,w);
	    p2_theta_p->Fill(track->fPositionThetaoff,w);
	    p2_phi_p->Fill(track->fPositionPhioff,w);
	  // since track matching results are stored based on package number of Region 2 (front)
	  //  may need to switch which limits we use if we are in a "reverse" run configuration
	    if(track->fFront->GetPackage()==1){
	      phioff_lower=pkg1_phioff_lower;
	      phioff_upper=pkg1_phioff_upper;
	      thetaoff_lower=pkg1_thetaoff_lower;
	      thetaoff_upper=pkg1_thetaoff_upper;
	    }
	    else{
	      phioff_lower=pkg2_phioff_lower;
	      phioff_upper=pkg2_phioff_upper;
	      thetaoff_lower=pkg2_thetaoff_lower;
	      thetaoff_upper=pkg2_thetaoff_upper;
	    }

	    //  these lines below used for debugging
	    //	    	    cout << "front package = " << track->fFront->GetPackage() << "  back package = "
	    //			 << track->fBack->GetPackage() << " pkg2_phioff_lower = " << pkg2_phioff_lower << endl; 
	    //	        cout << " phi lower " << phioff_lower << " phi_Off " << track->fDirectionPhioff << endl;
	    //	        cout << " phi upper " << phioff_upper << " phi_Off " << track->fDirectionPhioff << endl;
	    //	    cout << " theta lower " << thetaoff_lower << " theta_Off " << track->fDirectionThetaoff << endl;
	    //	    cout << " theta upper " << thetaoff_upper << " theta_Off " << track->fDirectionThetaoff << endl;

	    if(track->fDirectionPhioff>phioff_lower && track->fDirectionPhioff<phioff_upper
	       && track->fDirectionThetaoff>thetaoff_lower && track->fDirectionThetaoff<thetaoff_upper){

  	      n_good_track_2++;

	      //if valid for package 2 fill the Q^2 and angle  histograms
		angle_2->Fill(fEvent->fScatteringAngle,w);
		q2_0_2->Fill(fEvent->fKin.fQ2,w);
		//	  		q2_2->Fill(fEvent->fKin.fQ2,w);
		q2_2->Fill(fEvent->fKinElasticWithLoss.fQ2,w);
		angle->Fill(fEvent->fScatteringAngle,w);
		//	  		q2->Fill(fEvent->fKin.fQ2,w);
		q2_0->Fill(fEvent->fKin.fQ2,w);
		q2->Fill(fEvent->fKinElasticWithLoss.fQ2,w);
		eprime_2->Fill(fEvent->fKin.fPp,w);
	   }
	}
      }
    }  // end of for loop over events


    cout << "raw and good tracks per package" << endl;
    if (n_raw_track_1>0){
    cout << "pkg1: raw: " << n_raw_track_1 << "\t good: " << n_good_track_1 << 
      "\t good/raw: " << n_good_track_1/n_raw_track_1 << endl;
    }
    if (n_raw_track_2>0){
    cout << "pkg2: raw: " << n_raw_track_2 << "\t good: " << n_good_track_2 << 
      "\t good/raw: " << n_good_track_2/n_raw_track_2 << endl;
    }

    cout << "scattering angle: " << endl;
    if(angle->GetEntries()!=0) {
      cout << "all: " << std::setprecision(5) << angle->GetMean() << " error RMS/sqrt(N): " << std::setprecision(2) << angle->GetRMS()/sqrt(angle->GetEntries()) << endl; 
    }
    if(n_good_track_1!=0) {
      cout << "pkg1: " << std::setprecision(5) << angle_1->GetMean() << " error RMS/sqrt(N): " << std::setprecision(2) << angle_1->GetRMS()/sqrt(angle_1->GetEntries()) << endl;
    }
    if(n_good_track_2!=0) {
      cout << "pkg2: " <<  std::setprecision(5) << angle_2->GetMean() << " error RMS/sqrt(N): " <<  std::setprecision(2) << angle_2->GetRMS()/sqrt(angle_2->GetEntries()) << endl;
    }

    cout << "q2: " << endl;
    if(q2->GetEntries()!=0) {
      cout << "all: " <<  std::setprecision(5) << 1000*q2->GetMean() << " error RMS/sqrt(N): " <<  std::setprecision(2) << 1000*q2->GetRMS()/sqrt(q2->GetEntries()) << endl;
    }
    if(q2_1->GetEntries()!=0) {
      cout << "pkg1: " <<  std::setprecision(5) << 1000*q2_1->GetMean() << " error RMS/sqrt(N): " <<  std::setprecision(2) << 1000*q2_1->GetRMS()/sqrt(q2_1->GetEntries()) << endl;
    }
    if(q2_2->GetEntries()!=0) {
      cout << "pkg2: " <<  std::setprecision(5) << 1000*q2_2->GetMean() << " error RMS/sqrt(N): " <<  std::setprecision(2) << 1000*q2_2->GetRMS()/sqrt(q2_2->GetEntries()) << endl;
    }
    
    //print to file 
    //this file and evrything related to it is fout
    std::ofstream fout;
    
    //open file
    // open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if needed
    fout.open(outputPrefix+"q2.txt");
    if (!fout.is_open()) cout << "File not opened" << endl;
    
    //outputPrefix will include run number which we need.
    
    //Name what each column is; Note Package 0 is the combination of package 1 and package 2
    //Error is the RMS divided by sqrt(N)
    fout << "Run number:  " << runnum << "\t Total Triggers:  " << nevents << endl;

   
    fout << endl << "What \t Run \t Pkg \t Octant\t Value \t Error" <<endl; 

    fout << "rawTrks\t " << runnum << " \t 1 \t  " << md_1 << "\t" << n_raw_track_1 << "\t \t" << endl;
    fout << "rawTrks\t " << runnum << " \t 2 \t  " << md_2 << "\t" << n_raw_track_2 << "\t \t" << endl;
    fout << "gd Trks\t " << runnum << " \t 1 \t  " << md_1 << "\t" << n_good_track_1 << "\t \t" << endl; 
    fout << "gd Trks\t " << runnum << " \t 2 \t  " << md_2 << "\t" << n_good_track_2 << "\t \t" << endl; 
    if (n_raw_track_1>0){
      fout << "% good\t " << runnum << " \t 1 \t  " << md_1 << "\t" <<std::setprecision(4) << 100.*float(n_good_track_1)/n_raw_track_1 << "\t \t" << endl; 
    }
    if (n_raw_track_2>0){
      fout << "% good\t " << runnum << " \t 2 \t  " << md_2 << "\t" <<  std::setprecision(4) << 100.*float(n_good_track_2)/n_raw_track_2 << "\t \t" << endl; 
    }

    if(angle->GetEntries()!=0) {
      fout << "angle \t " << runnum <<" \t 0 \t \t" << std::setprecision(5) << angle->GetMean() << "\t " << std::setprecision(2) << angle->GetRMS()/sqrt(angle->GetEntries()) << endl; 
    }
    if(n_good_track_1!=0) {
      fout << "angle \t " << runnum <<" \t 1 \t  " << md_1 << "\t" << std::setprecision(5) << angle_1->GetMean() << "\t " << std::setprecision(2) << angle_1->GetRMS()/sqrt(angle_1->GetEntries()) << endl;
    }
    if(n_good_track_2!=0) {
      fout << "angle \t " << runnum <<" \t 2 \t  " << md_2 << "\t" << std::setprecision(5) << angle_2->GetMean() << " \t " <<  std::setprecision(2) << angle_2->GetRMS()/sqrt(angle_2->GetEntries()) << endl;
    }

    if(q2->GetEntries()!=0) {
      fout << "Q2 v.1\t " << runnum <<" \t 0 \t \t" <<  std::setprecision(5) << 1000*q2->GetMean() << " \t " <<  std::setprecision(2) << 1000*q2->GetRMS()/sqrt(q2->GetEntries()) << endl;
    }
    if(q2_1->GetEntries()!=0) {
      fout << "Q2 v.1\t " << runnum <<" \t 1 \t  " << md_1 << "\t"<<  std::setprecision(5) << 1000*q2_1->GetMean() << " \t " <<  std::setprecision(2) << 1000*q2_1->GetRMS()/sqrt(q2_1->GetEntries()) << endl;
    }
    if(q2_2->GetEntries()!=0) {
      fout << "Q2 v.1\t " << runnum <<" \t 2 \t  " << md_2 << "\t"<<  std::setprecision(5) << 1000*q2_2->GetMean() << " \t " <<  std::setprecision(2) << 1000*q2_2->GetRMS()/sqrt(q2_2->GetEntries()) << endl;
    }
    if(q2_0->GetEntries()!=0) {
      fout << "Q2 v.0\t " << runnum <<" \t 0 \t \t" <<  std::setprecision(5) << 1000*q2_0->GetMean() << " \t " <<  std::setprecision(2) << 1000*q2_0->GetRMS()/sqrt(q2_0->GetEntries()) << endl;
    }
    if(q2_0_1->GetEntries()!=0) {
      fout << "Q2 v.0\t " << runnum <<" \t 1 \t  " << md_1 << "\t" <<  std::setprecision(5) << 1000*q2_0_1->GetMean() << " \t " <<  std::setprecision(2) << 1000*q2_0_1->GetRMS()/sqrt(q2_0_1->GetEntries()) << endl;
    }
    if(q2_0_2->GetEntries()!=0) {
      fout << "Q2 v.0\t " << runnum <<" \t 2 \t  " << md_2 << "\t" <<  std::setprecision(5) << 1000*q2_0_2->GetMean() << " \t " <<  std::setprecision(2) << 1000*q2_0_2->GetRMS()/sqrt(q2_0_2->GetEntries()) << endl;
    }
    if(eprime_1->GetEntries()!=0) {
      fout << "E'\t " << runnum <<" \t 1 \t  " << md_1 << "\t" <<  std::setprecision(5) << eprime_1->GetMean() << " \t " <<  std::setprecision(2) << eprime_1->GetRMS()/sqrt(eprime_1->GetEntries()) << endl;
    }
    if(eprime_2->GetEntries()!=0) {
      fout << "E'\t " << runnum <<" \t 2 \t  " << md_2 << "\t" <<  std::setprecision(5) << eprime_2->GetMean() << " \t " <<  std::setprecision(2) << eprime_2->GetRMS()/sqrt(eprime_2->GetEntries()) << endl;
    }

    // Now, output the track matching deviations (before the track matching parameter cuts)
    // Note: the package number here is defined by the Region 3 package
    fout << "  Track Matching: Deviations in radians; Run, Package, Octant, value, error" << endl;

    if(p1_theta->GetEntries()!=0) {
      fout << "Theta\t " << runnum <<" \t 1 \t  " << md_1 << "\t" <<  std::setprecision(3) << p1_theta->GetMean() << "\t\t" <<  std::setprecision(2) << p1_theta->GetRMS()/sqrt(p1_theta->GetEntries()) << endl;
    }
    if(p2_theta->GetEntries()!=0) {
      fout << "Theta\t " << runnum <<" \t 2 \t  " << md_2 << "\t" <<  std::setprecision(3) << p2_theta->GetMean() << "\t\t" <<  std::setprecision(2) << p2_theta->GetRMS()/sqrt(p2_theta->GetEntries()) << endl;
    }
    if(p1_phi->GetEntries()!=0) {
      fout << "Phi\t " << runnum <<" \t 1 \t  " << md_1 << "\t" <<  std::setprecision(3) << p1_phi->GetMean() << "\t\t" <<  std::setprecision(2) << p1_phi->GetRMS()/sqrt(p1_phi->GetEntries()) << endl;
    }
    if(p2_phi->GetEntries()!=0) {
      fout << "Phi\t " << runnum <<" \t 2 \t  " << md_2 << "\t" <<  std::setprecision(3) << p2_phi->GetMean() << "\t\t" <<  std::setprecision(2) << p2_phi->GetRMS()/sqrt(p2_phi->GetEntries()) << endl;
    }

    if(p1_theta_p->GetEntries()!=0) {
      fout << "Theta_P\t " << runnum <<" \t 1 \t  " << md_1 << "\t" <<  std::setprecision(3) << p1_theta_p->GetMean() << "\t\t" <<  std::setprecision(2) << p1_theta_p->GetRMS()/sqrt(p1_theta_p->GetEntries()) << endl;
    }
    if(p2_theta_p->GetEntries()!=0) {
      fout << "Theta_P\t " << runnum <<" \t 2 \t  " << md_2 << "\t" <<  std::setprecision(3) << p2_theta_p->GetMean() << "\t\t" <<  std::setprecision(2) << p2_theta_p->GetRMS()/sqrt(p2_theta_p->GetEntries()) << endl;
    }
    if(p1_phi_p->GetEntries()!=0) {
      fout << "Phi_P\t " << runnum <<" \t 1 \t  " << md_1 << "\t" <<  std::setprecision(3) << p1_phi_p->GetMean() << "\t\t" <<  std::setprecision(2) << p1_phi_p->GetRMS()/sqrt(p1_phi_p->GetEntries()) << endl;
    }
    if(p2_phi_p->GetEntries()!=0) {
      fout << "Phi_P\t " << runnum <<" \t 2 \t  " << md_2 << "\t" <<  std::setprecision(3) << p2_phi_p->GetMean() << "\t\t" <<  std::setprecision(2) << p2_phi_p->GetRMS()/sqrt(p2_phi_p->GetEntries()) << endl;
    }

    /// these lines below for debugging
    //    fout << " These next are matching params from all tracks " << endl;
    //    fout << "mean theta off Pack 1 = " << mean_thetaoff_pkg1 << endl;
    //    fout << "mean theta off Pack 2 = " << mean_thetaoff_pkg2 << endl;
    //    fout << "mean phi off Pack 1 = " << mean_phioff_pkg1 << endl;
    //    fout << "mean phi off Pack 2 = " << mean_phioff_pkg2 << endl;

    //close the file
    fout.close();

  /*************************************
  This part is to output files for the ROOT Tree
  Therefore they will not be included in the website
  on the html file, but will be in the directory
  however I am still making it human readable :)
  *************************************/

  //Num of Tiggers
  fout.open(outputPrefix+"NumTriggers.txt");
  if (!fout.is_open()) cout << "File not opened - Num Tiggers" << endl;

  fout << "Run # \t Total Triggers" << endl;
  fout << runnum << " \t " << nevents << endl;

  //close the file
  fout.close();

  //Num of Raw Tracks
  fout.open(outputPrefix+"RawTracks.txt");
  if (!fout.is_open()) cout << "File not opened - raw tracks" << endl;

  fout << "Run # \t pkg \t RawTracks" << endl;
  fout << runnum << " \t 1 \t " << n_raw_track_1 << endl;
  fout << runnum << " \t 2 \t " <<  n_raw_track_2 << endl;

  //close the file
  fout.close();

  //num of good tracks
  fout.open(outputPrefix+"GoodTracks.txt");
  if (!fout.is_open()) cout << "File not opened - good tracks" << endl;

  fout << "Run # \t pkg \t GoodTracks" << endl;
  fout << runnum << " \t 1 \t " << n_good_track_1 << endl;
  fout << runnum << " \t 2 \t " << n_good_track_2 << endl;

  //close the file
  fout.close();

  //num of % good tracks
  fout.open(outputPrefix+"PercentGood.txt");
  if (!fout.is_open()) cout << "File not opened - % good" << endl;

  fout << "Run # \t pkg \t % Tracks" << endl;
  if (n_raw_track_1>0)
  {
    fout << runnum << " \t 1 \t " << std::setprecision(4)
	 << 100.*float(n_good_track_1)/n_raw_track_1 << endl;
  }
  if (n_raw_track_2>0)
  {
    fout << runnum << " \t 2 \t " <<  std::setprecision(4)
	 << 100.*float(n_good_track_2)/n_raw_track_2 << endl;
  }

  //close the file
  fout.close();

  //Angle
  fout.open(outputPrefix+"Angle.txt");
  if (!fout.is_open()) cout << "File not opened - angle" << endl;

  fout << "Run # \t pkg \t Angle \t Error" << endl;
  if(angle->GetEntries()!=0)
  {
    fout << runnum <<" \t 0 \t" << value_with_error(angle->GetMean(), (angle->GetRMS()/sqrt(angle->GetEntries()))) << endl;
  }
  if(n_good_track_1!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(angle_1->GetMean(),(angle_1->GetRMS()/sqrt(angle_1->GetEntries()))) << endl;
  }
  if(n_good_track_2!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(angle_2->GetMean(), (angle_2->GetRMS()/sqrt(angle_2->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Q2 Version 1 - with loss
  fout.open(outputPrefix+"Q2_WithLoss.txt");
  if (!fout.is_open()) cout << "File not opened - Q2 w/ loss" << endl;

  fout << "Run # \t pkg \t Q2 \t Error (w/ loss)" << endl;
  if(q2->GetEntries()!=0)
  {
    fout << runnum <<" \t 0 \t" << value_with_error(q2->GetMean(), (q2->GetRMS()/sqrt(q2->GetEntries()))) << endl;
  }
  if(q2_1->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(q2_1->GetMean(),(q2_1->GetRMS()/sqrt(q2_1->GetEntries()))) << endl;
  }
  if(q2_2->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(q2_2->GetMean(), (q2_2->GetRMS()/sqrt(q2_2->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Q2 Version 1 - without loss
  fout.open(outputPrefix+"Q2_WithOutLoss.txt");
  if (!fout.is_open()) cout << "File not opened - Q2 w/o loss" << endl;

  fout << "Run # \t pkg \t Q2 \t Error (w/o loss)" << endl;
  if(q2_0->GetEntries()!=0)
  {
    fout << runnum <<" \t 0 \t" << value_with_error(q2_0->GetMean(), (q2_0->GetRMS()/sqrt(q2_0->GetEntries()))) << endl;
  }
  if(q2_0_1->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(q2_0_1->GetMean(),(q2_0_1->GetRMS()/sqrt(q2_0_1->GetEntries()))) << endl;
  }
  if(q2_0_2->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(q2_0_2->GetMean(), (q2_0_2->GetRMS()/sqrt(q2_0_2->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Scattering Energy
  fout.open(outputPrefix+"ScatteringEnergy.txt");
  if (!fout.is_open()) cout << "File not opened - Scat Energy" << endl;

  fout << "Run # \t pkg \t Energy \t Error " << endl;
  if(eprime_1->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(eprime_1->GetMean(),(eprime_1->GetRMS()/sqrt(eprime_1->GetEntries()))) << endl;
  }
  if(eprime_2->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(eprime_2->GetMean(), (eprime_2->GetRMS()/sqrt(eprime_2->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Theta Match
  fout.open(outputPrefix+"ThetaMatch.txt");
  if (!fout.is_open()) cout << "File not opened - Theta" << endl;

  fout << "Run # \t pkg \t deltaTheta \t Error " << endl;
  if(p1_theta->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(p1_theta->GetMean(),(p1_theta->GetRMS()/sqrt(p1_theta->GetEntries()))) << endl;
  }
  if(p2_theta->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(p2_theta->GetMean(), (p2_theta->GetRMS()/sqrt(p2_theta->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Phi Match
  fout.open(outputPrefix+"PhiMatch.txt");
  if (!fout.is_open()) cout << "File not opened - Phi" << endl;

  fout << "Run # \t pkg \t deltaPhi \t Error " << endl;
  if(p1_phi->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(p1_phi->GetMean(),(p1_phi->GetRMS()/sqrt(p1_phi->GetEntries()))) << endl;
  }
  if(p2_phi->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(p2_phi->GetMean(), (p2_phi->GetRMS()/sqrt(p2_phi->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Theta Perp Match
  fout.open(outputPrefix+"ThetaProjMatch.txt");
  if (!fout.is_open()) cout << "File not opened - Theta Perp" << endl;

  fout << "Run # \t pkg \t deltaTheta Proj \t Error " << endl;
  if(p1_theta_p->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(p1_theta_p->GetMean(),(p1_theta_p->GetRMS()/sqrt(p1_theta_p->GetEntries()))) << endl;
  }
  if(p2_theta_p->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(p2_theta_p->GetMean(), (p2_theta_p->GetRMS()/sqrt(p2_theta_p->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

  //Phi Perp Match
  fout.open(outputPrefix+"PhiProjMatch.txt");
  if (!fout.is_open()) cout << "File not opened - Phi Perp" << endl;

  fout << "Run # \t pkg \t deltaPhi Proj \t Error " << endl;
  if(p1_phi_p->GetEntries()!=0)
  {
    fout << runnum <<" \t 1 \t" << value_with_error(p1_phi_p->GetMean(),(p1_phi_p->GetRMS()/sqrt(p1_phi_p->GetEntries()))) << endl;
  }
  if(p2_phi_p->GetEntries()!=0)
  {
    fout << runnum <<" \t 2 \t" << value_with_error(p2_phi_p->GetMean(), (p2_phi_p->GetRMS()/sqrt(p2_phi_p->GetEntries()))) << endl;
  }
  //close the file
  fout.close();

/*****************************************
That is all folks in terms of the ROOT tree :D
*****************************************/


    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(2,3);
    c->cd(1);
    gStyle->SetStatW(0.3);
// Set width of stat-box (fraction of pad size)
    gStyle->SetStatH(0.3);
// Set height of stat-box (fraction of pad size)

    gPad->SetLogy();
    angle->Draw();
    angle->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle->SetTitle("Scattering Angle in Package 1+2");

    c->cd(2);
    gPad->SetLogy();
    q2->Draw();
    q2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2->SetTitle("Q2 (version 1) in Package 1+2");
    
    c->cd(3);
    gPad->SetLogy();
    angle_1->Draw();
    angle_1->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle_1->SetTitle(Form("Scattering Angle  Package 1  octant %d",md_1));

    c->cd(4);
    gPad->SetLogy();
    q2_1->Draw();
    q2_1->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_1->SetTitle(Form("Q2 (version 1) in Package 1  octant %d",md_1));
    
    c->cd(5);
    gPad->SetLogy();
    angle_2->Draw();
    angle_2->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle_2->SetTitle(Form("Scattering Angle  Package 2  octant %d",md_2));

    c->cd(6);
    gPad->SetLogy();
    q2_2->Draw();
    q2_2->GetXaxis()->SetTitle("Q2: (GeV/c)^2");
    q2_2->SetTitle(Form("Q2 (version 1) in Package 2  octant %d",md_2));

    c->SaveAs(outputPrefix+"q2.png");
    c->SaveAs(outputPrefix+"q2.C");
}

