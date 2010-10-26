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
//                  Thursday, September 16 02:53:18 EDT 2010, jhlee
//                  Test in progress
//
//          0.0.3 :   Buddhini
//                  - Added residual plots
//
//          0.0.4 :  Monday, September 20 00:46:30 EDT 2010, jhlee
//                  - changed scaler names
//                    now simply sca_bcm1 and sca_bcm2
//                  - fixed plot command for residual plots
//                    thus, sca plots can be printed out
//                  - seperated canvases in calibrate()
//                    from main() in order to see old plots
//                    after programs run (don't need to open
//                    ps file). 
//                  - added "SaveAs" to ROOT  C++ Macro file
//                    [] root -l qwk_bcm1.cxx to see an interactive plot
//                         
//          0.0.5 : Wednesday, October 13 23:23:04 EDT 2010, jhlee
//                  - added "bcm" gain and its error
//                    gain  = 1/slope, gain_error = slope_error/slope^2
//                  - changed the default fit range [10,80] % of unser range
//                  - changed the default naive beam off cut as 518
//                  - added the run number in the histogram title
//
//          0.0.6 : Sunday, October 17, Buddhini
//                  - added use of chained rootfiles.
//                  - Apply Device_Error_Code == 0 check for data being used for plotting.
//
//          0.0.7 : Monday, October 18 03:16:31 EDT 2010. jhlee
//                  - added check_branches function in order to check whether 
//                    the opened ROOT file is valid or not for the BCM calibration.
// 
//          0.0.8 : Tuesday, October 19 11:28:52 EDT 2010, jhlee
//                  - added fitted result on the fitted plot
//                  - renamed several function in order to compare with bpm calibration
//                    easily
//                  - sync BeamMonitor class with hallc_bpm_calib.C
//
//          0.0.9 : Sunday, October 24 00:49:20 EDT 2010, jhlee
//                  - changed the method to find QW_ROOTFILES directory
//                  - added png output
//
//          0.0.10 : Monday, October 25 01:17:55 EDT 2010, jhlee
//                   - improved the way to handle (a) ROOT file(s) (TChain) 
//
//          0.0.11 : Monday, October 25 10:55:36 EDT 2010, jhlee
//                   - replaced event_number by CodaEventNumber
//                     , because event_number is belong to a "helicity" subsystem.
//

// TODO List

// Additional BCM calibration run info
//
//  run    Gain
//  5070   5        * BCM calibration RUN
//                    https://hallcweb.jlab.org/hclog/1008_archive/100806051827.html
//  5260   2        * BCM calibration RUN 
//                    https://hallcweb.jlab.org/hclog/1008_archive/100810204416.html
//  ???? 
//  5669   2
//  5807   2        * BCM calibration RUN 

//  5889 - 6114      BCM gain setting 2
//  6115 - 6158      BCM gain setting 5  >>  BCM calibration RUN 6158 / ./hallc_bcm_calib -r 6158 -e 5:60 
//  6159 -           BCM gain setting 2  

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
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TUnixSystem.h"

#include "TStopwatch.h"

#include "TCut.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TStyle.h"
#include "TSystem.h"


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
  void SetSlop(const Double_t in) {slope[0] = in; SetGain(in);};
  void SetSlopErr(const Double_t in) {slope[1] = in; SetGainErr();} // must calculate gain[1] error later, jhlee
 
  void SetFitRange(const Double_t in[2]) {fit_range[0] = in[0]; fit_range[1] = in[1];};
  void SetReference()  {reference_flag = true;};
  void SetFileStream() {filestream_flag = true;};
  void SetBPM()        {bpm_flag = true;}

  TString GetName() {return name;};
  const char* GetCName() {return name.Data();};
  TString GetAliasName() {return alias_name;};
  const char* GetAliasCName() {return alias_name.Data();};
  Double_t GetPed() {return offset[0];};
  Double_t GetPedErr() {return offset[1];};
  Double_t GetSlope() {return slope[0];};
  Double_t GetSlopErr() {return slope[1];};
  // Double_t GetMean() {return mean[0];};
  // Double_t GetMeanErr() {return mean[1];};
  // Double_t GetRMS() {return rms[0];};
  // Double_t GetRMSErr() {return rms[1];};
  Double_t GetFitRangeMin() {return fit_range[0];};
  Double_t GetFitRangeMax() {return fit_range[1];};
  
  Bool_t IsReference()  {return reference_flag;};
  Bool_t IsFileStream() {return filestream_flag;};
  Bool_t IsBPM()        {return bpm_flag;};  

