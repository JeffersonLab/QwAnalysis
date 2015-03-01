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
  Bool_t PRELIMINARY = 1;


//   TString database="qw_run2_pass1";
//   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";

//   TString deviceTitle = "Pos. PMT"; TString deviceName = "PosPMT";
//   TString deviceTitle = "Neg. PMT";  TString deviceName = "NegPMT";
  TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";
//   TString deviceTitle = "Barsum"; TString deviceName = "Barsum";

  std::ofstream Myfile3;

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
  Int_t canvasSize[2] ={1200,600};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.945};
  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.83,0.65,0.95};
  Double_t yScale[2] = {-10.0,10.0};
  Double_t waterMark[2] = {2.5,-1.0};




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
  std::cout<<Form("0. off"     )<<std::endl;
  std::cout<<Form("1. on "     )<<std::endl;
  std::cout<<Form("2. %son_5+1 (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("3. set3 "   )<<std::endl;
  std::cout<<Form("4. set4 "   )<<std::endl;
  std::cout<<Form("5. set5 "   )<<std::endl;
  std::cout<<Form("6. set6 "   )<<std::endl;
  std::cout<<Form("7. set7 "   )<<std::endl;
  std::cout<<Form("8. set8 "   )<<std::endl;
  std::cout<<Form("9. set9 "   )<<std::endl;
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
      case    0: reg_calc="off";    reg_set="off";   break;
      case    1: reg_calc="on";     reg_set="off";   break;
      case    2: reg_calc="on_5+1"; reg_set="off";   break;
      case    3: reg_calc="set3";   reg_set="off";   break;
      case    4: reg_calc="set4";   reg_set="off";   break;
      case    5: reg_calc="set5";   reg_set="off";   break;
      case    6: reg_calc="set6";   reg_set="off";   break;
      case    7: reg_calc="set7";   reg_set="off";   break;
      case    8: reg_calc="set8";   reg_set="off";   break;
      case    9: reg_calc="set9";   reg_set="off";   break;
      default  : printf("Please insert correct regression information. Exiting program !!!\n"); exit(1); break;
      }
  }

  else{
    switch(regID)
      {
      case    0: reg_calc="off";  reg_set="off";     break;
      case    1: reg_calc="off";  reg_set="on";      break;
      case    2: reg_calc="off";  reg_set="on_5+1";  break;
      case    3: reg_calc="off";  reg_set="set3";    break;
      case    4: reg_calc="off";  reg_set="set4";    break;
      case    5: reg_calc="off";  reg_set="set5";    break;
      case    6: reg_calc="off";  reg_set="set6";    break;
      case    7: reg_calc="off";  reg_set="set7";    break;
      case    8: reg_calc="off";  reg_set="set8";    break;
      case    9: reg_calc="off";  reg_set="set9";    break;
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
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  TCanvas *c6 = new TCanvas("c6","c6",cx1,cy1,1200,950);
  TPad * pad60 = new TPad("pad60","pad60",ps1,ps2,ps4,ps4);
  TPad * pad61 = new TPad("pad61","pad61",ps1,ps1,ps4,ps3);
  pad60->Draw();
  pad61->Draw();
  pad60->cd();
  TText * ct60 = new TText(tll,tlr,Form("%sN to Delta %s Un-Regressed Position and Energy Differences",regTitle,dataInfo));
  ct60->SetTextSize(tsiz);
  ct60->Draw();
  pad61->cd();
  pad61->Divide(2,3);

  
  pad61->cd(1);

  TGraphErrors * runDiffXGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffXCalcUnRegInVal,zeroUnRegIn,ediffXCalcUnRegInVal);
//   TGraphErrors * runDiffXGraphUnRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testDiffXUnRegIn.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
  runDiffXGraphUnRegIn1->SetName("runDiffXGraphUnRegIn1");
  runDiffXGraphUnRegIn1->SetMarkerColor(kRed);
  runDiffXGraphUnRegIn1->SetLineColor(kRed);
  runDiffXGraphUnRegIn1->SetMarkerStyle(25);
  runDiffXGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffXGraphUnRegIn1->SetLineWidth(1);
//   runDiffXGraphUnRegIn1->Draw("AP");
  runDiffXGraphUnRegIn1->Fit("fitRunDiffXGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffXGraphUnRegIn1->SetLineColor(kRed);
  fitRunDiffXGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXUnRegIn1            =  fitRunDiffXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXUnRegIn1Error       =  fitRunDiffXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffXUnRegIn1Chisquare   =  fitRunDiffXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffXUnRegIn1NDF         =  fitRunDiffXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffXUnRegIn1Prob        =  fitRunDiffXGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffXGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffXCalcUnRegOutVal,zeroUnRegOut,ediffXCalcUnRegOutVal);
//   TGraphErrors * runDiffXGraphUnRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testDiffXUnRegOut.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
  runDiffXGraphUnRegOut1->SetName("runDiffXGraphUnRegOut1");
  runDiffXGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffXGraphUnRegOut1->SetLineColor(kBlack);
  runDiffXGraphUnRegOut1->SetMarkerStyle(25);
  runDiffXGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffXGraphUnRegOut1->SetLineWidth(1);
//   runDiffXGraphUnRegOut1->Draw("AP");
  runDiffXGraphUnRegOut1->Fit("fitRunDiffXGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffXGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXUnRegOut1           =  fitRunDiffXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXUnRegOut1Error      =  fitRunDiffXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffXUnRegOut1Chisquare  =  fitRunDiffXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffXUnRegOut1NDF        =  fitRunDiffXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffXUnRegOut1Prob       =  fitRunDiffXGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffXGraphUnReg1 = new TMultiGraph();
  runDiffXGraphUnReg1->Add(runDiffXGraphUnRegIn1);
  runDiffXGraphUnReg1->Add(runDiffXGraphUnRegOut1);
  runDiffXGraphUnReg1->Draw("AP");
  runDiffXGraphUnReg1->SetTitle("");
  runDiffXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXGraphUnReg1->GetYaxis()->SetTitle("Target X Diff [mm]");
  runDiffXGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffXGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXGraphUnReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffXGraphUnReg1= runDiffXGraphUnReg1->GetXaxis();
  xaxisRunDiffXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXGraphUnRegIn1->Draw("same");
  fitRunDiffXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffXUnRegIn1 =(TPaveStats*)runDiffXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXUnRegOut1=(TPaveStats*)runDiffXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXUnRegIn1->SetTextColor(kRed);
  statsRunDiffXUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffXUnRegOut1->SetTextColor(kBlack);
  statsRunDiffXUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXUnRegInpOut1       =  (fitResultRunDiffXUnRegIn1+fitResultRunDiffXUnRegOut1)/2;
  Double_t resultRunDiffXUnRegInmOut1       =  (fitResultRunDiffXUnRegIn1-fitResultRunDiffXUnRegOut1)/2;
  Double_t resultRunDiffXUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXUnRegIn1Error*fitResultRunDiffXUnRegIn1Error+fitResultRunDiffXUnRegOut1Error*fitResultRunDiffXUnRegOut1Error)/2;
  Double_t resultRunDiffXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXUnRegIn1Error*fitResultRunDiffXUnRegIn1Error+fitResultRunDiffXUnRegOut1Error*fitResultRunDiffXUnRegOut1Error)/2;


  TLegend *legRunDiffXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffXUnRegIn1,fitResultRunDiffXUnRegIn1Error),"lp");
  legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffXUnRegOut1,fitResultRunDiffXUnRegOut1Error),"lp");
