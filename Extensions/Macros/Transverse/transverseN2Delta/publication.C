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

  Double_t figSize = 1.5;

  Bool_t SCALE = 1;
  Bool_t FIGURE = 0;
  Bool_t PRELIMINARY = 0;



  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";

  TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";
  //  TString deviceTitle = "Barsum"; TString deviceName = "Barsum";

  std::ofstream Myfile3;

  TString target, polar,targ, goodfor, reg_set, reg_calc;

  Int_t opt =1;
  Int_t datopt = 3;
  Int_t ropt = 2;
  Int_t qtor_opt = 2;

  TString good_for;
  TString qtor_current;
  TString qtor_stem;
  TString good;
  TString interaction;
  TString showFit1,showFit2,showFit3,showFit4;


  /* Canvas and Pad and Scale parameters */
//   Int_t canvasSize[2] ={1200*figSize,800*figSize};
  Int_t canvasSize[2] ={1200*figSize,650*figSize};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t textSize[4] = {0.045*figSize,0.035*figSize,0.060*figSize,0.030*figSize};
  Double_t markerSize[6] = {1.5*figSize,1.3*figSize,0.7*figSize,0.5*figSize,1.2*figSize,0.8*figSize};
  Double_t legendCoordinates[4] = {0.1,0.900,0.75,0.995};
  Double_t yScale[2] = {-8.0,8.0};
  Double_t waterMark[2] = {2.5,-1.0};

  Double_t x_lo_stat_in4,y_lo_stat_in4,x_hi_stat_in4,y_hi_stat_in4,x_lo_stat_out4,y_lo_stat_out4,x_hi_stat_out4,y_hi_stat_out4;
  x_lo_stat_in4=0.76;y_lo_stat_in4=0.64;x_hi_stat_in4=0.99;y_hi_stat_in4=0.95;
  x_lo_stat_out4=0.76;y_lo_stat_out4=0.30;x_hi_stat_out4=0.99;y_hi_stat_out4=0.61;
  Double_t x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4;
  x_lo_leg4=0.76;y_lo_leg4=0.10;x_hi_leg4=0.99;y_hi_leg4=0.31;
  Double_t textTargetX1,textTargetX2,textTargetY1,textTargetY2,totalTextScale;


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

//   // Fixing reaction type to N-to-Delta
//   //  ropt = 2;
//   std::cout<<Form("Enter reaction type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
//   std::cout<<Form("1. elastic")<<std::endl;
//   std::cout<<Form("2. %sN->Delta (deafult)%s",blue,normal)<<std::endl;
//   std::cout<<Form("3. DIS ")<<std::endl;
//   //   std::cin>>ropt;
//   std::string input_ropt;
//   std::getline( std::cin, input_ropt );
//   if ( !input_ropt.empty() ) {
//     std::istringstream stream( input_ropt );
//     stream >> ropt;
//   }


//   if(ropt==2){
//     std::cout<<Form("Enter QTOR current (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
//     std::cout<<Form("1.6000 A ")<<std::endl;
//     std::cout<<Form("2.%s6700 A (deafult)%s",blue,normal)<<std::endl;
//     std::cout<<Form("3.7300 A ")<<std::endl;
//     //   std::cin>>qtor_opt;
//     std::string input_qtor_opt;
//     std::getline( std::cin, input_qtor_opt );
//     if ( !input_qtor_opt.empty() ) {
//       std::istringstream stream( input_qtor_opt );
//       stream >> qtor_opt;
//     }
//   }

  Int_t regSwitch = 1;
//   std::cout<<Form("Enter regression On or OFF (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
//   std::cout<<Form("0. OFF")<<std::endl;
//   std::cout<<Form("1. %sON (deafult)%s",blue,normal)<<std::endl;
//   //   std::cin>>regSwitch;
//   std::string input_regSwitch;
//   std::getline( std::cin, input_regSwitch );
//   if ( !input_regSwitch.empty() ) {
//     std::istringstream stream( input_regSwitch );
//     stream >> regSwitch;
//   }

  // Fixing regression type to on_5+1
  Int_t regID = 1;
