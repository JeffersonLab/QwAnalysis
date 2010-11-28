/********************************************************************/
/*                         Nuruzzaman                               */
/*                         08/10/2010                               */
/*                                                                  */
/*                   Last Edited:08/16/2010                         */
/*                                                                  */
/* This Macro create plots for Beam Modulation system. It includes  */
/* Figure run#_bpms_eve_1.png :BPMs w.r.t Event Number              */
/* Figure run#_bpms_eve_2.png :BPMs w.r.t Event Number              */
/* Figure run#_bpms_fgs_1.png :BPMs w.r.t Function Generator Signals*/
/* Figure run#_bpms_fgs_2.png :BPMs w.r.t Function Generator Signals*/
/********************************************************************/

{

  gROOT->Reset();
#include <TROOT.h>
#include <TFile.h>
#include <TH2.h>
#include <TProfile.h>
#include <TBox.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TString.h>
#include <TAxis.h>
#include <TCut.h>

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

  /*   //Get the root file */
  /*   sprintf(filename,"rootfiles/QwPass1_%d.000.root",directory.Data(),atoi(run_number)); */
  /*   f = new TFile(filename); */
  /*   if(!f->IsOpen()) */
  /*     return 0; */
  /*   std::cout<<"Obtaining data from: "<<filename<<"\n"; */

  /* Create an input TFile and load a root file from rootfiles directory */
  TFile f(Form("/net/cdaq/cdaql5data/qweak/rootfiles/Qweak_%d.000.root", run_number));

  if(!f->IsOpen()) {
    std::cout<<"Unable to find Rootfile for Run # "
	     << run_number
	     <<". Please insert a correct Run #. Exiting the program! "<<std::endl;
    exit(1);
  }

  cout << "Qweak_"
       << run_number 
       <<".000.root ROOT file loaded sucessfully" << endl;
   
  //   cout << "Please analize the data file to create root file" << endl;

  /* Create a directory by run number under plots directory */
  gSystem->mkdir(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d", run_number));
  cout << "Directory "
       << run_number 
       << " has been created sucessfully" << endl;

  cout << "*********************************************************************" <<endl;
  cout << "*                 Starting Beam Modulation Plots                    *" <<endl;
  cout << "*********************************************************************" << endl;

  /* Histogram parameters */
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.9);

//   const int bpmx = 6;
//   char *list_of_bpms[bpmx] = {
//     "3h09bX","3h09X","3h08X","3h07cX","3h07bX","3h07aX"
//   };


  /* Defining cuts for different FG signals */
TCut fg_event_cut1 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>28.5e3 && event_number<33e3";
// TCut fg_event_cut2 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>46e3 && event_number<52e3";
// TCut fg_event_cut3 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>66e3 && event_number<72e3";
// TCut fg_event_cut4 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>86e3 && event_number<94e3";
// TCut fg_event_cut5 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>108e3 && event_number<116e3";

  /********************************************************************/
  /*                    Main Detector for FGX1                        */
  /********************************************************************/
    TCanvas *canvas01 = new TCanvas("canvas01","BPM 3H09 Responses for Function Generator Signals1",0,0,900,700);
    //  canvas1->SetFillColor(kBlue-7);
  canvas01->Divide(3,2);
  canvas01->SetFillColor(0);

    TCanvas *canvas02 = new TCanvas("canvas02","Target BPM Responses for Function Generator Signals2",60,60,900,700);
    //  canvas1->SetFillColor(kBlue-7);
  canvas02->Divide(3,2);
  canvas02->SetFillColor(0);

    TCanvas *canvas1 = new TCanvas("canvas1","Main Detector Responses for Function Generator Signals1",500,0,900,700);
    //  canvas1->SetFillColor(kBlue-7);
  canvas1->Divide(3,3);
  canvas1->SetFillColor(0);
    TCanvas *canvas2 = new TCanvas("canvas2","Main Detector Responses for Function Generator Signals2",560,60,900,700);
    //  canvas2->SetFillColor(kBlue-7);
  canvas2->Divide(3,3);
  canvas2->SetFillColor(0);
    TCanvas *canvas3 = new TCanvas("canvas3","Main Detector Responses for Function Generator Signals3",620,120,900,700);
    //  canvas3->SetFillColor(kBlue-7);
  canvas3->Divide(3,3);
  canvas3->SetFillColor(0);
  TCanvas *canvas4 = new TCanvas("canvas4","Main Detector Responses for Function Generator Signals4",680,180,900,700);
  //  canvas4->SetFillColor(kBlue-7);
  canvas4->Divide(3,3);
  canvas4->SetFillColor(0);

  TCanvas *canvas5 = new TCanvas("canvas5","Main Detector Responses for Function Generator Signals5",740,240,900,700);
  //  canvas5->SetFillColor(kBlue-7);
  canvas5->Divide(3,3);
  canvas5->SetFillColor(0);

  TCanvas *canvas6 = new TCanvas("canvas6","Main Detector Responses for Function Generator Signals6",800,300,900,700);
  //  canvas6->SetFillColor(kBlue-7);
  canvas6->Divide(3,3);
  canvas6->SetFillColor(0);

  TCanvas *canvas7 = new TCanvas("canvas7","Main Detector Responses for Function Generator Signals7",800,300,900,700);
  //  canvas6->SetFillColor(kBlue-7);
  canvas7->Divide(1,2);
  canvas7->SetFillColor(0);

  TCanvas *canvas8 = new TCanvas("canvas8","Main Detector Responses for Function Generator Signals8",800,300,900,700);
  //  canvas6->SetFillColor(kBlue-7);
  canvas8->Divide(1,2);
  canvas8->SetFillColor(0);


/* load the MPS_Tree. It has data based on individul events */
    nt = (TTree*)f->Get("Mps_Tree");
    if(nt == NULL) {
      std::cout<<"Unable to find Mps_Tree. Exiting the program! "<<std::endl;
      exit(1);
    }


    canvas01->cd(1);
    canvas01->cd(1)->SetGridx();
    canvas01->cd(1)->SetGridy();

   TH2D *bpm3h09bX_ramp = new TH2D("bpm3h09bX_ramp","BPM 3H09B vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h09bX:ramp*76.29e-6>>bpm3h09bX_ramp",fg_event_cut1);
   TH2D *bpm3h09bY_ramp = new TH2D("bpm3h09bY_ramp","BPM 3H09B vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h09bY:ramp*76.29e-6>>bpm3h09bY_ramp",fg_event_cut1);

   bpm3h09bX_ramp->SetMarkerColor(kRed);
   bpm3h09bX_ramp->Draw("same");
   bpm3h09bY_ramp->SetMarkerColor(kGreen);
   bpm3h09bY_ramp->Draw("same");

    canvas01->cd(2);
    canvas01->cd(2)->SetGridx();
    canvas01->cd(2)->SetGridy();

   TH2D *bpm3h09X_ramp = new TH2D("bpm3h09X_ramp","BPM 3H09 vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h09X:ramp*76.29e-6>>bpm3h09X_ramp",fg_event_cut1);
   TH2D *bpm3h09Y_ramp = new TH2D("bpm3h09Y_ramp","BPM 3H09 vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h09Y:ramp*76.29e-6>>bpm3h09Y_ramp",fg_event_cut1);

   bpm3h09X_ramp->SetMarkerColor(kRed);
   bpm3h09X_ramp->Draw("same");
   bpm3h09Y_ramp->SetMarkerColor(kGreen);
   bpm3h09Y_ramp->Draw("same");

    canvas01->cd(3);
    canvas01->cd(3)->SetGridx();
    canvas01->cd(3)->SetGridy();

   TH2D *bpm3h08X_ramp = new TH2D("bpm3h08X_ramp","BPM 3H08 vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h08X:ramp*76.29e-6>>bpm3h08X_ramp",fg_event_cut1);
   TH2D *bpm3h08Y_ramp = new TH2D("bpm3h08Y_ramp","BPM 3H08 vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h08Y:ramp*76.29e-6>>bpm3h08Y_ramp",fg_event_cut1);

   bpm3h08X_ramp->SetMarkerColor(kRed);
   bpm3h08X_ramp->Draw("same");
   bpm3h08Y_ramp->SetMarkerColor(kGreen);
   bpm3h08Y_ramp->Draw("same");

    canvas01->cd(4);
    canvas01->cd(4)->SetGridx();
    canvas01->cd(4)->SetGridy();

   TH2D *bpm3h07cX_ramp = new TH2D("bpm3h07cX_ramp","BPM 3H07C vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h07cX:ramp*76.29e-6>>bpm3h07cX_ramp",fg_event_cut1);
   TH2D *bpm3h07cY_ramp = new TH2D("bpm3h07cY_ramp","BPM 3H07c vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h07cY:ramp*76.29e-6>>bpm3h07cY_ramp",fg_event_cut1);

   bpm3h07cX_ramp->SetMarkerColor(kRed);
   bpm3h07cX_ramp->Draw("same");
   bpm3h07cY_ramp->SetMarkerColor(kGreen);
   bpm3h07cY_ramp->Draw("same");

    canvas01->cd(5);
    canvas01->cd(5)->SetGridx();
    canvas01->cd(5)->SetGridy();

   TH2D *bpm3h07bX_ramp = new TH2D("bpm3h07bX_ramp","BPM 3H07B vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h07bX:ramp*76.29e-6>>bpm3h07bX_ramp",fg_event_cut1);
   TH2D *bpm3h07bY_ramp = new TH2D("bpm3h07bY_ramp","BPM 3H07B vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h07bY:ramp*76.29e-6>>bpm3h07bY_ramp",fg_event_cut1);

   bpm3h07bX_ramp->SetMarkerColor(kRed);
   bpm3h07bX_ramp->Draw("same");
   bpm3h07bY_ramp->SetMarkerColor(kGreen);
   bpm3h07bY_ramp->Draw("same");

    canvas01->cd(6);
    canvas01->cd(6)->SetGridx();
    canvas01->cd(6)->SetGridy();

   TH2D *bpm3h07aX_ramp = new TH2D("bpm3h07aX_ramp","BPM 3H07A vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h07aX:ramp*76.29e-6>>bpm3h07aX_ramp",fg_event_cut1);
   TH2D *bpm3h07aY_ramp = new TH2D("bpm3h07aY_ramp","BPM 3H07A vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_bpm3h07aY:ramp*76.29e-6>>bpm3h07aY_ramp",fg_event_cut1);

   bpm3h07aX_ramp->SetMarkerColor(kRed);
   bpm3h07aX_ramp->Draw("same");
   bpm3h07aY_ramp->SetMarkerColor(kGreen);
   bpm3h07aY_ramp->Draw("same");

   canvas02->cd();
   canvas02->cd()->SetGridx();
   canvas02->cd()->SetGridy();

   TH2D *bpmX_ramp = new TH2D("bpmX_ramp","TARGET BPM vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_targetX:ramp*76.29e-6>>bpmX_ramp",fg_event_cut1);
   TH2D *bpmY_ramp = new TH2D("bpmY_ramp","TARGET BPM vs RAMPWAVE",300,-0.0,0.3,300,-6,6);
   nt->Draw("qwk_targetY:ramp*76.29e-6>>bpmY_ramp",fg_event_cut1);

   bpmX_ramp->SetMarkerColor(kRed);
   bpmX_ramp->Draw("same");
   bpmY_ramp->SetMarkerColor(kGreen);
   bpmY_ramp->Draw("same");



  int position[9] = { 0, 4, 1, 2, 3, 6, 9, 8, 7 } ;

  int det=0;
  for ( int i=0; i<8; i++) {
    det = i+1;

    canvas1->cd(position[det]);
    canvas1->cd(position[det])->SetGridx();
    canvas1->cd(position[det])->SetGridy();

    TH2D *0; 
    detp_ramp = new TH2D("detp_ramp",Form("MD%d vs RAMPWAVE for FGX1",det),300,0,0.3,300,0,300);
    detm_ramp = new TH2D("detm_ramp",Form("MD%d vs RAMPWAVE for FGX1",det),300,0,0.3,300,0,300);
    nt->Draw(Form("md%dpos*76.29e-3:ramp*76.29e-6>>detp_ramp",det), fg_event_cut1);
    nt->Draw(Form("md%dneg*76.29e-3:ramp*76.29e-6>>detm_ramp",det), fg_event_cut1);
    TH2D *detp_ramp = (TH2D*)gDirectory->Get("detp_ramp");
    TH2D *detm_ramp = (TH2D*)gDirectory->Get("detm_ramp");

    detp_ramp->SetMarkerStyle(1);
    detp_ramp->SetMarkerColor(kRed);
    detp_ramp->SetLineColor(kRed);
    detp_ramp->Draw("same");
    detp_ramp->GetXaxis()->SetTitle("RAMPWAVE (V)");
    detp_ramp->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
    detp_ramp->SetTitle(Form("MD%d vs RAMPWAVE for FGX1",det));
    detp_ramp->GetYaxis()->SetRangeUser(20,100);

    detm_ramp->SetMarkerStyle(1);
    detm_ramp->SetMarkerColor(kBlack);
    detm_ramp->SetLineColor(kBlack);
    detm_ramp->Draw("same");
    detm_ramp->GetYaxis()->SetRangeUser(20,100);

    canvas2->cd(position[det]);
    canvas2->cd(position[det])->SetGridx();
    canvas2->cd(position[det])->SetGridy();

    TH2D *0;
    detp_ramp_prof = new TH2D("detp_ramp_prof",Form("MD%d vs RAMPWAVE for FGX1",det),300,0,0.3,300,0,300);
    detm_ramp_prof = new TH2D("detm_ramp_prof",Form("MD%d vs RAMPWAVE for FGX1",det),300,0,0.3,300,0,300);
    nt->Draw(Form("md%dpos*76.29e-3:ramp*76.29e-6>>detp_ramp_prof",det), fg_event_cut1,"prof");
    nt->Draw(Form("md%dneg*76.29e-3:ramp*76.29e-6>>detm_ramp_prof",det), fg_event_cut1,"prof");
    TH2D *detp_ramp_prof = (TH2D*)gDirectory->Get("detp_ramp_prof");
    TH2D *detm_ramp_prof = (TH2D*)gDirectory->Get("detm_ramp_prof");

    detp_ramp_prof->SetMarkerStyle(1);
    detp_ramp_prof->SetMarkerColor(kRed);
    detp_ramp_prof->SetLineColor(kRed);
    detp_ramp_prof->Draw();
    detp_ramp_prof->GetXaxis()->SetTitle("RAMPWAVE (V)");
    detp_ramp_prof->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
    detp_ramp_prof->SetTitle(Form("MD%d vs RAMPWAVE for FGX1",det));
    detp_ramp_prof->GetYaxis()->SetRangeUser(30,100);

    detm_ramp_prof->SetMarkerStyle(1);
    detm_ramp_prof->SetMarkerColor(kBlack);
    detm_ramp_prof->SetLineColor(kBlack);
    detm_ramp_prof->Draw("same");
    detm_ramp_prof->GetYaxis()->SetRangeUser(0,35);

    canvas3->cd(position[det]);
    canvas3->cd(position[det])->SetGridx();
    canvas3->cd(position[det])->SetGridy();

    TH2D *0;
    dett_ramp = new TH2D("dett_ramp",Form("MD%d vs RAMPWAVE for FGX1",det),300,0,0.3,300,0,300);
    nt->Draw(Form("(md%dpos+md%dneg)*76.29e-3:ramp*76.29e-6>>dett_ramp",det,det), fg_event_cut1);
    TH2D *dett_ramp = (TH2D*)gDirectory->Get("dett_ramp");

    dett_ramp->SetMarkerStyle(1);
    dett_ramp->SetMarkerColor(kRed-3);
    dett_ramp->SetLineColor(kRed-3);
    dett_ramp->Draw("same");
    dett_ramp->GetXaxis()->SetTitle("RAMPWAVE (V)");
    dett_ramp->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
    dett_ramp->SetTitle(Form("MD%d vs RAMPWAVE for FGX1",det));
    dett_ramp->GetYaxis()->SetRangeUser(60,180);

    canvas4->cd(position[det]);
    canvas4->cd(position[det])->SetGridx();
    canvas4->cd(position[det])->SetGridy();

    TH2D *0;
    dett_ramp_prof = new TH2D("dett_ramp_prof",Form("MD%d vs RAMPWAVE for FGX1",det),300,0,0.3,300,0,300);
    nt->Draw(Form("(md%dpos+md%dneg)*76.29e-3:ramp*76.29e-6>>dett_ramp_prof",det,det), fg_event_cut1,"prof");
    TH2D *dett_ramp_prof = (TH2D*)gDirectory->Get("dett_ramp_prof");

    dett_ramp_prof->SetMarkerStyle(1);
    dett_ramp_prof->SetMarkerColor(kRed-3);
    dett_ramp_prof->SetLineColor(kRed-3);
    dett_ramp_prof->Draw("same");
    dett_ramp_prof->GetXaxis()->SetTitle("RAMPWAVE (V)");
    dett_ramp_prof->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
    dett_ramp_prof->SetTitle(Form("MD%d vs RAMPWAVE for FGX1",det));
    dett_ramp_prof->GetYaxis()->SetRangeUser(60,180);

    canvas5->cd(position[det]);
    canvas5->cd(position[det])->SetGridx();
    canvas5->cd(position[det])->SetGridy();

    TH2D *0;
    dett_bpm3h09X = new TH2D("dett_bpm3h09X",Form("MD%d vs BPM 3H09 for FGX1",det),300,-10,10,300,0,300);
    dett_bpm3h09Y = new TH2D("dett_bpm3h09Y",Form("MD%d vs BPM 3H09 for FGX1",det),300,-10,10,300,0,300);
    nt->Draw(Form("(md%dpos+md%dneg)*76.29e-3:qwk_bpm3h09X>>dett_bpm3h09X",det,det), fg_event_cut1);
    nt->Draw(Form("(md%dpos+md%dneg)*76.29e-3:qwk_bpm3h09Y>>dett_bpm3h09Y",det,det), fg_event_cut1);
    TH2D *dett_bpm3h09X = (TH2D*)gDirectory->Get("dett_bpm3h09X");
    TH2D *dett_bpm3h09Y = (TH2D*)gDirectory->Get("dett_bpm3h09Y");

    dett_bpm3h09X->SetMarkerStyle(1);
    dett_bpm3h09X->SetMarkerColor(kRed);
    dett_bpm3h09X->SetLineColor(kRed);
    dett_bpm3h09X->Draw("same");
    dett_bpm3h09X->GetXaxis()->SetTitle("BPM 3H09 (mm)");
    dett_bpm3h09X->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
    dett_bpm3h09X->SetTitle(Form("MD%d vs BPM 3H09 for FGX1",det));
    dett_bpm3h09X->GetYaxis()->SetRangeUser(50,180);
    dett_bpm3h09X->GetXaxis()->SetRangeUser(-2,2);

    dett_bpm3h09Y->SetMarkerStyle(1);
    dett_bpm3h09Y->SetMarkerColor(kGreen);
    dett_bpm3h09Y->SetLineColor(kGreen);
    dett_bpm3h09Y->Draw("same");
    dett_bpm3h09Y->GetYaxis()->SetRangeUser(50,180);
    dett_bpm3h09Y->GetXaxis()->SetRangeUser(-2,2);

    canvas6->cd(position[det]);
    canvas6->cd(position[det])->SetGridx();
    canvas6->cd(position[det])->SetGridy();

    TH2D *0;
    dett_bpmX = new TH2D("dett_bpmX",Form("MD%d vs TARGET BPM for FGX1",det),300,-10,10,300,0,300);
    dett_bpmY = new TH2D("dett_bpmY",Form("MD%d vs TARGET BPM for FGX1",det),300,-10,10,300,0,300);
    nt->Draw(Form("(md%dpos+md%dneg)*76.29e-3:qwk_targetX>>dett_bpmX",det,det), fg_event_cut1);
    nt->Draw(Form("(md%dpos+md%dneg)*76.29e-3:qwk_targetY>>dett_bpmY",det,det), fg_event_cut1);
    TH2D *dett_bpmX = (TH2D*)gDirectory->Get("dett_bpmX");
    TH2D *dett_bpmY = (TH2D*)gDirectory->Get("dett_bpmY");

    dett_bpmX->SetMarkerStyle(1);
    dett_bpmX->SetMarkerColor(kRed);
    dett_bpmX->SetLineColor(kRed);
    dett_bpmX->Draw("same");
    dett_bpmX->GetXaxis()->SetTitle("TARGET BPM (mm)");
    dett_bpmX->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
    dett_bpmX->SetTitle(Form("MD%d vs TARGET BPM for FGX1",det));
    dett_bpmX->GetYaxis()->SetRangeUser(60,160);
    dett_bpmX->GetXaxis()->SetRangeUser(-3,1);

    dett_bpmY->SetMarkerStyle(1);
    dett_bpmY->SetMarkerColor(kGreen);
    dett_bpmY->SetLineColor(kGreen);
    dett_bpmY->Draw("same");
    dett_bpmY->GetYaxis()->SetRangeUser(60,160);
    dett_bpmY->GetXaxis()->SetRangeUser(-3,1);

    canvas7->cd(1);
    canvas7->cd(1)->SetGridx();
    canvas7->cd(1)->SetGridy();

    TH2D *0; 
    detp_ramp1 = new TH2D("detp_ramp1","MD1 vs RAMPWAVE for FGX1",300,0,0.3,300,0,300);
    nt->Draw("md1pos*76.29e-3:ramp*76.29e-6>>detp_ramp1", fg_event_cut1,"prof");
    TH2D *detp_ramp1 = (TH2D*)gDirectory->Get("detp_ramp1");

    detp_ramp1->SetMarkerStyle(1);
    detp_ramp1->SetMarkerColor(kRed);
    detp_ramp1->SetLineColor(kRed);
    detp_ramp1->Draw("same");
    detp_ramp1->GetXaxis()->SetTitle("RAMPWAVE (V)");
    detp_ramp1->GetYaxis()->SetTitle("MD1 (mV)");
    detp_ramp1->SetTitle("MD1 vs RAMPWAVE for FGX1");
    detp_ramp1->GetYaxis()->SetRangeUser(36,36.5);


    canvas7->cd(2);
    canvas7->cd(2)->SetGridx();
    canvas7->cd(2)->SetGridy();

    TH2D *0; 
    detp_ramp5 = new TH2D("detp_ramp5","MD5 vs RAMPWAVE for FGX1",300,0,0.3,300,0,300);
    nt->Draw("md5pos*76.29e-3:ramp*76.29e-6>>detp_ramp5", fg_event_cut1,"prof");
    TH2D *detp_ramp5 = (TH2D*)gDirectory->Get("detp_ramp5");

    detp_ramp5->SetMarkerStyle(1);
    detp_ramp5->SetMarkerColor(kRed);
    detp_ramp5->SetLineColor(kRed);
    detp_ramp5->Draw("same");
    detp_ramp5->GetXaxis()->SetTitle("RAMPWAVE (V)");
    detp_ramp5->GetYaxis()->SetTitle("MD5 (mV)");
    detp_ramp5->SetTitle("MD5 vs RAMPWAVE for FGX1");
    detp_ramp5->GetYaxis()->SetRangeUser(57.9,59);

    canvas8->cd(1);
    canvas8->cd(1)->SetGridx();
    canvas8->cd(1)->SetGridy();

    TH2D *0; 
    detp_3h09X1 = new TH2D("detp_3h09X1","MD1 vs 3H09X for FGX1",300,-2,2,300,0,300);
    nt->Draw("md1pos*76.29e-3:qwk_bpm3h09X>>detp_3h09X1", fg_event_cut1,"prof");
    TH2D *detp_3h09X1 = (TH2D*)gDirectory->Get("detp_3h09X1");

    detp_3h09X1->SetMarkerStyle(1);
    detp_3h09X1->SetMarkerColor(kRed);
    detp_3h09X1->SetLineColor(kRed);
    detp_3h09X1->Draw("same");
    detp_3h09X1->GetXaxis()->SetTitle("3H09X (mm)");
    detp_3h09X1->GetYaxis()->SetTitle("MD1 (mV)");
    detp_3h09X1->SetTitle("MD1 vs 3H09X for FGX1");
    detp_3h09X1->GetYaxis()->SetRangeUser(36,36.5);


    canvas8->cd(2);
    canvas8->cd(2)->SetGridx();
    canvas8->cd(2)->SetGridy();

    TH2D *0; 
    detp_3h09X5 = new TH2D("detp_3h09X5","MD5 vs 3H09X for FGX1",300,-2,2,300,0,300);
    nt->Draw("md5pos*76.29e-3:qwk_bpm3h09X>>detp_3h09X5", fg_event_cut1,"prof");
    TH2D *detp_3h09X5 = (TH2D*)gDirectory->Get("detp_3h09X5");

    detp_3h09X5->SetMarkerStyle(1);
    detp_3h09X5->SetMarkerColor(kRed);
    detp_3h09X5->SetLineColor(kRed);
    detp_3h09X5->Draw("same");
    detp_3h09X5->GetXaxis()->SetTitle("3H09X (mm)");
    detp_3h09X5->GetYaxis()->SetTitle("MD5 (mV)");
    detp_3h09X5->SetTitle("MD5 vs 3H09X for FGX1");
    detp_3h09X5->GetYaxis()->SetRangeUser(57.9,59);

    cout << "Plotting MD"
	 << det 
	 << " vs RAMPWAVE from Mps_Tree" <<endl;

  }
  canvas01->Update();
  canvas01->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_bpm_fgx1.png", run_number, run_number));

  canvas02->Update();
  canvas02->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_targetBPM_fgx1.png", run_number, run_number));
  canvas1->Update();
  canvas1->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_fgx1.png", run_number, run_number));
  canvas2->Update();
  canvas2->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_fgx1_prof.png", run_number, run_number));
  canvas3->Update();
  canvas3->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_fgx1_com.png", run_number, run_number));
  canvas4->Update();
  canvas4->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_fgx1_com_prof.png", run_number, run_number));
  canvas5->Update();
  canvas5->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_3h09_com.png", run_number, run_number));  canvas6->Update();
  canvas6->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_targetBPM_com.png", run_number, run_number));
  canvas7->Update();
  canvas7->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det15_fgx1.png", run_number, run_number));
  canvas8->Update();
  canvas8->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det15_3h09_fgx1.png", run_number, run_number));


//   /********************************************************************/
//   /*                    Main Detector and BPM 3H09                    */
//   /********************************************************************/

// //  for (int c=0; c<4; ++c) { 
// //    TCanvas *canvas = new TCanvas(Form("canvas_%d",c)); TPaveText t; t.AddText(Form("We are in canvas %d",c)); t.Draw(); 

// //  for (int c=0; c<4; ++c}{
// //    can = c+1;

//    //  int canvas[4] = { 1, 2, 3, 4} ;

// //  TCanvas *canvas = new TCanvas(Form("canvas_%d",c)); TPaveText t; t.AddText(Form("We are in canvas %d",c)); t.Draw(); 

//     TCanvas *canvas = new TCanvas("canvas","Main Detector Responses for Function Generator Signals5",240,240,900,700);

//     //  canvas->SetFillColor(kBlue-7);
//   canvas->Divide(3,3);
//   canvas->SetFillColor(0);


// //   for (int i=0; i<pdetectors; ++i) {
// //     char* title = list_of_pdetectors[i];
// //     TH2D *h= new TH2D(title);
// //   }
// //   const int mdetectors = 8;
// //   char *list_of_mdetectors[mdetectors] = {
// //     "md1neg","md2neg","md3neg","md4neg","md5neg","md6neg","md7neg","md8neg"
// //   };

// //   for (int i=0; i<mdetectors; ++i) {
// //     char* title = list_of_mdetectors[i];
// //     TH2D *h= new TH2D(title);
// //   }



//   int position[9] = { 0, 4, 1, 2, 3, 6, 9, 8, 7 } ;

//   int TCut fg_event_cut2 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>46e3 && event_number<52e3";
TCut fg_event_cut3 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>66e3 && event_number<72e3";
TCut fg_event_cut4 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>86e3 && event_number<94e3";
TCut fg_event_cut5 = "abs(((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))*4)<30 && ramp>0 && event_number>108e3 && event_number<116e3";det=0;
//   for ( int i=0; i<8; i++) {
//     det = i+1;

//     canvas->cd(position[det]);
//     canvas->cd(position[det])->SetGridx();
//     canvas->cd(position[det])->SetGridy();

//     TH2D *0; 
//     detp_bpmx = new TH2D("detp_bpmx",Form("MD%d vs BPM 3H09 for FGX1",det),300,-0.5,0.5,300,0,300);
//     detp_bpmy = new TH2D("detp_bpmy",Form("MD%d vs BPM 3H09 for FGX1",det),300,-0.5,0.5,300,0,300);
//     detm_bpmx = new TH2D("detm_bpmx",Form("MD%d vs BPM 3H09 for FGX1",det),300,-0.5,0.5,300,0,300);
//     detm_bpmy = new TH2D("detm_bpmy",Form("MD%d vs BPM 3H09 for FGX1",det),300,-0.5,0.5,300,0,300);

// /* load the MPS_Tree. It has data based on individul events */
//     nt = (TTree*)f->Get("Mps_Tree");
//     if(nt == NULL) {
//       std::cout<<"Unable to find Mps_Tree. Exiting the program! "<<std::endl;
//       exit(1);
//     }

//     //   int pad=0;
//     //   while ( int pad=4) {
//     //     i = 1;


//     nt->Draw(Form("md%dpos*76.29e-3:qwk_bpm3h09X>>detp_bpmx",det), fg_event_cut1,"prof");
//     nt->Draw(Form("md%dpos*76.29e-3:qwk_bpm3h09Y>>detp_bpmy",det), fg_event_cut1,"prof");
//     nt->Draw(Form("md%dneg*76.29e-3:qwk_bpm3h09X>>detm_bpmx",det), fg_event_cut1,"prof");
//     nt->Draw(Form("md%dneg*76.29e-3:qwk_bpm3h09Y>>detm_bpmy",det), fg_event_cut1,"prof");
//     TH2D *detp_bpmx =  (TH2D*)gDirectory->Get("detp_bpmx");
//     TH2D *detp_bpmy =  (TH2D*)gDirectory->Get("detp_bpmy");
//     TH2D *detm_bpmx =  (TH2D*)gDirectory->Get("detm_bpmx");
//     TH2D *detm_bpmy =  (TH2D*)gDirectory->Get("detm_bpmy");

//     detp_bpmx->SetMarkerStyle(1);
//     detp_bpmx->SetMarkerColor(kRed);
//     detp_bpmx->SetLineColor(kRed);
//     detp_bpmx->Draw();
//     detp_bpmx->GetXaxis()->SetTitle("BPM 3H09 (mm)");
//     detp_bpmx->GetYaxis()->SetTitle(Form("MD%d (mV)",det));
//     detp_bpmx->SetTitle(Form("MD%d vs BPM 3H09 for FGX1",det));
//     detp_bpmx->GetYaxis()->SetRangeUser(30,100);

//     detp_bpmy->SetMarkerStyle(1);
//     detp_bpmy->SetMarkerColor(kGreen);
//     detp_bpmy->SetLineColor(kGreen);
//     detp_bpmy->Draw("same");

//     detm_bpmx->SetMarkerStyle(1);
//     detm_bpmx->SetMarkerColor(kRed-3);
//     detm_bpmx->SetLineColor(kRed-3);
//     detm_bpmx->Draw("same");

//     detm_bpmy->SetMarkerStyle(1);
//     detm_bpmy->SetMarkerColor(kGreen+2);
//     detm_bpmy->SetLineColor(kGreen+2);
//     detm_bpmy->Draw("same");

//     cout << "Plotting MD"
// 	 << det 
// 	 << " vs BPM 3H09 from Mps_Tree" <<endl;

//   }

//   canvas->Update();
//   canvas->SaveAs(Form("/net/cdaqfs/home/cdaq/beamModulation/plots/%d/%d_det_bpm3h09.png", run_number, run_number));

  cout << "*********************************************************************" <<endl;
  cout << "*       All plots are sucessfully saved in 'plots' directory        *" <<endl;
  cout << "*********************************************************************"<<endl;

}
/********************************************************************/
/*                         User Manual                              */
/*                                                                  */
/*Step1: Open a terminal and open root by doing: 'root -l -b'.      */
/*       To run without display remove '-b'. It will still create   */
/*       plots.                                                     */
/*Step2: Execute the macro inside root by doing: '.x bm_physics.c' */
/*Step3: It will ask for a 'run #'. Type 'run '# and press 'ENTER'. */
/*       Plots will be produced in the directory 'plots/run#/' in   */
/*       format. Type 'display filename.png' to view the figure.    */
/********************************************************************/
