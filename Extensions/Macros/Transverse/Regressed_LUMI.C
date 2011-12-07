//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : March 9th 2011
//*****************************************************************************************************//
//
//
// This macro goes throug the regressed histograms of LUMIs to get their mean and error to plot the
// LUMI responce.
// 
//   
//  Tuesday, February  8 16:58:25 EST 2011, jhlee
//     - fixed a bug not to disply MD canvas
//    
//*****************************************************************************************************//


using namespace std;


#include <vector>
#include <iomanip>
#include <TRandom.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMultiGraph.h>
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
#include <TPaveStats.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TObject.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>

// for transverse data
TString directory = "/u/group/qweak/www/html/onlineRegression/regression-9500-9998/";


// us lumi sum
TString us_lumi[4]=
  {"uslumi1_sum","uslumi3_sum","uslumi5_sum","uslumi7_sum"};

TString us_lumi_reg[4]=
  {"regresY25","regresY26","regresY27","regresY28"};

// ds lumi sum
TString ds_lumi[8]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8"};

TString ds_lumi_reg[8]=
  {"regresY29","regresY30","regresY31","regresY32","regresY33","regresY34","regresY35","regresY36"};
//TString ds_lumi_reg[5]=
  //{"regresY29","regresY30","regresY31","regresY32","regresY33"};



TPad * pad1, *pad2;
TText *t1;
TTree *tree;
TString CanvasTitle;
TString var1;
Int_t runnum;

Int_t opt =1;
TTree * nt;

std::ofstream Myfile;
Char_t textfile[100];


Double_t p0[8] ={0.0};
Double_t ep0[8] ={0.0};
Double_t p1[8] ={0.0};
Int_t i = -1;
Double_t ep1[8] ={0.0};

Double_t value111[8] ={0.0};
Double_t err111[8] ={0.0};

Double_t value11[4] ={0.0};
Double_t err11[4] ={0.0};

Double_t value[4] ={0.0};
Double_t err[4] ={0.0};
Double_t value1[4] ={0.0};
Double_t err1[4] ={0.0};

TString xunit, yunit, slopeunit;

void plot_octant(Int_t size,TString device, Double_t values[],Double_t error[]);  
void get_octant_data(TFile * file, Int_t size, TString devicelist[], Int_t run, 
		     Double_t value[], Double_t error[]);


int main(Int_t argc,Char_t* argv[])
{


  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nRegressed results LUMI asymmetries\n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter the run number:"<<std::endl;
  std::cin>>opt;


  if(opt>=9887 && opt<=9824){
    std::cout<<"This is not a known transverse run"<<std::endl;
    exit(1);
  }
  
  std::cout<<"Getting data for regressed us lumis and ds lumis"<<std::endl;
  TApplication theApp("App",&argc,argv);

 
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.1);
  gStyle->SetStatH(0.3);

  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
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


  //Get the root file
  TFile *f;
  Char_t filename[200]; 
  
  sprintf(filename,"%s/R%d.000/blueR%d.000.hist.root",directory.Data(),opt,opt);
  
  f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  if (f->IsZombie())
    exit(1);
  
  std::cout<<"Obtaining data from: "<<filename<<"\n";


  TString title2 = Form("%i: LUMI asymmetries:  FIT = p0*cos(phi + p1) + p2",opt);
  TCanvas * Canvas2 = new TCanvas("canvas2", title2,0,0,1000,800);
  Canvas2->Draw();


  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TString text2 = Form(title2);
  TText*t1 = new TText(0.06,0.3,text2);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();
  pad2->Divide(1,2);
  pad2->SetFillColor(20);



  // clear arrays;
  for(Int_t i =0;i<4;i++){
    value[i] = 0.0;
    err[i] = 0.0;
    value1[i] = 0.0;
    err1[i] = 0.0;
  }

  for(Int_t i =0;i<8;i++){
    value11[i] = 0.0;
    err11[i] = 0.0; 
    value111[i] = 0.0;
    err111[i] = 0.0;  
  }



  pad2->cd(1);
  get_octant_data(f,4,us_lumi_reg,opt, value,err);
  plot_octant(4,"US LUMI", value1,err1);
  gPad->Update();


  pad2->cd(2);
  get_octant_data(f,8,ds_lumi_reg,opt, value11,err11);
  plot_octant(8,"DS LUMI", value111,err111);
  gPad->Update();

  Canvas2-> Update();
  Canvas2->Print(Form("%i_lumi_regressed_plots.gif",opt));



  std::cout<<"Done plotting fits \n";

  theApp.Run();
  return(1);

};


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(TFile * file, Int_t size, TString devicelist[], Int_t run, Double_t value[], Double_t error[])
{
  Bool_t ldebug = true;
  TH1D* histo=NULL;
  TString histoname;
  
//   TCanvas * Canvas1 = new TCanvas("canvas1","LUMI regressed histos",0,0,1000,800);
//   Canvas1->Draw();
//   Canvas1->Divide(2,4);

  for(Int_t i=0 ; i<size ;i++){
    //    Canvas1->cd(i+1);
    if(ldebug) {
      std::cout<<"Getting data for "<<devicelist[i]<<std::endl;
    }
    histoname = devicelist[i];
    histo=(TH1D*)file->Get(histoname);
    if (histo==NULL){
      std::cerr<<"Histogram "<<histoname<<" does not exist in the root file \n";
      return;
    } 
    //   histo->Draw();
    value[i] = (Double_t)(histo->GetMean()); // these are in ppm
    error[i] = (Double_t)(histo->GetMeanError());
    if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
  }
}

//***************************************************
//***************************************************
//         Plor octant multi graph                   
//***************************************************
//***************************************************

void plot_octant(Int_t size,TString device, Double_t values[],Double_t error[])  
{

  const int k = size;
  std::cout<<" Plotting multi size "<<k<<std::endl;
  
  Double_t x[k];
  Double_t errx[k];
  
  for(Int_t i =0;i<k;i++){
   if(device.Contains("US"))
       x[i] = 2*i+1;
   if(device.Contains("DS"))
       x[i] = i+1;
  }


  std::cout<<"size = "<<k<<std::endl;
 // Sin fit 
  TF1 *sinfit = new TF1("sinfit","[0]*sin((pi/180)*(45*(x-1) + [1]))+[2]",1,k);
  sinfit->SetParameter(0,0);
  sinfit->SetParameter(1,0);
  sinfit->SetParameter(2,0);
  
  // cos fit (if we want we can switch to this)
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,k);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(1,0);
  cosfit->SetParameter(2,0);


  TGraphErrors* grp  = new TGraphErrors(k,x,values,errx,error);
  grp->Draw("AP");
  grp ->SetMarkerSize(0.6);
  grp ->SetMarkerStyle(21);
  grp ->SetMarkerColor(kBlue);
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",device.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);
  grp->Fit("cosfit");
  TF1* fit1 = grp->GetFunction("cosfit");
  if(fit1 == NULL) exit(1);
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  std::cout<<"herer\n";

//   TPaveStats *stats1 = (TPaveStats*)grp->GetListOfFunctions()->FindObject("stats");
//   stats1->SetTextColor(kBlue); 
//   stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.6);
}



