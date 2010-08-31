//
// Author : Jeong Han Lee
// Date   : Wednesday, August 18 02:45:52 EDT 2010
//
//      
//          This program is used to do HallC BCMs calibration
//
//          To compile, 
//          "make hallc_bcm_calib"
//
//          To run, with default options
//
//          ./hallc_bcm_calib -r 5070  
//
//          with the fitting range of unser_current([0.15*unser_current, 0.90*unser_current])
//    
//          ./hallc_bcm_calib -r 5070 -e 15:90
//
//          with the unser beam off offset cutoff ( 300*1e3 )
//          ./hallc_bcm_calib -r 5070 -i 300 
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
//
//          0.0.2 : Wednesday, August 18 01:20:12 EDT 2010
//                  redesign, thus do BCM calibrations is quite similar
//                            to do BPM calibrations (hallc_bpm_calib.C)
//                  

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
#include "TCut.h"



class BeamMonitor 
{

public:
  BeamMonitor();
  BeamMonitor(TString in);
  ~BeamMonitor(){};
  friend std::ostream& operator<<(std::ostream& stream, const BeamMonitor &device);

  void SetName(const TString in) {name = in;};
  void SetAliasName(const TString in) {alias_name = in;};
  void SetPed(const Double_t in) {offset[0] = in;};
  void SetPedErr(const Double_t in) {offset[1] = in;};
  void SetSlop(const Double_t in) {slope[0] = in;};
  void SetSlopErr(const Double_t in) {slope[1] = in;}
  void SetFitRange(const Double_t in[2]) {fit_range[0] = in[0]; fit_range[1] = in[1];};
  void SetReference() {reference_flag = true;};
  void SetFileStream() {filestream_flag = true;};
  
  TString GetName() {return name;};
  const char* GetCName() {return name.Data();};
  TString GetAliasName() {return alias_name;};
  const char* GetAliasCName() {return alias_name.Data();};
  Double_t GetPed() {return offset[0];};
  Double_t GetPedErr() {return offset[1];};
  Double_t GetSlop() {return slope[0];};
  Double_t GetSlopErr() {return slope[1];};
  // Double_t GetMean() {return mean[0];};
  // Double_t GetMeanErr() {return mean[1];};
  // Double_t GetRMS() {return rms[0];};
  // Double_t GetRMSErr() {return rms[1];};
  Double_t GetFitRangeMin() {return fit_range[0];};
  Double_t GetFitRangeMax() {return fit_range[1];};
  
  Bool_t IsReference() { return reference_flag;};
  Bool_t IsFileStream() {return filestream_flag;};

private:
  TString name;
  TString alias_name;
  Double_t offset[2];
  Double_t slope[2];
  // Double_t mean[2];
  // Double_t rms[2];
  Double_t fit_range[2];
  Bool_t   reference_flag;
  Bool_t   filestream_flag;

};

BeamMonitor::BeamMonitor()
{
  name.Clear();
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope[0] = 0.0;
  slope[1] = 0.0;
  // mean[0] = 0.0;
  // mean[1] = 0.0;
  // rms[0] = 0.0;
  // rms[1] = 0.0;
  fit_range[0] = 0.0;
  fit_range[1] = 0.0;
  reference_flag = false;
  filestream_flag = false;
};


BeamMonitor::BeamMonitor(TString in)
{
  name = in;
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope[0] = 0.0;
  slope[1] = 0.0;
  // mean[0] = 0.0;
  // mean[1] = 0.0;
  // rms[0] = 0.0;
  // rms[1] = 0.0;
  fit_range[0] = 0.0;
  fit_range[1] = 0.0;
  reference_flag = false;
  filestream_flag = false;

};


std::ostream& operator<< (std::ostream& stream, const BeamMonitor &device)
{
  TString device_name = device.name;

  if(device.filestream_flag) {
    stream <<  std::setw(14) <<  device_name;
    stream << " ";
    stream <<  std::setw(10) << device.offset[0];
    stream << " " ;
    stream <<  std::setw(10) << device.offset[1];
    stream << " " ;
    stream <<  std::setw(10) << device.slope[0];
    stream << " " ;
    stream <<  std::setw(10) << device.slope[1];
    stream << "\n";
  }
  else {

    if(device.reference_flag) {
      stream <<  std::setw(14) << "Reference : " <<  std::setw(14) <<  device_name;
    }
    else {
      stream <<  std::setw(14) << " Name : " <<  std::setw(14) <<  device_name;
    }
    stream << " Offset : "  << std::setw(4) << device.offset[0];
    stream << " +- "     << device.offset[1];
    
    if(device.reference_flag) {
      stream << " Fit Range [" << device.fit_range[0];
      stream << " ," << device.fit_range[1];
      stream << "]";
    }
    else {
      stream << " Slope :" << std::setw(4)  << device.slope[0];
      stream << " +- "     << device.slope[1];
    }
  }
  return stream;
}


