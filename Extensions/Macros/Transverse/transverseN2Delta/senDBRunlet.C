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

int main(Int_t argc,Char_t* argv[]){

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  Bool_t SCALE = 0;
  Bool_t FIGURE = 0;


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
  Int_t datopt = 2;
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
  Int_t canvasSize[2] ={1200,650};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.900,0.75,0.995};
  Double_t yScale[2] = {-10.0,10.0};


  Double_t x_lo_stat_in4,y_lo_stat_in4,x_hi_stat_in4,y_hi_stat_in4,x_lo_stat_out4,y_lo_stat_out4,x_hi_stat_out4,y_hi_stat_out4;
  x_lo_stat_in4=0.76;y_lo_stat_in4=0.64;x_hi_stat_in4=0.99;y_hi_stat_in4=0.95;
  x_lo_stat_out4=0.76;y_lo_stat_out4=0.30;x_hi_stat_out4=0.99;y_hi_stat_out4=0.61;
  Double_t x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4;
  x_lo_leg4=0.76;y_lo_leg4=0.10;x_hi_leg4=0.99;y_hi_leg4=0.31;

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

  TString senType,senTitle,senUnit;
  Double_t unitScale = 0;
  Int_t sen_opt = 1;
  std::cout<<Form("Enter sensitivity type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. %sX (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("2. Y")<<std::endl;
  std::cout<<Form("3. XSlope")<<std::endl;
  std::cout<<Form("4. YSlope")<<std::endl;
  std::cout<<Form("5. E")<<std::endl;
  std::cout<<Form("6. Charge")<<std::endl;
  std::string input_sen_opt;
  std::getline( std::cin, input_sen_opt );
  if ( !input_sen_opt.empty() ) {
    std::istringstream stream( input_sen_opt );
    stream >> sen_opt;
  }
  if(sen_opt == 1){senType = "X";      senTitle = "X";   senUnit = "ppb/nm";     YRange[0]=-14; YRange[1]=14;       unitScale=1e+3;}
  if(sen_opt == 2){senType = "Y";      senTitle = "Y";   senUnit = "ppb/nm";     YRange[0]=-14; YRange[1]=14;       unitScale=1e+3;}
  if(sen_opt == 3){senType = "XSlope"; senTitle = "X'";  senUnit = "ppb/nrad";   YRange[0]=-250; YRange[1]=250;   unitScale=1e-0;}
  if(sen_opt == 4){senType = "YSlope"; senTitle = "Y'";  senUnit = "ppb/nrad";   YRange[0]=-250; YRange[1]=250;   unitScale=1e-0;}
  if(sen_opt == 5){senType = "E";      senTitle = "E";   senUnit = "ppb/ppb";    YRange[0]=-25; YRange[1]=25;         unitScale=1e-0;}
  if(sen_opt == 6){senType = "Charge"; senTitle = "A_{Q}";   senUnit = "ppb/ppb";    YRange[0]=-0.1; YRange[1]=0.1;   unitScale=1e-0;}
  if(sen_opt  > 6){printf("%sWrong choise of sensitivity \n%s",red,normal); exit(0);}



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
  showFit1 = Form("FIT = p_{1} cos(#phi) - (#partialA/#partial%s)*sin(#phi) + D",senType.Data());
  showFit2 = Form("FIT = p_{1} cos(#phi + #phi_{0}) + (#partialA/#partial%s)",senType.Data());
  showFit3 = Form("FIT = p_{1} sin(#phi + #phi_{0}) + (#partialA/#partial%s)",senType.Data());

//   int position1[8] = { 4, 1, 2, 3, 6, 9, 8, 7 };

  Int_t counter11=0;
  ifstream runletSen;
  const Int_t NUM11 = 500;
  Double_t octant11[NUM11],run_number11[NUM11],sen11[NUM11],senError11[NUM11],senRMS11[NUM11];
  TString hwp11[NUM11];

  std::vector <Double_t> run1in,run2in,run3in,run4in,run5in,run6in,run7in,run8in;
  std::vector <Double_t> run1out,run2out,run3out,run4out,run5out,run6out,run7out,run8out;
  std::vector <Double_t> sen1in,sen2in,sen3in,sen4in,sen5in,sen6in,sen7in,sen8in;
  std::vector <Double_t> sen1out,sen2out,sen3out,sen4out,sen5out,sen6out,sen7out,sen8out;
  std::vector <Double_t> sen1Erin,sen2Erin,sen3Erin,sen4Erin,sen5Erin,sen6Erin,sen7Erin,sen8Erin;
  std::vector <Double_t> sen1Erout,sen2Erout,sen3Erout,sen4Erout,sen5Erout,sen6Erout,sen7Erout,sen8Erout;
  std::vector <Double_t> myzero;

  const Int_t det = 8;
  Double_t calc_SenInp0[det],calc_eSenInp0[det],calc_SenInChisquare[det],calc_SenInNDF[det];
  Double_t calc_SenOutp0[det],calc_eSenOutp0[det],calc_SenOutChisquare[det],calc_SenOutNDF[det];


  runletSen.open(Form("dirPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_%s_sen%s_%s.txt"
			     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
		      ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),outputAvg.Data(),senType.Data(),database_stem.Data()));

   if (runletSen.is_open()) {
   
    while(1) {
      //       zero11[counter11]=0;
      
      runletSen >> octant11[counter11] >> run_number11[counter11] >> hwp11[counter11]
		   >> sen11[counter11]   >> senError11[counter11]  >> senRMS11[counter11] ;
      
      if (!runletSen.good()) break;
      
      if(hwp11[counter11]== "in"){
	
	if(octant11[counter11]==1) {
	  run1in.push_back(run_number11[counter11]);
	  sen1in.push_back(sen11[counter11]*unitScale);          //Coverting units to ppb/nm. By default its ppm/nm.
	  sen1Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==2) {
	  run2in.push_back(run_number11[counter11]);
	  sen2in.push_back(sen11[counter11]*unitScale); 
	  sen2Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==3) {
	  run3in.push_back(run_number11[counter11]);
	  sen3in.push_back(sen11[counter11]*unitScale); 
	  sen3Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==4) {
	  run4in.push_back(run_number11[counter11]);
	  sen4in.push_back(sen11[counter11]*unitScale); 
	  sen4Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==5) {
	  run5in.push_back(run_number11[counter11]);
	  sen5in.push_back(sen11[counter11]*unitScale); 
	  sen5Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==6) {
	  run6in.push_back(run_number11[counter11]);
	  sen6in.push_back(sen11[counter11]*unitScale);
	  sen6Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==7) {
	  run7in.push_back(run_number11[counter11]);
	  sen7in.push_back(sen11[counter11]*unitScale);
	  sen7Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==8) {
	  run8in.push_back(run_number11[counter11]);
	  sen8in.push_back(sen11[counter11]*unitScale);
	  sen8Erin.push_back(senError11[counter11]*unitScale);
	  myzero.push_back(0.0);
	}
	
	
      }



      if(hwp11[counter11]== "out"){
	
	if(octant11[counter11]==1) {
	  run1out.push_back(run_number11[counter11]);
	  sen1out.push_back(sen11[counter11]*unitScale);
	  sen1Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==2) {
	  run2out.push_back(run_number11[counter11]);
	  sen2out.push_back(sen11[counter11]*unitScale);
	  sen2Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==3) {
	  run3out.push_back(run_number11[counter11]);
	  sen3out.push_back(sen11[counter11]*unitScale);
	  sen3Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==4) {
	  run4out.push_back(run_number11[counter11]);
	  sen4out.push_back(sen11[counter11]*unitScale);
	  sen4Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==5) {
	  run5out.push_back(run_number11[counter11]);
	  sen5out.push_back(sen11[counter11]*unitScale);
	  sen5Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==6) {
	  run6out.push_back(run_number11[counter11]);
	  sen6out.push_back(sen11[counter11]*unitScale);
	  sen6Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==7) {
	  run7out.push_back(run_number11[counter11]);
	  sen7out.push_back(sen11[counter11]*unitScale);
	  sen7Erout.push_back(senError11[counter11]*unitScale);
	}
	if(octant11[counter11]==8) {
	  run8out.push_back(run_number11[counter11]);
	  sen8out.push_back(sen11[counter11]*unitScale);
	  sen8Erout.push_back(senError11[counter11]*unitScale);
	}
	
	
      }
      //       else{ cout<<green<<"Check Half Wave Plate settings "<<normal<<endl;}

      
      counter11++;
    }
   }
   else {
     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
     return -1;
   }
   runletSen.close();

