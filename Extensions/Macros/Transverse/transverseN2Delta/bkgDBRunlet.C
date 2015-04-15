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


  Bool_t USLUMI_PLOT = 0;  
  Bool_t USLUMI_PLOT2 = 1;
  Bool_t BKG_PLOT = 0;

  Bool_t FIGURE = 0;

//   TString database="qw_run2_pass1";
//   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";

  TString output;
  TString outputAvg;

  std::ofstream Myfile3;

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
//   Int_t canvasSize[2] ={1200,650};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
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
  Double_t YRange[2];

  device = "USLumi"; deviceTitle = "Sum"; deviceName = "Sum"; YRange[0]=-30; YRange[1]=30;


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

//   int position1[8] = { 4, 1, 2, 3, 6, 9, 8, 7 };

  Int_t counter11=0;
  ifstream runletAsym;
  const Int_t NUM11 = 500;
  Double_t octant11[NUM11],run_number11[NUM11],asym11[NUM11],asymError11[NUM11],asymRMS11[NUM11];
  TString hwp11[NUM11];

  std::vector <Double_t> run1in,run2in,run3in,run4in,run5in,run6in,run7in,run8in,run9in;
  std::vector <Double_t> run1out,run2out,run3out,run4out,run5out,run6out,run7out,run8out,run9out;
  std::vector <Double_t> asym1in,asym2in,asym3in,asym4in,asym5in,asym6in,asym7in,asym8in,asym9in;
  std::vector <Double_t> asym1out,asym2out,asym3out,asym4out,asym5out,asym6out,asym7out,asym8out,asym9out;
  std::vector <Double_t> asym1Erin,asym2Erin,asym3Erin,asym4Erin,asym5Erin,asym6Erin,asym7Erin,asym8Erin,asym9Erin;
  std::vector <Double_t> asym1Erout,asym2Erout,asym3Erout,asym4Erout,asym5Erout,asym6Erout,asym7Erout,asym8Erout,asym9Erout;
  std::vector <Double_t> myzero;

  std::vector <Double_t> run31in,run32in,run33in,run34in,run35in,run36in,run37in,run38in,run39in;
  std::vector <Double_t> run31out,run32out,run33out,run34out,run35out,run36out,run37out,run38out,run39out;
  std::vector <Double_t> asym31in,asym32in,asym33in,asym34in,asym35in,asym36in,asym37in,asym38in,asym39in;
  std::vector <Double_t> asym31out,asym32out,asym33out,asym34out,asym35out,asym36out,asym37out,asym38out,asym39out;
  std::vector <Double_t> asym31Erin,asym32Erin,asym33Erin,asym34Erin,asym35Erin,asym36Erin,asym37Erin,asym38Erin,asym39Erin;
  std::vector <Double_t> asym31Erout,asym32Erout,asym33Erout,asym34Erout,asym35Erout,asym36Erout,asym37Erout,asym38Erout,asym39Erout;


  const Int_t det = 9;
  Double_t calc_AsymInp0[det],calc_eAsymInp0[det],calc_AsymInChisquare[det],calc_AsymInNDF[det];
  Double_t calc_AsymOutp0[det],calc_eAsymOutp0[det],calc_AsymOutChisquare[det],calc_AsymOutNDF[det];

  const Int_t det3 = 5;
  Double_t calc3_AsymInp0[det3],calc3_eAsymInp0[det3],calc3_AsymInChisquare[det3],calc3_AsymInNDF[det3];
  Double_t calc3_AsymOutp0[det3],calc3_eAsymOutp0[det3],calc3_AsymOutChisquare[det3],calc3_AsymOutNDF[det3];

  Double_t calc33_AsymInp0[det3],calc33_eAsymInp0[det3],calc33_AsymInChisquare[det3],calc33_AsymInNDF[det3];
  Double_t calc33_AsymOutp0[det3],calc33_eAsymOutp0[det3],calc33_AsymOutChisquare[det3],calc33_AsymOutNDF[det3];
  Double_t calc35_AsymInp0[det3],calc35_eAsymInp0[det3],calc35_AsymInChisquare[det3],calc35_AsymInNDF[det3];
  Double_t calc35_AsymOutp0[det3],calc35_eAsymOutp0[det3],calc35_AsymOutChisquare[det3],calc35_AsymOutNDF[det3];
  Double_t calc37_AsymInp0[det3],calc37_eAsymInp0[det3],calc37_AsymInChisquare[det3],calc37_AsymInNDF[det3];
  Double_t calc37_AsymOutp0[det3],calc37_eAsymOutp0[det3],calc37_AsymOutChisquare[det3],calc37_AsymOutNDF[det3];



  runletAsym.open(Form("dirPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_%s_asym_%s.txt"
			     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			     ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

  cout<<Form("dirPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_%s_asym_%s.txt"
	     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
	     ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data())<<endl;

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
	if(octant11[counter11]==3) {
	  run2in.push_back(run_number11[counter11]);
	  asym2in.push_back(asym11[counter11]); 
	  asym2Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==5) {
	  run3in.push_back(run_number11[counter11]);
	  asym3in.push_back(asym11[counter11]); 
	  asym3Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==7) {
	  run4in.push_back(run_number11[counter11]);
	  asym4in.push_back(asym11[counter11]); 
	  asym4Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==-1) {
	  run5in.push_back(run_number11[counter11]);
	  asym5in.push_back(asym11[counter11]); 
	  asym5Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==-3) {
	  run6in.push_back(run_number11[counter11]);
	  asym6in.push_back(asym11[counter11]); 
	  asym6Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==-5) {
	  run7in.push_back(run_number11[counter11]);
	  asym7in.push_back(asym11[counter11]); 
	  asym7Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==-7) {
	  run8in.push_back(run_number11[counter11]);
	  asym8in.push_back(asym11[counter11]); 
	  asym8Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==0) {
	  run9in.push_back(run_number11[counter11]);
	  asym9in.push_back(asym11[counter11]); 
	  asym9Erin.push_back(asymError11[counter11]);
	  myzero.push_back(0.0);
	}
	
	
      }



      if(hwp11[counter11]== "out"){
	
	if(octant11[counter11]==1) {
	  run1out.push_back(run_number11[counter11]);
	  asym1out.push_back(asym11[counter11]); 
	  asym1Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==3) {
	  run2out.push_back(run_number11[counter11]);
	  asym2out.push_back(asym11[counter11]); 
	  asym2Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==5) {
	  run3out.push_back(run_number11[counter11]);
	  asym3out.push_back(asym11[counter11]); 
	  asym3Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==7) {
	  run4out.push_back(run_number11[counter11]);
	  asym4out.push_back(asym11[counter11]); 
	  asym4Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==-1) {
	  run5out.push_back(run_number11[counter11]);
	  asym5out.push_back(asym11[counter11]); 
	  asym5Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==-3) {
	  run6out.push_back(run_number11[counter11]);
	  asym6out.push_back(asym11[counter11]); 
	  asym6Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==-5) {
	  run7out.push_back(run_number11[counter11]);
	  asym7out.push_back(asym11[counter11]); 
	  asym7Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==-7) {
	  run8out.push_back(run_number11[counter11]);
	  asym8out.push_back(asym11[counter11]); 
	  asym8Erout.push_back(asymError11[counter11]);
	}
	if(octant11[counter11]==0) {
	  run9out.push_back(run_number11[counter11]);
	  asym9out.push_back(asym11[counter11]); 
	  asym9Erout.push_back(asymError11[counter11]);
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




  Int_t counter13=0;
  ifstream runletAsym13;
  const Int_t NUM13 = 500;
  Double_t octant13[NUM13],run_number13[NUM13],asym13[NUM13],asymError13[NUM13],asymRMS13[NUM13];
  TString hwp13[NUM13];



  runletAsym13.open(Form("dirPlot/%s_%s_%s_%s_%sSum_%s_regression_%s_%s_%s_asym_%s.txt"
			     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			     ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

  cout<<Form("dirPlot/%s_%s_%s_%s_%sSum_%s_regression_%s_%s_%s_asym_%s.txt"
	     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
	     ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data())<<endl;

   if (runletAsym13.is_open()) {
   
    while(1) {
      //       zero13[counter13]=0;
      
      runletAsym13 >> octant13[counter13] >> run_number13[counter13] >> hwp13[counter13]
		   >> asym13[counter13]   >> asymError13[counter13]  >> asymRMS13[counter13] ;
      
      if (!runletAsym13.good()) break;
      
      if(hwp13[counter13]== "in"){
	
	if(octant13[counter13]==1) {
	  run31in.push_back(run_number13[counter13]);
	  asym31in.push_back(asym13[counter13]); 
	  asym31Erin.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==3) {
	  run32in.push_back(run_number13[counter13]);
	  asym32in.push_back(asym13[counter13]); 
	  asym32Erin.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==5) {
	  run33in.push_back(run_number13[counter13]);
	  asym33in.push_back(asym13[counter13]); 
	  asym33Erin.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==7) {
	  run34in.push_back(run_number13[counter13]);
	  asym34in.push_back(asym13[counter13]); 
	  asym34Erin.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==0) {
	  run39in.push_back(run_number13[counter13]);
	  asym39in.push_back(asym13[counter13]); 
	  asym39Erin.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	
	
      }



      if(hwp13[counter13]== "out"){
	
	if(octant13[counter13]==1) {
	  run31out.push_back(run_number13[counter13]);
	  asym31out.push_back(asym13[counter13]); 
	  asym31Erout.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==3) {
	  run32out.push_back(run_number13[counter13]);
	  asym32out.push_back(asym13[counter13]); 
	  asym32Erout.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==5) {
	  run33out.push_back(run_number13[counter13]);
	  asym33out.push_back(asym13[counter13]); 
	  asym33Erout.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==7) {
	  run34out.push_back(run_number13[counter13]);
	  asym34out.push_back(asym13[counter13]); 
	  asym34Erout.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	if(octant13[counter13]==0) {
	  run39out.push_back(run_number13[counter13]);
	  asym39out.push_back(asym13[counter13]); 
	  asym39Erout.push_back(asymError13[counter13]);
	  myzero.push_back(0.0);
	}
	
	
      }
      //       else{ cout<<green<<"Check Half Wave Plate settings "<<normal<<endl;}

      
      counter13++;
    }
   }
   else {
     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
     return -1;
   }
   runletAsym13.close();












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
    cout<<Form("i:%d\trun1:%5.2f\tasym1IN11:%f",i,run1in[i],asym1in[i])<<endl;
  }
  cout<<"run1out: "<<run1out.size()<<endl;
  for ( Int_t i=0; i<run1out.size(); i++) {
    cout<<Form("i:%d\trun1:%5.2f\tasym1OUT11:%f",i,run1out[i],asym1out[i])<<endl;
  }
  cout<<"dummyArraySize:"<< dummyArraySize<<endl;


//   TF1* fitfunIn  = new TF1("fitfunIn","pol0",dummyINaxis[0],dummyINaxis[(Int_t)run1in.size()]);
//   TF1* fitfunIn  = new TF1("fitfunIn","pol0",-0.7,(Int_t)run1in.size());
//   TF1* fitfunOut = new TF1("fitfunOut","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);

  TF1* fitfun1In  = new TF1("fitfun1In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun1Out = new TF1("fitfun1Out","pol0",run1out.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
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
  TF1* fitfun9In  = new TF1("fitfun9In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun9Out = new TF1("fitfun9Out","pol0",run1in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);


  TF1* fitfun31In  = new TF1("fitfun31In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun31Out = new TF1("fitfun31Out","pol0",run1out.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun33In  = new TF1("fitfun33In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun33Out = new TF1("fitfun33Out","pol0",run31in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun35In  = new TF1("fitfun35In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun35Out = new TF1("fitfun35Out","pol0",run31in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun37In  = new TF1("fitfun37In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun37Out = new TF1("fitfun37Out","pol0",run31in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);
  TF1* fitfun39In  = new TF1("fitfun39In","pol0",-0.7,(Int_t)run1in.size());
  TF1* fitfun39Out = new TF1("fitfun39Out","pol0",run31in.size()-0.7,dummyOUTaxis[(Int_t)run1out.size()]);

  cout<<red<<__LINE__<<normal<<endl;

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
  fitfun9In->SetParNames("A_{IN}");
  fitfun9Out->SetParNames("A_{OUT}");

  fitfun31In->SetParNames("A_{IN}");
  fitfun31Out->SetParNames("A_{OUT}");
  fitfun33In->SetParNames("A_{IN}");
  fitfun33Out->SetParNames("A_{OUT}");
  fitfun35In->SetParNames("A_{IN}");
  fitfun35Out->SetParNames("A_{OUT}");
  fitfun37In->SetParNames("A_{IN}");
  fitfun37Out->SetParNames("A_{OUT}");
  fitfun39In->SetParNames("A_{IN}");
  fitfun39Out->SetParNames("A_{OUT}");


  if(USLUMI_PLOT){

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

  cout<<red<<__LINE__<<normal<<endl;
//     di = i+1;

//   pad112->cd(position1[i]);
//   pad112->cd(1);



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
  asym1GraphIN->Fit("fitfun1In","E M R F 0 Q");
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
  asym1GraphOUT->Fit("fitfun1Out","E M R F 0 Q");
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
  asym1Graph->GetYaxis()->SetTitle(Form("%s1 Pos. Asym. [ppm]",device.Data()));
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


//   TLatex* tAsymIn = new TLatex(5,-40,Form("%5.0f",run1in[0]));
  TLatex* tAsymIn = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tAsymIn->SetTextSize(0.06);
  tAsymIn->SetTextColor(kBlue);
  tAsymIn->Draw();
//   TLatex* tAsymOut = new TLatex(17,-40,Form("%5.0f",run1out[0]));
  TLatex* tAsymOut = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tAsymOut->SetTextSize(0.06);
  tAsymOut->SetTextColor(kRed);
  tAsymOut->Draw();

  Double_t valuesum0,valueerror0,valuediff0,errordiff0;
  valuesum0    = (calc_AsymInp0[0]+calc_AsymOutp0[0])/2.0;
  valueerror0  = (sqrt(pow(calc_eAsymInp0[0],2)+pow(calc_eAsymOutp0[0],2)))/2.0;
  valuediff0   = ((calc_AsymInp0[0]/pow(calc_eAsymInp0[0],2)) - (calc_AsymOutp0[0]/pow(calc_eAsymOutp0[0],2))) /((1/pow(calc_eAsymInp0[0],2)) + (1/pow(calc_eAsymOutp0[0],2)));
  errordiff0   = sqrt(1/((1/(pow(calc_eAsymInp0[0],2)))+(1/pow(calc_eAsymOutp0[0],2))));

  TLegend *legend0 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend0->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum0,valueerror0), "");
  legend0->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff0,errordiff0), "");
  legend0->SetFillColor(0);
  legend0->SetTextAlign(31);
  legend0->SetBorderSize(2);
  legend0->SetTextSize(0.040);
  legend0->Draw("");

  cout<<red<<__LINE__<<normal<<endl;

  pad112->cd(2);
  TGraphErrors * asym2GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym2in.data(),myzero.data(),asym2Erin.data());
  asym2GraphIN->SetName("asym2GraphIN");
  asym2GraphIN->SetMarkerColor(kBlue);
  asym2GraphIN->SetLineColor(kBlue);
  asym2GraphIN->SetMarkerStyle(20);
  asym2GraphIN->SetMarkerSize(markerSize[0]);
  asym2GraphIN->SetLineWidth(1);
  asym2GraphIN->Fit("fitfun2In","E M R F 0 Q");
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
  asym2GraphOUT->Fit("fitfun2Out","E M R F 0 Q");
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
  asym2Graph->GetYaxis()->SetTitle(Form("%s3 Pos. Asym. [ppm]",device.Data()));
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

  Double_t valuesum1,valueerror1,valuediff1,errordiff1;
  valuesum1    = (calc_AsymInp0[1]+calc_AsymOutp0[1])/2.0;
  valueerror1  = (sqrt(pow(calc_eAsymInp0[1],2)+pow(calc_eAsymOutp0[1],2)))/2.0;
  valuediff1   = ((calc_AsymInp0[1]/pow(calc_eAsymInp0[1],2)) - (calc_AsymOutp0[1]/pow(calc_eAsymOutp0[1],2))) /((1/pow(calc_eAsymInp0[1],2)) + (1/pow(calc_eAsymOutp0[1],2)));
  errordiff1   = sqrt(1/((1/(pow(calc_eAsymInp0[1],2)))+(1/pow(calc_eAsymOutp0[1],2))));

  TLegend *legend1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend1->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum1,valueerror1), "");
  legend1->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff1,errordiff1), "");
  legend1->SetFillColor(0);
  legend1->SetTextAlign(31);
  legend1->SetBorderSize(2);
  legend1->SetTextSize(0.040);
  legend1->Draw("");


  pad112->cd(6);  
  TGraphErrors * asym3GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym3in.data(),myzero.data(),asym3Erin.data());
  asym3GraphIN->SetName("asym3GraphIN");
  asym3GraphIN->SetMarkerColor(kBlue);
  asym3GraphIN->SetLineColor(kBlue);
  asym3GraphIN->SetMarkerStyle(20);
  asym3GraphIN->SetMarkerSize(markerSize[0]);
  asym3GraphIN->SetLineWidth(1);
  asym3GraphIN->Fit("fitfun3In","E M R F 0 Q");
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
  asym3GraphOUT->Fit("fitfun3Out","E M R F 0 Q");
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
  asym3Graph->GetYaxis()->SetTitle(Form("%s5 Pos. Asym. [ppm]",device.Data()));
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

  Double_t valuesum2,valueerror2,valuediff2,errordiff2;
  valuesum2    = (calc_AsymInp0[2]+calc_AsymOutp0[2])/2.0;
  valueerror2  = (sqrt(pow(calc_eAsymInp0[2],2)+pow(calc_eAsymOutp0[2],2)))/2.0;
  valuediff2   = ((calc_AsymInp0[2]/pow(calc_eAsymInp0[2],2)) - (calc_AsymOutp0[2]/pow(calc_eAsymOutp0[2],2))) /((1/pow(calc_eAsymInp0[2],2)) + (1/pow(calc_eAsymOutp0[2],2)));
  errordiff2   = sqrt(1/((1/(pow(calc_eAsymInp0[2],2)))+(1/pow(calc_eAsymOutp0[2],2))));

  TLegend *legend2 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend2->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum2,valueerror2), "");
  legend2->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff2,errordiff2), "");
  legend2->SetFillColor(0);
  legend2->SetTextAlign(31);
  legend2->SetBorderSize(2);
  legend2->SetTextSize(0.040);
  legend2->Draw("");


  pad112->cd(8);  
  TGraphErrors * asym4GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym4in.data(),myzero.data(),asym4Erin.data());
  asym4GraphIN->SetName("asym4GraphIN");
  asym4GraphIN->SetMarkerColor(kBlue);
  asym4GraphIN->SetLineColor(kBlue);
  asym4GraphIN->SetMarkerStyle(20);
  asym4GraphIN->SetMarkerSize(markerSize[0]);
  asym4GraphIN->SetLineWidth(1);
  asym4GraphIN->Fit("fitfun4In","E M R F 0 Q");
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
  asym4GraphOUT->Fit("fitfun4Out","E M R F 0 Q");
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
  asym4Graph->GetYaxis()->SetTitle(Form("%s7 Pos. Asym. [ppm]",device.Data()));
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

  Double_t valuesum3,valueerror3,valuediff3,errordiff3;
  valuesum3    = (calc_AsymInp0[3]+calc_AsymOutp0[3])/2.0;
  valueerror3  = (sqrt(pow(calc_eAsymInp0[3],2)+pow(calc_eAsymOutp0[3],2)))/2.0;
  valuediff3   = ((calc_AsymInp0[3]/pow(calc_eAsymInp0[3],2)) - (calc_AsymOutp0[3]/pow(calc_eAsymOutp0[3],2))) /((1/pow(calc_eAsymInp0[3],2)) + (1/pow(calc_eAsymOutp0[3],2)));
  errordiff3   = sqrt(1/((1/(pow(calc_eAsymInp0[3],2)))+(1/pow(calc_eAsymOutp0[3],2))));

  TLegend *legend3 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend3->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum3,valueerror3), "");
  legend3->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff3,errordiff3), "");
  legend3->SetFillColor(0);
  legend3->SetTextAlign(31);
  legend3->SetBorderSize(2);
  legend3->SetTextSize(0.040);
  legend3->Draw("");


  pad112->cd(1);  
  TGraphErrors * asym5GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym5in.data(),myzero.data(),asym5Erin.data());
  asym5GraphIN->SetName("asym5GraphIN");
  asym5GraphIN->SetMarkerColor(kBlue);
  asym5GraphIN->SetLineColor(kBlue);
  asym5GraphIN->SetMarkerStyle(22);
  asym5GraphIN->SetMarkerSize(markerSize[0]);
  asym5GraphIN->SetLineWidth(1);
  asym5GraphIN->Fit("fitfun5In","E M R F 0 Q");
  fitfun5In->SetLineStyle(1);
  fitfun5In->SetLineWidth(2);
  fitfun5In->SetLineColor(kBlue);
  fitfun5In->SetLineStyle(4);
  fitfun5In->SetLineWidth(2);


  TGraphErrors * asym5GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym5out.data(),myzero.data(),asym5Erout.data());
  asym5GraphOUT->SetName("asym5GraphOUT");
  asym5GraphOUT->SetMarkerColor(kRed);
  asym5GraphOUT->SetLineColor(kRed);
  asym5GraphOUT->SetMarkerStyle(26);
  asym5GraphOUT->SetMarkerSize(markerSize[0]);
  asym5GraphOUT->SetLineWidth(1);
  asym5GraphOUT->Fit("fitfun5Out","E M R F 0 Q");
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
  asym5Graph->GetYaxis()->SetTitle(Form("%s1 Neg. Asym. [ppm]",device.Data()));
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

  Double_t valuesum4,valueerror4,valuediff4,errordiff4;
  valuesum4    = (calc_AsymInp0[4]+calc_AsymOutp0[4])/2.0;
  valueerror4  = (sqrt(pow(calc_eAsymInp0[4],2)+pow(calc_eAsymOutp0[4],2)))/2.0;
  valuediff4   = ((calc_AsymInp0[4]/pow(calc_eAsymInp0[4],2)) - (calc_AsymOutp0[4]/pow(calc_eAsymOutp0[4],2))) /((1/pow(calc_eAsymInp0[4],2)) + (1/pow(calc_eAsymOutp0[4],2)));
  errordiff4   = sqrt(1/((1/(pow(calc_eAsymInp0[4],2)))+(1/pow(calc_eAsymOutp0[4],2))));

  TLegend *legend4 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend4->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum4,valueerror4), "");
  legend4->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff4,errordiff4), "");
  legend4->SetFillColor(0);
  legend4->SetTextAlign(31);
  legend4->SetBorderSize(2);
  legend4->SetTextSize(0.040);
  legend4->Draw("");


  pad112->cd(3);
  TGraphErrors * asym6GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym6in.data(),myzero.data(),asym6Erin.data());
  asym6GraphIN->SetName("asym6GraphIN");
  asym6GraphIN->SetMarkerColor(kBlue);
  asym6GraphIN->SetLineColor(kBlue);
  asym6GraphIN->SetMarkerStyle(22);
  asym6GraphIN->SetMarkerSize(markerSize[0]);
  asym6GraphIN->SetLineWidth(1);
  asym6GraphIN->Fit("fitfun6In","E M R F 0 Q");
  fitfun6In->SetLineStyle(1);
  fitfun6In->SetLineWidth(2);
  fitfun6In->SetLineColor(kBlue);
  fitfun6In->SetLineStyle(4);
  fitfun6In->SetLineWidth(2);


  TGraphErrors * asym6GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym6out.data(),myzero.data(),asym6Erout.data());
  asym6GraphOUT->SetName("asym6GraphOUT");
  asym6GraphOUT->SetMarkerColor(kRed);
  asym6GraphOUT->SetLineColor(kRed);
  asym6GraphOUT->SetMarkerStyle(26);
  asym6GraphOUT->SetMarkerSize(markerSize[0]);
  asym6GraphOUT->SetLineWidth(1);
  asym6GraphOUT->Fit("fitfun6Out","E M R F 0 Q");
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
  asym6Graph->GetYaxis()->SetTitle(Form("%s3 Neg. Asym. [ppm]",device.Data()));
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

  Double_t valuesum5,valueerror5,valuediff5,errordiff5;
  valuesum5    = (calc_AsymInp0[5]+calc_AsymOutp0[5])/2.0;
  valueerror5  = (sqrt(pow(calc_eAsymInp0[5],2)+pow(calc_eAsymOutp0[5],2)))/2.0;
  valuediff5   = ((calc_AsymInp0[5]/pow(calc_eAsymInp0[5],2)) - (calc_AsymOutp0[5]/pow(calc_eAsymOutp0[5],2))) /((1/pow(calc_eAsymInp0[5],2)) + (1/pow(calc_eAsymOutp0[5],2)));
  errordiff5   = sqrt(1/((1/(pow(calc_eAsymInp0[5],2)))+(1/pow(calc_eAsymOutp0[5],2))));

  TLegend *legend5 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend5->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum5,valueerror5), "");
  legend5->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff5,errordiff5), "");
  legend5->SetFillColor(0);
  legend5->SetTextAlign(31);
  legend5->SetBorderSize(2);
  legend5->SetTextSize(0.040);
  legend5->Draw("");


  pad112->cd(9);  
  TGraphErrors * asym7GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym7in.data(),myzero.data(),asym7Erin.data());
  asym7GraphIN->SetName("asym7GraphIN");
  asym7GraphIN->SetMarkerColor(kBlue);
  asym7GraphIN->SetLineColor(kBlue);
  asym7GraphIN->SetMarkerStyle(22);
  asym7GraphIN->SetMarkerSize(markerSize[0]);
  asym7GraphIN->SetLineWidth(1);
  asym7GraphIN->Fit("fitfun7In","E M R F 0 Q");
  fitfun7In->SetLineStyle(1);
  fitfun7In->SetLineWidth(2);
  fitfun7In->SetLineColor(kBlue);
  fitfun7In->SetLineStyle(4);
  fitfun7In->SetLineWidth(2);


  TGraphErrors * asym7GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym7out.data(),myzero.data(),asym7Erout.data());
  asym7GraphOUT->SetName("asym7GraphOUT");
  asym7GraphOUT->SetMarkerColor(kRed);
  asym7GraphOUT->SetLineColor(kRed);
  asym7GraphOUT->SetMarkerStyle(26);
  asym7GraphOUT->SetMarkerSize(markerSize[0]);
  asym7GraphOUT->SetLineWidth(1);
  asym7GraphOUT->Fit("fitfun7Out","E M R F 0 Q");
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
  asym7Graph->GetYaxis()->SetTitle(Form("%s5 Neg. Asym. [ppm]",device.Data()));
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

  Double_t valuesum6,valueerror6,valuediff6,errordiff6;
  valuesum6    = (calc_AsymInp0[6]+calc_AsymOutp0[6])/2.0;
  valueerror6  = (sqrt(pow(calc_eAsymInp0[6],2)+pow(calc_eAsymOutp0[6],2)))/2.0;
  valuediff6   = ((calc_AsymInp0[6]/pow(calc_eAsymInp0[6],2)) - (calc_AsymOutp0[6]/pow(calc_eAsymOutp0[6],2))) /((1/pow(calc_eAsymInp0[6],2)) + (1/pow(calc_eAsymOutp0[6],2)));
  errordiff6   = sqrt(1/((1/(pow(calc_eAsymInp0[6],2)))+(1/pow(calc_eAsymOutp0[6],2))));

  TLegend *legend6 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend6->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum6,valueerror6), "");
  legend6->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff6,errordiff6), "");
  legend6->SetFillColor(0);
  legend6->SetTextAlign(31);
  legend6->SetBorderSize(2);
  legend6->SetTextSize(0.040);
  legend6->Draw("");


  pad112->cd(7);  
  TGraphErrors * asym8GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym8in.data(),myzero.data(),asym8Erin.data());
  asym8GraphIN->SetName("asym8GraphIN");
  asym8GraphIN->SetMarkerColor(kBlue);
  asym8GraphIN->SetLineColor(kBlue);
  asym8GraphIN->SetMarkerStyle(22);
  asym8GraphIN->SetMarkerSize(markerSize[0]);
  asym8GraphIN->SetLineWidth(1);
  asym8GraphIN->Fit("fitfun8In","E M R F 0 Q");
  fitfun8In->SetLineStyle(1);
  fitfun8In->SetLineWidth(2);
  fitfun8In->SetLineColor(kBlue);
  fitfun8In->SetLineStyle(4);
  fitfun8In->SetLineWidth(2);


  TGraphErrors * asym8GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym8out.data(),myzero.data(),asym8Erout.data());
  asym8GraphOUT->SetName("asym8GraphOUT");
  asym8GraphOUT->SetMarkerColor(kRed);
  asym8GraphOUT->SetLineColor(kRed);
  asym8GraphOUT->SetMarkerStyle(26);
  asym8GraphOUT->SetMarkerSize(markerSize[0]);
  asym8GraphOUT->SetLineWidth(1);
  asym8GraphOUT->Fit("fitfun8Out","E M R F 0 Q");
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
  asym8Graph->GetYaxis()->SetTitle(Form("%s7 Neg. Asym. [ppm]",device.Data()));
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

  Double_t valuesum7,valueerror7,valuediff7,errordiff7;
  valuesum7    = (calc_AsymInp0[7]+calc_AsymOutp0[7])/2.0;
  valueerror7  = (sqrt(pow(calc_eAsymInp0[7],2)+pow(calc_eAsymOutp0[7],2)))/2.0;
  valuediff7   = ((calc_AsymInp0[7]/pow(calc_eAsymInp0[7],2)) - (calc_AsymOutp0[7]/pow(calc_eAsymOutp0[7],2))) /((1/pow(calc_eAsymInp0[7],2)) + (1/pow(calc_eAsymOutp0[7],2)));
  errordiff7   = sqrt(1/((1/(pow(calc_eAsymInp0[7],2)))+(1/pow(calc_eAsymOutp0[7],2))));

  TLegend *legend7 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend7->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum7,valueerror7), "");
  legend7->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff7,errordiff7), "");
  legend7->SetFillColor(0);
  legend7->SetTextAlign(31);
  legend7->SetBorderSize(2);
  legend7->SetTextSize(0.040);
  legend7->Draw("");


  pad112->cd(5);  
  TGraphErrors * asym9GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym9in.data(),myzero.data(),asym9Erin.data());
  asym9GraphIN->SetName("asym9GraphIN");
  asym9GraphIN->SetMarkerColor(kBlue);
  asym9GraphIN->SetLineColor(kBlue);
  asym9GraphIN->SetMarkerStyle(21);
  asym9GraphIN->SetMarkerSize(markerSize[0]);
  asym9GraphIN->SetLineWidth(1);
  asym9GraphIN->Fit("fitfun9In","E M R F 0 Q");
  fitfun9In->SetLineStyle(1);
  fitfun9In->SetLineWidth(2);
  fitfun9In->SetLineColor(kBlue);
  fitfun9In->SetLineWidth(2);


  TGraphErrors * asym9GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym9out.data(),myzero.data(),asym9Erout.data());
  asym9GraphOUT->SetName("asym9GraphOUT");
  asym9GraphOUT->SetMarkerColor(kRed);
  asym9GraphOUT->SetLineColor(kRed);
  asym9GraphOUT->SetMarkerStyle(25);
  asym9GraphOUT->SetMarkerSize(markerSize[0]);
  asym9GraphOUT->SetLineWidth(1);
  asym9GraphOUT->Fit("fitfun9Out","E M R F 0 Q");
  fitfun9Out->SetLineStyle(2);
  fitfun9Out->SetLineWidth(2);
  fitfun9Out->SetLineColor(kRed);
  fitfun9Out->SetLineWidth(2);

  TMultiGraph *asym9Graph = new TMultiGraph();
  asym9Graph->Add(asym9GraphIN);
  asym9Graph->Add(asym9GraphOUT);
  asym9Graph->Draw("AP");
  asym9Graph->SetTitle("");