//   legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXUnRegInpOut1,resultRunDiffXUnRegInpOut1Error),"");
//   legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXUnRegInmOut1,resultRunDiffXUnRegInmOut1Error),"");
  legRunDiffXUnReg1->SetTextSize(0.045);
  legRunDiffXUnReg1->SetFillColor(0);
  legRunDiffXUnReg1->SetBorderSize(2);
  legRunDiffXUnReg1->Draw();

  gPad->Update();
  

  pad61->cd(2);

  TGraphErrors * runDiffXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffXSlopeCalcUnRegInVal,zeroUnRegIn,ediffXSlopeCalcUnRegInVal);
  runDiffXSlopeGraphUnRegIn1->SetName("runDiffXSlopeGraphUnRegIn1");
  runDiffXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runDiffXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runDiffXSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runDiffXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffXSlopeGraphUnRegIn1->SetLineWidth(1);
//   runDiffXSlopeGraphUnRegIn1->Draw("AP");
  runDiffXSlopeGraphUnRegIn1->Fit("fitRunDiffXSlopeGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunDiffXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeUnRegIn1            =  fitRunDiffXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeUnRegIn1Error       =  fitRunDiffXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffXSlopeUnRegIn1Chisquare   =  fitRunDiffXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffXSlopeUnRegIn1NDF         =  fitRunDiffXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffXSlopeUnRegIn1Prob        =  fitRunDiffXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffXSlopeCalcUnRegOutVal,zeroUnRegOut,ediffXSlopeCalcUnRegOutVal);
  runDiffXSlopeGraphUnRegOut1->SetName("runDiffXSlopeGraphUnRegOut1");
  runDiffXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runDiffXSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runDiffXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffXSlopeGraphUnRegOut1->SetLineWidth(1);
//   runDiffXSlopeGraphUnRegOut1->Draw("AP");
  runDiffXSlopeGraphUnRegOut1->Fit("fitRunDiffXSlopeGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeUnRegOut1           =  fitRunDiffXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeUnRegOut1Error      =  fitRunDiffXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffXSlopeUnRegOut1Chisquare  =  fitRunDiffXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffXSlopeUnRegOut1NDF        =  fitRunDiffXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffXSlopeUnRegOut1Prob       =  fitRunDiffXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffXSlopeGraphUnReg1 = new TMultiGraph();
  runDiffXSlopeGraphUnReg1->Add(runDiffXSlopeGraphUnRegIn1);
  runDiffXSlopeGraphUnReg1->Add(runDiffXSlopeGraphUnRegOut1);
  runDiffXSlopeGraphUnReg1->Draw("AP");
  runDiffXSlopeGraphUnReg1->SetTitle("");
  runDiffXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXSlopeGraphUnReg1->GetYaxis()->SetTitle("Target X Slope Diff [#murad]");
  runDiffXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffXSlopeGraphUnReg1= runDiffXSlopeGraphUnReg1->GetXaxis();
  xaxisRunDiffXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXSlopeGraphUnRegIn1->Draw("same");
  fitRunDiffXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffXSlopeUnRegIn1 =(TPaveStats*)runDiffXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXSlopeUnRegOut1=(TPaveStats*)runDiffXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunDiffXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunDiffXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXSlopeUnRegInpOut1       =  (fitResultRunDiffXSlopeUnRegIn1+fitResultRunDiffXSlopeUnRegOut1)/2;
  Double_t resultRunDiffXSlopeUnRegInmOut1       =  (fitResultRunDiffXSlopeUnRegIn1-fitResultRunDiffXSlopeUnRegOut1)/2;
  Double_t resultRunDiffXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeUnRegIn1Error*fitResultRunDiffXSlopeUnRegIn1Error+fitResultRunDiffXSlopeUnRegOut1Error*fitResultRunDiffXSlopeUnRegOut1Error)/2;
  Double_t resultRunDiffXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeUnRegIn1Error*fitResultRunDiffXSlopeUnRegIn1Error+fitResultRunDiffXSlopeUnRegOut1Error*fitResultRunDiffXSlopeUnRegOut1Error)/2;


  TLegend *legRunDiffXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeUnRegIn1,fitResultRunDiffXSlopeUnRegIn1Error),"lp");
  legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeUnRegOut1,fitResultRunDiffXSlopeUnRegOut1Error),"lp");
//   legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXSlopeUnRegInpOut1,resultRunDiffXSlopeUnRegInpOut1Error),"");
//   legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXSlopeUnRegInmOut1,resultRunDiffXSlopeUnRegInmOut1Error),"");
  legRunDiffXSlopeUnReg1->SetTextSize(0.045);
  legRunDiffXSlopeUnReg1->SetFillColor(0);
  legRunDiffXSlopeUnReg1->SetBorderSize(2);
  legRunDiffXSlopeUnReg1->Draw();

  gPad->Update();


  pad61->cd(3);

  TGraphErrors * runDiffYGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffYCalcUnRegInVal,zeroUnRegIn,ediffYCalcUnRegInVal);
  runDiffYGraphUnRegIn1->SetName("runDiffYGraphUnRegIn1");
  runDiffYGraphUnRegIn1->SetMarkerColor(kGreen);
  runDiffYGraphUnRegIn1->SetLineColor(kGreen);
  runDiffYGraphUnRegIn1->SetMarkerStyle(25);
  runDiffYGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffYGraphUnRegIn1->SetLineWidth(1);
//   runDiffYGraphUnRegIn1->Draw("AP");
  runDiffYGraphUnRegIn1->Fit("fitRunDiffYGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunDiffYGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYUnRegIn1            =  fitRunDiffYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYUnRegIn1Error       =  fitRunDiffYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffYUnRegIn1Chisquare   =  fitRunDiffYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffYUnRegIn1NDF         =  fitRunDiffYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffYUnRegIn1Prob        =  fitRunDiffYGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffYGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffYCalcUnRegOutVal,zeroUnRegOut,ediffYCalcUnRegOutVal);
  runDiffYGraphUnRegOut1->SetName("runDiffYGraphUnRegOut1");
  runDiffYGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffYGraphUnRegOut1->SetLineColor(kBlack);
  runDiffYGraphUnRegOut1->SetMarkerStyle(25);
  runDiffYGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffYGraphUnRegOut1->SetLineWidth(1);
//   runDiffYGraphUnRegOut1->Draw("AP");
  runDiffYGraphUnRegOut1->Fit("fitRunDiffYGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffYGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYUnRegOut1           =  fitRunDiffYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYUnRegOut1Error      =  fitRunDiffYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffYUnRegOut1Chisquare  =  fitRunDiffYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffYUnRegOut1NDF        =  fitRunDiffYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffYUnRegOut1Prob       =  fitRunDiffYGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffYGraphUnReg1 = new TMultiGraph();
  runDiffYGraphUnReg1->Add(runDiffYGraphUnRegIn1);
  runDiffYGraphUnReg1->Add(runDiffYGraphUnRegOut1);
  runDiffYGraphUnReg1->Draw("AP");
  runDiffYGraphUnReg1->SetTitle("");
  runDiffYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYGraphUnReg1->GetYaxis()->SetTitle("Target Y Diff [mm]");
  runDiffYGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffYGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYGraphUnReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffYGraphUnReg1= runDiffYGraphUnReg1->GetXaxis();
  xaxisRunDiffYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYGraphUnRegIn1->Draw("same");
  fitRunDiffYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffYUnRegIn1 =(TPaveStats*)runDiffYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYUnRegOut1=(TPaveStats*)runDiffYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYUnRegIn1->SetTextColor(kGreen);
  statsRunDiffYUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffYUnRegOut1->SetTextColor(kBlack);
  statsRunDiffYUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYUnRegInpOut1       =  (fitResultRunDiffYUnRegIn1+fitResultRunDiffYUnRegOut1)/2;
  Double_t resultRunDiffYUnRegInmOut1       =  (fitResultRunDiffYUnRegIn1-fitResultRunDiffYUnRegOut1)/2;
  Double_t resultRunDiffYUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYUnRegIn1Error*fitResultRunDiffYUnRegIn1Error+fitResultRunDiffYUnRegOut1Error*fitResultRunDiffYUnRegOut1Error)/2;
  Double_t resultRunDiffYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYUnRegIn1Error*fitResultRunDiffYUnRegIn1Error+fitResultRunDiffYUnRegOut1Error*fitResultRunDiffYUnRegOut1Error)/2;


  TLegend *legRunDiffYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffYUnRegIn1,fitResultRunDiffYUnRegIn1Error),"lp");
  legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffYUnRegOut1,fitResultRunDiffYUnRegOut1Error),"lp");
