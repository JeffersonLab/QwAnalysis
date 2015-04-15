//*****************************************************************************************************//
// Author      : Nuruzzaman on 08/06/2012, Transverse N2Delta.
// Last Update : 08/07/2012
// 
//*****************************************************************************************************//
/*
  This macro is to plot only the main detector data.
  It will read the textfile produced by macro nurTransverse.C for  main detector asymmetry and 
  three plots will be generated . 
  Plot 1: Summary plot for MD asymmetries consists of IN, OUT, (IN+OUT)/2 and AVG(IN-OUT).
  Plot 2: MD asymmetries consists of AVG(IN-OUT).
  Plot 3: Sum and differences of (IN+OUT)/2 and AVG(IN-OUT).

  ###############################################
  
  Slug averages of Main Detector Asymmetries
  
  ###############################################

  Do the following to run the macro in a terminal.
  ./plotTransverseN2Delta

  Enter target type (Just Hit ENTER to choose default):
  1. Liquid Hydrogen (deafult)
  2. 4% DS Al
  3. 1.6% DS Carbon
  1
  Enter data type (Just Hit ENTER to choose default):
  1. Longitudinal
  2. Vertical Transverse (deafult)
  3. Horizontal Transverse
  2
  Enter reaction type (Just Hit ENTER to choose default):
  1. elastic
  2. N->Delta (deafult)
  3. DIS
  2
  Enter QTOR current (Just Hit ENTER to choose default)
  1. 6000 A
  2. 6700 A (deafult)
  3. 7300 A
  Enter regression On or OFF (Just Hit ENTER to choose default)
  0. OFF
  1. ON (deafult)
  Enter regression type (Just Hit ENTER to choose default)
  0. off
  1. on
  2. on_5+1 (deafult)
  3. set3
  4. set4
  5. set5
  6. set6
  7. set7
  8. set8
  9. set9
  10. set10

  You will be promted to enter the target type, 
  To compile this code do a make.

******************************************************************************************************/


using namespace std;
#include "NurClass.h"

//  void plotTransverseN2Delta(){
//  }

