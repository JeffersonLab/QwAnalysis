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
//   printf ( "%sThe current date/time is: %s%s%s",blue,red,asctime(timeinfo),normal);

  // }
  // void plotTransverseN2Delta(){

  Bool_t FIGURE = 0;


//   TString database="qw_run2_pass1";
//   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";
//   TString deviceTitle = "Barsum"; TString deviceName = "Barsum";
  TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";

  std::ofstream Myfile4;

  TText *t1;
  TString target, polar,targ, goodfor, reg_set, reg_calc;

  Int_t opt =1;
  Int_t datopt = 2;
  Int_t ropt = 2;
  Int_t qtor_opt = 2;

  TString good_for;
  TString qtor_current;
  TString qtor_stem;
  TString good;
  TString interaction;


  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[2] ={1400,1200};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.945};


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


  TString title1;
  TString titleSummary = Form("Physics Asym using Regression-%s (MD %s) for %s (%s, %s A)."
			      ,reg_set.Data(),deviceTitle.Data(),targ.Data(),polar.Data(),qtor_stem.Data());

  title1= Form("%s",titleSummary.Data());


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
  TString text = Form("%s",title1.Data());
  t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();

  gStyle->SetTextSize(0.02);

  Char_t  textfile3[400],textfile4[400];
  sprintf(textfile3,"dirPlot/resultText/%s_%s_%s_%s_MD_%s_regression_%s_%s_asym_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());
  sprintf(textfile4,"dirPlot/resultText/%s_%s_%s_%s_MD_%s_regression_%s_%s_physAsym_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());


//   printf("%sText File = %s\n%s",green,textfile3,normal);

  const Int_t NUM = 100;

  int counter=0;
  ifstream transverseMDAsym;
  Double_t p0diff[NUM],ep0diff[NUM],p1diff[NUM],ep1diff[NUM],p2diff[NUM],ep2diff[NUM],Chidiff[NUM],NDFdiff[NUM],Probdiff[NUM],zero[NUM];

  transverseMDAsym.open(Form("%s",textfile3));
 
  if (transverseMDAsym.is_open()) {
   
    printf("%s Det\tIn+-InError\tOut+-OutError\n%s",green,normal);
   
    while(!transverseMDAsym.eof()) {
      zero[counter]=0;
      transverseMDAsym >> p0diff[counter];
      transverseMDAsym >> ep0diff[counter];
      transverseMDAsym >> p1diff[counter];
      transverseMDAsym >> ep1diff[counter];
      transverseMDAsym >> p2diff[counter];
      transverseMDAsym >> ep2diff[counter];
      transverseMDAsym >> Chidiff[counter];
      transverseMDAsym >> NDFdiff[counter];
      transverseMDAsym >> Probdiff[counter];
   
      if (!transverseMDAsym.good()) break;

      printf("%s %4.2f+-%4.2f\t%4.2f+-%4.2f\t%4.2f+-%4.2f\t%4.2f %4.0f %4.2f\n%s"
	     ,blue,p0diff[counter],ep0diff[counter],p1diff[counter],ep1diff[counter],p2diff[counter],ep2diff[counter],Chidiff[counter],NDFdiff[counter],Probdiff[counter],normal);
 
      counter++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  transverseMDAsym.close();

  Double_t cor[2] = {0.01,0.85};
  Double_t corGaph[2] = {0.08,0.3};


  // Polarization 
  Double_t polarizationRun1,errorPolarizationRun1,polarizationRun2,errorPolarizationRun2,polarizationTotal,errorPolarizationTotal;
  polarizationRun1          =  0.8777;
  errorPolarizationRun1     =  0.0133;
  polarizationRun2          =  0.8796;
  errorPolarizationRun2     =  0.0050;
  
  polarizationTotal=( (polarizationRun1/pow(errorPolarizationRun1,2))+(polarizationRun2/pow(errorPolarizationRun2,2)) ) /((1/pow(errorPolarizationRun1,2)) + (1/pow(errorPolarizationRun2,2)));
  errorPolarizationTotal= sqrt(1/((1/(pow(errorPolarizationRun1,2)))+(1/pow(errorPolarizationRun2,2))));

  polarizationTotal         =  0.879;
  errorPolarizationTotal    =  0.018;

  // Dilution Factors
  Double_t dilution,errorDilution;

  if(opt == 1 && qtor_opt == 1){ dilution=0.249; errorDilution=0.004; }
  if(opt == 1 && qtor_opt == 2){ dilution=0.276; errorDilution=0.004; }
  if(opt == 1 && qtor_opt == 3){ dilution=0.170; errorDilution=0.002; }

  if(opt == 2 && qtor_opt == 1){ dilution=0.000; errorDilution=0.000; }
  if(opt == 2 && qtor_opt == 2){ dilution=0.000; errorDilution=0.000; }
  if(opt == 2 && qtor_opt == 3){ dilution=0.000; errorDilution=0.000; }

  if(opt == 2 && qtor_opt == 1){ dilution=0.000; errorDilution=0.000; }
  if(opt == 2 && qtor_opt == 2){ dilution=0.000; errorDilution=0.000; }
  if(opt == 2 && qtor_opt == 3){ dilution=0.000; errorDilution=0.000; }


  // Elastic Asymmetry
  Double_t elasticAsym,errorElasticAsym;

  if(datopt==1 && opt == 1){ elasticAsym=-5.305; errorElasticAsym=0.166; }
  if(datopt==1 && opt == 2){ elasticAsym=-8.66; errorElasticAsym=0.51; }
  if(datopt==1 && opt == 3){ elasticAsym=-8.41; errorElasticAsym=0.61; }

  if(datopt==2 && opt == 1){ elasticAsym=-5.305; errorElasticAsym=0.166; }
  if(datopt==2 && opt == 2){ elasticAsym=-9.20; errorElasticAsym=0.49; }
  if(datopt==2 && opt == 3){ elasticAsym=-8.41; errorElasticAsym=0.61; }

  if(datopt==3 && opt == 1){ elasticAsym=-5.305; errorElasticAsym=0.166; }
  if(datopt==3 && opt == 2){ elasticAsym=-8.66; errorElasticAsym=0.51; }
  if(datopt==3 && opt == 3){ elasticAsym=-8.41; errorElasticAsym=0.61; }



  // Physics Asymmetry
  Double_t physAsym,errorPhysAsym,error1PhysAsym,error2PhysAsym,error3PhysAsym,error4PhysAsym,errorSysPhysAsym,errorStatPhysAsym;

  physAsym = (p0diff[0]/polarizationTotal - elasticAsym*(1 - dilution))/(dilution);

  error1PhysAsym = (ep0diff[0]/polarizationTotal)*(1/(dilution));
  error2PhysAsym = (p0diff[0]/polarizationTotal)*(errorPolarizationTotal/polarizationTotal)*(1/(dilution));
  error3PhysAsym = (1 - dilution)*errorElasticAsym*(1/(dilution));
  error4PhysAsym = errorDilution*((p0diff[0]/polarizationTotal) - elasticAsym);

  errorStatPhysAsym = error1PhysAsym;
  errorSysPhysAsym = TMath::Sqrt(pow(error2PhysAsym,2) + pow(error3PhysAsym,2) + pow(error4PhysAsym,2));

  errorPhysAsym = TMath::Sqrt(pow(errorStatPhysAsym,2) + pow(errorSysPhysAsym,2));


  printf("Physics Asym = %4.2f\n",physAsym);

  // Writing in a canvas

  TLatex * tmeasuredAsym = new TLatex(cor[0],cor[1],Form("A_{M}^{in} = %4.2f #pm %4.2f",p0diff[0],ep0diff[0]));
  tmeasuredAsym->Draw();
  TLatex * tPolarization = new TLatex(cor[0],cor[1]-corGaph[0],Form("Polarization (P) = %4.2f #pm %4.2f",polarizationTotal,errorPolarizationTotal));
  tPolarization->Draw();
  TLatex * tDilution = new TLatex(cor[0],cor[1]-2*corGaph[0],Form("Dilution (f^{in}) = %4.2f #pm %4.2f",dilution,errorDilution));
  tDilution->Draw();
  TLatex * tElasticAsym = new TLatex(cor[0],cor[1]-3*corGaph[0],Form("A_{M}^{el} = %4.2f #pm %4.2f",elasticAsym,errorElasticAsym));
  tElasticAsym->Draw();

  TLatex * tFormula = new TLatex(cor[0]+corGaph[1],cor[1]-4.0*corGaph[0],Form("A_{PHYS}^{in} = #frac{ #frac{A_{M}^{in}}{P} - A_{M}^{el} f^{el} }{1 - f^{el}} = %4.2f ^{%4.2f}_{%4.2f}",physAsym,errorStatPhysAsym,errorSysPhysAsym));
  tFormula->Draw();
  tFormula->SetTextColor(kRed);
//   TLatex * tFormulaError = new TLatex(cor[0]+corGaph[1]+0.35,cor[1]-0.0*corGaph[0],Form("#left(dA_{PHYS}^{in}#right)_{Total} = #sqrt[]{ #left(dA_{PHYS}^{in}#right)_{A_{M}^{in}} + #left(dA_{PHYS}^{in}#right)_{P} + #left(dA_{PHYS}^{in}#right)_{A_{M}^{el}} + #left(dA_{PHYS}^{in}#right)_{f^{el}}} = %4.2f",errorPhysAsym));
//   tFormulaError->Draw();

  TLatex * tFormulaError1 = new TLatex(cor[0]+corGaph[1]+0.35,cor[1]-0.0*corGaph[0],Form("#left(dA_{PHYS}^{in}#right)_{A_{M}^{in}} = #frac{dA_{M}^{in}}{P} #frac{1}{1 - f^{el}} = %4.2f",error1PhysAsym));
  tFormulaError1->Draw();
  TLatex * tFormulaError2 = new TLatex(cor[0]+corGaph[1]+0.35,cor[1]-2.0*corGaph[0],Form("#left(dA_{PHYS}^{in}#right)_{P} = #frac{A_{M}^{in}}{P} #frac{dP}{P} #frac{1}{1 - f^{el}} = %4.2f",error2PhysAsym));
  tFormulaError2->Draw();
  TLatex * tFormulaError3 = new TLatex(cor[0]+corGaph[1]+0.35,cor[1]-4.0*corGaph[0],Form("#left(dA_{PHYS}^{in}#right)_{A_{M}^{el}} = f^{el} dA_{M}^{el} #frac{1}{1 - f^{el}} = %4.2f",error3PhysAsym));
  tFormulaError3->Draw();
  TLatex * tFormulaError4 = new TLatex(cor[0]+corGaph[1]+0.35,cor[1]-6.0*corGaph[0],Form("#left(dA_{PHYS}^{in}#right)_{f^{el}} = df^{el} #left[#frac{A_{in}^{M}}{P} - A_{M}^{el}#right] = %4.2f",error4PhysAsym));
  tFormulaError4->Draw();


  gPad->Update();

  Myfile4.open(textfile4);
  Myfile4<<Form("%4.4f %4.4f \t%4.4f %4.4f %4.4f %4.4f",physAsym,errorPhysAsym,error1PhysAsym,error2PhysAsym,error3PhysAsym,error4PhysAsym)<<endl;
  Myfile4.close();


  TString saveSummaryPlot = Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_%s_physics_asym_table_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());

  Canvas1->Update();
  Canvas1->Print(saveSummaryPlot+".png");
  if(FIGURE){
    Canvas1->Print(saveSummaryPlot+".svg");
    Canvas1->Print(saveSummaryPlot+".C");
  }



  /********************************************/
  /********************************************/
  /********************************************/
  /********************************************/

  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


