// Name:   Q2_check.C
// Author: Siyuan Yang
// Email:  sxyang@email.wm.edu
// Date:   Monday Jan 9th,2010
// Version:1.0
//
//
// This script is used to extract the Q2 information for both packages
// from the rootfiles. Now it has two filters turned on, one is the MD
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

// Another Example
// root[2] q2_check(-1,10000,13733,3,"_v2")
// this will carry out the same task as previous example for the first 10000 events. 
// However, the name of the rootfile
// has a suffix "_v2", so the script is looking for a rootfile named as Qweak_13653_v2.root.
// Furthuremore, the octant which package2 of R2 covers is 3, which leads to the fact that package1
// covers oct 7. The default value for the octant specification number is 1, which corresponds
// to the nominal situation(pkg2 is at oct1 and pkg1 is at oct5).

// notion: the package number is always consitent with package number in R2



#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

const int multiple=18;

void q2_check(int event_start=-1,int event_end=-1,int run=8658,int oct=1,string suffix=""){


  //string folder="/scratch/sxyang";
   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/Qweak_";
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;
   TFile* file=new TFile(file_name.c_str());

   TH1F* angle=new TH1F("Scattering Angle","ScatteringAngle",100,0,15);
   TH1F* q2=new TH1F("Q2","Q2",100,0,0.12);
   TH1F* angle_1=new TH1F("Scattering Angle in Package 1","ScatteringAngle in Package 1",100,0,15);
   TH1F* angle_2=new TH1F("Scattering Angle in Package 2","ScatteringAngle in Package 2",100,0,15);
   TH1F* q2_1=new TH1F("Q2 distribution in Package 1","Q2 distribution in Package 1",100,0,0.12);
   TH1F* q2_2=new TH1F("Q2 distribution in Package 2","Q2 distribution in Package 2",100,0,0.12);

   QwEvent* fEvent=0;
   QwTrack* track=0;
   TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
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
    //TLeaf* mdp_1=maindet_branch->GetLeaf(Form("md%dp_adc",md_1));
    //TLeaf* mdm_1=maindet_branch->GetLeaf(Form("md%dm_adc",md_1));
    //TLeaf* mdp_2=maindet_branch->GetLeaf(Form("md%dp_adc",md_2));
    //TLeaf* mdm_2=maindet_branch->GetLeaf(Form("md%dm_adc",md_2));
    
    TLeaf* tsp_1=maindet_branch->GetLeaf(Form("md%dp_f1",md_1));
    TLeaf* tsm_1=maindet_branch->GetLeaf(Form("md%dm_f1",md_1));
    TLeaf* tsp_2=maindet_branch->GetLeaf(Form("md%dp_f1",md_2));
    TLeaf* tsm_2=maindet_branch->GetLeaf(Form("md%dm_f1",md_2));
    
    for(int i=start;i<end;++i){

      if(i%100000==0)
	cout << "events processed so far: " << i << endl;
      
      event_branch->GetEntry(i);
      maindet_branch->GetEntry(i);
       
      //double mdp_value_1=mdp_1->GetValue();
      //double mdm_value_1=mdm_1->GetValue();
      //double mdp_value_2=mdp_2->GetValue();
      //double mdm_value_2=mdm_2->GetValue();

      trig_branch->GetEntry(i);
       
      double tsp_value_1=tsp_1->GetValue();
      double tsm_value_1=tsm_1->GetValue();
      double tsp_value_2=tsp_2->GetValue();
      double tsm_value_2=tsm_2->GetValue();
      
      double ntracks=fEvent->GetNumberOfTracks();
      // those filters are related with the quality of matching of r2 and r3 tracks
      double mean_phioff_pkg1=-0.006526;
      double mean_phioff_pkg2=-0.01667;
      double mean_thetaoff_pkg1=-0.0007041;
      double mean_thetaoff_pkg2=-0.01098;

      double rms_phioff_pkg1=0.0281;
      double rms_phioff_pkg2=0.02121;
      double rms_thetaoff_pkg1=0.002918;
      double rms_thetaoff_pkg2=0.003083;


      double width=25;
      double pkg1_phioff_lower=mean_phioff_pkg1-width*rms_phioff_pkg1;
      double pkg1_phioff_upper=mean_phioff_pkg1+width*rms_phioff_pkg1;
      double pkg2_phioff_lower=mean_phioff_pkg2-width*rms_phioff_pkg2;
      double pkg2_phioff_upper=mean_phioff_pkg2+width*rms_phioff_pkg2;

      double pkg1_thetaoff_lower=mean_thetaoff_pkg1-width*rms_thetaoff_pkg1;
      double pkg1_thetaoff_upper=mean_thetaoff_pkg1+width*rms_thetaoff_pkg1;
      double pkg2_thetaoff_lower=mean_thetaoff_pkg2-width*rms_thetaoff_pkg2;
      double pkg2_thetaoff_upper=mean_thetaoff_pkg2+width*rms_thetaoff_pkg2;
      // extract Q2 and Scattering Angle

      	int nhits=fEvent->GetNumberOfHits();
	int valid_hits_1=0,valid_hits_2=0;
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  if(hit->GetRegion() ==2 && hit->GetDriftDistance() >=0 && hit->GetHitNumber()==0){
	    if(hit->GetPackage()==1)
	      ++valid_hits_1;
	    else
	      ++valid_hits_2;

	  }
	}

      for(int j=0;j<ntracks;++j){
	track=fEvent->GetTrack(j);
	if(track->GetPackage()==1 && valid_hits_1 < multiple && tsm_value_1 >-1800 && tsm_value_1 < -1200 && tsp_value_1 > -1800 && tsp_value_1 < -1200){
	   if(track->fDirectionPhioff>pkg1_phioff_lower && track->fDirectionPhioff<pkg1_phioff_upper && track->fDirectionThetaoff>pkg1_thetaoff_lower && track->fDirectionThetaoff<pkg1_thetaoff_upper){
	  angle_1->Fill(track->fScatteringAngle);
	  q2_1->Fill(track->fQ2);
	  angle->Fill(track->fScatteringAngle);
	  q2->Fill(track->fQ2);
	   }
	}
      	else if(track->GetPackage()==2 && valid_hits_2 < multiple && tsm_value_2 > -1800 && tsm_value_2 <-1200 && tsp_value_2 > -1800 && tsp_value_2 < -1200 ){
	  if(track->fDirectionPhioff>pkg2_phioff_lower && track->fDirectionPhioff<pkg2_phioff_upper && track->fDirectionThetaoff>pkg2_thetaoff_lower && track->fDirectionThetaoff<pkg2_thetaoff_upper){
	angle_2->Fill(track->fScatteringAngle);
	  q2_2->Fill(track->fQ2);
	  angle->Fill(track->fScatteringAngle);
	  q2->Fill(track->fQ2);
	   }
	}
      }
    }  // end of for loop over events
   

    cout << "tracks per trigger" << endl;
    cout << "pkg1: " << angle_1->GetEntries() << endl;
    cout << "pkg2: " << angle_2->GetEntries() << endl;

    cout << "scattering angle: " << endl;
    cout << "all: " << setprecision(5) << angle->GetMean() << " error RMS/sqrt(N): " << setprecision(4) << angle->GetRMS()/sqrt(angle->GetEntries()) << endl; 
    if(angle_1->GetEntries()!=0)
    cout << "pkg1: " << setprecision(5) << angle_1->GetMean() << " error RMS/sqrt(N): " << setprecision(4) << angle_1->GetRMS()/sqrt(angle_1->GetEntries()) << endl;
    if(angle_2->GetEntries()!=0)
    cout << "pkg2: " <<  setprecision(5) << angle_2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << angle_2->GetRMS()/sqrt(angle_2->GetEntries()) << endl;

    cout << "q2: " << endl;
    cout << "all: " <<  setprecision(5) << 1000*q2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2->GetRMS()/sqrt(q2->GetEntries()) << endl;
    if(q2_1->GetEntries()!=0)
    cout << "pkg1: " <<  setprecision(5) << 1000*q2_1->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_1->GetRMS()/sqrt(q2_1->GetEntries()) << endl;
    if(q2_2->GetEntries()!=0)
    cout << "pkg2: " <<  setprecision(5) << 1000*q2_2->GetMean() << " error RMS/sqrt(N): " <<  setprecision(4) << 1000*q2_2->GetRMS()/sqrt(q2_2->GetEntries()) << endl;
      
    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(2,3);
    c->cd(1);
    angle->Draw();
    angle->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle->SetTitle("Scattering Angle Distribution in Package 1");

    c->cd(2);
    q2->Draw();
    q2->GetXaxis()->SetTitle("Scattering Angle: degree");
    q2->SetTitle("Q2 Distribution in Package 1");
    
    c->cd(3);
    angle_1->Draw();
    angle_1->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle_1->SetTitle("Scattering Angle Distribution in Package 1");

    c->cd(4);
    q2_1->Draw();
    q2_1->GetXaxis()->SetTitle("Scattering Angle: degree");
    q2_1->SetTitle("Q2 Distribution in Package 1");
    
     c->cd(5);
    angle_2->Draw();
    angle_2->GetXaxis()->SetTitle("Scattering Angle: degree");
    angle_2->SetTitle("Scattering Angle Distribution in Package 2");

    c->cd(6);
    q2_2->Draw();
    q2_2->GetXaxis()->SetTitle("Scattering Angle: degree");
    q2_2->SetTitle("Q2 Distribution in Package 2");
    
    return;

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