private:
  TString name;
  TString alias_name;
  Double_t offset[2];
  Double_t slope[2];
  Double_t gain[2];
  // Double_t mean[2];
  // Double_t rms[2];
  Double_t fit_range[2];
  Bool_t   reference_flag;
  Bool_t   filestream_flag;
  Bool_t   bpm_flag;
  
  void SetGain(Double_t in);
  void SetGainErr();

};

BeamMonitor::BeamMonitor()
{
  name.Clear();
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope [0] = 0.0;
  slope [1] = 0.0;
  gain  [0] = 0.0;
  gain  [1] = 0.0;

  // mean[0] = 0.0;
  // mean[1] = 0.0;
  // rms[0] = 0.0;
  // rms[1] = 0.0;

  fit_range[0]    = 0.0;
  fit_range[1]    = 0.0;
  reference_flag  = false;
  filestream_flag = false;
  bpm_flag        = false;
};


BeamMonitor::BeamMonitor(TString in)
{
  name = in;
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope [0] = 0.0;
  slope [1] = 0.0;
  gain  [0] = 0.0;
  gain  [1] = 0.0;

  // mean[0] = 0.0;
  // mean[1] = 0.0;
  // rms[0] = 0.0;
  // rms[1] = 0.0;

  fit_range[0]    = 0.0;
  fit_range[1]    = 0.0;
  reference_flag  = false;
  filestream_flag = false;
  bpm_flag        = false;

};


void 
BeamMonitor::SetGain(Double_t in)
{
  if( IsBPM() ) {
    gain[0] = 1.0;
  }
  else {
    if(in not_eq 0.0) gain[0] = 1.0/in;
    else              gain[0] = 0.0;
  }
  return;
};


void 
BeamMonitor::SetGainErr()
{
  if( IsBPM() ) {
    gain[1] = 0.0;
  }
  else {
    Double_t slope2 = 0.0;
    
    slope2 = slope[0]*slope[0];
    
    if(slope2 not_eq 0.0)  gain[1] = slope[1]/slope2;
    else                   gain[1] = 0.0;
  }
  return;
};