//   asym9Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym9Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym9Graph->GetYaxis()->SetTitle(Form("UsLumi Sum Asym. [ppm]"));
  asym9Graph->GetXaxis()->CenterTitle();
  asym9Graph->GetYaxis()->CenterTitle();
//   asym9Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym9Graph->GetXaxis()->SetLabelColor(0);
  asym9Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun9In->Draw("same");
  fitfun9Out->Draw("same");

  TPaveStats *statsAsym9In =(TPaveStats*)asym9GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym9Out =(TPaveStats*)asym9GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym9In->SetTextColor(kBlue);
  statsAsym9In->SetFillColor(kWhite); 
  statsAsym9Out->SetTextColor(kRed);
  statsAsym9Out->SetFillColor(kWhite);
  statsAsym9In->SetTextSize(0.045);
  statsAsym9Out->SetTextSize(0.045);
  statsAsym9In->SetX1NDC(x_lo_stat_in4);    statsAsym9In->SetX2NDC(x_hi_stat_in4); 
  statsAsym9In->SetY1NDC(y_lo_stat_in4);    statsAsym9In->SetY2NDC(y_hi_stat_in4);
  statsAsym9Out->SetX1NDC(x_lo_stat_out4);  statsAsym9Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym9Out->SetY1NDC(y_lo_stat_out4);  statsAsym9Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymInp0[8]         =   fitfun9In->GetParameter(0);
  calc_eAsymInp0[8]        =   fitfun9In->GetParError(0);
  calc_AsymInChisquare[8]  =   fitfun9In->GetChisquare();
  calc_AsymInNDF[8]        =   fitfun9In->GetNDF();

  calc_AsymOutp0[8]        =   fitfun9Out->GetParameter(0);
  calc_eAsymOutp0[8]       =   fitfun9Out->GetParError(0);
  calc_AsymOutChisquare[8] =   fitfun9Out->GetChisquare();
  calc_AsymOutNDF[8]       =   fitfun9Out->GetNDF();

  tAsymIn->Draw();
  tAsymOut->Draw();

  Double_t valuesum,valueerror,valuediff,errordiff;
  valuesum    = (calc_AsymInp0[8]+calc_AsymOutp0[8])/2.0;
  valueerror  = (sqrt(pow(calc_eAsymInp0[8],2)+pow(calc_eAsymOutp0[8],2)))/2.0;
  valuediff   = ((calc_AsymInp0[8]/pow(calc_eAsymInp0[8],2)) - (calc_AsymOutp0[8]/pow(calc_eAsymOutp0[8],2))) /((1/pow(calc_eAsymInp0[8],2)) + (1/pow(calc_eAsymOutp0[8],2)));
  errordiff   = sqrt(1/((1/(pow(calc_eAsymInp0[8],2)))+(1/pow(calc_eAsymOutp0[8],2))));

  TLegend *legend = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum,valueerror), "");
  legend->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff,errordiff), "");
  legend->SetFillColor(0);
  legend->SetTextAlign(31);
  legend->SetBorderSize(2);
  legend->SetTextSize(0.040);
  legend->Draw("");



  TString saveAllPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_%s_%s_regression_%s_%s_all_plot_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());


