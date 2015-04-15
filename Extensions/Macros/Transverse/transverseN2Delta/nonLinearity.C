//*****************************************************************************************************//
// Author      : Nuruzzaman on 06/17/2013, Transverse N2Delta.
// Last Update : 07/19/2013
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
  TString showFit1,showFit2,showFit3,showSinFit,showCosFit;

  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[4] ={1600*figSize,1000*figSize,1200*figSize,650*figSize};
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

//   /****************************************/
//   /****************************************/

//   diffAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_beam_parameter_differences_regression_%s_%s_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
// 		    ,reg_calc.Data(),reg_set.Data(),database_stem.Data()));

//   cout<<Form("dirPlot/resultText/%s_%s_%s_%s_beam_parameter_differences_regression_%s_%s_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
// 	     ,reg_calc.Data(),reg_set.Data(),database_stem.Data())<<endl;

//   if (diffAvg.is_open()) {

//     while(!diffAvg.eof()) {
      
//       diffAvg >> avgDiffXIn[counterDiff]>> eavgDiffXIn[counterDiff]
// 	      >> avgDiffYIn[counterDiff] >> eavgDiffYIn[counterDiff]
// 	      >> avgDiffXSlopeIn[counterDiff] >> eavgDiffXSlopeIn[counterDiff]
// 	      >> avgDiffYSlopeIn[counterDiff] >> eavgDiffYSlopeIn[counterDiff]
// 	      >> avgDiffEIn[counterDiff] >> eavgDiffEIn[counterDiff]
// 	      >> avgChargeIn[counterDiff] >> eavgChargeIn[counterDiff]
// 	      >> avgDiffXOut[counterDiff]>> eavgDiffXOut[counterDiff]
// 	      >> avgDiffYOut[counterDiff] >> eavgDiffYOut[counterDiff]
// 	      >> avgDiffXSlopeOut[counterDiff] >> eavgDiffXSlopeOut[counterDiff]
// 	      >> avgDiffYSlopeOut[counterDiff] >> eavgDiffYSlopeOut[counterDiff]
// 	      >> avgDiffEOut[counterDiff] >> eavgDiffEOut[counterDiff]
// 	      >> avgChargeOut[counterDiff] >> eavgChargeOut[counterDiff];
    
//       if (!diffAvg.good()) break;

//       counterDiff++;
//     }
//   }
//   else {
//     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
//     return -1;
//   }
//   diffAvg.close();

//   cout <<blue
//        <<"XIn = "<<avgDiffXIn[0]<<"+-"<<eavgDiffXIn[0]<<"\n"
//        <<"YIn = "<<avgDiffYIn[0]<<"+-"<<eavgDiffYIn[0]<<"\n"
//        <<"X'In = "<<avgDiffXSlopeIn[0]<<"+-"<<eavgDiffXSlopeIn[0]<<"\n"
//        <<"Y'In = "<<avgDiffYSlopeIn[0]<<"+-"<<eavgDiffYSlopeIn[0]<<"\n"
//        <<"EIn = "<<avgDiffEIn[0]<<"+-"<<eavgDiffEIn[0]<<"\n"
//        <<"QIn = "<<avgChargeIn[0]<<"+-"<<eavgChargeIn[0]
//        <<normal<<endl;

//   cout <<red
//        <<"XOut = "<<avgDiffXOut[0]<<"+-"<<eavgDiffXOut[0]<<"\n"
//        <<"YOut = "<<avgDiffYOut[0]<<"+-"<<eavgDiffYOut[0]<<"\n"
//        <<"X'Out = "<<avgDiffXSlopeOut[0]<<"+-"<<eavgDiffXSlopeOut[0]<<"\n"
//        <<"Y'Out = "<<avgDiffYSlopeOut[0]<<"+-"<<eavgDiffYSlopeOut[0]<<"\n"
//        <<"EOut = "<<avgDiffEOut[0]<<"+-"<<eavgDiffEOut[0]<<"\n"
//        <<"QOut = "<<avgChargeOut[0]<<"+-"<<eavgChargeOut[0]
//        <<normal<<endl;

  /****************************************/
  /****************************************/

