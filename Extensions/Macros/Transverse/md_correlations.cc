//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : February 4th, 2011
//*****************************************************************************************************//
//
//   This macro will plot the position correlations of DX and DY w.r.t an input BPM.
//
//   ./md_correlation_plots 9690
//
// 1. Position Differences
// 2. Charge ASymmetry
// 3. Energy 
//
//   When prompted. give the required bpm name and X/Y position.
//   To compile this code do a gmake.
//   To use the exe file from command prompt following inputs are needed:
//
//   
//   param1 : run number
//
//*****************************************************************************************************//


#include "correlation_plots.h"

using namespace std;

TString directory = "$QW_ROOTFILES";


//  For now instead of the main detector PMTs I will use 8 bpm wires.
//  Later these should be replaced by the actual ones.

// Left PMTs
TString quartz_bar_POS[8]=
  {"md1pos","md2pos","md3pos","md4pos","md5pos","md6pos","md7pos","md8pos",};

// Right PMTs
TString quartz_bar_NEG[8]=
  {"md1neg","md2neg","md3neg","md4neg","md5neg","md6neg","md7neg","md8neg",};

// barsum
TString quartz_bar_SUM[8]=
  {"md1barsum","md2barsum","md3barsum","md4barsum","md5barsum","md6barsum","md7barsum","md8barsum",};


TCanvas * Canvas1;
TCanvas * Canvas2;
TCanvas * Canvas3;
TPad * pad1, *pad2;
TText *t1;
TChain *tree;
TString CanvasTitle;
TString bpm,bpmdata,var1;
Int_t run_number;

Int_t opt =1;

std::ofstream Myfile;
Char_t textfile[100];


Double_t p0[8];
Double_t ep0[8];
Double_t p1[8];
Int_t i = -1;
Double_t ep1[8];

TString xunit, yunit, slopeunit;

Bool_t FindFiles(TString file_name, TChain * chain);
void plot_correlations(TChain *tree, Int_t run_number, TString device, Int_t opt);


int main(Int_t argc,Char_t* argv[])
{


  TFile *f;
  Char_t filename[200]; 
  
  if(argc == 2)
    {
      run_number   = atoi(argv[1]);     // runnumber
    }
  else if((argc < 2)||(argc > 2))
    {
      std::cout<<argc<<std::endl;
      std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
      std::cerr<<"./correlation_plots run number \n";
      exit(1);
    }
  std::cout<<"Plot correlations of PMT POS, PMT NEG, PMT BAR SUM with "<<std::endl;
  std::cout<<"1. Position Differences"<<std::endl;
  std::cout<<"2. Charge ASymmetry "<<std::endl;
  std::cout<<"3. Energy "<<std::endl;
  std::cin>>opt;
  if(opt == 1){
    std::cout<<"Enter the bpm name. e.g. 3h09X :"<<std::endl;
    std::cin>>bpm;
  }
  else if(opt == 2){
    std::cout<<"Enter the bcm name. e.g. bcm1 :"<<std::endl;
    std::cin>>bpm;
  }
  else
    std::cout<<"Using energy bpm qwk_energy"<<std::endl;
 
  
  std::cout<<"Plotting position correlations of MD PMT-, PMT+, barsum w.r.t "<<bpm<<std::endl;
  TApplication theApp("App",&argc,argv);

 
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.95);
  gStyle->SetStatW(0.28);
  gStyle->SetStatH(0.2);

  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);

  
  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.07);
  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");


  // Create trees
  tree = new TChain("Hel_Tree");


  // Find the rootfiles
  TString file_name = Form("QwPass1_%i.000.root", run_number);
  
  Bool_t found_hel = kFALSE;

  found_hel = FindFiles(file_name, tree);
  
  if(!found_hel){
    file_name = Form("first100k_%i.root", run_number);
    found_hel = FindFiles(file_name, tree);
    if(!found_hel){
      file_name = Form("Qweak_%i.000.root", run_number);
      found_hel = FindFiles(file_name, tree);
      if(!found_hel){
	std::cerr<<"Unable to find rootfile(s) for run "<<run_number<<std::endl;
	exit(1);
      }
    }
  }

  std::cout<<"Obtaining data from: "<<file_name<<"\n";
  plot_correlations(tree,run_number,bpm,opt);

  std::cout<<"Done with plotting correlations \n";
  
  theApp.Run();
  return(1);

};



//***************************************************
//***************************************************
//         plot correlations                   
//***************************************************
//***************************************************

