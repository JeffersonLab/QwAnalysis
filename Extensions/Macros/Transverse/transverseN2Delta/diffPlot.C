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

  Bool_t FIGURE = 0;

  Double_t figSize = 2.0;

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
  Int_t canvasSize[2] ={1600*figSize,1000*figSize};
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
  TString titleInOut2 = Form("%s (H+V transverse, %s A): Regression-%s %s %s. No sign flips, or cuts applied."
			     ,targ.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());

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
//   gStyle->SetStatFontSize(0.09);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  //   gStyle->SetNdivisions(507,"x");

  // histo parameters
  gStyle->SetTitleYOffset(0.50);
  gStyle->SetTitleXOffset(0.85);
  gStyle->SetLabelSize(0.09,"x");
  gStyle->SetLabelSize(0.09,"y");
  gStyle->SetTitleSize(0.09,"x");
  gStyle->SetTitleSize(0.09,"y");
  gStyle->SetTitleX(0.20);
  gStyle->SetTitleW(0.65);
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

  TFile *f = new TFile(Form("dirPlot/cutDependence/n-to-delta_6700_h_transverse_HYDROGEN-CELL_regression_off_on_5+1_RunletAvg_diff_run2_pass5.root"));

  //   TFile *f = new TFile("transverseN2deltaDiff.root");
