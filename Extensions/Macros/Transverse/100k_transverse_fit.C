//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : February 4th, 2011
//*****************************************************************************************************//
//
//
//   This macro will plot the PMT POS, PMT NEG, BAR SUM, US LUMI and DS LUMI asymmetry variation across the octantsusing the first 100K rootfile
//   and fit them with a sin fit of the form [0]Sin(phi+[1]) + [2] which is displayed in the left pannel in the canvas and 
//   a constant fit which is displayed in the right side pannel of the canvas.
//   The canvas is saved as ****_Transverse_fit.gif
//   e.g. use
//   ./transverse_fit 9690 
//
//   To compile this code do a gmake.
//
//   
//*****************************************************************************************************//
// 01-13-2012 B.Waidyawansa : Renamed the script from TransverseFit.cc to 100k_transverse_fit.C
//                            Changed the fit to [0]cos(phi)-[1]sin(phi)+[2]
//

using namespace std;


#include <vector>
#include <iomanip>
#include <TRandom.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TString.h>
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
#include <TObject.h>

TString directory = "$QW_ROOTFILES";

//  For now instead of the main detector PMTs I will use 8 bpm wires.
//  Later these should be replaced by the actual ones.

// Left PMTs
TString quartz_bar_POS[8]=
  {"qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos","qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};

// Right PMTs
TString quartz_bar_NEG[8]=
  {"qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg","qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg"};

// barsum
TString quartz_bar_SUM[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum","qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

// us lumi sum
TString us_lumi[4]=
  {"uslumi1_sum","uslumi3_sum","uslumi5_sum","uslumi7_sum"};

// ds lumi sum
TString ds_lumi[8]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8"};

TPad * pad1, *pad2;
TText *t1;
TTree *tree;
TString CanvasTitle;
TString bpm,bpmdata,var1;
Int_t runnum;

Int_t run;
Int_t opt =1;
TTree * nt;

std::ofstream Myfile;
Char_t textfile[100];


Double_t p0[8];
Double_t ep0[8];
Double_t p1[8];
Int_t i = -1;
Double_t ep1[8];
Double_t value1[8];
Double_t err1[8];
Double_t value2[8];
Double_t err2[8];
Double_t value3[8];
Double_t err3[8];

TString xunit, yunit, slopeunit;
void get_octant_data(TTree *heltree, Int_t run_number, TString devicelist[], Int_t opt,Double_t value[], Double_t error[]);
void plot_octant(Double_t values[],Double_t errors[]);  


int main(Int_t argc,Char_t* argv[])
{


  TFile *f;
  Char_t filename[200]; 
  
  if(argc == 2){
    run   = atoi(argv[1]);     // runnumber
  }
  else if((argc < 2)||(argc > 2)){
      std::cout<<argc<<std::endl;
      std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
      std::cerr<<"./correlation_plots run number \n";
      exit(1);
  }
  std::cout<<"Fitting with  "<<std::endl;
  std::cout<<"Left. pannel Av*cos(phi)-Ah*sin(phi)+const  fit"<<std::endl;
  std::cout<<"Right pannel constant fit "<<std::endl;
  // std::cin>>opt;
 
  
  std::cout<<"Fitting octants of MD PMT-, PMT+, barsum, US lumi, DS Lumi asymmetries"<<std::endl;
  TApplication theApp("App",&argc,argv);

 
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.5);

  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.3);
  gStyle->SetPadLeftMargin(0.08);

  
  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");


  TString title1 = Form("%i : MD Transverse asymmetry fits (left cos fit, right constant fit)",run);
  TString title2 = Form("%i : LUMI Transverse asymmetry fits (left cos fit, right constant fit)",run);  
  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,1200,800);  
  TCanvas * Canvas2 = new TCanvas("canvas2", title2,0,0,1200,800);  

  Canvas1->Draw();
  Canvas2->Draw();

  Canvas1->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title1);
  TText*t1 = new TText(0.23,0.3,text);
  t1->SetTextSize(0.6);
  t1->Draw();

  pad2->cd();
  pad2->Divide(2,3);
  pad2->SetFillColor(20);


  //Get the root file
  sprintf(filename,"%s/first100k_%d.root",directory.Data(),run);
    
  f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  if (f->IsZombie())
    exit(1);

  std::cout<<"Opened "<<filename<<"\n";

 
  //load the Hel_Tree. It has data based on quartets analysis.
  tree = (TTree*)(f->Get("Hel_Tree")); 
  if(tree==NULL) {
    std::cout<<"Unable to find the helicity tree!\n"<<std::endl;
    exit(1);
  }
  

  // xyfit
  TF1 *xyfit = new TF1("xyfit","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1)))+[2]",1,8);
  xyfit->SetParameter(0,0);
  xyfit->SetParameter(1,0);
  xyfit->SetParameter(2,0);
  xyfit->SetParName(0,"A_V");
  xyfit->SetParName(1,"A_H");
  xyfit->SetParName(2,"constant");

  
 //  // cos fit (if we want we can switch to this)
