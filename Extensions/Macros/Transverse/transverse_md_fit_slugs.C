//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : January 20, 2012
//*****************************************************************************************************//
/*
  This macro was adapted from slug_avrage_fit_regressed.C macro to plot only the main detector data.
  It will connect by default to qw_run2_pass1 data base and get the slug averages from the main detector
  regressed data and plot them in to three plots of in,out,in+out/2 , in-out and sum and difference 
  of oppsite octants.
  You have the option to change the database and the regression type at the command line. Just type:
  ./transverse_md_fit_slugs <db> [regression set] [color/B&W]
  e.g. 
  ./transverse_md_fit_slugs qw_run1_pass4 on_5+1
  ###############################################
  
  Slug averages of Main Detector Asymmetries
  
  ###############################################
  Enter target type:
  1. Liquid Hydrogen
  2. 4% DS Al
  3. 1.6% DS Carbon
  1
  Enter data type:
  1. Longitudinal
  2. Vertical Transverse
  3. Horizontal Transverse
  2
  Enter reaction type:
  1. elastic
  2. N->Delta
  ...
  ...
  

  To compile this code do a gmake.


  Updates:
  2012-02-23 B.Waidyawansa        Added the option to change database at the command line.
  2012-08-02 B.Waidyawansa        Added the option to change regression type at the command line
                                  Changes to formatting.
				  Added a check for NULL results.
				  Added opposite bar difference in addition to sums.
  2012-07-08 B.Waidyawansa        Added PMT based fits. 
                                  Added image saving as.eps 
  2012-08-08 B.Waidyawansa        Added	an option to draw B&W plots.
  2013-06-04 B.Waidyawansa        Replaced barsums with pmtaverages which are now calculated in pass5.
                                  Updated names of the plots.
  2014-06-25 B.Waidyawansa        Cleaned up obosolete codes and comments.
                                  Added  <cstdlib> library. It appears to be a gcc related issu. I have
				  gcc version 4.4.7 but still get compiler errors related to 'atoi' 
                                  and 'exit' functions. Adding cstdlib fixes these errors.
			          Added the date and time of analysis into the output text file.
                                  Enabled and option to select between using barsums vs pmtavg for the 
                                  detector asymmetries.
  2014-08-07 B.Waidyawansa        Added the option to change the fit. Now one can select between 1-parameter 
                                  (amplitude), 2-parameter (amplitude, constant) or 3-parameter (amplitude, 
                                  constant, phase offset).
                                  Changed the (IN+OUT)/2 fit from constant to dipole.

******************************************************************************************************/

using namespace std;

#include <iomanip>
#include <vector>
#include <iomanip>
#include <TRandom.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TString.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <TPaveStats.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TObject.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TText.h>
#include <TLatex.h>
#include <cstdlib>
#include <ctime>