//   legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYUnRegInpOut1,resultRunDiffYUnRegInpOut1Error),"");
//   legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYUnRegInmOut1,resultRunDiffYUnRegInmOut1Error),"");
  legRunDiffYUnReg1->SetTextSize(0.045);
  legRunDiffYUnReg1->SetFillColor(0);
  legRunDiffYUnReg1->SetBorderSize(2);
  legRunDiffYUnReg1->Draw();

  gPad->Update();


  pad61->cd(4);

  TGraphErrors * runDiffYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffYSlopeCalcUnRegInVal,zeroUnRegIn,ediffYSlopeCalcUnRegInVal);
  runDiffYSlopeGraphUnRegIn1->SetName("runDiffYSlopeGraphUnRegIn1");
  runDiffYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runDiffYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runDiffYSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runDiffYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffYSlopeGraphUnRegIn1->SetLineWidth(1);
//   runDiffYSlopeGraphUnRegIn1->Draw("AP");
  runDiffYSlopeGraphUnRegIn1->Fit("fitRunDiffYSlopeGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunDiffYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeUnRegIn1            =  fitRunDiffYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeUnRegIn1Error       =  fitRunDiffYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffYSlopeUnRegIn1Chisquare   =  fitRunDiffYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffYSlopeUnRegIn1NDF         =  fitRunDiffYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffYSlopeUnRegIn1Prob        =  fitRunDiffYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffYSlopeCalcUnRegOutVal,zeroUnRegOut,ediffYSlopeCalcUnRegOutVal);
  runDiffYSlopeGraphUnRegOut1->SetName("runDiffYSlopeGraphUnRegOut1");
  runDiffYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runDiffYSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runDiffYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffYSlopeGraphUnRegOut1->SetLineWidth(1);
//   runDiffYSlopeGraphUnRegOut1->Draw("AP");
  runDiffYSlopeGraphUnRegOut1->Fit("fitRunDiffYSlopeGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeUnRegOut1           =  fitRunDiffYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeUnRegOut1Error      =  fitRunDiffYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffYSlopeUnRegOut1Chisquare  =  fitRunDiffYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffYSlopeUnRegOut1NDF        =  fitRunDiffYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffYSlopeUnRegOut1Prob       =  fitRunDiffYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffYSlopeGraphUnReg1 = new TMultiGraph();
  runDiffYSlopeGraphUnReg1->Add(runDiffYSlopeGraphUnRegIn1);
  runDiffYSlopeGraphUnReg1->Add(runDiffYSlopeGraphUnRegOut1);
  runDiffYSlopeGraphUnReg1->Draw("AP");
  runDiffYSlopeGraphUnReg1->SetTitle("");
  runDiffYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYSlopeGraphUnReg1->GetYaxis()->SetTitle("Target Y Slope Diff [#murad]");
  runDiffYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffYSlopeGraphUnReg1= runDiffYSlopeGraphUnReg1->GetXaxis();
  xaxisRunDiffYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYSlopeGraphUnRegIn1->Draw("same");
  fitRunDiffYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffYSlopeUnRegIn1 =(TPaveStats*)runDiffYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYSlopeUnRegOut1=(TPaveStats*)runDiffYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunDiffYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunDiffYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYSlopeUnRegInpOut1       =  (fitResultRunDiffYSlopeUnRegIn1+fitResultRunDiffYSlopeUnRegOut1)/2;
  Double_t resultRunDiffYSlopeUnRegInmOut1       =  (fitResultRunDiffYSlopeUnRegIn1-fitResultRunDiffYSlopeUnRegOut1)/2;
  Double_t resultRunDiffYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeUnRegIn1Error*fitResultRunDiffYSlopeUnRegIn1Error+fitResultRunDiffYSlopeUnRegOut1Error*fitResultRunDiffYSlopeUnRegOut1Error)/2;
  Double_t resultRunDiffYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeUnRegIn1Error*fitResultRunDiffYSlopeUnRegIn1Error+fitResultRunDiffYSlopeUnRegOut1Error*fitResultRunDiffYSlopeUnRegOut1Error)/2;

  
  TLegend *legRunDiffYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeUnRegIn1,fitResultRunDiffYSlopeUnRegIn1Error),"lp");
  legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeUnRegOut1,fitResultRunDiffYSlopeUnRegOut1Error),"lp");
//   legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYSlopeUnRegInpOut1,resultRunDiffYSlopeUnRegInpOut1Error),"");
//   legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYSlopeUnRegInmOut1,resultRunDiffYSlopeUnRegInmOut1Error),"");
  legRunDiffYSlopeUnReg1->SetTextSize(0.045);
  legRunDiffYSlopeUnReg1->SetFillColor(0);
  legRunDiffYSlopeUnReg1->SetBorderSize(2);
  legRunDiffYSlopeUnReg1->Draw();

  gPad->Update();
  

  pad61->cd(5);
  
  TGraphErrors * runDiffEGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffECalcUnRegInVal,zeroUnRegIn,ediffECalcUnRegInVal);
  runDiffEGraphUnRegIn1->SetName("runDiffEGraphUnRegIn1");
  runDiffEGraphUnRegIn1->SetMarkerColor(kBlue);
  runDiffEGraphUnRegIn1->SetLineColor(kBlue);
  runDiffEGraphUnRegIn1->SetMarkerStyle(24);
  runDiffEGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffEGraphUnRegIn1->SetLineWidth(1);
//   runDiffEGraphUnRegIn1->Draw("AP");
  runDiffEGraphUnRegIn1->Fit("fitRunDiffEGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunDiffEGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffEGraphUnRegIn1->SetLineStyle(3);
  
  Double_t fitResultRunDiffEUnRegIn1            =  fitRunDiffEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffEUnRegIn1Error       =  fitRunDiffEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffEUnRegIn1Chisquare   =  fitRunDiffEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffEUnRegIn1NDF         =  fitRunDiffEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffEUnRegIn1Prob        =  fitRunDiffEGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffEGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffECalcUnRegOutVal,zeroUnRegOut,ediffECalcUnRegOutVal);
  runDiffEGraphUnRegOut1->SetName("runDiffEGraphUnRegOut1");
  runDiffEGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffEGraphUnRegOut1->SetLineColor(kBlack);
  runDiffEGraphUnRegOut1->SetMarkerStyle(24);
  runDiffEGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffEGraphUnRegOut1->SetLineWidth(1);