//   cout<<"valuesum:"<<valuesum<<"+-"<<valueerror<<endl;

  c11-> Update();
  c11->Print(saveAllPlot+".png");
  if(FIGURE){
    c11->Print(saveAllPlot+".svg");
    c11->Print(saveAllPlot+".C");
  }

 /*********************************************************************/
  }
 /*********************************************************************/

  if(USLUMI_PLOT2){

  TCanvas * c13 = new TCanvas("c13", titleInOut,0,0,1600,1000);
  c13->Draw();
  c13->SetBorderSize(0);
  c13->cd();
  TPad*pad131 = new TPad("pad131","pad131",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad132 = new TPad("pad132","pad132",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad131->SetFillColor(kWhite);
  pad131->Draw();
  pad132->Draw();
  pad131->cd();
  TLatex * t13 = new TLatex(0.00,0.3,Form("%s",titleInOut.Data()));
  t13->SetTextSize(0.45);
  t13->Draw();
  pad132->cd();
  pad132->Divide(3,3);



  pad132->cd(4);
  TGraphErrors * asym31GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym31in.data(),myzero.data(),asym31Erin.data());
  asym31GraphIN->SetName("asym31GraphIN");
  asym31GraphIN->SetMarkerColor(kBlue);
  asym31GraphIN->SetLineColor(kBlue);
  asym31GraphIN->SetMarkerStyle(20);
  asym31GraphIN->SetMarkerSize(markerSize[0]);
  asym31GraphIN->SetLineWidth(1);
  //   asym1GraphIN->Draw("AP");
  asym31GraphIN->Fit("fitfun31In","E M R F 0 Q");
  fitfun31In->SetLineStyle(1);
  fitfun31In->SetLineWidth(2);
  fitfun31In->SetLineColor(kBlue);
  fitfun31In->SetLineStyle(4);
  fitfun31In->SetLineWidth(2);


  //TGraphErrors * asym1GraphOUT = new TGraphErrors((Int_t)run1out.size(),run1out.data(),asym1out.data(),myzero.data(),asym1Erout.data());
  TGraphErrors * asym31GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym31out.data(),myzero.data(),asym31Erout.data());
//   TGraphErrors * asym1GraphOUT = new TGraphErrors(counter11,dummyOUT,asym1OUT11,zero11,asym1Error1OUT11);
  asym31GraphOUT->SetName("asym31GraphOUT");
  asym31GraphOUT->SetMarkerColor(kRed);
  asym31GraphOUT->SetLineColor(kRed);
  asym31GraphOUT->SetMarkerStyle(24);
  asym31GraphOUT->SetMarkerSize(markerSize[0]);
  asym31GraphOUT->SetLineWidth(1);
//   asym1GraphOUT->Draw("AP");
  asym31GraphOUT->Fit("fitfun31Out","E M R F 0 Q");
  fitfun31Out->SetLineStyle(1);
  fitfun31Out->SetLineWidth(2);
  fitfun31Out->SetLineColor(kRed);
  fitfun31Out->SetLineStyle(5);
  fitfun31Out->SetLineWidth(2);

  TMultiGraph *asym31Graph = new TMultiGraph();
  asym31Graph->Add(asym31GraphIN);
  asym31Graph->Add(asym31GraphOUT);
  asym31Graph->Draw("AP");
  asym31Graph->SetTitle("");
//   asym1Graph->GetXaxis()->SetNdivisions(25,0,0);
  asym31Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym31Graph->GetYaxis()->SetTitle(Form("%s1 Sum Asym. [ppm]",device.Data()));
  asym31Graph->GetXaxis()->CenterTitle();
  asym31Graph->GetYaxis()->CenterTitle();
//   asym1Graph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  asym31Graph->GetXaxis()->SetLabelColor(0);
  asym31Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);
//   TAxis *xaxisAsym1Graph= asym1Graph->GetXaxis();
//   xaxisAsym1Graph->SetLimits(16066.00,16066.14);
//   pad112->Range(16064.77,-30.66847,16067.63,47.58539);

  fitfun31In->Draw("same");
  fitfun31Out->Draw("same");


//   TAxis *xaxisAsym1Graph= asym1Graph->GetXaxis();
//   xaxisAsym1Graph->SetLimits(run_number1IN11[0],run_number1IN11[0]+30);
  
  TPaveStats *statsAsym31In =(TPaveStats*)asym31GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym31Out =(TPaveStats*)asym31GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym31In->SetTextColor(kBlue);
  statsAsym31In->SetFillColor(kWhite); 
  statsAsym31Out->SetTextColor(kRed);
  statsAsym31Out->SetFillColor(kWhite);
  statsAsym31In->SetTextSize(0.045);
  statsAsym31Out->SetTextSize(0.045);
  statsAsym31In->SetX1NDC(x_lo_stat_in4);    statsAsym31In->SetX2NDC(x_hi_stat_in4); 
  statsAsym31In->SetY1NDC(y_lo_stat_in4);    statsAsym31In->SetY2NDC(y_hi_stat_in4);
  statsAsym31Out->SetX1NDC(x_lo_stat_out4);  statsAsym31Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym31Out->SetY1NDC(y_lo_stat_out4);  statsAsym31Out->SetY2NDC(y_hi_stat_out4);

  calc3_AsymInp0[0]         =   fitfun31In->GetParameter(0);
  calc3_eAsymInp0[0]        =   fitfun31In->GetParError(0);
  calc3_AsymInChisquare[0]  =   fitfun31In->GetChisquare();
  calc3_AsymInNDF[0]        =   fitfun31In->GetNDF();

  calc3_AsymOutp0[0]        =   fitfun31Out->GetParameter(0);
  calc3_eAsymOutp0[0]       =   fitfun31Out->GetParError(0);
  calc3_AsymOutChisquare[0] =   fitfun31Out->GetChisquare();
  calc3_AsymOutNDF[0]       =   fitfun31Out->GetNDF();


//   TLatex* tAsymIn = new TLatex(5,-40,Form("%5.0f",run1in[0]));
  TLatex* tAsym31In = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tAsym31In->SetTextSize(0.06);
  tAsym31In->SetTextColor(kBlue);
  tAsym31In->Draw();
//   TLatex* tAsymOut = new TLatex(17,-40,Form("%5.0f",run1out[0]));
  TLatex* tAsym31Out = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tAsym31Out->SetTextSize(0.06);
  tAsym31Out->SetTextColor(kRed);
  tAsym31Out->Draw();

  Double_t valuesum31,valueerror31,valuediff31,errordiff31;
  valuesum31    = (calc3_AsymInp0[0]+calc3_AsymOutp0[0])/2.0;
  valueerror31  = (sqrt(pow(calc3_eAsymInp0[0],2)+pow(calc3_eAsymOutp0[0],2)))/2.0;
  valuediff31   = ((calc3_AsymInp0[0]/pow(calc3_eAsymInp0[0],2)) - (calc3_AsymOutp0[0]/pow(calc3_eAsymOutp0[0],2))) /((1/pow(calc3_eAsymInp0[0],2)) + (1/pow(calc3_eAsymOutp0[0],2)));
  errordiff31   = sqrt(1/((1/(pow(calc3_eAsymInp0[0],2)))+(1/pow(calc3_eAsymOutp0[0],2))));

  TLegend *legend31 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend31->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum31,valueerror31), "");
  legend31->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff31,errordiff31), "");
  legend31->SetFillColor(0);
  legend31->SetTextAlign(31);
  legend31->SetBorderSize(2);
  legend31->SetTextSize(0.040);
  legend31->Draw("");

  //  cout<<red<<__LINE__<<normal<<endl;



  pad132->cd(2);
  TGraphErrors * asym33GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym33in.data(),myzero.data(),asym33Erin.data());
  asym33GraphIN->SetName("asym33GraphIN");
  asym33GraphIN->SetMarkerColor(kBlue);
  asym33GraphIN->SetLineColor(kBlue);
  asym33GraphIN->SetMarkerStyle(20);
  asym33GraphIN->SetMarkerSize(markerSize[0]);
  asym33GraphIN->SetLineWidth(1);
  //   asym33GraphIN->Draw("AP");
  asym33GraphIN->Fit("fitfun33In","E M R F 0 Q");
  fitfun33In->SetLineStyle(1);
  fitfun33In->SetLineWidth(2);
  fitfun33In->SetLineColor(kBlue);
  fitfun33In->SetLineStyle(4);
  fitfun33In->SetLineWidth(2);


  TGraphErrors * asym33GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym33out.data(),myzero.data(),asym33Erout.data());
  asym33GraphOUT->SetName("asym33GraphOUT");
  asym33GraphOUT->SetMarkerColor(kRed);
  asym33GraphOUT->SetLineColor(kRed);
  asym33GraphOUT->SetMarkerStyle(24);
  asym33GraphOUT->SetMarkerSize(markerSize[0]);
  asym33GraphOUT->SetLineWidth(1);