int main(Int_t argc,Char_t* argv[]){

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  Bool_t CUT_FIGURE = 1;
  Bool_t FINAL_FIGURE = 1;
  Bool_t FIGURE = 0;

  Double_t figSize = 1.0;

  //   TString database="qw_run2_pass1";
  //   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";

  TString output;
  TString outputAvg;

  std::ofstream Myfile3;
  std::ofstream MyfileWrite;

  TString target, polar,targ, goodfor, reg_set, reg_calc;

  Int_t opt =1;
  Int_t datopt = 3;
  Int_t ropt = 2;
  Int_t qtor_opt = 2;
  Int_t runlet_sulg_opt = 2;

  TString good_for;
  TString qtor_current;
  TString qtor_stem;
  TString good;
  TString interaction;
  TString showFit1,showFit2,showFit3;

  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[2] ={1200*figSize,650*figSize};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  //  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
  //   Double_t yScale[2] = {-10.0,10.0};


  Double_t x_lo_stat_in4,y_lo_stat_in4,x_hi_stat_in4,y_hi_stat_in4,x_lo_stat_out4,y_lo_stat_out4,x_hi_stat_out4,y_hi_stat_out4;
  x_lo_stat_in4=0.76;y_lo_stat_in4=0.64;x_hi_stat_in4=0.99;y_hi_stat_in4=0.95;
  x_lo_stat_out4=0.76;y_lo_stat_out4=0.30;x_hi_stat_out4=0.99;y_hi_stat_out4=0.61;
  Double_t x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4;
  x_lo_leg4=0.76;y_lo_leg4=0.10;x_hi_leg4=0.99;y_hi_leg4=0.27;

  Double_t tsiz,tsiz3,tsiz4,tsiz5,tll,tlr,ps1,ps2,ps3,ps4,ps5;
  //   tsiz=0.45;tsiz3=0.40;tsiz4=0.35;tsiz5=0.30;tll=0.012;tlr=0.4;ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;ps5=0.99;
  tsiz=0.45;tsiz3=0.50;tsiz4=0.35;tsiz5=0.30;tll=0.012;tlr=0.4;ps1=0.005;ps2=0.935;ps3=0.945;ps4=0.995;ps5=0.99;


  //  Int_t argc;Char_t* argv[400];

  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form(" \nSlug averages of Main Detector Asymmetries \n")<<std::endl;
  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form("Enter target type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. %sLiquid Hydrogen (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("2. 4% DS Al ")<<std::endl;
  std::cout<<Form("3. 1.6% DS Carbon ")<<std::endl;
  //   std::cin>>opt;
  std::string input_opt;
  std::getline( std::cin, input_opt );
  if ( !input_opt.empty() ) {
    std::istringstream stream( input_opt );
    stream >> opt;
  }

  std::cout<<Form("Enter data type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. Longitudinal")<<std::endl;
  std::cout<<Form("2. Vertical Transverse")<<std::endl;
  std::cout<<Form("3. %sHorizontal Transverse (deafult)%s",blue,normal)<<std::endl;
  //   std::cin>>datopt;
  std::string input_datopt;
  std::getline( std::cin, input_datopt );
  if ( !input_datopt.empty() ) {
    std::istringstream stream( input_datopt );
    stream >> datopt;
  }

  // Fixing reaction type to N-to-Delta
  //  ropt = 2;
  std::cout<<Form("Enter reaction type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. elastic")<<std::endl;
  std::cout<<Form("2. %sN->Delta (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("3. DIS ")<<std::endl;
  //   std::cin>>ropt;
  std::string input_ropt;
  std::getline( std::cin, input_ropt );
  if ( !input_ropt.empty() ) {
    std::istringstream stream( input_ropt );
    stream >> ropt;
  }


  if(ropt==2){
    std::cout<<Form("Enter QTOR current (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
    std::cout<<Form("1.6000 A ")<<std::endl;
    std::cout<<Form("2.%s6700 A (deafult)%s",blue,normal)<<std::endl;
    std::cout<<Form("3.7300 A ")<<std::endl;
    //   std::cin>>qtor_opt;
    std::string input_qtor_opt;
    std::getline( std::cin, input_qtor_opt );
    if ( !input_qtor_opt.empty() ) {
      std::istringstream stream( input_qtor_opt );
      stream >> qtor_opt;
    }
  }

  Int_t regSwitch = 1;
  std::cout<<Form("Enter regression On or OFF (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("0. OFF")<<std::endl;
  std::cout<<Form("1. %sON (deafult)%s",blue,normal)<<std::endl;
  //   std::cin>>regSwitch;
  std::string input_regSwitch;
  std::getline( std::cin, input_regSwitch );
  if ( !input_regSwitch.empty() ) {
    std::istringstream stream( input_regSwitch );
    stream >> regSwitch;
  }

  // Fixing regression type to on_5+1
  Int_t regID = 2;
  std::cout<<Form("Enter regression type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form(" 0. off"     )<<std::endl;
  std::cout<<Form(" 1. on "     )<<std::endl;
  std::cout<<Form(" 2. %son_5+1 (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form(" 3. set3 "   )<<std::endl;
  std::cout<<Form(" 4. set4 "   )<<std::endl;
  std::cout<<Form(" 5. set5 "   )<<std::endl;
  std::cout<<Form(" 6. set6 "   )<<std::endl;
  std::cout<<Form(" 7. set7 "   )<<std::endl;
  std::cout<<Form(" 8. set8 "   )<<std::endl;
  std::cout<<Form(" 9. set9 "   )<<std::endl;
  std::cout<<Form("10. set10 "  )<<std::endl;
  std::cout<<Form("11. set11 "  )<<std::endl;
  std::cout<<Form("12. set12 "  )<<std::endl;
  std::cout<<Form("13. set13 "  )<<std::endl;
  //   std::cin>>regID;
  std::string input_regID;
  std::getline( std::cin, input_regID );
  if ( !input_regID.empty() ) {
    std::istringstream stream( input_regID );
    stream >> regID;
  }

  std::cout<<Form("Enter output type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. Slug averaged quantity")<<std::endl;
  std::cout<<Form("2. %sRunlet averaged quantity (deafult)%s",blue,normal)<<std::endl;
  std::string input_runlet_sulg_opt;
  std::getline( std::cin, input_runlet_sulg_opt );
  if ( !input_runlet_sulg_opt.empty() ) {
    std::istringstream stream( input_runlet_sulg_opt );
    stream >> runlet_sulg_opt;
  }
  if(runlet_sulg_opt == 1){outputAvg = "SlugAvg";}
  if(runlet_sulg_opt == 2){outputAvg = "RunletAvg";}
  if(runlet_sulg_opt  > 2){printf("%sWrong choise of output \n%s",red,normal); exit(0);}


  TString device,deviceTitle, deviceName; 

  device = "Beam Parameter"; deviceTitle = "Differences"; deviceName = "Differences"; 


  //   std::cout<<Form("Enter device (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  //   std::cout<<Form("1. %sMain Detector (deafult)%s",blue,normal)<<std::endl;
  //   std::cout<<Form("2. Downstream Lumi")<<std::endl;
  //   std::string input_device_opt;
  //   std::getline( std::cin, input_device_opt );
  //   if ( !input_device_opt.empty() ) {
  //     std::istringstream stream( input_device_opt );
  //     stream >> device_opt;
  //   }
  //   if(device_opt == 1){device = "MD"; deviceTitle = "Barsum"; deviceName = "Barsum"; YRange[0]=-35; YRange[1]=35; }
  //   if(device_opt == 2){device = "USLumi"; deviceTitle = "Sum"; deviceName = "Sum"; YRange[0]=-20; YRange[1]=20;}
  //   if(device_opt  > 2){printf("%sWrong choise of device \n%s",red,normal); exit(0);}

  /**********************************************************/


  if(argc>1) database = argv[1];

  // select the target
  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 %% Al";
  }
  else if(opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 %% Carbon";
  }
  else{
    std::cout<<Form("Unknown target type!")<<std::endl;
    exit(1);
  }


  // Select the polarization
  if(datopt == 1){
    polar = "longitudinal";
    good = "3";
  }
  else if(datopt == 2){
    polar = "v_transverse";
    good = "8";
  }
  else if(datopt == 3){
    polar = "h_transverse";
    good = "9";
  }
  else if(datopt == 4){
    polar = "h+v_transverse";
    good = "8,9";
  }
  else{
    std::cout<<Form("Unknown polarization type!")<<std::endl;
    exit(1);
  }
  
  // Select the interaction
  if(ropt == 1){
    good_for = "(md_data_view.good_for_id = '3' or md_data_view.good_for_id = '1,"+good+"')";
    interaction = "elastic";
    qtor_current=" (slow_controls_settings.qtor_current>8800 && slow_controls_settings.qtor_current<9000) ";
    qtor_stem = "8901";
  }
  else if(ropt == 2){
    good_for = "(md_data_view.good_for_id = '"+good+",18')";
    interaction = "n-to-delta"; 
  }
  else if(ropt == 3){
    good_for = "(md_data_view.good_for_id = '1,3,21')";
    qtor_current=" (slow_controls_settings.qtor_current>8990 && slow_controls_settings.qtor_current<9010) ";
    qtor_stem = "9000";
    interaction = "dis"; 
  }
  else{
    std::cout<<Form("Unknown interaction type!")<<std::endl;
    exit(1);
  }

  // QTOR current cut
  if(qtor_opt == 1){
    qtor_current=" (slow_controls_settings.qtor_current>5900 && slow_controls_settings.qtor_current<6100) ";
    qtor_stem = "6000";
  }
  else if(qtor_opt == 2){
    qtor_current=" (slow_controls_settings.qtor_current>6600 && slow_controls_settings.qtor_current<6800) ";
    qtor_stem = "6700";
  }
  else if(qtor_opt == 3){
    qtor_current=" (slow_controls_settings.qtor_current>7200 && slow_controls_settings.qtor_current<7400) ";
    qtor_stem = "7300";
  }


  // regression set
  if(regSwitch==0){
    switch(regID)
      {
      case    0: reg_calc="off";       reg_set="off";   break;
      case    1: reg_calc="on";        reg_set="off";   break;
      case    2: reg_calc="on_5+1";    reg_set="off";   break;
      case    3: reg_calc="on_set3";   reg_set="off";   break;
      case    4: reg_calc="on_set4";   reg_set="off";   break;
      case    5: reg_calc="on_set5";   reg_set="off";   break;
      case    6: reg_calc="on_set6";   reg_set="off";   break;
      case    7: reg_calc="on_set7";   reg_set="off";   break;
      case    8: reg_calc="on_set8";   reg_set="off";   break;
      case    9: reg_calc="on_set9";   reg_set="off";   break;
      case   10: reg_calc="on_set10";  reg_set="off";   break;
      case   11: reg_calc="on_set11";  reg_set="off";   break;
      case   12: reg_calc="on_set12";  reg_set="off";   break;
      case   13: reg_calc="on_set13";  reg_set="off";   break;
      default  : printf("Please insert correct regression information. Exiting program !!!\n"); exit(1); break;
      }
  }

  else{
    switch(regID)
      {
      case    0: reg_calc="off";  reg_set="off";        break;
      case    1: reg_calc="off";  reg_set="on";         break;
      case    2: reg_calc="off";  reg_set="on_5+1";     break;
      case    3: reg_calc="off";  reg_set="on_set3";    break;
      case    4: reg_calc="off";  reg_set="on_set4";    break;
      case    5: reg_calc="off";  reg_set="on_set5";    break;
      case    6: reg_calc="off";  reg_set="on_set6";    break;
      case    7: reg_calc="off";  reg_set="on_set7";    break;
      case    8: reg_calc="off";  reg_set="on_set8";    break;
      case    9: reg_calc="off";  reg_set="on_set9";    break;
      case   10: reg_calc="off";  reg_set="on_set10";   break;
      case   11: reg_calc="off";  reg_set="on_set11";   break;
      case   12: reg_calc="off";  reg_set="on_set12";   break;
      case   13: reg_calc="off";  reg_set="on_set13";   break;
      default  : printf("Please insert correct regression information. Exiting program !!!\n"); exit(1); break;
      }
  }



  TApplication theApp("App",&argc,argv);

  /**********************************************************/

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.5);
  
  //Pad parameters
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.8);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);

  //Set fonts
  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");
  
  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");


  // Fit function to show 
  showFit1 = "FIT = A_{M} cos(#phi) - C sin(#phi) + D";
  showFit2 = "FIT = A_{M} cos(#phi + #phi_{0}) + C";
  showFit3 = "FIT = A_{M} sin(#phi + #phi_{0}) + C";



  TString title11;
  TString titleInOut = Form("%s (%s, %s A): Regression-%s %s %s. No cuts applied."
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());
  TString titleInOut2 = Form("%s (%s, %s A): Regression-%s %s %s vs Runlets. No cuts applied."
			     ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());

  if(datopt==1) title11 = Form("%s %s",titleInOut.Data(),showFit1.Data());
  else if(datopt==2) title11 = Form("%s %s",titleInOut.Data(),showFit2.Data());
  else title11 = Form("%s %s",titleInOut.Data(),showFit3.Data());

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  //   gStyle->SetOptStat(0000000);
  gStyle->SetOptStat(1);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.20);
  gStyle->SetStatH(0.20);
  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatColor(0);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  //   gStyle->SetNdivisions(507,"x");

  // histo parameters
  gStyle->SetTitleYOffset(0.80);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gDirectory->Delete("*");



  Double_t YRangePos[2],YRangeAng[2],YRangeE[2],YRangeQ[2];
  YRangePos[0] = -3e2;   YRangePos[1] = 3e2;
  YRangeAng[0] = -0.01e3;  YRangeAng[1] = 0.01e3;
  YRangeE[0]   = -0.1e3;   YRangeE[1]   = 0.1e3;
  YRangeQ[0]   = -10e3;    YRangeQ[1]   = 10e3;

  TFile *f = new TFile(Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_%s_diff_%s.root"
			    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			    ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

  //   TFile *f = new TFile("transverseN2deltaDiff.root");
  if(!f->IsOpen()) {
    printf("%sUnable to find Rootfile%s\n",blue,normal);     
    exit(1);
  }

  TTree *Hel_Tree = (TTree*)f->Get("Hel_Tree");
  if(Hel_Tree == NULL) {printf("%sUnable to find %sHel_Tree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}
  //   TChain * tree = new TChain("Hel_Tree");


  TH1 *histDiffInX, *histDiffOutX, *histDiffInY, *histDiffOutY;
  TH1 *histDiffInXSlope, *histDiffOutXSlope, *histDiffInYSlope, *histDiffOutYSlope;
  TH1 *histDiffInE, *histDiffOutE, *histDiffInQ, *histDiffOutQ;
  //   char diffInX[255], diffOutX[255], diffInY[255], diffOutY[255];
  //   char diffInXSlope[255], diffOutXSlope[255], diffInYSlope[255], diffOutYSlope[255];
  //   char diffInE[255], diffOutE[255], diffInQ[255], diffOutQ[255];

  TH1 *histDiffRunInX, *histDiffRunOutX, *histDiffRunInY, *histDiffRunOutY;
  TH1 *histDiffRunInXSlope, *histDiffRunOutXSlope, *histDiffRunInYSlope, *histDiffRunOutYSlope;
  TH1 *histDiffRunInE, *histDiffRunOutE, *histDiffRunInQ, *histDiffRunOutQ;
  //   char diffRunInX[255], diffRunOutX[255], diffRunInY[255], diffRunOutY[255];
  //   char diffRunInXSlope[255], diffRunOutXSlope[255], diffRunInYSlope[255], diffRunOutYSlope[255];
  //   char diffRunInE[255], diffRunOutE[255], diffRunInQ[255], diffRunOutQ[255];


  cout<<blue<<__LINE__<<normal<<endl;


  /*********************************************************************/
  /*********************************************************************/
  if(CUT_FIGURE){

    TCanvas * c11 = new TCanvas("c11", titleInOut,0,0,1600,1000);
    c11->Draw();
    c11->SetBorderSize(0);
    c11->cd();
    TPad*pad111 = new TPad("pad111","pad111",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad112 = new TPad("pad112","pad112",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad111->SetFillColor(kWhite);
    pad111->Draw();
    pad112->Draw();
    pad111->cd();
    TLatex * t11 = new TLatex(0.00,0.3,Form("%s",titleInOut.Data()));
    t11->SetTextSize(0.45);
    t11->Draw();
    pad112->cd();
    pad112->Divide(2,3);


    pad112->cd(1);
    Hel_Tree->Draw("diff*1e6>>diffInX","id==1 && hwp==1","goff");
    histDiffInX = (TH1 *)gDirectory->Get("diffInX");
    histDiffInX->Draw("");
    histDiffInX->SetFillColor(kRed);
    histDiffInX->GetXaxis()->CenterTitle();
    histDiffInX->GetYaxis()->CenterTitle();
    histDiffInX->SetXTitle("diffX [nm]");
    histDiffInX->SetYTitle("Number of Runlets");
    histDiffInX->SetTitle("");
    histDiffInX->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad112->cd(2);
    Hel_Tree->Draw("diff*1e9>>diffInXSlope","id==3 && hwp==1","goff");
    histDiffInXSlope = (TH1 *)gDirectory->Get("diffInXSlope");
    histDiffInXSlope->Draw("");
    histDiffInXSlope->SetFillColor(kRed);
    histDiffInXSlope->GetXaxis()->CenterTitle();
    histDiffInXSlope->GetYaxis()->CenterTitle();
    histDiffInXSlope->SetXTitle("diffXSlope [nrad]");
    histDiffInXSlope->SetYTitle("Number of Runlets");
    histDiffInXSlope->SetTitle("");
    histDiffInXSlope->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad112->cd(3);
    Hel_Tree->Draw("diff*1e6>>diffInY","id==2 && hwp==1","goff");
    histDiffInY = (TH1 *)gDirectory->Get("diffInY");
    histDiffInY->Draw("");
    histDiffInY->SetFillColor(kGreen);
    histDiffInY->GetXaxis()->CenterTitle();
    histDiffInY->GetYaxis()->CenterTitle();
    histDiffInY->SetXTitle("diffY [nm]");
    histDiffInY->SetYTitle("Number of Runlets");
    histDiffInY->SetTitle("");
    histDiffInY->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad112->cd(4);
    Hel_Tree->Draw("diff*1e9>>diffInYSlope","id==4 && hwp==1","goff");
    histDiffInYSlope = (TH1 *)gDirectory->Get("diffInYSlope");
    histDiffInYSlope->Draw("");
    histDiffInYSlope->SetFillColor(kGreen);
    histDiffInYSlope->GetXaxis()->CenterTitle();
    histDiffInYSlope->GetYaxis()->CenterTitle();
    histDiffInYSlope->SetXTitle("diffYSlope [nrad]");
    histDiffInYSlope->SetYTitle("Number of Runlets");
    histDiffInYSlope->SetTitle("");
    histDiffInYSlope->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad112->cd(5);
    Hel_Tree->Draw("diff*1e9>>diffInE","id==5 && hwp==1","goff");
    histDiffInE = (TH1 *)gDirectory->Get("diffInE");
    histDiffInE->Draw("");
    histDiffInE->SetFillColor(kBlue);
    histDiffInE->GetXaxis()->CenterTitle();
    histDiffInE->GetYaxis()->CenterTitle();
    histDiffInE->SetXTitle("diffE [ppb]");
    histDiffInE->SetYTitle("Number of Runlets");
    histDiffInE->SetTitle("");
    histDiffInE->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad112->cd(6);
    Hel_Tree->Draw("diff*1e9>>diffInQ","id==6 && hwp==1","goff");
    histDiffInQ = (TH1 *)gDirectory->Get("diffInQ");
    histDiffInQ->Draw("");
    histDiffInQ->SetFillColor(kMagenta);
    histDiffInQ->GetXaxis()->CenterTitle();
    histDiffInQ->GetYaxis()->CenterTitle();
    histDiffInQ->SetXTitle("diffCharge [ppb]");
    histDiffInQ->SetYTitle("Number of Runlets");
    histDiffInQ->SetTitle("");
    histDiffInQ->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();


    TString savePlotDiffIn = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_diffIn_%s"
				  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				  ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c11-> Update();
    c11->Print(savePlotDiffIn+".png");
    if(FIGURE){
      c11->Print(savePlotDiffIn+".svg");
      c11->Print(savePlotDiffIn+".C");
    }


    /*********************************************************************/

    TCanvas * c12 = new TCanvas("c12", titleInOut,0,0,1600,1000);
    c12->Draw();
    c12->SetBorderSize(0);
    c12->cd();
    TPad*pad121 = new TPad("pad121","pad121",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad122 = new TPad("pad122","pad122",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad121->SetFillColor(kWhite);
    pad121->Draw();
    pad122->Draw();
    pad121->cd();
    TLatex * t12 = new TLatex(0.00,0.3,Form("%s",titleInOut.Data()));
    t12->SetTextSize(0.45);
    t12->Draw();
    pad122->cd();
    pad122->Divide(2,3);

    pad122->cd(1);
    Hel_Tree->Draw("diff*1e6>>diffOutX","id==1 && hwp==0","goff");
    histDiffOutX = (TH1 *)gDirectory->Get("diffOutX");
    histDiffOutX->Draw("");
    histDiffOutX->SetLineColor(kRed);
    histDiffOutX->GetXaxis()->CenterTitle();
    histDiffOutX->GetYaxis()->CenterTitle();
    histDiffOutX->SetXTitle("diffX [nm]");
    histDiffOutX->SetYTitle("Number of Runlets");
    histDiffOutX->SetTitle("");
    histDiffOutX->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad122->cd(2);
    Hel_Tree->Draw("diff*1e9>>diffOutXSlope","id==3 && hwp==0","goff");
    histDiffOutXSlope = (TH1 *)gDirectory->Get("diffOutXSlope");
    histDiffOutXSlope->Draw("");
    histDiffOutXSlope->SetLineColor(kRed);
    histDiffOutXSlope->GetXaxis()->CenterTitle();
    histDiffOutXSlope->GetYaxis()->CenterTitle();
    histDiffOutXSlope->SetXTitle("diffXSlope [nrad]");
    histDiffOutXSlope->SetYTitle("Number of Runlets");
    histDiffOutXSlope->SetTitle("");
    histDiffOutXSlope->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad122->cd(3);
    Hel_Tree->Draw("diff*1e6>>diffOutY","id==2 && hwp==0","goff");
    histDiffOutY = (TH1 *)gDirectory->Get("diffOutY");
    histDiffOutY->Draw("");
    histDiffOutY->SetLineColor(kGreen);
    histDiffOutY->GetXaxis()->CenterTitle();
    histDiffOutY->GetYaxis()->CenterTitle();
    histDiffOutY->SetXTitle("diffY [nm]");
    histDiffOutY->SetYTitle("Number of Runlets");
    histDiffOutY->SetTitle("");
    histDiffOutY->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad122->cd(4);
    Hel_Tree->Draw("diff*1e9>>diffOutYSlope","id==4 && hwp==0","goff");
    histDiffOutYSlope = (TH1 *)gDirectory->Get("diffOutYSlope");
    histDiffOutYSlope->Draw("");
    histDiffOutYSlope->SetLineColor(kGreen);
    histDiffOutYSlope->GetXaxis()->CenterTitle();
    histDiffOutYSlope->GetYaxis()->CenterTitle();
    histDiffOutYSlope->SetXTitle("diffYSlope [nrad]");
    histDiffOutYSlope->SetYTitle("Number of Runlets");
    histDiffOutYSlope->SetTitle("");
    histDiffOutYSlope->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad122->cd(5);
    Hel_Tree->Draw("diff*1e9>>diffOutE","id==5 && hwp==0","goff");
    histDiffOutE = (TH1 *)gDirectory->Get("diffOutE");
    histDiffOutE->Draw("");
    histDiffOutE->SetLineColor(kBlue);
    histDiffOutE->GetXaxis()->CenterTitle();
    histDiffOutE->GetYaxis()->CenterTitle();
    histDiffOutE->SetXTitle("diffE [ppb]");
    histDiffOutE->SetYTitle("Number of Runlets");
    histDiffOutE->SetTitle("");
    histDiffOutE->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad122->cd(6);
    Hel_Tree->Draw("diff*1e9>>diffOutQ","id==6 && hwp==0","goff");
    histDiffOutQ = (TH1 *)gDirectory->Get("diffOutQ");
    histDiffOutQ->Draw("");
    histDiffOutQ->SetLineColor(kMagenta);
    histDiffOutQ->GetXaxis()->CenterTitle();
    histDiffOutQ->GetYaxis()->CenterTitle();
    histDiffOutQ->SetXTitle("diffCharge [ppb]");
    histDiffOutQ->SetYTitle("Number of Runlets");
    histDiffOutQ->SetTitle("");
    histDiffOutQ->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();


    TString savePlotDiffOut = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_diffOut_%s"
				   ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				   ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c12-> Update();
    c12->Print(savePlotDiffOut+".png");
    if(FIGURE){
      c12->Print(savePlotDiffOut+".svg");
      c12->Print(savePlotDiffOut+".C");
    }

    /*********************************************************************/

    Double_t meanInDiffX = histDiffInX->GetMean();   Double_t errorInDiffX = histDiffInX->GetMeanError(); Double_t widthInDiffX = histDiffInX->GetRMS();
    Double_t meanInDiffY = histDiffInY->GetMean();   Double_t errorInDiffY = histDiffInY->GetMeanError(); Double_t widthInDiffY = histDiffInY->GetRMS();
    Double_t meanInDiffXSlope = histDiffInXSlope->GetMean();   Double_t errorInDiffXSlope = histDiffInXSlope->GetMeanError(); Double_t widthInDiffXSlope = histDiffInXSlope->GetRMS();
    Double_t meanInDiffYSlope = histDiffInYSlope->GetMean();   Double_t errorInDiffYSlope = histDiffInYSlope->GetMeanError(); Double_t widthInDiffYSlope = histDiffInYSlope->GetRMS();
    Double_t meanInDiffE = histDiffInE->GetMean();   Double_t errorInDiffE = histDiffInE->GetMeanError(); Double_t widthInDiffE = histDiffInE->GetRMS();
    Double_t meanInDiffQ = histDiffInQ->GetMean();   Double_t errorInDiffQ = histDiffInQ->GetMeanError(); Double_t widthInDiffQ = histDiffInQ->GetRMS();

    cout<<"IN X : "<<meanInDiffX<<"+-"<<errorInDiffX<<" "<<widthInDiffX<<endl;
    cout<<"IN Y : "<<meanInDiffY<<"+-"<<errorInDiffY<<" "<<widthInDiffY<<endl;
    cout<<"IN XSlope : "<<meanInDiffXSlope<<"+-"<<errorInDiffXSlope<<" "<<widthInDiffXSlope<<endl;
    cout<<"IN YSlope : "<<meanInDiffYSlope<<"+-"<<errorInDiffYSlope<<" "<<widthInDiffYSlope<<endl;
    cout<<"IN E : "<<meanInDiffE<<"+-"<<errorInDiffE<<" "<<widthInDiffE<<endl;
    cout<<"IN Q : "<<meanInDiffQ<<"+-"<<errorInDiffQ<<" "<<widthInDiffQ<<endl;

    Double_t meanOutDiffX = histDiffOutX->GetMean();   Double_t errorOutDiffX = histDiffOutX->GetMeanError(); Double_t widthOutDiffX = histDiffOutX->GetRMS();
    Double_t meanOutDiffY = histDiffOutY->GetMean();   Double_t errorOutDiffY = histDiffOutY->GetMeanError(); Double_t widthOutDiffY = histDiffOutY->GetRMS();
    Double_t meanOutDiffXSlope = histDiffOutXSlope->GetMean();   Double_t errorOutDiffXSlope = histDiffOutXSlope->GetMeanError(); Double_t widthOutDiffXSlope = histDiffOutXSlope->GetRMS();
    Double_t meanOutDiffYSlope = histDiffOutYSlope->GetMean();   Double_t errorOutDiffYSlope = histDiffOutYSlope->GetMeanError(); Double_t widthOutDiffYSlope = histDiffOutYSlope->GetRMS();
    Double_t meanOutDiffE = histDiffOutE->GetMean();   Double_t errorOutDiffE = histDiffOutE->GetMeanError(); Double_t widthOutDiffE = histDiffOutE->GetRMS();
    Double_t meanOutDiffQ = histDiffOutQ->GetMean();   Double_t errorOutDiffQ = histDiffOutQ->GetMeanError(); Double_t widthOutDiffQ = histDiffOutQ->GetRMS();

    cout<<"OUT X : "<<meanOutDiffX<<"+-"<<errorOutDiffX<<" "<<widthOutDiffX<<endl;
    cout<<"OUT Y : "<<meanOutDiffY<<"+-"<<errorOutDiffY<<" "<<widthOutDiffY<<endl;
    cout<<"OUT XSlope : "<<meanOutDiffXSlope<<"+-"<<errorOutDiffXSlope<<" "<<widthOutDiffXSlope<<endl;
    cout<<"OUT YSlope : "<<meanOutDiffYSlope<<"+-"<<errorOutDiffYSlope<<" "<<widthOutDiffYSlope<<endl;
    cout<<"OUT E : "<<meanOutDiffE<<"+-"<<errorOutDiffE<<" "<<widthOutDiffE<<endl;
    cout<<"OUT Q : "<<meanOutDiffQ<<"+-"<<errorOutDiffQ<<" "<<widthOutDiffQ<<endl;


  std::ofstream MyfileOut;
  MyfileOut.open(Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_%s_cutDependenceWidth_%s.txt"
			    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			    ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

  MyfileOut<<Form("%4.2f %4.2f %4.2f %4.2f %4.2f %4.2f\t %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f"
		  ,widthInDiffX,widthInDiffY,widthInDiffXSlope,widthInDiffYSlope,widthInDiffE,widthInDiffQ
		  ,widthOutDiffX,widthOutDiffY,widthOutDiffXSlope,widthOutDiffYSlope,widthOutDiffE,widthOutDiffQ)
		  <<endl;
  MyfileOut.close();


    Double_t tLineInRunStart, tLineInRunStop, tLineOutRunStart, tLineOutRunStop;
    Double_t tTextInRun, tTextOutRun;


    if(datopt == 1){
      tLineInRunStart = 16130.9;   tLineInRunStop  = 16132.2;
      tLineOutRunStart = 16128.9;  tLineOutRunStop  = 16130.2;
      tTextInRun = 16132.21;       tTextOutRun  = 16130.21;
    }
    else if(datopt == 2){
      tLineInRunStart = 16065.99;   tLineInRunStop  = 16066.13;
      tLineOutRunStart = 16065.025;  tLineOutRunStop  = 16065.125;
      tTextInRun = 16066.132;       tTextOutRun  = 16065.127;
    }
    else if(datopt == 3){
      tLineInRunStart = 16130.9;   tLineInRunStop  = 16132.2;
      tLineOutRunStart = 16128.9;  tLineOutRunStop  = 16130.2;
      tTextInRun = 16132.21;       tTextOutRun  = 16130.21;
    }
    else{
      std::cout<<Form("Unknown polarization type!")<<std::endl;
      exit(1);
    }
  

    //   Double_t tLineInRunStart = 16130.9; Double_t tLineInRunStop  = 16132.2;
    //   Double_t tLineOutRunStart = 16128.9; Double_t tLineOutRunStop  = 16130.2;
    //   Double_t tTextInRun = 16132.21; Double_t tTextOutRunStop  = 16130.21;

    gStyle->SetLineStyle(2);
    gStyle->SetLineColor(kGray);

    TLine * sigma2InPosX = new TLine(tLineInRunStart, 2*widthInDiffX, tLineInRunStop, 2*widthInDiffX);
    TLine * sigma2InNegX = new TLine(tLineInRunStart, -2*widthInDiffX, tLineInRunStop, -2*widthInDiffX);
    TLine * sigma2InPosY = new TLine(tLineInRunStart, 2*widthInDiffY, tLineInRunStop, 2*widthInDiffY);
    TLine * sigma2InNegY = new TLine(tLineInRunStart, -2*widthInDiffY, tLineInRunStop, -2*widthInDiffY);
    TLine * sigma2InPosXSlope = new TLine(tLineInRunStart, 2*widthInDiffXSlope, tLineInRunStop, 2*widthInDiffXSlope);
    TLine * sigma2InNegXSlope = new TLine(tLineInRunStart, -2*widthInDiffXSlope, tLineInRunStop, -2*widthInDiffXSlope);
    TLine * sigma2InPosYSlope = new TLine(tLineInRunStart, 2*widthInDiffYSlope, tLineInRunStop, 2*widthInDiffYSlope);
    TLine * sigma2InNegYSlope = new TLine(tLineInRunStart, -2*widthInDiffYSlope, tLineInRunStop, -2*widthInDiffYSlope);
    TLine * sigma2InPosE = new TLine(tLineInRunStart, 2*widthInDiffE, tLineInRunStop, 2*widthInDiffE);
    TLine * sigma2InNegE = new TLine(tLineInRunStart, -2*widthInDiffE, tLineInRunStop, -2*widthInDiffE);
    TLine * sigma2InPosQ = new TLine(tLineInRunStart, 2*widthInDiffQ, tLineInRunStop, 2*widthInDiffQ);
    TLine * sigma2InNegQ = new TLine(tLineInRunStart, -2*widthInDiffQ, tLineInRunStop, -2*widthInDiffQ);

    TLine * sigma2OutPosX = new TLine(tLineOutRunStart, 2*widthOutDiffX, tLineOutRunStop, 2*widthOutDiffX);
    TLine * sigma2OutNegX = new TLine(tLineOutRunStart, -2*widthOutDiffX, tLineOutRunStop, -2*widthOutDiffX);
    TLine * sigma2OutPosY = new TLine(tLineOutRunStart, 2*widthOutDiffY, tLineOutRunStop, 2*widthOutDiffY);
    TLine * sigma2OutNegY = new TLine(tLineOutRunStart, -2*widthOutDiffY, tLineOutRunStop, -2*widthOutDiffY);
    TLine * sigma2OutPosXSlope = new TLine(tLineOutRunStart, 2*widthOutDiffXSlope, tLineOutRunStop, 2*widthOutDiffXSlope);
    TLine * sigma2OutNegXSlope = new TLine(tLineOutRunStart, -2*widthOutDiffXSlope, tLineOutRunStop, -2*widthOutDiffXSlope);
    TLine * sigma2OutPosYSlope = new TLine(tLineOutRunStart, 2*widthOutDiffYSlope, tLineOutRunStop, 2*widthOutDiffYSlope);
    TLine * sigma2OutNegYSlope = new TLine(tLineOutRunStart, -2*widthOutDiffYSlope, tLineOutRunStop, -2*widthOutDiffYSlope);
    TLine * sigma2OutPosE = new TLine(tLineOutRunStart, 2*widthOutDiffE, tLineOutRunStop, 2*widthOutDiffE);
    TLine * sigma2OutNegE = new TLine(tLineOutRunStart, -2*widthOutDiffE, tLineOutRunStop, -2*widthOutDiffE);
    TLine * sigma2OutPosQ = new TLine(tLineOutRunStart, 2*widthOutDiffQ, tLineOutRunStop, 2*widthOutDiffQ);
    TLine * sigma2OutNegQ = new TLine(tLineOutRunStart, -2*widthOutDiffQ, tLineOutRunStop, -2*widthOutDiffQ);

    TLine * sigma3InPosX = new TLine(tLineInRunStart, 3*widthInDiffX, tLineInRunStop, 3*widthInDiffX);
    TLine * sigma3InNegX = new TLine(tLineInRunStart, -3*widthInDiffX, tLineInRunStop, -3*widthInDiffX);
    TLine * sigma3InPosY = new TLine(tLineInRunStart, 3*widthInDiffY, tLineInRunStop, 3*widthInDiffY);
    TLine * sigma3InNegY = new TLine(tLineInRunStart, -3*widthInDiffY, tLineInRunStop, -3*widthInDiffY);
    TLine * sigma3InPosXSlope = new TLine(tLineInRunStart, 3*widthInDiffXSlope, tLineInRunStop, 3*widthInDiffXSlope);
    TLine * sigma3InNegXSlope = new TLine(tLineInRunStart, -3*widthInDiffXSlope, tLineInRunStop, -3*widthInDiffXSlope);
    TLine * sigma3InPosYSlope = new TLine(tLineInRunStart, 3*widthInDiffYSlope, tLineInRunStop, 3*widthInDiffYSlope);
    TLine * sigma3InNegYSlope = new TLine(tLineInRunStart, -3*widthInDiffYSlope, tLineInRunStop, -3*widthInDiffYSlope);
    TLine * sigma3InPosE = new TLine(tLineInRunStart, 3*widthInDiffE, tLineInRunStop, 3*widthInDiffE);
    TLine * sigma3InNegE = new TLine(tLineInRunStart, -3*widthInDiffE, tLineInRunStop, -3*widthInDiffE);
    TLine * sigma3InPosQ = new TLine(tLineInRunStart, 3*widthInDiffQ, tLineInRunStop, 3*widthInDiffQ);
    TLine * sigma3InNegQ = new TLine(tLineInRunStart, -3*widthInDiffQ, tLineInRunStop, -3*widthInDiffQ);

    TLine * sigma3OutPosX = new TLine(tLineOutRunStart, 3*widthOutDiffX, tLineOutRunStop, 3*widthOutDiffX);
    TLine * sigma3OutNegX = new TLine(tLineOutRunStart, -3*widthOutDiffX, tLineOutRunStop, -3*widthOutDiffX);
    TLine * sigma3OutPosY = new TLine(tLineOutRunStart, 3*widthOutDiffY, tLineOutRunStop, 3*widthOutDiffY);
    TLine * sigma3OutNegY = new TLine(tLineOutRunStart, -3*widthOutDiffY, tLineOutRunStop, -3*widthOutDiffY);
    TLine * sigma3OutPosXSlope = new TLine(tLineOutRunStart, 3*widthOutDiffXSlope, tLineOutRunStop, 3*widthOutDiffXSlope);
    TLine * sigma3OutNegXSlope = new TLine(tLineOutRunStart, -3*widthOutDiffXSlope, tLineOutRunStop, -3*widthOutDiffXSlope);
    TLine * sigma3OutPosYSlope = new TLine(tLineOutRunStart, 3*widthOutDiffYSlope, tLineOutRunStop, 3*widthOutDiffYSlope);
    TLine * sigma3OutNegYSlope = new TLine(tLineOutRunStart, -3*widthOutDiffYSlope, tLineOutRunStop, -3*widthOutDiffYSlope);
    TLine * sigma3OutPosE = new TLine(tLineOutRunStart, 3*widthOutDiffE, tLineOutRunStop, 3*widthOutDiffE);
    TLine * sigma3OutNegE = new TLine(tLineOutRunStart, -3*widthOutDiffE, tLineOutRunStop, -3*widthOutDiffE);
    TLine * sigma3OutPosQ = new TLine(tLineOutRunStart, 3*widthOutDiffQ, tLineOutRunStop, 3*widthOutDiffQ);
    TLine * sigma3OutNegQ = new TLine(tLineOutRunStart, -3*widthOutDiffQ, tLineOutRunStop, -3*widthOutDiffQ);

    TLine * sigma4InPosX = new TLine(tLineInRunStart, 4*widthInDiffX, tLineInRunStop, 4*widthInDiffX);
    TLine * sigma4InNegX = new TLine(tLineInRunStart, -4*widthInDiffX, tLineInRunStop, -4*widthInDiffX);
    TLine * sigma4InPosY = new TLine(tLineInRunStart, 4*widthInDiffY, tLineInRunStop, 4*widthInDiffY);
    TLine * sigma4InNegY = new TLine(tLineInRunStart, -4*widthInDiffY, tLineInRunStop, -4*widthInDiffY);
    TLine * sigma4InPosXSlope = new TLine(tLineInRunStart, 4*widthInDiffXSlope, tLineInRunStop, 4*widthInDiffXSlope);
    TLine * sigma4InNegXSlope = new TLine(tLineInRunStart, -4*widthInDiffXSlope, tLineInRunStop, -4*widthInDiffXSlope);
    TLine * sigma4InPosYSlope = new TLine(tLineInRunStart, 4*widthInDiffYSlope, tLineInRunStop, 4*widthInDiffYSlope);
    TLine * sigma4InNegYSlope = new TLine(tLineInRunStart, -4*widthInDiffYSlope, tLineInRunStop, -4*widthInDiffYSlope);
    TLine * sigma4InPosE = new TLine(tLineInRunStart, 4*widthInDiffE, tLineInRunStop, 4*widthInDiffE);
    TLine * sigma4InNegE = new TLine(tLineInRunStart, -4*widthInDiffE, tLineInRunStop, -4*widthInDiffE);
    TLine * sigma4InPosQ = new TLine(tLineInRunStart, 4*widthInDiffQ, tLineInRunStop, 4*widthInDiffQ);
    TLine * sigma4InNegQ = new TLine(tLineInRunStart, -4*widthInDiffQ, tLineInRunStop, -4*widthInDiffQ);

    TLine * sigma4OutPosX = new TLine(tLineOutRunStart, 4*widthOutDiffX, tLineOutRunStop, 4*widthOutDiffX);
    TLine * sigma4OutNegX = new TLine(tLineOutRunStart, -4*widthOutDiffX, tLineOutRunStop, -4*widthOutDiffX);
    TLine * sigma4OutPosY = new TLine(tLineOutRunStart, 4*widthOutDiffY, tLineOutRunStop, 4*widthOutDiffY);
    TLine * sigma4OutNegY = new TLine(tLineOutRunStart, -4*widthOutDiffY, tLineOutRunStop, -4*widthOutDiffY);
    TLine * sigma4OutPosXSlope = new TLine(tLineOutRunStart, 4*widthOutDiffXSlope, tLineOutRunStop, 4*widthOutDiffXSlope);
    TLine * sigma4OutNegXSlope = new TLine(tLineOutRunStart, -4*widthOutDiffXSlope, tLineOutRunStop, -4*widthOutDiffXSlope);
    TLine * sigma4OutPosYSlope = new TLine(tLineOutRunStart, 4*widthOutDiffYSlope, tLineOutRunStop, 4*widthOutDiffYSlope);
    TLine * sigma4OutNegYSlope = new TLine(tLineOutRunStart, -4*widthOutDiffYSlope, tLineOutRunStop, -4*widthOutDiffYSlope);
    TLine * sigma4OutPosE = new TLine(tLineOutRunStart, 4*widthOutDiffE, tLineOutRunStop, 4*widthOutDiffE);
    TLine * sigma4OutNegE = new TLine(tLineOutRunStart, -4*widthOutDiffE, tLineOutRunStop, -4*widthOutDiffE);
    TLine * sigma4OutPosQ = new TLine(tLineOutRunStart, 4*widthOutDiffQ, tLineOutRunStop, 4*widthOutDiffQ);
    TLine * sigma4OutNegQ = new TLine(tLineOutRunStart, -4*widthOutDiffQ, tLineOutRunStop, -4*widthOutDiffQ);

    gStyle->SetLineStyle(1);
    gStyle->SetLineColor(kBlack);


    TLatex * t2SigmaPosXInPos = new TLatex(tTextInRun,+2*widthInDiffX,"+2#sigma");
    TLatex * t2SigmaPosXInNeg = new TLatex(tTextInRun,-2*widthInDiffX,"-2#sigma");
    TLatex * t2SigmaPosYInPos = new TLatex(tTextInRun,+2*widthInDiffY,"+2#sigma");
    TLatex * t2SigmaPosYInNeg = new TLatex(tTextInRun,-2*widthInDiffY,"-2#sigma");
    TLatex * t2SigmaPosXSlopeInPos = new TLatex(tTextInRun,+2*widthInDiffXSlope,"+2#sigma");
    TLatex * t2SigmaPosXSlopeInNeg = new TLatex(tTextInRun,-2*widthInDiffXSlope,"-2#sigma");
    TLatex * t2SigmaPosYSlopeInPos = new TLatex(tTextInRun,+2*widthInDiffYSlope,"+2#sigma");
    TLatex * t2SigmaPosYSlopeInNeg = new TLatex(tTextInRun,-2*widthInDiffYSlope,"-2#sigma");
    TLatex * t2SigmaPosEInPos = new TLatex(tTextInRun,+2*widthInDiffE,"+2#sigma");
    TLatex * t2SigmaPosEInNeg = new TLatex(tTextInRun,-2*widthInDiffE,"-2#sigma");
    TLatex * t2SigmaPosQInPos = new TLatex(tTextInRun,+2*widthInDiffQ,"+2#sigma");
    TLatex * t2SigmaPosQInNeg = new TLatex(tTextInRun,-2*widthInDiffQ,"-2#sigma");

    TLatex * t2SigmaPosXOutPos = new TLatex(tTextOutRun,+2*widthOutDiffX,"+2#sigma");
    TLatex * t2SigmaPosXOutNeg = new TLatex(tTextOutRun,-2*widthOutDiffX,"-2#sigma");
    TLatex * t2SigmaPosYOutPos = new TLatex(tTextOutRun,+2*widthOutDiffY,"+2#sigma");
    TLatex * t2SigmaPosYOutNeg = new TLatex(tTextOutRun,-2*widthOutDiffY,"-2#sigma");
    TLatex * t2SigmaPosXSlopeOutPos = new TLatex(tTextOutRun,+2*widthOutDiffXSlope,"+2#sigma");
    TLatex * t2SigmaPosXSlopeOutNeg = new TLatex(tTextOutRun,-2*widthOutDiffXSlope,"-2#sigma");
    TLatex * t2SigmaPosYSlopeOutPos = new TLatex(tTextOutRun,+2*widthOutDiffYSlope,"+2#sigma");
    TLatex * t2SigmaPosYSlopeOutNeg = new TLatex(tTextOutRun,-2*widthOutDiffYSlope,"-2#sigma");
    TLatex * t2SigmaPosEOutPos = new TLatex(tTextOutRun,+2*widthOutDiffE,"+2#sigma");
    TLatex * t2SigmaPosEOutNeg = new TLatex(tTextOutRun,-2*widthOutDiffE,"-2#sigma");
    TLatex * t2SigmaPosQOutPos = new TLatex(tTextOutRun,+2*widthOutDiffQ,"+2#sigma");
    TLatex * t2SigmaPosQOutNeg = new TLatex(tTextOutRun,-2*widthOutDiffQ,"-2#sigma");

    TLatex * t3SigmaPosXInPos = new TLatex(tTextInRun,+3*widthInDiffX,"+3#sigma");
    TLatex * t3SigmaPosXInNeg = new TLatex(tTextInRun,-3*widthInDiffX,"-3#sigma");
    TLatex * t3SigmaPosYInPos = new TLatex(tTextInRun,+3*widthInDiffY,"+3#sigma");
    TLatex * t3SigmaPosYInNeg = new TLatex(tTextInRun,-3*widthInDiffY,"-3#sigma");
    TLatex * t3SigmaPosXSlopeInPos = new TLatex(tTextInRun,+3*widthInDiffXSlope,"+3#sigma");
    TLatex * t3SigmaPosXSlopeInNeg = new TLatex(tTextInRun,-3*widthInDiffXSlope,"-3#sigma");
    TLatex * t3SigmaPosYSlopeInPos = new TLatex(tTextInRun,+3*widthInDiffYSlope,"+3#sigma");
    TLatex * t3SigmaPosYSlopeInNeg = new TLatex(tTextInRun,-3*widthInDiffYSlope,"-3#sigma");
    TLatex * t3SigmaPosEInPos = new TLatex(tTextInRun,+3*widthInDiffE,"+3#sigma");
    TLatex * t3SigmaPosEInNeg = new TLatex(tTextInRun,-3*widthInDiffE,"-3#sigma");
    TLatex * t3SigmaPosQInPos = new TLatex(tTextInRun,+3*widthInDiffQ,"+3#sigma");
    TLatex * t3SigmaPosQInNeg = new TLatex(tTextInRun,-3*widthInDiffQ,"-3#sigma");

    TLatex * t3SigmaPosXOutPos = new TLatex(tTextOutRun,+3*widthOutDiffX,"+3#sigma");
    TLatex * t3SigmaPosXOutNeg = new TLatex(tTextOutRun,-3*widthOutDiffX,"-3#sigma");
    TLatex * t3SigmaPosYOutPos = new TLatex(tTextOutRun,+3*widthOutDiffY,"+3#sigma");
    TLatex * t3SigmaPosYOutNeg = new TLatex(tTextOutRun,-3*widthOutDiffY,"-3#sigma");
    TLatex * t3SigmaPosXSlopeOutPos = new TLatex(tTextOutRun,+3*widthOutDiffXSlope,"+3#sigma");
    TLatex * t3SigmaPosXSlopeOutNeg = new TLatex(tTextOutRun,-3*widthOutDiffXSlope,"-3#sigma");
    TLatex * t3SigmaPosYSlopeOutPos = new TLatex(tTextOutRun,+3*widthOutDiffYSlope,"+3#sigma");
    TLatex * t3SigmaPosYSlopeOutNeg = new TLatex(tTextOutRun,-3*widthOutDiffYSlope,"-3#sigma");
    TLatex * t3SigmaPosEOutPos = new TLatex(tTextOutRun,+3*widthOutDiffE,"+3#sigma");
    TLatex * t3SigmaPosEOutNeg = new TLatex(tTextOutRun,-3*widthOutDiffE,"-3#sigma");
    TLatex * t3SigmaPosQOutPos = new TLatex(tTextOutRun,+3*widthOutDiffQ,"+3#sigma");
    TLatex * t3SigmaPosQOutNeg = new TLatex(tTextOutRun,-3*widthOutDiffQ,"-3#sigma");

    TLatex * t4SigmaPosXInPos = new TLatex(tTextInRun,+4*widthInDiffX,"+4#sigma");
    TLatex * t4SigmaPosXInNeg = new TLatex(tTextInRun,-4*widthInDiffX,"-4#sigma");
    TLatex * t4SigmaPosYInPos = new TLatex(tTextInRun,+4*widthInDiffY,"+4#sigma");
    TLatex * t4SigmaPosYInNeg = new TLatex(tTextInRun,-4*widthInDiffY,"-4#sigma");
    TLatex * t4SigmaPosXSlopeInPos = new TLatex(tTextInRun,+4*widthInDiffXSlope,"+4#sigma");
    TLatex * t4SigmaPosXSlopeInNeg = new TLatex(tTextInRun,-4*widthInDiffXSlope,"-4#sigma");
    TLatex * t4SigmaPosYSlopeInPos = new TLatex(tTextInRun,+4*widthInDiffYSlope,"+4#sigma");
    TLatex * t4SigmaPosYSlopeInNeg = new TLatex(tTextInRun,-4*widthInDiffYSlope,"-4#sigma");
    TLatex * t4SigmaPosEInPos = new TLatex(tTextInRun,+4*widthInDiffE,"+4#sigma");
    TLatex * t4SigmaPosEInNeg = new TLatex(tTextInRun,-4*widthInDiffE,"-4#sigma");
    TLatex * t4SigmaPosQInPos = new TLatex(tTextInRun,+4*widthInDiffQ,"+4#sigma");
    TLatex * t4SigmaPosQInNeg = new TLatex(tTextInRun,-4*widthInDiffQ,"-4#sigma");

    TLatex * t4SigmaPosXOutPos = new TLatex(tTextOutRun,+4*widthOutDiffX,"+4#sigma");
    TLatex * t4SigmaPosXOutNeg = new TLatex(tTextOutRun,-4*widthOutDiffX,"-4#sigma");
    TLatex * t4SigmaPosYOutPos = new TLatex(tTextOutRun,+4*widthOutDiffY,"+4#sigma");
    TLatex * t4SigmaPosYOutNeg = new TLatex(tTextOutRun,-4*widthOutDiffY,"-4#sigma");
    TLatex * t4SigmaPosXSlopeOutPos = new TLatex(tTextOutRun,+4*widthOutDiffXSlope,"+4#sigma");
    TLatex * t4SigmaPosXSlopeOutNeg = new TLatex(tTextOutRun,-4*widthOutDiffXSlope,"-4#sigma");
    TLatex * t4SigmaPosYSlopeOutPos = new TLatex(tTextOutRun,+4*widthOutDiffYSlope,"+4#sigma");
    TLatex * t4SigmaPosYSlopeOutNeg = new TLatex(tTextOutRun,-4*widthOutDiffYSlope,"-4#sigma");
    TLatex * t4SigmaPosEOutPos = new TLatex(tTextOutRun,+4*widthOutDiffE,"+4#sigma");
    TLatex * t4SigmaPosEOutNeg = new TLatex(tTextOutRun,-4*widthOutDiffE,"-4#sigma");
    TLatex * t4SigmaPosQOutPos = new TLatex(tTextOutRun,+4*widthOutDiffQ,"+4#sigma");
    TLatex * t4SigmaPosQOutNeg = new TLatex(tTextOutRun,-4*widthOutDiffQ,"-4#sigma");

    TLatex * t5SigmaPosXInPos = new TLatex(tTextInRun,+5*widthInDiffX,"+5#sigma");
    TLatex * t5SigmaPosXInNeg = new TLatex(tTextInRun,-5*widthInDiffX,"-5#sigma");
    TLatex * t5SigmaPosYInPos = new TLatex(tTextInRun,+5*widthInDiffY,"+5#sigma");
    TLatex * t5SigmaPosYInNeg = new TLatex(tTextInRun,-5*widthInDiffY,"-5#sigma");
    TLatex * t5SigmaPosXSlopeInPos = new TLatex(tTextInRun,+5*widthInDiffXSlope,"+5#sigma");
    TLatex * t5SigmaPosXSlopeInNeg = new TLatex(tTextInRun,-5*widthInDiffXSlope,"-5#sigma");
    TLatex * t5SigmaPosYSlopeInPos = new TLatex(tTextInRun,+5*widthInDiffYSlope,"+5#sigma");
    TLatex * t5SigmaPosYSlopeInNeg = new TLatex(tTextInRun,-5*widthInDiffYSlope,"-5#sigma");
    TLatex * t5SigmaPosEInPos = new TLatex(tTextInRun,+5*widthInDiffE,"+5#sigma");
    TLatex * t5SigmaPosEInNeg = new TLatex(tTextInRun,-5*widthInDiffE,"-5#sigma");
    TLatex * t5SigmaPosQInPos = new TLatex(tTextInRun,+5*widthInDiffQ,"+5#sigma");
    TLatex * t5SigmaPosQInNeg = new TLatex(tTextInRun,-5*widthInDiffQ,"-5#sigma");

    TLatex * t5SigmaPosXOutPos = new TLatex(tTextOutRun,+5*widthOutDiffX,"+5#sigma");
    TLatex * t5SigmaPosXOutNeg = new TLatex(tTextOutRun,-5*widthOutDiffX,"-5#sigma");
    TLatex * t5SigmaPosYOutPos = new TLatex(tTextOutRun,+5*widthOutDiffY,"+5#sigma");
    TLatex * t5SigmaPosYOutNeg = new TLatex(tTextOutRun,-5*widthOutDiffY,"-5#sigma");
    TLatex * t5SigmaPosXSlopeOutPos = new TLatex(tTextOutRun,+5*widthOutDiffXSlope,"+5#sigma");
    TLatex * t5SigmaPosXSlopeOutNeg = new TLatex(tTextOutRun,-5*widthOutDiffXSlope,"-5#sigma");
    TLatex * t5SigmaPosYSlopeOutPos = new TLatex(tTextOutRun,+5*widthOutDiffYSlope,"+5#sigma");
    TLatex * t5SigmaPosYSlopeOutNeg = new TLatex(tTextOutRun,-5*widthOutDiffYSlope,"-5#sigma");
    TLatex * t5SigmaPosEOutPos = new TLatex(tTextOutRun,+5*widthOutDiffE,"+5#sigma");
    TLatex * t5SigmaPosEOutNeg = new TLatex(tTextOutRun,-5*widthOutDiffE,"-5#sigma");
    TLatex * t5SigmaPosQOutPos = new TLatex(tTextOutRun,+5*widthOutDiffQ,"+5#sigma");
    TLatex * t5SigmaPosQOutNeg = new TLatex(tTextOutRun,-5*widthOutDiffQ,"-5#sigma");


    //   TLatex * t2SigmaRunXIn = new TLatex(tTextInRun,+2*widthInDiffX,"+2#sigma");


    /*********************************************************************/
    /*********************************************************************/
    gStyle->SetOptStat(0);

    TCanvas * c13 = new TCanvas("c13", titleInOut2,0,0,1600,1000);
    c13->Draw();
    c13->SetBorderSize(0);
    c13->cd();
    TPad*pad131 = new TPad("pad131","pad131",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad132 = new TPad("pad132","pad132",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad131->SetFillColor(kWhite);
    pad131->Draw();
    pad132->Draw();
    pad131->cd();
    TLatex * t13 = new TLatex(0.00,0.3,Form("%s",titleInOut2.Data()));
    t13->SetTextSize(0.45);
    t13->Draw();
    pad132->cd();
    pad132->Divide(2,3);

    pad132->cd(1);
    Hel_Tree->Draw("diff*1e6:run_number>>diffRunInX","id==1 && hwp==1","goff");
    histDiffRunInX = (TH1 *)gDirectory->Get("diffRunInX");
    histDiffRunInX->Draw("");
    histDiffRunInX->SetMarkerColor(kRed);
    histDiffRunInX->GetXaxis()->CenterTitle();
    histDiffRunInX->GetYaxis()->CenterTitle();
    histDiffRunInX->SetYTitle("diffX [nm]");
    histDiffRunInX->SetXTitle("Runlets");
    histDiffRunInX->SetTitle("");
    histDiffRunInX->SetMarkerStyle(21);
    sigma2InPosX->Draw();sigma2InNegX->Draw(); sigma3InPosX->Draw();sigma3InNegX->Draw(); sigma4InPosX->Draw();sigma4InNegX->Draw(); 
    t2SigmaPosXInPos->Draw();t2SigmaPosXInNeg->Draw();t3SigmaPosXInPos->Draw();t3SigmaPosXInNeg->Draw();t3SigmaPosXInPos->Draw();t3SigmaPosXInNeg->Draw();
    t4SigmaPosXInPos->Draw();t4SigmaPosXInNeg->Draw();t5SigmaPosXInPos->Draw();t5SigmaPosXInNeg->Draw();
    TAxis *yaxisInX= histDiffRunInX->GetYaxis();
    yaxisInX->SetLimits(-5*widthInDiffX,+5*widthInDiffX);
    //histDiffRunInX->GetYaxis()->SetRangeUser(-200,200);
    gPad->Update();

    pad132->cd(2);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunInXSlope","id==3 && hwp==1","goff");
    histDiffRunInXSlope = (TH1 *)gDirectory->Get("diffRunInXSlope");
    histDiffRunInXSlope->Draw("");
    histDiffRunInXSlope->SetMarkerColor(kRed);
    histDiffRunInXSlope->GetXaxis()->CenterTitle();
    histDiffRunInXSlope->GetYaxis()->CenterTitle();
    histDiffRunInXSlope->SetYTitle("diffXSlope [nrad]");
    histDiffRunInXSlope->SetXTitle("Runlets");
    histDiffRunInXSlope->SetTitle("");
    histDiffRunInXSlope->SetMarkerStyle(22);
    sigma2InPosXSlope->Draw(); sigma2InNegXSlope->Draw(); sigma3InPosXSlope->Draw(); sigma3InNegXSlope->Draw(); sigma4InPosXSlope->Draw(); sigma4InNegXSlope->Draw(); 
    t2SigmaPosXSlopeInPos->Draw();t2SigmaPosXSlopeInNeg->Draw();t3SigmaPosXSlopeInPos->Draw();t3SigmaPosXSlopeInNeg->Draw();
    t4SigmaPosXSlopeInPos->Draw();t4SigmaPosXSlopeInNeg->Draw();t5SigmaPosXSlopeInPos->Draw();t5SigmaPosXSlopeInNeg->Draw();
    TAxis *yaxisInXSlope= histDiffRunInXSlope->GetYaxis();
    yaxisInXSlope->SetLimits(-5*widthInDiffXSlope,+5*widthInDiffXSlope);
    gPad->Update();

    pad132->cd(3);
    Hel_Tree->Draw("diff*1e6:run_number>>diffRunInY","id==2 && hwp==1","goff");
    histDiffRunInY = (TH1 *)gDirectory->Get("diffRunInY");
    histDiffRunInY->Draw("");
    histDiffRunInY->SetMarkerColor(kGreen);
    histDiffRunInY->GetXaxis()->CenterTitle();
    histDiffRunInY->GetYaxis()->CenterTitle();
    histDiffRunInY->SetYTitle("diffY [nm]");
    histDiffRunInY->SetXTitle("Runlets");
    histDiffRunInY->SetTitle("");
    histDiffRunInY->SetMarkerStyle(21);
    sigma2InPosY->Draw(); sigma2InNegY->Draw(); sigma3InPosY->Draw(); sigma3InNegY->Draw();  sigma4InPosY->Draw(); sigma4InNegY->Draw(); 
    t2SigmaPosYInPos->Draw();t2SigmaPosYInNeg->Draw();t3SigmaPosYInPos->Draw();t3SigmaPosYInNeg->Draw();
    t4SigmaPosYInPos->Draw();t4SigmaPosYInNeg->Draw();t5SigmaPosYInPos->Draw();t5SigmaPosYInNeg->Draw();
    TAxis *yaxisInY= histDiffRunInY->GetYaxis(); 
    yaxisInY->SetLimits(-5*widthInDiffY,+5*widthInDiffY);
    gPad->Update();

    pad132->cd(4);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunInYSlope","id==4 && hwp==1","goff");
    histDiffRunInYSlope = (TH1 *)gDirectory->Get("diffRunInYSlope");
    histDiffRunInYSlope->Draw("");
    histDiffRunInYSlope->SetMarkerColor(kGreen);
    histDiffRunInYSlope->GetXaxis()->CenterTitle();
    histDiffRunInYSlope->GetYaxis()->CenterTitle();
    histDiffRunInYSlope->SetYTitle("diffYSlope [nrad]");
    histDiffRunInYSlope->SetXTitle("Runlets");
    histDiffRunInYSlope->SetTitle("");
    histDiffRunInYSlope->SetMarkerStyle(22);
    sigma2InPosYSlope->Draw(); sigma2InNegYSlope->Draw(); sigma3InPosYSlope->Draw(); sigma3InNegYSlope->Draw(); sigma4InPosYSlope->Draw(); sigma4InNegYSlope->Draw(); 
    t2SigmaPosYSlopeInPos->Draw();t2SigmaPosYSlopeInNeg->Draw();t3SigmaPosYSlopeInPos->Draw();t3SigmaPosYSlopeInNeg->Draw();
    t4SigmaPosYSlopeInPos->Draw();t4SigmaPosYSlopeInNeg->Draw();t5SigmaPosYSlopeInPos->Draw();t5SigmaPosYSlopeInNeg->Draw();
    TAxis *yaxisInYSlope= histDiffRunInYSlope->GetYaxis();
    yaxisInYSlope->SetLimits(-5*widthInDiffYSlope,+5*widthInDiffYSlope);
    gPad->Update();

    pad132->cd(5);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunInE","id==5 && hwp==1","goff");
    histDiffRunInE = (TH1 *)gDirectory->Get("diffRunInE");
    histDiffRunInE->Draw("");
    histDiffRunInE->SetMarkerColor(kBlue);
    histDiffRunInE->GetXaxis()->CenterTitle();
    histDiffRunInE->GetYaxis()->CenterTitle();
    histDiffRunInE->SetYTitle("diffE [ppb]");
    histDiffRunInE->SetXTitle("Runlets");
    histDiffRunInE->SetTitle("");
    histDiffRunInE->SetMarkerStyle(20);
    sigma2InPosE->Draw(); sigma2InNegE->Draw(); sigma3InPosE->Draw(); sigma3InNegE->Draw(); sigma4InPosE->Draw(); sigma4InNegE->Draw(); 
    t2SigmaPosEInPos->Draw();t2SigmaPosEInNeg->Draw();t3SigmaPosEInPos->Draw();t3SigmaPosEInNeg->Draw();
    t4SigmaPosEInPos->Draw();t4SigmaPosEInNeg->Draw();t5SigmaPosEInPos->Draw();t5SigmaPosEInNeg->Draw();
    TAxis *yaxisInE= histDiffRunInE->GetYaxis();
    yaxisInE->SetLimits(-5*widthInDiffE,+5*widthInDiffE);
    gPad->Update();

    pad132->cd(6);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunInQ","id==6 && hwp==1","goff");
    histDiffRunInQ = (TH1 *)gDirectory->Get("diffRunInQ");
    histDiffRunInQ->Draw("");
    histDiffRunInQ->SetMarkerColor(kMagenta);
    histDiffRunInQ->GetXaxis()->CenterTitle();
    histDiffRunInQ->GetYaxis()->CenterTitle();
    histDiffRunInQ->SetYTitle("diffCharge [ppb]");
    histDiffRunInQ->SetXTitle("Runlets");
    histDiffRunInQ->SetTitle("");
    histDiffRunInQ->SetMarkerStyle(20);
    sigma2InPosQ->Draw(); sigma2InNegQ->Draw(); sigma3InPosQ->Draw(); sigma3InNegQ->Draw(); sigma4InPosQ->Draw(); sigma4InNegQ->Draw(); 
    t2SigmaPosQInPos->Draw();t2SigmaPosQInNeg->Draw();t3SigmaPosQInPos->Draw();t3SigmaPosQInNeg->Draw();
    t4SigmaPosQInPos->Draw();t4SigmaPosQInNeg->Draw();t5SigmaPosQInPos->Draw();t5SigmaPosQInNeg->Draw();
    TAxis *yaxisInQ= histDiffRunInQ->GetYaxis();
    yaxisInQ->SetLimits(-5*widthInDiffQ,+5*widthInDiffQ);
    gPad->Update();

    TString savePlotDiffInVsRun = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_diffInVsRun_%s"
				       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				       ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c13-> Update();
    c13->Print(savePlotDiffInVsRun+".png");
    if(FIGURE){
      c13->Print(savePlotDiffInVsRun+".svg");
      c13->Print(savePlotDiffInVsRun+".C");
    }

    /*********************************************************************/


    TCanvas * c14 = new TCanvas("c14", titleInOut2,0,0,1600,1000);
    c14->Draw();
    c14->SetBorderSize(0);
    c14->cd();
    TPad*pad141 = new TPad("pad141","pad141",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad142 = new TPad("pad142","pad142",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad141->SetFillColor(kWhite);
    pad141->Draw();
    pad142->Draw();
    pad141->cd();
    TLatex * t14 = new TLatex(0.00,0.3,Form("%s",titleInOut2.Data()));
    t14->SetTextSize(0.45);
    t14->Draw();
    pad142->cd();
    pad142->Divide(2,3);

    pad142->cd(1);
    Hel_Tree->Draw("diff*1e6:run_number>>diffRunOutX","id==1 && hwp==0","goff");
    histDiffRunOutX = (TH1 *)gDirectory->Get("diffRunOutX");
    histDiffRunOutX->Draw("");
    histDiffRunOutX->SetMarkerColor(kRed);
    histDiffRunOutX->GetXaxis()->CenterTitle();
    histDiffRunOutX->GetYaxis()->CenterTitle();
    histDiffRunOutX->SetYTitle("diffX [nm]");
    histDiffRunOutX->SetXTitle("Runlets");
    histDiffRunOutX->SetTitle("");
    histDiffRunOutX->SetMarkerStyle(25);
    sigma2OutPosX->Draw(); sigma2OutNegX->Draw(); sigma3OutPosX->Draw(); sigma3OutNegX->Draw(); sigma4OutPosX->Draw(); sigma4OutNegX->Draw(); 
    t2SigmaPosXOutPos->Draw();t2SigmaPosXOutNeg->Draw();t3SigmaPosXOutPos->Draw();t3SigmaPosXOutNeg->Draw();
    t4SigmaPosXOutPos->Draw();t4SigmaPosXOutNeg->Draw();t5SigmaPosXOutPos->Draw();t5SigmaPosXOutNeg->Draw();
    TAxis *yaxisOutX= histDiffRunOutX->GetYaxis();
    yaxisOutX->SetLimits(-5*widthOutDiffX,+5*widthOutDiffX);
    gPad->Update();

    pad142->cd(2);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunOutXSlope","id==3 && hwp==0","goff");
    histDiffRunOutXSlope = (TH1 *)gDirectory->Get("diffRunOutXSlope");
    histDiffRunOutXSlope->Draw("");
    histDiffRunOutXSlope->SetMarkerColor(kRed);
    histDiffRunOutXSlope->GetXaxis()->CenterTitle();
    histDiffRunOutXSlope->GetYaxis()->CenterTitle();
    histDiffRunOutXSlope->SetYTitle("diffXSlope [nrad]");
    histDiffRunOutXSlope->SetXTitle("Runlets");
    histDiffRunOutXSlope->SetTitle("");
    histDiffRunOutXSlope->SetMarkerStyle(26);
    sigma2OutPosXSlope->Draw(); sigma2OutNegXSlope->Draw(); sigma3OutPosXSlope->Draw(); sigma3OutNegXSlope->Draw();  sigma4OutPosXSlope->Draw(); sigma4OutNegXSlope->Draw(); 
    t2SigmaPosXSlopeOutPos->Draw();t2SigmaPosXSlopeOutNeg->Draw();t3SigmaPosXSlopeOutPos->Draw();t3SigmaPosXSlopeOutNeg->Draw();
    t4SigmaPosXSlopeOutPos->Draw();t4SigmaPosXSlopeOutNeg->Draw();t5SigmaPosXSlopeOutPos->Draw();t5SigmaPosXSlopeOutNeg->Draw();
    TAxis *yaxisOutXSlope= histDiffRunOutXSlope->GetYaxis();
    yaxisOutXSlope->SetLimits(-5*widthOutDiffXSlope,+5*widthOutDiffXSlope);
    gPad->Update();

    pad142->cd(3);
    Hel_Tree->Draw("diff*1e6:run_number>>diffRunOutY","id==2 && hwp==0","goff");
    histDiffRunOutY = (TH1 *)gDirectory->Get("diffRunOutY");
    histDiffRunOutY->Draw("");
    histDiffRunOutY->SetMarkerColor(kGreen);
    histDiffRunOutY->GetXaxis()->CenterTitle();
    histDiffRunOutY->GetYaxis()->CenterTitle();
    histDiffRunOutY->SetYTitle("diffY [nm]");
    histDiffRunOutY->SetXTitle("Runlets");
    histDiffRunOutY->SetTitle("");
    histDiffRunOutY->SetMarkerStyle(25);
    sigma2OutPosY->Draw(); sigma2OutNegY->Draw(); sigma3OutPosY->Draw(); sigma3OutNegY->Draw(); sigma4OutPosY->Draw(); sigma4OutNegY->Draw(); 
    t2SigmaPosYOutPos->Draw();t2SigmaPosYOutNeg->Draw();t3SigmaPosYOutPos->Draw();t3SigmaPosYOutNeg->Draw();
    t4SigmaPosYOutPos->Draw();t4SigmaPosYOutNeg->Draw();t5SigmaPosYOutPos->Draw();t5SigmaPosYOutNeg->Draw();
    TAxis *yaxisOutY= histDiffRunOutY->GetYaxis();
    yaxisOutY->SetLimits(-5*widthOutDiffY,+5*widthOutDiffY);
    gPad->Update();

    pad142->cd(4);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunOutYSlope","id==4 && hwp==0","goff");
    histDiffRunOutYSlope = (TH1 *)gDirectory->Get("diffRunOutYSlope");
    histDiffRunOutYSlope->Draw("");
    histDiffRunOutYSlope->SetMarkerColor(kGreen);
    histDiffRunOutYSlope->GetXaxis()->CenterTitle();
    histDiffRunOutYSlope->GetYaxis()->CenterTitle();
    histDiffRunOutYSlope->SetYTitle("diffYSlope [nrad]");
    histDiffRunOutYSlope->SetXTitle("Runlets");
    histDiffRunOutYSlope->SetTitle("");
    histDiffRunOutYSlope->SetMarkerStyle(26);
    sigma2OutPosYSlope->Draw(); sigma2OutNegYSlope->Draw(); sigma3OutPosYSlope->Draw(); sigma3OutNegYSlope->Draw(); sigma4OutPosYSlope->Draw(); sigma4OutNegYSlope->Draw(); 
    t2SigmaPosYSlopeOutPos->Draw();t2SigmaPosYSlopeOutNeg->Draw();t3SigmaPosYSlopeOutPos->Draw();t3SigmaPosYSlopeOutNeg->Draw();
    t4SigmaPosYSlopeOutPos->Draw();t4SigmaPosYSlopeOutNeg->Draw();t5SigmaPosYSlopeOutPos->Draw();t5SigmaPosYSlopeOutNeg->Draw();
    TAxis *yaxisOutYSlope= histDiffRunOutYSlope->GetYaxis();
    yaxisOutYSlope->SetLimits(-5*widthOutDiffYSlope,+5*widthOutDiffYSlope);
    gPad->Update();

    pad142->cd(5);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunOutE","id==5 && hwp==0","goff");
    histDiffRunOutE = (TH1 *)gDirectory->Get("diffRunOutE");
    histDiffRunOutE->Draw("");
    histDiffRunOutE->SetMarkerColor(kBlue);
    histDiffRunOutE->GetXaxis()->CenterTitle();
    histDiffRunOutE->GetYaxis()->CenterTitle();
    histDiffRunOutE->SetYTitle("diffE [ppb]");
    histDiffRunOutE->SetXTitle("Runlets");
    histDiffRunOutE->SetTitle("");
    histDiffRunOutE->SetMarkerStyle(24);
    sigma2OutPosE->Draw(); sigma2OutNegE->Draw(); sigma3OutPosE->Draw(); sigma3OutNegE->Draw(); sigma4OutPosE->Draw(); sigma4OutNegE->Draw(); 
    t2SigmaPosEOutPos->Draw();t2SigmaPosEOutNeg->Draw();t3SigmaPosEOutPos->Draw();t3SigmaPosEOutNeg->Draw();
    t4SigmaPosEOutPos->Draw();t4SigmaPosEOutNeg->Draw();t5SigmaPosEOutPos->Draw();t5SigmaPosEOutNeg->Draw();
    TAxis *yaxisOutE= histDiffRunOutE->GetYaxis();
    yaxisOutE->SetLimits(-5*widthOutDiffE,+5*widthOutDiffE);
    gPad->Update();

    pad142->cd(6);
    Hel_Tree->Draw("diff*1e9:run_number>>diffRunOutQ","id==6 && hwp==0","goff");
    histDiffRunOutQ = (TH1 *)gDirectory->Get("diffRunOutQ");
    histDiffRunOutQ->Draw("");
    histDiffRunOutQ->SetMarkerColor(kMagenta);
    histDiffRunOutQ->GetXaxis()->CenterTitle();
    histDiffRunOutQ->GetYaxis()->CenterTitle();
    histDiffRunOutQ->SetYTitle("diffCharge [ppb]");
    histDiffRunOutQ->SetXTitle("Runlets");
    histDiffRunOutQ->SetTitle("");
    histDiffRunOutQ->SetMarkerStyle(24);
    sigma2OutPosQ->Draw(); sigma2OutNegQ->Draw(); sigma3OutPosQ->Draw(); sigma3OutNegQ->Draw(); sigma4OutPosQ->Draw(); sigma4OutNegQ->Draw(); 
    t2SigmaPosQOutPos->Draw();t2SigmaPosQOutNeg->Draw();t3SigmaPosQOutPos->Draw();t3SigmaPosQOutNeg->Draw();
    t4SigmaPosQOutPos->Draw();t4SigmaPosQOutNeg->Draw();t5SigmaPosQOutPos->Draw();t5SigmaPosQOutNeg->Draw();
    TAxis *yaxisOutQ= histDiffRunOutQ->GetYaxis();
    yaxisOutQ->SetLimits(-5*widthOutDiffQ,+5*widthOutDiffQ);
    gPad->Update();

    TString savePlotDiffOutVsRun = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_diffOutVsRun_%s"
					,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
					,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c14-> Update();
    c14->Print(savePlotDiffOutVsRun+".png");
    if(FIGURE){
      c14->Print(savePlotDiffOutVsRun+".svg");
      c14->Print(savePlotDiffOutVsRun+".C");
    }



  }

  /*********************************************************************/
  /*********************************************************************/

  if(FINAL_FIGURE){

    Char_t  textfile[400],textfileStats[400];
    sprintf(textfile,"dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_RunletAvg_cutDependence_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),target.Data()
	    ,reg_calc.Data(),reg_set.Data(),database_stem.Data()); 
    sprintf(textfileStats,"dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_stats_cutDependence_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),target.Data()
	    ,reg_calc.Data(),reg_set.Data(),database_stem.Data()); 

    //   printf("%sText File = %s\n%s",green,textfile,normal);
    
    const Int_t NUM = 4;

    int counter=0; int counterStats=0;
    ifstream transverseMDAsym,transverseMDAsymStats;
    Double_t zero[NUM],sigma[NUM],sigmaStats[NUM],sigmaPlot[NUM],sigmaStatsPlot[NUM],hTransAsym[NUM],vTransAsym[NUM],hTransAsymStats[NUM],vTransAsymStats[NUM],hTransAsymStatsPlot[NUM];
    Double_t hTransAsymIn[NUM],vTransAsymIn[NUM],hTransAsymOut[NUM],vTransAsymOut[NUM];


    transverseMDAsym.open(Form("%s",textfile));
    printf(Form("%s\n",textfile));
 
    if (transverseMDAsym.is_open()) {
   
      printf("%s Det\tIn+-InError\tOut+-OutError\n%s",green,normal);
   
      while(!transverseMDAsym.eof()) {
	zero[counter]=0;
	transverseMDAsym >> sigma[counter];
	transverseMDAsym >> hTransAsymIn[counter];
	transverseMDAsym >> hTransAsymOut[counter];
	transverseMDAsym >> hTransAsym[counter];
	transverseMDAsym >> vTransAsymIn[counter];
	transverseMDAsym >> vTransAsymOut[counter];
	transverseMDAsym >> vTransAsym[counter];
   
	if (!transverseMDAsym.good()) break;

	cout<<green<<sigma[counter]<<"\t H: "<<hTransAsym[counter]<<", V: "<<vTransAsym[counter]<<normal<<endl;

	counter++;
      }
    }
    else {
      printf("%sNo textfile exist for the configuration. dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_RunletAvg_cutDependence_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),target.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data(),normal);
      return -1;
    }
    transverseMDAsym.close();




    transverseMDAsymStats.open(Form("%s",textfileStats));

    if (transverseMDAsymStats.is_open()) {
   
      while(!transverseMDAsymStats.eof()) {
	zero[counterStats]=0;
	transverseMDAsymStats >> sigmaStats[counterStats];
	transverseMDAsymStats >> hTransAsymStats[counterStats];
	transverseMDAsymStats >> vTransAsymStats[counterStats];
   
	if (!transverseMDAsymStats.good()) break;

	cout<<green<<sigma[counterStats]<<"\t H: "<<hTransAsymStats[counterStats]<<", V: "<<vTransAsymStats[counterStats]<<normal<<endl;

	counterStats++;
      }
    }
    else {
      printf("%sNo textfile exist for the configuration. dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_stats_cutDependence_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),target.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data(),normal);
      return -1;
    }
    transverseMDAsymStats.close();


    for(Int_t i =0;i<NUM;i++){
      sigmaPlot[i]=TMath::Abs(sigma[i]-4);
      sigmaStatsPlot[i]=TMath::Abs(sigmaStats[i]-4);
      hTransAsymStatsPlot[i] = hTransAsymStats[i-4];

    }

    gStyle->SetTitleYOffset(0.70);
    gStyle->SetTitleXOffset(1.05);
    gStyle->SetTitleX(0.12);
    gStyle->SetTitleW(0.20);

    Double_t yScaleH = 0.60;
    Double_t yScaleV = 1.50;

    TString titleFinal = Form("%s (%s A): MD Regressed (%s) Asymmetry. Cut Dependence. "
			      ,targ.Data(),qtor_stem.Data(),reg_set.Data());

    TCanvas * c15 = new TCanvas("c15", titleFinal,0,0,canvasSize[0],canvasSize[1]);
    c15->Draw();
    c15->SetBorderSize(0);
    c15->cd();
    TPad*pad151 = new TPad("pad151","pad151",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad152 = new TPad("pad152","pad152",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad151->SetFillColor(kWhite);
    pad151->Draw();
    pad152->Draw();
    pad151->cd();
    TLatex * t15 = new TLatex(0.00,0.3,Form("%s",titleFinal.Data()));
    t15->SetTextSize(0.45);
    t15->Draw();
    pad152->cd();
    pad152->Divide(1,2);


    pad152->cd(1);

    TGraphErrors* grp_h_cut_in  = new TGraphErrors(NUM,sigmaPlot,hTransAsymIn,zero,zero);
    grp_h_cut_in ->SetName("grp_h_cut_in");
    grp_h_cut_in ->Draw("goff");
    grp_h_cut_in ->SetMarkerSize(0.9);
    grp_h_cut_in ->SetMarkerStyle(24);
    grp_h_cut_in ->SetMarkerColor(kRed);
    grp_h_cut_in ->SetLineColor(kRed);

    TGraphErrors* grp_h_cut_out  = new TGraphErrors(NUM,sigmaPlot,hTransAsymOut,zero,zero);
    grp_h_cut_out ->SetName("grp_h_cut_out");
    grp_h_cut_out ->Draw("goff");
    grp_h_cut_out ->SetMarkerSize(0.9);
    grp_h_cut_out ->SetMarkerStyle(24);
    grp_h_cut_out ->SetMarkerColor(kBlack);
    grp_h_cut_out ->SetLineColor(kBlack);

    TGraphErrors* grp_h_cut  = new TGraphErrors(NUM,sigmaPlot,hTransAsym,zero,zero);
    grp_h_cut ->SetName("grp_h_cut");
    grp_h_cut ->Draw("goff");
    grp_h_cut ->SetMarkerSize(0.9);
    grp_h_cut ->SetMarkerStyle(20);
    grp_h_cut ->SetMarkerColor(kOrange+10);
    grp_h_cut ->SetLineColor(kOrange+10);

    TGraphErrors* grp_h_stats  = new TGraphErrors(NUM,sigmaPlot,zero,zero,hTransAsymStats);
    grp_h_stats ->SetName("grp_h_stats");
    grp_h_stats ->Draw("goff");
    grp_h_stats ->SetFillStyle(3017);
    grp_h_stats ->SetFillColor(kOrange+10);
    grp_h_stats ->SetLineColor(kOrange+10);


    TMultiGraph * grp_h = new TMultiGraph();
    grp_h->Add(grp_h_cut,"P");
    grp_h->Add(grp_h_cut_in,"P");
    grp_h->Add(grp_h_cut_out,"P");
    grp_h ->Add(grp_h_stats,"l3");
    grp_h->Draw("A");
    grp_h->SetTitle("Horizontal Transverse");
    grp_h->GetYaxis()->CenterTitle();
    grp_h->GetYaxis()->SetTitle("MD Asym. Cut - NoCut [ppm]");
    grp_h->GetXaxis()->CenterTitle();
    grp_h->GetXaxis()->SetTitle("Applied cut");
    grp_h->GetXaxis()->SetNdivisions(4,0,2);
    grp_h->GetYaxis()->SetRangeUser(-yScaleH,yScaleH);
    TAxis *xaxisgrp_h= grp_h->GetXaxis();
    xaxisgrp_h->SetLimits(-0.2,2.2);
    grp_h->GetXaxis()->SetLabelColor(0);

    TLegend *legendH = new TLegend(0.13,0.650,0.50,0.895,"","brNDC");
    legendH->SetNColumns(2);
    legendH->AddEntry(grp_h_cut_in,  Form("IHWP-IN"), "p");
    legendH->AddEntry(grp_h_cut_out, Form("IHWP-OUT"), "p");
    legendH->AddEntry(grp_h_cut,Form("Measured [Ave(IN,-OUT)]"), "p");
    legendH->AddEntry(grp_h_stats,Form("Expected statistical shift"), "f");
    legendH->SetFillColor(0);
    legendH->SetBorderSize(2);
    legendH->SetTextSize(0.055);
    legendH->Draw("");


    Double_t xHCor = -0.06, yHCor = -1.15*yScaleH;
    gStyle->SetTextSize(0.06);
    TLatex* textHAxis8=new TLatex(xHCor-0.05,yHCor,Form("NoCut,7,6,5,4#sigma"));
    //   TLatex* textHAxis7=new TLatex(xHCor+1.0,yHCor,Form("7#sigma"));
    //   TLatex* textHAxis6=new TLatex(xHCor+2.0,yHCor,Form("6#sigma"));
    //   TLatex* textHAxis5=new TLatex(xHCor+3.0,yHCor,Form("5#sigma"));
    //   TLatex* textHAxis4=new TLatex(xHCor+4.0,yHCor,Form("4#sigma"));
    TLatex* textHAxis3=new TLatex(xHCor+1.03,yHCor,Form("3#sigma"));
    TLatex* textHAxis25=new TLatex(xHCor+1.53,yHCor,Form("2.5#sigma"));
    TLatex* textHAxis2=new TLatex(xHCor+2.03,yHCor,Form("2#sigma"));

    textHAxis2->Draw();textHAxis25->Draw();textHAxis3->Draw();textHAxis8->Draw();
    //   textHAxis4->Draw(); textHAxis5->Draw();textHAxis6->Draw();textHAxis7->Draw();


    pad152->cd(2);

    TGraphErrors* grp_v_cut_in  = new TGraphErrors(NUM,sigmaPlot,vTransAsymIn,zero,zero);
    grp_v_cut_in ->SetName("grp_v_cut_in");
    grp_v_cut_in ->Draw("goff");
    grp_v_cut_in ->SetMarkerSize(0.8);
    grp_v_cut_in ->SetMarkerStyle(25);
    grp_v_cut_in ->SetMarkerColor(kBlue);
    grp_v_cut_in ->SetLineColor(kBlue);

    TGraphErrors* grp_v_cut_out  = new TGraphErrors(NUM,sigmaPlot,vTransAsymOut,zero,zero);
    grp_v_cut_out ->SetName("grp_v_cut_out");
    grp_v_cut_out ->Draw("goff");
    grp_v_cut_out ->SetMarkerSize(0.8);
    grp_v_cut_out ->SetMarkerStyle(25);
    grp_v_cut_out ->SetMarkerColor(kBlack);
    grp_v_cut_out ->SetLineColor(kBlack);

    TGraphErrors* grp_v_cut  = new TGraphErrors(NUM,sigmaPlot,vTransAsym,zero,zero);
    grp_v_cut ->SetName("grp_v_cut");
    grp_v_cut ->Draw("goff");
    grp_v_cut ->SetMarkerSize(0.8);
    grp_v_cut ->SetMarkerStyle(21);
    grp_v_cut ->SetMarkerColor(kAzure+1);
    grp_v_cut ->SetLineColor(kAzure+1);

    TGraphErrors* grp_v_stats  = new TGraphErrors(NUM,sigmaPlot,zero,zero,vTransAsymStats);
    grp_v_stats ->SetName("grp_v_stats");
    grp_v_stats ->Draw("goff");
    grp_v_stats ->SetFillStyle(3018);
    grp_v_stats ->SetFillColor(kAzure+1);
    grp_v_stats ->SetLineColor(kAzure+1);


    TMultiGraph * grp_v = new TMultiGraph();
    grp_v->Add(grp_v_cut,"P");
    grp_v->Add(grp_v_cut_in,"P");
    grp_v->Add(grp_v_cut_out,"P");
    grp_v ->Add(grp_v_stats,"l3");
    grp_v->Draw("A");
    grp_v->SetTitle("Vertical Transverse");
    grp_v->GetYaxis()->CenterTitle();
    grp_v->GetYaxis()->SetTitle("MD Asym. Cut - NoCut [ppm]");
    grp_v->GetXaxis()->CenterTitle();
    grp_v->GetXaxis()->SetTitle("Applied cut");
    grp_v->GetXaxis()->SetNdivisions(4,0,2);
    grp_v->GetYaxis()->SetRangeUser(-yScaleV,yScaleV);
    TAxis *xaxisgrp_v= grp_v->GetXaxis();
    xaxisgrp_v->SetLimits(-0.2,2.2);
    grp_v->GetXaxis()->SetLabelColor(0);

    TLegend *legendV = new TLegend(0.13,0.650,0.50,0.895,"","brNDC");
    legendV->SetNColumns(2);
    legendV->AddEntry(grp_v_cut_in,  Form("IHWP-IN"), "p");
    legendV->AddEntry(grp_v_cut_out, Form("IHWP-OUT"), "p");
    legendV->AddEntry(grp_v_cut,Form("Measured [Ave(IN,-OUT)]"), "p");
    legendV->AddEntry(grp_v_stats,Form("Expected statistical shift"), "f");
    legendV->SetFillColor(0);
    legendV->SetBorderSize(2);
    legendV->SetTextSize(0.055);
    legendV->Draw("");

    //   Double_t xVCor = -0.06, yVCor = -1.15*yScaleV;
    //   TLatex* textVAxis8=new TLatex(xVCor-0.05,yVCor,Form("NoCut"));
    //   TLatex* textVAxis7=new TLatex(xVCor+1.0,yVCor,Form("7#sigma"));
    //   TLatex* textVAxis6=new TLatex(xVCor+2.0,yVCor,Form("6#sigma"));
    //   TLatex* textVAxis5=new TLatex(xVCor+3.0,yVCor,Form("5#sigma"));
    //   TLatex* textVAxis4=new TLatex(xVCor+4.0,yVCor,Form("4#sigma"));
    //   TLatex* textVAxis3=new TLatex(xVCor+5.0,yVCor,Form("3#sigma"));
    //   TLatex* textVAxis25=new TLatex(xVCor+5.5,yVCor,Form("2.5#sigma"));
    //   TLatex* textVAxis2=new TLatex(xVCor+6.0,yVCor,Form("2#sigma"));

    //   textVAxis2->Draw();textVAxis25->Draw();textVAxis3->Draw();textVAxis4->Draw();
    //   textVAxis5->Draw();textVAxis6->Draw();textVAxis7->Draw();textVAxis8->Draw();

    Double_t xVCor = -0.06, yVCor = -1.15*yScaleV;
    gStyle->SetTextSize(0.06);
    TLatex* textVAxis8=new TLatex(xVCor-0.05,yVCor,Form("NoCut,7,6,5,4#sigma"));
    //   TLatex* textVAxis7=new TLatex(xVCor+1.0,yVCor,Form("7#sigma"));
    //   TLatex* textVAxis6=new TLatex(xVCor+2.0,yVCor,Form("6#sigma"));
    //   TLatex* textVAxis5=new TLatex(xVCor+3.0,yVCor,Form("5#sigma"));
    //   TLatex* textVAxis4=new TLatex(xVCor+4.0,yVCor,Form("4#sigma"));
    TLatex* textVAxis3=new TLatex(xVCor+1.03,yVCor,Form("3#sigma"));
    TLatex* textVAxis25=new TLatex(xVCor+1.53,yVCor,Form("2.5#sigma"));
    TLatex* textVAxis2=new TLatex(xVCor+2.03,yVCor,Form("2#sigma"));

    textVAxis2->Draw();textVAxis25->Draw();textVAxis3->Draw();textVAxis8->Draw();
    //   textVAxis4->Draw(); textVAxis5->Draw();textVAxis6->Draw();textVAxis7->Draw();


    TString savePlotCutFinal = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_final_%s"
					,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
					,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c15-> Update();
    c15->Print(savePlotCutFinal+".png");
    if(FIGURE){
      c15->Print(savePlotCutFinal+".svg");
      c15->Print(savePlotCutFinal+".C");
    }

    /*********************************************************************/



    Char_t  textfileIndividual[400],textfileStatsIndividual[400];
    sprintf(textfileIndividual,"dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_RunletAvg_cutDependence_Individual_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),target.Data()
	    ,reg_calc.Data(),reg_set.Data(),database_stem.Data()); 
    sprintf(textfileStatsIndividual,"dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_stats_cutDependence_Individual_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),target.Data()
	    ,reg_calc.Data(),reg_set.Data(),database_stem.Data()); 

    //   printf("%sText File = %s\n%s",green,textfile,normal);
    
    const Int_t NUM2 = 4;

    int counterIndividual=0; int counterStatsIndividual=0;
    ifstream transverseMDAsymIndividual,transverseMDAsymStatsIndividual;
    Double_t zero2[NUM],sigma2[NUM],sigmaStats2[NUM],sigmaPlot2[NUM],sigmaStatsPlot2[NUM],hTransAsym2[NUM],vTransAsym2[NUM],hTransAsymStats2[NUM],vTransAsymStats2[NUM],hTransAsymStatsPlot2[NUM];
    Double_t hTransAsymIn2[NUM],vTransAsymIn2[NUM],hTransAsymOut2[NUM],vTransAsymOut2[NUM];
    Double_t hTransAsymInX[NUM],vTransAsymInX[NUM],hTransAsymOutX[NUM],vTransAsymOutX[NUM];
    Double_t hTransAsymInY[NUM],vTransAsymInY[NUM],hTransAsymOutY[NUM],vTransAsymOutY[NUM];
    Double_t hTransAsymInXp[NUM],vTransAsymInXp[NUM],hTransAsymOutXp[NUM],vTransAsymOutXp[NUM];
    Double_t hTransAsymInYp[NUM],vTransAsymInYp[NUM],hTransAsymOutYp[NUM],vTransAsymOutYp[NUM];
    Double_t hTransAsymInE[NUM],vTransAsymInE[NUM],hTransAsymOutE[NUM],vTransAsymOutE[NUM];
    Double_t hTransAsymInQ[NUM],vTransAsymInQ[NUM],hTransAsymOutQ[NUM],vTransAsymOutQ[NUM];




    transverseMDAsymIndividual.open(Form("%s",textfileIndividual));
    printf(Form("%s\n",textfileIndividual));
 
    if (transverseMDAsymIndividual.is_open()) {
   
      printf("%s Det\tIn+-InError\tOut+-OutError\n%s",green,normal);
   
      while(!transverseMDAsymIndividual.eof()) {
	zero[counterIndividual]=0;
	transverseMDAsymIndividual >> sigma2[counterIndividual];

	transverseMDAsymIndividual >> hTransAsymInX[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymInY[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymInXp[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymInYp[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymInE[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymInQ[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymIn2[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOutX[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOutY[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOutXp[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOutYp[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOutE[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOutQ[counterIndividual];
	transverseMDAsymIndividual >> hTransAsymOut2[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymInX[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymInY[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymInXp[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymInYp[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymInE[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymInQ[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymIn2[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOutX[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOutY[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOutXp[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOutYp[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOutE[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOutQ[counterIndividual];
	transverseMDAsymIndividual >> vTransAsymOut2[counterIndividual];
   
	if (!transverseMDAsymIndividual.good()) break;

	cout<<green<<sigma2[counterIndividual]<<"\t H: "<<hTransAsym2[counterIndividual]<<", V: "<<vTransAsym2[counterIndividual]<<normal<<endl;

	counterIndividual++;
      }
    }
    else {
      printf("%sNo textfile exist for the configuration. dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_RunletAvg_cutDependence_Individual_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),target.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data(),normal);
      return -1;
    }
    transverseMDAsymIndividual.close();




    transverseMDAsymStatsIndividual.open(Form("%s",textfileStatsIndividual));

    if (transverseMDAsymStatsIndividual.is_open()) {
   
      while(!transverseMDAsymStatsIndividual.eof()) {
	zero2[counterStatsIndividual]=0;
	transverseMDAsymStatsIndividual >> sigmaStats2[counterStatsIndividual];
	transverseMDAsymStatsIndividual >> hTransAsymStats2[counterStatsIndividual];
	transverseMDAsymStatsIndividual >> vTransAsymStats2[counterStatsIndividual];
   
	if (!transverseMDAsymStats.good()) break;

	cout<<green<<sigma2[counterStatsIndividual]<<"\t H: "<<hTransAsymStats2[counterStatsIndividual]<<", V: "<<vTransAsymStats2[counterStatsIndividual]<<normal<<endl;

	counterStatsIndividual++;
      }
    }
    else {
      printf("%sNo textfile exist for the configuration. dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_stats_cutDependence_Individual_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),target.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data(),normal);
      return -1;
    }
    transverseMDAsymStatsIndividual.close();


    for(Int_t i =0;i<NUM2;i++){
      sigmaPlot2[i]=TMath::Abs(sigma2[i]-4);
      sigmaStatsPlot2[i]=TMath::Abs(sigmaStats2[i]-4);
      hTransAsymStatsPlot2[i] = hTransAsymStats2[i-4];

    }

    gStyle->SetTitleYOffset(0.70);
    gStyle->SetTitleXOffset(1.05);
    gStyle->SetTitleX(0.12);
    gStyle->SetTitleW(0.20);

//     Double_t yScaleH = 0.60;
//     Double_t yScaleV = 1.50;

    TString titleIndividualH = Form("%s (%s A): MD Regressed (%s) Asymmetry. Cut Dependence for Individual Parameters. Horizontal Transverse."
			      ,targ.Data(),qtor_stem.Data(),reg_set.Data());

    TCanvas * c16 = new TCanvas("c16", titleIndividualH,0,0,canvasSize[0],canvasSize[1]);
    c16->Draw();
    c16->SetBorderSize(0);
    c16->cd();
    TPad*pad161 = new TPad("pad161","pad161",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad162 = new TPad("pad162","pad162",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad161->SetFillColor(kWhite);
    pad161->Draw();
    pad162->Draw();
    pad161->cd();
    TLatex * t16 = new TLatex(0.00,0.3,Form("%s",titleIndividualH.Data()));
    t16->SetTextSize(0.45);
    t16->Draw();
    pad162->cd();
    pad162->Divide(1,2);


    pad162->cd(1);

    TGraphErrors* grpX_h_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpX_h_cut_in ->SetName("grpX_h_cut_in");
    grpX_h_cut_in ->Draw("goff");
    grpX_h_cut_in ->SetMarkerSize(0.8);
    grpX_h_cut_in ->SetMarkerStyle(21);
    grpX_h_cut_in ->SetMarkerColor(kRed);
    grpX_h_cut_in ->SetLineColor(kRed);

    TGraphErrors* grpY_h_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInY,zero2,zero2);
    grpY_h_cut_in ->SetName("grpY_h_cut_in");
    grpY_h_cut_in ->Draw("goff");
    grpY_h_cut_in ->SetMarkerSize(0.8);
    grpY_h_cut_in ->SetMarkerStyle(21);
    grpY_h_cut_in ->SetMarkerColor(kGreen);
    grpY_h_cut_in ->SetLineColor(kGreen);

    TGraphErrors* grpXp_h_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpXp_h_cut_in ->SetName("grpXp_h_cut_in");
    grpXp_h_cut_in ->Draw("goff");
    grpXp_h_cut_in ->SetMarkerSize(0.9);
    grpXp_h_cut_in ->SetMarkerStyle(22);
    grpXp_h_cut_in ->SetMarkerColor(kRed);
    grpXp_h_cut_in ->SetLineColor(kRed);

    TGraphErrors* grpYp_h_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInY,zero2,zero2);
    grpYp_h_cut_in ->SetName("grpYp_h_cut_in");
    grpYp_h_cut_in ->Draw("goff");
    grpYp_h_cut_in ->SetMarkerSize(0.9);
    grpYp_h_cut_in ->SetMarkerStyle(22);
    grpYp_h_cut_in ->SetMarkerColor(kGreen);
    grpYp_h_cut_in ->SetLineColor(kGreen);
    
    TGraphErrors* grpE_h_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpE_h_cut_in ->SetName("grpE_h_cut_in");
    grpE_h_cut_in ->Draw("goff");
    grpE_h_cut_in ->SetMarkerSize(0.9);
    grpE_h_cut_in ->SetMarkerStyle(20);
    grpE_h_cut_in ->SetMarkerColor(kBlue);
    grpE_h_cut_in ->SetLineColor(kBlue);

    TGraphErrors* grpQ_h_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpQ_h_cut_in ->SetName("grpQ_h_cut_in");
    grpQ_h_cut_in ->Draw("goff");
    grpQ_h_cut_in ->SetMarkerSize(0.9);
    grpQ_h_cut_in ->SetMarkerStyle(20);
    grpQ_h_cut_in ->SetMarkerColor(kMagenta);
    grpQ_h_cut_in ->SetLineColor(kMagenta);


    TMultiGraph * grp2_h_cut_in = new TMultiGraph();
    grp2_h_cut_in->Add(grpX_h_cut_in,"P");
    grp2_h_cut_in->Add(grpY_h_cut_in,"P");
    grp2_h_cut_in->Add(grpXp_h_cut_in,"P");
    grp2_h_cut_in->Add(grpYp_h_cut_in,"P");
    grp2_h_cut_in->Add(grpE_h_cut_in,"P");
    grp2_h_cut_in->Add(grpQ_h_cut_in,"P");
    grp2_h_cut_in->Add(grp_h_cut_in,"P");
    grp2_h_cut_in ->Add(grp_h_stats,"l3");
    grp2_h_cut_in->Draw("A");
    grp2_h_cut_in->SetTitle("Horizontal Transverse: IHWP-IN");
    grp2_h_cut_in->GetYaxis()->CenterTitle();
    grp2_h_cut_in->GetYaxis()->SetTitle("MD Asym. Cut - NoCut [ppm]");
    grp2_h_cut_in->GetXaxis()->CenterTitle();
    grp2_h_cut_in->GetXaxis()->SetTitle("Applied cut");
    grp2_h_cut_in->GetXaxis()->SetNdivisions(4,0,2);
    grp2_h_cut_in->GetYaxis()->SetRangeUser(-yScaleH,yScaleH);
    TAxis *xaxisgrp2_h_cut_in= grp2_h_cut_in->GetXaxis();
    xaxisgrp2_h_cut_in->SetLimits(-0.2,2.2);
    grp2_h_cut_in->GetXaxis()->SetLabelColor(0);

    TLegend *legend2H_in = new TLegend(0.13,0.650,0.50,0.895,"","brNDC");
    legend2H_in->SetNColumns(2);
    legend2H_in->AddEntry(grpX_h_cut_in,  Form("X"), "p");
    legend2H_in->AddEntry(grpY_h_cut_in,  Form("Y"), "p");
    legend2H_in->AddEntry(grpXp_h_cut_in,  Form("X'"), "p");
    legend2H_in->AddEntry(grpYp_h_cut_in,  Form("Y'"), "p");
    legend2H_in->AddEntry(grpE_h_cut_in,  Form("E"), "p");
    legend2H_in->AddEntry(grpQ_h_cut_in,  Form("A_{Q}"), "p");
    legend2H_in->AddEntry(grp_h_cut_in,  Form("IHWP-IN Total"), "p");
    legend2H_in->AddEntry(grp_h_stats,Form("Expected statistical shift"), "f");
    legend2H_in->SetFillColor(0);
    legend2H_in->SetBorderSize(2);
    legend2H_in->SetTextSize(0.055);
    legend2H_in->Draw("");

    textHAxis2->Draw();textHAxis25->Draw();textHAxis3->Draw();textHAxis8->Draw();



    pad162->cd(2);

    TGraphErrors* grpX_h_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpX_h_cut_out ->SetName("grpX_h_cut_out");
    grpX_h_cut_out ->Draw("goff");
    grpX_h_cut_out ->SetMarkerSize(0.8);
    grpX_h_cut_out ->SetMarkerStyle(21);
    grpX_h_cut_out ->SetMarkerColor(kRed);
    grpX_h_cut_out ->SetLineColor(kRed);

    TGraphErrors* grpY_h_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutY,zero2,zero2);
    grpY_h_cut_out ->SetName("grpY_h_cut_out");
    grpY_h_cut_out ->Draw("goff");
    grpY_h_cut_out ->SetMarkerSize(0.8);
    grpY_h_cut_out ->SetMarkerStyle(21);
    grpY_h_cut_out ->SetMarkerColor(kGreen);
    grpY_h_cut_out ->SetLineColor(kGreen);

    TGraphErrors* grpXp_h_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpXp_h_cut_out ->SetName("grpXp_h_cut_out");
    grpXp_h_cut_out ->Draw("goff");
    grpXp_h_cut_out ->SetMarkerSize(0.9);
    grpXp_h_cut_out ->SetMarkerStyle(22);
    grpXp_h_cut_out ->SetMarkerColor(kRed);
    grpXp_h_cut_out ->SetLineColor(kRed);

    TGraphErrors* grpYp_h_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutY,zero2,zero2);
    grpYp_h_cut_out ->SetName("grpYp_h_cut_out");
    grpYp_h_cut_out ->Draw("goff");
    grpYp_h_cut_out ->SetMarkerSize(0.9);
    grpYp_h_cut_out ->SetMarkerStyle(22);
    grpYp_h_cut_out ->SetMarkerColor(kGreen);
    grpYp_h_cut_out ->SetLineColor(kGreen);
    
    TGraphErrors* grpE_h_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpE_h_cut_out ->SetName("grpE_h_cut_out");
    grpE_h_cut_out ->Draw("goff");
    grpE_h_cut_out ->SetMarkerSize(0.9);
    grpE_h_cut_out ->SetMarkerStyle(20);
    grpE_h_cut_out ->SetMarkerColor(kBlue);
    grpE_h_cut_out ->SetLineColor(kBlue);

    TGraphErrors* grpQ_h_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpQ_h_cut_out ->SetName("grpQ_h_cut_out");
    grpQ_h_cut_out ->Draw("goff");
    grpQ_h_cut_out ->SetMarkerSize(0.9);
    grpQ_h_cut_out ->SetMarkerStyle(20);
    grpQ_h_cut_out ->SetMarkerColor(kMagenta);
    grpQ_h_cut_out ->SetLineColor(kMagenta);


    TMultiGraph * grp2_h_cut_out = new TMultiGraph();
    grp2_h_cut_out->Add(grpX_h_cut_out,"P");
    grp2_h_cut_out->Add(grpY_h_cut_out,"P");
    grp2_h_cut_out->Add(grpXp_h_cut_out,"P");
    grp2_h_cut_out->Add(grpYp_h_cut_out,"P");
    grp2_h_cut_out->Add(grpE_h_cut_out,"P");
    grp2_h_cut_out->Add(grpQ_h_cut_out,"P");
    grp2_h_cut_out->Add(grp_h_cut_out,"P");
    grp2_h_cut_out ->Add(grp_h_stats,"l3");
    grp2_h_cut_out->Draw("A");
    grp2_h_cut_out->SetTitle("Horizontal Transverse: IHWP-OUT");
    grp2_h_cut_out->GetYaxis()->CenterTitle();
    grp2_h_cut_out->GetYaxis()->SetTitle("MD Asym. Cut - NoCut [ppm]");
    grp2_h_cut_out->GetXaxis()->CenterTitle();
    grp2_h_cut_out->GetXaxis()->SetTitle("Applied cut");
    grp2_h_cut_out->GetXaxis()->SetNdivisions(4,0,2);
    grp2_h_cut_out->GetYaxis()->SetRangeUser(-yScaleH,yScaleH);
    TAxis *xaxisgrp2_h_cut_out= grp2_h_cut_out->GetXaxis();
    xaxisgrp2_h_cut_out->SetLimits(-0.2,2.2);
    grp2_h_cut_out->GetXaxis()->SetLabelColor(0);

    TLegend *legend2H_out = new TLegend(0.13,0.650,0.50,0.895,"","brNDC");
    legend2H_out->SetNColumns(2);
    legend2H_out->AddEntry(grpX_h_cut_out,  Form("X"), "p");
    legend2H_out->AddEntry(grpY_h_cut_out,  Form("Y"), "p");
    legend2H_out->AddEntry(grpXp_h_cut_out,  Form("X'"), "p");
    legend2H_out->AddEntry(grpYp_h_cut_out,  Form("Y'"), "p");
    legend2H_out->AddEntry(grpE_h_cut_out,  Form("E"), "p");
    legend2H_out->AddEntry(grpQ_h_cut_out,  Form("A_{Q}"), "p");
    legend2H_out->AddEntry(grp_h_cut_out,  Form("IHWP-OUT Total"), "p");
    legend2H_out->AddEntry(grp_h_stats,Form("Expected statistical shift"), "f");
    legend2H_out->SetFillColor(0);
    legend2H_out->SetBorderSize(2);
    legend2H_out->SetTextSize(0.055);
    legend2H_out->Draw("");

    textHAxis2->Draw();textHAxis25->Draw();textHAxis3->Draw();textHAxis8->Draw();



    TString savePlotCutIndividualH = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_individual_hor_%s"
					,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
					,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c16-> Update();
    c16->Print(savePlotCutIndividualH+".png");
    if(FIGURE){
      c16->Print(savePlotCutIndividualH+".svg");
      c16->Print(savePlotCutIndividualH+".C");
    }

    /*********************************************************************/


    TString titleIndividualV = Form("%s (%s A): MD Regressed (%s) Asymmetry. Cut Dependence for Individual Parameters. Vertical Transverse."
			      ,targ.Data(),qtor_stem.Data(),reg_set.Data());

    TCanvas * c17 = new TCanvas("c17", titleIndividualV,0,0,canvasSize[0],canvasSize[1]);
    c17->Draw();
    c17->SetBorderSize(0);
    c17->cd();
    TPad*pad171 = new TPad("pad171","pad171",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad172 = new TPad("pad172","pad172",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad171->SetFillColor(kWhite);
    pad171->Draw();
    pad172->Draw();
    pad171->cd();
    TLatex * t17 = new TLatex(0.00,0.3,Form("%s",titleIndividualV.Data()));
    t17->SetTextSize(0.45);
    t17->Draw();
    pad172->cd();
    pad172->Divide(1,2);


    pad172->cd(1);

    TGraphErrors* grpX_v_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpX_v_cut_in ->SetName("grpX_v_cut_in");
    grpX_v_cut_in ->Draw("goff");
    grpX_v_cut_in ->SetMarkerSize(0.8);
    grpX_v_cut_in ->SetMarkerStyle(21);
    grpX_v_cut_in ->SetMarkerColor(kRed);
    grpX_v_cut_in ->SetLineColor(kRed);

    TGraphErrors* grpY_v_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInY,zero2,zero2);
    grpY_v_cut_in ->SetName("grpY_v_cut_in");
    grpY_v_cut_in ->Draw("goff");
    grpY_v_cut_in ->SetMarkerSize(0.8);
    grpY_v_cut_in ->SetMarkerStyle(21);
    grpY_v_cut_in ->SetMarkerColor(kGreen);
    grpY_v_cut_in ->SetLineColor(kGreen);

    TGraphErrors* grpXp_v_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpXp_v_cut_in ->SetName("grpXp_v_cut_in");
    grpXp_v_cut_in ->Draw("goff");
    grpXp_v_cut_in ->SetMarkerSize(0.9);
    grpXp_v_cut_in ->SetMarkerStyle(22);
    grpXp_v_cut_in ->SetMarkerColor(kRed);
    grpXp_v_cut_in ->SetLineColor(kRed);

    TGraphErrors* grpYp_v_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInY,zero2,zero2);
    grpYp_v_cut_in ->SetName("grpYp_v_cut_in");
    grpYp_v_cut_in ->Draw("goff");
    grpYp_v_cut_in ->SetMarkerSize(0.9);
    grpYp_v_cut_in ->SetMarkerStyle(22);
    grpYp_v_cut_in ->SetMarkerColor(kGreen);
    grpYp_v_cut_in ->SetLineColor(kGreen);
    
    TGraphErrors* grpE_v_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpE_v_cut_in ->SetName("grpE_v_cut_in");
    grpE_v_cut_in ->Draw("goff");
    grpE_v_cut_in ->SetMarkerSize(0.9);
    grpE_v_cut_in ->SetMarkerStyle(20);
    grpE_v_cut_in ->SetMarkerColor(kBlue);
    grpE_v_cut_in ->SetLineColor(kBlue);

    TGraphErrors* grpQ_v_cut_in  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymInX,zero2,zero2);
    grpQ_v_cut_in ->SetName("grpQ_v_cut_in");
    grpQ_v_cut_in ->Draw("goff");
    grpQ_v_cut_in ->SetMarkerSize(0.9);
    grpQ_v_cut_in ->SetMarkerStyle(20);
    grpQ_v_cut_in ->SetMarkerColor(kMagenta);
    grpQ_v_cut_in ->SetLineColor(kMagenta);


    TMultiGraph * grp2_v_cut_in = new TMultiGraph();
    grp2_v_cut_in->Add(grpX_v_cut_in,"P");
    grp2_v_cut_in->Add(grpY_v_cut_in,"P");
    grp2_v_cut_in->Add(grpXp_v_cut_in,"P");
    grp2_v_cut_in->Add(grpYp_v_cut_in,"P");
    grp2_v_cut_in->Add(grpE_v_cut_in,"P");
    grp2_v_cut_in->Add(grpQ_v_cut_in,"P");
    grp2_v_cut_in->Add(grp_v_cut_in,"P");
    grp2_v_cut_in ->Add(grp_v_stats,"l3");
    grp2_v_cut_in->Draw("A");
    grp2_v_cut_in->SetTitle("Vertical Transverse: IHWP-IN");
    grp2_v_cut_in->GetYaxis()->CenterTitle();
    grp2_v_cut_in->GetYaxis()->SetTitle("MD Asym. Cut - NoCut [ppm]");
    grp2_v_cut_in->GetXaxis()->CenterTitle();
    grp2_v_cut_in->GetXaxis()->SetTitle("Applied cut");
    grp2_v_cut_in->GetXaxis()->SetNdivisions(4,0,2);
    grp2_v_cut_in->GetYaxis()->SetRangeUser(-yScaleV,yScaleV);
    TAxis *xaxisgrp2_v_cut_in= grp2_v_cut_in->GetXaxis();
    xaxisgrp2_v_cut_in->SetLimits(-0.2,2.2);
    grp2_v_cut_in->GetXaxis()->SetLabelColor(0);

    TLegend *legend2V_in = new TLegend(0.13,0.650,0.50,0.895,"","brNDC");
    legend2V_in->SetNColumns(2);
    legend2V_in->AddEntry(grpX_v_cut_in,  Form("X"), "p");
    legend2V_in->AddEntry(grpY_v_cut_in,  Form("Y"), "p");
    legend2V_in->AddEntry(grpXp_v_cut_in,  Form("X'"), "p");
    legend2V_in->AddEntry(grpYp_v_cut_in,  Form("Y'"), "p");
    legend2V_in->AddEntry(grpE_v_cut_in,  Form("E"), "p");
    legend2V_in->AddEntry(grpQ_v_cut_in,  Form("A_{Q}"), "p");
    legend2V_in->AddEntry(grp_v_cut_in,  Form("IHWP-IN Total"), "p");
    legend2V_in->AddEntry(grp_v_stats,Form("Expected statistical shift"), "f");
    legend2V_in->SetFillColor(0);
    legend2V_in->SetBorderSize(2);
    legend2V_in->SetTextSize(0.055);
    legend2V_in->Draw("");

    textVAxis2->Draw();textVAxis25->Draw();textVAxis3->Draw();textVAxis8->Draw();



    pad172->cd(2);

    TGraphErrors* grpX_v_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpX_v_cut_out ->SetName("grpX_v_cut_out");
    grpX_v_cut_out ->Draw("goff");
    grpX_v_cut_out ->SetMarkerSize(0.8);
    grpX_v_cut_out ->SetMarkerStyle(21);
    grpX_v_cut_out ->SetMarkerColor(kRed);
    grpX_v_cut_out ->SetLineColor(kRed);

    TGraphErrors* grpY_v_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutY,zero2,zero2);
    grpY_v_cut_out ->SetName("grpY_v_cut_out");
    grpY_v_cut_out ->Draw("goff");
    grpY_v_cut_out ->SetMarkerSize(0.8);
    grpY_v_cut_out ->SetMarkerStyle(21);
    grpY_v_cut_out ->SetMarkerColor(kGreen);
    grpY_v_cut_out ->SetLineColor(kGreen);

    TGraphErrors* grpXp_v_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpXp_v_cut_out ->SetName("grpXp_v_cut_out");
    grpXp_v_cut_out ->Draw("goff");
    grpXp_v_cut_out ->SetMarkerSize(0.9);
    grpXp_v_cut_out ->SetMarkerStyle(22);
    grpXp_v_cut_out ->SetMarkerColor(kRed);
    grpXp_v_cut_out ->SetLineColor(kRed);

    TGraphErrors* grpYp_v_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutY,zero2,zero2);
    grpYp_v_cut_out ->SetName("grpYp_v_cut_out");
    grpYp_v_cut_out ->Draw("goff");
    grpYp_v_cut_out ->SetMarkerSize(0.9);
    grpYp_v_cut_out ->SetMarkerStyle(22);
    grpYp_v_cut_out ->SetMarkerColor(kGreen);
    grpYp_v_cut_out ->SetLineColor(kGreen);
    
    TGraphErrors* grpE_v_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpE_v_cut_out ->SetName("grpE_v_cut_out");
    grpE_v_cut_out ->Draw("goff");
    grpE_v_cut_out ->SetMarkerSize(0.9);
    grpE_v_cut_out ->SetMarkerStyle(20);
    grpE_v_cut_out ->SetMarkerColor(kBlue);
    grpE_v_cut_out ->SetLineColor(kBlue);

    TGraphErrors* grpQ_v_cut_out  = new TGraphErrors(NUM2,sigmaPlot2,hTransAsymOutX,zero2,zero2);
    grpQ_v_cut_out ->SetName("grpQ_v_cut_out");
    grpQ_v_cut_out ->Draw("goff");
    grpQ_v_cut_out ->SetMarkerSize(0.9);
    grpQ_v_cut_out ->SetMarkerStyle(20);
    grpQ_v_cut_out ->SetMarkerColor(kMagenta);
    grpQ_v_cut_out ->SetLineColor(kMagenta);


    TMultiGraph * grp2_v_cut_out = new TMultiGraph();
    grp2_v_cut_out->Add(grpX_v_cut_out,"P");
    grp2_v_cut_out->Add(grpY_v_cut_out,"P");
    grp2_v_cut_out->Add(grpXp_v_cut_out,"P");
    grp2_v_cut_out->Add(grpYp_v_cut_out,"P");
    grp2_v_cut_out->Add(grpE_v_cut_out,"P");
    grp2_v_cut_out->Add(grpQ_v_cut_out,"P");
    grp2_v_cut_out->Add(grp_v_cut_out,"P");
    grp2_v_cut_out ->Add(grp_v_stats,"l3");
    grp2_v_cut_out->Draw("A");
    grp2_v_cut_out->SetTitle("Vertical Transverse: IHWP-OUT");
    grp2_v_cut_out->GetYaxis()->CenterTitle();
    grp2_v_cut_out->GetYaxis()->SetTitle("MD Asym. Cut - NoCut [ppm]");
    grp2_v_cut_out->GetXaxis()->CenterTitle();
    grp2_v_cut_out->GetXaxis()->SetTitle("Applied cut");
    grp2_v_cut_out->GetXaxis()->SetNdivisions(4,0,2);
    grp2_v_cut_out->GetYaxis()->SetRangeUser(-yScaleV,yScaleV);
    TAxis *xaxisgrp2_v_cut_out= grp2_v_cut_out->GetXaxis();
    xaxisgrp2_v_cut_out->SetLimits(-0.2,2.2);
    grp2_v_cut_out->GetXaxis()->SetLabelColor(0);

    TLegend *legend2V_out = new TLegend(0.13,0.650,0.50,0.895,"","brNDC");
    legend2V_out->SetNColumns(2);
    legend2V_out->AddEntry(grpX_v_cut_out,  Form("X"), "p");
    legend2V_out->AddEntry(grpY_v_cut_out,  Form("Y"), "p");
    legend2V_out->AddEntry(grpXp_v_cut_out,  Form("X'"), "p");
    legend2V_out->AddEntry(grpYp_v_cut_out,  Form("Y'"), "p");
    legend2V_out->AddEntry(grpE_v_cut_out,  Form("E"), "p");
    legend2V_out->AddEntry(grpQ_v_cut_out,  Form("A_{Q}"), "p");
    legend2V_out->AddEntry(grp_v_cut_out,  Form("IHWP-OUT Total"), "p");
    legend2V_out->AddEntry(grp_v_stats,Form("Expected statistical shift"), "f");
    legend2V_out->SetFillColor(0);
    legend2V_out->SetBorderSize(2);
    legend2V_out->SetTextSize(0.055);
    legend2V_out->Draw("");

    textVAxis2->Draw();textVAxis25->Draw();textVAxis3->Draw();textVAxis8->Draw();



    TString savePlotCutIndividualV = Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_cutDependence_individual_var_%s"
					,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
					,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c17-> Update();
    c17->Print(savePlotCutIndividualV+".png");
    if(FIGURE){
      c17->Print(savePlotCutIndividualV+".svg");
      c17->Print(savePlotCutIndividualV+".C");
    }

    /*********************************************************************/


















  }












  //   Double_t verifyX = histDiffRunInX->GetMean(2);

  //   if(verifyX>2*widthInDiffX){
  //     cout<<red<<run_number<<normal<<endl;
  //     }
  //   else{
  //     cout<<red<<run_number<<normal<<endl;
  //   }


  /*********************************************************************/
  /*********************************************************************/
  /*********************************************************************/




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


