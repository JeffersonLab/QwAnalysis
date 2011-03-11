//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : April 1st, 2010
//*****************************************************************************************************//
//
//
//   This macro will plot the correlations between "param1"( = yield/asym) of the array of devices 
//   of type "det1"(= LPMT/RPMT) and "det2"(= any of the parameters in HEL_Tree).
//
//   ./correlation_plots runnumber param1 det1 det2 cut
//
//   To compile this code do a gmake.
//   To use the exe file from command prompt following inputs are needed:
//
//   
//   param1 : yield, asym.
//
//   det1   : LPMT (Left PMTs of the quartz bars), RPMT (Right PMTs of the quartz bars).
//
//   det2   : Any  of the parameters on HEL_Tree. Need to type in the exact format.
//            For bcm yields its            - yield_qwk_NAME
//            For bcm asymmetryies its      - asym_qwk_NAME
//            For bpm yields its            - yield_qwk_NAMERelX or yield_qwk_NAMERelY
//            For bpm differences its       - asym_qwk_NAMERelX or asym_qwk_NAMERelY
//            NAME is the correct device name as it appears in the tree.
//
//  e.g. If you want to plot correlation between the left PMT yields of the quartz bars
//  and the yield_qwk_bcm0l02 (bcm yield) for run 20194,you have to use;
//
// ./correlation_plots 20194  yield  LPMT  yield_qwk_bcm0l02 
//
//
//*****************************************************************************************************//



#include "correlation_plots.h"

using namespace std;

TString directory = "~/scratch/rootfiles/";


//  For now instead of the main detector PMTs I will use 8 bpm wires.
//  Later these should be replaced by the actual ones.

// Left PMTs
TString quartz_bar_L[8]=
  {"qwk_1i04RelX","qwk_1i06RelX","qwk_0i02RelX","qwk_0i02aRelX",
   "qwk_0i05RelX","qwk_0i07RelX","qwk_0l01RelX","qwk_0l03RelX"};

// Right PMTs
TString quartz_bar_R[8]=
  {"qwk_1i04RelY","qwk_1i06RelY","qwk_0i02RelY","qwk_0i02aRelY",
   "qwk_0i05RelY","qwk_0i07RelY","qwk_0l01RelY","qwk_0l03RelX"};
  

TCanvas * Canvas;
TPad * pad1, *pad2;
TText *t1;
TTree *tree;
TString CanvasTitle;
TString run, de2 ,prop1 ,dev1, runnum,dev2 ,var1;

TTree * nt;

std::ofstream Myfile;
Char_t textfile[100];


Double_t p0[8];
Double_t ep0[8];
Double_t p1[8];
Int_t i = -1;
Double_t ep1[8];

TString xunit, yunit, slopeunit;



int main(Int_t argc,Char_t* argv[])
{


  TFile *f;
  Char_t filename[200]; 
  
  if(argc == 5)
    {
      run   = argv[1];     // runnumber
      prop1 = argv[2];     // property
      dev1  = argv[3];     // device 1
      de2   = argv[4];     // device 2
    }
  else if((argc < 3)||(argc > 5))
    {
      std::cout<<argc<<std::endl;
      std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
      std::cerr<<"./correlation_plots run number param1 det1 det2 cut \n";
      std::cerr<<" param1 = yield / diff /asym \n";
      std::cerr<<" det1 = LPMT / RPMT \n";
      std::cerr<<" det2 = device  \n";
      exit(1);
    }
  
  
   TApplication theApp("App",&argc,argv);

  //  gROOT -> ProcessLine(".L /home/buddhini/QwAnalysisRegression/QwAnalysis/Extensions/Macros/Tracking/TSuperCanvas.cc+");
 
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000010);
  gStyle->SetStatY(0.96);
  gStyle->SetStatX(0.96);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.15);

  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);

  
  // histo parameters
  gStyle->SetTitleYOffset(1.8);
  gStyle->SetTitleXOffset(1.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.07);

  //Create a canvas
  CanvasTitle=Form("%s_%s_%s_vs_%s",run.Data(),prop1.Data(),dev1.Data(), de2.Data());
  Canvas = new TCanvas(CanvasTitle, CanvasTitle,0,0,1200,800);  
  Canvas->Divide(3,3);
  gPad->SetFillColor(20);
  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  //Get the root file
  sprintf(filename,"%sQweak_BeamLine_%d.root",directory.Data(),atoi(run));
    
  f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";
    
    
  //load the HEL_Tree. It has data based on quartets analysis.
  tree = (TTree*)f->Get("HEL_Tree"); 

  plot_correlations(tree, run, de2);

  std::cout<<"Done with plotting correlations \n";


  theApp.Run();
  return(1);

};


//***************************************************
//***************************************************
//         plot correlations                   
//***************************************************
//***************************************************