//   printf("%s Checking %d\n%s",green,__LINE__,normal);


  TString title11;
  TString titleInOut = Form("%s (%s, %s A): Regression-%s %s %s Sensitivity w.r.t %s."
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data(),senType.Data());

  if(datopt==1) title11 = Form("%s %s",titleInOut.Data(),showFit1.Data());
  else if(datopt==2) title11 = Form("%s %s",titleInOut.Data(),showFit2.Data());
  else title11 = Form("%s %s",titleInOut.Data(),showFit3.Data());

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

  TCanvas * c1 = new TCanvas("canvas1", title11,0,0,canvasSize[0],canvasSize[1]);
  c1->Draw();
  c1->SetBorderSize(0);
  c1->cd();
  TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  TLatex * t1 = new TLatex(0.06,0.3,Form("%s",title11.Data()));
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();



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
  pad112->Divide(3,3);


//   TCanvas * c1 = new TCanvas("canvas1", title11,0,0,canvasSize[0],canvasSize[1]);
//   c1->Draw();
//   c1->SetBorderSize(0);
//   c1->cd();
//   TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
//   TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
//   pad1->SetFillColor(kWhite);
//   pad1->Draw();
//   pad2->Draw();
//   pad1->cd();
//   TString text = Form("%s",title11.Data());
//   t1 = new TText(0.06,0.3,text);
//   t1->SetTextSize(0.5);
//   t1->Draw();
//   pad2->cd();



//   TCanvas * c11 = new TCanvas("c11", titleInOut,0,0,1600,1000);
//   c11->Draw();
//   c11->SetBorderSize(0);
//   c11->cd();
//   TPad*pad111 = new TPad("pad111","pad111",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
//   TPad*pad112 = new TPad("pad112","pad112",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
//   pad111->SetFillColor(kWhite);
//   pad111->Draw();
//   pad112->Draw();
//   pad111->cd();
//   TString text11 = Form("%s",titleInOut.Data());
//   TText * t11 = new TText(0.00,0.3,text11);
//   t11->SetTextSize(0.45);
//   t11->Draw();
//   pad112->cd();
//   pad112->Divide(3,3);

//     di = i+1;

//   pad112->cd(position1[i]);
//   pad112->cd(1);


  Double_t dummyINaxis[(Int_t)run1in.size()];
  Double_t dummyOUTaxis[(Int_t)run1out.size()];
  Int_t dummyArraySize = (Int_t)run1in.size() > (Int_t)run1out.size() ? (Int_t)run1in.size() : (Int_t)run1out.size();
  TString xAxisTitle = "Runlets";

  for(Int_t j = 0; j< dummyArraySize; j++) {
    dummyINaxis[j-1] = j;
    dummyOUTaxis[j-1] = j + dummyArraySize;
//     cout<<j<<"\t"<<dummyOUTaxis[j-1]<<"\t"<<dummyINaxis[j-1]<<endl;
  }
//   for(Int_t j = 0; j< run1in.size(); j++) {
//     dummyINaxis[j] = j;
//     dummyOUTaxis[j] = j + run1in.size();
//     cout<<dummyINaxis[j]<<endl;
//   }
//   for(Int_t j = 0; j< run1out.size(); j++) {
//     dummyOUTaxis[j] = j + run1in.size();
//     cout<<dummyOUTaxis[j]<<endl;
//   }


  cout<<"run1in: "<<run1in.size()<<endl;
  for ( Int_t i=0; i<run1in.size(); i++) {
    cout<<Form("i:%d\trun1:%5.2f\tsen1IN11:%f",i,run1in[i],sen1in[i])<<endl;
  }
  cout<<"run1out: "<<run1out.size()<<endl;
  for ( Int_t i=0; i<run1out.size(); i++) {
    cout<<Form("i:%d\trun1:%5.2f\tsen1OUT11:%f",i,run1out[i],sen1out[i])<<endl;
  }
  cout<<"dummyArraySize:"<< dummyArraySize<<endl;


