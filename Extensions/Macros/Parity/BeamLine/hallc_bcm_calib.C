//
// Author : Jeong Han Lee
// Date   : Wednesday, August 11 15:59:52 EDT 2010
//
//      
//          This program is used to do BCM calibrations 
//
//          To compile, 
//          "make bcm_calib"
//
//          To run, with default options
//
//          ./bcm_calib -r 5070  
//
//          with the fitting range of unser_current([0.15*unser_current, 0.90*unser_current])
//    
//          ./bcm_calib -r 5070 -e 15:90
//
//          with the unser beam off offset cutoff ( 300*1e3 )
//          ./bcm_calib -r 5070 -i 300 
// 
//
//          0.0.1 : Wednesday, August 11 16:07:20 EDT 2010
//                  can calibrate 
//                  qwk_sca_bcm1, qwk_sca_bcm2
//                  qwk_bcm1, qwk_bcm2, qwk_bcm5, qwk_bcm6
//  
//                  Thursday, August 12 00:30:22 EDT 2010
//                  try to open Qweak_****.000.root or Qweak_****.root. 
//                 
//                  Thursday, August 12 19:55:23 EDT 2010
//                  make an output file, contains qwk_bcm1, qwk_bcm2,
//                  qwk_bcm5, and qwk_bcm6 results.

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <getopt.h>
#include <sstream>
#include <vector>



#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
//#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TUnixSystem.h"


// TCanvas *canvas_sca; 
// TCanvas *canvas_vqwk;const char* program_name;



class BeamMonitor 
{

public:
  BeamMonitor();
  BeamMonitor(TString in);
  ~BeamMonitor(){};
  friend std::ostream& operator<<(std::ostream& stream, const BeamMonitor device);

  //
  // todo... seperate functions according to gaus and linear...
  //
  void SetName(const TString in) {name = in;};
  void SetAliasName(const TString in) {alias_name = in;};
  void SetPed(const Double_t in) {offset[0] = in;};
  void SetPedErr(const Double_t in) {offset[1] = in;};
  void SetSlop(const Double_t in) {slope[0] = in;};
  void SetSlopErr(const Double_t in) {slope[1] = in;}
  void SetMean(const Double_t in) {mean[0] = in;};
  void SetMeanErr(const Double_t in) {mean[1] = in;};
  void SetRMS(const Double_t in) {rms[0] = in;};
  void SetRMSErr(const Double_t in) {rms[1] = in;};
  void SetFitRangeMin(const Double_t in) {fit_range[0] = in;};
  void SetFitRangeMax(const Double_t in) {fit_range[1] = in;};
  void SetGausFit() {gauss_fit = true;};
  void SetLinearFit() {linear_fit = true;};

  
  TString GetName() {return name;};
  TString GetAliasName() {return alias_name;};
  Double_t GetPed() {return offset[0];};
  Double_t GetPedErr() {return offset[1];};
  Double_t GetSlop() {return slope[0];};
  Double_t GetSlopErr() {return slope[1];};
  Double_t GetMean() {return mean[0];};
  Double_t GetMeanErr() {return mean[1];};
  Double_t GetRMS() {return rms[0];};
  Double_t GetRMSErr() {return rms[1];};
  Double_t GetFitRangeMin() {return fit_range[0];};
  Double_t GetFitRangeMax() {return fit_range[1];};
  
  Bool_t IsGausFit() { return gauss_fit;};
  Bool_t IsLinearFit() {return linear_fit;};
  
private:
  TString name;
  TString alias_name;
  Double_t offset[2];
  Double_t slope[2];
  Double_t mean[2];
  Double_t rms[2];
  Double_t fit_range[2];
  Bool_t   gauss_fit;
  Bool_t   linear_fit;

};

BeamMonitor::BeamMonitor()
{
  name.Clear();
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope[0] = 0.0;
  slope[1] = 0.0;
  mean[0] = 0.0;
  mean[1] = 0.0;
  rms[0] = 0.0;
  rms[1] = 0.0;
  fit_range[0] = 0.0;
  fit_range[1] = 0.0;
  gauss_fit = false;
  linear_fit = false;
};


