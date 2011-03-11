/**************************************************************
 Author : B. Waidyawansa
 Date   : October 19, 2010
***************************************************************/
/*
This script creates the exe GetBPMWeights which is able to extract the 
weights of individual bpms.
A weight of a bpm is defined as the inverse of the square of its resolution. 
i.e. 1/sigma^2 of its signal distribution. 

To use this exe type
./get_bpm_weights run_number

 */


#include <TRandom.h>
#include <TGraphErrors.h>
#include <TStyle.h>
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
#include <TPad.h>
#include <TChain.h>
#include <cstdlib>

#include "LeastSquaresFit.cc"

Int_t run_number = 0;
const Int_t nbpms = 5; 

Double_t xmean[nbpms];
Double_t ymean[nbpms];
Double_t qmean[nbpms];

/*Bpms in front of the target*/
TString devicelist[nbpms] = 
  {"3h07a","3h07b","3h07c","3h09","3h09b"};

TString property[3] = 
  {"X", "Y", "_EffectiveCharge"};

Double_t zpos[nbpms] = {138406,139363,140329,144803,147351}; //in mm


void GetFitResults(Int_t device1, Int_t device2, Double_t fitresults[], Double_t y[]);
Double_t GetWeight(TChain * tree, TString bpm, Double_t zlocation, Double_t fit[], 
		Double_t weight);
void GetAllWeights(TChain * tree, TString fitbpm, Int_t bpm1, Int_t bpm2,
		   Double_t zpos, Double_t weights[]);


/*********************************************
  Main function
********************************************/