//   TF1* fitfunIn  = new TF1("fitfunIn","pol0",dummyINaxis[0],dummyINaxis[(Int_t)run1in.size()]);
//   TF1* fitfunIn  = new TF1("fitfunIn","pol0",-0.7,(Int_t)run1in.size());
//   TF1* fitfunOut = new TF1("fitfunOut","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);

  TF1* fitfun1In  = new TF1("fitfun1In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun1Out = new TF1("fitfun1Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun2In  = new TF1("fitfun2In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun2Out = new TF1("fitfun2Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun3In  = new TF1("fitfun3In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun3Out = new TF1("fitfun3Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun4In  = new TF1("fitfun4In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun4Out = new TF1("fitfun4Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun5In  = new TF1("fitfun5In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun5Out = new TF1("fitfun5Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun6In  = new TF1("fitfun6In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun6Out = new TF1("fitfun6Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun7In  = new TF1("fitfun7In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun7Out = new TF1("fitfun7Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun8In  = new TF1("fitfun8In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun8Out = new TF1("fitfun8Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);


  fitfun1In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun1Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun2In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun2Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun3In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun3Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun4In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun4Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun5In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun5Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun6In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun6Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun7In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun7Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
  fitfun8In->SetParNames(Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fitfun8Out->SetParNames(Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));


  pad112->cd(4);  
  //TGraphErrors * sen1GraphIN = new TGraphErrors(counter11,run_number1IN11,sen1IN11,zero11,senError1IN11);
  //TGraphErrors * sen1GraphIN = new TGraphErrors((Int_t)run1in.size(),run1in.data(),sen1in.data(),myzero.data(),sen1Erin.data());
  TGraphErrors * sen1GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen1in.data(),myzero.data(),sen1Erin.data());
  sen1GraphIN->SetName("sen1GraphIN");
  sen1GraphIN->SetMarkerColor(kBlue);
  sen1GraphIN->SetLineColor(kBlue);
  sen1GraphIN->SetMarkerStyle(20);
  sen1GraphIN->SetMarkerSize(markerSize[0]);
  sen1GraphIN->SetLineWidth(1);
  //   sen1GraphIN->Draw("AP");
  sen1GraphIN->Fit("fitfun1In","E M R F 0 Q");
  fitfun1In->SetLineStyle(1);
  fitfun1In->SetLineWidth(2);
  fitfun1In->SetLineColor(kBlue);
  fitfun1In->SetLineStyle(4);
  fitfun1In->SetLineWidth(2);


  //TGraphErrors * sen1GraphOUT = new TGraphErrors((Int_t)run1out.size(),run1out.data(),sen1out.data(),myzero.data(),sen1Erout.data());
  TGraphErrors * sen1GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen1out.data(),myzero.data(),sen1Erout.data());
//   TGraphErrors * sen1GraphOUT = new TGraphErrors(counter11,dummyOUT,sen1OUT11,zero11,sen1Error1OUT11);
  sen1GraphOUT->SetName("sen1GraphOUT");
  sen1GraphOUT->SetMarkerColor(kRed);
  sen1GraphOUT->SetLineColor(kRed);
  sen1GraphOUT->SetMarkerStyle(24);
  sen1GraphOUT->SetMarkerSize(markerSize[0]);
  sen1GraphOUT->SetLineWidth(1);
//   sen1GraphOUT->Draw("AP");
  sen1GraphOUT->Fit("fitfun1Out","E M R F 0 Q");
  fitfun1Out->SetLineStyle(1);
  fitfun1Out->SetLineWidth(2);
  fitfun1Out->SetLineColor(kRed);
  fitfun1Out->SetLineStyle(5);
  fitfun1Out->SetLineWidth(2);

  TMultiGraph *sen1Graph = new TMultiGraph();
  sen1Graph->Add(sen1GraphIN);
  sen1Graph->Add(sen1GraphOUT);
  sen1Graph->Draw("AP");
  sen1Graph->SetTitle("");
//   sen1Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen1Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen1Graph->GetYaxis()->SetTitle(Form("%s1 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen1Graph->GetXaxis()->CenterTitle();
  sen1Graph->GetYaxis()->CenterTitle();
//   sen1Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen1Graph->GetXaxis()->SetLabelColor(0);
  sen1Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);
//   TAxis *xaxisSen1Graph= sen1Graph->GetXaxis();
//   xaxisSen1Graph->SetLimits(16066.00,16066.14);
//   pad112->Range(16064.77,-30.66847,16067.63,47.58539);

  fitfun1In->Draw("same");
  fitfun1Out->Draw("same");


//   TAxis *xaxisSen1Graph= sen1Graph->GetXaxis();
//   xaxisSen1Graph->SetLimits(run_number1IN11[0],run_number1IN11[0]+30);
  
  TPaveStats *statsSen1In =(TPaveStats*)sen1GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen1Out =(TPaveStats*)sen1GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen1In->SetTextColor(kBlue);
  statsSen1In->SetFillColor(kWhite); 
  statsSen1Out->SetTextColor(kRed);
  statsSen1Out->SetFillColor(kWhite);
  statsSen1In->SetTextSize(0.045);
  statsSen1Out->SetTextSize(0.045);
  statsSen1In->SetX1NDC(x_lo_stat_in4);    statsSen1In->SetX2NDC(x_hi_stat_in4); 
  statsSen1In->SetY1NDC(y_lo_stat_in4);    statsSen1In->SetY2NDC(y_hi_stat_in4);
  statsSen1Out->SetX1NDC(x_lo_stat_out4);  statsSen1Out->SetX2NDC(x_hi_stat_out4); 
  statsSen1Out->SetY1NDC(y_lo_stat_out4);  statsSen1Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[0]         =   fitfun1In->GetParameter(0);
  calc_eSenInp0[0]        =   fitfun1In->GetParError(0);
  calc_SenInChisquare[0]  =   fitfun1In->GetChisquare();
  calc_SenInNDF[0]        =   fitfun1In->GetNDF();

  calc_SenOutp0[0]        =   fitfun1Out->GetParameter(0);
  calc_eSenOutp0[0]       =   fitfun1Out->GetParError(0);
  calc_SenOutChisquare[0] =   fitfun1Out->GetChisquare();
  calc_SenOutNDF[0]       =   fitfun1Out->GetNDF();

//   TLegend *legRunSen1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
//   legRunSen1->AddEntry("sen1GraphIN",  Form("A_{IN}= %4.2f#pm%4.2f [ppm]",calc_Sen1Inp0,calc_eSen1Inp0),"lp");
//   legRunSen1->AddEntry("sen1GraphOUT",  Form("A_{OUT}= %4.2f#pm%4.2f [ppm]",calc_Sen1Outp0,calc_eSen1Outp0),"lp");
//   legRunSen1->SetTextSize(0.042);
//   legRunSen1->SetFillColor(0);
//   legRunSen1->SetBorderSize(2);
//   legRunSen1->Draw();


//   TLatex* tSenIn = new TLatex(5,-40,Form("%5.0f",run1in[0]));
  TLatex* tSenIn = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tSenIn->SetTextSize(0.06);
  tSenIn->SetTextColor(kBlue);
  tSenIn->Draw();
//   TLatex* tSenOut = new TLatex(17,-40,Form("%5.0f",run1out[0]));
  TLatex* tSenOut = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tSenOut->SetTextSize(0.06);
  tSenOut->SetTextColor(kRed);
  tSenOut->Draw();




  pad112->cd(1);  
  TGraphErrors * sen2GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen2in.data(),myzero.data(),sen2Erin.data());
  sen2GraphIN->SetName("sen2GraphIN");
  sen2GraphIN->SetMarkerColor(kBlue);
  sen2GraphIN->SetLineColor(kBlue);
  sen2GraphIN->SetMarkerStyle(20);
  sen2GraphIN->SetMarkerSize(markerSize[0]);
  sen2GraphIN->SetLineWidth(1);
  sen2GraphIN->Fit("fitfun2In","E M R F 0 Q");
  fitfun2In->SetLineStyle(1);
  fitfun2In->SetLineWidth(2);
  fitfun2In->SetLineColor(kBlue);
  fitfun2In->SetLineStyle(4);
  fitfun2In->SetLineWidth(2);

  TGraphErrors * sen2GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen2out.data(),myzero.data(),sen2Erout.data());
  sen2GraphOUT->SetName("sen2GraphOUT");
  sen2GraphOUT->SetMarkerColor(kRed);
  sen2GraphOUT->SetLineColor(kRed);
  sen2GraphOUT->SetMarkerStyle(24);
  sen2GraphOUT->SetMarkerSize(markerSize[0]);
  sen2GraphOUT->SetLineWidth(1);
  sen2GraphOUT->Fit("fitfun2Out","E M R F 0 Q");
  fitfun2Out->SetLineStyle(1);
  fitfun2Out->SetLineWidth(2);
  fitfun2Out->SetLineColor(kRed);
  fitfun2Out->SetLineStyle(5);
  fitfun2Out->SetLineWidth(2);

  TMultiGraph *sen2Graph = new TMultiGraph();
  sen2Graph->Add(sen2GraphIN);
  sen2Graph->Add(sen2GraphOUT);
  sen2Graph->Draw("AP");
  sen2Graph->SetTitle("");
//   sen2Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen2Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen2Graph->GetYaxis()->SetTitle(Form("%s2 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen2Graph->GetXaxis()->CenterTitle();
  sen2Graph->GetYaxis()->CenterTitle();
//   sen2Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen2Graph->GetXaxis()->SetLabelColor(0);
  sen2Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun2In->Draw("same");
  fitfun2Out->Draw("same");

  TPaveStats *statsSen2In =(TPaveStats*)sen2GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen2Out =(TPaveStats*)sen2GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen2In->SetTextColor(kBlue);
  statsSen2In->SetFillColor(kWhite); 
  statsSen2Out->SetTextColor(kRed);
  statsSen2Out->SetFillColor(kWhite);
  statsSen2In->SetTextSize(0.045);
  statsSen2Out->SetTextSize(0.045);
  statsSen2In->SetX1NDC(x_lo_stat_in4);    statsSen2In->SetX2NDC(x_hi_stat_in4); 
  statsSen2In->SetY1NDC(y_lo_stat_in4);    statsSen2In->SetY2NDC(y_hi_stat_in4);
  statsSen2Out->SetX1NDC(x_lo_stat_out4);  statsSen2Out->SetX2NDC(x_hi_stat_out4); 
  statsSen2Out->SetY1NDC(y_lo_stat_out4);  statsSen2Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[1]         =   fitfun2In->GetParameter(0);
  calc_eSenInp0[1]        =   fitfun2In->GetParError(0);
  calc_SenInChisquare[1]  =   fitfun2In->GetChisquare();
  calc_SenInNDF[1]        =   fitfun2In->GetNDF();

  calc_SenOutp0[1]        =   fitfun2Out->GetParameter(0);
  calc_eSenOutp0[1]       =   fitfun2Out->GetParError(0);
  calc_SenOutChisquare[1] =   fitfun2Out->GetChisquare();
  calc_SenOutNDF[1]       =   fitfun2Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();



  pad112->cd(2);  
  TGraphErrors * sen3GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen3in.data(),myzero.data(),sen3Erin.data());
  sen3GraphIN->SetName("sen3GraphIN");
  sen3GraphIN->SetMarkerColor(kBlue);
  sen3GraphIN->SetLineColor(kBlue);
  sen3GraphIN->SetMarkerStyle(20);
  sen3GraphIN->SetMarkerSize(markerSize[0]);
  sen3GraphIN->SetLineWidth(1);
  sen3GraphIN->Fit("fitfun3In","E M R F 0 Q");
  fitfun3In->SetLineStyle(1);
  fitfun3In->SetLineWidth(2);
  fitfun3In->SetLineColor(kBlue);
  fitfun3In->SetLineStyle(4);
  fitfun3In->SetLineWidth(2);

  TGraphErrors * sen3GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen3out.data(),myzero.data(),sen3Erout.data());
  sen3GraphOUT->SetName("sen3GraphOUT");
  sen3GraphOUT->SetMarkerColor(kRed);
  sen3GraphOUT->SetLineColor(kRed);
  sen3GraphOUT->SetMarkerStyle(24);
  sen3GraphOUT->SetMarkerSize(markerSize[0]);
  sen3GraphOUT->SetLineWidth(1);
  sen3GraphOUT->Fit("fitfun3Out","E M R F 0 Q");
  fitfun3Out->SetLineStyle(1);
  fitfun3Out->SetLineWidth(2);
  fitfun3Out->SetLineColor(kRed);
  fitfun3Out->SetLineStyle(5);
  fitfun3Out->SetLineWidth(2);


  TMultiGraph *sen3Graph = new TMultiGraph();
  sen3Graph->Add(sen3GraphIN);
  sen3Graph->Add(sen3GraphOUT);
  sen3Graph->Draw("AP");
  sen3Graph->SetTitle("");
