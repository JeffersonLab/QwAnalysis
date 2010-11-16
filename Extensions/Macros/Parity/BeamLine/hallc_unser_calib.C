// Author : Jeong Han Lee
// Date   : Tuesday, September 21 23:05:50 EDT 2010
//
//      
//          This program is used to do HallC unser calibration
//
//          To compile, 
//          "make hallc_unser_calib"
//
//          To run, with default options
//
//          ./hallc_unser_calib -r 5070  
// 
//
//          0.0.1 : Tuesday, September 21 23:06:43 EDT 2010
//                  created


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
//#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TUnixSystem.h"
#include "TCut.h"
#include "TStopwatch.h"
#include "TGraph.h"

#include "TSpectrum.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "TLine.h"

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
};




void print_usage(FILE* stream, int exit_code);
Bool_t calibrate(BeamMonitor &device, BeamMonitor &reference);


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
};


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
 
  Double_t norm  = par[0];
  Double_t mean  = par[1];
  Double_t sigma = par[2];
  Double_t arg   = (x[0]-mean)/sigma;
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
TChain  *mps_tree_in_chain  = NULL;
TCanvas *unser_canvas = NULL;

Int_t
main(int argc, char **argv)
{
 
  TApplication theApp("App", &argc, argv);
  char* run_number = NULL;

  //  Double_t naive_beam_off_cut = 0.0;
  //  Double_t fit_range[2] = {0.0};

  Bool_t file_flag         = false;
  //  Bool_t fit_range_flag    = false;
  //  Bool_t unser_offset_flag = false;

  
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
	// case 'i':
	//   {
	//     unser_offset_flag    = true;
	//     naive_beam_off_cut = atof(optarg);
	//   }
	//   break;
	// case 'e':
	//   {
	//     fit_range_flag = true;
	//     char *c;
	//     /*
	//      * Allow the specification of alterations
	//      * to the pty search range.  It is legal to
	//      * specify only one, and not change the
	//      * other from its default.
	//      */
	//     c = strchr(optarg, ':');
	//     if (c) {
	//       *c++ = '\0';
	//       fit_range[1] = atof(c);
	//     }
	//     if (*optarg != '\0') {
	//       fit_range[0] = atof(optarg);
	//     }
	//     if ((fit_range[0] > fit_range[1]) || (fit_range[0] < 0) || (fit_range[1] > 100.0) ) 
	//       {
	// 	print_usage(stdout,0);
	//       }
	//   }
	//   break;
	case '?':
	  {
	    // if (optopt == 'e') 
	    //   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	    // else if (optopt == 'r')
	    //   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	    // else if (isprint (optopt))
	    //   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	    // else
	    fprintf (stderr, "Unknown option character `\\x%x'.\n",  optopt);
	    print_usage(stdout,0);
	    return 1;
	  }
	  break;
	default:
	  abort () ;
	}
    }
  
  // if(not fit_range_flag) {
  //   fit_range[0] = 0.10;
  //   fit_range[1] = 0.85;
  // }
  // else {
  //   fit_range[0] = fit_range[0]/100.0;
  //   fit_range[1] = fit_range[1]/100.0;
    
  // }
  // if(unser_offset_flag) {
  //   naive_beam_off_cut = naive_beam_off_cut*1e3;
  // }
  // else {
  //   naive_beam_off_cut = 320*1e3;
  // }


  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }



  Bool_t local_debug = false;

  TString unser_name = "sca_unser";
  TString clock_name = "sca_4mhz";

  BeamMonitor sca_unser(unser_name);
  sca_unser.SetReference();
  

  mps_tree_in_chain = new TChain("Mps_Tree");
  
  TString bcm_calibration_filename = Form("BCMCalib_%s*.root", run_number);
  Int_t chain_status = 0;
  chain_status = GetTree(bcm_calibration_filename, mps_tree_in_chain);

  if(chain_status == 0) {
    exit(1);
  }

  // Bool_t bcm_valid_rootfiles = false;
  // bcm_valid_rootfiles = check_bcm_branches(branches_for_bcm_calibration,mps_tree_in_chain);
  
  // if(bcm_valid_rootfiles) {
  //   std::cout << "all branches are in the root file(s)." << std::endl;
  // }
  // else {
  //   printf("\nThe root file, which you selected by run number, is invalid for the BCM calibration.\n");
  //   printf("Plese run the following command in order to create a right ROOT file.\n");
  //   printf("------------------------------------------ \n");
  //   printf(" qwparity -r %s -c hallc_bcm.conf\n", run_number);
  //   printf("------------------------------------------ \n");
  //   exit(-1);    
  // }

  //  Bool_t test_debug = true;
  static Double_t qwk_sca_unser = 0.0;
  static Double_t qwk_sca_4mhz  = 0.0;
  Double_t clock_corrected_unser = 0.0;

  TBranch *b_unser = mps_tree_in_chain->GetBranch(unser_name.Data());
  TBranch *b_4mhz  = mps_tree_in_chain->GetBranch(clock_name.Data());
  b_unser -> SetAddress(&qwk_sca_unser);
  b_4mhz  -> SetAddress(&qwk_sca_4mhz);

  Int_t nentries = (Int_t) mps_tree_in_chain -> GetEntries();
  Int_t nbins    = nentries - 1;

  TH1D *unser_TM   = new TH1D("UnserTM", Form("Run %s : Unser Timing Module", run_number), nbins, 0, nbins);

  unser_TM -> SetStats(0);
  unser_TM -> GetXaxis() -> SetTitle("measurement time (event number ?)");
  unser_TM -> GetYaxis() -> SetTitle("uncorrected unser");
  for (Int_t i=0; i<nentries; i++) 
    {
      b_unser -> GetEntry(i);
      b_4mhz  -> GetEntry(i);
      clock_corrected_unser = qwk_sca_unser*4e6/qwk_sca_4mhz;
      if(local_debug) {
	std::cout << "i " << i
		  << " unser " << qwk_sca_unser
		  << " 4mhz  " << qwk_sca_4mhz
		  << " cc unser "  << clock_corrected_unser
		  << std::endl;
      }

      unser_TM -> SetBinContent(i, clock_corrected_unser);
    }

  Double_t unser_max = 0.0;
  Double_t unser_min = 0.0;

  unser_max = unser_TM->GetBinContent(unser_TM->GetMaximumBin());
  unser_min = unser_TM->GetBinContent(unser_TM->GetMinimumBin());

  Int_t rd_unser_min   = (Int_t) unser_min;
  Int_t rd_unser_max   = (Int_t) (unser_max) + 1 ;
  Int_t nbins_rd_unser = (rd_unser_max - rd_unser_min)/1000; // normalize a kHz offset in order to increase "bin size".

  printf(" UnserHist Nbins %d Min %d Max %d\n", nbins_rd_unser, rd_unser_min, rd_unser_max);
  TH1D *unser_hist = new TH1D("UnserHist", Form("Run %s : Unser Histogram", run_number), nbins_rd_unser, rd_unser_min, rd_unser_max);
  unser_hist -> SetMarkerStyle(21);
  unser_hist -> SetMarkerSize(0.8);
  for (Int_t i=0; i<nbins; i++) 
    {
      unser_hist->Fill(unser_TM -> GetBinContent(i));
    }

  if(local_debug) printf("entries %d unser_max %lf min %lf\n", nentries, unser_max, unser_min);
  
  
  Int_t w = 1000;
  Int_t h = 500;
  unser_canvas = new TCanvas(Form("Run_%s_sca_unser_plots", run_number), Form("Run %s SCA Unser plots", run_number), w, h);  

  unser_canvas -> Divide(2,1);

  unser_canvas -> cd(1);
  unser_TM -> Draw();
  gPad->Update();
  unser_canvas -> cd(2);
  Int_t npeaks = 10;
  Int_t nfound = 0 ;

  TSpectrum *s = new TSpectrum(npeaks);
  nfound = s->Search(unser_hist, 2, "", 0.05);
 
  Float_t *xpeaks = s->GetPositionX();
  printf("Found %d candidate peaks to fit\n",nfound);


  Double_t peak_xpos = 0.0;
  Int_t    peak_bin  = 0;
  Double_t peak_ypos = 0.0;
  // Double_t raw_sigma = 0.0;

  Int_t    unser_peak_idx = 0;
  Int_t    unser_peak_bin = 0;
  Double_t unser_peak_xpos = 0.0;
  Double_t unser_peak_ypos = 0.0;
  Double_t unser_raw_sigma = 0.0;

  if ( nfound !=0 ) {

    for(Int_t i=0; i<nfound; i++)
      {
	peak_xpos = (Double_t) xpeaks[i];
	peak_bin  = unser_hist->GetXaxis()->FindBin(peak_xpos);
	if(i==0) unser_peak_bin = peak_bin;
	else    {
	  if(unser_peak_bin > peak_bin) { 
	    unser_peak_bin = peak_bin;
	    unser_peak_idx = i;
	  }
	}
	peak_ypos = unser_hist->GetBinContent(peak_bin);
	// raw_sigma = 0.34*TMath::Abs(peak_xpos-unser_min);
	// printf("%d : peak_xpos %4.2lf peak_bin %4d peak_ypos %4.2lf raw_sigma %4.2lf\n",
	// 	     i, peak_xpos, peak_bin, peak_ypos, raw_sigma);
      }

    unser_peak_xpos = (Double_t) xpeaks[unser_peak_idx];
    unser_peak_bin  = unser_hist->GetXaxis()->FindBin(unser_peak_xpos);
    unser_peak_ypos = unser_hist->GetBinContent(unser_peak_bin);
    unser_raw_sigma = 0.34*TMath::Abs(unser_peak_xpos-unser_min);

    printf("Unser Peak %d : peak_xpos %4.2lf peak_bin %4d peak_ypos %4.2lf raw_sigma %4.2lf\n",
	   unser_peak_idx, unser_peak_xpos, unser_peak_bin, unser_peak_ypos, unser_raw_sigma);

    // TH1 *hb = s->Background(unser_hist, 20, "same");
    // if (hb) gPad->Update();
 
  
    TF1 *fitFunc = new TF1("fitFcn", fitFunction, unser_min, unser_max, 6);
    fitFunc -> SetNpx(1000);
    // fitFunc -> SetParameter(0,1);
    // fitFunc -> SetParameter(1,1);
    // fitFunc -> SetParameter(2,1);
    // initialize three parameters according to what I found in the fitting routine.
    fitFunc -> SetParameter(3, unser_peak_ypos);
    fitFunc -> SetParameter(4, unser_peak_xpos);
    fitFunc -> SetParameter(5, unser_raw_sigma);

    fitFunc -> SetParName(0, "a1");
    fitFunc -> SetParName(1, "a2");
    fitFunc -> SetParName(2, "a3");
    fitFunc -> SetParName(3, "Norm");
    fitFunc -> SetParName(4, "Mean");
    fitFunc -> SetParName(5, "Sigma");

    fitFunc -> SetParLimits(4, unser_peak_xpos - unser_raw_sigma, unser_peak_xpos + unser_raw_sigma);
    fitFunc -> SetParLimits(5, 0, unser_peak_xpos-unser_min);

    fitFunc -> SetLineWidth(2);
    fitFunc -> SetLineColor(kRed);

    gStyle -> SetOptFit(1111);
    gStyle -> SetOptStat("ei");
  
    unser_hist->Fit("fitFcn", "MBQ");


    Double_t height[2] = {0.0};
    Double_t mean[2]   = {0.0};
    Double_t sigma[2]  = {0.0};

    height[0] = fitFunc -> GetParameter("Norm");
    height[1] = fitFunc -> GetParError(3);
    mean[0]   = fitFunc -> GetParameter("Mean");
    mean[1]   = fitFunc -> GetParError(4);
    sigma[0]  = fitFunc -> GetParameter("Sigma");
    sigma[1]  = fitFunc -> GetParError(5);
    printf("Gaussian N: %12.2lf +- %8.2lf, Mean: %12.2lf +-%8.2lf, Sigma: %12.2lf +-%8.2lf\n", 
	   height[0], height[1], mean[0], mean[1], sigma[0], sigma[1]);
    


    double vertical_line_xrange[2] = {0.0};
    vertical_line_xrange[0] = mean[0] - 3.0*sigma[0]; // 3 sigma guided lines
    vertical_line_xrange[1] = mean[0] + 3.0*sigma[0];
    TLine* line = new TLine();
    line -> SetLineStyle(3);

    line -> DrawLine(vertical_line_xrange[0], 0, vertical_line_xrange[0], 0.8*unser_peak_ypos);
    line -> DrawLine(vertical_line_xrange[1], 0, vertical_line_xrange[1], 0.8*unser_peak_ypos);
  
    
    double chisq         = fitFunc -> GetChisquare();
    double ndf           = fitFunc -> GetNDF();
    double reduced_chisq = chisq/ndf;
    
    double prob          = fitFunc -> GetProb();  // ROOT provide
    double probQ         = gammq(0.5*ndf, 0.5*chisq); // numerical recipe
    double probP         = gammp(0.5*ndf, 0.5*chisq); // numerical recipe
    
    printf("chisq %6.1lf ndf %3.0lf chisq/ndf %6.2lf, prob %6.3lf, probQ %6.3lf, probP %6.3lf\n",
	   chisq, ndf, reduced_chisq, prob, probQ, probP);

    TF1 *bgFunc = new TF1("backFcn", background, unser_min, unser_max, 3);
  
    bgFunc -> SetNpx(1000);
    bgFunc -> SetLineColor(13);
    bgFunc -> SetLineStyle(2);
    bgFunc -> SetLineWidth(3);
    bgFunc -> SetParameter(0,fitFunc -> GetParameter("a1"));
    bgFunc -> SetParameter(1,fitFunc -> GetParameter("a2"));
    bgFunc -> SetParameter(2,fitFunc -> GetParameter("a3"));
    bgFunc -> Draw("same");
    
 
    TF1 *sgFunc = new TF1("signFcn", gaussian, unser_min, unser_max, 3);
    sgFunc -> SetNpx(1000);
    sgFunc -> SetLineColor(kBlue);
    sgFunc -> SetLineWidth(2);
    sgFunc -> SetParameter(0, fitFunc -> GetParameter("Norm"));
    sgFunc -> SetParameter(1, fitFunc -> GetParameter("Mean"));
    sgFunc -> SetParameter(2, fitFunc -> GetParameter("Sigma"));
    sgFunc -> Draw("same");
    gPad->Update();
  }
  else {
    unser_hist->Draw();
  }
  unser_canvas -> Update();
  theApp.Run();
  return 0;
};


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
};




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

