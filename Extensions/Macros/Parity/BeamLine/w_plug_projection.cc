//**********************************************************************************************//
// Author : B. Waidyawansa
// Date   : Novemeber 12th, 2010
//**********************************************************************************************//
//
// This script creates an exe that projects the beam X and Y location on the tungsten plug using bpms 3H07C and 3H09B.
// (we can use any number of bpms we want later, but for now this is what we are going to use)
// It prints out the projected X and Y values and creates a plot that is saved as a .gif file.
//
// To use the exe, you have to pass a run number and if you want an upper and lowere event cut but it is not necessary.
//
// e.g., 
// ./project_to_Wplug 5832 100 1000 
//
//********************************************
// Main function
//********************************************


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

#include "LeastSquaresFit.cc"

Int_t    run_number   = 0;
Int_t    lcut = 0;
Int_t    ucut = 0;
TString  bcm      = ""; 
TString  current  = "";
TString  tempname = "";
Char_t   filename[300];
TString  directory="$QW_ROOTFILES/"; // the location of the rootfile used for calibration
Char_t   option[4]= {""};
Int_t    project = 0; // default beam projection is to target
Bool_t   do_harps = kFALSE;
Bool_t   done = kFALSE;


const size_t nbpms  = 2;
const size_t nharps = 4;


// Hall C bpms after the last quad.
TString devicelist[5]=
  {"bpm3h07a","bpm3h07b","bpm3h07c","bpm3h09","bpm3h09b"};

// bpm weights obtained using run 5832.
Double_t weightsX[5]={420.43, 532.87, 371.82, 313.04, 337.29};
Double_t weightsY[5]={324.53, 408.29, 308.33, 278.71, 330.10};

// bpm locations from the Qweak target to upstream in meters( from Nuruzzamans doccument on position and 
// angle determination on target).These are the actual device positions in millimeters.
// Qweak target is located at position 148.702. This was used as the position of the combined bpm
Double_t z_pos[5]= {138406,139363,140329,144803,147351};


// For now we are only going to use 3h07c and 3h09 to project to the wplug.
TString  bpms[2]   = {"bpm3h07c","bpm3h09"};
Double_t bpmsz[2]  = {140329,144803};
Double_t bpmswx[2] = {371.82, 313.04};
Double_t bpmswy[2] = {308.33, 278.71};


Double_t tgtpos   = 148739;
Double_t wplugpos = 148239; 



// Fit parameters
Double_t p0[2], p1[2], ep0[2], ep1[2];

// array to put fit reuslts
Double_t fit_results[4];
Double_t meanx[nbpms];
Double_t meany[nbpms];

// Arrays needed to plot the target bpm position
Double_t fakez[1];
Double_t fakepos[1];
Double_t fakeerr[1];
Double_t fakeerrz[1];
Double_t projectedX[1];
Double_t projectedXErr[1];
Double_t projectedY[1];
Double_t projectedYErr[1];