//   sen3Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen3Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen3Graph->GetYaxis()->SetTitle(Form("%s3 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen3Graph->GetXaxis()->CenterTitle();
  sen3Graph->GetYaxis()->CenterTitle();
//   sen3Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen3Graph->GetXaxis()->SetLabelColor(0);
  sen3Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun3In->Draw("same");
  fitfun3Out->Draw("same");

  TPaveStats *statsSen3In =(TPaveStats*)sen3GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen3Out =(TPaveStats*)sen3GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen3In->SetTextColor(kBlue);
  statsSen3In->SetFillColor(kWhite); 
  statsSen3Out->SetTextColor(kRed);
  statsSen3Out->SetFillColor(kWhite);
  statsSen3In->SetTextSize(0.045);
  statsSen3Out->SetTextSize(0.045);
  statsSen3In->SetX1NDC(x_lo_stat_in4);    statsSen3In->SetX2NDC(x_hi_stat_in4); 
  statsSen3In->SetY1NDC(y_lo_stat_in4);    statsSen3In->SetY2NDC(y_hi_stat_in4);
  statsSen3Out->SetX1NDC(x_lo_stat_out4);  statsSen3Out->SetX2NDC(x_hi_stat_out4); 
  statsSen3Out->SetY1NDC(y_lo_stat_out4);  statsSen3Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[2]         =   fitfun3In->GetParameter(0);
  calc_eSenInp0[2]        =   fitfun3In->GetParError(0);
  calc_SenInChisquare[2]  =   fitfun3In->GetChisquare();
  calc_SenInNDF[2]        =   fitfun3In->GetNDF();

  calc_SenOutp0[2]        =   fitfun3Out->GetParameter(0);
  calc_eSenOutp0[2]       =   fitfun3Out->GetParError(0);
  calc_SenOutChisquare[2] =   fitfun3Out->GetChisquare();
  calc_SenOutNDF[2]       =   fitfun3Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();


  pad112->cd(3);  
  TGraphErrors * sen4GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen4in.data(),myzero.data(),sen4Erin.data());
  sen4GraphIN->SetName("sen4GraphIN");
  sen4GraphIN->SetMarkerColor(kBlue);
  sen4GraphIN->SetLineColor(kBlue);
  sen4GraphIN->SetMarkerStyle(20);
  sen4GraphIN->SetMarkerSize(markerSize[0]);
  sen4GraphIN->SetLineWidth(1);
  sen4GraphIN->Fit("fitfun4In","E M R F 0 Q");
  fitfun4In->SetLineStyle(1);
  fitfun4In->SetLineWidth(2);
  fitfun4In->SetLineColor(kBlue);
  fitfun4In->SetLineStyle(4);
  fitfun4In->SetLineWidth(2);


  TGraphErrors * sen4GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen4out.data(),myzero.data(),sen4Erout.data());
  sen4GraphOUT->SetName("sen4GraphOUT");
  sen4GraphOUT->SetMarkerColor(kRed);
  sen4GraphOUT->SetLineColor(kRed);
  sen4GraphOUT->SetMarkerStyle(24);
  sen4GraphOUT->SetMarkerSize(markerSize[0]);
  sen4GraphOUT->SetLineWidth(1);
  sen4GraphOUT->Fit("fitfun4Out","E M R F 0 Q");
  fitfun4Out->SetLineStyle(1);
  fitfun4Out->SetLineWidth(2);
  fitfun4Out->SetLineColor(kRed);
  fitfun4Out->SetLineStyle(5);
  fitfun4Out->SetLineWidth(2);


  TMultiGraph *sen4Graph = new TMultiGraph();
  sen4Graph->Add(sen4GraphIN);
  sen4Graph->Add(sen4GraphOUT);
  sen4Graph->Draw("AP");
  sen4Graph->SetTitle("");
