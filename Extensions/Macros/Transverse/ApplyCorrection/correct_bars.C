//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : July 20, 2012
//*****************************************************************************************************//
/*
  This macro applys corrections to individual bars
  ./correct_bars db 

*/


#include "functions.h"
#include "functions.C"

using namespace std;

// Main function
int main(Int_t argc,Char_t* argv[])
{

  TApplication theApp("App",&argc,argv);

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.5);
  
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
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  
  //Set text
  gStyle->SetLabelFont(132,"x");
  gStyle->SetLabelFont(132,"y");
  gStyle->SetTitleFont(132,"x");
  gStyle->SetTitleFont(132,"y");
  gStyle->SetStatFont(132);
  //  gStyle->SetLegendFont(132);

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nCorrecting Bars for transverse effects   \n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;

  // second argument gives the database. Default is qw_run2_pass1
  if(argc>1) database = argv[1];
  std::cout<<"Using database "<<database<<std::endl;
  //connect to the data base
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");

  // For now, lets correct on consecative slugs
  // select the slugs 
  std::cout<<"Enter the starting slug with IHWP IN"<<std::endl;
  std::cin>>slug;
  std::cout<<"Using IN slug = "<<slug<<" and OUT slug ="<<slug+1<<std::endl;


  // select the target
  std::cout<<"Enter target type (default is LH2):"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cout<<"3. 1.6% DS Carbon "<<std::endl;
  std::cin>>opt;
  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 percent Al";
  }
  else if(opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 percent Carbon";
  }
  else{
    std::cout<<"Unknown target type!"<<std::endl;
    exit(1);
  }

  // select the polarization
  std::cout<<"Enter polarization type: (Default is Longitudinal)"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Vertical Transverse "<<std::endl;
  std::cout<<"3. Horizontal Transverse "<<std::endl;
  std::cin>>datopt;
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
    std::cout<<"Unknown polarization type!"<<std::endl;
    exit(1);
  }

  // select reaction
  std::cout<<"Enter reaction type: (default is elastic)"<<std::endl;
  std::cout<<"1. elastic"<<std::endl;
  std::cout<<"2. N->Delta "<<std::endl;
  std::cout<<"3. DIS "<<std::endl;
  std::cin>>ropt;
  if(ropt == 1){
    interaction = "elastic";
    good_for = "md_data_view.good_for_id = '1,"+good+"'";
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
    std::cout<<"Unknown interaction type!"<<std::endl;
    exit(1);
  }


  // for N->delta. select QTOR current
  if(ropt==2){
    std::cout<<"Enter QTOR current: (default is 1)"<<std::endl;
    std::cout<<"1.6000 A "<<std::endl;
    std::cout<<"2.6700 A "<<std::endl;
    std::cout<<"3.7300 A "<<std::endl;
    std::cin>>qtor_opt;
  }
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
 
  // use regression set
  reg_set="on_5+1";


  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Getting data from main detectors for "<<polar<<" "<<interaction
	   <<" ep scattering from "<<targ<<" with QTOR="<<qtor_stem<<std::endl;
  std::cout<<"###############################################"<<std::endl;

  // Open the txt file to store data
  Char_t  textfile[400];
  sprintf(textfile,"%s_%s_%s_%s_MD_corrections_%s_from_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 
  Myfile.open(textfile);
  Myfile<<" Slugs : in = "<<slug<<" and out = "<<slug+1<<std::endl;

  //Get the asymmetries from the detectors and calcualte IN-OUT 
  fit_and_correct();
  



  Myfile.close();

  std::cout<<"Done plotting fits \n";
  db->Close();
  delete db;

  theApp.Run();
  return(1);
}