void plot_data(TChain * t, TString axis, TString device[], Double_t zpos[], Double_t means[] );
void read_harps(TString pos, TString names[], Double_t readings[]);
void plot_residuals();


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

  gROOT->ForceStyle();


  TApplication theApp("App",&argc,argv);
 
  // Read in the arguments.
  // The first argument should be the program name itself.
  // The second argument should be the run number


  if(argc<2)
    {
      std::cerr<<"Please enter a run number"<<std::endl;
      exit(1);
    }
  else if(argc == 3)
    {
      std::cerr<<"Please enter an upper event cut. Can't use only a lower cut!"<<std::endl;
      exit(1);
    }
  else if((argc == 2)||(argc == 4))
    {
      if(argc==2)
	run_number = atoi(argv[1]);
      if(argc == 4){
	run_number = atoi(argv[1]);
	lcut = atoi(argv[2]);
	ucut = atoi(argv[3]);
      }
    }
  

  /*Open the rootfile/rootfiles*/
  TChain * tree      = new TChain("Mps_Tree");
  TString  Rfilename = Form("first100K_%i.root", run_number);
  TFile  * Rfile     = new TFile(Form("~/scratch/rootfiles/%s", Rfilename.Data()));

  if (Rfile->IsZombie()) {
    std::cout << "Error opening root file chain "<< Rfilename << std::endl;
    Rfilename = Form("QwPass1_%i.000.root", run_number);
    std::cout << "Try to open chain " << Rfilename << std::endl;
    Rfile = new TFile(Form("~/scratch/rootfiles/%s", Rfilename.Data()));

    if (Rfile->IsZombie()) {
      std::cout << "Error opening root file chain "<< Rfilename << std::endl;
      Rfilename = Form("Qweak_%i.000.root", run_number);
      std::cout << "Try to open chain " << Rfilename << std::endl;
      Rfile = new TFile(Form("~/scratch/rootfiles/%s", Rfilename.Data()));
      
      if (Rfile->IsZombie()){
	std::cout << "File exit failure."<<std::endl; 
	tree = NULL;
      }
      else
	tree->Add(Form("~/scratch/rootfiles/Qweak_%i.000.root", run_number));
    }
    else
      tree->Add(Form("~/scratch/rootfiles/QwPass1_%i.000.root", run_number));
  }
  else
    tree->Add(Form("~/scratch/rootfiles/first100K_%i.root", run_number));
  
  std::cout<<"Opened rootfile/s "<<Rfilename<<std::endl;

  if(tree == NULL){
    std::cout<< "Unable to find the Mps_Tree." << std::endl; 
    exit(1);
  }

  

  // Create a canvas and divide it in to pads
  TString title = Form("%i : X & Y Beam Projection on to Tungsten Plug",run_number);
  TCanvas *canvas = new TCanvas("canvas",title,1200,1000);

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(kBlue-8);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.25,0.3,text);
  t1->SetTextSize(0.6);
  t1->Draw();
  

  pad2->cd();
  pad2->Divide(1,2);
  pad2->SetFillColor(45);


  // Initialize arrays used for the calculations to avoid errors.  
  for(size_t i  = 0; i<2; i++){
    p0[i]    = 0.0;
    p1[i]    = 0.0;
    ep0[i]   = 0.0;
    ep1[i]   = 0.0;
  }
  
  
  fakepos[0]  = 0.0;
  fakez[0]    = wplugpos;
  fakeerr[0]  = 0.0;
  fakeerrz[0] = 0.0;    
  
  
  // define TGraphErrors 
  TGraphErrors * XVariation;
  TGraphErrors * YVariation;; 
  TGraphErrors * projX;
  TGraphErrors * projY;

 // Plot histograms
  pad2->cd(1); 
  plot_data(tree, "X", bpms,bpmsz,meanx);
  XVariation = (TGraphErrors*)gPad->GetPrimitive("Graph");

  pad2->cd(2);
  plot_data(tree, "Y", bpms,bpmsz,meany);
  YVariation = (TGraphErrors*)gPad->GetPrimitive("Graph");

  // use multigraphs to overlay the graphs
  TMultiGraph *mgx = new TMultiGraph();
  TMultiGraph *mgy = new TMultiGraph();

  // Now fit the data on X and Y variaiton to predict the harp readings
  pad2->cd(1);
  XVariation->SetMarkerColor(2);
  XVariation->SetMarkerStyle(20);
  XVariation->SetMarkerSize(1.2);

  // Use my fit function to fit the graphs. Why am I using this? See comment in 
  // PlotResiduals() function. ; )
  DoLinearLeastSquareFit(nbpms,bpmsz, meanx, bpmswx, fit_results);
  TF1 *fitx = new TF1("fitx","[0]*x+[1]",bpmsz[0],wplugpos); 
  fitx->SetParameter(0,fit_results[0]);
  fitx->SetParameter(1,fit_results[1]);
  fitx->SetLineWidth(2);
  fitx->SetLineColor(49);
  gPad->Update();

  p0[0]  = fit_results[1]; // get the offset
  ep0[0] = fit_results[3]; // get the error on the offset
  p1[0]  = fit_results[0]; // get the slope
  ep1[0] = fit_results[2]; // get the error on the slope

  pad2->cd(2);
  YVariation->SetMarkerColor(2);
  YVariation->SetMarkerStyle(20);
  YVariation->SetMarkerSize(1.2);
  DoLinearLeastSquareFit(nbpms,bpmsz, meany, bpmswy, fit_results);
  TF1 *fity = new TF1("fity","[0]*x+[1]",bpmsz[0],wplugpos); 
  fity->SetParameter(0,fit_results[0]);
  fity->SetParameter(1,fit_results[1]);
  fity->SetLineWidth(2);
  fity->SetLineColor(49);
  gPad->Update();

  p0[1]  = fit_results[1]; // get the offset
  ep0[1] = fit_results[3]; // get the error on the offset
  p1[1]  = fit_results[0]; // get the slope
  ep1[1] = fit_results[2]; // get the error on the slope


  // Beam X on plug from fit.
  projectedX[0] =  p1[0]*wplugpos +  p0[0];
  // Used error propergation to get the error equation assuming there was no error on z measurement.
  projectedXErr[0] = sqrt(pow((wplugpos*ep1[0]),2) + pow(ep0[0],2));

  std::cout<<"\n###############################################\n";
  std::cout<<"Projected beam X position on w-plug = "<<projectedX[0]<<"+-"<< projectedXErr[0]<<std::endl;

  projX = new TGraphErrors(1, fakez , projectedX, fakeerrz, projectedXErr);
  projX->SetMarkerColor(kGreen+2);
  projX->SetMarkerStyle(20);
  projX->SetMarkerSize(2);


  TLegend *legend = new TLegend(0.93,0.25,0.99,0.7,"","brNDC");
  legend->AddEntry(YVariation, "bpms", "p");
  legend->AddEntry(projX, "projected", "p");

  // Beam Y on plug from fit.
  projectedY[0] =  p1[1]*wplugpos +  p0[1];
  // Used error propergation to get the error equation assuming there was no error on z measurement.
  projectedYErr[0] = sqrt(pow((wplugpos*ep1[1]),2) + pow(ep0[1],2));

  std::cout<<"Projected beam Y position on w-plug = "<<projectedY[0]<<"+-"<< projectedYErr[0]<<std::endl;
  std::cout<<"###############################################\n";

  projY = new TGraphErrors(1, fakez , projectedY, fakeerrz, projectedYErr);
  projY->SetMarkerColor(kGreen+2);
  projY->SetMarkerStyle(20);
  projY->SetMarkerSize(2);
  

  // Set axis and draw graphs
  pad2->cd(1);
  gPad->Clear();
  gPad->SetFillColor(18);
  mgx->Add(XVariation);
  mgx->Add(projX);
  mgx->SetTitle("X Position");
  mgx->Draw("AEP");
  mgx->GetYaxis()->SetTitle("Beam Position (mm)");
  
  TAxis *axis = mgx->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);

  Double_t ypad = gPad->GetUymax() - gPad->GetUymin();
  Double_t yl = gPad->GetUymin() - 0.01*ypad;
  TText t;
  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);

  for (size_t i=0;i<nbpms;i++){
    t.DrawText(bpmsz[i],yl,bpms[i]);
  }
  t.DrawText(wplugpos,yl,"w-plug");

  fitx->Draw("same");
  canvas->Update();

  legend->Draw("");
  pad2->cd(2);
  gPad->Clear();
  gPad->SetFillColor(18);
  mgy->Add(YVariation);
  //  mgy->Add(wplugY);
  mgy->Add(projY);
  mgy->SetTitle("Y Position");
  mgy->Draw("AEP");
  mgy->GetYaxis()->SetTitle("Beam Position (mm)");

  axis = mgy->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  ypad = gPad->GetUymax() - gPad->GetUymin();
  yl = gPad->GetUymin() - 0.01*ypad;
  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);

  for (size_t i=0;i<nbpms;i++){
    t.DrawText(bpmsz[i],yl,bpms[i]);
  }
  t.DrawText(wplugpos,yl,"w-plug");

  legend->Draw("");
  fity->Draw("same");
  canvas->Update();


  // Save the canvas on to a .gif file
  TString impodva = Form("%i_w-plug_projection.gif",run_number);
  canvas->Print(impodva.Data());  

 
  std::cout<<"Done plotting! "<<std::endl;
  theApp.Run();
  return(0);

};

