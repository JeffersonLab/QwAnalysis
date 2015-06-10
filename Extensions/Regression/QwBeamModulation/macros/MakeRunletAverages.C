#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TObject.h"
#include "TObjArray.h"


#include <vector>
#include "math.h"
#include <iostream>
#include <fstream>
#include <utility>

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: April 14, 2014
//Program averages runlet average trees to produce slug average trees. Averages  
//are weighted by statistical error of mdallbars.
///////////////////////////////////////////////////////////////////////////////

const Int_t nRUNS=20000, nSLUGS=325, nMD = 8;

typedef struct Branch_t{
  double value;
  double err;
  double rms;
  int n;
};
typedef struct mytype_t{
  bool isInt;
  TString name;
};

const Double_t ERROR = -1.0e6, epsilon = 1.0e-20;
using namespace std;
Int_t RoundToNearestWhole(double num)
{
  int val = ((int(num + 0.5) > int(num)) ? int(num) + 1 : int(num));
  return val;
}

Branch_t ProcessRunlet(double mean, double meansq, double n)
{
  Branch_t t;
  mean /= n;
  meansq /= n;
  t.value = mean;
  t.rms = sqrt((meansq - pow(mean, 2))*n/(n-1));
  t.err = t.rms / sqrt(n);
  if(t.rms<epsilon){
    t.value = ERROR;
    t.rms = ERROR;
    t.err  = ERROR;
  }
  t.n = (int)n;
  return t;
}




//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: July 30, 2014
//Program averages quartet slugfiles to produce runlet averages files.
//
//Usage: slug         slug number
//       type         beginning of filename to be runlet averaged-- must be 
//                    "reduced", "corrected" or "*regressed"
//       treename     name of tree in file
//       stem         additional file identifier such as "_new_monitors", 
//                    "_compton_bpm" or "_sine_only"
///////////////////////////////////////////////////////////////////////////////



