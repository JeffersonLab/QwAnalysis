//*****************************************************************************************************//
// Author      : Nuruzzaman, July 19th 2012, Transverse N2Delta.
// Last Update : 08/03/2012
// 
//*****************************************************************************************************//
/*
  This macro is to plot only the main detector data.
  It will connect by default to qw_run2_pass1 data base and get the slug averages from the main detector
  regressed data and plot them in to three plots of in,out,in+out/2 , in-out and sum of oppsite octants
  You have the option to change the database at the command line. Just type:
  e.g. 
  ./nurTransverse "qw_run1_pass4"
  
  ###############################################
  
  Slug averages of Main Detector Asymmetries
  
  ###############################################
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

  You will be promted to enter the target type, 
  To compile this code do a make.

******************************************************************************************************/

using namespace std;
#include "NurClass.h"


Bool_t SCALE = 0;
Bool_t FIGURE = 0;
Bool_t PRELIMINARY = 1;

// TString quartz_bar_SUM[8]= {"qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos",
// 			    "qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};
// TString deviceTitle = "Pos. PMT"; TString deviceName = "PosPMT";

// TString quartz_bar_SUM[8]= {"qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg",
// 			    "qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg"};
// TString deviceTitle = "Neg. PMT";  TString deviceName = "NegPMT";

// TString quartz_bar_SUM[8]= {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum",
// 			    "qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"}; 
// TString deviceTitle = "Barsum"; TString deviceName = "Barsum";

TString quartz_bar_SUM[8]= {"md1pmtavg","md2pmtavg","md3pmtavg","md4pmtavg",
			    "md5pmtavg","md6pmtavg","md7pmtavg","md8pmtavg"}; 
TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";


TSQLServer *db;
// TString database="qw_run2_pass1";
// TString database_stem="run2_pass1";
TString database="qw_run2_pass5";
TString database_stem="run2_pass5";

TText *t1;
TString target, polar,targ, goodfor, reg_set, reg_calc;

Int_t opt =1;
Int_t datopt = 2;
Int_t ropt = 2;
Int_t qtor_opt = 2;
Int_t runlet_sulg_opt = 2;

TString good_for,good_for_sen,good_for_diff;
TString qtor_current;
TString qtor_stem;
TString good;
TString interaction;
TString showFit1,showFit2,showFit3;
TString senOutput;
TString outputAvg;

Double_t value1[8] ={0.0};
Double_t err1[8] ={0.0};
Double_t rms1[8] ={0.0};
Double_t value2[8] ={0.0};
Double_t err2[8] ={0.0};
Double_t rms2[8] ={0.0};
Double_t value3[8] ={0.0};
Double_t err3[8] ={0.0};
Double_t rms3[8] ={0.0};
Double_t value11[8] ={0.0};
Double_t err11[8] ={0.0};
Double_t rms11[8] ={0.0};
Double_t value22[8] ={0.0};
Double_t err22[8] ={0.0};
Double_t rms22[8] ={0.0};
Double_t value33[8] ={0.0};
Double_t err33[8] ={0.0};
Double_t rms33[8] ={0.0};

Double_t value111[4] ={0.0};
Double_t err111[4] ={0.0};
Double_t value222[4] ={0.0};
Double_t err222[4] ={0.0};
Double_t rms222[8] ={0.0};

Double_t valuein[8] ={0.0};
Double_t errorin[8] ={0.0};
Double_t valueout[8] ={0.0};
Double_t errorout[8] ={0.0};

Double_t valueinX[8] ={0.0};
Double_t errinX[8] ={0.0};
Double_t rmsinX[8] ={0.0};
Double_t valueoutX[8] ={0.0};
Double_t erroutX[8] ={0.0};
Double_t rmsoutX[8] ={0.0};

Double_t valueinCharge[8] ={0.0};
Double_t errinCharge[8] ={0.0};
Double_t rmsinCharge[8] ={0.0};
Double_t valueoutCharge[8] ={0.0};
Double_t erroutCharge[8] ={0.0};
Double_t rmsoutCharge[8] ={0.0};


// Double_t run1[100][8] ={0.0};
// Double_t runletid1[100][8]={0.0};
Double_t run[8] ={0.0};
Double_t runletid[8]={0.0};
Double_t run1[8] ={0.0};
Double_t runletid1[8]={0.0};
Double_t run11[8] ={0.0};
Double_t runletid11[8]={0.0};