//   runDiffEGraphUnRegOut1->Draw("AP");
  runDiffEGraphUnRegOut1->Fit("fitRunDiffEGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffEGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffEUnRegOut1           =  fitRunDiffEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffEUnRegOut1Error      =  fitRunDiffEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffEUnRegOut1Chisquare  =  fitRunDiffEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffEUnRegOut1NDF        =  fitRunDiffEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffEUnRegOut1Prob       =  fitRunDiffEGraphUnRegOut1->GetProb();
  
  TMultiGraph *runDiffEGraphUnReg1 = new TMultiGraph();
  runDiffEGraphUnReg1->Add(runDiffEGraphUnRegIn1);
  runDiffEGraphUnReg1->Add(runDiffEGraphUnRegOut1);
  runDiffEGraphUnReg1->Draw("AP");
  runDiffEGraphUnReg1->SetTitle("");
  runDiffEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffEGraphUnReg1->GetYaxis()->SetTitle("Energy Diff [ppm]");
  runDiffEGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffEGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffEGraphUnReg1->GetYaxis()->SetRangeUser(-0.0e-6,0.15e-6);
  TAxis *xaxisRunDiffEGraphUnReg1= runDiffEGraphUnReg1->GetXaxis();
  xaxisRunDiffEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffEGraphUnRegIn1->Draw("same");
  fitRunDiffEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffEUnRegIn1 =(TPaveStats*)runDiffEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffEUnRegOut1=(TPaveStats*)runDiffEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffEUnRegIn1->SetTextColor(kBlue);
  statsRunDiffEUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffEUnRegOut1->SetTextColor(kBlack);
  statsRunDiffEUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffEUnRegInpOut1       =  (fitResultRunDiffEUnRegIn1+fitResultRunDiffEUnRegOut1)/2;
  Double_t resultRunDiffEUnRegInmOut1       =  (fitResultRunDiffEUnRegIn1-fitResultRunDiffEUnRegOut1)/2;
  Double_t resultRunDiffEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffEUnRegIn1Error*fitResultRunDiffEUnRegIn1Error+fitResultRunDiffEUnRegOut1Error*fitResultRunDiffEUnRegOut1Error)/2;
  Double_t resultRunDiffEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffEUnRegIn1Error*fitResultRunDiffEUnRegIn1Error+fitResultRunDiffEUnRegOut1Error*fitResultRunDiffEUnRegOut1Error)/2;

  
  TLegend *legRunDiffEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffEUnRegIn1,fitResultRunDiffEUnRegIn1Error),"lp");
  legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffEUnRegOut1,fitResultRunDiffEUnRegOut1Error),"lp");
//   legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffEUnRegInpOut1,resultRunDiffEUnRegInpOut1Error),"");
//   legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffEUnRegInmOut1,resultRunDiffEUnRegInmOut1Error),"");
  legRunDiffEUnReg1->SetTextSize(0.045);
  legRunDiffEUnReg1->SetFillColor(0);
  legRunDiffEUnReg1->SetBorderSize(2);
  legRunDiffEUnReg1->Draw();
  
  gPad->Update();

  
  pad61->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,chargeRegInVal,zeroUnRegIn,echargeRegInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q ");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();

  
  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,chargeRegOutVal,zeroUnRegOut,echargeRegOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q ");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-20,20);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();
  
  ofstream outfileUnRegDiffAvgIn(Form("%s/plots/%s/summary%sRunletUnRegDiffAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileUnRegDiffAvgIn << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
				,fitResultRunDiffXUnRegIn1,fitResultRunDiffXUnRegIn1Error
				,fitResultRunDiffXSlopeUnRegIn1,fitResultRunDiffXSlopeUnRegIn1Error
				,fitResultRunDiffYUnRegIn1,fitResultRunDiffYUnRegIn1Error
				,fitResultRunDiffYSlopeUnRegIn1,fitResultRunDiffYSlopeUnRegIn1Error
				,fitResultRunDiffEUnRegIn1,fitResultRunDiffEUnRegIn1Error
				,fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error)
			<<endl;
  outfileUnRegDiffAvgIn.close();

  ofstream outfileUnRegDiffAvgOut(Form("%s/plots/%s/summary%sRunletUnRegDiffAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileUnRegDiffAvgOut << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
				,fitResultRunDiffXUnRegOut1,fitResultRunDiffXUnRegOut1Error
				,fitResultRunDiffXSlopeUnRegOut1,fitResultRunDiffXSlopeUnRegOut1Error
				,fitResultRunDiffYUnRegOut1,fitResultRunDiffYUnRegOut1Error
				,fitResultRunDiffYSlopeUnRegOut1,fitResultRunDiffYSlopeUnRegOut1Error
				,fitResultRunDiffEUnRegOut1,fitResultRunDiffEUnRegOut1Error
				,fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error)
			<<endl;
  outfileUnRegDiffAvgOut.close();

  
  c6->Update();
  c6->SaveAs(Form("%s/plots/%s/TargetDiffUnRegRunletbyRunlet.png",n2deltaAnalysisDir.Data(),regScheme));

  
  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c7 = new TCanvas("c7","c7",cx1,cy1,1200,950);
  TPad * pad70 = new TPad("pad70","pad70",ps1,ps2,ps4,ps4);
  TPad * pad71 = new TPad("pad71","pad71",ps1,ps1,ps4,ps3);
  pad70->Draw();
  pad71->Draw();
  pad70->cd();
  TText * ct70 = new TText(tll,tlr,Form("%sN to Delta %s Regressed Position and Energy Differences",regTitle,dataInfo));
  ct70->SetTextSize(tsiz);
  ct70->Draw();
  pad71->cd();
  pad71->Divide(2,3);


  pad71->cd(1);

  TGraphErrors * runDiffXGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffXRegInVal,zeroRegIn,ediffXRegInVal);
  runDiffXGraphRegIn1->SetName("runDiffXGraphRegIn1");
  runDiffXGraphRegIn1->SetMarkerColor(kRed);
  runDiffXGraphRegIn1->SetLineColor(kRed);
  runDiffXGraphRegIn1->SetMarkerStyle(21);
  runDiffXGraphRegIn1->SetMarkerSize(0.5);
  runDiffXGraphRegIn1->SetLineWidth(1);
//   runDiffXGraphRegIn1->Draw("AP");
  runDiffXGraphRegIn1->Fit("fitRunDiffXGraphRegIn1","E M R F 0 Q W");
  fitRunDiffXGraphRegIn1->SetLineColor(kRed);
  fitRunDiffXGraphRegIn1->SetLineWidth(2);
  fitRunDiffXGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXRegIn1            =  fitRunDiffXGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXRegIn1Error       =  fitRunDiffXGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffXRegIn1Chisquare   =  fitRunDiffXGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffXRegIn1NDF         =  fitRunDiffXGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffXRegIn1Prob        =  fitRunDiffXGraphRegIn1->GetProb();


  TGraphErrors * runDiffXGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffXRegOutVal,zeroRegOut,ediffXRegOutVal);
  runDiffXGraphRegOut1->SetName("runDiffXGraphRegOut1");
  runDiffXGraphRegOut1->SetMarkerColor(kBlack);
  runDiffXGraphRegOut1->SetLineColor(kBlack);
  runDiffXGraphRegOut1->SetMarkerStyle(21);
  runDiffXGraphRegOut1->SetMarkerSize(0.5);
  runDiffXGraphRegOut1->SetLineWidth(1);
