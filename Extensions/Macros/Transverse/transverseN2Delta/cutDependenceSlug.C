//*****************************************************************************************************//
// Author      : Nuruzzaman on 11/19/2012
// Last Update : 11/27/2012
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
//   printf ( "%sThe current date/time is: %s%s%s",blue,red,asctime(timeinfo),normal);

  // }
  // void plotTransverseN2Delta(){

  Bool_t SCALE = 0;
  Bool_t FIGURE = 0;

  Bool_t FIG_DIFF_IN = 1;
  Bool_t FIG_DIFF_OUT = 1;
  Bool_t FIG_ASYM = 1;

//   TString database="qw_run2_pass1";
//   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";

//   TString deviceTitle = "Pos. PMT"; TString deviceName = "PosPMT";
//   TString deviceTitle = "Neg. PMT";  TString deviceName = "NegPMT";
//   TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";
  TString deviceTitle = "Barsum"; TString deviceName = "Barsum";

  TString target, polar,targ, goodfor, reg_set, reg_calc, reg_root;

  Int_t opt =1;
  Int_t datopt = 2;
  Int_t ropt = 2;
  Int_t qtor_opt = 2;
  Int_t sigmaNUM1 = -5;
  Int_t sigmaNUM2 = 5;

  TString good_for;
  TString qtor_current;
  TString qtor_stem;
  TString good;
  TString interaction;
  TString showFit1,showFit2,showFit3;

  const Int_t NUM = 10;
  const Int_t detNUM = 8;

  Double_t meanXIn, meanErrorXIn, rmsXIn;
  Double_t meanXSlopeIn, meanErrorXSlopeIn, rmsXSlopeIn;
  Double_t meanYIn, meanErrorYIn, rmsYIn;
  Double_t meanYSlopeIn, meanErrorYSlopeIn, rmsYSlopeIn;
  Double_t meanEIn, meanErrorEIn, rmsEIn;
  Double_t meanChargeIn, meanErrorChargeIn, rmsChargeIn;

  Double_t meanXOut, meanErrorXOut, rmsXOut;
  Double_t meanXSlopeOut, meanErrorXSlopeOut, rmsXSlopeOut;
  Double_t meanYOut, meanErrorYOut, rmsYOut;
  Double_t meanYSlopeOut, meanErrorYSlopeOut, rmsYSlopeOut;
  Double_t meanEOut, meanErrorEOut, rmsEOut;
  Double_t meanChargeOut, meanErrorChargeOut, rmsChargeOut;


  Double_t meanXInV[NUM], meanErrorXInV[NUM], rmsXInV[NUM];
  Double_t meanXSlopeInV[NUM], meanErrorXSlopeInV[NUM], rmsXSlopeInV[NUM];
  Double_t meanYInV[NUM], meanErrorYInV[NUM], rmsYInV[NUM];
  Double_t meanYSlopeInV[NUM], meanErrorYSlopeInV[NUM], rmsYSlopeInV[NUM];
  Double_t meanEInV[NUM], meanErrorEInV[NUM], rmsEInV[NUM];
  Double_t meanChargeInV[NUM], meanErrorChargeInV[NUM], rmsChargeInV[NUM];

  Double_t meanXOutV[NUM], meanErrorXOutV[NUM], rmsXOutV[NUM];
  Double_t meanXSlopeOutV[NUM], meanErrorXSlopeOutV[NUM], rmsXSlopeOutV[NUM];
  Double_t meanYOutV[NUM], meanErrorYOutV[NUM], rmsYOutV[NUM];
  Double_t meanYSlopeOutV[NUM], meanErrorYSlopeOutV[NUM], rmsYSlopeOutV[NUM];
  Double_t meanEOutV[NUM], meanErrorEOutV[NUM], rmsEOutV[NUM];
  Double_t meanChargeOutV[NUM], meanErrorChargeOutV[NUM], rmsChargeOutV[NUM];

  Double_t asymMDIn[detNUM],asymErrorMDIn[detNUM],rmsMDIn[detNUM];
  Double_t asymMDOut[detNUM],asymErrorMDOut[detNUM],rmsMDOut[detNUM];


  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[2] ={1300,1000};
  Int_t canvasSize2[2] ={1200,600};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.945};
  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.83,0.65,0.95};
  Double_t yScale[2] = {-10.0,10.0};


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
  std::cout<<Form("2. %sVertical Transverse (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("3. Horizontal Transverse ")<<std::endl;
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

//   Int_t sigmaNUM = 1;
  std::cout<<Form("Enter Left CUT in Sigma. (%sJust Hit ENTER to choose default  = -5%s):",blue,normal)<<std::endl;
  std::string input_sigmaNUM1;
  std::getline( std::cin, input_sigmaNUM1 );
  if ( !input_sigmaNUM1.empty() ) {
    std::istringstream stream( input_sigmaNUM1 );
    stream >> sigmaNUM1;
  }
  std::cout<<Form("Enter Right CUT in Sigma. (%sJust Hit ENTER to choose default = 5%s):",blue,normal)<<std::endl;
  std::string input_sigmaNUM2;
  std::getline( std::cin, input_sigmaNUM2 );
  if ( !input_sigmaNUM2.empty() ) {
    std::istringstream stream( input_sigmaNUM2 );
    stream >> sigmaNUM2;
  }



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
      case    0: reg_calc="off";       reg_set="off";   reg_root="off";     break;
      case    1: reg_calc="on";        reg_set="off";   reg_root="std";     break;
      case    2: reg_calc="on_5+1";    reg_set="off";   reg_root="5+1";     break;
      case    3: reg_calc="on_set3";   reg_set="off";   reg_root="set3";    break;
      case    4: reg_calc="on_set4";   reg_set="off";   reg_root="set4";    break;
      case    5: reg_calc="on_set5";   reg_set="off";   reg_root="set5";    break;
      case    6: reg_calc="on_set6";   reg_set="off";   reg_root="set6";    break;
      case    7: reg_calc="on_set7";   reg_set="off";   reg_root="set7";    break;
      case    8: reg_calc="on_set8";   reg_set="off";   reg_root="set8";    break;
      case    9: reg_calc="on_set9";   reg_set="off";   reg_root="set9";    break;
      case   10: reg_calc="on_set10";  reg_set="off";   reg_root="set10";   break;
      case   11: reg_calc="on_set11";  reg_set="off";   reg_root="set11";   break;
      case   12: reg_calc="on_set12";  reg_set="off";   reg_root="set12";   break;
      case   13: reg_calc="on_set13";  reg_set="off";   reg_root="set13";   break;
      default  : printf("Please insert correct regression information. Exiting program !!!\n"); exit(1); break;
      }
  }

  else{
    switch(regID)
      {
      case    0: reg_calc="off";  reg_set="off";        reg_root="off";     break;
      case    1: reg_calc="off";  reg_set="on";         reg_root="std";     break;
      case    2: reg_calc="off";  reg_set="on_5+1";     reg_root="5+1";     break;
      case    3: reg_calc="off";  reg_set="on_set3";    reg_root="set3";    break;
      case    4: reg_calc="off";  reg_set="on_set4";    reg_root="set4";    break;
      case    5: reg_calc="off";  reg_set="on_set5";    reg_root="set5";    break;
      case    6: reg_calc="off";  reg_set="on_set6";    reg_root="set6";    break;
      case    7: reg_calc="off";  reg_set="on_set7";    reg_root="set7";    break;
      case    8: reg_calc="off";  reg_set="on_set8";    reg_root="set8";    break;
      case    9: reg_calc="off";  reg_set="on_set9";    reg_root="set9";    break;
      case   10: reg_calc="off";  reg_set="on_set10";   reg_root="set10";   break;
      case   11: reg_calc="off";  reg_set="on_set11";   reg_root="set11";   break;
      case   12: reg_calc="off";  reg_set="on_set12";   reg_root="set12";   break;
      case   13: reg_calc="off";  reg_set="on_set13";   reg_root="set13";   break;
      default  : printf("Please insert correct regression information. Exiting program !!!\n"); exit(1); break;
      }
  }


  TApplication theApp("App",&argc,argv);

  /**********************************************/
  /**********************************************/

  TString rootFileDir = "/cache/mss/hallc/qweak/rootfiles/pass5/Slugs";
  Int_t slugNumberIn=0, slugNumberOut=0, slugNumberOut2=0;

  // Create chains to put the slugfiles into
  TChain *rawChainIn   =  new TChain("slug");
  TChain *rawChainOut  =  new TChain("slug");
  TChain *regChainIn   =  new TChain("slug");
  TChain *regChainOut  =  new TChain("slug");

  /* LH2, Horizontal, N2Delta, 6000A  */
  if(opt==1 && datopt==3 && ropt==2 && qtor_opt==1){slugNumberIn= 700005; slugNumberOut= 700006; }
  /* LH2, Horizontal, N2Delta, 6700A  */
  if(opt==1 && datopt==3 && ropt==2 && qtor_opt==2){slugNumberIn= 700002; slugNumberOut= 700001; }
  /* LH2, Horizontal, N2Delta, 7300A  */
  if(opt==1 && datopt==3 && ropt==2 && qtor_opt==3){slugNumberIn= 700003; slugNumberOut= 700004; }

  /* LH2, Vertical, N2Delta, 6700A (default) */
