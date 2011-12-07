//**********************************************************************************************//
// Author : B. Waidyawansa
// Date   : April 19th, 2010
//**********************************************************************************************//
// This macro plots;
//      a). The variation of the beam position in X and Y direction inside hall C 
//          starting from IPM3H02 (first bpm after the last quadropple magnet (MQE3M03).
//      b). If selected, the harp readings and the fit residuals of the harps based on bpm readings.
//           This allows us to use residual from the actual harp reading and the fitted values from 
//           the bpms to set the beam position on the target correctly.
//
// To use this , compile hallc_beam_motion.cc. This should create hallc_beam_motion.exe
// Then do 
//         ./hallc_beam_motion run_numberber lcut ucut
//
// You will be asked the option to input harp readings.
//          Input Harp readings? (Y/N) 
//          y
//          Enter X reading of iha3h07
//          1
//          Enter X reading of iha3h07a
//          2
//          ....
//
// If Y is selected the harp readings should be entered for the relevent harp in micro meters.
// This will then generate two canvasses containing a fit of the bpm readings projected on to the 
// target and another canvas containing the fit residuals of this fit and the harp readings.
//
// If N is selected only the bpm readings and the fit are displayed in the canvas.
// 
// NOTE : Since we are using the mean values of the bpms to do this projection, the projected values will be
// different from what we get using the combined bpm "qwk_target". qwk_target values are projected event by event.
// But as you can see, the values from the qwk_target fall within the error of the fit done here. 
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


const size_t nbpms  = 5;
const size_t nharps = 4;


// Hall C bpms after the last quad.
TString devicelist[nbpms]=
  {"bpm3h07a","bpm3h07b","bpm3h07c","bpm3h09","bpm3h09b"};

// bpm weights obtained using run 5832.
Double_t weightsX[nbpms]={420.43, 532.87, 371.82, 313.04, 337.29};
Double_t weightsY[nbpms]={324.53, 408.29, 308.33, 278.71, 330.10};

// bpm locations from the Qweak target to upstream in meters( from Nuruzzamans doccument on position and 
// angle determination on target).These are the actual device positions in millimeters.
// Qweak target is located at position 148.702. This was used as the position of the combined bpm
Double_t z_pos[nbpms]= {138406,139363,140329,144803,147351};

Double_t tgtpos   = 148739;
Double_t wplugpos = 148239; 

// Hall c harps
TString harps[4] = {"iha3h07", "iha3h07a", "iha3h09", "iha3h09a"};

// Harp positions were obtained usng a BPM doccument by Dave Mack that contains a bpm info latice.
// These positions were matched with those of the bpms to get these values.
// The units are milimeters
Double_t harp_pos[4] = {137412, 139400, 144548, 147097};


