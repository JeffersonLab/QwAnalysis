//**********************************************************************************************//
// Author : B. Waidyawansa (buddhini@jlab.org)
// Date   : November 6th, 2010
//**********************************************************************************************//
// 
// November 16th Buddhini : Updated to read trees instead of histograms. 
//                          Use error Device_Error_Code == 0 to identify good events.
// 
// This macro can access a range of runs specified by "run1" to "run2" to
// to get the mean value and mean error of the "property" of the "device".
// The canvas is then saved in to a .gif file with name e.g.5070_5080_qwk_bpm3h09bXP_s_summary.gif.
//
// To use it, compile and do,
// ./get_from_runs run1 run2 device property
//  device - Give the full name of the device as it appears in the mapfiles. e.g. qwk_bpm3h09b
//  property - There are 4 properties that you can access with this script.
//          1. "a"  which is asymmetry from hel_histo
//          2. "d"  which is differences from hel_histo
//          3. "y"  which is yield from hel_histo
//          4. "s"  which is signal from mps_histo
//
// e.g. ./get_from_runs 5070 5080 qwk_bpm3h09bXP s
// 
// bugs - While scanning through the given run range, if it comes across a rootfile from a tracking run it will ignore that run
// as its supposed to do but will crash after going through like next five + runs. Don't know why its happening so couldn't fix
// it.
//
 
//********************************************
// Main function
//********************************************


#include <vector>
#include <TRandom.h>
#include <TSystem.h>
#include <TGraphErrors.h>
#include <TChainElement.h>
#include <TStyle.h>
#include <TFile.h>
#include <TVector.h>
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
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
#include <TPaveLabel.h>
#include <TGraphErrors.h>  
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TChain.h>
#include <cstdlib>


Int_t    run1 = 0;
Int_t    run2 = 0;
TString  device    = "";
TString  property  = "";
TString  filename  = "";
TString  directory="$QW_ROOTFILES/"; // the location of the rootfile used for calibration

Bool_t FindFiles(TString filename, std::vector<TFile*> &atfile_list, TChain * chain);

//***********************************
// main function
//***********************************