//   senXAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senX_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
// 		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

//   if (senXAvg.is_open()) {

//     while(!senXAvg.eof()) {
//       zeroAvg[counterSenX]=0;
//       eavgModCorXIn2[counterSenX] = 0; eavgModCorXOut2[counterSenX] = 0;

//       senXAvg >> octant[counterSenX]>>avgSenXIn[counterSenX]>>eavgSenXIn[counterSenX]>>avgSenXOut[counterSenX]>>eavgSenXOut[counterSenX];
//       if (!senXAvg.good()) break;
//       avgCorXIn[counterSenX]  = avgSenXIn[counterSenX]*avgDiffXIn[0];   eavgCorXIn[counterSenX]  = eavgSenXIn[counterSenX]*eavgDiffXOut[0];
//       avgCorXOut[counterSenX] = avgSenXOut[counterSenX]*avgDiffXOut[0]; eavgCorXOut[counterSenX] = eavgSenXOut[counterSenX]*eavgDiffXOut[0];

//       avgModSenXIn[counterSenX] = avgSenXIn[counterSenX]*1e0; eavgModSenXIn[counterSenX] = eavgSenXIn[counterSenX]*1e0;
//       avgModSenXOut[counterSenX] = avgSenXOut[counterSenX]*1e0; eavgModSenXOut[counterSenX] = eavgSenXOut[counterSenX]*1e0;

//       avgModCorXIn[counterSenX] = avgCorXIn[counterSenX];  eavgModCorXIn[counterSenX] = eavgCorXIn[counterSenX];
//       avgModCorXOut[counterSenX] = avgCorXOut[counterSenX]; eavgModCorXOut[counterSenX] = eavgCorXOut[counterSenX]; 

//       counterSenX++;
//     }
//   }
//   else {
//     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
//     return -1;
//   }
//   senXAvg.close();

//   /****************************************/

//   senYAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senY_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
// 		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

//   if (senYAvg.is_open()) {

//     while(!senYAvg.eof()) {
//       eavgModCorYIn2[counterSenY] = 0; eavgModCorYOut2[counterSenY] = 0;

//       senYAvg >> octant[counterSenY]>>avgSenYIn[counterSenY]>>eavgSenYIn[counterSenY]>>avgSenYOut[counterSenY]>>eavgSenYOut[counterSenY];
//       if (!senYAvg.good()) break;
//       avgCorYIn[counterSenY]  = avgSenYIn[counterSenY]*avgDiffYIn[0];   eavgCorYIn[counterSenY]  = eavgSenYIn[counterSenY]*eavgDiffYOut[0];
//       avgCorYOut[counterSenY] = avgSenYOut[counterSenY]*avgDiffYOut[0]; eavgCorYOut[counterSenY] = eavgSenYOut[counterSenY]*eavgDiffYOut[0];

//       avgModSenYIn[counterSenY] = avgSenYIn[counterSenY]*1e0; eavgModSenYIn[counterSenY] = eavgSenYIn[counterSenY]*1e0;
//       avgModSenYOut[counterSenY] = avgSenYOut[counterSenY]*1e0; eavgModSenYOut[counterSenY] = eavgSenYOut[counterSenY]*1e0;

//       avgModCorYIn[counterSenY] = avgCorYIn[counterSenY];  eavgModCorYIn[counterSenY] = eavgCorYIn[counterSenY];
//       avgModCorYOut[counterSenY] = avgCorYOut[counterSenY]; eavgModCorYOut[counterSenY] = eavgCorYOut[counterSenY]; 

//       counterSenY++;
//     }
//   }
//   else {
//     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
//     return -1;
//   }
//   senYAvg.close();

