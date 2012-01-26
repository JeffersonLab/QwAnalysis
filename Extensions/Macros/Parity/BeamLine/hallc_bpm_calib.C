//
// Author : Jeong Han Lee
// Date   : Wednesday, August 18 16:54:49 EDT 2010
//
//      
//      
//          This program is used to do HallC BPMs calibration by using a result of BCMs calibration
//          see hallc_bcm_calib.C
//
//          
//          To compile, 
//          "make hallc_bpm_calib"
//
//          To run, with default options
//
//          ./hallc_bpm_calib -r 5084  
//
//          with the fitting range of a refererence bcm ([0.15*bcm_current, 0.90*bcm_current])
//    
//          ./hallc_bpm_calib -r 5084 -e 15:90
//
//          with a specified bcm calibration run 
//          
//          ./hallc_bpm_calib -r 5084 -c 5070
//
//          using a specific bcm (default is bcm1)
//          
//          ./hallc_bpm_calib -r 5084 -c 5070 -b bcm1
//
//          0.0.1 : Wednesday, August 18 17:13:48 EDT 2010
//                  redesign...
//
//          0.0.2 : Friday, August 20 Buddhini
//                  Added the rest of the bpms. Now there are total of 23 bpms.
//
//          0.0.3 : Monday, August 23 Buddhini
//                  Added some comments to clear things out.
//                  Changed tmp used as a bcm to tmpbcm on line 653    
//                  Added methods to use a user prefferd bcm by adding              
//                  ref_bcm_flag.
//
//          0.0.4 : Monday Septembed 13th  Buddhini
//                  Changed current from adc counts to uA using the bcm calibration slope.
//                  Modified the wire fit plots. Added new plots to plot the fit residuals.
//                  All the plots are now saved in to a .ps file
//
//          0.0.5 : Thursday, September 16 01:47:15 EDT 2010 jhlee
//                  - Removed the compiling warning: 
//                    deprecated conversion from string constant to ‘char*’
//                  - Changed LaTeX expression for the current unit #muA
//
//          0.0.6 : Saturday, October 16th Buddhini
//                  - Added root file chaining.
//                  - Added ability to use either the calinrated bcm results from the run being analyzed
//                    or use a preferd bcm calibration run. The default is using the calibrated bcm from
//                    the current run.
//                  - Modified the format of the output .txt file.
//                  - Changed the default fit range 0.15-0.85 to 0.20 to 0.95. This is what Paul is using 
//                    his version of hallc_bpm_calib_pking.C
//                  - Applied Device_Error_Code == 0 check for data being plotted.
//
//          0.0.7 : Tuesday, October 19 11:25:44 EDT 2010, jhlee
//                  - added check_bpm_branch function in order to check the opened ROOT file
//                    has the right structure for the BPM calibration.
//                  - added several classes to read "qweak_beamline_geometry.map" easily,
//                    that is used to extract real BPM names at Hall C
//                  - did fine tune check_bpm_branch and calibration function.
//                  - did a tweak that make the better output for a pedstral file of QwAnalyzer
//
//          0.0.8 : Monday, October 25 01:23:55 EDT 2010, jhlee
//                   - improved the way to handle (a) ROOT file(s) (TChain) 
//
//          0.0.9 : Sunday, Nomember 14 10:47:55 EDT 2010, Buddhini
//                   - added a command line option, "s" to apply event cuts if needed. Default is no event cuts. 
//                   - fine tuned the current range obtained from the bcm. Earlier we were just using the mac and min from the
//                     bcm histogram axis which gave default min and max set by root and so was not right always.
//                     Now we scan the axis to find the minimum and maximum current.
//                   - Fixed the way the fitting range was being calculated.
//                   - Passed the correct command to plot residuals in to residual plots. 
//
//          0.1.0 : Friday, May 6th,2011 Buddhini
//                   - Added the bcm1 vs mps_counter plot.
//                   - Added a method get the minimum and maximum current from the bcm1 reading.
//                   - Disabled getting the bpm names from the geometry map file. Due to the new column of rotation in the map file
//                     the code reading the map files seems to skip bpms that have the rotation column filled.
//                     I was in a hurry to get the calibrations so I did not try to go and fix the map file reading routine.
//
//          0.1.1 : Sunday, November 20th, 2011 Buddhini
//                   - Changed to search for rootfiles with the extension *.trees.root
 
// Additional BPM calibration run info
//
//  run 