//   sen4Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen4Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen4Graph->GetYaxis()->SetTitle(Form("%s4 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen4Graph->GetXaxis()->CenterTitle();
  sen4Graph->GetYaxis()->CenterTitle();
//   sen4Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen4Graph->GetXaxis()->SetLabelColor(0);
  sen4Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun4In->Draw("same");
  fitfun4Out->Draw("same");

  TPaveStats *statsSen4In =(TPaveStats*)sen4GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen4Out =(TPaveStats*)sen4GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen4In->SetTextColor(kBlue);
  statsSen4In->SetFillColor(kWhite); 
  statsSen4Out->SetTextColor(kRed);
  statsSen4Out->SetFillColor(kWhite);
  statsSen4In->SetTextSize(0.045);
  statsSen4Out->SetTextSize(0.045);
  statsSen4In->SetX1NDC(x_lo_stat_in4);    statsSen4In->SetX2NDC(x_hi_stat_in4); 
  statsSen4In->SetY1NDC(y_lo_stat_in4);    statsSen4In->SetY2NDC(y_hi_stat_in4);
  statsSen4Out->SetX1NDC(x_lo_stat_out4);  statsSen4Out->SetX2NDC(x_hi_stat_out4); 
  statsSen4Out->SetY1NDC(y_lo_stat_out4);  statsSen4Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[3]         =   fitfun4In->GetParameter(0);
  calc_eSenInp0[3]        =   fitfun4In->GetParError(0);
  calc_SenInChisquare[3]  =   fitfun4In->GetChisquare();
  calc_SenInNDF[3]        =   fitfun4In->GetNDF();

  calc_SenOutp0[3]        =   fitfun4Out->GetParameter(0);
  calc_eSenOutp0[3]       =   fitfun4Out->GetParError(0);
  calc_SenOutChisquare[3] =   fitfun4Out->GetChisquare();
  calc_SenOutNDF[3]       =   fitfun4Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();


  pad112->cd(6);  
  TGraphErrors * sen5GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen5in.data(),myzero.data(),sen5Erin.data());
  sen5GraphIN->SetName("sen5GraphIN");
  sen5GraphIN->SetMarkerColor(kBlue);
  sen5GraphIN->SetLineColor(kBlue);
  sen5GraphIN->SetMarkerStyle(20);
  sen5GraphIN->SetMarkerSize(markerSize[0]);
  sen5GraphIN->SetLineWidth(1);
  sen5GraphIN->Fit("fitfun5In","E M R F 0 Q");
  fitfun5In->SetLineStyle(1);
  fitfun5In->SetLineWidth(2);
  fitfun5In->SetLineColor(kBlue);
  fitfun5In->SetLineStyle(4);
  fitfun5In->SetLineWidth(2);


  TGraphErrors * sen5GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen5out.data(),myzero.data(),sen5Erout.data());
  sen5GraphOUT->SetName("sen5GraphOUT");
  sen5GraphOUT->SetMarkerColor(kRed);
  sen5GraphOUT->SetLineColor(kRed);
  sen5GraphOUT->SetMarkerStyle(24);
  sen5GraphOUT->SetMarkerSize(markerSize[0]);
  sen5GraphOUT->SetLineWidth(1);
  sen5GraphOUT->Fit("fitfun5Out","E M R F 0 Q");
  fitfun5Out->SetLineStyle(1);
  fitfun5Out->SetLineWidth(2);
  fitfun5Out->SetLineColor(kRed);
  fitfun5Out->SetLineStyle(5);
  fitfun5Out->SetLineWidth(2);


  TMultiGraph *sen5Graph = new TMultiGraph();
  sen5Graph->Add(sen5GraphIN);
  sen5Graph->Add(sen5GraphOUT);
  sen5Graph->Draw("AP");
  sen5Graph->SetTitle("");
//   sen5Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen5Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen5Graph->GetYaxis()->SetTitle(Form("%s5 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen5Graph->GetXaxis()->CenterTitle();
  sen5Graph->GetYaxis()->CenterTitle();