//   /****************************************/

//   senXSlopeAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senXSlope_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
// 		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

//   if (senXSlopeAvg.is_open()) {

//     while(!senXSlopeAvg.eof()) {
//       eavgModCorXSlopeIn2[counterSenXSlope] = 0; eavgModCorXSlopeOut2[counterSenXSlope] = 0;

//       senXSlopeAvg >> octant[counterSenXSlope]>>avgSenXSlopeIn[counterSenXSlope]>>eavgSenXSlopeIn[counterSenXSlope]>>avgSenXSlopeOut[counterSenXSlope]>>eavgSenXSlopeOut[counterSenXSlope];
//       if (!senXSlopeAvg.good()) break;
//       avgCorXSlopeIn[counterSenXSlope]  = avgSenXSlopeIn[counterSenXSlope]*avgDiffXSlopeIn[0];   eavgCorXSlopeIn[counterSenXSlope]  = eavgSenXSlopeIn[counterSenXSlope]*eavgDiffXSlopeOut[0];
//       avgCorXSlopeOut[counterSenXSlope] = avgSenXSlopeOut[counterSenXSlope]*avgDiffXSlopeOut[0]; eavgCorXSlopeOut[counterSenXSlope] = eavgSenXSlopeOut[counterSenXSlope]*eavgDiffXSlopeOut[0];

//       avgModSenXSlopeIn[counterSenXSlope] = avgSenXSlopeIn[counterSenXSlope]*1e0; eavgModSenXSlopeIn[counterSenXSlope] = eavgSenXSlopeIn[counterSenXSlope]*1e0;
//       avgModSenXSlopeOut[counterSenXSlope] = avgSenXSlopeOut[counterSenXSlope]*1e0; eavgModSenXSlopeOut[counterSenXSlope] = eavgSenXSlopeOut[counterSenXSlope]*1e0;

//       avgModCorXSlopeIn[counterSenXSlope] = avgCorXSlopeIn[counterSenXSlope];  eavgModCorXSlopeIn[counterSenXSlope] = eavgCorXSlopeIn[counterSenXSlope];
//       avgModCorXSlopeOut[counterSenXSlope] = avgCorXSlopeOut[counterSenXSlope]; eavgModCorXSlopeOut[counterSenXSlope] = eavgCorXSlopeOut[counterSenXSlope]; 

//       counterSenXSlope++;
//     }
//   }
//   else {
//     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
//     return -1;
//   }
//   senXSlopeAvg.close();

//   /****************************************/

//   senYSlopeAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senYSlope_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
// 		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

//   if (senYSlopeAvg.is_open()) {

//     while(!senYSlopeAvg.eof()) {
//       eavgModCorYSlopeIn2[counterSenYSlope] = 0; eavgModCorYSlopeOut2[counterSenYSlope] = 0;

//       senYSlopeAvg >> octant[counterSenYSlope]>>avgSenYSlopeIn[counterSenYSlope]>>eavgSenYSlopeIn[counterSenYSlope]>>avgSenYSlopeOut[counterSenYSlope]>>eavgSenYSlopeOut[counterSenYSlope];
//       if (!senYSlopeAvg.good()) break;
//       avgCorYSlopeIn[counterSenYSlope]  = avgSenYSlopeIn[counterSenYSlope]*avgDiffYSlopeIn[0];   eavgCorYSlopeIn[counterSenYSlope]  = eavgSenYSlopeIn[counterSenYSlope]*eavgDiffYSlopeOut[0];
//       avgCorYSlopeOut[counterSenYSlope] = avgSenYSlopeOut[counterSenYSlope]*avgDiffYSlopeOut[0]; eavgCorYSlopeOut[counterSenYSlope] = eavgSenYSlopeOut[counterSenYSlope]*eavgDiffYSlopeOut[0];