//   TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
//   cosfit->SetParameter(0,0);


  // Draw MD asymmetries

  TGraphErrors * gr;
  TString gtitle;

  pad2->cd(1);
  get_octant_data(tree, run, quartz_bar_POS,opt, value1,err1);
  plot_octant(value1,err1);
  gr=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr->Fit("xyfit");
  TF1* fit = gr->GetFunction("xyfit");
  fit->DrawCopy("same");
  gtitle = Form("PMT POS Asymmetry Fit :  A_{V}*cos(#phi)-A_{H}*sin(#phi)+const");
  gr->SetTitle(gtitle);

  pad2->cd(3);
  get_octant_data(tree, run, quartz_bar_NEG,opt, value2,err2);
  plot_octant(value2,err2);
  gr=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr->Fit("xyfit");
  fit = gr->GetFunction("xyfit");
  fit->DrawCopy("same");
  gtitle = Form("PMT NEG Asymmetry Fit : A_{V}*cos(#phi)-A_{H}*sin(#phi)+const");
  gr->SetTitle(gtitle);


  pad2->cd(5);
  get_octant_data(tree, run, quartz_bar_SUM,opt, value3,err3);
  plot_octant(value3,err3);
  gr=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr->Fit("xyfit");
  fit = gr->GetFunction("xyfit");
  fit->DrawCopy("same");
  gtitle = Form("BAR SUM Asymmetry Fit : A_{V}*cos(#phi)-A_{H}*sin(#phi)+const");
  gr->SetTitle(gtitle);

  pad2->cd(2);
  plot_octant(value1,err1);
  gr=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr->Fit("pol0");
  fit = gr->GetFunction("pol0");
  fit->DrawCopy("same");
  gtitle = Form("PMT POS Asymmetry Fit :  p0");
  gr->SetTitle(gtitle);

  pad2->cd(4);
  plot_octant(value2,err2);
  gr=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr->Fit("pol0");
  fit = gr->GetFunction("pol0");
  fit->DrawCopy("same");
  gtitle = Form("PMT NEG Asymmetry Fit :  p0");
  gr->SetTitle(gtitle);

  pad2->cd(6);
  plot_octant(value3,err3);
  gr=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr->Fit("pol0");
  fit = gr->GetFunction("pol0");
  fit->DrawCopy("same");
  gtitle = Form("BAR SUM Asymmetry Fit :  p0");
  gr->SetTitle(gtitle);


  Canvas1->Print(Form("100k_%i_MD_Transverse_fit_plots.gif",run));


  // Draw Lumi asymmetries

  TGraphErrors * gr1;
  TString gtitle1;

  Canvas2->cd();
  TPad*pad11 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(20);
  pad11->Draw();
  pad22->Draw();


  pad11->cd();
  TString text1 = Form(title2);
  TText*t11 = new TText(0.23,0.3,text1);
  t11->SetTextSize(0.6);
  t11->Draw();

  pad22->cd();
  pad22->Divide(2,2);
  pad22->SetFillColor(20);

  pad22->cd(1);
  get_octant_data(tree, run, us_lumi,opt, value1,err1);
  plot_octant(value1,err1);
  gr1=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr1->Fit("xyfit");
  TF1* fit1 = gr1->GetFunction("xyfit");
  fit1->DrawCopy("same");
  gtitle1 = Form("US lumi sum Asymmetry Fit : A_{V}*cos(#phi)-A_{H}*sin(#phi)+const");
  gr1->SetTitle(gtitle1);

  pad22->cd(3);
  get_octant_data(tree, run, ds_lumi,opt, value2,err2);
  plot_octant(value2,err2);
  gr1=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr1->Fit("xyfit");
  fit1 = gr1->GetFunction("xyfit");
  fit1->DrawCopy("same");
  gtitle1 = Form(" DS Lumi Asymmetry Fit : A_{V}*cos(#phi)-A_{H}*sin(#phi)+const");
  gr1->SetTitle(gtitle1);

  pad22->cd(2);
  plot_octant(value1,err1);
  gr1=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr1->Fit("pol0");
  fit1 = gr1->GetFunction("pol0");
  fit1->DrawCopy("same");
  gtitle1 = Form("US Lumi Asymmetry Fit :  p0");
  gr1->SetTitle(gtitle1);

  pad22->cd(4);
  plot_octant(value2,err2);
  gr1=(TGraphErrors*)gPad->GetPrimitive("Graph");
  gr1->Fit("pol0");
  fit1 = gr->GetFunction("pol0");
  fit1->DrawCopy("same");
  gtitle1 = Form("DS Lumi Asymmetry Fit :  p0");
  gr1->SetTitle(gtitle1);


  Canvas2->Print(Form("100k_%i_Lumi_Transverse_fit_plots.gif",run));


  std::cout<<"Done plotting fits \n";

  theApp.Run();
  return(1);

};