//   sen5Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen5Graph->GetXaxis()->SetLabelColor(0);
  sen5Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun5In->Draw("same");
  fitfun5Out->Draw("same");

  TPaveStats *statsSen5In =(TPaveStats*)sen5GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen5Out =(TPaveStats*)sen5GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen5In->SetTextColor(kBlue);
  statsSen5In->SetFillColor(kWhite); 
  statsSen5Out->SetTextColor(kRed);
  statsSen5Out->SetFillColor(kWhite);
  statsSen5In->SetTextSize(0.045);
  statsSen5Out->SetTextSize(0.045);
  statsSen5In->SetX1NDC(x_lo_stat_in4);    statsSen5In->SetX2NDC(x_hi_stat_in4); 
  statsSen5In->SetY1NDC(y_lo_stat_in4);    statsSen5In->SetY2NDC(y_hi_stat_in4);
  statsSen5Out->SetX1NDC(x_lo_stat_out4);  statsSen5Out->SetX2NDC(x_hi_stat_out4); 
  statsSen5Out->SetY1NDC(y_lo_stat_out4);  statsSen5Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[4]         =   fitfun5In->GetParameter(0);
  calc_eSenInp0[4]        =   fitfun5In->GetParError(0);
  calc_SenInChisquare[4]  =   fitfun5In->GetChisquare();
  calc_SenInNDF[4]        =   fitfun5In->GetNDF();

  calc_SenOutp0[4]        =   fitfun5Out->GetParameter(0);
  calc_eSenOutp0[4]       =   fitfun5Out->GetParError(0);
  calc_SenOutChisquare[4] =   fitfun5Out->GetChisquare();
  calc_SenOutNDF[4]       =   fitfun5Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();


  pad112->cd(9);  
  TGraphErrors * sen6GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen6in.data(),myzero.data(),sen6Erin.data());
  sen6GraphIN->SetName("sen6GraphIN");
  sen6GraphIN->SetMarkerColor(kBlue);
  sen6GraphIN->SetLineColor(kBlue);
  sen6GraphIN->SetMarkerStyle(20);
  sen6GraphIN->SetMarkerSize(markerSize[0]);
  sen6GraphIN->SetLineWidth(1);
  sen6GraphIN->Fit("fitfun6In","E M R F 0 Q");
  fitfun6In->SetLineStyle(1);
  fitfun6In->SetLineWidth(2);
  fitfun6In->SetLineColor(kBlue);
  fitfun6In->SetLineStyle(4);
  fitfun6In->SetLineWidth(2);


  TGraphErrors * sen6GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen6out.data(),myzero.data(),sen6Erout.data());
  sen6GraphOUT->SetName("sen6GraphOUT");
  sen6GraphOUT->SetMarkerColor(kRed);
  sen6GraphOUT->SetLineColor(kRed);
  sen6GraphOUT->SetMarkerStyle(24);
  sen6GraphOUT->SetMarkerSize(markerSize[0]);
  sen6GraphOUT->SetLineWidth(1);
  sen6GraphOUT->Fit("fitfun6Out","E M R F 0 Q");
  fitfun6Out->SetLineStyle(1);
  fitfun6Out->SetLineWidth(2);
  fitfun6Out->SetLineColor(kRed);
  fitfun6Out->SetLineStyle(5);
  fitfun6Out->SetLineWidth(2);

  TMultiGraph *sen6Graph = new TMultiGraph();
  sen6Graph->Add(sen6GraphIN);
  sen6Graph->Add(sen6GraphOUT);
  sen6Graph->Draw("AP");
  sen6Graph->SetTitle("");
//   sen6Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen6Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen6Graph->GetYaxis()->SetTitle(Form("%s6 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen6Graph->GetXaxis()->CenterTitle();
  sen6Graph->GetYaxis()->CenterTitle();
//   sen6Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen6Graph->GetXaxis()->SetLabelColor(0);
  sen6Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun6In->Draw("same");
  fitfun6Out->Draw("same");

  TPaveStats *statsSen6In =(TPaveStats*)sen6GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen6Out =(TPaveStats*)sen6GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen6In->SetTextColor(kBlue);
  statsSen6In->SetFillColor(kWhite); 
  statsSen6Out->SetTextColor(kRed);
  statsSen6Out->SetFillColor(kWhite);
  statsSen6In->SetTextSize(0.045);
  statsSen6Out->SetTextSize(0.045);
  statsSen6In->SetX1NDC(x_lo_stat_in4);    statsSen6In->SetX2NDC(x_hi_stat_in4); 
  statsSen6In->SetY1NDC(y_lo_stat_in4);    statsSen6In->SetY2NDC(y_hi_stat_in4);
  statsSen6Out->SetX1NDC(x_lo_stat_out4);  statsSen6Out->SetX2NDC(x_hi_stat_out4); 
  statsSen6Out->SetY1NDC(y_lo_stat_out4);  statsSen6Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[5]         =   fitfun6In->GetParameter(0);
  calc_eSenInp0[5]        =   fitfun6In->GetParError(0);
  calc_SenInChisquare[5]  =   fitfun6In->GetChisquare();
  calc_SenInNDF[5]        =   fitfun6In->GetNDF();

  calc_SenOutp0[5]        =   fitfun6Out->GetParameter(0);
  calc_eSenOutp0[5]       =   fitfun6Out->GetParError(0);
  calc_SenOutChisquare[5] =   fitfun6Out->GetChisquare();
  calc_SenOutNDF[5]       =   fitfun6Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();


  pad112->cd(8);  
  TGraphErrors * sen7GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen7in.data(),myzero.data(),sen7Erin.data());
  sen7GraphIN->SetName("sen7GraphIN");
  sen7GraphIN->SetMarkerColor(kBlue);
  sen7GraphIN->SetLineColor(kBlue);
  sen7GraphIN->SetMarkerStyle(20);
  sen7GraphIN->SetMarkerSize(markerSize[0]);
  sen7GraphIN->SetLineWidth(1);
  sen7GraphIN->Fit("fitfun7In","E M R F 0 Q");
  fitfun7In->SetLineStyle(1);
  fitfun7In->SetLineWidth(2);
  fitfun7In->SetLineColor(kBlue);
  fitfun7In->SetLineStyle(4);
  fitfun7In->SetLineWidth(2);


  TGraphErrors * sen7GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen7out.data(),myzero.data(),sen7Erout.data());
  sen7GraphOUT->SetName("sen7GraphOUT");
  sen7GraphOUT->SetMarkerColor(kRed);
  sen7GraphOUT->SetLineColor(kRed);
  sen7GraphOUT->SetMarkerStyle(24);
  sen7GraphOUT->SetMarkerSize(markerSize[0]);
  sen7GraphOUT->SetLineWidth(1);
  sen7GraphOUT->Fit("fitfun7Out","E M R F 0 Q");
  fitfun7Out->SetLineStyle(1);
  fitfun7Out->SetLineWidth(2);
  fitfun7Out->SetLineColor(kRed);
  fitfun7Out->SetLineStyle(5);
  fitfun7Out->SetLineWidth(2);

  TMultiGraph *sen7Graph = new TMultiGraph();
  sen7Graph->Add(sen7GraphIN);
  sen7Graph->Add(sen7GraphOUT);
  sen7Graph->Draw("AP");
  sen7Graph->SetTitle("");
//   sen7Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen7Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen7Graph->GetYaxis()->SetTitle(Form("%s7 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen7Graph->GetXaxis()->CenterTitle();
  sen7Graph->GetYaxis()->CenterTitle();
