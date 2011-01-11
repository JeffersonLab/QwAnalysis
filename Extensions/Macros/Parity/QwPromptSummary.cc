//********************************************************************//
// Author : Rakitha Beminiwattha (rakithab@jlab.org)
// Date   : August 23, 2010
//********************************************************************//
//
// This macro generates a summary report for a given run number
//
// To compile this code do a make.
// To use the exe file from command prompt type:
// ./promptsummary  runnumber  i stem
//contact rakitha b (rakithab@jlab.org) for any information/updates

// Tuesday, December 21 22:43:31 EST 2010, jhlee
// - align all outputs for clarity
// - fixed bcm1 normalization of halo rate
// - fixed halo rate unit



// TODO
//    -- check all units which we try to print out again.
//       there are some unclear units 
//    -- check 31MHz and its return are empty in
//       hel_histo, but aren't in hel_tree
//          



#include <vector>


#include <cmath>
#include <iostream>
#include <fstream>
#include <new>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <time.h>


#include "TRandom.h"
#include "TH2F.h"
#include "TTree.h"
#include "TF1.h"
#include "TF2.h"
#include "TCut.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TString.h"
#include "TDatime.h"
#include "TMath.h"
#include "TDirectory.h"


TTree *nt;

TString CanvasTitle;
TString get;

TChain tree_chain("Hel_Tree");


Bool_t comparisonon=kFALSE;//kTRUE;  //this should be truned on in order to compare to golden values from the reference file
Bool_t expertmode = kTRUE;//kFALSE;//kTRUE;//plots generated from the tree
Int_t Read_ref_file(TString name_of_ref_file);
void compare_to_golden_value(TString valuetocompareto, Double_t valuet, Double_t precision=0);

void FillBeamParameters();
void FillMDParameters();
void FillLUMIParameters();
//Fit the histogram with a gaussian to obtain the mean and width
void Fit_with_a_gaussian(TString histname, Double_t *container, Double_t factor=1);
//Fit the diff histogram with a gaussian to obtain the mean and width
void Fit_with_a_gaussian_diff(TString histname1,TString histname2, Double_t *container, Double_t factor=1);
//Obtain the mean
void Get_Mean(TString histname, Double_t &container, Double_t factor);
//Obtain the mean from the tree
void Get_Tree_Mean(TString device_expression, Double_t &container, Double_t factor);
//Fit the histogram obtained from the tree  with a gaussian to obtain the mean and width
void Get_Tree_Mean_Fit_gaus(TString histname, Double_t *container, Double_t factor=1);

//Obtain the mean for diff histo
void Get_Mean_Diff(TString histname1,TString histname2, Double_t &container, Double_t factor);
//To obtain a histo from set of runlets using TH::Add
TH1F* GetHisto(TString histoname);
//To obtain a histo of asymmetry difference for two devices  from set of runlets using TH::Add
TH1F* GetDiffHisto(TString histoname1,TString histoname2);
//To obtain a 1D histogram from helicity tree
TH1F* Get1DHisto(const TString name, const TCut cut, Option_t* option="goff");
//To obtain a 2D histogram from helicity tree
TH2F* Get2DHisto(const TString name, const TCut cut, Option_t* option="goff");
//Load the tree
Int_t  GetTree(TString filename, TChain* chain);

TString 
TopRule(const char* one, const char* two, const char* three, const char* four)
{
  return Form("\n %14s | %12s | %25s | %25s", one, two, three, four);
};

TString 
MidRule_2(const char* one, Double_t d_one, Double_t val[4])
{
  return Form("\n %14s | %12.2f | %+12.2f +/- %8.2f | %+12.2f +/- %8.2f", one, d_one, val[0], val[2], val[1], val[3]);
};


TString 
MidRule_3(const char* one, Double_t d_one, Double_t val[4])
{
  return Form("\n %14s | %12.3f | %+12.3f +/- %8.3f | %+12.3f +/- %8.3f", one, d_one, val[0], val[2], val[1], val[3]);
};



TString 
MidRule_6(const char* one, Double_t d_one, Double_t val[4])
{
  return Form("\n %14s | %12.6f | %+12.6f +/- %8.6f | %+12.6f +/- %8.6f", one, d_one, val[0], val[2], val[1], val[3]);
};

TString
Halo(const char*one, Double_t d_mean, Double_t d_intensity)
{
  return Form("\n %20s rate (Hz) = %12.1f (%12.1f (Hz/uA))\n", one, d_mean*1000., d_intensity==0? 0.0:d_mean*1000./d_intensity);
};

TString
Clock(const char*one, Double_t d_mean)
{
  return Form("\n %20s rate (Hz) = %12.1f \n", one, d_mean*1000. );
};

TString
HorSingleLine()
{
  TString tmp = "---------------------------------------------------------------------------------------\n";
  return tmp;
};

TString
HorDoubleLine()
{
  TString tmp = "=======================================================================================\n";
  return tmp;
};

//////////////////////////////////////////////////////// 
// the following values are needed for the comparison of the extracted
// values with the golden values
typedef struct REFDATA
{
  TString name;
  Double_t value;
  Double_t tolerance;
  TString unit;
};
std::vector <REFDATA> ref;//stores the reference parameters/values

std::vector <TString> results;//Keeps the run summary 
std::vector <TString> util;//keeps the parameters for each sub section of summary
std::vector<TString> filenames;//keeps the list of root files that used during the analysis
Double_t intensity;
Double_t bcm1_current_uA;
Int_t good_or_bad;
Int_t total_bad; 
Bool_t verbose;
//vectors to store TDirecotry from each runlet
std::vector<TFile*> file_list;//List of TFile  to access histograms
#include <TF1.h>

Bool_t NoFits=kTRUE;

