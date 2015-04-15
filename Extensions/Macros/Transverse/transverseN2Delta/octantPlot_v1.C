//*****************************************************************************************************//
// Author      : Nuruzzaman on 03/19/2013, Transverse N2Delta.
// Last Update : 03/19/2013
// 
//*****************************************************************************************************//


using namespace std;
#include "NurClass.h"

int main(Int_t argc,Char_t* argv[]){


  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  Bool_t SCALE = 0;
  Bool_t FIGURE = 0;
  Bool_t PRELIMINARY = 0;


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
  TString showFit1,showFit2,showFit3,showSinFit,showCosFit;

  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[4] ={1600,1000,1200,650};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t markerSize[6] = {0.9,0.7,0.7,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.900,0.75,0.995};
  Double_t yScale[2] = {-10.0,10.0};
  Double_t waterMark[2] = {2.5,-1.0};


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
  Double_t YRange[2];
  //   TString device = "MD"; 
  // //   TString deviceTitle = "Pos. PMT"; TString deviceName = "PosPMT";
  // //   TString deviceTitle = "Neg. PMT";  TString deviceName = "NegPMT";
  // //   TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";
  //   TString deviceTitle = "Barsum"; TString deviceName = "Barsum";
  Int_t device_opt = 1;

  std::cout<<Form("Enter device (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. %sMain Detector (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("2. Downstream Lumi")<<std::endl;
  std::string input_device_opt;
  std::getline( std::cin, input_device_opt );
  if ( !input_device_opt.empty() ) {
    std::istringstream stream( input_device_opt );
    stream >> device_opt;
  }
  if(device_opt == 1){device = "MD"; deviceTitle = "PMTavg"; deviceName = "PMTavg"; }
  //   if(device_opt == 1){device = "MD"; deviceTitle = "Barsum"; deviceName = "Barsum"; }
  if(device_opt == 2){device = "DSLumi"; deviceTitle = "Sum"; deviceName = "Sum"; }
  if(device_opt  > 2){printf("%sWrong choise of device \n%s",red,normal); exit(0);}



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

  //   // Fit function to show 
  //   showFit1 = Form("FIT = p_{1} cos(#phi) - (#partialA/#partial%s)*sin(#phi) + D",senType.Data());
  //   showFit2 = Form("FIT = p_{1} cos(#phi + #phi_{0}) + (#partialA/#partial%s)",senType.Data());
  //   showFit3 = Form("FIT = p_{1} sin(#phi + #phi_{0}) + (#partialA/#partial%s)",senType.Data());


  //   int position1[8] = { 4, 1, 2, 3, 6, 9, 8, 7 };


  const Int_t NUMC = 10;

  Double_t octant[NUMC];
  Double_t avgDiffXIn[NUMC],avgDiffXSlopeIn[NUMC],avgDiffYIn[NUMC],avgDiffYSlopeIn[NUMC],avgDiffEIn[NUMC],avgChargeIn[NUMC],zeroAvg[NUMC];
  Double_t eavgDiffXIn[NUMC],eavgDiffXSlopeIn[NUMC],eavgDiffYIn[NUMC],eavgDiffYSlopeIn[NUMC],eavgDiffEIn[NUMC],eavgChargeIn[NUMC];
  Double_t avgSenXIn[NUMC],avgSenXSlopeIn[NUMC],avgSenYIn[NUMC],avgSenYSlopeIn[NUMC],avgSenEIn[NUMC],avgSenChargeIn[NUMC];
  Double_t eavgSenXIn[NUMC],eavgSenXSlopeIn[NUMC],eavgSenYIn[NUMC],eavgSenYSlopeIn[NUMC],eavgSenEIn[NUMC],eavgSenChargeIn[NUMC];
  Double_t avgCorXIn[NUMC],avgCorXSlopeIn[NUMC],avgCorYIn[NUMC],avgCorYSlopeIn[NUMC],avgCorEIn[NUMC],avgCorChargeIn[NUMC],avgCorTotalIn[NUMC];
  Double_t eavgCorXIn[NUMC],eavgCorXSlopeIn[NUMC],eavgCorYIn[NUMC],eavgCorYSlopeIn[NUMC],eavgCorEIn[NUMC],eavgCorChargeIn[NUMC],eavgCorTotalIn[NUMC];

  Double_t avgDiffXOut[NUMC],avgDiffXSlopeOut[NUMC],avgDiffYOut[NUMC],avgDiffYSlopeOut[NUMC],avgDiffEOut[NUMC],avgChargeOut[NUMC];
  Double_t eavgDiffXOut[NUMC],eavgDiffXSlopeOut[NUMC],eavgDiffYOut[NUMC],eavgDiffYSlopeOut[NUMC],eavgDiffEOut[NUMC],eavgChargeOut[NUMC];
  Double_t avgSenXOut[NUMC],avgSenXSlopeOut[NUMC],avgSenYOut[NUMC],avgSenYSlopeOut[NUMC],avgSenEOut[NUMC],avgSenChargeOut[NUMC];
  Double_t eavgSenXOut[NUMC],eavgSenXSlopeOut[NUMC],eavgSenYOut[NUMC],eavgSenYSlopeOut[NUMC],eavgSenEOut[NUMC],eavgSenChargeOut[NUMC];
  Double_t avgCorXOut[NUMC],avgCorXSlopeOut[NUMC],avgCorYOut[NUMC],avgCorYSlopeOut[NUMC],avgCorEOut[NUMC],avgCorChargeOut[NUMC],avgCorTotalOut[NUMC];
  Double_t eavgCorXOut[NUMC],eavgCorXSlopeOut[NUMC],eavgCorYOut[NUMC],eavgCorYSlopeOut[NUMC],eavgCorEOut[NUMC],eavgCorChargeOut[NUMC],eavgCorTotalOut[NUMC];

  Double_t avgModDiffXIn[NUMC],avgModDiffXSlopeIn[NUMC],avgModDiffYIn[NUMC],avgModDiffYSlopeIn[NUMC],avgModDiffEIn[NUMC],avgModChargeIn[NUMC];
  Double_t eavgModDiffXIn[NUMC],eavgModDiffXSlopeIn[NUMC],eavgModDiffYIn[NUMC],eavgModDiffYSlopeIn[NUMC],eavgModDiffEIn[NUMC],eavgModChargeIn[NUMC];
  Double_t avgModSenXIn[NUMC],avgModSenXSlopeIn[NUMC],avgModSenYIn[NUMC],avgModSenYSlopeIn[NUMC],avgModSenEIn[NUMC],avgModSenChargeIn[NUMC];
  Double_t eavgModSenXIn[NUMC],eavgModSenXSlopeIn[NUMC],eavgModSenYIn[NUMC],eavgModSenYSlopeIn[NUMC],eavgModSenEIn[NUMC],eavgModSenChargeIn[NUMC];
  Double_t avgModCorXIn[NUMC],avgModCorXSlopeIn[NUMC],avgModCorYIn[NUMC],avgModCorYSlopeIn[NUMC],avgModCorEIn[NUMC],avgModCorChargeIn[NUMC],avgModCorTotalIn[NUMC];
  Double_t eavgModCorXIn[NUMC],eavgModCorXSlopeIn[NUMC],eavgModCorYIn[NUMC],eavgModCorYSlopeIn[NUMC],eavgModCorEIn[NUMC],eavgModCorChargeIn[NUMC],eavgModCorTotalIn[NUMC];
  Double_t eavgModCorXIn2[NUMC],eavgModCorXSlopeIn2[NUMC],eavgModCorYIn2[NUMC],eavgModCorYSlopeIn2[NUMC],eavgModCorEIn2[NUMC],eavgModCorChargeIn2[NUMC],eavgModCorTotalIn2[NUMC];

  Double_t avgModDiffXOut[NUMC],avgModDiffXSlopeOut[NUMC],avgModDiffYOut[NUMC],avgModDiffYSlopeOut[NUMC],avgModDiffEOut[NUMC],avgModChargeOut[NUMC];
  Double_t eavgModDiffXOut[NUMC],eavgModDiffXSlopeOut[NUMC],eavgModDiffYOut[NUMC],eavgModDiffYSlopeOut[NUMC],eavgModDiffEOut[NUMC],eavgModChargeOut[NUMC];
  Double_t avgModSenXOut[NUMC],avgModSenXSlopeOut[NUMC],avgModSenYOut[NUMC],avgModSenYSlopeOut[NUMC],avgModSenEOut[NUMC],avgModSenChargeOut[NUMC];
  Double_t eavgModSenXOut[NUMC],eavgModSenXSlopeOut[NUMC],eavgModSenYOut[NUMC],eavgModSenYSlopeOut[NUMC],eavgModSenEOut[NUMC],eavgModSenChargeOut[NUMC];
  Double_t avgModCorXOut[NUMC],avgModCorXSlopeOut[NUMC],avgModCorYOut[NUMC],avgModCorYSlopeOut[NUMC],avgModCorEOut[NUMC],avgModCorChargeOut[NUMC],avgModCorTotalOut[NUMC];
  Double_t eavgModCorXOut[NUMC],eavgModCorXSlopeOut[NUMC],eavgModCorYOut[NUMC],eavgModCorYSlopeOut[NUMC],eavgModCorEOut[NUMC],eavgModCorChargeOut[NUMC],eavgModCorTotalOut[NUMC];
  Double_t eavgModCorXOut2[NUMC],eavgModCorXSlopeOut2[NUMC],eavgModCorYOut2[NUMC],eavgModCorYSlopeOut2[NUMC],eavgModCorEOut2[NUMC],eavgModCorChargeOut2[NUMC],eavgModCorTotalOut2[NUMC];


  int counterDiff=0,counterSenX=0,counterSenY=0,counterSenXSlope=0,counterSenYSlope=0,counterSenE=0,counterSenCharge=0;
  ifstream diffAvg,senXAvg,senYAvg,senXSlopeAvg,senYSlopeAvg,senEAvg,senChargeAvg;

  /****************************************/
  /****************************************/

  diffAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_beam_parameter_differences_regression_%s_%s_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
		    ,reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (diffAvg.is_open()) {

    while(!diffAvg.eof()) {
      
      diffAvg >> avgDiffXIn[counterDiff]>> eavgDiffXIn[counterDiff]
	      >> avgDiffYIn[counterDiff] >> eavgDiffYIn[counterDiff]
	      >> avgDiffXSlopeIn[counterDiff] >> eavgDiffXSlopeIn[counterDiff]
	      >> avgDiffYSlopeIn[counterDiff] >> eavgDiffYSlopeIn[counterDiff]
	      >> avgDiffEIn[counterDiff] >> eavgDiffEIn[counterDiff]
	      >> avgChargeIn[counterDiff] >> eavgChargeIn[counterDiff]
	      >> avgDiffXOut[counterDiff]>> eavgDiffXOut[counterDiff]
	      >> avgDiffYOut[counterDiff] >> eavgDiffYOut[counterDiff]
	      >> avgDiffXSlopeOut[counterDiff] >> eavgDiffXSlopeOut[counterDiff]
	      >> avgDiffYSlopeOut[counterDiff] >> eavgDiffYSlopeOut[counterDiff]
	      >> avgDiffEOut[counterDiff] >> eavgDiffEOut[counterDiff]
	      >> avgChargeOut[counterDiff] >> eavgChargeOut[counterDiff];
    
      if (!diffAvg.good()) break;

      counterDiff++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  diffAvg.close();

  cout <<blue
       <<"XIn = "<<avgDiffXIn[0]<<"+-"<<eavgDiffXIn[0]<<"\n"
       <<"YIn = "<<avgDiffYIn[0]<<"+-"<<eavgDiffYIn[0]<<"\n"
       <<"X'In = "<<avgDiffXSlopeIn[0]<<"+-"<<eavgDiffXSlopeIn[0]<<"\n"
       <<"Y'In = "<<avgDiffYSlopeIn[0]<<"+-"<<eavgDiffYSlopeIn[0]<<"\n"
       <<"EIn = "<<avgDiffEIn[0]<<"+-"<<eavgDiffEIn[0]<<"\n"
       <<"QIn = "<<avgChargeIn[0]<<"+-"<<eavgChargeIn[0]
       <<normal<<endl;

  cout <<red
       <<"XOut = "<<avgDiffXOut[0]<<"+-"<<eavgDiffXOut[0]<<"\n"
       <<"YOut = "<<avgDiffYOut[0]<<"+-"<<eavgDiffYOut[0]<<"\n"
       <<"X'Out = "<<avgDiffXSlopeOut[0]<<"+-"<<eavgDiffXSlopeOut[0]<<"\n"
       <<"Y'Out = "<<avgDiffYSlopeOut[0]<<"+-"<<eavgDiffYSlopeOut[0]<<"\n"
       <<"EOut = "<<avgDiffEOut[0]<<"+-"<<eavgDiffEOut[0]<<"\n"
       <<"QOut = "<<avgChargeOut[0]<<"+-"<<eavgChargeOut[0]
       <<normal<<endl;

  /****************************************/
  /****************************************/

  senXAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senX_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (senXAvg.is_open()) {

    while(!senXAvg.eof()) {
      zeroAvg[counterSenX]=0;
      eavgModCorXIn2[counterSenX] = 0; eavgModCorXOut2[counterSenX] = 0;

      senXAvg >> octant[counterSenX]>>avgSenXIn[counterSenX]>>eavgSenXIn[counterSenX]>>avgSenXOut[counterSenX]>>eavgSenXOut[counterSenX];
      if (!senXAvg.good()) break;
      avgCorXIn[counterSenX]  = avgSenXIn[counterSenX]*avgDiffXIn[0];   eavgCorXIn[counterSenX]  = eavgSenXIn[counterSenX]*eavgDiffXOut[0];
      avgCorXOut[counterSenX] = avgSenXOut[counterSenX]*avgDiffXOut[0]; eavgCorXOut[counterSenX] = eavgSenXOut[counterSenX]*eavgDiffXOut[0];

      avgModSenXIn[counterSenX] = avgSenXIn[counterSenX]*1e0; eavgModSenXIn[counterSenX] = eavgSenXIn[counterSenX]*1e0;
      avgModSenXOut[counterSenX] = avgSenXOut[counterSenX]*1e0; eavgModSenXOut[counterSenX] = eavgSenXOut[counterSenX]*1e0;

      avgModCorXIn[counterSenX] = avgCorXIn[counterSenX];  eavgModCorXIn[counterSenX] = eavgCorXIn[counterSenX];
      avgModCorXOut[counterSenX] = avgCorXOut[counterSenX]; eavgModCorXOut[counterSenX] = eavgCorXOut[counterSenX]; 

      counterSenX++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  senXAvg.close();

  /****************************************/

  senYAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senY_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (senYAvg.is_open()) {

    while(!senYAvg.eof()) {
      eavgModCorYIn2[counterSenY] = 0; eavgModCorYOut2[counterSenY] = 0;

      senYAvg >> octant[counterSenY]>>avgSenYIn[counterSenY]>>eavgSenYIn[counterSenY]>>avgSenYOut[counterSenY]>>eavgSenYOut[counterSenY];
      if (!senYAvg.good()) break;
      avgCorYIn[counterSenY]  = avgSenYIn[counterSenY]*avgDiffYIn[0];   eavgCorYIn[counterSenY]  = eavgSenYIn[counterSenY]*eavgDiffYOut[0];
      avgCorYOut[counterSenY] = avgSenYOut[counterSenY]*avgDiffYOut[0]; eavgCorYOut[counterSenY] = eavgSenYOut[counterSenY]*eavgDiffYOut[0];

      avgModSenYIn[counterSenY] = avgSenYIn[counterSenY]*1e0; eavgModSenYIn[counterSenY] = eavgSenYIn[counterSenY]*1e0;
      avgModSenYOut[counterSenY] = avgSenYOut[counterSenY]*1e0; eavgModSenYOut[counterSenY] = eavgSenYOut[counterSenY]*1e0;

      avgModCorYIn[counterSenY] = avgCorYIn[counterSenY];  eavgModCorYIn[counterSenY] = eavgCorYIn[counterSenY];
      avgModCorYOut[counterSenY] = avgCorYOut[counterSenY]; eavgModCorYOut[counterSenY] = eavgCorYOut[counterSenY]; 

      counterSenY++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  senYAvg.close();

  /****************************************/

  senXSlopeAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senXSlope_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (senXSlopeAvg.is_open()) {

    while(!senXSlopeAvg.eof()) {
      eavgModCorXSlopeIn2[counterSenXSlope] = 0; eavgModCorXSlopeOut2[counterSenXSlope] = 0;

      senXSlopeAvg >> octant[counterSenXSlope]>>avgSenXSlopeIn[counterSenXSlope]>>eavgSenXSlopeIn[counterSenXSlope]>>avgSenXSlopeOut[counterSenXSlope]>>eavgSenXSlopeOut[counterSenXSlope];
      if (!senXSlopeAvg.good()) break;
      avgCorXSlopeIn[counterSenXSlope]  = avgSenXSlopeIn[counterSenXSlope]*avgDiffXSlopeIn[0];   eavgCorXSlopeIn[counterSenXSlope]  = eavgSenXSlopeIn[counterSenXSlope]*eavgDiffXSlopeOut[0];
      avgCorXSlopeOut[counterSenXSlope] = avgSenXSlopeOut[counterSenXSlope]*avgDiffXSlopeOut[0]; eavgCorXSlopeOut[counterSenXSlope] = eavgSenXSlopeOut[counterSenXSlope]*eavgDiffXSlopeOut[0];

      avgModSenXSlopeIn[counterSenXSlope] = avgSenXSlopeIn[counterSenXSlope]*1e0; eavgModSenXSlopeIn[counterSenXSlope] = eavgSenXSlopeIn[counterSenXSlope]*1e0;
      avgModSenXSlopeOut[counterSenXSlope] = avgSenXSlopeOut[counterSenXSlope]*1e0; eavgModSenXSlopeOut[counterSenXSlope] = eavgSenXSlopeOut[counterSenXSlope]*1e0;

      avgModCorXSlopeIn[counterSenXSlope] = avgCorXSlopeIn[counterSenXSlope];  eavgModCorXSlopeIn[counterSenXSlope] = eavgCorXSlopeIn[counterSenXSlope];
      avgModCorXSlopeOut[counterSenXSlope] = avgCorXSlopeOut[counterSenXSlope]; eavgModCorXSlopeOut[counterSenXSlope] = eavgCorXSlopeOut[counterSenXSlope]; 

      counterSenXSlope++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  senXSlopeAvg.close();

  /****************************************/

  senYSlopeAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senYSlope_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (senYSlopeAvg.is_open()) {

    while(!senYSlopeAvg.eof()) {
      eavgModCorYSlopeIn2[counterSenYSlope] = 0; eavgModCorYSlopeOut2[counterSenYSlope] = 0;

      senYSlopeAvg >> octant[counterSenYSlope]>>avgSenYSlopeIn[counterSenYSlope]>>eavgSenYSlopeIn[counterSenYSlope]>>avgSenYSlopeOut[counterSenYSlope]>>eavgSenYSlopeOut[counterSenYSlope];
      if (!senYSlopeAvg.good()) break;
      avgCorYSlopeIn[counterSenYSlope]  = avgSenYSlopeIn[counterSenYSlope]*avgDiffYSlopeIn[0];   eavgCorYSlopeIn[counterSenYSlope]  = eavgSenYSlopeIn[counterSenYSlope]*eavgDiffYSlopeOut[0];
      avgCorYSlopeOut[counterSenYSlope] = avgSenYSlopeOut[counterSenYSlope]*avgDiffYSlopeOut[0]; eavgCorYSlopeOut[counterSenYSlope] = eavgSenYSlopeOut[counterSenYSlope]*eavgDiffYSlopeOut[0];

      avgModSenYSlopeIn[counterSenYSlope] = avgSenYSlopeIn[counterSenYSlope]*1e0; eavgModSenYSlopeIn[counterSenYSlope] = eavgSenYSlopeIn[counterSenYSlope]*1e0;
      avgModSenYSlopeOut[counterSenYSlope] = avgSenYSlopeOut[counterSenYSlope]*1e0; eavgModSenYSlopeOut[counterSenYSlope] = eavgSenYSlopeOut[counterSenYSlope]*1e0;

      avgModCorYSlopeIn[counterSenYSlope] = avgCorYSlopeIn[counterSenYSlope];  eavgModCorYSlopeIn[counterSenYSlope] = eavgCorYSlopeIn[counterSenYSlope];
      avgModCorYSlopeOut[counterSenYSlope] = avgCorYSlopeOut[counterSenYSlope]; eavgModCorYSlopeOut[counterSenYSlope] = eavgCorYSlopeOut[counterSenYSlope]; 

      counterSenYSlope++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  senYSlopeAvg.close();

  /****************************************/

  senEAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senE_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (senEAvg.is_open()) {

    while(!senEAvg.eof()) {
      eavgModCorEIn2[counterSenE] = 0; eavgModCorEOut2[counterSenE] = 0;

      senEAvg >> octant[counterSenE]>>avgSenEIn[counterSenE]>>eavgSenEIn[counterSenE]>>avgSenEOut[counterSenE]>>eavgSenEOut[counterSenE];
      if (!senEAvg.good()) break;
      avgCorEIn[counterSenE]  = avgSenEIn[counterSenE]*avgDiffEIn[0];   eavgCorEIn[counterSenE]  = eavgSenEIn[counterSenE]*eavgDiffEOut[0];
      avgCorEOut[counterSenE] = avgSenEOut[counterSenE]*avgDiffEOut[0]; eavgCorEOut[counterSenE] = eavgSenEOut[counterSenE]*eavgDiffEOut[0];

      avgModSenEIn[counterSenE] = avgSenEIn[counterSenE]*1e0; eavgModSenEIn[counterSenE] = eavgSenEIn[counterSenE]*1e0;
      avgModSenEOut[counterSenE] = avgSenEOut[counterSenE]*1e0; eavgModSenEOut[counterSenE] = eavgSenEOut[counterSenE]*1e0;

      avgModCorEIn[counterSenE] = avgCorEIn[counterSenE];  eavgModCorEIn[counterSenE] = eavgCorEIn[counterSenE];
      avgModCorEOut[counterSenE] = avgCorEOut[counterSenE]; eavgModCorEOut[counterSenE] = eavgCorEOut[counterSenE]; 

      counterSenE++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  senEAvg.close();

  /****************************************/

  senChargeAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senCharge_%s.txt"
		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  if (senChargeAvg.is_open()) {

    while(!senChargeAvg.eof()) {
      eavgModCorChargeIn2[counterSenCharge] = 0; eavgModCorChargeOut2[counterSenCharge] = 0;

      senChargeAvg >> octant[counterSenCharge]>>avgSenChargeIn[counterSenCharge]>>eavgSenChargeIn[counterSenCharge]>>avgSenChargeOut[counterSenCharge]>>eavgSenChargeOut[counterSenCharge];
      if (!senChargeAvg.good()) break;
      avgCorChargeIn[counterSenCharge]  = avgSenChargeIn[counterSenCharge]*avgChargeIn[0];   eavgCorChargeIn[counterSenCharge]  = eavgSenChargeIn[counterSenCharge]*eavgChargeOut[0];
      avgCorChargeOut[counterSenCharge] = avgSenChargeOut[counterSenCharge]*avgChargeOut[0]; eavgCorChargeOut[counterSenCharge] = eavgSenChargeOut[counterSenCharge]*eavgChargeOut[0];

      avgModSenChargeIn[counterSenCharge] = avgSenChargeIn[counterSenCharge]*1e0; eavgModSenChargeIn[counterSenCharge] = eavgSenChargeIn[counterSenCharge]*1e0;
      avgModSenChargeOut[counterSenCharge] = avgSenChargeOut[counterSenCharge]*1e0; eavgModSenChargeOut[counterSenCharge] = eavgSenChargeOut[counterSenCharge]*1e0;

      avgModCorChargeIn[counterSenCharge] = avgCorChargeIn[counterSenCharge];  eavgModCorChargeIn[counterSenCharge] = eavgCorChargeIn[counterSenCharge];
      avgModCorChargeOut[counterSenCharge] = avgCorChargeOut[counterSenCharge]; eavgModCorChargeOut[counterSenCharge] = eavgCorChargeOut[counterSenCharge]; 

      counterSenCharge++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  senChargeAvg.close();

  for ( Int_t i=0; i<8; i++) {
    avgModCorTotalIn[i] = avgModCorXIn[i]+avgModCorYIn[i]+avgModCorXSlopeIn[i]+avgModCorYSlopeIn[i]+avgModCorYSlopeIn[i]+avgModCorEIn[i]+avgModCorChargeIn[i];
    avgModCorTotalOut[i] = avgModCorXOut[i]+avgModCorYOut[i]+avgModCorXSlopeOut[i]+avgModCorYSlopeOut[i]+avgModCorYSlopeOut[i]+avgModCorEOut[i]+avgModCorChargeOut[i];
    cout <<"MD-"<<i+1<<blue<<" TotalIn = "<<avgModCorTotalIn[i]<<red<<"\tTotalOut = "<<avgModCorTotalIn[i]<<normal<<endl;
  }

  /****************************************/

  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" XIn = "<<avgSenXIn[i]<<"+-"<<eavgSenXIn[i]<<red<<"\tXOut = "<<avgSenXOut[i]<<"+-"<<eavgSenXOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" YIn = "<<avgSenYIn[i]<<"+-"<<eavgSenXIn[i]<<red<<"\tYOut = "<<avgSenYOut[i]<<"+-"<<eavgSenYOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" X'In = "<<avgSenXSlopeIn[i]<<"+-"<<eavgSenXSlopeIn[i]<<red<<"\tX'Out = "<<avgSenXSlopeOut[i]<<"+-"<<eavgSenXSlopeOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" Y'In = "<<avgSenYSlopeIn[i]<<"+-"<<eavgSenYSlopeIn[i]<<red<<"\tY'Out = "<<avgSenYSlopeOut[i]<<"+-"<<eavgSenYSlopeOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" EIn = "<<avgSenEIn[i]<<"+-"<<eavgSenEIn[i]<<red<<"\tEOut = "<<avgSenEOut[i]<<"+-"<<eavgSenEOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" QIn = "<<avgSenChargeIn[i]<<"+-"<<eavgSenChargeIn[i]<<red<<"\tQOut = "<<avgSenChargeOut[i]<<"+-"<<eavgSenChargeOut[i]<<normal<<endl;
  }
  /****************************************/

  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" XIn = "<<avgCorXIn[i]<<"+-"<<eavgCorXIn[i]<<red<<"\tXOut = "<<avgCorXOut[i]<<"+-"<<eavgCorXOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" YIn = "<<avgCorYIn[i]<<"+-"<<eavgCorXIn[i]<<red<<"\tYOut = "<<avgCorYOut[i]<<"+-"<<eavgCorYOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" X'In = "<<avgCorXSlopeIn[i]<<"+-"<<eavgCorXSlopeIn[i]<<red<<"\tX'Out = "<<avgCorXSlopeOut[i]<<"+-"<<eavgCorXSlopeOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" Y'In = "<<avgCorYSlopeIn[i]<<"+-"<<eavgCorYSlopeIn[i]<<red<<"\tY'Out = "<<avgCorYSlopeOut[i]<<"+-"<<eavgCorYSlopeOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" EIn = "<<avgCorEIn[i]<<"+-"<<eavgCorEIn[i]<<red<<"\tEOut = "<<avgCorEOut[i]<<"+-"<<eavgCorEOut[i]<<normal<<endl;
  }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" QIn = "<<avgCorChargeIn[i]<<"+-"<<eavgCorChargeIn[i]<<red<<"\tQOut = "<<avgCorChargeOut[i]<<"+-"<<eavgCorChargeOut[i]<<normal<<endl;
  }


  /****************************************/
  /****************************************/

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.10);
  gStyle->SetStatH(0.3);
  
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
  gStyle->SetPadRightMargin(0.25);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.85);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetTitleX(0.12);
  gStyle->SetTitleW(0.28);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.06);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gDirectory->Delete("*");

  Double_t det_range[2],fit_range[2];
  det_range[0] = 0.5; det_range[1] = 8.5;
  fit_range[0] = 0.8; fit_range[1] = 8.2;

  TF1* cosFit  = new TF1("cosFit","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* sinFit  = new TF1("sinFit","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);

//   TF1* fitOctantVariationXIn  = new TF1("fitOctantVariationXIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationXOut = new TF1("fitOctantVariationXOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationXSlopeIn  = new TF1("fitOctantVariationXSlopeIn","[0] + [1]*cos((x-1)*pi/4 + [2])",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationXSlopeOut = new TF1("fitOctantVariationXSlopeOut","[0] + [1]*cos((x-1)*pi/4 + [2])",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationYIn  = new TF1("fitOctantVariationYIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationYOut = new TF1("fitOctantVariationYOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationYSlopeIn  = new TF1("fitOctantVariationYSlopeIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationYSlopeOut = new TF1("fitOctantVariationYSlopeOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationEIn  = new TF1("fitOctantVariationEIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* fitOctantVariationEOut = new TF1("fitOctantVariationEOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);

  showCosFit = "FIT = p_{0} + p_{1}cos[#frac{#pi}{4}(x-1)]";
  showSinFit = "FIT = p_{0} + p_{1}sin[#frac{#pi}{4}(x-1)]";

  TString titleSen = Form("%s (%s, %s A): Regression-%s %s %s Sensitivity "
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());

  TString titleCor = Form("%s (%s, %s A): Regression-%s %s %s Corrections "
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());


  TCanvas * c1 = new TCanvas("canvas1", titleSen,0,0,canvasSize[0],canvasSize[1]);
  c1->Draw();
  c1->SetBorderSize(0);
  c1->cd();
  TPad*pad11 = new TPad("pad11","pad11",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad12 = new TPad("pad12","pad12",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad11->SetFillColor(kWhite);
  pad11->Draw();
  pad12->Draw();
  pad11->cd();
  TLatex * t11 = new TLatex(0.06,0.3,Form("%s",titleSen.Data()));
  t11->SetTextSize(0.5);
  t11->Draw();
  pad12->cd();
  pad12->Divide(2,3);

  /********************************************************/

  pad12->cd(1);

  TGraphErrors * SenXGraphIn = new TGraphErrors(counterSenX,octant,avgModSenXIn,zeroAvg,eavgModSenXIn);
  SenXGraphIn->SetName("SenXGraphIn");
  SenXGraphIn->SetMarkerColor(kRed);
  SenXGraphIn->SetLineColor(kRed);
  SenXGraphIn->SetMarkerStyle(21);
  SenXGraphIn->SetMarkerSize(markerSize[1]);
  SenXGraphIn->SetLineWidth(1);
  //SenXGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  SenXGraphIn->Fit("cosFit","EMRF0Q");
  TF1* fitSenXIn = SenXGraphIn->GetFunction("cosFit");
  fitSenXIn->SetLineStyle(1);
  fitSenXIn->SetLineWidth(2);
  fitSenXIn->SetLineColor(kRed);

  TGraphErrors * SenXGraphOut = new TGraphErrors(counterSenX,octant,avgModSenXOut,zeroAvg,eavgModSenXOut);
  SenXGraphOut->SetName("SenXGraphOut");
  SenXGraphOut->SetMarkerColor(kBlack);
  SenXGraphOut->SetLineColor(kBlack);
  SenXGraphOut->SetMarkerStyle(21);
  SenXGraphOut->SetMarkerSize(markerSize[1]);
  SenXGraphOut->SetLineWidth(1);
  SenXGraphOut->Fit("cosFit","EMRF0Q");
  TF1* fitSenXOut = SenXGraphOut->GetFunction("cosFit");
  fitSenXOut->SetLineStyle(1);
  fitSenXOut->SetLineWidth(2);
  fitSenXOut->SetLineColor(kBlack);

  TMultiGraph *SenXGraph = new TMultiGraph();
  SenXGraph->Add(SenXGraphIn);
  SenXGraph->Add(SenXGraphOut);
  SenXGraph->Draw("AP");
  SenXGraph->SetTitle(Form("X : %s",showCosFit.Data()));
  SenXGraph->GetXaxis()->SetNdivisions(8,0,0);
  SenXGraph->GetXaxis()->SetTitle(Form("Octant"));
  SenXGraph->GetYaxis()->SetTitle("X Sensitivity [ppb/nm]");
  SenXGraph->GetXaxis()->CenterTitle();
  SenXGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisSenXGraph= SenXGraph->GetXaxis();
//   xaxisSenXGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenXIn =(TPaveStats*)SenXGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenXOut =(TPaveStats*)SenXGraphOut->GetListOfFunctions()->FindObject("stats");
  statsSenXIn->SetTextColor(kRed);
  statsSenXIn->SetFillColor(kWhite); 
  statsSenXOut->SetTextColor(kBlack);
  statsSenXOut->SetFillColor(kWhite);
  statsSenXIn->SetTextSize(0.045);
  statsSenXOut->SetTextSize(0.045);
  statsSenXIn->SetX1NDC(x_lo_stat_in4);    statsSenXIn->SetX2NDC(x_hi_stat_in4); 
  statsSenXIn->SetY1NDC(y_lo_stat_in4);    statsSenXIn->SetY2NDC(y_hi_stat_in4);
  statsSenXOut->SetX1NDC(x_lo_stat_out4);  statsSenXOut->SetX2NDC(x_hi_stat_out4); 
  statsSenXOut->SetY1NDC(y_lo_stat_out4);  statsSenXOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenXInp0        =   fitSenXIn->GetParameter(0);
  Double_t calc_SenXInp1        =   fitSenXIn->GetParameter(1);
  Double_t calc_SenXInp2        =   fitSenXIn->GetParameter(2);
  Double_t calc_eSenXInp0       =   fitSenXIn->GetParError(0);
  Double_t calc_eSenXInp1       =   fitSenXIn->GetParError(1);
  Double_t calc_eSenXInp2       =   fitSenXIn->GetParError(2);
  Double_t calc_SenXInChisquare =   fitSenXIn->GetChisquare();
  Double_t calc_SenXInNDF       =   fitSenXIn->GetNDF();

  Double_t calc_SenXOutp0        =   fitSenXOut->GetParameter(0);
  Double_t calc_SenXOutp1        =   fitSenXOut->GetParameter(1);
  Double_t calc_SenXOutp2        =   fitSenXOut->GetParameter(2);
  Double_t calc_eSenXOutp0       =   fitSenXOut->GetParError(0);
  Double_t calc_eSenXOutp1       =   fitSenXOut->GetParError(1);
  Double_t calc_eSenXOutp2       =   fitSenXOut->GetParError(2);
  Double_t calc_SenXOutChisquare =   fitSenXOut->GetChisquare();
  Double_t calc_SenXOutNDF       =   fitSenXOut->GetNDF();

  TLegend *legSenX = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenX->AddEntry("SenXGraphIn",  Form("In X = %2.2f #pm %2.2f",calc_SenXInp0,calc_eSenXInp0),"lp");
  legSenX->AddEntry("SenXGraphOut",  Form("Out X = %2.2f #pm %2.2f",calc_SenXOutp0,calc_eSenXOutp0),"lp");
  legSenX->SetTextSize(0.040);
  legSenX->SetFillColor(0);
  legSenX->SetBorderSize(2);
  legSenX->Draw();

  fitSenXIn->Draw("same");
  fitSenXOut->Draw("same");

  fitSenXIn->SetParNames("#frac{#partialA}{#partialX_{IN}}","amp.");

  gPad->Update();

  /********************************************************/

  pad12->cd(2);

  TGraphErrors * SenXSlopeGraphIn = new TGraphErrors(counterSenXSlope,octant,avgModSenXSlopeIn,zeroAvg,eavgModSenXSlopeIn);
  SenXSlopeGraphIn->SetName("SenXSlopeGraphIn");
  SenXSlopeGraphIn->SetMarkerColor(kRed);
  SenXSlopeGraphIn->SetLineColor(kRed);
  SenXSlopeGraphIn->SetMarkerStyle(22);
  SenXSlopeGraphIn->SetMarkerSize(markerSize[0]);
  SenXSlopeGraphIn->SetLineWidth(1);
  //SenXSlopeGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  SenXSlopeGraphIn->Fit("cosFit","EMRF0Q");
  TF1* fitSenXSlopeIn = SenXSlopeGraphIn->GetFunction("cosFit");
  fitSenXSlopeIn->SetLineStyle(1);
  fitSenXSlopeIn->SetLineWidth(2);
  fitSenXSlopeIn->SetLineColor(kRed);

  TGraphErrors * SenXSlopeGraphOut = new TGraphErrors(counterSenXSlope,octant,avgModSenXSlopeOut,zeroAvg,eavgModSenXSlopeOut);
  SenXSlopeGraphOut->SetName("SenXSlopeGraphOut");
  SenXSlopeGraphOut->SetMarkerColor(kBlack);
  SenXSlopeGraphOut->SetLineColor(kBlack);
  SenXSlopeGraphOut->SetMarkerStyle(22);
  SenXSlopeGraphOut->SetMarkerSize(markerSize[0]);
  SenXSlopeGraphOut->SetLineWidth(1);
  SenXSlopeGraphOut->Fit("cosFit","EMRF0Q");
  TF1* fitSenXSlopeOut = SenXSlopeGraphOut->GetFunction("cosFit");
  fitSenXSlopeOut->SetLineStyle(1);
  fitSenXSlopeOut->SetLineWidth(2);
  fitSenXSlopeOut->SetLineColor(kBlack);

  TMultiGraph *SenXSlopeGraph = new TMultiGraph();
  SenXSlopeGraph->Add(SenXSlopeGraphIn);
  SenXSlopeGraph->Add(SenXSlopeGraphOut);
  SenXSlopeGraph->Draw("AP");
  SenXSlopeGraph->SetTitle(Form("X' : %s",showCosFit.Data()));
  SenXSlopeGraph->GetXaxis()->SetNdivisions(8,0,0);
  SenXSlopeGraph->GetXaxis()->SetTitle(Form("Octant"));
  SenXSlopeGraph->GetYaxis()->SetTitle("X' Sensitivity [ppb/nrad]");
  SenXSlopeGraph->GetXaxis()->CenterTitle();
  SenXSlopeGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisSenXSlopeGraph= SenXSlopeGraph->GetXaxis();
//   xaxisSenXSlopeGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenXSlopeIn =(TPaveStats*)SenXSlopeGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenXSlopeOut =(TPaveStats*)SenXSlopeGraphOut->GetListOfFunctions()->FindObject("stats");
  statsSenXSlopeIn->SetTextColor(kRed);
  statsSenXSlopeIn->SetFillColor(kWhite); 
  statsSenXSlopeOut->SetTextColor(kBlack);
  statsSenXSlopeOut->SetFillColor(kWhite);
  statsSenXSlopeIn->SetTextSize(0.045);
  statsSenXSlopeOut->SetTextSize(0.045);
  statsSenXSlopeIn->SetX1NDC(x_lo_stat_in4);    statsSenXSlopeIn->SetX2NDC(x_hi_stat_in4); 
  statsSenXSlopeIn->SetY1NDC(y_lo_stat_in4);    statsSenXSlopeIn->SetY2NDC(y_hi_stat_in4);
  statsSenXSlopeOut->SetX1NDC(x_lo_stat_out4);  statsSenXSlopeOut->SetX2NDC(x_hi_stat_out4); 
  statsSenXSlopeOut->SetY1NDC(y_lo_stat_out4);  statsSenXSlopeOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenXSlopeInp0        =   fitSenXSlopeIn->GetParameter(0);
  Double_t calc_SenXSlopeInp1        =   fitSenXSlopeIn->GetParameter(1);
  Double_t calc_SenXSlopeInp2        =   fitSenXSlopeIn->GetParameter(2);
  Double_t calc_eSenXSlopeInp0       =   fitSenXSlopeIn->GetParError(0);
  Double_t calc_eSenXSlopeInp1       =   fitSenXSlopeIn->GetParError(1);
  Double_t calc_eSenXSlopeInp2       =   fitSenXSlopeIn->GetParError(2);
  Double_t calc_SenXSlopeInChisquare =   fitSenXSlopeIn->GetChisquare();
  Double_t calc_SenXSlopeInNDF       =   fitSenXSlopeIn->GetNDF();

  Double_t calc_SenXSlopeOutp0        =   fitSenXSlopeOut->GetParameter(0);
  Double_t calc_SenXSlopeOutp1        =   fitSenXSlopeOut->GetParameter(1);
  Double_t calc_SenXSlopeOutp2        =   fitSenXSlopeOut->GetParameter(2);
  Double_t calc_eSenXSlopeOutp0       =   fitSenXSlopeOut->GetParError(0);
  Double_t calc_eSenXSlopeOutp1       =   fitSenXSlopeOut->GetParError(1);
  Double_t calc_eSenXSlopeOutp2       =   fitSenXSlopeOut->GetParError(2);
  Double_t calc_SenXSlopeOutChisquare =   fitSenXSlopeOut->GetChisquare();
  Double_t calc_SenXSlopeOutNDF       =   fitSenXSlopeOut->GetNDF();

  TLegend *legSenXSlope = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenXSlope->AddEntry("SenXSlopeGraphIn",  Form("In X' = %2.2f #pm %2.2f",calc_SenXSlopeInp0,calc_eSenXSlopeInp0),"lp");
  legSenXSlope->AddEntry("SenXSlopeGraphOut",  Form("Out X' = %2.2f #pm %2.2f",calc_SenXSlopeOutp0,calc_eSenXSlopeOutp0),"lp");
  legSenXSlope->SetTextSize(0.040);
  legSenXSlope->SetFillColor(0);
  legSenXSlope->SetBorderSize(2);
  legSenXSlope->Draw();

  fitSenXSlopeIn->Draw("same");
  fitSenXSlopeOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad12->cd(3);

  TGraphErrors * SenYGraphIn = new TGraphErrors(counterSenY,octant,avgModSenYIn,zeroAvg,eavgModSenYIn);
  SenYGraphIn->SetName("SenYGraphIn");
  SenYGraphIn->SetMarkerColor(kGreen+1);
  SenYGraphIn->SetLineColor(kGreen+1);
  SenYGraphIn->SetMarkerStyle(21);
  SenYGraphIn->SetMarkerSize(markerSize[1]);
  SenYGraphIn->SetLineWidth(1);
  //SenYGraphIn->Fit("fitOctantVariationYIn","E M R F 0 Q");
  SenYGraphIn->Fit("sinFit","EMRF0Q");
  TF1* fitSenYIn = SenYGraphIn->GetFunction("sinFit");
  fitSenYIn->SetLineStyle(1);
  fitSenYIn->SetLineWidth(2);
  fitSenYIn->SetLineColor(kGreen+1);

  TGraphErrors * SenYGraphOut = new TGraphErrors(counterSenY,octant,avgModSenYOut,zeroAvg,eavgModSenYOut);
  SenYGraphOut->SetName("SenYGraphOut");
  SenYGraphOut->SetMarkerColor(kBlack);
  SenYGraphOut->SetLineColor(kBlack);
  SenYGraphOut->SetMarkerStyle(21);
  SenYGraphOut->SetMarkerSize(markerSize[1]);
  SenYGraphOut->SetLineWidth(1);
  SenYGraphOut->Fit("sinFit","EMRF0Q");
  TF1* fitSenYOut = SenYGraphOut->GetFunction("sinFit");
  fitSenYOut->SetLineStyle(1);
  fitSenYOut->SetLineWidth(2);
  fitSenYOut->SetLineColor(kBlack);

  TMultiGraph *SenYGraph = new TMultiGraph();
  SenYGraph->Add(SenYGraphIn);
  SenYGraph->Add(SenYGraphOut);
  SenYGraph->Draw("AP");
  SenYGraph->SetTitle(Form("Y : %s",showSinFit.Data()));
  SenYGraph->GetXaxis()->SetNdivisions(8,0,0);
  SenYGraph->GetXaxis()->SetTitle(Form("Octant"));
  SenYGraph->GetYaxis()->SetTitle("Y Sensitivity [ppb/nm]");
  SenYGraph->GetXaxis()->CenterTitle();
  SenYGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisSenYGraph= SenYGraph->GetXaxis();
//   xaxisSenYGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenYIn =(TPaveStats*)SenYGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenYOut =(TPaveStats*)SenYGraphOut->GetListOfFunctions()->FindObject("stats");
  statsSenYIn->SetTextColor(kGreen+1);
  statsSenYIn->SetFillColor(kWhite); 
  statsSenYOut->SetTextColor(kBlack);
  statsSenYOut->SetFillColor(kWhite);
  statsSenYIn->SetTextSize(0.045);
  statsSenYOut->SetTextSize(0.045);
  statsSenYIn->SetX1NDC(x_lo_stat_in4);    statsSenYIn->SetX2NDC(x_hi_stat_in4); 
  statsSenYIn->SetY1NDC(y_lo_stat_in4);    statsSenYIn->SetY2NDC(y_hi_stat_in4);
  statsSenYOut->SetX1NDC(x_lo_stat_out4);  statsSenYOut->SetX2NDC(x_hi_stat_out4); 
  statsSenYOut->SetY1NDC(y_lo_stat_out4);  statsSenYOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenYInp0        =   fitSenYIn->GetParameter(0);
  Double_t calc_SenYInp1        =   fitSenYIn->GetParameter(1);
  Double_t calc_SenYInp2        =   fitSenYIn->GetParameter(2);
  Double_t calc_eSenYInp0       =   fitSenYIn->GetParError(0);
  Double_t calc_eSenYInp1       =   fitSenYIn->GetParError(1);
  Double_t calc_eSenYInp2       =   fitSenYIn->GetParError(2);
  Double_t calc_SenYInChisquare =   fitSenYIn->GetChisquare();
  Double_t calc_SenYInNDF       =   fitSenYIn->GetNDF();

  Double_t calc_SenYOutp0        =   fitSenYOut->GetParameter(0);
  Double_t calc_SenYOutp1        =   fitSenYOut->GetParameter(1);
  Double_t calc_SenYOutp2        =   fitSenYOut->GetParameter(2);
  Double_t calc_eSenYOutp0       =   fitSenYOut->GetParError(0);
  Double_t calc_eSenYOutp1       =   fitSenYOut->GetParError(1);
  Double_t calc_eSenYOutp2       =   fitSenYOut->GetParError(2);
  Double_t calc_SenYOutChisquare =   fitSenYOut->GetChisquare();
  Double_t calc_SenYOutNDF       =   fitSenYOut->GetNDF();

  TLegend *legSenY = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenY->AddEntry("SenYGraphIn",  Form("In Y = %2.2f #pm %2.2f",calc_SenYInp0,calc_eSenYInp0),"lp");
  legSenY->AddEntry("SenYGraphOut",  Form("Out Y = %2.2f #pm %2.2f",calc_SenYOutp0,calc_eSenYOutp0),"lp");
  legSenY->SetTextSize(0.040);
  legSenY->SetFillColor(0);
  legSenY->SetBorderSize(2);
  legSenY->Draw();

  fitSenYIn->Draw("same");
  fitSenYOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad12->cd(4);

  TGraphErrors * SenYSlopeGraphIn = new TGraphErrors(counterSenYSlope,octant,avgModSenYSlopeIn,zeroAvg,eavgModSenYSlopeIn);
  SenYSlopeGraphIn->SetName("SenYSlopeGraphIn");
  SenYSlopeGraphIn->SetMarkerColor(kGreen+1);
  SenYSlopeGraphIn->SetLineColor(kGreen+1);
  SenYSlopeGraphIn->SetMarkerStyle(22);
  SenYSlopeGraphIn->SetMarkerSize(markerSize[0]);
  SenYSlopeGraphIn->SetLineWidth(1);
  //SenYSlopeGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  SenYSlopeGraphIn->Fit("sinFit","EMRF0Q");
  TF1* fitSenYSlopeIn = SenYSlopeGraphIn->GetFunction("sinFit");
  fitSenYSlopeIn->SetLineStyle(1);
  fitSenYSlopeIn->SetLineWidth(2);
  fitSenYSlopeIn->SetLineColor(kGreen+1);

  TGraphErrors * SenYSlopeGraphOut = new TGraphErrors(counterSenYSlope,octant,avgModSenYSlopeOut,zeroAvg,eavgModSenYSlopeOut);
  SenYSlopeGraphOut->SetName("SenYSlopeGraphOut");
  SenYSlopeGraphOut->SetMarkerColor(kBlack);
  SenYSlopeGraphOut->SetLineColor(kBlack);
  SenYSlopeGraphOut->SetMarkerStyle(22);
  SenYSlopeGraphOut->SetMarkerSize(markerSize[0]);
  SenYSlopeGraphOut->SetLineWidth(1);
  SenYSlopeGraphOut->Fit("sinFit","EMRF0Q");
  TF1* fitSenYSlopeOut = SenYSlopeGraphOut->GetFunction("sinFit");
  fitSenYSlopeOut->SetLineStyle(1);
  fitSenYSlopeOut->SetLineWidth(2);
  fitSenYSlopeOut->SetLineColor(kBlack);

  TMultiGraph *SenYSlopeGraph = new TMultiGraph();
  SenYSlopeGraph->Add(SenYSlopeGraphIn);
  SenYSlopeGraph->Add(SenYSlopeGraphOut);
  SenYSlopeGraph->Draw("AP");
  SenYSlopeGraph->SetTitle(Form("Y' : %s",showSinFit.Data()));
  SenYSlopeGraph->GetXaxis()->SetNdivisions(8,0,0);
  SenYSlopeGraph->GetXaxis()->SetTitle(Form("Octant"));
  SenYSlopeGraph->GetYaxis()->SetTitle("Y' Sensitivity [ppb/nrad]");
  SenYSlopeGraph->GetXaxis()->CenterTitle();
  SenYSlopeGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisSenYSlopeGraph= SenYSlopeGraph->GetXaxis();
//   xaxisSenYSlopeGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenYSlopeIn =(TPaveStats*)SenYSlopeGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenYSlopeOut =(TPaveStats*)SenYSlopeGraphOut->GetListOfFunctions()->FindObject("stats");
  statsSenYSlopeIn->SetTextColor(kGreen+1);
  statsSenYSlopeIn->SetFillColor(kWhite); 
  statsSenYSlopeOut->SetTextColor(kBlack);
  statsSenYSlopeOut->SetFillColor(kWhite);
  statsSenYSlopeIn->SetTextSize(0.045);
  statsSenYSlopeOut->SetTextSize(0.045);
  statsSenYSlopeIn->SetX1NDC(x_lo_stat_in4);    statsSenYSlopeIn->SetX2NDC(x_hi_stat_in4); 
  statsSenYSlopeIn->SetY1NDC(y_lo_stat_in4);    statsSenYSlopeIn->SetY2NDC(y_hi_stat_in4);
  statsSenYSlopeOut->SetX1NDC(x_lo_stat_out4);  statsSenYSlopeOut->SetX2NDC(x_hi_stat_out4); 
  statsSenYSlopeOut->SetY1NDC(y_lo_stat_out4);  statsSenYSlopeOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenYSlopeInp0        =   fitSenYSlopeIn->GetParameter(0);
  Double_t calc_SenYSlopeInp1        =   fitSenYSlopeIn->GetParameter(1);
  Double_t calc_SenYSlopeInp2        =   fitSenYSlopeIn->GetParameter(2);
  Double_t calc_eSenYSlopeInp0       =   fitSenYSlopeIn->GetParError(0);
  Double_t calc_eSenYSlopeInp1       =   fitSenYSlopeIn->GetParError(1);
  Double_t calc_eSenYSlopeInp2       =   fitSenYSlopeIn->GetParError(2);
  Double_t calc_SenYSlopeInChisquare =   fitSenYSlopeIn->GetChisquare();
  Double_t calc_SenYSlopeInNDF       =   fitSenYSlopeIn->GetNDF();

  Double_t calc_SenYSlopeOutp0        =   fitSenYSlopeOut->GetParameter(0);
  Double_t calc_SenYSlopeOutp1        =   fitSenYSlopeOut->GetParameter(1);
  Double_t calc_SenYSlopeOutp2        =   fitSenYSlopeOut->GetParameter(2);
  Double_t calc_eSenYSlopeOutp0       =   fitSenYSlopeOut->GetParError(0);
  Double_t calc_eSenYSlopeOutp1       =   fitSenYSlopeOut->GetParError(1);
  Double_t calc_eSenYSlopeOutp2       =   fitSenYSlopeOut->GetParError(2);
  Double_t calc_SenYSlopeOutChisquare =   fitSenYSlopeOut->GetChisquare();
  Double_t calc_SenYSlopeOutNDF       =   fitSenYSlopeOut->GetNDF();

  TLegend *legSenYSlope = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenYSlope->AddEntry("SenYSlopeGraphIn",  Form("In Y' = %2.2f #pm %2.2f",calc_SenYSlopeInp0,calc_eSenYSlopeInp0),"lp");
  legSenYSlope->AddEntry("SenYSlopeGraphOut",  Form("Out Y' = %2.2f #pm %2.2f",calc_SenYSlopeOutp0,calc_eSenYSlopeOutp0),"lp");
  legSenYSlope->SetTextSize(0.040);
  legSenYSlope->SetFillColor(0);
  legSenYSlope->SetBorderSize(2);
  legSenYSlope->Draw();

  fitSenYSlopeIn->Draw("same");
  fitSenYSlopeOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad12->cd(5);

  TGraphErrors * SenEGraphIn = new TGraphErrors(counterSenE,octant,avgModSenEIn,zeroAvg,eavgModSenEIn);
  SenEGraphIn->SetName("SenEGraphIn");
  SenEGraphIn->SetMarkerColor(kBlue);
  SenEGraphIn->SetLineColor(kBlue);
  SenEGraphIn->SetMarkerStyle(20);
  SenEGraphIn->SetMarkerSize(markerSize[0]);
  SenEGraphIn->SetLineWidth(1);
  //SenEGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  SenEGraphIn->Fit("cosFit","EMRF0Q");
  TF1* fitSenEIn = SenEGraphIn->GetFunction("cosFit");
  fitSenEIn->SetLineStyle(1);
  fitSenEIn->SetLineWidth(2);
  fitSenEIn->SetLineColor(kBlue);

  TGraphErrors * SenEGraphOut = new TGraphErrors(counterSenE,octant,avgModSenEOut,zeroAvg,eavgModSenEOut);
  SenEGraphOut->SetName("SenEGraphOut");
  SenEGraphOut->SetMarkerColor(kBlack);
  SenEGraphOut->SetLineColor(kBlack);
  SenEGraphOut->SetMarkerStyle(20);
  SenEGraphOut->SetMarkerSize(markerSize[0]);
  SenEGraphOut->SetLineWidth(1);
  SenEGraphOut->Fit("cosFit","EMRF0Q");
  TF1* fitSenEOut = SenEGraphOut->GetFunction("cosFit");
  fitSenEOut->SetLineStyle(1);
  fitSenEOut->SetLineWidth(2);
  fitSenEOut->SetLineColor(kBlack);

  TMultiGraph *SenEGraph = new TMultiGraph();
  SenEGraph->Add(SenEGraphIn);
  SenEGraph->Add(SenEGraphOut);
  SenEGraph->Draw("AP");
  SenEGraph->SetTitle(Form("E : %s",showCosFit.Data()));
  SenEGraph->GetXaxis()->SetNdivisions(8,0,0);
  SenEGraph->GetXaxis()->SetTitle(Form("Octant"));
  SenEGraph->GetYaxis()->SetTitle("E Sensitivity [ppb/ppb]");
  SenEGraph->GetXaxis()->CenterTitle();
  SenEGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisSenEGraph= SenEGraph->GetXaxis();
//   xaxisSenEGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenEIn =(TPaveStats*)SenEGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenEOut =(TPaveStats*)SenEGraphOut->GetListOfFunctions()->FindObject("stats");
  statsSenEIn->SetTextColor(kBlue);
  statsSenEIn->SetFillColor(kWhite); 
  statsSenEOut->SetTextColor(kBlack);
  statsSenEOut->SetFillColor(kWhite);
  statsSenEIn->SetTextSize(0.045);
  statsSenEOut->SetTextSize(0.045);
  statsSenEIn->SetX1NDC(x_lo_stat_in4);    statsSenEIn->SetX2NDC(x_hi_stat_in4); 
  statsSenEIn->SetY1NDC(y_lo_stat_in4);    statsSenEIn->SetY2NDC(y_hi_stat_in4);
  statsSenEOut->SetX1NDC(x_lo_stat_out4);  statsSenEOut->SetX2NDC(x_hi_stat_out4); 
  statsSenEOut->SetY1NDC(y_lo_stat_out4);  statsSenEOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenEInp0        =   fitSenEIn->GetParameter(0);
  Double_t calc_SenEInp1        =   fitSenEIn->GetParameter(1);
  Double_t calc_SenEInp2        =   fitSenEIn->GetParameter(2);
  Double_t calc_eSenEInp0       =   fitSenEIn->GetParError(0);
  Double_t calc_eSenEInp1       =   fitSenEIn->GetParError(1);
  Double_t calc_eSenEInp2       =   fitSenEIn->GetParError(2);
  Double_t calc_SenEInChisquare =   fitSenEIn->GetChisquare();
  Double_t calc_SenEInNDF       =   fitSenEIn->GetNDF();

  Double_t calc_SenEOutp0        =   fitSenEOut->GetParameter(0);
  Double_t calc_SenEOutp1        =   fitSenEOut->GetParameter(1);
  Double_t calc_SenEOutp2        =   fitSenEOut->GetParameter(2);
  Double_t calc_eSenEOutp0       =   fitSenEOut->GetParError(0);
  Double_t calc_eSenEOutp1       =   fitSenEOut->GetParError(1);
  Double_t calc_eSenEOutp2       =   fitSenEOut->GetParError(2);
  Double_t calc_SenEOutChisquare =   fitSenEOut->GetChisquare();
  Double_t calc_SenEOutNDF       =   fitSenEOut->GetNDF();

  TLegend *legSenE = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenE->AddEntry("SenEGraphIn",  Form("In E = %2.2f #pm %2.2f",calc_SenEInp0,calc_eSenEInp0),"lp");
  legSenE->AddEntry("SenEGraphOut",  Form("Out E = %2.2f #pm %2.2f",calc_SenEOutp0,calc_eSenEOutp0),"lp");
  legSenE->SetTextSize(0.040);
  legSenE->SetFillColor(0);
  legSenE->SetBorderSize(2);
  legSenE->Draw();

  fitSenEIn->Draw("same");
  fitSenEOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad12->cd(6);

  TGraphErrors * SenChargeGraphIn = new TGraphErrors(counterSenCharge,octant,avgModSenChargeIn,zeroAvg,eavgModSenChargeIn);
  SenChargeGraphIn->SetName("SenChargeGraphIn");
  SenChargeGraphIn->SetMarkerColor(kMagenta);
  SenChargeGraphIn->SetLineColor(kMagenta);
  SenChargeGraphIn->SetMarkerStyle(23);
  SenChargeGraphIn->SetMarkerSize(markerSize[0]);
  SenChargeGraphIn->SetLineWidth(1);
  //SenChargeGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  SenChargeGraphIn->Fit("cosFit","EMRF0Q");
  TF1* fitSenChargeIn = SenChargeGraphIn->GetFunction("cosFit");
  fitSenChargeIn->SetLineStyle(1);
  fitSenChargeIn->SetLineWidth(2);
  fitSenChargeIn->SetLineColor(kMagenta);

  TGraphErrors * SenChargeGraphOut = new TGraphErrors(counterSenCharge,octant,avgModSenChargeOut,zeroAvg,eavgModSenChargeOut);
  SenChargeGraphOut->SetName("SenChargeGraphOut");
  SenChargeGraphOut->SetMarkerColor(kBlack);
  SenChargeGraphOut->SetLineColor(kBlack);
  SenChargeGraphOut->SetMarkerStyle(23);
  SenChargeGraphOut->SetMarkerSize(markerSize[0]);
  SenChargeGraphOut->SetLineWidth(1);
  SenChargeGraphOut->Fit("cosFit","EMRF0Q");
  TF1* fitSenChargeOut = SenChargeGraphOut->GetFunction("cosFit");
  fitSenChargeOut->SetLineStyle(1);
  fitSenChargeOut->SetLineWidth(2);
  fitSenChargeOut->SetLineColor(kBlack);

  TMultiGraph *SenChargeGraph = new TMultiGraph();
  SenChargeGraph->Add(SenChargeGraphIn);
  SenChargeGraph->Add(SenChargeGraphOut);
  SenChargeGraph->Draw("AP");
  SenChargeGraph->SetTitle(Form("A_{Q} : %s",showCosFit.Data()));
  SenChargeGraph->GetXaxis()->SetNdivisions(8,0,0);
  SenChargeGraph->GetXaxis()->SetTitle(Form("Octant"));
  SenChargeGraph->GetYaxis()->SetTitle("Charge Sensitivity [ppb/ppb]");
  SenChargeGraph->GetXaxis()->CenterTitle();
  SenChargeGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisSenChargeGraph= SenChargeGraph->GetXaxis();
//   xaxisSenChargeGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenChargeIn =(TPaveStats*)SenChargeGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenChargeOut =(TPaveStats*)SenChargeGraphOut->GetListOfFunctions()->FindObject("stats");
  statsSenChargeIn->SetTextColor(kMagenta);
  statsSenChargeIn->SetFillColor(kWhite); 
  statsSenChargeOut->SetTextColor(kBlack);
  statsSenChargeOut->SetFillColor(kWhite);
  statsSenChargeIn->SetTextSize(0.045);
  statsSenChargeOut->SetTextSize(0.045);
  statsSenChargeIn->SetX1NDC(x_lo_stat_in4);    statsSenChargeIn->SetX2NDC(x_hi_stat_in4); 
  statsSenChargeIn->SetY1NDC(y_lo_stat_in4);    statsSenChargeIn->SetY2NDC(y_hi_stat_in4);
  statsSenChargeOut->SetX1NDC(x_lo_stat_out4);  statsSenChargeOut->SetX2NDC(x_hi_stat_out4); 
  statsSenChargeOut->SetY1NDC(y_lo_stat_out4);  statsSenChargeOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenChargeInp0        =   fitSenChargeIn->GetParameter(0);
  Double_t calc_SenChargeInp1        =   fitSenChargeIn->GetParameter(1);
  Double_t calc_SenChargeInp2        =   fitSenChargeIn->GetParameter(2);
  Double_t calc_eSenChargeInp0       =   fitSenChargeIn->GetParError(0);
  Double_t calc_eSenChargeInp1       =   fitSenChargeIn->GetParError(1);
  Double_t calc_eSenChargeInp2       =   fitSenChargeIn->GetParError(2);
  Double_t calc_SenChargeInChisquare =   fitSenChargeIn->GetChisquare();
  Double_t calc_SenChargeInNDF       =   fitSenChargeIn->GetNDF();

  Double_t calc_SenChargeOutp0        =   fitSenChargeOut->GetParameter(0);
  Double_t calc_SenChargeOutp1        =   fitSenChargeOut->GetParameter(1);
  Double_t calc_SenChargeOutp2        =   fitSenChargeOut->GetParameter(2);
  Double_t calc_eSenChargeOutp0       =   fitSenChargeOut->GetParError(0);
  Double_t calc_eSenChargeOutp1       =   fitSenChargeOut->GetParError(1);
  Double_t calc_eSenChargeOutp2       =   fitSenChargeOut->GetParError(2);
  Double_t calc_SenChargeOutChisquare =   fitSenChargeOut->GetChisquare();
  Double_t calc_SenChargeOutNDF       =   fitSenChargeOut->GetNDF();

  TLegend *legSenCharge = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenCharge->AddEntry("SenChargeGraphIn",  Form("In A_{Q} = %2.2f #pm %2.2f",calc_SenChargeInp0,calc_eSenChargeInp0),"lp");
  legSenCharge->AddEntry("SenChargeGraphOut",  Form("Out A_{Q} = %2.2f #pm %2.2f",calc_SenChargeOutp0,calc_eSenChargeOutp0),"lp");
  legSenCharge->SetTextSize(0.040);
  legSenCharge->SetFillColor(0);
  legSenCharge->SetBorderSize(2);
  legSenCharge->Draw();

  fitSenChargeIn->Draw("same");
  fitSenChargeOut->Draw("same");

  gPad->Update();

  /********************************************************/

  TString saveSenPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_slug_sensitivity_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),database_stem.Data());


  c1-> Update();
  c1->Print(saveSenPlot+".png");
  if(FIGURE){
    c1->Print(saveSenPlot+".svg");
    c1->Print(saveSenPlot+".C");
  }


  /********************************************************/
  /********************************************************/
  /********************************************************/
  /********************************************************/


  TCanvas * c2 = new TCanvas("canvas2", titleCor,0,0,canvasSize[0],canvasSize[1]);
  c2->Draw();
  c2->SetBorderSize(0);
  c2->cd();
  TPad*pad21 = new TPad("pad21","pad21",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad22 = new TPad("pad22","pad22",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad21->SetFillColor(kWhite);
  pad21->Draw();
  pad22->Draw();
  pad21->cd();
  TLatex * t21 = new TLatex(0.06,0.3,Form("%s",titleCor.Data()));
  t21->SetTextSize(0.5);
  t21->Draw();
  pad22->cd();
  pad22->Divide(2,3);

  /********************************************************/

  pad22->cd(1);

  TGraphErrors * CorXGraphIn = new TGraphErrors(counterSenX,octant,avgModCorXIn,zeroAvg,eavgModCorXIn2);
  CorXGraphIn->SetName("CorXGraphIn");
  CorXGraphIn->SetMarkerColor(kRed);
  CorXGraphIn->SetLineColor(kRed);
  CorXGraphIn->SetMarkerStyle(21);
  CorXGraphIn->SetMarkerSize(markerSize[1]);
  CorXGraphIn->SetLineWidth(1);
  //CorXGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  CorXGraphIn->Fit("cosFit","B0Q");
  TF1* fitCorXIn = CorXGraphIn->GetFunction("cosFit");
  fitCorXIn->SetLineStyle(1);
  fitCorXIn->SetLineWidth(2);
  fitCorXIn->SetLineColor(kRed);

  TGraphErrors * CorXGraphOut = new TGraphErrors(counterSenX,octant,avgModCorXOut,zeroAvg,eavgModCorXOut2);
  CorXGraphOut->SetName("CorXGraphOut");
  CorXGraphOut->SetMarkerColor(kBlack);
  CorXGraphOut->SetLineColor(kBlack);
  CorXGraphOut->SetMarkerStyle(21);
  CorXGraphOut->SetMarkerSize(markerSize[1]);
  CorXGraphOut->SetLineWidth(1);
  CorXGraphOut->Fit("cosFit","B0Q");
  TF1* fitCorXOut = CorXGraphOut->GetFunction("cosFit");
  fitCorXOut->SetLineStyle(1);
  fitCorXOut->SetLineWidth(2);
  fitCorXOut->SetLineColor(kBlack);

  TMultiGraph *CorXGraph = new TMultiGraph();
  CorXGraph->Add(CorXGraphIn);
  CorXGraph->Add(CorXGraphOut);
  CorXGraph->Draw("AP");
  CorXGraph->SetTitle(Form("X : %s",showCosFit.Data()));
  CorXGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorXGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorXGraph->GetYaxis()->SetTitle("X Correction [ppb]");
  CorXGraph->GetXaxis()->CenterTitle();
  CorXGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorXGraph= CorXGraph->GetXaxis();
//   xaxisCorXGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorXIn =(TPaveStats*)CorXGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorXOut =(TPaveStats*)CorXGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorXIn->SetTextColor(kRed);
  statsCorXIn->SetFillColor(kWhite); 
  statsCorXOut->SetTextColor(kBlack);
  statsCorXOut->SetFillColor(kWhite);
  statsCorXIn->SetTextSize(0.045);
  statsCorXOut->SetTextSize(0.045);
  statsCorXIn->SetX1NDC(x_lo_stat_in4);    statsCorXIn->SetX2NDC(x_hi_stat_in4); 
  statsCorXIn->SetY1NDC(y_lo_stat_in4);    statsCorXIn->SetY2NDC(y_hi_stat_in4);
  statsCorXOut->SetX1NDC(x_lo_stat_out4);  statsCorXOut->SetX2NDC(x_hi_stat_out4); 
  statsCorXOut->SetY1NDC(y_lo_stat_out4);  statsCorXOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorXInp0        =   fitCorXIn->GetParameter(0);
  Double_t calc_CorXInp1        =   fitCorXIn->GetParameter(1);
  Double_t calc_CorXInp2        =   fitCorXIn->GetParameter(2);
  Double_t calc_eCorXInp0       =   fitCorXIn->GetParError(0);
  Double_t calc_eCorXInp1       =   fitCorXIn->GetParError(1);
  Double_t calc_eCorXInp2       =   fitCorXIn->GetParError(2);
  Double_t calc_CorXInChisquare =   fitCorXIn->GetChisquare();
  Double_t calc_CorXInNDF       =   fitCorXIn->GetNDF();

  Double_t calc_CorXOutp0        =   fitCorXOut->GetParameter(0);
  Double_t calc_CorXOutp1        =   fitCorXOut->GetParameter(1);
  Double_t calc_CorXOutp2        =   fitCorXOut->GetParameter(2);
  Double_t calc_eCorXOutp0       =   fitCorXOut->GetParError(0);
  Double_t calc_eCorXOutp1       =   fitCorXOut->GetParError(1);
  Double_t calc_eCorXOutp2       =   fitCorXOut->GetParError(2);
  Double_t calc_CorXOutChisquare =   fitCorXOut->GetChisquare();
  Double_t calc_CorXOutNDF       =   fitCorXOut->GetNDF();

  TLegend *legCorX = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorX->AddEntry("CorXGraphIn",  Form("In X = %2.2f #pm %2.2f",calc_CorXInp0,calc_eCorXInp0),"lp");
  legCorX->AddEntry("CorXGraphOut",  Form("Out X = %2.2f #pm %2.2f",calc_CorXOutp0,calc_eCorXOutp0),"lp");
  legCorX->SetTextSize(0.040);
  legCorX->SetFillColor(0);
  legCorX->SetBorderSize(2);
  legCorX->Draw();

  fitCorXIn->Draw("same");
  fitCorXOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad22->cd(2);

  TGraphErrors * CorXSlopeGraphIn = new TGraphErrors(counterSenXSlope,octant,avgModCorXSlopeIn,zeroAvg,eavgModCorXSlopeIn2);
  CorXSlopeGraphIn->SetName("CorXSlopeGraphIn");
  CorXSlopeGraphIn->SetMarkerColor(kRed);
  CorXSlopeGraphIn->SetLineColor(kRed);
  CorXSlopeGraphIn->SetMarkerStyle(22);
  CorXSlopeGraphIn->SetMarkerSize(markerSize[0]);
  CorXSlopeGraphIn->SetLineWidth(1);
  //CorXSlopeGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  CorXSlopeGraphIn->Fit("cosFit","B0Q");
  TF1* fitCorXSlopeIn = CorXSlopeGraphIn->GetFunction("cosFit");
  fitCorXSlopeIn->SetLineStyle(1);
  fitCorXSlopeIn->SetLineWidth(2);
  fitCorXSlopeIn->SetLineColor(kRed);

  TGraphErrors * CorXSlopeGraphOut = new TGraphErrors(counterSenXSlope,octant,avgModCorXSlopeOut,zeroAvg,eavgModCorXSlopeOut2);
  CorXSlopeGraphOut->SetName("CorXSlopeGraphOut");
  CorXSlopeGraphOut->SetMarkerColor(kBlack);
  CorXSlopeGraphOut->SetLineColor(kBlack);
  CorXSlopeGraphOut->SetMarkerStyle(22);
  CorXSlopeGraphOut->SetMarkerSize(markerSize[0]);
  CorXSlopeGraphOut->SetLineWidth(1);
  CorXSlopeGraphOut->Fit("cosFit","B0Q");
  TF1* fitCorXSlopeOut = CorXSlopeGraphOut->GetFunction("cosFit");
  fitCorXSlopeOut->SetLineStyle(1);
  fitCorXSlopeOut->SetLineWidth(2);
  fitCorXSlopeOut->SetLineColor(kBlack);

  TMultiGraph *CorXSlopeGraph = new TMultiGraph();
  CorXSlopeGraph->Add(CorXSlopeGraphIn);
  CorXSlopeGraph->Add(CorXSlopeGraphOut);
  CorXSlopeGraph->Draw("AP");
  CorXSlopeGraph->SetTitle(Form("X' : %s",showCosFit.Data()));
  CorXSlopeGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorXSlopeGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorXSlopeGraph->GetYaxis()->SetTitle("X' Correction [ppb]");
  CorXSlopeGraph->GetXaxis()->CenterTitle();
  CorXSlopeGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorXSlopeGraph= CorXSlopeGraph->GetXaxis();
//   xaxisCorXSlopeGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorXSlopeIn =(TPaveStats*)CorXSlopeGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorXSlopeOut =(TPaveStats*)CorXSlopeGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorXSlopeIn->SetTextColor(kRed);
  statsCorXSlopeIn->SetFillColor(kWhite); 
  statsCorXSlopeOut->SetTextColor(kBlack);
  statsCorXSlopeOut->SetFillColor(kWhite);
  statsCorXSlopeIn->SetTextSize(0.045);
  statsCorXSlopeOut->SetTextSize(0.045);
  statsCorXSlopeIn->SetX1NDC(x_lo_stat_in4);    statsCorXSlopeIn->SetX2NDC(x_hi_stat_in4); 
  statsCorXSlopeIn->SetY1NDC(y_lo_stat_in4);    statsCorXSlopeIn->SetY2NDC(y_hi_stat_in4);
  statsCorXSlopeOut->SetX1NDC(x_lo_stat_out4);  statsCorXSlopeOut->SetX2NDC(x_hi_stat_out4); 
  statsCorXSlopeOut->SetY1NDC(y_lo_stat_out4);  statsCorXSlopeOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorXSlopeInp0        =   fitCorXSlopeIn->GetParameter(0);
  Double_t calc_CorXSlopeInp1        =   fitCorXSlopeIn->GetParameter(1);
  Double_t calc_CorXSlopeInp2        =   fitCorXSlopeIn->GetParameter(2);
  Double_t calc_eCorXSlopeInp0       =   fitCorXSlopeIn->GetParError(0);
  Double_t calc_eCorXSlopeInp1       =   fitCorXSlopeIn->GetParError(1);
  Double_t calc_eCorXSlopeInp2       =   fitCorXSlopeIn->GetParError(2);
  Double_t calc_CorXSlopeInChisquare =   fitCorXSlopeIn->GetChisquare();
  Double_t calc_CorXSlopeInNDF       =   fitCorXSlopeIn->GetNDF();

  Double_t calc_CorXSlopeOutp0        =   fitCorXSlopeOut->GetParameter(0);
  Double_t calc_CorXSlopeOutp1        =   fitCorXSlopeOut->GetParameter(1);
  Double_t calc_CorXSlopeOutp2        =   fitCorXSlopeOut->GetParameter(2);
  Double_t calc_eCorXSlopeOutp0       =   fitCorXSlopeOut->GetParError(0);
  Double_t calc_eCorXSlopeOutp1       =   fitCorXSlopeOut->GetParError(1);
  Double_t calc_eCorXSlopeOutp2       =   fitCorXSlopeOut->GetParError(2);
  Double_t calc_CorXSlopeOutChisquare =   fitCorXSlopeOut->GetChisquare();
  Double_t calc_CorXSlopeOutNDF       =   fitCorXSlopeOut->GetNDF();

  TLegend *legCorXSlope = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorXSlope->AddEntry("CorXSlopeGraphIn",  Form("In X' = %2.2f #pm %2.2f",calc_CorXSlopeInp0,calc_eCorXSlopeInp0),"lp");
  legCorXSlope->AddEntry("CorXSlopeGraphOut",  Form("Out X' = %2.2f #pm %2.2f",calc_CorXSlopeOutp0,calc_eCorXSlopeOutp0),"lp");
  legCorXSlope->SetTextSize(0.040);
  legCorXSlope->SetFillColor(0);
  legCorXSlope->SetBorderSize(2);
  legCorXSlope->Draw();

  fitCorXSlopeIn->Draw("same");
  fitCorXSlopeOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad22->cd(3);

  TGraphErrors * CorYGraphIn = new TGraphErrors(counterSenY,octant,avgModCorYIn,zeroAvg,eavgModCorYIn2);
  CorYGraphIn->SetName("CorYGraphIn");
  CorYGraphIn->SetMarkerColor(kGreen+1);
  CorYGraphIn->SetLineColor(kGreen+1);
  CorYGraphIn->SetMarkerStyle(21);
  CorYGraphIn->SetMarkerSize(markerSize[1]);
  CorYGraphIn->SetLineWidth(1);
  //CorYGraphIn->Fit("fitOctantVariationYIn","E M R F 0 Q");
  CorYGraphIn->Fit("sinFit","B0Q");
  TF1* fitCorYIn = CorYGraphIn->GetFunction("sinFit");
  fitCorYIn->SetLineStyle(1);
  fitCorYIn->SetLineWidth(2);
  fitCorYIn->SetLineColor(kGreen+1);

  TGraphErrors * CorYGraphOut = new TGraphErrors(counterSenY,octant,avgModCorYOut,zeroAvg,eavgModCorYOut2);
  CorYGraphOut->SetName("CorYGraphOut");
  CorYGraphOut->SetMarkerColor(kBlack);
  CorYGraphOut->SetLineColor(kBlack);
  CorYGraphOut->SetMarkerStyle(21);
  CorYGraphOut->SetMarkerSize(markerSize[1]);
  CorYGraphOut->SetLineWidth(1);
  CorYGraphOut->Fit("sinFit","B0Q");
  TF1* fitCorYOut = CorYGraphOut->GetFunction("sinFit");
  fitCorYOut->SetLineStyle(1);
  fitCorYOut->SetLineWidth(2);
  fitCorYOut->SetLineColor(kBlack);

  TMultiGraph *CorYGraph = new TMultiGraph();
  CorYGraph->Add(CorYGraphIn);
  CorYGraph->Add(CorYGraphOut);
  CorYGraph->Draw("AP");
  CorYGraph->SetTitle(Form("Y : %s",showSinFit.Data()));
  CorYGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorYGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorYGraph->GetYaxis()->SetTitle("Y Correction [ppb]");
  CorYGraph->GetXaxis()->CenterTitle();
  CorYGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorYGraph= CorYGraph->GetXaxis();
//   xaxisCorYGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorYIn =(TPaveStats*)CorYGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorYOut =(TPaveStats*)CorYGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorYIn->SetTextColor(kGreen+1);
  statsCorYIn->SetFillColor(kWhite); 
  statsCorYOut->SetTextColor(kBlack);
  statsCorYOut->SetFillColor(kWhite);
  statsCorYIn->SetTextSize(0.045);
  statsCorYOut->SetTextSize(0.045);
  statsCorYIn->SetX1NDC(x_lo_stat_in4);    statsCorYIn->SetX2NDC(x_hi_stat_in4); 
  statsCorYIn->SetY1NDC(y_lo_stat_in4);    statsCorYIn->SetY2NDC(y_hi_stat_in4);
  statsCorYOut->SetX1NDC(x_lo_stat_out4);  statsCorYOut->SetX2NDC(x_hi_stat_out4); 
  statsCorYOut->SetY1NDC(y_lo_stat_out4);  statsCorYOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorYInp0        =   fitCorYIn->GetParameter(0);
  Double_t calc_CorYInp1        =   fitCorYIn->GetParameter(1);
  Double_t calc_CorYInp2        =   fitCorYIn->GetParameter(2);
  Double_t calc_eCorYInp0       =   fitCorYIn->GetParError(0);
  Double_t calc_eCorYInp1       =   fitCorYIn->GetParError(1);
  Double_t calc_eCorYInp2       =   fitCorYIn->GetParError(2);
  Double_t calc_CorYInChisquare =   fitCorYIn->GetChisquare();
  Double_t calc_CorYInNDF       =   fitCorYIn->GetNDF();

  Double_t calc_CorYOutp0        =   fitCorYOut->GetParameter(0);
  Double_t calc_CorYOutp1        =   fitCorYOut->GetParameter(1);
  Double_t calc_CorYOutp2        =   fitCorYOut->GetParameter(2);
  Double_t calc_eCorYOutp0       =   fitCorYOut->GetParError(0);
  Double_t calc_eCorYOutp1       =   fitCorYOut->GetParError(1);
  Double_t calc_eCorYOutp2       =   fitCorYOut->GetParError(2);
  Double_t calc_CorYOutChisquare =   fitCorYOut->GetChisquare();
  Double_t calc_CorYOutNDF       =   fitCorYOut->GetNDF();

  TLegend *legCorY = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorY->AddEntry("CorYGraphIn",  Form("In Y = %2.2f #pm %2.2f",calc_CorYInp0,calc_eCorYInp0),"lp");
  legCorY->AddEntry("CorYGraphOut",  Form("Out Y = %2.2f #pm %2.2f",calc_CorYOutp0,calc_eCorYOutp0),"lp");
  legCorY->SetTextSize(0.040);
  legCorY->SetFillColor(0);
  legCorY->SetBorderSize(2);
  legCorY->Draw();

  fitCorYIn->Draw("same");
  fitCorYOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad22->cd(4);

  TGraphErrors * CorYSlopeGraphIn = new TGraphErrors(counterSenYSlope,octant,avgModCorYSlopeIn,zeroAvg,eavgModCorYSlopeIn2);
  CorYSlopeGraphIn->SetName("CorYSlopeGraphIn");
  CorYSlopeGraphIn->SetMarkerColor(kGreen+1);
  CorYSlopeGraphIn->SetLineColor(kGreen+1);
  CorYSlopeGraphIn->SetMarkerStyle(22);
  CorYSlopeGraphIn->SetMarkerSize(markerSize[0]);
  CorYSlopeGraphIn->SetLineWidth(1);
  //CorYSlopeGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  CorYSlopeGraphIn->Fit("sinFit","B0Q");
  TF1* fitCorYSlopeIn = CorYSlopeGraphIn->GetFunction("sinFit");
  fitCorYSlopeIn->SetLineStyle(1);
  fitCorYSlopeIn->SetLineWidth(2);
  fitCorYSlopeIn->SetLineColor(kGreen+1);

  TGraphErrors * CorYSlopeGraphOut = new TGraphErrors(counterSenYSlope,octant,avgModCorYSlopeOut,zeroAvg,eavgModCorYSlopeOut2);
  CorYSlopeGraphOut->SetName("CorYSlopeGraphOut");
  CorYSlopeGraphOut->SetMarkerColor(kBlack);
  CorYSlopeGraphOut->SetLineColor(kBlack);
  CorYSlopeGraphOut->SetMarkerStyle(22);
  CorYSlopeGraphOut->SetMarkerSize(markerSize[0]);
  CorYSlopeGraphOut->SetLineWidth(1);
  CorYSlopeGraphOut->Fit("sinFit","B0Q");
  TF1* fitCorYSlopeOut = CorYSlopeGraphOut->GetFunction("sinFit");
  fitCorYSlopeOut->SetLineStyle(1);
  fitCorYSlopeOut->SetLineWidth(2);
  fitCorYSlopeOut->SetLineColor(kBlack);

  TMultiGraph *CorYSlopeGraph = new TMultiGraph();
  CorYSlopeGraph->Add(CorYSlopeGraphIn);
  CorYSlopeGraph->Add(CorYSlopeGraphOut);
  CorYSlopeGraph->Draw("AP");
  CorYSlopeGraph->SetTitle(Form("Y' : %s",showSinFit.Data()));
  CorYSlopeGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorYSlopeGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorYSlopeGraph->GetYaxis()->SetTitle("Y' Correction [ppb]");
  CorYSlopeGraph->GetXaxis()->CenterTitle();
  CorYSlopeGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorYSlopeGraph= CorYSlopeGraph->GetXaxis();
//   xaxisCorYSlopeGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorYSlopeIn =(TPaveStats*)CorYSlopeGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorYSlopeOut =(TPaveStats*)CorYSlopeGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorYSlopeIn->SetTextColor(kGreen+1);
  statsCorYSlopeIn->SetFillColor(kWhite); 
  statsCorYSlopeOut->SetTextColor(kBlack);
  statsCorYSlopeOut->SetFillColor(kWhite);
  statsCorYSlopeIn->SetTextSize(0.045);
  statsCorYSlopeOut->SetTextSize(0.045);
  statsCorYSlopeIn->SetX1NDC(x_lo_stat_in4);    statsCorYSlopeIn->SetX2NDC(x_hi_stat_in4); 
  statsCorYSlopeIn->SetY1NDC(y_lo_stat_in4);    statsCorYSlopeIn->SetY2NDC(y_hi_stat_in4);
  statsCorYSlopeOut->SetX1NDC(x_lo_stat_out4);  statsCorYSlopeOut->SetX2NDC(x_hi_stat_out4); 
  statsCorYSlopeOut->SetY1NDC(y_lo_stat_out4);  statsCorYSlopeOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorYSlopeInp0        =   fitCorYSlopeIn->GetParameter(0);
  Double_t calc_CorYSlopeInp1        =   fitCorYSlopeIn->GetParameter(1);
  Double_t calc_CorYSlopeInp2        =   fitCorYSlopeIn->GetParameter(2);
  Double_t calc_eCorYSlopeInp0       =   fitCorYSlopeIn->GetParError(0);
  Double_t calc_eCorYSlopeInp1       =   fitCorYSlopeIn->GetParError(1);
  Double_t calc_eCorYSlopeInp2       =   fitCorYSlopeIn->GetParError(2);
  Double_t calc_CorYSlopeInChisquare =   fitCorYSlopeIn->GetChisquare();
  Double_t calc_CorYSlopeInNDF       =   fitCorYSlopeIn->GetNDF();

  Double_t calc_CorYSlopeOutp0        =   fitCorYSlopeOut->GetParameter(0);
  Double_t calc_CorYSlopeOutp1        =   fitCorYSlopeOut->GetParameter(1);
  Double_t calc_CorYSlopeOutp2        =   fitCorYSlopeOut->GetParameter(2);
  Double_t calc_eCorYSlopeOutp0       =   fitCorYSlopeOut->GetParError(0);
  Double_t calc_eCorYSlopeOutp1       =   fitCorYSlopeOut->GetParError(1);
  Double_t calc_eCorYSlopeOutp2       =   fitCorYSlopeOut->GetParError(2);
  Double_t calc_CorYSlopeOutChisquare =   fitCorYSlopeOut->GetChisquare();
  Double_t calc_CorYSlopeOutNDF       =   fitCorYSlopeOut->GetNDF();

  TLegend *legCorYSlope = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorYSlope->AddEntry("CorYSlopeGraphIn",  Form("In Y' = %2.2f #pm %2.2f",calc_CorYSlopeInp0,calc_eCorYSlopeInp0),"lp");
  legCorYSlope->AddEntry("CorYSlopeGraphOut",  Form("Out Y' = %2.2f #pm %2.2f",calc_CorYSlopeOutp0,calc_eCorYSlopeOutp0),"lp");
  legCorYSlope->SetTextSize(0.040);
  legCorYSlope->SetFillColor(0);
  legCorYSlope->SetBorderSize(2);
  legCorYSlope->Draw();

  fitCorYSlopeIn->Draw("same");
  fitCorYSlopeOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad22->cd(5);

  TGraphErrors * CorEGraphIn = new TGraphErrors(counterSenE,octant,avgModCorEIn,zeroAvg,eavgModCorEIn2);
  CorEGraphIn->SetName("CorEGraphIn");
  CorEGraphIn->SetMarkerColor(kBlue);
  CorEGraphIn->SetLineColor(kBlue);
  CorEGraphIn->SetMarkerStyle(20);
  CorEGraphIn->SetMarkerSize(markerSize[0]);
  CorEGraphIn->SetLineWidth(1);
  //CorEGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  CorEGraphIn->Fit("cosFit","B0Q");
  TF1* fitCorEIn = CorEGraphIn->GetFunction("cosFit");
  fitCorEIn->SetLineStyle(1);
  fitCorEIn->SetLineWidth(2);
  fitCorEIn->SetLineColor(kBlue);

  TGraphErrors * CorEGraphOut = new TGraphErrors(counterSenE,octant,avgModCorEOut,zeroAvg,eavgModCorEOut2);
  CorEGraphOut->SetName("CorEGraphOut");
  CorEGraphOut->SetMarkerColor(kBlack);
  CorEGraphOut->SetLineColor(kBlack);
  CorEGraphOut->SetMarkerStyle(20);
  CorEGraphOut->SetMarkerSize(markerSize[0]);
  CorEGraphOut->SetLineWidth(1);
  CorEGraphOut->Fit("cosFit","B0Q");
  TF1* fitCorEOut = CorEGraphOut->GetFunction("cosFit");
  fitCorEOut->SetLineStyle(1);
  fitCorEOut->SetLineWidth(2);
  fitCorEOut->SetLineColor(kBlack);

  TMultiGraph *CorEGraph = new TMultiGraph();
  CorEGraph->Add(CorEGraphIn);
  CorEGraph->Add(CorEGraphOut);
  CorEGraph->Draw("AP");
  CorEGraph->SetTitle(Form("E : %s",showCosFit.Data()));
  CorEGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorEGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorEGraph->GetYaxis()->SetTitle("E Correction [ppb]");
  CorEGraph->GetXaxis()->CenterTitle();
  CorEGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorEGraph= CorEGraph->GetXaxis();
//   xaxisCorEGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorEIn =(TPaveStats*)CorEGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorEOut =(TPaveStats*)CorEGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorEIn->SetTextColor(kBlue);
  statsCorEIn->SetFillColor(kWhite); 
  statsCorEOut->SetTextColor(kBlack);
  statsCorEOut->SetFillColor(kWhite);
  statsCorEIn->SetTextSize(0.045);
  statsCorEOut->SetTextSize(0.045);
  statsCorEIn->SetX1NDC(x_lo_stat_in4);    statsCorEIn->SetX2NDC(x_hi_stat_in4); 
  statsCorEIn->SetY1NDC(y_lo_stat_in4);    statsCorEIn->SetY2NDC(y_hi_stat_in4);
  statsCorEOut->SetX1NDC(x_lo_stat_out4);  statsCorEOut->SetX2NDC(x_hi_stat_out4); 
  statsCorEOut->SetY1NDC(y_lo_stat_out4);  statsCorEOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorEInp0        =   fitCorEIn->GetParameter(0);
  Double_t calc_CorEInp1        =   fitCorEIn->GetParameter(1);
  Double_t calc_CorEInp2        =   fitCorEIn->GetParameter(2);
  Double_t calc_eCorEInp0       =   fitCorEIn->GetParError(0);
  Double_t calc_eCorEInp1       =   fitCorEIn->GetParError(1);
  Double_t calc_eCorEInp2       =   fitCorEIn->GetParError(2);
  Double_t calc_CorEInChisquare =   fitCorEIn->GetChisquare();
  Double_t calc_CorEInNDF       =   fitCorEIn->GetNDF();

  Double_t calc_CorEOutp0        =   fitCorEOut->GetParameter(0);
  Double_t calc_CorEOutp1        =   fitCorEOut->GetParameter(1);
  Double_t calc_CorEOutp2        =   fitCorEOut->GetParameter(2);
  Double_t calc_eCorEOutp0       =   fitCorEOut->GetParError(0);
  Double_t calc_eCorEOutp1       =   fitCorEOut->GetParError(1);
  Double_t calc_eCorEOutp2       =   fitCorEOut->GetParError(2);
  Double_t calc_CorEOutChisquare =   fitCorEOut->GetChisquare();
  Double_t calc_CorEOutNDF       =   fitCorEOut->GetNDF();

  TLegend *legCorE = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorE->AddEntry("CorEGraphIn",  Form("In E = %2.2f #pm %2.2f",calc_CorEInp0,calc_eCorEInp0),"lp");
  legCorE->AddEntry("CorEGraphOut",  Form("Out E = %2.2f #pm %2.2f",calc_CorEOutp0,calc_eCorEOutp0),"lp");
  legCorE->SetTextSize(0.040);
  legCorE->SetFillColor(0);
  legCorE->SetBorderSize(2);
  legCorE->Draw();

  fitCorEIn->Draw("same");
  fitCorEOut->Draw("same");

  gPad->Update();

  /********************************************************/

  pad22->cd(6);

  TGraphErrors * CorChargeGraphIn = new TGraphErrors(counterSenCharge,octant,avgModCorChargeIn,zeroAvg,eavgModCorChargeIn2);
  CorChargeGraphIn->SetName("CorChargeGraphIn");
  CorChargeGraphIn->SetMarkerColor(kMagenta);
  CorChargeGraphIn->SetLineColor(kMagenta);
  CorChargeGraphIn->SetMarkerStyle(23);
  CorChargeGraphIn->SetMarkerSize(markerSize[0]);
  CorChargeGraphIn->SetLineWidth(1);
  //CorChargeGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  CorChargeGraphIn->Fit("cosFit","B0Q");
  TF1* fitCorChargeIn = CorChargeGraphIn->GetFunction("cosFit");
  fitCorChargeIn->SetLineStyle(1);
  fitCorChargeIn->SetLineWidth(2);
  fitCorChargeIn->SetLineColor(kMagenta);

  TGraphErrors * CorChargeGraphOut = new TGraphErrors(counterSenCharge,octant,avgModCorChargeOut,zeroAvg,eavgModCorChargeOut2);
  CorChargeGraphOut->SetName("CorChargeGraphOut");
  CorChargeGraphOut->SetMarkerColor(kBlack);
  CorChargeGraphOut->SetLineColor(kBlack);
  CorChargeGraphOut->SetMarkerStyle(23);
  CorChargeGraphOut->SetMarkerSize(markerSize[0]);
  CorChargeGraphOut->SetLineWidth(1);
  CorChargeGraphOut->Fit("cosFit","B0Q");
  TF1* fitCorChargeOut = CorChargeGraphOut->GetFunction("cosFit");
  fitCorChargeOut->SetLineStyle(1);
  fitCorChargeOut->SetLineWidth(2);
  fitCorChargeOut->SetLineColor(kBlack);

  TMultiGraph *CorChargeGraph = new TMultiGraph();
  CorChargeGraph->Add(CorChargeGraphIn);
  CorChargeGraph->Add(CorChargeGraphOut);
  CorChargeGraph->Draw("AP");
  CorChargeGraph->SetTitle(Form("A_{Q} : %s",showCosFit.Data()));
  CorChargeGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorChargeGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorChargeGraph->GetYaxis()->SetTitle("Charge Correction [ppb]");
  CorChargeGraph->GetXaxis()->CenterTitle();
  CorChargeGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorChargeGraph= CorChargeGraph->GetXaxis();
//   xaxisCorChargeGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorChargeIn =(TPaveStats*)CorChargeGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorChargeOut =(TPaveStats*)CorChargeGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorChargeIn->SetTextColor(kMagenta);
  statsCorChargeIn->SetFillColor(kWhite); 
  statsCorChargeOut->SetTextColor(kBlack);
  statsCorChargeOut->SetFillColor(kWhite);
  statsCorChargeIn->SetTextSize(0.045);
  statsCorChargeOut->SetTextSize(0.045);
  statsCorChargeIn->SetX1NDC(x_lo_stat_in4);    statsCorChargeIn->SetX2NDC(x_hi_stat_in4); 
  statsCorChargeIn->SetY1NDC(y_lo_stat_in4);    statsCorChargeIn->SetY2NDC(y_hi_stat_in4);
  statsCorChargeOut->SetX1NDC(x_lo_stat_out4);  statsCorChargeOut->SetX2NDC(x_hi_stat_out4); 
  statsCorChargeOut->SetY1NDC(y_lo_stat_out4);  statsCorChargeOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorChargeInp0        =   fitCorChargeIn->GetParameter(0);
  Double_t calc_CorChargeInp1        =   fitCorChargeIn->GetParameter(1);
  Double_t calc_CorChargeInp2        =   fitCorChargeIn->GetParameter(2);
  Double_t calc_eCorChargeInp0       =   fitCorChargeIn->GetParError(0);
  Double_t calc_eCorChargeInp1       =   fitCorChargeIn->GetParError(1);
  Double_t calc_eCorChargeInp2       =   fitCorChargeIn->GetParError(2);
  Double_t calc_CorChargeInChisquare =   fitCorChargeIn->GetChisquare();
  Double_t calc_CorChargeInNDF       =   fitCorChargeIn->GetNDF();

  Double_t calc_CorChargeOutp0        =   fitCorChargeOut->GetParameter(0);
  Double_t calc_CorChargeOutp1        =   fitCorChargeOut->GetParameter(1);
  Double_t calc_CorChargeOutp2        =   fitCorChargeOut->GetParameter(2);
  Double_t calc_eCorChargeOutp0       =   fitCorChargeOut->GetParError(0);
  Double_t calc_eCorChargeOutp1       =   fitCorChargeOut->GetParError(1);
  Double_t calc_eCorChargeOutp2       =   fitCorChargeOut->GetParError(2);
  Double_t calc_CorChargeOutChisquare =   fitCorChargeOut->GetChisquare();
  Double_t calc_CorChargeOutNDF       =   fitCorChargeOut->GetNDF();

  TLegend *legCorCharge = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorCharge->AddEntry("CorChargeGraphIn",  Form("In A_{Q} = %2.2f #pm %2.2f",calc_CorChargeInp0,calc_eCorChargeInp0),"lp");
  legCorCharge->AddEntry("CorChargeGraphOut",  Form("Out A_{Q} = %2.2f #pm %2.2f",calc_CorChargeOutp0,calc_eCorChargeOutp0),"lp");
  legCorCharge->SetTextSize(0.040);
  legCorCharge->SetFillColor(0);
  legCorCharge->SetBorderSize(2);
  legCorCharge->Draw();

  fitCorChargeIn->Draw("same");
  fitCorChargeOut->Draw("same");

  gPad->Update();

  /********************************************************/

  TString saveCorPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_slug_correction_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),database_stem.Data());


  c2-> Update();
  c2->Print(saveCorPlot+".png");
  if(FIGURE){
    c2->Print(saveCorPlot+".svg");
    c2->Print(saveCorPlot+".C");
  }




  /********************************************************/
  /********************************************************/


  TCanvas * c3 = new TCanvas("canvas3", titleCor,0,0,canvasSize[2],canvasSize[3]);
  c3->Draw();
  c3->SetBorderSize(0);
  c3->cd();
  TPad*pad31 = new TPad("pad31","pad31",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad32 = new TPad("pad32","pad32",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad31->SetFillColor(kWhite);
  pad31->Draw();
  pad32->Draw();
  pad31->cd();
  TLatex * t31 = new TLatex(0.06,0.3,Form("%s",titleCor.Data()));
  t31->SetTextSize(0.5);
  t31->Draw();
  pad32->cd();
  pad32->Divide(1,1);

  /********************************************************/

  pad32->cd(1);

  TGraphErrors * CorTotalGraphIn = new TGraphErrors(counterSenX,octant,avgModCorTotalIn,zeroAvg,zeroAvg);
  CorTotalGraphIn->SetName("CorTotalGraphIn");
  CorTotalGraphIn->SetMarkerColor(kCyan+1);
  CorTotalGraphIn->SetLineColor(kCyan+1);
  CorTotalGraphIn->SetMarkerStyle(21);
  CorTotalGraphIn->SetMarkerSize(markerSize[0]);
  CorTotalGraphIn->SetLineWidth(1);
  //CorTotalGraphIn->Fit("fitOctantVariationXIn","E M R F 0 Q");
  CorTotalGraphIn->Fit("cosFit","B0Q");
  TF1* fitCorTotalIn = CorTotalGraphIn->GetFunction("cosFit");
  fitCorTotalIn->SetLineStyle(1);
  fitCorTotalIn->SetLineWidth(2);
  fitCorTotalIn->SetLineColor(kCyan+1);

  TGraphErrors * CorTotalGraphOut = new TGraphErrors(counterSenX,octant,avgModCorTotalOut,zeroAvg,zeroAvg);
  CorTotalGraphOut->SetName("CorTotalGraphOut");
  CorTotalGraphOut->SetMarkerColor(kBlack);
  CorTotalGraphOut->SetLineColor(kBlack);
  CorTotalGraphOut->SetMarkerStyle(21);
  CorTotalGraphOut->SetMarkerSize(markerSize[0]);
  CorTotalGraphOut->SetLineWidth(1);
  CorTotalGraphOut->Fit("cosFit","B0Q");
  TF1* fitCorTotalOut = CorTotalGraphOut->GetFunction("cosFit");
  fitCorTotalOut->SetLineStyle(1);
  fitCorTotalOut->SetLineWidth(2);
  fitCorTotalOut->SetLineColor(kBlack);

  TMultiGraph *CorTotalGraph = new TMultiGraph();
  CorTotalGraph->Add(CorTotalGraphIn);
  CorTotalGraph->Add(CorTotalGraphOut);
  CorTotalGraph->Draw("AP");
  CorTotalGraph->SetTitle(Form("Total : %s",showCosFit.Data()));
  CorTotalGraph->GetXaxis()->SetNdivisions(8,0,0);
  CorTotalGraph->GetXaxis()->SetTitle(Form("Octant"));
  CorTotalGraph->GetYaxis()->SetTitle("Total Correction [ppb]");
  CorTotalGraph->GetXaxis()->CenterTitle();
  CorTotalGraph->GetYaxis()->CenterTitle();
  TAxis *xaxisCorTotalGraph= CorTotalGraph->GetXaxis();
//   xaxisCorTotalGraph->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorTotalIn =(TPaveStats*)CorTotalGraphIn->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorTotalOut =(TPaveStats*)CorTotalGraphOut->GetListOfFunctions()->FindObject("stats");
  statsCorTotalIn->SetTextColor(kCyan+1);
  statsCorTotalIn->SetFillColor(kWhite); 
  statsCorTotalOut->SetTextColor(kBlack);
  statsCorTotalOut->SetFillColor(kWhite);
  statsCorTotalIn->SetTextSize(0.045);
  statsCorTotalOut->SetTextSize(0.045);
  statsCorTotalIn->SetX1NDC(x_lo_stat_in4);    statsCorTotalIn->SetX2NDC(x_hi_stat_in4); 
  statsCorTotalIn->SetY1NDC(y_lo_stat_in4);    statsCorTotalIn->SetY2NDC(y_hi_stat_in4);
  statsCorTotalOut->SetX1NDC(x_lo_stat_out4);  statsCorTotalOut->SetX2NDC(x_hi_stat_out4); 
  statsCorTotalOut->SetY1NDC(y_lo_stat_out4);  statsCorTotalOut->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorTotalInp0        =   fitCorTotalIn->GetParameter(0);
  Double_t calc_CorTotalInp1        =   fitCorTotalIn->GetParameter(1);
  Double_t calc_CorTotalInp2        =   fitCorTotalIn->GetParameter(2);
  Double_t calc_eCorTotalInp0       =   fitCorTotalIn->GetParError(0);
  Double_t calc_eCorTotalInp1       =   fitCorTotalIn->GetParError(1);
  Double_t calc_eCorTotalInp2       =   fitCorTotalIn->GetParError(2);
  Double_t calc_CorTotalInChisquare =   fitCorTotalIn->GetChisquare();
  Double_t calc_CorTotalInNDF       =   fitCorTotalIn->GetNDF();

  Double_t calc_CorTotalOutp0        =   fitCorTotalOut->GetParameter(0);
  Double_t calc_CorTotalOutp1        =   fitCorTotalOut->GetParameter(1);
  Double_t calc_CorTotalOutp2        =   fitCorTotalOut->GetParameter(2);
  Double_t calc_eCorTotalOutp0       =   fitCorTotalOut->GetParError(0);
  Double_t calc_eCorTotalOutp1       =   fitCorTotalOut->GetParError(1);
  Double_t calc_eCorTotalOutp2       =   fitCorTotalOut->GetParError(2);
  Double_t calc_CorTotalOutChisquare =   fitCorTotalOut->GetChisquare();
  Double_t calc_CorTotalOutNDF       =   fitCorTotalOut->GetNDF();

  TLegend *legCorTotal = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legCorTotal->AddEntry("CorTotalGraphIn",  Form("In Total = %1.1f #pm %1.1f",calc_CorTotalInp0,calc_eCorTotalInp0),"lp");
  legCorTotal->AddEntry("CorTotalGraphOut",  Form("Out Total = %1.1f #pm %1.1f",calc_CorTotalOutp0,calc_eCorTotalOutp0),"lp");
  legCorTotal->SetTextSize(0.040);
  legCorTotal->SetFillColor(0);
  legCorTotal->SetBorderSize(2);
  legCorTotal->Draw();

  fitCorTotalIn->Draw("same");
  fitCorTotalOut->Draw("same");

  gPad->Update();

  /********************************************************/

  TString saveCorTotalPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_slug_total_correction_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),database_stem.Data());


  c3-> Update();
  c3->Print(saveCorTotalPlot+".png");
  if(FIGURE){
    c3->Print(saveCorTotalPlot+".svg");
    c3->Print(saveCorTotalPlot+".C");
  }


  /********************************************************/
  /********************************************************/
  /********************************************************/
  /********************************************************/


  Char_t  textfileAsym[400],textfileAsymWrite[400];
  sprintf(textfileAsym,"dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),database_stem.Data()); 

  sprintf(textfileAsymWrite,"dirPlot/%s_%s_%s_%s_MD_%s_manual_regression_%s_%s_asym_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());

//n-to-delta_6700_h_transverse_HYDROGEN-CELL_MD_PMTavg_regression_on_5+1_off_run2_pass5.txt

  int counter=0;
  ifstream transverseMDAsym;
  Double_t zero[NUMC],valuesin[NUMC],errorsin[NUMC],valuesout[NUMC],errorsout[NUMC];


  transverseMDAsym.open(Form("%s",textfileAsym));
  printf(Form("%s\n",textfileAsym));
 
  if (transverseMDAsym.is_open()) {
   
    printf("%s Det\tIn+-InError\tOut+-OutError\n%s",green,normal);
   
    while(!transverseMDAsym.eof()) {
      zero[counter]=0;
      transverseMDAsym >> octant[counter];
      transverseMDAsym >> valuesin[counter];
      transverseMDAsym >> errorsin[counter];
      transverseMDAsym >> valuesout[counter];
      transverseMDAsym >> errorsout[counter];
   
      if (!transverseMDAsym.good()) break;

      cout<<green
	  <<octant[counter]<<"\t"<<valuesin[counter]<<"+-"<<errorsin[counter]<<"\t"
	  <<valuesout[counter]<<"+-"<<errorsout[counter]
	  <<normal<<endl;

//       printf("%s %4.0f\t%4.2f+-%4.2f\t%4.2f+-%4.2f\n%s"
// 	     ,green,octant[counter],valuesin[counter],errorsin[counter],valuesout[counter],errorsout[counter],normal);
 
      counter++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  transverseMDAsym.close();

  /****************************************/
  /****************************************/


  TString title1;
  TString titleSummary = Form("%s (%s, %s A): Slug Avg. Sensitivity Correceted MD %s Asymmetries."
			      ,targ.Data(),polar.Data(),qtor_stem.Data(),deviceTitle.Data());

  if(datopt==1){ title1= Form("%s %s",titleSummary.Data(),showFit1.Data());}
  else if(datopt==2){ title1= Form("%s %s",titleSummary.Data(),showFit2.Data());}
  else{
    title1= Form("%s %s",titleSummary.Data(),showFit3.Data());
  }

  TCanvas * c10 = new TCanvas("c10", title1,0,0,1200,650);
  c10->Draw();
  c10->SetBorderSize(0);
  c10->cd();
  TPad*pad101 = new TPad("pad101","pad101",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad102 = new TPad("pad102","pad102",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad101->SetFillColor(kWhite);
  pad101->Draw();
  pad102->Draw();
  pad101->cd();
  TLatex * t1 = new TLatex(0.06,0.3,Form("%s",title1.Data()));
  t1->SetTextSize(0.5);
  t1->Draw();
  pad102->cd();


  const int k =8;
  Double_t valuesin2[k];Double_t errorsin2[k];Double_t valuesout2[k];Double_t errorsout2[k];
  Double_t valuesum[k];
//   Double_t errorsum[k];
  Double_t valueerror[k];
  Double_t valuediff[k];
  Double_t errordiff[k];
  Double_t x[k];
  Double_t errx[k];

  Double_t valuesumopp[4];
  Double_t errorsumopp[4];  
  Double_t valuediffopp[4];
  Double_t errordiffopp[4];
  
  for(Int_t i =0;i<k;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }


//   TPad* pad = (TPad*)(gPad->GetMother());
 
  // cos fit in
  TF1 *fit_in;
  if(datopt==1) {
    TString fit_func_in = "[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]";
    fit_in = new TF1("fit_in",Form("%s",fit_func_in.Data()),1,8);
  }
  else if(datopt==2){
    TString fit_func_in = "[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]";
    fit_in = new TF1("fit_in",Form("%s",fit_func_in.Data()),1,8);
  }
  else{
    TString fit_func_in = "[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]";
    fit_in = new TF1("fit_in",Form("%s",fit_func_in.Data()),1,8);
  }

  fit_in->SetParameter(0,0);


  for(Int_t i =0;i<k;i++){
    valuesin2[i] = valuesin[i] - avgModCorTotalIn[i]*1e-3;  //manual correction for regression.
    valuesout2[i] = valuesout[i] - avgModCorTotalOut[i]*1e-3; //manual correction for regression.
//     valuesin2[i] = valuesin[i] - avgModCorTotalIn[i]*0;
//     valuesout2[i] = valuesout[i] - avgModCorTotalOut[i]*0;

    valuesum[i]=(valuesin2[i]+valuesout2[i])/2.0;
    valueerror[i]= (sqrt(pow(errorsin[i],2)+pow(errorsout[i],2)))/2.0;
    valuediff[i]=((valuesin2[i]/pow(errorsin[i],2)) - (valuesout2[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));


  }

  /********************************************/

  for(Int_t i =0;i<4;i++){
    valuesumopp[i]= valuesum[i]+valuesum[i+4];
    errorsumopp[i]= sqrt(pow(valueerror[i],2)+pow(valueerror[i+4],2));
    valuediffopp[i]=valuediff[i]-valuediff[i+4];
    errordiffopp[i]=sqrt(pow(errordiff[i],2)+pow(errordiff[i+4],2));

  }


  // Draw IN values
  TGraphErrors* grp_in  = new TGraphErrors(k,octant,valuesin2,zero,errorsin);
  grp_in ->SetName("grp_in");
  grp_in ->Draw("goff");
  grp_in ->SetMarkerSize(markerSize[0]);
  grp_in ->SetMarkerStyle(20);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->SetLineColor(kBlue);
  grp_in->Fit("fit_in","B0Q");
  TF1* fit1 = grp_in->GetFunction("fit_in");
  fit1->SetLineColor(kBlue);
  fit1->SetLineStyle(4);
  fit1->SetLineWidth(2);


  // Draw OUT values
  TGraphErrors* grp_out  = new TGraphErrors(k,octant,valuesout2,zero,errorsout);
  grp_out ->SetName("grp_out");
  grp_out ->Draw("goff");
  grp_out ->SetMarkerSize(markerSize[0]);
  grp_out ->SetMarkerStyle(24);
  grp_out ->SetMarkerColor(kRed);
  grp_out ->SetLineColor(kRed);


  TF1 *fit_out;
  if(datopt==1){
    TString fit_func_out = "[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]";
    fit_out = new TF1("fit_out",Form("%s",fit_func_out.Data()),1,8);
  }
  else if(datopt==2){
    TString fit_func_out = "[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]";
    fit_out = new TF1("fit_out",Form("%s",fit_func_out.Data()),1,8);
  }
  else{
    TString fit_func_out = "[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]";
    fit_out = new TF1("fit_out",Form("%s",fit_func_out.Data()),1,8);
  }
  
  /*Initialize this fit with the results from the previous fit*/
  //  cosfit_out->SetParameter(0,-1*(fit1->GetParameter(0)));
  //  cosfit_out->SetParLimits(0,-99999,0);
  //cosfit_out->SetParLimits(1,-1,179);
  //cosfit_out->SetParameter(1, fit1->GetParameter(1));
  //cosfit_out->SetParameter(2, -(fit1->GetParameter(2)));

  grp_out->Fit("fit_out","B0Q");
  TF1* fit2 = grp_out->GetFunction("fit_out");
  fit2->SetLineColor(kRed);
  fit2->SetLineStyle(5);
  fit2->SetLineWidth(2);

  // Draw In+Out values
  TGraphErrors* grp_sum  = new TGraphErrors(k,x,valuesum,errx,valueerror);
  grp_sum ->SetName("grp_sum");
  grp_sum ->Draw("goff");
  grp_sum ->SetMarkerSize(markerSize[0]);
  //  grp_sum ->SetLineWidth(0);
  grp_sum ->SetMarkerStyle(22);
  grp_sum ->SetMarkerColor(kGreen-3);
  grp_sum ->SetLineColor(kGreen-3);
  grp_sum->Fit("pol0","B0Q");
  TF1* fit3 = grp_sum->GetFunction("pol0");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen-2);
  fit3->SetLineStyle(2);
  fit3->SetLineWidth(3);

  // Draw In-Out
  TGraphErrors* grp_diff  = new TGraphErrors(k,x,valuediff,errx,errordiff);
  grp_diff ->SetName("grp_diff");
  grp_diff ->Draw("goff");
  grp_diff ->SetMarkerSize(markerSize[0]);
  grp_diff ->SetLineWidth(0);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta);
  grp_diff ->SetLineColor(kMagenta);
  grp_diff->Fit("fit_in","B0Q");
  TF1* fit4 = grp_diff->GetFunction("fit_in");
  fit4->DrawCopy("same");
  fit4->SetLineColor(kMagenta+1);
  fit4->SetLineStyle(1);
  fit4->SetLineWidth(4);

  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in,"P");
  grp->Add(grp_out,"P");
  grp->Add(grp_sum,"P");
  grp->Add(grp_diff,"P");
  grp->Draw("A");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("MD %s Asymmetry [ppm]",deviceTitle.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);
  grp->GetXaxis()->SetNdivisions(8,0,0);
  grp->GetYaxis()->SetNdivisions(8,5,0);
  if(SCALE){
    grp->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  }
  TAxis *xaxisGrp= grp->GetXaxis();
  xaxisGrp->SetLimits(0.5,8.5);

  fit1->DrawCopy("same");
  fit2->DrawCopy("same");
  fit3->DrawCopy("same");
  fit4->DrawCopy("same");


  fit1->SetParNames("A_{M}","#phi_{0}","C");
  fit2->SetParNames("A_{M}","#phi_{0}","C");
  fit3->SetParNames("C");
  fit4->SetParNames("A_{M}","#phi_{0}","C");

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 
  stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(0.72);stats1->SetY2NDC(0.95);

  Double_t p0in     =  fit1->GetParameter(0);
  Double_t ep0in    =  fit1->GetParError(0);
  //Double_t p1in     =  fit1->GetParameter(1);
  //Double_t ep1in    =  fit1->GetParError(1);
  //Double_t p2in     =  fit1->GetParameter(2);
  //Double_t ep2in    =  fit1->GetParError(2);
  //Double_t Chiin    =  fit1->GetChisquare();
  //Double_t NDFin    =  fit1->GetNDF();
  //Double_t Probin   =  fit1->GetProb();


  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
  stats2->SetX1NDC(x_lo_stat_in4); stats2->SetX2NDC(x_hi_stat_in4); stats2->SetY1NDC(0.47);stats2->SetY2NDC(0.70);

  Double_t p0out    =  fit2->GetParameter(0);
  Double_t ep0out   =  fit2->GetParError(0);
  //Double_t p1out    =  fit2->GetParameter(1);
  //Double_t ep1out   =  fit2->GetParError(1);
  //Double_t p2out    =  fit2->GetParameter(2);
  //Double_t ep2out   =  fit2->GetParError(2);
  //Double_t Chiout   =  fit2->GetChisquare();
  //Double_t NDFout   =  fit2->GetNDF();
  //Double_t Probout  =  fit2->GetProb();


  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");
  stats3->SetTextColor(kGreen-2);
  stats3->SetFillColor(kWhite); 
  stats3->SetX1NDC(x_lo_stat_in4); stats3->SetX2NDC(x_hi_stat_in4); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);

  Double_t p0sum    =  fit3->GetParameter(0);
  Double_t ep0sum   =  fit3->GetParError(0);
  //Double_t Chisum   =  fit3->GetChisquare();
  //Double_t NDFsum   =  fit3->GetNDF();
  //Double_t Probsum  =  fit3->GetProb();


  TPaveStats *stats4 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");
  stats4->SetTextColor(kMagenta+1);
  stats4->SetFillColor(kWhite); 
  stats4->SetX1NDC(x_lo_stat_in4); stats4->SetX2NDC(x_hi_stat_in4); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);

  Double_t p0diff   =  fit4->GetParameter(0);
  Double_t ep0diff  =  fit4->GetParError(0);
  Double_t p1diff   =  fit4->GetParameter(1);
  Double_t ep1diff  =  fit4->GetParError(1);
  Double_t p2diff   =  fit4->GetParameter(2);
  Double_t ep2diff  =  fit4->GetParError(2);
  Double_t Chidiff  =  fit4->GetChisquare();
  Double_t NDFdiff  =  fit4->GetNDF();
  Double_t Probdiff =  fit4->GetProb();


  TLegend *legend = new TLegend(legendCoordinates[0],legendCoordinates[1],legendCoordinates[2],legendCoordinates[3],"","brNDC");
  legend->SetNColumns(2);
  legend->AddEntry(grp_in,  Form("A_{IHWP-IN} = %4.2f #pm %4.2f ppm",p0in,ep0in), "lp");
  legend->AddEntry(grp_sum, Form("A_{(IN+OUT)/2} = %4.2f #pm %4.2f ppm",p0sum,ep0sum), "lp");
  legend->AddEntry(grp_out, Form("A_{IHWP-OUT} = %4.2f #pm %4.2f ppm",p0out,ep0out), "lp");
  legend->AddEntry(grp_diff,Form("A_{measured} = %4.2f #pm %4.2f ppm",p0diff,ep0diff), "lp");
  legend->SetFillColor(0);
  legend->SetBorderSize(2);
  legend->SetTextSize(0.035);
  legend->Draw("");


  if(PRELIMINARY){
    // Preliminary water mark sign 
    TLatex * prelim = new TLatex(waterMark[0],waterMark[1],"Preliminary");
    prelim->SetTextColor(18);
    prelim->SetTextSize(0.1991525);
    prelim->SetTextAngle(15.0);
    prelim->SetLineWidth(2);
    prelim->Draw();
    grp->Draw("P");
  }

  gPad->Update();

  MyfileWrite.open(textfileAsymWrite);
  MyfileWrite<<Form("%4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.0f %4.4f",p0diff,ep0diff,p1diff,ep1diff,p2diff,ep2diff,Chidiff,NDFdiff,Probdiff)<<endl;
  MyfileWrite.close();


  TString saveSummaryPlot = Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_%s_slug_summary_manual_reg_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());

  c10->Update();
  c10->Print(saveSummaryPlot+".png");
  if(FIGURE){
    c10->Print(saveSummaryPlot+".svg");
    c10->Print(saveSummaryPlot+".C");
  }


  /********************************************************/
  /********************************************************/


  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);


}
