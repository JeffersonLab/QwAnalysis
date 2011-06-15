//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : March 9th, 2011
//*****************************************************************************************************//
//
//
//  This macro will connect to the qw_linreg_20110125 data base and draw the runlet based correltions between
//  1. The main detectors asymmetry vs diff X of target
//  2.  main detectors asymmetry vs diff Y of target
//   e.g. use
//   ./draw_correlations_runlets
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
#include <TVector.h>


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
  //  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5"};
TString DS_lumi[8]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8"};

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

TVectorD  valuexin(1);
TVectorD  errxin(1);
TVectorD  valuexout(1);
TVectorD  errxout(1);

TVectorD  valuey(1);
TVectorD  erry(1);

TVectorD  value1(1);
TVectorD  err1(1);
TVectorD  value11(1);
TVectorD  err11(1);

TVectorD  value2(1);
TVectorD  err2(1);
TVectorD  value22(1);
TVectorD  err22(1);

TVectorD  value3(1);
TVectorD  err3(1);
TVectorD  value33(1);
TVectorD  err33(1);


TString xunit, yunit, slopeunit;

TString get_query(TString detector, TString target, 
		  TString ihwp, TString detector_type, TString goodfor, TString polar);
void plot_data(TVectorD valuesin, TVectorD errorsin, 
	       TVectorD valuesout, TVectorD errorsout);
void get_data(TString device, TString detector_type,TString goodfor, 
	      TString target, TString ihwp, TVectorD*value, TVectorD*error);
void plot_data1(TString device, TVectorD valuesin, TVectorD errorsin, 
	       TVectorD valuesout, TVectorD errorsout);
void get_data1(TString device, TString detector_type,TString goodfor, 
	      TString target, TString ihwp, TVectorD*value, TVectorD*error);

