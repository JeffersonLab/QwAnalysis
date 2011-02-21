//************************************************************************************************************************************************//
//                                                                                                                                                //
//    This macros performs the Fast Fourier Transform of an array of devices and gives out,                                                       //
//    the FFT of the devices.                                                                                                                     // 
//    The reuslts are saved in to two  xxxx.gif files.                                                                                                   // 
//                                                                                                                                                //
//    Author   : B. Waidyawansa                                                                                                                   //
//    contact  : buddhini@jlab.org                                                                                                                //
//    Reference: http://root.cern.ch/root/html/tutorials/fft/FFT.C.html                                                                           //
//                                                                                                                                                //
//************************************************************************************************************************************************//
//                                                                                                                                                //
//                                                                                                                                                //
//     run_number        : Number of the run containing the signal                                                                                //
//     device list       : The device signal which is to be FFT'd. eg, "hcbpmx","hcbpmy","injbpmx" and injbpmy"                                  //
//                         Right now there are the only two device arrays implemented.                                                            //
//     min               : The lower limit of the event cut.                                                                                      //
//     max               : The upper limit of the event cut                                                                                       //
//                                                                                                                                                //
//************************************************************************************************************************************************// 
// Hw to use: e.g.
//root [6] FFT_multi(6206,"bpmx",5000,30000)
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

