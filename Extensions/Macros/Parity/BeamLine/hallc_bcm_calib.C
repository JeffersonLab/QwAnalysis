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
//          0.0.12 : Saturday, November 13 21:48:48 EST 2010, jhlee
//                   - added sca_bcm17
//
//          0.0.13 : Wednesday, November 17 15:52:53 EST 2010, jhlee
//                   - removed the naive method to find the unser pedestral
//                     added the better way to determine it,
//                     but it works with a well-measured calibration BCM
//                     calibration run. (5070, 6846)
//          0.0.14 : Thursday, November 18 12:03:36 EST 2010, jhlee
//                   - cleaned up codes and added some commments
//
//          0.0.15 : Friday, December  3 00:15:25 EST 2010, jhlee
//                  - changed fit_range from % to current (uA)
//                    if there is no default range, 10 - max uA is used
//
//          0.0.16 : Friday, December  3 17:07:32 EST 2010, jhlee
//                  - removed some warnings during compiling
//                  - prepared to replace all draw commands to extract data points
//                  - adjusted the position of TPaveStat on bcm fitting pad.
//
//          0.0.17 : Tuesday, January 25 22:36:42 EST 2011, jhlee
//                  - added units on axis
//                  - added one more peak search routine, that is useful for
//                    the ramping current does not exist.

//          0.0.18 : Friday, November 18 15:58:19 EST 2011, jhlee
//                  - changed a method to access sca....
//
//
// TODO List
// 1) replaced all Draw command with real data points


//
// Additional BCM calibration run info
//
//  run    Gain
//  5070   5        * BCM calibration RUN
//                    https://hallcweb.jlab.org/hclog/1008_archive/100806051827.html
//  5260   2        * BCM calibration RUN
//                    https://hallcweb.jlab.org/hclog/1008_archive/100810204416.html
//  ????
//
//  5669             BCM gain setting 2
//  5807             BCM gain setting 2   * BCM calibration RUN
//                   https://hallcweb.jlab.org/hclog/1010_archive/101013045438.html
//                   ./hallc_bcm_calib -r 5807 -e 10:75
//                   https://hallcweb.jlab.org/hclog/1010_archive/101014011357.html

//  5889 - 6114      BCM gain setting 2
//  6115 - 6158      BCM gain setting 5  >>  BCM calibration RUN 6158 / ./hallc_bcm_calib -r 6158 -e 5:60
//  6159 - 6301      BCM gain setting 2
//  6302 - 6542      BCM gain setting 5
//                   https://hallcweb.jlab.org/hclog/1010_archive/101029193112.html
//  6543 - 6845      BCM gain setting
//  6846 - 7756      BCM gain setting 2  >> BCM calibration RUN 6846 / 20uA - 150uA
//                   https://hallcweb.jlab.org/hclog/1011_archive/101110223106.html
//
//  7757 - 8350      BCM gain setting 2 >> BCM calibration RUN 7757/ 0uA - 140uA
//                   https://hallcweb.jlab.org/hclog/1012_archive/101202051245.html
//                   https://hallcweb.jlab.org/hclog/1012_archive/101202063008.html
//                   https://hallcweb.jlab.org/hclog/1012_archive/101204145444.html


//                   BCM gain setting 2
//                   https://hallcweb.jlab.org/hclog/1012_archive/101211001641.html


//  8351-8394        https://hallcweb.jlab.org/hclog/1012_archive/101221062621.html
//
//  8351-8369        BCM calibration RUN 8351 (*)
//                   https://hallcweb.jlab.org/hclog/1012_archive/101219234658.html
//
//  8370-8394        BCM calibration RUN 8370 (*)
//                   https://hallcweb.jlab.org/hclog/1012_archive/101220200033.html
//                   https://hallcweb.jlab.org/hclog/1012_archive/101221120541.html
//
//  8395-8396        BCM gain setting 3
//                   https://hallcweb.jlab.org/hclog/1012_archive/101221042257.html
//                   only 8395, 8396 is junk
//
//
//  8397-            BCM gain setting 2
//                   https://hallcweb.jlab.org/hclog/1012_archive/101221061819.html
//                   https://hallcweb.jlab.org/hclog/1012_archive/101221062342.html
//                   https://hallcweb.jlab.org/hclog/1012_archive/101221062621.html
//
//
//
// 9273              BCM gain setting 2
//                   https://hallcweb.jlab.org/hclog/1101_archive/110120230848.html
//                   https://hallcweb.jlab.org/hclog/1101_archive/110125222318.html


#define ITMAX 100
#define MAXN  1000
#define EPS 3.0e-7
#define FPMIN 1.0e-30 //Number near the smallest representable floating-point number.



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

#include "TSpectrum.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "TLine.h"

#include "TPaveStats.h"
#include "TCut.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TLeaf.h"

Bool_t output_flag = true;


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
  Double_t GetGain() {return gain[0];};
  Double_t GetGainErr() {return gain[1];};
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
    stream << std::setw(name_output_width) << device_name;
    stream << " ";
    stream << std::setw(file_output_width) << device.offset[0];
    stream << " " ;
    stream << std::setw(file_output_width) << device.offset[1];
    stream << " " ;
    if(not device.bpm_flag) {
      stream << std::setw(file_output_width) << device.slope[0];
      stream << " " ;
      stream << std::setw(file_output_width) << device.slope[1];
      stream << " " ;
    }
    stream << std::setw(file_output_width) << device.gain[0];
    stream << " " ;
    stream << std::setw(file_output_width) << device.gain[1];
    //    stream << "\n";
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
      stream << " +- "            << std::setw(term_output_width) << device.fit_range[1];
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
bcm_calibrate_draw(BeamMonitor &device, BeamMonitor &reference, const char* run_number);