/**************************************************
 *  function to plot the data from the rootfile
 **************************************************/

void plot_data(TChain * t, TString axis,  TString device[], Double_t zpos[], Double_t means[] )
{
  TH1F * h      = NULL;
  TString hname = "";
  TString cut = "";
  double error[nbpms];
  double errorz[nbpms];

  // initialize the arrays used for calculations
  for(size_t k = 0; k<nbpms; k++){
    means[k] = 0.0;
    error[k] = 0.0;
    errorz[k]=0.0;
  }
  
  for(size_t k = 0; k<nbpms;k++){
    hname = Form("qwk_%s%s.hw_sum>>htemp",device[k].Data(), axis.Data());
    if((lcut !=0) || (ucut != 0))
      cut = Form("qwk_%s%s.Device_Error_Code == 0 && mps_counter>%i && mps_counter < %i",device[k].Data(), axis.Data(),lcut, ucut);
    else
      cut = Form("qwk_%s%s.Device_Error_Code == 0",device[k].Data(), axis.Data());

    t->Draw(hname,cut);
    h = (TH1F*)gDirectory->Get("htemp");
    if(h == NULL) {
      std::cout<<"Cannot get data from "<<hname<<std::endl;
      std::cout<<"Check to see if the histogram exsists."<<std::endl;
      exit(1);
    }
    // beam position in micrometers
    means[k]  = h->GetMean();
    // error on the mean
    error[k]  = h->GetMeanError();    
    // error on beam z position is fixed by bpm positions
    errorz[k] = 0.0;
    delete h;
    }

  TGraphErrors* v = new TGraphErrors(nbpms, zpos ,means, errorz, error);
  v->Draw("AEP");
}



