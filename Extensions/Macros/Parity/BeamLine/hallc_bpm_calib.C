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
//          0.0.2 : Friday, August 20 Buddhini
//                  Added the rest of the bpms. Now there are total of 23 bpms.
//          0.0.3 : Monday, August 23 Buddhini
//                  Added some comments to clear things out.
//                  Changed tmp used as a bcm to tmpbcm on line 653    
//                  Added methods to use a user prefferd bcm by adding              
//                  ref_bcm_flag.
//          0.0.4 : Monday Septembed 13th  Buddhini
//                  Changed current from adc counts to uA using the bcm calibration slope.
//                  Modified the wire fit plots. Added new plots to plot the fit residuals.
//                  All the plots are now saved in to a .ps file
//          0.0.5 : Thursday, September 16 01:47:15 EDT 2010 jhlee
//                  - Removed the compiling warning: 
//                    deprecated conversion from string constant to ‘char*’
//                  - Changed LaTeX expression for the current unit #muA


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
#include "TStyle.h"



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
  Double_t GetSlope() {return slope[0];};
  Double_t GetSlopeErr() {return slope[1];};
  Double_t GetFitRangeMin() {return fit_range[0];};
  Double_t GetFitRangeMax() {return fit_range[1];};

  Bool_t IsReference() { return reference_flag;};
  Bool_t IsFileStream() {return filestream_flag;};

private:
  TString name;
  TString alias_name;
  Double_t offset[2];
  Double_t slope[2];
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


  // stream << " Name : " << device.name;
  // stream << " Offset : "  << std::setw(4) << device.offset[0];
  // stream << " +- "     << device.offset[1];
  // stream << " Slope :" << std::setw(4)  << device.slope[0];
  // stream << " +- "     << device.slope[1];
  // stream << " Fit Range [" << device.fit_range[0];
  // stream << " ," << device.fit_range[1];
  // stream << "]";
    
  
  return stream;
};

      

void print_usage(FILE* stream, int exit_code);
Bool_t calibrate(std::vector<BeamMonitor> &bpm, BeamMonitor &reference);

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

Int_t w = 1200;
Int_t h = 800;
TCanvas *Canvas =NULL;
TString bpm_plots_filename;

std::vector< std::vector<BeamMonitor> > hallc_bpms_list;
//std::vector<BeamMonitor> hallc_bcms_list;
BeamMonitor hallc_bcm;