Bool_t
bcm_calibrate_nodraw(BeamMonitor &device, BeamMonitor &reference, const char* run_number);

Bool_t
Unser_calibrate(BeamMonitor &unser, TString clock_name, const char* run_number);

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

  //  printf("GetHisto:Name %s\n", name.Data());
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



Double_t
background(Double_t *x, Double_t *par)
{
  return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
};

Double_t
gaussian(Double_t *x, Double_t *par)
{

  Double_t norm  = par[0]; // par[3] in fitFunction
  Double_t mean  = par[1]; // par[4] in fitFunction
  Double_t sigma = par[2]; // par[5] in fitFunction
  Double_t arg = 0.0;
  if(sigma not_eq 0.0) arg = (x[0]-mean)/sigma;
  else                 return 1.e30; // the same as TMath::Gaus // not sure it works??
  Double_t res   = TMath::Exp(-0.5*arg*arg);

  return norm*res;

};



Double_t
fitFunction(Double_t *x, Double_t *par)
{
  return background(x,par)+gaussian(x,&par[3]);
};



void nrerror(const char *error_text);
double gammln(double xx);
void gser(double *gamser, double a, double x, double *gln);
void gcf(double *gammcf, double a, double x, double *gln);
double gammp(double a, double x);
double gammq(double a, double x);


const char* program_name;
std::vector<BeamMonitor> hallc_bcms_list;

TChain  *mps_tree_in_chain  = NULL;
TCanvas *unser_canvas       = NULL;
TCanvas *bcm_canvas         = NULL;
TString  g_bcm_plots_filename;



Int_t
main(int argc, char **argv)
{

  TApplication theApp("App", &argc, argv);

  char* run_number = NULL;

  //  Double_t naive_beam_off_cut = 0.0;
  Double_t fit_range[2] = {0.0};

  Bool_t file_flag         = false;
  Bool_t fit_range_flag    = false;
  //  Bool_t unser_offset_flag = false;

    // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatH(0.1);
  gStyle->SetStatW(0.2);
  program_name = argv[0];

  int cc = 0;

  while ( (cc= getopt(argc, argv, "r:e:")) != -1)
    {
      switch (cc)
	{
	case 'r':
	  {
	    file_flag = true;
	    run_number = optarg;
	  }
	  break;
	// case 'i':
	//   {
	//     unser_offset_flag    = true;
	//     naive_beam_off_cut = atof(optarg);
	//   }
	//   break;
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
	    if ((fit_range[0] > fit_range[1]) || (fit_range[0] < 0) || (fit_range[1] > 180.0) )
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
    fit_range[0] = 10.0;
    fit_range[1] = -10.0;
    // if the fit range is not defined, we use the 10uA as lower limit
    // and extract the maximum value for the upper limit.

    // fit_range[0] = 0.15;
    // fit_range[1] = 0.95;
  }

  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }


  // any other way to handle these?
  // Monday, October 18 03:25:55 EDT 2010, jhlee
  std::vector<TString> branches_for_bcm_calibration;

  // open the file with beamline elements
  std::ifstream inputfile;
  inputfile.open("hallc_bcm_calib.txt");
  while(inputfile.good()) {
    TString channel;
    inputfile >> channel;
    if (channel.Length() == 0) continue;
    branches_for_bcm_calibration.push_back(channel);
  }
  inputfile.close();


  TString unser_branch_name;
  TString clock_name;

  std::size_t i = 0;
  for(i=0; i<branches_for_bcm_calibration.size(); i++)
    {
      TString temp = branches_for_bcm_calibration.at(i);
      TString temp_lc = temp; temp_lc.ToLower();
      if( temp_lc.Contains("bcm") ) {
	hallc_bcms_list.push_back(BeamMonitor(temp));
      }
      else if( temp_lc.Contains("unser")) {
	unser_branch_name = temp;
	//	unser_branch_name += ".raw";
      }
      else if( temp_lc.Contains("4mhz")) {
	clock_name = temp;
	//	clock_name += ".raw";
      }
      else
        std::cout << "(ignored " << temp << ")" << std::endl;
    }


  // define unser, and set it as reference
  BeamMonitor sca_unser(unser_branch_name);
  sca_unser.SetReference();

  // define chain to access tree in (a) root file(s)
  mps_tree_in_chain = new TChain("Mps_Tree");

  TString bcm_calibration_filename = Form("BCMCalib_%s*.root", run_number);
  Int_t chain_status = 0;
  chain_status = GetTree(bcm_calibration_filename, mps_tree_in_chain);
  if(chain_status == 0) {
    exit(1);
  }

  // check the rootfiles are valid for the BCM calibration
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

  // do the 4MHz clock corrections
  mps_tree_in_chain -> SetAlias("clock_correct", Form("4e6/%s.raw", clock_name.Data()));
  mps_tree_in_chain -> SetAlias("cc_sca_unser",  Form("clock_correct*%s.raw", sca_unser.GetCName()));
  for(i=0; i<hallc_bcms_list.size(); i++)
    {
      TString name = hallc_bcms_list.at(i).GetName();
      mps_tree_in_chain -> SetAlias("cc_" + name,  "clock_correct*" + name);
      hallc_bcms_list.at(i).SetAliasName("cc_" + name);
    }
  // Print the plot on to a file
  // file containing the bcm calibration plots

  g_bcm_plots_filename = Form("%s_hallc_bcm_calib_plots.ps",run_number);

  //
  // deliver the initial fit_range in order to scale them to unser max current range
  //
  Bool_t unser_calib_flag = false;
  sca_unser.SetFitRange(fit_range);

  //
  // Unser calibration
  //
  unser_calib_flag = Unser_calibrate(sca_unser, clock_name, run_number);
  if(not unser_calib_flag) {
    printf("Unser calibration is failed, no meaning to go further.\n");
    printf("Ctrl+c will be used to close this program.\n");
    theApp.Run();
  }

  Bool_t only_unser_flag = false;

  if(! only_unser_flag) {
    Int_t cnt = 0;
    std::cout << "how many bcms at Hall C ? " << hallc_bcms_list.size() << std::endl;

    for (i=0; i<hallc_bcms_list.size(); i++)
      //for (i=3; i<4; i++) // for only qwk_bcm1
      {
	std::cout << "\n"
		  << cnt++
		  << ": onto calibrating " << hallc_bcms_list.at(i).GetName()
		  << std::endl;
	Bool_t check = false;
	check = bcm_calibrate_draw(hallc_bcms_list.at(i), sca_unser, run_number) ;
	//check = bcm_calibrate_nodraw(hallc_bcms_list.at(i), sca_unser, run_number) ;
	//  if(not check) theApp.Run();
      }


    // Close ps file
    if(output_flag) {
      unser_canvas -> Print(g_bcm_plots_filename+"]");
      //  open a file to store the calibration results for
      //  qwk_bcm1, qwk_bcm2, qwk_bcm5, and qwk_bcm6
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
	    hallc_bcms_pedestal_stream << hallc_bcms_list.at(i) << "\n";
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


      Double_t ped     = 0.0;
      //   Double_t pederr  = 0.0;
      Double_t gain    = 0.0;
      //      Double_t gainerr = 0.0;

      printf("\n======= copy/& paste  for qweak_hallc_pedestal.xxx-.map  file =========\n\n");
      for (i=0; i < hallc_bcms_list.size(); i++)
	{
	  ped     = hallc_bcms_list.at(i).GetPed();
	  //	  pederr  = hallc_bcms_list.at(i).GetPedErr();
	  gain    = hallc_bcms_list.at(i).GetGain();
	  //	  gainerr = hallc_bcms_list.at(i).GetGainErr();

	  printf("%10s, %+20.6lf, %+20.6lf\n", hallc_bcms_list.at(i).GetName().Data(), ped, gain);
	}
      printf("\n\n===================================================================\n");

    }
  }

  theApp.Run();
  return 0;
}