//   asym33GraphOUT->Draw("AP");
  asym33GraphOUT->Fit("fitfun33Out","E M R F 0 Q");
  fitfun33Out->SetLineStyle(1);
  fitfun33Out->SetLineWidth(2);
  fitfun33Out->SetLineColor(kRed);
  fitfun33Out->SetLineStyle(5);
  fitfun33Out->SetLineWidth(2);

  TMultiGraph *asym33Graph = new TMultiGraph();
  asym33Graph->Add(asym33GraphIN);
  asym33Graph->Add(asym33GraphOUT);
  asym33Graph->Draw("AP");
  asym33Graph->SetTitle("");
  asym33Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym33Graph->GetYaxis()->SetTitle(Form("%s3 Sum Asym. [ppm]",device.Data()));
  asym33Graph->GetXaxis()->CenterTitle();
  asym33Graph->GetYaxis()->CenterTitle();
  asym33Graph->GetXaxis()->SetLabelColor(0);
  asym33Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun33In->Draw("same");
  fitfun33Out->Draw("same");

  TPaveStats *statsAsym33In =(TPaveStats*)asym33GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym33Out =(TPaveStats*)asym33GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym33In->SetTextColor(kBlue);
  statsAsym33In->SetFillColor(kWhite); 
  statsAsym33Out->SetTextColor(kRed);
  statsAsym33Out->SetFillColor(kWhite);
  statsAsym33In->SetTextSize(0.045);
  statsAsym33Out->SetTextSize(0.045);
  statsAsym33In->SetX1NDC(x_lo_stat_in4);    statsAsym33In->SetX2NDC(x_hi_stat_in4); 
  statsAsym33In->SetY1NDC(y_lo_stat_in4);    statsAsym33In->SetY2NDC(y_hi_stat_in4);
  statsAsym33Out->SetX1NDC(x_lo_stat_out4);  statsAsym33Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym33Out->SetY1NDC(y_lo_stat_out4);  statsAsym33Out->SetY2NDC(y_hi_stat_out4);

  calc33_AsymInp0[0]         =   fitfun33In->GetParameter(0);
  calc33_eAsymInp0[0]        =   fitfun33In->GetParError(0);
  calc33_AsymInChisquare[0]  =   fitfun33In->GetChisquare();
  calc33_AsymInNDF[0]        =   fitfun33In->GetNDF();

  calc33_AsymOutp0[0]        =   fitfun33Out->GetParameter(0);
  calc33_eAsymOutp0[0]       =   fitfun33Out->GetParError(0);
  calc33_AsymOutChisquare[0] =   fitfun33Out->GetChisquare();
  calc33_AsymOutNDF[0]       =   fitfun33Out->GetNDF();


  TLatex* tAsym33In = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tAsym33In->SetTextSize(0.06);
  tAsym33In->SetTextColor(kBlue);
  tAsym33In->Draw();
  TLatex* tAsym33Out = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tAsym33Out->SetTextSize(0.06);
  tAsym33Out->SetTextColor(kRed);
  tAsym33Out->Draw();

  Double_t valuesum33,valueerror33,valuediff33,errordiff33;
  valuesum33    = (calc33_AsymInp0[0]+calc33_AsymOutp0[0])/2.0;
  valueerror33  = (sqrt(pow(calc33_eAsymInp0[0],2)+pow(calc33_eAsymOutp0[0],2)))/2.0;
  valuediff33   = ((calc33_AsymInp0[0]/pow(calc33_eAsymInp0[0],2)) - (calc33_AsymOutp0[0]/pow(calc33_eAsymOutp0[0],2))) /((1/pow(calc33_eAsymInp0[0],2)) + (1/pow(calc33_eAsymOutp0[0],2)));
  errordiff33   = sqrt(1/((1/(pow(calc33_eAsymInp0[0],2)))+(1/pow(calc33_eAsymOutp0[0],2))));

  TLegend *legend33 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend33->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum33,valueerror33), "");
  legend33->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff33,errordiff33), "");
  legend33->SetFillColor(0);
  legend33->SetTextAlign(31);
  legend33->SetBorderSize(2);
  legend33->SetTextSize(0.040);
  legend33->Draw("");


  pad132->cd(6);
  TGraphErrors * asym35GraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,asym35in.data(),myzero.data(),asym35Erin.data());
  asym35GraphIN->SetName("asym35GraphIN");
  asym35GraphIN->SetMarkerColor(kBlue);
  asym35GraphIN->SetLineColor(kBlue);
  asym35GraphIN->SetMarkerStyle(20);
  asym35GraphIN->SetMarkerSize(markerSize[0]);
  asym35GraphIN->SetLineWidth(1);
  asym35GraphIN->Fit("fitfun35In","E M R F 0 Q");
  fitfun35In->SetLineStyle(1);
  fitfun35In->SetLineWidth(2);
  fitfun35In->SetLineColor(kBlue);
  fitfun35In->SetLineStyle(4);
  fitfun35In->SetLineWidth(2);


  TGraphErrors * asym35GraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,asym35out.data(),myzero.data(),asym35Erout.data());
  asym35GraphOUT->SetName("asym35GraphOUT");
  asym35GraphOUT->SetMarkerColor(kRed);
  asym35GraphOUT->SetLineColor(kRed);
  asym35GraphOUT->SetMarkerStyle(24);
  asym35GraphOUT->SetMarkerSize(markerSize[0]);
  asym35GraphOUT->SetLineWidth(1);
  asym35GraphOUT->Fit("fitfun35Out","E M R F 0 Q");
  fitfun35Out->SetLineStyle(1);
  fitfun35Out->SetLineWidth(2);
  fitfun35Out->SetLineColor(kRed);
  fitfun35Out->SetLineStyle(5);
  fitfun35Out->SetLineWidth(2);

  TMultiGraph *asym35Graph = new TMultiGraph();
  asym35Graph->Add(asym35GraphIN);
  asym35Graph->Add(asym35GraphOUT);
  asym35Graph->Draw("AP");
  asym35Graph->SetTitle("");
  asym35Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  asym35Graph->GetYaxis()->SetTitle(Form("%s5 Sum Asym. [ppm]",device.Data()));
  asym35Graph->GetXaxis()->CenterTitle();
  asym35Graph->GetYaxis()->CenterTitle();
  asym35Graph->GetXaxis()->SetLabelColor(0);
  asym35Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfun35In->Draw("same");
  fitfun35Out->Draw("same");

  TPaveStats *statsAsym35In =(TPaveStats*)asym35GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsym35Out =(TPaveStats*)asym35GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsym35In->SetTextColor(kBlue);
  statsAsym35In->SetFillColor(kWhite); 
  statsAsym35Out->SetTextColor(kRed);
  statsAsym35Out->SetFillColor(kWhite);
  statsAsym35In->SetTextSize(0.045);
  statsAsym35Out->SetTextSize(0.045);
  statsAsym35In->SetX1NDC(x_lo_stat_in4);    statsAsym35In->SetX2NDC(x_hi_stat_in4); 
  statsAsym35In->SetY1NDC(y_lo_stat_in4);    statsAsym35In->SetY2NDC(y_hi_stat_in4);
  statsAsym35Out->SetX1NDC(x_lo_stat_out4);  statsAsym35Out->SetX2NDC(x_hi_stat_out4); 
  statsAsym35Out->SetY1NDC(y_lo_stat_out4);  statsAsym35Out->SetY2NDC(y_hi_stat_out4);

  calc35_AsymInp0[0]         =   fitfun35In->GetParameter(0);
  calc35_eAsymInp0[0]        =   fitfun35In->GetParError(0);
  calc35_AsymInChisquare[0]  =   fitfun35In->GetChisquare();
  calc35_AsymInNDF[0]        =   fitfun35In->GetNDF();

  calc35_AsymOutp0[0]        =   fitfun35Out->GetParameter(0);
  calc35_eAsymOutp0[0]       =   fitfun35Out->GetParError(0);
  calc35_AsymOutChisquare[0] =   fitfun35Out->GetChisquare();
  calc35_AsymOutNDF[0]       =   fitfun35Out->GetNDF();


  TLatex* tAsym35In = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tAsym35In->SetTextSize(0.06);
  tAsym35In->SetTextColor(kBlue);
  tAsym35In->Draw();
  TLatex* tAsym35Out = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tAsym35Out->SetTextSize(0.06);
  tAsym35Out->SetTextColor(kRed);
  tAsym35Out->Draw();

  Double_t valuesum35,valueerror35,valuediff35,errordiff35;
  valuesum35    = (calc35_AsymInp0[0]+calc35_AsymOutp0[0])/2.0;
  valueerror35  = (sqrt(pow(calc35_eAsymInp0[0],2)+pow(calc35_eAsymOutp0[0],2)))/2.0;
  valuediff35   = ((calc35_AsymInp0[0]/pow(calc35_eAsymInp0[0],2)) - (calc35_AsymOutp0[0]/pow(calc35_eAsymOutp0[0],2))) /((1/pow(calc35_eAsymInp0[0],2)) + (1/pow(calc35_eAsymOutp0[0],2)));
  errordiff35   = sqrt(1/((1/(pow(calc35_eAsymInp0[0],2)))+(1/pow(calc35_eAsymOutp0[0],2))));

  TLegend *legend35 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legend35->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum35,valueerror35), "");
  legend35->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff35,errordiff35), "");
  legend35->SetFillColor(0);
  legend35->SetTextAlign(31);
  legend35->SetBorderSize(2);
  legend35->SetTextSize(0.040);
  legend35->Draw("");


//   pad132->cd(8);
//   TGraphErrors * asym37GraphIN = new TGraphErrors((Int_t)run31in.size(),dummyINaxis,asym37in.data(),myzero.data(),asym37Erin.data());
//   asym37GraphIN->SetName("asym37GraphIN");
//   asym37GraphIN->SetMarkerColor(kBlue);
//   asym37GraphIN->SetLineColor(kBlue);
//   asym37GraphIN->SetMarkerStyle(20);
//   asym37GraphIN->SetMarkerSize(markerSize[0]);
//   asym37GraphIN->SetLineWidth(1);
//   //   asym37GraphIN->Draw("AP");
//   asym37GraphIN->Fit("fitfun37In","E M R F 0 Q");
//   fitfun37In->SetLineStyle(1);
//   fitfun37In->SetLineWidth(2);
//   fitfun37In->SetLineColor(kBlue);
//   fitfun37In->SetLineStyle(4);
//   fitfun37In->SetLineWidth(2);


//   TGraphErrors * asym37GraphOUT = new TGraphErrors((Int_t)run31out.size(),dummyOUTaxis,asym37out.data(),myzero.data(),asym37Erout.data());
//   asym37GraphOUT->SetName("asym37GraphOUT");
//   asym37GraphOUT->SetMarkerColor(kRed);
//   asym37GraphOUT->SetLineColor(kRed);
//   asym37GraphOUT->SetMarkerStyle(24);
//   asym37GraphOUT->SetMarkerSize(markerSize[0]);
//   asym37GraphOUT->SetLineWidth(1);
// //   asym37GraphOUT->Draw("AP");
//   asym37GraphOUT->Fit("fitfun37Out","E M R F 0 Q");
//   fitfun37Out->SetLineStyle(1);
//   fitfun37Out->SetLineWidth(2);
//   fitfun37Out->SetLineColor(kRed);
//   fitfun37Out->SetLineStyle(5);
//   fitfun37Out->SetLineWidth(2);

//   TMultiGraph *asym37Graph = new TMultiGraph();
//   asym37Graph->Add(asym37GraphIN);
//   asym37Graph->Add(asym37GraphOUT);
//   asym37Graph->Draw("AP");
//   asym37Graph->SetTitle("");
//   asym37Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
//   asym37Graph->GetYaxis()->SetTitle(Form("%s7 Sum Asym. [ppm]",device.Data()));
//   asym37Graph->GetXaxis()->CenterTitle();
//   asym37Graph->GetYaxis()->CenterTitle();
//   asym37Graph->GetXaxis()->SetLabelColor(0);
//   asym37Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

//   fitfun37In->Draw("same");
//   fitfun37Out->Draw("same");

//   TPaveStats *statsAsym37In =(TPaveStats*)asym37GraphIN->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsAsym37Out =(TPaveStats*)asym37GraphOUT->GetListOfFunctions()->FindObject("stats");
//   statsAsym37In->SetTextColor(kBlue);
//   statsAsym37In->SetFillColor(kWhite); 
//   statsAsym37Out->SetTextColor(kRed);
//   statsAsym37Out->SetFillColor(kWhite);
//   statsAsym37In->SetTextSize(0.045);
//   statsAsym37Out->SetTextSize(0.045);
//   statsAsym37In->SetX1NDC(x_lo_stat_in4);    statsAsym37In->SetX2NDC(x_hi_stat_in4); 
//   statsAsym37In->SetY1NDC(y_lo_stat_in4);    statsAsym37In->SetY2NDC(y_hi_stat_in4);
//   statsAsym37Out->SetX1NDC(x_lo_stat_out4);  statsAsym37Out->SetX2NDC(x_hi_stat_out4); 
//   statsAsym37Out->SetY1NDC(y_lo_stat_out4);  statsAsym37Out->SetY2NDC(y_hi_stat_out4);

//   calc37_AsymInp0[0]         =   fitfun37In->GetParameter(0);
//   calc37_eAsymInp0[0]        =   fitfun37In->GetParError(0);
//   calc37_AsymInChisquare[0]  =   fitfun37In->GetChisquare();
//   calc37_AsymInNDF[0]        =   fitfun37In->GetNDF();

//   calc37_AsymOutp0[0]        =   fitfun37Out->GetParameter(0);
//   calc37_eAsymOutp0[0]       =   fitfun37Out->GetParError(0);
//   calc37_AsymOutChisquare[0] =   fitfun37Out->GetChisquare();
//   calc37_AsymOutNDF[0]       =   fitfun37Out->GetNDF();


//   TLatex* tAsym37In = new TLatex(dummyArraySize*0.2,YRange[0]*1.15, "HWP-IN");
//   tAsym37In->SetTextSize(0.06);
//   tAsym37In->SetTextColor(kBlue);
//   tAsym37In->Draw();
//   TLatex* tAsym37Out = new TLatex(dummyArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
//   tAsym37Out->SetTextSize(0.06);
//   tAsym37Out->SetTextColor(kRed);
//   tAsym37Out->Draw();

//   Double_t valuesum37,valueerror37,valuediff37,errordiff37;
//   valuesum37    = (calc37_AsymInp0[0]+calc37_AsymOutp0[0])/2.0;
//   valueerror37  = (sqrt(pow(calc37_eAsymInp0[0],2)+pow(calc37_eAsymOutp0[0],2)))/2.0;
//   valuediff37   = ((calc37_AsymInp0[0]/pow(calc37_eAsymInp0[0],2)) - (calc37_AsymOutp0[0]/pow(calc37_eAsymOutp0[0],2))) /((1/pow(calc37_eAsymInp0[0],2)) + (1/pow(calc37_eAsymOutp0[0],2)));
//   errordiff37   = sqrt(1/((1/(pow(calc37_eAsymInp0[0],2)))+(1/pow(calc37_eAsymOutp0[0],2))));

//   TLegend *legend37 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
//   legend37->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesum37,valueerror37), "");
//   legend37->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediff37,errordiff37), "");
//   legend37->SetFillColor(0);
//   legend37->SetTextAlign(31);
//   legend37->SetBorderSize(2);
//   legend37->SetTextSize(0.040);
//   legend37->Draw("");



  TString saveAllPlot3 = Form("dirPlot/resultPlot/%s_%s_%s_%sSum_%s_%s_regression_%s_%s_all_plot_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),device.Data()
			       ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());

  c13-> Update();