Int_t
main(int argc, char **argv)
{
 

  TApplication theApp("App", &argc, argv);


  char* run_number = NULL;
  char* ref_bcm_name = NULL;

  Bool_t file_flag = false;
  Bool_t bcm_ped_file_flag = false;
  Bool_t ref_bcm_flag = false;
  Bool_t fit_range_flag = false;
  Double_t fit_range[2] = {0.0};

  TString bcm_ped_filename;
 
  Int_t  bcm_ped_runnumber = 0;

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
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);

  program_name = argv[0];

  int cc = 0; 

  /*command line arguments*/
  while ( (cc= getopt(argc, argv, "r:c:b:e")) != -1)
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
    fit_range[0] = 0.15;
    fit_range[1] = 0.85;
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

  /*if a bcm calibration run is not specified, use the default calibration run 5070*/
  if (not bcm_ped_file_flag) {
    bcm_ped_runnumber = 5070;
  }

  /*if a bcm is not specified use the defaults bcm , bcm1*/
  if (not ref_bcm_flag) {
    ref_bcm_name = Form("bcm1");
  }

  /*file containing the bcm calibration results*/
  bcm_ped_filename = Form("hallc_bcm_pedestal_%d.txt", bcm_ped_runnumber);

  /*file containing the bpm calibration plots*/
  bpm_plots_filename = Form("%s_hallc_bpm_calib_plots", run_number);

 
  /*list of hallc bpms*/

  BeamMonitor qwk_3c07 ("qwk_3c07");    // begining of arc
  BeamMonitor qwk_3c08 ("qwk_3c08");
  BeamMonitor qwk_3c11 ("qwk_3c11");
  BeamMonitor qwk_3c12 ("qwk_3c12");    // highest dispersion
  BeamMonitor qwk_3c14 ("qwk_3c14");
  BeamMonitor qwk_3c16 ("qwk_3c16");
  BeamMonitor qwk_3c17 ("qwk_3c17");    // end of arc (green wall)
  BeamMonitor qwk_3c18 ("qwk_3c18");    // Region 1 after the shielding wall (in not songsheet)
  BeamMonitor qwk_3c19 ("qwk_3c19");    // Region 1 after the shielding wall (in not songsheet)
  BeamMonitor qwk_3c07a("qwk_3c07a"); 
  BeamMonitor qwk_3p02a("qwk_3p02a");   // compton chicane
  BeamMonitor qwk_3p02b("qwk_3p02b");   // compton chicane
  BeamMonitor qwk_3p03a("qwk_3p03a");   // compton chicane
  BeamMonitor qwk_3c20 ("qwk_3c20");    // Region 3
  BeamMonitor qwk_3c21 ("qwk_3c21");    // Region 3
  BeamMonitor qwk_3h02 ("qwk_3h02");    // Region 4
  BeamMonitor qwk_3h04a("qwk_3h04a"); 
  BeamMonitor qwk_3h07a("qwk_3h07a");
  BeamMonitor qwk_3h07b("qwk_3h07b"); 
  BeamMonitor qwk_3h07c("qwk_3h07c");  
  BeamMonitor qwk_3h08 ("qwk_3h08");    // Region 5
  BeamMonitor qwk_3h09 ("qwk_3h09");    // Region 5
  BeamMonitor qwk_3h09b("qwk_3h09b");   // Region 5


  std::vector <BeamMonitor> tmp_bpm;
  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c07);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c08);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c11);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c12);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c14);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c16);
  hallc_bpms_list.push_back(tmp_bpm);
  
  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c17);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c18);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c19);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c07a);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3p02a);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3p02b);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3p03a);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c20);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3c21);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h02);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h04a);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h07a);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h07b);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h07c);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h08);
  hallc_bpms_list.push_back(tmp_bpm);

  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h09);
  hallc_bpms_list.push_back(tmp_bpm);
  
  tmp_bpm.clear();
  tmp_bpm.assign(4, qwk_3h09b);
  hallc_bpms_list.push_back(tmp_bpm);

 

  /*Open the root file*/
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

  
  /*Open the bcm calibration results file*/  
  ifstream in;
  in.open(bcm_ped_filename.Data());
  if(not in.is_open() ) {
    std::cout << "There is no BCMs calibration file with the name "
	      << bcm_ped_filename 
	      << ". Please check or run BCM calibration first."
	      << std::endl;
    return EXIT_FAILURE;
  };

  TString name;
  Double_t offset[2] = {0.0};
  Double_t slope[2] = {0.0};
  Int_t cnt = 0;


  /*Load the calibration information for the bcm from the bcm calibration file*/
  while(in.good()) {

    in >> name >> offset[0] >> offset[1] >> slope[0] >> slope[1];
    if(not in.good()) break;
    else {
      if(name.Contains(ref_bcm_name)){
	hallc_bcm.SetName(name);
	hallc_bcm.SetPed(offset[0]);
	hallc_bcm.SetPedErr(offset[1]);
	hallc_bcm.SetSlop(slope[0]);
	hallc_bcm.SetSlopErr(slope[1]);
      }
    }
      
    // std::cout << name << " " 
    // 	      << offset[0] << " " << offset[1] << " "
    // 	      << slope[0] << " " << slope[1] << " "
    // 	      << std::endl;
  }

  in.close();
  mps_tree = (TTree*) file->Get("Mps_Tree");
  TH2D *tmp;
  Double_t tmp_max = 0.0;
  //  Int_t alias_id = 0;

  // Draw the current from the bcm
  Canvas = new TCanvas("Current" , Form("Range of currents form %s",ref_bcm_name), w, h);  
  Canvas->Clear();
  Canvas->cd();
 
  mps_tree->SetAlias("current",
		     Form("(((%s.hw_sum_raw/%s.num_samples)-%1f)/ %lf )", 
			  hallc_bcm.GetName().Data(), hallc_bcm.GetName().Data(),
			  hallc_bcm.GetPed(),hallc_bcm.GetSlope())
		     ); 
		    
  mps_tree->Draw("current>>tmp", 
		 Form("%s.num_samples>0", hallc_bcm.GetName().Data()));

  tmp = (TH2D*)gDirectory->Get("tmp");
  if(not tmp) {
    std::cout << "Please check current values" << std::endl;
    theApp.Run();
    
  }
  tmp_max = tmp -> GetXaxis() -> GetXmax();
  tmp -> GetXaxis() -> SetTitle("current (#muA)");

  fit_range[0] *= tmp_max;
  fit_range[1] *= tmp_max;
  
  hallc_bcm.SetAliasName("current"); 
  hallc_bcm.SetFitRange(fit_range);
  hallc_bcm.SetReference();


  // Print the plot on to a file
  Canvas->Print(bpm_plots_filename+".ps(");
  
  /*Start to calibrate the bpms*/
  std::size_t i = 0; 
  cnt = 0;
  std::cout << "how many bpms ? " << hallc_bpms_list.size() << std::endl;
  for (i=0;i < hallc_bpms_list.size(); i++ ) {
    std::cout << "\n" 
	      << cnt++ 
	      << ": onto calibrating " << hallc_bpms_list.at(i).at(0).GetName() 
	      << std::endl;
    
    Bool_t check = false;
    check = calibrate(hallc_bpms_list.at(i), hallc_bcm ); 
    //    if(not check) theApp.Run();
  }
  
  /*open a file to store the pedestals*/
  std::ofstream       hallc_bpms_pedestal_output;
  std::ostringstream  hallc_bpms_pedestal_stream;
  hallc_bpms_pedestal_output.clear();


  hallc_bpms_pedestal_output.open(Form("hallc_bpm_pedestal_%s.txt", run_number));
  hallc_bpms_pedestal_stream.clear();
  
  std::size_t antenna_id = 0;
 
  // // // std::cout << "size " << hallc_bpms_list.size() << std::endl;
  for (i=0; i < hallc_bpms_list.size(); i++) {

    for(antenna_id = 0; antenna_id <hallc_bpms_list.at(i).size(); antenna_id++ ) {
      hallc_bpms_list.at(i).at(antenna_id).SetFileStream();
      hallc_bpms_pedestal_stream << hallc_bpms_list.at(i).at(antenna_id);
    }
  } 

  hallc_bpms_pedestal_output << hallc_bpms_pedestal_stream.str();
  std::cout << "\n" << hallc_bpms_pedestal_stream.str() <<std::endl;
  hallc_bpms_pedestal_output.close();
      
  gDirectory->Delete("*");

  theApp.Run();
  return 0;
}