void 
FFT_mutli(Int_t run_number, TString devicelist, Int_t min, Int_t max)
{
  gDirectory->Delete("*") ;

  //stats
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);     
  gStyle->SetStatH(0.3);
  gStyle->SetStatW(0.3); 
  
  // histo parameters
  //  gStyle->SetTitleYOffset(1);
  gStyle->SetTitleXOffset(0.9);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.65);
  gStyle->SetTitleSize(0.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(18);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");

  gROOT->ForceStyle();




  //******* ADC settings
  const Double_t time_per_sample = 2e-06;//s
  const Double_t t_settle = 70e-06;//s
  

  //*******BPM list
  const Int_t nbpms = 23;
  const Int_t injbpms = 21;

  TString axis[2] ={"X","Y"};
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
    "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
    "qwk_0r06"
  };
  
  
  TCanvas *canvas = NULL;
  canvas = new TCanvas("canvas","Fast Fourier Transform",10,10,1550,1180);
  canvas->Draw();


  //******** get tree from the appropriate root file
  TChain *tree = new TChain("Mps_Tree");

  TString filename = Form("QwPass1_%i.000.root", run_number);
  TFile *file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
  
  if (file->IsZombie()) {
    std::cout << "Error opening root file chain " << filename << std::endl;
    filename = Form("first100k_%i.root", run_number);
    std::cout << "Try to open chain " << filename << std::endl;
    file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
    if (file->IsZombie()) {
      std::cout << "Error opening root file chain " << filename << std::endl;
      filename = Form("Qweak_%i.000.root", run_number);
      std::cout << "Try to open chain " << filename << std::endl;
      file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
      if(file->IsZombie()){
	tree = NULL;
	exit(-1);
      }
      else
	tree->Add(Form("$QW_ROOTFILES/Qweak_%i.*.root", run_number));	
    }
    else 
    tree->Add(Form("$QW_ROOTFILES/first100k_%i.root", run_number));
  }
  else 
    tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root", run_number));

  
  //************ Calculate signal properties

  // number of events/samples
  Int_t   events = (max-min);
  TString command = "";
  TString cut = "";
  TString useaxis ="";
  TString time    = "";

  // get sampling rate/event rate
  // for this I am going to use a specific device.
  if(devicelist.Contains("inj")){
    if(devicelist.Contains("injbpmy")) useaxis = axis[1];
    if(devicelist.Contains("injbpmx")) useaxis = axis[0];
    command = "qwk_1i02XP.num_samples>>htemp";
    cut  =  "qwk_1i02XP.Device_Error_Code == 0";
    time = Form("mps_counter*((qwk_1i02XP.num_samples*%f)+%f)",time_per_sample,t_settle);// units seconds.

  }
  else if(devicelist.Contains("hc")){
    if(devicelist.Contains("hcbpmy")) useaxis = axis[1];
    if(devicelist.Contains("hcbpmx")) useaxis = axis[0];
    command = "qwk_bpm3h09bXP.num_samples>>htemp";
    cut  =  "qwk_bpm3h09bXP.Device_Error_Code == 0";
    time = Form("mps_counter*((qwk_bpm3h09bXP.num_samples*%f)+%f)",time_per_sample,t_settle);// units seconds.

  }
  else{
    std::cout<<"Unknown device type "<<devicelist<<std::endl;
    exit(1);
  }; 

  tree->SetAlias("time",time);

  TH1*h=NULL;
  tree->Draw(command);
  h = (TH1*)gDirectory->Get("htemp");
  if(h==NULL){
    std::cout<<"Unable to get num_samples using bcm1!"<<std::endl;
    exit(1);
  }

  Double_t event_rate = 1.0/(h->GetMean()*time_per_sample+t_settle);
  Double_t signal_length = events*1.0/event_rate; 
  
  std::cout<<" --- Signal = "<<devicelist<<"\n";
  std::cout<<" --- Length of the signal = "<<signal_length<<"s\n";
  std::cout<<" --- Sampling Rate = "<<event_rate<<"\n";
  std::cout<<" --- Sample Size = "<<(max-min)<<"\n";
  delete h;

  
  //************  Plot the signal.

  // conver events to seconds using the sampling rate.
  Double_t up  = (1.0*max)/event_rate;
  Double_t low = (1.0*min)/event_rate;

  TH1D *h2_1   = new TH1D("h2_1","Distributions",events+1,min-0.5,max+0.5);    

  
  Int_t k =1;
  Int_t j= 0;
  canvas -> Divide(4,4);

  //************************************* Do FFT on hallC bpms
  if(devicelist.Contains("hc")){
    for(Int_t i=0;i<nbpms;i++){

      canvas -> cd(2*k);
      gStyle->SetOptStat(0);
      gPad->SetLogy();
      gPad->SetLogx();
      TString device = Form("%s%s",bpms[i].Data(),useaxis.Data());      
      std::cout<<"Get FFT of "<<device<<std::endl;
      get_fft(run_number, tree, device, events, up, low, cut, event_rate); 

      canvas -> cd(2*k-1);
      gPad->SetLogy();
      gStyle->SetOptStat(1);
      // Get the base signal distribution.
      tree->Draw(Form("%s.hw_sum>>htemp",device.Data()),
		 Form("mps_counter>%d && mps_counter<%d && %s.Device_Error_Code == 0",
		      min,max,device.Data()));
      h2_1 = (TH1D*) gDirectory -> Get("htemp");
      if(h2_1 == NULL){
	std::cout<<"Unable to plot "<<Form("%s.hw_sum>>htemp",device.Data())<<std::endl;
	exit(1);
      }
      
      h2_1 -> SetTitle(Form("Distribution of %s in %i",device.Data(), run_number));
      h2_1 -> GetXaxis() -> SetTitle("Position (mm)");
      h2_1 -> GetXaxis() -> SetTitleSize(0.05);
      h2_1 -> GetYaxis() -> SetTitle("Events");
      h2_1 -> SetName(device);
      h2_1 -> SetTitle("");
      h2_1->DrawCopy();
      delete h2_1;

      canvas -> Modified();
      canvas -> Update();
      k++;
      if(k==9){
	k=1;
	j++;
	canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
      }

    };
     //Clear out the rest of the pads
     for(Int_t i =15; i<17; i++){
       canvas->cd(i);
       gPad->Clear();
     }
  }


  //***********************************  Do FFT on injector bpms
   if(devicelist.Contains("inj")){
     for(Int_t i=0;i<injbpms;i++){

       canvas -> cd(2*k);
       gPad->SetLogy();
       gPad->SetLogx();
       TString device = Form("%s%s",injbpm[i].Data(),useaxis.Data());
       std::cout<<"Get FFT of "<<device<<std::endl;
       get_fft(run_number, tree, device, events, up, low, cut, event_rate); 

       canvas ->cd(2*k-1);
       gPad->SetLogy();
       if(h2_1 != NULL) h2_1 ->DrawCopy();
       else {
	 std::cout<<"Unable to plot signal "<<device<<std::endl;
	 exit(1);
       }       
       canvas -> Modified();
       canvas -> Update();
       k++;
       if(k==9){
	 k=1;
	 j++;
	 canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
       }
     };
     //Clear out the rest of the pads
     for(Int_t i =11; i<17; i++){
       canvas->cd(i);
       gPad->Clear();
     }
   }

   // Print the last page of the canvas.
   std::cout<<j<<std::endl;
   j++;
   canvas -> Print(Form("%d_FFT_of_%s_%i.gif",run_number,devicelist.Data(),j));
  
   return;
}