//   runDiffXGraphRegOut1->Draw("AP");
  runDiffXGraphRegOut1->Fit("fitRunDiffXGraphRegOut1","E M R F 0 Q W");
  fitRunDiffXGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffXGraphRegOut1->SetLineWidth(2);
  fitRunDiffXGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXRegOut1           =  fitRunDiffXGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXRegOut1Error      =  fitRunDiffXGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffXRegOut1Chisquare  =  fitRunDiffXGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffXRegOut1NDF        =  fitRunDiffXGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffXRegOut1Prob       =  fitRunDiffXGraphRegOut1->GetProb();

  TMultiGraph *runDiffXGraphReg1 = new TMultiGraph();
  runDiffXGraphReg1->Add(runDiffXGraphRegIn1);
  runDiffXGraphReg1->Add(runDiffXGraphRegOut1);
  runDiffXGraphReg1->Draw("AP");
  runDiffXGraphReg1->SetTitle("");
  runDiffXGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXGraphReg1->GetYaxis()->SetTitle("Target X Diff [mm]");
  runDiffXGraphReg1->GetXaxis()->CenterTitle();
  runDiffXGraphReg1->GetYaxis()->CenterTitle();
  runDiffXGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXGraphReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffXGraphReg1= runDiffXGraphReg1->GetXaxis();
  xaxisRunDiffXGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXGraphRegIn1->Draw("same");
  fitRunDiffXGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffXRegIn1 =(TPaveStats*)runDiffXGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXRegOut1=(TPaveStats*)runDiffXGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXRegIn1->SetTextColor(kRed);
  statsRunDiffXRegIn1->SetFillColor(kWhite); 
  statsRunDiffXRegOut1->SetTextColor(kBlack);
  statsRunDiffXRegOut1->SetFillColor(kWhite); 
  statsRunDiffXRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXRegInpOut1       =  (fitResultRunDiffXRegIn1+fitResultRunDiffXRegOut1)/2;
  Double_t resultRunDiffXRegInmOut1       =  (fitResultRunDiffXRegIn1-fitResultRunDiffXRegOut1)/2;
  Double_t resultRunDiffXRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXRegIn1Error*fitResultRunDiffXRegIn1Error+fitResultRunDiffXRegOut1Error*fitResultRunDiffXRegOut1Error)/2;
  Double_t resultRunDiffXRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXRegIn1Error*fitResultRunDiffXRegIn1Error+fitResultRunDiffXRegOut1Error*fitResultRunDiffXRegOut1Error)/2;


  TLegend *legRunDiffXReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXReg1->AddEntry("runDiffXGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffXRegIn1,fitResultRunDiffXRegIn1Error),"lp");
  legRunDiffXReg1->AddEntry("runDiffXGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffXRegOut1,fitResultRunDiffXRegOut1Error),"lp");
//   legRunDiffXReg1->AddEntry("runDiffXGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXRegInpOut1,resultRunDiffXRegInpOut1Error),"");
//   legRunDiffXReg1->AddEntry("runDiffXGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXRegInmOut1,resultRunDiffXRegInmOut1Error),"");
  legRunDiffXReg1->SetTextSize(0.045);
  legRunDiffXReg1->SetFillColor(0);
  legRunDiffXReg1->SetBorderSize(2);
  legRunDiffXReg1->Draw();

  gPad->Update();


  pad71->cd(2);

  TGraphErrors * runDiffXSlopeGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffXSlopeRegInVal,zeroRegIn,ediffXSlopeRegInVal);
  runDiffXSlopeGraphRegIn1->SetName("runDiffXSlopeGraphRegIn1");
  runDiffXSlopeGraphRegIn1->SetMarkerColor(kRed);
  runDiffXSlopeGraphRegIn1->SetLineColor(kRed);
  runDiffXSlopeGraphRegIn1->SetMarkerStyle(22);
  runDiffXSlopeGraphRegIn1->SetMarkerSize(0.5);
  runDiffXSlopeGraphRegIn1->SetLineWidth(1);
//   runDiffXSlopeGraphRegIn1->Draw("AP");
  runDiffXSlopeGraphRegIn1->Fit("fitRunDiffXSlopeGraphRegIn1","E M R F 0 Q W");
  fitRunDiffXSlopeGraphRegIn1->SetLineColor(kRed);
  fitRunDiffXSlopeGraphRegIn1->SetLineWidth(2);
  fitRunDiffXSlopeGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeRegIn1            =  fitRunDiffXSlopeGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeRegIn1Error       =  fitRunDiffXSlopeGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffXSlopeRegIn1Chisquare   =  fitRunDiffXSlopeGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffXSlopeRegIn1NDF         =  fitRunDiffXSlopeGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffXSlopeRegIn1Prob        =  fitRunDiffXSlopeGraphRegIn1->GetProb();


  TGraphErrors * runDiffXSlopeGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffXSlopeRegOutVal,zeroRegOut,ediffXSlopeRegOutVal);
  runDiffXSlopeGraphRegOut1->SetName("runDiffXSlopeGraphRegOut1");
  runDiffXSlopeGraphRegOut1->SetMarkerColor(kBlack);
  runDiffXSlopeGraphRegOut1->SetLineColor(kBlack);
  runDiffXSlopeGraphRegOut1->SetMarkerStyle(22);
  runDiffXSlopeGraphRegOut1->SetMarkerSize(0.5);
  runDiffXSlopeGraphRegOut1->SetLineWidth(1);
//   runDiffXSlopeGraphRegOut1->Draw("AP");
  runDiffXSlopeGraphRegOut1->Fit("fitRunDiffXSlopeGraphRegOut1","E M R F 0 Q W");
  fitRunDiffXSlopeGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffXSlopeGraphRegOut1->SetLineWidth(2);
  fitRunDiffXSlopeGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeRegOut1           =  fitRunDiffXSlopeGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeRegOut1Error      =  fitRunDiffXSlopeGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffXSlopeRegOut1Chisquare  =  fitRunDiffXSlopeGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffXSlopeRegOut1NDF        =  fitRunDiffXSlopeGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffXSlopeRegOut1Prob       =  fitRunDiffXSlopeGraphRegOut1->GetProb();

  TMultiGraph *runDiffXSlopeGraphReg1 = new TMultiGraph();
  runDiffXSlopeGraphReg1->Add(runDiffXSlopeGraphRegIn1);
  runDiffXSlopeGraphReg1->Add(runDiffXSlopeGraphRegOut1);
  runDiffXSlopeGraphReg1->Draw("AP");
  runDiffXSlopeGraphReg1->SetTitle("");
  runDiffXSlopeGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXSlopeGraphReg1->GetYaxis()->SetTitle("Target X Slope Diff [#murad]");
  runDiffXSlopeGraphReg1->GetXaxis()->CenterTitle();
  runDiffXSlopeGraphReg1->GetYaxis()->CenterTitle();
  runDiffXSlopeGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXSlopeGraphReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffXSlopeGraphReg1= runDiffXSlopeGraphReg1->GetXaxis();
  xaxisRunDiffXSlopeGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXSlopeGraphRegIn1->Draw("same");
  fitRunDiffXSlopeGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffXSlopeRegIn1 =(TPaveStats*)runDiffXSlopeGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXSlopeRegOut1=(TPaveStats*)runDiffXSlopeGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXSlopeRegIn1->SetTextColor(kRed);
  statsRunDiffXSlopeRegIn1->SetFillColor(kWhite); 
  statsRunDiffXSlopeRegOut1->SetTextColor(kBlack);
  statsRunDiffXSlopeRegOut1->SetFillColor(kWhite); 
  statsRunDiffXSlopeRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXSlopeRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXSlopeRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXSlopeRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXSlopeRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXSlopeRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXSlopeRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXSlopeRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXSlopeRegInpOut1       =  (fitResultRunDiffXSlopeRegIn1+fitResultRunDiffXSlopeRegOut1)/2;
  Double_t resultRunDiffXSlopeRegInmOut1       =  (fitResultRunDiffXSlopeRegIn1-fitResultRunDiffXSlopeRegOut1)/2;
  Double_t resultRunDiffXSlopeRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeRegIn1Error*fitResultRunDiffXSlopeRegIn1Error+fitResultRunDiffXSlopeRegOut1Error*fitResultRunDiffXSlopeRegOut1Error)/2;
  Double_t resultRunDiffXSlopeRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeRegIn1Error*fitResultRunDiffXSlopeRegIn1Error+fitResultRunDiffXSlopeRegOut1Error*fitResultRunDiffXSlopeRegOut1Error)/2;


  TLegend *legRunDiffXSlopeReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeRegIn1,fitResultRunDiffXSlopeRegIn1Error),"lp");
  legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeRegOut1,fitResultRunDiffXSlopeRegOut1Error),"lp");
//   legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXSlopeRegInpOut1,resultRunDiffXSlopeRegInpOut1Error),"");
//   legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXSlopeRegInmOut1,resultRunDiffXSlopeRegInmOut1Error),"");
  legRunDiffXSlopeReg1->SetTextSize(0.045);
  legRunDiffXSlopeReg1->SetFillColor(0);
  legRunDiffXSlopeReg1->SetBorderSize(2);
  legRunDiffXSlopeReg1->Draw();

  gPad->Update();


  pad71->cd(3);

  TGraphErrors * runDiffYGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffYRegInVal,zeroRegIn,ediffYRegInVal);
  runDiffYGraphRegIn1->SetName("runDiffYGraphRegIn1");
  runDiffYGraphRegIn1->SetMarkerColor(kGreen);
  runDiffYGraphRegIn1->SetLineColor(kGreen);
  runDiffYGraphRegIn1->SetMarkerStyle(21);
  runDiffYGraphRegIn1->SetMarkerSize(0.5);
  runDiffYGraphRegIn1->SetLineWidth(1);