// 5083            * BPM calibration. 
//                    https://hallcweb.jlab.org/hclog/1008_archive/100806050806.html
// 5900            * BPM Calibration
//
// 6908            * BPM Calibration (IOCSE14 was on autogain mode so we could not use this data).
// 11956           * BPM Calibration 
//                     https://hallcweb.jlab.org/hclog/1105_archive/110505195024.html

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <getopt.h>
#include <sstream>
#include <vector>

#include <algorithm>


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

#include "TProfile.h"
#include "TLine.h"
#include "TBox.h"

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



class QwString: public TString 
{
 public:
  QwString();
  ~QwString(){};

public:
  void SetCommentSymbol(const std::string s) {
    fCommentSymbol = s;
  }
  std::string GetCommentSymbol() { return fCommentSymbol;};

  QwString PurifyFromComment(const std::string s);
  QwString RemoveWhiteSpaces();
  QwString Purify(const std::string s);

  TString TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id) ;

protected:
  std::string fCommentSymbol;
private:
  QwString PurifyFromDefault();
  
};

QwString::QwString()
{
  fCommentSymbol.clear();
};

QwString 
QwString::PurifyFromDefault()
{
  this -> ToLower();
  if (not (this->BeginsWith(fCommentSymbol)) ) {
    if( this->Contains(fCommentSymbol) ) {
      Int_t pos = this->First(fCommentSymbol.c_str()); 
      // pos doesn't need to be initialized by "0", because it always has "value" form "First"
      this->Replace(pos, this->Sizeof() - pos, "");
    }
  }
  else {
    this->Clear();
  }
  return *this;
};

QwString 
QwString::PurifyFromComment(const std::string s)
{
  fCommentSymbol = s;
  this->PurifyFromDefault();
  return *this;
  
};


QwString 
QwString::RemoveWhiteSpaces()
{
  this -> ReplaceAll(' ', '\0'); // space
  this -> ReplaceAll('\t','\0'); // tab
  return *this;
};


QwString 
QwString::Purify(const std::string s)
{
 
  this -> PurifyFromComment(s);
  if(not this->IsNull()) {
    this -> RemoveWhiteSpaces();
  }
  return *this;
};


TString 
QwString::TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id)
{
  Bool_t local_debug = false;
  TString name;  
  
  TObjArray* Strings = this->Tokenize(delim); // break line into tokens
  Int_t token_numbers = Strings->GetEntriesFast();
  
  if(token_numbers == interesting_token_num) {
    TIter iString(Strings);
    TObjString* os= NULL;
    Int_t token_ids = 0;
    while ((os=(TObjString*)iString())) {
      if(token_ids == interesting_token_id) {
	name = os->GetString();
	break;
      }
      token_ids++;
    }
    delete Strings;

    if(local_debug) {
      std::cout << " tokens #"
		<< token_numbers
		<< " " << *this
		<< " " << name
		<< std::endl;
    }
  }
  else {   
    name.Clear(); // return empty string
    if(local_debug) {
      std::cout << "Token number is not in the possible token numbers" << std::endl;
    }
  }
  return name;
}
	



class QwBeamMonitorDevice
{
public:
  QwBeamMonitorDevice();
  ~QwBeamMonitorDevice(){};

  Bool_t OpenMapFile(TString mapfilename);

  std::vector<TString > monitors;

  std::vector<TString> GetMonitorsList() {return monitors;};
  
  Bool_t IsMonitors() {return fMonitorsOnFlag;};

private:

  ifstream fMapFile; 
 
  TString GetMapDir(TString mapfilename, Bool_t current_dir_flag);

  Bool_t fMonitorsOnFlag;
 

};


QwBeamMonitorDevice::QwBeamMonitorDevice()
{
  fMonitorsOnFlag = false;
  fMapFile.clear(); 
  monitors.clear();
};

TString 
QwBeamMonitorDevice::GetMapDir(TString mapfilename, Bool_t current_dir_flag)
{
  TString get;
  if(current_dir_flag) {
    get = getenv("PWD");
    get += "/";
    if(get.IsNull()) {
      std::cout << "Please check your path "  
		<< std::endl;
      return get;
    }
    else {
      mapfilename = get + mapfilename;
      return mapfilename;
    }
  }
  else {
    get =  getenv("QWANALYSIS");
    get += "/";
    if(get.IsNull()) {
      std::cout << "Please check your QWANALYSIS path "  
		<< std::endl;
      return get;
    }
    else {
      mapfilename = get + "Parity/prminput/" + mapfilename;
      return mapfilename;
    }
  }
};