void get_fft(Int_t runnumber, TChain* tree, TString device, Int_t events, Double_t up, Double_t low,
	     TString error_code, Double_t event_rate)
{
  
  TProfile *profile1 = new TProfile("profile1","Signal Profile1",events+1,low-0.5,up+0.5);
  TProfile *profile2 = new TProfile("profile2","Signal Profile2",events+1,low-0.5,up+0.5);
  TProfile *profile3 = new TProfile("profile3","Signal Profile3",events+1,low-0.5,up+0.5);
  TProfile *profile4 = new TProfile("profile4","Signal Profile4",events+1,low-0.5,up+0.5);
 
  TH1D *htemp0 = new TH1D("htemp0","htemp0",events+1,low-0.5,up+0.5);
  TH1D *htemp1 = new TH1D("htemp1","htemp1",events+1,low-0.5,up+0.5);
  TH1D *htemp2 = new TH1D("htemp2","htemp2",events+1,low-0.5,up+0.5);
  TH1D *htemp3 = new TH1D("htemp3","htemp3",events+1,low-0.5,up+0.5);


  //  TString scut = Form("time>%f && time<%f && %s.Device_Error_Code == 0",low,up,device.Data());
  TString scut = Form("time>%f && time<%f && %s",low,up,error_code.Data());
  //  Double_t elcut =  low * event_rate/(1.0*min);
  //  Double_t eucut =  up * event_rate/(1.0*max);


  // Here I am using the individual sub_blocks instead of the hw_sum because I want to use theoversampling feature of VQWKS
  // to increase the detectable frequency range by 4 times. 
  // e.g. with oversampling we can detect upto 960/2 *4 Hz = 1860Hz of frequency at 960Hz running.
  // This is still not working .. sigh..

  TString amplitude = Form("%s.bclock0",device.Data());
  tree->SetAlias("amplitude",amplitude);

   // Now plot the signal
  tree->Draw("amplitude:time>>profile1",scut,"prof");
  profile1 = (TProfile*) gDirectory -> Get("profile1");
  if(profile1 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile1 -> DrawCopy();
  Double_t m = profile1->GetMean(2);
  htemp0 = (TH1D*)profile1->ProjectionX("Signal block0");
  //htemp0->DrawCopy();
  delete profile1;

  amplitude = Form("%s.bclock1",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile2",scut,"");
  profile2 = (TProfile*) gDirectory -> Get("profile2");
  if(profile2 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile2 -> DrawCopy();
  m+=profile2->GetMean(2);
  htemp1 = (TH1D*)profile2->ProjectionX("Signal block1");
  //  htemp1->DrawCopy();
  delete profile2;

  amplitude = Form("%s.bclock2",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile3",scut,"");
  profile3 = (TProfile*) gDirectory -> Get("profile3");
  if(profile3 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile3 -> DrawCopy();
  m+=profile3->GetMean(2);
  htemp2 = (TH1D*)profile3->ProjectionX("Signal block2");
  //htemp2->DrawCopy();
  delete profile3;
      
  amplitude = Form("%s.bclock3",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile4",scut,"");
  profile4 = (TProfile*) gDirectory -> Get("profile4");
  if(profile4 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile4 -> DrawCopy();
  m+=profile4->GetMean(2);
  htemp3 = (TH1D*)profile4->ProjectionX("Signal block3");
  //htemp3->DrawCopy();
  delete profile4;
 
  htemp0->Add(htemp1,1);
  htemp0->Add(htemp2,1);
  htemp0->Add(htemp3,1);
  htemp0->Scale(1.0/4);
  htemp0->Draw();
  

  
  std::cout<<" --- Average signal ="<<m/4<<"\n";
      
  
  //Remove the DC/zero frequency component 
  Double_t setvalue;
  
  TH1D * h2 = new TH1D("h2","h2",events+1,low-0.5,up+0.5);
  //  TH1D * h2_1 = new TH1D("h2_1","h2_1",events+1,low-0.5,up+0.5);

  TAxis *xa = htemp0 -> GetXaxis();
  Int_t nbins =  xa->GetNbins();
  
  for(Int_t n = -1;n < nbins;n++){
  
    setvalue = (htemp0->GetBinContent(n+1)-m/4);
    //   if((htemp0->GetBinContent(n+1))!= 0)
    //       h2_1->SetBinContent(n+1,setvalue);
//     if((htemp0->GetBinContent(n+1))!= 0)
//       h2_1->SetBinContent(n+1,(htemp0->GetBinContent(n+1)));
    h2->SetBinContent(n+1,setvalue);
  }
//   h2_1 -> SetTitle(Form("Signal of %s in %i",device.Data(), runnumber));
//   h2_1 -> GetXaxis() -> SetTitle("Time (s)");
//   h2_1 -> GetXaxis() -> SetTitleSize(0.05);
//   h2_1 -> GetYaxis() -> SetTitle("signal (V)");
//   h2_1->SetLineColor(46);
//   h2_1->Draw();

  
  // //Get the magnitude of the fourier transform
  TH1 * fftmag = NULL;
  TVirtualFFT::SetTransform(0);
  fftmag = ((TH1*)h2)->FFT(fftmag,"MAG");
  //  fftmag -> SetTitle(Form("FFT of %s in %i",device.Data(), runnumber));
  fftmag -> Draw();
  
  // rescale the x axis of the transform to get only the positive frequencies. 
  // Due to the periodicity of FFT in number of samples the FFT output will be a mirrored image.
  
  xa = fftmag -> GetXaxis();
  nbins =  xa->GetNbins();
  
  Double_t frequency = event_rate; //consider the oversampling using the 4 sub_blocks 
  Double_t scale = 1.0/sqrt(events);
  
  TH1D *h3 = new TH1D("h3",Form("FFT of %s in run %i",device.Data(), runnumber) ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    h3->SetBinContent(n+1,scale*fftmag->GetBinContent(n+1)); 
  }
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetXaxis() -> SetTitleSize(0.05);
  h3 -> GetYaxis() -> SetTitle("Amplitude");
  h3->Draw();
  
  xa = h3 -> GetXaxis();
  xa -> SetRangeUser((xa->GetBinLowEdge(1)), xa->GetBinUpEdge(xa->GetNbins()/3.0));
  h3 -> GetYaxis() -> SetRangeUser(0.00001,10000);
  h3->SetLineColor(1);
  h3 -> DrawCopy();

  delete htemp0;  
  delete htemp1;
  delete htemp2;
  delete htemp3;
  delete h2;
  delete h3;
  delete fftmag;
}