int main(Int_t argc,Char_t* argv[])
{
  //The default root file stem is QwRun_
  TDatime tday;
  TString runnum;
  TString s_argc;
  TString rootfile_stem="Qweak_";
  TFile *f;
  Int_t outputformat=0;
  Int_t noofrunlets;

  results.clear();
  if(argc<2){
    std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
    std::cerr<<" ./promptsummary  runnumber i rootfile-stem=Qweak_\n";
    std::cerr<<" Give a run number \n";
    std::cerr<<" Give i=0 print result on the screen (Default setting) and  \n";
    std::cerr<<" Give i=1 print result into a text file.\n";
    std::cerr<<" Give root file stem name if it is different from default value 'Qweak_' \n";
    exit(1);
  }
  else {
    runnum=argv[1];
    if (!runnum.IsDigit()){
      std::cerr<<" ********You NEED to give a valid run number************ \n";
      std::cerr<<" ./promptsummary  runnumber i rootfile-stem=Qweak_\n";
      std::cerr<<" Give a run number \n";
      std::cerr<<" Give i=0 print result on the screen (Default setting) and  \n";
      std::cerr<<" Give i=1 print result into a text file.\n";
      std::cerr<<" Give root file stem name if it is different from default value 'Qweak_' \n";
      exit(1);
    }
    if (argc > 2){//read the print option
      s_argc=argv[2];
      if (!s_argc.IsDigit()){
	std::cerr<<" ***** You NEED to give a valid print option 1 or 0 ************\n";
	std::cerr<<" ./promptsummary  runnumber i=0 rootfile-stem=Qweak_\n";
	std::cerr<<" Give a run number \n";
	std::cerr<<" Give i=0 print result on the screen (Default setting) and  \n";
	std::cerr<<" Give i=1 print result into a text file.\n";
	std::cerr<<" Give root file stem name if it is different from default value 'Qweak_' \n";
	exit(1);
      }
      outputformat=atoi(argv[2]);
      if (argc >= 4){
	s_argc=argv[3];//reads the root file stemTChain * tree_chain;
	rootfile_stem=s_argc;
      }
      
    }
  }

  std::cout<<"Run # "<<runnum<<" Print option is "<<outputformat<<" Root file stem  "<<rootfile_stem<<std::endl;

  //Looking for the given root file
  TString rootfilename;
  Int_t foundfile = 0;
  TString runlet="";
  Int_t chain_status = 0;
  rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+".root";
  filenames.clear(); 
  if((f = new TFile(rootfilename.Data(),"READ"))!=NULL &&!(f->IsZombie()) &&(f->IsOpen())){
    foundfile = 1;//found regular root file (No runlets)
    filenames.push_back(rootfilename);
    if (expertmode){
      chain_status = GetTree(Form("%s%s.root",rootfile_stem.Data(),runnum.Data()), &tree_chain);
      if(chain_status == 0) {
	expertmode=kFALSE;
	std::cerr<<" Failed to access the tree \n";
      }
    }
  }
  
  if (foundfile==0){//looking for runlet based root files
    rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+".000.root";
    if((f = new TFile(rootfilename.Data(),"READ"))!=NULL &&!(f->IsZombie()) &&(f->IsOpen())){
      noofrunlets=0;
      runlet.Form("%03d",noofrunlets);
      rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+"."+runlet+".root";
      if (expertmode){
	chain_status = GetTree(Form("%s%s*.root",rootfile_stem.Data(),runnum.Data()), &tree_chain);
	if(chain_status == 0) {
	  expertmode=kFALSE;
	  std::cerr<<" Failed to access the tree \n";
	}
      }

      while ((f = new TFile(rootfilename.Data(),"READ"))!=NULL &&!(f->IsZombie()) &&(f->IsOpen())){
	std::cout<<rootfilename<<std::endl;
	file_list.push_back(f);
	filenames.push_back(rootfilename);
	f=NULL;//reset the root file
	noofrunlets++;
	runlet.Form("%03d",noofrunlets);
	rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+"."+runlet+".root";
      }
      
      foundfile = 2;//found runlet base root file 
    }
  }
  if (foundfile==0){//no root files found
    std::cout<<"Root file "<<rootfilename<<" not found "<<std::endl;
    exit(1);
  }
    
  if (foundfile == 1){//regular root file found
    file_list.push_back(f);//add the regular file to the file-list vector
  }else if (foundfile == 2){//In runlet mode. We need to create the TChain
    std::cout<<" found  "<<noofrunlets<<" runlets "<<std::endl;
  }

  //std::cout<<" Mps_Tree "<<chain_Mps_Tree->GetNbranches()<<" Hel_Tree "<<chain_Hel_Tree->GetNbranches()<<std::endl;
  //std::cout<<" Mps_Tree "<<mps_tree->GetNbranches()<<std::endl;
  std::cout<<" No. of runlets : "<<file_list.size()<<std::endl;

  //Open the reference value file
  //TString name_of_ref_file=TString(getenv("QWSCRATCH")) +"/calib/golden_values_run_summary";
  TString name_of_ref_file="./golden_values_run_summary";
  results.push_back(" \n \nRoot file  on which this analysis based =\n \t");
  for (UInt_t nf=0;nf<filenames.size();nf++){
    results.push_back(filenames.at(nf));
    results.push_back("\n");
  }

  results.push_back("\n\n");
  if(comparisonon)
    results.push_back("\nFile containing the golden values =\n \t"+name_of_ref_file+"\n\n");
  Read_ref_file(name_of_ref_file);

  TH1F* h = NULL;
  Double_t mps_normalizer=0;
  Double_t qrt_normalizer=0;

  h=GetHisto("mps_histo/qwk_charge_hw");
  mps_normalizer=h->GetMean()*h->GetEntries();
  delete h;
  h=NULL;
  h = GetHisto("hel_histo/yield_qwk_charge_hw");
  qrt_normalizer=h->GetMean()*h->GetEntries();

  results.push_back(HorDoubleLine());
  //  results.push_back("========================================================\n");
  results.push_back("ANALYSIS CHECKS : \n");
  results.push_back("-----------------\n");
  results.push_back(Form("number of good quartets in this run = %6.0f \n",h->GetEntries()));
  results.push_back(Form("based on this number the run lasted = %5.1f minutes \n\n",h->GetEntries()*4./1000./60.));
  h=NULL;
  
  FillBeamParameters();
  FillMDParameters();
  FillLUMIParameters();

  results.push_back("\n \n ======== END of SUMMARY======== \n");
  if (expertmode)
    results.push_back("\n Expert mode is ON. \n");
  else
    results.push_back("\n No information from the tree. Expert mode is OFF. \n");
  
 if(outputformat==1)
    {
      TString outputfilename=TString(getenv("QWANALYSIS"))+"/Extensions/Macros/Parity/summary_"+runnum+".text";
      std::cout<<" name of the ouputfile "<<outputfilename<<std::endl;
      ofstream output(outputfilename);
      output<<"\n ======= BEGIN ======= \n";
      output<<"\t RUN = "<<runnum<<std::endl<<std::endl<<std::endl;
      for(size_t i=0;i<results.size();i++)
	output<<results[i];
      output<<"\n ======== END ======== \n";
      output<<"\n Please contact Rakitha Beminiwattha for any queries and suggestions \n rakithab@jlab.org \n\n";
      output.close();
    }
  else if(outputformat==0)
    {
      std::cout<<"\n ======= BEGIN ======= \n";
      std::cout<<"\t RUN = "<<runnum<<std::endl<<std::endl;
      for(size_t i=0;i<results.size();i++)
	std::cout<<results[i];
      std::cout<<"\n ======== END ======== \n";
      std::cout<<"\n Please contact Rakitha Beminiwattha for any queries and suggestions \n rakithab@jlab.org \n\n";
      std::cout<<std::endl<<std::endl;
    }
  else
    std::cout<<"I do not know how to output the results of this program \n";


  return(0);
}; 

//***************************************************
//***************************************************
//         Add same histogram in each runlet root file to get histogram for the run   
// Parameter - Pass the histogram name along with the directory ex. "mps_histo/qwk_bcm6_hw"        
//***************************************************
//***************************************************