Bool_t
calibrate(std::vector<BeamMonitor> &bpm, BeamMonitor &reference)
{
  std::cout << "\n";
  std::cout << reference << std::endl;
  
  TString devname;
  TString antenna[4]={"XP","XM","YP","YM"};
  TString plotcommand[4];
  TString bpm_name[4];
  TString bpm_samples[4];
  TString reference_name;
  Double_t reference_fit_range[2] = {0.0};
  TCut scut = "";

  TF1 *fit[4] = {NULL};
  TH1D *hprof[4] = {NULL};
  TH1D *hres[4] = {NULL};

  Int_t i = 0;
//   Int_t w = 1100;
//   Int_t h = 600;
//   TCanvas *Canvas =NULL;
  devname = bpm.at(0).GetName(); // the same name over the beamMonitor vector

  
  Canvas->Clear();
  Canvas->Divide(4,2);
  

  reference_name = reference.GetAliasName();
 
  reference_fit_range[0] = reference.GetFitRangeMin();
  reference_fit_range[1] = reference.GetFitRangeMax();

  
  for(i=0;i<4;i++) {
    
    Canvas->cd(i+1);
    bpm_name[i]    = devname + antenna[i];
    bpm.at(i).SetName(bpm_name[i]);
    bpm_samples[i] = bpm_name[i] + ".num_samples";
    plotcommand[i] = bpm_name[i]+".hw_sum_raw/" + bpm_samples[i] + ":" + reference_name;
    
    hprof[i] = GetHisto(mps_tree, plotcommand[i], scut, "prof"); // profs : large errors why?
    if(not hprof[i]) {
      std::cout << "Please check " << plotcommand[i].Data()
		<< std::endl;

      Canvas ->Close();
      delete Canvas; Canvas = NULL;
      return false;
    }
    hprof[i] -> GetXaxis()->SetTitle("current (#muA)");
    hprof[i] -> GetYaxis()->SetTitle("ADC coounts/num_samples");
    hprof[i] -> GetYaxis()-> SetTitleOffset(2.2);
    hprof[i] -> SetTitle(bpm_name[i]+" fit");
    hprof[i] -> Draw("");
    hprof[i] -> Fit("pol1", "E M Q" , "", reference_fit_range[0],  reference_fit_range[1]);

    fit[i] = hprof[i]  -> GetFunction("pol1");
    
    if(fit[i]) {
      fit[i] -> SetLineColor(kRed);
      bpm.at(i).SetPed(fit[i]->GetParameter(0));
      bpm.at(i).SetPedErr(fit[i]->GetParError(0));
      bpm.at(i).SetSlop(fit[i]->GetParameter(1));
      bpm.at(i).SetSlopErr(fit[i]->GetParError(1));

      // Draw the residuals
      Canvas->cd(4+i+1);

      plotcommand[i] = Form("(( %s.hw_sum_raw/%s )-( %s*%1f + %1f )):%s",
			    bpm_name[i].Data(),bpm_samples[i].Data(),reference_name.Data(),
			    bpm.at(i).GetSlope(),bpm.at(i).GetPed(),reference_name.Data() );


      mps_tree->Draw(plotcommand[i],"","box");
      hres[i] = (TH1D*) gPad->GetPrimitive("htemp");

      if (not hres[i]) {
	std::cout<<" Please check residual plot"<<plotcommand[i]<<std::endl;
	Canvas ->Close();
	delete Canvas; Canvas = NULL;
	return false;	
      }
      hres[i] -> GetXaxis()->SetTitle("current (#muA)");
      hres[i] -> GetYaxis()->SetTitle("fit residual");
      hres[i] -> GetYaxis()-> SetTitleOffset(2.2);
      hres[i] -> SetTitle(bpm_name[i]+" fit residual");
      
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
  Canvas -> Modified();
  Canvas -> Update();
  Canvas->Print(bpm_plots_filename+".ps");

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
	   " -e fit percent range (default [0.01*a*bcm_current_range, 0.01*b*bcm_current_range]\n"
	   );
  fprintf (stream, "\n");
  exit (exit_code);
}






// class QwString: public TString 
// {
//  public:
//   QwString();
//   ~QwString(){};

// public:
//   void SetCommentSymbol(const std::string s) {
//     fCommentSymbol = s;
//   }
//   std::string GetCommentSymbol() { return fCommentSymbol;};

//   QwString PurifyFromComment(const std::string s);
//   QwString RemoveWhiteSpaces();
//   QwString Purify(const std::string s);

//   TString TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id) ;

// protected:
//   std::string fCommentSymbol;
// private:
//   QwString PurifyFromDefault();
  
// };

// QwString::QwString()
// {
//   fCommentSymbol.clear();
// };

// QwString 
// QwString::PurifyFromDefault()
// {
//   this -> ToLower();
//   if (not (this->BeginsWith(fCommentSymbol)) ) {
//     if( this->Contains(fCommentSymbol) ) {
//       Int_t pos = this->First(fCommentSymbol.c_str()); 
//       // pos doesn't need to be initialized by "0", because it always has "value" form "First"
//       this->Replace(pos, this->Sizeof() - pos, "");
//     }
//   }
//   else {
//     this->Clear();
//   }
//   return *this;
// };

// QwString 
// QwString::PurifyFromComment(const std::string s)
// {
//   fCommentSymbol = s;
//   this->PurifyFromDefault();
//   return *this;
  
// };


// QwString 
// QwString::RemoveWhiteSpaces()
// {
//   this -> ReplaceAll(' ', '\0');
//   return *this;
// };


// QwString 
// QwString::Purify(const std::string s)
// {
 
//   this -> PurifyFromComment(s);
//   if(not this->IsNull()) {
//     this -> RemoveWhiteSpaces();
//   }
//   return *this;
// };


// TString 
// QwString::TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id)
// {
//   Bool_t local_debug = false;
//   TString name;  
  
//   TObjArray* Strings = this->Tokenize(delim); // break line into tokens
//   Int_t token_numbers = Strings->GetEntriesFast();

//   if(token_numbers == interesting_token_num) {
//     TIter iString(Strings);
//     TObjString* os= NULL;
//     Int_t token_ids = 0;
//     while ((os=(TObjString*)iString())) {
//       if(token_ids == interesting_token_id) {
// 	name = os->GetString();
// 	break;
//       }
//       token_ids++;
//     }
//     delete Strings;

//     if(local_debug) {
//       std::cout << " tokens #"
// 		<< token_numbers
// 		<< " " << *this
// 		<< " " << name
// 		<< std::endl;
//     }
//   }
//   else {   
//     name.Clear(); // return empty string
//     if(local_debug) {
//       std::cout << "Token number is not in the possible token numbers" << std::endl;
//     }
//   }
//   return name;
// }
	



// class QwBeamMonitorDevice
// {
// public:
//   QwBeamMonitorDevice();
//   ~QwBeamMonitorDevice(){};

//   Bool_t OpenMapFile(TString mapfilename);

//   std::vector<TString > wdevices;
//   std::vector<TString > bpms;
//   // std::vector<TString > trimbpms;
//   std::vector<TString > bcms;
//   std::vector<TString > halos;
//   std::vector<TString > others;
//   std::vector<TString > monitors;


//   std::vector<TString> GetBcmsList() {return bcms;};
//   std::vector<TString> GetBpmsList() {return bpms;};
//   std::vector<TString> GetTrimBpmsList();//{return trimbpms;};
//   std::vector<TString> GetHalosList() {return halos;};
//   std::vector<TString> GetOthersList() {return others;};
//   std::vector<TString> GetMonitorsList() {return monitors;};
  
//   void WantBpms(Bool_t in)   {
//     if(not IsCustomizedMonitors()) fBpmsOnFlag = in;
//     else std::cout << "BPMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };

//   // void WantTrimBpms(Bool_t in)   {
//   //   if(not IsCustomizedMonitors())  {
//   //     fTrimBpmsOnFlag = in;
//   //     this->WantBpms(in);
//   //   }
//   //   else std::cout << "Trim BPMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   // };

//   void WantBcms(Bool_t in)   {
//     if(not IsCustomizedMonitors()) fBcmsOnFlag = in;
//     else std::cout << "BCMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };
//   void WantHalos(Bool_t in)  {
//     if(not IsCustomizedMonitors()) fHalosOnFlag = in;
//     else std::cout << "Halos is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };
  
//   void WantOthers(Bool_t in) {
//     if(not IsCustomizedMonitors()) fOthersOnFlag = in;
//     else std::cout << "Others is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };

//   void WantCustomizedMonitors(Bool_t in, TString filename);
//   void TrimBpmsAntenna();

//   Bool_t IsBpms()   {return fBpmsOnFlag;};
//   // Bool_t IsTrimBpms() {return fTrimBpmsOnFlag;};
//   Bool_t IsBcms()   {return fBcmsOnFlag;};
//   Bool_t IsHalos()  {return fHalosOnFlag;};
//   Bool_t IsOthers() {return fOthersOnFlag;};
 
//   Bool_t IsCustomizedMonitors() {return fMonitorsOnFlag;};

// private:

//   ifstream fMapFile; 
//   ifstream fWantedDeviceFile;

//   void  OpenWantedDeviceFile(TString filename);
//   TString GetMapDir(TString mapfilename, Bool_t current_dir_flag);

//   Bool_t fBpmsOnFlag;
//   // Bool_t fTrimBpmsOnFlag;
//   Bool_t fBcmsOnFlag;
//   Bool_t fHalosOnFlag;
//   Bool_t fOthersOnFlag;
//   Bool_t fMonitorsOnFlag;
 

// };


// QwBeamMonitorDevice::QwBeamMonitorDevice()
// {
//   fBpmsOnFlag   = false;
//   // fTrimBpmsOnFlag = false;
//   fBcmsOnFlag   = false;
//   fHalosOnFlag  = false;
//   fOthersOnFlag = false;
//   fMonitorsOnFlag = false;

//   fMapFile.clear(); 
//   fWantedDeviceFile.clear();

//   wdevices.clear();
//   bpms.clear();
//   //  trimbpms.clear();
//   bcms.clear();
//   halos.clear();
//   others.clear();
//   monitors.clear();
// };

// void 
// QwBeamMonitorDevice::WantCustomizedMonitors(Bool_t in, TString filename)
// {
//   fMonitorsOnFlag = in;
//   // if one wants to use their own monitors,
//   // turn off all monitors by default
//   if(fMonitorsOnFlag) {
//     if(IsBpms())   fBpmsOnFlag   = false;
//     // if(IsTrimBpms()) fTrimBpmsOnFlag = false;
//     if(IsBcms())   fBcmsOnFlag   = false;
//     if(IsHalos())  fHalosOnFlag  = false;
//     if(IsOthers()) fOthersOnFlag = false;
//     OpenWantedDeviceFile(filename);
//   }
//   else {
//     if(IsBpms())   fBpmsOnFlag   = true;
//     // if(IsTrimBpms()) fTrimBpmsOnFlag = true;
//     if(IsBcms())   fBcmsOnFlag   = true;
//     if(IsHalos())  fHalosOnFlag  = true;
//     if(IsOthers()) fOthersOnFlag = false;
//   }
  
//   return;
// }


// void
// QwBeamMonitorDevice::OpenWantedDeviceFile(TString filename)
// {
//   Bool_t local_debug = false;
//   if(local_debug) {
//     std::cout << "QwBeamMonitorDevice::OpenWantedDeviceFile()"
// 	      << std::endl;
//   }
 
//   fWantedDeviceFile.clear();
//   filename = GetMapDir("", true) + "beam_calib_data/" +filename;
//   fWantedDeviceFile.open(filename);
//   if(not fWantedDeviceFile.is_open()) {
//     std::cout << "I cannot open " 
// 	      << filename  
// 	      << ". Use the default configuration."
// 	      << std::endl;
//   }
//   else {
//     QwString line;
//     //    QwString line; line.Clear();
    
//     while (not fWantedDeviceFile.eof() ) {
//       line.ReadLine(fWantedDeviceFile);   
//       line.Purify("#");
//       if(not line.IsNull()) {
// 	if(local_debug) { 
// 	  std::cout << line << std::endl;
// 	}
// 	wdevices.push_back(line);
// 	line.Clear();
//       } // if(not line.IsNull()) {
//     } //   while (not fWantedDeviceFile.eof() ) {
//     line.Clear();
//   }

//   fWantedDeviceFile.close();

//   return;
// };

// TString 
// QwBeamMonitorDevice::GetMapDir(TString mapfilename, Bool_t current_dir_flag)
// {
//   TString get;
//   if(current_dir_flag) {
//     get = getenv("PWD");
//     get += "/";
//     if(get.IsNull()) {
//       std::cout << "Please check your path "  
// 		<< std::endl;
//       return get;
//     }
//     else {
//       mapfilename = get + mapfilename;
//       return mapfilename;
//     }
//   }
//   else {
//     get =  getenv("QWANALYSIS");
//     get += "/";
//     if(get.IsNull()) {
//       std::cout << "Please check your QWANALYSIS path "  
// 		<< std::endl;
//       return get;
//     }
//     else {
//       mapfilename = get + "Parity/prminput/" + mapfilename;
//       return mapfilename;
//     }
//   }
// };


// Bool_t
// QwBeamMonitorDevice::OpenMapFile(TString mapfilename)
// {


//   Bool_t local_debug = false;

//   if(local_debug) {
//     std::cout << "QwBeamMonitorDevice::OpenMapFile " << mapfilename << std::endl;
//   }

//   fMapFile.clear();
//   fMapFile.open(GetMapDir(mapfilename, false));
  
//   if(not fMapFile.is_open()) {
//     std::cout << "I cannot open your mapfile " 
// 	      << mapfilename  
// 	      << ". Please Check it again."
// 	      << std::endl;
//     return false;
//   }
//   else {
//     //    if(local_debug) {
//     std::cout << mapfilename << " is opened." << std::endl;
//     //    }

//     char delimiters[] = ",";
//     QwString line; 
//     TString device_type; device_type.Clear();
//     TString device_name; device_name.Clear();

//     std::vector<TString>::iterator pd;

//     while (not fMapFile.eof() ) {
//       line.ReadLine(fMapFile);   
//       line.Purify("!");
//       if(not line.IsNull()) {
// 	if(local_debug) std::cout << line << std::endl;
// 	device_type = line.TokenString(delimiters, 5, 3);
// 	if (not device_type.IsNull()) {
// 	  device_name = line.TokenString(delimiters, 5, 4);
// 	  if(IsCustomizedMonitors()) {
// 	    for (pd = wdevices.begin(); pd!=wdevices.end(); pd++) {
// 	      if(device_name == *pd) {
// 		monitors.push_back(*pd);
// 	      }
// 	    }
// 	  }
// 	  else {
// 	    if(device_type == "bcm") {
// 	      if(IsBcms()) bcms.push_back(device_name);
// 	    }
// 	    else if (device_type == "bpmstripline") {
// 	      if(IsBpms()) bpms.push_back(device_name);
// 	    }
// 	    else if (device_type == "halomonitor") {
// 	      if(IsHalos()) halos.push_back(device_name);
// 	    }
// 	    else {
// 	      if(IsOthers()) others.push_back(device_name);
// 	    }
// 	  }
// 	} // if (not device_type.IsNull()) {
//       } // if(not line.IsNull()) {
//     } //   while (not mapfile.eof() ) {
//   }
//   fMapFile.close();
//   return true;
// }

// std::vector<TString> 
// QwBeamMonitorDevice::GetTrimBpmsList()
// {
//   std::vector<TString > trimbpms;
//   std::vector<TString>::iterator it;
  
//   trimbpms.clear();

//   TString string_old;
//   TString string_new;
//   string_old.Clear();
//   string_new.Clear();

//   Int_t cnt = 0;
//   std::size_t i  = 0;
//   for(i=0; i< bpms.size(); i++ ){
//     string_new = bpms.at(i).Remove(8);
//     if (string_old not_eq string_new) trimbpms.push_back(string_new);
        
//     string_old = string_new;
//     cnt++;
//   //   if(not *it) trimbpms.push_back(temp);
//   }
//   return trimbpms;
// }