Bool_t
Unser_calibrate(BeamMonitor &unser, TString clock_name, const char* run_number)
{
  Bool_t local_debug = false;
  Int_t i =0;

  TString unser_name = unser.GetName();
  Double_t unser_fit_range[2] = {0.0};
  // get user defined fit ranges
  unser_fit_range[0] = unser.GetFitRangeMin();
  unser_fit_range[1] = unser.GetFitRangeMax();

  Double_t qwk_sca_unser = 0.0;
  Double_t qwk_sca_4mhz  = 0.0;
  Double_t clock_corrected_unser_Hz = 0.0;

  TBranch *b_unser = mps_tree_in_chain->GetBranch(unser_name.Data());
  TBranch *b_4mhz  = mps_tree_in_chain->GetBranch(clock_name.Data());

  TLeaf *l_unser = (TLeaf*) b_unser -> GetLeaf("raw");
  TLeaf *l_4mhz  = (TLeaf*) b_4mhz  -> GetLeaf("raw");

  Int_t nentries = (Int_t)  b_unser -> GetEntries();
  Int_t nbins    = nentries - 1;

   TH1D *unser_TM = new TH1D("UnserTM",
			    Form("Run %s : Unser Timing Module", run_number),
			    nbins, 0, nbins);

  unser_TM -> SetStats(0);
  unser_TM -> GetXaxis() -> SetTitle("event number");
  unser_TM -> GetYaxis() -> SetTitle("Unser (Hz)");

  Double_t four_mega_hertz = 4e+6;
 
  for (i=0; i<nentries; i++)
    {
      b_unser -> GetEntry(i);
      b_4mhz  -> GetEntry(i);
      qwk_sca_4mhz = l_4mhz -> GetValue();
      qwk_sca_unser = l_unser -> GetValue();

      if (qwk_sca_4mhz != 0) {
        clock_corrected_unser_Hz = qwk_sca_unser*four_mega_hertz/qwk_sca_4mhz; // Hz
	if(local_debug) {
	  std::cout << "i " << i
		    << " unser " << qwk_sca_unser
		    << " 4mhz  " << qwk_sca_4mhz
		    << " cc unser Hz "  << clock_corrected_unser_Hz
		    << std::endl;
	}
	unser_TM -> SetBinContent(i, clock_corrected_unser_Hz);
      }

    
    }
 
  Double_t unser_max = 0.0;
  Double_t unser_min = 0.0;

  unser_max = unser_TM->GetBinContent(unser_TM->GetMaximumBin());
  unser_min = unser_TM->GetBinContent(unser_TM->GetMinimumBin());

  Int_t rd_unser_min   = (Int_t) unser_min;
  Int_t rd_unser_max   = (Int_t) (unser_max) + 1 ;
  Int_t nbins_rd_unser = (rd_unser_max - rd_unser_min)/100; // normalize a kHz offset in order to increase "bin size".

  printf(" UnserHist Nbins %d Min %d Max %d\n", nbins_rd_unser, rd_unser_min, rd_unser_max);
  TH1D *unser_hist = new TH1D("UnserHist", Form("Run %s : Unser Histogram", run_number), nbins_rd_unser, rd_unser_min, rd_unser_max);
  unser_hist -> SetMarkerStyle(21);
  unser_hist -> SetMarkerSize(0.8);
  unser_hist -> GetYaxis() -> SetTitle("Counts");
  unser_hist -> GetXaxis() -> SetTitle("Unser (Hz)");
  for (i=0; i<nbins; i++)
    {
      unser_hist->Fill(unser_TM -> GetBinContent(i));
    }

  if(local_debug) printf("entries %d unser_max %lf min %lf\n", nentries, unser_max, unser_min);


  Int_t w = 1000;
  Int_t h = 500;

  unser_canvas = new TCanvas("sca_unser", "sca_unser", w, h);

  unser_canvas -> Divide(2,1);

  unser_canvas -> cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  unser_TM -> Draw();
  gPad->Update();
  unser_canvas -> cd(2);
  gPad->SetGridy();
  Int_t npeaks = 20;
  Int_t nfound = 0 ;

  TSpectrum *s = new TSpectrum(npeaks);
  nfound = s->Search(unser_hist, 2, "noMarkov goff", 0.05);

  if(nfound == 0) {
    // Sometime, we have no backgroud which means no beam ramping stage...
    // TSpectrum returns the following error
    // Error in <TSpectrum::SearchHighRes>: Too large clipping window
    // In that case, try with no background to find the peak...
    // 2011.01.20, run 9273

    nfound = s->Search(unser_hist, 2, "noMarkov goff nobackground", 0.05);
  }


  Float_t *xpeaks = s->GetPositionX();
  printf("Found %d candidate peaks to fit\n",nfound);


  Double_t peak_xpos = 0.0;
  Int_t    peak_bin  = 0;
  Double_t peak_ypos = 0.0;
  // Double_t raw_sigma = 0.0;

  Int_t    unser_peak_idx  = 0;
  Int_t    unser_peak_bin  = 0;
  Double_t unser_peak_xpos = 0.0;
  Double_t unser_peak_ypos = 0.0;
  Double_t unser_raw_sigma = 0.0;

  if ( nfound !=0 ) {


    // try to find the Unser peak that has the smallest bin number
    //
    for(Int_t i=0; i<nfound; i++)
      {
	peak_xpos = (Double_t) xpeaks[i];
	peak_bin  = unser_hist->GetXaxis()->FindBin(peak_xpos);
	if(i==0) {
	  unser_peak_bin = peak_bin;
	}
	else {
	  if(unser_peak_bin > peak_bin) {
	    unser_peak_bin = peak_bin;
	    unser_peak_idx = i;
	  }
	}
	peak_ypos = unser_hist->GetBinContent(peak_bin);
      }

    //
    // get xpos, bin, ypos, estimated width of the Unser peak
    //
    unser_peak_xpos = (Double_t) xpeaks[unser_peak_idx];
    unser_peak_bin  = unser_hist->GetXaxis()->FindBin(unser_peak_xpos);
    unser_peak_ypos = unser_hist->GetBinContent(unser_peak_bin);

    //
    // Run 8351 has the strange Unser signal when beam is off.
    // Thus I use the sigma valus of 1% of  xpos as the initial fitting parameter
    //

    unser_raw_sigma = 0.34*TMath::Abs(0.01*unser_peak_xpos);
    //    unser_raw_sigma = 0.34*TMath::Abs(unser_peak_xpos-unser_min); // 0.5 sigma I selected
    // With Run 8370, the above two methods return the same result. Thus, I want to use
    // the first one.

    printf("Unser Peak %d : peak_xpos %4.2lf peak_bin %4d peak_ypos %4.2lf raw_sigma %4.2lf\n",
	   unser_peak_idx, unser_peak_xpos, unser_peak_bin, unser_peak_ypos, unser_raw_sigma);

    // TH1 *hb = s->Background(unser_hist, 20, "same");
    // if (hb) gPad->Update();



    // Default the number of points to draw TF1 function
    Int_t number_of_points = 1000;

    TF1 *unser_fit = new TF1("fitFcn", fitFunction, unser_min, unser_max, 6);// 6 is the number of parameter in fitFunction

    unser_fit -> SetNpx(number_of_points);
    // unser_fit -> SetParameter(0,1);
    // unser_fit -> SetParameter(1,1);
    // unser_fit -> SetParameter(2,1);

    // give the parameter names to par[i]
    unser_fit -> SetParName(0, "a1");
    unser_fit -> SetParName(1, "a2");
    unser_fit -> SetParName(2, "a3");
    unser_fit -> SetParName(3, "Norm");
    unser_fit -> SetParName(4, "Mean");
    unser_fit -> SetParName(5, "Sigma");

    // initialize three parameters with values from guessing and results from TSpectrum
    unser_fit -> SetParameter(3, unser_peak_ypos);
    unser_fit -> SetParameter(4, unser_peak_xpos);
    unser_fit -> SetParameter(5, unser_raw_sigma);

    // set estimated parameter limits to mean and sigma
    unser_fit -> SetParLimits(4, unser_peak_xpos - unser_raw_sigma, unser_peak_xpos + unser_raw_sigma);
    unser_fit -> SetParLimits(5, 0, unser_peak_xpos-unser_min);

    unser_fit -> SetLineWidth(2);
    unser_fit -> SetLineColor(kRed);

    gStyle -> SetOptFit(1111);
    gStyle -> SetOptStat("ei");

    unser_hist->Fit("fitFcn", "E M B Q");

    if(unser_fit) {

      Double_t height[2] = {0.0};
      Double_t mean[2]   = {0.0};
      Double_t sigma[2]  = {0.0};

      height[0] = unser_fit -> GetParameter("Norm");
      height[1] = unser_fit -> GetParError(3);
      mean[0]   = unser_fit -> GetParameter("Mean");
      mean[1]   = unser_fit -> GetParError(4);
      sigma[0]  = unser_fit -> GetParameter("Sigma");
      sigma[1]  = unser_fit -> GetParError(5);
      printf("Gaussian N: %12.2lf +- %8.2lf, Mean: %12.2lf +-%8.2lf [Hz], Sigma: %12.2lf +-%8.2lf\n",
	     height[0], height[1], mean[0], mean[1], sigma[0], sigma[1]);

      //
      // draw two vertical lines to estimate the unser baseline, just for a nice view
      //

      Double_t vertical_line_xrange[2] = {0.0};
      vertical_line_xrange[0] = mean[0] - 3.0*sigma[0]; // 3 sigma guided lines
      vertical_line_xrange[1] = mean[0] + 3.0*sigma[0];

      TLine* line = new TLine();
      line -> SetLineStyle(3);

      line -> DrawLine(vertical_line_xrange[0], 0, vertical_line_xrange[0], 0.8*unser_peak_ypos);
      line -> DrawLine(vertical_line_xrange[1], 0, vertical_line_xrange[1], 0.8*unser_peak_ypos);

      // get some important (?) values from unser_fit
      // and calculate probabilities
      // double chisq         = unser_fit -> GetChisquare();
      // double ndf           = unser_fit -> GetNDF();
      //   double reduced_chisq = chisq/ndf;

      // double prob          = unser_fit -> GetProb();  // ROOT provide
      // double probQ         = gammq(0.5*ndf, 0.5*chisq); // numerical recipe
      // double probP         = gammp(0.5*ndf, 0.5*chisq); // numerical recipe

      // printf("chisq %6.1lf ndf %3.0lf chisq/ndf %6.2lf, prob %6.3lf, probQ %6.3lf, probP %6.3lf\n",
      // 	     chisq, ndf, reduced_chisq, prob, probQ, probP);

      //
      // draw the background and Gaussian
      //
      TF1 *bgFunc = new TF1("backFcn", background, unser_min, unser_max, 3);

      bgFunc -> SetNpx(number_of_points);
      bgFunc -> SetLineColor(13);
      bgFunc -> SetLineStyle(2);
      bgFunc -> SetLineWidth(3);
      bgFunc -> SetParameter(0,unser_fit -> GetParameter("a1"));
      bgFunc -> SetParameter(1,unser_fit -> GetParameter("a2"));
      bgFunc -> SetParameter(2,unser_fit -> GetParameter("a3"));
      bgFunc -> Draw("same");
      gPad->Update();

      TF1 *sgFunc = new TF1("signFcn", gaussian, unser_min, unser_max, 3);
      sgFunc -> SetNpx(number_of_points);
      sgFunc -> SetLineColor(kBlue);
      sgFunc -> SetLineWidth(2);
      sgFunc -> SetParameter(0, unser_fit -> GetParameter("Norm"));
      sgFunc -> SetParameter(1, unser_fit -> GetParameter("Mean"));
      sgFunc -> SetParameter(2, unser_fit -> GetParameter("Sigma"));
      sgFunc -> Draw("same");
      gPad->Update();

      TString unser_name = "unser_current";

      Double_t unser_scaler_unit = 2.5e-3; // unser gain from Dave Mack [Hz/microA]
      Double_t unser_max_current = (unser_max-mean[0])*unser_scaler_unit;

      if(unser_fit_range[1] == -10.0) {
	unser_fit_range[1] = unser_max_current;
      }

      printf("Unser maximum current %8.4lf\n", unser_max_current);
      printf("Unser Fit range ( %8.4lf, %8.4lf ) [microA] \n", unser_fit_range[0], unser_fit_range[1]);


      // return important values to a unser class
      // 1) alias name
      // 2) pedestal (mean)
      // 3) pedestal error (sigma)
      // 4) fit ranges

      mps_tree_in_chain->SetAlias(unser_name.Data(), Form("((cc_sca_unser-%lf)*%lf)", mean[0], unser_scaler_unit));
      unser.SetAliasName(unser_name.Data());
      unser.SetPed(mean[0]);
      unser.SetPedErr(sigma[0]);
      unser.SetFitRange(unser_fit_range);

      std::cout << unser << std::endl;
    }
    else {
      printf("This program does not fit the Unser BCM correctly.\n");
      printf("Please report this problem via an email to jhlee@jlab.org.\n");
      return false;
    }
  }
  else {
    // if any peaks are not found, just draw only unser_hist
    unser_hist->Draw();
  }

  unser_canvas -> Modified();
  unser_canvas -> Update();

  //
  // Save as ROOT C++ Macro and png of unser_canvas
  //
  if(output_flag) {
    TString output_name = unser_canvas->GetName();
    unser_canvas -> SaveAs(Form("BCMCalib%s_%s.cxx", run_number, output_name.Data()));
    unser_canvas -> SaveAs(Form("BCMCalib%s_%s.png", run_number, output_name.Data()));

    // // Open ps file
    unser_canvas -> Print(g_bcm_plots_filename+"[");

    // Save "unser" plot into the ps file.
    unser_canvas -> Print(g_bcm_plots_filename);
  }
  return true;
}

