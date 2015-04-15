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

  Double_t figSize = 1.0;


  Bool_t DIFF_PLOT = 1;
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
  Double_t textSize[4] = {0.060*figSize,0.035*figSize,0.060*figSize,0.030*figSize};
  Double_t xScalePlot = 1.0;

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
    xScalePlot = 1.0;
  }
  else if(datopt == 3){
    polar = "h_transverse";
    good = "9";
    xScalePlot = 1.06;
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
  ifstream runletDiff;
  const Int_t NUM11 = 500;
  Double_t octant11[NUM11],run_number11[NUM11],diff11[NUM11],diffError11[NUM11],diffRMS11[NUM11],ent11[NUM11];
  TString hwp11[NUM11],var11[NUM11];

  std::vector <Double_t> run1in,run2in,run3in,run4in,run5in,run6in;
  std::vector <Double_t> run1out,run2out,run3out,run4out,run5out,run6out;
  std::vector <Double_t> ent1in,ent2in,ent3in,ent4in,ent5in,ent6in;
  std::vector <Double_t> ent1out,ent2out,ent3out,ent4out,ent5out,ent6out;
  std::vector <Double_t> varXin,varYin,varXSlopein,varYSlopein,varEin,varChargein;
  std::vector <Double_t> varXout,varYout,varXSlopeout,varYSlopeout,varEout,varChargeout;
  std::vector <Double_t> diffXin,diffYin,diffXSlopein,diffYSlopein,diffEin,diffChargein;
  std::vector <Double_t> diffXout,diffYout,diffXSlopeout,diffYSlopeout,diffEout,diffChargeout;
  std::vector <Double_t> diffXErin,diffYErin,diffXSlopeErin,diffYSlopeErin,diffEErin,diffChargeErin;
  std::vector <Double_t> diffXErout,diffYErout,diffXSlopeErout,diffYSlopeErout,diffEErout,diffChargeErout;
  std::vector <Double_t> diffXEr2in,diffYEr2in,diffXSlopeEr2in,diffYSlopeEr2in,diffEEr2in,diffChargeEr2in;
  std::vector <Double_t> diffXEr2out,diffYEr2out,diffXSlopeEr2out,diffYSlopeEr2out,diffEEr2out,diffChargeEr2out;
  std::vector <Double_t> myzero;

//   Double_t diffXSigin[NUM11],diffYSigin[NUM11],diffXSlopeSigin[NUM11],diffYSlopeSigin[NUM11],diffESigin[NUM11],diffChargeSigin[NUM11];
//   Double_t diffXSigout[NUM11],diffYSigout[NUM11],diffXSlopeSigout[NUM11],diffYSlopeSigout[NUM11],diffESigout[NUM11],diffChargeSigout[NUM11];

  std::vector <Double_t> diffXSigin,diffYSigin,diffXSlopeSigin,diffYSlopeSigin,diffESigin,diffChargeSigin;
  std::vector <Double_t> diffXSigout,diffYSigout,diffXSlopeSigout,diffYSlopeSigout,diffESigout,diffChargeSigout;

  Double_t bpmResolution = 2570; 
  Double_t angleResolution = 150; 
  Double_t chargeResolution = 62000; 
  Double_t bpmNonlinearity = 0.01; // This factor multiplies to abs(diffX) to consider non-linearity in BPM. Its same for X and Y.

  const Int_t det = 6;
  Double_t calc_DiffInp0[det],calc_eDiffInp0[det],calc_DiffInChisquare[det],calc_DiffInNDF[det];
  Double_t calc_DiffOutp0[det],calc_eDiffOutp0[det],calc_DiffOutChisquare[det],calc_DiffOutNDF[det];
  Double_t calc_DiffInPOutp0[det],calc_eDiffInPOutp0[det],calc_DiffInPOutChisquare[det],calc_DiffInPOutNDF[det];
  Double_t calc_DiffInMOutp0[det],calc_eDiffInMOutp0[det],calc_DiffInMOutChisquare[det],calc_DiffInMOutNDF[det];


  runletDiff.open(Form("dirPlot/%s_%s_%s_%s_regression_%s_%s_%s_diff_%s.txt"
			     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			     ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

   if (runletDiff.is_open()) {
   
    while(1) {
      //       zero11[counter11]=0;
      
      runletDiff >> octant11[counter11] >> var11[counter11] >> run_number11[counter11] >> hwp11[counter11]
		 >> diff11[counter11] >> diffError11[counter11] >> diffRMS11[counter11] >> ent11[counter11] ;
      
      if (!runletDiff.good()) break;
      
      if(hwp11[counter11]== "in"){
	
	if(octant11[counter11]==1) {
	  run1in.push_back(run_number11[counter11]);
	  diffXin.push_back(diff11[counter11]*1e6);           //converted to nm from mm
	  diffXErin.push_back(diffError11[counter11]*1e6);
	  ent1in.push_back(ent11[counter11]);
	  //diffXEr2in.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  diffXEr2in.push_back( TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e6),2) ) );
	  //diffXSigin.push_back( (diff11[counter11]*1e6)/(TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e6),2) )));
	  diffXSigin.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	  myzero.push_back(0.0);
	}
	if(octant11[counter11]==2) {
	  run2in.push_back(run_number11[counter11]);
	  diffYin.push_back(diff11[counter11]*1e6);           //converted to nm from mm
 	  diffYErin.push_back(diffError11[counter11]*1e6);
	  diffYEr2in.push_back( TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e6),2) ) );
	  //diffYEr2in.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent2in.push_back(ent11[counter11]);
	  diffYSigin.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==3) {
	  run3in.push_back(run_number11[counter11]);
	  diffXSlopein.push_back(diff11[counter11]*1e9);       //converted to nrad
	  diffXSlopeErin.push_back(diffError11[counter11]*1e9);
	  diffXSlopeEr2in.push_back( TMath::Sqrt( pow((angleResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffXSlopeEr2in.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent3in.push_back(ent11[counter11]);
	  diffXSlopeSigin.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==4) {
	  run4in.push_back(run_number11[counter11]);
	  diffYSlopein.push_back(diff11[counter11]*1e9);       //converted to nrad
	  diffYSlopeErin.push_back(diffError11[counter11]*1e9);
	  diffYSlopeEr2in.push_back( TMath::Sqrt( pow((angleResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffYSlopeEr2in.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent4in.push_back(ent11[counter11]);
	  diffYSlopeSigin.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==5) {
	  run5in.push_back(run_number11[counter11]);
	  diffEin.push_back(diff11[counter11]*1e9);       //converted to ppb
	  diffEErin.push_back(diffError11[counter11]*1e9);
	  diffEEr2in.push_back( TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffEEr2in.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent5in.push_back(ent11[counter11]);
	  diffESigin.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==6) {
	  run6in.push_back(run_number11[counter11]);
	  diffChargein.push_back(diff11[counter11]*1e9);        //converted to ppb
	  diffChargeErin.push_back(diffError11[counter11]*1e9);
	  diffChargeEr2in.push_back( TMath::Sqrt( pow((angleResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffChargeEr2in.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent6in.push_back(ent11[counter11]);
	  diffChargeSigin.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	
	
      }



      if(hwp11[counter11]== "out"){
	
	if(octant11[counter11]==1) {
	  run1out.push_back(run_number11[counter11]);
	  diffXout.push_back(diff11[counter11]*1e6); 
	  diffXErout.push_back(diffError11[counter11]*1e6);
	  diffXEr2out.push_back( TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e6),2) ) );
	  //diffXEr2out.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent1out.push_back(ent11[counter11]);
	  diffXSigout.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==2) {
	  run2out.push_back(run_number11[counter11]);
	  diffYout.push_back(diff11[counter11]*1e6); 
	  diffYErout.push_back(diffError11[counter11]*1e6);
	  diffYEr2out.push_back( TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e6),2) ) );
	  //diffYEr2out.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent2out.push_back(ent11[counter11]);
	  diffYSigout.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==3) {
	  run3out.push_back(run_number11[counter11]);
	  diffXSlopeout.push_back(diff11[counter11]*1e9); 
	  diffXSlopeErout.push_back(diffError11[counter11]*1e9);
	  diffXSlopeEr2out.push_back( TMath::Sqrt( pow((angleResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffXSlopeEr2out.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent3out.push_back(ent11[counter11]);
	  diffXSlopeSigout.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==4) {
	  run4out.push_back(run_number11[counter11]);
	  diffYSlopeout.push_back(diff11[counter11]*1e9); 
	  diffYSlopeErout.push_back(diffError11[counter11]*1e9);
	  diffYSlopeEr2out.push_back( TMath::Sqrt( pow((angleResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffYSlopeEr2out.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent4out.push_back(ent11[counter11]);
	  diffYSlopeSigout.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==5) {
	  run5out.push_back(run_number11[counter11]);
	  diffEout.push_back(diff11[counter11]*1e9); 
	  diffEErout.push_back(diffError11[counter11]*1e9);
	  diffEEr2out.push_back( TMath::Sqrt( pow((bpmResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffEEr2out.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent5out.push_back(ent11[counter11]);
	  diffESigout.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
	}
	if(octant11[counter11]==6) {
	  run6out.push_back(run_number11[counter11]);
	  diffChargeout.push_back(diff11[counter11]*1e9); 
	  diffChargeErout.push_back(diffError11[counter11]*1e9);
	  diffChargeEr2out.push_back( TMath::Sqrt( pow((chargeResolution/TMath::Sqrt(ent11[counter11])),2) + pow((bpmNonlinearity*diff11[counter11]*1e9),2) ) );
	  //diffChargeEr2out.push_back(bpmResolution/TMath::Sqrt(ent11[counter11]));
	  ent6out.push_back(ent11[counter11]);
	  diffChargeSigout.push_back( (diff11[counter11]*1e6)/(diffError11[counter11]*1e6) );
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
   runletDiff.close();


  TString title11;
  TString titleInOut = Form("%s (%s, %s A): Regression-%s %s %s."
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

  if(DIFF_PLOT){

  TCanvas * c11 = new TCanvas("c11", titleInOut,0,0,1600*figSize,1000*figSize);
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
    cout<<Form("i:%d\trun1:%5.2f\tdiffXIN11:%g",i,run1in[i],diffXin[i])<<endl;
  }
  cout<<"run1out: "<<run1out.size()<<endl;
  for ( Int_t i=0; i<run1out.size(); i++) {
    cout<<Form("i:%d\trun1:%5.2f\tdiffXOUT11:%g",i,run1out[i],diffXout[i])<<endl;
  }
  cout<<"dummyArraySize:"<< dummyArraySize<<endl;


  for ( Int_t i=0; i<run1in.size(); i++) {
    cout<<blue<<Form("Sigma: IN Runlet = %5.2f : X = %5.2f, Y = %5.2f, X' = %5.2f, Y' = %5.2f, E = %5.2f, Q = %5.2f"
		     ,run1in[i],diffXSigin[i],diffYSigin[i],diffXSlopeSigin[i],diffYSlopeSigin[i],diffESigin[i],diffChargeSigin[i])<<normal<<endl;
  }

  for ( Int_t i=0; i<run1out.size(); i++) {
    cout<<red<<Form("Sigma: OUT Runlet = %5.2f : X = %5.2f, Y = %5.2f, X' = %5.2f, Y' = %5.2f, E = %5.2f, Q = %5.2f"
		     ,run1out[i],diffXSigout[i],diffYSigout[i],diffXSlopeSigout[i],diffYSlopeSigout[i],diffESigout[i],diffChargeSigout[i])<<normal<<endl;
  }

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


  fitfun1In->SetParNames("#DeltaX_{IN}");
  fitfun1Out->SetParNames("#DeltaX_{OUT}");
  fitfun2In->SetParNames("#DeltaY_{IN}");
  fitfun2Out->SetParNames("#DeltaY_{OUT}");
  fitfun3In->SetParNames("#DeltaX^{'}_{IN}");
  fitfun3Out->SetParNames("#DeltaX^{'}_{OUT}");
  fitfun4In->SetParNames("#DeltaY^{'}_{IN}");
  fitfun4Out->SetParNames("#DeltaY^{'}_{OUT}");
  fitfun5In->SetParNames("#DeltaE_{IN}");
  fitfun5Out->SetParNames("#DeltaE_{OUT}");
  fitfun6In->SetParNames("#DeltaA_{Q IN}");
  fitfun6Out->SetParNames("#DeltaA_{Q OUT}");

  Double_t YRangePos[2],YRangeAng[2],YRangeE[2],YRangeQ[2];
  YRangePos[0] = -3e2;   YRangePos[1] = 3e2;
  YRangeAng[0] = -0.01e3;  YRangeAng[1] = 0.01e3;
  YRangeE[0]   = -0.1e3;   YRangeE[1]   = 0.1e3;
  YRangeQ[0]   = -10e3;    YRangeQ[1]   = 10e3;


  pad112->cd(1);
  //TGraphErrors * diffXGraphIN = new TGraphErrors(counter11,run_number1IN11,diffXIN11,zero11,diffError1IN11);
  //TGraphErrors * diffXGraphIN = new TGraphErrors((Int_t)run1in.size(),run1in.data(),diffXin.data(),myzero.data(),diffXErin.data());
  TGraphErrors * diffXGraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,diffXin.data(),myzero.data(),diffXEr2in.data());
  diffXGraphIN->SetName("diffXGraphIN");
  diffXGraphIN->SetMarkerColor(kBlue);
  diffXGraphIN->SetLineColor(kBlue);
  diffXGraphIN->SetMarkerStyle(20);
  diffXGraphIN->SetMarkerSize(markerSize[0]);
  diffXGraphIN->SetLineWidth(1);
  //   diffXGraphIN->Draw("AP");
  diffXGraphIN->Fit("fitfun1In","E M R F 0 Q");
  fitfun1In->SetLineStyle(1);
  fitfun1In->SetLineWidth(2);
  fitfun1In->SetLineColor(kBlue);
  fitfun1In->SetLineStyle(4);
  fitfun1In->SetLineWidth(2);


  //TGraphErrors * diffXGraphOUT = new TGraphErrors((Int_t)run1out.size(),run1out.data(),diffXout.data(),myzero.data(),diffXErout.data());
  TGraphErrors * diffXGraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,diffXout.data(),myzero.data(),diffXEr2out.data());
//   TGraphErrors * diffXGraphOUT = new TGraphErrors(counter11,dummyOUT,diffXOUT11,zero11,diffXError1OUT11);
  diffXGraphOUT->SetName("diffXGraphOUT");
  diffXGraphOUT->SetMarkerColor(kRed);
  diffXGraphOUT->SetLineColor(kRed);
  diffXGraphOUT->SetMarkerStyle(24);
  diffXGraphOUT->SetMarkerSize(markerSize[0]);
  diffXGraphOUT->SetLineWidth(1);
//   diffXGraphOUT->Draw("AP");
  diffXGraphOUT->Fit("fitfun1Out","E M R F 0 Q");
  fitfun1Out->SetLineStyle(1);
  fitfun1Out->SetLineWidth(2);
  fitfun1Out->SetLineColor(kRed);
  fitfun1Out->SetLineStyle(5);
  fitfun1Out->SetLineWidth(2);

  TMultiGraph *diffXGraph = new TMultiGraph();
  diffXGraph->Add(diffXGraphIN);
  diffXGraph->Add(diffXGraphOUT);
  diffXGraph->Draw("AP");
  diffXGraph->SetTitle("");
//   diffXGraph->GetXaxis()->SetNdivisions(25,0,0);
  diffXGraph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  diffXGraph->GetYaxis()->SetTitle(Form("Diff. Target-X [nm]"));
  diffXGraph->GetXaxis()->CenterTitle();
  diffXGraph->GetYaxis()->CenterTitle();
//   diffXGraph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  diffXGraph->GetXaxis()->SetLabelColor(0);
  diffXGraph->GetYaxis()->SetRangeUser(YRangePos[0],YRangePos[1]);

  fitfun1In->Draw("same");
  fitfun1Out->Draw("same");


//   TAxis *xaxisDiffXGraph= diffXGraph->GetXaxis();
//   xaxisDiffXGraph->SetLimits(run_number1IN11[0],run_number1IN11[0]+30);
  
  TPaveStats *statsDiffXIn =(TPaveStats*)diffXGraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDiffXOut =(TPaveStats*)diffXGraphOUT->GetListOfFunctions()->FindObject("stats");
  statsDiffXIn->SetTextColor(kBlue);
  statsDiffXIn->SetFillColor(kWhite); 
  statsDiffXOut->SetTextColor(kRed);
  statsDiffXOut->SetFillColor(kWhite);
  statsDiffXIn->SetTextSize(textSize[0]);
  statsDiffXOut->SetTextSize(textSize[0]);
  statsDiffXIn->SetX1NDC(x_lo_stat_in4);    statsDiffXIn->SetX2NDC(x_hi_stat_in4); 
  statsDiffXIn->SetY1NDC(y_lo_stat_in4);    statsDiffXIn->SetY2NDC(y_hi_stat_in4);
  statsDiffXOut->SetX1NDC(x_lo_stat_out4);  statsDiffXOut->SetX2NDC(x_hi_stat_out4); 
  statsDiffXOut->SetY1NDC(y_lo_stat_out4);  statsDiffXOut->SetY2NDC(y_hi_stat_out4);

  calc_DiffInp0[0]         =   fitfun1In->GetParameter(0);
  calc_eDiffInp0[0]        =   fitfun1In->GetParError(0);
  calc_DiffInChisquare[0]  =   fitfun1In->GetChisquare();
  calc_DiffInNDF[0]        =   fitfun1In->GetNDF();

  calc_DiffOutp0[0]        =   fitfun1Out->GetParameter(0);
  calc_eDiffOutp0[0]       =   fitfun1Out->GetParError(0);
  calc_DiffOutChisquare[0] =   fitfun1Out->GetChisquare();
  calc_DiffOutNDF[0]       =   fitfun1Out->GetNDF();

  calc_DiffInPOutp0[0]     =   (calc_DiffInp0[0]+calc_DiffOutp0[0])/2.0;
  calc_eDiffInPOutp0[0]    =   (sqrt(pow(calc_eDiffInp0[0],2)+pow(calc_eDiffOutp0[0],2)))/2.0;
  calc_DiffInMOutp0[0]     =   ((calc_DiffInp0[0]/pow(calc_eDiffInp0[0],2)) - (calc_DiffOutp0[0]/pow(calc_eDiffOutp0[0],2))) /((1/pow(calc_eDiffInp0[0],2)) + (1/pow(calc_eDiffOutp0[0],2)));
  calc_eDiffInMOutp0[0]    =   sqrt(1/((1/pow(calc_eDiffInp0[0],2)) + (1/pow(calc_eDiffOutp0[0],2))));

//   TLegend *legRunDiffX = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
//   legRunDiffX->AddEntry("diffXGraphIN",  Form("(<IN>+<OUT>)/2 = %4.2f#pm%4.2f",calc_DiffInPOutp0[0],calc_eDiffInPOutp0[0]),"");
//   legRunDiffX->AddEntry("diffXGraphOUT",  Form("<IN,-OUT> = %4.2f#pm%4.2f",calc_DiffInMOutp0[0],calc_eDiffInMOutp0[0]),"");
//   legRunDiffX->SetTextSize(textSize[0]);
//   legRunDiffX->SetFillColor(0);
//   legRunDiffX->SetBorderSize(2);
//   legRunDiffX->Draw();
  TLatex* tDiffXPosInPOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangePos[0]*0.85, Form("IN+OUT: %2.2f #pm %2.2f",calc_DiffInPOutp0[0],calc_eDiffInPOutp0[0]));
  tDiffXPosInPOut->SetTextSize(textSize[0]);
  tDiffXPosInPOut->SetTextColor(kBlack);
  TLatex* tDiffXPosInMOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangePos[0]*1.10, Form("IN-OUT: %2.2f #pm %2.2f",calc_DiffInMOutp0[0],calc_eDiffInMOutp0[0]));
  tDiffXPosInMOut->SetTextSize(textSize[0]);
  tDiffXPosInMOut->SetTextColor(kBlack);

  tDiffXPosInPOut->Draw();
  tDiffXPosInMOut->Draw();


  TLatex* tDiffPosIn = new TLatex(dummyArraySize*0.2,YRangePos[0]*1.15, "HWP-IN");
  tDiffPosIn->SetTextSize(0.06);
  tDiffPosIn->SetTextColor(kBlue);
  tDiffPosIn->Draw();
  TLatex* tDiffPosOut = new TLatex(dummyArraySize*1.15,YRangePos[0]*1.15, "HWP-OUT");
  tDiffPosOut->SetTextSize(0.06);
  tDiffPosOut->SetTextColor(kRed);
  tDiffPosOut->Draw();



  pad112->cd(2);
  TGraphErrors * diffYGraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,diffYin.data(),myzero.data(),diffYEr2in.data());
  diffYGraphIN->SetName("diffYGraphIN");
  diffYGraphIN->SetMarkerColor(kBlue);
  diffYGraphIN->SetLineColor(kBlue);
  diffYGraphIN->SetMarkerStyle(20);
  diffYGraphIN->SetMarkerSize(markerSize[0]);
  diffYGraphIN->SetLineWidth(1);
  diffYGraphIN->Fit("fitfun2In","E M R F 0 Q");
  fitfun2In->SetLineStyle(1);
  fitfun2In->SetLineWidth(2);
  fitfun2In->SetLineColor(kBlue);
  fitfun2In->SetLineStyle(4);
  fitfun2In->SetLineWidth(2);

  TGraphErrors * diffYGraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,diffYout.data(),myzero.data(),diffYEr2out.data());
  diffYGraphOUT->SetName("diffYGraphOUT");
  diffYGraphOUT->SetMarkerColor(kRed);
  diffYGraphOUT->SetLineColor(kRed);
  diffYGraphOUT->SetMarkerStyle(24);
  diffYGraphOUT->SetMarkerSize(markerSize[0]);
  diffYGraphOUT->SetLineWidth(1);
  diffYGraphOUT->Fit("fitfun2Out","E M R F 0 Q");
  fitfun2Out->SetLineStyle(1);
  fitfun2Out->SetLineWidth(2);
  fitfun2Out->SetLineColor(kRed);
  fitfun2Out->SetLineStyle(5);
  fitfun2Out->SetLineWidth(2);

  TMultiGraph *diffYGraph = new TMultiGraph();
  diffYGraph->Add(diffYGraphIN);
  diffYGraph->Add(diffYGraphOUT);
  diffYGraph->Draw("AP");
  diffYGraph->SetTitle("");
//   diffYGraph->GetXaxis()->SetNdivisions(25,0,0);
  diffYGraph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  diffYGraph->GetYaxis()->SetTitle(Form("Diff. Target-Y [nm]"));
  diffYGraph->GetXaxis()->CenterTitle();
  diffYGraph->GetYaxis()->CenterTitle();
//   diffYGraph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  diffYGraph->GetXaxis()->SetLabelColor(0);
  diffYGraph->GetYaxis()->SetRangeUser(YRangePos[0],YRangePos[1]);

  fitfun2In->Draw("same");
  fitfun2Out->Draw("same");

  TPaveStats *statsDiffYIn =(TPaveStats*)diffYGraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDiffYOut =(TPaveStats*)diffYGraphOUT->GetListOfFunctions()->FindObject("stats");
  statsDiffYIn->SetTextColor(kBlue);
  statsDiffYIn->SetFillColor(kWhite); 
  statsDiffYOut->SetTextColor(kRed);
  statsDiffYOut->SetFillColor(kWhite);
  statsDiffYIn->SetTextSize(textSize[0]);
  statsDiffYOut->SetTextSize(textSize[0]);
  statsDiffYIn->SetX1NDC(x_lo_stat_in4);    statsDiffYIn->SetX2NDC(x_hi_stat_in4); 
  statsDiffYIn->SetY1NDC(y_lo_stat_in4);    statsDiffYIn->SetY2NDC(y_hi_stat_in4);
  statsDiffYOut->SetX1NDC(x_lo_stat_out4);  statsDiffYOut->SetX2NDC(x_hi_stat_out4); 
  statsDiffYOut->SetY1NDC(y_lo_stat_out4);  statsDiffYOut->SetY2NDC(y_hi_stat_out4);

  calc_DiffInp0[1]         =   fitfun2In->GetParameter(0);
  calc_eDiffInp0[1]        =   fitfun2In->GetParError(0);
  calc_DiffInChisquare[1]  =   fitfun2In->GetChisquare();
  calc_DiffInNDF[1]        =   fitfun2In->GetNDF();

  calc_DiffOutp0[1]        =   fitfun2Out->GetParameter(0);
  calc_eDiffOutp0[1]       =   fitfun2Out->GetParError(0);
  calc_DiffOutChisquare[1] =   fitfun2Out->GetChisquare();
  calc_DiffOutNDF[1]       =   fitfun2Out->GetNDF();

  tDiffPosIn->Draw();
  tDiffPosOut->Draw();

  calc_DiffInPOutp0[1]     =   (calc_DiffInp0[1]+calc_DiffOutp0[1])/2.0;
  calc_eDiffInPOutp0[1]    =   (sqrt(pow(calc_eDiffInp0[1],2)+pow(calc_eDiffOutp0[1],2)))/2.0;
  calc_DiffInMOutp0[1]     =   ((calc_DiffInp0[1]/pow(calc_eDiffInp0[1],2)) - (calc_DiffOutp0[1]/pow(calc_eDiffOutp0[1],2))) /((1/pow(calc_eDiffInp0[1],2)) + (1/pow(calc_eDiffOutp0[1],2)));
  calc_eDiffInMOutp0[1]    =   sqrt(1/((1/pow(calc_eDiffInp0[1],2)) + (1/pow(calc_eDiffOutp0[1],2))));

  TLatex* tDiffYPosInPOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangePos[0]*0.85, Form("IN+OUT: %2.2f #pm %2.2f",calc_DiffInPOutp0[1],calc_eDiffInPOutp0[1]));
  tDiffYPosInPOut->SetTextSize(textSize[0]);
  tDiffYPosInPOut->SetTextColor(kBlack);
  TLatex* tDiffYPosInMOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangePos[0]*1.10, Form("IN-OUT: %2.2f #pm %2.2f",calc_DiffInMOutp0[1],calc_eDiffInMOutp0[1]));
  tDiffYPosInMOut->SetTextSize(textSize[0]);
  tDiffYPosInMOut->SetTextColor(kBlack);

  tDiffYPosInPOut->Draw();
  tDiffYPosInMOut->Draw();


  pad112->cd(3);  
  TGraphErrors * diffXSlopeGraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,diffXSlopein.data(),myzero.data(),diffXSlopeEr2in.data());
  diffXSlopeGraphIN->SetName("diffXSlopeGraphIN");
  diffXSlopeGraphIN->SetMarkerColor(kBlue);
  diffXSlopeGraphIN->SetLineColor(kBlue);
  diffXSlopeGraphIN->SetMarkerStyle(22);
  diffXSlopeGraphIN->SetMarkerSize(markerSize[0]);
  diffXSlopeGraphIN->SetLineWidth(1);
  diffXSlopeGraphIN->Fit("fitfun3In","E M R F 0 Q");
  fitfun3In->SetLineStyle(1);
  fitfun3In->SetLineWidth(2);
  fitfun3In->SetLineColor(kBlue);
  fitfun3In->SetLineStyle(4);
  fitfun3In->SetLineWidth(2);

  TGraphErrors * diffXSlopeGraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,diffXSlopeout.data(),myzero.data(),diffXSlopeEr2out.data());
  diffXSlopeGraphOUT->SetName("diffXSlopeGraphOUT");
  diffXSlopeGraphOUT->SetMarkerColor(kRed);
  diffXSlopeGraphOUT->SetLineColor(kRed);
  diffXSlopeGraphOUT->SetMarkerStyle(26);
  diffXSlopeGraphOUT->SetMarkerSize(markerSize[0]);
  diffXSlopeGraphOUT->SetLineWidth(1);
  diffXSlopeGraphOUT->Fit("fitfun3Out","E M R F 0 Q");
  fitfun3Out->SetLineStyle(1);
  fitfun3Out->SetLineWidth(2);
  fitfun3Out->SetLineColor(kRed);
  fitfun3Out->SetLineStyle(5);
  fitfun3Out->SetLineWidth(2);


  TMultiGraph *diffXSlopeGraph = new TMultiGraph();
  diffXSlopeGraph->Add(diffXSlopeGraphIN);
  diffXSlopeGraph->Add(diffXSlopeGraphOUT);
  diffXSlopeGraph->Draw("AP");
  diffXSlopeGraph->SetTitle("");
//   diffXSlopeGraph->GetXaxis()->SetNdivisions(25,0,0);
  diffXSlopeGraph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  diffXSlopeGraph->GetYaxis()->SetTitle(Form("Diff. Target-XSlope [nrad]"));
  diffXSlopeGraph->GetXaxis()->CenterTitle();
  diffXSlopeGraph->GetYaxis()->CenterTitle();
//   diffXSlopeGraph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  diffXSlopeGraph->GetXaxis()->SetLabelColor(0);
  diffXSlopeGraph->GetYaxis()->SetRangeUser(YRangeAng[0],YRangeAng[1]);

  fitfun3In->Draw("same");
  fitfun3Out->Draw("same");

  TPaveStats *statsDiffXSlopeIn =(TPaveStats*)diffXSlopeGraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDiffXSlopeOut =(TPaveStats*)diffXSlopeGraphOUT->GetListOfFunctions()->FindObject("stats");
  statsDiffXSlopeIn->SetTextColor(kBlue);
  statsDiffXSlopeIn->SetFillColor(kWhite); 
  statsDiffXSlopeOut->SetTextColor(kRed);
  statsDiffXSlopeOut->SetFillColor(kWhite);
  statsDiffXSlopeIn->SetTextSize(textSize[0]);
  statsDiffXSlopeOut->SetTextSize(textSize[0]);
  statsDiffXSlopeIn->SetX1NDC(x_lo_stat_in4);    statsDiffXSlopeIn->SetX2NDC(x_hi_stat_in4); 
  statsDiffXSlopeIn->SetY1NDC(y_lo_stat_in4);    statsDiffXSlopeIn->SetY2NDC(y_hi_stat_in4);
  statsDiffXSlopeOut->SetX1NDC(x_lo_stat_out4);  statsDiffXSlopeOut->SetX2NDC(x_hi_stat_out4); 
  statsDiffXSlopeOut->SetY1NDC(y_lo_stat_out4);  statsDiffXSlopeOut->SetY2NDC(y_hi_stat_out4);

  calc_DiffInp0[2]         =   fitfun3In->GetParameter(0);
  calc_eDiffInp0[2]        =   fitfun3In->GetParError(0);
  calc_DiffInChisquare[2]  =   fitfun3In->GetChisquare();
  calc_DiffInNDF[2]        =   fitfun3In->GetNDF();

  calc_DiffOutp0[2]        =   fitfun3Out->GetParameter(0);
  calc_eDiffOutp0[2]       =   fitfun3Out->GetParError(0);
  calc_DiffOutChisquare[2] =   fitfun3Out->GetChisquare();
  calc_DiffOutNDF[2]       =   fitfun3Out->GetNDF();

  calc_DiffInPOutp0[2]     =   (calc_DiffInp0[2]+calc_DiffOutp0[2])/2.0;
  calc_eDiffInPOutp0[2]    =   (sqrt(pow(calc_eDiffInp0[2],2)+pow(calc_eDiffOutp0[2],2)))/2.0;
  calc_DiffInMOutp0[2]     =   ((calc_DiffInp0[2]/pow(calc_eDiffInp0[2],2)) - (calc_DiffOutp0[2]/pow(calc_eDiffOutp0[2],2))) /((1/pow(calc_eDiffInp0[2],2)) + (1/pow(calc_eDiffOutp0[2],2)));
  calc_eDiffInMOutp0[2]    =   sqrt(1/((1/pow(calc_eDiffInp0[2],2)) + (1/pow(calc_eDiffOutp0[2],2))));

  TLatex* tDiffXSlopePosInPOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeAng[0]*0.85, Form("IN+OUT: %2.2f #pm %2.2f",calc_DiffInPOutp0[2],calc_eDiffInPOutp0[2]));
  tDiffXSlopePosInPOut->SetTextSize(textSize[0]);
  tDiffXSlopePosInPOut->SetTextColor(kBlack);
  TLatex* tDiffXSlopePosInMOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeAng[0]*1.10, Form("IN-OUT: %2.2f #pm %2.2f",calc_DiffInMOutp0[2],calc_eDiffInMOutp0[2]));
  tDiffXSlopePosInMOut->SetTextSize(textSize[0]);
  tDiffXSlopePosInMOut->SetTextColor(kBlack);

  tDiffXSlopePosInPOut->Draw();
  tDiffXSlopePosInMOut->Draw();


  TLatex* tDiffAngIn = new TLatex(dummyArraySize*0.2,YRangeAng[0]*1.15, "HWP-IN");
  tDiffAngIn->SetTextSize(0.06);
  tDiffAngIn->SetTextColor(kBlue);
  tDiffAngIn->Draw();
  TLatex* tDiffAngOut = new TLatex(dummyArraySize*1.15,YRangeAng[0]*1.15, "HWP-OUT");
  tDiffAngOut->SetTextSize(0.06);
  tDiffAngOut->SetTextColor(kRed);
  tDiffAngOut->Draw();



  pad112->cd(4);  
  TGraphErrors * diffYSlopeGraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,diffYSlopein.data(),myzero.data(),diffYSlopeEr2in.data());
  diffYSlopeGraphIN->SetName("diffYSlopeGraphIN");
  diffYSlopeGraphIN->SetMarkerColor(kBlue);
  diffYSlopeGraphIN->SetLineColor(kBlue);
  diffYSlopeGraphIN->SetMarkerStyle(22);
  diffYSlopeGraphIN->SetMarkerSize(markerSize[0]);
  diffYSlopeGraphIN->SetLineWidth(1);
  diffYSlopeGraphIN->Fit("fitfun4In","E M R F 0 Q");
  fitfun4In->SetLineStyle(1);
  fitfun4In->SetLineWidth(2);
  fitfun4In->SetLineColor(kBlue);
  fitfun4In->SetLineStyle(4);
  fitfun4In->SetLineWidth(2);


  TGraphErrors * diffYSlopeGraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,diffYSlopeout.data(),myzero.data(),diffYSlopeEr2out.data());
  diffYSlopeGraphOUT->SetName("diffYSlopeGraphOUT");
  diffYSlopeGraphOUT->SetMarkerColor(kRed);
  diffYSlopeGraphOUT->SetLineColor(kRed);
  diffYSlopeGraphOUT->SetMarkerStyle(26);
  diffYSlopeGraphOUT->SetMarkerSize(markerSize[0]);
  diffYSlopeGraphOUT->SetLineWidth(1);
  diffYSlopeGraphOUT->Fit("fitfun4Out","E M R F 0 Q");
  fitfun4Out->SetLineStyle(1);
  fitfun4Out->SetLineWidth(2);
  fitfun4Out->SetLineColor(kRed);
  fitfun4Out->SetLineStyle(5);
  fitfun4Out->SetLineWidth(2);


  TMultiGraph *diffYSlopeGraph = new TMultiGraph();
  diffYSlopeGraph->Add(diffYSlopeGraphIN);
  diffYSlopeGraph->Add(diffYSlopeGraphOUT);
  diffYSlopeGraph->Draw("AP");
  diffYSlopeGraph->SetTitle("");
//   diffYSlopeGraph->GetXaxis()->SetNdivisions(25,0,0);
  diffYSlopeGraph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  diffYSlopeGraph->GetYaxis()->SetTitle(Form("Diff. Target-YSlope [nrad]"));
  diffYSlopeGraph->GetXaxis()->CenterTitle();
  diffYSlopeGraph->GetYaxis()->CenterTitle();
//   diffYSlopeGraph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  diffYSlopeGraph->GetXaxis()->SetLabelColor(0);
  diffYSlopeGraph->GetYaxis()->SetRangeUser(YRangeAng[0],YRangeAng[1]);

  fitfun4In->Draw("same");
  fitfun4Out->Draw("same");

  TPaveStats *statsDiffYSlopeIn =(TPaveStats*)diffYSlopeGraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDiffYSlopeOut =(TPaveStats*)diffYSlopeGraphOUT->GetListOfFunctions()->FindObject("stats");
  statsDiffYSlopeIn->SetTextColor(kBlue);
  statsDiffYSlopeIn->SetFillColor(kWhite); 
  statsDiffYSlopeOut->SetTextColor(kRed);
  statsDiffYSlopeOut->SetFillColor(kWhite);
  statsDiffYSlopeIn->SetTextSize(textSize[0]);
  statsDiffYSlopeOut->SetTextSize(textSize[0]);
  statsDiffYSlopeIn->SetX1NDC(x_lo_stat_in4);    statsDiffYSlopeIn->SetX2NDC(x_hi_stat_in4); 
  statsDiffYSlopeIn->SetY1NDC(y_lo_stat_in4);    statsDiffYSlopeIn->SetY2NDC(y_hi_stat_in4);
  statsDiffYSlopeOut->SetX1NDC(x_lo_stat_out4);  statsDiffYSlopeOut->SetX2NDC(x_hi_stat_out4); 
  statsDiffYSlopeOut->SetY1NDC(y_lo_stat_out4);  statsDiffYSlopeOut->SetY2NDC(y_hi_stat_out4);

  calc_DiffInp0[3]         =   fitfun4In->GetParameter(0);
  calc_eDiffInp0[3]        =   fitfun4In->GetParError(0);
  calc_DiffInChisquare[3]  =   fitfun4In->GetChisquare();
  calc_DiffInNDF[3]        =   fitfun4In->GetNDF();

  calc_DiffOutp0[3]        =   fitfun4Out->GetParameter(0);
  calc_eDiffOutp0[3]       =   fitfun4Out->GetParError(0);
  calc_DiffOutChisquare[3] =   fitfun4Out->GetChisquare();
  calc_DiffOutNDF[3]       =   fitfun4Out->GetNDF();

  tDiffAngIn->Draw();
  tDiffAngOut->Draw();

  calc_DiffInPOutp0[3]     =   (calc_DiffInp0[3]+calc_DiffOutp0[3])/2.0;
  calc_eDiffInPOutp0[3]    =   (sqrt(pow(calc_eDiffInp0[3],2)+pow(calc_eDiffOutp0[3],2)))/2.0;
  calc_DiffInMOutp0[3]     =   ((calc_DiffInp0[3]/pow(calc_eDiffInp0[3],2)) - (calc_DiffOutp0[3]/pow(calc_eDiffOutp0[3],2))) /((1/pow(calc_eDiffInp0[3],2)) + (1/pow(calc_eDiffOutp0[3],2)));
  calc_eDiffInMOutp0[3]    =   sqrt(1/((1/pow(calc_eDiffInp0[3],2)) + (1/pow(calc_eDiffOutp0[3],2))));

  TLatex* tDiffYSlopePosInPOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeAng[0]*0.85, Form("IN+OUT: %2.2f #pm %2.2f",calc_DiffInPOutp0[3],calc_eDiffInPOutp0[3]));
  tDiffYSlopePosInPOut->SetTextSize(textSize[0]);
  tDiffYSlopePosInPOut->SetTextColor(kBlack);
  TLatex* tDiffYSlopePosInMOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeAng[0]*1.10, Form("IN-OUT: %2.2f #pm %2.2f",calc_DiffInMOutp0[3],calc_eDiffInMOutp0[3]));
  tDiffYSlopePosInMOut->SetTextSize(textSize[0]);
  tDiffYSlopePosInMOut->SetTextColor(kBlack);

  tDiffYSlopePosInPOut->Draw();
  tDiffYSlopePosInMOut->Draw();

  pad112->cd(5);  
  TGraphErrors * diffEGraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,diffEin.data(),myzero.data(),diffEEr2in.data());
  diffEGraphIN->SetName("diffEGraphIN");
  diffEGraphIN->SetMarkerColor(kBlue);
  diffEGraphIN->SetLineColor(kBlue);
  diffEGraphIN->SetMarkerStyle(21);
  diffEGraphIN->SetMarkerSize(markerSize[0]);
  diffEGraphIN->SetLineWidth(1);
  diffEGraphIN->Fit("fitfun5In","E M R F 0 Q");
  fitfun5In->SetLineStyle(1);
  fitfun5In->SetLineWidth(2);
  fitfun5In->SetLineColor(kBlue);
  fitfun5In->SetLineStyle(4);
  fitfun5In->SetLineWidth(2);


  TGraphErrors * diffEGraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,diffEout.data(),myzero.data(),diffEEr2out.data());
  diffEGraphOUT->SetName("diffEGraphOUT");
  diffEGraphOUT->SetMarkerColor(kRed);
  diffEGraphOUT->SetLineColor(kRed);
  diffEGraphOUT->SetMarkerStyle(25);
  diffEGraphOUT->SetMarkerSize(markerSize[0]);
  diffEGraphOUT->SetLineWidth(1);
  diffEGraphOUT->Fit("fitfun5Out","E M R F 0 Q");
  fitfun5Out->SetLineStyle(1);
  fitfun5Out->SetLineWidth(2);
  fitfun5Out->SetLineColor(kRed);
  fitfun5Out->SetLineStyle(5);
  fitfun5Out->SetLineWidth(2);


  TMultiGraph *diffEGraph = new TMultiGraph();
  diffEGraph->Add(diffEGraphIN);
  diffEGraph->Add(diffEGraphOUT);
  diffEGraph->Draw("AP");
  diffEGraph->SetTitle("");
//   diffEGraph->GetXaxis()->SetNdivisions(25,0,0);
  diffEGraph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  diffEGraph->GetYaxis()->SetTitle(Form("Diff. Energy [ppb]",device.Data()));
  diffEGraph->GetXaxis()->CenterTitle();
  diffEGraph->GetYaxis()->CenterTitle();
//   diffEGraph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  diffEGraph->GetXaxis()->SetLabelColor(0);
  diffEGraph->GetYaxis()->SetRangeUser(YRangeE[0],YRangeE[1]);

  fitfun5In->Draw("same");
  fitfun5Out->Draw("same");

  TPaveStats *statsDiffEIn =(TPaveStats*)diffEGraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDiffEOut =(TPaveStats*)diffEGraphOUT->GetListOfFunctions()->FindObject("stats");
  statsDiffEIn->SetTextColor(kBlue);
  statsDiffEIn->SetFillColor(kWhite); 
  statsDiffEOut->SetTextColor(kRed);
  statsDiffEOut->SetFillColor(kWhite);
  statsDiffEIn->SetTextSize(textSize[0]);
  statsDiffEOut->SetTextSize(textSize[0]);
  statsDiffEIn->SetX1NDC(x_lo_stat_in4);    statsDiffEIn->SetX2NDC(x_hi_stat_in4); 
  statsDiffEIn->SetY1NDC(y_lo_stat_in4);    statsDiffEIn->SetY2NDC(y_hi_stat_in4);
  statsDiffEOut->SetX1NDC(x_lo_stat_out4);  statsDiffEOut->SetX2NDC(x_hi_stat_out4); 
  statsDiffEOut->SetY1NDC(y_lo_stat_out4);  statsDiffEOut->SetY2NDC(y_hi_stat_out4);

  calc_DiffInp0[4]         =   fitfun5In->GetParameter(0);
  calc_eDiffInp0[4]        =   fitfun5In->GetParError(0);
  calc_DiffInChisquare[4]  =   fitfun5In->GetChisquare();
  calc_DiffInNDF[4]        =   fitfun5In->GetNDF();

  calc_DiffOutp0[4]        =   fitfun5Out->GetParameter(0);
  calc_eDiffOutp0[4]       =   fitfun5Out->GetParError(0);
  calc_DiffOutChisquare[4] =   fitfun5Out->GetChisquare();
  calc_DiffOutNDF[4]       =   fitfun5Out->GetNDF();

  calc_DiffInPOutp0[4]     =   (calc_DiffInp0[4]+calc_DiffOutp0[4])/2.0;
  calc_eDiffInPOutp0[4]    =   (sqrt(pow(calc_eDiffInp0[4],2)+pow(calc_eDiffOutp0[4],2)))/2.0;
  calc_DiffInMOutp0[4]     =   ((calc_DiffInp0[4]/pow(calc_eDiffInp0[4],2)) - (calc_DiffOutp0[4]/pow(calc_eDiffOutp0[4],2))) /((1/pow(calc_eDiffInp0[4],2)) + (1/pow(calc_eDiffOutp0[4],2)));
  calc_eDiffInMOutp0[4]    =   sqrt(1/((1/pow(calc_eDiffInp0[4],2)) + (1/pow(calc_eDiffOutp0[4],2))));

  TLatex* tDiffEPosInPOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeE[0]*0.85, Form("IN+OUT: %2.2f #pm %2.2f",calc_DiffInPOutp0[4],calc_eDiffInPOutp0[4]));
  tDiffEPosInPOut->SetTextSize(textSize[0]);
  tDiffEPosInPOut->SetTextColor(kBlack);
  TLatex* tDiffEPosInMOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeE[0]*1.10, Form("IN-OUT: %2.2f #pm %2.2f",calc_DiffInMOutp0[4],calc_eDiffInMOutp0[4]));
  tDiffEPosInMOut->SetTextSize(textSize[0]);
  tDiffEPosInMOut->SetTextColor(kBlack);

  tDiffEPosInPOut->Draw();
  tDiffEPosInMOut->Draw();

  TLatex* tDiffEIn = new TLatex(dummyArraySize*0.2,YRangeE[0]*1.15, "HWP-IN");
  tDiffEIn->SetTextSize(0.06);
  tDiffEIn->SetTextColor(kBlue);
  tDiffEIn->Draw();
  TLatex* tDiffEOut = new TLatex(dummyArraySize*1.15,YRangeE[0]*1.15, "HWP-OUT");
  tDiffEOut->SetTextSize(0.06);
  tDiffEOut->SetTextColor(kRed);
  tDiffEOut->Draw();


  pad112->cd(6);
  TGraphErrors * diffChargeGraphIN = new TGraphErrors((Int_t)run1in.size(),dummyINaxis,diffChargein.data(),myzero.data(),diffChargeEr2in.data());
  diffChargeGraphIN->SetName("diffChargeGraphIN");
  diffChargeGraphIN->SetMarkerColor(kBlue);
  diffChargeGraphIN->SetLineColor(kBlue);
  diffChargeGraphIN->SetMarkerStyle(21);
  diffChargeGraphIN->SetMarkerSize(markerSize[0]);
  diffChargeGraphIN->SetLineWidth(1);
  diffChargeGraphIN->Fit("fitfun6In","E M R F 0 Q");
  fitfun6In->SetLineStyle(1);
  fitfun6In->SetLineWidth(2);
  fitfun6In->SetLineColor(kBlue);
  fitfun6In->SetLineStyle(4);
  fitfun6In->SetLineWidth(2);


  TGraphErrors * diffChargeGraphOUT = new TGraphErrors((Int_t)run1out.size(),dummyOUTaxis,diffChargeout.data(),myzero.data(),diffChargeEr2out.data());
  diffChargeGraphOUT->SetName("diffChargeGraphOUT");
  diffChargeGraphOUT->SetMarkerColor(kRed);
  diffChargeGraphOUT->SetLineColor(kRed);
  diffChargeGraphOUT->SetMarkerStyle(25);
  diffChargeGraphOUT->SetMarkerSize(markerSize[0]);
  diffChargeGraphOUT->SetLineWidth(1);
  diffChargeGraphOUT->Fit("fitfun6Out","E M R F 0 Q");
  fitfun6Out->SetLineStyle(1);
  fitfun6Out->SetLineWidth(2);
  fitfun6Out->SetLineColor(kRed);
  fitfun6Out->SetLineStyle(5);
  fitfun6Out->SetLineWidth(2);

  TMultiGraph *diffChargeGraph = new TMultiGraph();
  diffChargeGraph->Add(diffChargeGraphIN);
  diffChargeGraph->Add(diffChargeGraphOUT);
  diffChargeGraph->Draw("AP");
  diffChargeGraph->SetTitle("");
//   diffChargeGraph->GetXaxis()->SetNdivisions(25,0,0);
  diffChargeGraph->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  diffChargeGraph->GetYaxis()->SetTitle(Form("Charge Asym. [ppb]"));
  diffChargeGraph->GetXaxis()->CenterTitle();
  diffChargeGraph->GetYaxis()->CenterTitle();
//   diffChargeGraph->GetXaxis()->SetLimits(-1,dummyOUTaxis[(Int_t)run1out.size()]+1);
  diffChargeGraph->GetXaxis()->SetLabelColor(0);
  diffChargeGraph->GetYaxis()->SetRangeUser(YRangeQ[0],YRangeQ[1]);

  fitfun6In->Draw("same");
  fitfun6Out->Draw("same");

  TPaveStats *statsDiffChargeIn =(TPaveStats*)diffChargeGraphIN->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDiffChargeOut =(TPaveStats*)diffChargeGraphOUT->GetListOfFunctions()->FindObject("stats");
  statsDiffChargeIn->SetTextColor(kBlue);
  statsDiffChargeIn->SetFillColor(kWhite); 
  statsDiffChargeOut->SetTextColor(kRed);
  statsDiffChargeOut->SetFillColor(kWhite);
  statsDiffChargeIn->SetTextSize(textSize[0]);
  statsDiffChargeOut->SetTextSize(textSize[0]);
  statsDiffChargeIn->SetX1NDC(x_lo_stat_in4);    statsDiffChargeIn->SetX2NDC(x_hi_stat_in4); 
  statsDiffChargeIn->SetY1NDC(y_lo_stat_in4);    statsDiffChargeIn->SetY2NDC(y_hi_stat_in4);
  statsDiffChargeOut->SetX1NDC(x_lo_stat_out4);  statsDiffChargeOut->SetX2NDC(x_hi_stat_out4); 
  statsDiffChargeOut->SetY1NDC(y_lo_stat_out4);  statsDiffChargeOut->SetY2NDC(y_hi_stat_out4);

  calc_DiffInp0[5]         =   fitfun6In->GetParameter(0);
  calc_eDiffInp0[5]        =   fitfun6In->GetParError(0);
  calc_DiffInChisquare[5]  =   fitfun6In->GetChisquare();
  calc_DiffInNDF[5]        =   fitfun6In->GetNDF();

  calc_DiffOutp0[5]        =   fitfun6Out->GetParameter(0);
  calc_eDiffOutp0[5]       =   fitfun6Out->GetParError(0);
  calc_DiffOutChisquare[5] =   fitfun6Out->GetChisquare();
  calc_DiffOutNDF[5]       =   fitfun6Out->GetNDF();

  calc_DiffInPOutp0[5]     =   (calc_DiffInp0[5]+calc_DiffOutp0[5])/2.0;
  calc_eDiffInPOutp0[5]    =   (sqrt(pow(calc_eDiffInp0[5],2)+pow(calc_eDiffOutp0[5],2)))/2.0;
  calc_DiffInMOutp0[5]     =   ((calc_DiffInp0[5]/pow(calc_eDiffInp0[5],2)) - (calc_DiffOutp0[5]/pow(calc_eDiffOutp0[5],2))) /((1/pow(calc_eDiffInp0[5],2)) + (1/pow(calc_eDiffOutp0[5],2)));
  calc_eDiffInMOutp0[5]    =   sqrt(1/((1/pow(calc_eDiffInp0[5],2)) + (1/pow(calc_eDiffOutp0[5],2))));

  TLatex* tDiffChargePosInPOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeQ[0]*0.85, Form("IN+OUT: %2.2f #pm %2.2f",calc_DiffInPOutp0[5],calc_eDiffInPOutp0[5]));
  tDiffChargePosInPOut->SetTextSize(textSize[0]);
  tDiffChargePosInPOut->SetTextColor(kBlack);
  TLatex* tDiffChargePosInMOut = new TLatex(dummyArraySize*1.93*xScalePlot,YRangeQ[0]*1.10, Form("IN-OUT: %2.2f #pm %2.2f",calc_DiffInMOutp0[5],calc_eDiffInMOutp0[5]));
  tDiffChargePosInMOut->SetTextSize(textSize[0]);
  tDiffChargePosInMOut->SetTextColor(kBlack);

  tDiffChargePosInPOut->Draw();
  tDiffChargePosInMOut->Draw();

  TLatex* tDiffQIn = new TLatex(dummyArraySize*0.2,YRangeQ[0]*1.15, "HWP-IN");
  tDiffQIn->SetTextSize(0.06);
  tDiffQIn->SetTextColor(kBlue);
  tDiffQIn->Draw();
  TLatex* tDiffQOut = new TLatex(dummyArraySize*1.15,YRangeQ[0]*1.15, "HWP-OUT");
  tDiffQOut->SetTextSize(0.06);
  tDiffQOut->SetTextColor(kRed);
  tDiffQOut->Draw();



  TString saveAllPlot = Form("dirPlot/resultPlot/%s_%s_%s_%s_regression_%s_%s_diff_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			       ,reg_calc.Data(),reg_set.Data(),database_stem.Data());

//   cout<<"valuesum:"<<valuesum<<"+-"<<valueerror<<endl;

  c11-> Update();
  c11->Print(saveAllPlot+".png");
  if(FIGURE){
    c11->Print(saveAllPlot+".svg");
    c11->Print(saveAllPlot+".C");
  }

  MyfileWrite.open(Form("dirPlot/resultText/%s_%s_%s_%s_beam_parameter_differences_regression_%s_%s_%s.txt"
			,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			,reg_calc.Data(),reg_set.Data(),database_stem.Data()));

  MyfileWrite<<calc_DiffInp0[0]<<" "<<calc_eDiffInp0[0]<<" "<<calc_DiffInp0[1]<<" "<<calc_eDiffInp0[1]<<" "<<calc_DiffInp0[2]<<" "<<calc_eDiffInp0[2]<<" "<<calc_DiffInp0[3]<<" "<<calc_eDiffInp0[3]<<" "<<calc_DiffInp0[4]<<" "<<calc_eDiffInp0[4]<<" "<<calc_DiffInp0[5]<<" "<<calc_eDiffInp0[5]<<"\t"<<calc_DiffOutp0[0]<<" "<<calc_eDiffOutp0[0]<<" "<<calc_DiffOutp0[1]<<" "<<calc_eDiffOutp0[1]<<" "<<calc_DiffOutp0[2]<<" "<<calc_eDiffOutp0[2]<<" "<<calc_DiffOutp0[3]<<" "<<calc_eDiffOutp0[3]<<" "<<calc_DiffOutp0[4]<<" "<<calc_eDiffOutp0[4]<<" "<<calc_DiffOutp0[5]<<" "<<calc_eDiffOutp0[5]<<endl;
  MyfileWrite.close();


  }

  /*********************************************************************/
  /*********************************************************************/
  /*********************************************************************/




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


