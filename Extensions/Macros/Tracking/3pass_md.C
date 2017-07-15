//  3pass_md.C
//
// authors: David Armstrong
//
//    Special purposes for 3-pass data 
//
//   to see the pion events clearly, I use an "OR" of the two MD PMTs, rather
//   than the tighter "AND".
//
//  Usage: first you must put the rootfile which contains 
//   the data under the $QW_ROOTFILES directory and then type:
//   project_root(package, octant, runnumber,events)
//   where "package" = package number (1 or 2)
//         "octant" = main detector octant number (1 to 8)
//         "runnumber" = run number
//         "events" is number of events to analyze (default is whole run)
//   
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

  //  gROOT->SetBatch(1);  // don't write canvasses to screen


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

  TH1D* md1padc = new TH1D("md1padc", "md1+ adc data", 200, 0, 1000);
  TH1D* md1madc = new TH1D("md1madc", "md1- adc data", 200, 0, 1000);
  TH1D* md5padc = new TH1D("md5padc", "md5+ adc data", 200, 0, 1000);
  TH1D* md5madc = new TH1D("md5madc", "md5- adc data", 200, 0, 1000);

  TH1D* md1pskip = new TH1D("md1pskip", "md1+ adc data skip", 200, 0, 1000);
  TH1D* md1mskip = new TH1D("md1mskip", "md1- adc data skip", 200, 0, 1000);
  TH1D* md5pskip = new TH1D("md5pskip", "md5+ adc data skip", 200, 0, 1000);
  TH1D* md5mskip = new TH1D("md5mskip", "md5- adc data skip", 200, 0, 1000);

  TH1D* tdcph = new TH1D("TDCP", "TDCP data", 1650, 0, 0);
  TH1D* tdcmh = new TH1D("TDCM", "TDCM data", 1650, 0, 0);

  TH1D* md1adc_sum = new TH1D("md1adc_sum", "md 1 summed ADC spectrum", 500, 0, 5000);
  TH1D* md5adc_sum = new TH1D("md5adc_sum", "md 5 summed ADC spectrum", 500, 0, 5000);

  TH1F* q2_1_e=new TH1F("q2_1_e","Q2 distribution in Package 1, e",100,0,0.3);
  TH1F* q2_2_e=new TH1F("q2_2_e","Q2 distribution in Package 2, e",100,0,0.3);
  TH1F* q2_1_pi=new TH1F("q2_1_pi","Q2 distribution in Package 1, pi",100,0,0.3);
  TH1F* q2_2_pi=new TH1F("q2_2_pi","Q2 distribution in Package 2, pi",100,0,0.3);

  TH1F* q2_1_e_lw=new TH1F("q2_1_e_lw","light-weighted Q2 in Package 1, e",100,0,0.3);
  TH1F* q2_2_e_lw=new TH1F("q2_2_e_lw","light-weighted Q2 distribution in Package 2, e",100,0,0.3);
  TH1F* q2_1_pi_lw=new TH1F("q2_1_pi_lw","light-weighted Q2 distribution in Package 1, pi",100,0,0.3);
  TH1F* q2_2_pi_lw=new TH1F("q2_2_pi_lw","light-weighted Q2 distribution in Package 2, pi",100,0,0.3);

  TH1F* W_1_e=new TH1F("W_1_e","W distribution in Package 1, e",100,1.0,3.0);
  TH1F* W_2_e=new TH1F("W_2_e","W distribution in Package 2, e",100,1.0,3.0);
  TH1F* W_1_pi=new TH1F("W_1_pi","W distribution in Package 1, pi",100,1.0,3.0);
  TH1F* W_2_pi=new TH1F("W_2_pi","W distribution in Package 2, pi",100,1.0,3.0);

  TH1F* Theta_1_e=new TH1F("Theta_1_e","Theta distribution in Package 1, e",100,1.0,15.0);
  TH1F* Theta_2_e=new TH1F("Theta_2_e","Theta distribution in Package 2, e",100,1.0,15.0);
  TH1F* Theta_1_pi=new TH1F("Theta_1_pi","Theta distribution in Package 1, pi",100,1.0,15.0);
  TH1F* Theta_2_pi=new TH1F("Theta_2_pi","Theta distribution in Package 2, pi",100,1.0,15.0);

  TH1F* W2_1=new TH1F("W2_1","W^2 distribution in Package 1",100,0,8.0);
  TH1F* W2_2=new TH1F("W2_2","W^2 distribution in Package 2",100,0,8.0);
  TH1F* W_1=new TH1F("W_1","W distribution in Package 1",100,1.0,3.0);
  TH1F* W_2=new TH1F("W_2","W distribution in Package 2",100,1.0,3.0);
  TH1F* nu_1=new TH1F("nu_1","nu distribution in Package 1",100,1.0,3.0);
  TH1F* nu_2=new TH1F("nu_2","nu distribution in Package 2",100,1.00,3.0);
  TH1F* x_1=new TH1F("x_1","Bjorken x distribution in Package 1",100,0,0.2);
  TH1F* x_2=new TH1F("x_2","Bjorken x distribution in Package 2",100,0,0.2);



  TBranch* branch_event=event_tree->GetBranch("events");
  TBranch* branch     = event_tree->GetBranch("maindet");
  branch_event->SetAddress(&fEvent);

  TLeaf* md1p=branch->GetLeaf(Form("md%dp_adc",1));
  TLeaf* md1m=branch->GetLeaf(Form("md%dm_adc",1));
  TLeaf* md1p_t=branch->GetLeaf(Form("md%dp_f1",1));
  TLeaf* md1m_t=branch->GetLeaf(Form("md%dm_f1",1));
  TLeaf* md5p=branch->GetLeaf(Form("md%dp_adc",5));
  TLeaf* md5m=branch->GetLeaf(Form("md%dm_adc",5));
  TLeaf* md5p_t=branch->GetLeaf(Form("md%dp_f1",5));
  TLeaf* md5m_t=branch->GetLeaf(Form("md%dm_f1",5));
  QwTrack* track=0;

  Int_t nevents=event_tree->GetEntries();
  cout << "Number of Triggers  = " << nevents << endl; 
  if (max_events == -1)max_events = nevents; 


  for (int i=start_events;i<max_events;i++)
    {
      if(i%10000==0) cout << "events process so far: " << i << std::endl;
      branch_event->GetEntry(i);
      branch->GetEntry(i);
 
      if (fEvent->GetNumberOfTracks()>0)
	{
	  track=fEvent->GetTrack(0);
	  double Q2_val=fEvent->fKinWithLoss.fQ2;
	  double Theta_val=fEvent->GetScatteringAngle();
	  double W2_val=fEvent->fKinWithLoss.fW2;
	  double nu_val=fEvent->fKinWithLoss.fNu;
	  double x_val=fEvent->fKinWithLoss.fX;
	  double W_val=sqrt(W2_val);

	  int pack = track->fBack->GetPackage();
	  //	  cout << "Event " << i << "  track package =  " << pack << endl;
	  
	  if(pack==2){
	  Double_t p1dataplus = md1p_t->GetValue();
 	  Double_t p1dataminus = md1m_t->GetValue();

 	  if ((p1dataplus !=0) || (p1dataminus !=0)) {  
	    //	    cout << "Event " << i << "  track package =  " << pack << endl;
	    //	    cout << " Event " << i << "  md1 plus time = " << p1dataplus << endl;
	    //	    cout << " Event " << i << "  md1 minus time = " << p1dataminus << endl;
	      Double_t p1adcp = md1p->GetValue();
	      md1padc->Fill(p1adcp); 
	      //	      cout << " Event " << i << "  md1 plus adc = " << p1adcp << endl;

	      Double_t p1adcm = md1m->GetValue();
	      md1madc->Fill(p1adcm); 
	      //	      cout << " Event " << i << "  md1 minus adc = " << p1adcm << endl;
	    }
	  }
	  if(pack==1){
	  Double_t p5dataplus = md5p_t->GetValue();
	  Double_t p5dataminus = md5m_t->GetValue();

	  if ((p5dataplus !=0) || (p5dataminus !=0)) {  
	    //	    cout << "Event " << i << "  track package =  " << pack << endl;
	    //	    cout << " Event " << i << "  md5 plus time = " << p5dataplus << endl;
	    //	    cout << " Event " << i << "  md5 minus time = " << p5dataminus << endl;
	    Double_t p5adcp = md5p->GetValue();
	    md5padc->Fill(p5adcp); 
	    //	    cout << " Event " << i << "  md5 plus adc = " << p5adcp << endl;
	    Double_t p5adcm = md5m->GetValue();
	    md5madc->Fill(p5adcm); 
	    //	    cout << " Event " << i << "  md5 minus adc = " << p5adcm << endl;
	  }
	  }

	  branch->GetEntry(i+6);

	  if(pack==2){
	    if ((p1dataplus !=0) || (p1dataminus !=0)) {  
	      Double_t p1adcp_skip = md1p->GetValue();
	      //	      cout << "  Event " << i << "  forward skip md1 plus adc = " << p1adcp_skip << endl;
	      
	      Double_t p1adcm_skip = md1m->GetValue();
	      //	  md1madc->Fill(p1adcm); 
	      //	      cout << "  Event " << i << "  forward skip md1 minus adc = " << p1adcm_skip << endl;
	      md1adc_sum->Fill(p1adcp_skip+p1adcm_skip); 
	      if ((p1adcp_skip+p1adcm_skip)>800) {
		q2_2_e->Fill(Q2_val);
		Double_t light = abs(p1adcp_skip+p1adcm_skip - 462); //ped-subtracted
		q2_2_e_lw->Fill(Q2_val,light);
		W_2_e->Fill(W_val);
		Theta_2_e->Fill(Theta_val);
	      }
	      else{
		q2_2_pi->Fill(Q2_val);
		Double_t light = abs(p1adcp_skip+p1adcm_skip - 462); //ped-subtracted
		q2_2_pi_lw->Fill(Q2_val,light);
		W_2_pi->Fill(W_val);
		Theta_2_pi->Fill(Theta_val);
		}

	    }
	  }

	  if(pack==1){
	    if ((p5dataplus !=0) || (p5dataminus !=0)) {  
	      Double_t p5adcp_skip = md5p->GetValue();
	      //	  md5padc->Fill(p1adcp); 
	      //	      cout << "  Event " << i << "  forward skip md5 plus adc = " << p5adcp_skip << endl;
	      
	      Double_t p5adcm_skip = md1m->GetValue();
	      //	  md5madc->Fill(p1adcm); 
	      //	      cout << "  Event " << i << "  forward skip md5 minus adc = " << p5adcm_skip << endl;
	      md5adc_sum->Fill(p5adcp_skip+p5adcm_skip); 
	      if ((p5adcp_skip+p5adcm_skip)>800) {
		q2_1_e->Fill(Q2_val);
		Double_t light = abs(p5adcp_skip+p5adcm_skip - 421); //ped-subtracted
		q2_1_e_lw->Fill(Q2_val,light);
		W_1_e->Fill(W_val);
		Theta_1_e->Fill(Theta_val);
	      }
	      else{
		q2_1_pi->Fill(Q2_val);
		Double_t light = abs(p5adcp_skip+p5adcm_skip - 421); //ped-subtracted
		q2_1_pi_lw->Fill(Q2_val,light);
		W_1_pi->Fill(W_val);
		Theta_1_pi->Fill(Theta_val);
	      }
	    }
	  }
	}
    }
  

  TFile f(Form("3pass_%d.root",run_number),"new");
  md1adc_sum->Write();
  md5adc_sum->Write();
  q2_1_e->Write();
  q2_2_e->Write();
  q2_1_pi->Write();
  q2_2_pi->Write();
  q2_1_e_lw->Write();
  q2_2_e_lw->Write();
  q2_1_pi_lw->Write();
  q2_2_pi_lw->Write();
  W_1_e->Write();
  W_2_e->Write();
  W_1_pi->Write();
  W_2_pi->Write();
  Theta_1_e->Write();
  Theta_2_e->Write();
  Theta_1_pi->Write();
  Theta_2_pi->Write();

  c2 = new TCanvas("c2","MD ADCs",10, 10, 600,600);
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

  c4 = new TCanvas("c4","Q^2",10, 10, 700,700);
  c4->Divide(2,2);
  gStyle->SetStatH(0.3);
  c4->cd(1);
  q2_1_e->Draw();
  c4->cd(2);
  q2_2_e->Draw();
  c4->cd(3);
  q2_1_pi->Draw();
  c4->cd(4);
  q2_2_pi->Draw();
  c4->SaveAs(outputPrefix+"Q2_pi_e.pdf");

  c5 = new TCanvas("c5","W",10, 10, 700,700);
  c5->Divide(2,2);
  gStyle->SetStatH(0.3);
  c5->cd(1);
  W_1_e->Draw();
  c5->cd(2);
  W_2_e->Draw();
  c5->cd(3);
  W_1_pi->Draw();
  c5->cd(4);
  W_2_pi->Draw();
  c5->SaveAs(outputPrefix+"W_pi_e.pdf");

  c6 = new TCanvas("c6","Theta",10, 10, 700,700);
  c6->Divide(2,2);
  gStyle->SetStatH(0.3);
  c6->cd(1);
  Theta_1_e->Draw();
  c6->cd(2);
  Theta_2_e->Draw();
  c6->cd(3);
  Theta_1_pi->Draw();
  c6->cd(4);
  Theta_2_pi->Draw();
  c6->SaveAs(outputPrefix+"Theta_pi_e.pdf");

  c7 = new TCanvas("c7","Q^2 light-weighted",10, 10, 700,700);
  c7->Divide(2,2);
  gStyle->SetStatH(0.3);
  c7->cd(1);
  q2_1_e_lw->Draw();
  c7->cd(2);
  q2_2_e_lw->Draw();
  c7->cd(3);
  q2_1_pi_lw->Draw();
  c7->cd(4);
  q2_2_pi_lw->Draw();
  c7->SaveAs(outputPrefix+"Q2_pi_e_lw.pdf");
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