TString pmt_avg[8]={"md1pmtavg","md2pmtavg","md3pmtavg","md4pmtavg","md5pmtavg","md6pmtavg","md7pmtavg","md8pmtavg"};
TString bar_sum[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum","qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

TString negative_pmt[8]=
  {"qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg"
   ,"qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg"};

TString positive_pmt[8]=
  {"qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos"
   ,"qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};


// DB and default settings
TSQLServer *db;
TString database;
TString reg_set = "on_5+1"; //default
Bool_t plot_colors = true;
TString color = "color";

// txt file to store values
std::ofstream Myfile;    

TString target, polar,targ;

// Default data selection options
Int_t bartype    = 1; // pmtavg
Bool_t barsums = false;
TString bars = "pmtavg";
Int_t target_opt = 1; // Hydrogen 
Int_t pol_opt    = 1; // Longitudinal
Int_t int_opt    = 1; // elastic
Int_t qtor_opt   = 4; // 8901A
Int_t fit_opt    = 3; // 3 parameter fit

TString good_for;
TString qtor_current;
TString qtor_stem;
TString good;
TString interaction;
			

Char_t textfile[100];

Double_t valuein[8] ={0.0};
Double_t errin[8] ={0.0};
Double_t valueout[8] ={0.0};
Double_t errout[8] ={0.0};

Double_t posvaluein[8] ={0.0};
Double_t poserrin[8] ={0.0};
Double_t posvalueout[8] ={0.0};
Double_t poserrout[8] ={0.0};

Double_t negvaluein[8] ={0.0};
Double_t negerrin[8] ={0.0};
Double_t negvalueout[8] ={0.0};
Double_t negerrout[8] ={0.0};


Double_t valuefalse[8]={0.0};
Double_t errorfalse[8]={0.0};
Double_t valuephys[8]={0.0};
Double_t errorphys[8]={0.0};


Double_t negvaluefalse[8]={0.0};
Double_t negerrorfalse[8]={0.0};
Double_t negvaluephys[8]={0.0};
Double_t negerrorphys[8]={0.0};
Double_t posvaluefalse[8]={0.0};
Double_t poserrorfalse[8]={0.0};
Double_t posvaluephys[8]={0.0};
Double_t poserrorphys[8]={0.0};



TString get_query(TString detector, TString measurement, TString ihwp);
void get_octant_data(TString devicelist[],TString ihwp, Double_t value[], Double_t error[]);
void plot_octant(Double_t valuesin[],Double_t errorsin[],
		 Double_t valuesout[],Double_t errorsout[],
		 Double_t valuesum[], Double_t errorsum[],
		 Double_t valuediff[], Double_t errodiff[],
		 TString detector
		 );
void plot_pmt(Double_t posvaluesin[],Double_t poserrorsin[],
	      Double_t posvaluesout[],Double_t poserrorsout[],
	      Double_t negvaluesin[],Double_t negerrorsin[],
	      Double_t negvaluesout[],Double_t negerrorsout[]
	      );
 
void get_opposite_octant_average( Double_t v_in_p_out[],Double_t e_in_p_out[],
				  Double_t v_in_m_out[],Double_t e_in_m_out[]);

int main(Int_t argc,Char_t* argv[])
{


  // Handle command line inputs for DB and.or regression type
  if(argc==2) database = argv[1];
  else if(argc==3){
    database = argv[1];
    reg_set  = argv[2];
  } 
  else if(argc==4){
    database = argv[1];
    reg_set  = argv[2];
    plot_colors =  atoi(argv[3]);
  }
  else {
    std::cout<<"Proper usage of this program is"<<std::endl;
    std::cout<<"./transverse_md_fit_slugs <db> [regression set] [color/B&W]"<<std::endl;
    std::cout<<"     db             : database name. e.g qw_run1_pass2"<<std::endl;
    std::cout<<"     regression_set : regression set out of on,on_5+1,on_set3, etc. Default os on_5+1"<<std::endl;
    std::cout<<"     color/b&w      : Color of the plots. Defauls it color(1). Use 0 to get B&W plots"<<std::endl;
    exit(1);
  }

  //connect to the data base
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");
  
  if(db){
    printf("Database server info : %s\n", db->ServerInfo());
  }
  else
    exit(1);
  
  if(!plot_colors) color="bnw";

  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nSlug averages of Main Detector Asymmetries \n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cout<<"3. 1.6% DS Carbon "<<std::endl;
  std::cin>>target_opt;

  std::cout<<"Enter data type:"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Vertical Transverse "<<std::endl;
  std::cout<<"3. Horizontal Transverse "<<std::endl;
  std::cin>>pol_opt;

  if(pol_opt!=1){
    std::cout<<"Enter the fit type:"<<std::endl;
    std::cout<<"1. Amplitude only fit. e.g. A Cos(phi)"<<std::endl;
    std::cout<<"2. Amplitude and Constant offset only. e.g A cos(phi) + C "<<std::endl;
    std::cout<<"3. Ampliutde, phase offset and constant offset. e.g A cos(phi+Phi_0)+C "<<std::endl;
    std::cin>>fit_opt;
  }


  std::cout<<"Enter intraction type:"<<std::endl;
  std::cout<<"1. elastic"<<std::endl;
  std::cout<<"2. N->Delta "<<std::endl;
  std::cout<<"3. DIS "<<std::endl;
  std::cin>>int_opt;

  if(int_opt==2){
    std::cout<<"Enter QTOR current:"<<std::endl;
    std::cout<<"1.6000 A "<<std::endl;
    std::cout<<"2.6700 A "<<std::endl;
    std::cout<<"3.7300 A "<<std::endl;
    std::cin>>qtor_opt;
  }


  
  // select barsums or pmtavg
  std::cout<<"User barsums or pmtavg to get detector asymmetries?:"<<std::endl;
  std::cout<<"1. pmtavg "<<std::endl;
  std::cout<<"2. barsums"<<std::endl;
  std::cin>>bartype;

  // set the target based on user input
  if(target_opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(target_opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 % Al";
  }
  else if(target_opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 % Carbon";
  }
  else{
    std::cout<<"Unknown target type!"<<std::endl;
    exit(1);
  }


  // Set the polarization based on user input
  if(pol_opt == 1){
    polar = "longitudinal";
    good = "3";
  }
  else if(pol_opt == 2){
    polar = "v_transverse";
    good = "8";
  }
  else if(pol_opt == 3){
    polar = "h_transverse";
    good = "9";
  }
  else{
    std::cout<<"Unknown polarization type!"<<std::endl;
    exit(1);
  }
  
  // Set the interaction based on user input
  if(int_opt == 1){
    good_for = "(md_data_view.good_for_id = '"+good+"' or md_data_view.good_for_id = '1,"+good+"')";
    interaction = "elastic";
    qtor_current=" and (slow_controls_settings.qtor_current>8800 && slow_controls_settings.qtor_current<9000) ";
    qtor_stem = "8901";
  }
  else if(int_opt == 2){
    good_for = "(md_data_view.good_for_id = '"+good+",18')";
    interaction = "n-to-delta"; 
  }
  else if(int_opt == 3){
    good_for = "(md_data_view.good_for_id = '1,3,21')";
    qtor_current=" and  (slow_controls_settings.qtor_current>8990 && slow_controls_settings.qtor_current<9010) ";
    qtor_stem = "9000";
    interaction = "dis"; 
  }
  else{
    std::cout<<"Unknown interaction type!"<<std::endl;
    exit(1);
  }

  // Set QTOR current cut based on user input
  if(qtor_opt == 1){
    qtor_current=" and  (slow_controls_settings.qtor_current>5900 && slow_controls_settings.qtor_current<6100) ";
    qtor_stem = "6000";
  }
  else if(qtor_opt == 2){
    qtor_current=" and  (slow_controls_settings.qtor_current>6600 && slow_controls_settings.qtor_current<6800) ";
    qtor_stem = "6700";
  }
  else if(qtor_opt == 3){
    qtor_current="  and (slow_controls_settings.qtor_current>7200 && slow_controls_settings.qtor_current<7400) ";
    qtor_stem = "7300";
  }
  
  // set barsum or pmtavg
  if(bartype==2) {
    barsums = true;
    bars = "barsums";
  }
  
  std::cout<<"###########################\n";
  std::cout<<"Getting error weighted averages of main detector "<<bars<<" "<<reg_set
	   <<" regressed asymmetries for "<<interaction<<" scattering from "
	   <<target<<" with "<<polar<<" polarization."<<std::endl;
  

  // Open the txt file to store data
  Char_t  textfile[400];
  sprintf(textfile,"%s_%s_%s_%s_MD_regressed_%s_in_out_values_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database.Data()); 
  Myfile.open(textfile);

  // Analysis time
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  printf ("Current local time and date: %s", asctime(timeinfo));

  Myfile << " !========================================"<<std::endl;
  Myfile << " !Date of analysis :-"<<asctime(timeinfo)<<std::endl;
  Myfile << " !Input parameters :-                     "<<std::endl;
  Myfile << " !   MDasyms from : "<<bars                <<std::endl;
  Myfile << " !   DB used      : "<<database            <<std::endl;
  Myfile << " !   Regression   : "<<reg_set             <<std::endl;
  Myfile << " !   Target       : "<<target              <<std::endl;
  Myfile << " !   Interaction  : "<<interaction         <<std::endl;
  Myfile << " !   Polarization : "<<polar               <<std::endl;
  Myfile << " !   QTOR cut     : "<<qtor_stem           <<std::endl;
  Myfile << " !========================================"<<std::endl;


 
  // Start application
  TApplication theApp("App",&argc,argv);
 
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatFormat("4.3f");
  gStyle->SetFitFormat("4.3f");

  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.08,"x");
  gStyle->SetLabelSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"x");
  gStyle->SetTitleSize(0.08,"y");
  gStyle->SetTitleOffset(1.0,"x");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);

  // Font style set to times new roman 12 pt.
  gStyle->SetLabelFont(132,"X");  
  gStyle->SetLabelFont(132,"Y"); 
  gStyle->SetTitleFont(132,"X");  
  gStyle->SetTitleFont(132,"Y");  
  gStyle->SetStatFont(132);  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");



  //************************
  // Draw detector plots
  //************************

  std::cout<<"Draw plots..\n";
 //Set canvas parameters
  TString title1;
  if(pol_opt==1) 
    title1 = Form("%s (%s): Averages of regressd %s asymmetries. FIT = A_{V}*cos(phi) - A_{H}*sin(#phi) + C",targ.Data(),polar.Data(),bars.Data());
  else if(pol_opt==2) {
    if(fit_opt==1)
      title1= Form("%s (%s): Averages of regressed %s asymmetries. FIT = A_{V}*cos(#phi)",targ.Data(),polar.Data(),bars.Data());
    else if(fit_opt==2)
      title1= Form("%s (%s): Averages of regressed %s asymmetries. FIT = A_{V}*cos(#phi) + C",targ.Data(),polar.Data(),bars.Data());
  }
  else
    title1= Form("%s (%s): Averages of regressed %s asymmetries. FIT = A_{H}*sin(#phi + #phi_{0}) + C",targ.Data(),polar.Data(),bars.Data());

  TCanvas * Canvas1 = new TCanvas("Canvas1", title1,0,0,1000,500);
  Canvas1->Draw();
  Canvas1->SetFillColor(0);
  Canvas1->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.945,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title1);
  TLatex*t1 = new TLatex(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->SetTextFont(132);

  t1->Draw();

  pad2->cd();
  Myfile << "\n !======================================="<<std::endl;
  Myfile << " ! Main detector error weighted averages "<<std::endl;
  Myfile << " !========================================"<<std::endl;

  if(barsums){
    get_octant_data(bar_sum,"in", valuein,  errin);
    get_octant_data(bar_sum,"out",  valueout, errout);
  } else{ //pmtavg
    get_octant_data(pmt_avg,"in", valuein,  errin);
    get_octant_data(pmt_avg,"out",  valueout, errout);
  }

  std::cout<<"\nDraw IN+OUT, AVG(IN,-OUT)\n";
  plot_octant(valuein,errin,valueout,errout,valuefalse,errorfalse,valuephys, errorphys,bars);
  gPad->Update();

  if(!plot_colors) Canvas1->SetGrayscale();
  Canvas1->Modified();
  Canvas1->Update();
  Canvas1->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_in_p_out_plots_"+database+"_"+color+Form("_%iparam_fit.png",fit_opt));
  Canvas1->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_in_p_out_plots_"+database+"_"+color+Form("_%iparam_fit.svg",fit_opt));
  Canvas1->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_in_p_out_plots_"+database+"_"+color+Form("_%iparam_fit.C",fit_opt));

  // Calculate sum of opposite octants
  get_opposite_octant_average(valuefalse,errorfalse,valuephys,errorphys);
  
  if(!barsums){

    std::cout<<"##################################\n";
    std::cout<<"Plot positive and negative PMT asymmetries"<<std::endl;

    //************************
    // Draw individual PMT plots 
    //************************
    Myfile << "\n !======================================="<<std::endl;
    Myfile << " ! Positive PMT error weighted averages "<<std::endl;
    Myfile << " !========================================"<<std::endl;
    get_octant_data(negative_pmt,"in", negvaluein,  negerrin);
    get_octant_data(negative_pmt,"out",  negvalueout, negerrout);
    
    Myfile << "\n !======================================="<<std::endl;
    Myfile << " ! Negative PMT error weighted averages "<<std::endl;
    Myfile << " !========================================"<<std::endl;
    
    get_octant_data(positive_pmt,"in", posvaluein,  poserrin);
    get_octant_data(positive_pmt,"out",  posvalueout, poserrout);
    
    plot_pmt(posvaluein,poserrin,posvalueout,poserrout,
	     negvaluein,negerrin,negvalueout,negerrout);
  
  }

  Myfile.close();

  std::cout<<"Done plotting!\n";
  db->Close();
  delete db;

  theApp.Run();
  return(1);

};

//***************************************************
//***************************************************
//  Function to create the DB query              
//***************************************************
//***************************************************
TString get_query(TString detector, TString measurement, TString ihwp){

  
  Bool_t ldebug = false;
  TString datatable;

  datatable = "md_data_view";

  // std::cout<<"Getting regressed data for "<<detector<<std::endl;

  TString output = " IFNULL(sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))),0), IFNULL(SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2))))),0)";
  
  // select only good runs (for now)
  TString run_quality =  Form("(%s.run_quality_id = '1') ",datatable.Data());

  TString regression;

  if(reg_set.Contains("off"))
    regression = Form("%s.slope_calculation = 'on' and %s.slope_correction = '%s' ",
			    datatable.Data(),datatable.Data(),reg_set.Data()); 
  else
    regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = '%s' ",
			    datatable.Data(),datatable.Data(),reg_set.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = '"+target+"' AND "
    +regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0 "+qtor_current+"; ";

  if(ldebug) std::cout<<query<<std::endl;

  Myfile<<" ! DB query used : - "<<std::endl;
  Myfile<<query                <<std::endl;

  return query;
}


//***************************************************
//***************************************************
//  Function to get all the octant data                  
//***************************************************
//***************************************************

void get_octant_data(TString devicelist[],TString ihwp, Double_t value[], Double_t error[])
{
  Bool_t ldebug = false;
  Myfile<<"! IHWP : - "<<ihwp<<std::endl;

  for(Int_t i=0 ; i<8 ;i++){
    std::cout<<"Getting data for "<<ihwp<<" of "<< devicelist[i].Data()<<std::endl;

    Myfile<<"! detector : "<<setw(10)<<devicelist[i]<<std::endl;
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",ihwp);
    TSQLStatement* stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }

    // process statement
    if (stmt->Process()) {
      
      // store result of statement in buffer
      stmt->StoreResult();
      while (stmt->NextResultRow()) {
	value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	if(ldebug) printf(" value = %e +- %10.8lf [ppm]\n", value[i], error[i]);
	if(value[i] == 0 ) {
	  std::cout<<"\nQuery returned NULL values for "<<devicelist[i]<<". Check to see if the DB is correct or you enetered the correct data selection options! Exiting .."<<std::endl;
	  exit(1);
	}
	Myfile<<setw(10)<<devicelist[i]<<": "<<value[i]<<"+-"<<setw(10)<<error[i]<<std::endl;
	
      }
    }
    delete stmt;    
  }
}

