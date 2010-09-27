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



#include <vector>
#include "TRandom.h"

#include <math.h>
#include <TH2F.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TDatime.h>
#include <stdexcept>
#include <time.h>
#include <cstdio>
#include <TMath.h>

#include <cstdlib>
TTree *nt;

TString CanvasTitle;
TString get;

TTree *t_mps;
TTree *t_hel;



Bool_t comparisonon=kTRUE;  //this should be truned on in order to compare to golden values from the reference file

Int_t Read_ref_file(TString name_of_ref_file);
void compare_to_golden_value(TString valuetocompareto, Double_t valuet, Double_t precision=0);

void FillBeamParameters();
void FillMDParameters();
void FillLUMIParameters();
//Fit the histogram with a gaussian to obtain the mean and width
void Fit_with_a_gaussian(TString histname, Double_t *container, Double_t factor=1);
void Get_Mean(TString histname, Double_t &container, Double_t factor);
//To obtain a histo from set of runlets using TH::Add
TH1F* GetHisto(TString histoname);
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
Int_t good_or_bad;
Int_t total_bad; 
Bool_t verbose;
//vectors to store TDirecotry from each runlet
std::vector<TFile*> file_list;//List of TFile  to access histograms


int main(Int_t argc,Char_t* argv[])
{
  //The default root file stem is QwRun_
  TDatime tday;
  TString runnum;
  TString s_argc;
  TString rootfile_stem="QwRun_";
  TFile *f;
  TChain *chain_Mps_Tree=NULL;
  TChain *chain_Hel_Tree=NULL;
  TTree* mps_tree;
  TTree* hel_tree;
  Int_t outputformat=0;
  Int_t noofrunlets;

  if(argc<2){
    std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
    std::cerr<<" ./promptsummary  runnumber i rootfile-stem=QwRun_\n";
    std::cerr<<" Give a run number \n";
    std::cerr<<" Give i=0 print result on the screen (Default setting) and  \n";
    std::cerr<<" Give i=1 print result into a text file.\n";
    std::cerr<<" Give root file stem name if it is different from default value 'QwRun_' \n";
    exit(1);
  }
  else {
    runnum=argv[1];
    if (!runnum.IsDigit()){
      std::cerr<<" ********You NEED to give a valid run number************ \n";
      std::cerr<<" ./promptsummary  runnumber i rootfile-stem=QwRun_\n";
      std::cerr<<" Give a run number \n";
      std::cerr<<" Give i=0 print result on the screen (Default setting) and  \n";
      std::cerr<<" Give i=1 print result into a text file.\n";
      std::cerr<<" Give root file stem name if it is different from default value 'QwRun_' \n";
      exit(1);
    }
    if (argc > 2){//read the print option
      s_argc=argv[2];
      if (!s_argc.IsDigit()){
	std::cerr<<" ***** You NEED to give a valid print option 1 or 0 ************\n";
	std::cerr<<" ./promptsummary  runnumber i=0 rootfile-stem=QwRun_\n";
	std::cerr<<" Give a run number \n";
	std::cerr<<" Give i=0 print result on the screen (Default setting) and  \n";
	std::cerr<<" Give i=1 print result into a text file.\n";
	std::cerr<<" Give root file stem name if it is different from default value 'QwRun_' \n";
	exit(1);
      }
      outputformat=atoi(argv[2]);
      if (argc >= 4){
	s_argc=argv[3];//reads the root file stem
	rootfile_stem=s_argc;
      }
      
    }
  }

  std::cout<<"Run # "<<runnum<<" Print option is "<<outputformat<<" Root file stem  "<<rootfile_stem<<std::endl;

  //Looking for the given root file
  TString rootfilename;
  Int_t foundfile = 0;
  TString runlet="";
  rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+".root";
  filenames.clear(); 
  if((f = new TFile(rootfilename.Data(),"READ"))!=NULL &&!(f->IsZombie()) &&(f->IsOpen())){
    foundfile = 1;//found regular root file (No runlets)
    filenames.push_back(rootfilename);
  }
  
  if (foundfile==0){//looking for runlet based root files
    rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+".000.root";
    if((f = new TFile(rootfilename.Data(),"READ"))!=NULL &&!(f->IsZombie()) &&(f->IsOpen())){
      noofrunlets=0;
      runlet.Form("%03d",noofrunlets);
      rootfilename=TString(getenv("QW_ROOTFILES")) +  "/"+rootfile_stem+runnum+"."+runlet+".root";
      //      std::cout<<rootfilename<<std::endl;

      chain_Mps_Tree = new  TChain("Mps_Tree");
      chain_Hel_Tree = new  TChain("Hel_Tree");
      while ((f = new TFile(rootfilename.Data(),"READ"))!=NULL &&!(f->IsZombie()) &&(f->IsOpen())){
	std::cout<<rootfilename<<std::endl;
	file_list.push_back(f);
	filenames.push_back(rootfilename);
	f=NULL;//reset the root file
	chain_Mps_Tree->Add(rootfilename);
	chain_Hel_Tree->Add(rootfilename);
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
    mps_tree=(TTree*)f->Get("Mps_Tree");
    hel_tree=(TTree*)f->Get("Hel_Tree");
    file_list.push_back(f);//add the regular file to the file-list vector
  }else if (foundfile == 2){//In runlet mode. We need to create the TChain
    std::cout<<" found  "<<noofrunlets<<" runlets "<<std::endl;
  }

  std::cout<<" Mps_Tree "<<chain_Mps_Tree->GetNbranches()<<" Hel_Tree "<<chain_Hel_Tree->GetNbranches()<<std::endl;
  //std::cout<<" Mps_Tree "<<mps_tree->GetNbranches()<<std::endl;
  std::cout<<" No. of runlets : "<<file_list.size()<<std::endl;

  //Open the reference value file
  //TString name_of_ref_file=TString(getenv("QWSCRATCH")) +"/calib/golden_values_run_summary";
  TString name_of_ref_file="./golden_values_run_summary";
  results.push_back(" \n \nRoot file  on which this analysis based =\n \t");
  for (UInt_t nf=0;nf<filenames.size();nf++)
    results.push_back(filenames.at(nf));

  results.push_back("\n\n");
  if(comparisonon)
    results.push_back("\nFile containing the golden values =\n \t"+name_of_ref_file+"\n\n");
  Read_ref_file(name_of_ref_file);

  TH1F* h = NULL;
  Double_t mps_normalizer=0;
  Double_t qrt_normalizer=0;

  h=GetHisto("mps_histo/qwk_charge_hw");
  mps_normalizer=h->GetMean()*h->GetEntries();
  h=NULL;
  h = GetHisto("hel_histo/yield_qwk_charge_hw");
  qrt_normalizer=h->GetMean()*h->GetEntries();

  results.push_back("========================================================\n");
  results.push_back("ANALYSIS CHECKS : \n");
  results.push_back("-----------------\n");
  results.push_back(Form("number of good quartets in this run = %6.0f \n",h->GetEntries()));
  results.push_back(Form("based on this number the run lasted = %5.1f minutes \n\n",h->GetEntries()*4./1000./60.));
  h=NULL;
  
  FillBeamParameters();
  FillMDParameters();
  FillLUMIParameters();


  
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
      output.close();
    }
  else if(outputformat==0)
    {
      std::cout<<"\n ======= BEGIN ======= \n";
      std::cout<<"\t RUN = "<<runnum<<std::endl<<std::endl;
      for(size_t i=0;i<results.size();i++)
	std::cout<<results[i];
      std::cout<<"\n ======== END ======== \n";
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
  if (file_list.size()==0)
    return histo;

  histo=(TH1F*)file_list.at(0)->Get(histoname);
  if (histo==NULL){
    std::cerr<<"Histogram name does not exist in MPS \n";
    return histo;
  }

  for(UInt_t i=1; i<file_list.size();i++){
    histo->Add((TH1F*)file_list.at(i)->Get(histoname),1);//this=this+histo*c1 where c1=1
  }
  
  return histo;
};




//***************************************************
//***************************************************
//         Read the beam properties              
//***************************************************
//***************************************************
void FillBeamParameters(){
  util.clear();
  TString comp;


  char tmp[100];
  util.push_back("\n");
  util.push_back("========================================================\n");
  util.push_back("BEAM PARAMETERS \n");
  util.push_back("--------------- \n \n");

  Get_Mean("mps_histo/qwk_charge_hw",intensity,1);


  if(intensity==0){
    util.push_back(" Beam current is zero \n Therefore then summary is not going to make much sense ");
    for (int i=0;i<10;i++)
      std::cout<<" NO beam detected ! \n";
  }

  util.push_back("\n quantity|    value   |      Asym/Diff       | Asym/Diff width");
  util.push_back("\n ........|(uA,mm,mrad)|    (ppm,nm,nrad)     | (ppm, nm, nrad) ");

  Double_t mean;
  Double_t val[4];
  //Compare beam charge
  Fit_with_a_gaussian("hel_histo/asym_qwk_charge_hw",val,1.e+6);//factor 1e+6 to convert to ppm
  util.push_back(Form("\n charge  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",intensity,val[0],val[2],val[1],val[3]));
  compare_to_golden_value("charge_asymmetry", val[0], val[2]);
  compare_to_golden_value("charge_asymmetry_width", val[1], val[3]);

  //Beam on target positions
  Get_Mean("mps_histo/qwk_targetX_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetX_hw",val,1.e+6);//factor 1e+6 to convert to nm
  util.push_back(Form("\n x targ  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",mean,val[0],val[2],val[1],val[3]));
  compare_to_golden_value("x_beam_position", mean,0);
  compare_to_golden_value("x_position_difference", val[0], val[2]);
  compare_to_golden_value("x_position_difference_width", val[1], val[3]);

  Get_Mean("mps_histo/qwk_targetY_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetY_hw",val,1.e+6);
  util.push_back(Form("\n y targ  |   %5.1f    | %7.1f +/- %7.1f  | %7.1f +/- %7.1f ",mean,val[0],val[2],val[1],val[3]));
  compare_to_golden_value("y_beam_position", mean,0);
  compare_to_golden_value("y_position_difference", val[0], val[2]);
  compare_to_golden_value("y_position_difference_width", val[1], val[3]);

  Get_Mean("mps_histo/qwk_targetXSlope_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetXSlope_hw",val,1000.);
  util.push_back(Form("\n Angle x    |   %5.1f    | %7.1f +/- %7.1f  | %7.1f +/- %7.1f ",TMath::ATan(mean),val[0],val[2],val[1],val[3]));
  compare_to_golden_value("x_beam_angle", TMath::ATan(mean),0);
  compare_to_golden_value("x_angle_difference", val[0], val[2]);
  compare_to_golden_value("x_angle_difference_width", val[1], val[3]);


  Get_Mean("mps_histo/qwk_targetYSlope_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/diff_qwk_targetYSlope_hw",val,1000.);
  util.push_back(Form("\n Angle y    |   %5.1f    | %7.1f +/- %7.1f  | %7.1f +/- %7.1f ",TMath::ATan(mean),val[0],val[2],val[1],val[3]));
  compare_to_golden_value("y_beam_angle", TMath::ATan(mean),0);
  compare_to_golden_value("y_angle_difference", val[0], val[2]);
  compare_to_golden_value("y_angle_difference_width", val[1], val[3]);
  util.push_back("\n Note: Angles are in radians while angle differences are still presented in gradient differences /n");

  
  Get_Mean("mps_histo/qwk_energy_hw",mean,1.);
  Fit_with_a_gaussian("hel_histo/asym_qwk_energy_hw",val,1.e+6);
  util.push_back(Form("\n Energy (dP/P)  |   %5.1f    | %7.1f +/- %7.1f  | %7.1f +/- %7.1f ",mean,val[0],val[2],val[1],val[3]));
  compare_to_golden_value("beam_energy", mean,0);
  compare_to_golden_value("beam_energy_asymmetry", val[0], val[2]);
  compare_to_golden_value("beam_energy_asymmetry_width", val[1], val[3]);
  


  // halo rates
  Get_Mean("mps_histo/sca_halo3",mean,1.);
  sprintf(tmp,"\n\n  Halo 3 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_halo4",mean,1.);
  sprintf(tmp,"\n\n  Halo 4 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_halo5",mean,1.);
  sprintf(tmp,"\n\n  Halo 5 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_halo6",mean,1.);
  sprintf(tmp,"\n\n  Halo 6 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_halo7",mean,1.);
  sprintf(tmp,"\n\n  Halo 7 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_halo8",mean,1.);
  sprintf(tmp,"\n\n  Halo 8 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_halo9",mean,1.);
  sprintf(tmp,"\n\n  Halo 9 rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_31mhz",mean,1.);
  sprintf(tmp,"\n\n  31MHz Clock rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_31mhzreturn",mean,1.);
  sprintf(tmp,"\n\n  31MHz Clock return rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
  Get_Mean("mps_histo/sca_4mhz",mean,1.);
  sprintf(tmp,"\n\n  4MHz Clock rate (KHz) = %7.1f (%7.1f (KHz/uA))", mean*1000.,intensity ==0? 0.0:mean*1000./intensity);
  util.push_back(TString(tmp)+" \n");
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
  util.push_back("========================================================\n");
  util.push_back("MAIN DETECTOR PARAMETERS \n");
  util.push_back("------------------------ \n \n");

  util.push_back("\n quantity|    value   |      Asym            | Asym width");
  util.push_back("\n ........|(V/uA)      |    (ppm)             | (ppm) ");

  Double_t mean;
  Double_t val[4];
  for (int count=1;count<9;count++){
    histname=Form("hel_histo/yield_md%ibarsum_hw",count);
    //std::cout<<histname<<"\n";
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_md%ibarsum_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to nm
    util.push_back(Form("\n MD%i  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    histname=Form("MD_pmt%i_pos",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("MD_pmt%i_pos_asymmetry",count);
    compare_to_golden_value(histname, val[0], val[2]);
    histname=Form("MD_pmt%i_pos_width",count);
    compare_to_golden_value(histname, val[1], val[3]);
  }


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
  util.push_back("========================================================\n");
  util.push_back("DOWN STREAM LUMINOSITY MONITORS \n");
  util.push_back("------------------------------- \n \n");

  util.push_back("\n quantity|    value   |      Asym            | Asym width");
  util.push_back("\n ........|(V/uA)      |     (ppm)            | (ppm) ");

  Double_t mean;
  Double_t val[4];
  for (int count=1;count<9;count++){
    histname=Form("hel_histo/yield_qwk_dslumi%i_hw",count);
    //std::cout<<histname<<"\n";
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_qwk_dslumi%i_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to ppm
   util.push_back(Form("\n DSLUMI%i  |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    histname=Form("dslumi_det_%i_yield",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("dslumi_det_%i_asymmetry",count);
    compare_to_golden_value(histname, val[0], val[2]);
    histname=Form("dslumi_det_%i_asymmetry_width",count);
    compare_to_golden_value(histname, val[1], val[3]);
  }

  util.push_back("\n");
  util.push_back("========================================================\n");
  util.push_back("UP STREAM LUMINOSITY MONITORS \n");
  util.push_back("----------------------------- \n \n");

  util.push_back("\n quantity|    value   |      Asym/           | Asym width");
  util.push_back("\n ........|(V/uA)      |    (ppm)             | (ppm) ");
 
  for (int count=1;count<9;count+=2){

    histname=Form("hel_histo/yield_qwk_uslumi%ineg_hw",count);
    Get_Mean(histname,mean,1.);
    histname=Form("hel_histo/asym_qwk_uslumi%ineg_hw",count);
    Fit_with_a_gaussian(histname,val,1.e+6);//factor 1e+6 to convert to ppm
    util.push_back(Form("\n uslumi%ineg   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
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
    util.push_back(Form("\n uslumi%ipos   |   %5.2f    | %7.2f +/- %7.2f  | %7.2f +/- %7.2f ",count,mean,val[0],val[2],val[1],val[3]));
    histname=Form("uslumi_det_%i_yield",count);
    compare_to_golden_value(histname, mean,0);
    histname=Form("uslumi_det_%i_asymmetry",count);
    compare_to_golden_value(histname, val[0],val[2]);
    histname=Form("uslumi_det_%i_asymmetry_width",count);
    compare_to_golden_value(histname,val[1],val[3] );    
  }


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
    h->Fit("gaus","Q");
    *container=h->GetFunction("gaus")->GetParameter(1)*factor; // mean
    *(container+1)=h->GetFunction("gaus")->GetParameter(2)*factor; //width
    *(container+2)=h->GetFunction("gaus")->GetParError(1)*factor;
    *(container+3)=h->GetFunction("gaus")->GetParError(2)*factor;
    delete h;
  } else {
    std::cout<<"Fit_with_a_gaussian Error: "<<histname<<" Not found!"<<std::endl;
    *container     = 0.0;
    *(container+1) = 0.0;
    *(container+2) = 0.0;
    *(container+3) = 0.0;
  }

  return;
}

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
	      if(!util[util.size()-1].Contains("***")&&comparisonon)
		//		 util[util.size()-1].Append(" *** out of acceptable range ***");
		 util[util.size()-1].Append(" *** values out of acceptable range:");
	      util[util.size()-1].Append(Form("%5.2f; ",value));
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