//       avgModSenYSlopeIn[counterSenYSlope] = avgSenYSlopeIn[counterSenYSlope]*1e0; eavgModSenYSlopeIn[counterSenYSlope] = eavgSenYSlopeIn[counterSenYSlope]*1e0;
//       avgModSenYSlopeOut[counterSenYSlope] = avgSenYSlopeOut[counterSenYSlope]*1e0; eavgModSenYSlopeOut[counterSenYSlope] = eavgSenYSlopeOut[counterSenYSlope]*1e0;

//       avgModCorYSlopeIn[counterSenYSlope] = avgCorYSlopeIn[counterSenYSlope];  eavgModCorYSlopeIn[counterSenYSlope] = eavgCorYSlopeIn[counterSenYSlope];
//       avgModCorYSlopeOut[counterSenYSlope] = avgCorYSlopeOut[counterSenYSlope]; eavgModCorYSlopeOut[counterSenYSlope] = eavgCorYSlopeOut[counterSenYSlope]; 

//       counterSenYSlope++;
//     }
//   }
//   else {
//     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
//     return -1;
//   }
//   senYSlopeAvg.close();

//   /****************************************/

//   senEAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senE_%s.txt"
// 		    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
// 		    ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));

//   if (senEAvg.is_open()) {

//     while(!senEAvg.eof()) {
//       eavgModCorEIn2[counterSenE] = 0; eavgModCorEOut2[counterSenE] = 0;

//       senEAvg >> octant[counterSenE]>>avgSenEIn[counterSenE]>>eavgSenEIn[counterSenE]>>avgSenEOut[counterSenE]>>eavgSenEOut[counterSenE];
//       if (!senEAvg.good()) break;
//       avgCorEIn[counterSenE]  = avgSenEIn[counterSenE]*avgDiffEIn[0];   eavgCorEIn[counterSenE]  = eavgSenEIn[counterSenE]*eavgDiffEOut[0];
//       avgCorEOut[counterSenE] = avgSenEOut[counterSenE]*avgDiffEOut[0]; eavgCorEOut[counterSenE] = eavgSenEOut[counterSenE]*eavgDiffEOut[0];

//       avgModSenEIn[counterSenE] = avgSenEIn[counterSenE]*1e0; eavgModSenEIn[counterSenE] = eavgSenEIn[counterSenE]*1e0;
//       avgModSenEOut[counterSenE] = avgSenEOut[counterSenE]*1e0; eavgModSenEOut[counterSenE] = eavgSenEOut[counterSenE]*1e0;

//       avgModCorEIn[counterSenE] = avgCorEIn[counterSenE];  eavgModCorEIn[counterSenE] = eavgCorEIn[counterSenE];
//       avgModCorEOut[counterSenE] = avgCorEOut[counterSenE]; eavgModCorEOut[counterSenE] = eavgCorEOut[counterSenE]; 

//       counterSenE++;
//     }
//   }
//   else {
//     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
//     return -1;
//   }
//   senEAvg.close();

//   /****************************************/

  senChargeAvg.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senCharge_%s.txt"
			 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));
//   senChargeAvg.open(Form("dirPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_senCharge_%s.txt"
// 			 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
// 			 ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),database_stem.Data()));

  cout<<Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_senCharge_%s.txt"
	     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
	     ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data())<<endl;

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