/* Canvas and Pad and Scale parameters */
Int_t canvasSize[2] ={1200,600};
Double_t pad1x[2] = {0.005,0.995};
Double_t pad1y[2] = {0.935,0.995};
Double_t pad2x[2] = {0.005,0.995};
Double_t pad2y[2] = {0.005,0.945};
Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
Double_t legendCoordinates[4] = {0.1,0.83,0.65,0.95};
Double_t yScale[2] = {-10.0,10.0};
Double_t waterMark[2] = {2.5,-1.0};

TString get_query(TString detector, TString measurement, TString ihwp);
// void get_octant_data(TString devicelist[],TString ihwp, Double_t value[], Double_t error[], Double_t rms[]);
void get_octant_data(TString devicelist[],TString ihwp, Double_t value[], Double_t error[], Double_t rms[], Double_t run[]);
void write_octant_data(TString devicelist[], Double_t valuein[], Double_t errorin[], Double_t valueout[], Double_t errorout[]);

Char_t  textfile1[400],textfile2[400],textfile3[400];
std::ofstream fileAsym,fileYield,fileSenX,fileSenY,fileSenE,fileSenXSlope,fileSenYSlope,fileSenCharge;

int main(Int_t argc,Char_t* argv[])
{


  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form(" \n          Main Detector Asymmetries        \n")<<std::endl;
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
    good = "9";
  }
  else{
    std::cout<<Form("Unknown polarization type!")<<std::endl;
    exit(1);
  }
  
  // Select the interaction
  if(ropt == 1){
    good_for = "(md_data_view.good_for_id = '3' or md_data_view.good_for_id = '1,"+good+"')";
    good_for_sen = "(md_slope_view.good_for_id = '3' or md_slope_view.good_for_id = '1,"+good+"')";
    interaction = "elastic";
    qtor_current=" (slow_controls_settings.qtor_current>8800 && slow_controls_settings.qtor_current<9000) ";
    qtor_stem = "8901";
  }
  else if(ropt == 2){
    good_for = "(md_data_view.good_for_id = '"+good+",18')";
    good_for_sen = "(md_slope_view.good_for_id = '"+good+",18')";
    interaction = "n-to-delta"; 
  }
  else if(ropt == 3){
    good_for = "(md_data_view.good_for_id = '1,3,21')";
    good_for_sen = "(md_slope_view.good_for_id = '1,3,21')";
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
  

  // Get current directory
  TString currentDir  =  gSystem->pwd();
  // Make plots directory
  gSystem->mkdir("dirPlot");

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //   printf ( "%sThe current date/time is: %s%s%s",blue,red,asctime(timeinfo),normal);

  std::cout<<Form("Getting slug averages of main detectors")<<std::endl;
  TApplication theApp("App",&argc,argv);
 
  //connect to the data base
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");


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


  if(db){
    printf("Database server info : %s\n", db->ServerInfo());
  }
  else
    exit(1);
  
  // clear arrays;
  for(Int_t i =0;i<8;i++){
    value1[i] = 0.0;
    err1[i] = 0.0;
    rms1[i] = 0.0;
    value11[i]=0.0;
    err11[i]=0.0;
    rms11[i] = 0.0;
    value22[i] =0.0;
    err22[i]=0.0;  
    rms22[i] = 0.0;
    value33[i]=0.0;
    err33[i]=0.0;
    rms33[i] = 0.0;
    run1[i] = 0.0;
    runletid1[i] = 0.0;
    run11[i] = 0.0;
    runletid11[i] = 0.0;
  }


  for(Int_t i =0;i<4;i++){
    value2[i] = 0.0;
    err2[i] = 0.0;
    rms2[i] = 0.0;
  }

  for(Int_t i =0;i<5;i++){
    value3[i] = 0.0;
    err3[i] = 0.0;
    rms3[i] = 0.0;
  }


  /* Creating file for writing results */
  fileSenX.open(Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_off_%s_senX_%s.txt"
		     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
		     ,deviceName.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));

  fileSenY.open(Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_off_%s_senY_%s.txt"
		     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
		     ,deviceName.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));
  
  fileSenE.open(Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_off_%s_senE_%s.txt"
		     ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
		     ,deviceName.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));
  
  fileSenXSlope.open(Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_off_%s_senXSlope_%s.txt"
			  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			  ,deviceName.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));
  
  fileSenYSlope.open(Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_off_%s_senYSlope_%s.txt"
			  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			  ,deviceName.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));
  
  fileSenCharge.open(Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_off_%s_senCharge_%s.txt"
			  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			  ,deviceName.Data(),reg_set.Data(),outputAvg.Data(),database_stem.Data()));