//   sen7Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen7Graph->GetXaxis()->SetLabelColor(0);
  sen7Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun7In->Draw("same");
  fitfun7Out->Draw("same");

  TPaveStats *statsSen7In =(TPaveStats*)sen7GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen7Out =(TPaveStats*)sen7GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen7In->SetTextColor(kBlue);
  statsSen7In->SetFillColor(kWhite); 
  statsSen7Out->SetTextColor(kRed);
  statsSen7Out->SetFillColor(kWhite);
  statsSen7In->SetTextSize(0.045);
  statsSen7Out->SetTextSize(0.045);
  statsSen7In->SetX1NDC(x_lo_stat_in4);    statsSen7In->SetX2NDC(x_hi_stat_in4); 
  statsSen7In->SetY1NDC(y_lo_stat_in4);    statsSen7In->SetY2NDC(y_hi_stat_in4);
  statsSen7Out->SetX1NDC(x_lo_stat_out4);  statsSen7Out->SetX2NDC(x_hi_stat_out4); 
  statsSen7Out->SetY1NDC(y_lo_stat_out4);  statsSen7Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[6]         =   fitfun7In->GetParameter(0);
  calc_eSenInp0[6]        =   fitfun7In->GetParError(0);
  calc_SenInChisquare[6]  =   fitfun7In->GetChisquare();
  calc_SenInNDF[6]        =   fitfun7In->GetNDF();

  calc_SenOutp0[6]        =   fitfun7Out->GetParameter(0);
  calc_eSenOutp0[6]       =   fitfun7Out->GetParError(0);
  calc_SenOutChisquare[6] =   fitfun7Out->GetChisquare();
  calc_SenOutNDF[6]       =   fitfun7Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();


  pad112->cd(7);  
  TGraphErrors * sen8GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,sen8in.data(),myzero.data(),sen8Erin.data());
  sen8GraphIN->SetName("sen8GraphIN");
  sen8GraphIN->SetMarkerColor(kBlue);
  sen8GraphIN->SetLineColor(kBlue);
  sen8GraphIN->SetMarkerStyle(20);
  sen8GraphIN->SetMarkerSize(markerSize[0]);
  sen8GraphIN->SetLineWidth(1);
  sen8GraphIN->Fit("fitfun8In","E M R F 0 Q");
  fitfun8In->SetLineStyle(1);
  fitfun8In->SetLineWidth(2);
  fitfun8In->SetLineColor(kBlue);
  fitfun8In->SetLineStyle(4);
  fitfun8In->SetLineWidth(2);


  TGraphErrors * sen8GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,sen8out.data(),myzero.data(),sen8Erout.data());
  sen8GraphOUT->SetName("sen8GraphOUT");
  sen8GraphOUT->SetMarkerColor(kRed);
  sen8GraphOUT->SetLineColor(kRed);
  sen8GraphOUT->SetMarkerStyle(24);
  sen8GraphOUT->SetMarkerSize(markerSize[0]);
  sen8GraphOUT->SetLineWidth(1);
  sen8GraphOUT->Fit("fitfun8Out","E M R F 0 Q");
  fitfun8Out->SetLineStyle(1);
  fitfun8Out->SetLineWidth(2);
  fitfun8Out->SetLineColor(kRed);
  fitfun8Out->SetLineStyle(5);
  fitfun8Out->SetLineWidth(2);

  TMultiGraph *sen8Graph = new TMultiGraph();
  sen8Graph->Add(sen8GraphIN);
  sen8Graph->Add(sen8GraphOUT);
  sen8Graph->Draw("AP");
  sen8Graph->SetTitle("");
//   sen8Graph->GetXaxis()->SetNdivisions(25,0,0);
  sen8Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  sen8Graph->GetYaxis()->SetTitle(Form("%s8 Sen-%s [%s]",device.Data(),senType.Data(),senUnit.Data()));
  sen8Graph->GetXaxis()->CenterTitle();
  sen8Graph->GetYaxis()->CenterTitle();
//   sen8Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  sen8Graph->GetXaxis()->SetLabelColor(0);
  sen8Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun8In->Draw("same");
  fitfun8Out->Draw("same");

  TPaveStats *statsSen8In =(TPaveStats*)sen8GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSen8Out =(TPaveStats*)sen8GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsSen8In->SetTextColor(kBlue);
  statsSen8In->SetFillColor(kWhite); 
  statsSen8Out->SetTextColor(kRed);
  statsSen8Out->SetFillColor(kWhite);
  statsSen8In->SetTextSize(0.045);
  statsSen8Out->SetTextSize(0.045);
  statsSen8In->SetX1NDC(x_lo_stat_in4);    statsSen8In->SetX2NDC(x_hi_stat_in4); 
  statsSen8In->SetY1NDC(y_lo_stat_in4);    statsSen8In->SetY2NDC(y_hi_stat_in4);
  statsSen8Out->SetX1NDC(x_lo_stat_out4);  statsSen8Out->SetX2NDC(x_hi_stat_out4); 
  statsSen8Out->SetY1NDC(y_lo_stat_out4);  statsSen8Out->SetY2NDC(y_hi_stat_out4);

  calc_SenInp0[7]         =   fitfun8In->GetParameter(0);
  calc_eSenInp0[7]        =   fitfun8In->GetParError(0);
  calc_SenInChisquare[7]  =   fitfun8In->GetChisquare();
  calc_SenInNDF[7]        =   fitfun8In->GetNDF();

  calc_SenOutp0[7]        =   fitfun8Out->GetParameter(0);
  calc_eSenOutp0[7]       =   fitfun8Out->GetParError(0);
  calc_SenOutChisquare[7] =   fitfun8Out->GetChisquare();
  calc_SenOutNDF[7]       =   fitfun8Out->GetNDF();

  tSenIn->Draw();
  tSenOut->Draw();


  pad112->cd(5);  


  Double_t octant[det];
  Double_t zero[det]={0.0};
  Double_t valuesum[det];
  Double_t valueerror[det];
  Double_t valuediff[det];
  Double_t errordiff[det];
  Double_t x[det];
  Double_t errx[det];


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


  for(Int_t i =0;i<det;i++){
    octant[i] = i+1;
    valuesum[i]=(calc_SenInp0[i]+calc_SenOutp0[i])/2.0;
    valueerror[i]= (sqrt(pow(calc_eSenInp0[i],2)+pow(calc_eSenOutp0[i],2)))/2.0;
    valuediff[i]=((calc_SenInp0[i]/pow(calc_eSenInp0[i],2)) - (calc_SenOutp0[i]/pow(calc_eSenOutp0[i],2))) /((1/pow(calc_eSenInp0[i],2)) + (1/pow(calc_eSenOutp0[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(calc_eSenInp0[i],2)))+(1/pow(calc_eSenOutp0[i],2))));


  }

  /********************************************/

  for(Int_t i =0;i<det;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }


  // Draw IN values
  TGraphErrors* grp_in  = new TGraphErrors(det,octant,calc_SenInp0,zero,calc_eSenInp0);
  grp_in ->SetName("grp_in");
  grp_in ->Draw("goff");
  grp_in ->SetMarkerSize(markerSize[0]);
  grp_in ->SetMarkerStyle(20);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->SetLineColor(kBlue);
  grp_in->Fit("fit_in","B");
  TF1* fit1 = grp_in->GetFunction("fit_in");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
  fit1->SetLineStyle(4);
  fit1->SetLineWidth(2);

  // Draw OUT values
  TGraphErrors* grp_out  = new TGraphErrors(det,octant,calc_SenOutp0,zero,calc_eSenOutp0);
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

  grp_out->Fit("fit_out","B");
  TF1* fit2 = grp_out->GetFunction("fit_out");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);
  fit2->SetLineStyle(5);
  fit2->SetLineWidth(2);