//   std::cout<<Form("Enter regression type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
//   std::cout<<Form(" 0. off"     )<<std::endl;
//   std::cout<<Form(" 1. on "     )<<std::endl;
// //   std::cout<<Form(" 2. %son (deafult)%s",blue,normal)<<std::endl;
//   std::cout<<Form(" 2. on_5+1"  )<<std::endl;
//   std::cout<<Form(" 3. set3 "   )<<std::endl;
//   std::cout<<Form(" 4. set4 "   )<<std::endl;
//   std::cout<<Form(" 5. set5 "   )<<std::endl;
//   std::cout<<Form(" 6. set6 "   )<<std::endl;
//   std::cout<<Form(" 7. set7 "   )<<std::endl;
//   std::cout<<Form(" 8. set8 "   )<<std::endl;
//   std::cout<<Form(" 9. set9 "   )<<std::endl;
//   std::cout<<Form("10. set10 "  )<<std::endl;
//   std::cout<<Form("11. set11 "  )<<std::endl;
//   std::cout<<Form("12. set12 "  )<<std::endl;
//   std::cout<<Form("13. set13 "  )<<std::endl;
//   //   std::cin>>regID;
  std::string input_regID;
//   std::getline( std::cin, input_regID );
  if ( !input_regID.empty() ) {
    std::istringstream stream( input_regID );
    stream >> regID;
  }


  if(argc>1) database = argv[1];

  // select the target
  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
    textTargetX1= 3.45; textTargetY1=0.65; textTargetX2=1.61; textTargetY2=-0.16; totalTextScale=0.65;
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 % Al";
    textTargetX1= 4.1; textTargetY1=0.40; textTargetX2=2.8; textTargetY2=-0.99; totalTextScale=0.95;
  }
  else if(opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 % Carbon";
    textTargetX1= 3.1; textTargetY1=0.50; textTargetX2=1.7; textTargetY2=-0.39; totalTextScale=0.65;
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
//   gStyle->SetPadRightMargin(0.05);
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
  showFit2 = "FIT_V = A_{M} cos(#phi + #phi_{0}) + C";
  showFit3 = "FIT_H = A_{M} sin(#phi + #phi_{0}) + C";
//   showFit4 = "FIT_{H/V} = #epsilon_{M}^{^{H/V}} sin/cos(#phi) + C^{H/V}";
  showFit4 = "FIT_{H/V} = #epsilon_{M}^{^{H/V}} sin/cos(#phi)";


  TString title1;
  TString titleSummary = Form("%s (transverse, %s A): Regression-%s MD %s Asymmetries."
			      ,targ.Data(),qtor_stem.Data(),reg_set.Data(),deviceTitle.Data());

  if(datopt==1){ title1= Form("%s %s",titleSummary.Data(),showFit1.Data());}
  else if(datopt==2){ 
    title1= Form("%s %s, %s",titleSummary.Data(),showFit3.Data(),showFit2.Data());
  }
  else{
//     title1= Form("%s %s, %s",titleSummary.Data(),showFit3.Data(),showFit2.Data());
    title1= Form("%s %s",titleSummary.Data(),showFit4.Data());
  }

  TCanvas * canvas1 = new TCanvas("canvas1", title1,0,0,canvasSize[0],canvasSize[1]);
  canvas1->Draw();
  canvas1->SetBorderSize(0);
  canvas1->cd();
  TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  TLatex * t1 = new TLatex(0.01,0.3,Form("%s",title1.Data()));
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();


  Char_t  textfile[400],textfileV[400],textfileH[400],textfile3[400];

  sprintf(textfileV,"dirPlot/%s_%s_v_transverse_%s_MD_%s_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()); 
  sprintf(textfileH,"dirPlot/%s_%s_h_transverse_%s_MD_%s_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()); 


  const Int_t NUM = 100;

  int counter=0;
  ifstream transverseMDAsym,transverseMDAsymV,transverseMDAsymH;
  Double_t octant[NUM];
  Double_t zero[NUM],valuesin[NUM],errorsin[NUM],valuesout[NUM],errorsout[NUM];
  Double_t octantH[NUM],valuesinH[NUM],errorsinH[NUM],valuesoutH[NUM],errorsoutH[NUM];
  Double_t octantV[NUM],valuesinV[NUM],errorsinV[NUM],valuesoutV[NUM],errorsoutV[NUM];


  transverseMDAsymH.open(Form("%s",textfileH));
  transverseMDAsymV.open(Form("%s",textfileV));

  //  printf(Form("%s\n%s\n%s\n",textfile,textfileH,textfileV));
 
  if (transverseMDAsymH.is_open() && transverseMDAsymV.is_open()) {
   
    while(!transverseMDAsymH.eof() && !transverseMDAsymV.eof()) {
      zero[counter]=0;

      transverseMDAsymH >> octantH[counter];
      transverseMDAsymH >> valuesinH[counter];
      transverseMDAsymH >> errorsinH[counter];
      transverseMDAsymH >> valuesoutH[counter];
      transverseMDAsymH >> errorsoutH[counter];
      if (!transverseMDAsymH.good()) break;

      transverseMDAsymV >> octantV[counter];
      transverseMDAsymV >> valuesinV[counter];
      transverseMDAsymV >> errorsinV[counter];
      transverseMDAsymV >> valuesoutV[counter];
      transverseMDAsymV >> errorsoutV[counter];
      if (!transverseMDAsymV.good()) break;


//       printf("%s %4.0f\t%4.2f+-%4.2f\t%4.2f+-%4.2f\n%s"
// 	     ,blue,octantH[counter],valuesinH[counter],errorsinH[counter],valuesoutH[counter],errorsoutH[counter],normal);
//       printf("%s %4.0f\t%4.2f+-%4.2f\t%4.2f+-%4.2f\n%s"
// 	     ,red,octantV[counter],valuesinV[counter],errorsinV[counter],valuesoutV[counter],errorsoutV[counter],normal);
 
      counter++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  transverseMDAsymH.close();
  transverseMDAsymV.close();


  const int k =8;

  Double_t valuesum[k],valueerror[k],valuediff[k],errordiff[k],x[k],errx[k];


  Double_t valuesumH[k],valueerrorH[k],valuediffH[k],errordiffH[k],xH[k],errxH[k];
  Double_t valuesumV[k],valueerrorV[k],valuediffV[k],errordiffV[k],xV[k],errxV[k];
  
  
  for(Int_t i =0;i<k;i++){
    x[i] = i+1; errx[i] = 0.0;
    xH[i] = i+1; errxH[i] = 0.0;
    xV[i] = i+1; errxV[i] = 0.0;
  }


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
  


  // cos fit in
  TF1 *fit_inH;
//   TString fit_func_inH = "[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]";
//   TString fit_func_inH = "[0]*sin((pi/180)*(45*(x-1) )) + [1]";
  TString fit_func_inH = "[0]*sin((pi/180)*(45*(x-1) ))";
  fit_inH = new TF1("fit_inH",Form("%s",fit_func_inH.Data()),1,8);
  fit_inH->SetParameter(0,0);

  TF1 *fit_inV;
//   TString fit_func_inV = "[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]";
//   TString fit_func_inV = "[0]*cos((pi/180)*(45*(x-1) )) + [1]";
  TString fit_func_inV = "[0]*cos((pi/180)*(45*(x-1) ))";
  fit_inV = new TF1("fit_inV",Form("%s",fit_func_inV.Data()),1,8);
  fit_inV->SetParameter(0,0);


  for(Int_t i =0;i<k;i++){
    valuesum[i]=(valuesin[i]+valuesout[i])/2.0;
    valueerror[i]= (sqrt(pow(errorsin[i],2)+pow(errorsout[i],2)))/2.0;
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));

    valuesumH[i]=(valuesinH[i]+valuesoutH[i])/2.0;
    valueerrorH[i]= (sqrt(pow(errorsinH[i],2)+pow(errorsoutH[i],2)))/2.0;
    valuediffH[i]=((valuesinH[i]/pow(errorsinH[i],2)) - (valuesoutH[i]/pow(errorsoutH[i],2))) /((1/pow(errorsinH[i],2)) + (1/pow(errorsoutH[i],2)));
    errordiffH[i]= sqrt(1/((1/(pow(errorsinH[i],2)))+(1/pow(errorsoutH[i],2))));

    valuesumV[i]=(valuesinV[i]+valuesoutV[i])/2.0;
    valueerrorV[i]= (sqrt(pow(errorsinV[i],2)+pow(errorsoutV[i],2)))/2.0;
    valuediffV[i]=((valuesinV[i]/pow(errorsinV[i],2)) - (valuesoutV[i]/pow(errorsoutV[i],2))) /((1/pow(errorsinV[i],2)) + (1/pow(errorsoutV[i],2)));
    errordiffV[i]= sqrt(1/((1/(pow(errorsinV[i],2)))+(1/pow(errorsoutV[i],2))));


  }

  /********************************************/


  // Draw In-Out
  TGraphErrors* grp_diffH  = new TGraphErrors(k,x,valuediffH,errx,errordiffH);
  grp_diffH ->SetName("grp_diffH");
  grp_diffH ->Draw("goff");
  grp_diffH ->SetMarkerSize(markerSize[1]);
  grp_diffH ->SetLineWidth(0);
  grp_diffH ->SetMarkerStyle(21);
  grp_diffH ->SetMarkerColor(kRed);
  grp_diffH ->SetLineColor(kRed);
  grp_diffH->Fit("fit_inH","B0Q");
  TF1* fitH = grp_diffH->GetFunction("fit_inH");
  //  fitH->DrawCopy("same");
  fitH->SetLineColor(kRed+1);
  fitH->SetLineStyle(2);
  fitH->SetLineWidth(4);
  Double_t p1H   =  fitH->GetParameter(0);
  Double_t ep1H  =  fitH->GetParError(0);

  // Draw In-Out
  TGraphErrors* grp_diffV  = new TGraphErrors(k,x,valuediffV,errx,errordiffV);
  grp_diffV ->SetName("grp_diffV");
  grp_diffV ->Draw("goff");
  grp_diffV ->SetMarkerSize(markerSize[0]);
  grp_diffV ->SetLineWidth(0);
  grp_diffV ->SetMarkerStyle(23);
  grp_diffV ->SetMarkerColor(kBlue);
  grp_diffV ->SetLineColor(kBlue);
  grp_diffV->Fit("fit_inV","B0Q");
  TF1* fitV = grp_diffV->GetFunction("fit_inV");
  //fitV->DrawCopy("same");
  fitV->SetLineColor(kBlue+1);
  fitV->SetLineStyle(5);
  fitV->SetLineWidth(4);
  Double_t p1V   =  fitV->GetParameter(0);
  Double_t ep1V  =  fitV->GetParError(0);


  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_diffH,"P");
  grp->Add(grp_diffV,"P");
  grp->Draw("A");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
//   grp->GetYaxis()->SetTitle(Form("MD %s Asymmetry [ppm]",deviceTitle.Data()));
  grp->GetYaxis()->SetTitle(Form("Asymmetry [ppm]"));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.9);
  grp->GetXaxis()->SetNdivisions(8,0,0);
  grp->GetYaxis()->SetNdivisions(8,5,0);
  if(SCALE){
    grp->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  }
  TAxis *xaxisGrp= grp->GetXaxis();
  xaxisGrp->SetLimits(0.5,8.5);

  fitH->DrawCopy("same");
  fitV->DrawCopy("same");

//   fitH->SetParNames("A_{M}^{H}","#phi_{0}^{ H}","C^{H}");
//   fitV->SetParNames("A_{M}^{V}","#phi_{0}^{ V}","C^{V}");
  fitH->SetParNames("#epsilon_{M}^{^{H}}","C^{^{H}}");
  fitV->SetParNames("#epsilon_{M}^{^{V}}","C^{^{V}}");

  TPaveStats *statsH = (TPaveStats*)grp_diffH->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsV = (TPaveStats*)grp_diffV->GetListOfFunctions()->FindObject("stats");
  statsH->SetTextColor(kRed+1);  statsH->SetFillColor(0); //statsH->SetFillColor(kWhite); 
  statsV->SetTextColor(kBlue+1); statsV->SetFillColor(0); //statsV->SetFillColor(kWhite); 
  statsH->SetX1NDC(x_lo_stat_in4); statsH->SetX2NDC(x_hi_stat_in4); statsH->SetY1NDC(0.67);statsH->SetY2NDC(0.95);
  statsV->SetX1NDC(x_lo_stat_in4); statsV->SetX2NDC(x_hi_stat_in4); statsV->SetY1NDC(0.37);statsV->SetY2NDC(0.65);
//   statsH->SetX1NDC(0.60); statsH->SetX2NDC(0.88); statsH->SetY1NDC(0.64);statsH->SetY2NDC(0.94);
//   statsV->SetX1NDC(0.12); statsV->SetX2NDC(0.40); statsV->SetY1NDC(0.16);statsV->SetY2NDC(0.46);
  statsH->SetTextSize(textSize[1]);  
  statsV->SetTextSize(textSize[1]);


  TLegend *legend = new TLegend(legendCoordinates[0],legendCoordinates[1],legendCoordinates[2],legendCoordinates[3],"","brNDC");
  legend->SetNColumns(2);
  legend->AddEntry(grp_diffH,Form("A_{M}^{H} = %4.2f #pm %4.2f ppm",p1H,ep1H), "lp");
  legend->AddEntry(grp_diffV,Form("A_{M}^{V} = %4.2f #pm %4.2f ppm",p1V,ep1V), "lp");
  legend->SetFillColor(0);
  legend->SetBorderSize(2);
  legend->SetTextSize(textSize[0]);

  TLatex* tText_h = new TLatex(textTargetX1,yScale[1]*textTargetY1,Form("Horizontal"));
  tText_h->SetTextSize(textSize[0]);
  tText_h->SetTextColor(kRed);
  TLatex* tText_v = new TLatex(textTargetX2,yScale[1]*textTargetY2,Form("Vertical"));
  tText_v->SetTextSize(textSize[0]);
  tText_v->SetTextColor(kBlue);

  tText_h->Draw(); tText_v->Draw();

  cout<<"H "<<p1H<<"+-"<<ep1H<<", V "<<p1V<<"+-"<<ep1V<<endl;
  Double_t A_msr,A_msr_h,A_msr_v,dAmsr_stat,dAmsr_stat_h,dAmsr_stat_v;
  A_msr      = ((1/pow(ep1H,2))*(p1H) + (1/pow(ep1V,2))*(p1V))/((1/pow(ep1H,2)) + (1/pow(ep1V,2)));
  dAmsr_stat = 1/TMath::Sqrt( (1/pow(ep1H,2)) + (1/pow(ep1V,2)) );


  TLatex* tText_total = new TLatex(8.75,yScale[0]*totalTextScale,Form("#epsilon_{reg} = %2.3f #pm %2.3f",A_msr,dAmsr_stat));
  tText_total->SetTextSize(textSize[1]);
  tText_total->SetTextColor(kBlack);
//   tText_total->Draw();

  cout<<blue<<"A_msr^H = "<<p1H<<" +- "<<ep1H<<normal<<endl;
  cout<<blue<<"A_msr^V = "<<p1V<<" +- "<<ep1V<<normal<<endl;
  cout<<red<<"A_msr   = "<<A_msr<<" +- "<<dAmsr_stat<<normal<<endl;


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



  TString saveSummaryPlot = Form("dirPlot/finalPlot/%s_%s_transverseAsymmetries_%s_MD_%s_regression_%s_%s_%s_2"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());

  canvas1->Update();
  canvas1->Print(saveSummaryPlot+".png");
  if(FIGURE){
    canvas1->Print(saveSummaryPlot+".eps");
  }







  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