//   get_octant_data(quartz_bar_SUM,"out", valueoutX,  erroutX, rmsoutX);
//   get_octant_data(quartz_bar_SUM,"in",  valueinX,  errinX, rmsinX);

//   get_octant_data(quartz_bar_SUM,"out", valueoutCharge,  erroutCharge, rmsoutCharge);
//   get_octant_data(quartz_bar_SUM,"in",  valueinCharge,  errinCharge, rmsinCharge);

//   write_octant_data(quartz_bar_SUM, valueinX, errinX, valueoutX, erroutX);
//   write_octant_data(quartz_bar_SUM, valueinCharge, errinCharge, valueoutCharge, erroutCharge);
  
  get_octant_data(quartz_bar_SUM,"out", value1,  err1, rms1, run1);
  get_octant_data(quartz_bar_SUM,"in",  value11, err11, rms11, run11);
  

//   get_octant_yield(quartz_bar_SUM,"out", value22,  err22, rms22);
//   get_octant_yield(quartz_bar_SUM,"in",  value33, err33, rms33);


//   get_octant_yield(quartz_bar_SUM,"out", value1,  err1, rms1);
//   get_octant_yield(quartz_bar_SUM,"in",  value11, err11, rms11);

//   write_octant_data(quartz_bar_SUM, value11, err11, value1, err1);

  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);
  //  std::cout<<Form("Done plotting fits \n");
  db->Close();
  delete db;

  theApp.Run();

  fileSenX.close();
  fileSenY.close();
  fileSenE.close();
  fileSenXSlope.close();
  fileSenYSlope.close();
  fileSenCharge.close();

  return(1);

};

//***************************************************
//***************************************************
//         get query              
//***************************************************
//***************************************************
TString get_query(TString detector, TString measurement, TString ihwp){
  
  Bool_t ldebug = 0;

  TString datatable     =  "md_data_view";
  TString senDatatable  =  "md_slope_view";
  TString diffDatatable =  "md_slope_view";

  TString regression = Form("%s.slope_calculation = '%s' AND %s.slope_correction = '%s' ",
			    datatable.Data(),reg_calc.Data(),datatable.Data(),reg_set.Data()); 

  if(runlet_sulg_opt == 1){
    senOutput = "sum( distinct("+senDatatable+".value/(POWER("
      +senDatatable+".error,2))))/sum( distinct(1/(POWER("
      +senDatatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
      +senDatatable+".error,2))))), SQRT(SUM("+senDatatable+".n))*SQRT(1/SUM(distinct(1/(POWER("
      +senDatatable+".error,2))))) , "+senDatatable+".slug";
  }
  if(runlet_sulg_opt == 2){
    senOutput = " distinct("+senDatatable+".value) , "
      +senDatatable+".error , "+senDatatable+".error*SQRT("+senDatatable+".n) , "
      "(run_number + segment_number*0.01) , "+senDatatable+".runlet_id ";
  }

//   TString senOutput = " sum( distinct("+senDatatable+".value/(POWER("
//     +senDatatable+".error,2))))/sum( distinct(1/(POWER("
//     +senDatatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
//     +senDatatable+".error,2)))))";

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			      datatable.Data());

  TString run_quality_sen =  Form("(%s.run_quality_id = '1') ",
			      senDatatable.Data());


//   /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
//   TString asym_query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
//     +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
//     +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
//     +datatable+".measurement_type = '"+measurement+"' AND target_position = '"+target+"' AND "
//     +regression+" AND "+run_quality+" AND "
//     +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
//     +datatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";


//   /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
//   TString yield_query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
//     +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
//     +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
//     +datatable+".measurement_type = '"+measurement+"' AND target_position = '"+target+"' AND "
//     +regressionY+" AND "+run_quality+" AND "
//     +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
//     +datatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";



  /*Select md barsum sensitivities for LH2 from parity, production that are good/suspect*/
  TString senX_query =" SELECT " +senOutput+ " FROM "+senDatatable+", slow_controls_settings WHERE "
    +senDatatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +senDatatable+".detector = '"+detector+"' AND "+senDatatable+".subblock = 0 AND "
    "target_position = '"+target+"' AND "
    +senDatatable+".slope_calculation = '"+reg_set+"' AND "+senDatatable+".slope_correction = 'off' AND "
