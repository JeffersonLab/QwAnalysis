/**************************************************************************/
/*                              Nuruzzaman                                */
/*                              10/22/2010                                */
/*                                                                        */
/*                        Last Edited:10/22/2010                          */
/*                                                                        */
/* This Macro create plots for Position Sensitivities.Please see the User */
/* Manual at the bottom of this macro for its use                         */
/**************************************************************************/

{
#include <SensitivityClass.h>
  cout << "----------------------------------------------------------------------------------" << endl;
/* Ask to input the run number to user */
  UInt_t run_number = 0;
  cout << "Enter run number" << endl;
  cout << "----------------------------------------------------------------------------------" << endl;
  cin >> run_number;

/* Create an input TFile and load a root file from rootfiles directory */
  TFile f(Form("/net/cdaq/cdaql5data/qweak/rootfiles/Qweak_%d.000.root", run_number));

  if(!f->IsOpen()) {
    std::cout<<"Unable to find Rootfile for Run # "
	     << run_number
	     <<". Please insert a correct Run #. Exiting the program! "<<std::endl;
    exit(1);
  }

  cout << "QwPass1_"
       << run_number 
       <<".000.root ROOT file loaded sucessfully" << endl;
   
/* Create a directory by run number under plots directory */
  gSystem->mkdir(Form("/net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots/%d", run_number));
  cout << "Directory "
       << run_number 
       << " has been created sucessfully" << endl;



  /* Ask to input the run condition to user */
  cout << "----------------------------------------------------------------------------------"<< endl;
  cout << "Enter beam current & raster size in following format: e.g; for 15 uA, 2x2 raster"  << endl;
  cout << "15 2"                                                                              << endl;
  cout << "----------------------------------------------------------------------------------"<< endl;
  UInt_t current = 0;
  cin >> current;
  UInt_t raster = 0;
  cin >> raster;
//   UInt_t moved = 0;
//   cin >> moved;

  gStyle->Reset();

/* Canvas parameters */
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetTitleYOffset(1.5);
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetTitleSize(0.035);
  gStyle->SetTitleYSize(0.035);
  gStyle->SetTitleXSize(0.035);
  //  gStyle->SetOptStat(0);
  gStyle->SetOptStat(111111);
//  gStyle->SetOptStat(000001111);

/* Set paper size Letter */
  gStyle->SetPaperSize(TStyle::kUSLetter);

/* load the Hel_Tree. It has data based on individul events */
    nt = (TTree*)f->Get("Hel_Tree");
    if(nt == NULL) {
      std::cout<<"Unable to find Hel_Tree. Exiting the program! "<<std::endl;
      exit(1);
    }

  cout << "----------------------------------------------------------------------------------" << endl;
  cout << "Please Insert Configuration. Insert 1 for -1mm X, 2 for +1mm X,3 for -1mm Y and   " << endl;
  cout << "4 for +1mm Y and hit ENTER                                                        " << endl;
  cout << "----------------------------------------------------------------------------------" << endl;
  UInt_t move = 0;
  cin >> move;

char *pos[4];
 pos[1] = "Beam moved :-1 mm in X";
 pos[2] = "Beam moved :+1 mm in X";
 pos[3] = "Beam moved :-1 mm in Y";
 pos[4] = "Beam moved :+1 mm in Y";


  cout << "**********************************************************************************" <<endl;
  cout << "*                   Starting Main Detector Sensitivity Plots                     *" <<endl;
  cout << "**********************************************************************************" << endl;

 char *s1 = "";

TCanvas *c1 = new TCanvas("c1",Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09X",run_number),0,0,900,700);
//     c1->SetFillColor(kBlue-9);
   pad10 = new TPad("pad10","pad10",0.01,0.93,0.99,0.99);
   pad11 = new TPad("pad11","pad11",0.01,0.01,0.99,0.94);
   pad10->Draw();
   pad11->Draw();
   pad10->cd();
  TString text = Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09X",run_number);
   ct10 = new TText(0.012,0.4,text);
   ct10->SetTextSize(0.5);
   ct10->Draw();
   pad11->cd();
   pad11->Divide(3,3);

   pad11->cd(5);
   pad11->cd(5)->SetFillColor(kWhite);
   TString text1 = Form("Q-weak Target: 4% US Al");
   TString text2 = Form("Beam Current : %d uA",current);
   TString text3 = Form("Raster Size  : %dx%d ",raster,raster);
   TString text4 = Form("%s",pos[move]);
   t12 = new TText(0.1,0.65,text1);
   t13 = new TText(0.1,0.55,text2);
   t14 = new TText(0.1,0.45,text3);
   t15 = new TText(0.1,0.35,text4);
   t12->SetTextSize(0.07);
   t13->SetTextSize(0.07);
   t14->SetTextSize(0.07);
   t15->SetTextSize(0.07);
   t12->Draw();
   t13->Draw();
   t14->Draw();
   t15->Draw();

TCanvas *c2 = new TCanvas("c2",Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09Y",run_number),30,50,900,700);
    //  canvas1->SetFillColor(kBlue-9);
   pad20 = new TPad("pad20","pad20",0.01,0.93,0.99,0.99);
   pad21 = new TPad("pad21","pad21",0.01,0.01,0.99,0.94);
   pad20->Draw();
   pad21->Draw();
   pad20->cd();
  TString text = Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09Y",run_number);
   ct20 = new TText(0.012,0.4,text);
   ct20->SetTextSize(0.5);
   ct20->Draw();
   pad21->cd();
   pad21->Divide(3,3);

   pad21->cd(5);
   pad21->cd(5)->SetFillColor(kWhite);
   t12->Draw();
   t13->Draw();
   t14->Draw();
   t15->Draw();

TCanvas *c3 = new TCanvas("c3",Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09X with Fit",run_number),60,100,900,700);
    //  canvas1->SetFillColor(kBlue-9);
   pad30 = new TPad("pad30","pad30",0.01,0.93,0.99,0.99);
   pad31 = new TPad("pad31","pad31",0.01,0.01,0.99,0.94);
   pad30->Draw();
   pad31->Draw();
   pad30->cd();
  TString text = Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09X with Fit",run_number);
   ct30 = new TText(0.012,0.4,text);
   ct30->SetTextSize(0.5);
   ct30->Draw();
   pad31->cd();
   pad31->Divide(3,3);

   pad31->cd(5);
   pad31->cd(5)->SetFillColor(kWhite);
   t12->Draw();
   t13->Draw();
   t14->Draw();
   t15->Draw();

TCanvas *c4 = new TCanvas("c4",Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09Y with Fit",run_number),90,150,900,700);
    //  canvas1->SetFillColor(kBlue-9);
   pad40 = new TPad("pad40","pad40",0.01,0.93,0.99,0.99);
   pad41 = new TPad("pad41","pad41",0.01,0.01,0.99,0.94);
   pad40->Draw();
   pad41->Draw();
   pad40->cd();
  TString text = Form("Run %d: Position Sensitivity of Main Detector for BPM 3H09Y with Fit",run_number);
   ct40 = new TText(0.012,0.4,text);
   ct40->SetTextSize(0.5);
   ct40->Draw();
   pad41->cd();
   pad41->Divide(3,3);

   pad41->cd(5);
   pad41->cd(5)->SetFillColor(kWhite);
   t12->Draw();
   t13->Draw();
   t14->Draw();
   t15->Draw();

  int position[9] = { 0, 4, 1, 2, 3, 6, 9, 8, 7 } ;

  int det=0;
  for ( int i=0; i<8; i++) {
    det = i+1;

    pad11->cd(position[det]);
    pad11->cd(position[det])->SetGridx();
    pad11->cd(position[det])->SetGridy();

    mdx = new TProfile("mdx",Form("MD%d vs Asymmetry vs BPM 3H09X",det),300,-1,1,-2,2);
    nt->Draw(Form("asym_md%dbarsum:diff_qwk_bpm3h09X>>mdx",det),s1,"prof");
    TProfile *mdx = (TProfile*)gDirectory->Get("mdx");
    mdx->Clone();
    mdx->SetMarkerStyle(1);
    mdx->SetMarkerColor(kRed);
    mdx->SetLineColor(kRed);
    mdx->Draw("same");
    mdx->GetXaxis()->SetTitle("BPM 3H09X (mm)");
    mdx->GetYaxis()->SetTitle(Form("MD%d Asymmetry",det));
    mdx->SetTitle(Form("MD%d Asymmetry vs BPM 3H09X",det));
    mdx->GetYaxis()->SetRangeUser(-0.012,0.012);
    mdx->GetXaxis()->SetRangeUser(-0.2,0.2);


    pad21->cd(position[det]);
    pad21->cd(position[det])->SetGridx();
    pad21->cd(position[det])->SetGridy();

    mdy = new TProfile("mdy",Form("MD%d vs Asymmetry vs BPM 3H09Y",det),300,-1,1,-2,2);
    nt->Draw(Form("asym_md%dbarsum:diff_qwk_bpm3h09Y>>mdy",det),s1,"prof");
    TProfile *mdy = (TProfile*)gDirectory->Get("mdy");
    mdy->Clone();
    mdy->SetMarkerStyle(1);
    mdy->SetMarkerColor(kGreen);
    mdy->SetLineColor(kGreen);
    mdy->Draw("same");
    mdy->GetXaxis()->SetTitle("BPM 3H09Y (mm)");
    mdy->GetYaxis()->SetTitle(Form("MD%d Asymmetry",det));
    mdy->SetTitle(Form("MD%d Asymmetry vs BPM 3H09Y",det));
    mdy->GetYaxis()->SetRangeUser(-0.012,0.012);
    mdy->GetXaxis()->SetRangeUser(-0.2,0.2);

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    //    gStyle->SetOptFit(1101);

    pad31->cd(position[det]);
    pad31->cd(position[det])->SetGridx();
    pad31->cd(position[det])->SetGridy();

    mdx_fit = new TProfile("mdx_fit",Form("MD%d vs Asymmetry vs BPM 3H09X",det),300,-1,1,-2,2);
    nt->Draw(Form("asym_md%dbarsum:diff_qwk_bpm3h09X>>mdx_fit",det),"diff_qwk_bpm3h09X>-0.05 && diff_qwk_bpm3h09X<0.05","prof");
    TProfile *mdx_fit = (TProfile*)gDirectory->Get("mdx_fit");
    mdx_fit->Clone();
    mdx_fit->SetMarkerStyle(1);
    mdx_fit->SetMarkerColor(kRed);
    mdx->SetLineColor(kRed);
    mdx->Draw("same");
    mdx_fit->SetLineColor(kRed);
    mdx_fit->Draw("same");
    mdx_fit->GetXaxis()->SetTitle("BPM 3H09X (mm)");
    mdx_fit->GetYaxis()->SetTitle(Form("MD%d Asymmetry",det));
    mdx_fit->SetTitle(Form("MD%d Asymmetry vs BPM 3H09X",det));
    mdx_fit->GetYaxis()->SetRangeUser(-0.012,0.012);
    mdx_fit->GetXaxis()->SetRangeUser(-0.2,0.2);
    TF1 *my_fitx = new TF1("my_fitx","pol1",-0.05,0.05);
    // my_fit->SetRange(-0.05,0.05);
    mdx_fit->Fit("my_fitx","0");
    my_fitx->SetLineColor(kRed-3);
    //    my_fitx->SetMarkerColor(kRed-3);
    my_fitx->Draw("same");

    pad41->cd(position[det]);
    pad41->cd(position[det])->SetGridx();
    pad41->cd(position[det])->SetGridy();

    mdy_fit = new TProfile("mdy_fit",Form("MD%d vs Asymmetry vs BPM 3H09Y",det),300,-1,1,-2,2);
    nt->Draw(Form("asym_md%dbarsum:diff_qwk_bpm3h09Y>>mdy_fit",det),"diff_qwk_bpm3h09Y>-0.06 && diff_qwk_bpm3h09Y<0.06","prof");
    TProfile *mdy_fit = (TProfile*)gDirectory->Get("mdy_fit");
    mdy_fit->Clone();
    mdy_fit->SetMarkerStyle(1);
    mdy_fit->SetMarkerColor(kGreen);
    mdy_fit->SetLineColor(kGreen);
    mdy->SetLineColor(kGreen);
    mdy->Draw("same");
    mdy_fit->Draw("same");
    mdy_fit->GetXaxis()->SetTitle("BPM 3H09Y (mm)");
    mdy_fit->GetYaxis()->SetTitle(Form("MD%d Asymmetry",det));
    mdy_fit->SetTitle(Form("MD%d Asymmetry vs BPM 3H09Y",det));
    mdy_fit->GetYaxis()->SetRangeUser(-0.012,0.012);
    mdy_fit->GetXaxis()->SetRangeUser(-0.2,0.2);
    TF1 *my_fity = new TF1("my_fity","pol1",-0.07,0.07);
    //    mdy_fit->Fit("pol1");
    mdy_fit->Fit("my_fity","0");
    my_fity->SetLineColor(kGreen+2);
    my_fity->Draw("same");

  }
  c1->Update();
  c1->SaveAs(Form("/net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots/%d/%d_det_3h09X.png",run_number,run_number));
  c2->Update();
  c2->SaveAs(Form("/net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots/%d/%d_det_3h09Y.png",run_number,run_number));
  c3->Update();
  c3->SaveAs(Form("/net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots/%d/%d_det_3h09X_fit.png",run_number,run_number));
  c4->Update();
  c4->SaveAs(Form("/net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots/%d/%d_det_3h09Y_fit.png",run_number,run_number));
 

  cout << "**********************************************************************************" <<endl;
  cout << "*        Congratulation !!!!!       All plots are sucessfully saved in           *" <<endl;
  cout << "*     /net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots     *" <<endl;
  cout << "**********************************************************************************" <<endl;

}
/**********************************************************************************/
/*                                User Manual                                     */
/*                                                                                */
/*Step1: Open a terminal and open root by doing: "root -l".                       */
/*       To run with display remove "-b".                                         */
/*Step2: Execute the macro inside root using:".x posSensitivity.cc"               */
/*Step3: It will ask for a 'run #'. Type 'run '# and press 'ENTER'.               */
/*Step4: Then it will ask for 'current & raster size'.                            */
/* e.g;  for 15 uA, 2x2 raster type                                               */
/*       15 2  and press 'ENTER'.                                                 */
/*Step5: Finally it will ask for 'configuration'.Which implies which way we moved */
/*       the beam and how much.Insert 1 for -1mm X, 2 for +1mm X, 3 for -1mm Y    */
/*       & 4 for +1mm Y and press 'ENTER'.                                        */
/*                                                                                */
/*       Plots will be produced in the directory:                                 */
/* "/net/cdaqfs/home/cdaq/qweak/QwScratch/plots/position_sensitivity_plots/"      */
/*       Type 'display filename.png' to view the figure.                          */
/**********************************************************************************/
