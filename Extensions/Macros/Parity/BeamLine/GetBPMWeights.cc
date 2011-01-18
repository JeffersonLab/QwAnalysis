/**************************************************************
 Author : B. Waidyawansa
 Date   : October 19, 2010
***************************************************************/
/*
This script creates the exe GetBPMWeights which is able to extract the weights of individual bpms. A weight of a bpm is defined as the inverse of the square of its resolution. i.e. 1/sigma^2 of its signal distribution. 

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

Int_t run_number = 0;
const Int_t nbpms = 6; 
Double_t weightsx[3]  = {0.0, 0.0, 0.0};
Double_t weightsy[3]  = {0.0, 0.0, 0.0};
Double_t weightsq[3]  = {0.0, 0.0, 0.0};


/*Device list*/
TString devicelist[nbpms] = 
  {"3h04","3h07b","3h07c","3h07c","3h09","3h09b"};
TString property[3] = 
  {"X", "Y", "_EffectiveCharge"};

Double_t GetDifferenceRMS(TChain * tree, 
			  TString bpm1, TString bpm2, 
			  TString property);

void GetDeviceWeights(TChain * tree, TString property, 
		      TString bpm1, TString bpm2, TString bpm3, 
		      Double_t weights[]);

/*********************************************
  Main function
********************************************/

int main(Int_t argc,Char_t* argv[])
{

  TApplication theApp("App",&argc,argv);

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
	tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.000.root", run_number));
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

  /* loop through the device list 3 by 3*/
  for(Int_t i=0; i<2; i++){
    GetDeviceWeights(tree, 
		     property[0], 
		     devicelist[i*3], 
		     devicelist[i*3+1], 
		     devicelist[i*3+2],  
		     weightsx);
  
    GetDeviceWeights(tree,
		     property[1], 
		     devicelist[i*3], 
		     devicelist[i*3+1], 
		     devicelist[i*3+2], 
		     weightsy);
    
    GetDeviceWeights(tree, 
		     property[2], 
		     devicelist[i*3], 
		     devicelist[i*3+1], 
		     devicelist[i*3+2],  
		     weightsq);
    
    for(Int_t j=0; j<3; j++){
      file << devicelist[i*3+j] <<"\t\t"
	   << weightsx[j]       <<"\t\t"
	   << weightsy[j]       <<"\t\t"
	   << weightsq[j]       << std::endl; 
      
    }
    
  }
  
  std::cout<<" Done!"<<std::endl;
  // theApp.Run();

};
 
 
/*function to get the rms of distribution of the difference of signals*/
Double_t GetDifferenceRMS(TChain * tree, TString bpm1, TString bpm2, TString property){

  TH1* h = NULL;  
  Double_t rms = 0;
  // TCanvas * canvas = new TCanvas("canvas","difference distributions",1200,1000);
  // canvas->Draw();

  TString plotcommand = Form("(qwk_bpm%s%s.hw_sum-qwk_bpm%s%s.hw_sum)>>htemp",
			     bpm1.Data(),property.Data(),bpm2.Data(),property.Data());

  TString scut = Form("qwk_bpm%s%s.Device_Error_Code == 0 && qwk_bpm%s%s.Device_Error_Code == 0 && mps_counter< 600e+3",bpm1.Data(),property.Data(),bpm2.Data(),property.Data());
  tree->Draw(plotcommand, scut);
  h = (TH1*)(gDirectory->Get("htemp"));
  if(!h){
    std::cout<<"Unable to plot "<<plotcommand<<std::endl;
    exit(1);
  }
  h->Draw();
  rms = h->GetRMS();
  delete h;
  std::cout<<"Get difference of "<<bpm1<<property<<" and "<<bpm2<<property<<".  RMS of that is "<<rms<<std::endl;
  return (rms); 
};


/*Function to get individual weights*/
void GetDeviceWeights(TChain * tree, TString property, 
		      TString bpm1, TString bpm2, TString bpm3,
		      Double_t weights[]){
  
  /*
    diff_rms[0] = rms_(1-2)
    diff_rms[1] = rms_(2-3)
    diff_rms[2] = rms_(3-1)
  */
  
  Double_t diff_rms[3] = {0.0, 0.0, 0.0};
  Double_t rms_square[3] = {0.0, 0.0, 0.0}; 

  diff_rms[0] = GetDifferenceRMS(tree, bpm1, bpm2, property);
  diff_rms[1] = GetDifferenceRMS(tree, bpm2, bpm3, property);
  diff_rms[2] = GetDifferenceRMS(tree, bpm3, bpm1, property);
  
  rms_square[0] = (  pow(diff_rms[0],2) - pow(diff_rms[1],2) + pow(diff_rms[2],2))/2.0;
  rms_square[1] = (  pow(diff_rms[0],2) + pow(diff_rms[1],2) - pow(diff_rms[2],2))/2.0;
  rms_square[2] = ( (-1)*pow(diff_rms[0],2) + pow(diff_rms[1],2) + pow(diff_rms[2],2))/2.0;

  std::cout<<"rms_square[0] = "<<rms_square[0]<<std::endl;
  std::cout<<"rms_square[1] = "<<rms_square[1]<<std::endl;
  std::cout<<"rms_square[2] = "<<rms_square[2]<<std::endl;

  
  for(Int_t i=0;i<3;i++)
    weights[i] = 1.0/rms_square[i];
  
}
