//************************************************************************************************************************************************//
//                                                                                                                                                //
//    This macros performs the Fast Fourier Transform of an array of devices and gives out,                                                       //
//    the FFT of the devices.                                                                                                                     // 
//    The reuslts are saved in to two  xxxx.gif files.                                                                                            // 
//                                                                                                                                                //
//    Author   : B. Waidyawansa                                                                                                                   //
//    contact  : buddhini@jlab.org                                                                                                                //
//    Reference: http://root.cern.ch/root/html/tutorials/fft/FFT.C.html                                                                           //
//                                                                                                                                                //
//************************************************************************************************************************************************//
//                                                                                                                                                //
//                                                                                                                                                //
//     run_number        : Number of the run containing the signal                                                                                //
//     device list       : The device signal which is to be FFT'd. eg, "hcbpmx","hcbpmy","injbpmx" and injbpmy", "mdbars", "lumis"                //
//                         Right now there are the only two device arrays implemented.                                                            //
//     min               : The lower limit of the event cut.                                                                                      //
//     max               : The upper limit of the event cut                                                                                       //
//                                                                                                                                                //
//************************************************************************************************************************************************// 
// Hw to use: e.g.
//root [6] FFT_multi(6206,"bc",5000,30000)
// --- Signal = bpmx 
// --- Length of the signal = 24s
// --- Sampling Rate = 1041.67
// --- Sample Size = 25000
// --- Average signal =-4.68837
// DC component: -336356.167598 Hz
// Nyquist harmonic: 3.649824 Hz
//Info in <TCanvas::Print>: GIF file 6206_FFT_of_bpmx_1.gif has been created
//Info in <TCanvas::Print>: GIF file 6206_FFT_of_bpmx_2.gif has been created
//
// 01-06-2012 B. Waidyawansa : Switched to using time derived from mps and helfreq instead of time per sample.
// 01-09-2012 B. Waidyawansa : Added FFT plotting option for MD bar sums and lumis.

#include <cstdlib>

#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <stdexcept>
#include <time.h>

#include "TMath.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TApplication.h"


#include "TH2F.h"
#include "TCanvas.h"
#include "TTree.h"

#include "TF1.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TProfile.h"
#include "TString.h"
#include "TLine.h"
#include "TBox.h"
#include "TChain.h"
#include "TVirtualFFT.h"

void get_fft(Int_t runnumber, TChain* tree, TString device, Int_t events,  Double_t up, Double_t low, 
	     TString error_code, Double_t event_rate);
Bool_t FindFiles(TString filename, TChain * chain);




// BPM list
const Int_t nbpms   = 23;
const Int_t injbpms = 21;
const Int_t nbars   = 8;
const Int_t nlumis  = 12;

TString axis[2] ={"X","Y"};
TString useaxis ="";

TString bpms[nbpms]= {
  "qwk_bpm3c07","qwk_bpm3c07a","qwk_bpm3c08","qwk_bpm3c11","qwk_bpm3c12","qwk_bpm3c14",
  "qwk_bpm3c16","qwk_bpm3c17","qwk_bpm3c18","qwk_bpm3c19","qwk_bpm3p02a","qwk_bpm3p02b",
  "qwk_bpm3p03a","qwk_bpm3c20","qwk_bpm3c21","qwk_bpm3h02","qwk_bpm3h04","qwk_bpm3h07a",
  "qwk_bpm3h07b","qwk_bpm3h07c","qwk_bpm3h08","qwk_bpm3h09","qwk_bpm3h09b"
};

TString injbpm[injbpms]={
  "qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
  "qwk_0i05" ,"qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
  "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
  "qwk_0l09","qwk_0l10","qwk_0r03","qwk_0r04","qwk_0r05",
  "qwk_0r06"
};

// MD bar list
TString mdbarsums[nbars]={
  "qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum",
  "qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"
};