void print_usage(FILE* stream, int exit_code);
Bool_t calibrate(BeamMonitor &device, BeamMonitor &reference);

TH1D*
GetHisto(TTree *tree, const TString name, const TCut cut, Option_t* option = "")
{
  tree ->Draw(name, cut, option);
  TH1D* tmp;
  tmp = (TH1D*)  gPad -> GetPrimitive("htemp");
  if(not tmp) {
    return 0;
  }
  return tmp;
}


const char* program_name;
TTree *mps_tree = NULL;
TFile *file = NULL;
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


  hallc_bcms_list.push_back(BeamMonitor("qwk_sca_bcm1"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_sca_bcm2"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm1"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm2"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm5"));
  hallc_bcms_list.push_back(BeamMonitor("qwk_bcm6"));



  BeamMonitor sca_unser("qwk_sca_unser");
  sca_unser.SetReference();
  

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

  Int_t w = 800;
  Int_t h = 400;
  TCanvas *canvas_unser = new TCanvas("qwk_bcm_unser","SCA Unser", w, h);  
  //
  // extract unser maximum range in order to determine fit range
  //

  // Clock Corrected qwk_sca_unser;

  TString clock_name = "qwk_sca_4mhz";
  
  mps_tree -> SetAlias("clock_correct", Form("4e6/%s", clock_name.Data())); 
  mps_tree -> SetAlias("cc_sca_unser",  Form("clock_correct*%s", sca_unser.GetCName()));

  mps_tree -> SetAlias("cc_sca_bcm1",  "clock_correct*qwk_sca_bcm1");
  mps_tree -> SetAlias("cc_sca_bcm2",  "clock_correct*qwk_sca_bcm2");
  hallc_bcms_list.at(0).SetAliasName("cc_sca_bcm1");
  hallc_bcms_list.at(1).SetAliasName("cc_sca_bcm2");


  //  mps_tree -> SetAlias("TM_cc_unser",  "cc_sca_unser:event_number"); //  Bad numerical expression  ??
  

  canvas_unser -> Clear();
  canvas_unser -> Divide(2,1);
  canvas_unser -> cd(1);
  
  TH1D* sca_user_event;
  sca_user_event = GetHisto(mps_tree, "cc_sca_unser:event_number", "");
  if(not sca_user_event) {
    std::cout << "Please check cc_sca_unser:event_number>>tmp"
       	      << std::endl;
    theApp.Run();
  }

  Double_t unser_max = sca_user_event -> GetYaxis() -> GetXmax();


  //
  // extract unser offset, offset error, slope, and slope error.
  //
  canvas_unser -> cd(2);

  TCut unser_cut(Form("%s<%lf", "cc_sca_unser",  human_input_beam_off_range));

  TH1D* cc_sca_unser_gaus = GetHisto(mps_tree, "cc_sca_unser", unser_cut);

  if(not cc_sca_unser_gaus) {
    std::cout << "Please check "
  	      << "cc_sca_unser"
  	      << " in " << filename.Data()
  	      << std::endl;
    std::cout << "or\n This programs doesn't handle this run correctly. "
  	      << "Please send an email to jhlee@jlab.org"
  	      << std::endl;
    std::cout << "or\n You can adjust unser_beam_off_offset_cutoff value."
  	      << std::endl;
    theApp.Run();
  }
  cc_sca_unser_gaus -> SetTitle("cc_sca_unser");
  cc_sca_unser_gaus -> Fit("gaus", "M Q");
  TF1 *unser_fit = cc_sca_unser_gaus -> GetFunction("gaus");
  unser_fit->SetLineColor(kRed);
  TString unser_name = "unser_current";

  if(unser_fit) {
  
    Double_t unser_scaler_unit = 2.5e-3;
    Double_t mean = unser_fit -> GetParameter(1);
    Double_t unser_max_current = (unser_max-mean)*unser_scaler_unit;

    fit_range[0] *= unser_max_current;
    fit_range[1] *= unser_max_current;

    std::cout << "range " << fit_range[0] << " " << fit_range[1] << std::endl;
    mps_tree->SetAlias(unser_name.Data(), Form("((cc_sca_unser-%lf)*%lf)", mean, unser_scaler_unit));
    sca_unser.SetAliasName(unser_name.Data());
    sca_unser.SetPed(mean);
    sca_unser.SetPedErr(unser_fit -> GetParError(1));
    sca_unser.SetFitRange(fit_range);
    std::cout << sca_unser << std::endl;
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
 


  std::size_t i = 0; 
  Int_t cnt = 0;
  std::cout << "how many bcms at Hall C ? " << hallc_bcms_list.size() << std::endl;
  for (i=0;i < hallc_bcms_list.size(); i++ ) {
    std::cout << "\n" 
	      << cnt++ 
	      << ": onto calibrating " << hallc_bcms_list.at(i).GetName() 
	      << std::endl;
    Bool_t check = false;
    check = calibrate(hallc_bcms_list.at(i), sca_unser) ; 
    //  if(not check) theApp.Run();
  }

	
           
  std::ofstream       hallc_bcms_pedestal_output;
  std::ostringstream  hallc_bcms_pedestal_stream;
  hallc_bcms_pedestal_output.clear();
  hallc_bcms_pedestal_output.open(Form("hallc_bcm_pedestal_%s.txt", run_number));
  hallc_bcms_pedestal_stream.clear();

 
  // // std::cout << "size " << hallc_bcms_list.size() << std::endl;
  for (i=0; i < hallc_bcms_list.size(); i++) {

    // exclude sca_bcm results into an output file.

    if(not hallc_bcms_list.at(i).GetName().Contains("sca")) {
      hallc_bcms_list.at(i).SetFileStream();
      hallc_bcms_pedestal_stream << hallc_bcms_list.at(i);
    }
  } 
  hallc_bcms_pedestal_output << hallc_bcms_pedestal_stream.str();
  std::cout << "\n" << hallc_bcms_pedestal_stream.str() <<std::endl;
  hallc_bcms_pedestal_output.close();
      
  theApp.Run();
  return 0;
}



