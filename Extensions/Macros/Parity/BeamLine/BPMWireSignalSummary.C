//********************************************************************//
// Author : B. Waidyawansa
// Date   : January 25th, 2011
//********************************************************************//
//
// This macro reads the mean and sigma from bpm wires and stores the hw_sum in to a text file.



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

void PrintBPMSignalSummary(Int_t runnum)
{
    
  const Int_t ndevices = 23;
  
  //**************** List of devices in the injector
  TString devicelist1[ndevices]=
    {
      "bpm3c07","bpm3c07a","bpm3c08","bpm3c11","bpm3c12","bpm3c14","bpm3c16","bpm3c17",
      "bpm3c18","bpm3c19","bpm3p02a","bpm3p02b","bpm3p03a","bpm3c20","bpm3c21","bpm3h02",
      "bpm3h04","bpm3h07a","bpm3h07b","bpm3h07c","bpm3h08","bpm3h09","bpm3h09b"
    };
  

  TString pos[2] = {"X","Y"};

  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetOptStat(0); 


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
  TString directory="$QW_ROOTFILES"; // the location of the rootfile used for calibration
  Char_t filename[300];
  sprintf(filename,"%s/first100k_%i.root",directory.Data(),runnum);
  TFile * f = new TFile(filename);
  if(!f->IsOpen())
    exit(1);
  std::cout<<"Obtaining data from: "<<filename<<"\n";
  if(!f) exit(1);

  TTree* nt = (TTree*)f->Get("Mps_Tree"); //load the Tree

  

  TString plotcommand;
  TString cut;
  Double_t mean;
  Double_t sigma;

  std::ofstream Myfile;    
  Char_t  textfile[400];
  sprintf(textfile,"%i_mean_error_rms_of_bpms.txt",runnum); 
  Myfile.open(textfile);
  Myfile << " name  \t\t mean \t\t sigma "<<std::endl;

  
  
  for(Int_t i=0; i<ndevices; i++)    
    {  
      for(size_t j=0;j<2;j++){
	plotcommand = Form("qwk_%s%sP.hw_sum>>htemp1",devicelist1[i].Data(),pos[j].Data());
	cut = Form("qwk_%s%sP.Device_Error_Code == 0 && ErrorFlag ==0",devicelist1[i].Data(),pos[j].Data());
	nt->Draw(plotcommand,cut,"goff"); 
	  
	TH1 * h1 = (TH1D*)gDirectory->Get("htemp1");
	if(h1 == NULL) exit(1);

	mean = h1->GetMean();
	sigma = h1->GetRMS();
	Myfile <<devicelist1[i].Data()<<pos[j].Data()<<"P\t\t"
	       << mean <<"\t\t"
	       << sigma <<std::endl;
	
	std::cout<<devicelist1[i].Data()<<pos[j].Data()<<"P\t\t"
		 << mean <<"\t\t"
		 << sigma <<std::endl;
	delete h1;

	plotcommand = Form("qwk_%s%sM.hw_sum>>htemp2",devicelist1[i].Data(),pos[j].Data());
	cut = Form("qwk_%s%sM.Device_Error_Code == 0 && ErrorFlag == 0",devicelist1[i].Data(),pos[j].Data());
	nt->Draw(plotcommand,cut,"goff"); 
	  
	TH1 * h2 = (TH1D*)gDirectory->Get("htemp2");
	if(h2 == NULL) exit(1);

	mean = h2->GetMean();
	sigma = h2->GetRMS();

	Myfile <<devicelist1[i].Data()<<pos[j].Data()<<"M\t\t"
	       << mean <<"\t\t"
	       << sigma <<std::endl;
	
	std::cout<<devicelist1[i].Data()<<pos[j].Data()<<"M\t\t"
		 << mean <<"\t\t"
		 << sigma <<std::endl;
	delete h2;

      }
  }
  Myfile.close();
  std::cout<<"Done!\n";
};
  



