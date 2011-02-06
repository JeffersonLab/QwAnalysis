//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : February 5th, 2011
//*****************************************************************************************************//
//
//
//  This macro will connect to the qw_fall2010_20101204 data base and get the slug averages and plot
//  them in to three plots for pmt+, pmt- and bar sum asymmetries. 
//   e.g. use
//   ./slug_averages
//
//   To compile this code do a gmake.
//
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

TString directory = "$QW_ROOTFILES";

//  For now instead of the main detector PMTs I will use 8 bpm wires.
//  Later these should be replaced by the actual ones.

// Left PMTs
TString quartz_bar_POS[8]=
  {"md1pos","md2pos","md3pos","md4pos","md5pos","md6pos","md7pos","md8pos"};

// Right PMTs
TString quartz_bar_NEG[8]=
  {"md1neg","md2neg","md3neg","md4neg","md5neg","md6neg","md7neg","md8neg"};

// barsum
TString quartz_bar_SUM[8]=
  {"md1barsum","md2barsum","md3barsum","md4barsum","md5barsum","md6barsum","md7barsum","md8barsum"};

TSQLServer *db;

TPad * pad1, *pad2;
TText *t1;
TTree *tree;
TString CanvasTitle;
TString bpm,bpmdata,var1, target;
Int_t runnum;

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
Double_t value11[8];
Double_t err11[8];
Double_t value22[8];
Double_t err22[8];
Double_t value33[8];
Double_t err33[8];

TString xunit, yunit, slopeunit;
void get_octant_data(TString devicelist[], TString target, TString ihwp, Double_t value[], Double_t error[]);
TString get_query(TString detector, TString measurement, TString target, TString ihwp);
void plot_octant(TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[]);


int main(Int_t argc,Char_t* argv[])
{


  std::cout<<"#####################################"<<std::endl;
  std::cout<<" \nSlug averages of MD asymmetries\n"<<std::endl;
  std::cout<<"#####################################"<<std::endl;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al :"<<std::endl;
  std::cin>>opt;


  if(opt == 1) target = "HYDROGEN-CELL";
  else if(opt == 2) target = "DS-4%-Aluminum";
  else{
    std::cout<<"Unknow target type!"<<std::endl;
    exit(1);
  }
  
  std::cout<<"Slug averages of octants in MD PMT-, PMT+, barsum"<<std::endl;
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


  TString title = "Slug averages of Main detector asymmetries. FIT = p0*cos(phi + p1) + p2";
  TCanvas * Canvas1 = new TCanvas("canvas1", title,0,0,1000,800);  
  Canvas1->Draw();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.11,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();
  pad2->Divide(1,3);
  pad2->SetFillColor(20);



//connect to the data base
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");
  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);

  // clear arrays;
  for(Int_t i =0;i<8;i++){
    value1[i] = 0.0;
    err1[i] = 0.0;
    value2[i] =0.0;
    err2[i]=0.0;
    value3[i]=0.0;
    err3[i]=0.0;
    value11[i]=0.0;
    err11[i]=0.0;
    value22[i]=0.0;
    err22[i]=0.0;
    value33[i]=0.0;
    err33[i]=0.0;
  }

  
  TGraphErrors * mg;

  pad2->cd(1);
  get_octant_data(quartz_bar_POS, target,"out", value1,err1);
  get_octant_data(quartz_bar_POS, target,"in", value11,err11);
  plot_octant("MD POS", value11,err11,value1,err1);
  

  pad2->cd(2);
  get_octant_data(quartz_bar_NEG, target,"out", value2,err2);
  get_octant_data(quartz_bar_NEG, target,"in", value22,err22);
  plot_octant("MD NEG",value22,err22,value2,err2);

  pad2->cd(3);
  get_octant_data(quartz_bar_SUM, target,"out", value3,err3);
  get_octant_data(quartz_bar_SUM, target,"in", value33,err33);
  plot_octant("MD BAR SUM", value33,err33,value3,err3);




  Canvas1->Print("slug_summary_plots.gif");

  std::cout<<"Done plotting fits \n";
  db->Close();
  delete db;

  theApp.Run();
  return(1);

};