//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(TTree *heltree, Int_t run_number, TString devicelist[], Int_t opt, Double_t value[], Double_t error[])
{
  Bool_t ldebug =kFALSE;
  runnum = run_number;
  nt = heltree;
  TString cut;
 
  for(Int_t i=1 ; i<9 ;i++){
    //event cut 
    cut = Form("asym_%s.Device_Error_Code == 0 && ErrorFlag == 0",
	       devicelist[i-1].Data());
    TString ytitle =Form("1e6*%s",devicelist[i-1].Data()); //plot in ppm	
    
    // Plotcommand
    TString plotc = Form("asym_%s.hw_sum*1e6>>htemp",devicelist[i-1].Data()); 
    nt->Draw(plotc,cut,"goff");
    
    TH1D * h = (TH1D*)gDirectory->Get("htemp");
    if(h == NULL) exit(1);
    value[i-1] = h->GetMean();
    error[i-1] = h->GetMeanError();
    if(ldebug)  std::cout<<"value = "<<value[i-1]<<"+-"<<error[i-1]<<std::endl;
    delete h;
   
    
  }
}

//***************************************************
//***************************************************
//         Plor octant graph                   
//***************************************************
//***************************************************

void plot_octant(Double_t values[],Double_t errors[])  
{
  std::cout<<" Plotting graph "<<std::endl;
  Bool_t ldebug =kFALSE;

  Double_t x[8];
  Double_t errx[8];
  
  for(Int_t i =0;i<8;i++){
    x[i] = i+1;
    errx[i] = 0.0;
    if(ldebug)  std::cout<<"value = "<<values[i]<<"+-"<< errors[i]<<std::endl;
  }
  
  TGraphErrors * gr = new TGraphErrors(8,x,values,errx,errors); 
  gr->SetTitle("");
  gr->SetMarkerColor(4);
  gr->SetLineColor(kRed);
  gr->SetMarkerStyle(21);
  gr->Draw("AP");
  gr->GetXaxis()->SetTitle("Octant");
  gr->GetXaxis()->CenterTitle();
  gr->GetYaxis()->SetTitle("Asymmetry (ppm)");
  gr->GetYaxis()->CenterTitle();
  gr->GetYaxis()->SetTitleOffset(0.7);
  gr->GetXaxis()->SetTitleOffset(0.8);

}