int main(Int_t argc,Char_t* argv[])
{

  TApplication theApp("App",&argc,argv);
 //stats
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1); 

  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetTitleSize(0.08);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.25);
  gStyle->SetPadLeftMargin(0.08);  
  gStyle->SetPadRightMargin(0.08);  

  // histo parameters
  gStyle->SetTitleYOffset(1);
  gStyle->SetTitleXOffset(0.08);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(18);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");

  /*Read the commandline arguments to get runnumber*/
  if(argc<1 || argc>2){
    std::cout<<"Please enter a run number"<<std::endl;
    exit(1);
  }
  else if (argc == 2)
    run_number = atoi(argv[1]);
  std::cout<<"Extracting bpm weights...."<<std::endl;

  /*Open a file to store the output*/
  std::ofstream file;
  TString filename = Form("%i_BPM_wieghts.txt",run_number);
  file.open(filename);
  file<<"!  BPM weights extracted from run "<<run_number<<std::endl;

  time_t theTime;
  time(&theTime);
  file<<"!  Date of analysis "<<ctime(&theTime)<<std::endl;
  file<<"!  device\tX weight\tY weight\tQ weight"<<std::endl;
  std::cout<<"Results will be stored in file "<<filename<<std::endl;



  /*Open the rootfile/rootfiles*/
  TChain * tree      = new TChain("Mps_Tree");
  TString  Rfilename = Form("Qweak_%i.000.root", run_number);
  TFile  * Rfile     = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

  if (Rfile->IsZombie()) {
    std::cout << "Error opening root file chain "<< Rfilename << std::endl;
    Rfilename = Form("Qweak_%i.root", run_number);
    std::cout << "Try to open chain " << Rfilename << std::endl;
    Rfile = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

    if (Rfile->IsZombie()) {
      std::cout << "Error opening root file chain "<< Rfilename << std::endl;
      Rfilename = Form("QwPass1_%i.000.root", run_number);
      std::cout << "Try to open chain " << Rfilename << std::endl;
      Rfile = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));
      
      if (Rfile->IsZombie()){
	std::cout << "File exit failure."<<std::endl; 
	tree = NULL;
      }
      else
	tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root", run_number));
    }
    else
      tree->Add(Form("$QW_ROOTFILES/Qweak_%i.root", run_number));
  }
  else
    tree->Add(Form("$QW_ROOTFILES/Qweak_%i.*.root", run_number));

  std::cout<<"Opened rootfile/s "<<Rfilename<<std::endl;

  if(tree == NULL){
    std::cout<< "Unable to find the Mps_Tree." << std::endl; 
    exit(1);
  }


  /*First get the mean X and Y positions*/

  TString plotcommand;
  TString scut;

  TCanvas* c = new TCanvas("c","Distributions of X,Y and effective charge,",1000,1000);
  c->Draw();
  c->Divide(3,5);

  for(Int_t i=0;i<nbpms;i++){
    std::cout<<"Onto plotting "<<devicelist[i]<<std::endl;
    c->cd(3*i+1);
    gPad->SetLogy();
    plotcommand = Form("qwk_bpm%sX.hw_sum>>htemp1", devicelist[i].Data());
    scut = Form("qwk_bpm%sX.Device_Error_Code == 0 && ErrorFlag == 0",devicelist[i].Data());
    tree->Draw(plotcommand,scut);
    TH1D * histox=(TH1D*)gDirectory->Get("htemp1");   
    xmean[i]=histox->GetMean();
    histox->SetTitle(Form("bpm%sX",devicelist[i].Data()));
    histox->DrawCopy();
    delete histox;

    c->cd(3*i+2);
    gPad->SetLogy();
    plotcommand = Form("qwk_bpm%sY.hw_sum>>htemp2", devicelist[i].Data());
    scut = Form("qwk_bpm%sY.Device_Error_Code == 0 && ErrorFlag == 0",devicelist[i].Data());
    tree->Draw(plotcommand,scut);
    TH1D *histoy=(TH1D*)gDirectory->Get("htemp2");   
    histoy->Draw();
    ymean[i]=histoy->GetMean();
    histoy->SetTitle(Form("bpm%sY",devicelist[i].Data()));
    histoy->DrawCopy();
    delete histoy;


    c->cd(3*i+3);
    gPad->SetLogy();
    plotcommand = Form("qwk_bpm%s_EffectiveCharge.hw_sum>>htemp3", devicelist[i].Data());
    scut = Form("qwk_bpm%s_EffectiveCharge.Device_Error_Code == 0 && ErrorFlag == 0",devicelist[i].Data());
    tree->Draw(plotcommand,scut);
    TH1D * histoq=(TH1D*)gDirectory->Get("htemp3");   
    histoq->Draw();
    qmean[i]=histoq->GetMean();
    histoq->SetTitle(Form("bpm%sQ",devicelist[i].Data()));
    histoq->DrawCopy();
    delete histoq;


  }
  c->Update();
  c->Print(Form("%i_bpm_distributions.gif",run_number));

  /*Get the fits for different bpm combinations*/
  Double_t weights[3];

  for(Int_t i = 0;i<3;i++) weights[i] = 0.0;

  /*Fit 3h07b with 3h07a and 3h07c*/
  GetAllWeights(tree, "3h09b", 1, 3, zpos[1], weights);
  file << devicelist[1] <<",\t\t"
       << weights[0]    <<",\t\t"
       << weights[1]    <<",\t\t"
       << weights[2]    << std::endl; 
 


 /*Fit 3h07a with 3h07b and 3h07c*/
  GetAllWeights(tree, "3h07a", 1, 3, zpos[0], weights);
  file << devicelist[0] <<",\t\t"
       << weights[0]    <<",\t\t"
       << weights[1]    <<",\t\t"
       << weights[2]    << std::endl; 

  for(Int_t i = 0;i<3;i++) weights[i] = 0.0;


 /*Fit 3h07c with 3h07b and 3h09*/
  GetAllWeights(tree, "3h07c", 2, 4, zpos[2], weights);
  file << devicelist[2] <<",\t\t"
       << weights[0]    <<",\t\t"
       << weights[1]    <<",\t\t"
       << weights[2]    << std::endl; 

  for(Int_t i = 0;i<3;i++) weights[i] = 0.0;


 /*Fit 3h09 with 3h07c and 3h09b*/
  GetAllWeights(tree, "3h09", 2, 5, zpos[3], weights);  
  file << devicelist[3] <<",\t\t"
       << weights[0]    <<",\t\t"
       << weights[1]    <<",\t\t"
       << weights[2]    << std::endl; 

  for(Int_t i = 0;i<3;i++) weights[i] = 0.0;

  
 /*Fit 3h09b with 3h07c and 3h09*/
  GetAllWeights(tree, "3h09b", 2, 4, zpos[4], weights);  
  file << devicelist[4] <<",\t\t"
       << weights[0]    <<",\t\t"
       << weights[1]    <<",\t\t"
       << weights[2]    << std::endl; 

  for(Int_t i = 0;i<3;i++) weights[i] = 0.0;

  std::cout<<" Done!"<<std::endl;
  theApp.Run();

};
 
 
/*function to get the rms of distribution of the difference of signals*/
Double_t GetWeight(TChain * tree, TString bpm, Double_t zlocation, Double_t fit[]){

  Double_t rms = 0;
  Double_t weight = 0;

  TString plotcommand;
  TString scut;

  plotcommand = Form("(%e*%e+%e)-qwk_bpm%s.hw_sum>>htemp",
		     fit[0],zlocation,fit[1],bpm.Data());
  
  scut = Form("qwk_bpm%s.Device_Error_Code == 0 && ErrorFlag == 0",bpm.Data());
  tree->Draw(plotcommand, scut, "goff");
  
  TH1 * h = (TH1*)(gDirectory->Get("htemp"));
  if(!h){
    std::cout<<"Unable to plot "<<plotcommand<<std::endl;
    exit(1);
  }
  
  rms = h->GetRMS();
  
  weight = 1.0/(rms*rms);
  
  delete h;
  std::cout<<"RMS of fit residual of "<<bpm<<" is "<<rms<<std::endl;
  std::cout<<"weight of "<<bpm<<" is "<<weight<<std::endl;

  //std::cout<<"Fit residual was obtained from: \n"<<plotcommand<<std::endl;
  return weight;
 
};


/*Function to get the fit parameters*/
void GetFitResults(Int_t device1, Int_t device2, Double_t fitresults[], Double_t y[]){
  Double_t X[2];
  Double_t Y[2];
  Double_t W[2]={1.0,1.0};

  X[0]=zpos[device1-1];
  X[1]=zpos[device2-1];
  Y[0]=y[device1-1];
  Y[1]=y[device2-1];

  DoLinearLeastSquareFit( 2, X, Y, W, fitresults);

}

void GetAllWeights(TChain * tree, TString fitbpm, Int_t bpm1, Int_t bpm2, 
		   Double_t zpos, Double_t weights[])
{
  Double_t fitresults[4];
  TString bpm;

  for(Int_t i = 0;i<4;i++) fitresults[i] = 0.0;

  // Get weights for X
  bpm = Form("%sX",fitbpm.Data());
  GetFitResults(bpm1, bpm2, fitresults, xmean);
  weights[0] = GetWeight(tree, bpm, zpos, fitresults);

  // Get weights for Y
  bpm = Form("%sY",fitbpm.Data());
  GetFitResults(bpm1, bpm2, fitresults, ymean);
  weights[1] = GetWeight(tree, bpm, zpos, fitresults);

  // Get weights for effective charge
  bpm = Form("%s_EffectiveCharge",fitbpm.Data());
  GetFitResults(bpm1, bpm2, fitresults, qmean);
  weights[2] = GetWeight(tree, bpm, zpos, fitresults);
  

}
