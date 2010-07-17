//********************************************************************//
// Author : B. Waidyawansa
// Date   : April 1st, 2010
//********************************************************************//
//
// This macro plots correlations between any two device parameters in HEL Tree. 
//  To use this macro, open root and type 
// .L correlations.cc 
//  correlation( run number, varA, varB)
//
//          run number - number of the run
//          var A - variable 1. Need to be specified accurately. e.g. asym_qwk_bcm0l02.hw_sum
//          var B - Need to be specified accurately. e.g, yield_qwk_1i02RelX.hw_sum.
//
// for e.g. if you want to plot the correlation between the bpm 1i04 RelX and bcm yield for runn number 1672
// use,
// root [0] correlations(1672, "yield_qwk_1i04RelX","yield_qwk_bcm0l02")
//
// If the macro crash, first check to see if the HEL tree in the root file you are using is correctly filled. 


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
#include <TH2F.h>
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
#include <TObject.h>


TString Directory = "~/scratch/rootfiles/";
TCanvas *C;
TTree *ntree;
Char_t fname[200];
TString d1, d2;
TString cutL;
TString plotc;
TString cut;
TH2F * ht;

void plot(TString title, TTree * ntree, Int_t rnum, TString device1, TString device2 )
{
  
  TGraph * gr;

  //event cut 
  cut = Form("%s.Device_Error_Code == 0 && %s.Device_Error_Code ==0",device1.Data(),device2.Data() );
 
  
  // Rescale asymmetries to apprea in parts per millions 
  if(device1.Contains("asym")) 
    d1 = Form("1000000*%s",device1.Data()); //plot in ppm 
  else d1 = device1;
  if(device2.Contains("asym")) 
    d2 = Form("1000000*%s",device2.Data()); // plot in ppm
  else d2 = device2;


  // Plotcommand
  plotc = Form("%s.hw_sum:%s.hw_sum",d1.Data(),d2.Data()); 
  ntree->Draw(plotc,cut);
  gPad->Update();

  // Get the graph. I am not using TProfile here because there is something wrong with the way it is fitting data
  // which results in a very large chi square value that I can't explain.


  gr=(TGraph*)(gPad->GetPrimitive("Graph"));
  TH2F* ht = (TH2F*)(gPad->GetPrimitive("htemp"));

  if(ht == NULL)
    {
      std::cerr<<" Attempt to plot NULL parameters : "<<plotc<<std::endl;
      std::cerr<<" Check the HEL Tree."<<std::endl;
      std::cerr<<" Exiting program."<<std::endl;
      exit(1);
    }

  gr->SetMarkerColor(2);
  gr->Draw("P");
  gr->Fit("pol1","W");
  gr->SetName("h");
   

  //Set axis titles and units
  if(device1.Contains("asym")) 
    ht->GetYaxis()->SetTitle(Form("%s (ppm)",device1.Data()));  //y axis in ppb. no units 
  else if ((device1.Contains("Rel")))
    ht->GetYaxis()->SetTitle(Form("%s (#mum)",device1.Data())); // positions. 
  else 
    ht->GetYaxis()->SetTitle(Form("%s (#muA)",device1.Data())); // currents
  

  if(device2.Contains("asym")) 
    ht->GetXaxis()->SetTitle(Form("%s (ppm)",device2.Data()));  //x axis units in ppb
  else if (device2.Contains("RelX")) 
    ht->GetXaxis()->SetTitle(Form("%s (#mum)",device2.Data())); //positions
  else
    ht->GetXaxis()->SetTitle(Form("%s (#muA)",device2.Data())); // currents
  
  ht->SetTitle("");
  gPad->Update();

}


///********************************************
// the main funciton
///********************************************

void correlations( Int_t rnum, TString device1, TString device2)
{

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000010);
  gStyle->SetStatY(0.96);
  gStyle->SetStatX(0.96);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.15); 

 
  //Pad parameters
  gStyle->SetPadColor(1); 
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);

  // histo parameters
  gStyle->SetTitleYOffset(1.8);
  gStyle->SetTitleXOffset(1.8);

  //Create a canvas
  TString CTitle=Form("%i_%s-vs-%s_correlation",rnum,device2.Data(), device1.Data());
  C= new TCanvas(CTitle, CTitle,0,0,1200,800);  

  TPad *pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  TPad *pad2 = new TPad("pad2","pad2",0.01,0.01,0.99,0.94);
  pad1->SetFillColor(24);
  pad1->Draw();
  pad2->Draw();
  
  pad1->cd();
  TString text = Form("%i :  %s  vs %s ",rnum,device1.Data(), device2.Data());
  TText*t1 = new TText(0.1,0.4,text);
  t1->SetTextSize(0.7);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(1,2);
  gPad->SetFillColor(20);

  //Get the root file
  sprintf(fname,"%sQweak_BeamLine_%d.000.root",Directory.Data(),rnum);
  TFile* rf = new TFile(fname);
  if(!rf->IsOpen())
    {
      std::cerr<<"Cannot locate file "<<fname<<std::endl;
      exit(1);
    }
  else
    std::cout<<"Obtaining data from: "<<fname<<"\n";


  //load the HEL_Tree. It has data based on quartets analysis
  ntree = (TTree*)rf->Get("HEL_Tree"); 

  //plot the correlations
  plot(CTitle,ntree,rnum, device1, device2);


  // Print the bpm calibration polt on to gif file
  C->Print(CTitle+".png");
  std::cout<<" done with calibration \n";

};