//**************************************************************
//**************************************************************
//         Plot the variaiton of the fit parameters                   
//**************************************************************
//**************************************************************


void fit_parameter_variation(TString device_list[], TString name)  
{ 

  //Create a canvas
  TString ctitle =Form("%i_%s_vs_%s_fit_parameters",runnum,name.Data(), bpmdata.Data());
  TCanvas * c= new TCanvas(ctitle, ctitle,0,0,1200,800);  

  pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  pad2 = new TPad("pad2","pad2",0.01,0.01,0.99,0.94);
  pad1->SetFillColor(24);
  pad1->Draw();
  pad2->Draw();
  
  pad1->cd();
  TString text = Form("%i :  %s vs %s fit parameter variation",runnum,name.Data(),bpmdata.Data());
  t1 = new TText(0.1,0.4,text);
  t1->SetTextSize(0.7);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(1,2);
  gPad->SetFillColor(20);


  // Create dummy errors and values for x axis and error x to set labels with
  Double_t ex[]  = {0,0,0,0,0,0,0,0};
  Double_t x[]  = {1,2,3,4,5,6,7,8};


  // Plot the variation of the slope (p1)
  pad2->cd(1);
  gPad->SetGrid();
  TGraphErrors *gr1 = new TGraphErrors(8,x,p1,ex,ep1);
  gr1->SetTitle("Variaiton of the slope of the correlation fits");
  gr1->SetMarkerColor(4);
  gr1->SetMarkerStyle(21);
  gr1->GetXaxis()->SetTitle("Detector");
  gr1->GetYaxis()->SetTitle(Form("slope (%s/%s)",yunit.Data(),xunit.Data()));
  gr1->GetXaxis()->CenterTitle();
  gr1->GetYaxis()->CenterTitle();
  gr1->Draw("ACP");



  // Draw device labels on the X axis
  TText *t = new TText();
  Double_t offset1 = p1[i]/10;
  t->SetTextAlign(52);
  t->SetTextSize(0.035);
  t->SetTextFont(72);
  t->SetTextAngle(60);
  for (Int_t j=0;j<8;j++) 
      t->DrawText(x[j]+0.1,p1[j]+offset1,device_list[j]);
  


  // Plot the variation of the offset (p0)
  pad2->cd(2);
  gPad->SetGrid();
  TGraphErrors *gr2 = new TGraphErrors(8,x,p0,ex,ep0);
  gr2->SetTitle("Variaiton of the offsets of the correlation fits");
  gr2->SetMarkerColor(4);
  gr2->SetMarkerStyle(21);
  gr2->GetXaxis()->SetTitle("Octant");
  gr2->GetYaxis()->SetTitle(Form("offset (%s)",yunit.Data()));
  gr2->GetXaxis()->CenterTitle();
  gr2->GetYaxis()->CenterTitle();
  gr2->Draw("ACP");


  // Draw device labels on the X axis
  TText *t1 = new TText();
  Double_t offset2 = p1[i]/10;
  t1->SetTextAlign(52);
  t1->SetTextSize(0.035);
  t1->SetTextFont(72);
  t1->SetTextAngle(60);

  for (Int_t j=0;j<8;j++) 
      t1->DrawText(x[j]+0.1,p0[j]+offset2,device_list[j]);


  // Print the canvas on to a file
  c->Print(ctitle+".gif");
}