int main(Int_t argc,Char_t* argv[])
{
  
  //stats
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.1);     
  gStyle->SetOptStat(0); 

  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(0);

  // pads parameters
  gStyle->SetPadColor(kOrange-8); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadTopMargin(0.15);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.1);  
  gStyle->SetPadRightMargin(0.05);  

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.7);
  gStyle->SetTitleX(0.08);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleSize(0.05);
  gStyle->SetTitleOffset(1.5);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetTitleFillColor(kYellow-8);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");
  gStyle->SetHistMinimumZero();
  gStyle->SetBarOffset(0.25);
  gStyle->SetBarWidth(0.5);

  gROOT->ForceStyle();


  TApplication theApp("App",&argc,argv);
  TChain * tree = NULL;
  std::vector<TFile*> file_list;//List of TFile  to access histograms
  TString histoname = "";
  TString cut = "";
  TCanvas *c1 = new TCanvas();
  c1->Draw();
  c1->cd();
  // Read in the arguments.
  // The first argument should be the program name itself.
  // The second argument should be the run1
  // The 3rd is run2
  // 4th is device name
  // 5th is device property.


  if(argc<5 || argc>5){
    std::cerr<<"The correct way to execute this exe is to give"<<std::endl;
    std::cerr<<" ./get_from_range run1 run2 device prperty"<<std::endl;
    std::cerr<<"Please try again."<<std::endl;
    exit(1);
  }
  else if(argc == 5){
    run1     = atoi(argv[1]);
    run2     = atoi(argv[2]);
    device   = argv[3];
    property = argv[4];
    if(run1>run2){
      std::cerr<<"run1 should be < run2"<<std::endl;
      exit(1);
    }
    std::cout<<"Read run range "<<run1<<" - "<<run2<<std::endl;
    std::cout<<"Get mean and mean error for "<<property<<" of "<<device<<std::endl;

  }
   
  // Now clear out what we want to get from where.
  // Q:Which tree to access?
  // Q:What histogram to get?

  
 // Find the files from given run range.
  Bool_t found = kFALSE;
  Int_t k = 0;
  TVectorD mean(0);
  TVectorD sigma(0);
  TVectorD error(0);
  TVectorD runs(0);
  TVectorD fakeerror(0);
  TString filename = "";
  file_list.clear();
  Char_t  treename[200];
  for(Int_t run = run1; run< run2+1; run++){
 
    // first create chains
    if(property.Contains("d")||property.Contains("a")||property.Contains("y")){
      tree = new TChain("Hel_Tree");
      sprintf(treename,"Hel_Tree");
      if(property.Contains("y")){
	histoname = Form("yield_%s.hw_sum>>htemp",device.Data());
	cut = Form("yield_%s.Device_Error_Code == 0",device.Data());
      }
      if(property.Contains("a")){
	histoname = Form("asym_%s.hw_sum>>htemp",device.Data());
	cut = Form("asym_%s.Device_Error_Code == 0",device.Data());
      }
      if(property.Contains("d")){
	histoname = Form("diff_%s.hw_sum>>htemp",device.Data());
	cut = Form("diff_%s.Device_Error_Code == 0",device.Data());
      }
    }
    else if (property.Contains("s")){
      tree = new TChain("Mps_Tree");
      sprintf(treename,"Mps_Tree");
      histoname = Form("%s.hw_sum>>htemp",device.Data());
      cut = Form("%s.Device_Error_Code == 0",device.Data());
    }
    else{
      std::cout<<"Requested unknown "<<property<<" from device "<<device<<".\n I don't know this"<<std::endl;
      exit(1);
    }
    

    // Open the file
    file_list.clear();
    filename = "";
    filename = Form("QwPass*_%i.*root", run);
    found = FindFiles(filename, file_list, tree);
    if(!found){
      filename = Form("first*_%i.root", run);      
      found = FindFiles(filename, file_list, tree);
      if(!found){
	filename = Form("Qweak*_%i.*root", run);
	found = FindFiles(filename, file_list, tree);
	if(!found){
	  std::cerr<<"Unable to find root file(s) for run "<<run<<std::endl;
	  file_list.clear();
	} 
      }
    }
    // If there are no files in this range, nothing to plot. Move to next run.
    if(file_list.size() == 0){
      //do nithing.
    }
    else{
      
      //Found rootfiles for this run, get the histogram info from them.
      TH1F* htemp = NULL;
      Double_t meanl = 0;
      Double_t errorl = 0;
      Double_t widthl = 0.0;

      for(Int_t i = 0;i<file_list.size();i++){
	TFile* f = new TFile(file_list[i]->GetName()); 
	TTree* nt = (TTree*)(f->Get(treename));
	std::cout<<file_list[i]->GetName()<<std::endl;
	if(!nt) exit(1);
	nt->Draw(histoname,cut);
	htemp = (TH1F*)gDirectory->Get("htemp");
	if (!htemp){
	  std::cerr<<"Histogram name does not exist"<<std::endl;
	} else {
	  meanl  =  htemp->GetMean();
	  errorl =  htemp->GetMeanError();
	  widthl =  htemp->GetRMS();
	  if((meanl == 0) &&  (errorl == 0)){
	    std::cout<<"histogram has zero mean and szero width!"<<std::endl;
	  } 
	  else{
	    mean.ResizeTo(k+1);
	    sigma.ResizeTo(k+1);
	    error.ResizeTo(k+1);
	    runs.ResizeTo(k+1);
	    fakeerror.ResizeTo(k+1);
	    
	    if(property.Contains("a") || property.Contains("d")){
	      if(device.Contains("md"))
		mean.operator()(k) = meanl*1e+6 + 80; //put asymmetry in ppm and add a blinder to md asyms of 60ppm
	      else
		mean.operator()(k) = meanl*1e+6; //put ather asyms and diffs in ppm/nm. 
	      error.operator()(k) =  errorl*1e+6;
	      sigma.operator()(k)  = widthl*1e+6; // the width in root is called the RMS.
	    
	    }else{
	      mean.operator()(k)  = meanl;
	      error.operator()(k) = errorl;
	      sigma.operator()(k) = widthl; // the width in root is called the RMS.
	    }
	    
	    runs.operator()(k)      = run+(i*0.1);
	    fakeerror.operator()(k) = 0.0;
	    k++;
	  }
	}
      }
      delete tree;
    }
    
  }

  c1->Close();

  // If all the available histograms have 0 netries, nothing to plot. exit.
  if(mean.GetNoElements() == 0){
    std::cerr<<"There are no histograms called "<<histoname<<" in this range ("<<run1<<"-"<<run2<<")!."<<std::endl;
    exit(1);
  }
    
  // Create a graph and a histogram using these information.
  TGraphErrors* g  = new TGraphErrors(runs, mean, fakeerror, error);
  TGraph* g1       = new TGraph(runs, sigma);

  g->GetXaxis()->SetTitle("run number");
  g1->GetXaxis()->SetTitle("run number");
    
  TString prop;
  if(property.Contains("a")) prop = "asymmetry (ppm)";
  if(property.Contains("y")) {
    if(device.Contains("bcm")) prop = "yields (uA)";
    else prop = "yields";
  }
  if(property.Contains("d")) prop = "differences(nm)";
  if(property.Contains("s")) {
    if(device.Contains("bcm")) prop = "current (uA)";
    else if(device.Contains("bpm")) prop = "position (mm)";
    else prop = "signal";
  }
  g->GetYaxis()->SetTitle(Form("%s %s",device.Data(), prop.Data()));
  g->GetXaxis()->SetTitle("run number");
  //  g->SetTitle(Form("%s  %s  during run range %i to %i",device.Data(), prop.Data(),run1, run2));
  g->SetMarkerColor(kBlue);
  g->SetMarkerStyle(21);
  g->SetMarkerSize(0.7);
  g->SetLineWidth(2);
  g->SetLineColor(2);
  //  g->Fit("pol0");


  g1->SetTitle("");
  g1->SetFillColor(kGreen-1);
  
  // Create a canvas 
  TString title = Form("Run range summary of %s %s for runs %i to %i",device.Data(), prop.Data(),run1,run2);
  TCanvas *canvas = new TCanvas("canvas",title,800,600);
  canvas->SetFillColor(kYellow-8);
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->Draw();
  pad2->Draw();
  pad1->SetFillColor(45);
  pad1->cd();
  TText*t = new TText(0.03,0.25,title);
  t->SetTextSize(0.7);
  t->Draw();
  pad2->cd();
  pad2->Divide(1,2);
  pad2->cd(1);
  g->Draw("APE1");
  
  pad2->cd(2);
  //  gPad->SetLogy();
  g1->Draw("ABP");
  g1->GetYaxis()->SetTitle(Form("%s %s widths",device.Data(), prop.Data()));
  canvas->Update();
  canvas->SetBorderMode(0);
  // Save the canvas on to a .gif file
  canvas->SaveAs(Form("$QWANALYSIS/Extensions/Macros/Parity/Plots/%i_%i_%s_%s_summary.gif",run1, run2, device.Data(),property.Data())); 
  //   theApp.Run(); 
  Int_t l = 0;
  std::cin>>l;
};


Bool_t FindFiles(TString filename, std::vector<TFile*> &atfile_list, TChain * chain)
{
  TString file_dir;
  Bool_t  chain_status = kFALSE;
  TObjArray *fileElements = NULL;
  TChainElement *chain_element = NULL;

  chain ->Clear();
  file_dir = gSystem->Getenv("QWSCRATCH");
  if (!file_dir) file_dir = "~/scratch/";
  file_dir += "/rootfiles/";
  chain_status = chain->Add(Form("%s%s", file_dir.Data(), filename.Data()));

  if(chain_status) {

    fileElements = chain->GetListOfFiles();

    TIter next(fileElements);
    while (( chain_element=(TChainElement*)next() )){
      TFile *f=new TFile(chain_element->GetTitle());
      atfile_list.push_back(f);
      std::cout << "Found file:  ";
      f->Print(); 
    }
  }

  // else {
  //   std::cout << "There is (are) no "
  // 	      << filename 
  // 	      << "."
  // 	      << std::endl;
  // }

  return chain_status;
};