//   c13->Print(saveAllPlot3+".png");



  }



  /*********************************************************************/
  /*********************************************************************/
  /*********************************************************************/


  Int_t counter12=0;
  ifstream runletAsymBkg;
  const Int_t NUM12 = 500;
  Double_t octant12[NUM12],run_number12[NUM12],asym12[NUM12],asymError12[NUM12],asymRMS12[NUM12];
  TString hwp12[NUM12];

  std::vector <Double_t> runBkg1in,runBkg2in,runBkg3in,runBkg4in,runBkg5in,runBkg6in,runBkg7in,runBkg8in,runBkg9in;
  std::vector <Double_t> runBkg1out,runBkg2out,runBkg3out,runBkg4out,runBkg5out,runBkg6out,runBkg7out,runBkg8out,runBkg9out;
  std::vector <Double_t> asymBkg1in,asymBkg2in,asymBkg3in,asymBkg4in,asymBkg5in,asymBkg6in,asymBkg7in,asymBkg8in,asymBkg9in;
  std::vector <Double_t> asymBkg1out,asymBkg2out,asymBkg3out,asymBkg4out,asymBkg5out,asymBkg6out,asymBkg7out,asymBkg8out,asymBkg9out;
  std::vector <Double_t> asymBkg1Erin,asymBkg2Erin,asymBkg3Erin,asymBkg4Erin,asymBkg5Erin,asymBkg6Erin,asymBkg7Erin,asymBkg8Erin,asymBkg9Erin;
  std::vector <Double_t> asymBkg1Erout,asymBkg2Erout,asymBkg3Erout,asymBkg4Erout,asymBkg5Erout,asymBkg6Erout,asymBkg7Erout,asymBkg8Erout,asymBkg9Erout;
  std::vector <Double_t> myBkgZero;

  const Int_t detBkg = 9;
  Double_t calc_AsymBkgInp0[detBkg],calc_eAsymBkgInp0[detBkg],calc_AsymBkgInChisquare[detBkg],calc_AsymBkgInNDF[detBkg];
  Double_t calc_AsymBkgOutp0[detBkg],calc_eAsymBkgOutp0[detBkg],calc_AsymBkgOutChisquare[detBkg],calc_AsymBkgOutNDF[detBkg];


  runletAsymBkg.open(Form("dirPlot/%s_%s_%s_%s_MD_Bkg_regression_%s_%s_%s_asym_%s.txt"
			  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			  ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

   if (runletAsymBkg.is_open()) {
   
    while(1) {
      //       zero12[counter12]=0;
      
      runletAsymBkg >> octant12[counter12] >> run_number12[counter12] >> hwp12[counter12]
		    >> asym12[counter12]   >> asymError12[counter12]  >> asymRMS12[counter12] ;
      
      if (!runletAsymBkg.good()) break;
      
      if(hwp12[counter12]== "in"){
	
	if(octant12[counter12]==1) {
	  runBkg1in.push_back(run_number12[counter12]);
	  asymBkg1in.push_back(asym12[counter12]); 
	  asymBkg1Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==2) {
	  runBkg2in.push_back(run_number12[counter12]);
	  asymBkg2in.push_back(asym12[counter12]); 
	  asymBkg2Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==3) {
	  runBkg3in.push_back(run_number12[counter12]);
	  asymBkg3in.push_back(asym12[counter12]); 
	  asymBkg3Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==4) {
	  runBkg4in.push_back(run_number12[counter12]);
	  asymBkg4in.push_back(asym12[counter12]); 
	  asymBkg4Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==5) {
	  runBkg5in.push_back(run_number12[counter12]);
	  asymBkg5in.push_back(asym12[counter12]); 
	  asymBkg5Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==6) {
	  runBkg6in.push_back(run_number12[counter12]);
	  asymBkg6in.push_back(asym12[counter12]); 
	  asymBkg6Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==7) {
	  runBkg7in.push_back(run_number12[counter12]);
	  asymBkg7in.push_back(asym12[counter12]); 
	  asymBkg7Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==8) {
	  runBkg8in.push_back(run_number12[counter12]);
	  asymBkg8in.push_back(asym12[counter12]); 
	  asymBkg8Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	if(octant12[counter12]==9) {
	  runBkg9in.push_back(run_number12[counter12]);
	  asymBkg9in.push_back(asym12[counter12]); 
	  asymBkg9Erin.push_back(asymError12[counter12]);
	  myBkgZero.push_back(0.0);
	}
	
	
      }



      if(hwp12[counter12]== "out"){
	
	if(octant12[counter12]==1) {
	  runBkg1out.push_back(run_number12[counter12]);
	  asymBkg1out.push_back(asym12[counter12]); 
	  asymBkg1Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==2) {
	  runBkg2out.push_back(run_number12[counter12]);
	  asymBkg2out.push_back(asym12[counter12]); 
	  asymBkg2Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==3) {
	  runBkg3out.push_back(run_number12[counter12]);
	  asymBkg3out.push_back(asym12[counter12]); 
	  asymBkg3Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==4) {
	  runBkg4out.push_back(run_number12[counter12]);
	  asymBkg4out.push_back(asym12[counter12]); 
	  asymBkg4Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==5) {
	  runBkg5out.push_back(run_number12[counter12]);
	  asymBkg5out.push_back(asym12[counter12]); 
	  asymBkg5Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==6) {
	  runBkg6out.push_back(run_number12[counter12]);
	  asymBkg6out.push_back(asym12[counter12]); 
	  asymBkg6Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==7) {
	  runBkg7out.push_back(run_number12[counter12]);
	  asymBkg7out.push_back(asym12[counter12]); 
	  asymBkg7Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==8) {
	  runBkg8out.push_back(run_number12[counter12]);
	  asymBkg8out.push_back(asym12[counter12]); 
	  asymBkg8Erout.push_back(asymError12[counter12]);
	}
	if(octant12[counter12]==9) {
	  runBkg9out.push_back(run_number12[counter12]);
	  asymBkg9out.push_back(asym12[counter12]); 
	  asymBkg9Erout.push_back(asymError12[counter12]);
	}
	
	
      }
      //       else{ cout<<green<<"Check Half Wave Plate settings "<<normal<<endl;}

      
      counter12++;
    }
   }
   else {
     printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
     return -1;
   }
   runletAsymBkg.close();

//   printf("%s Checking %d\n%s",green,__LINE__,normal);


  TString title12;
  TString titleInOutBkg = Form("%s (%s, %s A): Regression-%s MD Background Asymmetries."
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data());

  if(datopt==1) title12 = Form("%s %s",titleInOutBkg.Data(),showFit1.Data());
  else if(datopt==2) title12 = Form("%s %s",titleInOutBkg.Data(),showFit2.Data());
  else title12 = Form("%s %s",titleInOutBkg.Data(),showFit3.Data());

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
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.25);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(0.8);
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

  if(BKG_PLOT){

  TCanvas * c12 = new TCanvas("c12", titleInOutBkg,0,0,1600,1000);
  c12->Draw();
  c12->SetBorderSize(0);
  c12->cd();
  TPad*pad121 = new TPad("pad121","pad121",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad122 = new TPad("pad122","pad122",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad121->SetFillColor(kWhite);
  pad121->Draw();
  pad122->Draw();
  pad121->cd();
  TString text12 = Form("%s",titleInOutBkg.Data());
  TText * t12 = new TText(0.00,0.3,text12);
  t12->SetTextSize(0.45);
  t12->Draw();
  pad122->cd();
  pad122->Divide(2,3);


//   Double_t dummyBkgINaxis[(Int_t)runBkg1in.size()];
//   Double_t dummyBkgOUTaxis[(Int_t)runBkg1out.size()];
//   Int_t dummyBkgArraySize = (Int_t)runBkg1in.size() > (Int_t)runBkg1out.size() ? (Int_t)runBkg1in.size() : (Int_t)runBkg1out.size();
//   TString xAxisTitleBkg = "Runlets";

//   for(Int_t j = 0; j< dummyBkgArraySize; j++) {
//     dummyBkgINaxis[j-1] = j;
//     dummyBkgOUTaxis[j-1] = j + dummyBkgArraySize;
//   }

  Double_t dummyBkgINaxis[(Int_t)runBkg4in.size()];
  Double_t dummyBkgOUTaxis[(Int_t)runBkg4out.size()];
  Int_t dummyBkgArraySize = (Int_t)runBkg4in.size() > (Int_t)runBkg4out.size() ? (Int_t)runBkg4in.size() : (Int_t)runBkg4out.size();
  TString xAxisTitleBkg = "Runlets";

  for(Int_t j = 0; j< dummyBkgArraySize; j++) {
    dummyBkgINaxis[j-1] = j;
    dummyBkgOUTaxis[j-1] = j + dummyBkgArraySize;
  }


  cout<<"runBkg1in: "<<runBkg1in.size()<<endl;
  for ( Int_t i=0; i<runBkg1in.size(); i++) {
    cout<<Form("i:%d\trunBkg1:%5.2f\tasym1IN12:%f",i,runBkg1in[i],asymBkg1in[i])<<endl;
  }
  cout<<"runBkg1out: "<<runBkg1out.size()<<endl;
  for ( Int_t i=0; i<runBkg1out.size(); i++) {
    cout<<Form("i:%d\trunBkg1:%5.2f\tasym1OUT12:%f",i,runBkg1out[i],asymBkg1out[i])<<endl;
  }
  cout<<"dummyBkgArraySize:"<< dummyBkgArraySize<<endl;


  TF1* fitfunBkg1In  = new TF1("fitfunBkg1In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg1Out = new TF1("fitfunBkg1Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg2In  = new TF1("fitfunBkg2In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg2Out = new TF1("fitfunBkg2Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg3In  = new TF1("fitfunBkg3In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg3Out = new TF1("fitfunBkg3Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg4In  = new TF1("fitfunBkg4In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg4Out = new TF1("fitfunBkg4Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg5In  = new TF1("fitfunBkg5In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg5Out = new TF1("fitfunBkg5Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg6In  = new TF1("fitfunBkg6In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg6Out = new TF1("fitfunBkg6Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg7In  = new TF1("fitfunBkg7In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg7Out = new TF1("fitfunBkg7Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg8In  = new TF1("fitfunBkg8In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg8Out = new TF1("fitfunBkg8Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);
  TF1* fitfunBkg9In  = new TF1("fitfunBkg9In","pol0",-0.7,(Int_t)runBkg4in.size());
  TF1* fitfunBkg9Out = new TF1("fitfunBkg9Out","pol0",runBkg4in.size()-0.7,dummyBkgOUTaxis[(Int_t)runBkg4out.size()]);


  fitfunBkg1In->SetParNames("A_{IN}");
  fitfunBkg1Out->SetParNames("A_{OUT}");
  fitfunBkg2In->SetParNames("A_{IN}");
  fitfunBkg2Out->SetParNames("A_{OUT}");
  fitfunBkg3In->SetParNames("A_{IN}");
  fitfunBkg3Out->SetParNames("A_{OUT}");
  fitfunBkg4In->SetParNames("A_{IN}");
  fitfunBkg4Out->SetParNames("A_{OUT}");
  fitfunBkg5In->SetParNames("A_{IN}");
  fitfunBkg5Out->SetParNames("A_{OUT}");
  fitfunBkg6In->SetParNames("A_{IN}");
  fitfunBkg6Out->SetParNames("A_{OUT}");
  fitfunBkg7In->SetParNames("A_{IN}");
  fitfunBkg7Out->SetParNames("A_{OUT}");
  fitfunBkg8In->SetParNames("A_{IN}");
  fitfunBkg8Out->SetParNames("A_{OUT}");
  fitfunBkg9In->SetParNames("A_{IN}");
  fitfunBkg9Out->SetParNames("A_{OUT}");

  if(datopt == 2){

  cout<<red<<__LINE__<<normal<<endl;

  pad122->cd(1);
  TGraphErrors * asymBkg1GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg1in.data(),myBkgZero.data(),asymBkg1Erin.data());
  asymBkg1GraphIN->SetName("asymBkg1GraphIN");
  asymBkg1GraphIN->SetMarkerColor(kBlue);
  asymBkg1GraphIN->SetLineColor(kBlue);
  asymBkg1GraphIN->SetMarkerStyle(20);
  asymBkg1GraphIN->SetMarkerSize(markerSize[0]);
  asymBkg1GraphIN->SetLineWidth(1);
  asymBkg1GraphIN->Fit("fitfunBkg1In","E M R F 0 Q");
  fitfunBkg1In->SetLineStyle(1);
  fitfunBkg1In->SetLineWidth(2);
  fitfunBkg1In->SetLineColor(kBlue);
  fitfunBkg1In->SetLineStyle(4);
  fitfunBkg1In->SetLineWidth(2);


  TGraphErrors * asymBkg1GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg1out.data(),myBkgZero.data(),asymBkg1Erout.data());
  asymBkg1GraphOUT->SetName("asymBkg1GraphOUT");
  asymBkg1GraphOUT->SetMarkerColor(kRed);
  asymBkg1GraphOUT->SetLineColor(kRed);
  asymBkg1GraphOUT->SetMarkerStyle(24);
  asymBkg1GraphOUT->SetMarkerSize(markerSize[0]);
  asymBkg1GraphOUT->SetLineWidth(1);
  asymBkg1GraphOUT->Fit("fitfunBkg1Out","E M R F 0 Q");
  fitfunBkg1Out->SetLineStyle(1);
  fitfunBkg1Out->SetLineWidth(2);
  fitfunBkg1Out->SetLineColor(kRed);
  fitfunBkg1Out->SetLineStyle(5);
  fitfunBkg1Out->SetLineWidth(2);

  TMultiGraph *asymBkg1Graph = new TMultiGraph();
  asymBkg1Graph->Add(asymBkg1GraphIN);
  asymBkg1Graph->Add(asymBkg1GraphOUT);
  asymBkg1Graph->Draw("AP");
  asymBkg1Graph->SetTitle("");
  asymBkg1Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
  asymBkg1Graph->GetYaxis()->SetTitle(Form("MD9 Barsum Asym. [ppm]"));
  asymBkg1Graph->GetXaxis()->CenterTitle();
  asymBkg1Graph->GetYaxis()->CenterTitle();
  asymBkg1Graph->GetXaxis()->SetLabelColor(0);
  asymBkg1Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfunBkg1In->Draw("same");
  fitfunBkg1Out->Draw("same");

  
  TPaveStats *statsAsymBkg1In =(TPaveStats*)asymBkg1GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsymBkg1Out =(TPaveStats*)asymBkg1GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsymBkg1In->SetTextColor(kBlue);
  statsAsymBkg1In->SetFillColor(kWhite); 
  statsAsymBkg1Out->SetTextColor(kRed);
  statsAsymBkg1Out->SetFillColor(kWhite);
  statsAsymBkg1In->SetTextSize(0.045);
  statsAsymBkg1Out->SetTextSize(0.045);
  statsAsymBkg1In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg1In->SetX2NDC(x_hi_stat_in4); 
  statsAsymBkg1In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg1In->SetY2NDC(y_hi_stat_in4);
  statsAsymBkg1Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg1Out->SetX2NDC(x_hi_stat_out4); 
  statsAsymBkg1Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg1Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymBkgInp0[0]         =   fitfunBkg1In->GetParameter(0);
  calc_eAsymBkgInp0[0]        =   fitfunBkg1In->GetParError(0);
  calc_AsymBkgInChisquare[0]  =   fitfunBkg1In->GetChisquare();
  calc_AsymBkgInNDF[0]        =   fitfunBkg1In->GetNDF();

  calc_AsymBkgOutp0[0]        =   fitfunBkg1Out->GetParameter(0);
  calc_eAsymBkgOutp0[0]       =   fitfunBkg1Out->GetParError(0);
  calc_AsymBkgOutChisquare[0] =   fitfunBkg1Out->GetChisquare();
  calc_AsymBkgOutNDF[0]       =   fitfunBkg1Out->GetNDF();

  Double_t valuesumBkg1,valueerrorBkg1,valuediffBkg1,errordiffBkg1;
  valuesumBkg1    = (calc_AsymBkgInp0[0]+calc_AsymBkgOutp0[0])/2.0;
  valueerrorBkg1  = (sqrt(pow(calc_eAsymBkgInp0[0],2)+pow(calc_eAsymBkgOutp0[0],2)))/2.0;
  valuediffBkg1   = ((calc_AsymBkgInp0[0]/pow(calc_eAsymBkgInp0[0],2)) - (calc_AsymBkgOutp0[0]/pow(calc_eAsymBkgOutp0[0],2))) /((1/pow(calc_eAsymBkgInp0[0],2)) + (1/pow(calc_eAsymBkgOutp0[0],2)));
  errordiffBkg1   = sqrt(1/((1/(pow(calc_eAsymBkgInp0[0],2)))+(1/pow(calc_eAsymBkgOutp0[0],2))));

  TLegend *legendBkg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legendBkg1->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesumBkg1,valueerrorBkg1), "");
  legendBkg1->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediffBkg1,errordiffBkg1), "");
  legendBkg1->SetFillColor(0);
  legendBkg1->SetTextAlign(31);
  legendBkg1->SetBorderSize(2);
  legendBkg1->SetTextSize(0.040);
  legendBkg1->Draw("");


  TLatex* tAsymBkg1In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tAsymBkg1In->SetTextSize(0.06);
  tAsymBkg1In->SetTextColor(kBlue);
  tAsymBkg1In->Draw();
  TLatex* tAsymBkg1Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tAsymBkg1Out->SetTextSize(0.06);
  tAsymBkg1Out->SetTextColor(kRed);
  tAsymBkg1Out->Draw();
  }
  else{cout<<red<<"No data for MD9 Barsum"<<normal<<endl;}

  if(datopt == 2){

  pad122->cd(3);
  TGraphErrors * asymBkg2GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg2in.data(),myBkgZero.data(),asymBkg2Erin.data());
  asymBkg2GraphIN->SetName("asymBkg2GraphIN");
  asymBkg2GraphIN->SetMarkerColor(kBlue);
  asymBkg2GraphIN->SetLineColor(kBlue);
  asymBkg2GraphIN->SetMarkerStyle(20);
  asymBkg2GraphIN->SetMarkerSize(markerSize[0]);
  asymBkg2GraphIN->SetLineWidth(1);
  asymBkg2GraphIN->Fit("fitfunBkg2In","E M R F 0 Q");
  fitfunBkg2In->SetLineStyle(1);
  fitfunBkg2In->SetLineWidth(2);
  fitfunBkg2In->SetLineColor(kBlue);
  fitfunBkg2In->SetLineStyle(4);
  fitfunBkg2In->SetLineWidth(2);

  TGraphErrors * asymBkg2GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg2out.data(),myBkgZero.data(),asymBkg2Erout.data());
  asymBkg2GraphOUT->SetName("asymBkg2GraphOUT");
  asymBkg2GraphOUT->SetMarkerColor(kRed);
  asymBkg2GraphOUT->SetLineColor(kRed);
  asymBkg2GraphOUT->SetMarkerStyle(24);
  asymBkg2GraphOUT->SetMarkerSize(markerSize[0]);
  asymBkg2GraphOUT->SetLineWidth(1);
  asymBkg2GraphOUT->Fit("fitfunBkg2Out","E M R F 0 Q");
  fitfunBkg2Out->SetLineStyle(1);
  fitfunBkg2Out->SetLineWidth(2);
  fitfunBkg2Out->SetLineColor(kRed);
  fitfunBkg2Out->SetLineStyle(5);
  fitfunBkg2Out->SetLineWidth(2);

  TMultiGraph *asymBkg2Graph = new TMultiGraph();
  asymBkg2Graph->Add(asymBkg2GraphIN);
  asymBkg2Graph->Add(asymBkg2GraphOUT);
  asymBkg2Graph->Draw("AP");
  asymBkg2Graph->SetTitle("");
  asymBkg2Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
  asymBkg2Graph->GetYaxis()->SetTitle(Form("MD9 Pos. Asym. [ppm]"));
  asymBkg2Graph->GetXaxis()->CenterTitle();
  asymBkg2Graph->GetYaxis()->CenterTitle();
  asymBkg2Graph->GetXaxis()->SetLabelColor(0);
  asymBkg2Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

  fitfunBkg2In->Draw("same");
  fitfunBkg2Out->Draw("same");
  
  TPaveStats *statsAsymBkg2In =(TPaveStats*)asymBkg2GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsymBkg2Out =(TPaveStats*)asymBkg2GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsymBkg2In->SetTextColor(kBlue);
  statsAsymBkg2In->SetFillColor(kWhite); 
  statsAsymBkg2Out->SetTextColor(kRed);
  statsAsymBkg2Out->SetFillColor(kWhite);
  statsAsymBkg2In->SetTextSize(0.045);
  statsAsymBkg2Out->SetTextSize(0.045);
  statsAsymBkg2In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg2In->SetX2NDC(x_hi_stat_in4); 
  statsAsymBkg2In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg2In->SetY2NDC(y_hi_stat_in4);
  statsAsymBkg2Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg2Out->SetX2NDC(x_hi_stat_out4); 
  statsAsymBkg2Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg2Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymBkgInp0[1]         =   fitfunBkg2In->GetParameter(0);
  calc_eAsymBkgInp0[1]        =   fitfunBkg2In->GetParError(0);
  calc_AsymBkgInChisquare[1]  =   fitfunBkg2In->GetChisquare();
  calc_AsymBkgInNDF[1]        =   fitfunBkg2In->GetNDF();

  calc_AsymBkgOutp0[1]        =   fitfunBkg2Out->GetParameter(0);
  calc_eAsymBkgOutp0[1]       =   fitfunBkg2Out->GetParError(0);
  calc_AsymBkgOutChisquare[1] =   fitfunBkg2Out->GetChisquare();
  calc_AsymBkgOutNDF[1]       =   fitfunBkg2Out->GetNDF();

  Double_t valuesumBkg2,valueerrorBkg2,valuediffBkg2,errordiffBkg2;
  valuesumBkg2    = (calc_AsymBkgInp0[1]+calc_AsymBkgOutp0[1])/2.0;
  valueerrorBkg2  = (sqrt(pow(calc_eAsymBkgInp0[1],2)+pow(calc_eAsymBkgOutp0[1],2)))/2.0;
  valuediffBkg2   = ((calc_AsymBkgInp0[1]/pow(calc_eAsymBkgInp0[1],2)) - (calc_AsymBkgOutp0[1]/pow(calc_eAsymBkgOutp0[1],2))) /((1/pow(calc_eAsymBkgInp0[1],2)) + (1/pow(calc_eAsymBkgOutp0[1],2)));
  errordiffBkg2   = sqrt(1/((1/(pow(calc_eAsymBkgInp0[1],2)))+(1/pow(calc_eAsymBkgOutp0[1],2))));

  TLegend *legendBkg2 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legendBkg2->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesumBkg2,valueerrorBkg2), "");
  legendBkg2->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediffBkg2,errordiffBkg2), "");
  legendBkg2->SetFillColor(0);
  legendBkg2->SetTextAlign(31);
  legendBkg2->SetBorderSize(2);
  legendBkg2->SetTextSize(0.040);
  legendBkg2->Draw("");


  TLatex* tAsymBkg2In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*1.15, "HWP-IN");
  tAsymBkg2In->SetTextSize(0.06);
  tAsymBkg2In->SetTextColor(kBlue);
  tAsymBkg2In->Draw();
  TLatex* tAsymBkg2Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
  tAsymBkg2Out->SetTextSize(0.06);
  tAsymBkg2Out->SetTextColor(kRed);
  tAsymBkg2Out->Draw();
  }
  else{cout<<red<<"No data for MD9 Pos"<<normal<<endl;}


  if(datopt == 2){

  pad122->cd(5);
  TGraphErrors * asymBkg3GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg3in.data(),myBkgZero.data(),asymBkg3Erin.data());
  asymBkg3GraphIN->SetName("asymBkg3GraphIN");
  asymBkg3GraphIN->SetMarkerColor(kBlue);
  asymBkg3GraphIN->SetLineColor(kBlue);
  asymBkg3GraphIN->SetMarkerStyle(20);
  asymBkg3GraphIN->SetMarkerSize(markerSize[0]);
  asymBkg3GraphIN->SetLineWidth(1);
  asymBkg3GraphIN->Fit("fitfunBkg3In","E M R F 0 Q");
  fitfunBkg3In->SetLineStyle(1);
  fitfunBkg3In->SetLineWidth(2);
  fitfunBkg3In->SetLineColor(kBlue);
  fitfunBkg3In->SetLineStyle(4);
  fitfunBkg3In->SetLineWidth(2);

  TGraphErrors * asymBkg3GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg3out.data(),myBkgZero.data(),asymBkg3Erout.data());
  asymBkg3GraphOUT->SetName("asymBkg3GraphOUT");
  asymBkg3GraphOUT->SetMarkerColor(kRed);
  asymBkg3GraphOUT->SetLineColor(kRed);
  asymBkg3GraphOUT->SetMarkerStyle(24);
  asymBkg3GraphOUT->SetMarkerSize(markerSize[0]);
  asymBkg3GraphOUT->SetLineWidth(1);
  asymBkg3GraphOUT->Fit("fitfunBkg3Out","E M R F 0 Q");
  fitfunBkg3Out->SetLineStyle(1);
  fitfunBkg3Out->SetLineWidth(2);
  fitfunBkg3Out->SetLineColor(kRed);
  fitfunBkg3Out->SetLineStyle(5);
  fitfunBkg3Out->SetLineWidth(2);

  TMultiGraph *asymBkg3Graph = new TMultiGraph();
  asymBkg3Graph->Add(asymBkg3GraphIN);
  asymBkg3Graph->Add(asymBkg3GraphOUT);
  asymBkg3Graph->Draw("AP");
  asymBkg3Graph->SetTitle("");
  asymBkg3Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
  asymBkg3Graph->GetYaxis()->SetTitle(Form("MD Neg. Asym. [ppm]"));
  asymBkg3Graph->GetXaxis()->CenterTitle();
  asymBkg3Graph->GetYaxis()->CenterTitle();
  asymBkg3Graph->GetXaxis()->SetLabelColor(0);
  asymBkg3Graph->GetYaxis()->SetRangeUser(YRange[0]*3,YRange[1]*3);

  fitfunBkg3In->Draw("same");
  fitfunBkg3Out->Draw("same");
  
  TPaveStats *statsAsymBkg3In =(TPaveStats*)asymBkg3GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsymBkg3Out =(TPaveStats*)asymBkg3GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsymBkg3In->SetTextColor(kBlue);
  statsAsymBkg3In->SetFillColor(kWhite); 
  statsAsymBkg3Out->SetTextColor(kRed);
  statsAsymBkg3Out->SetFillColor(kWhite);
  statsAsymBkg3In->SetTextSize(0.045);
  statsAsymBkg3Out->SetTextSize(0.045);
  statsAsymBkg3In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg3In->SetX2NDC(x_hi_stat_in4); 
  statsAsymBkg3In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg3In->SetY2NDC(y_hi_stat_in4);
  statsAsymBkg3Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg3Out->SetX2NDC(x_hi_stat_out4); 
  statsAsymBkg3Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg3Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymBkgInp0[2]         =   fitfunBkg3In->GetParameter(0);
  calc_eAsymBkgInp0[2]        =   fitfunBkg3In->GetParError(0);
  calc_AsymBkgInChisquare[2]  =   fitfunBkg3In->GetChisquare();
  calc_AsymBkgInNDF[2]        =   fitfunBkg3In->GetNDF();

  calc_AsymBkgOutp0[2]        =   fitfunBkg3Out->GetParameter(0);
  calc_eAsymBkgOutp0[2]       =   fitfunBkg3Out->GetParError(0);
  calc_AsymBkgOutChisquare[2] =   fitfunBkg3Out->GetChisquare();
  calc_AsymBkgOutNDF[2]       =   fitfunBkg3Out->GetNDF();

  Double_t valuesumBkg3,valueerrorBkg3,valuediffBkg3,errordiffBkg3;
  valuesumBkg3    = (calc_AsymBkgInp0[2]+calc_AsymBkgOutp0[2])/2.0;
  valueerrorBkg3  = (sqrt(pow(calc_eAsymBkgInp0[2],2)+pow(calc_eAsymBkgOutp0[2],2)))/2.0;
  valuediffBkg3   = ((calc_AsymBkgInp0[2]/pow(calc_eAsymBkgInp0[2],2)) - (calc_AsymBkgOutp0[2]/pow(calc_eAsymBkgOutp0[2],2))) /((1/pow(calc_eAsymBkgInp0[2],2)) + (1/pow(calc_eAsymBkgOutp0[2],2)));
  errordiffBkg3   = sqrt(1/((1/(pow(calc_eAsymBkgInp0[2],2)))+(1/pow(calc_eAsymBkgOutp0[2],2))));

  TLegend *legendBkg3 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legendBkg3->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesumBkg3,valueerrorBkg3), "");
  legendBkg3->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediffBkg3,errordiffBkg3), "");
  legendBkg3->SetFillColor(0);
  legendBkg3->SetTextAlign(31);
  legendBkg3->SetBorderSize(2);
  legendBkg3->SetTextSize(0.040);
  legendBkg3->Draw("");


  TLatex* tAsymBkg3In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*3*1.15, "HWP-IN");
  tAsymBkg3In->SetTextSize(0.06);
  tAsymBkg3In->SetTextColor(kBlue);
  tAsymBkg3In->Draw();
  TLatex* tAsymBkg3Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*3*1.15, "HWP-OUT");
  tAsymBkg3Out->SetTextSize(0.06);
  tAsymBkg3Out->SetTextColor(kRed);
  tAsymBkg3Out->Draw();

  }
  else{cout<<red<<"No data for MD9 Neg"<<normal<<endl;}

  pad122->cd(2);
  TGraphErrors * asymBkg4GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg4in.data(),myBkgZero.data(),asymBkg4Erin.data());
  asymBkg4GraphIN->SetName("asymBkg4GraphIN");
  asymBkg4GraphIN->SetMarkerColor(kBlue);
  asymBkg4GraphIN->SetLineColor(kBlue);
  asymBkg4GraphIN->SetMarkerStyle(20);
  asymBkg4GraphIN->SetMarkerSize(markerSize[0]);
  asymBkg4GraphIN->SetLineWidth(1);
  asymBkg4GraphIN->Fit("fitfunBkg4In","E M R F 0 Q");
  fitfunBkg4In->SetLineStyle(1);
  fitfunBkg4In->SetLineWidth(2);
  fitfunBkg4In->SetLineColor(kBlue);
  fitfunBkg4In->SetLineStyle(4);
  fitfunBkg4In->SetLineWidth(2);

  TGraphErrors * asymBkg4GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg4out.data(),myBkgZero.data(),asymBkg4Erout.data());
  asymBkg4GraphOUT->SetName("asymBkg4GraphOUT");
  asymBkg4GraphOUT->SetMarkerColor(kRed);
  asymBkg4GraphOUT->SetLineColor(kRed);
  asymBkg4GraphOUT->SetMarkerStyle(24);
  asymBkg4GraphOUT->SetMarkerSize(markerSize[0]);
  asymBkg4GraphOUT->SetLineWidth(1);
  asymBkg4GraphOUT->Fit("fitfunBkg4Out","E M R F 0 Q");
  fitfunBkg4Out->SetLineStyle(1);
  fitfunBkg4Out->SetLineWidth(2);
  fitfunBkg4Out->SetLineColor(kRed);
  fitfunBkg4Out->SetLineStyle(5);
  fitfunBkg4Out->SetLineWidth(2);

  TMultiGraph *asymBkg4Graph = new TMultiGraph();
  asymBkg4Graph->Add(asymBkg4GraphIN);
  asymBkg4Graph->Add(asymBkg4GraphOUT);
  asymBkg4Graph->Draw("AP");
  asymBkg4Graph->SetTitle("");
  asymBkg4Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
  asymBkg4Graph->GetYaxis()->SetTitle(Form("PMTonl Asym. [ppm]"));
  asymBkg4Graph->GetXaxis()->CenterTitle();
  asymBkg4Graph->GetYaxis()->CenterTitle();
  asymBkg4Graph->GetXaxis()->SetLabelColor(0);
  asymBkg4Graph->GetYaxis()->SetRangeUser(YRange[0]*3,YRange[1]*3);

  fitfunBkg4In->Draw("same");
  fitfunBkg4Out->Draw("same");
  
  TPaveStats *statsAsymBkg4In =(TPaveStats*)asymBkg4GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsymBkg4Out =(TPaveStats*)asymBkg4GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsymBkg4In->SetTextColor(kBlue);
  statsAsymBkg4In->SetFillColor(kWhite); 
  statsAsymBkg4Out->SetTextColor(kRed);
  statsAsymBkg4Out->SetFillColor(kWhite);
  statsAsymBkg4In->SetTextSize(0.045);
  statsAsymBkg4Out->SetTextSize(0.045);
  statsAsymBkg4In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg4In->SetX2NDC(x_hi_stat_in4); 
  statsAsymBkg4In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg4In->SetY2NDC(y_hi_stat_in4);
  statsAsymBkg4Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg4Out->SetX2NDC(x_hi_stat_out4); 
  statsAsymBkg4Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg4Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymBkgInp0[3]         =   fitfunBkg4In->GetParameter(0);
  calc_eAsymBkgInp0[3]        =   fitfunBkg4In->GetParError(0);
  calc_AsymBkgInChisquare[3]  =   fitfunBkg4In->GetChisquare();
  calc_AsymBkgInNDF[3]        =   fitfunBkg4In->GetNDF();

  calc_AsymBkgOutp0[3]        =   fitfunBkg4Out->GetParameter(0);
  calc_eAsymBkgOutp0[3]       =   fitfunBkg4Out->GetParError(0);
  calc_AsymBkgOutChisquare[3] =   fitfunBkg4Out->GetChisquare();
  calc_AsymBkgOutNDF[3]       =   fitfunBkg4Out->GetNDF();

  Double_t valuesumBkg4,valueerrorBkg4,valuediffBkg4,errordiffBkg4;
  valuesumBkg4    = (calc_AsymBkgInp0[3]+calc_AsymBkgOutp0[3])/2.0;
  valueerrorBkg4  = (sqrt(pow(calc_eAsymBkgInp0[3],2)+pow(calc_eAsymBkgOutp0[3],2)))/2.0;
  valuediffBkg4   = ((calc_AsymBkgInp0[3]/pow(calc_eAsymBkgInp0[3],2)) - (calc_AsymBkgOutp0[3]/pow(calc_eAsymBkgOutp0[3],2))) /((1/pow(calc_eAsymBkgInp0[3],2)) + (1/pow(calc_eAsymBkgOutp0[3],2)));
  errordiffBkg4   = sqrt(1/((1/(pow(calc_eAsymBkgInp0[3],2)))+(1/pow(calc_eAsymBkgOutp0[3],2))));

  TLegend *legendBkg4 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legendBkg4->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesumBkg4,valueerrorBkg4), "");
  legendBkg4->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediffBkg4,errordiffBkg4), "");
  legendBkg4->SetFillColor(0);
  legendBkg4->SetTextAlign(31);
  legendBkg4->SetBorderSize(2);
  legendBkg4->SetTextSize(0.040);
  legendBkg4->Draw("");


  TLatex* tAsymBkg4In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*3*1.15, "HWP-IN");
  tAsymBkg4In->SetTextSize(0.06);
  tAsymBkg4In->SetTextColor(kBlue);
  tAsymBkg4In->Draw();
  TLatex* tAsymBkg4Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*3*1.15, "HWP-OUT");
  tAsymBkg4Out->SetTextSize(0.06);
  tAsymBkg4Out->SetTextColor(kRed);
  tAsymBkg4Out->Draw();