// Readings from the harps in milimeters.
Double_t harp_readingsX[nharps];
Double_t harp_readingsY[nharps];


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

      while(!done){

	// First request to input harp readings.
	std::cout<<"Input Harp readings? (Y/N)"<<std::endl;
	std::cin >> option;
	if(strcmp(option, "y") == 0){
	  read_harps("X",harps,harp_readingsX);
	  read_harps("Y",harps,harp_readingsY);
	  do_harps = kTRUE;
	  done = kTRUE;
	}
	else if (strcmp(option, "n") == 0){
	  do_harps = kFALSE;
	  done = kTRUE;
	}
	else{
	  std::cerr<<"Please enter Y/N."<<std::endl;
	}

// 	// Next request for peojection option.
// 	std::cout<<"Project to target (enter 0) or tungsten plug (enter 1)?"<<std::endl;
// 	std::cin >> project;
// 	if(project == 0 || project == 1){
// 	  done = kTRUE;
// 	}
// 	else {
// 	  std::cerr<<"Please enter 1 or 0 for correction option."<<std::endl;
// 	}
      }
    }


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

  

  // Create a canvas and divide it in to pads
  TString title = Form("%i : X & Y Beam Position Across Hall C Beamline",run_number);
  TCanvas *canvas = new TCanvas("canvas",title,1200,1000);

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(45);
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
  fakez[0]    = 0.0;
  fakeerr[0]  = 0.0;
  fakeerrz[0] = 0.0;    
  
  
  // define TGraphErrors 
  TGraphErrors * XVariation;
  TGraphErrors * YVariation; 
  TGraphErrors * tgtX;
  TGraphErrors * tgtY;
  TGraphErrors * projX;
  TGraphErrors * projY;

  
  
  //Primitive("Graph");

  pad2->cd(2);
  plot_data(tree, "Y", devicelist,z_pos,meany);
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
  DoLinearLeastSquareFit(nbpms,z_pos, meanx, weightsX, fit_results);
  TF1 *fitx = new TF1("fitx","[0]*x+[1]",z_pos[0],tgtpos); 
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
  DoLinearLeastSquareFit(nbpms,z_pos, meany, weightsY, fit_results);
  TF1 *fity = new TF1("fity","[0]*x+[1]",z_pos[0],tgtpos); 
  fity->SetParameter(0,fit_results[0]);
  fity->SetParameter(1,fit_results[1]);
  fity->SetLineWidth(2);
  fity->SetLineColor(49);
  gPad->Update();

  p0[1]  = fit_results[1]; // get the offset
  ep0[1] = fit_results[3]; // get the error on the offset
  p1[1]  = fit_results[0]; // get the slope
  ep1[1] = fit_results[2]; // get the error on the slope

 
  //  Now add the beam position on target that is given by the combined bpm. 
  //  Beam on target X 
  pad2->cd(1);
  TH1F * hx = NULL;
  tree->Draw(Form("qwk_targetX.hw_sum>>htemp","qwk_targetX.Device_Error_Code ==0 && mps_counter>%i && mps_counter < %i",lcut, ucut));
  hx = (TH1F*)gDirectory->Get("htemp");
  if(!hx){
    std::cout<<"Unable to plot qwk_targetX."<<std::endl;
    exit(1);
  }
  //  Now create arrays of a single element to put in to the graph to plot the target.
  //  There is noway to plot a graph with a single point that is not an array.
  //  This is what I can think of right now..

  fakepos[0] = hx->GetMean();
  fakez[0]   = tgtpos;
  fakeerr[0] = hx->GetMeanError();
  delete hx;

  tgtX = new TGraphErrors(1, fakez ,fakepos, fakeerrz,fakeerr);
  tgtX->SetMarkerColor(8);
  tgtX->SetMarkerStyle(20);
  tgtX->SetMarkerSize(2);

  // Target X from fit.
  projectedX[0] =  p1[0]*tgtpos +  p0[0];
  // Used error propergation to get the error equation assuming there was no error on z measurement.
  projectedXErr[0] = sqrt(pow((tgtpos*ep1[0]),2) + pow(ep0[0],2));
  projX = new TGraphErrors(1, fakez , projectedX, fakeerrz, projectedXErr);
  projX->SetMarkerColor(4);
  projX->SetMarkerStyle(20);
  projX->SetMarkerSize(2);


  //  Beam on target Y 
  TH1F * hy = NULL;
  tree->Draw(Form("qwk_targetY.hw_sum>>htemp","qwk_targetY.Device_Error_Code ==0  && mps_counter>%i && mps_counter < %i",lcut, ucut));
  hy = (TH1F*)gDirectory->Get("htemp");
  if(!hy){
    std::cout<<"Unable to plot qwk_targetY."<<std::endl;
    exit(1);
  }
 
  fakepos[0] = hy->GetMean();
  fakez[0]   = tgtpos;
  fakeerr[0] = hy->GetMeanError();
  delete hy;

  tgtY = new TGraphErrors(1, fakez ,fakepos, fakeerrz,fakeerr);
  tgtY->SetMarkerColor(8);
  tgtY->SetMarkerStyle(20);
  tgtY->SetMarkerSize(2);

  TLegend *legend = new TLegend(0.93,0.25,0.99,0.7,"","brNDC");
  legend->AddEntry(YVariation, "bpms", "p");
  legend->AddEntry(tgtY, "target", "p");
  legend->AddEntry(projX, "projected", "p");

  // Target Y from fit.
  projectedY[0] =  p1[1]*tgtpos +  p0[1];
  // Used error propergation to get the error equation assuming there was no error on z measurement.
  projectedYErr[0] = sqrt(pow((tgtpos*ep1[1]),2) + pow(ep0[1],2));
  projY = new TGraphErrors(1, fakez , projectedY, fakeerrz, projectedYErr);
  projY->SetMarkerColor(4);
  projY->SetMarkerStyle(20);
  projY->SetMarkerSize(2);
  


  // If harps needs to be added,
  if (do_harps){ 
    // Add the harp readings to the variation plots 
    TGraph * harpX = new TGraph(4, harp_pos, harp_readingsX);
    TGraph * harpY = new TGraph(4, harp_pos, harp_readingsY);

    harpX->SetMarkerColor(4);
    harpX->SetMarkerStyle(21);
    mgx->Add(harpX);
    
    harpY->SetMarkerColor(4);
    harpY->SetMarkerStyle(21);
    mgy->Add(harpY);
 
    legend->AddEntry(harpX, "harps", "p");
    
    // Draw the fit residuals and save that canvas on to a .gif file
    plot_residuals();
  }

  // Set axis and draw graphs
  pad2->cd(1);
  gPad->Clear();
  gPad->SetFillColor(18);
  mgx->Add(XVariation);
  mgx->Add(tgtX);
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
    t.DrawText(z_pos[i],yl,devicelist[i]);
  }
  t.DrawText(tgtpos,yl,"target");
  if(do_harps){
    for (size_t i=0;i<nharps;i++) {
      t.DrawText(harp_pos[i],yl,harps[i]);
    }  
  }
  fitx->Draw("same");
  canvas->Update();

  legend->Draw("");
  pad2->cd(2);
  gPad->Clear();
  gPad->SetFillColor(18);
  mgy->Add(YVariation);
  mgy->Add(tgtY);
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
    t.DrawText(z_pos[i],yl,devicelist[i]);
  }
  t.DrawText(tgtpos,yl,"target");
  if(do_harps){
    for (size_t i=0;i<nharps;i++) {
      t.DrawText(harp_pos[i],yl,harps[i]);
    }  
  }

  legend->Draw("");
  fity->Draw("same");
  canvas->Update();


  // Save the canvas on to a .gif file
  TString impodva = Form("%i_hallC_beam_motion.gif",run_number);
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
    error[k] = h->GetMeanError();    
    // error on beam z position is fixed by bpm positions
    errorz[k] = 0.0;
    delete h;
    }

  TGraphErrors* v = new TGraphErrors(nbpms, zpos ,means, errorz, error);
  v->Draw("AEP");
}