//***************************************************
//***************************************************
//         get query              
//***************************************************
//***************************************************
TString get_query(TString detector, TString measurement, TString target, TString ihwp){



  Bool_t ldebug = kFALSE;

  TString output = "slow_controls_settings.slow_helicity_plate, target_position, sum(md_data_view.value/(POWER(md_data_view.error,2)*md_data_view.n))/sum(1/(POWER(md_data_view.error,2)*md_data_view.n)), SQRT(1/SUM(1/(POWER(md_data_view.error,2))))";

 
  TString good_for_cut = "run.run_type ='parity' AND md_data_view.good_for_id = NULL || ((md_data_view.good_for_id = 'parity' || md_data_view.good_for_id = 'production') && md_data_view.good_for_id != 'commissioning'))";

  TString run_quality_cut = "(md_data_view.run_quality_id = NULL || md_data_view.run_quality_id != 'bad')";


  TString query =" SELECT " + output
    + " FROM md_data_view, analysis, run, runlet, slow_controls_settings "
    + " WHERE "
    + " md_data_view.analysis_id = analysis.analysis_id AND"
    + " analysis.runlet_id = runlet.runlet_id AND"
    + " runlet.runlet_id = slow_controls_settings.runlet_id AND "
    + " run.run_number = md_data_view.run_number AND "
    + " md_data_view.detector = '"+detector+"' AND "
    + " md_data_view.subblock = 0 AND "
    + " md_data_view.measurement_type = '"+measurement+"' AND "
    //  + good_for_cut+" AND "+run_quality_cut+ " AND "
    + " error !=0  AND md_data_view.slug != 0 AND"
    + " slow_controls_settings.slow_helicity_plate = '"+ihwp+"'"
    + " AND target_position = '"+target+"';"; 

			 
  if(ldebug) std::cout<<query<<std::endl;

  return query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(TString devicelist[], TString target, TString ihwp, Double_t value[], Double_t error[])
{
  Bool_t ldebug =kFALSE;

  for(Int_t i=0 ; i<8 ;i++){
    std::cout<<"Getting data for "<<devicelist[i]<<" ihwp "<<ihwp<<std::endl;
    TString query = get_query(Form("qwk_%s",devicelist[i].Data()),"a",target,ihwp);
    TSQLStatement* stmt = db->Statement(query,100);
    // process statement
    if (stmt->Process()) {
      // store result of statement in buffer
      stmt->StoreResult();
      while (stmt->NextResultRow()) {
	value[i] = (Double_t)(stmt->GetDouble(2))*1e6; // convert to  ppm
	error[i]= (Double_t)(stmt->GetDouble(3))*1e6; // ppm
	if(ldebug)  std::cout<<"value = "<<value[i]<<"+-"<<error[i]<<std::endl;
      }
    }
    delete stmt;    
  }
}

//***************************************************
//***************************************************
//         Plor octant multi graph                   
//***************************************************
//***************************************************

void plot_octant(TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[])  
{
  std::cout<<" Plotting multi "<<std::endl;

  Double_t x[8];
  Double_t errx[8];
  
  for(Int_t i =0;i<8;i++){
    x[i] = i+1;
    errx[i] = 0.0;
  }
  

 // Sin fit 
  TF1 *sinfit = new TF1("sinfit","[0]*sin((pi/180)*(45*(x-1) + [1]))+[2]",1,8);
  sinfit->SetParameter(0,0);
  sinfit->SetParameter(1,0);
  sinfit->SetParameter(2,0);
  
  // cos fit (if we want we can switch to this)
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(1,0);
  cosfit->SetParameter(2,0);


  TGraphErrors* grp_in  = new TGraphErrors(8,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("cosfit");
  TF1* fit1 = grp_in->GetFunction("cosfit");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  TGraphErrors* grp_out  = new TGraphErrors(8,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out->Fit("cosfit");
  TF1* fit2 = grp_out->GetFunction("cosfit");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);



  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in);
  grp->Add(grp_out);
  grp->Draw("AP");



  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",device.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend = new TLegend(0.1,0.80,0.29,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IHWP-IN", "p");
  legend->AddEntry(grp_out, "IHWP-OUT", "p");
  legend->SetFillColor(0);
  legend->Draw("");

  gPad->Update();
  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  stats1->SetTextColor(kBlue); 
  stats2->SetTextColor(kRed); 
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.6);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.05);stats2->SetY2NDC(0.4);

  gPad->Modified();

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