Bool_t
bcm_calibrate_draw(BeamMonitor &device, BeamMonitor &reference, const char* run_number )
{

  std::cout << "\n";

  std::cout << reference << std::endl;

  TString plotcommand;
  TString plot_residuals_command;

  TString device_name;
  TString device_samples;

  TString reference_name;
  TString residuals_name;

  Bool_t  sca_flag = false;
  Double_t fit_range[2] = {0.0};

  TCut  device_cut = "";
  TH1D* device_hist = NULL;
  TH1D* device_res  = NULL;

  TF1*  device_fit = NULL;


  Int_t w = 800;
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
  // prof+    >> prof
  // prof+"s" >> profs
  // prof+"i" >> profi
  // prof+"G" >> profG
  // I choose "s" , before I completely remove "Draw" command
  // in this program.
  // See
  // http://root.cern.ch/root/html/TProfile.html#TProfile:BuildOptions
  //

  const char plot_option[] = "profs";

  device_hist = GetHisto(
			 mps_tree_in_chain,
			 plotcommand,
			 device_cut,
			 plot_option);

  if(not device_hist) {
    std::cout << "Please check  "
	      << device_name
	      << std::endl;

    bcm_canvas ->Close();
    delete bcm_canvas; bcm_canvas = NULL;

    return false;
  }

  Int_t x_axis_binrange[2] = {0};
  x_axis_binrange[0] = device_hist->GetMinimumBin();
  x_axis_binrange[1] = device_hist->GetMaximumBin()+2; // 2 bins more

  device_hist -> SetName(device.GetName().Data());
  //device_hist -> GetXaxis() -> SetTitle(reference_name.Data());
  device_hist -> GetYaxis() -> SetTitle("counts per sample_number");
  device_hist -> SetTitle(Form("Run %s : %s vs %s",
			       run_number,
			       device_name.Data(),
			       reference_name.Data()
			       )
			  );
  device_hist -> GetXaxis() -> SetTitle(Form("%s (#muA)", reference_name.Data()));
  device_hist -> GetXaxis() -> SetRange(x_axis_binrange[0], x_axis_binrange[1]);
  device_hist -> Fit("pol1", "E M R F Q", "", fit_range[0], fit_range[1]);
  //  printf(">>%d %d\n", device_hist -> GetMaximumBin(), device_hist->GetMinimumBin());
  device_hist -> SetMarkerStyle(21);
  device_hist -> SetMarkerSize(0.8);
  device_hist -> Draw("");


  device_fit = device_hist  -> GetFunction("pol1");

  // gStyle -> SetOptFit(1111);
  // gStyle -> SetOptStat("ei");

  // device_fit = 0;
  if(device_fit) {

    device_fit -> SetLineWidth(2);
    device_fit -> SetLineColor(kRed);

    device.SetPed    (device_fit->GetParameter(0));
    device.SetPedErr (device_fit->GetParError(0));
    device.SetSlop   (device_fit->GetParameter(1));
    device.SetSlopErr(device_fit->GetParError(1));
    gPad->Update();
    TPaveStats *st = (TPaveStats*)device_hist->FindObject("stats");
    st->SetX1NDC(0.76); //new x start position
    st->SetX2NDC(1.0); //new x end position
    st->SetY1NDC(0.16); //new y start position
    st->SetY2NDC(0.46); //new y end position
    st->Draw("same");
    gPad->Update();
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

    plot_residuals_command = device_name + ":" +  reference_name;

    device_res = GetHisto(mps_tree_in_chain, plot_residuals_command, device_cut, plot_option);

    if (not device_res) {
      std::cout << " Please check residuals plot : "
		<< plot_residuals_command
		<<std::endl;

      return false;	
    }

    device_res -> GetXaxis()->SetTitle("current (#muA)");
    device_res -> GetXaxis()->SetRange(x_axis_binrange[0], x_axis_binrange[1]);
    device_res -> GetYaxis()->SetTitle("Residuals (Hz)");
    //  device_res -> GetYaxis()->SetTitleOffset(2.2);
    device_res -> SetTitle(device_name+" fit residuals");
    device_res -> SetMarkerStyle(21);
    device_res -> SetMarkerSize(0.8);
    device_res -> Draw("same p");


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

  if(output_flag) {
    // Open ps file
    // unser_canvas -> Print(g_bcm_plots_filename+"[");

    //
    // Print bcm_canvas into a ps file that is defined in main() function
    //
    bcm_canvas -> Print(g_bcm_plots_filename);

    //
    // Save as ROOT C++ Macro and png of bcm_canvas.
    //
    TString root_c_name = bcm_canvas->GetName();
    bcm_canvas -> SaveAs(Form("BCMCalib%s_%s.cxx", run_number, root_c_name.Data()));
    bcm_canvas -> SaveAs(Form("BCMCalib%s_%s.png", run_number, root_c_name.Data()));
  }
  return true;
}



Bool_t
bcm_calibrate_nodraw(BeamMonitor &device, BeamMonitor &reference, const char* run_number )
{

  printf("\nWe are entering bcm calibration routine without Draw command\n");
  std::cout << reference << std::endl;

  Bool_t local_debug = true;
  TString device_name;
  TString reference_name;

  device_name    = device.GetName();
  reference_name = reference.GetName();

  if(local_debug) {
    std::cout << " Device name " << device_name
	      << " Reference name " << reference_name
	      << std::endl;
  }

  // TString plotcommand;
  // TString plot_residuals_command;

  // TString device_name;
  // TString device_samples;

  // TString reference_name;
  // TString residuals_name;

  // Bool_t  sca_flag = false;
  // Double_t fit_range[2] = {0.0};

  // TCut  device_cut = "";
  // TH1D* device_hist = NULL;
  // TH1D* device_res  = NULL;

  // TF1*  device_fit = NULL;


  // Int_t w = 800;
  // Int_t h = 600;


  // //  unser_canvas->Clear();

  // device_name = device.GetName();
  // reference_name = reference.GetAliasName();
  // if(device_name.Contains("sca")) {
  //   sca_flag = true;
  // }

  // bcm_canvas = new TCanvas(device_name.Data() , device_name.Data(), w, h);

  // fit_range[0] = reference.GetFitRangeMin();
  // fit_range[1] = reference.GetFitRangeMax();

  // if(not sca_flag) {
  //   device_samples = device_name + ".num_samples";
  //   device_cut     = device_name + ".Device_Error_Code == 0";
  //   device_name    = device_name + ".hw_sum_raw/" + device_samples;
  // }
  // else {
  //   device_name = device.GetAliasName();
  // }

  // plotcommand = device_name + ":" +  reference_name;


  // bcm_canvas->Divide(1,2);
  // bcm_canvas->cd(1);
  // gPad->SetGridx();
  // gPad->SetGridy();
  // // prof+    >> prof
  // // prof+"s" >> profs
  // // prof+"i" >> profi
  // // prof+"G" >> profG
  // // I choose "s" , before I completely remove "Draw" command
  // // in this program.
  // // See
  // // http://root.cern.ch/root/html/TProfile.html#TProfile:BuildOptions
  // //

  // const char plot_option[] = "profs";

  // device_hist = GetHisto(
  // 			 mps_tree_in_chain,
  // 			 plotcommand,
  // 			 device_cut,
  // 			 plot_option);

  // if(not device_hist) {
  //   std::cout << "Please check  "
  // 	      << device_name
  // 	      << std::endl;

  //   bcm_canvas ->Close();
  //   delete bcm_canvas; bcm_canvas = NULL;

  //   return false;
  // }

  // Int_t x_axis_binrange[2] = {0};
  // x_axis_binrange[0] = device_hist->GetMinimumBin();
  // x_axis_binrange[1] = device_hist->GetMaximumBin()+2; // 2 bins more

  // device_hist -> SetName(device.GetName().Data());
  // device_hist -> GetXaxis() -> SetTitle(reference_name.Data());
  // //  device_hist -> GetYaxis() -> SetTitle(device_name.Data());
  // device_hist -> SetTitle(Form("Run %s : %s vs %s",
  // 			       run_number,
  // 			       device_name.Data(),
  // 			       reference_name.Data()
  // 			       )
  // 			  );
  // device_hist -> GetXaxis() -> SetTitle(Form("%s (#muA)", reference_name.Data()));
  // device_hist -> GetXaxis() -> SetRange(x_axis_binrange[0], x_axis_binrange[1]);
  // device_hist -> Fit("pol1", "E M R F Q", "", fit_range[0], fit_range[1]);
  // //  printf(">>%d %d\n", device_hist -> GetMaximumBin(), device_hist->GetMinimumBin());
  // device_hist -> SetMarkerStyle(21);
  // device_hist -> SetMarkerSize(0.8);
  // device_hist -> Draw("");


  // device_fit = device_hist  -> GetFunction("pol1");

  // // gStyle -> SetOptFit(1111);
  // // gStyle -> SetOptStat("ei");

  // if(device_fit) {

  //   device_fit -> SetLineWidth(2);
  //   device_fit -> SetLineColor(kRed);

  //   device.SetPed    (device_fit->GetParameter(0));
  //   device.SetPedErr (device_fit->GetParError(0));
  //   device.SetSlop   (device_fit->GetParameter(1));
  //   device.SetSlopErr(device_fit->GetParError(1));
  //   gPad->Update();
  //   TPaveStats *st = (TPaveStats*)device_hist->FindObject("stats");
  //   st->SetX1NDC(0.76); //new x start position
  //   st->SetX2NDC(1.0); //new x end position
  //   st->SetY1NDC(0.16); //new y start position
  //   st->SetY2NDC(0.46); //new y end position
  //   st->Draw("same");
  //   gPad->Update();
  //   // Draw the residuals
  //   bcm_canvas->cd(2);
  //   gPad->SetGridx();
  //   gPad->SetGridy();
  //   // device_name is already well determined before
  //   // now we don't need to distinguish between sca and vqwk

  //   device_name = Form("(%s) - (%s*%e + %e)"
  // 		       , device_name.Data()
  // 		       , reference_name.Data()
  // 		       , device.GetSlope()
  // 		       , device.GetPed()
  // 		       );

  //   plot_residual_command = device_name + ":" +  reference_name;

  //   device_res = GetHisto(mps_tree_in_chain, plot_residual_command, device_cut, plot_option);

  //   if (not device_res) {
  //     std::cout << " Please check residual plot : "
  // 		<< plot_residual_command
  // 		<<std::endl;

  //     return false;
  //   }

  //   device_res -> GetXaxis()->SetTitle("current (#muA)");
  //   device_res -> GetXaxis()->SetRange(x_axis_binrange[0], x_axis_binrange[1]);
  //   device_res -> GetYaxis()->SetTitle("fit residual");
  //   device_res -> GetYaxis()->SetTitleOffset(2.2);
  //   device_res -> SetTitle(device_name+" fit residual");
  //   device_res -> SetMarkerStyle(21);
  //   device_res -> SetMarkerSize(0.8);
  //   device_res -> Draw("same p");


  // }
  // else {

  //   device.SetPed(-1);
  //   device.SetPedErr(-1);
  //   device.SetSlop(-1);
  //   device.SetSlopErr(-1);
  //   bcm_canvas->Close();
  //   delete bcm_canvas; bcm_canvas = NULL;
  //   return false;
  // }

  // std::cout << device << std::endl;

  // bcm_canvas -> Modified();
  // bcm_canvas -> Update();

  // if(output_flag) {
  //   // Open ps file
  //   // unser_canvas -> Print(g_bcm_plots_filename+"[");

  //   //
  //   // Print bcm_canvas into a ps file that is defined in main() function
  //   //
  //   bcm_canvas -> Print(g_bcm_plots_filename);

  //   //
  //   // Save as ROOT C++ Macro and png of bcm_canvas.
  //   //
  //   TString root_c_name = bcm_canvas->GetName();
  //   bcm_canvas -> SaveAs(Form("BCMCalib%s_%s.cxx", run_number, root_c_name.Data()));
  //   bcm_canvas -> SaveAs(Form("BCMCalib%s_%s.png", run_number, root_c_name.Data()));
  // }
  return true;
}




void
print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "\n");
  fprintf (stream, "This program is used to do BCM calibrations.\n");
  fprintf (stream, "Usage: %s -r {run number} -e {a:b} \n", program_name);
  fprintf (stream,
	   " -r run number.\n"
	   " -e current fit range (default [10, the maximum unser current] microA ) \n"
	   );
  fprintf (stream, "\n");
  exit (exit_code);
}