Int_t MakeRunletAverages(Int_t slug, TString type = "reduced", 
			 TString  treename = "slug", TString stem = "",
			 Int_t harddisk = 1) 
{
  bool debug = 0, add_composite_leaves = 1;
  int run_period = (slug<137 ? 1 : 2);
  TString dirname = Form("/net/data2/paschkelab%i/reduced_slugfiles/run%i",	
			 run_period+1, run_period);
  TString dirname_1 = Form("/net/data2/paschkelab%i/reduced_slugfiles/run%i",	
			   harddisk, run_period);

  //Get tree to be averaged. 
  /////////////////////////////////////////////////////////
  char* suffix = (char*)stem.Data();
  TString reduced_filename = Form("%s/reduced_slug%i.root",dirname.Data(), slug);
  TString filename = Form("%s/%s_slug%i%s.root", dirname_1.Data(), 
			  type.Data(), slug, suffix);
  TFile *file = new TFile(filename);
  TTree *tree = (TTree*)file->Get(treename.Data());

  if(!tree){cout<<"Tree not found. Exiting.\n"; return -1;}
  else cout<<filename.Data()<<" found.\n";
  /////////////////////////////////////////////////////////

  
  //Get list of leaves
  /////////////////////////////////////////////////////////
  vector<mytype_t> leafnames;
  Int_t nInt = 0, nDouble = 0;
  TObjArray *lfArr = tree->GetListOfLeaves();
  lfArr->ls();
  for(int ileaf=0; ileaf<=lfArr->GetLast(); ++ileaf){
    mytype_t temp;
    TString lfname = lfArr->At(ileaf)->ClassName();
    temp.isInt = lfname.CompareTo("TLeafI")==0;
    temp.name = lfArr->At(ileaf)->GetName();

    //Don't include DEC's
    if(temp.name.Contains("Device_Error_Code")) continue;
    //Include only yields, differences and asymmetries
    if(!(temp.name.Contains("yield")||temp.name.Contains("diff") ||
	 temp.name.Contains("asym"))) continue;
    //Don't include new monitors unless using them
    if(!stem.Contains("new_monitors") && 
       (temp.name.Contains("diff_MX")||temp.name.Contains("diff_MY")))continue;

    leafnames.push_back(temp);
    if(lfname.CompareTo("TLeafI"))++nDouble;
    else ++nInt;
  }
  cout<<"Leaves:"<<nInt<<" "<<nDouble<<endl;
  for(size_t i=0; i<leafnames.size();++i)
    cout<<leafnames.at(i).name.Data()<<" "<<
      (leafnames.at(i).isInt ? "I" : "D")<<endl;
  /////////////////////////////////////////////////////////

  if(filename.CompareTo(reduced_filename)!=0){
    cout<<"Adding reduced tree as friend.\n";
    tree->AddFriend("slug", reduced_filename.Data());
  }
  int size = (int)leafnames.size();

  // Only enable desired leaves
  int runlet, run;
  double *value = new double[size];
  tree->SetBranchStatus("*",0); // disable all branches
  tree->SetBranchStatus("runlet", 1);
  tree->SetBranchAddress("runlet", &runlet);
  tree->SetBranchStatus("run", 1);
  tree->SetBranchAddress("run", &run);
  for (int i=0; i<size; ++i) {
    tree->SetBranchStatus(Form("%s",leafnames[i].name.Data()), 1);	
    tree->SetBranchAddress(Form("%s",leafnames[i].name.Data()), &value[i]);
  }

  //Additional information needed to create averages of positive and 
  //negative PMT's on main detector
  int idxPos[nMD], idxNeg[nMD], initial = -1;
  if(add_composite_leaves){
    for(int i=0; i<nMD;++i){
      idxNeg[i] = initial;
      idxPos[i] = initial;
    }

    for(int ileaf=0;ileaf<size;++ileaf){
      if(!leafnames[ileaf].name.Contains("asym_qwk_md")) continue;
      for(int imd=0;imd<nMD;++imd){
	TString name = leafnames[ileaf].name;
	if(name.CompareTo(Form("asym_qwk_md%ineg", imd+1))==0)
	  idxNeg[imd] = ileaf;
	else if(name.CompareTo(Form("corrected_asym_qwk_md%ineg",imd+1))==0)
	  idxNeg[imd] = ileaf;

	if(name.CompareTo(Form("asym_qwk_md%ipos", imd+1))==0)
	  idxPos[imd] = ileaf;
	else if(name.CompareTo(Form("corrected_asym_qwk_md%ipos", imd+1))==0)
	  idxPos[imd] = ileaf;
      }
    }
    for(int i=0; i<nMD;++i){
      if(idxNeg[i]==initial || idxPos[i]==initial){
	cout<<"Not all individual PMT's found. Exiting.\n";
	return -1;
      }
    }
  }
  double *mean = new double[size], *meansq = new double[size], N = 0;
  double meanPos = 0, meanNeg = 0, meansqPos = 0, meansqNeg = 0;
  double meanDiff = 0, meansqDiff = 0, meanAll = 0, meansqAll = 0;
  int prev_runlet = 0, prev_run = 0, nEnt = tree->GetEntries();
  for(int i=0; i<size; ++i){
    mean[i] = 0;
    meansq[i] = 0;
  }
  TString file_stem = (type.Contains("std_reg_reduced") ? "std_regressed" : 
		  type.Data());
  file_stem = (type.Contains("set11_reg_reduced") ? "set11_regressed" : 
	  file_stem.Data());  

  char *all_avg_name = "corrected_asym_mdallpmtavg";
  char *neg_avg_name = "corrected_asym_pmtavgneg";
  char *pos_avg_name = "corrected_asym_pmtavgpos";
  char *diff_avg_name = "corrected_asym_pmtavg_pos_minus_neg";



  for(int ient = 0; ient<nEnt; ++ient){
    tree->GetEntry(ient);

    //intialize arrays & store data
    if((runlet != prev_runlet || run != prev_run) && ient > 0){
      printf("Processing slug %i run %i.%03i\n",slug, prev_run, prev_runlet);
      FILE *avg_file = fopen(Form("%s/runlet_averages/%s_averages%i_%03i%s.dat",
			  dirname.Data(), file_stem.Data(), prev_run,
			  prev_runlet, suffix), "w+");
      if(avg_file == NULL ){
	cout<<"Failed to open file for writing. Exiting.\n";
	return -1;
      }
      for(int i=0; i<size; ++i){
	Branch_t t = ProcessRunlet(mean[i], meansq[i], N);
	//skip doubly named branch
	if(leafnames[i].name.CompareTo("asym_md6pmtavg")==0 && t.rms == ERROR)
	  continue;
	if(debug)
	  printf("%s  %18.12e  %18.12e  %18.12e  %d\n", leafnames[i].name.Data(), 
		 t.value, t.err, t.rms, t.n);
	fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
		leafnames[i].name.Data(), t.value, t.err, t.rms, t.n);
	mean[i] = 0;
	meansq[i] = 0;
      }

      //process composite leaves
      if(add_composite_leaves){
	//	cout<<"Adding pmtavg leaves\n";
	Branch_t tEx = ProcessRunlet(meanNeg, meansqNeg, N);
	fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
		neg_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
	meanNeg = 0;
	meansqNeg = 0;
	
	tEx = ProcessRunlet(meanPos, meansqPos, N);
	fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
		pos_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
	meanPos = 0;
	meansqPos = 0;
	
	tEx = ProcessRunlet(meanDiff, meansqDiff, N);
	fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
		diff_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
	meanDiff = 0;
	meansqDiff = 0;

	tEx = ProcessRunlet(meanAll, meansqAll, N);
	fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
		all_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
	meanAll = 0;
	meansqAll = 0;
      }



      N = 0;
      fclose(avg_file);
    }
    N++;
    for(int i=0; i<size; ++i){
      mean[i] += value[i];
      meansq[i] += value[i] * value[i];
    }
    if(add_composite_leaves){
      for(int imd=0; imd<nMD; ++imd){
	meanPos += value[idxPos[imd]] / nMD;
	meansqPos += pow(value[idxPos[imd]] / nMD ,2);
	meanNeg += value[idxNeg[imd]] / nMD;
	meansqNeg += pow(value[idxNeg[imd]] / nMD ,2);
	meanDiff += (value[idxPos[imd]] - value[idxNeg[imd]]) / nMD;
	meansqDiff += pow((value[idxPos[imd]] - value[idxNeg[imd]]) / nMD ,2);
	meanAll += (value[idxPos[imd]] + value[idxNeg[imd]]) / (2.0 * nMD);
	meansqAll += pow((value[idxPos[imd]] + value[idxNeg[imd]])/(2.0*nMD) ,2);
      }
    }
    prev_runlet = (int)runlet;
    prev_run = (int)run;
  }

  FILE *avg_file = fopen(Form("%s/runlet_averages/%s_averages%i_%03i%s.dat",
			  dirname.Data(), file_stem.Data(), prev_run,
			  prev_runlet, suffix), "w+");
  if(avg_file == NULL ){
    cout<<"Failed to open file for writing. Exiting.\n";
    return -1;
  }
  printf("Processing slug %i run %i.%03i\n",slug, prev_run, prev_runlet);
  for(int i=0; i<size; ++i){
    Branch_t t = ProcessRunlet(mean[i], meansq[i], N);
    //skip doubly named branch
    if(leafnames[i].name.CompareTo("asym_md6pmtavg")==0 && t.rms == ERROR)
      continue;

    if(debug)
      printf("%s  %18.12e  %18.12e  %18.12e  %d\n", leafnames[i].name.Data(), 
	     t.value, t.err, t.rms, t.n);
    fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
	    leafnames[i].name.Data(), t.value, t.err, t.rms, t.n);
    
  }
  //process composite leaves
  if(add_composite_leaves){
    //    cout<<"Adding pmtavg leaves\n";
    Branch_t tEx = ProcessRunlet(meanNeg, meansqNeg, N);
    fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
	    neg_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
    meanNeg = 0;
    meansqNeg = 0;
    
    tEx = ProcessRunlet(meanPos, meansqPos, N);
    fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
	    pos_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
    meanPos = 0;
    meansqPos = 0;
    
    tEx = ProcessRunlet(meanDiff, meansqDiff, N);
    fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
	    diff_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
    meanDiff = 0;
    meansqDiff = 0;
    
    tEx = ProcessRunlet(meanAll, meansqAll, N);
    fprintf(avg_file, "%s  %18.12e  %18.12e  %18.12e  %d\n", 
	    all_avg_name, tEx.value, tEx.err, tEx.rms, tEx.n);
    meanAll = 0;
    meansqAll = 0;
  }
  fclose(avg_file);

  return 0;
}