TH1F* GetHisto(TString histoname){
  TH1F* histo=NULL;
  TH1F* histo_tmp=NULL;
  if (file_list.size()==0)
    return histo;

  histo=(TH1F*)file_list.at(0)->Get(histoname);
  if (histo==NULL){
    std::cerr<<"Histogram "<<histoname<<" does not exist in the root file \n";
    return histo;
  }

  for(UInt_t i=1; i<file_list.size();i++){
    histo_tmp=(TH1F*)file_list.at(i)->Get(histoname);
    if (histo_tmp!=NULL)
      histo->Add(histo_tmp,1);//this=this+histo*c1 where c1=1
    //else
    //std::cerr<<"Histogram "<<histoname<<" does not exist in the runlet "<<i<<" \n";

  }
  
  return histo;
};

//***************************************************
//***************************************************
//         Differnce of two histogram for given two devices
// Parameter - Pass the two histogram names along with the directory ex. "mps_histo/qwk_bcm6_hw"        
//***************************************************
//***************************************************

TH1F* GetDiffHisto(TString histoname1,TString histoname2){
  TH1F* histo1=NULL;
  TH1F* histo2=NULL;
  
  histo1= GetHisto(histoname1);
  histo2= GetHisto(histoname2);
  if (histo1!=NULL && histo2!=NULL){
    histo1->Add((TH1F*)histo2,-1);
  }

  return histo1;
};

//***************************************************
//***************************************************
//         Get 1D histogram for the helicity tree   
// Parameter - Pass the histogram name, cut and draw options
//***************************************************
//***************************************************

TH1F* Get1DHisto(const TString name, const TCut cut, Option_t* option){
  tree_chain.Draw(Form("(%s)>>temp",name.Data()), cut, option);
  TH1F* temp;
  temp = (TH1F*)  gDirectory -> Get("temp");
  if(!temp) {
    return 0;
  }
  return temp;  
};
//***************************************************
//***************************************************
//         Get 2D histogram for the helicity tree   
// Parameter - Pass the histogram name, cut and draw options
//***************************************************
//***************************************************
TH2F* Get2DHisto(const TString name, const TCut cut, Option_t* option){
  TH2F* temp;
  tree_chain.Draw(Form("(%s)>>temp",name.Data()), cut, option);
  temp = (TH2F*)  gDirectory -> Get("temp");
  if(!temp) {
    return 0;
  }
  return temp;  
};