//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" XIn = "<<avgSenXIn[i]<<"+-"<<eavgSenXIn[i]<<red<<"\tXOut = "<<avgSenXOut[i]<<"+-"<<eavgSenXOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" YIn = "<<avgSenYIn[i]<<"+-"<<eavgSenXIn[i]<<red<<"\tYOut = "<<avgSenYOut[i]<<"+-"<<eavgSenYOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" X'In = "<<avgSenXSlopeIn[i]<<"+-"<<eavgSenXSlopeIn[i]<<red<<"\tX'Out = "<<avgSenXSlopeOut[i]<<"+-"<<eavgSenXSlopeOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" Y'In = "<<avgSenYSlopeIn[i]<<"+-"<<eavgSenYSlopeIn[i]<<red<<"\tY'Out = "<<avgSenYSlopeOut[i]<<"+-"<<eavgSenYSlopeOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" EIn = "<<avgSenEIn[i]<<"+-"<<eavgSenEIn[i]<<red<<"\tEOut = "<<avgSenEOut[i]<<"+-"<<eavgSenEOut[i]<<normal<<endl;
//   }
  cout <<"\n"<<endl;
  for ( Int_t i=0; i<8; i++) {
    cout <<"MD-"<<i+1<<blue<<" QIn = "<<avgSenChargeIn[i]<<"+-"<<eavgSenChargeIn[i]<<red<<"\tQOut = "<<avgSenChargeOut[i]<<"+-"<<eavgSenChargeOut[i]<<normal<<endl;
  }
  /****************************************/