//   pad122->cd(5);
//   TGraphErrors * asymBkg5GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg5in.data(),myBkgZero.data(),asymBkg5Erin.data());
//   asymBkg5GraphIN->SetName("asymBkg5GraphIN");
//   asymBkg5GraphIN->SetMarkerColor(kBlue);
//   asymBkg5GraphIN->SetLineColor(kBlue);
//   asymBkg5GraphIN->SetMarkerStyle(20);
//   asymBkg5GraphIN->SetMarkerSize(markerSize[0]);
//   asymBkg5GraphIN->SetLineWidth(1);
//   asymBkg5GraphIN->Fit("fitfunBkg5In","E M R F 0 Q");
//   fitfunBkg5In->SetLineStyle(1);
//   fitfunBkg5In->SetLineWidth(2);
//   fitfunBkg5In->SetLineColor(kBlue);
//   fitfunBkg5In->SetLineStyle(4);
//   fitfunBkg5In->SetLineWidth(2);

//   TGraphErrors * asymBkg5GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg5out.data(),myBkgZero.data(),asymBkg5Erout.data());
//   asymBkg5GraphOUT->SetName("asymBkg5GraphOUT");
//   asymBkg5GraphOUT->SetMarkerColor(kRed);
//   asymBkg5GraphOUT->SetLineColor(kRed);
//   asymBkg5GraphOUT->SetMarkerStyle(24);
//   asymBkg5GraphOUT->SetMarkerSize(markerSize[0]);
//   asymBkg5GraphOUT->SetLineWidth(1);
//   asymBkg5GraphOUT->Fit("fitfunBkg5Out","E M R F 0 Q");
//   fitfunBkg5Out->SetLineStyle(1);
//   fitfunBkg5Out->SetLineWidth(2);
//   fitfunBkg5Out->SetLineColor(kRed);
//   fitfunBkg5Out->SetLineStyle(5);
//   fitfunBkg5Out->SetLineWidth(2);