/******************************************************************************
 *  function to get the harp readings from the user
 *****************************************************************************/
void read_harps(TString pos, TString names[],Double_t readings[])
{

  Double_t readval;
  for(size_t i = 0;i<nharps;i++){
    readings[i] = 0.0;
    readval = 0.0;
    std::cout<<"Enter "<<pos<<" reading of "<<names[i]<<std::endl;
    std::cin >> readval;  
    readings[i] = readval;
  }
}


/******************************************************************************
 *  function to plot the residuals of the harp readings and harp projections
 *****************************************************************************/

void plot_residuals()
{

  // Arrays needed to put the harp fit results
  Double_t harp_predictX[nharps];
  Double_t harp_predictY[nharps];

  Double_t resx[nharps];
  Double_t resy[nharps];
  Double_t w[nharps];
  Double_t fitx[4];
  Double_t fity[4];


  // Plot the X and Y residuals for the harp readings
  TString title1 = Form("%i : X & Y fit residuals of the harps",run_number);
  TCanvas *canvas1 = new TCanvas("canvas1",title1,1200,1000);

  TPad*pad11 = new TPad("pad11","pad11",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(45);
  pad11->Draw();
  pad22->Draw();

  pad11->cd();
  TText*t11 = new TText(0.22,0.3,title1);
  t11->SetTextSize(0.7);
  t11->Draw();

  pad22->cd();
  pad22->Divide(1,2);
  pad22->SetFillColor(45);

  // Initialize arrays
  for(size_t i=0;i<nharps; i++){
    harp_predictY[i]=0.0;
    harp_predictX[i]=0.0;
    resx[i] = 0.0;
    resy[i] = 0.0;
    w[i] = 1.0;
    fitx[i]=0.0;
    fity[i]=0.0;

  }

   
  //  Now predict the harp readings using the bpm fit results.
  //  Remember the z axis and y axis units are microm.

  for(size_t i=0; i<nharps; i++){
    harp_predictX[i] = p1[0]*harp_pos[i] + p0[0];
    harp_predictY[i] = p1[1]*harp_pos[i] + p0[1];
    resx[i] = TMath::Abs( harp_readingsX[i] - harp_predictX[i]);
    resy[i] = TMath::Abs( harp_readingsY[i] - harp_predictY[i]);
  }

  // Draw and fit the residual graphs
  TGraph* XResidual = new TGraph(nharps, harp_pos ,resx);
  pad22->cd(1);
  XResidual->Draw("AP");
  XResidual ->SetName("XResidual");
  XResidual->SetMarkerColor(2);
  XResidual->SetMarkerStyle(20);
  XResidual->SetMarkerSize(2);

  DoLinearLeastSquareFit(nharps,harp_pos, resx, w, fitx);
  XResidual->Fit("pol1");
  gPad->Modified();
  gPad->Update();


  TGraph* YResidual = new TGraph(nharps, harp_pos ,resy);
  pad22->cd(2);
  YResidual->Draw("AP");
  YResidual ->SetName("YResidual");
  YResidual->SetMarkerColor(2);
  YResidual->SetMarkerStyle(20);
  YResidual->SetMarkerSize(2);

  DoLinearLeastSquareFit(nharps,harp_pos, resy, w, fity);
  YResidual->Fit("pol1");
  gPad->Modified();
  gPad->Update();




  // Set Labels and markers 
  pad22->cd(1);
  gPad->SetRightMargin(0.15);
  gPad->SetFillColor(18);
  XResidual->SetTitle("Harp X Residual");
  XResidual->GetYaxis()->SetTitle("( Harp X Reading - Fit results )(mm)");

  TAxis *axis = XResidual->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  Double_t ypos =  XResidual->GetHistogram()->GetMaximum()-XResidual->GetHistogram()->GetMinimum();
  
  Double_t ylabel = XResidual->GetHistogram()->GetMinimum() - 0.01*ypos;
  TText tlabel;
  tlabel.SetTextSize(0.04);
  tlabel.SetTextAlign(31);
  tlabel.SetTextAngle(90);
  for (size_t i=0;i<nharps;i++) {
    tlabel.DrawText(harp_pos[i],ylabel,harps[i]);
  }  
  canvas1->Update();


  pad22->cd(2);
  gPad->SetRightMargin(0.15);
  gPad->SetFillColor(18);
  YResidual->SetTitle("Harp Y Residual");
  YResidual->GetYaxis()->SetTitle("( Harp Y Reading - Fit results )(mm)");

  axis = YResidual->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  ypos =  YResidual->GetHistogram()->GetMaximum()-YResidual->GetHistogram()->GetMinimum();
  ylabel = YResidual->GetHistogram()->GetMinimum() - 0.01*ypos;
  tlabel.SetTextSize(0.04);
  tlabel.SetTextAlign(31);
  tlabel.SetTextAngle(90);
  for (size_t i=0;i<nharps;i++) {
    tlabel.DrawText(harp_pos[i],ylabel,harps[i]);
  }  
  canvas1->Update();

  // Save the canvas on to a .gif file  
  TString print = Form("%i_X_Y_Harp_Residuals.gif",run_number);
  canvas1->Print(print.Data()); 
}
