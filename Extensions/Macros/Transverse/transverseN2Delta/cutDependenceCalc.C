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
  std::cout<<Form("1. Longitudinal Transverse ")<<std::endl;
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
     printf("%sNo textfile exist for the configuration.dirPlot/%s_%s_%s_%s_regression_%s_%s_%s_diff_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data(),normal);
     return -1;
   }
   runletDiff.close();






   ///*****************************************************//

  Int_t counter12=0;
  ifstream runletDiffLimits;
  const Int_t NUM12 = 2;
  Double_t XWidthIn[NUM12],YWidthIn[NUM12],XSlopeWidthIn[NUM12],YSlopeWidthIn[NUM12],EWidthIn[NUM12],ChargeWidthIn[NUM12];
  Double_t XWidthOut[NUM12],YWidthOut[NUM12],XSlopeWidthOut[NUM12],YSlopeWidthOut[NUM12],EWidthOut[NUM12],ChargeWidthOut[NUM12];
  runletDiffLimits.open(Form("dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_%s_cutDependenceWidth_%s.txt"
			    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			    ,reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

  if (runletDiffLimits.is_open()) {
    
    while(1) {
      
      runletDiffLimits >> 
	XWidthIn[counter12] >> 
	YWidthIn[counter12] >> 
	XSlopeWidthIn[counter12] >> 
	YSlopeWidthIn[counter12] >> 
	EWidthIn[counter12] >> 
	ChargeWidthIn[counter12] >> 
	XWidthOut[counter12] >> 
	YWidthOut[counter12] >> 
	XSlopeWidthOut[counter12] >> 
	YSlopeWidthOut[counter12] >> 
	EWidthOut[counter12] >> 
	ChargeWidthOut[counter12]; 
      
      if (!runletDiffLimits.good()) break;

      
       
      }
      
      counter12++;
    }
   else {
     printf("%sNo textfile exist for the configuration.dirPlot/cutDependence/%s_%s_%s_%s_regression_%s_%s_%s_cutDependenceWidth_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_calc.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data(),normal);
     return -1;
   }
   runletDiffLimits.close();




   cout<<green<<"X "<<XWidthIn[0]<<"\tY "<<YWidthIn[0]<<"\tXSlope "<<XSlopeWidthIn[0]<<"\tYSlope "<<YSlopeWidthIn[0]<<"\tE "<<EWidthIn[0]<<"\tCharge "<<ChargeWidthIn[0] <<normal<<endl;
   cout<<green<<"X "<<XWidthOut[0]<<"\tY "<<YWidthOut[0]<<"\tXSlope "<<XSlopeWidthOut[0]<<"\tYSlope "<<YSlopeWidthOut[0]<<"\tE "<<EWidthOut[0]<<"\tCharge "<<ChargeWidthOut[0] <<normal<<endl;




  Double_t dummyINaxis[(Int_t)run1in.size()];
  Double_t dummyOUTaxis[(Int_t)run1out.size()];
  Int_t dummyArraySize = (Int_t)run1in.size() > (Int_t)run1out.size() ? (Int_t)run1in.size() : (Int_t)run1out.size();
  TString xAxisTitle = "Runlets";

  for(Int_t j = 0; j< dummyArraySize; j++) {
    dummyINaxis[j-1] = j;
    dummyOUTaxis[j-1] = j + dummyArraySize;

  }


  Double_t sigma = 1;

  for ( Int_t i=0; i<run1in.size(); i++) {
    
    // cout<<blue<<Form("IN Runlet = %5.2f : X = %5.2f\t Y = %5.2f\t X' = %5.2f\t Y' = %5.2f\t E = %5.2f\t Q = %5.2f\t",run1in[i],diffXin[i],diffYin[i],diffXSlopein[i],diffYSlopein[i],diffEin[i],diffChargein[i] )<<normal<<endl;

    sigma = 5;
    if(diffXin[i]>sigma*XWidthIn[0] || diffXin[i]<-1*sigma*XWidthIn[0] || diffYin[i]>sigma*YWidthIn[0] || diffYin[i]<-1*sigma*YWidthIn[0] || diffXSlopein[i]>sigma*XSlopeWidthIn[0] || diffYSlopein[i]<-1*sigma*YSlopeWidthIn[0] || diffEin[i]<-1*sigma*EWidthIn[0] || diffChargein[i]<-1*sigma*ChargeWidthIn[0] ){
      cout<<blue<<"Cut IN runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1in[i])<<normal<<endl;
    }

    sigma = 4;
    if(diffXin[i]>sigma*XWidthIn[0] || diffXin[i]<-1*sigma*XWidthIn[0] || diffYin[i]>sigma*YWidthIn[0] || diffYin[i]<-1*sigma*YWidthIn[0] || diffXSlopein[i]>sigma*XSlopeWidthIn[0] || diffYSlopein[i]<-1*sigma*YSlopeWidthIn[0] || diffEin[i]<-1*sigma*EWidthIn[0] || diffChargein[i]<-1*sigma*ChargeWidthIn[0] ){
      cout<<blue<<"Cut IN runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1in[i])<<normal<<endl;
    }

    sigma = 3;
    if(diffXin[i]>sigma*XWidthIn[0] || diffXin[i]<-1*sigma*XWidthIn[0] || diffYin[i]>sigma*YWidthIn[0] || diffYin[i]<-1*sigma*YWidthIn[0] || diffXSlopein[i]>sigma*XSlopeWidthIn[0] || diffYSlopein[i]<-1*sigma*YSlopeWidthIn[0] || diffEin[i]<-1*sigma*EWidthIn[0] || diffChargein[i]<-1*sigma*ChargeWidthIn[0] ){
      cout<<blue<<"Cut IN runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1in[i])<<normal<<endl;
    }

    sigma = 2.5;
    if(diffXin[i]>sigma*XWidthIn[0] || diffXin[i]<-1*sigma*XWidthIn[0] || diffYin[i]>sigma*YWidthIn[0] || diffYin[i]<-1*sigma*YWidthIn[0] || diffXSlopein[i]>sigma*XSlopeWidthIn[0] || diffYSlopein[i]<-1*sigma*YSlopeWidthIn[0] || diffEin[i]<-1*sigma*EWidthIn[0] || diffChargein[i]<-1*sigma*ChargeWidthIn[0] ){
      cout<<blue<<"Cut IN runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1in[i])<<normal<<endl;
    }

    sigma = 2;
    if(diffXin[i]>sigma*XWidthIn[0] || diffXin[i]<-1*sigma*XWidthIn[0] || diffYin[i]>sigma*YWidthIn[0] || diffYin[i]<-1*sigma*YWidthIn[0] || diffXSlopein[i]>sigma*XSlopeWidthIn[0] || diffYSlopein[i]<-1*sigma*YSlopeWidthIn[0] || diffEin[i]<-1*sigma*EWidthIn[0] || diffChargein[i]<-1*sigma*ChargeWidthIn[0] ){
      cout<<blue<<"Cut IN runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1in[i])<<normal<<endl;
    }

  }



  for ( Int_t i=0; i<run1out.size(); i++) {
    
    // cout<<blue<<Form("OUT Runlet = %5.2f : X = %5.2f\t Y = %5.2f\t X' = %5.2f\t Y' = %5.2f\t E = %5.2f\t Q = %5.2f\t",run1out[i],diffXout[i],diffYout[i],diffXSlopeout[i],diffYSlopeout[i],diffEout[i],diffChargeout[i] )<<normal<<endl;

    sigma = 5;
    if(diffXout[i]>sigma*XWidthOut[0] || diffXout[i]<-1*sigma*XWidthOut[0] || diffYout[i]>sigma*YWidthOut[0] || diffYout[i]<-1*sigma*YWidthOut[0] || diffXSlopeout[i]>sigma*XSlopeWidthOut[0] || diffYSlopeout[i]<-1*sigma*YSlopeWidthOut[0] || diffEout[i]<-1*sigma*EWidthOut[0] || diffChargeout[i]<-1*sigma*ChargeWidthOut[0] ){
      cout<<red<<"Cut OUT runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1out[i])<<normal<<endl;
    }

    sigma = 4;
    if(diffXout[i]>sigma*XWidthOut[0] || diffXout[i]<-1*sigma*XWidthOut[0] || diffYout[i]>sigma*YWidthOut[0] || diffYout[i]<-1*sigma*YWidthOut[0] || diffXSlopeout[i]>sigma*XSlopeWidthOut[0] || diffYSlopeout[i]<-1*sigma*YSlopeWidthOut[0] || diffEout[i]<-1*sigma*EWidthOut[0] || diffChargeout[i]<-1*sigma*ChargeWidthOut[0] ){
      cout<<red<<"Cut OUT runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1out[i])<<normal<<endl;
    }

    sigma = 3;
    if(diffXout[i]>sigma*XWidthOut[0] || diffXout[i]<-1*sigma*XWidthOut[0] || diffYout[i]>sigma*YWidthOut[0] || diffYout[i]<-1*sigma*YWidthOut[0] || diffXSlopeout[i]>sigma*XSlopeWidthOut[0] || diffYSlopeout[i]<-1*sigma*YSlopeWidthOut[0] || diffEout[i]<-1*sigma*EWidthOut[0] || diffChargeout[i]<-1*sigma*ChargeWidthOut[0] ){
      cout<<red<<"Cut OUT runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1out[i])<<normal<<endl;
    }

    sigma = 2.5;
    if(diffXout[i]>sigma*XWidthOut[0] || diffXout[i]<-1*sigma*XWidthOut[0] || diffYout[i]>sigma*YWidthOut[0] || diffYout[i]<-1*sigma*YWidthOut[0] || diffXSlopeout[i]>sigma*XSlopeWidthOut[0] || diffYSlopeout[i]<-1*sigma*YSlopeWidthOut[0] || diffEout[i]<-1*sigma*EWidthOut[0] || diffChargeout[i]<-1*sigma*ChargeWidthOut[0] ){
      cout<<red<<"Cut OUT runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1out[i])<<normal<<endl;
    }

    sigma = 2;
    if(diffXout[i]>sigma*XWidthOut[0] || diffXout[i]<-1*sigma*XWidthOut[0] || diffYout[i]>sigma*YWidthOut[0] || diffYout[i]<-1*sigma*YWidthOut[0] || diffXSlopeout[i]>sigma*XSlopeWidthOut[0] || diffYSlopeout[i]<-1*sigma*YSlopeWidthOut[0] || diffEout[i]<-1*sigma*EWidthOut[0] || diffChargeout[i]<-1*sigma*ChargeWidthOut[0] ){
      cout<<red<<"Cut OUT runlet for "<<sigma<<" sigma "<<Form("%5.2f",run1out[i])<<normal<<endl;
    }

  }






  /*********************************************************************/
  /*********************************************************************/
  /*********************************************************************/




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