Bool_t
QwBeamMonitorDevice::OpenMapFile(TString mapfilename)
{

  Bool_t local_debug = false;

  if(local_debug) {
    std::cout << "QwBeamMonitorDevice::OpenMapFile " << mapfilename << std::endl;
  }

  fMapFile.clear();
  fMapFile.open(GetMapDir(mapfilename, false));
  
  if(not fMapFile.is_open()) {
    std::cout << "I cannot open your mapfile " 
	      << mapfilename  
	      << ". Please Check it again."
	      << std::endl;
    return false;
  }
  else {

    std::cout << mapfilename << " is opened." << std::endl;

    char delimiters[] = ",";
    QwString line; 
    TString device_name; device_name.Clear();

    std::vector<TString>::iterator pd;
    
    while (not fMapFile.eof() ) {
      line.ReadLine(fMapFile);   
      line.Purify("!");
      if(not line.IsNull()) {
	device_name = line.TokenString(delimiters, 8, 1); // 8 total, 1st what I am interested in [0,8)
	if (not device_name.IsNull()) {
	  if(local_debug) std::cout << " device_name " << device_name << std::endl;
	  monitors.push_back(device_name);
	} // if (not device_type.IsNull()) {
      } // if(not line.IsNull()) {
    } //   while (not mapfile.eof() ) {
  }
  fMapFile.close();
  return true;
}



void 
print_usage(FILE* stream, int exit_code);

Bool_t 
bpm_calibrate(std::vector<BeamMonitor> &bpm, BeamMonitor &reference, const char* run_number);


Bool_t
check_bpm_branches(std::vector<TString> &branches, TTree *roottree)
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
  TString  antenna    [4] = {"XP","XM","YP","YM"};

  for (branches_idx=0; branches_idx<branches_size; branches_idx++) 
    {
      for(Int_t i=0; i<4; i++) 
	{
	  TString branch_name = branches.at(branches_idx) + antenna[i];
	  if(local_debug) std::cout  << std::setw(30) << branch_name;
	  
	  TBranch* branch = roottree -> FindBranch(branch_name.Data());
	  if(branch) { 
	    branch_valid &= true;
	    if(local_debug) std::cout << " : true" << std::endl;
	  }
	  else  {
	    branch_valid &= false;
	    if(local_debug) std::cout << " : *** false *** " << std::endl;
	  }
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
std::vector< std::vector<BeamMonitor> > hallc_bpms_list;
BeamMonitor hallc_bcm;

TChain  *mps_tree_in_chain = NULL;
TCanvas *bpm_canvas        = NULL;
TCanvas *bcm_canvas        = NULL;
TString  bpm_plots_filename;

Bool_t event_range_flag    = false;
Double_t event_range[2] = {0.0};

Double_t tmp_max = 180;
Double_t tmp_min = 110;

Int_t w = 1200;
Int_t h = 800;

Int_t
main(int argc, char **argv)
{
 
  Bool_t local_debug  = false;
  
  TApplication theApp("App", &argc, argv);


  char* run_number   = NULL;
  char* ref_bcm_name = NULL;

  Bool_t file_flag         = false;
  Bool_t bcm_ped_file_flag = false;
  Bool_t ref_bcm_flag      = false;
  Bool_t fit_range_flag    = false;

  Double_t fit_range[2] = {0.0};

  TString bcm_ped_filename;
 
  Int_t  bcm_ped_runnumber = 0;


  // Canvas parameters
  gStyle->SetFrameFillColor(10);


  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.2);  
  gStyle->SetPadRightMargin(0.08);  
 

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatH(0.1);
  gStyle->SetStatW(0.2);

  program_name = argv[0];

  int cc = 0; 

  /*command line arguments*/
  while ( (cc= getopt(argc, argv, "r:c:b:e:s:")) != -1)
    switch (cc)
      {
      case 'r': /*run number of the pedestal run*/
	{
	  file_flag = true;
	  run_number = optarg;
	}
	break;
      case 'c': /*run number of the bcm calibration run*/
       	{
       	  bcm_ped_file_flag = true;
	  bcm_ped_runnumber = atoi(optarg);
	}
	break;
      case 'b': /*bcm to be used for calibration*/
       	{
       	  ref_bcm_flag = true;
	  ref_bcm_name = optarg;
	}
	break;
      case 'e': /*fitting  range as a percentage*/
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
      case 's': /*event range*/
	{
	  event_range_flag    = true;
	  char *s;
	  /*
	   * Allow the specification of alterations
	   * to the pty search range.  It is legal to
	   * specify only one, and not change the
	   * other from its default.
	   */
	  s = strchr(optarg, ':');
	  if (s) {
	    *s++ = '\0';
	    event_range[1] = atof(s);
	  }
	  if (*optarg != '\0') {
	    event_range[0] = atof(optarg);
	  }
	  if ((event_range[0] > event_range[1]) || (event_range[0] < 0) ) 
	    {
	      print_usage(stdout,0);
	    }
	}
	break;
      case '?':  /*if no argument is given*/
	{
	  if (optopt == 'e') 
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'r')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'c')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'b')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	 else if (optopt == 's')
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
  
  /*default fit range*/	  
  if(not fit_range_flag) {
    fit_range[0] = 0.20;
    fit_range[1] = 0.95;
  }
  else {
    /*user chosen fit range*/
    fit_range[0] = fit_range[0]/100.0;
    fit_range[1] = fit_range[1]/100.0;
    
  }
 
  /*if no pedestal run is specified, print the usage requirements and exit the program*/
  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }

  /*if a bcm is not specified use the defaults bcm , bcm1*/
  if (not ref_bcm_flag) {
    ref_bcm_name = Form("qwk_bcm1");
  }


  /*file containing the bpm calibration plots*/
  bpm_plots_filename = Form("%s_hallc_bpm_calib_plots", run_number);

  // 
  // Geometry information for the BPMs in Hall C beamline.
  // Parity/prminput directory
  //
  TString mapfilename = "qweak_beamline_geometry.map";

  QwBeamMonitorDevice beam_monitors;
  std::vector<TString> bpm_name_list;
  std::vector<TString>::iterator pd;

