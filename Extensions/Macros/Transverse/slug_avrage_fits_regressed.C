//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : March 9th, 2011
//*****************************************************************************************************//
//
//
//  This macro will connect to the qw_linreg_20110125 data base and get the slug averages from the 
//  regressed data and plot 
//  them in to three plots for pmt+, pmt- and bar sum, us lumi and ds lumi asymmetries. 
//   e.g. use
//   ./slug_average_regressed
//
//   To compile this code do a gmake.
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
#include <TText.h>


//  For now instead of the main detector PMTs I will use 8 bpm wires.
//  Later these should be replaced by the actual ones.

// Left PMTs
TString quartz_bar_POS[8]=
  {"qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos","qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};

// Right PMTs
TString quartz_bar_NEG[8]=
  {"qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg","qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg"};

TString quartz_bar_SUM[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum","qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};



// us lumi sum
TString us_lumi[4]=
  {"uslumi1_sum","uslumi3_sum","uslumi5_sum","uslumi7_sum"};

// ds lumi sum
TString DS_lumi[5]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5"};

//  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8"};

TSQLServer *db;

TPad * pad1, *pad2;
TText *t1;
TTree *tree;
TString CanvasTitle;
TString bpm,bpmdata,var1, target, polar,targ, goodfor;
Int_t runnum;

Int_t opt =1;
Int_t datopt = 1;
TTree * nt;

std::ofstream Myfile;
Char_t textfile[100];


Double_t p0[8] ={0.0};
Double_t ep0[8] ={0.0};
Double_t p1[8] ={0.0};
Int_t i = -1;
Double_t ep1[8] ={0.0};

Double_t value1[8] ={0.0};
Double_t err1[8] ={0.0};
Double_t value2[8] ={0.0};
Double_t err2[8] ={0.0};
Double_t value3[8] ={0.0};
Double_t err3[8] ={0.0};
Double_t value11[8] ={0.0};
Double_t err11[8] ={0.0};
Double_t value22[8] ={0.0};
Double_t err22[8] ={0.0};
Double_t value33[8] ={0.0};
Double_t err33[8] ={0.0};

Double_t value111[4] ={0.0};
Double_t err111[4] ={0.0};
Double_t value222[4] ={0.0};
Double_t err222[4] ={0.0};

Double_t valuein[8] ={0.0};
Double_t errin[8] ={0.0};
Double_t valueout[8] ={0.0};
Double_t errout[8] ={0.0};


TString xunit, yunit, slopeunit;

void get_octant_data(Int_t size,TString devicelist[], TString detector_type, TString goodfor, TString target, TString ihwp, Double_t value[], Double_t error[]);
TString get_query(TString detector, TString measurement, TString target, TString ihwp, TString detector_type, TString goodfor, TString polar);
void plot_octant(Int_t size,TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[]);
void get_octant_average(Int_t size,TString device, Double_t value[],Double_t error[]);
void get_opposite_octant_average(Int_t size,TString device, 
				 Double_t valuesin[],Double_t errorsin[],
				 Double_t valuesout[],Double_t errorsout[]);

int main(Int_t argc,Char_t* argv[])
{


  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nSlug averages of MD and LUMI asymmetries\n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cin>>opt;
  std::cout<<"Enter data type:"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Transverse "<<std::endl;
  std::cin>>datopt;


  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 percent Al";
  }
  else{
    std::cout<<"Unknown target type!"<<std::endl;
    exit(1);
  }

  if(datopt == 1){
    polar = "longitudinal";
    goodfor = "3";
  }
  else if(datopt == 2){
    polar = "transverse";
    goodfor = "8";
  }
  else{
    std::cout<<"Unknown data type!"<<std::endl;
    exit(1);
  }
  

  std::cout<<"Getting slug averages of octants in MD PMT-, PMT+, barsum, us lumi and ds lumi"<<std::endl;
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

//connect to the data bases
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_linreg_20110125","qweak", "QweakQweak");

  if(db){
    printf("Unregreesed database server info: %s\n", db->ServerInfo());
  }
  else
    exit(1);
  
  // clear arrays;
  for(Int_t i =0;i<8;i++){
    value1[i] = 0.0;
    err1[i] = 0.0;
    value11[i]=0.0;
    err11[i]=0.0;
  }


  for(Int_t i =0;i<4;i++){
    value2[i] = 0.0;
    err2[i] = 0.0;
    value22[i] =0.0;
    err22[i]=0.0;  
  }

  for(Int_t i =0;i<5;i++){
    value3[i] = 0.0;
    err3[i] = 0.0;
    value33[i]=0.0;
    err33[i]=0.0;
  }

  //plot MD asymmetries
  TString title1 = Form("%s (%s): Regressed slug averages of Main detector asymmetries. FIT = p0*cos(phi + p1) + p2",targ.Data(),polar.Data());
  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,1000,500);
  Canvas1->Draw();
  Canvas1->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title1);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();
  get_octant_data(8,quartz_bar_SUM, "MD", goodfor, target, "out", value1,  err1);
  get_octant_data(8,quartz_bar_SUM, "MD", goodfor, target, "in",  value11, err11);
  plot_octant(8,"MD", value11,err11,value1,err1);
  gPad->Update();


  Canvas1->Update();
  Canvas1->Print(polar+"_"+target+"_md_regressed_slug_summary_plots.png");

  // Calculate sum of opposite octants
  get_opposite_octant_average(8,"MD", value11,err11,value1,err1);


  // plot US LUMI asymmetries
  TString title2 = targ+"("+polar+"): Regressed slug averages of US Lumi asymmetries. FIT = p0*cos(phi + p1) + p2";
  TCanvas * Canvas2 = new TCanvas("canvas2", title2,0,0,1000,500);   
  Canvas2->Draw();
  Canvas2->cd();

  TPad*pad11 = new TPad("pad11","pad11",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(20);
  pad11->Draw();
  pad22->Draw();

  pad11->cd();
  TString text2 = Form(title2);
  TText*t11 = new TText(0.06,0.3,text2);
  t11->SetTextSize(0.5);
  t11->Draw();

  pad22->cd();
  get_octant_data(4,us_lumi,"LUMI", goodfor, target, "out", value2,err2);
  get_octant_data(4,us_lumi,"LUMI", goodfor, target, "in", value22,err22);
  plot_octant(4,"US LUMI", value22,err22,value2,err2);
  gPad->Update();

  Canvas2-> Update();
  Canvas2->Print(polar+"_"+target+"_uslumi_regressed_slug_summary_plots.png");

  // plot DS LUMI asymmetries
  TString title3 = targ+"("+polar+"): Regressed slug averages of DS LUMI asymmetries. FIT = p0*cos(phi + p1) + p2";
  TCanvas * Canvas3 = new TCanvas("canvas1", title1,0,0,1000,500);
  Canvas3->Draw();

  TPad*pad111 = new TPad("pad111","pad111",0.005,0.935,0.995,0.995);
  TPad*pad222 = new TPad("pad222","pad222",0.005,0.005,0.995,0.945);
  pad111->SetFillColor(20);
  pad111->Draw();
  pad222->Draw();


  pad111->cd();
  TString text3 = Form(title3);
  TText*t111 = new TText(0.06,0.3,text3);
  t111->SetTextSize(0.5);
  t111->Draw();

  pad222->cd();
  get_octant_data(5,DS_lumi,"LUMI", goodfor, target, "out", value3,err3);
  get_octant_data(5,DS_lumi,"LUMI", goodfor, target, "in", value33,err33);
  plot_octant(5,"DS_LUMI",value33,err33,value3,err3);
  gPad->Update();

  Canvas3-> Update();
  Canvas3->Print(polar+"_"+target+"_dslumi_regressed_slug_summary_plots.png");


 
  // Calculated the error weighted averages of the octants.
  std::cout<<"\n\n############################################################"<<std::endl;
  std::cout<<"Error weighted averages of the asymmetries in the octants "<<std::endl;

  std::cout<<"\n##################"<<std::endl;
  std::cout<<"IHWP state OUT "<<std::endl;
  get_octant_average(8,"MD", value1,err1);
  get_octant_average(4,"USLUMI", value2,err2);
  get_octant_average(5,"DSLUMI", value3,err3);
  std::cout<<"\n##################"<<std::endl;

  std::cout<<"IHWP state IN "<<std::endl;
  get_octant_average(8,"MD", value11,err11);
  get_octant_average(4,"USLUMI", value22,err22);
  get_octant_average(5,"DSLUMI", value33,err33);
  std::cout<<"##################"<<std::endl;

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
TString get_query(TString detector, TString measurement, TString target, TString ihwp, 
		  TString detector_type, TString goodfor, TString polar){



  Bool_t ldebug = true;
  TString datatable;
  TString plate_cut;
  TString good_for_cut;
 
  if(detector_type == "MD")
    datatable = "md_data_view";
  if(detector_type == "LUMI")
    datatable = "lumi_data_view";
  if(detector_type == "BEAM")
    datatable = "beam_view";

  TString output = " sum( distinct (linreg.value/(POWER(linreg.error,2))))/sum( distinct (1/(POWER(linreg.error,2)))), SQRT(1/SUM( distinct (1/(POWER(linreg.error,2)))))";
  TString run_quality_cut = "(fall2010.run_quality_id = '1,3' or fall2010.run_quality_id = '1') "; // good/suspect

  TString join = Form(" qw_fall2010_20101204.%s as fall2010 JOIN qw_linreg_20110125.%s AS linreg ON fall2010.run_number = linreg.run_number JOIN qw_fall2010_20101204.slow_controls_settings AS fall_slow ON fall2010.runlet_id = fall_slow.runlet_id ",datatable.Data(),datatable.Data());

  TString tgt_cut = Form(" fall_slow.target_position = '%s' ",target.Data());
  TString ihwp_cut = Form(" fall_slow.slow_helicity_plate = '%s' ",ihwp.Data());

  // To get rid of runs that have large charge asymmetries
  TString run_cut = "linreg.run_number != 9831 AND linreg.run_number != 9837 AND linreg.run_number != 9853 AND linreg.run_number != 9888  AND linreg.run_number != 9885 AND linreg.run_number != 9880 AND linreg.run_number != 9851 ";
  

  /*************************
  longitudinal

  **************************/
  if(polar == "longitudinal"){
    //    run_cut = " (linreg.run_number<9824  or linreg.run_number>9886)";
    run_cut = " (linreg.run_number<9824)";
    good_for_cut = " (fall2010.good_for_id = '1,3' or fall2010.good_for_id='3') ";

  }
  

  /*************************
   Transverse

  **************************/

  if(polar == "transverse"){
    run_cut = " (linreg.run_number>=9824 and linreg.run_number<=9886) ";
    good_for_cut = " (fall2010.good_for_id = '1,8' or fall2010.good_for_id='8') ";
  }

  TString query =" SELECT " + output
    + " FROM "+join+"WHERE linreg.detector='"+detector+"' AND linreg.subblock = 0 AND "+tgt_cut+" AND "+run_quality_cut+" AND "
    +ihwp_cut+" AND "+good_for_cut+" AND linreg.measurement_type = '"+measurement+"' AND linreg.slope_correction = 'on' AND linreg.slope_calculation = 'off' AND linreg.error !=0 and linreg.value != 0;";
 			 
  if(ldebug)  std::cout<<query<<std::endl;

  return query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(Int_t size, TString devicelist[], TString detector_type,TString goodfor, TString target, TString ihwp, Double_t value[], Double_t error[])
{
  Bool_t ldebug = false;

  if(ldebug) printf("\nDetector Type %s, size %2d\n", detector_type.Data(), size);
  for(Int_t i=0 ; i<size ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    }
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",target,ihwp,detector_type,goodfor,polar);
    TSQLStatement* stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    // process statement
    if (stmt->Process()) {
      // store result of statement in buffer
      stmt->StoreResult();
      while (stmt->NextResultRow()) {
	value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
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

void plot_octant(Int_t size,TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[])  
{

  const int k = size;
  std::cout<<" Plotting multi size "<<k<<std::endl;
  
  Double_t valuesum[k];
  Double_t valueerror[k];
  Double_t valuediff[k];
  Double_t errordiff[k];
  
  Double_t x[k];
  Double_t errx[k];
  
  for(Int_t i =0;i<k;i++){
    if(device.Contains("US"))
      x[i] = 2*i+1;
    else if(device.Contains("DS")){
	x[i] = i+1;
    }
    else
      x[i] = i+1;

    errx[i] = 0.0;
  }

  TPad* pad = (TPad*)(gPad->GetMother());
  std::cout<<"size = "<<k<<std::endl;
 // Sin fit 
  TF1 *sinfit = new TF1("sinfit","[0]*sin((pi/180)*(45*(x-1) + [1]))+[2]",1,8);
  sinfit->SetParameter(0,0);
  sinfit->SetParameter(1,0);
  sinfit->SetParameter(2,0);
  sinfit->SetParLimits(1, 0, 180);

  // cos fit (if we want we can switch to this)
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(1,0);
  cosfit->SetParameter(2,0);
  cosfit->SetParLimits(1, 0, 180);


  TGraphErrors* grp_in  = new TGraphErrors(k,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("cosfit","B");
  TF1* fit1 = grp_in->GetFunction("cosfit");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  TGraphErrors* grp_out  = new TGraphErrors(k,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out->Fit("cosfit","B");
  TF1* fit2 = grp_out->GetFunction("cosfit");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);


 
  // Sum over the in and out half wave plate values
  for(Int_t i =0;i<size;i++){
    valuesum[i]=((valuesin[i]/pow(errorsin[i],2)) + (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    valueerror[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));
  }

 

  TGraphErrors* grp_sum  = new TGraphErrors(k,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.6);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum->Fit("cosfit");
  TF1* fit3 = grp_sum->GetFunction("cosfit");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen-2);

 
  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in);
  grp->Add(grp_out);
  grp->Add(grp_sum);
  grp->Draw("AP");

  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",device.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IHWP-IN", "p");
  legend->AddEntry(grp_out, "IHWP-OUT", "p");
  legend->AddEntry(grp_sum, "IN+OUT", "p");
  legend->SetFillColor(0);
  legend->Draw("");

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");

  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 

  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
 
  stats3->SetTextColor(kGreen-2);
  stats3->SetFillColor(kWhite); 

  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  
  pad->Update();
  
 // Difference over the in and out half wave plate values
  TString title = targ+"("+polar+"): Regressed In-OUT averages for "+device+" asymmetries. FIT = p0*cos(phi + p1) + p2";

  TCanvas * Canvas11 = new TCanvas("canvas11",title,0,0,1000,500);
  Canvas11->Draw();
  Canvas11->cd();

  TPad*pad11 = new TPad("pad11","pad11",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(20);
  pad11->Draw();
  pad22->Draw();
  

  pad11->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad22->cd();

  for(Int_t i =0;i<size;i++){
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));

  }

  TGraphErrors* grp_diff  = new TGraphErrors(k,x,valuediff,errx,errordiff);
  grp_diff ->SetMarkerSize(0.6);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kBlack-2);
  grp_diff->Fit("cosfit");
 
  TF1* fit4 = grp_diff->GetFunction("cosfit");
  fit4->DrawCopy("same");
  fit4->SetLineColor(kYellow-2);
  grp_diff->Draw("AP");


  grp_diff->SetTitle("");
  grp_diff->GetXaxis()->SetTitle("Octant");
  grp_diff->GetXaxis()->CenterTitle();
  grp_diff->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",device.Data()));
  grp_diff->GetYaxis()->CenterTitle();
  grp_diff->GetYaxis()->SetTitleOffset(0.7);
  grp_diff->GetXaxis()->SetTitleOffset(0.8);

 //  TPaveStats *stats11 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");
//   if(stats11==NULL) exit(1);
//   stats11->SetTextColor(kBlue);
//   stats11->SetFillColor(kWhite); 
//   stats11->SetX1NDC(0.8); stats11->SetX2NDC(0.99); stats11->SetY1NDC(0.7);stats11->SetY2NDC(0.95);

//   stats11->Draw();
  

  Canvas11-> Update();
  Canvas11->Print(polar+"_"+target+"_"+device+"_regressed_in_out_plots.png");


}


//***************************************************
//***************************************************
//         get error weighted average of octants                
//***************************************************
//***************************************************

void get_octant_average(Int_t size,TString device, Double_t value[],Double_t error[])  
{
  Bool_t ldebug = false;
  Double_t avgvalue = 0.0;
  Double_t avg = 0.0;

  Double_t avgerror = 0.0;
  Double_t avgerr = 0.0;

  if(ldebug) printf("Summing over the octants of %s\n", device.Data());

  // Calculated weighted average
  for(Int_t i =0;i<size;i++){
    if(ldebug) std::cout<<"value = "<<value[i]<<" error = "<<error[i]<<std::endl;
    avg += value[i]/pow(error[i],2);
    avgerr +=  1/(pow(error[i],2));
  }

  avgvalue = avg/avgerr;
  avgerror = sqrt(1/avgerr);

  Printf("Error weighted average for all the octants of %8s is (%3.4f +- %3.4f) ppm ",device.Data(),avgvalue,avgerror);
}


//***************************************************
//***************************************************
//         get error weighted average of opposite octants                
//***************************************************
//***************************************************

void get_opposite_octant_average(Int_t size,TString device, 
				 Double_t valuesin[],Double_t errorsin[],
				 Double_t valuesout[],Double_t errorsout[])  
{
  Bool_t ldebug = false;
  Double_t avgvalue = 0.0;
  Double_t avg = 0.0;

  Double_t avgerror = 0.0;
  Double_t avgerr = 0.0;

  Double_t valuesum[4];
  Double_t valueerror[4];

  Double_t valuesumin[4];
  Double_t valueerrorin[4];
  Double_t valuesumout[4];
  Double_t valueerrorout[4];
 
  Double_t x[4];
  Double_t errx[4];
  for(Int_t i =0;i<4;i++){
    x[i]=i+1;
    errx[i]=0.0;
  }

  if(ldebug) printf("Summing over the opposite octants of %s\n", device.Data());

  // Calculated weighted average for ihwp-in
  for(Int_t i =0;i<4;i++){
    std::cout<<valuesin[i]<<"  ffffffff "<<valuesin[i+4]<<std::endl;
    valuesumin[i]=((valuesin[i]/pow(errorsin[i],2)) + (valuesin[i+4]/pow(errorsin[i+4],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsin[i+4],2)));
    valueerrorin[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsin[i+4],2))));
    valuesumout[i]=((valuesout[i]/pow(errorsout[i],2)) + (valuesout[i+4]/pow(errorsout[i+4],2))) /((1/pow(errorsout[i],2)) + (1/pow(errorsout[i+4],2)));
    valueerrorout[i]= sqrt(1/((1/(pow(errorsout[i],2)))+(1/pow(errorsout[i+4],2))));
  }
  
  TString title = targ+"("+polar+"): Regressed slug based opposite octant averages of "+device+"s. FIT = p0*cos(phi + p1) + p2";
  
  TCanvas * Canvas11 = new TCanvas("canvas11",title,0,0,1000,500);
  Canvas11->Draw();
  Canvas11->cd();
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();
  // cos fit (if we want we can switch to this)
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(1,0);
  cosfit->SetParameter(2,0);
  cosfit->SetParLimits(1, 0, 180);

  TGraphErrors* grp_in  = new TGraphErrors(4,x,valuesumin,errx,valueerrorin);
  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("pol0");
  TF1* fit1 = grp_in->GetFunction("pol0");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  TGraphErrors* grp_out  = new TGraphErrors(4,x,valuesumout,errx,valueerrorout);
  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out->Fit("pol0");
  TF1* fit2 = grp_out->GetFunction("pol0");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed); 

 // Sum over the in and out half wave plate values
  for(Int_t i =0;i<4;i++){
    valuesum[i]=((valuesumin[i]/pow(valueerrorin[i],2)) + (valuesumout[i]/pow(valueerrorout[i],2))) /((1/pow(valueerrorin[i],2)) + (1/pow(valueerrorout[i],2)));
    valueerror[i]= sqrt(1/((1/(pow(valueerrorin[i],2)))+(1/pow(valueerrorout[i],2))));
  } 

  TGraphErrors* grp_sum  = new TGraphErrors(4,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.6);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum->Fit("pol0");
  TF1* fit3 = grp_sum->GetFunction("pol0");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen-2);


  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in);
  grp->Add(grp_out);
  grp->Add(grp_sum);
  grp->Draw("AP");

  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant+opposite Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Opposite Octant AVG Asymmetry (ppm)",device.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleSize(0.03);
  grp->GetYaxis()->SetTitleOffset(0.8);
  grp->GetXaxis()->SetTitleOffset(0.9);

  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IHWP-IN", "p");
  legend->AddEntry(grp_out, "IHWP-OUT", "p");
  legend->AddEntry(grp_sum, "IN-OUT", "p");
  legend->SetFillColor(0);
  legend->Draw("");

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");

  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 

  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 

  stats3->SetTextColor(kGreen-2);
  stats3->SetFillColor(kWhite); 

  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  Canvas11-> Update();
  Canvas11->Print(polar+"_"+target+"_"+device+"_regressed_opposite_octant_plots.png");
 
 
}