//   TMultiGraph *asymBkg5Graph = new TMultiGraph();
//   asymBkg5Graph->Add(asymBkg5GraphIN);
//   asymBkg5Graph->Add(asymBkg5GraphOUT);
//   asymBkg5Graph->Draw("AP");
//   asymBkg5Graph->SetTitle("");
//   asymBkg5Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
//   asymBkg5Graph->GetYaxis()->SetTitle(Form("PMTled Asym. [ppm]"));
//   asymBkg5Graph->GetXaxis()->CenterTitle();
//   asymBkg5Graph->GetYaxis()->CenterTitle();
//   asymBkg5Graph->GetXaxis()->SetLabelColor(0);
//   asymBkg5Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

//   fitfunBkg5In->Draw("same");
//   fitfunBkg5Out->Draw("same");
  
//   TPaveStats *statsAsymBkg5In =(TPaveStats*)asymBkg5GraphIN->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsAsymBkg5Out =(TPaveStats*)asymBkg5GraphOUT->GetListOfFunctions()->FindObject("stats");
//   statsAsymBkg5In->SetTextColor(kBlue);
//   statsAsymBkg5In->SetFillColor(kWhite); 
//   statsAsymBkg5Out->SetTextColor(kRed);
//   statsAsymBkg5Out->SetFillColor(kWhite);
//   statsAsymBkg5In->SetTextSize(0.045);
//   statsAsymBkg5Out->SetTextSize(0.045);
//   statsAsymBkg5In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg5In->SetX2NDC(x_hi_stat_in4); 
//   statsAsymBkg5In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg5In->SetY2NDC(y_hi_stat_in4);
//   statsAsymBkg5Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg5Out->SetX2NDC(x_hi_stat_out4); 
//   statsAsymBkg5Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg5Out->SetY2NDC(y_hi_stat_out4);

//   calc_AsymBkgInp0[4]         =   fitfunBkg5In->GetParameter(0);
//   calc_eAsymBkgInp0[4]        =   fitfunBkg5In->GetParError(0);
//   calc_AsymBkgInChisquare[4]  =   fitfunBkg5In->GetChisquare();
//   calc_AsymBkgInNDF[4]        =   fitfunBkg5In->GetNDF();

//   calc_AsymBkgOutp0[4]        =   fitfunBkg5Out->GetParameter(0);
//   calc_eAsymBkgOutp0[4]       =   fitfunBkg5Out->GetParError(0);
//   calc_AsymBkgOutChisquare[4] =   fitfunBkg5Out->GetChisquare();
//   calc_AsymBkgOutNDF[4]       =   fitfunBkg5Out->GetNDF();

//   TLatex* tAsymBkg5In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*1.15, "HWP-IN");
//   tAsymBkg5In->SetTextSize(0.06);
//   tAsymBkg5In->SetTextColor(kBlue);
//   tAsymBkg5In->Draw();
//   TLatex* tAsymBkg5Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
//   tAsymBkg5Out->SetTextSize(0.06);
//   tAsymBkg5Out->SetTextColor(kRed);
//   tAsymBkg5Out->Draw();




  pad122->cd(4);
  TGraphErrors * asymBkg6GraphIN = new TGraphErrors((Int_t)runBkg6in.size(),dummyBkgINaxis,asymBkg6in.data(),myBkgZero.data(),asymBkg6Erin.data());
  asymBkg6GraphIN->SetName("asymBkg6GraphIN");
  asymBkg6GraphIN->SetMarkerColor(kBlue);
  asymBkg6GraphIN->SetLineColor(kBlue);
  asymBkg6GraphIN->SetMarkerStyle(20);
  asymBkg6GraphIN->SetMarkerSize(markerSize[0]);
  asymBkg6GraphIN->SetLineWidth(1);
  asymBkg6GraphIN->Fit("fitfunBkg6In","E M R F 0 Q");
  fitfunBkg6In->SetLineStyle(1);
  fitfunBkg6In->SetLineWidth(2);
  fitfunBkg6In->SetLineColor(kBlue);
  fitfunBkg6In->SetLineStyle(4);
  fitfunBkg6In->SetLineWidth(2);

  TGraphErrors * asymBkg6GraphOUT = new TGraphErrors((Int_t)runBkg6out.size(),dummyBkgOUTaxis,asymBkg6out.data(),myBkgZero.data(),asymBkg6Erout.data());
  asymBkg6GraphOUT->SetName("asymBkg6GraphOUT");
  asymBkg6GraphOUT->SetMarkerColor(kRed);
  asymBkg6GraphOUT->SetLineColor(kRed);
  asymBkg6GraphOUT->SetMarkerStyle(24);
  asymBkg6GraphOUT->SetMarkerSize(markerSize[0]);
  asymBkg6GraphOUT->SetLineWidth(1);
  asymBkg6GraphOUT->Fit("fitfunBkg6Out","E M R F 0 Q");
  fitfunBkg6Out->SetLineStyle(1);
  fitfunBkg6Out->SetLineWidth(2);
  fitfunBkg6Out->SetLineColor(kRed);
  fitfunBkg6Out->SetLineStyle(5);
  fitfunBkg6Out->SetLineWidth(2);

  TMultiGraph *asymBkg6Graph = new TMultiGraph();
  asymBkg6Graph->Add(asymBkg6GraphIN);
  asymBkg6Graph->Add(asymBkg6GraphOUT);
  asymBkg6Graph->Draw("AP");
  asymBkg6Graph->SetTitle("");
  asymBkg6Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
  asymBkg6Graph->GetYaxis()->SetTitle(Form("PMTltg Asym. [ppm]"));
  asymBkg6Graph->GetXaxis()->CenterTitle();
  asymBkg6Graph->GetYaxis()->CenterTitle();
  asymBkg6Graph->GetXaxis()->SetLabelColor(0);
  asymBkg6Graph->GetYaxis()->SetRangeUser(YRange[0]*3,YRange[1]*3);

  fitfunBkg6In->Draw("same");
  fitfunBkg6Out->Draw("same");
  
  TPaveStats *statsAsymBkg6In =(TPaveStats*)asymBkg6GraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsAsymBkg6Out =(TPaveStats*)asymBkg6GraphOUT->GetListOfFunctions()->FindObject("stats");
  statsAsymBkg6In->SetTextColor(kBlue);
  statsAsymBkg6In->SetFillColor(kWhite); 
  statsAsymBkg6Out->SetTextColor(kRed);
  statsAsymBkg6Out->SetFillColor(kWhite);
  statsAsymBkg6In->SetTextSize(0.045);
  statsAsymBkg6Out->SetTextSize(0.045);
  statsAsymBkg6In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg6In->SetX2NDC(x_hi_stat_in4); 
  statsAsymBkg6In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg6In->SetY2NDC(y_hi_stat_in4);
  statsAsymBkg6Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg6Out->SetX2NDC(x_hi_stat_out4); 
  statsAsymBkg6Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg6Out->SetY2NDC(y_hi_stat_out4);

  calc_AsymBkgInp0[5]         =   fitfunBkg6In->GetParameter(0);
  calc_eAsymBkgInp0[5]        =   fitfunBkg6In->GetParError(0);
  calc_AsymBkgInChisquare[5]  =   fitfunBkg6In->GetChisquare();
  calc_AsymBkgInNDF[5]        =   fitfunBkg6In->GetNDF();

  calc_AsymBkgOutp0[5]        =   fitfunBkg6Out->GetParameter(0);
  calc_eAsymBkgOutp0[5]       =   fitfunBkg6Out->GetParError(0);
  calc_AsymBkgOutChisquare[5] =   fitfunBkg6Out->GetChisquare();
  calc_AsymBkgOutNDF[5]       =   fitfunBkg6Out->GetNDF();

  Double_t valuesumBkg6,valueerrorBkg6,valuediffBkg6,errordiffBkg6;
  valuesumBkg6    = (calc_AsymBkgInp0[5]+calc_AsymBkgOutp0[5])/2.0;
  valueerrorBkg6  = (sqrt(pow(calc_eAsymBkgInp0[5],2)+pow(calc_eAsymBkgOutp0[5],2)))/2.0;
  valuediffBkg6   = ((calc_AsymBkgInp0[5]/pow(calc_eAsymBkgInp0[5],2)) - (calc_AsymBkgOutp0[5]/pow(calc_eAsymBkgOutp0[5],2))) /((1/pow(calc_eAsymBkgInp0[5],2)) + (1/pow(calc_eAsymBkgOutp0[5],2)));
  errordiffBkg6   = sqrt(1/((1/(pow(calc_eAsymBkgInp0[5],2)))+(1/pow(calc_eAsymBkgOutp0[5],2))));

  TLegend *legendBkg6 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4,"","brNDC");
  legendBkg6->AddEntry((TObject*)0, Form("A_{(IN+OUT)/2} = %4.2f#pm%4.2f",valuesumBkg6,valueerrorBkg6), "");
  legendBkg6->AddEntry((TObject*)0, Form("A_{measured} = %4.2f#pm%4.2f",valuediffBkg6,errordiffBkg6), "");
  legendBkg6->SetFillColor(0);
  legendBkg6->SetTextAlign(31);
  legendBkg6->SetBorderSize(2);
  legendBkg6->SetTextSize(0.040);
  legendBkg6->Draw("");

  TLatex* tAsymBkg6In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*3*1.15, "HWP-IN");
  tAsymBkg6In->SetTextSize(0.06);
  tAsymBkg6In->SetTextColor(kBlue);
  tAsymBkg6In->Draw();
  TLatex* tAsymBkg6Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*3*1.15, "HWP-OUT");
  tAsymBkg6Out->SetTextSize(0.06);
  tAsymBkg6Out->SetTextColor(kRed);
  tAsymBkg6Out->Draw();



//   pad122->cd(7);
//   TGraphErrors * asymBkg7GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg7in.data(),myBkgZero.data(),asymBkg7Erin.data());
//   asymBkg7GraphIN->SetName("asymBkg7GraphIN");
//   asymBkg7GraphIN->SetMarkerColor(kBlue);
//   asymBkg7GraphIN->SetLineColor(kBlue);
//   asymBkg7GraphIN->SetMarkerStyle(20);
//   asymBkg7GraphIN->SetMarkerSize(markerSize[0]);
//   asymBkg7GraphIN->SetLineWidth(1);
//   asymBkg7GraphIN->Fit("fitfunBkg7In","E M R F 0 Q");
//   fitfunBkg7In->SetLineStyle(1);
//   fitfunBkg7In->SetLineWidth(2);
//   fitfunBkg7In->SetLineColor(kBlue);
//   fitfunBkg7In->SetLineStyle(4);
//   fitfunBkg7In->SetLineWidth(2);