//***************************************************
//***************************************************
//  Function to plot  multi graphs for asym vs octants                   
//***************************************************
//***************************************************

void plot_octant(Double_t valuesin[],Double_t errorsin[],
		 Double_t valuesout[],Double_t errorsout[],
		 Double_t valuesum[], Double_t errorsum[],
		 Double_t valuediff[], Double_t errordiff[],
		 TString detector
		 )  
{

  gStyle->SetOptFit(1111);

  const int k =8;  
  Double_t x[k];
  Double_t errx[k];
  
  for(Int_t i =0;i<k;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }

  TPad* pad = (TPad*)(gPad->GetMother());

  // Take the average over the in and out half wave plate values. 
  // Here we take just the average and not the weighted average because we are testing a hypothesis
  // IN+OUT=? 0. Since we are not sure if IN ~ OUT we can't consider them as a measurement of the
  // same value to do the weighted average.

  // Take the weighted difference in the IHWP in and out half wave plate values.
  // Once we know that IN+OUT ~ 0 we know that IN and OUT are a measurement of the same thing
  // so we can take the weighted error difference when we take IN-OUT.

  Myfile<<" !=================================="<<std::endl;
  Myfile<<" !(IN+OUT)/2 and AVG(IN-OUT) values "<<std::endl;
  for(Int_t i =0;i<k;i++){

    valuesum[i]=(valuesin[i]+valuesout[i])/2.0;
    errorsum[i]= (sqrt(pow(errorsin[i],2)+pow(errorsout[i],2)))/2.0;
    
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));

    std::cout<<"\n detector "<<i+1<<std::endl;
    std::cout<<" IN          = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesin[i]  
	     <<setw(4)<<" +- "<<errorsin[i] <<std::endl;
    std::cout<<" OUT         = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesout[i] 
	     <<setw(4)<<" +- "<<errorsout[i]<<std::endl;
    std::cout<<" (IN+OUT)/2  = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesum[i]  
	     <<setw(4)<<" +- "<<errorsum[i] <<std::endl;
    std::cout<<" AVG(IN,-OUT) = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuediff[i] 
	     <<setw(4)<<" +- "<<errordiff[i] <<std::endl;


    Myfile<<"\n detector "<<i+1<<std::endl;
    Myfile<<" IN          = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesin[i]  
	  <<setw(4)<<" +- "<<errorsin[i] <<std::endl;
    Myfile<<" OUT         = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesout[i] 
	  <<setw(4)<<" +- "<<errorsout[i]<<std::endl;
    Myfile<<" (IN+OUT)/2  = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesum[i] 
	  <<setw(4)<<" +- "<<errorsum[i] <<std::endl;
    Myfile<<" AVG(IN,-OUT) = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuediff[i] 
	  <<setw(4)<<" +- "<<errordiff[i] <<std::endl;

  }


  // Fits for the data
  TF1 *fit_in;
  if(pol_opt==1) {
    fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]",1,8);
    fit_in->SetParName(0,"A_{V}^{IN}");
    fit_in->SetParName(1,"A_{H}^{IN}");
    fit_in->SetParName(2,"C^{IN}");
  }
  else if(pol_opt==2) {
    if(fit_opt==1){
      fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1)))",1,8);
      fit_in->SetParName(0,"A_{V}^{IN}");
    }
    else if(fit_opt==2){
      fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_in->SetParName(0,"A_{V}^{IN}");
      fit_in->SetParName(1,"C^{IN}");
    }
    else{
      fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1)+[1])) + [2]",1,8);
      fit_in->SetParName(0,"A_{V}^{IN}");
      fit_in->SetParName(1,"#phi_{0}^{IN}");
      fit_in->SetParName(2,"C^{IN}");
      fit_in->SetParameter(1,0);
    }
  }
  else {
    if(fit_opt==1){
      fit_in = new TF1("fit_in","[0]*sin((pi/180)*(45*(x-1)))",1,8);
      fit_in->SetParName(0,"A_{H}^{IN}");
    }
    else if(fit_opt==2){
      fit_in = new TF1("fit_in","[0]*sin((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_in->SetParName(0,"A_{H}^{IN}");
      fit_in->SetParName(1,"C^{IN}");
    }
    else{
      fit_in = new TF1("fit_in","[0]*sin((pi/180)*(45*(x-1)+[1])) + [2]",1,8);
      fit_in->SetParName(0,"A_{H}^{IN}");
      fit_in->SetParName(1,"#phi_{0}^{IN}");
      fit_in->SetParName(2,"C^{IN}");
      fit_in->SetParameter(1,0);
    }
  }


  TF1 *fit_out;
  if(pol_opt==1){
    fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]",1,8);
    fit_out->SetParName(0,"A_{V}^{OUT}");
    fit_out->SetParName(1,"A_{H}^{OUT}");
    fit_out->SetParName(2,"C^{OUT}"); 
  }
  else if(pol_opt==2){
    if(fit_opt==1){
      fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1)))",1,8);
      fit_out->SetParName(0,"A_{V}^{OUT}");
    }
    else if(fit_opt ==2){
      fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_out->SetParName(0,"A_{V}^{OUT}");
      fit_out->SetParName(1,"C^{OUT}");
    } 
    else{
      fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1)+[1])) + [2]",1,8);
      fit_out->SetParName(0,"A_{V}^{OUT}");
      fit_out->SetParName(1,"#phi_{0}^{OUT}");
      fit_out->SetParName(2,"C^{OUT}");
      fit_out->SetParameter(1,0);
    }
  }
  else{
    if(fit_opt==1){
      fit_out = new TF1("fit_out","[0]*sin((pi/180)*(45*(x-1)))",1,8);
      fit_out->SetParName(0,"A_{H}^{OUT}");
    }
    else if(fit_opt==2){
      fit_out = new TF1("fit_out","[0]*sin((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_out->SetParName(0,"A_{H}^{OUT}");
      fit_out->SetParName(1,"C^{OUT}");
    }
    else{
      fit_out = new TF1("fit_out","[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
      fit_out->SetParName(0,"A_{H}^{IN}");
      fit_out->SetParName(1,"#phi_{0}^{IN}");
      fit_out->SetParName(2,"C^{IN}");
      fit_out->SetParameter(1,0);
    }
  }

  // Initialize fits
  fit_in->SetParameter(0,0);
  fit_out->SetParameter(0,0);

  // Create a multigraph
  TMultiGraph * grp = new TMultiGraph();

  // Draw IN values
  std::cout<<"Draw and fit IN data\n";
  TGraphErrors* grp_in  = new TGraphErrors(k,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(1.0);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->Fit("fit_in","QEM");
  TF1* fit1 = grp_in->GetFunction("fit_in");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
  fit1->SetLineWidth(3);
  fit1->SetLineStyle(5);
  grp->Add(grp_in);

  // Draw OUT values
  std::cout<<"Draw and fit OUT data\n";
  TGraphErrors* grp_out  = new TGraphErrors(k,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(1.0);
  grp_out ->SetMarkerStyle(23);
  grp_out ->SetMarkerColor(kRed);
  grp_out->Fit("fit_out","QEM");
  TF1* fit2 = grp_out->GetFunction("fit_out");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);
  fit2->SetLineWidth(3);
  fit2->SetLineStyle(5);
  grp->Add(grp_out);

  // Draw (IN+OUT)/2 values
  std::cout<<"Draw and fit (IN+OUT)/2 data\n";
  TGraphErrors* grp_sum  = new TGraphErrors(k,x,valuesum,errx,errorsum);
  grp_sum ->SetMarkerSize(1.0);
  grp_sum ->SetMarkerStyle(20);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum ->Fit("fit_in","QEM");
  TF1* fit3 = grp_sum->GetFunction("fit_in");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen+3);
  fit3->SetParName(0,"A^{(IN+OUT)/2}");
  fit3->SetLineWidth(3);
  fit3->SetLineStyle(3);
  grp->Add(grp_sum);

  grp->Draw("AP");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant Number");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",detector.Data()));
  grp->GetYaxis()->CenterTitle();
  pad->Update();

  TPaveStats *stats1 = (TPaveStats*)(grp_in->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)(grp_out->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)(grp_sum->GetListOfFunctions())->FindObject("stats");

  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 
  
  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
  
  stats3->SetTextColor(kGreen+3);
  stats3->SetFillColor(kWhite); 
  
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);


  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IN", "p");
  legend->AddEntry(grp_out, "OUT", "p");
  legend->AddEntry(grp_sum, "(IN+OUT)/2", "p");
  legend->SetTextFont(132);
  legend->SetFillColor(0);
  legend->Draw("");

    
  // Difference over the in and out half wave plate values
  TString title;
  
  if(pol_opt==1)
    title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries.  FIT = A_{V}*cos(phi) - A_{H}*sin(#phi) +  C";
  else if(pol_opt==2){ 
    if(fit_opt==1)  
      title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries. FIT = A_{V}*cos(#phi)";
    else if(fit_opt==2)
      title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries. FIT = A_{V}*cos(#phi) + C";
    else
      title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries. FIT = A_{V}*cos(#phi + #phi_{0}) + C";
  }
  else{
    if(fit_opt==1)  
      title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries. FIT = A_{V}*sin(#phi)";
    else if(fit_opt==2)
      title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries. FIT = A_{V}*sin(#phi) + C";
    else
      title = targ+"("+polar+"): AVG(IN,-OUT) of regressed MD asymmetries. FIT = A_{V}*sin(#phi + #phi_{0}) + C";
  }

  TCanvas * Canvas11 = new TCanvas("Canvas11",title,0,0,1000,500);
  Canvas11->Draw();
  Canvas11->SetFillColor(0);
  Canvas11->cd();
  
  TPad*pad11 = new TPad("pad11","pad11",0.005,0.940,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.940);
  pad11->SetFillColor(0);
  pad11->Draw();
  pad22->Draw();
  
  
  pad11->cd();
  TLatex*t1 = new TLatex(0.06,0.3,title);
  t1->SetTextSize(0.68);
  t1->SetTextFont(132);
  t1->Draw();
  
  pad22->cd();
  pad22->SetFillColor(0);
  
  // Draw AVG(In,-Out)
  std::cout<<"Draw and fit AVG(IN,-OUT) data\n";

  pad22->SetRightMargin(0.08);
  pad22->SetTopMargin(0.05);

  TGraphErrors* grp_diff  = new TGraphErrors(k,x,valuediff,errx,errordiff);
  grp_diff ->SetMarkerSize(0.8);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta);
  grp_diff ->Fit("fit_in","QEM");
  
  TF1* fit4 = grp_diff->GetFunction("fit_in");
  fit4->DrawCopy("same");
  fit4->SetLineColor(kViolet+5);
  fit4->SetLineWidth(3);
  fit4->SetLineStyle(5);
  fit4->SetParName(2,"C");
  if(pol_opt==1) {
    fit4->SetParName(0,"A_{V}");
    fit4->SetParName(1,"A_{H}");
  } 
  else if(pol_opt==2) {
    if(fit_opt==1)
      fit4->SetParName(0,"A_{V}");
    else if(fit_opt==2){
      fit4->SetParName(0,"A_{V}");
      fit4->SetParName(1,"C_{V}");
    }
    else{
      fit4->SetParName(0,"A_{V}");
      fit4->SetParName(1,"#phi_{0}^{V}");
      fit4->SetParName(2,"C_{V}");
      fit4->SetParameter(1,0);
    }
  }
  else{
    if(fit_opt==1)
      fit4->SetParName(0,"A_{H}");
    else if(fit_opt==2){
      fit4->SetParName(0,"A_{H}");
      fit4->SetParName(1,"C_{H}");
    }
    else{
      fit4->SetParName(0,"A_{H}");
      fit4->SetParName(1,"#phi_{0}^{H}");
      fit4->SetParName(2,"C_{H}");
      fit4->SetParameter(1,0);
    }
  }

  TMultiGraph * grp1 = new TMultiGraph();
  grp1->Add(grp_diff);
  grp1->Draw("AP");
  
  grp1->SetTitle("");
  grp1->GetXaxis()->SetTitle("Octant");
  grp1->GetXaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp1->GetYaxis()->CenterTitle();
  
  
  TPaveStats *stats11 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");
  stats11->SetTextColor(kBlack);
  stats11->SetFillColor(kWhite);
  stats11->SetFitFormat("4.3f");

  stats11->SetX1NDC(0.1); 
  stats11->SetX2NDC(0.35); 
  stats11->SetY1NDC(0.65);
  stats11->SetY2NDC(0.98);  
  
  if(!plot_colors) Canvas11->SetGrayscale();
  Canvas11-> Modified();
  Canvas11-> Update();
  
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.png",fit_opt));
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.svg",fit_opt));
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.C",fit_opt));
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+bars+"_regressed_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.eps",fit_opt));
  
  
}




//***************************************************
//***************************************************
//    Function to get opposite octant sum and diffs                
//***************************************************
//***************************************************

void get_opposite_octant_average( Double_t v_in_p_out[],Double_t e_in_p_out[],
				  Double_t v_in_m_out[],Double_t e_in_m_out[])  
{
  
  gStyle->SetOptFit(0);


  Double_t valuesum_in_p_out[4]={0};
  Double_t errorsum_in_p_out[4]={0};  
  Double_t valuediff_in_p_out[4]={0};
  Double_t errordiff_in_p_out[4]={0};
  Double_t valuesum_in_m_out[4]={0};
  Double_t errorsum_in_m_out[4]={0};  
  Double_t valuediff_in_m_out[4]={0};
  Double_t errordiff_in_m_out[4]={0};
  Double_t x[4]={1,2,3,4};
  Double_t errx[4]={0};

  std::cout<<"###########################################\n";
  std::cout<<"Sum and difference of detectors in opposite octants\n"<<std::endl;
 
  for(Int_t i =0;i<4;i++){
    std::cout<<"bar"<<i+1<<" + bar"<<i+5<<std::endl;
    valuesum_in_p_out[i]= v_in_p_out[i]+v_in_p_out[i+4];
    errorsum_in_p_out[i]= sqrt(pow(e_in_p_out[i],2)+pow(e_in_p_out[i+4],2));

    valuesum_in_m_out[i]= v_in_m_out[i]+v_in_m_out[i+4];
    errorsum_in_m_out[i]= sqrt(pow(e_in_m_out[i],2)+pow(e_in_m_out[i+4],2));

    valuediff_in_p_out[i]= v_in_p_out[i]-v_in_p_out[i+4];
    errordiff_in_p_out[i]= sqrt(pow(e_in_p_out[i],2)+pow(e_in_p_out[i+4],2));

    valuediff_in_m_out[i]= v_in_m_out[i]-v_in_m_out[i+4];
    errordiff_in_m_out[i]= sqrt(pow(e_in_m_out[i],2)+pow(e_in_m_out[i+4],2));

    std::cout<<" (IN+OUT)/2  (bar"<<i+1<<" + bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesum_in_p_out[i]  <<setw(4)<<" +- "<< errorsum_in_p_out[i] <<std::endl;
    std::cout<<" (IN+OUT)/2  (bar"<<i+1<<" - bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuediff_in_p_out[i]  <<setw(4)<<" +- "<< errordiff_in_p_out[i] <<std::endl;
    std::cout<<" AVG(IN,-OUT) (bar"<<i+1<<" - bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuediff_in_m_out[i]  <<setw(4)<<" +- "<< errordiff_in_m_out[i] <<std::endl;
    std::cout<<" AVG(IN,-OUT) (bar"<<i+1<<" + bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesum_in_m_out[i]  <<setw(4)<<" +- "<< errorsum_in_m_out[i] <<std::endl;

   }


  Myfile<<" \n!=================================="<<std::endl;
  Myfile<<" ! Sum of opposite octants for (IN+OUT)/2 and AVG(IN-OUT) "<<std::endl;
  for(Int_t i =0;i<4;i++)
      Myfile<<" (IN+OUT)/2  (bar"<<i+1<<" + bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesum_in_p_out[i]  <<setw(4)<<" +- "<< errorsum_in_p_out[i] <<std::endl;
  for(Int_t i =0;i<4;i++)
     Myfile<<" AVG(IN,-OUT) (bar"<<i+1<<" + bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuesum_in_m_out[i]  <<setw(4)<<" +- "<< errorsum_in_m_out[i] <<std::endl;

  
  Myfile<<" \n!=================================="<<std::endl;
  Myfile<<" ! Difference of opposite octants for (IN+OUT)/2 and AVG(IN-OUT) "<<std::endl;
  for(Int_t i =0;i<4;i++)
    Myfile<<" (IN+OUT)/2  (bar"<<i+1<<" - bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuediff_in_p_out[i]  <<setw(4)<<" +- "<< errordiff_in_p_out[i] <<std::endl;
  for(Int_t i =0;i<4;i++)
    Myfile<<" AVG(IN+OUT) (bar"<<i+1<<" - bar"<<i+5<<") = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<valuediff_in_m_out[i]  <<setw(4)<<" +- "<< errordiff_in_m_out[i] <<std::endl;


  TString title1 = targ+"("+polar+"): Opposite octant sums of main detectors";
  TString title2 = targ+"("+polar+"): Opposite octant difference of main detectors";

  TCanvas * Canvas = new TCanvas("Canvas","Opposite bar sums/differences",0,0,1000,800);
  Canvas->Draw();
  Canvas->SetFillColor(0);
  Canvas->cd();
  
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.945,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TLatex*t1 = new TLatex(0.06,0.3,title1);
  t1->SetTextSize(0.5);
  t1->SetTextFont(132);
  t1->Draw();


  pad2->Divide(1,2);
  pad2->cd(1);
  gPad->SetRightMargin(0.08);

  TGraphErrors* grp_sum_in_p_out  = new TGraphErrors(4,x,valuesum_in_p_out,errx,errorsum_in_p_out);
  grp_sum_in_p_out  ->SetMarkerSize(1.2);
  grp_sum_in_p_out  ->SetMarkerStyle(21);
  grp_sum_in_p_out  ->SetMarkerColor(kGreen-2);
  grp_sum_in_p_out  ->Fit("pol0","QEM");
  TF1* fit1 = grp_sum_in_p_out->GetFunction("pol0");
  fit1->SetLineColor(kGreen-2);
  fit1->SetLineWidth(2);
  fit1->SetLineStyle(3);

  TGraphErrors* grp_sum_in_m_out   = new TGraphErrors(4,x,valuesum_in_m_out ,errx,errorsum_in_m_out );
  grp_sum_in_m_out ->SetMarkerSize(1.2);
  grp_sum_in_m_out ->SetMarkerStyle(22);
  grp_sum_in_m_out ->SetMarkerColor(kMagenta+1);
  grp_sum_in_m_out->Fit("pol0","QEM");
  TF1* fit2 = grp_sum_in_m_out->GetFunction("pol0");
  fit2->SetLineColor(kMagenta+1);
  fit2->SetLineWidth(2);
  fit2->SetLineStyle(3);
 
  TMultiGraph * grp_sum = new TMultiGraph();
  grp_sum->Add(grp_sum_in_p_out);
  grp_sum->Add(grp_sum_in_m_out);
  grp_sum->Draw("AP");

  grp_sum->SetTitle("");
  grp_sum->GetXaxis()->SetTitle("Octant+Opposite Octant");
  grp_sum->GetXaxis()->CenterTitle();
  grp_sum->GetYaxis()->SetTitle(" ppm ");
  grp_sum->GetYaxis()->CenterTitle();

  TLegend *legend = new TLegend(0.1,0.83,0.6,0.99,"","brNDC");
  legend->AddEntry(grp_sum_in_p_out, Form("(IN+OUT)/2 fit [%2.2f +- %2.2f ppm][Min.Chi.=%1.1f]",fit1->GetParameter(0),fit1->GetParError(0), (fit1->GetChisquare()/fit1->GetNDF())), "p");
  legend->AddEntry(grp_sum_in_m_out, Form("AVG(IN-OUT) fit [%2.2f +- %2.2f ppm][Min.Chi.=%1.1f]",fit2->GetParameter(0),fit2->GetParError(0), (fit2->GetChisquare()/fit2->GetNDF())), "p");
  legend->SetFillColor(0);
  legend->SetTextFont(132);
  legend->SetBorderSize(1);
  legend->Draw("");

  pad2->cd(2);
  gPad->SetRightMargin(0.08);

  TGraphErrors* grp_diff_in_p_out  = new TGraphErrors(4,x,valuediff_in_p_out,errx,errordiff_in_p_out);
  grp_diff_in_p_out  ->SetMarkerSize(1.2);
  grp_diff_in_p_out  ->SetMarkerStyle(21);
  grp_diff_in_p_out  ->SetMarkerColor(kGreen-2);
  grp_diff_in_p_out  ->Fit("pol0","QEM");
  TF1* fit3 = grp_diff_in_p_out->GetFunction("pol0");
  fit3->SetLineColor(kGreen-2);
  fit3->SetLineWidth(2);
  fit3->SetLineStyle(3);
 

  TGraphErrors* grp_diff_in_m_out   = new TGraphErrors(4,x,valuediff_in_m_out ,errx,errordiff_in_m_out );
  grp_diff_in_m_out ->SetMarkerSize(1.2);
  grp_diff_in_m_out ->SetMarkerStyle(22);
  grp_diff_in_m_out ->SetMarkerColor(kMagenta+1);
 
  TMultiGraph * grp_diff = new TMultiGraph();
  grp_diff->Add(grp_diff_in_p_out);
  grp_diff->Add(grp_diff_in_m_out);
  grp_diff->Draw("AP");

  grp_diff->SetTitle("");
  grp_diff->GetXaxis()->SetTitle("Octant - Opposite Octant");
  grp_diff->GetXaxis()->CenterTitle();
  grp_diff->GetYaxis()->SetTitle(" ppm ");
  grp_diff->GetYaxis()->CenterTitle();


  TLegend *legend1 = new TLegend(0.1,0.83,0.6,0.99,"","brNDC");
  legend1->AddEntry(grp_diff_in_p_out, Form("(IN+OUT)/2 fit [%2.2f +- %2.2f ppm] (Chi.D.F = %1.1f)", fit3->GetParameter(0),fit3->GetParError(0), (fit3->GetChisquare()/fit3->GetNDF())), "p");
  legend1->AddEntry(grp_diff_in_m_out, "AVG(IN-OUT)", "p");
  legend1->SetFillColor(0);
  legend1->SetTextFont(132);
  legend1->SetBorderSize(1);
  legend1->Draw("");


  if(!plot_colors) Canvas->SetGrayscale();
  Canvas->Modified();
  Canvas-> Update();
  Canvas->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_regressed_"+bars+"_opposite_octant_plots_"+reg_set+"_"+database+"_"+color+Form("_%iparam_fit.png",fit_opt));
  Canvas->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_regressed_"+bars+"_opposite_octant_plots_"+reg_set+"_"+database+"_"+color+Form("_%iparam_fit.svg",fit_opt));
  Canvas->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_regressed_"+bars+"_opposite_octant_plots_"+reg_set+"_"+database+"_"+color+Form("_%iparam_fit.C",fit_opt));
  
}

//***************************************************
//***************************************************
//  Function to plot  multi graphs for PMT asymmetries                   
//***************************************************
//***************************************************

void plot_pmt(Double_t posvaluesin[],Double_t poserrorsin[],
	      Double_t posvaluesout[],Double_t poserrorsout[],
	      Double_t negvaluesin[],Double_t negerrorsin[],
	      Double_t negvaluesout[],Double_t negerrorsout[]
	      )  
{
  


  gStyle->SetOptFit(1111);
  TString title1;
  if(pol_opt==1) 
    title1 = Form("%s (%s): Averages of regressd PMT asymmetries. FIT = A_{V}*cos(phi) - A_{H}*sin(#phi) + C",targ.Data(),polar.Data());
  else if(pol_opt==2) {
    if(fit_opt==1)
      title1= Form("%s (%s): Averages of regressed PMT asymmetries. FIT = A_{V}*cos(#phi)",targ.Data(),polar.Data());
    else if(fit_opt==2)
      title1= Form("%s (%s): Averages of regressed PMT asymmetries. FIT = A_{V}*cos(#phi) + C",targ.Data(),polar.Data());
    else
      title1= Form("%s (%s): Averages of regressed PMT asymmetries. FIT = A_{V}*cos(#phi + #phi_{0}) + C",targ.Data(),polar.Data());
  }
  else{
    if(fit_opt==1)
      title1= Form("%s (%s): Averages of regressed PMT asymmetries. FIT = A_{V}*cos(#phi)",targ.Data(),polar.Data());
    else if(fit_opt==2)
      title1= Form("%s (%s): Averages of regressed PMT asymmetries. FIT = A_{V}*sin(#phi) + C",targ.Data(),polar.Data());
    else
      title1= Form("%s (%s): Averages of regressed PMT asymmetries. FIT = A_{V}*sin(#phi + #phi_{0}) + C",targ.Data(),polar.Data());
  }
  TCanvas * Canvas2 = new TCanvas("Canvas2", title1,0,0,1600,800);
  Canvas2->Draw();
  Canvas2->SetFillColor(0);
  Canvas2->cd();
  
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.940,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.940);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TLatex*t1 = new TLatex(0.06,0.3,title1);
  t1->SetTextSize(0.68);
  t1->SetTextFont(132);
  t1->Draw();

  pad2->cd();

  const int k =8;  
  Double_t x[k];
  Double_t errx[k];

  Double_t  posvaluesum[8]={0};
  Double_t  poserrorsum[8]={0};
  Double_t  negvaluesum[8]={0};
  Double_t  negerrorsum[8]={0};
  Double_t  posvaluediff[8]={0};
  Double_t  poserrordiff[8]={0};
  Double_t  negvaluediff[8]={0};
  Double_t  negerrordiff[8]={0};


  
  for(Int_t i =0;i<k;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }

  // Take the average over the in and out half wave plate values. 
  // Here we take just the average and not the weighted average because we are testing a hypothesis
  // IN+OUT=? 0. Since we are not sure if IN ~ OUT we can't consider them as a measurement of the
  // same value to do the weighted average.

  // Take the weighted difference in the IHWP in and out half wave plate values.
  // Once we know that IN+OUT ~ 0 we know that IN and OUT are a measurement of the same thing
  // so we can take the weighted error difference when we take IN-OUT.
 

  Myfile<<" !=================================="<<std::endl;
  Myfile<<" !POS PMT (IN+OUT)/2 and AVG(IN-OUT) values "<<std::endl;
  for(Int_t i =0;i<k;i++){

    posvaluesum[i]=(posvaluesin[i]+posvaluesout[i])/2.0;
    poserrorsum[i]= (sqrt(pow(poserrorsin[i],2)+pow(poserrorsout[i],2)))/2.0;
    
    posvaluediff[i]=((posvaluesin[i]/pow(poserrorsin[i],2)) - (posvaluesout[i]/pow(poserrorsout[i],2))) /((1/pow(poserrorsin[i],2)) + (1/pow(poserrorsout[i],2)));
    poserrordiff[i]= sqrt(1/((1/(pow(poserrorsin[i],2)))+(1/pow(poserrorsout[i],2))));

    std::cout<<"\n positive PMT "<<i+1<<std::endl;
    std::cout<<" IN          = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluesin[i]  
	     <<setw(4)<<" +- "<<poserrorsin[i] <<std::endl;
    std::cout<<" OUT         = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluesout[i] 
	     <<setw(4)<<" +- "<<poserrorsout[i]<<std::endl;
    std::cout<<" (IN+OUT)/2  = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluesum[i]  
	     <<setw(4)<<" +- "<<poserrorsum[i] <<std::endl;
    std::cout<<" AVG(IN,-OUT) = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluediff[i] 
	     <<setw(4)<<" +- "<<poserrordiff[i] <<std::endl;


    Myfile<<"\n positive PMT "<<i+1<<std::endl;
    Myfile<<" IN          = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluesin[i]  
	  <<setw(4)<<" +- "<<poserrorsin[i] <<std::endl;
    Myfile<<" OUT         = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluesout[i] 
	  <<setw(4)<<" +- "<<poserrorsout[i]<<std::endl;
    Myfile<<" (IN+OUT)/2  = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluesum[i] 
	  <<setw(4)<<" +- "<<poserrorsum[i] <<std::endl;
    Myfile<<" AVG(IN,-OUT) = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<posvaluediff[i] 
	  <<setw(4)<<" +- "<<poserrordiff[i] <<std::endl;

  }

  Myfile<<" !=================================="<<std::endl;
  Myfile<<" !NEG PMT (IN+OUT)/2 and AVG(IN,-OUT) values "<<std::endl;
  for(Int_t i =0;i<k;i++){

    negvaluesum[i]=(negvaluesin[i]+negvaluesout[i])/2.0;
    negerrorsum[i]= (sqrt(pow(negerrorsin[i],2)+pow(negerrorsout[i],2)))/2.0;
    
    negvaluediff[i]=((negvaluesin[i]/pow(negerrorsin[i],2)) - (negvaluesout[i]/pow(negerrorsout[i],2))) /((1/pow(negerrorsin[i],2)) + (1/pow(negerrorsout[i],2)));
    negerrordiff[i]= sqrt(1/((1/(pow(negerrorsin[i],2)))+(1/pow(negerrorsout[i],2))));

    std::cout<<"\n negative PMT "<<i+1<<std::endl;
    std::cout<<" IN          = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluesin[i]  
	     <<setw(4)<<" +- "<<negerrorsin[i] <<std::endl;
    std::cout<<" OUT         = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluesout[i] 
	     <<setw(4)<<" +- "<<negerrorsout[i]<<std::endl;
    std::cout<<" (IN+OUT)/2  = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluesum[i]  
	     <<setw(4)<<" +- "<<negerrorsum[i] <<std::endl;
    std::cout<<" AVG(IN,-OUT) = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluediff[i] 
	     <<setw(4)<<" +- "<<negerrordiff[i] <<std::endl;


    Myfile<<"\n negative PMT "<<i+1<<std::endl;
    Myfile<<" IN          = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluesin[i]  
	  <<setw(4)<<" +- "<<negerrorsin[i] <<std::endl;
    Myfile<<" OUT         = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluesout[i] 
	  <<setw(4)<<" +- "<<negerrorsout[i]<<std::endl;
    Myfile<<" (IN+OUT)/2  = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluesum[i] 
	  <<setw(4)<<" +- "<<negerrorsum[i] <<std::endl;
    Myfile<<" AVG(IN,-OUT) = "<<setiosflags(ios::fixed|ios::left)<<setw(5)<<setprecision(3)<<negvaluediff[i] 
	  <<setw(4)<<" +- "<<negerrordiff[i] <<std::endl;

  }

  // Fits for the data
  TF1 *fit_in;
  if(pol_opt==1) {
    fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]",1,8);
    fit_in->SetParName(2,"C^{IN}");
  }
  else if(pol_opt==2) {
    if(fit_opt==1){
      fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1)))",1,8);
    }
    else if(fit_opt==2){
      fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_in->SetParName(1,"C^{IN}");
    }
    else {
      fit_in = new TF1("fit_in","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
      fit_in->SetParName(2,"C^{IN}");
      fit_in->SetParameter(1,0);
    }
  }
  else{
    if(fit_opt==1){
      fit_in = new TF1("fit_in","[0]*sin((pi/180)*(45*(x-1)))",1,8);
    }
    else if(fit_opt==2){
      fit_in = new TF1("fit_in","[0]*sin((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_in->SetParName(1,"C^{IN}");
    }
    else {
      fit_in = new TF1("fit_in","[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
      fit_in->SetParName(2,"C^{IN}");
      fit_in->SetParameter(1,0);
    }
  }


  TF1 *fit_out;
  if(pol_opt==1){
    fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]",1,8); 
    fit_in->SetParName(2,"C^{OUT}");
  }
  else if(pol_opt==2){
    if(fit_opt==1){
      fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1)))",1,8);
    }
    else if(fit_opt ==2){
      fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_out->SetParName(1,"C^{OUT}");
    }
    else{
      fit_out = new TF1("fit_out","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
      fit_out->SetParName(2,"C^{OUT}");
      fit_out->SetParameter(1,0);
    }
  }
  else{
    if(fit_opt==1){
      fit_out = new TF1("fit_out","[0]*sin((pi/180)*(45*(x-1)))",1,8);
    }
    else if(fit_opt ==2){
      fit_out = new TF1("fit_out","[0]*sin((pi/180)*(45*(x-1))) + [1]",1,8);
      fit_out->SetParName(1,"C^{OUT}");
    }
    else{
      fit_out = new TF1("fit_out","[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
      fit_out->SetParName(2,"C^{OUT}");
      fit_out->SetParameter(1,0);
    }
  }
  
  // Initialize fits
  fit_in->SetParameter(0,0);
  fit_out->SetParameter(0,0);

  // Create a multigraph
  TMultiGraph * grp1 = new TMultiGraph();

  // Draw IN values
  std::cout<<"Draw and fit IN data\n";
  TGraphErrors* grp_pos_in  = new TGraphErrors(k,x,posvaluesin,errx,poserrorsin);
  grp_pos_in ->SetMarkerSize(0.8);
  grp_pos_in ->SetMarkerStyle(25);
  grp_pos_in ->SetMarkerColor(kBlue);
  grp_pos_in-> RemovePoint(1);
  grp_pos_in->Fit("fit_in","QEM");
  TF1* fit1 = grp_pos_in->GetFunction("fit_in");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
  fit1->SetLineWidth(2);
  fit1->SetLineStyle(2);
  if(pol_opt==1) {
    fit1->SetParName(0,"A_{V}^{POS}");
    fit1->SetParName(1,"A_{H}^{POS}");
  } 
  else if(pol_opt==2) {
    if(fit_opt==1){
      fit1->SetParName(0,"A_{V}^{POS}");
    }
    else if(fit_opt==2){
      fit1->SetParName(0,"A_{V}^{POS}");
    }
    else{
      fit1->SetParName(0,"A_{V}^{POS}");
      fit1->SetParName(1,"#phi_{0}^{POS}");
      fit1->SetParameter(1,0);
    }
  }
  else{
   if(fit_opt==1){
     fit1->SetParName(0,"A_{H}^{POS}");
   }
   else if(fit_opt==2){
     fit1->SetParName(0,"A_{H}^{POS}");
   }
   else{
     fit1->SetParName(0,"A_{H}^{POS}");
     fit1->SetParName(1,"#phi_{0}^{POS}");
     fit1->SetParameter(1,0);
   }
  }
  grp1->Add(grp_pos_in);

  TGraphErrors* grp_neg_in  = new TGraphErrors(k,x,negvaluesin,errx,negerrorsin);
  grp_neg_in ->SetMarkerSize(0.8);
  grp_neg_in ->SetMarkerStyle(21);
  grp_neg_in ->SetMarkerColor(kBlue);
  grp_neg_in-> RemovePoint(1);
  grp_neg_in->Fit("fit_in","QEM");
  TF1* fit2 = grp_neg_in->GetFunction("fit_in");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kBlue);
  fit2->SetLineWidth(2);
  fit2->SetLineStyle(10);
  fit2->SetParName(2,"C^{NEG}");
  if(pol_opt==1) {
    fit2->SetParName(0,"A_{V}^{NEG}");
    fit2->SetParName(1,"A_{H}^{NEG}");
  } 
  else if(pol_opt==2) {
    if(fit_opt==1){
      fit2->SetParName(0,"A_{V}^{NEG}");
    }
    else if(fit_opt==2){
      fit2->SetParName(0,"A_{V}^{NEg}");
    }
    else{
      fit2->SetParName(0,"A_{V}^{NEG}");
      fit2->SetParName(1,"#phi_{0}^{NEG}");
      fit2->SetParameter(1,0);
    }
  }
  else{
   if(fit_opt==1){
     fit2->SetParName(0,"A_{H}^{NEG}");
   }
   else if(fit_opt==2){
     fit2->SetParName(0,"A_{H}^{NEG}");
   }
   else{
     fit2->SetParName(0,"A_{H}^{NEG}");
     fit2->SetParName(1,"#phi_{0}^{NEG}");
     fit2->SetParameter(1,0);
   }
  }

  grp1->Add(grp_neg_in);


  // Draw OUT values
  std::cout<<"Draw and fit OUT data\n";
  TGraphErrors* grp_pos_out  = new TGraphErrors(k,x,posvaluesout,errx,poserrorsout);
  grp_pos_out ->SetMarkerSize(0.8);
  grp_pos_out ->SetMarkerStyle(25);
  grp_pos_out ->SetMarkerColor(kRed);
  grp_pos_out-> RemovePoint(1);
  grp_pos_out->Fit("fit_out","QEM");
  TF1* fit3 = grp_pos_out->GetFunction("fit_out");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kRed);
  fit3->SetLineWidth(2);
  fit3->SetLineStyle(2);
  if(pol_opt==1) {
    fit3->SetParName(0,"A_{V}^{POS}");
    fit3->SetParName(1,"A_{H}^{POS}");
  } 
  else if(pol_opt==2) {
    if(fit_opt==1){
      fit3->SetParName(0,"A_{V}^{POS}");
    }
    else if(fit_opt==2){
      fit3->SetParName(0,"A_{V}^{POS}");
    }
    else{
      fit3->SetParName(0,"A_{V}^{POS}");
      fit3->SetParName(1,"#phi_{0}^{POS}");
      fit3->SetParameter(1,0);
    }
  }
  else{
   if(fit_opt==1){
     fit3->SetParName(0,"A_{H}^{POS}");
   }
   else if(fit_opt==2){
     fit3->SetParName(0,"A_{H}^{POS}");
   }
   else{
     fit3->SetParName(0,"A_{H}^{POS}");
     fit3->SetParName(1,"#phi_{0}^{POS}");
     fit3->SetParameter(1,0);
   }
  }

  grp1->Add(grp_pos_out);

  TGraphErrors* grp_neg_out  = new TGraphErrors(k,x,negvaluesout,errx,negerrorsout);
  grp_neg_out ->SetMarkerSize(0.8);
  grp_neg_out ->SetMarkerStyle(21);
  grp_neg_out ->SetMarkerColor(kRed);
  grp_neg_out-> RemovePoint(1);
  grp_neg_out->Fit("fit_out","QEM");
  TF1* fit4 = grp_neg_out->GetFunction("fit_out");
  fit4->DrawCopy("same");
  fit4->SetLineColor(kRed);
  fit4->SetLineWidth(2);
  fit4->SetLineStyle(10);
  if(pol_opt==1) {
    fit4->SetParName(0,"A_{V}^{NEG}");
    fit4->SetParName(1,"A_{H}^{NEG}");
  } 
  else if(pol_opt==2) {
    if(fit_opt==1){
      fit4->SetParName(0,"A_{V}^{NEG}");
    }
    else if(fit_opt==2){
      fit4->SetParName(0,"A_{V}^{NEg}");
    }
    else{
      fit4->SetParName(0,"A_{V}^{NEG}");
      fit4->SetParName(1,"#phi_{0}^{NEG}");
      fit4->SetParameter(1,0);
    }
  }
  else{
    if(fit_opt==1){
     fit4->SetParName(0,"A_{H}^{NEG}");
   }
   else if(fit_opt==2){
     fit4->SetParName(0,"A_{H}^{NEG}");
   }
   else{
     fit4->SetParName(0,"A_{H}^{NEG}");
     fit4->SetParName(1,"#phi_{0}^{NEG}");
     fit4->SetParameter(1,0);
   }
  }
  grp1->Add(grp_neg_out);

 
 // Get the canvas
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.05);

  pad2->SetFillColor(0);
  pad2->Divide(1,2);
  pad2->cd(1);

  grp1->Draw("AP");
  grp1->SetTitle("");
  grp1->GetXaxis()->SetTitle("Octant Number");
  grp1->GetXaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitle(" Asymmetry (ppm)");
  grp1->GetYaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitleSize(0.08);
  grp1->GetXaxis()->SetTitleSize(0.08);
  grp1->GetYaxis()->SetTitleOffset(0.5);
  grp1->GetXaxis()->SetTitleOffset(0.85);
  grp1->GetYaxis()->SetLabelSize(0.08);
  grp1->GetXaxis()->SetLabelSize(0.08);

  pad2->Update();

  TPaveStats *stats1 = (TPaveStats*)(grp_pos_in->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)(grp_neg_in->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)(grp_pos_out->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats4 = (TPaveStats*)(grp_neg_out->GetListOfFunctions())->FindObject("stats");

  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 
  
  stats2->SetTextColor(kBlue);
  stats2->SetFillColor(kWhite); 
  
  stats3->SetTextColor(kRed);
  stats3->SetFillColor(kWhite); 

  stats4->SetTextColor(kRed);
  stats4->SetFillColor(kWhite); 
  
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.75);stats1->SetY2NDC(0.99);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.51);stats2->SetY2NDC(0.75);
  stats3->SetX1NDC(0.8); stats3->SetX2NDC(0.99); stats3->SetY1NDC(0.27);stats3->SetY2NDC(0.51);
  stats4->SetX1NDC(0.8); stats4->SetX2NDC(0.99); stats4->SetY1NDC(0.03);stats4->SetY2NDC(0.27);


  TLegend *legend = new TLegend(0.08,0.85,0.77,0.99,"","brNDC");
  legend->SetNColumns(4);
  legend->SetBorderSize(1);
  legend->AddEntry(grp_pos_in, "Positive PMT IN", "p");
  legend->AddEntry(grp_neg_in, "Negative PMT IN", "p");
  legend->AddEntry(grp_pos_out, "Positive PMT OUT", "p");
  legend->AddEntry(grp_neg_out, "Negative PMT OUT", "p");
  legend->AddEntry(fit1, "Positive PMT IN fit", "l");
  legend->AddEntry(fit2, "Negative PMT IN fit", "l");
  legend->AddEntry(fit3, "Positive PMT OUT fit", "l");
  legend->AddEntry(fit4, "Negative PMT OUT fit", "l");

  legend->SetTextFont(132);
  legend->SetFillColor(0);
  legend->Draw("");

  pad2->cd(2);

  TMultiGraph * grp2 = new TMultiGraph();

  // Draw IN+OUT values
  std::cout<<"Draw and fit (IN+OUT)/2 data\n";
  TGraphErrors* grp_pos_sum  = new TGraphErrors(k,x,posvaluesum,errx,poserrorsum);
  grp_pos_sum ->SetMarkerSize(0.6);
  grp_pos_sum ->SetMarkerStyle(25);
  grp_pos_sum ->SetMarkerColor(kGreen+3);
  grp_pos_sum-> RemovePoint(1);
  grp_pos_sum ->Fit("fit_in","QEM");
  TF1* fit5 = grp_pos_sum->GetFunction("fit_in");
  fit5->DrawCopy("same");
  fit5->SetLineColor(kGreen+3);
  fit5->SetParName(0,"A^{POS}");
  fit5->SetLineWidth(2);
  fit5->SetLineStyle(2);
  grp2->Add(grp_pos_sum);

  TGraphErrors* grp_neg_sum  = new TGraphErrors(k,x,negvaluesum,errx,negerrorsum);
  grp_neg_sum ->SetMarkerSize(0.8);
  grp_neg_sum ->SetMarkerStyle(21);
  grp_neg_sum ->SetMarkerColor(kGreen+3);
  grp_neg_sum-> RemovePoint(1);
  grp_neg_sum ->Fit("fit_in","QEM");
  TF1* fit6 = grp_neg_sum->GetFunction("fit_in");
  fit6->DrawCopy("same");
  fit6->SetLineColor(kGreen+3);
  fit6->SetParName(0,"A^{NEG}");
  fit6->SetLineWidth(2);
  fit6->SetLineStyle(10);
  grp2->Add(grp_neg_sum);

  grp2->Draw("AP");
  grp2->SetTitle("");
  grp2->GetXaxis()->SetTitle("Octant Number");
  grp2->GetXaxis()->CenterTitle();
  grp2->GetYaxis()->SetTitle(" Asymmetry (ppm)");
  grp2->GetYaxis()->CenterTitle();
  grp2->GetYaxis()->SetTitleSize(0.08);
  grp2->GetXaxis()->SetTitleSize(0.08);
  grp2->GetYaxis()->SetTitleOffset(0.5);
  grp2->GetXaxis()->SetTitleOffset(0.85);
  grp2->GetYaxis()->SetLabelSize(0.08);
  grp2->GetXaxis()->SetLabelSize(0.08);

  pad2->Update();

  TPaveStats *stats11 = (TPaveStats*)(grp_pos_sum->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats21 = (TPaveStats*)(grp_neg_sum->GetListOfFunctions())->FindObject("stats");
  
  stats11->SetTextColor(kBlack);
  stats11->SetFillColor(kWhite); 
  
  stats21->SetTextColor(kBlack);
  stats21->SetFillColor(kWhite); 
    
  stats11->SetX1NDC(0.8); stats11->SetX2NDC(0.99); stats11->SetY1NDC(0.50);stats11->SetY2NDC(0.95);
  stats21->SetX1NDC(0.8); stats21->SetX2NDC(0.99); stats21->SetY1NDC(0.05);stats21->SetY2NDC(0.50);
 

  TLegend *legend1 = new TLegend(0.08,0.83,0.5,0.97,"","brNDC");
  legend1->SetNColumns(2);
  legend1->SetBorderSize(1);
  legend1->AddEntry(grp_pos_sum, "Positive PMT (IN+OUT)/2", "p");
  legend1->AddEntry(grp_neg_sum, "Negative PMT (IN+OUT)/2", "p");
  legend1->AddEntry(fit5, "Positive PMT fit", "l");
  legend1->AddEntry(fit6, "Negative PMT fit", "l");

  legend1->SetTextFont(132);
  legend1->SetFillColor(0);
  legend1->Draw("");

  if(!plot_colors) Canvas2->SetGrayscale();
  Canvas2->Modified();
  Canvas2->Update();
  Canvas2->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_compare_single_"+bars+"_"+reg_set+"_in_p_out_plots_"+database+"_"+color+Form("_%iparam_fit.png",fit_opt));
  Canvas2->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_compare_single_"+bars+"_"+reg_set+"_in_p_out_plots_"+database+"_"+color+Form("_%iparam_fit.svg",fit_opt));
  Canvas2->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_compare_single_"+bars+"_"+reg_set+"_in_p_out_plots_"+database+"_"+color+Form("_%iparam_fit.C",fit_opt));


  // New canvas for AVG(IN,-OUT)
  TCanvas * Canvas21 = new TCanvas("Canvas21", title1,0,0,1600,800);
  Canvas21->Draw();
  Canvas21->SetFillColor(0);
  Canvas21->cd();
  
  TPad*pad11 = new TPad("pad11","pad11",0.005,0.940,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.940);
  pad11->SetFillColor(0);
  pad22->SetFillColor(0);
  pad11->Draw();
  pad22->Draw();

  pad11->cd();
  t1->Draw();

  pad22->cd();

  // Draw AVG(IN,-OUT) values
  std::cout<<"Draw and fit AVG(IN,-OUT) data\n";
  TMultiGraph * grp3 = new TMultiGraph();

  TGraphErrors* grp_pos_diff  = new TGraphErrors(k,x,posvaluediff,errx,poserrordiff);
  grp_pos_diff ->SetMarkerSize(1.2);
  grp_pos_diff ->SetMarkerStyle(22);
  grp_pos_diff ->SetMarkerColor(kBlack);
  grp_pos_diff-> RemovePoint(1);
  grp_pos_diff ->Fit("fit_in","QEM");
  TF1* fit7 = grp_pos_diff->GetFunction("fit_in");
  fit7->DrawCopy("same");
  fit7->SetParName(2,"C^{POS}");
  if(pol_opt==1) {
    fit7->SetParName(0,"A_{V}^{POS}");
    fit7->SetParName(1,"A_{H}^{POS}");
  } 
  else if(pol_opt==2) {
    fit7->SetParName(0,"A_{V}^{POS}");
  }
  else{
    fit7->SetParName(0,"A_{H}^{POS}");
    fit7->SetParName(1,"#phi_{0}^{POS}");
  }
  fit7->SetLineColor(kMagenta+1);
  fit7->SetLineWidth(2);
  fit7->SetLineStyle(2);
  grp3->Add(grp_pos_diff);

  TGraphErrors* grp_neg_diff  = new TGraphErrors(k,x,negvaluediff,errx,negerrordiff);
  grp_neg_diff ->SetMarkerSize(1.0);
  grp_neg_diff ->SetMarkerStyle(20);
  grp_neg_diff ->SetMarkerColor(kBlack);
  grp_neg_diff ->RemovePoint(1);
  grp_neg_diff ->Fit("fit_in","QEM");
  TF1* fit8 = grp_neg_diff->GetFunction("fit_in");
  fit8->DrawCopy("same");
  fit8->SetParName(2,"C^{NEG}");
  fit8->SetLineColor(kMagenta+1);
  if(pol_opt==1) {
    fit8->SetParName(0,"A_{V}^{NEG}");
    fit8->SetParName(1,"A_{H}^{NEG}");
  } 
  else if(pol_opt==2) {
    fit8->SetParName(0,"A_{V}^{NEG}");
  }
  else{
    fit8->SetParName(0,"A_{H}^{NEG}");
    fit8->SetParName(1,"#phi_{0}^{NEG}");
    fit8->SetParameter(1,0);
  }
  fit8->SetLineWidth(2);
  fit8->SetLineStyle(10);
  grp3->Add(grp_neg_diff);

  // barsum avg(in-out)
  TGraphErrors* grp_barsum  = new TGraphErrors(k,x,valuephys,errx,errorphys);
  grp_barsum ->SetMarkerSize(1.0);
  grp_barsum ->SetMarkerStyle(25);
  grp_barsum ->SetMarkerColor(kBlack);
  grp_barsum ->RemovePoint(1);
  grp_barsum ->Fit("fit_in","QEM");
  TF1* fit9 =  grp_barsum->GetFunction("fit_in");
  fit9->DrawCopy("same");
  fit9->SetParName(2,"C^{SUM}");
  fit9->SetLineColor(kMagenta+1);
  if(pol_opt==1) {
    fit9->SetParName(0,"A_{V}^{SUM}");
    fit9->SetParName(1,"A_{H}^{SUM}");
  } 
  else if(pol_opt==2) {
    fit9->SetParName(0,"A_{V}^{SUM}");
  }
  else{
    fit9->SetParName(0,"A_{H}^{SUM}");
    fit9->SetParName(1,"#phi_{0}^{SUM}");
    fit9->SetParameter(1,0);
  }
  fit9->SetLineWidth(2);
  fit9->SetLineStyle(1);
  grp3->Add(grp_barsum);


  grp3->Draw("AP");
  grp3->SetTitle("");
  grp3->GetXaxis()->SetTitle("Octant Number");
  grp3->GetXaxis()->CenterTitle();
  grp3->GetYaxis()->SetTitle(" Asymmetry (ppm)");
  grp3->GetYaxis()->CenterTitle();
  pad22->Update();

  TPaveStats *stats12 = (TPaveStats*)(grp_pos_diff->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats22 = (TPaveStats*)(grp_neg_diff->GetListOfFunctions())->FindObject("stats");
  TPaveStats *stats32 = (TPaveStats*)(grp_barsum->GetListOfFunctions())->FindObject("stats");
  
  stats12->SetTextColor(kBlack);
  stats12->SetFillColor(kWhite); 
  
  stats22->SetTextColor(kBlack);
  stats22->SetFillColor(kWhite); 

  stats32->SetTextColor(kBlack);
  stats32->SetFillColor(kWhite); 
    
  stats12->SetX1NDC(0.8); stats12->SetX2NDC(0.99); stats12->SetY1NDC(0.65);stats12->SetY2NDC(0.95);
  stats22->SetX1NDC(0.8); stats22->SetX2NDC(0.99); stats22->SetY1NDC(0.35);stats22->SetY2NDC(0.65);
  stats32->SetX1NDC(0.8); stats32->SetX2NDC(0.99); stats32->SetY1NDC(0.05);stats32->SetY2NDC(0.35);
 
  TLegend *legend11 = new TLegend(0.08,0.83,0.78,0.95,"","brNDC");
  legend11->SetNColumns(3);
  legend11->SetBorderSize(1);
  legend11->AddEntry(grp_pos_diff, "Positive PMT AVG(IN-OUT)", "p");
  legend11->AddEntry(grp_neg_diff, "Negative PMT AVG(IN-OUT)", "p");
  legend11->AddEntry(grp_barsum, "PMT AVG(IN-OUT)", "p");
  legend11->AddEntry(fit7, "Positive PMT fit", "l");
  legend11->AddEntry(fit8, "Negative PMT fit", "l");
  legend11->AddEntry(fit9, "PMT Average fit", "l");

  legend11->SetTextFont(132);
  legend11->SetFillColor(0);
  legend11->Draw("");

  if(!plot_colors) Canvas2->SetGrayscale();
  Canvas21->Modified();
  Canvas21->Update();
  Canvas21->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_compare_single_"+bars+"_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.png",fit_opt));
  Canvas21->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_compare_single_"+bars+"_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.svg",fit_opt));
  Canvas21->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_compare_single_"+bars+"_"+reg_set+"_avg_in_out_plots_"+database+"_"+color+Form("_%iparam_fit.C",fit_opt));
 
}