//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" XIn = "<<avgCorXIn[i]<<"+-"<<eavgCorXIn[i]<<red<<"\tXOut = "<<avgCorXOut[i]<<"+-"<<eavgCorXOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" YIn = "<<avgCorYIn[i]<<"+-"<<eavgCorXIn[i]<<red<<"\tYOut = "<<avgCorYOut[i]<<"+-"<<eavgCorYOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" X'In = "<<avgCorXSlopeIn[i]<<"+-"<<eavgCorXSlopeIn[i]<<red<<"\tX'Out = "<<avgCorXSlopeOut[i]<<"+-"<<eavgCorXSlopeOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" Y'In = "<<avgCorYSlopeIn[i]<<"+-"<<eavgCorYSlopeIn[i]<<red<<"\tY'Out = "<<avgCorYSlopeOut[i]<<"+-"<<eavgCorYSlopeOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" EIn = "<<avgCorEIn[i]<<"+-"<<eavgCorEIn[i]<<red<<"\tEOut = "<<avgCorEOut[i]<<"+-"<<eavgCorEOut[i]<<normal<<endl;
//   }
//   cout <<"\n"<<endl;
//   for ( Int_t i=0; i<8; i++) {
//     cout <<"MD-"<<i+1<<blue<<" QIn = "<<avgCorChargeIn[i]<<"+-"<<eavgCorChargeIn[i]<<red<<"\tQOut = "<<avgCorChargeOut[i]<<"+-"<<eavgCorChargeOut[i]<<normal<<endl;
//   }


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
  gStyle->SetTitleYOffset(0.90);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.055,"x");
  gStyle->SetLabelSize(0.055,"y");
  gStyle->SetTitleSize(0.055,"x");
  gStyle->SetTitleSize(0.055,"y");
  gStyle->SetTitleX(0.12);
  gStyle->SetTitleW(0.28);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.07);

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

  TF1* cosFit  = new TF1("cosFit","[0] + [1]*sin((x-1)*pi/4) + [2]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
//   TF1* cosFit  = new TF1("cosFit","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
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

//   showCosFit = "FIT = p_{0} + p_{1}sin[#frac{#pi}{4}(x-1)] + p_{2}cos[#frac{#pi}{4}(x-1)]";
  showCosFit = "FIT = p_{0} + p_{1}sin(#phi) + p_{2}cos(#phi)";
  showSinFit = "FIT = p_{0} + p_{1}sin[#frac{#pi}{4}(x-1)]";

  TString titleSen = Form("%s (%s, %s A): Regression-%s %s %s Sensitivity "
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());

  TString titleCor = Form("%s (%s, %s A): Regression-%s %s %s Corrections "
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());




//   /********************************************************/
//   /********************************************************/

  TString titleChargeSen = Form("%s (%s, %s A): Regression-%s %s %s Slug Averaged Charge Sensitivities "
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());


  TCanvas * c3 = new TCanvas("canvas3", titleChargeSen,0,0,canvasSize[2],canvasSize[3]);
  c3->Draw();
  c3->SetBorderSize(0);
  c3->cd();
  TPad*pad31 = new TPad("pad31","pad31",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad32 = new TPad("pad32","pad32",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad31->SetFillColor(kWhite);
  pad31->Draw();
  pad32->Draw();
  pad31->cd();
  TLatex * t31 = new TLatex(0.06,0.3,Form("%s",titleChargeSen.Data()));
  t31->SetTextSize(0.5);
  t31->Draw();
  pad32->cd();
  pad32->Divide(1,1);

  /********************************************************/

  pad32->cd(1);

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
  SenChargeGraph->SetTitle(Form("#frac{#partialA}{#partialQ} : %s",showCosFit.Data()));
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
  statsSenChargeIn->SetTextSize(0.040);
  statsSenChargeOut->SetTextSize(0.040);
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


  Double_t calc_SenChargeInMoutp0     =   ((calc_SenChargeInp0/pow(calc_eSenChargeInp0,2)) - (calc_SenChargeOutp0/pow(calc_eSenChargeOutp0,2))) /((1/pow(calc_eSenChargeInp0,2)) + (1/pow(calc_eSenChargeOutp0,2)));
  Double_t calc_eSenChargeInMoutp0    =   TMath::Sqrt(1/((1/pow(calc_eSenChargeInp0,2)) + (1/pow(calc_eSenChargeOutp0,2))));

  cout<<blue<<"In - Out = "<<calc_SenChargeInMoutp0<<" +- "<<calc_SenChargeInMoutp0<<normal<<endl;

  Double_t calc_SenChargeInOutp0StatAvg     =   ((calc_SenChargeInp0/pow(calc_eSenChargeInp0,2)) + (calc_SenChargeOutp0/pow(calc_eSenChargeOutp0,2))) /((1/pow(calc_eSenChargeInp0,2)) + (1/pow(calc_eSenChargeOutp0,2)));
  Double_t calc_eSenChargeInOutp0StatAvg    =   TMath::Sqrt(1/((1/pow(calc_eSenChargeInp0,2)) + (1/pow(calc_eSenChargeOutp0,2))));

  cout<<red<<"In, Out Stat Avg. = "<<calc_SenChargeInOutp0StatAvg<<" +- "<<calc_eSenChargeInOutp0StatAvg<<normal<<endl;


  TLegend *legSenCharge = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legSenCharge->AddEntry("SenChargeGraphIn",  Form("<IN> = %2.2f #pm %2.2f",calc_SenChargeInp0,calc_eSenChargeInp0),"lp");
  legSenCharge->AddEntry("SenChargeGraphOut",  Form("<OUT> = %2.2f #pm %2.2f",calc_SenChargeOutp0,calc_eSenChargeOutp0),"lp");
  legSenCharge->AddEntry("SenChargeGraphIn",  Form("AVG = %2.2f #pm %2.2f",calc_SenChargeInOutp0StatAvg,calc_eSenChargeInOutp0StatAvg),"");
  legSenCharge->SetTextSize(0.040);
  legSenCharge->SetFillColor(0);
  legSenCharge->SetBorderSize(2);
  legSenCharge->Draw();

  fitSenChargeIn->Draw("same");
  fitSenChargeOut->Draw("same");

  fitSenChargeIn->SetParNames("p_{0}","p_{1}","p_{2}");
  fitSenChargeOut->SetParNames("p_{0}","p_{1}","p_{2}");

  gPad->Update();


  /********************************************************/

  TString saveChargeNonlinearityPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_charge_nonlinearity_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),database_stem.Data());


  c3-> Update();
  c3->Print(saveChargeNonlinearityPlot+".png");
  if(FIGURE){
    c3->Print(saveChargeNonlinearityPlot+".svg");
    c3->Print(saveChargeNonlinearityPlot+".C");
  }



  /********************************************************/
  /********************************************************/


  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);


}