std::ostream& operator<< (std::ostream& stream, const BeamMonitor &device)
{
  TString device_name = device.name;

  Int_t file_output_width = 20;
  Int_t name_output_width = 14;
  Int_t term_output_width = 14;
  Int_t file_precision    = 10;

  if(device.filestream_flag) { 
    stream << std::setprecision(file_precision);
    stream << std::setiosflags(std::ios_base::scientific);
    stream <<  std::setw(name_output_width) << device_name;
    stream << ", ";
    stream <<  std::setw(file_output_width) << device.offset[0];
    stream << ", " ;
    stream <<  std::setw(file_output_width) << device.offset[1];
    stream << ", " ;
    if(not device.bpm_flag) {
      stream <<  std::setw(file_output_width) << device.slope[0];
      stream << ", " ;
      stream <<  std::setw(file_output_width) << device.slope[1];
      stream << ", " ;
    }
    stream <<  std::setw(file_output_width) << device.gain[0];
    stream << ", " ;
    stream <<  std::setw(file_output_width) << device.gain[1];
    stream << "\n";
  }
  else {

    if(device.reference_flag) {
      stream <<  std::setw(name_output_width) 
	     << "Reference : " 
	     <<  std::setw(name_output_width) 
	     <<  device_name;
    }
    else {
      stream <<  std::setw(name_output_width) 
	     << " Name : " 
	     <<  std::setw(name_output_width) 
	     << device_name;
    }

    stream << " Offset : "  << std::setw(term_output_width) << device.offset[0];
    stream << " +- "        << std::setw(term_output_width) << device.offset[1];
    stream << "\n";
    
    if(device.reference_flag) {
      stream << std::setfill(' ') << std::setw(38);
      stream << " Fit Range : "   << std::setw(term_output_width) << device.fit_range[0];
      stream << " -- "            << std::setw(term_output_width) << device.fit_range[1];
    }
    else {
      stream << std::setfill(' ') << std::setw(38);
      stream << " Slope  : "      << std::setw(term_output_width) << device.slope[0];
      stream << " +- "            << std::setw(term_output_width) << device.slope[1];
      stream << " \n";
      stream << std::setfill(' ') << std::setw(38);
      stream << " Gain   : "      << std::setw(term_output_width) << device.gain[0];
      stream << " +- "            << std::setw(term_output_width) << device.gain[1];
    }
  }
  return stream;
}




void 
print_usage(FILE* stream, int exit_code);

Bool_t 
bcm_calibrate(BeamMonitor &device, BeamMonitor &reference, const char* run_number);


Bool_t
check_bcm_branches(std::vector<TString> &branches, TTree *roottree)
{

  Bool_t local_debug = true;

  std::size_t branches_size = 0;     
  branches_size = branches.size(); 

  Bool_t branch_valid = true; 
  // must be true, if not
  // even if all branches are found in the ROOT file
  // it returns false
  // Monday, October 18 02:42:41 EDT 2010, jhlee

  std::size_t branches_idx = 0;
  
  for (branches_idx=0; branches_idx<branches_size; branches_idx++) 
    {
      if(local_debug) std::cout << std::setw(30) << branches.at(branches_idx);
      
      TBranch* branch = roottree -> FindBranch(branches.at(branches_idx).Data());
      if(branch) { 
	branch_valid &= true;
	if(local_debug) std::cout << " : true" << std::endl;
      }
      else  {
	branch_valid &= false;
	if(local_debug) std::cout << " : *** false *** " << std::endl;
      }
    }
  return branch_valid;

};

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

Int_t 
GetTree(TString filename, TChain* chain)
{
  TString file_dir;
  Int_t   chain_status = 0;

  file_dir = gSystem->Getenv("QWSCRATCH");
  if (!file_dir) file_dir = "~/scratch/";
  file_dir += "/rootfiles/";
 
  chain_status = chain->Add(Form("%s%s", file_dir.Data(), filename.Data()));
  
  if(chain_status) {
    
    TString chain_name = chain -> GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;
    while (( chain_element=(TChainElement*)next() )) 
      {
	std::cout << "File:  " 
		  << chain_element->GetTitle() 
		  << " is added into the Chain with the name ("
		  << chain_name
		  << ")."
		  << std::endl;
      }
  }
  else {
    std::cout << "There is (are) no "
	      << filename 
	      << "."
	      << std::endl;
  }
  return chain_status;
};

const char* program_name;
std::vector<BeamMonitor> hallc_bcms_list;

TChain  *mps_tree_in_chain  = NULL;
TCanvas *unser_canvas       = NULL;
TCanvas *bcm_canvas         = NULL;
TString  bcm_plots_filename;