void
nrerror(const char *error_text)
/* Numerical Recipes standard error handler */
{
  fprintf(stderr,"Numerical Recipes run-time error...\n");
  fprintf(stderr,"%s\n", error_text);
  fprintf(stderr,"...now exiting to system...\n");
  exit(1);
};


double
gammln(double xx)
{
  double x,y,tmp,ser;
  static double cof[6]={76.18009172947146,-86.50532032941677,
			24.01409824083091,-1.231739572450155,
			0.1208650973866179e-2,-0.5395239384953e-5};
  int j;
  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
};


void
gser(double *gamser, double a, double x, double *gln)
{
  int n;
  double sum,del,ap;
  *gln=gammln(a);
  if (x <= 0.0)
    {
      if (x < 0.0) nrerror("x less than 0 in routine gser");
      *gamser=0.0;
      return;
    }
  else
    {
      ap=a;
      del=sum=1.0/a;
      for (n=1;n<=ITMAX;n++)
	{
	  ++ap;
	  del *= x/ap;
	  sum += del;
	  if (fabs(del) < fabs(sum)*EPS)
	    {
	      *gamser=sum*exp(-x+a*log(x)-(*gln));
	      return;
	    }
	}
      nrerror("a too large, ITMAX too small in routine gser");
      return;
    }
};
;