//     +run_quality_sen+" AND slope = 'wrt_diff_targetX' AND "
    +run_quality_sen+" AND slope = '"+measurement+"' AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for_sen+" AND "
    +senDatatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";

  TString senY_query =" SELECT " +senOutput+ " FROM "+senDatatable+", slow_controls_settings WHERE "
    +senDatatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +senDatatable+".detector = '"+detector+"' AND "+senDatatable+".subblock = 0 AND "
    "target_position = '"+target+"' AND "
    +senDatatable+".slope_calculation = '"+reg_set+"' AND "+senDatatable+".slope_correction = 'off' AND "
    +run_quality_sen+" AND slope = '"+measurement+"' AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for_sen+" AND "
    +senDatatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";

  TString senE_query =" SELECT " +senOutput+ " FROM "+senDatatable+", slow_controls_settings WHERE "
    +senDatatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +senDatatable+".detector = '"+detector+"' AND "+senDatatable+".subblock = 0 AND "
    "target_position = '"+target+"' AND "
    +senDatatable+".slope_calculation = '"+reg_set+"' AND "+senDatatable+".slope_correction = 'off' AND "
    +run_quality_sen+" AND slope = '"+measurement+"' AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for_sen+" AND "
    +senDatatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";

  TString senXSlope_query =" SELECT " +senOutput+ " FROM "+senDatatable+", slow_controls_settings WHERE "
    +senDatatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +senDatatable+".detector = '"+detector+"' AND "+senDatatable+".subblock = 0 AND "
    "target_position = '"+target+"' AND "
    +senDatatable+".slope_calculation = '"+reg_set+"' AND "+senDatatable+".slope_correction = 'off' AND "
    +run_quality_sen+" AND slope = '"+measurement+"' AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for_sen+" AND "
    +senDatatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";

  TString senYSlope_query =" SELECT " +senOutput+ " FROM "+senDatatable+", slow_controls_settings WHERE "
    +senDatatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +senDatatable+".detector = '"+detector+"' AND "+senDatatable+".subblock = 0 AND "
    "target_position = '"+target+"' AND "
    +senDatatable+".slope_calculation = '"+reg_set+"' AND "+senDatatable+".slope_correction = 'off' AND "
    +run_quality_sen+" AND slope = '"+measurement+"' AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for_sen+" AND "
    +senDatatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";

  TString senCharge_query =" SELECT " +senOutput+ " FROM "+senDatatable+", slow_controls_settings WHERE "
    +senDatatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +senDatatable+".detector = '"+detector+"' AND "+senDatatable+".subblock = 0 AND "
    "target_position = '"+target+"' AND "
    +senDatatable+".slope_calculation = '"+reg_set+"' AND "+senDatatable+".slope_correction = 'off' AND "
    +run_quality_sen+" AND slope = '"+measurement+"' AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for_sen+" AND "
    +senDatatable+".error != 0 AND run_number>16000 AND "+qtor_current+"; ";




  if(ldebug){
    std::cout<< senX_query       <<std::endl;
    std::cout<< senY_query       <<std::endl;
    std::cout<< senE_query       <<std::endl;
    std::cout<< senXSlope_query  <<std::endl;
    std::cout<< senYSlope_query  <<std::endl;
    std::cout<< senCharge_query  <<std::endl;
  }

  return senX_query;
  return senY_query;
  return senE_query;
  return senXSlope_query;
  return senYSlope_query;
  return senCharge_query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************