//   if (beam_monitors.OpenMapFile(mapfilename)) {
//     bpm_name_list = beam_monitors.GetMonitorsList();
//     if (local_debug) {
//       std::cout << "Beam Monitors : bpm size "
// 		<< bpm_name_list.size()
// 		<< std::endl;
//       for( pd = bpm_name_list.begin(); pd != bpm_name_list.end(); pd++ ) 
// 	{
// 	  std::cout << *pd << std::endl;
// 	}
//     }
//   }
//   else {
    // if cannot find "geometry map file",
    // use several bpms defined by hand...
    //
    bpm_name_list.push_back("qwk_bpm3c07");    // begining of arc
    bpm_name_list.push_back("qwk_bpm3c07a");
    bpm_name_list.push_back("qwk_bpm3c08");

    bpm_name_list.push_back("qwk_bpm3c11");
    bpm_name_list.push_back("qwk_bpm3c12");    // highest dispersion
    bpm_name_list.push_back("qwk_bpm3c14");
    bpm_name_list.push_back("qwk_bpm3c16");
    bpm_name_list.push_back("qwk_bpm3c17");    // end of arc (green wall)
    bpm_name_list.push_back("qwk_bpm3c18");    // Region 1 after the shielding wall (in not songsheet)
    bpm_name_list.push_back("qwk_bpm3c19");    // Region 1 after the shielding wall (in not songsheet)
    
    bpm_name_list.push_back("qwk_bpm3p02a");   // compton chicane
    bpm_name_list.push_back("qwk_bpm3p02b");   // compton chicane
    bpm_name_list.push_back("qwk_bpm3p03a");   // compton chicane

    bpm_name_list.push_back("qwk_bpm3c20");    // Region 3
    bpm_name_list.push_back("qwk_bpm3c21");    // Region 3
    bpm_name_list.push_back("qwk_bpm3h02");    // Region 4
    bpm_name_list.push_back("qwk_bpm3h04"); 
    bpm_name_list.push_back("qwk_bpm3h07a");
    bpm_name_list.push_back("qwk_bpm3h07b"); 
    bpm_name_list.push_back("qwk_bpm3h07c");  
    bpm_name_list.push_back("qwk_bpm3h08");    // Region 5
    bpm_name_list.push_back("qwk_bpm3h09");    // Region 5
    bpm_name_list.push_back("qwk_bpm3h09b");   // Region 5
    
    //  }

   
  std::vector <BeamMonitor> tmp_bpm;
  std::size_t bpm_idx  = 0;
  std::size_t bpm_size = 0;
  
  bpm_size = bpm_name_list.size();
  
  for( bpm_idx = 0; bpm_idx < bpm_size; bpm_idx++ ) 
    {
      tmp_bpm.clear();
      BeamMonitor a_bpm (bpm_name_list.at(bpm_idx));
      a_bpm.SetBPM();
      tmp_bpm.assign(4, a_bpm); // 4 antenna
      hallc_bpms_list.push_back(tmp_bpm);
    }  
  
  /*If a bcm calibration file is specified, open the bcm calibration results file*/
  if (bcm_ped_file_flag) {
    bcm_ped_filename = Form("hallc_bcm_pedestal_%d.txt", bcm_ped_runnumber);
    //bcm_ped_filename = Form("hallc_bcm_pedestal_%d_events_%d_%d.txt", bcm_ped_runnumber,event_range[0], event_range[1]);

    TString name;
    Double_t offset[2] = {0.0};
    Double_t slope[2]  = {0.0};    
    Double_t gain[2] = {0.0};    
    // gain can also be calculated using slope automatically in the hallc_bcm class

    ifstream in;
    in.open(bcm_ped_filename.Data());
    if(in.is_open()) {
      while (in.good()) 
	{
	  in >> name >> offset[0] >> offset[1] >> slope[0] >> slope[1] >> gain[0] >> gain[1];
	  printf("%s %e %e %e %e %e %e\n", name.Data(), offset[0], offset[1], slope[0], slope[1], gain[0], gain[1]);
	  if(name.Contains(ref_bcm_name)){
	    hallc_bcm.SetName(name);
	    hallc_bcm.SetPed(offset[0]);
	    hallc_bcm.SetPedErr(offset[1]);
	    hallc_bcm.SetSlop(slope[0]);
	    hallc_bcm.SetSlopErr(slope[1]);
	    in.close();
	    break;
	  }
	  else {
	    std::cout << "We cannot find any reference bcm data in "
		      << bcm_ped_filename 
		      << ". Please check them again."
		      << std::endl;
	    in.close();
	  }
	}
    }
    else {
      std::cout << "There is no BCMs calibration file with the name "
		<< bcm_ped_filename 
		<< ". Please check or run BCM calibration first."
		<< std::endl;
      return EXIT_FAILURE;
    }

    
  
    // /*Load the calibration information for the bcm from the bcm calibration file*/
    // while(not in.eof()) {

    //   in >> name >> offset[0] >> offset[1] >> slope[0] >> slope[1] >> gain[0] >> gain[1];
    //   printf("%s %e %e %e %e %e %e\n", name.Data(), offset[0], offset[1], slope[0], slope[1], gain[0], gain[1]);
    //   // if(not in.good()) {
    //   // 	std::cout << "We cannot read the " 
    //   // 		  << bcm_ped_filename
    //   // 		  << ". Please check it agiain." 
    //   // 		  << std::endl;
    //   // 	//	break;
    //   // 	in.close();
    //   // 	return EXIT_FAILURE;
    //   // }
    //   // else {
    //   if(name.Contains(ref_bcm_name)){
    // 	hallc_bcm.SetName(name);
    // 	hallc_bcm.SetPed(offset[0]);
    // 	hallc_bcm.SetPedErr(offset[1]);
    // 	hallc_bcm.SetSlop(slope[0]);
    // 	hallc_bcm.SetSlopErr(slope[1]);
    //   }
    //   //      }
    // }
    
    // in.close();
  }
  else {
    /*When there is no bcm calibration run specified we use the bcm value of the current run directly.*/
    /*This will be the default setting.*/ 
    hallc_bcm.SetName(ref_bcm_name);
  }
 
  mps_tree_in_chain = new TChain("Mps_Tree");

  TString bpm_calibration_filename = Form("BPMCalib_%s*.trees.root", run_number);

  Int_t chain_status = 0;
  chain_status = GetTree(bpm_calibration_filename, mps_tree_in_chain);

  if(chain_status == 0) {
    exit(1);
  }

  Bool_t bpm_valid_rootfiles = false;
  bpm_valid_rootfiles = check_bpm_branches(bpm_name_list, mps_tree_in_chain);
  
  if(bpm_valid_rootfiles) {
    std::cout << "all branches are in the root file(s)." << std::endl;
  }
  else {
    printf("\nThe root file, which you selected by run number, is invalid for the BPM calibration.\n");
    printf("Plese run the following command in order to create a right ROOT file.\n");
    printf("------------------------------------------ \n");
    printf(" qwparity -r %s -c hallc_bpm.conf\n", run_number);
    printf("------------------------------------------ \n");
    exit(-1);    
  }



  TH1D *tmp;
  Int_t nbins = 0;
  Double_t range_length = 0.0;

  TString scut = "";

  /* Use the current readings from the bcm */
  bpm_canvas = new TCanvas("Current" , Form("Range of currents form %s",ref_bcm_name), w, h);  
  bpm_canvas->Clear();
  bpm_canvas->cd();
  bpm_canvas-> Divide(1,2);
  bpm_canvas->cd(1);

  // hw_sum_raw : ADC and sca raw signals (uncalibrated)
  if(bcm_ped_file_flag) {
    mps_tree_in_chain->SetAlias("current",
		       Form("(((%s.hw_sum_raw/%s.num_samples)-%1f)/ %lf )", 
			    hallc_bcm.GetName().Data(), hallc_bcm.GetName().Data(),
			    hallc_bcm.GetPed(),hallc_bcm.GetSlope())
		       ); 
  }
  else {
    // hw_sum : it is already the calibrated bcm signal,
    //          thus it is valid if the BCM calibration is done, and update
    //          the pedestal values in the qweak_hallc_pedestal.map correctly.
    //          if not, it is the wrong. 
    //          Monday, October 18 13:05:37 EDT 2010, jhlee

    mps_tree_in_chain->SetAlias("current",
				Form("%s.hw_sum",hallc_bcm.GetName().Data())); 
  }

  if(event_range_flag)
    scut = Form("%s.Device_Error_Code==0 && CodaEventNumber < %f && CodaEventNumber > %f", 
		hallc_bcm.GetName().Data(),event_range[1],event_range[0]);
  else
    Form("%s.Device_Error_Code==0", hallc_bcm.GetName().Data());

  mps_tree_in_chain->Draw("current>>tmp", scut);
  
  tmp = (TH1D*)gDirectory->Get("tmp");
  if(not tmp) {
    std::cout << "Please check to see if " 
	      << ref_bcm_name <<" exsists."
	      << std::endl;
    theApp.Run();
    
  }

  /* get maximum and minimum range of the plot */
  tmp_max = tmp -> GetXaxis() -> GetXmax();
  tmp_min = tmp -> GetXaxis() -> GetXmin();
  nbins = tmp -> GetXaxis() -> GetNbins();

  /* now to get maximum and minimum range of current, */
  for(Int_t i=0;i<nbins; i++){
    if(tmp->GetBinContent(i)>1){
      tmp_min  = tmp-> GetBinLowEdge(i);
      break;  
    }
  }

  for(Int_t i=nbins;i>0;i--){
    if(tmp->GetBinContent(i)>1){
      tmp_max = tmp->GetXaxis()->GetBinUpEdge(i);
      break;
    }
  }

  tmp -> GetXaxis() -> SetTitle("current (#muA)");

  /*Calculate the fitting range for current based on the percent range
    given by user/default */

  range_length = tmp_max-tmp_min;
  fit_range[0] *= range_length;
  fit_range[0] += tmp_min; //lower limit of range

  fit_range[1] *= range_length; 
  fit_range[1] += tmp_min ; //upper limit of range

  bpm_canvas->cd(2);
  mps_tree_in_chain->Draw("current:mps_counter", scut);