void
gcf(double *gammcf, double a, double x, double *gln)
{

  int i;
  double an,b,c,d,del,h;
  *gln=gammln(a);
  b=x+1.0-a;
  // Set up for evaluating continued fraction
  //				     by modified Lentzs method (§5.2)
  //				     with b0 = 0.
  c=1.0/FPMIN;
  d=1.0/b;
  h=d;
  for (i=1;i<=ITMAX;i++)
    {// Iterate to convergence.
      an = -i*(i-a);
      b += 2.0;
      d=an*d+b;
      if (fabs(d) < FPMIN) d=FPMIN;
      c=b+an/c;
      if (fabs(c) < FPMIN) c=FPMIN;
      d=1.0/d;
      del=d*c;
      h *= del;
      if (fabs(del-1.0) < EPS) break;
    }
  if (i > ITMAX) nrerror("a too large, ITMAX too small in gcf");
  *gammcf=exp(-x+a*log(x)-(*gln))*h;
  //Put factors in front.
};



double
gammp(double a, double x)
{
  double gamser,gammcf,gln;
  if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine gammp");
  if (x < (a+1.0)) {
    gser(&gamser,a,x,&gln);
    return gamser;
  } else {
    gcf(&gammcf,a,x,&gln);
    return 1.0-gammcf;
  }
}



double
gammq(double a, double x)
{
  double gamser,gammcf,gln;
  if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine gammq");
  if (x < (a+1.0)) {
    gser(&gamser,a,x,&gln);
    return 1.0-gamser;
  } else {
    gcf(&gammcf,a,x,&gln);
    return gammcf;
  }
};