//   runDiffYGraphRegIn1->Draw("AP");
  runDiffYGraphRegIn1->Fit("fitRunDiffYGraphRegIn1","E M R F 0 Q W");
  fitRunDiffYGraphRegIn1->SetLineColor(kGreen);
  fitRunDiffYGraphRegIn1->SetLineWidth(2);
  fitRunDiffYGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYRegIn1            =  fitRunDiffYGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYRegIn1Error       =  fitRunDiffYGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffYRegIn1Chisquare   =  fitRunDiffYGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffYRegIn1NDF         =  fitRunDiffYGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffYRegIn1Prob        =  fitRunDiffYGraphRegIn1->GetProb();


  TGraphErrors * runDiffYGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffYRegOutVal,zeroRegOut,ediffYRegOutVal);
  runDiffYGraphRegOut1->SetName("runDiffYGraphRegOut1");
  runDiffYGraphRegOut1->SetMarkerColor(kBlack);
  runDiffYGraphRegOut1->SetLineColor(kBlack);
  runDiffYGraphRegOut1->SetMarkerStyle(21);
  runDiffYGraphRegOut1->SetMarkerSize(0.5);
  runDiffYGraphRegOut1->SetLineWidth(1);
//   runDiffYGraphRegOut1->Draw("AP");
  runDiffYGraphRegOut1->Fit("fitRunDiffYGraphRegOut1","E M R F 0 Q W");
  fitRunDiffYGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffYGraphRegOut1->SetLineWidth(2);
  fitRunDiffYGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYRegOut1           =  fitRunDiffYGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYRegOut1Error      =  fitRunDiffYGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffYRegOut1Chisquare  =  fitRunDiffYGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffYRegOut1NDF        =  fitRunDiffYGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffYRegOut1Prob       =  fitRunDiffYGraphRegOut1->GetProb();

  TMultiGraph *runDiffYGraphReg1 = new TMultiGraph();
  runDiffYGraphReg1->Add(runDiffYGraphRegIn1);
  runDiffYGraphReg1->Add(runDiffYGraphRegOut1);
  runDiffYGraphReg1->Draw("AP");
  runDiffYGraphReg1->SetTitle("");
  runDiffYGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYGraphReg1->GetYaxis()->SetTitle("Target Y Diff [mm]");
  runDiffYGraphReg1->GetXaxis()->CenterTitle();
  runDiffYGraphReg1->GetYaxis()->CenterTitle();
  runDiffYGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYGraphReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffYGraphReg1= runDiffYGraphReg1->GetXaxis();
  xaxisRunDiffYGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYGraphRegIn1->Draw("same");
  fitRunDiffYGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffYRegIn1 =(TPaveStats*)runDiffYGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYRegOut1=(TPaveStats*)runDiffYGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYRegIn1->SetTextColor(kGreen);
  statsRunDiffYRegIn1->SetFillColor(kWhite); 
  statsRunDiffYRegOut1->SetTextColor(kBlack);
  statsRunDiffYRegOut1->SetFillColor(kWhite); 
  statsRunDiffYRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYRegInpOut1       =  (fitResultRunDiffYRegIn1+fitResultRunDiffYRegOut1)/2;
  Double_t resultRunDiffYRegInmOut1       =  (fitResultRunDiffYRegIn1-fitResultRunDiffYRegOut1)/2;
  Double_t resultRunDiffYRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYRegIn1Error*fitResultRunDiffYRegIn1Error+fitResultRunDiffYRegOut1Error*fitResultRunDiffYRegOut1Error)/2;
  Double_t resultRunDiffYRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYRegIn1Error*fitResultRunDiffYRegIn1Error+fitResultRunDiffYRegOut1Error*fitResultRunDiffYRegOut1Error)/2;


  TLegend *legRunDiffYReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYReg1->AddEntry("runDiffYGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffYRegIn1,fitResultRunDiffYRegIn1Error),"lp");
  legRunDiffYReg1->AddEntry("runDiffYGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffYRegOut1,fitResultRunDiffYRegOut1Error),"lp");
//   legRunDiffYReg1->AddEntry("runDiffYGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYRegInpOut1,resultRunDiffYRegInpOut1Error),"");
//   legRunDiffYReg1->AddEntry("runDiffYGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYRegInmOut1,resultRunDiffYRegInmOut1Error),"");
  legRunDiffYReg1->SetTextSize(0.045);
  legRunDiffYReg1->SetFillColor(0);
  legRunDiffYReg1->SetBorderSize(2);
  legRunDiffYReg1->Draw();

  gPad->Update();


  pad71->cd(4);

  TGraphErrors * runDiffYSlopeGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffYSlopeRegInVal,zeroRegIn,ediffYSlopeRegInVal);
  runDiffYSlopeGraphRegIn1->SetName("runDiffYSlopeGraphRegIn1");
  runDiffYSlopeGraphRegIn1->SetMarkerColor(kGreen);
  runDiffYSlopeGraphRegIn1->SetLineColor(kGreen);
  runDiffYSlopeGraphRegIn1->SetMarkerStyle(22);
  runDiffYSlopeGraphRegIn1->SetMarkerSize(0.5);
  runDiffYSlopeGraphRegIn1->SetLineWidth(1);
//   runDiffYSlopeGraphRegIn1->Draw("AP");
  runDiffYSlopeGraphRegIn1->Fit("fitRunDiffYSlopeGraphRegIn1","E M R F 0 Q W");
  fitRunDiffYSlopeGraphRegIn1->SetLineColor(kGreen);
  fitRunDiffYSlopeGraphRegIn1->SetLineWidth(2);
  fitRunDiffYSlopeGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeRegIn1            =  fitRunDiffYSlopeGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeRegIn1Error       =  fitRunDiffYSlopeGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffYSlopeRegIn1Chisquare   =  fitRunDiffYSlopeGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffYSlopeRegIn1NDF         =  fitRunDiffYSlopeGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffYSlopeRegIn1Prob        =  fitRunDiffYSlopeGraphRegIn1->GetProb();


  TGraphErrors * runDiffYSlopeGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffYSlopeRegOutVal,zeroRegOut,ediffYSlopeRegOutVal);
  runDiffYSlopeGraphRegOut1->SetName("runDiffYSlopeGraphRegOut1");
  runDiffYSlopeGraphRegOut1->SetMarkerColor(kBlack);
  runDiffYSlopeGraphRegOut1->SetLineColor(kBlack);
  runDiffYSlopeGraphRegOut1->SetMarkerStyle(22);
  runDiffYSlopeGraphRegOut1->SetMarkerSize(0.5);
  runDiffYSlopeGraphRegOut1->SetLineWidth(1);