Bool_t
calibrate(BeamMonitor &device, BeamMonitor &reference)
{

  std::cout << "\n";
 
  std::cout << reference << std::endl;

  TString plotcommand;
  TString device_name;
  TString device_samples;
  TString reference_name;
  Bool_t  sca_flag = false;
  Double_t fit_range[2] = {0.0};
  TCut device_cut = "";
  TH1D* device_hist = NULL;
  TF1* device_fit = NULL;
  Int_t w = 600;
  Int_t h = 400;
  TCanvas *Canvas = NULL;

  device_name = device.GetName();
  reference_name = reference.GetAliasName();
  if(device_name.Contains("sca")) {
    sca_flag = true;
  }
  Canvas = new TCanvas(device_name.Data() , device_name.Data(), w, h);  

  fit_range[0] = reference.GetFitRangeMin();
  fit_range[1] = reference.GetFitRangeMax();

  if(not sca_flag) {
    device_samples = device_name + ".num_samples";
    device_cut     = Form("%s>0", device_samples.Data());
    device_name    = device_name + ".hw_sum_raw/" + device_samples;
  }
  else {
    device_name = device.GetAliasName();
  }

  plotcommand = device_name + ":" +  reference_name;
  
  Canvas->Clear();
  //  Canvas->Divide(2,1);
  
  //  Canvas -> cd(1);
  device_hist = GetHisto(mps_tree, plotcommand, device_cut, "prof"); // profs : large errors why?
  
  if(not device_hist) {
    std::cout << "Please check  "
	      << device_name
	      << std::endl;
    Canvas ->Close();
    delete Canvas; Canvas = NULL;
    return false;
  }
  device_hist -> SetName(device.GetName().Data());
  device_hist -> GetXaxis() -> SetTitle(reference_name.Data());
  //  device_hist -> GetYaxis() -> SetTitle(device_name.Data());
  device_hist -> SetTitle(Form("%s vs %s", device_name.Data(), reference_name.Data()));
  device_hist -> GetXaxis() ->SetTitle(Form("%s (#muA)", reference_name.Data()));
  device_hist -> Fit("pol1", "E M Q", "", fit_range[0], fit_range[1]);
  device_fit = device_hist  -> GetFunction("pol1");
 
  if(device_fit) {
    device_fit -> SetLineColor(kRed);
    device.SetPed(device_fit->GetParameter(0));
    device.SetPedErr(device_fit->GetParError(0));
    device.SetSlop(device_fit->GetParameter(1));
    device.SetSlopErr(device_fit->GetParError(1));
  }
  else {
  
    device.SetPed(-1);
    device.SetPedErr(-1);
    device.SetSlop(-1);
    device.SetSlopErr(-1);
    Canvas ->Close();
    delete Canvas; Canvas = NULL;
    return false;
  }
  gPad->Update();
  // Canvas -> cd(2);

  
  // mps_tree->Draw(Form("bcm1-%lf:event_number", bcm_vqwk_offset[0][0]), "qwk_bcm1.num_samples>0");
  // bcm_vqwk_tmp[0] = (TH1D*) gPad -> GetPrimitive("htemp");
  // bcm_vqwk_tmp[0] -> SetTitle("corrected qwk_bcm1 vs event_number");
   


  // gPad->Update();
  std::cout << device << std::endl;

  Canvas -> Modified();
  Canvas -> Update();
  return true;
}



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