//   TGraphErrors * asymBkg7GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg7out.data(),myBkgZero.data(),asymBkg7Erout.data());
//   asymBkg7GraphOUT->SetName("asymBkg7GraphOUT");
//   asymBkg7GraphOUT->SetMarkerColor(kRed);
//   asymBkg7GraphOUT->SetLineColor(kRed);
//   asymBkg7GraphOUT->SetMarkerStyle(24);
//   asymBkg7GraphOUT->SetMarkerSize(markerSize[0]);
//   asymBkg7GraphOUT->SetLineWidth(1);
//   asymBkg7GraphOUT->Fit("fitfunBkg7Out","E M R F 0 Q");
//   fitfunBkg7Out->SetLineStyle(1);
//   fitfunBkg7Out->SetLineWidth(2);
//   fitfunBkg7Out->SetLineColor(kRed);
//   fitfunBkg7Out->SetLineStyle(5);
//   fitfunBkg7Out->SetLineWidth(2);

//   TMultiGraph *asymBkg7Graph = new TMultiGraph();
//   asymBkg7Graph->Add(asymBkg7GraphIN);
//   asymBkg7Graph->Add(asymBkg7GraphOUT);
//   asymBkg7Graph->Draw("AP");
//   asymBkg7Graph->SetTitle("");
//   asymBkg7Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
//   asymBkg7Graph->GetYaxis()->SetTitle(Form("preamp Asym. [ppm]"));
//   asymBkg7Graph->GetXaxis()->CenterTitle();
//   asymBkg7Graph->GetYaxis()->CenterTitle();
//   asymBkg7Graph->GetXaxis()->SetLabelColor(0);
//   asymBkg7Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

//   fitfunBkg7In->Draw("same");
//   fitfunBkg7Out->Draw("same");
  
//   TPaveStats *statsAsymBkg7In =(TPaveStats*)asymBkg7GraphIN->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsAsymBkg7Out =(TPaveStats*)asymBkg7GraphOUT->GetListOfFunctions()->FindObject("stats");
//   statsAsymBkg7In->SetTextColor(kBlue);
//   statsAsymBkg7In->SetFillColor(kWhite); 
//   statsAsymBkg7Out->SetTextColor(kRed);
//   statsAsymBkg7Out->SetFillColor(kWhite);
//   statsAsymBkg7In->SetTextSize(0.045);
//   statsAsymBkg7Out->SetTextSize(0.045);
//   statsAsymBkg7In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg7In->SetX2NDC(x_hi_stat_in4); 
//   statsAsymBkg7In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg7In->SetY2NDC(y_hi_stat_in4);
//   statsAsymBkg7Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg7Out->SetX2NDC(x_hi_stat_out4); 
//   statsAsymBkg7Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg7Out->SetY2NDC(y_hi_stat_out4);

//   calc_AsymBkgInp0[6]         =   fitfunBkg7In->GetParameter(0);
//   calc_eAsymBkgInp0[6]        =   fitfunBkg7In->GetParError(0);
//   calc_AsymBkgInChisquare[6]  =   fitfunBkg7In->GetChisquare();
//   calc_AsymBkgInNDF[6]        =   fitfunBkg7In->GetNDF();

//   calc_AsymBkgOutp0[6]        =   fitfunBkg7Out->GetParameter(0);
//   calc_eAsymBkgOutp0[6]       =   fitfunBkg7Out->GetParError(0);
//   calc_AsymBkgOutChisquare[6] =   fitfunBkg7Out->GetChisquare();
//   calc_AsymBkgOutNDF[6]       =   fitfunBkg7Out->GetNDF();

//   TLatex* tAsymBkg7In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*1.15, "HWP-IN");
//   tAsymBkg7In->SetTextSize(0.06);
//   tAsymBkg7In->SetTextColor(kBlue);
//   tAsymBkg7In->Draw();
//   TLatex* tAsymBkg7Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
//   tAsymBkg7Out->SetTextSize(0.06);
//   tAsymBkg7Out->SetTextColor(kRed);
//   tAsymBkg7Out->Draw();




//   pad122->cd(8);
//   TGraphErrors * asymBkg8GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg8in.data(),myBkgZero.data(),asymBkg8Erin.data());
//   asymBkg8GraphIN->SetName("asymBkg8GraphIN");
//   asymBkg8GraphIN->SetMarkerColor(kBlue);
//   asymBkg8GraphIN->SetLineColor(kBlue);
//   asymBkg8GraphIN->SetMarkerStyle(20);
//   asymBkg8GraphIN->SetMarkerSize(markerSize[0]);
//   asymBkg8GraphIN->SetLineWidth(1);
//   asymBkg8GraphIN->Fit("fitfunBkg8In","E M R F 0 Q");
//   fitfunBkg8In->SetLineStyle(1);
//   fitfunBkg8In->SetLineWidth(2);
//   fitfunBkg8In->SetLineColor(kBlue);
//   fitfunBkg8In->SetLineStyle(4);
//   fitfunBkg8In->SetLineWidth(2);

//   TGraphErrors * asymBkg8GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg8out.data(),myBkgZero.data(),asymBkg8Erout.data());
//   asymBkg8GraphOUT->SetName("asymBkg8GraphOUT");
//   asymBkg8GraphOUT->SetMarkerColor(kRed);
//   asymBkg8GraphOUT->SetLineColor(kRed);
//   asymBkg8GraphOUT->SetMarkerStyle(24);
//   asymBkg8GraphOUT->SetMarkerSize(markerSize[0]);
//   asymBkg8GraphOUT->SetLineWidth(1);
//   asymBkg8GraphOUT->Fit("fitfunBkg8Out","E M R F 0 Q");
//   fitfunBkg8Out->SetLineStyle(1);
//   fitfunBkg8Out->SetLineWidth(2);
//   fitfunBkg8Out->SetLineColor(kRed);
//   fitfunBkg8Out->SetLineStyle(5);
//   fitfunBkg8Out->SetLineWidth(2);

//   TMultiGraph *asymBkg8Graph = new TMultiGraph();
//   asymBkg8Graph->Add(asymBkg8GraphIN);
//   asymBkg8Graph->Add(asymBkg8GraphOUT);
//   asymBkg8Graph->Draw("AP");
//   asymBkg8Graph->SetTitle("");
//   asymBkg8Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
//   asymBkg8Graph->GetYaxis()->SetTitle(Form("cagesr Asym. [ppm]"));
//   asymBkg8Graph->GetXaxis()->CenterTitle();
//   asymBkg8Graph->GetYaxis()->CenterTitle();
//   asymBkg8Graph->GetXaxis()->SetLabelColor(0);
//   asymBkg8Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

//   fitfunBkg8In->Draw("same");
//   fitfunBkg8Out->Draw("same");
  
//   TPaveStats *statsAsymBkg8In =(TPaveStats*)asymBkg8GraphIN->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsAsymBkg8Out =(TPaveStats*)asymBkg8GraphOUT->GetListOfFunctions()->FindObject("stats");
//   statsAsymBkg8In->SetTextColor(kBlue);
//   statsAsymBkg8In->SetFillColor(kWhite); 
//   statsAsymBkg8Out->SetTextColor(kRed);
//   statsAsymBkg8Out->SetFillColor(kWhite);
//   statsAsymBkg8In->SetTextSize(0.045);
//   statsAsymBkg8Out->SetTextSize(0.045);
//   statsAsymBkg8In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg8In->SetX2NDC(x_hi_stat_in4); 
//   statsAsymBkg8In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg8In->SetY2NDC(y_hi_stat_in4);
//   statsAsymBkg8Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg8Out->SetX2NDC(x_hi_stat_out4); 
//   statsAsymBkg8Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg8Out->SetY2NDC(y_hi_stat_out4);

//   calc_AsymBkgInp0[7]         =   fitfunBkg8In->GetParameter(0);
//   calc_eAsymBkgInp0[7]        =   fitfunBkg8In->GetParError(0);
//   calc_AsymBkgInChisquare[7]  =   fitfunBkg8In->GetChisquare();
//   calc_AsymBkgInNDF[7]        =   fitfunBkg8In->GetNDF();

//   calc_AsymBkgOutp0[7]        =   fitfunBkg8Out->GetParameter(0);
//   calc_eAsymBkgOutp0[7]       =   fitfunBkg8Out->GetParError(0);
//   calc_AsymBkgOutChisquare[7] =   fitfunBkg8Out->GetChisquare();
//   calc_AsymBkgOutNDF[7]       =   fitfunBkg8Out->GetNDF();

//   TLatex* tAsymBkg8In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*1.15, "HWP-IN");
//   tAsymBkg8In->SetTextSize(0.06);
//   tAsymBkg8In->SetTextColor(kBlue);
//   tAsymBkg8In->Draw();
//   TLatex* tAsymBkg8Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
//   tAsymBkg8Out->SetTextSize(0.06);
//   tAsymBkg8Out->SetTextColor(kRed);
//   tAsymBkg8Out->Draw();



//   pad122->cd(9);
//   TGraphErrors * asymBkg9GraphIN = new TGraphErrors((Int_t)runBkg4in.size(),dummyBkgINaxis,asymBkg9in.data(),myBkgZero.data(),asymBkg9Erin.data());
//   asymBkg9GraphIN->SetName("asymBkg9GraphIN");
//   asymBkg9GraphIN->SetMarkerColor(kBlue);
//   asymBkg9GraphIN->SetLineColor(kBlue);
//   asymBkg9GraphIN->SetMarkerStyle(20);
//   asymBkg9GraphIN->SetMarkerSize(markerSize[0]);
//   asymBkg9GraphIN->SetLineWidth(1);
//   asymBkg9GraphIN->Fit("fitfunBkg9In","E M R F 0 Q");
//   fitfunBkg9In->SetLineStyle(1);
//   fitfunBkg9In->SetLineWidth(2);
//   fitfunBkg9In->SetLineColor(kBlue);
//   fitfunBkg9In->SetLineStyle(4);
//   fitfunBkg9In->SetLineWidth(2);

//   TGraphErrors * asymBkg9GraphOUT = new TGraphErrors((Int_t)runBkg4out.size(),dummyBkgOUTaxis,asymBkg9out.data(),myBkgZero.data(),asymBkg9Erout.data());
//   asymBkg9GraphOUT->SetName("asymBkg9GraphOUT");
//   asymBkg9GraphOUT->SetMarkerColor(kRed);
//   asymBkg9GraphOUT->SetLineColor(kRed);
//   asymBkg9GraphOUT->SetMarkerStyle(24);
//   asymBkg9GraphOUT->SetMarkerSize(markerSize[0]);
//   asymBkg9GraphOUT->SetLineWidth(1);
//   asymBkg9GraphOUT->Fit("fitfunBkg9Out","E M R F 0 Q");
//   fitfunBkg9Out->SetLineStyle(1);
//   fitfunBkg9Out->SetLineWidth(2);
//   fitfunBkg9Out->SetLineColor(kRed);
//   fitfunBkg9Out->SetLineStyle(5);
//   fitfunBkg9Out->SetLineWidth(2);

//   TMultiGraph *asymBkg9Graph = new TMultiGraph();
//   asymBkg9Graph->Add(asymBkg9GraphIN);
//   asymBkg9Graph->Add(asymBkg9GraphOUT);
//   asymBkg9Graph->Draw("AP");
//   asymBkg9Graph->SetTitle("");
//   asymBkg9Graph->GetXaxis()->SetTitle(Form("%s",xAxisTitleBkg.Data()));
//   asymBkg9Graph->GetYaxis()->SetTitle(Form("isourc Asym. [ppm]"));
//   asymBkg9Graph->GetXaxis()->CenterTitle();
//   asymBkg9Graph->GetYaxis()->CenterTitle();
//   asymBkg9Graph->GetXaxis()->SetLabelColor(0);
//   asymBkg9Graph->GetYaxis()->SetRangeUser(YRange[0],YRange[1]);

//   fitfunBkg9In->Draw("same");
//   fitfunBkg9Out->Draw("same");
  
//   TPaveStats *statsAsymBkg9In =(TPaveStats*)asymBkg9GraphIN->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsAsymBkg9Out =(TPaveStats*)asymBkg9GraphOUT->GetListOfFunctions()->FindObject("stats");
//   statsAsymBkg9In->SetTextColor(kBlue);
//   statsAsymBkg9In->SetFillColor(kWhite); 
//   statsAsymBkg9Out->SetTextColor(kRed);
//   statsAsymBkg9Out->SetFillColor(kWhite);
//   statsAsymBkg9In->SetTextSize(0.045);
//   statsAsymBkg9Out->SetTextSize(0.045);
//   statsAsymBkg9In->SetX1NDC(x_lo_stat_in4);    statsAsymBkg9In->SetX2NDC(x_hi_stat_in4); 
//   statsAsymBkg9In->SetY1NDC(y_lo_stat_in4);    statsAsymBkg9In->SetY2NDC(y_hi_stat_in4);
//   statsAsymBkg9Out->SetX1NDC(x_lo_stat_out4);  statsAsymBkg9Out->SetX2NDC(x_hi_stat_out4); 
//   statsAsymBkg9Out->SetY1NDC(y_lo_stat_out4);  statsAsymBkg9Out->SetY2NDC(y_hi_stat_out4);

//   calc_AsymBkgInp0[8]         =   fitfunBkg9In->GetParameter(0);
//   calc_eAsymBkgInp0[8]        =   fitfunBkg9In->GetParError(0);
//   calc_AsymBkgInChisquare[8]  =   fitfunBkg9In->GetChisquare();
//   calc_AsymBkgInNDF[8]        =   fitfunBkg9In->GetNDF();

//   calc_AsymBkgOutp0[8]        =   fitfunBkg9Out->GetParameter(0);
//   calc_eAsymBkgOutp0[8]       =   fitfunBkg9Out->GetParError(0);
//   calc_AsymBkgOutChisquare[8] =   fitfunBkg9Out->GetChisquare();
//   calc_AsymBkgOutNDF[8]       =   fitfunBkg9Out->GetNDF();

//   TLatex* tAsymBkg9In = new TLatex(dummyBkgArraySize*0.2,YRange[0]*1.15, "HWP-IN");
//   tAsymBkg9In->SetTextSize(0.06);
//   tAsymBkg9In->SetTextColor(kBlue);
//   tAsymBkg9In->Draw();
//   TLatex* tAsymBkg9Out = new TLatex(dummyBkgArraySize*1.15,YRange[0]*1.15, "HWP-OUT");
//   tAsymBkg9Out->SetTextSize(0.06);
//   tAsymBkg9Out->SetTextColor(kRed);
//   tAsymBkg9Out->Draw();



  TString saveAllPlot2 = Form("dirPlot/resultPlot/%s_%s_%s_%s_MDBkg_regression_%s_%s_all_plot_%s"
			      ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			      ,reg_calc.Data(),reg_set.Data(),database_stem.Data());

  c12-> Update();
  c12->Print(saveAllPlot2+".png");
  if(FIGURE){
    c12->Print(saveAllPlot2+".svg");
    c12->Print(saveAllPlot2+".C");
  }


  }




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