BeamMonitor::BeamMonitor(TString in)
{
  name = in;
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope[0] = 0.0;
  slope[1] = 0.0;
  mean[0] = 0.0;
  mean[1] = 0.0;
  rms[0] = 0.0;
  rms[1] = 0.0;
  fit_range[0] = 0.0;
  fit_range[1] = 0.0;
  gauss_fit = false;
  linear_fit = false;

};



std::ostream& operator<< (std::ostream& stream, const BeamMonitor device)
{

  stream << " Name : " << device.name;
  if(device.linear_fit) {
    stream << " Offset : "  << std::setw(4) << device.offset[0];
    stream << " +- "     << device.offset[1];
    stream << " Slope :" << std::setw(4)  << device.slope[0];
    stream << " +- "     << device.slope[1];
  }
  if (device.gauss_fit) {
    stream << " Mean :" << std::setw(4)  << device.mean[0];
    stream << " +- "     << device.mean[1];
    stream << " RMS :" << std::setw(4)  << device.rms[0];
    stream << " +- "     << device.rms[1];
  }
  stream << " Fit Range [" << device.fit_range[0];
  stream << " ," << device.fit_range[1];
  stream << "]";
    
  
  return stream;
};

const char* program_name;
TTree *mps_tree = NULL;
TFile *file = NULL;

void 
print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "\n");
  fprintf (stream, "This program is used to do BCM calibrations.\n");
  fprintf (stream, "Usage: %s -r {run number} -i {n} -e {a:b} \n", program_name);
  fprintf (stream, 
	   " -r run number.\n"
	   " -i unser beam off offset (n*1e3) \n"
	   " -e fit percent range (default [0.01*a*unser_current_range, 0.01*b*unser_current_range]\n"
	   );
  fprintf (stream, "\n");
  exit (exit_code);
}




std::vector<BeamMonitor> hallc_bcms_list;



