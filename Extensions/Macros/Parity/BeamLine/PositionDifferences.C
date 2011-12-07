//**********************************************************************************************//
// Author : B. Waidyawansa
// Date   : March 10, 2009
//**********************************************************************************************//
//
// This script plots the variaiton of the  position differences across beamline
// The resulting plot is printed on to * .gif file.
//
// To use this script type
//
//       gmake  position_differnces.
//
// This should create the exe position_differnces.
//
// To run the exe type
//
// ./position_differnces run number
//   run_num - run number
//
// e.g.
// ./position_differnces 1672 
//



//********************************************
// Main function
//********************************************



#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TChain.h>
#include <cstdlib>
#include <TVector.h>


Double_t maxy1=0.0, maxy2=0.0;
Int_t runnum;
TString bcm; 
TString current;
TString tempname;

Char_t filename[300];

TString directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration

const Int_t ndevices = 7;

TString devicelist[ndevices]=
  {"1i02","1i04","1i06","0i02","0i02a",
   "0i05" ,"0i07"};

// TString devicelist[ndevices]=
//   {"1i02","1i04","1i06","0i02","0i02a",
//    "0i05" ,"0i07","0l01","0l02","0l03",
//    "0l04","0l05","0l06","0l07","0l08",
//    "0l09","0l10","0r01","0r03","0r04",
//    "0r05","0r06"};
// TString devicelist[ndevices]=
//   {
//     "bpm3c07","bpm3c07a","bpm3c08","bpm3c11","bpm3c12","bpm3c14","bpm3c16","bpm3c17",
//     "bpm3c18","bpm3c19","bpm3p02a","bpm3p02b","bpm3p03a","bpm3c20","bpm3c21","bpm3h02",
//     "bpm3h04","bpm3h07a","bpm3h07b","bpm3h07c","bpm3h08","bpm3h09","bpm3h09b","target"
//   };

// TString devicelist[ndevices]=
//   {
//    "bpm3h07a","bpm3h07b","bpm3h07c","bpm3h08","bpm3h09","bpm3h09b","target"
//   };