int main(Int_t argc,Char_t* argv[])
{


  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nDraw Correlations to beam parameters \n"<<std::endl;
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
  

  std::cout<<"Getting data from the data base"<<std::endl;
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
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  
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

  
  // clear arrays;
  valuexin.Clear();
  errxin.Clear();
  valuexout.Clear();
  errxout.Clear();

  valuey.Clear();
  erry.Clear();

  value1.Clear();
  err1.Clear();
  value11.Clear();
  err11.Clear();

  value2.Clear();
  err2.Clear();
  value22.Clear();
  err22.Clear();

  value3.Clear();
  err3.Clear();
  value33.Clear();
  err33.Clear();
  
//connect to the data bases
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_linreg_20110125","qweak", "QweakQweak");

  if(db){
    printf("Unregreesed database server info: %s\n", db->ServerInfo());
  }
  else{
    std::cout<<"Unable to connect to the database!"<<std::endl;   
    exit(1);
  }
  //Get beam parameters from unregressed database

  get_data1("qwk_charge", "BEAM", goodfor, target, "out", &valuexout,  &errxout);
  get_data1("qwk_charge", "BEAM", goodfor, target, "in",  &valuexin, &errxin);

  //plot MD asymmetries
  TString title1 = Form("%s (%s): Regressed Main detector asymmetries vs charge asymmetry",targ.Data(),polar.Data());
  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,910,800);
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
  pad2->Divide(3,3);
  pad2->cd(4);
  get_data(quartz_bar_SUM[0], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[0], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad2->cd(1);
  get_data(quartz_bar_SUM[1], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[1], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(2);
  get_data(quartz_bar_SUM[2], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[2], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(3);
  get_data(quartz_bar_SUM[3], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[3], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(6);
  get_data(quartz_bar_SUM[4], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[4], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(9);
  get_data(quartz_bar_SUM[5], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[5], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(8);
  get_data(quartz_bar_SUM[6], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[6], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(7);
  get_data(quartz_bar_SUM[7], "MD", goodfor, target, "out", &value1,  &err1);
  get_data(quartz_bar_SUM[7], "MD", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad2->cd(5);
  TPaveText *pt = new TPaveText(.05,.1,.95,.8);
  pt->AddText("Main detector regressed asymmetry ");
  pt->AddText(" vs ");
  pt->AddText("Charge asymmetry.");
  pt->AddText("#color[2]{IHWP  OUT}");
  pt->AddText("#color[4]{IHWP  IN}");
  pt->SetFillColor(0);
  pt->Draw();

  Canvas1->Modified();
  Canvas1->Update();
  Canvas1->SaveAs(polar+"_"+target+"_md_regressed_vs_charge_aq_correlation_plots.png");



  // plot US LUMI asymmetries
  TString title2 = targ+"("+polar+"): Regressed US Lumi asymmetries vs Charge asymmetry.";
  TCanvas * Canvas2 = new TCanvas("canvas2", title2,0,0,910,800);   
  Canvas2->Draw();
  Canvas2->cd();

  TPad*pad11 = new TPad("pad11","pad11",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(20);
  pad11->Draw();
  pad22->Draw();

  pad11->cd();
  text = Form(title2);
  TText* t2 = new TText(0.06,0.3,text);
  t2->SetTextSize(0.5);
  t2->Draw();

  pad22->cd();
  pad22->Divide(3,3);

  pad22->cd(4);
  get_data(us_lumi[0], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(us_lumi[0], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad22->cd(2);
  get_data(us_lumi[1], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(us_lumi[1], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad22->cd(6);
  get_data(us_lumi[2], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(us_lumi[2], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad22->cd(8);
  get_data(us_lumi[3], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(us_lumi[3], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);


  pad22->cd(5);
  TPaveText *pt1 = new TPaveText(.05,.1,.95,.8);
  pt1->AddText("Upstream Lumi regressed asymmetry ");
  pt1->AddText(" vs ");
  pt1->AddText("Charge asymmetry..");
  pt1->AddText("#color[2]{IHWP  OUT}");
  pt1->AddText("#color[4]{IHWP  IN}");
  pt1->SetFillColor(0);
  pt1->Draw();


  Canvas2-> Update();
  Canvas2->Print(polar+"_"+target+"_uslumi_regressed_vs_charge_aq_correlation_plots.png");

  // plot DS LUMI asymmetries
  TString title3 = targ+"("+polar+"): Regressed DS LUMI asymmetries vs Charge asymmetry.";
  TCanvas * Canvas3 = new TCanvas("canvas3",title3,0,0,910,800);
  Canvas3->Draw();

  TPad*pad111 = new TPad("pad111","pad111",0.005,0.935,0.995,0.995);
  TPad*pad222 = new TPad("pad222","pad222",0.005,0.005,0.995,0.945);
  pad111->SetFillColor(20);
  pad111->Draw();
  pad222->Draw();

  pad111->cd();
  text = Form(title3);
  TText*t3 = new TText(0.06,0.3,text);
  t3->SetTextSize(0.5);
  t3->Draw();

  pad222->cd();
  pad222->Divide(3,3);
  pad222->cd(4);
  get_data(DS_lumi[0], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[0], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(1);
  get_data(DS_lumi[1], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[1], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(2);
  get_data(DS_lumi[2], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[2], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(3);
  get_data(DS_lumi[3], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[3], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(6);
  get_data(DS_lumi[4], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[4], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(9);
  get_data(DS_lumi[5], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[5], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(8);
  get_data(DS_lumi[6], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[6], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(7);
  get_data(DS_lumi[7], "LUMI", goodfor, target, "out", &value1,  &err1);
  get_data(DS_lumi[7], "LUMI", goodfor, target, "in",  &value11, &err11);
  plot_data(value11,err11,value1,err1);

  pad222->cd(5);
  TPaveText *pt2 = new TPaveText(.05,.1,.95,.8);
  pt2->AddText("Downstream Lumi regressed asymmetry ");
  pt2->AddText(" vs ");
  pt2->AddText("Charge asymmetry.");
  pt2->AddText("#color[2]{IHWP  OUT}");
  pt2->AddText("#color[4]{IHWP  IN}");
  pt2->SetFillColor(0);
  pt2->Draw();

  Canvas3-> Update();
  Canvas3->Print(polar+"_"+target+"_dslumi_regressed_charge_aq_correlation_plots.png");

  std::cout<<"Done plotting fits \n";
  db->Close();
  delete db;

  theApp.Run();
  return(1);

};

//***************************************************
//***************************************************
//         get query  from regressed data base           
//***************************************************
//***************************************************
TString get_query(TString detector, TString target, TString ihwp, 
		  TString detector_type, TString goodfor, TString polar){



  Bool_t ldebug = true;
  TString datatable;
  TString plate_cut;
  TString good_for_cut;
  TString type;
  TString measurement;

  if(detector_type == "MD")
    datatable = "md_data_view";
  if(detector_type == "LUMI")
    datatable = "lumi_data_view";
  if(detector_type == "BEAM")
    datatable = "beam_view";

  if(detector_type == "BEAM"){
    type="monitor";
    measurement = "d";
  }
  else{
    type = "detector";
    measurement = "a";
  }
  TString output = " distinct linreg.run_number, linreg.value, linreg.error ";
  TString run_quality_cut = "(fall2010.run_quality_id = '1,3' or fall2010.run_quality_id = '1') "; // good/suspect

  TString join = Form(" qw_fall2010_20101204.%s as fall2010 JOIN qw_linreg_20110125.%s AS linreg ON fall2010.run_number = linreg.run_number JOIN qw_fall2010_20101204.slow_controls_settings AS fall_slow ON fall2010.runlet_id = fall_slow.runlet_id ",datatable.Data(),datatable.Data());

  TString tgt_cut = Form(" fall_slow.target_position = '%s' ",target.Data());
  TString ihwp_cut = Form(" fall_slow.slow_helicity_plate = '%s' ",ihwp.Data());

  // To get rid of runs that have large charge asymmetries
  TString run_cut = "linreg.run_number != 9831 AND linreg.run_number != 9837 AND linreg.run_number != 9853 AND linreg.run_number != 9888  AND linreg.run_number != 9885 AND linreg.run_number != 9880 AND linreg.run_number != 9851 ";
  
  TString detector_cut = Form(" linreg.%s = '%s' ",type.Data(),detector.Data());

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
    + " FROM "+join+"WHERE "+detector_cut+" AND linreg.subblock = 0 AND "+run_cut+" AND "+tgt_cut+" AND "+run_quality_cut+" AND "
    +ihwp_cut+" AND "+good_for_cut+" AND linreg.measurement_type = '"+measurement+"' AND linreg.slope_correction = 'on' AND linreg.slope_calculation = 'off' AND linreg.error !=0 and linreg.value != 0;";
 			 
  if(ldebug)  std::cout<<query<<std::endl;

  return query;
}



//***************************************************
//***************************************************
//         get query from fall data base      
//***************************************************
//***************************************************
TString get_query1(TString detector, TString target, TString ihwp, 
		  TString detector_type, TString goodfor, TString polar){



  Bool_t ldebug = true;
  TString datatable;
  TString plate_cut;
  TString good_for_cut;
  TString type;
  TString measurement;

  if(detector_type == "MD")
    datatable = "md_data_view";
  if(detector_type == "LUMI")
    datatable = "lumi_data_view";
  if(detector_type == "BEAM")
    datatable = "beam_view";

  if(detector_type == "BEAM"){
    type="monitor";
    measurement = "a";
  }
  else{
    type = "detector";
    measurement = "a";
  }
  TString output = " distinct fall2010.run_number, fall2010.value, fall2010.error ";
  TString run_quality_cut = "(fall2010.run_quality_id = '1,3' or fall2010.run_quality_id = '1') "; // good/suspect

  TString join = Form(" qw_fall2010_20101204.%s as fall2010 JOIN qw_linreg_20110125.%s AS linreg ON fall2010.run_number = linreg.run_number JOIN qw_fall2010_20101204.slow_controls_settings AS fall_slow ON fall2010.runlet_id = fall_slow.runlet_id and linreg.segment_number = fall2010.segment_number ",datatable.Data(),datatable.Data());

  TString tgt_cut = Form(" fall_slow.target_position = '%s' ",target.Data());
  TString ihwp_cut = Form(" fall_slow.slow_helicity_plate = '%s' ",ihwp.Data());

  // To get rid of runs that have large charge asymmetries
  TString run_cut = "fall2010.run_number != 9831 AND fall2010.run_number != 9837 AND fall2010.run_number != 9853 AND fall2010.run_number != 9888  AND fall2010.run_number != 9885 AND fall2010.run_number != 9880 AND fall2010.run_number != 9851 ";
  
  TString detector_cut = Form(" fall2010.%s = '%s' ",type.Data(),detector.Data());

  /*************************
  longitudinal

  **************************/
  if(polar == "longitudinal"){
    //    run_cut = " (linreg.run_number<9824  or linreg.run_number>9886)";
    run_cut = " (fall2010.run_number<9824)";
    good_for_cut = " (fall2010.good_for_id = '1,3' or fall2010.good_for_id='3') ";

  }
  

  /*************************
   Transverse

  **************************/

  if(polar == "transverse"){
    run_cut = " (fall2010.run_number>=9824 and fall2010.run_number<=9886) ";
    good_for_cut = " (fall2010.good_for_id = '1,8' or fall2010.good_for_id='8') ";
  }

  TString query =" SELECT " + output
    + " FROM "+join+"WHERE "+detector_cut+" AND fall2010.subblock = 0 AND "+run_cut+" AND "+tgt_cut+" AND "+run_quality_cut+" AND "
    +ihwp_cut+" AND "+good_for_cut+" AND fall2010.measurement_type = '"+measurement+"' AND fall2010.slope_correction = 'off' AND fall2010.slope_calculation = 'off' AND fall2010.error !=0 and fall2010.value != 0 and linreg.monitor = 'qwk_bpm3h09X' and linreg.subblock=0 and linreg.measurement_type='d';";
 			 
  if(ldebug)  std::cout<<query<<std::endl;

  return query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_data(TString device, TString detector_type,TString goodfor, 
		     TString target, TString ihwp, TVectorD*value, TVectorD*error)
{
  Bool_t ldebug = false;

  if(ldebug) printf("\nDetector Type %s \n", detector_type.Data());
  TString query = get_query(Form("%s",device.Data()),target,ihwp,detector_type,goodfor,polar);
  TSQLStatement* stmt = db->Statement(query,100);
  if(!stmt)  {
    db->Close();
    exit(1);
  }

  value->Clear();
  error->Clear();
  Int_t k=1;
  
  // process statement
  if (stmt->Process()) {
    // store result of statement in buffer
    stmt->StoreResult();
    while (stmt->NextResultRow()) {
      value->ResizeTo(k+1);
      error->ResizeTo(k+1);
      value->operator()(k) = (Double_t)(stmt->GetDouble(1))*1e6; // convert to  ppm (asymmetry) or nano meters(position difference)
      error->operator()(k) = (Double_t)(stmt->GetDouble(2))*1e6; // 
      //if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[k], error[k]);
      k++;
      if(ldebug) std::cout<<k<<std::endl;
    }
  }
  delete stmt;    
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_data1(TString device, TString detector_type,TString goodfor, 
		     TString target, TString ihwp, TVectorD*value, TVectorD*error)
{
  Bool_t ldebug = false;

  if(ldebug) printf("\nDetector Type %s \n", detector_type.Data());
  TString query = get_query1(Form("%s",device.Data()),target,ihwp,detector_type,goodfor,polar);
  TSQLStatement* stmt = db->Statement(query,100);
  if(!stmt)  {
    db->Close();
    exit(1);
  }

  value->Clear();
  error->Clear();
  Int_t k=1;
  
  // process statement
  if (stmt->Process()) {
    // store result of statement in buffer
    stmt->StoreResult();
    while (stmt->NextResultRow()) {
      value->ResizeTo(k+1);
      error->ResizeTo(k+1);
      value->operator()(k) = (Double_t)(stmt->GetDouble(1))*1e6; // convert to  ppm (asymmetry) or nano meters(position difference)
      error->operator()(k) = (Double_t)(stmt->GetDouble(2))*1e6; // 
      //if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[k], error[k]);
      k++;
      if(ldebug) std::cout<<k<<std::endl;
    }
  }
  delete stmt;    
}




//***************************************************
//***************************************************
//         Plor multi graph                   
//***************************************************
//***************************************************

void plot_data(TVectorD valuesin, TVectorD errorsin, 
TVectorD valuesout, TVectorD errorsout)  {


  TPad* pad = (TPad*)(gPad->GetMother());

  //  TGraphErrors* grp_in  = new TGraphErrors( valuexin,valuesin,errxin,errorsin);
  TGraph* grp_in  = new TGraph( valuexin,valuesin);

  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->SetName("grp_in");

  //  TGraphErrors* grp_out  = new TGraphErrors(valuexout,valuesout,errxout,errorsout);
  TGraph* grp_out  = new TGraph(valuexout,valuesout);

  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out->SetName("grp_out");



  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in);
  grp->Add(grp_out);
  grp->Draw("AP");

  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Asymmetry (ppm)");
  grp->GetXaxis()->CenterTitle();
  grp->GetXaxis()->SetNdivisions(508);
  grp->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.9);
  grp->GetXaxis()->SetTitleOffset(0.9);
  
  pad->Update();
  pad->cd(2);


}