// lumi list
TString lumis[nlumis]={
  "qwk_dslumi1",
  "qwk_dslumi2",
  "qwk_dslumi3",
  "qwk_dslumi4",
  "qwk_dslumi5",
  "qwk_dslumi6",
  "qwk_dslumi7",
  "qwk_dslumi8",
  "uslumi1_sum",
  "uslumi3_sum",
  "uslumi5_sum",
  "uslumi7_sum",
};

// Function
void 
FFT_mutli(Int_t run_number, TString devicelist, Int_t min, Int_t max)
{

  //stats
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);     
  gStyle->SetStatH(0.3);
  gStyle->SetStatW(0.3); 
  
  // histo parameters
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetTitleYOffset(1.3);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.65);
  gStyle->SetTitleSize(0.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(18);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");


  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.15);  
  gStyle->SetPadRightMargin(0.05);  


  gROOT->ForceStyle();


  
  

  // Get trees from the appropriate root file
  TChain *tree = new TChain("Mps_Tree");
  TChain *slow_tree = new TChain("Slow_Tree");

  TString filename = Form("QwPass1_%i.000.root", run_number);
  TFile *file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
  
  Bool_t found_hel = kFALSE;
  Bool_t found_slow = kFALSE;

  found_hel = FindFiles(filename, tree);
  found_slow = FindFiles(filename, slow_tree);
  
  if(!found_hel || !found_slow){
    filename = Form("first100k_%i.root", run_number);
    found_hel = FindFiles(filename, tree);
    found_slow = FindFiles(filename, slow_tree);
    if(!found_hel || ! found_slow){
      filename = Form("inj_parity_%i.000.trees.root", run_number);
      found_hel = FindFiles(filename, tree);
      found_slow = FindFiles(filename, slow_tree);
      if(!found_hel || !found_slow){
	std::cerr<<"Unable to find rootfile(s) for run "<<run_number<<std::endl;
	exit(1);
      }
    }
  }


  
  // Signal properties

  // number of events/samples
  Int_t   events = (max-min);

  if(devicelist.Contains("inj")){
    if(devicelist.Contains("injbpmy")) useaxis = axis[1];
    else if(devicelist.Contains("injbpmx")) useaxis = axis[0];
    else{
    std::cout<<"Unknown device type "<<devicelist<<std::endl;
    exit(1);
    }
  }
  else if(devicelist.Contains("hc")){
    if(devicelist.Contains("hcbpmy")) useaxis = axis[1];
    else if(devicelist.Contains("hcbpmx")) useaxis = axis[0];
    else {
      std::cout<<"Unknown device type "<<devicelist<<std::endl;
      exit(1);
    }
  }
  else if(devicelist.Contains("md")){
    // do nothing
  }
  else if(devicelist.Contains("lumi")){
    // do nothing
  }
  else{
    std::cout<<"Unknown device type "<<devicelist<<std::endl;
    exit(1);
  }; 



  // sampling rate/event rate
  TH1*h=NULL;
  TString command = "HELFREQ>>htemp";
  slow_tree->Draw(command,"","goff");
  h = (TH1*)gDirectory->Get("htemp");
  if(h==NULL){
    std::cout<<"Unable to find HELFREQ. Check Slow_Tree!"<<std::endl;
    exit(1);
  }

  Double_t event_rate = h->GetMean();
  delete h;
  if((event_rate>961) || (event_rate< 959)) {
    std::cout<<"Sampling rate "<<event_rate<<"Hz is not acceptable as the default Qweak setting. The default sampling rate of Qweak ADCs should be ~ 960HZ!"<<std::endl;
    exit(1);
  }
  Double_t length = events*1.0/event_rate; // signal length in seconds 
  Double_t up = (1.0*max)/event_rate; // uper limit of the signal in seconds
  Double_t low = (1.0*min)/event_rate; // lower limit of the signal in seconds
  std::cout<<" --- Signal = "<<devicelist<<"\n";
  std::cout<<" --- Signal starting at "<<low<<"s and ending at "<<up<<"s "<<std::endl;
  std::cout<<" --- Length of the signal = "<<length<<" s\n";
  std::cout<<" --- Sampling Rate = "<<event_rate<<" Hz \n";
  std::cout<<" --- Sample Size = "<<(max-min)<<" events \n";


  //***********************************
  //***** Plot the signal.
  //***********************************

  TH2D *h2_2;
  Int_t k =1;
  Int_t j= 1;

  TCanvas *canvas = NULL;
  canvas = new TCanvas("canvas","Fast Fourier Transform",10,10,1550,1180);
  canvas->Draw();
  canvas -> Divide(4,4);

  TString cut;
  TString device;

  // Do FFT on hallC bpms
  if(devicelist.Contains("hc")){
    for(Int_t i=0;i<nbpms;i++){

      canvas -> cd(2*k);
      gStyle->SetOptStat(0);
      device = Form("%s%s",bpms[i].Data(),useaxis.Data());      
      std::cout<<"Get FFT of "<<device<<std::endl;
      gPad->SetLogy();
      //  gPad->SetLogx();
      cut = Form("mps_counter>%i && mps_counter<%i && ErrorFlag == 0 && %s.Device_Error_Code == 0",
		 min,max,device.Data());
      get_fft(run_number, tree, device, events, up, low, cut, event_rate); 
      gPad->Update();

      // Plot the base signal distribution.
      canvas -> cd(2*k-1);
      gStyle->SetOptStat(1);
      tree->Draw(Form("%s.hw_sum:mps_counter>>htemp",device.Data()),cut);
      h2_2 = (TH2D*) gDirectory -> Get("htemp");
      if(h2_2 == NULL){
	std::cout<<"Unable to plot "<<Form("%s.hw_sum>>htemp",device.Data())<<std::endl;
	exit(1);
      }
      
      h2_2 -> SetTitle(Form("Distribution of %s in %i",device.Data(), run_number));
      h2_2 -> GetYaxis() -> SetTitle("Position (mm)");
      h2_2 -> GetYaxis() -> SetTitleSize(0.05);
      h2_2 -> GetXaxis() -> SetTitle("Events");
      h2_2 -> SetName(device);
      h2_2 -> SetTitle("");
      h2_2->DrawCopy();
      delete h2_2;

      canvas -> Modified();
      canvas -> Update();
      k++;
      if(k==9){
	k=1;
	canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
	j++;
      }

    };
    //Clear out the rest of the pads
    for(Int_t i =15; i<17; i++){
       canvas->cd(i);
       gPad->Clear();
    }
    canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
  }
  else if(devicelist.Contains("inj")){
    
    TH2D *h2_1;
    
    // Do FFT on injector bpms
    for(Int_t i=0;i<injbpms;i++){
      
      canvas -> cd(2*k);
      gStyle->SetOptStat(0);
      gPad->SetLogy();
      device = Form("%s%s",injbpm[i].Data(),useaxis.Data());
      std::cout<<"Get FFT of "<<device<<std::endl;
      cut = Form("mps_counter>%i && mps_counter<%i && ErrorFlag == 0 && %s.Device_Error_Flag == 0",
		 min,max,device.Data());
      get_fft(run_number, tree, device, events, up, low, cut, event_rate); 
      
      canvas ->cd(2*k-1);
      gStyle->SetOptStat(1);
      // Get the base signal distribution.
      tree->Draw(Form("%s.hw_sum:mps_counter>>htemp",device.Data()),cut);
      h2_1 = (TH2D*) gDirectory -> Get("htemp");
      
      if(h2_1 != NULL) h2_1 ->DrawCopy();
      else {
	std::cout<<"Unable to plot signal "<<device<<std::endl;
	exit(1);
      }      
      h2_1 -> SetTitle(Form("Distribution of %s in %i",device.Data(), run_number));
      h2_1 -> GetYaxis() -> SetTitle("Position (mm)");
      h2_1 -> GetYaxis() -> SetTitleSize(0.05);
      h2_1 -> GetXaxis() -> SetTitle("Events");
      h2_1 -> SetName(device);
      h2_1 -> SetTitle("");
      h2_1->DrawCopy();
      delete h2_1;

      canvas -> Modified();
      canvas -> Update();
      k++;
      if(k==9){
	k=1;
	canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j)); // save this canvas
	j++;
      }
    };
    //Clear out the rest of the pads
    for(Int_t i =11; i<17; i++){
      canvas->cd(i);
      gPad->Clear();
    }
    canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j)); // save this canvas
  }
  else if(devicelist.Contains("md")){

    for(Int_t i=0;i<nbars;i++){

      canvas -> cd(2*k);
      gStyle->SetOptStat(0);
      device = Form("%s",mdbarsums[i].Data());      
      std::cout<<"Get FFT of "<<device<<std::endl;
      gPad->SetLogy();
      //  gPad->SetLogx();
      cut = Form("mps_counter>%i && mps_counter<%i && ErrorFlag == 0 && %s.Device_Error_Code == 0",
 		 min,max,device.Data());
      get_fft(run_number, tree, device, events, up, low, cut, event_rate); 

      gPad->Update();

      // Plot the base signal distribution.
      canvas -> cd(2*k-1);
      gStyle->SetOptStat(1);
      tree->Draw(Form("%s.hw_sum:mps_counter>>htemp",device.Data()),cut);
      h2_2 = (TH2D*) gDirectory -> Get("htemp");
      if(h2_2 == NULL){
	std::cout<<"Unable to plot "<<Form("%s.hw_sum>>htemp",device.Data())<<std::endl;
	exit(1);
      }
      
      h2_2 -> SetTitle(Form("Distribution of %s in %i",device.Data(), run_number));
      h2_2 -> GetYaxis() -> SetTitle("Signal (V/muA)");
      h2_2 -> GetYaxis() -> SetTitleSize(0.05);
      h2_2 -> GetXaxis() -> SetTitle("Events");
      h2_2 -> SetName(device);
      h2_2 -> SetTitle("");
      h2_2->DrawCopy();
      delete h2_2;

      canvas -> Modified();
      canvas -> Update();
      k++; // 8th device was drawn..k is now 9
      if(k==9){
	k=1; //reset k
	canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
	j++;
      }

    }; 
  }
  else if(devicelist.Contains("lumi")){
    
    for(Int_t i=0;i<nlumis;i++){
      
      canvas -> cd(2*k);
      gStyle->SetOptStat(0);
      device = Form("%s",lumis[i].Data());      
      std::cout<<"Get FFT of "<<device<<std::endl;
      gPad->SetLogy();
      //  gPad->SetLogx();
      cut = Form("mps_counter>%i && mps_counter<%i && ErrorFlag == 0 && %s.Device_Error_Code == 0",
 		 min,max,device.Data());
      get_fft(run_number, tree, device, events, up, low, cut, event_rate); 

      gPad->Update();

      // Plot the base signal distribution.
      canvas -> cd(2*k-1);
      gStyle->SetOptStat(1);
      tree->Draw(Form("%s.hw_sum:mps_counter>>htemp",device.Data()),cut);
      h2_2 = (TH2D*) gDirectory -> Get("htemp");
      if(h2_2 == NULL){
	std::cout<<"Unable to plot "<<Form("%s.hw_sum>>htemp",device.Data())<<std::endl;
	exit(1);
      }
      
      h2_2 -> SetTitle(Form("Distribution of %s in %i",device.Data(), run_number));
      h2_2 -> GetYaxis() -> SetTitle("Signal (V/muA)");
      h2_2 -> GetYaxis() -> SetTitleSize(0.05);
      h2_2 -> GetXaxis() -> SetTitle("Events");
      h2_2 -> SetName(device);
      h2_2 -> SetTitle("");
      h2_2->DrawCopy();
      delete h2_2;

      canvas -> Modified();
      canvas -> Update();
      k++; // 8th device was drawn..k is now 9
      if(k==9){
	k=1; //reset k
	canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
	j++;
      }
    };
    //Clear out the rest of the pads
    for(Int_t i =10; i<17; i++){
      canvas->cd(i);
      gPad->Clear();
    }
    canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
    
    
  }
  else {
    std::cout<<"Unknown devicelist "<<devicelist<<std::endl;
    exit(1);
  }
  
  return;
}