//   runDiffYSlopeGraphRegOut1->Draw("AP");
  runDiffYSlopeGraphRegOut1->Fit("fitRunDiffYSlopeGraphRegOut1","E M R F 0 Q W");
  fitRunDiffYSlopeGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffYSlopeGraphRegOut1->SetLineWidth(2);
  fitRunDiffYSlopeGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeRegOut1           =  fitRunDiffYSlopeGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeRegOut1Error      =  fitRunDiffYSlopeGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffYSlopeRegOut1Chisquare  =  fitRunDiffYSlopeGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffYSlopeRegOut1NDF        =  fitRunDiffYSlopeGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffYSlopeRegOut1Prob       =  fitRunDiffYSlopeGraphRegOut1->GetProb();

  TMultiGraph *runDiffYSlopeGraphReg1 = new TMultiGraph();
  runDiffYSlopeGraphReg1->Add(runDiffYSlopeGraphRegIn1);
  runDiffYSlopeGraphReg1->Add(runDiffYSlopeGraphRegOut1);
  runDiffYSlopeGraphReg1->Draw("AP");
  runDiffYSlopeGraphReg1->SetTitle("");
  runDiffYSlopeGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYSlopeGraphReg1->GetYaxis()->SetTitle("Target Y Slope Diff [#murad]");
  runDiffYSlopeGraphReg1->GetXaxis()->CenterTitle();
  runDiffYSlopeGraphReg1->GetYaxis()->CenterTitle();
  runDiffYSlopeGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYSlopeGraphReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffYSlopeGraphReg1= runDiffYSlopeGraphReg1->GetXaxis();
  xaxisRunDiffYSlopeGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYSlopeGraphRegIn1->Draw("same");
  fitRunDiffYSlopeGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffYSlopeRegIn1 =(TPaveStats*)runDiffYSlopeGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYSlopeRegOut1=(TPaveStats*)runDiffYSlopeGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYSlopeRegIn1->SetTextColor(kGreen);
  statsRunDiffYSlopeRegIn1->SetFillColor(kWhite); 
  statsRunDiffYSlopeRegOut1->SetTextColor(kBlack);
  statsRunDiffYSlopeRegOut1->SetFillColor(kWhite); 
  statsRunDiffYSlopeRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYSlopeRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYSlopeRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYSlopeRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYSlopeRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYSlopeRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYSlopeRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYSlopeRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYSlopeRegInpOut1       =  (fitResultRunDiffYSlopeRegIn1+fitResultRunDiffYSlopeRegOut1)/2;
  Double_t resultRunDiffYSlopeRegInmOut1       =  (fitResultRunDiffYSlopeRegIn1-fitResultRunDiffYSlopeRegOut1)/2;
  Double_t resultRunDiffYSlopeRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeRegIn1Error*fitResultRunDiffYSlopeRegIn1Error+fitResultRunDiffYSlopeRegOut1Error*fitResultRunDiffYSlopeRegOut1Error)/2;
  Double_t resultRunDiffYSlopeRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeRegIn1Error*fitResultRunDiffYSlopeRegIn1Error+fitResultRunDiffYSlopeRegOut1Error*fitResultRunDiffYSlopeRegOut1Error)/2;


  TLegend *legRunDiffYSlopeReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeRegIn1,fitResultRunDiffYSlopeRegIn1Error),"lp");
  legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeRegOut1,fitResultRunDiffYSlopeRegOut1Error),"lp");
//   legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYSlopeRegInpOut1,resultRunDiffYSlopeRegInpOut1Error),"");
//   legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYSlopeRegInmOut1,resultRunDiffYSlopeRegInmOut1Error),"");
  legRunDiffYSlopeReg1->SetTextSize(0.045);
  legRunDiffYSlopeReg1->SetFillColor(0);
  legRunDiffYSlopeReg1->SetBorderSize(2);
  legRunDiffYSlopeReg1->Draw();

  gPad->Update();


  pad71->cd(5);

  TGraphErrors * runDiffEGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffERegInVal,zeroRegIn,ediffERegInVal);
  runDiffEGraphRegIn1->SetName("runDiffEGraphRegIn1");
  runDiffEGraphRegIn1->SetMarkerColor(kBlue);
  runDiffEGraphRegIn1->SetLineColor(kBlue);
  runDiffEGraphRegIn1->SetMarkerStyle(20);
  runDiffEGraphRegIn1->SetMarkerSize(0.5);
  runDiffEGraphRegIn1->SetLineWidth(1);
//   runDiffEGraphRegIn1->Draw("AP");
  runDiffEGraphRegIn1->Fit("fitRunDiffEGraphRegIn1","E M R F 0 Q W");
  fitRunDiffEGraphRegIn1->SetLineColor(kBlue);
  fitRunDiffEGraphRegIn1->SetLineWidth(2);
  fitRunDiffEGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffERegIn1            =  fitRunDiffEGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffERegIn1Error       =  fitRunDiffEGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffERegIn1Chisquare   =  fitRunDiffEGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffERegIn1NDF         =  fitRunDiffEGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffERegIn1Prob        =  fitRunDiffEGraphRegIn1->GetProb();


  TGraphErrors * runDiffEGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffERegOutVal,zeroRegOut,ediffERegOutVal);
  runDiffEGraphRegOut1->SetName("runDiffEGraphRegOut1");
  runDiffEGraphRegOut1->SetMarkerColor(kBlack);
  runDiffEGraphRegOut1->SetLineColor(kBlack);
  runDiffEGraphRegOut1->SetMarkerStyle(20);
  runDiffEGraphRegOut1->SetMarkerSize(0.5);
  runDiffEGraphRegOut1->SetLineWidth(1);