//   fit_range[0] *= 0.15;
//   fit_range[1] *= tmp_max;

  hallc_bcm.SetAliasName("current"); 
  hallc_bcm.SetFitRange(fit_range);
  hallc_bcm.SetReference();

  bpm_canvas->Update();

  /* Print the plot on to a file */
  bpm_canvas->Print(bpm_plots_filename+".ps(");
  
  /* Start to calibrate the bpms */

  std::size_t i = 0; 
  Int_t cnt = 0;
  std::cout << "how many bpms ? " << hallc_bpms_list.size() << std::endl;
  for (i=0;i < hallc_bpms_list.size(); i++ ) {
    std::cout << "\n" 
	      << cnt++ 
	      << ": onto calibrating " << hallc_bpms_list.at(i).at(0).GetName() 
	      << std::endl;
    
    Bool_t check = false;
    check = bpm_calibrate(hallc_bpms_list.at(i), hallc_bcm, run_number); 
  }
  
  /* open a file to store the pedestals */
  std::ofstream       hallc_bpms_pedestal_output;
  std::ostringstream  hallc_bpms_pedestal_stream;
  hallc_bpms_pedestal_output.clear();


  hallc_bpms_pedestal_output.open(Form("hallc_bpm_pedestal_%s.txt", run_number));
  hallc_bpms_pedestal_stream.clear();
  
  std::size_t antenna_id = 0;
 
  for (i=0; i < hallc_bpms_list.size(); i++) {

    for(antenna_id = 0; antenna_id <hallc_bpms_list.at(i).size(); antenna_id++ ) {
      hallc_bpms_list.at(i).at(antenna_id).SetFileStream();
      hallc_bpms_pedestal_stream << hallc_bpms_list.at(i).at(antenna_id);
    }
  } 

  time_t theTime;
  time(&theTime);
 
  hallc_bpms_pedestal_output <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			     <<"!  HallC BPM pedestals (per sample) obtained using \n"
			     <<"!  reference bcm = "<<hallc_bcm.GetName() 
			     <<std::endl;
  if(bcm_ped_file_flag)
    hallc_bpms_pedestal_output  <<"!  bcm calibration run used = " << bcm_ped_runnumber <<std::endl;
  else
    hallc_bpms_pedestal_output  <<"!  Used calibrated bcm output from current run," << run_number <<std::endl;
 
  hallc_bpms_pedestal_output  <<"!  Date of analysis "<<ctime(&theTime)<<std::endl;
  hallc_bpms_pedestal_output <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			     <<"!  device        ,  pedestal ,   error,    gain,     error\n"
			     <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			     <<std::endl;
  
  hallc_bpms_pedestal_output << hallc_bpms_pedestal_stream.str();
  std::cout << "\n" << hallc_bpms_pedestal_stream.str() <<std::endl;
  hallc_bpms_pedestal_output.close();
      
  gDirectory->Delete("*");

  theApp.Run();
  return 0;
}