Int_t
main(int argc, char **argv)
{
 
  TApplication theApp("App", &argc, argv);

  char* run_number = NULL;

  Double_t naive_beam_off_cut = 0.0;
  Double_t fit_range[2] = {0.0};

  Bool_t file_flag         = false;
  Bool_t fit_range_flag    = false;
  Bool_t unser_offset_flag = false;

    // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatH(0.1);
  gStyle->SetStatW(0.2);
  program_name = argv[0];

  int cc = 0; 

  while ( (cc= getopt(argc, argv, "r:i:e:")) != -1)
    {
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
	    naive_beam_off_cut = atof(optarg);
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
    }
  
  if(not fit_range_flag) {
    fit_range[0] = 0.10;
    fit_range[1] = 0.80;
  }
  else {
    fit_range[0] = fit_range[0]/100.0;
    fit_range[1] = fit_range[1]/100.0;
    
  }
  if(unser_offset_flag) {
    naive_beam_off_cut = naive_beam_off_cut*1e3;
  }
  else {
    naive_beam_off_cut = 318*1e3;   
  }
  
  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }


  // any other way to handle these?
  // Monday, October 18 03:25:55 EDT 2010, jhlee
  std::vector<TString> branches_for_bcm_calibration;

  branches_for_bcm_calibration.push_back("sca_4mhz");
  branches_for_bcm_calibration.push_back("sca_unser");
  branches_for_bcm_calibration.push_back("sca_bcm1");
  branches_for_bcm_calibration.push_back("sca_bcm2");
  branches_for_bcm_calibration.push_back("qwk_bcm1");
  branches_for_bcm_calibration.push_back("qwk_bcm2");
  branches_for_bcm_calibration.push_back("qwk_bcm5");
  branches_for_bcm_calibration.push_back("qwk_bcm6");
  branches_for_bcm_calibration.push_back("CodaEventNumber");


  TString temp;
  TString unser_branch_name;
  TString clock_name;

  for(std::size_t i=0; i<branches_for_bcm_calibration.size(); i++)
    {
      temp = branches_for_bcm_calibration.at(i);
      if( temp.Contains("bcm") ) {
	hallc_bcms_list.push_back(BeamMonitor(temp));
      }
      else if( temp.Contains("unser")) {
	unser_branch_name = temp;
      }
      else if( temp.Contains("4mhz")) {
	clock_name = temp;
      }
  
    }

  BeamMonitor sca_unser(unser_branch_name);
  sca_unser.SetReference();

  mps_tree_in_chain = new TChain("Mps_Tree");
  
  TString bcm_calibration_filename = Form("BCMCalib_%s*.root", run_number);
  Int_t chain_status = 0;
  chain_status = GetTree(bcm_calibration_filename, mps_tree_in_chain);

  if(chain_status == 0) {
    exit(1);
  }

  Bool_t bcm_valid_rootfiles = false;
  bcm_valid_rootfiles = check_bcm_branches(branches_for_bcm_calibration,mps_tree_in_chain);
  
  if(bcm_valid_rootfiles) {
    std::cout << "all branches are in the root file(s)." << std::endl;
  }
  else {
    printf("\nThe root file, which you selected by run number, is invalid for the BCM calibration.\n");
    printf("Plese run the following command in order to create a right ROOT file.\n");
    printf("------------------------------------------ \n");
    printf(" qwparity -r %s -c hallc_bcm.conf\n", run_number);
    printf("------------------------------------------ \n");
    exit(-1);    
  }

  // // TEST begin without using Draw()

  // //  Bool_t test_debug = true;
  // Double_t qwk_sca_unser = 0.0;
  // mps_tree->SetBranchAddress("qwk_sca_unser", &qwk_sca_unser);
  // Int_t nentries = (Int_t) mps_tree -> GetEntries();


  // // 1) vector 
  // std::vector<Double_t> unser;
  // TStopwatch timer;
  // timer.Start();

  // for (Int_t i=0; i<nentries; i++) 
  //   {
  //     mps_tree -> GetEntry(i);
  //     unser.push_back(qwk_sca_unser);
      
  //     // if(test_debug) {
  //     // 	if(i < 100)
  //     // 	  printf("%d unser qwk_sca_unser %12.4lf\n", i, qwk_sca_unser);
  //     // }
  //   }

  // std::cout << unser.size() << std::endl;

  // timer.Stop();
  // printf("RealTime=%f seconds, CpuTime=%f seconds\n", timer.RealTime(), timer.CpuTime());


  // // 2) array
  // Double_t *a_unser = new Double_t[nentries];
  // timer.Start();
  //  for (Int_t i=0; i<nentries; i++) 
  //   {
  //     mps_tree -> GetEntry(i);
  //     a_unser[i] = qwk_sca_unser;
      
  //   }
  //  // delete [] a_unser;

  //  timer.Stop(); 
  //  printf("RealTime=%f seconds, CpuTime=%f seconds\n", timer.RealTime(), timer.CpuTime());


  // TEST end


  Int_t w = 1200;
  Int_t h = 600;
  unser_canvas = new TCanvas("sca_unser","SCA Unser", w, h);  
  //
  // extract unser maximum range in order to determine fit range
  //

  // Clock Corrected qwk_sca_unser;

  //  TString clock_name = "sca_4mhz";
  
  mps_tree_in_chain -> SetAlias("clock_correct", Form("4e6/%s", clock_name.Data())); 
  mps_tree_in_chain -> SetAlias("cc_sca_unser",  Form("clock_correct*%s", sca_unser.GetCName()));

  mps_tree_in_chain -> SetAlias("cc_sca_bcm1",  "clock_correct*sca_bcm1");
  mps_tree_in_chain -> SetAlias("cc_sca_bcm2",  "clock_correct*sca_bcm2");
  hallc_bcms_list.at(0).SetAliasName("cc_sca_bcm1");
  hallc_bcms_list.at(1).SetAliasName("cc_sca_bcm2");


  unser_canvas -> Divide(2,1);
  unser_canvas -> cd(1);
  
  TH1D* sca_unser_event = NULL;
  sca_unser_event = GetHisto(mps_tree_in_chain, "cc_sca_unser:CodaEventNumber", "");
  if(not sca_unser_event) {
    std::cout << "Please check clock corrected sca_unser:CodaEventNumber"
	      << std::endl;
    theApp.Run();
    return 0;
  }

  sca_unser_event -> SetTitle(Form("Run %s  cc_sca_unser:CodaEventNumber", run_number));
  gPad->Update();

  Double_t unser_max = sca_unser_event -> GetYaxis() -> GetXmax();


  //
  // extract unser offset, offset error, slope, and slope error.
  //
  unser_canvas -> cd(2);

  TCut unser_cut(Form("%s<%lf", "cc_sca_unser",  naive_beam_off_cut));

  TH1D* cc_sca_unser_gaus = GetHisto(mps_tree_in_chain, "cc_sca_unser", unser_cut);

  if(not cc_sca_unser_gaus) {
    std::cout << " This programs doesn't handle this run correctly. \n"
  	      << " Please report this problem via an email to jhlee@jlab.org\n"
	      << " or\n You can adjust unser beam off offset. See usage."
  	      << std::endl;
    theApp.Run();
  }
  cc_sca_unser_gaus -> SetTitle(Form("Run %s cc_sca_unser", run_number));
  cc_sca_unser_gaus -> Fit("gaus", "M");

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
    mps_tree_in_chain->SetAlias(unser_name.Data(), Form("((cc_sca_unser-%lf)*%lf)", mean, unser_scaler_unit));
    sca_unser.SetAliasName(unser_name.Data());
    sca_unser.SetPed(mean);
    sca_unser.SetPedErr(unser_fit -> GetParError(1));
    sca_unser.SetFitRange(fit_range);
    std::cout << sca_unser << std::endl;
  }
  else {
    std::cout << "This program does not fit the Unser BCM correctly."
	      << "Please report this problem via an email to jhlee@jlab.org"
	      << std::endl;
    theApp.Run();
  }



  unser_canvas -> Modified();
  unser_canvas -> Update();
 
  // 
  // Save as ROOT C++ Macro and png of unser_canvas
  //
  TString output_name = unser_canvas->GetName();
  unser_canvas -> SaveAs(Form("BCMCalib%s_%s.cxx", run_number, output_name.Data()));
  unser_canvas -> SaveAs(Form("BCMCalib%s_%s.png", run_number, output_name.Data()));

  // Print the plot on to a file
  // file containing the bcm calibration plots

  bcm_plots_filename = "_hallc_bcm_calib_plots.ps";
  bcm_plots_filename.Insert(0, run_number);

  // Open ps file
  unser_canvas -> Print(bcm_plots_filename+"[");
  // Save "unser" plot into the ps file.
  unser_canvas -> Print(bcm_plots_filename);

  std::size_t i = 0; 
  Int_t cnt = 0;
  std::cout << "how many bcms at Hall C ? " << hallc_bcms_list.size() << std::endl;

  for (i=0; i<hallc_bcms_list.size(); i++) 
    //  for (i=2; i<3; i++)  for only qwk_bcm1
    {
      std::cout << "\n" 
		<< cnt++ 
		<< ": onto calibrating " << hallc_bcms_list.at(i).GetName() 
		<< std::endl;
      Bool_t check = false;
      check = bcm_calibrate(hallc_bcms_list.at(i), sca_unser, run_number) ; 
      //  if(not check) theApp.Run();
    }

  /* Close ps file */
  unser_canvas -> Print(bcm_plots_filename+"]");

  /* open a file to store the calibration results for qwk_bcm1, qwk_bcm2, qwk_bcm5, and qwk_bcm6*/
  std::ofstream       hallc_bcms_pedestal_output;
  std::ostringstream  hallc_bcms_pedestal_stream;
  hallc_bcms_pedestal_output.clear();

  hallc_bcms_pedestal_output.open(Form("hallc_bcm_pedestal_%s.txt", run_number));
  hallc_bcms_pedestal_stream.clear();

  for (i=0; i < hallc_bcms_list.size(); i++) 
    {
      /* exclude sca_bcm results from the output to the file. */
      if(not hallc_bcms_list.at(i).GetName().Contains("sca")) {
	hallc_bcms_list.at(i).SetFileStream();
	hallc_bcms_pedestal_stream << hallc_bcms_list.at(i);
      }
    } 

  //  time_t theTime;
  // time(&theTime);
  //hallc_bcms_pedestal_output <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" 
  // 			     <<"!  HallC BCM pedestals (per sample"<<std::endl;
  // hallc_bcms_pedestal_output <<"!  Date of analysis "<<ctime(&theTime)<<std::endl;
  // hallc_bcms_pedestal_output <<"!  device        , pedestal ,error,     slope,error,     gain,error\n"
  // 			     <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
  // 			     <<std::endl;

  hallc_bcms_pedestal_output << hallc_bcms_pedestal_stream.str();
  std::cout << "\n" << hallc_bcms_pedestal_stream.str() <<std::endl;
  hallc_bcms_pedestal_output.close();
  
  
  theApp.Run();
  return 0;
}