void get_octant_data(TString devicelist[], TString ihwp, Double_t value[], Double_t error[], Double_t rms[], Double_t run[])
{
  Bool_t ldebug = 1;

  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) {
      printf("Getting data for %15s ihwp %5s\n", devicelist[i].Data(), ihwp.Data());
    }

    /**********************************************/
    TString senX_query         =  get_query(Form("%s",devicelist[i].Data()),"wrt_diff_targetX",ihwp);
    TSQLStatement* senX_stmt   =  db->Statement(senX_query,500);
    if(!senX_stmt){
      db->Close(); exit(1);
    }
    // process statement for senX
    if (senX_stmt->Process()) {
      // store result of statement in buffer for senX
      senX_stmt->StoreResult();
      while (senX_stmt->NextResultRow()) {
	value[i] =      (Double_t)(senX_stmt->GetDouble(0)); // senX, ppm/nm
	error[i] =      (Double_t)(senX_stmt->GetDouble(1)); // senX error, ppm/nm
	rms[i] =      (Double_t)(senX_stmt->GetDouble(2)); // senX, rms ppm/nm
	run[i] =      (Double_t)(senX_stmt->GetDouble(3));     // run number
	// runletid[i] = (Double_t)(senX_stmt->GetDouble(4)); // runlet id

	if(runlet_sulg_opt == 1){
	  if(ldebug){printf("%sMD %s %d %5.0f SenX %s = %g+-%g %g [ppm/nm] %s\n", red, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenX<<Form("%d %5.0f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}
	if(runlet_sulg_opt == 2){
	  if(ldebug){printf("%sMD %s %d SenX %5.2f %s = %g+-%g %g [ppm/nm] %s\n", red, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenX<<Form("%d %5.2f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}

      }
      
    }
    delete senX_stmt;

    /**********************************************/

    TString senY_query         =  get_query(Form("%s",devicelist[i].Data()),"wrt_diff_targetY",ihwp);
    TSQLStatement* senY_stmt   =  db->Statement(senY_query,500);
    if(!senY_stmt){
      db->Close(); exit(1);
    }
    // process statement for senY
    if (senY_stmt->Process()) {
      // store result of statement in buffer for senY
      senY_stmt->StoreResult();
      while (senY_stmt->NextResultRow()) {
	value[i] =      (Double_t)(senY_stmt->GetDouble(0)); // senY, convert to ppm/nm
	error[i] =      (Double_t)(senY_stmt->GetDouble(1)); // senY error, ppm/nm
	rms[i] =      (Double_t)(senY_stmt->GetDouble(2)); // senY, rms ppm/nm
	run[i] =      (Double_t)(senY_stmt->GetDouble(3));     // run number

	if(runlet_sulg_opt == 1){
	  if(ldebug){printf("%sMD %s %d %5.0f SenY %s = %g+-%g %g [ppm/nm] %s\n", green, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenY<<Form("%d %5.0f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}
	if(runlet_sulg_opt == 2){
	  if(ldebug){printf("%sMD %s %d SenY %5.2f %s = %g+-%g %g [ppm/nm] %s\n", green, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenY<<Form("%d %5.2f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}

      }

    }
    delete senY_stmt;

    /**********************************************/

    TString senE_query         =  get_query(Form("%s",devicelist[i].Data()),"wrt_diff_energy",ihwp);
    TSQLStatement* senE_stmt   =  db->Statement(senE_query,500);
    if(!senE_stmt){
      db->Close(); exit(1);
    }
    // process statement for senE
    if (senE_stmt->Process()) {
      // store result of statement in buffer for senE
      senE_stmt->StoreResult();
      while (senE_stmt->NextResultRow()) {
	value[i] =      (Double_t)(senE_stmt->GetDouble(0)); // senE, dim. less, ppm/ppm
	error[i] =      (Double_t)(senE_stmt->GetDouble(1)); // senE error, dim. less, ppm/ppm
	rms[i] =      (Double_t)(senE_stmt->GetDouble(2)); // senE, rms dim. less, ppm/ppm
	run[i] =      (Double_t)(senE_stmt->GetDouble(3)); // run number

	if(runlet_sulg_opt == 1){
	  if(ldebug){printf("%sMD %s %d %5.0f SenE %s = %g+-%g %g [ppm/ppm] %s\n", blue, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenE<<Form("%d %5.0f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}
	if(runlet_sulg_opt == 2){
	  if(ldebug){printf("%sMD %s %d SenE %5.2f %s = %g+-%g %g [ppm/ppm] %s\n", blue, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenE<<Form("%d %5.2f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}

      }

    }
    delete senE_stmt;

    /**********************************************/

    TString senXSlope_query         =  get_query(Form("%s",devicelist[i].Data()),"wrt_diff_targetXSlope",ihwp);
    TSQLStatement* senXSlope_stmt   =  db->Statement(senXSlope_query,500);
    if(!senXSlope_stmt){
      db->Close(); exit(1);
    }
    // process statement for senXSlope
    if (senXSlope_stmt->Process()) {
      // store result of statement in buffer for senXSlope
      senXSlope_stmt->StoreResult();
      while (senXSlope_stmt->NextResultRow()) {
	value[i] =      (Double_t)(senXSlope_stmt->GetDouble(0)); // senXSlope, ppm/murad
	error[i] =      (Double_t)(senXSlope_stmt->GetDouble(1)); // senXSlope error, ppm/murad
	rms[i] =      (Double_t)(senXSlope_stmt->GetDouble(2)); // senXSlope, rms ppm/murad
	run[i] =      (Double_t)(senXSlope_stmt->GetDouble(3)); // run number

	if(runlet_sulg_opt == 1){
	  if(ldebug){printf("%sMD %s %d %5.0f SenXSlope %s = %g+-%g %g [ppm/murad] %s\n", red, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenXSlope<<Form("%d %5.0f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}
	if(runlet_sulg_opt == 2){
	  if(ldebug){printf("%sMD %s %d SenXSlope %5.2f %s = %g+-%g %g [ppm/murad] %s\n", red, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenXSlope<<Form("%d %5.2f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}

      }

    }
    delete senXSlope_stmt;

    /**********************************************/

    TString senYSlope_query         =  get_query(Form("%s",devicelist[i].Data()),"wrt_diff_targetYSlope",ihwp);
    TSQLStatement* senYSlope_stmt   =  db->Statement(senYSlope_query,500);
    if(!senYSlope_stmt){
      db->Close(); exit(1);
    }
    // process statement for senYSlope
    if (senYSlope_stmt->Process()) {
      // store result of statement in buffer for senYSlope
      senYSlope_stmt->StoreResult();
      while (senYSlope_stmt->NextResultRow()) {
	value[i] =      (Double_t)(senYSlope_stmt->GetDouble(0)); // senYSlope, ppm/murad
	error[i] =      (Double_t)(senYSlope_stmt->GetDouble(1)); // senYSlope error, ppm/murad
	rms[i] =      (Double_t)(senYSlope_stmt->GetDouble(2)); // senYSlope, rms ppm/murad
	run[i] =      (Double_t)(senYSlope_stmt->GetDouble(3)); // run number

	if(runlet_sulg_opt == 1){
	  if(ldebug){printf("%sMD %s %d %5.0f SenYSlope %s = %g+-%g %g[ppm/murad] %s\n", green, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenYSlope<<Form("%d %5.0f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}
	if(runlet_sulg_opt == 2){
	  if(ldebug){printf("%sMD %s %d SenYSlope %5.2f %s = %g+-%g %g [ppm/murad] %s\n", green, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenYSlope<<Form("%d %5.2f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}

      }

    }
    delete senYSlope_stmt;

    /**********************************************/

    TString senCharge_query         =  get_query(Form("%s",devicelist[i].Data()),"wrt_asym_charge",ihwp);
    TSQLStatement* senCharge_stmt   =  db->Statement(senCharge_query,500);
    if(!senCharge_stmt){
      db->Close(); exit(1);
    }
    // process statement for senCharge
    if (senCharge_stmt->Process()) {
      // store result of statement in buffer for senCharge
      senCharge_stmt->StoreResult();
      while (senCharge_stmt->NextResultRow()) {
	value[i] =      (Double_t)(senCharge_stmt->GetDouble(0)); // senCharge, ppm/ppm
	error[i] =      (Double_t)(senCharge_stmt->GetDouble(1)); // senCharge error, ppm/ppm
	rms[i] =      (Double_t)(senCharge_stmt->GetDouble(2)); // senCharge, rms ppm/ppm
	run[i] =      (Double_t)(senCharge_stmt->GetDouble(3)); // run number

	if(runlet_sulg_opt == 1){
	  if(ldebug){printf("%sMD %s %d %5.0f SenCharge %s = %g+-%g %g [ppm/ppm] %s\n", magenta, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenCharge<<Form("%d %5.0f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}
	if(runlet_sulg_opt == 2){
	  if(ldebug){printf("%sMD %s %d SenCharge %5.2f %s = %g+-%g %g [ppm/ppm] %s\n", magenta, deviceName.Data(), i+1, run[i], ihwp.Data(), value[i], error[i], rms[i], normal);}
	  fileSenCharge<<Form("%d %5.2f %s\t %g %g %g", i+1, run[i], ihwp.Data(), value[i], error[i], rms[i])<<endl;
	}

      }

    }
    delete senCharge_stmt;

    /**********************************************/
    /**********************************************/

  }

}