int main(Int_t argc,Char_t* argv[])
{
  
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.08);  
  gStyle->SetPadRightMargin(0.08);  
  gStyle->SetPaperSize(10,12);

  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetTitleSize(0.08);


   TApplication theApp("App",&argc,argv);

  // Read in the arguments.
  // The first argument should be the program name itself.
  // The second argument should be the run number
  // The third argument should be the BCM to be used with.

  if(argc<2 || argc>3){
    std::cout<<"Please enter a run number"<<std::endl;
    exit(1);
  }
  else if(argc == 2){
    runnum = atoi(argv[1]);
  }



  /*Open the rootfile/rootfiles*/
  TChain * tree      = new TChain("Hel_Tree");
  TString  Rfilename = Form("QwPass1_%i.000.root", runnum);
  TFile  * Rfile     = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

  if (Rfile->IsZombie()) {
    std::cout << "Error opening root file chain "<< Rfilename << std::endl;
    Rfilename = Form("qwinjector_%i.000.root", runnum);
    //    Rfilename = Form("first100k_%i.root", runnum);

    std::cout << "Try to open chain " << Rfilename << std::endl;
    Rfile = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

    if (Rfile->IsZombie()) {
      std::cout << "Error opening root file chain "<< Rfilename << std::endl;
      Rfilename = Form("Qweak_%i.000.root", runnum);
      std::cout << "Try to open chain " << Rfilename << std::endl;
      Rfile = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));
      
      if (Rfile->IsZombie()){
	std::cout << "File exit failure."<<std::endl; 
	tree = NULL;
      }
      else
	tree->Add(Form("$QW_ROOTFILES/Qweak_%i.000.root", runnum));
    }
    else
      tree->Add(Form("$QW_ROOTFILES/qwinjector_%i.000.root", runnum));
  }
  else
    tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root", runnum));
  
    std::cout<<"Opened rootfile/s "<<Rfilename<<std::endl;

  if(tree == NULL){
    std::cout<< "Unable to find the Mps_Tree." << std::endl; 
    exit(1);
  }


  //Create a new canvas
  TString ctitle = Form("dX & dY in run %i",runnum);
  TCanvas *c = new TCanvas("c",ctitle,1200,1000);
  c->Draw();

  //The title pad.
  TPad * pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  TPad * pad2 = new TPad("pad2","pad2",0.01,0.02,0.99,0.94);
  pad1->SetFillColor(11);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TText * t1 = new TText(0.02,0.3,ctitle);
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->Divide(1,2);

  // Draw histograms
  TH1D * h = NULL; //clear the contents
  TString cut;
  TString command;
  std::vector<TString>  names;
  TVectorD  mean(0);
  TVectorD  width(0);	
  TVectorD  meanerr(0);
  TVectorD  fakeerr(0);
  TVectorD  fakename(0);
  TVectorD  mean1(0);
  TVectorD  width1(0);	
  TVectorD  meanerr1(0);
  TVectorD  fakeerr1(0);
  Int_t k = 0;

  pad2->cd(1);

  for(Int_t i=0; i<ndevices; i++)    {  
    cut =  Form("diff_qwk_%sX.Device_Error_Code. == 0 && ErrorFlag != 0",
		devicelist[i].Data());
    command = Form("diff_qwk_%sX.hw_sum",devicelist[i].Data());
    tree->Draw(command, cut);
    h = (TH1D*) gPad->GetPrimitive("htemp");

    // Avoids the program from crashing when trying to plot NULL histograms
    if(h == NULL) 
      {
	std::cout<<"Cannot draw "<<command<<std::endl;
	exit(1);
      }
    mean.ResizeTo(k+1);
    width.ResizeTo(k+1);	
    meanerr.ResizeTo(k+1);
    fakeerr.ResizeTo(k+1);
    fakename.ResizeTo(k+1);

    std::cout<<"Getting data from : "<<devicelist[i]<<std::endl;
    mean.operator()(k) = h->GetMean()*1e6;
    width.operator()(k) = h->GetRMS();
    meanerr.operator()(k) = (h->GetMeanError()*1e6);
    fakeerr.operator()(k) = (0.0);
    fakename.operator()(k) =(k+1);
    names.push_back(devicelist[i]);
    delete h;

    cut =  Form("diff_qwk_%sY.Device_Error_Code. == 0 && ErrorFlag != 0",
		devicelist[i].Data());
    command = Form("diff_qwk_%sY.hw_sum",devicelist[i].Data());
    tree->Draw(command, cut);
    h = (TH1D*) gPad->GetPrimitive("htemp");

    // Avoids the program from crashing when trying to plot NULL histograms
    if(h == NULL) 
      {
	std::cout<<"Cannot draw "<<command<<std::endl;
	exit(1);
      }
    mean1.ResizeTo(k+1);
    width1.ResizeTo(k+1);	
    meanerr1.ResizeTo(k+1);
    fakeerr1.ResizeTo(k+1);

    std::cout<<"Getting data from : "<<devicelist[i]<<std::endl;
    mean1.operator()(k) = h->GetMean()*1e6;
    width1.operator()(k) = h->GetRMS();
    meanerr1.operator()(k) = (h->GetMeanError()*1e6);
    fakeerr1.operator()(k) = (0.0);
    k++;
    delete h;

  }

  Int_t size = mean.GetNoElements();
  if(mean.Norm1() == 0  ) {
    std::cout<<"There are no data of the type requested in this run!"<<std::endl;
    exit(1);
  };
  

  k = 0;

  TGraphErrors* grp = new TGraphErrors(fakename, mean, fakeerr,meanerr);
  grp->SetMarkerColor(2);
  grp->SetMarkerStyle(21);
  grp->SetMarkerSize(1);
  grp->SetTitle("");
  grp->GetYaxis()->SetTitle("X Position Differences (nm)");
  grp->GetYaxis()->SetTitleOffset(0.9);
  grp->Draw("AEP");

  TAxis *axis = grp->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  grp->GetHistogram()->GetYaxis()->SetRangeUser(-200,200);
  Double_t ypos =  grp->GetHistogram()->GetMaximum()-grp->GetHistogram()->GetMinimum();
  Double_t ylabel = grp->GetHistogram()->GetMinimum() - 0.01*ypos;
  grp->Draw("AEP");

  TText t;
  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);
  for (Int_t i=0;i<size;i++){
    t.DrawText(fakename[i],ylabel,names[i]);
  }
  c->Modified();
  c->Update();



  pad2->cd(2);


  TGraphErrors* grp1 = new TGraphErrors(fakename, mean1, fakeerr1,meanerr1);
  grp1->SetMarkerColor(2);
  grp1->SetMarkerStyle(21);
  grp1->SetMarkerSize(1);
  grp1->SetTitle("");
  grp1->GetYaxis()->SetTitle("Y Position Differences (nm)");
  grp1->GetYaxis()->SetTitleOffset(0.9);
  grp1->Draw("AEP");


  TAxis *axis1 = grp1->GetHistogram()->GetXaxis();
  axis1->SetLabelOffset(999);
  grp1->GetHistogram()->GetYaxis()->SetRangeUser(-200,200);
  Double_t ypos1 =  grp1->GetHistogram()->GetMaximum()-grp1->GetHistogram()->GetMinimum();
  Double_t ylabel1 = grp1->GetHistogram()->GetMinimum() - 0.01*ypos1;
  grp1->Draw("AEP");

 
  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);
  for (Int_t i=0;i<size;i++){
    t.DrawText(fakename[i],ylabel,names[i]);
  }

//   //now draw lines to represent aparutus
//   TLine *  A1 = new TLine(5.5,-maxy1-20.0,5.5,maxy1+20.0);
//   TLine *  A2 = new TLine(6.5,-maxy1-20.0,6.5,maxy1+20.0);
//   TLine *  A3 = new TLine(7.5,-maxy1-20.0,7.5,maxy1+20.0);
//   TLine *  A4 = new TLine(8.5,-maxy1-20.0,8.5,maxy1+20.0);
//   TLine *  A5 = new TLine(9.5,-maxy1-20.0,9.5,maxy1+20.0);

//   A1->Draw("same");
//   A2->Draw("same");
//   A3->Draw("same");
//   A4->Draw("same");
//   A5->Draw("same");



//   //now draw lines to represent aparutus
//   TLine * A11 = new TLine(5.5,-maxy2-20.0,5.5,maxy2+20.0);
//   TLine * A22 = new TLine(6.5,-maxy2-20.0,6.5,maxy2+20.0);
//   TLine * A33 = new TLine(7.5,-maxy2-20.0,7.5,maxy2+20.0);
//   TLine * A44 = new TLine(8.5,-maxy2-20.0,8.5,maxy2+20.0);
//   TLine * A55 = new TLine(9.5,-maxy2-20.0,9.5,maxy2+20.0);

//   A11->Draw("same");
//   A22->Draw("same");
//   A33->Draw("same");
//   A44->Draw("same");
//   A55->Draw("same");
 
  c->Modified();
  c->Update();
  
  // Save the canvas on to a .gif file
  c->Print(Form("%i_bpm_dx_dy.gif",runnum));
  std::cout<<"Done plotting!\n";
  theApp.Run();
  
  return(0);

};