//   if(opt==1 && datopt==2 && ropt==2 && qtor_opt==2){slugNumberIn= 600002; slugNumberOut= 600001; }
  if(opt==1 && datopt==2 && ropt==2 && qtor_opt==2){slugNumberIn= 39; slugNumberOut= 39; }

  /* Al, Horizontal, N2Delta, 6700A   */
  if(opt==2 && datopt==3 && ropt==2 && qtor_opt==2){slugNumberIn= 701001; slugNumberOut= 701002; }
  /* Al, Vertical,   N2Delta, 6700A   */
  if(opt==2 && datopt==2 && ropt==2 && qtor_opt==2){slugNumberIn= 601001; slugNumberOut= 601002; }

  /* Al, Horizontal, N2Delta, 7300A   */
  if(opt==2 && datopt==3 && ropt==2 && qtor_opt==3){slugNumberIn= 701004; slugNumberOut= 701003; slugNumberOut2= 701005; }

  /* Carbon, Horizontal, N2Delta, 6700A   */
  if(opt==3 && datopt==3 && ropt==2 && qtor_opt==2){slugNumberIn= 703002; slugNumberOut= 703001; }


  // Raw
  rawChainIn->Add(Form("%s/QwPass5_16066.000.trees.root",rootFileDir.Data()));
  rawChainOut->Add(Form("%s/QwPass5_16065.000.trees.root",rootFileDir.Data()));

//   // Regressed
//   regChainIn->Add(Form("%s/slug%d_lrb_%s.root",rootFileDir.Data(),slugNumberIn,reg_root.Data()));
//   regChainOut->Add(Form("%s/slug%d_lrb_%s.root",rootFileDir.Data(),slugNumberOut,reg_root.Data()));


