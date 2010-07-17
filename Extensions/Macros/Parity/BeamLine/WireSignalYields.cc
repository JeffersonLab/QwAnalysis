//********************************************************************//
// Author : B. Waidyawansa
// Date   : April 14, 2010
//********************************************************************//
//
// This macros plots the pattern based realtive X and Y yields for of the bpms defined in the 
// devicelist below and the canvas are printed on to .gif files. 
// To do this, the bpms needs to be analysed as bcm.
// To use this macro, 
// open root and load the macro by typing .L WireSignalPlots.cc and type in the command.
//
//  PlotSignals(run_num)
//  run_num -: run number
//  
//
// e.g.
// root[0] .L WireSignalYields.cc
// root[1]  PlotYields(1160)



//********************************************
// Main function
//********************************************


#include "TMath.h"
void PlotYields(Int_t runnum)
{
  
  gROOT -> ProcessLine(".L /home/buddhini/NoiseStudies/QwAnalysis/Extensions/Macros/Tracking/TSuperCanvas.cc+");
  
  const Int_t ndevices = 8;
  
  //**************** List of devices in the injector
  //   TString devicelist[ndevices]=
  //     {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
  //      "qwk_0i05" ,"qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
  //      "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
  //      "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05"};
  

  //*****************List of devices in the Hall C beamline  
  //  TString devicelist[ndevices]={"ipm3c07","ipm3c08","ipm3c11","ipm3c12","ipm3c14","ipm3c16"};

  //*****************List of devices for LED tests  
  // TString devicelist[ndevices]={"pmt","current","pmt_plus","pmt_minus","empty"};

  //*****************List of devices for MD tests  
  TString devicelist1[ndevices]={"qwk_pmt1p","qwk_pmt2p","qwk_hel","qwk_pmt4p","qwk_pmt5p","qwk_pmt6p","qwk_pmt7p","qwk_pmt8p"};
  TString devicelist2[ndevices]={"qwk_pmt1m","qwk_pmt2m","qwk_pmt3m","qwk_pmt4m","qwk_pmt5m","qwk_pmt6m","qwk_pmt7m","qwk_pmt8m"};

// //*****************List of devices for Beam Modulation tests  
//   TString devicelist1[ndevices]={"qwk_fgx1","qwk_fgx2","qwk_fgy1","qwk_fgy2",
// 				"qwk_fge","qwk_fgphase","qwk_spare","qwk_spare"};

// //*****************List of devices for Beam Modulation tests  
//   TString devicelist2[ndevices]={"qwk_lemx1","qwk_lemx2","qwk_lemy1","qwk_lemy2",
// 				 "qwk_trimx1","qwk_trimx2","qwk_trimy1","qwk_trimy2"};

  TString pos[2] = {"x","y"};

  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetOptStat(1); 


  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.2);  
  gStyle->SetPadRightMargin(0.08);  
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);


  //Get the root file
  TString directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration
  Char_t filename[300];
  sprintf(filename,"%sQweak_BeamLine_%d.delayed.root",directory.Data(),runnum);
  f = new TFile(filename);
  if(!f->IsOpen())
      return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";

  nt = (TTree*)f->Get("HEL_Tree"); //load the Tree


  TString ctitle = Form("Yield plots for  %iforst 5 mins",runnum);
  //  c1 = new TSuperCanvas("c1",c1title,10,10,1000,680);
  TCanvas *c = new TCanvas("c",ctitle,1200,1000);

  c->SetFillColor(0);

  pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  pad2 = new TPad("pad2","pad2",0.01,0.02,0.99,0.94);
  pad1->SetFillColor(11);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  t1 = new TText(0.20,0.3,ctitle);
  t1->SetTextSize(0.8);
  t1->Draw();

  pad2->Divide(4,4);

  
  TH1D * hx[50];
  TH1D * hy[50];
  TH1 *h1;
  Char_t var;

  for(size_t j=0;j<2;j++){

    for(size_t i=0; i<ndevices; i++)    
      {  
	pad2->cd(i+1);
	//	gPad->SetLogy();
	nt->Draw(Form("yield_%s.hw_sum*0.000076/(yield_%s.num_samples)>>h%s[%d]",
		      devicelist1[i].Data(),devicelist1[i].Data(),pos[j].Data(),i),"pattern_number>66810 || pattern_number<66790"); 
	  
	h1 = (TH1D*)gDirectory->Get(Form("h%s[%d]",pos[j].Data(),(i)));
	h1->SetLineColor(4);
	h1->GetXaxis()->CenterTitle();
	h1->GetXaxis()->SetTitleOffset(1.0);
	h1->GetXaxis()->SetTitleSize(0.08);
	h1->GetXaxis()->SetTitle(Form("%s (V)",devicelist1[i].Data()));
	h1->GetYaxis()->SetTitleOffset(1.2);
	h1->GetYaxis()->SetTitleSize(0.08);
	h1->GetYaxis()->SetTitle("quartets");
	h1->SetTitle("");
	h1->DrawCopy();
	delete gDirectory->Get(Form("h%s[%d]",pos[j].Data(),(i)));

	pad2->cd(i+ndevices+1);
	//	gPad->SetLogy();
	nt->Draw(Form("yield_%s.hw_sum*0.000076/(yield_%s.num_samples)>>h%s[%d]",
		      devicelist2[i].Data(),devicelist2[i].Data(),pos[j].Data(),i+ndevices+1),"pattern_number>66810 || pattern_number<66790");
	h1 = (TH1D*)gDirectory->Get(Form("h%s[%d]",pos[j].Data(),(i+ndevices+1)));
	h1->SetLineColor(2);
	h1->GetXaxis()->CenterTitle();
	h1->GetXaxis()->SetTitleOffset(1.0);
	h1->GetXaxis()->SetTitleSize(0.08);
	h1->GetXaxis()->SetTitle(Form("%s (V)",devicelist2[i].Data()));
	h1->GetYaxis()->SetTitle("quartets");
	h1->GetYaxis()->SetTitleOffset(1.2);
	h1->GetYaxis()->SetTitleSize(0.08);
	h1->SetTitle("");
	h1->DrawCopy();
	delete gDirectory->Get(Form("h%s[%d]",pos[j].Data(),(i+ndevices+1)));

      }
    c->Print(Form("%i_delayed_signal_yields.gif",runnum));
  }
  c->Delete();
  std::cout<<"Done plotting!\n";
};
  



