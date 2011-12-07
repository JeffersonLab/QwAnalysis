/********************************************************************/
/*                         Nuruzzaman                               */
/*                         08/04/2010                               */
/*                                                                  */
/*                   Last Edited:08/16/2010                         */
/*                                                                  */
/* This Macro create plots for Beam Modulation system. It includes  */
/* Figure fg_event.png :function generator plot w.r.t Event Number  */
/* Figure lem_event.png :current transducer plot w.r.t Event Number */
/* Figure fg_ramp.png :function generator plot w.r.t RAMPWAVE       */
/* Figure lem_ramp.png :LEM current transducer plot w.r.t RAMPWAVE  */
/* Figure fg_vs_lem.png :FG vs LEM.                                 */
/********************************************************************/

{

  gROOT->Reset();
#include <TROOT.h>
#include <TFile.h>
#include <TH2.h>
#include <TBox.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TString.h>

/* Ask to input the run number to user */
   UInt_t run_number = 0;
   cout << "Enter run number" << endl;
   cin >> run_number;

/* /\* Create an soft link created rootfiles directory *\/ */
/*    TString cmd= Form("ln -sf /work/hallc/qweak/cdaq/summer2010/QwPass1_%d.000.root /work/hallc/qweak/nur/data_analysis/rootfiles/QwPass1_%d.000.root", run_number, run_number); */

/*    gSystem->Exec(cmd.Data());  */

/*    cout << "A soft link created of QwPass1_"  */
/* 	<< run_number  */
/* 	<<".000.root in rootfiles directory" << endl; */

/* Create an input TFile and load a root file from rootfiles directory */
   TFile f(Form("/net/cdaq/cdaql5data/qweak/rootfiles/Qweak_%d.000.root", run_number));

   cout << "Qweak_"
	<< run_number 
	<<".000.root ROOT file loaded sucessfully" << endl;
   
   //   cout << "Please analize the data file to create root file" << endl;

/* Create a directory by run number under plots directory */
   gSystem->mkdir(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d", run_number));
   cout << "Directory "
	<< run_number 
	<< " has been created sucessfully" << endl;
 
  cout << "*********************************************************************" << endl;
  cout << "*                 Starting Beam Modulation Plots                    *" << endl;
  cout << "*********************************************************************" << endl;

 /* Histogram parameters */
   gStyle->SetTitleYOffset(1.0);
   gStyle->SetTitleXOffset(0.9);


  /* Defining cuts for different FG signals */
TCut ramp_event_cut = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0";
TCut fg_event_cut1 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>28.5e3 && event_number<33e3";
TCut fg_event_cut2 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>46e3 && event_number<52e3";
TCut fg_event_cut3 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>66e3 && event_number<72e3";
TCut fg_event_cut4 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>86e3 && event_number<94e3";
TCut fg_event_cut5 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>108e3 && event_number<116e3";


/********************************************************************/
/*        Modulation Cycles and Function Generator Signals          */
/********************************************************************/

   TCanvas *c1 = new TCanvas("c1","Modulation Cycles and Function Generator Signals",0,0,1200,1000);

   //  c1->SetFillColor(kBlue-7);
   c1->Divide(1,6);
   c1->SetFillColor(0);

   c1->cd(1);
   c1->cd(1)->SetGridx();
   c1->cd(1)->SetGridy();

   TH2D *ramp_eve = new TH2D("ramp_eve","Modulation Cycles",500,0.2e5,1.25e5,600,-0.3,0.3);
   Mps_Tree->Draw("ramp*76.29e-6:event_number>>ramp_eve",ramp_event_cut);
   ramp_eve->SetMarkerColor(1);
   ramp_eve->Draw();
   ramp_eve->GetXaxis()->SetTitle("Event Number");
   ramp_eve->GetYaxis()->SetTitle("FG Amplitudes (V)");
   //   ramp_eve->GetYaxis()->SetRangeUser(120e5,240e5);

   c1->cd(2);
   c1->cd(2)->SetGridx();
   c1->cd(2)->SetGridy();

   TH2D *fgx1_eve = new TH2D("fgx1_eve","FGX1",500,0.2e5,1.25e5,600,-0.3,0.3);
   Mps_Tree->Draw("fgx1*76.29e-6:event_number>>fgx1_eve",fg_event_cut1);
   fgx1_eve->SetMarkerColor(kRed);
   fgx1_eve->Draw();
   fgx1_eve->GetXaxis()->SetTitle("Event Number");
   fgx1_eve->GetYaxis()->SetTitle("FGX1 (V)");
   //   fgx1_eve->GetYaxis()->SetRangeUser(120e5,240e5);

   c1->cd(3);
   c1->cd(3)->SetGridx();
   c1->cd(3)->SetGridy();

   TH2D *fgy1_eve = new TH2D("fgy1_eve","FGY1",500,0.2e5,1.25e5,600,-0.3,0.3);
   Mps_Tree->Draw("fgy1*76.29e-6:event_number>>fgy1_eve",fg_event_cut2);
   fgy1_eve->SetMarkerColor(kGreen);
   fgy1_eve->Draw();
   fgy1_eve->GetXaxis()->SetTitle("Event Number");
   fgy1_eve->GetYaxis()->SetTitle("FGY1 (V)");
   //   fgy1_eve->GetYaxis()->SetRangeUser(120e5,240e5);

   c1->cd(4);
   c1->cd(4)->SetGridx();
   c1->cd(4)->SetGridy();

   TH2D *fge_eve = new TH2D("fge_eve","FGE",500,0.2e5,1.25e5,600,-0.3,0.3);
   Mps_Tree->Draw("fge*76.29e-6:event_number>>fge_eve",fg_event_cut3);
   fge_eve->SetMarkerColor(kViolet-4);
   fge_eve->Draw();
   fge_eve->GetXaxis()->SetTitle("Event Number");
   fge_eve->GetYaxis()->SetTitle("FGE (V)");
   //   fge_eve->GetYaxis()->SetRangeUser(120e5,240e5);

   c1->cd(5);
   c1->cd(5)->SetGridx();
   c1->cd(5)->SetGridy();

   TH2D *fgx2_eve = new TH2D("fgx2_eve","FGX2",500,0.2e5,1.25e5,600,-0.3,0.3);
   Mps_Tree->Draw("fgx2*76.29e-6:event_number>>fgx2_eve",fg_event_cut4);
   fgx2_eve->SetMarkerColor(kRed);
   fgx2_eve->Draw();
   fgx2_eve->GetXaxis()->SetTitle("Event Number");
   fgx2_eve->GetYaxis()->SetTitle("FGX2 (V)");
   //   fgx2_eve->GetYaxis()->SetRangeUser(120e5,240e5);

   c1->cd(6);
   c1->cd(6)->SetGridx();
   c1->cd(6)->SetGridy();

   TH2D *fgy2_eve = new TH2D("fgy2_eve","FGY2",500,0.2e5,1.25e5,600,-0.3,0.3);
   Mps_Tree->Draw("fgy2*76.29e-6:event_number>>fgy2_eve",fg_event_cut5);
   fgy2_eve->SetMarkerColor(kGreen);
   fgy2_eve->Draw();
   fgy2_eve->GetXaxis()->SetTitle("Event Number");
   fgy2_eve->GetYaxis()->SetTitle("FGY2 (V)");
   //   fgy2_eve->GetYaxis()->SetRangeUser(120e5,240e5);




   c1->Update();
   c1->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_cycle_eve.png",run_number,run_number));

/* /\* Either exit program or continue  *\/ */
/*   TString in; */
/*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; */
/*   std::cin >> in; */
/*   if(in == "q") exit(-1); */

/* /\********************************************************************\/ */
/* /\*        Modulation Cycles and Current Transducer Signals          *\/ */
/* /\********************************************************************\/ */

/*    TCanvas *c2 = new TCanvas("c2","Modulation Cycles and Current Transducer Signals",10,8,1200,1000); */

/*    c2->SetFillColor(kBlue-7); */
/*    c2->Divide(1,6); */
/*    c2->SetFillColor(0); */

/*    c2->cd(1); */
/*    c2->cd(1)->SetGridx(); */
/*    c2->cd(1)->SetGridy(); */

/*    TH2D *ramp_eve2 = new TH2D("ramp_eve2","Modulation Cycles",500,0,1.8e5,600,-0.5,0.5); */
/*    Mps_Tree->Draw("ramp*76.29e-6:event_number>>ramp_eve2","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0"); */
/*    ramp_eve2->SetMarkerColor(1); */
/*    ramp_eve2->Draw(); */
/*    ramp_eve2->GetXaxis()->SetTitle("Event Number"); */
/*    ramp_eve2->GetYaxis()->SetTitle("LEM Amplitudes (V)"); */

/*    c2->cd(2); */
/*    c2->cd(2)->SetGridx(); */
/*    c2->cd(2)->SetGridy(); */

/*    TH2D *lemx1_eve = new TH2D("lemx1_eve","LEMX1",500,0,1.8e5,600,-0.5,0.5); */
/*    Mps_Tree->Draw("lemx1*76.29e-6:event_number>>lemx1_eve","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0"); */
/*    lemx1_eve->SetMarkerColor(kRed); */
/*    lemx1_eve->Draw(); */
/*    lemx1_eve->GetXaxis()->SetTitle("Event Number"); */
/*    lemx1_eve->GetYaxis()->SetTitle("LEMX1 (V)"); */

/*    c2->cd(3); */
/*    c2->cd(3)->SetGridx(); */
/*    c2->cd(3)->SetGridy(); */

/*    TH2D *lemx2_eve = new TH2D("lemx2_eve","LEMX2",500,0,1.8e5,600,-0.5,0.5); */
/*    Mps_Tree->Draw("lemx2*76.29e-6:event_number>>lemx2_eve","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0"); */
/*    lemx2_eve->SetMarkerColor(kRed); */
/*    lemx2_eve->Draw(); */
/*    lemx2_eve->GetXaxis()->SetTitle("Event Number"); */
/*    lemx2_eve->GetYaxis()->SetTitle("LEMX2 (V)"); */

/*    c2->cd(4); */
/*    c2->cd(4)->SetGridx(); */
/*    c2->cd(4)->SetGridy(); */

/*    TH2D *lemy1_eve = new TH2D("lemy1_eve","LEMY1",500,0,1.8e5,600,-0.5,0.5); */
/*    Mps_Tree->Draw("lemy1*76.29e-6:event_number>>lemy1_eve","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0"); */
/*    lemy1_eve->SetMarkerColor(kGreen); */
/*    lemy1_eve->Draw(); */
/*    lemy1_eve->GetXaxis()->SetTitle("Event Number"); */
/*    lemy1_eve->GetYaxis()->SetTitle("LEMY1 (V)"); */

/*    c2->cd(5); */
/*    c2->cd(5)->SetGridx(); */
/*    c2->cd(5)->SetGridy(); */

/*    TH2D *lemy2_eve = new TH2D("lemy2_eve","LEMY2",500,0,1.8e5,600,-0.5,0.5); */
/*    Mps_Tree->Draw("lemy2*76.29e-6:event_number>>lemy2_eve","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0"); */
/*    lemy2_eve->SetMarkerColor(kGreen); */
/*    lemy2_eve->Draw(); */
/*    lemy2_eve->GetXaxis()->SetTitle("Event Number"); */
/*    lemy2_eve->GetYaxis()->SetTitle("LEMY2 (V)"); */

/*    c2->cd(6); */
/*    c2->cd(6)->SetGridx(); */
/*    c2->cd(6)->SetGridy(); */

/*    TH2D *fge_eve2 = new TH2D("fge_eve2","FGE",500,0,1.8e5,600,-0.5,0.5); */
/*    Mps_Tree->Draw("fge*76.29e-6:event_number>>fge_eve2","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0"); */
/*    fge_eve->SetMarkerColor(kBlue+5); */
/*    fge_eve->Draw(); */
/*    fge_eve->GetXaxis()->SetTitle("Event Number"); */
/*    fge_eve->GetYaxis()->SetTitle("FGE (V)"); */

/*    c2->Update(); */
/*    c2->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_lem_eve.png",run_number,run_number)); */

/* /\* /\\* Either exit program or continue  *\\/ *\/ */
/* /\*   TString in; *\/ */
/* /\*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; *\/ */
/* /\*   std::cin >> in; *\/ */
/* /\*   if(in == "q") exit(-1); *\/ */

/********************************************************************/
/*              Function Generator vs Rampwave Signals              */
/********************************************************************/

   TCanvas *c3 = new TCanvas("c3","Function Generator vs Rampwave Signals",60,60,950,780);

   //   c3->SetFillColor(kBlue-7);
   c3->Divide(2,2);
   c3->SetFillColor(0);

   c3->cd(1);
   c3->cd(1)->SetGridx();
   c3->cd(1)->SetGridy();

   TH2D *fgx1_ramp = new TH2D("fgx1_ramp","FGX1 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("fgx1*76.29e-6:ramp*76.29e-6>>fgx1_ramp",fg_event_cut1);
   fgx1_ramp->SetMarkerColor(kRed);
   fgx1_ramp->Draw();
   fgx1_ramp->GetXaxis()->SetTitle("RAMP (V)");
   fgx1_ramp->GetYaxis()->SetTitle("FGX1 (V)");
   fgx1_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c3->cd(2);
   c3->cd(2)->SetGridx();
   c3->cd(2)->SetGridy();

   TH2D *fgx2_ramp = new TH2D("fgx2_ramp","FGX2 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("fgx2*76.29e-6:ramp*76.29e-6>>fgx2_ramp",fg_event_cut4);
   fgx2_ramp->SetMarkerColor(kRed);
   fgx2_ramp->Draw();
   fgx2_ramp->GetXaxis()->SetTitle("RAMP (V)");
   fgx2_ramp->GetYaxis()->SetTitle("FGX2 (V)");
   fgx2_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c3->cd(3);
   c3->cd(3)->SetGridx();
   c3->cd(3)->SetGridy();

   TH2D *fgy1_ramp = new TH2D("fgy1_ramp","FGY1 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("fgy1*76.29e-6:ramp*76.29e-6>>fgy1_ramp",fg_event_cut2);
   fgy1_ramp->SetMarkerColor(kGreen);
   fgy1_ramp->Draw();
   fgy1_ramp->GetXaxis()->SetTitle("RAMP (V)");
   fgy1_ramp->GetYaxis()->SetTitle("FGY1 (V)");
   fgy1_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c3->cd(4);
   c3->cd(4)->SetGridx();
   c3->cd(4)->SetGridy();

   TH2D *fgy2_ramp = new TH2D("fgy2_ramp","FGY2 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("fgy2*76.29e-6:ramp*76.29e-6>>fgy2_ramp",fg_event_cut5);
   fgy2_ramp->SetMarkerColor(kGreen);
   fgy2_ramp->Draw();
   fgy2_ramp->GetXaxis()->SetTitle("RAMP (V)");
   fgy2_ramp->GetYaxis()->SetTitle("FGY2 (V)");
   fgx2_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c3->Update();
   c3->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_fg_ramp.png",run_number,run_number));

/* /\* Either exit program or continue  *\/ */
/*   TString in; */
/*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; */
/*   std::cin >> in; */
/*   if(in == "q") exit(-1);  */

/********************************************************************/
/*              Current Transducer vs Rampwave Signals              */
/********************************************************************/

   TCanvas *c4 = new TCanvas("c4","Current Transducer vs Rampwave Signals",120,120,950,780);

   //   c4->SetFillColor(kBlue-7);
   c4->Divide(2,2);
   c4->SetFillColor(0);

   c4->cd(1);
   c4->cd(1)->SetGridx();
   c4->cd(1)->SetGridy();

   TH2D *lemx1_ramp = new TH2D("lemx1_ramp","LEMX1 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("lemx1*76.29e-6:ramp*76.29e-6>>lemx1_ramp",fg_event_cut1);
   lemx1_ramp->SetMarkerColor(kRed);
   lemx1_ramp->Draw();
   lemx1_ramp->GetXaxis()->SetTitle("RAMP (V)");
   lemx1_ramp->GetYaxis()->SetTitle("LEMX1 (V)");
   lemx1_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c4->cd(2);
   c4->cd(2)->SetGridx();
   c4->cd(2)->SetGridy();

   TH2D *lemx2_ramp = new TH2D("lemx2_ramp","LEMX2 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("lemx2*76.29e-6:ramp*76.29e-6>>lemx2_ramp",fg_event_cut4);
   lemx2_ramp->SetMarkerColor(kRed);
   lemx2_ramp->Draw();
   lemx2_ramp->GetXaxis()->SetTitle("RAMP (V)");
   lemx2_ramp->GetYaxis()->SetTitle("LEMX2 (V)");
   lemx2_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c4->cd(3);
   c4->cd(3)->SetGridx();
   c4->cd(3)->SetGridy();

   TH2D *lemy1_ramp = new TH2D("lemy1_ramp","LEMY1 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("lemy1*76.29e-6:ramp*76.29e-6>>lemy1_ramp",fg_event_cut2);
   lemy1_ramp->SetMarkerColor(kGreen);
   lemy1_ramp->Draw();
   lemy1_ramp->GetXaxis()->SetTitle("RAMP (V)");
   lemy1_ramp->GetYaxis()->SetTitle("FGY1 (V)");
   lemy1_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c4->cd(4);
   c4->cd(4)->SetGridx();
   c4->cd(4)->SetGridy();

   TH2D *lemy2_ramp = new TH2D("lemy2_ramp","LEMY2 vs RAMP",500,-0.03,0.3,600,-0.4,0.4);
   Mps_Tree->Draw("lemy2*76.29e-6:ramp*76.29e-6>>lemy2_ramp",fg_event_cut5);
   lemy2_ramp->SetMarkerColor(kGreen);
   lemy2_ramp->Draw();
   lemy2_ramp->GetXaxis()->SetTitle("RAMP (V)");
   lemy2_ramp->GetYaxis()->SetTitle("LEMY2 (V)");
   lemy2_ramp->GetYaxis()->SetRangeUser(-0.4,0.4);

   c4->Update();
   c4->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_lem_ramp.png",run_number,run_number));

/* /\* Either exit program or continue  *\/ */
/*   TString in; */
/*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; */
/*   std::cin >> in; */
/*   if(in == "q") exit(-1); */

/********************************************************************/
/*          Function Generator vs Current Transducer Signals        */
/********************************************************************/

   TCanvas *c5 = new TCanvas("c5","Function Generator vs Current Transducer Signals",180,180,950,780);

   //   c5->SetFillColor(kBlue-7);
   c5->Divide(2,2);
   c5->SetFillColor(0);

   c5->cd(1);
   c5->cd(1)->SetGridx();
   c5->cd(1)->SetGridy();

   TH2D *fgx1_lemx1 = new TH2D("fgx1_lemx1","FGX1 vs LEMX1",500,-0.1,0.1,600,-0.5,0.5);
   Mps_Tree->Draw("fgx1*76.29e-6:lemx1*76.29e-6>>fgx1_lemx1",fg_event_cut1);
   fgx1_lemx1->SetMarkerColor(kRed);
   fgx1_lemx1->Draw();
   fgx1_lemx1->GetXaxis()->SetTitle("LEMX1 (V)");
   fgx1_lemx1->GetYaxis()->SetTitle("FGX1 (V)");
   fgx1_lemx1->GetYaxis()->SetRangeUser(-0.4,0.4);

   c5->cd(2);
   c5->cd(2)->SetGridx();
   c5->cd(2)->SetGridy();

   TH2D *fgx2_lemx2 = new TH2D("fgx2_lemx2","FGX2 vs LEMX2",500,-0.1,0.1,600,-0.5,0.5);
   Mps_Tree->Draw("fgx2*76.29e-6:lemx2*76.29e-6>>fgx2_lemx2",fg_event_cut4);
   fgx2_lemx2->SetMarkerColor(kRed);
   fgx2_lemx2->Draw();
   fgx2_lemx2->GetXaxis()->SetTitle("LEMX2 (V)");
   fgx2_lemx2->GetYaxis()->SetTitle("FGX2 (V)");
   fgx2_lemx2->GetYaxis()->SetRangeUser(-0.4,0.4);

   c5->cd(3);
   c5->cd(3)->SetGridx();
   c5->cd(3)->SetGridy();

   TH2D *fgy1_lemy1 = new TH2D("fgy1_lemy1","FGY1 vs LEMY1",500,-0.1,0.1,600,-0.5,0.5);
   Mps_Tree->Draw("fgy1*76.29e-6:lemy1*76.29e-6>>fgy1_lemy1",fg_event_cut2);
   fgy1_lemy1->SetMarkerColor(kGreen);
   fgy1_lemy1->Draw();
   fgy1_lemy1->GetXaxis()->SetTitle("LEMY1 (V)");
   fgy1_lemy1->GetYaxis()->SetTitle("FGY1 (V)");
   fgy1_lemy1->GetYaxis()->SetRangeUser(-0.4,0.4);

   c5->cd(4);
   c5->cd(4)->SetGridx();
   c5->cd(4)->SetGridy();

   TH2D *fgy2_lemy2 = new TH2D("fgy2_lemy2","FGY2 vs LEMY2",500,-0.1,0.1,600,-0.5,0.5);
   Mps_Tree->Draw("fgy2*76.29e-6:lemy2*76.29e-6>>fgy2_lemy2",fg_event_cut5);
   fgy2_lemy2->SetMarkerColor(kGreen);
   fgy2_lemy2->Draw();
   fgy2_lemy2->GetXaxis()->SetTitle("LEMY2 (V)");
   fgy2_lemy2->GetYaxis()->SetTitle("FGY2 (V)");
   fgy2_lemy2->GetYaxis()->SetRangeUser(-0.4,0.4);

   c5->Update();
   c5->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_fg_lem.png",run_number,run_number));

/* /\* Either exit program or continue  *\/ */
/*   TString in; */
/*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; */
/*   std::cin >> in; */
/*   if(in == "q") exit(-1); */

/* /\********************************************************************\/ */
/* /\*           Function Generator vs Rampwave Signals Profile         *\/ */
/* /\********************************************************************\/ */

/*    TCanvas *c6 = new TCanvas("c6","Function Generator vs Rampwave Signals",10,8,950,780); */

/*    c6->SetFillColor(kBlue-7); */
/*    c6->Divide(2,2); */
/*    c6->SetFillColor(0); */

/*    c6->cd(1); */
/*    c6->cd(1)->SetGridx(); */
/*    c6->cd(1)->SetGridy(); */

/*    fgx1_ramp_prof = new TProfile("fgx1_ramp_prof","FGX1 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("fgx1*76.29e-6:ramp*76.29e-6>>fgx1_ramp_prof",fg_event_cut1,"prof"); */
/*    TProfile *fgx1_ramp_prof = (TProfile*)gDirectory->Get("fgx1_ramp_prof"); */

/*    fgx1_ramp_prof->SetMarkerColor(kRed); */
/*    fgx1_ramp_prof->SetLineColor(kRed); */
/*    fgx1_ramp_prof->Draw(); */
/*    fgx1_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    fgx1_ramp_prof->GetYaxis()->SetTitle("FGX1 (V)"); */
/*    fgx1_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c6->cd(2); */
/*    c6->cd(2)->SetGridx(); */
/*    c6->cd(2)->SetGridy(); */

/*    fgx2_ramp_prof = new TProfile("fgx2_ramp_prof","FGX2 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("fgx2*76.29e-6:ramp*76.29e-6>>fgx2_ramp_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>167.5e3 && event_number<171.5e3","prof"); */
/*    TProfile *fgx2_ramp_prof = (TProfile*)gDirectory->Get("fgx2_ramp_prof"); */

/*    fgx2_ramp_prof->SetMarkerColor(kRed); */
/*    fgx2_ramp_prof->SetLineColor(kRed); */
/*    fgx2_ramp_prof->Draw(); */
/*    fgx2_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    fgx2_ramp_prof->GetYaxis()->SetTitle("FGX2 (V)"); */
/*    fgx2_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c6->cd(3); */
/*    c6->cd(3)->SetGridx(); */
/*    c6->cd(3)->SetGridy(); */

/*    fgy1_ramp_prof = new TProfile("fgy1_ramp_prof","FGY1 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("fgy1*76.29e-6:ramp*76.29e-6>>fgy1_ramp_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>195e3 && event_number<200e3","prof"); */
/*    TProfile *fgy1_ramp_prof = (TProfile*)gDirectory->Get("fgy1_ramp_prof"); */

/*    fgy1_ramp_prof->SetMarkerColor(kGreen); */
/*    fgy1_ramp_prof->SetLineColor(kGreen); */
/*    fgy1_ramp_prof->Draw(); */
/*    fgy1_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    fgy1_ramp_prof->GetYaxis()->SetTitle("FGY1 (V)"); */
/*    fgy1_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c6->cd(4); */
/*    c6->cd(4)->SetGridx(); */
/*    c6->cd(4)->SetGridy(); */

/*    fgy2_ramp_prof = new TProfile("fgy2_ramp_prof","FGY2 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("fgy2*76.29e-6:ramp*76.29e-6>>fgy2_ramp_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>223e3 && event_number<227e3","prof"); */
/*    TProfile *fgy2_ramp_prof = (TProfile*)gDirectory->Get("fgy2_ramp_prof"); */

/*    fgy2_ramp_prof->SetMarkerColor(kGreen); */
/*    fgy2_ramp_prof->SetLineColor(kGreen); */
/*    fgy2_ramp_prof->Draw(); */
/*    fgy2_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    fgy2_ramp_prof->GetYaxis()->SetTitle("FGY2 (V)"); */
/*    fgy2_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c6->Update(); */
/*    c6->SaveAs(Form("plots/%d/%d_fg_ramp_prof.png",run_number,run_number)); */

/* /\* /\\* Either exit program or continue  *\\/ *\/ */
/* /\*   TString in; *\/ */
/* /\*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; *\/ */
/* /\*   std::cin >> in; *\/ */
/* /\*   if(in == "q") exit(-1); *\/ */

/* /\********************************************************************\/ */
/* /\*           Current Transducer vs Rampwave Signals Profile         *\/ */
/* /\********************************************************************\/ */

/*    TCanvas *c7 = new TCanvas("c7","Current Transducer vs Rampwave Signals",10,8,950,780); */

/*    c7->SetFillColor(kBlue-7); */
/*    c7->Divide(2,2); */
/*    c7->SetFillColor(0); */

/*    c7->cd(1); */
/*    c7->cd(1)->SetGridx(); */
/*    c7->cd(1)->SetGridy(); */

/*    lemx1_ramp_prof = new TProfile("lemx1_ramp_prof","LEMX1 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("lemx1/lemx1.num_samples*76.29e-6:ramp*76.29e-6>>lemx1_ramp_prof",fg_event_cut1,"prof"); */
/*    TProfile *lemx1_ramp_prof = (TProfile*)gDirectory->Get("lemx1_ramp_prof"); */

/*    lemx1_ramp_prof->SetMarkerColor(kRed); */
/*    lemx1_ramp_prof->SetLineColor(kRed); */
/*    lemx1_ramp_prof->Draw(); */
/*    lemx1_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    lemx1_ramp_prof->GetYaxis()->SetTitle("LEMX1 (V)"); */
/*    lemx1_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c7->cd(2); */
/*    c7->cd(2)->SetGridx(); */
/*    c7->cd(2)->SetGridy(); */

/*    lemx2_ramp_prof = new TProfile("lemx2_ramp_prof","LEMX2 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("lemx2/lemx1.num_samples*76.29e-6:ramp*76.29e-6>>lemx2_ramp_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>167.5e3 && event_number<171.5e3","prof"); */
/*    TProfile *lemx2_ramp_prof = (TProfile*)gDirectory->Get("lemx2_ramp_prof"); */

/*    lemx2_ramp_prof->SetMarkerColor(kRed); */
/*    lemx2_ramp_prof->SetLineColor(kRed); */
/*    lemx2_ramp_prof->Draw(); */
/*    lemx2_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    lemx2_ramp_prof->GetYaxis()->SetTitle("LEMX2 (V)"); */
/*    lemx2_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c7->cd(3); */
/*    c7->cd(3)->SetGridx(); */
/*    c7->cd(3)->SetGridy(); */

/*    lemy1_ramp_prof = new TProfile("lemy1_ramp_prof","LEMY1 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("lemy1/lemx1.num_samples*76.29e-6:ramp*76.29e-6>>lemy1_ramp_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>195e3 && event_number<200e3","prof"); */
/*    TProfile *lemy1_ramp_prof = (TProfile*)gDirectory->Get("lemy1_ramp_prof"); */

/*    lemy1_ramp_prof->SetMarkerColor(kGreen); */
/*    lemy1_ramp_prof->SetLineColor(kGreen); */
/*    lemy1_ramp_prof->Draw(); */
/*    lemy1_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    lemy1_ramp_prof->GetYaxis()->SetTitle("LEMY1 (V)"); */
/*    lemy1_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c7->cd(4); */
/*    c7->cd(4)->SetGridx(); */
/*    c7->cd(4)->SetGridy(); */

/*    lemy2_ramp_prof = new TProfile("lemy2_ramp_prof","LEMY2 vs RAMP",150,-0.03,0.3,-0.4,0.4); */
/*    Mps_Tree->Draw("lemy2/lemx1.num_samples*76.29e-6:ramp*76.29e-6>>lemy2_ramp_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>223e3 && event_number<227e3","prof"); */
/*    TProfile *lemy2_ramp_prof = (TProfile*)gDirectory->Get("lemy2_ramp_prof"); */

/*    lemy2_ramp_prof->SetMarkerColor(kGreen); */
/*    lemy2_ramp_prof->SetLineColor(kGreen); */
/*    lemy2_ramp_prof->Draw(); */
/*    lemy2_ramp_prof->GetXaxis()->SetTitle("RAMP (V)"); */
/*    lemy2_ramp_prof->GetYaxis()->SetTitle("LEMY2 (V)"); */
/*    lemy2_ramp_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c7->Update(); */
/*    c7->SaveAs(Form("plots/%d/%d_lem_ramp_prof.png",run_number,run_number)); */

/* /\* /\\* Either exit program or continue  *\\/ *\/ */
/* /\*   TString in; *\/ */
/* /\*   std::cout << "Please type any key and press ENTER to continue or type q to quit ROOT terminal" << std::endl; *\/ */
/* /\*   std::cin >> in; *\/ */
/* /\*   if(in == "q") exit(-1); *\/ */

/* /\********************************************************************\/ */
/* /\*     Function Generator vs Current Transducer Signals Profile     *\/ */
/* /\********************************************************************\/ */

/*    TCanvas *c8 = new TCanvas("c8","Function Generator vs Current Transducer Signals",10,8,950,780); */

/*    c8->SetFillColor(kBlue-7); */
/*    c8->Divide(2,2); */
/*    c8->SetFillColor(0); */

/*    c8->cd(1); */
/*    c8->cd(1)->SetGridx(); */
/*    c8->cd(1)->SetGridy(); */

/*    fgx1_lemx1_prof = new TProfile("fgx1_lemx1_prof","FGX1 vs LEMX1",150,-0.1,0.1,-0.4,0.4); */
/*    Mps_Tree->Draw("fgx1*76.29e-6:lemx1*76.29e-6>>fgx1_lemx1_prof",fg_event_cut1,"prof"); */
/*    TProfile *fgx1_lemx1_prof = (TProfile*)gDirectory->Get("fgx1_lemx1_prof"); */

/*    fgx1_lemx1_prof->SetMarkerColor(kRed); */
/*    fgx1_lemx1_prof->SetLineColor(kRed); */
/*    fgx1_lemx1_prof->Draw(); */
/*    fgx1_lemx1_prof->GetXaxis()->SetTitle("LEMX1 (V)"); */
/*    fgx1_lemx1_prof->GetYaxis()->SetTitle("FGX1 (V)"); */
/*    fgx1_lemx1_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c8->cd(2); */
/*    c8->cd(2)->SetGridx(); */
/*    c8->cd(2)->SetGridy(); */

/*    fgx2_lemx2_prof = new TProfile("fgx2_lemx2_prof","FGX2 vs LEMX2",150,-0.1,0.1,-0.4,0.4); */
/*    Mps_Tree->Draw("fgx2*76.29e-6:lemx2*76.29e-6>>fgx2_lemx2_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>167.5e3 && event_number<171.5e3","prof"); */
/*    TProfile *fgx2_lemx2_prof = (TProfile*)gDirectory->Get("fgx2_lemx2_prof"); */

/*    fgx2_lemx2_prof->SetMarkerColor(kRed); */
/*    fgx2_lemx2_prof->SetLineColor(kRed); */
/*    fgx2_lemx2_prof->Draw(); */
/*    fgx2_lemx2_prof->GetXaxis()->SetTitle("LEMX2 (V)"); */
/*    fgx2_lemx2_prof->GetYaxis()->SetTitle("FGX2 (V)"); */
/*    fgx2_lemx2_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c8->cd(3); */
/*    c8->cd(3)->SetGridx(); */
/*    c8->cd(3)->SetGridy(); */

/*    fgy1_lemy1_prof = new TProfile("fgy1_lemy1_prof","FGY1 vs LEMY1",150,-0.1,0.1,-0.4,0.4); */
/*    Mps_Tree->Draw("fgy1*76.29e-6:lemy1*76.29e-6>>fgy1_lemy1_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>195e3 && event_number<200e3","prof"); */
/*    TProfile *fgy1_lemy1_prof = (TProfile*)gDirectory->Get("fgy1_lemy1_prof"); */

/*    fgy1_lemy1_prof->SetMarkerColor(kGreen); */
/*    fgy1_lemy1_prof->SetLineColor(kGreen); */
/*    fgy1_lemy1_prof->Draw(); */
/*    fgy1_lemy1_prof->GetXaxis()->SetTitle("LEMY1 (V)"); */
/*    fgy1_lemy1_prof->GetYaxis()->SetTitle("FGY1 (V)"); */
/*    fgy1_lemy1_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c8->cd(4); */
/*    c8->cd(4)->SetGridx(); */
/*    c8->cd(4)->SetGridy(); */

/*    fgy2_lemy2_prof = new TProfile("fgy2_lemy2_prof","FGY2 vs LEMY2",150,-0.1,0.1,-0.4,0.4); */
/*    Mps_Tree->Draw("fgy2*76.29e-6:lemy2*76.29e-6>>fgy2_lemy2_prof","abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>223e3 && event_number<227e3","prof"); */
/*    TProfile *fgy2_lemy2_prof = (TProfile*)gDirectory->Get("fgy2_lemy2_prof"); */

/*    fgy2_lemy2_prof->SetMarkerColor(kGreen); */
/*    fgy2_lemy2_prof->SetLineColor(kGreen); */
/*    fgy2_lemy2_prof->Draw(); */
/*    fgy2_lemy2_prof->GetXaxis()->SetTitle("LEMY2 (V)"); */
/*    fgy2_lemy2_prof->GetYaxis()->SetTitle("FGY2 (V)"); */
/*    fgy2_lemy2_prof->GetYaxis()->SetRangeUser(-0.4,0.4); */

/*    c8->Update(); */
/*    c8->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_fg_lem_prof.png",run_number,run_number)); */


  cout << "*********************************************************************" << endl;
  cout << "*       All plots are sucessfully saved in 'plots' directory        *" << endl;
  cout << "*********************************************************************" << endl;

}

/********************************************************************/
/*                         User Manual                              */
/*                                                                  */
/*Step1: Open a terminal and open root by doing: 'root -l -b'.      */
/*       To run without display remove '-b'. It will still create   */
/*       plots.                                                     */
/*Step2: Execute the macro inside root by doing: '.x bm_analysis.c' */
/*Step3: It will ask for a 'run #'. Type 'run '# and press 'ENTER'. */
/*       Plots will be produced in the directory 'plots/run#/' in   */
/*       format. Type 'display filename.png' to view the figure.    */
/********************************************************************/