//   //  Make them friends
//   rawChainIn->AddFriend(regChainIn,"regRawChainIn");
//   rawChainOut->AddFriend(regChainOut,"regRawChainOut");
// //   rawChainIn->AddFriend("regChainIn","regRawChainIn");
// //   rawChainOut->AddFriend("regChainOut","regRawChainOut");
// //   rawChainIn->AddFriend("regRawChainIn",regChainIn);
// //   rawChainOut->AddFriend("regRawChainOut",regChainOut);

  TString cutErrorFlag = "ErrorFlag==0";

  TString diffCut = Form("%s",cutErrorFlag.Data());

  printf("In Slug File  = slug%d.root and slug%d_lrb_%s.root \n",slugNumberIn,slugNumberIn,reg_root.Data());
  printf("Out Slug File = slug%d.root and slug%d_lrb_%s.root \n",slugNumberOut,slugNumberOut,reg_root.Data());

  /**********************************************/
  /**********************************************/

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
//   gStyle->SetOptStat(0000000);
  gStyle->SetOptStat("eMr");
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.32);
  gStyle->SetStatH(0.18);
  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatColor(0);
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(1.0);
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


  // Fit function to show 
  showFit1 = "FIT = Am*cos(phi) - C*sin(phi) + D";
  showFit2 = "FIT = Am*cos(phi + phi0) + C";
  showFit3 = "FIT = Am*sin(phi + phi0) + C";


  TString title1;
  TString titleSummary = Form("%s (%s, %s A): Regression-%s Differences "
			      ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data());

  if(datopt==1){ title1= Form("%s ",titleSummary.Data());}
  else if(datopt==2){ title1= Form("%s ",titleSummary.Data());}
  else{
    title1= Form("%s ",titleSummary.Data());
  }


  std::ofstream diffFileIn,diffFileOut,textMDInputIn,textMDInputOut,Myfile3;
  Char_t  textFileIn[400],textFileOut[400],textMDOutputIn[400],textMDOutputOut[400],textfile3[400];
  sprintf(textFileIn,"dirPlot/%s_%s_%s_%s_Diff_In_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
  sprintf(textFileOut,"dirPlot/%s_%s_%s_%s_Diff_In_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
  sprintf(textMDOutputIn,"dirPlot/%s_%s_%s_%s_MD_%d%d-sigma_In_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,sigmaNUM1,sigmaNUM2,reg_calc.Data(),reg_set.Data(),database_stem.Data());  
  sprintf(textMDOutputOut,"dirPlot/%s_%s_%s_%s_MD_%d%d-sigma_Out_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,sigmaNUM1,sigmaNUM2,reg_calc.Data(),reg_set.Data(),database_stem.Data());  

  sprintf(textfile3,"dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_%s_%d%d-sigma_cut_dependence_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),sigmaNUM1,sigmaNUM2,database_stem.Data());

  if(FIG_DIFF_IN){

  TH1* histDiffInX;
  TH1* histDiffInY;
  TH1* histDiffInXSlope;
  TH1* histDiffInYSlope;
  TH1* histDiffInE;
  TH1* histAsymInCharge;

  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,canvasSize[0],canvasSize[1]);
  Canvas1->Draw();
  Canvas1->SetBorderSize(0);
  Canvas1->cd();
  TPad*pad11 = new TPad("pad11","pad11",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad12 = new TPad("pad12","pad12",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad11->SetFillColor(kWhite);
  pad11->Draw();
  pad12->Draw();
  pad11->cd();
  TString text1 = Form("%s (%s, %s A): Regression-off Differences, HWP-IN ",targ.Data(),polar.Data(),qtor_stem.Data());
  TText * t1 = new TText(0.00,0.3,text1);
  t1->SetTextSize(0.45);
  t1->Draw();
  pad12->Divide(2,3);


  pad12->cd(1);
  gPad->SetLogy();
  rawChainIn->Draw("diff_qwk_targetX*1e3>>hDiffInX",""+diffCut+"");
  histDiffInX = (TH1F *)gDirectory->Get("hDiffInX");
  histDiffInX->SetLineColor(kRed);
  histDiffInX->GetXaxis()->SetNdivisions(8,5,0);
  histDiffInX->GetYaxis()->SetNdivisions(5,5,0);
  histDiffInX->GetXaxis()->CenterTitle();
//   histDiffInX->GetYaxis()->CenterTitle();
//   histDiffInX->GetYaxis()->SetTitle("Arbitraray ");
  histDiffInX->GetXaxis()->SetTitle(Form("Diff Target X [#mum]"));
  histDiffInX->SetTitle(Form(""));
  histDiffInX->Draw("");
  gPad->Update();

  pad12->cd(2);
  gPad->SetLogy();
  rawChainIn->Draw("diff_qwk_targetXSlope*1e6>>hDiffInXSlope",""+diffCut+"");
  histDiffInXSlope = (TH1F *)gDirectory->Get("hDiffInXSlope");
  histDiffInXSlope->SetLineColor(kRed);
  histDiffInXSlope->GetXaxis()->SetNdivisions(8,5,0);
  histDiffInXSlope->GetYaxis()->SetNdivisions(5,5,0);
  histDiffInXSlope->GetXaxis()->CenterTitle();
//   histDiffInXSlope->GetYaxis()->CenterTitle();
//   histDiffInXSlope->GetYaxis()->SetTitle("Arbitraray ");
  histDiffInXSlope->GetXaxis()->SetTitle(Form("Diff Target XSlope [#murad]"));
  histDiffInXSlope->SetTitle(Form(""));
  histDiffInXSlope->Draw("");
  gPad->Update();

  pad12->cd(3);
  gPad->SetLogy();
  rawChainIn->Draw("diff_qwk_targetY*1e3>>hDiffInY",""+diffCut+"");
  histDiffInY = (TH1F *)gDirectory->Get("hDiffInY");
  histDiffInY->SetLineColor(kGreen);
  histDiffInY->GetXaxis()->SetNdivisions(8,5,0);
  histDiffInY->GetYaxis()->SetNdivisions(5,5,0);
  histDiffInY->GetXaxis()->CenterTitle();
//   histDiffInY->GetYaxis()->CenterTitle();
//   histDiffInY->GetYaxis()->SetTitle("Arbitraray ");
  histDiffInY->GetXaxis()->SetTitle(Form("Diff Target Y [#mum]"));
  histDiffInY->SetTitle(Form(""));
  histDiffInY->Draw("");
  gPad->Update();

  pad12->cd(4);
  gPad->SetLogy();
  rawChainIn->Draw("diff_qwk_targetYSlope*1e6>>hDiffInYSlope",""+diffCut+"");
  histDiffInYSlope = (TH1F *)gDirectory->Get("hDiffInYSlope");
  histDiffInYSlope->SetLineColor(kGreen);
  histDiffInYSlope->GetXaxis()->SetNdivisions(8,5,0);
  histDiffInYSlope->GetYaxis()->SetNdivisions(5,5,0);
  histDiffInYSlope->GetXaxis()->CenterTitle();
//   histDiffInYSlope->GetYaxis()->CenterTitle();
//   histDiffInYSlope->GetYaxis()->SetTitle("Arbitraray ");
  histDiffInYSlope->GetXaxis()->SetTitle(Form("Diff Target YSlope [#murad]"));
  histDiffInYSlope->SetTitle(Form(""));
  histDiffInYSlope->Draw("");
  gPad->Update();

  pad12->cd(5);
  gPad->SetLogy();
  rawChainIn->Draw("diff_qwk_energy>>hDiffInE",""+diffCut+"");
  histDiffInE = (TH1F *)gDirectory->Get("hDiffInE");
  histDiffInE->SetLineColor(kBlue);
  histDiffInE->GetXaxis()->SetNdivisions(8,5,0);
  histDiffInE->GetYaxis()->SetNdivisions(5,5,0);
  histDiffInE->GetXaxis()->CenterTitle();
//   histDiffInE->GetYaxis()->CenterTitle();
//   histDiffInE->GetYaxis()->SetTitle("Arbitraray ");
  histDiffInE->GetXaxis()->SetTitle(Form("Diff E [dim. less]"));
  histDiffInE->SetTitle(Form(""));
  histDiffInE->Draw("");
  gPad->Update();

  pad12->cd(6);
  gPad->SetLogy();
  rawChainIn->Draw("asym_qwk_charge*1e6*1e3>>hAsymInCharge",""+diffCut+"");
  histAsymInCharge = (TH1F *)gDirectory->Get("hAsymInCharge");
  histAsymInCharge->SetLineColor(kMagenta);
  histAsymInCharge->GetXaxis()->SetNdivisions(8,5,0);
  histAsymInCharge->GetYaxis()->SetNdivisions(5,5,0);
  histAsymInCharge->GetXaxis()->CenterTitle();
//   histAsymInCharge->GetYaxis()->CenterTitle();
//   histAsymInCharge->GetYaxis()->SetTitle("Arbitraray ");
  histAsymInCharge->GetXaxis()->SetTitle(Form("Charge Asym. [ppb]"));
  histAsymInCharge->SetTitle(Form(""));
  histAsymInCharge->Draw("");
  gPad->Update();

  meanXIn            =  histDiffInX->GetMean();
  meanErrorXIn       =  histDiffInX->GetMeanError();
  rmsXIn             =  histDiffInX->GetRMS();
  
  meanXSlopeIn       =  histDiffInXSlope->GetMean();
  meanErrorXSlopeIn  =  histDiffInXSlope->GetMeanError();
  rmsXSlopeIn        =  histDiffInXSlope->GetRMS();
  
  meanYIn            =  histDiffInY->GetMean();
  meanErrorYIn       =  histDiffInY->GetMeanError();
  rmsYIn             =  histDiffInY->GetRMS();
  
  meanYSlopeIn       =  histDiffInYSlope->GetMean();
  meanErrorYSlopeIn  =  histDiffInYSlope->GetMeanError();
  rmsYSlopeIn        =  histDiffInYSlope->GetRMS();
  
  meanEIn            =  histDiffInE->GetMean();
  meanErrorEIn       =  histDiffInE->GetMeanError();
  rmsEIn             =  histDiffInE->GetRMS();
  
  meanChargeIn       =  histAsymInCharge->GetMean();
  meanErrorChargeIn  =  histAsymInCharge->GetMeanError();
  rmsChargeIn        =  histAsymInCharge->GetRMS();
 

  diffFileIn.open(textFileIn);
  
  diffFileIn<<Form("%4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f"
		   ,meanXIn,meanErrorXIn,rmsXIn
		   ,meanXSlopeIn,meanErrorXSlopeIn,rmsXSlopeIn
		   ,meanYIn,meanErrorYIn,rmsYIn
		   ,meanYSlopeIn,meanErrorYSlopeIn,rmsYSlopeIn
		   ,meanEIn,meanErrorEIn,rmsEIn
		   ,meanChargeIn,meanErrorChargeIn,rmsChargeIn
		   )<<endl;
  diffFileIn.close();


  TString saveDiffInPlot = Form("dirPlot/%s_%s_%s_%s_%s_%s_diff_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			       ,reg_calc.Data(),reg_set.Data(),database_stem.Data());

  Canvas1->Update();
  Canvas1->Print(saveDiffInPlot+".png");
  if(FIGURE){
    Canvas1->Print(saveDiffInPlot+".svg");
    Canvas1->Print(saveDiffInPlot+".C");
  }

  }

//   /***********************************************************/
//   /***********************************************************/

  if(FIG_DIFF_OUT){

  TH1* histDiffOutX;
  TH1* histDiffOutY;
  TH1* histDiffOutXSlope;
  TH1* histDiffOutYSlope;
  TH1* histDiffOutE;
  TH1* histAsymOutCharge;

  TCanvas * Canvas2 = new TCanvas("canvas2", title1,0,0,canvasSize[0],canvasSize[1]);
  Canvas2->Draw();
  Canvas2->SetBorderSize(0);
  Canvas2->cd();
  TPad*pad21 = new TPad("pad21","pad21",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad22 = new TPad("pad22","pad22",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad21->SetFillColor(kWhite);
  pad21->Draw();
  pad22->Draw();
  pad21->cd();
  TString text2 = Form("%s (%s, %s A): Regression-off Differences, HWP-OUT ",targ.Data(),polar.Data(),qtor_stem.Data());
  TText * t2 = new TText(0.00,0.3,text2);
  t2->SetTextSize(0.45);
  t2->Draw();
  pad22->Divide(2,3);


  pad22->cd(1);
  gPad->SetLogy();
  rawChainOut->Draw("diff_qwk_targetX*1e3>>hDiffOutX",""+diffCut+"");
  histDiffOutX = (TH1F *)gDirectory->Get("hDiffOutX");
  histDiffOutX->SetLineColor(kRed);
  histDiffOutX->GetXaxis()->SetNdivisions(8,5,0);
  histDiffOutX->GetYaxis()->SetNdivisions(5,5,0);
  histDiffOutX->GetXaxis()->CenterTitle();
//   histDiffOutX->GetYaxis()->CenterTitle();
//   histDiffOutX->GetYaxis()->SetTitle("Arbitraray ");
  histDiffOutX->GetXaxis()->SetTitle(Form("Diff Target X [#mum]"));
  histDiffOutX->SetTitle(Form(""));
  histDiffOutX->Draw("");
  gPad->Update();

  pad22->cd(2);
  gPad->SetLogy();
  rawChainOut->Draw("diff_qwk_targetXSlope*1e6>>hDiffOutXSlope",""+diffCut+"");
  histDiffOutXSlope = (TH1F *)gDirectory->Get("hDiffOutXSlope");
  histDiffOutXSlope->SetLineColor(kRed);
  histDiffOutXSlope->GetXaxis()->SetNdivisions(8,5,0);
  histDiffOutXSlope->GetYaxis()->SetNdivisions(5,5,0);
  histDiffOutXSlope->GetXaxis()->CenterTitle();
//   histDiffOutXSlope->GetYaxis()->CenterTitle();
//   histDiffOutXSlope->GetYaxis()->SetTitle("Arbitraray ");
  histDiffOutXSlope->GetXaxis()->SetTitle(Form("Diff Target XSlope [#murad]"));
  histDiffOutXSlope->SetTitle(Form(""));
  histDiffOutXSlope->Draw("");
  gPad->Update();

  pad22->cd(3);
  gPad->SetLogy();
  rawChainOut->Draw("diff_qwk_targetY*1e3>>hDiffOutY",""+diffCut+"");
  histDiffOutY = (TH1F *)gDirectory->Get("hDiffOutY");
  histDiffOutY->SetLineColor(kGreen);
  histDiffOutY->GetXaxis()->SetNdivisions(8,5,0);
  histDiffOutY->GetYaxis()->SetNdivisions(5,5,0);
  histDiffOutY->GetXaxis()->CenterTitle();
//   histDiffOutY->GetYaxis()->CenterTitle();
//   histDiffOutY->GetYaxis()->SetTitle("Arbitraray ");
  histDiffOutY->GetXaxis()->SetTitle(Form("Diff Target Y [#mum]"));
  histDiffOutY->SetTitle(Form(""));
  histDiffOutY->Draw("");
  gPad->Update();

  pad22->cd(4);
  gPad->SetLogy();
  rawChainOut->Draw("diff_qwk_targetYSlope*1e6>>hDiffOutYSlope",""+diffCut+"");
  histDiffOutYSlope = (TH1F *)gDirectory->Get("hDiffOutYSlope");
  histDiffOutYSlope->SetLineColor(kGreen);
  histDiffOutYSlope->GetXaxis()->SetNdivisions(8,5,0);
  histDiffOutYSlope->GetYaxis()->SetNdivisions(5,5,0);
  histDiffOutYSlope->GetXaxis()->CenterTitle();
//   histDiffOutYSlope->GetYaxis()->CenterTitle();
//   histDiffOutYSlope->GetYaxis()->SetTitle("Arbitraray ");
  histDiffOutYSlope->GetXaxis()->SetTitle(Form("Diff Target YSlope [#murad]"));
  histDiffOutYSlope->SetTitle(Form(""));
  histDiffOutYSlope->Draw("");
  gPad->Update();

  pad22->cd(5);
  gPad->SetLogy();
  rawChainOut->Draw("diff_qwk_energy>>hDiffOutE",""+diffCut+"");
  histDiffOutE = (TH1F *)gDirectory->Get("hDiffOutE");
  histDiffOutE->SetLineColor(kBlue);
  histDiffOutE->GetXaxis()->SetNdivisions(8,5,0);
  histDiffOutE->GetYaxis()->SetNdivisions(5,5,0);
  histDiffOutE->GetXaxis()->CenterTitle();
//   histDiffOutE->GetYaxis()->CenterTitle();
//   histDiffOutE->GetYaxis()->SetTitle("Arbitraray ");
  histDiffOutE->GetXaxis()->SetTitle(Form("Diff E [dim. less]"));
  histDiffOutE->SetTitle(Form(""));
  histDiffOutE->Draw("");
  gPad->Update();

  pad22->cd(6);
  gPad->SetLogy();
  rawChainOut->Draw("asym_qwk_charge*1e6*1e3>>hAsymOutCharge",""+diffCut+"");
  histAsymOutCharge = (TH1F *)gDirectory->Get("hAsymOutCharge");
  histAsymOutCharge->SetLineColor(kMagenta);
  histAsymOutCharge->GetXaxis()->SetNdivisions(8,5,0);
  histAsymOutCharge->GetYaxis()->SetNdivisions(5,5,0);
  histAsymOutCharge->GetXaxis()->CenterTitle();
//   histAsymOutCharge->GetYaxis()->CenterTitle();
//   histAsymOutCharge->GetYaxis()->SetTitle("Arbitraray ");
  histAsymOutCharge->GetXaxis()->SetTitle(Form("Charge Asym. [ppb]"));
  histAsymOutCharge->SetTitle(Form(""));
  histAsymOutCharge->Draw("");
  gPad->Update();


  meanXOut            =  histDiffOutX->GetMean();
  meanErrorXOut       =  histDiffOutX->GetMeanError();
  rmsXOut             =  histDiffOutX->GetRMS();
  
  meanXSlopeOut       =  histDiffOutXSlope->GetMean();
  meanErrorXSlopeOut  =  histDiffOutXSlope->GetMeanError();
  rmsXSlopeOut        =  histDiffOutXSlope->GetRMS();
  
  meanYOut            =  histDiffOutY->GetMean();
  meanErrorYOut       =  histDiffOutY->GetMeanError();
  rmsYOut             =  histDiffOutY->GetRMS();
  
  meanYSlopeOut       =  histDiffOutYSlope->GetMean();
  meanErrorYSlopeOut  =  histDiffOutYSlope->GetMeanError();
  rmsYSlopeOut        =  histDiffOutYSlope->GetRMS();
  
  meanEOut            =  histDiffOutE->GetMean();
  meanErrorEOut       =  histDiffOutE->GetMeanError();
  rmsEOut             =  histDiffOutE->GetRMS();
  
  meanChargeOut       =  histAsymOutCharge->GetMean();
  meanErrorChargeOut  =  histAsymOutCharge->GetMeanError();
  rmsChargeOut        =  histAsymOutCharge->GetRMS();
  

  diffFileOut.open(textFileOut);

  diffFileOut<<Form("%4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f"
		   ,meanXOut,meanErrorXOut,rmsXOut
		   ,meanXSlopeOut,meanErrorXSlopeOut,rmsXSlopeOut
		   ,meanYOut,meanErrorYOut,rmsYOut
		   ,meanYSlopeOut,meanErrorYSlopeOut,rmsYSlopeOut
		   ,meanEOut,meanErrorEOut,rmsEOut
		   ,meanChargeOut,meanErrorChargeOut,rmsChargeOut
		   )<<endl;
  diffFileOut.close();


  TString saveDiffOutPlot = Form("dirPlot/%s_%s_%s_%s_%s_%s_diff_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			       ,reg_calc.Data(),reg_set.Data(),database_stem.Data());

  Canvas2->Update();
  Canvas2->Print(saveDiffOutPlot+".png");
  if(FIGURE){
    Canvas2->Print(saveDiffOutPlot+".svg");
    Canvas2->Print(saveDiffOutPlot+".C");
  }

  }

  /***********************************************************/
  /***********************************************************/

  int position1[8] = { 4, 1, 2, 3, 6, 9, 8, 7 };

  int counter=0;
  ifstream textDiffIn,textDiffOut;

  textDiffIn.open(textFileIn);
  textDiffOut.open(textFileOut);

  printf("Text file to open %s\n",textFileIn);

  if (textDiffIn.is_open()) {
   
    while(1) {
      textDiffIn >> meanXInV[counter]>>meanErrorXInV[counter]>>rmsXInV[counter]
		 >> meanXSlopeInV[counter]>>meanErrorXSlopeInV[counter]>>rmsXSlopeInV[counter]
		 >> meanYInV[counter]>>meanErrorYInV[counter]>>rmsYInV[counter]
		 >> meanYSlopeInV[counter]>>meanErrorYSlopeInV[counter]>>rmsYSlopeInV[counter]
		 >> meanEInV[counter]>>meanErrorEInV[counter]>>rmsEInV[counter]
		 >> meanChargeInV[counter]>>meanErrorChargeInV[counter]>>rmsChargeInV[counter];
   
      if (!textDiffIn.good()) break;

      textDiffOut >> meanXOutV[counter]>>meanErrorXOutV[counter]>>rmsXOutV[counter]
		  >> meanXSlopeOutV[counter]>>meanErrorXSlopeOutV[counter]>>rmsXSlopeOutV[counter]
		  >> meanYOutV[counter]>>meanErrorYOutV[counter]>>rmsYOutV[counter]>>meanYSlopeOutV[counter]
		  >> meanErrorYSlopeOutV[counter]>>rmsYSlopeOutV[counter]
		  >> meanEOutV[counter]>>meanErrorEOutV[counter]>>rmsEOutV[counter]
		  >> meanChargeOutV[counter]>>meanErrorChargeOutV[counter]>>rmsChargeOutV[counter];
      
      if (!textDiffOut.good()) break;

      counter++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  textDiffIn.close();
  textDiffOut.close();

  printf("In %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f %4.9f\n",meanXInV[0],meanErrorXInV[0],rmsXInV[0],meanXSlopeInV[0],meanErrorXSlopeInV[0],rmsXSlopeInV[0],meanYInV[0],meanErrorYInV[0],rmsYInV[0],meanYSlopeInV[0],meanErrorYSlopeInV[0],rmsYSlopeInV[0],meanEInV[0],meanErrorEInV[0],rmsEInV[0],meanChargeInV[0],meanErrorChargeInV[0],rmsChargeInV[0]);


//   TString sigmaXIn          =  Form("abs(diff_qwk_targetX*1e3 - %f)<(%d*%f)",meanXInV[0],sigmaNUM2,rmsXInV[0]);
//   TString sigmaXSlopeIn     =  Form("abs(diff_qwk_targetXSlope*1e6 - %f)<(%d*%f)",meanXSlopeInV[0],sigmaNUM2,rmsXSlopeInV[0]);
//   TString sigmaYIn          =  Form("abs(diff_qwk_targetY*1e3 - %f)<(%d*%f)",meanYInV[0],sigmaNUM2,rmsYInV[0]);
//   TString sigmaYSlopeIn     =  Form("abs(diff_qwk_targetYSlope*1e6 - %f)<(%d*%f)",meanYSlopeInV[0],sigmaNUM2,rmsYSlopeInV[0]);
//   TString sigmaEIn          =  Form("abs(diff_qwk_energy - %f)<(%d*%f)",meanEInV[0],sigmaNUM2,rmsEInV[0]);
//   TString sigmaChargeIn     =  Form("abs(asym_qwk_charge*1e6*1e3 - %f)<(%d*%f)",meanChargeInV[0],sigmaNUM2,rmsChargeInV[0]);

//   TString sigmaXOut          =  Form("abs(diff_qwk_targetX*1e3 - %f)<(%d*%f)",meanXOutV[0],sigmaNUM2,rmsXOutV[0]);
//   TString sigmaXSlopeOut     =  Form("abs(diff_qwk_targetXSlope*1e6 - %f)<(%d*%f)",meanXSlopeOutV[0],sigmaNUM2,rmsXSlopeOutV[0]);
//   TString sigmaYOut          =  Form("abs(diff_qwk_targetY*1e3 - %f)<(%d*%f)",meanYOutV[0],sigmaNUM2,rmsYOutV[0]);
//   TString sigmaYSlopeOut     =  Form("abs(diff_qwk_targetYSlope*1e6 - %f)<(%d*%f)",meanYSlopeOutV[0],sigmaNUM2,rmsYSlopeOutV[0]);
//   TString sigmaEOut          =  Form("abs(diff_qwk_energy - %f)<(%d*%f)",meanEOutV[0],sigmaNUM2,rmsEOutV[0]);
//   TString sigmaChargeOut     =  Form("abs(asym_qwk_charge*1e6*1e3 - %f)<(%d*%f)",meanChargeOutV[0],sigmaNUM2,rmsChargeOutV[0]);
//   printf("abs(diff_qwk_targetX*1e3 - %f)<(%d*%f)\n",meanXInV[0],sigmaNUM2,rmsXInV[0]);

  TString sigmaXIn          =  Form("diff_qwk_targetX*1e3>(%f + %f*%d) && diff_qwk_targetX*1e3<(%f + %f*%d)",meanXInV[0],rmsXInV[0],sigmaNUM1,meanXInV[0],rmsXInV[0],sigmaNUM2);
  TString sigmaXSlopeIn     =  Form("diff_qwk_targetXSlope*1e6 >(%f + %f*%d) && diff_qwk_targetXSlope*1e6 <(%f + %f*%d)",meanXSlopeInV[0],rmsXSlopeInV[0],sigmaNUM1,meanXSlopeInV[0],rmsXSlopeInV[0],sigmaNUM2);
  TString sigmaYIn          =  Form("diff_qwk_targetY*1e3 >(%f + %f*%d) && diff_qwk_targetY*1e3 <(%f + %f*%d)",meanYInV[0],rmsYInV[0],sigmaNUM1,meanYInV[0],rmsYInV[0],sigmaNUM2);
  TString sigmaYSlopeIn     =  Form("diff_qwk_targetYSlope*1e6 >(%f + %f*%d) && diff_qwk_targetYSlope*1e6 <(%f + %f*%d)",meanYSlopeInV[0],rmsYSlopeInV[0],sigmaNUM1,meanYSlopeInV[0],rmsYSlopeInV[0],sigmaNUM2);
  TString sigmaEIn          =  Form("diff_qwk_energy >(%f + %f*%d) && diff_qwk_energy <(%f + %f*%d)",meanEInV[0],rmsEInV[0],sigmaNUM1,meanEInV[0],rmsEInV[0],sigmaNUM2);
  TString sigmaChargeIn     =  Form("asym_qwk_charge*1e6*1e3 >(%f + %f*%d) && asym_qwk_charge*1e6*1e3 <(%f + %f*%d)",meanChargeInV[0],rmsChargeInV[0],sigmaNUM1,meanChargeInV[0],rmsChargeInV[0],sigmaNUM2);

  TString sigmaXOut         =  Form("diff_qwk_targetX*1e3 >(%f + %f*%d) && diff_qwk_targetX*1e3 <(%f + %f*%d)",meanXOutV[0],rmsXOutV[0],sigmaNUM1,meanXOutV[0],rmsXOutV[0],sigmaNUM2);
  TString sigmaXSlopeOut    =  Form("diff_qwk_targetXSlope*1e6 >(%f + %f*%d) && diff_qwk_targetXSlope*1e6 <(%f + %f*%d)",meanXSlopeOutV[0],rmsXSlopeOutV[0],sigmaNUM1,meanXSlopeOutV[0],rmsXSlopeOutV[0],sigmaNUM2);
  TString sigmaYOut         =  Form("diff_qwk_targetY*1e3 >(%f + %f*%d) && diff_qwk_targetY*1e3 <(%f + %f*%d)",meanYOutV[0],rmsYOutV[0],sigmaNUM1,meanYOutV[0],rmsYOutV[0],sigmaNUM2);
  TString sigmaYSlopeOut    =  Form("diff_qwk_targetYSlope*1e6 >(%f + %f*%d) && diff_qwk_targetYSlope*1e6 <(%f + %f*%d)",meanYSlopeOutV[0],rmsYSlopeOutV[0],sigmaNUM1,meanYSlopeOutV[0],rmsYSlopeOutV[0],sigmaNUM2);
  TString sigmaEOut         =  Form("diff_qwk_energy >(%f + %f*%d) && diff_qwk_energy <(%f + %f*%d)",meanEOutV[0],rmsEOutV[0],sigmaNUM1,meanEOutV[0],rmsEOutV[0],sigmaNUM2);
  TString sigmaChargeOut    =  Form("asym_qwk_charge*1e6*1e3 >(%f + %f*%d) && asym_qwk_charge*1e6*1e3 <(%f + %f*%d)",meanChargeOutV[0],rmsChargeOutV[0],sigmaNUM1,meanChargeOutV[0],rmsChargeOutV[0],sigmaNUM2);
  printf("diff_qwk_targetX*1e3 >(%f + %f*%d) && diff_qwk_targetX*1e3 <(%f + %f*%d)\n",meanXInV[0],rmsXInV[0],sigmaNUM1,meanXInV[0],rmsXInV[0],sigmaNUM2);

  TString sigmaCutIn        =  Form("%s && %s && %s && %s && %s && %s"
				    ,sigmaXIn.Data(),sigmaXSlopeIn.Data(),sigmaYIn.Data(),sigmaYSlopeIn.Data(),sigmaEIn.Data(),sigmaChargeIn.Data());
  TString cutRegGlobErrorIn =  "regRawChainIn.regGlobErrorCode==0";
  TString regCutIn          =  Form("%s && %s",cutRegGlobErrorIn.Data(),sigmaCutIn.Data());
//   TString regCutIn          =  Form("%s ",cutRegGlobErrorIn.Data());

  TString sigmaCutOut        =  Form("%s && %s && %s && %s && %s && %s"
				    ,sigmaXOut.Data(),sigmaXSlopeOut.Data(),sigmaYOut.Data(),sigmaYSlopeOut.Data(),sigmaEOut.Data(),sigmaChargeOut.Data());
  TString cutRegGlobErrorOut =  "regRawChainOut.regGlobErrorCode==0";
  TString regCutOut          =  Form("%s && %s",cutRegGlobErrorOut.Data(),sigmaCutOut.Data());
//   TString regCutOut          =  Form("%s ",cutRegGlobErrorOut.Data());


  /***********************************************************/
  /***********************************************************/
  if(FIG_ASYM){

  TH1* histMDAsymIn;
  TH1* histMDAsymOut;

  TCanvas * Canvas11 = new TCanvas("Canvas11", title1,0,0,canvasSize[0],canvasSize[1]);
  Canvas11->Draw();
  Canvas11->SetBorderSize(0);
  Canvas11->cd();
  TPad*pad111 = new TPad("pad111","pad111",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad112 = new TPad("pad112","pad112",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad111->SetFillColor(kWhite);
  pad111->Draw();
  pad112->Draw();
  pad111->cd();
  TString text11 = Form("%s (%s, %s A): Regression-%s MD Asym., HWP-IN, %d to %d Sigma cut ",targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),sigmaNUM1,sigmaNUM2);
  TText * t11 = new TText(0.00,0.3,text11);
  t11->SetTextSize(0.45);
  t11->Draw();
  pad112->cd();
  pad112->Divide(3,3);

  TCanvas * Canvas12 = new TCanvas("Canvas12", title1,0,0,canvasSize[0],canvasSize[1]);
  Canvas12->Draw();
  Canvas12->SetBorderSize(0);
  Canvas12->cd();
  TPad*pad121 = new TPad("pad121","pad121",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad122 = new TPad("pad122","pad122",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad121->SetFillColor(kWhite);
  pad121->Draw();
  pad122->Draw();
  pad121->cd();
  TString text12 = Form("%s (%s, %s A): Regression-%s MD Asym., HWP-OUT, %d to %d Sigma cut ",targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),sigmaNUM1,sigmaNUM2);
  TText * t12 = new TText(0.00,0.3,text12);
  t12->SetTextSize(0.45);
  t12->Draw();
  pad122->cd();
  pad122->Divide(3,3);


  textMDInputIn.open(textMDOutputIn);
  textMDInputOut.open(textMDOutputOut);

  int di=0;
  for ( int i=0; i<8; i++) {
    di = i+1;

  pad112->cd(position1[i]);
  gPad->SetLogy();
//   rawChainIn->Draw(Form("asym_qwk_md%dbarsum*1e6>>hMDAsymIn%d",di,di),"ErrorFlag==0","");
  rawChainIn->Draw(Form("regRawChainIn.reg_asym_qwk_md%dbarsum*1e6>>hMDAsymIn%d",di,di),""+regCutIn+"","");
  histMDAsymIn = (TH1F *)gDirectory->Get(Form("hMDAsymIn%d",di));
  histMDAsymIn->SetLineColor(kBlue);
  histMDAsymIn->GetXaxis()->SetNdivisions(5,5,0);
  histMDAsymIn->GetYaxis()->SetNdivisions(5,5,0);
  histMDAsymIn->GetXaxis()->CenterTitle();
//   histMDAsymIn->GetYaxis()->CenterTitle();
//   histMDAsymIn->GetYaxis()->SetTitle("Arbitraray ");
  histMDAsymIn->GetXaxis()->SetTitle(Form("MD%d Asym. [ppm]",di));
  histMDAsymIn->SetTitle(Form(""));
  histMDAsymIn->Draw("");
  gPad->Update();


  pad122->cd(position1[i]);
  gPad->SetLogy();
//   rawChainOut->Draw(Form("asym_qwk_md%dbarsum*1e6>>hMDAsymOut%d",di,di),"ErrorFlag==0","");
  rawChainOut->Draw(Form("regRawChainOut.reg_asym_qwk_md%dbarsum*1e6>>hMDAsymOut%d",di,di),""+regCutOut+"","");
  histMDAsymOut = (TH1F *)gDirectory->Get(Form("hMDAsymOut%d",di));
  histMDAsymOut->SetLineColor(kRed);
  histMDAsymOut->GetXaxis()->SetNdivisions(5,5,0);
  histMDAsymOut->GetYaxis()->SetNdivisions(5,5,0);
  histMDAsymOut->GetXaxis()->CenterTitle();
//   histMDAsymOut->GetYaxis()->CenterTitle();
//   histMDAsymOut->GetYaxis()->SetTitle("Arbitraray ");
  histMDAsymOut->GetXaxis()->SetTitle(Form("MD%d Asym. [ppm]",di));
  histMDAsymOut->SetTitle(Form(""));
  histMDAsymOut->Draw("");
  gPad->Update();

  asymMDIn[i]       =  histMDAsymIn->GetMean();
  asymErrorMDIn[i]  =  histMDAsymIn->GetMeanError();
  rmsMDIn[i]        =  histMDAsymIn->GetRMS();

  asymMDOut[i]      =  histMDAsymOut->GetMean();
  asymErrorMDOut[i] =  histMDAsymOut->GetMeanError();
  rmsMDOut[i]       =  histMDAsymOut->GetRMS();


  textMDInputIn<<Form("%d %4.5f %4.5f %4.5f",di,asymMDIn[i],asymErrorMDIn[i],rmsMDIn[i])<<endl;
  textMDInputOut<<Form("%d %4.5f %4.5f %4.5f",di,asymMDOut[i],asymErrorMDOut[i],rmsMDOut[i])<<endl;

  printf("%s IN: MD %d Asym = %4.5f+-%4.5f %4.5f \n%s",blue,di,asymMDIn[i],asymErrorMDIn[i],rmsMDIn[i],normal);
  printf("%s OUT: MD %d Asym = %4.5f+-%4.5f %4.5f \n%s",red,di,asymMDOut[i],asymErrorMDOut[i],rmsMDOut[i],normal);

  }
  textMDInputIn.close();
  textMDInputOut.close();

  TString saveCutMDInPlot = Form("dirPlot/%s_%s_%s_%s_MD_%s_%s_IN_%d%d-sigma_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,reg_calc.Data(),reg_set.Data(),sigmaNUM1,sigmaNUM2,database_stem.Data());

  Canvas11->Update();
  Canvas11->Print(saveCutMDInPlot+".png");
  if(FIGURE){
    Canvas11->Print(saveCutMDInPlot+".svg");
    Canvas11->Print(saveCutMDInPlot+".C");
  }

  TString saveCutMDOutPlot = Form("dirPlot/%s_%s_%s_%s_MD_%s_%s_OUT_%d%d-sigma_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,reg_calc.Data(),reg_set.Data(),sigmaNUM1,sigmaNUM2,database_stem.Data());

  Canvas12->Update();
  Canvas12->Print(saveCutMDOutPlot+".png");
  if(FIGURE){
    Canvas12->Print(saveCutMDOutPlot+".svg");
    Canvas12->Print(saveCutMDOutPlot+".C");
  }

  /***********************************************************/

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
  showFit1 = "FIT = Am*cos(phi) - C*sin(phi) + D";
  showFit2 = "FIT = Am*cos(phi + phi0) + C";
  showFit3 = "FIT = Am*sin(phi + phi0) + C";

  TString title1;
  TString titleSummary = Form("%s (%s, %s A): Regression-%s averages of MD %s asymmetries."
			      ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),deviceTitle.Data());

  if(datopt==1){ title1= Form("%s %s",titleSummary.Data(),showFit1.Data());}
  else if(datopt==2){ title1= Form("%s %s",titleSummary.Data(),showFit2.Data());}
  else{
    title1= Form("%s %s",titleSummary.Data(),showFit3.Data());
  }

  TCanvas * Canvas22 = new TCanvas("canvas1", title1,0,0,canvasSize2[0],canvasSize2[1]);
  Canvas22->Draw();
  Canvas22->SetBorderSize(0);
  Canvas22->cd();
  TPad*pad221 = new TPad("pad221","pad221",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad222 = new TPad("pad222","pad222",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad221->SetFillColor(kWhite);
  pad221->Draw();
  pad222->Draw();
  pad221->cd();
  TString text = Form("%s",title1.Data());
  TText *t1 = new TText(0.00,0.3,text);
  t1->SetTextSize(0.45);
  t1->Draw();
  pad222->cd();


  const int k =8;
  //  Double_t valuesin[k];Double_t errorsin[k];Double_t valuesout[k];Double_t errorsout[k];
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

  Double_t zero[k];

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
    valuesum[i]=(asymMDIn[i]+asymMDOut[i])/2.0;
    valueerror[i]= (sqrt(pow(asymErrorMDIn[i],2)+pow(asymErrorMDOut[i],2)))/2.0;
    valuediff[i]=((asymMDIn[i]/pow(asymErrorMDIn[i],2)) - (asymMDOut[i]/pow(asymErrorMDOut[i],2))) /((1/pow(asymErrorMDIn[i],2)) + (1/pow(asymErrorMDOut[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(asymErrorMDIn[i],2)))+(1/pow(asymErrorMDOut[i],2))));


  }

  /********************************************/

  for(Int_t i =0;i<4;i++){
    valuesumopp[i]= valuesum[i]+valuesum[i+4];
    errorsumopp[i]= sqrt(pow(valueerror[i],2)+pow(valueerror[i+4],2));
    valuediffopp[i]=valuediff[i]-valuediff[i+4];
    errordiffopp[i]=sqrt(pow(errordiff[i],2)+pow(errordiff[i+4],2));

  }


  // Draw IN values
  TGraphErrors* grp_in  = new TGraphErrors(k,x,asymMDIn,zero,asymErrorMDIn);
  grp_in ->SetName("grp_in");
  grp_in ->Draw("goff");
  grp_in ->SetMarkerSize(markerSize[0]);
  grp_in ->SetMarkerStyle(20);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("fit_in","B");
  TF1* fit1 = grp_in->GetFunction("fit_in");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
  fit1->SetLineStyle(4);
  fit1->SetLineWidth(2);

  // Draw OUT values
  TGraphErrors* grp_out  = new TGraphErrors(k,x,asymMDOut,zero,asymErrorMDOut);
  grp_out ->SetName("grp_out");
  grp_out ->Draw("goff");
  grp_out ->SetMarkerSize(markerSize[0]);
  grp_out ->SetMarkerStyle(24);
  grp_out ->SetMarkerColor(kRed);


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

  grp_out->Fit("fit_out","B");
  TF1* fit2 = grp_out->GetFunction("fit_out");
  fit2->DrawCopy("same");
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
  grp_sum->Fit("pol0","B");
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
  grp_diff->Fit("fit_in","B");
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

  fit3->DrawCopy("same");
  fit4->DrawCopy("same");


  fit1->SetParNames("A_{M}","#phi_{0}","C");
  fit2->SetParNames("A_{M}","#phi_{0}","C");
  fit3->SetParNames("C");
  fit4->SetParNames("A_{M}","#phi_{0}","C");

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats4 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");


  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 

  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
 
  stats3->SetTextColor(kGreen-2);
  stats3->SetFillColor(kWhite); 

  stats4->SetTextColor(kMagenta+1);
  stats4->SetFillColor(kWhite); 

  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.72);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.47);stats2->SetY2NDC(0.70);
  stats3->SetX1NDC(0.8); stats3->SetX2NDC(0.99); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);
  stats4->SetX1NDC(0.8); stats4->SetX2NDC(0.99); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);


  Double_t p0in     =  fit1->GetParameter(0);
  Double_t ep0in    =  fit1->GetParError(0);
//   Double_t p1in     =  fit1->GetParameter(1);
//   Double_t ep1in    =  fit1->GetParError(1);
//   Double_t p2in     =  fit1->GetParameter(2);
//   Double_t ep2in    =  fit1->GetParError(2);
//   Double_t Chiin    =  fit1->GetChisquare();
//   Double_t NDFin    =  fit1->GetNDF();
//   Double_t Probin   =  fit1->GetProb();

  Double_t p0out    =  fit2->GetParameter(0);
  Double_t ep0out   =  fit2->GetParError(0);
//   Double_t p1out    =  fit2->GetParameter(1);
//   Double_t ep1out   =  fit2->GetParError(1);
//   Double_t p2out    =  fit2->GetParameter(2);
//   Double_t ep2out   =  fit2->GetParError(2);
//   Double_t Chiout   =  fit2->GetChisquare();
//   Double_t NDFout   =  fit2->GetNDF();
//   Double_t Probout  =  fit2->GetProb();

  Double_t p0sum    =  fit3->GetParameter(0);
  Double_t ep0sum   =  fit3->GetParError(0);
//   Double_t Chisum   =  fit3->GetChisquare();
//   Double_t NDFsum   =  fit3->GetNDF();
//   Double_t Probsum  =  fit3->GetProb();

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
  legend->AddEntry(grp_in,  Form("A_{IHWP-IN} = %4.2f #pm %4.2f ppm",p0in,ep0in), "p");
  legend->AddEntry(grp_sum, Form("A_{(IN+OUT)/2} = %4.2f #pm %4.2f ppm",p0sum,ep0sum), "p");
  legend->AddEntry(grp_out, Form("A_{IHWP-OUT} = %4.2f #pm %4.2f ppm",p0out,ep0out), "p");
  legend->AddEntry(grp_diff,Form("A_{measured} = %4.2f #pm %4.2f ppm",p0diff,ep0diff), "p");
  legend->SetFillColor(0);
  legend->SetBorderSize(2);
  legend->SetTextSize(0.035);
  legend->Draw("");


  gPad->Update();

  Myfile3.open(textfile3);
  Myfile3<<Form("%4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.0f %4.4f",p0diff,ep0diff,p1diff,ep1diff,p2diff,ep2diff,Chidiff,NDFdiff,Probdiff)<<endl;
  Myfile3.close();


  TString saveSummaryPlot = Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_%s_%d%d-sigma_cut_dependence_plots_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),sigmaNUM1,sigmaNUM2,database_stem.Data());

  Canvas22->Update();
  Canvas22->Print(saveSummaryPlot+".png");
  if(FIGURE){
    Canvas22->Print(saveSummaryPlot+".svg");
    Canvas22->Print(saveSummaryPlot+".C");
  }





  }

  /***********************************************************/
  /***********************************************************/
  /***********************************************************/
  /***********************************************************/

  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}

