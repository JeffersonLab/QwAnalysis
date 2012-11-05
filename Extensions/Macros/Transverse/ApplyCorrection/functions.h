#ifndef FUNCTIONS_H
#define FUNCTIONS_H
 

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
#include <TMath.h>
#include <string.h>

TString query_regressed(TString datatable, TString detector, TString measurement, TString ihwp, Double_t slug_number);
TString get_query(TString detector, TString measurement, TString ihwp, Double_t slug_number);
Bool_t get_octant_data(TString devicelist[],TString ihwp,Double_t slug_number, 
		     Double_t value[], Double_t error[]);
void fit_and_correct();
void get_residual_polarization(Double_t in_slug, Double_t out_slug);
void read_slug_numbers(TString filename);
Double_t get_patterns(TString detector, TString measurement, TString ihwp, Double_t slug_number);
Double_t get_patterns(Double_t in_slug, Double_t out_slug);

// hcb list
TString hcb[6]=
  {"qwk_charge","qwk_targetX","qwk_targetY","qwk_targetXSlope","qwk_targetYSlope",
   "qwk_energy"};


// detector list
TString quartz_barsum[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum"
   ,"qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

// database info
TSQLServer *db;
TString database="qw_run2_pass1";
TString database_stem="run2_pass1";

// text file to store asymmetries and corrections
std::ofstream Myfile;    
std::ofstream MyfileRes;    

// general variables in the order of usage
Int_t opt =1;
Int_t datopt = 1;
Int_t ropt = 1;
Int_t qtor_opt = 4;
TString target,targ, polar, good, good_for,qtor_current,qtor_stem,interaction,reg_set;
Int_t slug = 0;
Double_t in_slug;
Double_t out_slug;

// Bn
Double_t Bn = -4.80; //ppm From Qweak Transverse Preliminary v2


#endif
