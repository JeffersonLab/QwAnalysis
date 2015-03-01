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
//   if(device_opt == 1){device = "MD"; deviceTitle = "Barsum"; deviceName = "Barsum"; YRange[0]=-35; YRange[1]=35; }
//   if(device_opt == 2){device = "DSLumi"; deviceTitle = "Sum"; deviceName = "Sum"; YRange[0]=-20; YRange[1]=20;}
//   if(device_opt  > 2){printf("%sWrong choise of device \n%s",red,normal); exit(0);}
  if(device_opt == 1){device = "MD"; deviceTitle = "PMTavg"; deviceName = "PMTavg"; YRange[0]=-35; YRange[1]=35; }
  if(device_opt == 2){device = "DSLumi"; deviceTitle = "Sum"; deviceName = "Sum"; YRange[0]=-20; YRange[1]=20;}
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

//   int position1[8] = { 4, 1, 2, 3, 6, 9, 8, 7 };

  Int_t counter11=0;
  ifstream runletAsym;
  const Int_t NUM11 = 5000;
  Double_t octant11[NUM11],run_number11[NUM11],asym11[NUM11],asymError11[NUM11],asymRMS11[NUM11];
  TString hwp11[NUM11];

  std::vector <Double_t> run1in,run2in,run3in,run4in,run5in,run6in,run7in,run8in;
  std::vector <Double_t> run1out,run2out,run3out,run4out,run5out,run6out,run7out,run8out;
  std::vector <Double_t> asym1in,asym2in,asym3in,asym4in,asym5in,asym6in,asym7in,asym8in;
  std::vector <Double_t> asym1out,asym2out,asym3out,asym4out,asym5out,asym6out,asym7out,asym8out;
  std::vector <Double_t> asym1Erin,asym2Erin,asym3Erin,asym4Erin,asym5Erin,asym6Erin,asym7Erin,asym8Erin;
  std::vector <Double_t> asym1Erout,asym2Erout,asym3Erout,asym4Erout,asym5Erout,asym6Erout,asym7Erout,asym8Erout;
  std::vector <Double_t> myzero;

  const Int_t det = 8;
  Double_t calc_AsymInp0[det],calc_eAsymInp0[det],calc_AsymInChisquare[det],calc_AsymInNDF[det];
  Double_t calc_AsymOutp0[det],calc_eAsymOutp0[det],calc_AsymOutChisquare[det],calc_AsymOutNDF[det];


  runletAsym.open(Form("dirPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_%s_asym_%s.txt"
			     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			     ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

   if (runletAsym.is_open()) {
   
    while(1) {
      //       zero11[counter11]=0;
      
      runletAsym >> octant11[counter11] >> run_number11[counter11] >> hwp11[counter11]
		   >> asym11[counter11]   >> asymError11[counter11]  >> asymRMS11[counter11] ;
      
      if (!runletAsym.good()) break;
      
      if(hwp11[counter11]== "in"){
	
	if(octant11[counter11]==1) {
	  run1in.push_back(run_number11[counter11]);
	  asym1in.push_back(asym11[counter11]); 
	  asym1Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==2) {
	  run2in.push_back(run_number11[counter11]);
	  asym2in.push_back(asym11[counter11]); 
	  asym2Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==3) {
	  run3in.push_back(run_number11[counter11]);
	  asym3in.push_back(asym11[counter11]); 
	  asym3Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==4) {
	  run4in.push_back(run_number11[counter11]);
	  asym4in.push_back(asym11[counter11]); 
	  asym4Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==5) {
	  run5in.push_back(run_number11[counter11]);
	  asym5in.push_back(asym11[counter11]); 
	  asym5Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==6) {
	  run6in.push_back(run_number11[counter11]);
	  asym6in.push_back(asym11[counter11]); 
	  asym6Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==7) {
	  run7in.push_back(run_number11[counter11]);
	  asym7in.push_back(asym11[counter11]); 
	  asym7Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==8) {
	  run8in.push_back(run_number11[counter11]);
	  asym8in.push_back(asym11[counter11]); 
	  asym8Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	
	
      }



      if(hwp11[counter11]== "out"){
	
	if(octant11[counter11]==1) {
	  run1out.push_back(run_number11[counter11]);
	  asym1out.push_back(asym11[counter11]); 
	  asym1Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==2) {
	  run2out.push_back(run_number11[counter11]);
	  asym2out.push_back(asym11[counter11]); 
	  asym2Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==3) {
	  run3out.push_back(run_number11[counter11]);
	  asym3out.push_back(asym11[counter11]); 
	  asym3Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==4) {
	  run4out.push_back(run_number11[counter11]);
	  asym4out.push_back(asym11[counter11]); 
	  asym4Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==5) {
	  run5out.push_back(run_number11[counter11]);
	  asym5out.push_back(asym11[counter11]); 
	  asym5Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==6) {
	  run6out.push_back(run_number11[counter11]);
	  asym6out.push_back(asym11[counter11]); 
	  asym6Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==7) {
	  run7out.push_back(run_number11[counter11]);
	  asym7out.push_back(asym11[counter11]); 
	  asym7Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==8) {
	  run8out.push_back(run_number11[counter11]);
	  asym8out.push_back(asym11[counter11]); 
	  asym8Erout.push_back(asymError11[counter11]);
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
   runletAsym.close();

//   printf("%s Checking %d\n%s",green,__LINE__,normal);


  TString title11;
  TString titleInOut = Form("%s (%s, %s A): Regression-%s %s %s Asymmetries."
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());

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




  Double_t dummyINaxis[(Int_t)run1in.size()];
  Double_t dummyOUTaxis[(Int_t)run1out.size()];
  Int_t dummyArraySize;
//   Int_t dummyArraySize = (Int_t)run1in.size() < (Int_t)run1out.size() ? (Int_t)run1in.size() : (Int_t)run1out.size();
  TString xAxisTitle = "Runlets";

  TF1 *fitfun1In,*fitfun2In,*fitfun3In,*fitfun4In,*fitfun5In,*fitfun6In,*fitfun7In,*fitfun8In;
  TF1 *fitfun1Out,*fitfun2Out,*fitfun3Out,*fitfun4Out,*fitfun5Out,*fitfun6Out,*fitfun7Out,*fitfun8Out;

  TLatex *tAsymIn, *tAsymOut;
//   TF1* fitfunIn  = new TF1("fitfunIn","pol0",dummyINaxis[0],dummyINaxis[(Int_t)run1in.size()]);
//   TF1* fitfunIn  = new TF1("fitfunIn","pol0",-0.7,(Int_t)run1in.size());
//   TF1* fitfunOut = new TF1("fitfunOut","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);

  if(qtor_opt==2){
  cout<<red<<__LINE__<<normal<<endl;

  dummyArraySize = (Int_t)run1in.size() > (Int_t)run1out.size() ? (Int_t)run1in.size() : (Int_t)run1out.size();
  for(Int_t j = 0; j< dummyArraySize; j++) {
    dummyINaxis[j-1] = j;
    dummyOUTaxis[j-1] = j + dummyArraySize;
  }

    fitfun1In  = new TF1("fitfun1In","pol0",-0.7,(Int_t)run1in.size());
    fitfun1Out = new TF1("fitfun1Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun2In  = new TF1("fitfun2In","pol0",-0.7,(Int_t)run1in.size());
    fitfun2Out = new TF1("fitfun2Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun3In  = new TF1("fitfun3In","pol0",-0.7,(Int_t)run1in.size());
    fitfun3Out = new TF1("fitfun3Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun4In  = new TF1("fitfun4In","pol0",-0.7,(Int_t)run1in.size());
    fitfun4Out = new TF1("fitfun4Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun5In  = new TF1("fitfun5In","pol0",-0.7,(Int_t)run1in.size());
    fitfun5Out = new TF1("fitfun5Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun6In  = new TF1("fitfun6In","pol0",-0.7,(Int_t)run1in.size());
    fitfun6Out = new TF1("fitfun6Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun7In  = new TF1("fitfun7In","pol0",-0.7,(Int_t)run1in.size());
    fitfun7Out = new TF1("fitfun7Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
    fitfun8In  = new TF1("fitfun8In","pol0",-0.7,(Int_t)run1in.size());
    fitfun8Out = new TF1("fitfun8Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);

    tAsymIn = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
    tAsymOut = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  }

  else{
    cout<<red<<__LINE__<<normal<<endl;
    
    dummyArraySize = (Int_t)run1in.size() < (Int_t)run1out.size() ? (Int_t)run1in.size() : (Int_t)run1out.size();

  for(Int_t j = 0; j< dummyArraySize; j++) {
    dummyINaxis[j-1] = j + dummyArraySize;
    dummyOUTaxis[j-1] = j;
  }

    fitfun1In  = new TF1("fitfun1In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun1Out = new TF1("fitfun1Out","pol0",-0.1,run1in.size()-0.1);
    fitfun2In  = new TF1("fitfun2In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun2Out = new TF1("fitfun2Out","pol0",-0.1,run1in.size()-0.1);
    fitfun3In  = new TF1("fitfun3In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun3Out = new TF1("fitfun3Out","pol0",-0.1,run1in.size()-0.1);
    fitfun4In  = new TF1("fitfun4In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun4Out = new TF1("fitfun4Out","pol0",-0.1,run1in.size()-0.1);
    fitfun5In  = new TF1("fitfun5In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun5Out = new TF1("fitfun5Out","pol0",-0.1,run1in.size()-0.1);
    fitfun6In  = new TF1("fitfun6In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun6Out = new TF1("fitfun6Out","pol0",-0.1,run1in.size()-0.1);
    fitfun7In  = new TF1("fitfun7In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun7Out = new TF1("fitfun7Out","pol0",-0.1,run1in.size()-0.1);
    fitfun8In  = new TF1("fitfun8In","pol0",run1in.size()-0.1,run1in.size()+run1out.size()-0.1);
    fitfun8Out = new TF1("fitfun8Out","pol0",-0.1,run1in.size()-0.1);

    tAsymIn = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-IN");
    tAsymOut = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-OUT");
  }

  cout<<"run1in: "<<run1in.size()<<endl;
  for ( Int_t i=0; i<run1in.size(); i++) {
    cout<<Form("i:%d\trun1:%5.2f\tasym1IN11:%f",i,run1in[i],asym1in[i])<<endl;
  }
  cout<<"run1out: "<<run1out.size()<<endl;
  for ( Int_t i=0; i<run1out.size(); i++) {
    cout<<Form("i:%d\trun1:%5.2f\tasym1OUT11:%f",i,run1out[i],asym1out[i])<<endl;
  }
  cout<<"dummyArraySize:"<< dummyArraySize<<endl;


  tAsymIn->SetTextSize(0.06);
  tAsymIn->SetTextColor(kBlue);
  tAsymOut->SetTextSize(0.06);
  tAsymOut->SetTextColor(kRed);


  fitfun1In->SetParNames("A_{IN}");
  fitfun1Out->SetParNames("A_{OUT}");
  fitfun2In->SetParNames("A_{IN}");
  fitfun2Out->SetParNames("A_{OUT}");
  fitfun3In->SetParNames("A_{IN}");
  fitfun3Out->SetParNames("A_{OUT}");
  fitfun4In->SetParNames("A_{IN}");
  fitfun4Out->SetParNames("A_{OUT}");
  fitfun5In->SetParNames("A_{IN}");
  fitfun5Out->SetParNames("A_{OUT}");
  fitfun6In->SetParNames("A_{IN}");
  fitfun6Out->SetParNames("A_{OUT}");
  fitfun7In->SetParNames("A_{IN}");
  fitfun7Out->SetParNames("A_{OUT}");
  fitfun8In->SetParNames("A_{IN}");
  fitfun8Out->SetParNames("A_{OUT}");


  /**********************************/

  pad112->cd(4);  

  //TGraphErrors * asym1GraphIN = new TGraphErrors(counter11,run_number1IN11,asym1IN11,zero11,asymError1IN11);
  //TGraphErrors * asym1GraphIN = new TGraphErrors((Int_t)run1in.size(),run1in.data(),asym1in.data(),myzero.data(),asym1Erin.data());
  TGraphErrors * asym1GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym1in.data(),myzero.data(),asym1Erin.data());
  asym1GraphIN->SetName("asym1GraphIN");
  asym1GraphIN->SetMarkerColor(kBlue);
  asym1GraphIN->SetLineColor(kBlue);
  asym1GraphIN->SetMarkerStyle(20);
  asym1GraphIN->SetMarkerSize(markerSize[0]);
  asym1GraphIN->SetLineWidth(1);
  //   asym1GraphIN->Draw("AP");
  asym1GraphIN->Fit("fitfun1In","EMRF0Q");
  fitfun1In->SetLineStyle(1);
  fitfun1In->SetLineWidth(2);
  fitfun1In->SetLineColor(kBlue);
  fitfun1In->SetLineStyle(4);
  fitfun1In->SetLineWidth(2);


  //TGraphErrors * asym1GraphOUT = new TGraphErrors((Int_t)run1out.size(),run1out.data(),asym1out.data(),myzero.data(),asym1Erout.data());
  TGraphErrors * asym1GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym1out.data(),myzero.data(),asym1Erout.data());
//   TGraphErrors * asym1GraphOUT = new TGraphErrors(counter11,dummyOUT,asym1OUT11,zero11,asym1Error1OUT11);
  asym1GraphOUT->SetName("asym1GraphOUT");
  asym1GraphOUT->SetMarkerColor(kRed);
  asym1GraphOUT->SetLineColor(kRed);
  asym1GraphOUT->SetMarkerStyle(24);
  asym1GraphOUT->SetMarkerSize(markerSize[0]);
  asym1GraphOUT->SetLineWidth(1);
//   asym1GraphOUT->Draw("AP");
  asym1GraphOUT->Fit("fitfun1Out","EMRF0Q");
  fitfun1Out->SetLineStyle(1);
  fitfun1Out->SetLineWidth(2);
  fitfun1Out->SetLineColor(kRed);
  fitfun1Out->SetLineStyle(5);
  fitfun1Out->SetLineWidth(2);

  TMultiGraph *asym1Graph = new TMultiGraph();
  asym1Graph->Add(asym1GraphIN);
  asym1Graph->Add(asym1GraphOUT);
  asym1Graph->Draw("AP");
  asym1Graph->SetTitle("");
//   asym1Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym1Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym1Graph->GetYaxis()->SetTitle(Form("%s1 Asym. [ppm]",device.Data()));
  asym1Graph->GetXaxis()->CenterTitle();
  asym1Graph->GetYaxis()->CenterTitle();
//   asym1Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym1Graph->GetXaxis()->SetLabelColor(0);
  asym1Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);
//   TAxis *xaxisAsym1Graph= asym1Graph->GetXaxis();
//   xaxisAsym1Graph->SetLimits(16066.00,16066.14);
//   pad112->Range(16064.77,-30.66847,16067.63,47.58539);

  fitfun1In->Draw("same");
  fitfun1Out->Draw("same");


//   TAxis *xaxisAsym1Graph= asym1Graph->GetXaxis();
//   xaxisAsym1Graph->SetLimits(run_number1IN11[0],run_number1IN11[0]+30);
  
  TPaveStats *statsAsym1In =(TPaveStats*)asym1GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym1Out =(TPaveStats*)asym1GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym1In->SetTextColor(kBlue);
  statsAsym1In->SetFillColor(kWhite); 
  statsAsym1Out->SetTextColor(kRed);
  statsAsym1Out->SetFillColor(kWhite);
  statsAsym1In->SetTextSize(0.045);
  statsAsym1Out->SetTextSize(0.045);
  statsAsym1In->SetX1NDC(x_lo_stat_in4);    statsAsym1In->SetX2NDC(x_hi_stat_in4); 
  statsAsym1In->SetY1NDC(y_lo_stat_in4);    statsAsym1In->SetY2NDC(y_hi_stat_in4);
  statsAsym1Out->SetX1NDC(x_lo_stat_out4);  statsAsym1Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym1Out->SetY1NDC(y_lo_stat_out4);  statsAsym1Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[0]         =   fitfun1In->GetParameter(0);
  calc_eAsymInp0[0]        =   fitfun1In->GetParError(0);
  calc_AsymInChisquare[0]  =   fitfun1In->GetChisquare();
  calc_AsymInNDF[0]        =   fitfun1In->GetNDF();

  calc_AsymOutp0[0]        =   fitfun1Out->GetParameter(0);
  calc_eAsymOutp0[0]       =   fitfun1Out->GetParError(0);
  calc_AsymOutChisquare[0] =   fitfun1Out->GetChisquare();
  calc_AsymOutNDF[0]       =   fitfun1Out->GetNDF();

//   TLegend *legRunAsym1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
//   legRunAsym1->AddEntry("asym1GraphIN",  Form("A_{IN}= %4.2f#pm%4.2f [ppm]",calc_Asym1Inp0,calc_eAsym1Inp0),"lp");
//   legRunAsym1->AddEntry("asym1GraphOUT",  Form("A_{OUT}= %4.2f#pm%4.2f [ppm]",calc_Asym1Outp0,calc_eAsym1Outp0),"lp");
//   legRunAsym1->SetTextSize(0.042);
//   legRunAsym1->SetFillColor(0);
//   legRunAsym1->SetBorderSize(2);
//   legRunAsym1->Draw();


// //   TLatex* tAsymIn = new TLatex(5,-40,Form("%5.0f",run1in[0]));
//   TLatex* tAsymIn = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
//   tAsymIn->SetTextSize(0.06);
//   tAsymIn->SetTextColor(kBlue);
//   tAsymIn->Draw();
// //   TLatex* tAsymOut = new TLatex(17,-40,Form("%5.0f",run1out[0]));
//   TLatex* tAsymOut = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
//   tAsymOut->SetTextSize(0.06);
//   tAsymOut->SetTextColor(kRed);
//   tAsymOut->Draw();


  tAsymIn->Draw();
  tAsymOut->Draw();


//   cout<<red<<__LINE__<<normal<<endl;

  pad112->cd(1);  
  TGraphErrors * asym2GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym2in.data(),myzero.data(),asym2Erin.data());
  asym2GraphIN->SetName("asym2GraphIN");
  asym2GraphIN->SetMarkerColor(kBlue);
  asym2GraphIN->SetLineColor(kBlue);
  asym2GraphIN->SetMarkerStyle(20);
  asym2GraphIN->SetMarkerSize(markerSize[0]);
  asym2GraphIN->SetLineWidth(1);
  asym2GraphIN->Fit("fitfun2In","EMRF0Q");
  fitfun2In->SetLineStyle(1);
  fitfun2In->SetLineWidth(2);
  fitfun2In->SetLineColor(kBlue);
  fitfun2In->SetLineStyle(4);
  fitfun2In->SetLineWidth(2);

  TGraphErrors * asym2GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym2out.data(),myzero.data(),asym2Erout.data());
  asym2GraphOUT->SetName("asym2GraphOUT");
  asym2GraphOUT->SetMarkerColor(kRed);
  asym2GraphOUT->SetLineColor(kRed);
  asym2GraphOUT->SetMarkerStyle(24);
  asym2GraphOUT->SetMarkerSize(markerSize[0]);
  asym2GraphOUT->SetLineWidth(1);
  asym2GraphOUT->Fit("fitfun2Out","EMRF0Q");
  fitfun2Out->SetLineStyle(1);
  fitfun2Out->SetLineWidth(2);
  fitfun2Out->SetLineColor(kRed);
  fitfun2Out->SetLineStyle(5);
  fitfun2Out->SetLineWidth(2);

  TMultiGraph *asym2Graph = new TMultiGraph();
  asym2Graph->Add(asym2GraphIN);
  asym2Graph->Add(asym2GraphOUT);
  asym2Graph->Draw("AP");
  asym2Graph->SetTitle("");
//   asym2Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym2Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym2Graph->GetYaxis()->SetTitle(Form("%s2 Asym. [ppm]",device.Data()));
  asym2Graph->GetXaxis()->CenterTitle();
  asym2Graph->GetYaxis()->CenterTitle();
//   asym2Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym2Graph->GetXaxis()->SetLabelColor(0);
  asym2Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun2In->Draw("same");
  fitfun2Out->Draw("same");

  TPaveStats *statsAsym2In =(TPaveStats*)asym2GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym2Out =(TPaveStats*)asym2GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym2In->SetTextColor(kBlue);
  statsAsym2In->SetFillColor(kWhite); 
  statsAsym2Out->SetTextColor(kRed);
  statsAsym2Out->SetFillColor(kWhite);
  statsAsym2In->SetTextSize(0.045);
  statsAsym2Out->SetTextSize(0.045);
  statsAsym2In->SetX1NDC(x_lo_stat_in4);    statsAsym2In->SetX2NDC(x_hi_stat_in4); 
  statsAsym2In->SetY1NDC(y_lo_stat_in4);    statsAsym2In->SetY2NDC(y_hi_stat_in4);
  statsAsym2Out->SetX1NDC(x_lo_stat_out4);  statsAsym2Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym2Out->SetY1NDC(y_lo_stat_out4);  statsAsym2Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[1]         =   fitfun2In->GetParameter(0);
  calc_eAsymInp0[1]        =   fitfun2In->GetParError(0);
  calc_AsymInChisquare[1]  =   fitfun2In->GetChisquare();
  calc_AsymInNDF[1]        =   fitfun2In->GetNDF();

  calc_AsymOutp0[1]        =   fitfun2Out->GetParameter(0);
  calc_eAsymOutp0[1]       =   fitfun2Out->GetParError(0);
  calc_AsymOutChisquare[1] =   fitfun2Out->GetChisquare();
  calc_AsymOutNDF[1]       =   fitfun2Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(2);  
  TGraphErrors * asym3GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym3in.data(),myzero.data(),asym3Erin.data());
  asym3GraphIN->SetName("asym3GraphIN");
  asym3GraphIN->SetMarkerColor(kBlue);
  asym3GraphIN->SetLineColor(kBlue);
  asym3GraphIN->SetMarkerStyle(20);
  asym3GraphIN->SetMarkerSize(markerSize[0]);
  asym3GraphIN->SetLineWidth(1);
  asym3GraphIN->Fit("fitfun3In","EMRF0Q");
  fitfun3In->SetLineStyle(1);
  fitfun3In->SetLineWidth(2);
  fitfun3In->SetLineColor(kBlue);
  fitfun3In->SetLineStyle(4);
  fitfun3In->SetLineWidth(2);

  TGraphErrors * asym3GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym3out.data(),myzero.data(),asym3Erout.data());
  asym3GraphOUT->SetName("asym3GraphOUT");
  asym3GraphOUT->SetMarkerColor(kRed);
  asym3GraphOUT->SetLineColor(kRed);
  asym3GraphOUT->SetMarkerStyle(24);
  asym3GraphOUT->SetMarkerSize(markerSize[0]);
  asym3GraphOUT->SetLineWidth(1);
  asym3GraphOUT->Fit("fitfun3Out","EMRF0Q");
  fitfun3Out->SetLineStyle(1);
  fitfun3Out->SetLineWidth(2);
  fitfun3Out->SetLineColor(kRed);
  fitfun3Out->SetLineStyle(5);
  fitfun3Out->SetLineWidth(2);


  TMultiGraph *asym3Graph = new TMultiGraph();
  asym3Graph->Add(asym3GraphIN);
  asym3Graph->Add(asym3GraphOUT);
  asym3Graph->Draw("AP");
  asym3Graph->SetTitle("");
//   asym3Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym3Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym3Graph->GetYaxis()->SetTitle(Form("%s3 Asym. [ppm]",device.Data()));
  asym3Graph->GetXaxis()->CenterTitle();
  asym3Graph->GetYaxis()->CenterTitle();
//   asym3Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym3Graph->GetXaxis()->SetLabelColor(0);
  asym3Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun3In->Draw("same");
  fitfun3Out->Draw("same");

  TPaveStats *statsAsym3In =(TPaveStats*)asym3GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym3Out =(TPaveStats*)asym3GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym3In->SetTextColor(kBlue);
  statsAsym3In->SetFillColor(kWhite); 
  statsAsym3Out->SetTextColor(kRed);
  statsAsym3Out->SetFillColor(kWhite);
  statsAsym3In->SetTextSize(0.045);
  statsAsym3Out->SetTextSize(0.045);
  statsAsym3In->SetX1NDC(x_lo_stat_in4);    statsAsym3In->SetX2NDC(x_hi_stat_in4); 
  statsAsym3In->SetY1NDC(y_lo_stat_in4);    statsAsym3In->SetY2NDC(y_hi_stat_in4);
  statsAsym3Out->SetX1NDC(x_lo_stat_out4);  statsAsym3Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym3Out->SetY1NDC(y_lo_stat_out4);  statsAsym3Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[2]         =   fitfun3In->GetParameter(0);
  calc_eAsymInp0[2]        =   fitfun3In->GetParError(0);
  calc_AsymInChisquare[2]  =   fitfun3In->GetChisquare();
  calc_AsymInNDF[2]        =   fitfun3In->GetNDF();

  calc_AsymOutp0[2]        =   fitfun3Out->GetParameter(0);
  calc_eAsymOutp0[2]       =   fitfun3Out->GetParError(0);
  calc_AsymOutChisquare[2] =   fitfun3Out->GetChisquare();
  calc_AsymOutNDF[2]       =   fitfun3Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(3);  
  TGraphErrors * asym4GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym4in.data(),myzero.data(),asym4Erin.data());
  asym4GraphIN->SetName("asym4GraphIN");
  asym4GraphIN->SetMarkerColor(kBlue);
  asym4GraphIN->SetLineColor(kBlue);
  asym4GraphIN->SetMarkerStyle(20);
  asym4GraphIN->SetMarkerSize(markerSize[0]);
  asym4GraphIN->SetLineWidth(1);
  asym4GraphIN->Fit("fitfun4In","EMRF0Q");
  fitfun4In->SetLineStyle(1);
  fitfun4In->SetLineWidth(2);
  fitfun4In->SetLineColor(kBlue);
  fitfun4In->SetLineStyle(4);
  fitfun4In->SetLineWidth(2);


  TGraphErrors * asym4GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym4out.data(),myzero.data(),asym4Erout.data());
  asym4GraphOUT->SetName("asym4GraphOUT");
  asym4GraphOUT->SetMarkerColor(kRed);
  asym4GraphOUT->SetLineColor(kRed);
  asym4GraphOUT->SetMarkerStyle(24);
  asym4GraphOUT->SetMarkerSize(markerSize[0]);
  asym4GraphOUT->SetLineWidth(1);
  asym4GraphOUT->Fit("fitfun4Out","EMRF0Q");
  fitfun4Out->SetLineStyle(1);
  fitfun4Out->SetLineWidth(2);
  fitfun4Out->SetLineColor(kRed);
  fitfun4Out->SetLineStyle(5);
  fitfun4Out->SetLineWidth(2);


  TMultiGraph *asym4Graph = new TMultiGraph();
  asym4Graph->Add(asym4GraphIN);
  asym4Graph->Add(asym4GraphOUT);
  asym4Graph->Draw("AP");
  asym4Graph->SetTitle("");
//   asym4Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym4Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym4Graph->GetYaxis()->SetTitle(Form("%s4 Asym. [ppm]",device.Data()));
  asym4Graph->GetXaxis()->CenterTitle();
  asym4Graph->GetYaxis()->CenterTitle();
//   asym4Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym4Graph->GetXaxis()->SetLabelColor(0);
  asym4Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun4In->Draw("same");
  fitfun4Out->Draw("same");

  TPaveStats *statsAsym4In =(TPaveStats*)asym4GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym4Out =(TPaveStats*)asym4GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym4In->SetTextColor(kBlue);
  statsAsym4In->SetFillColor(kWhite); 
  statsAsym4Out->SetTextColor(kRed);
  statsAsym4Out->SetFillColor(kWhite);
  statsAsym4In->SetTextSize(0.045);
  statsAsym4Out->SetTextSize(0.045);
  statsAsym4In->SetX1NDC(x_lo_stat_in4);    statsAsym4In->SetX2NDC(x_hi_stat_in4); 
  statsAsym4In->SetY1NDC(y_lo_stat_in4);    statsAsym4In->SetY2NDC(y_hi_stat_in4);
  statsAsym4Out->SetX1NDC(x_lo_stat_out4);  statsAsym4Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym4Out->SetY1NDC(y_lo_stat_out4);  statsAsym4Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[3]         =   fitfun4In->GetParameter(0);
  calc_eAsymInp0[3]        =   fitfun4In->GetParError(0);
  calc_AsymInChisquare[3]  =   fitfun4In->GetChisquare();
  calc_AsymInNDF[3]        =   fitfun4In->GetNDF();

  calc_AsymOutp0[3]        =   fitfun4Out->GetParameter(0);
  calc_eAsymOutp0[3]       =   fitfun4Out->GetParError(0);
  calc_AsymOutChisquare[3] =   fitfun4Out->GetChisquare();
  calc_AsymOutNDF[3]       =   fitfun4Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(6);  
  TGraphErrors * asym5GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym5in.data(),myzero.data(),asym5Erin.data());
  asym5GraphIN->SetName("asym5GraphIN");
  asym5GraphIN->SetMarkerColor(kBlue);
  asym5GraphIN->SetLineColor(kBlue);
  asym5GraphIN->SetMarkerStyle(20);
  asym5GraphIN->SetMarkerSize(markerSize[0]);
  asym5GraphIN->SetLineWidth(1);
  asym5GraphIN->Fit("fitfun5In","EMRF0Q");
  fitfun5In->SetLineStyle(1);
  fitfun5In->SetLineWidth(2);
  fitfun5In->SetLineColor(kBlue);
  fitfun5In->SetLineStyle(4);
  fitfun5In->SetLineWidth(2);


  TGraphErrors * asym5GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym5out.data(),myzero.data(),asym5Erout.data());
  asym5GraphOUT->SetName("asym5GraphOUT");
  asym5GraphOUT->SetMarkerColor(kRed);
  asym5GraphOUT->SetLineColor(kRed);
  asym5GraphOUT->SetMarkerStyle(24);
  asym5GraphOUT->SetMarkerSize(markerSize[0]);
  asym5GraphOUT->SetLineWidth(1);
  asym5GraphOUT->Fit("fitfun5Out","EMRF0Q");
  fitfun5Out->SetLineStyle(1);
  fitfun5Out->SetLineWidth(2);
  fitfun5Out->SetLineColor(kRed);
  fitfun5Out->SetLineStyle(5);
  fitfun5Out->SetLineWidth(2);


  TMultiGraph *asym5Graph = new TMultiGraph();
  asym5Graph->Add(asym5GraphIN);
  asym5Graph->Add(asym5GraphOUT);
  asym5Graph->Draw("AP");
  asym5Graph->SetTitle("");
//   asym5Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym5Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym5Graph->GetYaxis()->SetTitle(Form("%s5 Asym. [ppm]",device.Data()));
  asym5Graph->GetXaxis()->CenterTitle();
  asym5Graph->GetYaxis()->CenterTitle();
//   asym5Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym5Graph->GetXaxis()->SetLabelColor(0);
  asym5Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun5In->Draw("same");
  fitfun5Out->Draw("same");

  TPaveStats *statsAsym5In =(TPaveStats*)asym5GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym5Out =(TPaveStats*)asym5GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym5In->SetTextColor(kBlue);
  statsAsym5In->SetFillColor(kWhite); 
  statsAsym5Out->SetTextColor(kRed);
  statsAsym5Out->SetFillColor(kWhite);
  statsAsym5In->SetTextSize(0.045);
  statsAsym5Out->SetTextSize(0.045);
  statsAsym5In->SetX1NDC(x_lo_stat_in4);    statsAsym5In->SetX2NDC(x_hi_stat_in4); 
  statsAsym5In->SetY1NDC(y_lo_stat_in4);    statsAsym5In->SetY2NDC(y_hi_stat_in4);
  statsAsym5Out->SetX1NDC(x_lo_stat_out4);  statsAsym5Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym5Out->SetY1NDC(y_lo_stat_out4);  statsAsym5Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[4]         =   fitfun5In->GetParameter(0);
  calc_eAsymInp0[4]        =   fitfun5In->GetParError(0);
  calc_AsymInChisquare[4]  =   fitfun5In->GetChisquare();
  calc_AsymInNDF[4]        =   fitfun5In->GetNDF();

  calc_AsymOutp0[4]        =   fitfun5Out->GetParameter(0);
  calc_eAsymOutp0[4]       =   fitfun5Out->GetParError(0);
  calc_AsymOutChisquare[4] =   fitfun5Out->GetChisquare();
  calc_AsymOutNDF[4]       =   fitfun5Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(9);  
  TGraphErrors * asym6GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym6in.data(),myzero.data(),asym6Erin.data());
  asym6GraphIN->SetName("asym6GraphIN");
  asym6GraphIN->SetMarkerColor(kBlue);
  asym6GraphIN->SetLineColor(kBlue);
  asym6GraphIN->SetMarkerStyle(20);
  asym6GraphIN->SetMarkerSize(markerSize[0]);
  asym6GraphIN->SetLineWidth(1);
  asym6GraphIN->Fit("fitfun6In","EMRF0Q");
  fitfun6In->SetLineStyle(1);
  fitfun6In->SetLineWidth(2);
  fitfun6In->SetLineColor(kBlue);
  fitfun6In->SetLineStyle(4);
  fitfun6In->SetLineWidth(2);


  TGraphErrors * asym6GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym6out.data(),myzero.data(),asym6Erout.data());
  asym6GraphOUT->SetName("asym6GraphOUT");
  asym6GraphOUT->SetMarkerColor(kRed);
  asym6GraphOUT->SetLineColor(kRed);
  asym6GraphOUT->SetMarkerStyle(24);
  asym6GraphOUT->SetMarkerSize(markerSize[0]);
  asym6GraphOUT->SetLineWidth(1);
  asym6GraphOUT->Fit("fitfun6Out","EMRF0Q");
  fitfun6Out->SetLineStyle(1);
  fitfun6Out->SetLineWidth(2);
  fitfun6Out->SetLineColor(kRed);
  fitfun6Out->SetLineStyle(5);
  fitfun6Out->SetLineWidth(2);

  TMultiGraph *asym6Graph = new TMultiGraph();
  asym6Graph->Add(asym6GraphIN);
  asym6Graph->Add(asym6GraphOUT);
  asym6Graph->Draw("AP");
  asym6Graph->SetTitle("");
//   asym6Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym6Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym6Graph->GetYaxis()->SetTitle(Form("%s6 Asym. [ppm]",device.Data()));
  asym6Graph->GetXaxis()->CenterTitle();
  asym6Graph->GetYaxis()->CenterTitle();
//   asym6Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym6Graph->GetXaxis()->SetLabelColor(0);
  asym6Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun6In->Draw("same");
  fitfun6Out->Draw("same");

  TPaveStats *statsAsym6In =(TPaveStats*)asym6GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym6Out =(TPaveStats*)asym6GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym6In->SetTextColor(kBlue);
  statsAsym6In->SetFillColor(kWhite); 
  statsAsym6Out->SetTextColor(kRed);
  statsAsym6Out->SetFillColor(kWhite);
  statsAsym6In->SetTextSize(0.045);
  statsAsym6Out->SetTextSize(0.045);
  statsAsym6In->SetX1NDC(x_lo_stat_in4);    statsAsym6In->SetX2NDC(x_hi_stat_in4); 
  statsAsym6In->SetY1NDC(y_lo_stat_in4);    statsAsym6In->SetY2NDC(y_hi_stat_in4);
  statsAsym6Out->SetX1NDC(x_lo_stat_out4);  statsAsym6Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym6Out->SetY1NDC(y_lo_stat_out4);  statsAsym6Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[5]         =   fitfun6In->GetParameter(0);
  calc_eAsymInp0[5]        =   fitfun6In->GetParError(0);
  calc_AsymInChisquare[5]  =   fitfun6In->GetChisquare();
  calc_AsymInNDF[5]        =   fitfun6In->GetNDF();

  calc_AsymOutp0[5]        =   fitfun6Out->GetParameter(0);
  calc_eAsymOutp0[5]       =   fitfun6Out->GetParError(0);
  calc_AsymOutChisquare[5] =   fitfun6Out->GetChisquare();
  calc_AsymOutNDF[5]       =   fitfun6Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(8);  
  TGraphErrors * asym7GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym7in.data(),myzero.data(),asym7Erin.data());
  asym7GraphIN->SetName("asym7GraphIN");
  asym7GraphIN->SetMarkerColor(kBlue);
  asym7GraphIN->SetLineColor(kBlue);
  asym7GraphIN->SetMarkerStyle(20);
  asym7GraphIN->SetMarkerSize(markerSize[0]);
  asym7GraphIN->SetLineWidth(1);
  asym7GraphIN->Fit("fitfun7In","EMRF0Q");
  fitfun7In->SetLineStyle(1);
  fitfun7In->SetLineWidth(2);
  fitfun7In->SetLineColor(kBlue);
  fitfun7In->SetLineStyle(4);
  fitfun7In->SetLineWidth(2);


  TGraphErrors * asym7GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym7out.data(),myzero.data(),asym7Erout.data());
  asym7GraphOUT->SetName("asym7GraphOUT");
  asym7GraphOUT->SetMarkerColor(kRed);
  asym7GraphOUT->SetLineColor(kRed);
  asym7GraphOUT->SetMarkerStyle(24);
  asym7GraphOUT->SetMarkerSize(markerSize[0]);
  asym7GraphOUT->SetLineWidth(1);
  asym7GraphOUT->Fit("fitfun7Out","EMRF0Q");
  fitfun7Out->SetLineStyle(1);
  fitfun7Out->SetLineWidth(2);
  fitfun7Out->SetLineColor(kRed);
  fitfun7Out->SetLineStyle(5);
  fitfun7Out->SetLineWidth(2);

  TMultiGraph *asym7Graph = new TMultiGraph();
  asym7Graph->Add(asym7GraphIN);
  asym7Graph->Add(asym7GraphOUT);
  asym7Graph->Draw("AP");
  asym7Graph->SetTitle("");
//   asym7Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym7Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym7Graph->GetYaxis()->SetTitle(Form("%s7 Asym. [ppm]",device.Data()));
  asym7Graph->GetXaxis()->CenterTitle();
  asym7Graph->GetYaxis()->CenterTitle();
//   asym7Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym7Graph->GetXaxis()->SetLabelColor(0);
  asym7Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun7In->Draw("same");
  fitfun7Out->Draw("same");

  TPaveStats *statsAsym7In =(TPaveStats*)asym7GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym7Out =(TPaveStats*)asym7GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym7In->SetTextColor(kBlue);
  statsAsym7In->SetFillColor(kWhite); 
  statsAsym7Out->SetTextColor(kRed);
  statsAsym7Out->SetFillColor(kWhite);
  statsAsym7In->SetTextSize(0.045);
  statsAsym7Out->SetTextSize(0.045);
  statsAsym7In->SetX1NDC(x_lo_stat_in4);    statsAsym7In->SetX2NDC(x_hi_stat_in4); 
  statsAsym7In->SetY1NDC(y_lo_stat_in4);    statsAsym7In->SetY2NDC(y_hi_stat_in4);
  statsAsym7Out->SetX1NDC(x_lo_stat_out4);  statsAsym7Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym7Out->SetY1NDC(y_lo_stat_out4);  statsAsym7Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[6]         =   fitfun7In->GetParameter(0);
  calc_eAsymInp0[6]        =   fitfun7In->GetParError(0);
  calc_AsymInChisquare[6]  =   fitfun7In->GetChisquare();
  calc_AsymInNDF[6]        =   fitfun7In->GetNDF();

  calc_AsymOutp0[6]        =   fitfun7Out->GetParameter(0);
  calc_eAsymOutp0[6]       =   fitfun7Out->GetParError(0);
  calc_AsymOutChisquare[6] =   fitfun7Out->GetChisquare();
  calc_AsymOutNDF[6]       =   fitfun7Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(7);  
  TGraphErrors * asym8GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym8in.data(),myzero.data(),asym8Erin.data());
  asym8GraphIN->SetName("asym8GraphIN");
  asym8GraphIN->SetMarkerColor(kBlue);
  asym8GraphIN->SetLineColor(kBlue);
  asym8GraphIN->SetMarkerStyle(20);
  asym8GraphIN->SetMarkerSize(markerSize[0]);
  asym8GraphIN->SetLineWidth(1);
  asym8GraphIN->Fit("fitfun8In","EMRF0Q");
  fitfun8In->SetLineStyle(1);
  fitfun8In->SetLineWidth(2);
  fitfun8In->SetLineColor(kBlue);
  fitfun8In->SetLineStyle(4);
  fitfun8In->SetLineWidth(2);


  TGraphErrors * asym8GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym8out.data(),myzero.data(),asym8Erout.data());
  asym8GraphOUT->SetName("asym8GraphOUT");
  asym8GraphOUT->SetMarkerColor(kRed);
  asym8GraphOUT->SetLineColor(kRed);
  asym8GraphOUT->SetMarkerStyle(24);
  asym8GraphOUT->SetMarkerSize(markerSize[0]);
  asym8GraphOUT->SetLineWidth(1);
  asym8GraphOUT->Fit("fitfun8Out","EMRF0Q");
  fitfun8Out->SetLineStyle(1);
  fitfun8Out->SetLineWidth(2);
  fitfun8Out->SetLineColor(kRed);
  fitfun8Out->SetLineStyle(5);
  fitfun8Out->SetLineWidth(2);

  TMultiGraph *asym8Graph = new TMultiGraph();
  asym8Graph->Add(asym8GraphIN);
  asym8Graph->Add(asym8GraphOUT);
  asym8Graph->Draw("AP");
  asym8Graph->SetTitle("");
//   asym8Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym8Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym8Graph->GetYaxis()->SetTitle(Form("%s8 Asym. [ppm]",device.Data()));
  asym8Graph->GetXaxis()->CenterTitle();
  asym8Graph->GetYaxis()->CenterTitle();
//   asym8Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym8Graph->GetXaxis()->SetLabelColor(0);
  asym8Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun8In->Draw("same");
  fitfun8Out->Draw("same");

  TPaveStats *statsAsym8In =(TPaveStats*)asym8GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym8Out =(TPaveStats*)asym8GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym8In->SetTextColor(kBlue);
  statsAsym8In->SetFillColor(kWhite); 
  statsAsym8Out->SetTextColor(kRed);
  statsAsym8Out->SetFillColor(kWhite);
  statsAsym8In->SetTextSize(0.045);
  statsAsym8Out->SetTextSize(0.045);
  statsAsym8In->SetX1NDC(x_lo_stat_in4);    statsAsym8In->SetX2NDC(x_hi_stat_in4); 
  statsAsym8In->SetY1NDC(y_lo_stat_in4);    statsAsym8In->SetY2NDC(y_hi_stat_in4);
  statsAsym8Out->SetX1NDC(x_lo_stat_out4);  statsAsym8Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym8Out->SetY1NDC(y_lo_stat_out4);  statsAsym8Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[7]         =   fitfun8In->GetParameter(0);
  calc_eAsymInp0[7]        =   fitfun8In->GetParError(0);
  calc_AsymInChisquare[7]  =   fitfun8In->GetChisquare();
  calc_AsymInNDF[7]        =   fitfun8In->GetNDF();

  calc_AsymOutp0[7]        =   fitfun8Out->GetParameter(0);
  calc_eAsymOutp0[7]       =   fitfun8Out->GetParError(0);
  calc_AsymOutChisquare[7] =   fitfun8Out->GetChisquare();
  calc_AsymOutNDF[7]       =   fitfun8Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();


  pad112->cd(5);  


  Double_t octant[det];
  Double_t zero[det]={0.0};
  Double_t valuesum[det];
  Double_t valueerror[det];
  Double_t valuediff[det];
  Double_t errordiff[det];
  Double_t x[det];
  Double_t errx[det];

  Double_t valuesumopp[4];
  Double_t errorsumopp[4];
  Double_t valuediffopp[4];
  Double_t errordiffopp[4];


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
    valuesum[i]=(calc_AsymInp0[i]+calc_AsymOutp0[i])/2.0;
    valueerror[i]= (sqrt(pow(calc_eAsymInp0[i],2)+pow(calc_eAsymOutp0[i],2)))/2.0;
    valuediff[i]=((calc_AsymInp0[i]/pow(calc_eAsymInp0[i],2)) - (calc_AsymOutp0[i]/pow(calc_eAsymOutp0[i],2))) /((1/pow(calc_eAsymInp0[i],2)) + (1/pow(calc_eAsymOutp0[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(calc_eAsymInp0[i],2)))+(1/pow(calc_eAsymOutp0[i],2))));


  }

  /********************************************/

  MyfileWrite.open(Form("dirPlot/resultText/%s_%s_%s_%s_%s_%s_regression_%s_%s_asym_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()));



  for(Int_t i =0;i<4;i++){
    valuesumopp[i]= valuesum[i]+valuesum[i+4];
    errorsumopp[i]= sqrt(pow(valueerror[i],2)+pow(valueerror[i+4],2));
    valuediffopp[i]=valuediff[i]-valuediff[i+4];
    errordiffopp[i]=sqrt(pow(errordiff[i],2)+pow(errordiff[i+4],2));

  }

  for(Int_t i =0;i<det;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }


  // Draw IN values
  TGraphErrors* grp_in  = new TGraphErrors(det,octant,calc_AsymInp0,zero,calc_eAsymInp0);
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
  TGraphErrors* grp_out  = new TGraphErrors(det,octant,calc_AsymOutp0,zero,calc_eAsymOutp0);
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

  // Draw In+Out values
  TGraphErrors* grp_sum  = new TGraphErrors(det,x,valuesum,errx,valueerror);
  grp_sum ->SetName("grp_sum");
  grp_sum ->Draw("goff");
  grp_sum ->SetMarkerSize(markerSize[0]);
  //  grp_sum ->SetLineWidth(0);
  grp_sum ->SetMarkerStyle(22);
  grp_sum ->SetMarkerColor(kGreen-3);
  grp_sum ->SetLineColor(kGreen-3);
  grp_sum->Fit("pol0","B");
  TF1* fit3 = grp_sum->GetFunction("pol0");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen-2);
  fit3->SetLineStyle(2);
  fit3->SetLineWidth(3);

  // Draw In-Out
  TGraphErrors* grp_diff  = new TGraphErrors(det,x,valuediff,errx,errordiff);
  grp_diff ->SetName("grp_diff");
  grp_diff ->Draw("goff");
  grp_diff ->SetMarkerSize(markerSize[0]);
  grp_diff ->SetLineWidth(0);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta);
  grp_diff ->SetLineColor(kMagenta);
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
  grp->GetYaxis()->SetTitle(Form("%s %s Asym. [ppm]",device.Data(),deviceTitle.Data()));
  grp->GetYaxis()->CenterTitle();
//   grp->GetYaxis()->SetTitleOffset(0.7);
//   grp->GetXaxis()->SetTitleOffset(0.8);
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

  stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(0.72);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(x_lo_stat_in4); stats2->SetX2NDC(x_hi_stat_in4); stats2->SetY1NDC(0.47);stats2->SetY2NDC(0.70);
  stats3->SetX1NDC(x_lo_stat_in4); stats3->SetX2NDC(x_hi_stat_in4); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);
  stats4->SetX1NDC(x_lo_stat_in4); stats4->SetX2NDC(x_hi_stat_in4); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);


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
  legend->AddEntry(grp_in,  Form("A_{IHWP-IN} = %4.2f #pm %4.2f ppm",p0in,ep0in), "lp");
  legend->AddEntry(grp_sum, Form("A_{(IN+OUT)/2} = %4.2f #pm %4.2f ppm",p0sum,ep0sum), "lp");
  legend->AddEntry(grp_out, Form("A_{IHWP-OUT} = %4.2f #pm %4.2f ppm",p0out,ep0out), "lp");
  legend->AddEntry(grp_diff,Form("A_{measured} = %4.2f #pm %4.2f ppm",p0diff,ep0diff), "lp");
  legend->SetFillColor(0);
  legend->SetBorderSize(2);
  legend->SetTextSize(0.035);
  legend->Draw("");



  pad2->cd();

  grp->Draw("A");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s %s Asym. [ppm]",device.Data(),deviceTitle.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetXaxis()->SetNdivisions(8,0,0);
  grp->GetYaxis()->SetNdivisions(8,5,0);
  if(SCALE){
    grp->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  }
  xaxisGrp->SetLimits(0.5,8.5);

  fit3->DrawCopy("same");
  fit4->DrawCopy("same");

  stats1->SetX1NDC(x_lo_stat_in4); stats1->SetX2NDC(x_hi_stat_in4); stats1->SetY1NDC(0.72);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(x_lo_stat_in4); stats2->SetX2NDC(x_hi_stat_in4); stats2->SetY1NDC(0.47);stats2->SetY2NDC(0.70);
  stats3->SetX1NDC(x_lo_stat_in4); stats3->SetX2NDC(x_hi_stat_in4); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);
  stats4->SetX1NDC(x_lo_stat_in4); stats4->SetX2NDC(x_hi_stat_in4); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);

  legend->Draw("");



  gPad->Modified();
  pad112->Modified();
  c11->Modified();
  gPad->Update();



  TString saveAllPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_all_plot_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());
  TString saveIndividualPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_individual_plot_%s"
				      ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
				      ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());



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

//   MyfileWrite<<Form("%4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.0f %4.4f",p0diff,ep0diff,p1diff,ep1diff,p2diff,ep2diff,Chidiff,NDFdiff,Probdiff)<<endl;
  MyfileWrite<<p0diff<<" "<<ep0diff<<" "<<p1diff<<" "<<ep1diff<<" "<<p2diff<<" "<<ep2diff<<" "<<Chidiff<<" "<<NDFdiff<<" "<<Probdiff<<endl;
  MyfileWrite.close();

  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


