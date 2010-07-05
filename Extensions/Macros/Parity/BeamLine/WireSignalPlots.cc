//********************************************************************//
// Author : B. Waidyawansa
// Date   : March 10, 2009
//********************************************************************//
//
// This macros plots the event based X and Y signals for of the bpms defined in the 
// devicelist below and the canvas are printed on to .gif files.
// To use this macro, 
// open root and load the macro by typing .L WireSignalPlots.cc and type in the command.
//
//  PlotSignals(run_num)
//  run_num -: run number
//  
//
// e.g.
// root[0] .L WireSignalPlots.cc
// root[1]  PlotSignals(1160)



//********************************************
// Main function
//********************************************


#include "TMath.h"
void PlotSignals(Int_t runnum)
{
  
  gROOT -> ProcessLine(".L /home/buddhini/QwAnalysis/Extensions/Macros/Tracking/TSuperCanvas.cc+");
  
  const Int_t ndevices = 8;
  
  //**************** List of devices in the injector
  //   TString devicelist[ndevices]=
  //     {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
  //      "qwk_0i05" ,"qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
  //      "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
  //      "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05"};
  

  //*****************List of devices in the Hall C beamline  
  //  TString devicelist[ndevices]={"ipm3c07","ipm3c08","ipm3c11","ipm3c12","ipm3c14","ipm3c16"};

//*****************List of devices for MD tests  
  TString devicelist2[ndevices]={"qwk_pmt1m","qwk_pmt2m","qwk_pmt3m","qwk_pmt4m",
				"qwk_pmt5m","qwk_pmt6m","qwk_pmt7m","qwk_pmt8m"};
  TString devicelist1[ndevices]={"qwk_pmt1p","qwk_pmt2p","qwk_hel","qwk_pmt4p",
				"qwk_pmt5p","qwk_pmt6p","qwk_pmt7p","qwk_pmt8p"};

// //*****************List of devices for MD tests  
//   TString devicelist[ndevices]={"qwk_pmt5p","qwk_pmt6m","qwk_pmt6p","qwk_pmt7m"};

// //*****************List of devices for Beam Modulation tests  
//   TString devicelist1[ndevices]={"qwk_fgx1","qwk_fgx2","qwk_fgy1","qwk_fgy2",
// 				"qwk_fge","qwk_fgphase","qwk_spare1","qwk_spare2"};

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
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.2);  
  gStyle->SetPadRightMargin(0.08);  
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);



  //histogram parameters
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(1.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.07);


  //Get the root file
  TString directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration
  Char_t filename[300];
  sprintf(filename,"%sQweak_BeamLine_%d.delayed.root",directory.Data(),runnum);
  f = new TFile(filename);
  if(!f->IsOpen())
      return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";

  nt = (TTree*)f->Get("MPS_Tree"); //load the Tree


  TString ctitle = Form("Signal distributions for run  %i",runnum);
  //  c = new TSuperCanvas("c1",ctitle,10,10,1000,680);
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

  for(size_t j=0;j<1;j++){

    for(size_t i=0; i<ndevices; i++)    
      {  
	pad2->cd(i+1);
	nt->Draw(Form("(%s.hw_sum_raw*0.000076/%s.num_samples)>>h%s[%d]",
		      devicelist1[i].Data(),
		      devicelist1[i].Data(),
		      pos[j].Data(),i),"evnum<267160 || evnum>267240");

	h1 = (TH1D*)gDirectory->Get(Form("h%s[%d]",pos[j].Data(),(i)));
	h1->SetLineColor(2);
	h1->SetMarkerColor(4);
	h1->GetXaxis()->CenterTitle();
	h1->GetXaxis()->SetTitleOffset(1.0);
	h1->GetXaxis()->SetTitleSize(0.08);
	h1->GetYaxis()->SetTitle("Signal (V)");
	h1->GetYaxis()->CenterTitle();
	h1->GetYaxis()->SetTitleSize(0.08);
	h1->GetXaxis()->SetTitle(" Events");
	h1->GetYaxis()->SetTitleOffset(1.0);
	h1->SetTitle(Form("%s",devicelist1[i].Data()));
	h1->DrawCopy();


	pad2->cd(i+ndevices+1);
	nt->Draw(Form("(%s.hw_sum_raw*0.000076/%s.num_samples)>>h%s[%d]",
		      devicelist2[i].Data(),
		      devicelist2[i].Data(),
		      pos[j].Data(),(i+ndevices+1)),"evnum<267160||evnum>267240");

 	h1 = (TH1D*)gDirectory->Get(Form("h%s[%d]",pos[j].Data(),(i+ndevices+1)));
	h1->SetLineColor(2);
	h1->GetXaxis()->CenterTitle();
	h1->GetXaxis()->SetTitleOffset(1.0);
	h1->GetXaxis()->SetTitleSize(0.05);
	h1->GetXaxis()->SetTitle("Events");
	h1->GetYaxis()->CenterTitle();
	h1->GetYaxis()->SetTitleSize(0.07);
	h1->GetYaxis()->SetTitle("Signal (V)");
	h1->GetYaxis()->SetTitleOffset(1.6);
	h1->SetMarkerColor(2);
	h1->SetTitle(Form("%s",devicelist2[i].Data()));
 	h1->DrawCopy();

      }
    c->Print(Form("%i_signal_dist.gif",runnum,pos[j].Data()));
  }
  c->Delete();
  std::cout<<"Done plotting!\n";
};
  