Bool_t
bcm_calibrate(BeamMonitor &device, BeamMonitor &reference, const char* run_number )
{

  std::cout << "\n";
 
  std::cout << reference << std::endl;

  TString plotcommand;
  TString plot_residual_command;

  TString device_name;
  TString device_samples;
  
  TString reference_name;
  TString residual_name;

  Bool_t  sca_flag = false;
  Double_t fit_range[2] = {0.0};
  TCut device_cut = "";
  TH1D* device_hist = NULL;
  TF1* device_fit = NULL;
  TH1D * device_res = NULL;
  Int_t w = 1200;
  Int_t h = 600;


  //  unser_canvas->Clear();

  device_name = device.GetName();
  reference_name = reference.GetAliasName();
  if(device_name.Contains("sca")) {
    sca_flag = true;
  }
  
  bcm_canvas = new TCanvas(device_name.Data() , device_name.Data(), w, h);  

  fit_range[0] = reference.GetFitRangeMin();
  fit_range[1] = reference.GetFitRangeMax();

  if(not sca_flag) {
    device_samples = device_name + ".num_samples";
    device_cut     = device_name + ".Device_Error_Code == 0";
    device_name    = device_name + ".hw_sum_raw/" + device_samples;
  }
  else {
    device_name = device.GetAliasName();
  }

  plotcommand = device_name + ":" +  reference_name;
  
 
  bcm_canvas->Divide(1,2);
  bcm_canvas->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  device_hist = GetHisto(
			 mps_tree_in_chain, 
			 plotcommand, 
			 device_cut, 
			 "prof"); // profs : large errors why?
  
  if(not device_hist) {
    std::cout << "Please check  "
	      << device_name
	      << std::endl;
    
    bcm_canvas ->Close();
    delete bcm_canvas; bcm_canvas = NULL;
    
    return false;
  }
  
  device_hist -> SetName(device.GetName().Data());
  device_hist -> GetXaxis() -> SetTitle(reference_name.Data());
  //  device_hist -> GetYaxis() -> SetTitle(device_name.Data());
  device_hist -> SetTitle(Form("Run %s : %s vs %s", 
			       run_number, 
			       device_name.Data(), 
			       reference_name.Data()
			       )
			  );
  device_hist -> GetXaxis() ->SetTitle(Form("%s (#muA)", reference_name.Data()));
  device_hist -> Fit("pol1", "E M R F Q", "", fit_range[0], fit_range[1]);
  device_hist -> Draw("");
  device_fit = device_hist  -> GetFunction("pol1");
  
  
  if(device_fit) {
    device_fit -> SetLineColor(kRed);
    device.SetPed    (device_fit->GetParameter(0));
    device.SetPedErr (device_fit->GetParError(0));
    device.SetSlop   (device_fit->GetParameter(1));
    device.SetSlopErr(device_fit->GetParError(1));

   // Draw the residuals
    bcm_canvas->cd(2);
    gPad->SetGridx();
    gPad->SetGridy();
    // device_name is already well determined before
    // now we don't need to distinguish between sca and vqwk

    device_name = Form("(%s) - (%s*%e + %e)"
		       , device_name.Data()
		       , reference_name.Data()
		       , device.GetSlope()
		       , device.GetPed()
		       );

    plot_residual_command = device_name + ":" +  reference_name;

    device_res = GetHisto(mps_tree_in_chain, plot_residual_command, device_cut, "BOX");

    if (not device_res) {
      std::cout << " Please check residual plot : "
		<< plot_residual_command
		<<std::endl;

      return false;	
    }
    
    device_res -> GetXaxis()->SetTitle("current (#muA)");
    device_res -> GetYaxis()->SetTitle("fit residual");
    device_res -> GetYaxis()->SetTitleOffset(2.2);
    device_res -> SetTitle(device_name+" fit residual");
      
  }
  else {
  
    device.SetPed(-1);
    device.SetPedErr(-1);
    device.SetSlop(-1);
    device.SetSlopErr(-1);
    bcm_canvas->Close();
    delete bcm_canvas; bcm_canvas = NULL;
    return false;
  }

  std::cout << device << std::endl;

  bcm_canvas -> Modified();
  bcm_canvas -> Update();
  //
  // Print bcm_canvas into a ps file that is defined in main() function
  // 
  bcm_canvas -> Print(bcm_plots_filename);

  // 
  // Save as ROOT C++ Macro and png of bcm_canvas. 
  //
  TString root_c_name = bcm_canvas->GetName();
  bcm_canvas -> SaveAs(Form("BCMCalib%s_%s.cxx", run_number, root_c_name.Data()));
  bcm_canvas -> SaveAs(Form("BCMCalib%s_%s.png", run_number, root_c_name.Data()));

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


