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

  Bool_t FINAL_FIGURE = 1;
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
  //  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
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

  // Fit function to show 
  showFit1 = "FIT = A_{M} cos(#phi) - C sin(#phi) + D";
  showFit2 = "FIT = A_{M} cos(#phi + #phi_{0}) + C";
  showFit3 = "FIT = A_{M} sin(#phi + #phi_{0}) + C";



  TString title11;
  TString titleInOut = Form("%s (%s, %s A): Regression-%s %s %s. No cuts applied."
			    ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),device.Data(),deviceTitle.Data());

  if(datopt==1) title11 = Form("%s %s",titleInOut.Data(),showFit1.Data());
  else if(datopt==2) title11 = Form("%s %s",titleInOut.Data(),showFit2.Data());
  else title11 = Form("%s %s",titleInOut.Data(),showFit3.Data());

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
//   gStyle->SetOptStat(0000000);
  gStyle->SetOptStat(1);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.20);
  gStyle->SetStatH(0.20);
  
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
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

//   gStyle->SetNdivisions(507,"x");

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



  /*********************************************************************/
  /*********************************************************************/
  const Int_t sNUM=1;
  
  Double_t x1[sNUM]       = {1.0};
  Double_t x2[sNUM]       = {2.0};
  Double_t x3[sNUM]       = {3.0};
  Double_t x4[sNUM]       = {4.0};

  
  Double_t ex[sNUM]       = {0.0};

  ex[sNUM]       = 0;

  Double_t hRegOut[sNUM],hRegIn[sNUM],ehRegOut[sNUM],ehRegIn[sNUM];
  Double_t hRegInpOut[sNUM],hRegInmOut[sNUM],ehRegInpOut[sNUM],ehRegInmOut[sNUM];

  Double_t summaryScale[2]={-10,10};
  Double_t summaryAxis = 1.6;


  hRegIn[sNUM]    = 6.092;
  ehRegIn[sNUM]   = 2.734;
  hRegOut[sNUM]   = 10.13;
  ehRegOut[sNUM]  = 3.012;



  hRegInpOut[sNUM]   = (hRegIn[sNUM] + hRegOut[sNUM])/2;
  ehRegInpOut[sNUM]  = sqrt(pow(ehRegIn[sNUM],2)+pow(ehRegOut[sNUM],2))/2;

  hRegInmOut[sNUM]   = ( (1/pow(ehRegIn[sNUM],2))*hRegIn[sNUM] - (1/pow(ehRegOut[sNUM],2))*hRegOut[sNUM]  )/( (1/pow(ehRegIn[sNUM],2)) + (1/pow(ehRegOut[sNUM],2)) );
  ehRegInmOut[sNUM]  = 1/sqrt(  (1/pow(ehRegIn[sNUM],2)) + (1/pow(ehRegOut[sNUM],2))  );

  cout<<blue<<"IN: "<<hRegIn[sNUM]<<"+-"<<ehRegIn[sNUM] <<normal<<endl;
  cout<<red<<"OUT: "<<hRegOut[sNUM]<<"+-"<<ehRegOut[sNUM] <<normal<<endl;
  cout<<green<<"IN+OUT: "<<hRegInpOut[sNUM]<<"+-"<<ehRegInpOut[sNUM] <<normal<<endl;
  cout<<magenta<<"IN-OUT: "<<hRegInmOut[sNUM]<<"+-"<<ehRegInmOut[sNUM] <<normal<<endl;


  if(FINAL_FIGURE){

//   gStyle->SetTitleYOffset(0.70);
//   gStyle->SetTitleXOffset(1.05);
//   gStyle->SetTitleX(0.12);
//   gStyle->SetTitleW(0.20);


  TString titleFinal = Form("%s (%s A): MD Regressed (%s) Asymmetry. Beamline Background. "
			    ,targ.Data(),qtor_stem.Data(),reg_set.Data());

  TCanvas * c11 = new TCanvas("c11", titleFinal,0,0,canvasSize[0],canvasSize[1]);
  c11->Draw();
  c11->SetBorderSize(0);
  c11->cd();
  TPad*pad111 = new TPad("pad111","pad111",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad112 = new TPad("pad112","pad112",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad111->SetFillColor(kWhite);
  pad111->Draw();
  pad112->Draw();
  pad111->cd();
  TLatex * t11 = new TLatex(0.00,0.3,Form("%s",titleFinal.Data()));
  t11->SetTextSize(0.45);
  t11->Draw();
  pad112->cd();
  pad112->Divide(1,2);


  pad112->cd(1);

  TGraphErrors *grRegOut = new TGraphErrors(sNUM,x1,hRegOut,ex,ehRegOut);
  grRegOut->SetName("grRegOut");
  grRegOut->Draw("goff");
  grRegOut->SetMarkerColor(kRed);
  grRegOut->SetLineColor(kRed);
  grRegOut->SetMarkerStyle(24);

  TGraphErrors *grRegIn = new TGraphErrors(sNUM,x2,hRegIn,ex,ehRegIn);
  grRegIn->SetName("grRegIn");
  grRegIn->Draw("goff");
  grRegIn->SetMarkerColor(kBlue);
  grRegIn->SetLineColor(kBlue);
  grRegIn->SetMarkerStyle(20);

  TGraphErrors *grRegInpOut = new TGraphErrors(sNUM,x3,hRegInpOut,ex,ehRegInpOut);
  grRegInpOut->SetName("grRegInpOut");
  grRegInpOut->Draw("goff");
  grRegInpOut->SetMarkerColor(kGreen-3);
  grRegInpOut->SetLineColor(kGreen-3);
  grRegInpOut->SetMarkerStyle(22);

  TGraphErrors *grRegInmOut = new TGraphErrors(sNUM,x4,hRegInmOut,ex,ehRegInmOut);
  grRegInmOut->SetName("grRegInmOut");
  grRegInmOut->Draw("goff");
  grRegInmOut->SetMarkerColor(kMagenta);
  grRegInmOut->SetLineColor(kMagenta);
  grRegInmOut->SetMarkerStyle(21);

  TMultiGraph *detGraph = new TMultiGraph();
  detGraph->Add(grRegOut,"P");
  detGraph->Add(grRegIn,"P");
  detGraph->Add(grRegInpOut,"P");
  detGraph->Add(grRegInmOut,"P");
  detGraph->Draw("A");
  detGraph->SetTitle("");
  detGraph->GetYaxis()->SetTitle(Form(" PMTLTG Asym [ppm]"));
  //  detGraph->GetXaxis()->CenterTitle();
  detGraph->GetYaxis()->CenterTitle();
  detGraph->GetXaxis()->SetNdivisions(5,0,0);
  detGraph->GetYaxis()->SetRangeUser(summaryScale[0],summaryScale[1]);
  TAxis *xaxisDetGraphReg1= detGraph->GetXaxis();
  xaxisDetGraphReg1->SetLimits(-0.2,4.8);
   detGraph->GetXaxis()->SetLabelColor(0);




  TLegend *legRegDet = new TLegend(0.70,0.70,0.90,0.95);
  legRegDet->SetNColumns(2);
  legRegDet->AddEntry("grRegIn","IHWP-IN","lp");
  legRegDet->AddEntry("grRegInpOut","#frac{IN+OUT}{2}","lp");
  legRegDet->AddEntry("grRegOut","IHWP-OUT","lp");
  legRegDet->AddEntry("grRegInmOut","A_{M}","lp");
  legRegDet->SetTextSize(0.028);
  legRegDet->SetFillColor(0);
  legRegDet->SetBorderSize(2);
  legRegDet->Draw("");


  TLatex* text1=new TLatex(0.85,summaryScale[0]-summaryAxis,Form("OUT"));
  text1->SetTextSize(0.06);
  TLatex* text2=new TLatex(1.90,summaryScale[0]-summaryAxis,Form("IN"));
  text2->SetTextSize(0.06);
  TLatex* text3=new TLatex(2.85,summaryScale[0]-summaryAxis,Form("#frac{IN+OUT}{2}"));
  text3->SetTextSize(0.06);
  TLatex* text4=new TLatex(3.92,summaryScale[0]-summaryAxis,Form("A_{M}"));
  text4->SetTextSize(0.06);

  text1->Draw("");
  text2->Draw("");
  text3->Draw("");
  text4->Draw("");

//   gStyle->SetLineStyle(1);
//   gStyle->SetLineWidth(3);
//   gStyle->SetLineColor(kBlack);
//   TLine * l = new TLine(0.2, 0, 4.7, 0); 
//   l->SetLineStyle(3);
//   l->Draw();
//   gStyle->SetLineStyle(1);
//   gStyle->SetLineWidth(1); 




//   pad112->cd(2);









  TString savePlotDiffOutVsRun = Form("dirPlot/beamlineBkg/%s_%s_%s_%s_regression_%s_%s_beamlineBkg_%s"
			       ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
			       ,reg_calc.Data(),reg_set.Data(),database_stem.Data());
  c11-> Update();
  c11->Print(savePlotDiffOutVsRun+".png");
  if(FIGURE){
    c11->Print(savePlotDiffOutVsRun+".svg");
    c11->Print(savePlotDiffOutVsRun+".C");
  }

  }






  /*********************************************************************/
  /*********************************************************************/
  /*********************************************************************/




  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