//   runDiffEGraphRegOut1->Draw("AP");
  runDiffEGraphRegOut1->Fit("fitRunDiffEGraphRegOut1","E M R F 0 Q W");
  fitRunDiffEGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffEGraphRegOut1->SetLineWidth(2);
  fitRunDiffEGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffERegOut1           =  fitRunDiffEGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffERegOut1Error      =  fitRunDiffEGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffERegOut1Chisquare  =  fitRunDiffEGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffERegOut1NDF        =  fitRunDiffEGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffERegOut1Prob       =  fitRunDiffEGraphRegOut1->GetProb();

  TMultiGraph *runDiffEGraphReg1 = new TMultiGraph();
  runDiffEGraphReg1->Add(runDiffEGraphRegIn1);
  runDiffEGraphReg1->Add(runDiffEGraphRegOut1);
  runDiffEGraphReg1->Draw("AP");
  runDiffEGraphReg1->SetTitle("");
  runDiffEGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffEGraphReg1->GetYaxis()->SetTitle("Energy Diff [ppm]");
  runDiffEGraphReg1->GetXaxis()->CenterTitle();
  runDiffEGraphReg1->GetYaxis()->CenterTitle();
  runDiffEGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffEGraphReg1->GetYaxis()->SetRangeUser(-0.0e-6,0.15e-6);
  TAxis *xaxisRunDiffEGraphReg1= runDiffEGraphReg1->GetXaxis();
  xaxisRunDiffEGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffEGraphRegIn1->Draw("same");
  fitRunDiffEGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffERegIn1 =(TPaveStats*)runDiffEGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffERegOut1=(TPaveStats*)runDiffEGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffERegIn1->SetTextColor(kBlue);
  statsRunDiffERegIn1->SetFillColor(kWhite); 
  statsRunDiffERegOut1->SetTextColor(kBlack);
  statsRunDiffERegOut1->SetFillColor(kWhite); 
  statsRunDiffERegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffERegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffERegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffERegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffERegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffERegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffERegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffERegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffERegInpOut1       =  (fitResultRunDiffERegIn1+fitResultRunDiffERegOut1)/2;
  Double_t resultRunDiffERegInmOut1       =  (fitResultRunDiffERegIn1-fitResultRunDiffERegOut1)/2;
  Double_t resultRunDiffERegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffERegIn1Error*fitResultRunDiffERegIn1Error+fitResultRunDiffERegOut1Error*fitResultRunDiffERegOut1Error)/2;
  Double_t resultRunDiffERegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffERegIn1Error*fitResultRunDiffERegIn1Error+fitResultRunDiffERegOut1Error*fitResultRunDiffERegOut1Error)/2;


  TLegend *legRunDiffEReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffEReg1->AddEntry("runDiffEGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffERegIn1,fitResultRunDiffERegIn1Error),"lp");
  legRunDiffEReg1->AddEntry("runDiffEGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffERegOut1,fitResultRunDiffERegOut1Error),"lp");
//   legRunDiffEReg1->AddEntry("runDiffEGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffERegInpOut1,resultRunDiffERegInpOut1Error),"");
//   legRunDiffEReg1->AddEntry("runDiffEGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffERegInmOut1,resultRunDiffERegInmOut1Error),"");
  legRunDiffEReg1->SetTextSize(0.045);
  legRunDiffEReg1->SetFillColor(0);
  legRunDiffEReg1->SetBorderSize(2);
  legRunDiffEReg1->Draw();

  gPad->Update();

  pad71->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,chargeRegInVal,zeroUnRegIn,echargeRegInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,chargeRegOutVal,zeroUnRegOut,echargeRegOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-20,20);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();

  ofstream outfileRegDiffAvgIn(Form("%s/plots/%s/summary%sRunletRegDiffAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRegDiffAvgIn << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
			      ,fitResultRunDiffXRegIn1,fitResultRunDiffXRegIn1Error
			      ,fitResultRunDiffXSlopeRegIn1,fitResultRunDiffXSlopeRegIn1Error
			      ,fitResultRunDiffYRegIn1,fitResultRunDiffYRegIn1Error
			      ,fitResultRunDiffYSlopeRegIn1,fitResultRunDiffYSlopeRegIn1Error
			      ,fitResultRunDiffERegIn1,fitResultRunDiffERegIn1Error
			      ,fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error)
			<<endl;
  outfileRegDiffAvgIn.close();

  ofstream outfileRegDiffAvgOut(Form("%s/plots/%s/summary%sRunletRegDiffAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRegDiffAvgOut << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
			       ,fitResultRunDiffXRegOut1,fitResultRunDiffXRegOut1Error
			       ,fitResultRunDiffXSlopeRegOut1,fitResultRunDiffXSlopeRegOut1Error
			       ,fitResultRunDiffYRegOut1,fitResultRunDiffYRegOut1Error
			       ,fitResultRunDiffYSlopeRegOut1,fitResultRunDiffYSlopeRegOut1Error
			       ,fitResultRunDiffERegOut1,fitResultRunDiffERegOut1Error
			       ,fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error)
			<<endl;
  outfileRegDiffAvgOut.close();

  c7->Update();
  c7->SaveAs(Form("%s/plots/%s/TargetDiffRegRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,regScheme));


  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/









  // Fit function to show 
  showFit1 = "FIT = Am*cos(phi) - C*sin(phi) + D";
  showFit2 = "FIT = Am*cos(phi + phi0) + C";
  showFit3 = "FIT = Am*sin(phi + phi0) + C";


  TString title1;
  TString titleSummary = Form("%s (%s, %s A): Regression-%s averages of MD %s asymmetries."
			      ,targ.Data(),polar.Data(),qtor_stem.Data(),reg_set.Data(),deviceTitle.Data());

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
  TString text = Form("%s",title1.Data());
  t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();


  Char_t  textfile[400],textfile3[400];
  sprintf(textfile,"dirPlot/resultText/%s_%s_%s_%s_MD_%s_regression_%s_%s_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data()); 
  sprintf(textfile3,"dirPlot/resultText/%s_%s_%s_%s_MD_%s_regression_%s_%s_asym_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
	  ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());
  Myfile3.open(textfile3);

//   printf("%sText File = %s\n%s",green,textfile,normal);

  const Int_t NUM = 100;

  int counter=0;
  ifstream transverseMDSen;
  Double_t zero[NUM],octant[NUM],valuesin[NUM],errorsin[NUM],valuesout[NUM],errorsout[NUM];


  transverseMDSen.open(Form("%s",textfile));
  // printf(Form("%s",textfile));
 
  if (transverseMDSen.is_open()) {
   
    printf("%s Det\tIn+-InError\tOut+-OutError\n%s",green,normal);
   
    while(!transverseMDSen.eof()) {
      zero[counter]=0;
      transverseMDSen >> octant[counter];
      transverseMDSen >> valuesin[counter];
      transverseMDSen >> errorsin[counter];
      transverseMDSen >> valuesout[counter];
      transverseMDSen >> errorsout[counter];
   
      if (!transverseMDSen.good()) break;

      printf("%s %4.0f\t%4.2f+-%4.2f\t%4.2f+-%4.2f\n%s"
	     ,green,octant[counter],valuesin[counter],errorsin[counter],valuesout[counter],errorsout[counter],normal);
 
      counter++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  transverseMDSen.close();


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
  grp_in->Fit("fit_in","B");
  TF1* fit1 = grp_in->GetFunction("fit_in");
  fit1->DrawCopy("same");
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
  TGraphErrors* grp_sum  = new TGraphErrors(k,x,valuesum,errx,valueerror);
  grp_sum ->SetName("grp_sum");
  grp_sum ->Draw("goff");
  grp_sum ->SetMarkerSize(markerSize[0]);
  //  grp_sum ->SetLineWidth(0);
  grp_sum ->SetMarkerStyle(22);
  grp_sum ->SetMarkerColor(kGreen-3);
  grp_sum->Fit("pol0","B");
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

  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.72);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.47);stats2->SetY2NDC(0.70);
  stats3->SetX1NDC(0.8); stats3->SetX2NDC(0.99); stats3->SetY1NDC(0.30);stats3->SetY2NDC(0.45);
  stats4->SetX1NDC(0.8); stats4->SetX2NDC(0.99); stats4->SetY1NDC(0.05);stats4->SetY2NDC(0.28);


  Double_t p0in     =  fit1->GetParameter(0);
  Double_t ep0in    =  fit1->GetParError(0);
  Double_t p1in     =  fit1->GetParameter(1);
  Double_t ep1in    =  fit1->GetParError(1);
  Double_t p2in     =  fit1->GetParameter(2);
  Double_t ep2in    =  fit1->GetParError(2);
  Double_t Chiin    =  fit1->GetChisquare();
  Double_t NDFin    =  fit1->GetNDF();
  Double_t Probin   =  fit1->GetProb();

  Double_t p0out    =  fit2->GetParameter(0);
  Double_t ep0out   =  fit2->GetParError(0);
  Double_t p1out    =  fit2->GetParameter(1);
  Double_t ep1out   =  fit2->GetParError(1);
  Double_t p2out    =  fit2->GetParameter(2);
  Double_t ep2out   =  fit2->GetParError(2);
  Double_t Chiout   =  fit2->GetChisquare();
  Double_t NDFout   =  fit2->GetNDF();
  Double_t Probout  =  fit2->GetProb();

  Double_t p0sum    =  fit3->GetParameter(0);
  Double_t ep0sum   =  fit3->GetParError(0);
  Double_t Chisum   =  fit3->GetChisquare();
  Double_t NDFsum   =  fit3->GetNDF();
  Double_t Probsum  =  fit3->GetProb();

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
  legend->AddEntry(grp_in,  Form("A_{IHWP-IN} = %4.2f #pm %4.2f ppm",p0in,ep0in), "p");
  legend->AddEntry(grp_sum, Form("A_{(IN+OUT)/2} = %4.2f #pm %4.2f ppm",p0sum,ep0sum), "p");
  legend->AddEntry(grp_out, Form("A_{IHWP-OUT} = %4.2f #pm %4.2f ppm",p0out,ep0out), "p");
  legend->AddEntry(grp_diff,Form("A_{measured} = %4.2f #pm %4.2f ppm",p0diff,ep0diff), "p");
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


  TString saveSummaryPlot = Form("dirPlot/%s_%s_%s_%s_MD_%s_regression_%s_%s_slug_summary_plots_%s"
				 ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data()
				 ,deviceName.Data(),reg_calc.Data(),reg_set.Data(),database_stem.Data());

  Canvas1->Update();
//   Canvas1->Print(saveSummaryPlot+".png");
  if(FIGURE){
    Canvas1->Print(saveSummaryPlot+".svg");
    Canvas1->Print(saveSummaryPlot+".C");
  }








  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