//   // Draw In+Out values
//   TGraphErrors* grp_sum  = new TGraphErrors(det,x,valuesum,errx,valueerror);
//   grp_sum ->SetName("grp_sum");
//   grp_sum ->Draw("goff");
//   grp_sum ->SetMarkerSize(markerSize[0]);
//   //  grp_sum ->SetLineWidth(0);
//   grp_sum ->SetMarkerStyle(22);
//   grp_sum ->SetMarkerColor(kGreen-3);
//   grp_sum ->SetLineColor(kGreen-3);
//   grp_sum->Fit("pol0","B");
//   TF1* fit3 = grp_sum->GetFunction("pol0");
//   fit3->DrawCopy("same");
//   fit3->SetLineColor(kGreen-2);
//   fit3->SetLineStyle(2);
//   fit3->SetLineWidth(3);

//   // Draw In-Out
//   TGraphErrors* grp_diff  = new TGraphErrors(det,x,valuediff,errx,errordiff);
//   grp_diff ->SetName("grp_diff");
//   grp_diff ->Draw("goff");
//   grp_diff ->SetMarkerSize(markerSize[0]);
//   grp_diff ->SetLineWidth(0);
//   grp_diff ->SetMarkerStyle(21);
//   grp_diff ->SetMarkerColor(kMagenta);
//   grp_diff ->SetLineColor(kMagenta);
//   grp_diff->Fit("fit_in","B");
//   TF1* fit4 = grp_diff->GetFunction("fit_in");
//   fit4->DrawCopy("same");
//   fit4->SetLineColor(kMagenta+1);
//   fit4->SetLineStyle(1);
//   fit4->SetLineWidth(4);

  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in,"P");
  grp->Add(grp_out,"P");
//   grp->Add(grp_sum,"P");
//   grp->Add(grp_diff,"P");
  grp->Draw("A");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s %s Sen-%s [%s]",device.Data(),deviceTitle.Data(),senType.Data(),senUnit.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.8);
  grp->GetXaxis()->SetTitleOffset(0.8);
  grp->GetXaxis()->SetNdivisions(8,0,0);
  grp->GetYaxis()->SetNdivisions(8,5,0);
  if(SCALE){
    grp->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  }
  TAxis *xaxisGrp= grp->GetXaxis();
  xaxisGrp->SetLimits(0.5,8.5);

//   fit3->DrawCopy("same");
//   fit4->DrawCopy("same");


  fit1->SetParNames("p1","#phi_{0}",Form("#frac{#partialA}{#partial%s}_{IN}",senTitle.Data()));
  fit2->SetParNames("p1","#phi_{0}",Form("#frac{#partialA}{#partial%s}_{OUT}",senTitle.Data()));
//   fit3->SetParNames("C");
//   fit4->SetParNames("A_{M}","#phi_{0}","C");

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *stats4 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");


  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 

  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
 
//   stats3->SetTextColor(kGreen-2);
//   stats3->SetFillColor(kWhite); 

//   stats4->SetTextColor(kMagenta+1);
//   stats4->SetFillColor(kWhite); 

  stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(y_lo_stat_in4*0.82);stats1->SetY2NDC(y_hi_stat_in4);
  stats2->SetX1NDC(x_lo_stat_out4); stats2->SetX2NDC(x_hi_stat_out4); stats2->SetY1NDC(y_lo_stat_out4*0.35);stats2->SetY2NDC(y_hi_stat_out4*0.82);

//   stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(y_lo_stat_in4);stats1->SetY2NDC(y_hi_stat_in4);
//   stats2->SetX1NDC(x_lo_stat_out4); stats2->SetX2NDC(x_hi_stat_out4); stats2->SetY1NDC(y_lo_stat_out4);stats2->SetY2NDC(y_hi_stat_out4);
//   stats3->SetX1NDC(x_lo_stat_in4); stats3->SetX2NDC(x_hi_stat_in4); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);
//   stats4->SetX1NDC(x_lo_stat_in4); stats4->SetX2NDC(x_hi_stat_in4); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);


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



//   TLegend *legend = new TLegend(legendCoordinates[0],legendCoordinates[1],legendCoordinates[2],legendCoordinates[3],"","brNDC");
//   legend->SetNColumns(2);
//   legend->AddEntry(grp_in,  Form("A_{IHWP-IN} = %4.2f #pm %4.2f ppm",p0in,ep0in), "p");
//   legend->AddEntry(grp_out, Form("A_{IHWP-OUT} = %4.2f #pm %4.2f ppm",p0out,ep0out), "p");
//   legend->SetFillColor(0);
//   legend->SetBorderSize(2);
//   legend->SetTextSize(0.035);
//   legend->Draw("");



  pad2->cd();

  grp->Draw("A");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s %s Sen-%s [%s]",device.Data(),deviceTitle.Data(),senType.Data(),senUnit.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetXaxis()->SetNdivisions(8,0,0);
  grp->GetYaxis()->SetNdivisions(8,5,0);
  if(SCALE){
    grp->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  }
  xaxisGrp->SetLimits(0.5,8.5);

//   fit3->DrawCopy("same");
//   fit4->DrawCopy("same");

  stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(y_lo_stat_in4*0.82);stats1->SetY2NDC(y_hi_stat_in4);
  stats2->SetX1NDC(x_lo_stat_out4); stats2->SetX2NDC(x_hi_stat_out4); stats2->SetY1NDC(y_lo_stat_out4*0.35);stats2->SetY2NDC(y_hi_stat_out4*0.82);
//   stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(0.72);stats1->SetY2NDC(0.95);
//   stats2->SetX1NDC(x_lo_stat_in4); stats2->SetX2NDC(x_hi_stat_in4); stats2->SetY1NDC(0.47);stats2->SetY2NDC(0.70);
//   stats3->SetX1NDC(x_lo_stat_in4); stats3->SetX2NDC(x_hi_stat_in4); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);
//   stats4->SetX1NDC(x_lo_stat_in4); stats4->SetX2NDC(x_hi_stat_in4); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);

//   legend->Draw("");



  gPad->Modified();
  pad112->Modified();
  c11->Modified();
  gPad->Update();



  TString saveAllPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_sen%s_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),senType.Data(),database_stem.Data());
  TString saveIndividualPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_individual_sen%s_%s"
				      ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
				      ,deviceName.Data(),reg_set.Data(),reg_calc.Data(),senType.Data(),database_stem.Data());
  

  c11-> Update();
  c11->Print(saveAllPlot+".png");
  if(FIGURE){
    c11->Print(saveAllPlot+".svg");
    c11->Print(saveAllPlot+".C");
  }

  c1-> Update();
  c1->Print(saveIndividualPlot+".png");
  if(FIGURE){
    c1->Print(saveIndividualPlot+".svg");
    c1->Print(saveIndividualPlot+".C");
  }


  MyfileWrite.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_sen%s_%s.txt"
			,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			,deviceName.Data(),reg_calc.Data(),reg_set.Data(),senType.Data(),database_stem.Data()));

  for(Int_t i =0;i<det;i++){
    MyfileWrite<<i+1<<" "<<calc_SenInp0[i]<<" "<<calc_eSenInp0[i]<<"\t"<<calc_SenOutp0[i]<<" "<<calc_eSenOutp0[i]<<endl;
  }

  MyfileWrite.close();




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