//   if(!f->IsOpen()) {
//     printf("%sUnable to find Rootfile%s\n",blue,normal);     
//     exit(1);
//   }

  /* Load different Trees. */
  TChain *Hel_Tree = new TChain("Hel_Tree");
  if(Hel_Tree == NULL) {printf("%sUnable to find %sHel_Tree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}
  Char_t runH[255],runV[255];
//   sprintf(runCC,"dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_%s_diff_%s.root"
// 	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
// 	  ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data());

//   Hel_Tree->Add(Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_%s_diff_%s.root",interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
// 		     ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

//   sprintf(runCC,"dirPlot/cutDependence/%s_%s_*_%s_regression_%s_%s_%s_diff_%s.root"
// 	  ,interaction.Data(),qtor_stem.Data(),target.Data()
// 	  ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data());

  sprintf(runH, "dirPlot/cutDependence/n-to-delta_6700_h_transverse_HYDROGEN-CELL_regression_off_on_5+1_RunletAvg_diff_run2_pass5.root");
  sprintf(runV, "dirPlot/cutDependence/n-to-delta_6700_v_transverse_HYDROGEN-CELL_regression_off_on_5+1_RunletAvg_diff_run2_pass5.root");


  TFile f1(runH);
  Hel_Tree->Add(runH);
  Hel_Tree->Add(runV);

//   Hel_Tree->Add(Form("dirPlot/cutDependence/%s_%s_v-transverse_%s_regression_%s_%s_%s_diff_%s.root",interaction.Data(),qtor_stem.Data(),target.Data()
// 		     ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));
//   Hel_Tree->Add(Form("dirPlot/cutDependence/%s_%s_h-transverse_%s_regression_%s_%s_%s_diff_%s.root",interaction.Data(),qtor_stem.Data(),target.Data()
// 		     ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

//   TTree *Hel_Tree = (TTree*)f->Get("Hel_Tree");
//   if(Hel_Tree == NULL) {printf("%sUnable to find %sHel_Tree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}
//   //   TChain * tree = new TChain("Hel_Tree");


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


  TH1 *histDiffX,  *histDiffY;
  TH1 *histDiffXSlope, *histDiffYSlope;
  TH1 *histDiffE, *histDiffQ;

  cout<<blue<<__LINE__<<normal<<endl;


  /*********************************************************************/
  /*********************************************************************/

    TCanvas * c11 = new TCanvas("c11", titleInOut,0,0,canvasSize[0],canvasSize[1]);
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
    histDiffInX->SetXTitle("X Differences [nm]");
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
    histDiffInXSlope->SetXTitle("XSlope Differences [nrad]");
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
    histDiffInY->SetXTitle("Y Differences [nm]");
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
    histDiffInYSlope->SetXTitle("YSlope Differences [nrad]");
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
    histDiffInE->SetXTitle("E Differences [ppb]");
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
    histDiffInQ->SetXTitle("Charge Differences [ppb]");
    histDiffInQ->SetYTitle("Number of Runlets");
    histDiffInQ->SetTitle("");
    histDiffInQ->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();


    TString savePlotDiffIn = Form("dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_cutDependence_diffIn_%s"
				  ,interaction.Data(),qtor_stem.Data(),target.Data()
				  ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c11-> Update();
    c11->Print(savePlotDiffIn+".png");
    if(FIGURE){
      c11->Print(savePlotDiffIn+".svg");
      c11->Print(savePlotDiffIn+".C");
    }


    /*********************************************************************/

    TCanvas * c12 = new TCanvas("c12", titleInOut,0,0,canvasSize[0],canvasSize[1]);
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
    histDiffOutX->SetXTitle("X Differences [nm]");
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
    histDiffOutXSlope->SetXTitle("XSlope Differences [nrad]");
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
    histDiffOutY->SetXTitle("Y Differences [nm]");
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
    histDiffOutYSlope->SetXTitle("YSlope Differences [nrad]");
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
    histDiffOutE->SetXTitle("E Differences [ppb]");
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
    histDiffOutQ->SetXTitle("Charge Differences [ppb]");
    histDiffOutQ->SetYTitle("Number of Runlets");
    histDiffOutQ->SetTitle("");
    histDiffOutQ->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();


    TString savePlotDiffOut = Form("dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_cutDependence_diffOut_%s"
				   ,interaction.Data(),qtor_stem.Data(),target.Data()
				   ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c12-> Update();
//     c12->Print(savePlotDiffOut+".png");
//     if(FIGURE){
//       c12->Print(savePlotDiffOut+".svg");
//       c12->Print(savePlotDiffOut+".C");
//     }

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

    /*********************************************************************/
    /*********************************************************************/


    TCanvas * c21 = new TCanvas("c21", titleInOut,0,0,canvasSize[0],canvasSize[1]);
    c21->Draw();
    c21->SetBorderSize(0);
    c21->cd();
    TPad*pad211 = new TPad("pad211","pad211",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad212 = new TPad("pad212","pad212",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad211->SetFillColor(kWhite);
    pad211->Draw();
    pad212->Draw();
    pad211->cd();
    TLatex * t21 = new TLatex(0.00,0.3,Form("%s",titleInOut2.Data()));
    t21->SetTextSize(0.45);
    t21->Draw();
    pad212->cd();
    pad212->Divide(2,3);


    pad212->cd(1);
    Hel_Tree->Draw("diff*1e6>>diffX","id==1","goff");
    histDiffX = (TH1 *)gDirectory->Get("diffX");
    histDiffX->Draw("");
    histDiffX->SetFillColor(kRed);
    histDiffX->GetXaxis()->CenterTitle();
    histDiffX->GetYaxis()->CenterTitle();
    histDiffX->SetXTitle("X Differences [nm]");
    histDiffX->SetYTitle("Number of Runlets");
    histDiffX->SetTitle("");
    histDiffX->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad212->cd(2);
    Hel_Tree->Draw("diff*1e9>>diffXSlope","id==3","goff");
    histDiffXSlope = (TH1 *)gDirectory->Get("diffXSlope");
    histDiffXSlope->Draw("");
    histDiffXSlope->SetFillColor(kRed);
    histDiffXSlope->GetXaxis()->CenterTitle();
    histDiffXSlope->GetYaxis()->CenterTitle();
    histDiffXSlope->SetXTitle("XSlope Differences [nrad]");
    histDiffXSlope->SetYTitle("Number of Runlets");
    histDiffXSlope->SetTitle("");
    histDiffXSlope->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad212->cd(3);
    Hel_Tree->Draw("diff*1e6>>diffY","id==2","goff");
    histDiffY = (TH1 *)gDirectory->Get("diffY");
    histDiffY->Draw("");
    histDiffY->SetFillColor(kGreen);
    histDiffY->GetXaxis()->CenterTitle();
    histDiffY->GetYaxis()->CenterTitle();
    histDiffY->SetXTitle("Y Differences [nm]");
    histDiffY->SetYTitle("Number of Runlets");
    histDiffY->SetTitle("");
    histDiffY->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad212->cd(4);
    Hel_Tree->Draw("diff*1e9>>diffYSlope","id==4","goff");
    histDiffYSlope = (TH1 *)gDirectory->Get("diffYSlope");
    histDiffYSlope->Draw("");
    histDiffYSlope->SetFillColor(kGreen);
    histDiffYSlope->GetXaxis()->CenterTitle();
    histDiffYSlope->GetYaxis()->CenterTitle();
    histDiffYSlope->SetXTitle("YSlope Differences [nrad]");
    histDiffYSlope->SetYTitle("Number of Runlets");
    histDiffYSlope->SetTitle("");
    histDiffYSlope->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad212->cd(5);
    Hel_Tree->Draw("diff*1e9>>diffE","id==5","goff");
    histDiffE = (TH1 *)gDirectory->Get("diffE");
    histDiffE->Draw("");
    histDiffE->SetFillColor(kBlue);
    histDiffE->GetXaxis()->CenterTitle();
    histDiffE->GetYaxis()->CenterTitle();
    histDiffE->SetXTitle("E Differences [ppb]");
    histDiffE->SetYTitle("Number of Runlets");
    histDiffE->SetTitle("");
    histDiffE->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();

    pad212->cd(6);
    Hel_Tree->Draw("diff*1e9>>diffQ","id==6","goff");
    histDiffQ = (TH1 *)gDirectory->Get("diffQ");
    histDiffQ->Draw("");
    histDiffQ->SetFillColor(kMagenta);
    histDiffQ->GetXaxis()->CenterTitle();
    histDiffQ->GetYaxis()->CenterTitle();
    histDiffQ->SetXTitle("Charge Differences [ppb]");
    histDiffQ->SetYTitle("Number of Runlets");
    histDiffQ->SetTitle("");
    histDiffQ->GetYaxis()->SetNdivisions(4,0,0);
    gPad->Update();


    TString savePlotDiff = Form("dirPlot/cutDependence/%s_%s_%s_regression_%s_%s_cutDependence_diff_%s"
				  ,interaction.Data(),qtor_stem.Data(),target.Data()
				  ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
    c21-> Update();
    c21->Print(savePlotDiff+".png");
    if(FIGURE){
      c21->Print(savePlotDiff+".svg");
      c21->Print(savePlotDiff+".C");
    }


    /*********************************************************************/











  /*********************************************************************/
  /*********************************************************************/
  /*********************************************************************/




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