void plot_correlations(TChain *tree, Int_t run_number, TString device, Int_t opt)
  {
    

    //set beam property based on the option selected
    if(opt == 1)
      bpmdata = Form("diff_qwk_bpm%s",bpm.Data());
    else if(opt == 2)
      bpmdata = Form("asym_qwk_%s",bpm.Data());
    else
      bpmdata = "asym_qwk_energy";

    //Create a canvas
    Canvas1 = new TCanvas("canvas1", "",0,0,1200,800);  
    Canvas1->Divide(3,3);
    gPad->SetFillColor(20);
   
    // Plot the correaltions for PMT POS
    main_detector_correlations(quartz_bar_POS,Canvas1); 

    // Set the plot information on the center pad
    Canvas1->cd(5);
    gPad->SetFillColor(24);
    TPaveText *pt = new TPaveText(.05,.05,.95,.95);
    pt->AddText(Form("Run = %i", run_number));
    pt->AddText("Correlations between");
    pt->AddText(Form("PMT POS"));
    pt->AddText("and");
    pt->AddText(Form("%s", bpmdata.Data()));
    pt->Draw();
    
     // Print the canvas on to a  file
    CanvasTitle=Form("correlations_of_pmt_pos");
    Canvas1->Print(CanvasTitle+".gif");    

    // Plot the correaltions for PMT NEG
    Canvas2 = new TCanvas("canvas2", "",0,0,1200,800);  
    Canvas2->Divide(3,3);
    gPad->SetFillColor(20);
 
    main_detector_correlations(quartz_bar_POS,Canvas2); 

     // Set the plot information on the center pad
    Canvas2->cd(5);
    gPad->SetFillColor(24);
    pt = new TPaveText(.05,.05,.95,.95);
    pt->AddText(Form("Run = %i", run_number));
    pt->AddText("Correlations between");
    pt->AddText(Form("PMT NEG"));
    pt->AddText("and");
    pt->AddText(Form("%s", bpmdata.Data()));
    pt->Draw();

    // Print the canvas on to a  file
    CanvasTitle=Form("correlations_of_pmt_neg");
    Canvas2->Print(CanvasTitle+".gif");

    // Plot the correaltions for PMT SUM
    Canvas3 = new TCanvas("canvas3", "",0,0,1200,800);  
    Canvas3->Divide(3,3);
    gPad->SetFillColor(20);
    main_detector_correlations(quartz_bar_SUM,Canvas3); 

   // Set the plot information on the center pad
    Canvas3->cd(5);
    gPad->SetFillColor(24);
    pt = new TPaveText(.05,.05,.95,.95);
    pt->AddText(Form("Run = %i", run_number));
    pt->AddText("Correlations between");
    pt->AddText(Form("BAR sum"));
    pt->AddText("and");
    pt->AddText(Form("%s", bpmdata.Data()));
    pt->Draw();
    // Print the canvas on to a  file
    CanvasTitle=Form("correlations_of_barsum");
    Canvas3->Print(CanvasTitle+".gif");
      
  }

//***************************************************
//***************************************************
//         Main detector correlations                   
//***************************************************
//***************************************************
void main_detector_correlations(TString device_list[], TCanvas *Canvas)  
{
  std::cout<<" Plotting array "<<std::endl;

  // Draw correlations simultaneously
  // Octant 1
  Canvas->cd(4);
  var1 = Form("asym_qwk_%s",device_list[0].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Octant 2
  Canvas->cd(1);  
  var1 = Form("asym_qwk_%s",device_list[1].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Octant 3
  Canvas->cd(2);
  var1 = Form("asym_qwk_%s",device_list[2].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Octant 4
  Canvas->cd(3);
  var1 = Form("asym_qwk_%s",device_list[3].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Ocatant 5
  Canvas->cd(6);
  var1 = Form("asym_qwk_%s",device_list[4].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Octant 6
  Canvas->cd(9);
  var1 = Form("asym_qwk_%s",device_list[5].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Octant 7
  Canvas->cd(8);
  var1 = Form("asym_qwk_%s",device_list[6].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

  // Octant 8
  Canvas->cd(7);
  var1 = Form("asym_qwk_%s",device_list[7].Data());
  plot(CanvasTitle, tree,run_number, var1, bpmdata);
  //get_fit_parameters( ); 

}


//**************************************************************
//**************************************************************
//         Get fit parameters from the histograms                   
//**************************************************************
//**************************************************************


void get_fit_parameters( )  
{
  TGraph* g;
  i++;
  std::cout<<i<<std::endl;

  g=(TGraph*)gPad->GetPrimitive("h");
  Double_t n = 0.0;
  n = g->GetN();

  TF1 *fit = g->GetFunction("pol1");
  p0[i] = fit->GetParameter(0);
  ep0[i] = fit->GetParError(0);
  p1[i] = fit->GetParameter(1);
  ep1[i] = fit->GetParError(1);
  Double_t chi2 = 0.0;
  chi2 = fit->GetChisquare() / (fit->GetNDF());

//   Myfile << setw(20) <<  var1 <<",\t"
// 	 << setw(20) <<  bpmdata << ",\t" 
// 	 << setiosflags(ios::fixed) << setprecision(4) 
// 	 << setw(10) <<  p0[i]  
// 	 << setw(4)  <<  "+-" 
// 	 << setw(10) <<  ep0[i] 
// 	 << setw(5)  <<",\t" 
// 	 << setw(10) <<  p1[i]  
// 	 << setw(4)  <<"+-" 
// 	 << setw(10) <<  ep1[i] 
// 	 << setw(10)  <<",\t" 
// 	 << setw(15) <<  chi2   <<",\t\t"
// 	 << setw(10) <<  n      <<std::endl;


}


//**************************************************************
//**************************************************************
//         Plot the variaiton of the fit parameters                   
//**************************************************************
//**************************************************************


void fit_parameter_variation(TString device_list[], TString name)  
{ 

  //Create a canvas
  TString ctitle =Form("%i_%s_vs_%s_fit_parameters",run_number,name.Data(), bpmdata.Data());
  TCanvas * c= new TCanvas(ctitle, ctitle,0,0,1200,800);  

  pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  pad2 = new TPad("pad2","pad2",0.01,0.01,0.99,0.94);
  pad1->SetFillColor(24);
  pad1->Draw();
  pad2->Draw();
  
  pad1->cd();
  TString text = Form("%i :  %s vs %s fit parameter variation",run_number,name.Data(),bpmdata.Data());
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