void plot_correlations(TTree *heltree, TString run_number, TString det2)
  {
    
    runnum = run_number;
    dev2 = det2;
    nt = heltree;


    // Open file to store the correlation fit paramters
    sprintf(textfile,"%s_%s_%s_vs_%s_correlation_fit_paramters.txt",
	    runnum.Data(),prop1.Data(),dev1.Data(),dev2.Data()); 
    Myfile.open(textfile);
    
    
    //Put the date and time to the text file
    time_t theTime;
    time(&theTime);
    Myfile <<"! date of analysis ="<<ctime(&theTime)<<"\n\n";
    
    
    // select the unit for the y axis
    if(prop1.Contains("asym")) yunit = " ";
    else if ((dev1.Contains("LPMT")) && quartz_bar_L[0].Contains("Rel")) yunit = "#mum";
    else if ((dev1.Contains("RPMT")) && quartz_bar_R[0].Contains("Rel")) yunit = "#mum";
    else yunit = "#muA";
    
    
    if(dev2.Contains("asym")) xunit = " ";
    else if (dev2.Contains("Rel")) xunit = "(#mum)";
    else xunit = "(#muA)";
    
    slopeunit = Form("%s/%s",yunit.Data(),xunit.Data());
    
    
    //Put the run number
    Myfile <<"! run number ="<<runnum<<"\n\n";
    Myfile << setw(20) << "!detector\t" 
	   << setw(20) << "beam parameter \t"  
	   << setw(10) << "(p0"                
	   << setw(4)  << "+-" 
	   << setw(10) << "dp0)"               
	   << setw(5)  << yunit <<",\t" 
	   << setw(10) << "(p1"                
	   << setw(4)  << "+-" 
	   << setw(10) << "dp1)"                
	   << setw(10)  << slopeunit            <<",\t" 
	   << setw(15) << "reduced chi square" <<",\t\t"
	   << setw(10) << "entries"            <<"\n\n";
    
    
    
    
    // Plot the correaltions
    if(dev1.Contains("LPMT"))
      {
	main_detector_correlations(quartz_bar_L); 
	fit_parameter_variation(quartz_bar_L );  
      }
    else if (dev1.Contains("RPMT"))
      {
	main_detector_correlations(quartz_bar_R); 
	fit_parameter_variation(quartz_bar_R );
      }
    else
      {
	std::cerr<<"Unknown device type : "<<dev1<<". Cannot Plot correlations"<<std::endl;
	exit(1);
      }      
    
    
    
    // Set the plot information on the center pad
    Canvas->cd(5);
    gPad->SetFillColor(24);
    TPaveText *pt = new TPaveText(.05,.05,.95,.95);
    pt->AddText(Form("Run = %s", runnum.Data()));
    pt->AddText("Correlations between");
    pt->AddText(Form("%s %s's", dev1.Data(), prop1.Data()));
    pt->AddText("and");
    pt->AddText(Form("%s", dev2.Data()));
    pt->Draw();
    
    Myfile.close();
    
    // Print the canvas on to a  file
    Canvas->Print(CanvasTitle+".gif");
    
  }

//***************************************************
//***************************************************
//         Main detector correlations                   
//***************************************************
//***************************************************

void main_detector_correlations(TString device_list[])  
{
  std::cout<<" Plotting array "<<dev1<<std::endl;

  // Draw correlations simultaneously
  // Octant 1
  Canvas->cd(4);
  var1 = Form("%s_%s",prop1.Data(),device_list[0].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Octant 2
  Canvas->cd(1);
  var1 = Form("%s_%s",prop1.Data(),device_list[1].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Octant 3
  Canvas->cd(2);
  var1 = Form("%s_%s",prop1.Data(),device_list[2].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Octant 4
  Canvas->cd(3);
  var1 = Form("%s_%s",prop1.Data(),device_list[3].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Ocatant 5
  Canvas->cd(6);
  var1 = Form("%s_%s",prop1.Data(),device_list[4].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Octant 6
  Canvas->cd(9);
  var1 = Form("%s_%s",prop1.Data(),device_list[5].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Octant 7
  Canvas->cd(8);
  var1 = Form("%s_%s",prop1.Data(),device_list[6].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

  // Octant 8
  Canvas->cd(7);
  var1 = Form("%s_%s",prop1.Data(),device_list[7].Data());
  plot(CanvasTitle, nt,atoi(runnum), var1, dev2);
  get_fit_parameters( ); 

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
  Double_t n = g->GetN();

  TF1 *fit = g->GetFunction("pol1");
  p0[i] = fit->GetParameter(0);
  ep0[i] = fit->GetParError(0);
  p1[i] = fit->GetParameter(1);
  ep1[i] = fit->GetParError(1);
  Double_t chi2 = fit->GetChisquare() / (fit->GetNDF());

  Myfile << setw(20) <<  var1 <<",\t"
	 << setw(20) <<  dev2 << ",\t" 
	 << setiosflags(ios::fixed) << setprecision(4) 
	 << setw(10) <<  p0[i]  
	 << setw(4)  <<  "+-" 
	 << setw(10) <<  ep0[i] 
	 << setw(5)  <<",\t" 
	 << setw(10) <<  p1[i]  
	 << setw(4)  <<"+-" 
	 << setw(10) <<  ep1[i] 
	 << setw(10)  <<",\t" 
	 << setw(15) <<  chi2   <<",\t\t"
	 << setw(10) <<  n      <<std::endl;


}


//**************************************************************
//**************************************************************
//         Plot the variaiton of the fit parameters                   
//**************************************************************
//**************************************************************


void fit_parameter_variation(TString device_list[] )  
{ 

  //Create a canvas
  TString ctitle =Form("%s_%s_%s_vs_%s_fit_parameters",runnum.Data(),prop1.Data(),dev1.Data(), dev2.Data());
  TCanvas * c= new TCanvas(ctitle, ctitle,0,0,1200,800);  

  pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  pad2 = new TPad("pad2","pad2",0.01,0.01,0.99,0.94);
  pad1->SetFillColor(24);
  pad1->Draw();
  pad2->Draw();
  
  pad1->cd();
  TString text = Form("%s :  %s %s vs %s fit parameter variation",runnum.Data(),prop1.Data(),dev1.Data(), dev2.Data());
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