Bool_t
bpm_calibrate(std::vector<BeamMonitor> &bpm, BeamMonitor &reference, const char* run_number)
{
  std::cout << "\n";

  std::cout << reference << std::endl;
  
  TString  device_name;
  TString  antenna    [4] = {"XP","XM","YP","YM"};
  TString  plotcommand[4];
  TString  plot_residual_command[4];
  TString  bpm_name   [4];
  TString  bpm_samples[4];
  TString  reference_name;
  TCut     bpm_cut    [4] = {""};

  Double_t reference_fit_range[2] = {0.0};
 
  TF1  *bpm_fit[4]  = {NULL};
  TH1D *bpm_hist[4] = {NULL};
  TH1D *bpm_res[4]  = {NULL};

  Int_t i = 0;

  device_name = bpm.at(0).GetName(); // the same name over the beamMonitor vector
  reference_name = reference.GetAliasName();
  reference_fit_range[0] = reference.GetFitRangeMin();
  reference_fit_range[1] = reference.GetFitRangeMax();
  
  bpm_canvas->Clear();
  bpm_canvas->Divide(4,2);
 

  for(i=0;i<4;i++) {
    
    bpm_name[i]    = device_name + antenna[i];
    bpm.at(i).SetName(bpm_name[i]);
    
    bpm_samples[i] = bpm_name[i] + ".num_samples";
    if(event_range_flag)
      bpm_cut[i]     = Form("%s.Device_Error_Code == 0 && %s.Device_Error_Code == 0 && CodaEventNumber>%f && CodaEventNumber<%f", 
			    bpm_name[i].Data(),hallc_bcm.GetName().Data(),event_range[0],event_range[1]);
    else
      bpm_cut[i]     = Form("%s.Device_Error_Code == 0 && %s.Device_Error_Code == 0", 
			    bpm_name[i].Data(), hallc_bcm.GetName().Data());

    plotcommand[i] = bpm_name[i] + ".hw_sum_raw/" + bpm_samples[i] + ":" + reference_name;

    bpm_canvas->cd(i+1);
    
    bpm_hist[i] = GetHisto(
			   mps_tree_in_chain, 
			   plotcommand[i], 
			   bpm_cut[i], 
			   "prof"); // profs : large errors why?

    if(not bpm_hist[i]) {
      std::cout << "Please check " 
		<< plotcommand[i].Data()
		<< std::endl;
      bpm_canvas ->Close();
      delete bpm_canvas; bpm_canvas = NULL;
      return false;
    }

    bpm_hist[i] -> GetXaxis()->SetTitle("current (#muA)");
    bpm_hist[i] -> GetYaxis()->SetTitle("ADC coounts/num_samples");
    bpm_hist[i] -> GetYaxis()-> SetTitleOffset(2.2);
    bpm_hist[i] -> SetTitle(Form("Run %s : %s fit", run_number, bpm_name[i].Data()));

    bpm_hist[i] -> Draw("");
    bpm_hist[i] -> Fit("pol1", "E M R F Q" , "", reference_fit_range[0],  reference_fit_range[1]);

    bpm_fit[i] = bpm_hist[i]  -> GetFunction("pol1");
    gPad->Update();
    
    if(bpm_fit[i]) {
      bpm_fit[i] -> SetLineColor(kRed);
      bpm.at(i).SetPed(bpm_fit[i]->GetParameter(0));
      bpm.at(i).SetPedErr(bpm_fit[i]->GetParError(0));
      bpm.at(i).SetSlop(bpm_fit[i]->GetParameter(1));
      bpm.at(i).SetSlopErr(bpm_fit[i]->GetParError(1));
      
      /* Draw fit residuals */
      bpm_canvas->cd(4+i+1);
      
      plot_residual_command[i] = Form("(( %s.hw_sum_raw/%s )-( %s*%f + %f )):%s",
				      bpm_name[i].Data(),
				      bpm_samples[i].Data(),
				      reference_name.Data(),
				      bpm.at(i).GetSlope(),
				      bpm.at(i).GetPed(),
				      reference_name.Data() );
      
      if(event_range_flag)
	bpm_cut[i]     = Form(" %s.Device_Error_Code == 0 && %s.Device_Error_Code == 0 && CodaEventNumber>%f && CodaEventNumber<%f && %s>%f && %s<%f", 
			      bpm_name[i].Data(),reference.GetName().Data(),event_range[0],event_range[1],
			      reference_name.Data(),reference_fit_range[0],reference_name.Data(),reference_fit_range[1]);
      else
	bpm_cut[i]     = Form(" %s.Device_Error_Code == 0 && %s.Device_Error_Code == 0 &&  %s>%f && %s<%f", 
			      bpm_name[i].Data(), hallc_bcm.GetName().Data(),
			      reference_name.Data(),reference_fit_range[0],reference_name.Data(),reference_fit_range[1]);
      

      bpm_res[i] = GetHisto(mps_tree_in_chain, plot_residual_command[i], bpm_cut[i], "BOX");

      if (not bpm_res[i]) {
	std::cout<<" Unable to draw residual plot"<<plot_residual_command[i]<<std::endl;
	//	bpm_canvas ->Close();
	//	delete bpm_canvas; bpm_canvas = NULL;
	return false;	
      }
      
      bpm_res[i] -> GetXaxis()->SetTitle("current (#muA)");
      bpm_res[i] -> GetYaxis()->SetTitle("fit residual");
      bpm_res[i] -> GetYaxis()-> SetTitleOffset(2.2);
      bpm_res[i] -> SetTitle(bpm_name[i]+" fit residual");
      
    }
    else {
      bpm.at(i).SetPed(-1);
      bpm.at(i).SetPedErr(-1);
      bpm.at(i).SetSlop(-1);
      bpm.at(i).SetSlopErr(-1);
      
    }

    gPad->Update();

    std::cout << bpm.at(i) << std::endl;
  }
  
  bpm_canvas -> Modified();
  bpm_canvas -> Update();
  bpm_canvas -> Print(bpm_plots_filename+".ps");

  return true;
}




void 
print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "\n");
  fprintf (stream, "This program is used to do BPM calibrations.\n");
  fprintf (stream, "Usage: %s -r {bpm_run_number} -c {bcm_run_number} -e {a:b} \n", program_name);
  fprintf (stream, 
  	   " -r bpm_run_number : run number of BPMs calibration \n"
	   " -c bcm_run_number : run number of BCMs calibration \n"
	   " -c ref_bcm_name : bcm used for calibration \n"
	   " -e fit percent range :  -e a:b [0.01*a*bcm_current_range, 0.01*b*bcm_current_range] default a=20 and b=95\n"
	   " -s event range : event cut based on CodaEventNumber\n"
	   );
  fprintf(stream, 
	  "\n Note: If a bcm calibration run is not selected using the -c flag,\n"
	  " by default, the bcm results from the current run will be used to obtain values of beam current.\n"
	  " Use the -c flag only if you want to use a different bcm calibration to calibrate the bcm in this run.\n");
  fprintf (stream, "\n");
  exit (exit_code);
}




