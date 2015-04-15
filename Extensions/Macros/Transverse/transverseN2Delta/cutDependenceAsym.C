//*****************************************************************************************************//
// Author      : Nuruzzaman on 08/06/2012
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
//   printf ( "%sThe current date/time is: %s%s%s",blue,red,asctime(timeinfo),normal);

  // }
  // void plotTransverseN2Delta(){

  Bool_t SCALE = 0;
  Bool_t FIGURE = 0;
  Bool_t PRELIMINARY = 0;


//   TString database="qw_run2_pass1";
//   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";

//   TString deviceTitle = "Pos. PMT"; TString deviceName = "PosPMT";
//   TString deviceTitle = "Neg. PMT";  TString deviceName = "NegPMT";
  TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";
  //  TString deviceTitle = "Barsum"; TString deviceName = "Barsum";

  std::ofstream Myfile3;

  TString target, polar,targ, goodfor, reg_set, reg_calc, sigma_set, sigma_draw;

  Int_t opt =1;
  Int_t datopt = 3;
  Int_t ropt = 2;
  Int_t qtor_opt = 2;
  Int_t sigmaopt = 3;

  TString good_for;
  TString qtor_current;
  TString qtor_stem;
  TString good;
  TString interaction;
  TString showFit1,showFit2,showFit3;


//   Double_t value1[8] ={0.0};
//   Double_t err1[8] ={0.0};
//   Double_t value2[8] ={0.0};
//   Double_t err2[8] ={0.0};
//   Double_t value3[8] ={0.0};
//   Double_t err3[8] ={0.0};
//   Double_t value11[8] ={0.0};
//   Double_t err11[8] ={0.0};
//   Double_t value22[8] ={0.0};
//   Double_t err22[8] ={0.0};
//   Double_t value33[8] ={0.0};
//   Double_t err33[8] ={0.0};

//   Double_t value111[4] ={0.0};
//   Double_t err111[4] ={0.0};
//   Double_t value222[4] ={0.0};
//   Double_t err222[4] ={0.0};

//   Double_t valuein[8] ={0.0};
//   Double_t errin[8] ={0.0};
//   Double_t valueout[8] ={0.0};
//   Double_t errout[8] ={0.0};


  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[2] ={1200,650};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.900,0.75,0.995};
  Double_t yScale[2] = {-10.0,10.0};
  Double_t waterMark[2] = {2.5,-1.0};

  Double_t x_lo_stat_in4,y_lo_stat_in4,x_hi_stat_in4,y_hi_stat_in4,x_lo_stat_out4,y_lo_stat_out4,x_hi_stat_out4,y_hi_stat_out4;
  x_lo_stat_in4=0.76;y_lo_stat_in4=0.64;x_hi_stat_in4=0.99;y_hi_stat_in4=0.95;
  x_lo_stat_out4=0.76;y_lo_stat_out4=0.30;x_hi_stat_out4=0.99;y_hi_stat_out4=0.61;
  Double_t x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4;
  x_lo_leg4=0.76;y_lo_leg4=0.10;x_hi_leg4=0.99;y_hi_leg4=0.31;


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


  std::cout<<Form("Enter sigma cut (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. 2 sigma")<<std::endl;
  std::cout<<Form("2. 2.5 sigma")<<std::endl;
  std::cout<<Form("3. %s3 sigma (deafult)%s",blue,normal)<<std::endl;
  std::string input_sigmaopt;
  std::getline( std::cin, input_sigmaopt );
  if ( !input_sigmaopt.empty() ) {
    std::istringstream stream( input_sigmaopt );
    stream >> sigmaopt;
  }
  if(sigmaopt == 1){
    sigma_set = "2sigma";
    sigma_draw = "2#sigma";
  }
  else if(sigmaopt == 2){
    sigma_set = "2.5sigma";
    sigma_draw = "2.5#sigma";
  }
  else if(sigmaopt == 3){
    sigma_set = "3sigma";
    sigma_draw = "3#sigma";
  }
  else{
    std::cout<<Form("Unknown polarization type!")<<std::endl;
    exit(1);
  }
  


  if(argc>1) database = argv[1];

  // select the target
  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 % Al";
  }
  else if(opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 % Carbon";
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

  // }

  // void plotTransverseN2Delta(Int_t argc,Char_t* argv[]){

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


  // Fit function to show 
  showFit1 = "FIT = A_{M} cos(#phi) - C sin(#phi) + D";
  showFit2 = "FIT = A_{M} cos(#phi + #phi_{0}) + C";
  showFit3 = "FIT = A_{M} sin(#phi + #phi_{0}) + C";


  TString title1;
  TString titleSummary = Form("%s (%s, %s A): Regression-%s MD %s Asymmetries. %s cut. "
			      ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),deviceTitle.Data(),sigma_draw.Data());

  if(datopt==1){ title1= Form("%s %s",titleSummary.Data(),showFit1.Data());}
  else if(datopt==2){ title1= Form("%s %s",titleSummary.Data(),showFit2.Data());}
  else{
    title1= Form("%s %s",titleSummary.Data(),showFit3.Data());
  }

  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,canvasSize[0],canvasSize[1]);
  Canvas1->Draw();
  Canvas1->SetBorderSize(0);
  Canvas1->cd();
  TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  TLatex * t1 = new TLatex(0.06,0.3,Form("%s",title1.Data()));
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();


  Char_t  textfile[400];
  sprintf(textfile,"dirPlot/cutDependence/%s_%s_%s_%s_MD_%s_regression_%s_%s_cutDependence_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),sigma_set.Data(),database_stem.Data()); 

//   printf("%sText File = %s\n%s",green,textfile,normal);

  const Int_t NUM = 100;

  int counter=0;
  ifstream transverseMDAsym;
  Double_t zero[NUM],octant[NUM],valuesin[NUM],errorsin[NUM],valuesout[NUM],errorsout[NUM];


  transverseMDAsym.open(Form("%s",textfile));
  printf(Form("%s\n",textfile));
 
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
    printf("%sNo textfile: dirPlot/cutDependence/%s_%s_%s_%s_MD_%s_regression_%s_%s_cutDependence_%s_%s.txt Exiting program!\n%s",red,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),deviceName.Data(),reg_calc.Data(),reg_set.Data(),sigma_set.Data(),database_stem.Data(),normal);
    return -1;
  }
  transverseMDAsym.close();


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
    valuesum[i]=(valuesin[i]+valuesout[i])/2.0;
    valueerror[i]= (sqrt(pow(errorsin[i],2)+pow(errorsout[i],2)))/2.0;
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
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
  TGraphErrors* grp_in  = new TGraphErrors(k,octant,valuesin,zero,errorsin);
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
  TGraphErrors* grp_out  = new TGraphErrors(k,octant,valuesout,zero,errorsout);
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

  Myfile3<<Form("%4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.4f %4.0f %4.4f",p0diff,ep0diff,p1diff,ep1diff,p2diff,ep2diff,Chidiff,NDFdiff,Probdiff)<<endl;
  Myfile3.close();


  TString saveSummaryPlot = Form("dirPlot/cutDependence/%s_%s_%s_%s_MD_%s_regression_%s_%s_cutDependence_%s_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),sigma_set.Data(),database_stem.Data());

  Canvas1->Update();
  Canvas1->Print(saveSummaryPlot+".png");
  if(FIGURE){
    Canvas1->Print(saveSummaryPlot+".svg");
    Canvas1->Print(saveSummaryPlot+".C");
  }




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