void get_fft(Int_t runnumber, TChain* tree, TString device, Int_t events, Double_t up, Double_t low,
	     TString error_code, Double_t event_rate)
{
  
  TProfile *profile = new TProfile("profile","Signal Profile",events,low,up);  
  TString amplitude = Form("%s.hw_sum",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw(Form("amplitude:(mps_counter/%f)>>profile",event_rate),error_code,"prof");

  profile = (TProfile*) gDirectory -> Get("profile");
  if(profile == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }

  //Remove the DC/zero frequency component 
  Double_t dc_component = profile->GetMean(2);
  TAxis *xa = profile -> GetXaxis();
  Double_t nbins_prof =  xa->GetNbins();
  TH1D *htemp = new TH1D("htemp","htemp",events,low,up);
  htemp = (TH1D*)profile->ProjectionX("Signal");
  
  Double_t setvalue=0.0;
  xa = htemp -> GetXaxis();
  Double_t nbins =  xa->GetNbins();
  TH1D *hnoise = new TH1D("hnoise","hnoise",events,low,up);  
  
  for(Int_t n = 0;n < nbins;n++){
    setvalue = (htemp->GetBinContent(n+1)-dc_component); // from each bin, subtract DC value
    if(htemp->GetBinContent(n+1) != 0){
      hnoise->SetBinContent(n+1,setvalue);
    }
  }

  // Get the magnitude of the fourier transform
  TH1 * fftmag = NULL;
  TVirtualFFT::SetTransform(0);
  fftmag = ((TH1*)hnoise)->FFT(fftmag,"MAG");  
  
  // Rescale the x axis of the transform to get only the positive frequencies. 
  // Due to the periodicity of FFT in number of samples the FFT output will be a mirrored image.
  
  xa = fftmag -> GetXaxis();
  Double_t nbins_mag =  xa->GetNbins();
  Double_t frequency = event_rate;
  Double_t scale = 1.0/sqrt(events);
  
  TH1D *hfft = new TH1D("hfft",Form("Frequency Spectrum of %s",device.Data()) ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    hfft->SetBinContent(n+1,scale*(fftmag->GetBinContent(n+1))); 
  }
  hfft -> GetXaxis() -> SetTitle("Frequency (Hz)");
  hfft -> GetYaxis() -> SetTitle("Amplitude");
  hfft -> GetXaxis() -> SetTitleSize(0.05);

  xa = hfft -> GetXaxis();
  xa -> SetRangeUser((xa->GetBinLowEdge(1)), xa->GetBinUpEdge(xa->GetNbins()/5.0));
  hfft -> GetYaxis() -> SetRangeUser(0.0000001,100);

  hfft->SetLineColor(1);
  hfft -> DrawCopy();

  delete hfft;
  delete htemp;  
  delete profile;
  delete hnoise;
  delete fftmag;
}


// Adapted from hallc_bcm_calib.C GetTree function written by J.H.Lee.
// This function use the chain to add all of the files with name 
// "filename" and then use that chain to get the corresponding file list.
// it return s 1 if files are found. 0 if not.

Bool_t FindFiles(TString filename, TChain * chain)
{
  TString file_dir;
  Int_t  chain_status = 0;

  file_dir = gSystem->Getenv("QW_ROOTFILES");
  // To make the chain actually check if a file exsists, have to set the number of entries<=0.
  chain_status = chain->Add(Form("%s/%s", file_dir.Data(), filename.Data()),0);
  
  if(chain_status) {
    
    TString chain_name = chain -> GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;

    while (( chain_element=(TChainElement*)next() )){      
      std::cout << "File:  " 
		<< chain_element->GetTitle() 
		<< " is added into the file list with the name"<< std::endl;
    }
  }
  else {
    std::cout << "There is (are) no "
	      << filename 
	      << "."
	      << std::endl;
  }
  return chain_status;
};


