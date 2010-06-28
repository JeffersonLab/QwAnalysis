//********************************************************************//
// Author : B. Waidyawansa
// Date   : June 25th 2010
//********************************************************************//
// IA Scan is perfromed to calibrate the IA cell  so that the feedback system can 
// effectively reduce the charge asymmetry. This is done by changing the beam intensity 
// deliberately and measuring the helicity correlated charge asymmetry with a BPM.
//
// This script gives the results of an IA scan.
// To run this compileand make the executable IA Scan.
// Command line arguments that are needed to be passed are:
//       . /IA_Scan 753 bcm 0l02
//     arg[1] = runnumber
//     arg[2] = device type bcm/bpm
//     arg[3] = device name (as it appears in qweak_beamline.map file with ommiting the "qwk").




#include <vector>
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
#include <TText.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>


TString  directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration
TTree   *nt;
TCanvas *Canvas;
TString  CanvasTitle;


void plot_bpm( TTree* tree,TPad* pad, TString name);
void plot_bcm( TTree* tree, TString name);

//********************************************
// Main function
//********************************************


int main(Int_t argc,Char_t* argv[]){

  TString runnum, devname, devtype;
  TString waveplate;
  TString angle;
  Char_t  filename[200];

  if(argc<2 || argc>4){
    std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
    std::cerr<<" Please enter the run number followed by device type and device name. \n";
    exit(1);
  }
  else if (argc == 4){
    runnum=argv[1];
    devtype = argv[2];
    devname = argv[3];
  }

  std::cout<<"IHWP IN/OUT ? ";
  std::cin>>waveplate;

  if(waveplate.Contains("IN")){
    std::cout<<"IHWP angle = ";
    std::cin>>angle;
  }

  TApplication theApp("App",&argc,argv);

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");
  gStyle->Reset();

  //stats
  //  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.1);
  gStyle->SetOptStat(0);      
  gStyle->SetOptFit(1); 

  //Pad parameters
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.08);


  // histo parameters
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetTitleSize(0.07,"x");
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetTitleSize(0.07,"y");
  gStyle->SetTitleYOffset(0.5);


  //Get the root file
  sprintf(filename,"%sQweak_BeamLine_%d.000.root",directory.Data(),atoi(runnum));
  TFile * f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";
  
  //Load the HEL_Tree. It has data based on quartets.
  nt = (TTree*)f->Get("HEL_Tree"); 
  
  if(waveplate.Contains("IN"))
    CanvasTitle=Form("IA scan of run %s  ( RHWP = %s, RHWP Angle = %s)",
		     runnum.Data(),waveplate.Data(),angle.Data());
  else
    CanvasTitle=Form("IA scan of run %s  ( RHWP = %s)",
		     runnum.Data(),waveplate.Data());
 
  Canvas= new TCanvas(CanvasTitle, CanvasTitle,0,0,1000,800);
  Canvas->SetFrameBorderMode(1);  

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->Draw();
  pad1->SetFillColor(kRed-8);
  pad2->Draw();
  pad2->SetFillColor(10);

  pad1->cd();
  TString text = Form( CanvasTitle);
  TText* t1 = new TText(0.25,0.3,text);
  t1->SetTextSize(0.65);
  t1->Draw();
  
  // Check to see if the device exsists in the root file
  TH1F * h = NULL;
  f->GetObject(Form("qwk_%s_hw",devname.Data()),h);
  if(h==NULL) {
    std::cout<<" Requesting data from non-exsisting device "<<devname<<". Exiting program"<<std::endl;
    exit(1);
  }

  if(devtype.Contains("bpm"))
    plot_bpm(nt,pad2, devname);
  else if(devtype.Contains("bcm"))
    plot_bcm( nt, devname);
  else {
    std::cout<<"Unknown device "<<devname<<" Exiting program."<<std::endl;
    exit(1);
  }


  std::cout<<"here\n";

  // Print the bpm calibration polts on to gif file
  Canvas->Print(Form("IA_Scan_%s.gif",runnum.Data()));// the ) puts all the plots in to a one ps file
  std::cout<<" done with calibration \n";


  theApp.Run();
  return(0);
  
};

//********************************************
// Plot BPM parameters
//*******************************************

void plot_bpm( TTree* tree, TPad* pad, TString name){
  TH1D* hAq = NULL;
  TH1D* hdx = NULL;
  TH1D* hdy = NULL;

  pad->Divide(1,3);

  // Draw the charge asymmetry variation 
  pad->cd(1);
  tree->Draw(Form("asym_qwk_%s_EffectiveCharge.hw_sum*1000:scandata1>>hAq",name.Data()),"cleandata==1");
  hAq = (TH1D*)gDirectory->Get("hAq");
  hAq->SetTitle("Effective Charge VS Voltage(V)");
  hAq->GetXaxis()->SetTitle("Voltage(V)");
  hAq->GetYaxis()->SetTitle("Effective Charge (ppm)");
  hAq->SetMarkerColor(2);
  hAq->Fit("pol1");


  // Draw the X position difference variation 
  pad->cd(2);
  tree->Draw(Form("diff_qwk_%sX.hw_sum:scandata1>>hdx",name.Data()),"cleandata==1");
  hdx = (TH1D*)gDirectory->Get("hdx");
  hdx->SetTitle("Diff X VS Voltage(V)");
  hdx->GetYaxis()->SetTitle(" #DeltaX (#mum)");
  hdx->GetXaxis()->SetTitle("Voltage(V)");
  hdx->SetMarkerColor(1);
  hdx->Fit("pol1");


  // Draw the Y position difference variation 
  pad->cd(3);
  tree->Draw(Form("diff_qwk_%sY.hw_sum:scandata1>>hdy",name.Data()),"cleandata==1");
  hdy = (TH1D*)gDirectory->Get("hdy"); 
  hdy->SetTitle("Diff Y VS Voltage(V))");
  hdy->GetYaxis()->SetTitle(" #DeltaY (#mum)");
  hdy->GetXaxis()->SetTitle("Voltage(V)");
  hdy->SetMarkerColor(1);
  hdy->Fit("pol1");

  std::cout<<"Done\n";

};

//********************************************
// Plot BCM parameters
//*******************************************

void plot_bcm( TTree* tree, TString name){
  TH1D* hAq = NULL;

  // Draw the charge asymmetry variation 

  tree->Draw(Form("asym_qwk_%s.hw_sum*1000:scandata1>>hAq",name.Data()),"cleandata==1");
  hAq = (TH1D*)gDirectory->Get("hAq");
  hAq->SetTitle("Charge Asymmetry VS Voltage(V)");
  hAq->GetXaxis()->SetTitle("Voltage(V)");
  hAq->GetYaxis()->SetTitle("Aq(ppm)");
  hAq->SetMarkerColor(2);
  hAq->Fit("pol1");

  std::cout<<"Done\n";

};