Int_t
main(int argc, char **argv)
{
 
  TApplication theApp("App", &argc, argv);
  char* run_number = NULL;
  Double_t human_input_beam_off_range = 0.0;

  Double_t fit_range[2] = {0.0};


  Bool_t file_flag = false;
  Bool_t fit_range_flag = false;
  Bool_t unser_offset_flag = false;
  
  program_name = argv[0];

  int cc = 0; 

  while ( (cc= getopt(argc, argv, "r:i:e:")) != -1)
    switch (cc)
      {
      case 'r':
	{
	  file_flag = true;
	  run_number = optarg;
	}
	break;
      case 'i':
	{
	  unser_offset_flag    = true;
	  human_input_beam_off_range = atof(optarg);
	}
	break;
      case 'e':
	{
	  fit_range_flag = true;
	  char *c;
	  /*
	   * Allow the specification of alterations
	   * to the pty search range.  It is legal to
	   * specify only one, and not change the
	   * other from its default.
	   */
	  c = strchr(optarg, ':');
	  if (c) {
	    *c++ = '\0';
	    fit_range[1] = atof(c);
	  }
	  if (*optarg != '\0') {
	    fit_range[0] = atof(optarg);
	  }
	  if ((fit_range[0] > fit_range[1]) || (fit_range[0] < 0) || (fit_range[1] > 100.0) ) 
	    {
	      print_usage(stdout,0);
	    }
	}
	break;
      case '?':
	{
	  if (optopt == 'e') 
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'r')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr, "Unknown option character `\\x%x'.\n",  optopt);
	  print_usage(stdout,0);
	  return 1;
	}
	break;
      default:
	abort () ;
      }
  
  if(not fit_range_flag) {
    fit_range[0] = 0.10;
    fit_range[1] = 0.85;
  }
  else {
    fit_range[0] = fit_range[0]/100.0;
    fit_range[1] = fit_range[1]/100.0;
    
  }
  if(unser_offset_flag) {
    human_input_beam_off_range = human_input_beam_off_range*1e3;
  }
  else {
    human_input_beam_off_range = 320*1e3;
  }


  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }


  hallc_bcms_list.push_back(BeamMonitor("qwk_sca_unser"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_sca_bcm1"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_sca_bcm2"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm1"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm2"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm5"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm6"));



  TString filename = Form("Qweak_%s.000.root", run_number);
  file = new TFile(Form("~/scratch/rootfiles/%s", filename.Data()));

  if (file->IsZombie()) {
    std::cout << "Error opening file " << filename << std::endl;
    filename = Form("Qweak_%s.root", run_number);
    std::cout << "Try to open " << filename << std::endl;
    file = new TFile(Form("~/scratch/rootfiles/%s", filename.Data()));
    if (file->IsZombie()) {
      return EXIT_FAILURE;
    }
  }
  
  mps_tree = (TTree*) file->Get("Mps_Tree");

  Int_t w = 1100;
  Int_t h = 600;
  TCanvas *canvas_unser = new TCanvas("qwk_bcm_unser","SCA Unser", w, h);  
  //
  // extract unser maximum range in order to determine fit range
  //

  // Clock Corrected qwk_sca_unser;

  TString clock_name = "qwk_sca_4mhz";
  TString unser_name = "qwk_sca_unser";
 

  mps_tree -> SetAlias("clock_correct", Form("4e6/%s", clock_name.Data())); 
  mps_tree -> SetAlias("cc_sca_unser",  Form("clock_correct*%s", unser_name.Data()));

  canvas_unser -> Clear();
  canvas_unser -> Divide(2,1);
  canvas_unser -> cd(1);
  mps_tree -> Draw("cc_sca_unser:event_number", "");
  TH1D* sca_user_event;
  sca_user_event = (TH1D*)  gPad -> GetPrimitive("htemp");
  if(not sca_user_event) {
    std::cout << "Please check cc_sca_unser:event_number>>tmp"
	      << std::endl;
    theApp.Run();
  }

  Double_t unser_max = 0.0;
  unser_max = sca_user_event -> GetYaxis() -> GetXmax();

  //
  // extract unser offset, offset error, slope, and slope error.
  //
  canvas_unser -> cd(2);

  TH1D* cc_sca_user_gaus;
  mps_tree->Draw("cc_sca_unser", Form("cc_sca_unser<%lf", human_input_beam_off_range));
  cc_sca_user_gaus = (TH1D*)  gPad -> GetPrimitive("htemp");
  if(not cc_sca_user_gaus) {
    std::cout << "Please check "
  	      << unser_name
  	      << " in " << filename.Data()
  	      << std::endl;
    std::cout << "or\n This programs doesn't handle this run correctly. "
  	      << "Please send an email to jhlee@jlab.org"
  	      << std::endl;
    std::cout << "or\n You can adjust unser_beam_off_offset_cutoff value."
  	      << std::endl;
    theApp.Run();
  }
 
  cc_sca_user_gaus -> Fit("gaus", "M Q");
  TF1 *unser_fit = cc_sca_user_gaus -> GetFunction("gaus");

  if(unser_fit) {

    Double_t unser_scaler_unit = 2.5e-3;
    Double_t mean = unser_fit -> GetParameter(1);
    Double_t unser_max_current = 0.0;
    unser_max_current = (unser_max-mean)*unser_scaler_unit;
    mps_tree->SetAlias("unser_current", Form("((cc_sca_unser-%lf)*%lf)", mean, unser_scaler_unit));
    
    hallc_bcms_list.at(0).SetAliasName("user_current");
    hallc_bcms_list.at(0).SetGausFit();
    hallc_bcms_list.at(0).SetMean(mean);
    hallc_bcms_list.at(0).SetMeanErr(unser_fit -> GetParError(1));
    hallc_bcms_list.at(0).SetRMS(unser_fit -> GetParameter(2));
    hallc_bcms_list.at(0).SetRMSErr(unser_fit -> GetParameter(2));
    hallc_bcms_list.at(0).SetFitRangeMax(unser_max_current*fit_range[1]);
    hallc_bcms_list.at(0).SetFitRangeMin(unser_max_current*fit_range[0]);
    std::cout << hallc_bcms_list.at(0) << std::endl;

  }
  else {
    std::cout << "Please check "
  	      << unser_name
  	      << " in " << filename.Data()
  	      << std::endl;
    std::cout << "or\n This programs doesn't handle this run correctly. "
  	      << "Please send an email to jhlee@jlab.org"
  	      << std::endl;
    theApp.Run();
  }

  canvas_unser -> Modified();
  canvas_unser -> Update();
  // Int_t w = 1100;
  // Int_t h = 600;
  
  // canvas_sca = new TCanvas("bcm_sca_calib_test","BCM SCA Calibration Test", w, h);  
  
  // TString filename = Form("Qweak_%s.000.root", run_number);


  // TFile *file = new TFile(Form("~/scratch/rootfiles/%s", filename.Data()));
  // TTree *mps_tree = NULL;
      
  // Double_t bcm_vqwk_offset[10][2] = {{0.0}};
  // Double_t bcm_vqwk_slope[10][2] = {{0.0}};

  // if (file->IsZombie()) {
  //   std::cout << "Error opening file " << filename << std::endl;
  //   filename = Form("Qweak_%s.root", run_number);
  //   std::cout << "Try to open " << filename << std::endl;
  //   file = new TFile(Form("~/scratch/rootfiles/%s", filename.Data()));
  //   if (file->IsZombie()) {
  //     return EXIT_FAILURE;
  //   }
  // }

  // mps_tree = (TTree*) file->Get("Mps_Tree");
	
  // TString branch_name;
  // TString unser_name = "qwk_sca_unser";
  // TString clock_name = "qwk_sca_4mhz";
    
  // TH1D *sca_unser;
  // //  TH1D *sca_clock;
  // TH1D *bcm_sca[10]; // 10 has no meaning.. a quick and dirty way to do...
  // TH1D *bcm_vqwk[10];
  // TH1D *bcm_tmp[10];
  // TF1 *bcm_sca_fit[10];
   
	


  // canvas_sca -> Clear();
  // canvas_sca -> Divide(3,2);
	
  // // canvas_sca -> cd(1);
  // // mps_tree->Draw(Form("%s", clock_name.Data()));
  // // sca_clock  = (TH1D*) gPad -> GetPrimitive("htemp");
  // // if(not sca_clock) {
  // //   std::cout << "Please check "
  // // 		<< clock_name 
  // // 		<< " in " << filename.Data()
  // // 		<< std::endl;
  // //   theApp.Run();
  // // }
 
  // // sca_clock -> Fit("gaus", "M Q");
  // // TF1 *clock_fit = sca_clock -> GetFunction("gaus");
	
  // // Double_t clock_mean[2] = {0.0};
  // // Double_t clock_sigma[2] = {0.0};


  // // clock_mean[0] = clock_fit -> GetParameter(1);
  // // clock_mean[1] = clock_fit -> GetParError(1);
  // // clock_sigma[0] = clock_fit -> GetParameter(2);
  // // clock_sigma[1] = clock_fit -> GetParError(2);
	
  // // printf("--------- qwk_sca_4hmz ----------------------------------------\n");
  // // printf("clock_mean      %12.3lf +- %8.3lf\n", clock_mean[0], clock_mean[1]);
  // // printf("clock_sigma     %12.3lf +- %8.3lf\n", clock_sigma[0], clock_sigma[1]);
  // // printf("\n");



  // canvas_sca -> cd(1);

  // Double_t unser_scaler_unit = 0.0;

  // unser_scaler_unit = 2.5e-3;
  // mps_tree->SetAlias("clock_correct", Form("4e6/%s", clock_name.Data()));
  // mps_tree->SetAlias("cc_sca_unser", Form("clock_correct*%s", unser_name.Data()));
  // mps_tree -> Draw("cc_sca_unser:event_number", "");
  // bcm_tmp[0] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_tmp[0]) {
  //   std::cout << "Please check "
  // 	      << unser_name
  // 	      << " in " << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }
  // Double_t unser_max = 0.0;
  // unser_max = bcm_tmp[0] -> GetYaxis() -> GetXmax();



  // canvas_sca -> cd(2);

  // mps_tree->Draw("cc_sca_unser", Form("cc_sca_unser<%lf", human_input_beam_off_range));
  // sca_unser = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not sca_unser) {
  //   std::cout << "Please check "
  // 	      << unser_name
  // 	      << " in " << filename.Data()
  // 	      << std::endl;
  //   std::cout << "or\n This programs doesn't handle this run correctly. "
  // 	      << "Please send an email to jhlee@jlab.org"
  // 	      << std::endl;
  //   std::cout << "or\n You can adjust unser_beam_off_offset_cutoff value."
  // 	      << std::endl;
  //   theApp.Run();
  // }
 
  // sca_unser -> Fit("gaus", "M Q");
  // TF1 *unser_fit = sca_unser -> GetFunction("gaus");
  // Double_t unser_mean[2] = {0.0};
  // Double_t unser_sigma[2] = {0.0};

  // if(unser_fit) {
    
  //   unser_mean[0]  = unser_fit -> GetParameter(1);
  //   unser_mean[1]  = unser_fit -> GetParError(1);
  //   unser_sigma[0] = unser_fit -> GetParameter(2);
  //   unser_sigma[1] = unser_fit -> GetParError(2);
    
  //   printf("--------- qwk_sca_unser ---------------------------------------\n");
  //   printf("unser_mean      %12.3lf +- %8.3lf\n", unser_mean[0], unser_mean[1]);
  //   printf("unser_sigma     %12.3lf +- %8.3lf\n", unser_sigma[0], unser_sigma[1]);
  //   printf("\n");
	
  // }

  // canvas_sca -> cd(3);

  // mps_tree->SetAlias("unser_current", Form("((cc_sca_unser-%lf)*%lf)", unser_mean[0], unser_scaler_unit));
  // mps_tree->SetAlias("cc_sca_bcm1",  "clock_correct*qwk_sca_bcm1");
  // mps_tree->SetAlias("cc_sca_bcm2",  "clock_correct*qwk_sca_bcm2");


  // Double_t fit_unser_range[2] = {0.0};
  // Double_t unser_max_current = 0.0;
  // unser_max_current = (unser_max-unser_mean[0])*unser_scaler_unit;
  // fit_unser_range[0] = fit_range[0]*unser_max_current;
  // fit_unser_range[1] = fit_range[1]*unser_max_current;

  // std::cout << "\nUnser Fit Range  [" 
  // 	    << fit_unser_range[0] 
  // 	    << ", " 
  // 	    << fit_unser_range[1]
  // 	    << "]\n"
  // 	    << std::endl;

  // mps_tree->Draw("cc_sca_bcm1:unser_current", "" ,"profs");

  // bcm_sca[0] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_sca[0]) {
  //   std::cout << "Please check qwk_sca_bcm1 in "
  // 	      << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }

  // bcm_sca[0] -> SetTitle("cc_sca_bcm1 vs unser_current");
  // bcm_sca[0] -> GetXaxis() -> SetTitle("unser_current");
  // bcm_sca[0] -> Fit("pol1", "E F Q", "", fit_unser_range[0], fit_unser_range[1]);
  // bcm_sca_fit[0] = bcm_sca[0] -> GetFunction("pol1");

  // Double_t sca_bcm1_offset[2] = {0.0};
  // Double_t sca_bcm1_slope[2] = {0.0};
	
  // if(bcm_sca_fit[0]) {
  //   sca_bcm1_offset[0] = bcm_sca_fit[0] -> GetParameter(0);
  //   sca_bcm1_offset[1] = bcm_sca_fit[0] -> GetParError(0);
  //   sca_bcm1_slope[0]  = bcm_sca_fit[0] -> GetParameter(1);
  //   sca_bcm1_slope[1]  = bcm_sca_fit[0] -> GetParError(1);
    
  //   printf("--------- qwk_sca_bcm1  ---------------------------------------\n");
  //   printf("offset    %12.3lf +- %8.3lf\n", sca_bcm1_offset[0], sca_bcm1_offset[1]);
  //   printf("slope     %12.3lf +- %8.3lf\n", sca_bcm1_slope[0], sca_bcm1_slope[1]);
  //   printf("\n");
  // }
  
  // canvas_sca -> cd(4);

  // mps_tree->Draw(Form("(cc_sca_bcm1-%lf):event_number", sca_bcm1_offset[0]));
  // bcm_tmp[1] = (TH1D*) gPad -> GetPrimitive("htemp");
  
  // bcm_tmp[1] -> SetTitle("corrected qwk_sca_bcm1 vs event_number");
  // bcm_tmp[1] -> GetYaxis() -> SetTitle("(bcm1-bcm1_ped)*4e6/qwk_sca_4mhz*2.5e-3");


  // canvas_sca -> cd(5);

  // mps_tree->Draw("cc_sca_bcm2:unser_current", "" ,"profs");

  // bcm_sca[1] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_sca[1]) {
  //   std::cout << "Please check qwk_sca_bcm2 in "
  // 	      << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }
  // bcm_sca[1] -> SetTitle("cc_sca_bcm2 vs unser_current");
  // bcm_sca[1] -> GetXaxis() -> SetTitle("unser_current");
  // bcm_sca[1] -> Fit("pol1", "E F Q", "", fit_unser_range[0], fit_unser_range[1]);
  // bcm_sca_fit[1] = bcm_sca[1] -> GetFunction("pol1");

  // Double_t sca_bcm2_offset[2] = {0.0};
  // Double_t sca_bcm2_slope[2] = {0.0};
  // if(bcm_sca_fit[1]) {
  //   sca_bcm2_offset[0] = bcm_sca_fit[1] -> GetParameter(0);
  //   sca_bcm2_offset[1] = bcm_sca_fit[1] -> GetParError(0);
  //   sca_bcm2_slope[0]  = bcm_sca_fit[1] -> GetParameter(1);
  //   sca_bcm2_slope[1]  = bcm_sca_fit[1] -> GetParError(1);
    
  //   printf("--------- qwk_sca_bcm2  ---------------------------------------\n");
  //   printf("offset    %12.3lf +- %8.3lf\n", sca_bcm2_offset[0], sca_bcm2_offset[1]);
  //   printf("slope     %12.3lf +- %8.3lf\n", sca_bcm2_slope[0], sca_bcm2_slope[1]);
  //   printf("\n");
  // }
  
  // canvas_sca -> cd(6);

  // mps_tree->Draw(Form("(cc_sca_bcm2-%lf):event_number", sca_bcm2_offset[0]));
  // bcm_tmp[2] = (TH1D*) gPad -> GetPrimitive("htemp");
  // bcm_tmp[2] -> SetTitle("corrected qwk_sca_bcm2 vs event_number");
  // bcm_tmp[2] -> GetYaxis() -> SetTitle("(bcm1-bcm1_ped)*4e6/qwk_sca_4mhz*2.5e-3");

  // canvas_sca->Modified();

  // canvas_sca -> Update();

  // TH1D *bcm_vqwk_tmp[10];
  // TF1  *bcm_vqwk_fit[10];

  // mps_tree->SetAlias("bcm1", "(qwk_bcm1.hw_sum_raw)/qwk_bcm1.num_samples");
  // mps_tree->SetAlias("bcm2", "(qwk_bcm2.hw_sum_raw)/qwk_bcm2.num_samples");
  // mps_tree->SetAlias("bcm5", "(qwk_bcm5.hw_sum_raw)/qwk_bcm5.num_samples");
  // mps_tree->SetAlias("bcm6", "(qwk_bcm6.hw_sum_raw)/qwk_bcm6.num_samples");

  // canvas_vqwk = new TCanvas("bcm_vqk_calib_test","BCM VQWK Calibration Test", w, h);  
    
  // canvas_vqwk -> Clear();
  // canvas_vqwk -> Divide(4,2);
  // canvas_vqwk -> cd(1);
  // mps_tree->Draw("bcm1:unser_current", "qwk_bcm1.num_samples>0", "profs");

  // bcm_vqwk[0] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_vqwk[0]) {
  //   std::cout << "Please check qwk_bcm1 in "
  // 	      << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }
  // bcm_vqwk[0] -> SetTitle("bcm1 vs unser_current");
  // bcm_vqwk[0] -> GetXaxis() -> SetTitle("unser_current");
  // bcm_vqwk[0] -> Fit("pol1", "E F Q", "",  fit_unser_range[0], fit_unser_range[1]);
  // bcm_vqwk_fit[0] = bcm_vqwk[0] -> GetFunction("pol1");
  // if(bcm_vqwk_fit[0]) {
  //   bcm_vqwk_offset[0][0] = bcm_vqwk_fit[0] -> GetParameter(0);
  //   bcm_vqwk_offset[0][1] = bcm_vqwk_fit[0] -> GetParError(0);
  //   bcm_vqwk_slope[0][0]  = bcm_vqwk_fit[0] -> GetParameter(1);
  //   bcm_vqwk_slope[0][1]  = bcm_vqwk_fit[0] -> GetParError(1);
    
  //   printf("--------- qwk_bcm1      ---------------------------------------\n");
  //   printf("qwk_bcm1 offset    %12.3lf +- %8.3lf\n", bcm_vqwk_offset[0][0], bcm_vqwk_offset[0][1]);
  //   printf("qwk_bcm1 slope     %12.3lf +- %8.3lf\n", bcm_vqwk_slope[0][0], bcm_vqwk_slope[0][1]);
  //   printf("\n");
  // }

  // canvas_vqwk -> cd(2);
  // mps_tree->Draw("bcm2:unser_current", "qwk_bcm2.num_samples>0", "profs");
  // bcm_vqwk[1] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_vqwk[1]) {
  //   std::cout << "Please check qwk_bcm2 in "
  // 	      << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }
  // bcm_vqwk[1] -> SetTitle("bcm2 vs unser_current");
  // bcm_vqwk[1] -> GetXaxis() -> SetTitle("unser_current");
  // bcm_vqwk[1] -> Fit("pol1", "E F Q", "",  fit_unser_range[0], fit_unser_range[1]);
  // bcm_vqwk_fit[1] = bcm_vqwk[1] -> GetFunction("pol1");

  // if(bcm_vqwk_fit[1]) {
  //   bcm_vqwk_offset[1][0] = bcm_vqwk_fit[1] -> GetParameter(0);
  //   bcm_vqwk_offset[1][1] = bcm_vqwk_fit[1] -> GetParError(0);
  //   bcm_vqwk_slope[1][0]  = bcm_vqwk_fit[1] -> GetParameter(1);
  //   bcm_vqwk_slope[1][1]  = bcm_vqwk_fit[1] -> GetParError(1);
    
  //   printf("--------- qwk_bcm2      ---------------------------------------\n");
  //   printf("qwk_bcm2 offset    %12.3lf +- %8.3lf\n", bcm_vqwk_offset[1][0], bcm_vqwk_offset[1][1]);
  //   printf("qwk_bcm2 slope     %12.3lf +- %8.3lf\n", bcm_vqwk_slope[1][0], bcm_vqwk_slope[1][1]);
  //   printf("\n");
  // }

  // canvas_vqwk -> cd(3);
  // mps_tree->Draw("bcm5:unser_current", "qwk_bcm5.num_samples>0", "profs");
  // bcm_vqwk[2] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_vqwk[2]) {
  //   std::cout << "Please check qwk_bcm5 in "
  // 	      << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }

  // bcm_vqwk[2] -> SetTitle("bcm5 vs unser_current");
  // bcm_vqwk[2] -> GetXaxis() -> SetTitle("unser_current");
  // bcm_vqwk[2] -> Fit("pol1", "E F Q", "",  fit_unser_range[0], fit_unser_range[1]);
  // bcm_vqwk_fit[2] = bcm_vqwk[2] -> GetFunction("pol1");
  // if(bcm_vqwk_fit[2]) {
  //   bcm_vqwk_offset[2][0] = bcm_vqwk_fit[2] -> GetParameter(0);
  //   bcm_vqwk_offset[2][1] = bcm_vqwk_fit[2] -> GetParError(0);
  //   bcm_vqwk_slope[2][0]  = bcm_vqwk_fit[2] -> GetParameter(1);
  //   bcm_vqwk_slope[2][1]  = bcm_vqwk_fit[2] -> GetParError(1);
    
  //   printf("--------- qwk_bcm5      ---------------------------------------\n");
  //   printf("qwk_bcm 5 offset    %12.3lf +- %8.3lf\n", bcm_vqwk_offset[2][0], bcm_vqwk_offset[2][1]);
  //   printf("qwk_bcm 5 slope     %12.3lf +- %8.3lf\n", bcm_vqwk_slope[2][0], bcm_vqwk_slope[2][1]);
  //   printf("\n");
  // }

  // canvas_vqwk -> cd(4);
  // mps_tree->Draw("bcm6:unser_current", "qwk_bcm6.num_samples>0", "profs");
  // bcm_vqwk[3] = (TH1D*) gPad -> GetPrimitive("htemp");
  // if(not bcm_vqwk[3]) {
  //   std::cout << "Please check qwk_bcm6 in "
  // 	      << filename.Data()
  // 	      << std::endl;
  //   theApp.Run();
  // }
  // bcm_vqwk[3] -> SetTitle("bcm6 vs unser_current");
  // bcm_vqwk[3] -> GetXaxis() -> SetTitle("unser_current");
  // bcm_vqwk[3] -> Fit("pol1", "E F Q", "",  fit_unser_range[0], fit_unser_range[1]);
  // bcm_vqwk_fit[3] = bcm_vqwk[3] -> GetFunction("pol1");
  
  // if(bcm_vqwk_fit[3]) {
  //   bcm_vqwk_offset[3][0] = bcm_vqwk_fit[3] -> GetParameter(0);
  //   bcm_vqwk_offset[3][1] = bcm_vqwk_fit[3] -> GetParError(0);
  //   bcm_vqwk_slope[3][0]  = bcm_vqwk_fit[3] -> GetParameter(1);
  //   bcm_vqwk_slope[3][1]  = bcm_vqwk_fit[3] -> GetParError(1);
	
  //   printf("--------- qwk_bcm6      ---------------------------------------\n");
  //   printf("qwk_bcm 6 offset    %12.3lf +- %8.3lf\n", bcm_vqwk_offset[3][0], bcm_vqwk_offset[3][1]);
  //   printf("qwk_bcm 6 slope     %12.3lf +- %8.3lf\n", bcm_vqwk_slope[3][0], bcm_vqwk_slope[3][1]);
  //   printf("\n");
  // }

  
  // canvas_vqwk -> cd(5);
  // mps_tree->Draw(Form("bcm1-%lf:event_number", bcm_vqwk_offset[0][0]), "qwk_bcm1.num_samples>0");
  // bcm_vqwk_tmp[0] = (TH1D*) gPad -> GetPrimitive("htemp");
  // bcm_vqwk_tmp[0] -> SetTitle("corrected qwk_bcm1 vs event_number");
   
  // canvas_vqwk -> cd(6);
  // mps_tree->Draw(Form("bcm2-%lf:event_number", bcm_vqwk_offset[1][0]), "qwk_bcm2.num_samples>0");
  // bcm_vqwk_tmp[1] = (TH1D*) gPad -> GetPrimitive("htemp");
  // bcm_vqwk_tmp[1] -> SetTitle("corrected qwk_bcm2 vs event_number");

  // canvas_vqwk -> cd(7);
  // mps_tree->Draw(Form("bcm5-%lf:event_number", bcm_vqwk_offset[2][0]), "qwk_bcm5.num_samples>0");
  // bcm_vqwk_tmp[2] = (TH1D*) gPad -> GetPrimitive("htemp");
  // bcm_vqwk_tmp[2] -> SetTitle("corrected qwk_bcm5 vs event_number");
   
  // canvas_vqwk -> cd(8);
  // mps_tree->Draw(Form("bcm6-%lf:event_number", bcm_vqwk_offset[3][0]), "qwk_bcm6.num_samples>0");
  // bcm_vqwk_tmp[3] = (TH1D*) gPad -> GetPrimitive("htemp");
  // bcm_vqwk_tmp[3] -> SetTitle("corrected qwk_bcm6 vs event_number");
   
  // canvas_vqwk -> Modified();
  // canvas_vqwk -> Update();
  
	
           
  // std::ofstream       hallc_bcm_pedestal_output;
  // std::ostringstream  hallc_bcm_pedestal_stream;
  // hallc_bcm_pedestal_output.clear();
  // hallc_bcm_pedestal_output.open(Form("hallc_bcm_pedestal_%s.txt", run_number));
  // hallc_bcm_pedestal_stream.clear();

 
  // // hallc_bcm_pedestal_stream << "!device name"
  // // 			    << ", ped"
  // // 			    << ", ped err"
  // // 			    << ", slop"
  // // 			    << ", slor err"
  // // 			    << "\n";
  // std::size_t i=0;
  

  // // std::cout << "size " << hallc_bcm_list.size() << std::endl;
  // for (i=0; i < hallc_bcm_list.size(); i++) {
  //   hallc_bcm_pedestal_stream << hallc_bcm_list[i].Data()
  // 			      << "  "
  // 			      << bcm_vqwk_offset[i][0]
  // 			      << " "
  //    			      << bcm_vqwk_offset[i][1]
  // 			      << " "
  // 			      << bcm_vqwk_slope[i][0]
  // 			      << " "
  // 			      << bcm_vqwk_slope[i][1]
  // 			      << "\n";
  // } 
  // hallc_bcm_pedestal_output << hallc_bcm_pedestal_stream.str();
  // std::cout << hallc_bcm_pedestal_stream.str() <<std::endl;
  // hallc_bcm_pedestal_output.close();
      
  theApp.Run();
  return 0;
}