//***************************************************
//***************************************************
//         Load the tree for given file name
// Parameter - Pass the file name and pointer to the TChain
//***************************************************
//***************************************************
Int_t  GetTree(TString filename, TChain* chain){
  TString file_dir;
  Int_t   chain_status = 0;

  file_dir = TString(getenv("QW_ROOTFILES"));
  if (!file_dir) 
    file_dir = "/home/cdaq/qweak/QwScratch/rootfiles";

  //std::cout<<Form("%s/%s", file_dir.Data(), filename.Data())<<std::endl;

 
  chain_status = chain->Add(Form("%s/%s", file_dir.Data(), filename.Data()));
  
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




//***************************************************
//***************************************************
//         Read the beam properties              
//***************************************************
//***************************************************
void FillBeamParameters(){

  util.clear();
  TString comp;
  //char tmp[100];
  util.push_back("\n");
  util.push_back(HorDoubleLine());
  //util.push_back("========================================================\n");
  util.push_back("BEAM PARAMETERS \n");
  util.push_back("--------------- \n \n");

  Get_Mean("hel_histo/yield_qwk_charge_hw",intensity,1);
  
  if(intensity<0){
    util.push_back(" Beam current appears to be zero \n Therefore the summary is not going to make much sense ");
    for (int i=0;i<10;i++)
      std::cout<<" NO beam detected ! \n";
  }
  
  util.push_back("\n Beam Properties at the Target \n");
  //util.push_back(" ----------------------------- \n");
  //util.push_back("\n quantity|    value   |      Asym/Diff       | Asym/Diff width");
  //util.push_back("\n ........|(uA,mm,mrad)|    (ppm,mm,rad)     | (ppm, mm, rad) ");

  util.push_back(HorSingleLine());
  util.push_back(TopRule("quantity", "value", "Asym/Diff", "Asym/Diff width"));
  util.push_back(TopRule("........", "(uA,mm,mrad)", "(ppm,mm,rad)", "(ppm,mm,rad)"));

  Double_t mean;
  Double_t val[4];
  //Compare beam charge
  Fit_with_a_gaussian("hel_histo/asym_qwk_charge_hw",val,1.e+6);//factor 1e+6 to convert to ppm
//   util.push_back(Form("\n charge  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("charge", intensity, val));
  compare_to_golden_value("charge_asymmetry", val[0], val[2]);
  compare_to_golden_value("charge_asymmetry_width", val[1], val[3]);
  
  //Beam on target positions
  Get_Mean("hel_histo/yield_qwk_targetX_hw",mean,1.);
  //  Fit_with_a_gaussian("hel_histo/diff_qwk_targetX_hw",val,1.e+6);//factor 1e+6 to convert to nm 
  //  util.push_back(Form("\n x targ  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetX_hw",val,1e6); // unit is mm or nm?  
  util.push_back(MidRule_2("target x", mean, val));
  compare_to_golden_value("x_beam_position", mean,0);
  compare_to_golden_value("x_position_difference", val[0], val[2]);
  compare_to_golden_value("x_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_targetY_hw",mean,1.);
//   Fit_with_a_gaussian("hel_histo/diff_qwk_targetY_hw",val,1.e+6);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetY_hw",val,1e6);
  //  util.push_back(Form("\n y targ  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("target y", mean, val));
  compare_to_golden_value("y_beam_position", mean,0);
  compare_to_golden_value("y_position_difference", val[0], val[2]);
  compare_to_golden_value("y_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_targetXSlope_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetXSlope_hw",val,1.000);
//   util.push_back(Form("\n Angle x    |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("angle x", mean, val));
  compare_to_golden_value("x_beam_angle", mean,0);
  compare_to_golden_value("x_angle_difference", val[0], val[2]);
  compare_to_golden_value("x_angle_difference_width", val[1], val[3]);


  Get_Mean("hel_histo/yield_qwk_targetYSlope_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetYSlope_hw",val,1.000);
  //  util.push_back(Form("\n Angle y    |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("angle y", mean, val));
  compare_to_golden_value("y_beam_angle", mean,0);
  compare_to_golden_value("y_angle_difference", val[0], val[2]);
  compare_to_golden_value("y_angle_difference_width", val[1], val[3]);
  util.push_back("\n Note: Angles are in radians while angle differences are still presented in gradient differences\n");

  
  Get_Mean("hel_histo/yield_qwk_energy_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_energy_hw",val,1.e+6);
 //  util.push_back(Form("\n Energy (dP/P)  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("Energy (dP/P)", mean, val));
  compare_to_golden_value("beam_energy", mean,0);
  compare_to_golden_value("beam_energy_asymmetry", val[0], val[2]);
  compare_to_golden_value("beam_energy_asymmetry_width", val[1], val[3]);
  
  //individual devices
  //bcm 1,2,5,6
  util.push_back("\n\n\n Beam Line Devices Summary \n");
  util.push_back(HorSingleLine());
  //  util.push_back("\n quantity|    value   |      Asym/Diff       | Asym/Diff width");
  //   util.push_back("\n ........|(uA,mm,mrad)|    ( util.push_back("\n\n");ppm,mm,rad)     | (ppm, mm, rad) ");
  //  util.push_back(Form("\n %10s | %12s | %20s | %20s", "quantity", "value", "Asym/Diff", "Asym/Diff width"));
  //  util.push_back(Form("\n %10s | %12s | %20s | %20s", "........", "(uA,mm,mrad)", "(ppm,mm,rad)", "(ppm,mm,rad)"));
  util.push_back(TopRule("quantity", "value", "Asym/Diff", "Asym/Diff width"));
  util.push_back(TopRule("........", "(uA,mm,mrad)", "(ppm,mm,rad)", "(ppm,mm,rad)"));


  Get_Mean("hel_histo/yield_qwk_bcm1_hw", bcm1_current_uA, 1);
  Fit_with_a_gaussian("hel_histo/asym_qwk_bcm1_hw",val,1.e+6);//factor 1e+6 to convert to ppm
  //util.push_back(Form("\n bcm1  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("bcm1", bcm1_current_uA, val));
  compare_to_golden_value("charge_asymmetry", val[0], val[2]);
  compare_to_golden_value("charge_asymmetry_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bcm2_hw",intensity,1);
  Fit_with_a_gaussian("hel_histo/asym_qwk_bcm2_hw",val,1.e+6);//factor 1e+6 to convert to ppm
  //  util.push_back(Form("\n bcm2  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("bcm2", intensity, val));
  compare_to_golden_value("charge_asymmetry", val[0], val[2]);
  compare_to_golden_value("charge_asymmetry_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bcm5_hw",intensity,1);
  Fit_with_a_gaussian("hel_histo/asym_qwk_bcm5_hw",val,1.e+6);//factor 1e+6 to convert to ppm
  //  util.push_back(Form("\n bcm5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("bcm5", intensity, val));
  compare_to_golden_value("charge_asymmetry", val[0], val[2]);
  compare_to_golden_value("charge_asymmetry_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bcm6_hw",intensity,1);
  Fit_with_a_gaussian("hel_histo/asym_qwk_bcm6_hw",val,1.e+6);//factor 1e+6 to convert to ppm
  //  util.push_back(Form("\n bcm6  |   %5.2f   util.push_back("\n\n");  | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("bcm6", intensity, val));
  compare_to_golden_value("charge_asymmetry", val[0], val[2]);
  compare_to_golden_value("charge_asymmetry_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bpm3c12X_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_bpm3c12X_hw",val,1);
  //  util.push_back(Form("\n 3c12  x  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("3c12 x", mean, val));
  compare_to_golden_value("x_beam_position", mean,0);
  compare_to_golden_value("x_position_difference", val[0], val[2]);
  compare_to_golden_value("x_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bpm3c12Y_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_bpm3c12Y_hw",val,1);
  //  util.push_back(Form("\n 3c12  y  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("3c12 y", mean, val));
  compare_to_golden_value("y_beam_position", mean,0);
  compare_to_golden_value("y_position_difference", val[0], val[2]);
  compare_to_golden_value("y_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bpm3h04X_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_bpm3h04X_hw",val,1);
  //  util.push_back(Form("\n 3h04 x  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("3c04 x", mean, val));
  compare_to_golden_value("x_beam_position", mean,0);
  compare_to_golden_value("x_position_difference", val[0], val[2]);
  compare_to_golden_value("x_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bpm3h04Y_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_bpm3h04Y_hw",val,1);
  //  util.push_back(Form("\n  3h04 y  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("3c04 y", mean, val));
  compare_to_golden_value("y_beam_position", mean,0);
  compare_to_golden_value("y_position_difference", val[0], val[2]);
  compare_to_golden_value("y_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bpm3h09X_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_bpm3h09X_hw",val,1);
  //  util.push_back(Form("\n 3h09 x  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("3c09 x", mean, val));
  compare_to_golden_value("x_beam_position", mean,0);
  compare_to_golden_value("x_position_difference", val[0], val[2]);
  compare_to_golden_value("x_position_difference_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_bpm3h09Y_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_bpm3h09Y_hw",val,1);
  //  util.push_back(Form("\n 3h09 y  |   %3.6f    | %3.6f +/- %3.6f  | %3.6f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_6("3c09 y", mean, val));
  compare_to_golden_value("y_beam_position", mean,0);
  compare_to_golden_value("y_position_difference", val[0], val[2]);
  compare_to_golden_value("y_position_difference_width", val[1], val[3]);

   if (expertmode){

     util.push_back("\n\n\n Beam Line Devices Double Differences Summary \n");
     util.push_back(HorSingleLine());
     util.push_back(TopRule("quantity", "value", "Asym/Diff", "Asym/Diff width"));
     util.push_back(TopRule("........", "(uA,mm,mrad)", "(ppm,mm,rad)", "(ppm,mm,rad)"));
     //     util.push_back("\n quantity|    value   |      Asym/Diff       | Asym/Diff width");
     //     util.push_back("\n ........|(uA,mm,mrad)|    (ppm,mm,rad)     | (ppm, mm, rad) ");

     Get_Tree_Mean("(yield_qwk_bcm1.hw_sum-yield_qwk_bcm2.hw_sum)",intensity,1);
     Get_Tree_Mean_Fit_gaus("(asym_qwk_bcm1.hw_sum-asym_qwk_bcm2.hw_sum)",val,1.e+6);
     util.push_back(MidRule_2("bcm1-bcm2", intensity, val));
     //     util.push_back(Form("\n bcm1-bcm2  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
     compare_to_golden_value("double_diff_charge_asymmetry", val[0], val[2]);
     compare_to_golden_value("double_diff_charge_asymmetry_width", val[1], val[3]);

     Get_Tree_Mean("(yield_qwk_bcm1.hw_sum-yield_qwk_bcm5.hw_sum)",intensity,1);
     Get_Tree_Mean_Fit_gaus("(asym_qwk_bcm1.hw_sum-asym_qwk_bcm5.hw_sum)",val,1.e+6);
     util.push_back(MidRule_2("bcm1-bcm5", intensity, val));
     //     util.push_back(Form("\n bcm1-bcm5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
     compare_to_golden_value("double_diff_charge_asymmetry", val[0], val[2]);
     compare_to_golden_value("double_diff_charge_asymmetry_width", val[1], val[3]);

     Get_Tree_Mean("(yield_qwk_bcm1.hw_sum-yield_qwk_bcm6.hw_sum)",intensity,1);
     Get_Tree_Mean_Fit_gaus("(asym_qwk_bcm1.hw_sum-asym_qwk_bcm6.hw_sum)",val,1.e+6);
     util.push_back(MidRule_2("bcm1-bcm6", intensity, val));
     //     util.push_back(Form("\n bcm1-bcm5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
     compare_to_golden_value("double_diff_charge_asymmetry", val[0], val[2]);
     compare_to_golden_value("double_diff_charge_asymmetry_width", val[1], val[3]);


     Get_Tree_Mean("(yield_qwk_bcm2.hw_sum-yield_qwk_bcm5.hw_sum)",intensity,1);
     Get_Tree_Mean_Fit_gaus("(asym_qwk_bcm2.hw_sum-asym_qwk_bcm5.hw_sum)",val,1.e+6);
     util.push_back(MidRule_2("bcm2-bcm5", intensity, val));
     //     util.push_back(Form("\n bcm2-bcm5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
     compare_to_golden_value("double_diff_charge_asymmetry", val[0], val[2]);
     compare_to_golden_value("double_diff_charge_asymmetry_width", val[1], val[3]);

     Get_Tree_Mean("(yield_qwk_bcm2.hw_sum-yield_qwk_bcm6.hw_sum)",intensity,1);
     Get_Tree_Mean_Fit_gaus("(asym_qwk_bcm2.hw_sum-asym_qwk_bcm6.hw_sum)",val,1.e+6);
     util.push_back(MidRule_2("bcm2-bcm6", intensity, val));
     //     util.push_back(Form("\n bcm2-bcm5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
     compare_to_golden_value("double_diff_charge_asymmetry", val[0], val[2]);
     compare_to_golden_value("double_diff_charge_asymmetry_width", val[1], val[3]);


     Get_Tree_Mean("(yield_qwk_bcm5.hw_sum-yield_qwk_bcm6.hw_sum)",intensity,1);
     Get_Tree_Mean_Fit_gaus("(asym_qwk_bcm5.hw_sum-asym_qwk_bcm6.hw_sum)",val,1.e+6);
     util.push_back(MidRule_2("bcm5-bcm6", intensity, val));
     //     util.push_back(Form("\n bcm5-bcm6  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
     compare_to_golden_value("double_diff_charge_asymmetry", val[0], val[2]);
     compare_to_golden_value("double_diff_charge_asymmetry_width", val[1], val[3]);


     
     //    Get_Tree_Mean("(yield_qwk_bpm3h09X.hw_sum-yield_qwk_bpm3h04X.hw_sum)",intensity,1);
     //    Get_Tree_Mean_Fit_gaus("(diff_qwk_bpm3h09X.hw_sum-diff_qwk_bpm3h04X.hw_sum)",val,1.e+6);
     //     util.push_back(Form("\n (3h09-3h04) x  |   %3.6f    | %3.6f +/- %3.6f  | %3.2f +/- %3.6f ",mean,val[0],val[2],val[1],val[3]));

     Get_Tree_Mean("(yield_qwk_bpm3h09X.hw_sum-yield_qwk_bpm3h04X.hw_sum)",mean,1);
     Get_Tree_Mean_Fit_gaus("(diff_qwk_bpm3h09X.hw_sum-diff_qwk_bpm3h04X.hw_sum)",val,1e6);
     util.push_back(MidRule_3("(3h09-3h04) x", mean, val));
     compare_to_golden_value("double_x_diff_position", mean,0);
     compare_to_golden_value("double_x_difference", val[0], val[2]);
     compare_to_golden_value("double_x_difference_width", val[1], val[3]);

     //     Get_Tree_Mean("(yield_qwk_bpm3h09Y.hw_sum-yield_qwk_bpm3h04Y.hw_sum)",intensity,1);
     //      Get_Tree_Mean_Fit_gaus("(diff_qwk_bpm3h09Y.hw_sum-diff_qwk_bpm3h04Y.hw_sum)",val,1.e+6);
     //     util.push_back(Form("\n (3h09-3h04) y  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
     
     Get_Tree_Mean("(yield_qwk_bpm3h09Y.hw_sum-yield_qwk_bpm3h04Y.hw_sum)",mean,1);
     Get_Tree_Mean_Fit_gaus("(diff_qwk_bpm3h09Y.hw_sum-diff_qwk_bpm3h04Y.hw_sum)",val,1e6);
     util.push_back(MidRule_3("(3h09-3h04) y", mean, val));
    
     compare_to_golden_value("double_y_diff_position", mean,0);
     compare_to_golden_value("double_y_difference", val[0], val[2]);
     compare_to_golden_value("double_y_difference_width", val[1], val[3]);

   }//end of expert mode


  // halo rates
   util.push_back("\n\n\n Halo Rate Summary (normalized by qwk_bcm1)\n");
   util.push_back(HorSingleLine());
   Get_Mean("hel_histo/yield_sca_halo3",mean,1.);
   // sprintf(tmp,"\n  Halo 3 rate (KHz) = %7.1f (%7.1f (kHz/uA))\n", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //   util.push_back(TString(tmp)+" \n");
   util.push_back(Halo("Halo 3", mean, bcm1_current_uA));
   Get_Mean("hel_histo/yield_sca_halo4",mean,1.);
   util.push_back(Halo("Halo 4", mean, bcm1_current_uA));
   //sprintf(tmp,"\n  Halo 4 rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //   util.push_back(TString(tmp)+" \n");
   Get_Mean("hel_histo/yield_sca_halo5",mean,1.);
   //sprintf(tmp,"\n  Halo 5 rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Halo("Halo 5", mean, bcm1_current_uA));
   Get_Mean("hel_histo/yield_sca_halo6",mean,1.);
   //sprintf(tmp,"\n  Halo 6 rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Halo("Halo 6", mean, bcm1_current_uA));
   Get_Mean("hel_histo/yield_sca_halo7",mean,1.);
   //   sprintf(tmp,"\n  Halo 7 rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Halo("Halo 7", mean, bcm1_current_uA));
   Get_Mean("hel_histo/yield_sca_halo8",mean,1.);
   // sprintf(tmp,"\n  Halo 8 rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //   util.push_back(TString(tmp)+" \n");
   util.push_back(Halo("Halo 8", mean, bcm1_current_uA));
   Get_Mean("hel_histo/yield_sca_halo9",mean,1.);
   //   sprintf(tmp,"\n  Halo 9 rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Halo("Halo 9", mean, bcm1_current_uA));
   Get_Mean("hel_histo/yield_sca_31mhz",mean,1.);
   //sprintf(tmp,"\n  31MHz Clock rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Clock("31MHz Clock", mean));
   Get_Mean("hel_histo/yield_sca_31mhzreturn",mean,1.);
   //sprintf(tmp,"\n  31MHz Clock return rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Clock("31MHz Clock return", mean));
   Get_Mean("hel_histo/yield_sca_4mhz",mean,1.);
   //   sprintf(tmp,"\n  4MHz Clock rate (KHz) = %7.1f (%7.1f (kHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
   //util.push_back(TString(tmp)+" \n");
   util.push_back(Clock("4MHz Clock", mean));
   util.push_back("\n\n");
   for(size_t i=0;i<util.size();i++)
     results.push_back(util[i]);


   return;
   
};

//***************************************************
//***************************************************
//         Read the MD properties              
//***************************************************
//***************************************************
void FillMDParameters(){
   util.clear();
  TString histname;
  util.push_back("\n");
 //  util.push_back("========================================================\n");
  util.push_back(HorDoubleLine());
  util.push_back("MAIN DETECTOR PARAMETERS \n");
  util.push_back("------------------------ \n \n");
  //   util.push_back("\n quantity|    value   |      Asym            | Asym width");
  //   util.push_back("\n ........|(V/uA)      |    (ppm)             | (ppm) ");
  util.push_back(TopRule("quantity", "value", "asym", "asym width"));
  util.push_back(TopRule("........", "(V/uA)", "(ppm)", "(ppm)"));

  Double_t mean;
  Double_t val[4];

  for (int count=1;count<9;count++){
    histname=Form("hel_histo/yield_qwk_md%ibarsum_hw",count);
    //std::cout<<histname<<"\n";
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_qwk_md%ibarsum_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to nm
  //   util.push_back(Form("\n MD%i  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3(Form("MD%d", count), mean, val));
    histname=Form("MD_pmt%i_pos",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("MD_pmt%i_pos_asymmetry",count);
    compare_to_golden_value(histname, val[0], val[2]);
    histname=Form("MD_pmt%i_pos_width",count);
    compare_to_golden_value(histname, val[1], val[3]);
  }

  Get_Mean("hel_histo/yield_qwk_md1_qwk_md5_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_md1_qwk_md5_hw",val,1.e+6);//factor 1e+6 to convert to nm
 //  util.push_back(Form("\n md1-5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_3("MD1-MD5", mean, val));
  compare_to_golden_value("MD_1-5", mean,0);
  compare_to_golden_value("MD_1-5_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_1-5_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_md2_qwk_md6_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_md2_qwk_md6_hw",val,1.e+6);//factor 1e+6 to convert to nm
  //  util.push_back(Form("\n md2-6  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_3("MD2-MD6", mean, val));
  compare_to_golden_value("MD_2-6", mean,0);
  compare_to_golden_value("MD_2-6_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_2-6_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_md3_qwk_md7_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_md3_qwk_md7_hw",val,1.e+6);//factor 1e+6 to convert to nm
  //  util.push_back(Form("\n md1-5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_3("MD3-MD7", mean, val));
  compare_to_golden_value("MD_3-7", mean,0);
  compare_to_golden_value("MD_3-7_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_3-7_width", val[1], val[3]);

  Get_Mean("hel_histo/yield_qwk_md4_qwk_md8_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_md4_qwk_md8_hw",val,1.e+6);//factor 1e+6 to convert to nm
  //  util.push_back(Form("\n md1-5  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_3("MD4-MD8", mean, val));
  compare_to_golden_value("MD_4-8", mean,0);
  compare_to_golden_value("MD_4-8_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_4-8_width", val[1], val[3]);
    
  Get_Mean("hel_histo/yield_qwk_mdallbars_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_mdallbars_hw",val,1.e+6);//factor 1e+6 to convert to nm
 //  util.push_back(Form("\n mdallbars  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_3("MD_AllBars", mean, val));
  compare_to_golden_value("MD_all", mean,0);
  compare_to_golden_value("MD_all_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_all_width", val[1], val[3]);
 
  Get_Mean("hel_histo/yield_qwk_mdevenbars_hw",mean,1.);
  //  Fit_with_a_gaussian("hel_histo/asym_qwk_mdevenbars_hw",val,1.e+6);//factor 1e+6 to convert to nm
  //  util.push_back(Form("\n mdevenbars  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  util.push_back(MidRule_2("MD_EvenBars", mean, val));
  compare_to_golden_value("MD_even", mean,0);
  compare_to_golden_value("MD_even_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_even_width", val[1], val[3]);
 
  Get_Mean("hel_histo/yield_qwk_mdoddbars_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_mdoddbars_hw",val,1.e+6);//factor 1e+6 to convert to nm
  util.push_back(MidRule_3("MD_OddBars", mean, val));
  //  util.push_back(Form("\n mdoddbars  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  compare_to_golden_value("MD_odd", mean,0);
  compare_to_golden_value("MD_odd_asymmetry", val[0], val[2]);
  compare_to_golden_value("MD_odd_width", val[1], val[3]);

  if (expertmode){
 
    util.push_back("\n\n\n Main Detector Double Differences Summary \n");
 //    util.push_back("------------------------------------------\n");
//     util.push_back("\n quantity|    value   |      Asym/Diff       | Asym/Diff width");
//     util.push_back("\n ........|(V/uA)      |       (ppm)          |    (ppm) ");

    util.push_back(HorSingleLine());
    util.push_back(TopRule("quantity", "value", "asym", "asym width"));
    util.push_back(TopRule("........", "(V/uA)", "(ppm)", "(ppm)"));
    
    Get_Tree_Mean("(yield_qwk_mdevenbars.hw_sum-yield_qwk_mdoddbars.hw_sum)",intensity,1);
    Get_Tree_Mean_Fit_gaus("(asym_qwk_mdevenbars.hw_sum-asym_qwk_mdoddbars.hw_sum)",val,1.e+6);
    //    util.push_back(Form("\n even-odd   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3("Even-Odd", intensity, val));
    compare_to_golden_value("MD_all_diff", intensity,0);
    compare_to_golden_value("MD_all_diff_asymmetry", val[0], val[2]);
    compare_to_golden_value("MD_all_diff_width", val[1], val[3]);

    Get_Tree_Mean("(yield_qwk_md1_qwk_md5.hw_sum-yield_qwk_md3_qwk_md7.hw_sum)",intensity,1);
    Get_Tree_Mean_Fit_gaus("(asym_qwk_md1_qwk_md5.hw_sum-asym_qwk_md3_qwk_md7.hw_sum)",val,1.e+6);
    //util.push_back(Form("\n md 1/5-3/7   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3("MD1/5-MD3/7", intensity, val));
    compare_to_golden_value("MD_2bar_diff", intensity,0);
    compare_to_golden_value("MD_2bar_diff_asymmetry", val[0], val[2]);
    compare_to_golden_value("MD_2bar_diff_width", val[1], val[3]);
    
    Get_Tree_Mean("(yield_qwk_md2_qwk_md6.hw_sum-yield_qwk_md4_qwk_md8.hw_sum)",intensity,1);
    Get_Tree_Mean_Fit_gaus("(asym_qwk_md2_qwk_md6.hw_sum-asym_qwk_md4_qwk_md8.hw_sum)",val,1.e+6);
//    util.push_back(Form("\n md 2/6-4/8   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3("MD2/6-MD4/8", intensity, val));
    compare_to_golden_value("MD_2bar_diff", intensity,0);
    compare_to_golden_value("MD_2bar_diff_asymmetry", val[0], val[2]);
    compare_to_golden_value("MD_2bar_diff_width", val[1], val[3]);
  }
  
  util.push_back("\n\n");
  for(size_t i=0;i<util.size();i++)
    results.push_back(util[i]);

};

//***************************************************
//         Read the LUMI properties              
//***************************************************
//***************************************************
void FillLUMIParameters(){
   util.clear();
  TString histname;

  util.push_back("\n");
  //  util.push_back("========================================================\n");
  util.push_back(HorDoubleLine());
  util.push_back("LUMINOSITY MONITORS \n");
  util.push_back("------------------------------- \n \n");

  //  util.push_back("\n quantity|    value   |      Asym            | Asym width");
  //  util.push_back("\n ........|(V/uA)      |     (ppm)            | (ppm) ");


  util.push_back("\n Down Stream Luminosity Monitor Summary \n");
  util.push_back(HorSingleLine());
  util.push_back(TopRule("quantity", "value", "asym", "asym width"));
  util.push_back(TopRule("........", "(V/uA)", "(ppm)", "(ppm)"));

  Double_t mean;
  Double_t val[4];
  for (int count=1;count<9;count++){
    histname=Form("hel_histo/yield_qwk_dslumi%i_hw",count);
    //std::cout<<histname<<"\n";
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_qwk_dslumi%i_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to ppm
    //    util.push_back(Form("\n DSLUMI%i  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3(Form("dslumi%d", count), mean, val));
    histname=Form("dslumi_det_%i_yield",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("dslumi_det_%i_asymmetry",count);
    compare_to_golden_value(histname, val[0], val[2]);
    histname=Form("dslumi_det_%i_asymmetry_width",count);
    compare_to_golden_value(histname, val[1], val[3]);
  }

  util.push_back("\n\n");
  // util.push_back("========================================================\n");
  //  util.push_back("UP STREAM LUMINOSITY MONITORS \n");
  //  util.push_back("----------------------------- \n \n");
  util.push_back("\n Up Stream Luminosity Monitor Summary \n");
  util.push_back(HorSingleLine());
  util.push_back(TopRule("quantity", "value", "asym", "asym width"));
  util.push_back(TopRule("........", "(V/uA)", "(ppm)", "(ppm)"));

  //  util.push_back("\n quantity|    value   |      Asym/           | Asym width");
  //  util.push_back("\n ........|(V/uA)      |    (ppm)             | (ppm) ");
 
  for (int count=1;count<9;count+=2){

    histname=Form("hel_histo/yield_qwk_uslumi%ineg_hw",count);
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_qwk_uslumi%ineg_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to ppm
    //    util.push_back(Form("\n uslumi%ineg   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3(Form("uslumi%dneg", count), mean, val));
    histname=Form("uslumi_det_%i_yield",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("uslumi_det_%i_asymmetry",count);
    compare_to_golden_value(histname, val[0],val[2]);
    histname=Form("uslumi_det_%i_asymmetry_width",count);
    compare_to_golden_value(histname, val[1],val[3]);

    histname=Form("hel_histo/yield_qwk_uslumi%ipos_hw",count);
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_qwk_uslumi%ipos_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to nm
    //    util.push_back(Form("\n uslumi%ipos   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    util.push_back(MidRule_3(Form("uslumi%dpos", count), mean, val));
    histname=Form("uslumi_det_%i_yield",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("uslumi_det_%i_asymmetry",count);
    compare_to_golden_value(histname, val[0],val[2]);
    histname=Form("uslumi_det_%i_asymmetry_width",count);
    compare_to_golden_value(histname,val[1],val[3] );    
  }
  util.push_back("\n\n");

   for(size_t i=0;i<util.size();i++)
    results.push_back(util[i]);

};



//***************************************************
//***************************************************
//         Read the reference file to obtain values to compare                     
//***************************************************
//***************************************************
Int_t Read_ref_file(TString name_of_ref_file)
{
  TString line;
  TString tmp;
  REFDATA tmpref;
  Bool_t ver=kFALSE;
  //Bool_t ver=kTRUE;

  ifstream inputref(name_of_ref_file);
  if(!inputref)
    {
      std::cerr << "ERROR: file with reference values :"<<name_of_ref_file<<" not found trying to read the default file\n";
      return(1);
    }
  else
    if(ver) std::cout<<" file with reference values : "<<name_of_ref_file<<" is open \n";

  while(inputref)
    {
      line.ReadToken(inputref);
      if(ver) std::cout<<line<<std::endl;
      if(line(0,1)=="=")  //this is a line of commentary
	{
	  if(ver)  std::cout<<"this is a commentary line"<<std::endl;
	line.ReadLine(inputref);
	}
      else
	{
	  tmpref.name=line;
	  line.ReadToken(inputref);
	  line.ReadToken(inputref);
	  tmpref.unit=line;
	  line.ReadToken(inputref);
	  line.ReadToken(inputref);
	  tmpref.value=atof(line.Data());
	  line.ReadToken(inputref);
	  line.ReadToken(inputref);
	  // now the tolerance has been read
	  //assume that if % then it is in the last position
	  if(line.Contains("%"))
	  {
	    line.Remove(line.Length()-1);
	    tmpref.tolerance=tmpref.value*0.01*atof(line.Data());
	  }
	  else
	    tmpref.tolerance=atof(line.Data());
	
	  ref.push_back(tmpref);
	}
    }
  
  inputref.close();

  if(ver)
    for(size_t i=0;i<ref.size();i++)
      std::cout<<i<<" "<<ref[i].name<<"  :  "<<ref[i].value<<" : "<<ref[i].tolerance<<"\n";

  return(0);
}



//***************************************************
//***************************************************
//         Obtain the mean+/-error and width+/- error from a gaussian fit to a histogram              
//***************************************************
//***************************************************
void Fit_with_a_gaussian(TString histname, Double_t *container, Double_t factor)
{
  TH1F* h = NULL;
  h = GetHisto(histname);
  if (h != NULL) {
    if (NoFits){
      *container=h->GetMean()*factor;
      *(container+1)=h->GetRMS()*factor;
      *(container+2)=h->GetMeanError()*factor;
      *(container+3)=h->GetRMSError()*factor;
    }else{
      h->Fit("gaus","Q");
      *container=h->GetFunction("gaus")->GetParameter(1)*factor; // mean
      *(container+1)=h->GetFunction("gaus")->GetParameter(2)*factor; //width
      *(container+2)=h->GetFunction("gaus")->GetParError(1)*factor;
      *(container+3)=h->GetFunction("gaus")->GetParError(2)*factor;
    delete h;
    }
  } else {
    std::cout<<"Fit_with_a_gaussian Error: "<<histname<<" Not found!"<<std::endl;
    *container     = 0.0;
    *(container+1) = 0.0;
    *(container+2) = 0.0;
    *(container+3) = 0.0;
  }

  return;
};

//***************************************************
//***************************************************
//         Obtain the mean+/-error and width+/- error from a gaussian fit to a histogram from the tree              
//***************************************************
//***************************************************
void Get_Tree_Mean_Fit_gaus(TString device_expression, Double_t *container, Double_t factor){
  TH1F* h = NULL;
  h=Get1DHisto(device_expression,"ErrorFlag==0","goff");
    if (h != NULL) {
    if (NoFits){
      *container=h->GetMean()*factor;
      *(container+1)=h->GetRMS()*factor;
      *(container+2)=h->GetMeanError()*factor;
      *(container+3)=h->GetRMSError()*factor;
    }else{
      h->Fit("gaus","Q");
      *container=h->GetFunction("gaus")->GetParameter(1)*factor; // mean
      *(container+1)=h->GetFunction("gaus")->GetParameter(2)*factor; //width
      *(container+2)=h->GetFunction("gaus")->GetParError(1)*factor;
      *(container+3)=h->GetFunction("gaus")->GetParError(2)*factor;
    delete h;
    }
  } else {
    std::cout<<"Fit_gaus Error: "<<device_expression<<" Not found!"<<std::endl;
    *container     = 0.0;
    *(container+1) = 0.0;
    *(container+2) = 0.0;
    *(container+3) = 0.0;
  }

  return;

};




void Fit_with_a_gaussian_diff(TString histname1,TString histname2, Double_t *container, Double_t factor){
  TH1F* h = NULL;
  h = GetDiffHisto(histname1,histname2);
  if (h != NULL) {
    if (NoFits){
      *container=h->GetMean()*factor;
      *(container+1)=h->GetRMS()*factor;
      *(container+2)=h->GetMeanError()*factor;
      *(container+3)=h->GetRMSError()*factor;
    }else{
      h->Fit("gaus","Q");
      *container=h->GetFunction("gaus")->GetParameter(1)*factor; // mean
      *(container+1)=h->GetFunction("gaus")->GetParameter(2)*factor; //width
      *(container+2)=h->GetFunction("gaus")->GetParError(1)*factor;
      *(container+3)=h->GetFunction("gaus")->GetParError(2)*factor;
    delete h;
    }
  } else {
    std::cout<<"Fit_with_a_gaussian Error: "<<histname1<<" - "<<histname2<<" Not found!"<<std::endl;
    *container     = 0.0;
    *(container+1) = 0.0;
    *(container+2) = 0.0;
    *(container+3) = 0.0;
  }

  return;  
};

//***************************************************
//***************************************************
//         Obtain the mean from a histogram              
//***************************************************
//***************************************************
void Get_Mean(TString histname, Double_t &container, Double_t factor)
{
  TH1F* h = NULL;
  h = GetHisto(histname); 
  if (h != NULL) {
//     cout<<histname<<" mean value is "<<h->GetMean()<<endl;
//     cout<<histname<<" mean value is "<<h->GetEntries()<<endl;
//     cout<<"the factor is "<<factor<<endl;
    container=h->GetMean()*factor;
    delete h;
  } else {
    container = 0.0;
  }
  return;
}
//***************************************************
//***************************************************
//         Obtain the mean from the tree              
//***************************************************
//***************************************************
void Get_Tree_Mean(TString device_expression, Double_t &container, Double_t factor){
  TH1F* h = NULL;
  h=Get1DHisto(device_expression,"ErrorFlag==0","goff");
  if (h != NULL) {
//     cout<<histname<<" mean value is "<<h->GetMean()<<endl;
//     cout<<histname<<" mean value is "<<h->GetEntries()<<endl;
//     cout<<"the factor is "<<factor<<endl;
    container=h->GetMean()*factor;
    delete h;
  } else {
    container = 0.0;
  }
  return;
};
void Get_Mean_Diff(TString histname1,TString histname2, Double_t &container, Double_t factor){
  TH1F* h = NULL;
  h = GetDiffHisto(histname1,histname2); 
  if (h != NULL) {
//     cout<<histname<<" mean value is "<<h->GetMean()<<endl;
//     cout<<histname<<" mean value is "<<h->GetEntries()<<endl;
//     cout<<"the factor is "<<factor<<endl;
    container=h->GetMean()*factor;
    delete h;
  } else {
    container = 0.0;
  }
  return;  
};

//***************************************************
//***************************************************
//         Compare with reference values            
//***************************************************
//***************************************************
void compare_to_golden_value(TString valuetocompareto, Double_t value, Double_t precision){
  // you compare your value with a precision to a reference value with a tolerance
  // if the two of them are separated by more than two sigma than flag this result
  // as out of tolerance
  Double_t fac_dev=2.;//two sigma diviation

  size_t i=0;
  Double_t high;
  Double_t low;
  verbose=kFALSE;
  Bool_t notfound=kTRUE;
  good_or_bad=0;
  
  while(i<ref.size()&&notfound)
    {
      if(ref[i].name==valuetocompareto)
	{
	  notfound=kFALSE;
	  high=ref[i].value+ref[i].tolerance*fac_dev;
	  low =ref[i].value-ref[i].tolerance*fac_dev;
	  if(verbose)
	    {
	      std::cout<<" --------"<<std::endl;
	      std::cout<<"value to compare to ="<<valuetocompareto<<std::endl;
	      std::cout<<"this references values are number "<< i<< " in the reference file \n";
	      std::cout<<"value ="<<value<<" +/- "<<precision<<std::endl;
	      std::cout<<"golden value ="<<ref[i].value<<" +/- "<<ref[i].tolerance<<std::endl;
	      std::cout<<"low ="<<low<<std::endl;
	      std::cout<<"high ="<<high<<std::endl;
	    }
	  if(value+precision<low ||  value-precision>high)
	    {
	      if(verbose)
		{
		  std::cout<<" this is not ok !!!!\n";
		  std::cout<<valuetocompareto<<" with value ="<<value<<" +- "<<precision<<" is out of range \n";
		  std::cout<<"acceptable value is between "<<low<<" and "<<high<<std::endl;
		}
	      if(!util[util.size()-1].Contains("***")&&comparisonon){
		util[util.size()-1].Append(" *** values out of acceptable range:");
		util[util.size()-1].Append(Form("%5.2f; ",value));
	      }
	    }
	  notfound=kFALSE;
	}
      i+=1;
    }
  
  if(notfound==kTRUE)
    {
      std::cout<<" no golden value to compare to "<<std::endl;
      std::cout<<"there is a problem in the routine compare_to_golden_value for \n";
      std::cout<<"the quantity :"<<value<<" and the name "<<valuetocompareto<<"\n";
      std::cout<<" ** code problem **" <<std::endl;
    }  
};


