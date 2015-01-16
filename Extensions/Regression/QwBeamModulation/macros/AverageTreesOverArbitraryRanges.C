#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TIterator.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TObject.h"
#include "TObjArray.h"


#include <vector>
#include <iostream>
#include <fstream>
#include <utility>

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: April 14, 2014
//Program averages runlet average trees to produce slug or run average trees. 
//Averages are weighted by statistical error of mdallbars.
//
//Requires that $DB_ROOTFILES be set to the directory of the DB rootfiles.
//
//Usage: CreateAverageDBFriendTree(Int_t run_period, Bool_t run_average,
//				    TString filename, TString treename)
//run_period:   1 or 2
//avg_type: 0 run_average, 1 slug_average, 2 pitt_average, 3 wien_average
//filename:     name of rootfile to average
//treename:     name of tree in rootfile
//
//Example: CreateAverageDBFriendTree(1,0,"HYDROGEN-CELL_on_tree.root","tree")
//         creates a slug average of "HYDROGEN-CELL_on_tree.root" for Run 1
//         called "slug_averaged_HYDROGEN-CELL_on_tree.root".
//
//NOTE: not all integer leaves are copied but can be easily added as necessary.
//
///////////////////////////////////////////////////////////////////////////////

const Int_t nRUNS=20000, nSLUGS=325, nPITTS = 100, nWIENS = 13;
const Double_t ERROR = -1.0e6;
const int run1_start = 42, run2_start = 137;

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

using namespace std;

const int wien_range[nWIENS][2] = {{9594, 9812}, {9939, 10187}, {10196, 11129}, 
				   {11131, 11390}, {11391, 11711}, {11714, 12162},
				   {13843, 14258}, {14260, 14683}, {15163, 15574},
				   {15581, 15986}, {16172, 16717}, {16718, 17505},
				   {18415, 18927}};

Int_t GetIndex(int run_number, int slug, int avg_type, 
	       const int wien_range[nWIENS][2]){
  int idx = -1;
    switch (avg_type)
      {
      case 0:
	idx = run_number;
	break;
      case 1:
	idx = slug;
	break;
      case 2:
	if(slug>=run1_start&&slug<run2_start)
	  idx = (int)((slug - run1_start)/4.0)+1;
	else if (slug>=run2_start&&slug<=321)
	  idx = (int)((slug - run2_start)/4.0)+1;
	if(idx==-1){
	  cout<<"No pitt found for run "<<run_number<<". Exiting.\n";
	}
	break;
      case 3:
	for(int i=0;i<nWIENS;++i){
	  if(run_number >= wien_range[i][0] && run_number <= wien_range[i][1]){
	    idx = i;
	    break;
	  }
	}
	if(idx==-1){
	  cout<<"No wien found for run "<<run_number<<". Exiting.\n";
	}
	break;

      default:
	cout<<"No such average type. Exiting.\n";
	break;

      }
    return idx;
}


Int_t AverageTreesOverArbitraryRanges(Int_t run_period, Int_t avg_type,
				  TString filename, TString treename) 
{
  Bool_t debug = 0;
  double wien_num[nWIENS] = {0,1,2,3,4,5,6,7,8,8.5,9,9.5,10};

  //Get necessary trees 
  /////////////////////////////////////////////////////////
  TString dirname = Form("%s/run%i",
			 gSystem->Getenv("DB_ROOTFILES"), run_period);
  //1 Tree from which to get runlist
  TFile *file = new TFile(Form("%s/%s",dirname.Data(),
			       "HYDROGEN-CELL_off_tree.root"));
  TTree *tree = (TTree*)file->Get("tree");

  //2 Tree and leaf to use for weights
  TString weights_tree_name = "corrected_tree";
  TString weights_leaf_name = "corrected_asym_qwk_mdallbars";
  TString bmod_file_name = Form("%s/hydrogen_cell_corrected_tree"
				"_new_monitors.root",dirname.Data());
  tree->AddFriend(weights_tree_name.Data(), bmod_file_name.Data());

  //3 Tree to be averaged
  TFile *average_file = new TFile(Form("%s/%s",dirname.Data(),
				      filename.Data()));
  TTree *average_tree = (TTree*)average_file->Get(treename.Data());
  /////////////////////////////////////////////////////////
  
  
  //Get list of branches
  /////////////////////////////////////////////////////////
  vector<mytype_t> leafnames;
  vector<TString> branchnames;
  Int_t nInt = 0, nDouble = 0;
  TObjArray *brArr = average_tree->GetListOfBranches();
  TObjArray *lfArr = average_tree->GetListOfLeaves();
  for(int ibranch=0; ibranch<brArr->GetSize(); ++ibranch){
    Bool_t isleaf = false;
    mytype_t temp;
    for(int ileaf=0; ileaf<lfArr->GetSize(); ++ileaf){
      TString brname = brArr->At(ibranch)->GetName();
      if(brname.CompareTo(lfArr->At(ileaf)->GetName())==0){
	TString lfname = lfArr->At(ileaf)->ClassName();
	temp.isInt = lfname.CompareTo("TLeafI")==0;
	temp.name = lfArr->At(ileaf)->GetName();
	leafnames.push_back(temp);
	isleaf = 1;
	if(lfname.CompareTo("TLeafI"))++nDouble;
	else ++nInt;
	break;
      }
    }
    if(!isleaf)
      branchnames.push_back(brArr->At(ibranch)->GetName());
  }
  cout<<"Leaves:"<<nInt<<" "<<nDouble<<endl;
  for(size_t i=0; i<leafnames.size();++i)
    cout<<leafnames.at(i).name.Data()<<" "<<
      (leafnames.at(i).isInt ? "I" : "D")<<endl;

  cout<<"Branches:"<<endl;
  for(size_t i=0; i<branchnames.size();++i)
    cout<<branchnames.at(i)<<endl;
  /////////////////////////////////////////////////////////


  //Initialize vectors.run_number
  /////////////////////////////////////////////////////////
  Branch_t initval;
  initval.value = 0;
  initval.rms = 0;
  initval.err = 0;
  initval.n = 0;

  Int_t N = (avg_type==0? nRUNS : (avg_type==1? nSLUGS :
				  (avg_type==2? nPITTS : nWIENS)));
  vector<Int_t>vNRunlets, vSlug, vMasterList;
  vector<double>vWien, vRun;
  for(int i=0;i<N;++i){
    vNRunlets.push_back(0);
    vSlug.push_back(0);
    vMasterList.push_back(0);
    vRun.push_back(0);
    vWien.push_back(0);
  }
  vector<vector<Branch_t> >vBranches;
  vector<vector<Double_t> >vWeights;
  vBranches.resize(branchnames.size());
  vWeights.resize(branchnames.size());
  for(int i=0;i<int(branchnames.size());++i){
    for(int j=0;j<N;++j){
      vBranches.at(i).push_back(initval);
      vWeights.at(i).push_back(0);
    }
  }

  cout<<"All initialized and ready!"<<endl;
  /////////////////////////////////////////////////////////


  //Set up list of runs/slugs to be in tree.
  //In order to make all trees friendable we need a master list.
  /////////////////////////////////////////////////////////////
  Int_t rn, sl;
  tree->SetBranchAddress("run_number",&rn);
  tree->SetBranchAddress("slug",&sl);

  for(int i=0;i<tree->GetEntries();++i){
    tree->GetEntry(i);
    if(!(sl>=42&&sl<=321))continue;
    int val = GetIndex(rn, sl, avg_type, wien_range);
    if(tree->GetBranch("asym_qwk_mdallbars")->GetLeaf("n")->GetValue()>0
       && sl>=42 && sl <= 321){
      ++vMasterList[val];
      vWien.at(val) = (double)wien_num[GetIndex(rn, sl, 3, wien_range)];
      vRun.at(val) = tree->GetLeaf("run_number")->GetValue();
    }
  }
  tree->ResetBranchAddresses();
  /////////////////////////////////////////////////////////////


  //Loop over tree filling vectors with MD weighted averages.
  //////////////////////////////////////////////////////////
  const int nINTLEAVES = 6;
  TString integerLeaves[nINTLEAVES] = {"ihwp_setting","phwp_setting",
				       "precession_reversal","wien_reversal",
				       "sign_correction", "run_number"};
  TObjArray *obj = tree->GetFriend("corrected_tree")->GetListOfBranches();
  TString mdallbars;
  for(int i=0;i<obj->GetSize();++i){
    mdallbars = obj->At(i)->GetName();
    if( mdallbars.Contains(weights_leaf_name.Data()) )
      break;
  }
  if( !mdallbars.Contains(weights_leaf_name.Data()) ){
    cout<<"Leaf "<<weights_leaf_name.Data()<<" not found. Exiting.\n";
    return -1;
  }
  tree->ResetBranchAddresses();

  Int_t slug, run_number;
  Int_t size = (avg_type==0 ? nINTLEAVES:nINTLEAVES-1), nrunlets = 0;
  vector<vector<Int_t> >vLeaves;
  vLeaves.resize(size);
  for(int i=0;i<size;++i){
    vLeaves[i].resize(N);
  }
  Int_t *intLeaves = new Int_t[size], nSignErrors = 0;
  tree->SetBranchAddress("slug",&slug);
  for(int i=0;i<size;++i){
    tree->SetBranchAddress(integerLeaves[i].Data(), &intLeaves[i]);
  }
  for(int ient=0; ient<tree->GetEntries(); ++ient){
    if(ient%1000==0)cout<<"Processing entry "<<ient<<endl;
    tree->GetEntry(ient);
    average_tree->GetEntry(ient);
    run_number = (int)tree->GetLeaf("run_number")->GetValue();
    int idx = GetIndex(run_number, slug, avg_type, wien_range);
    //don't include values with no entries
    if(tree->GetBranch("asym_qwk_mdallbars")->GetLeaf("n")->GetValue()<1){
      continue;
    }
    //don't include slugs that aren't LH2
    if(!(slug>=42&&slug<=321))continue;

    //***Using mdall_error weights from "average tree"--perhaps not the best
    //***because it creates inconsistent weights between trees.
    double err_MD = tree->GetBranch(mdallbars.Data())->GetLeaf("err")->GetValue();

    vSlug[idx] = slug;
    //    cout<<"IDX: "<<idx<<endl;
    //don't include errors
    if(err_MD==ERROR)continue;

    ++vNRunlets[idx];

    //remove sign_correction error
    int sign = (int)((tree->GetLeaf("precession_reversal")->GetValue()*2.0-1.0) *
		     (tree->GetLeaf("wien_reversal")->GetValue()*2.0-1.0) *
		     (tree->GetLeaf("ihwp_setting")->GetValue()*2.0-1.0) *(-1.0));

    if(sign != tree->GetLeaf("sign_correction")->GetValue() ){
      ++nSignErrors;
      cout<<"Sign correction error at entry "<<ient<<". run: "<<run_number<<endl;
    }
    for(int ileaf=0;ileaf<size;++ileaf){
      vLeaves[ileaf][idx] = (integerLeaves[ileaf].Contains("sign_correction") ?
			     sign : intLeaves[ileaf]);
      if(debug)
	cout<<ient<<" "<<slug<<" "<<integerLeaves[ileaf].Data()
	    <<": "<<intLeaves[ileaf]<<endl;
    }

    for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
      double value = average_tree->GetBranch(branchnames[ibranch].Data())->
	GetLeaf("value")->GetValue();
      double err = average_tree->GetBranch(branchnames[ibranch].Data())->
	GetLeaf("err")->GetValue();
      int n = (int)average_tree->GetBranch(branchnames[ibranch].Data())->
	GetLeaf("n")->GetValue();
	//	cout<<branchnames[ibranch].Data()<<":"<<value<<" "<<err<<" "<<n
        //	    <<"  "<<err_MD<<endl;
      vBranches[ibranch][idx].value += value / (err_MD * err_MD);
      vBranches[ibranch][idx].err += pow(err / (err_MD * err_MD) , 2);
      vBranches[ibranch][idx].rms += pow(value / err_MD, 2);
      vWeights[ibranch][idx] += pow(err_MD , -2);
      vBranches[ibranch][idx].n += n;
     
    }
  }

  for(int i=0;i<= N;++i){
    for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
      if(vBranches[ibranch][i].n>0){
	vBranches[ibranch][i].value /=  vWeights[ibranch][i];
	if(vNRunlets[i]>1){
	  vBranches[ibranch][i].rms /=  vWeights[ibranch][i];
	  vBranches[ibranch][i].rms -= pow(vBranches[ibranch][i].value , 2);
	  vBranches[ibranch][i].rms = pow( vBranches[ibranch][i].rms, 0.5);
	}else
	  vBranches[ibranch][i].rms = 0;
	vBranches[ibranch][i].err = pow(vBranches[ibranch][i].err, 0.5);
	vBranches[ibranch][i].err /= vWeights[ibranch][i];
      }
      if(i>136&&i<322)
      cout<<branchnames[ibranch].Data()<<":: "<<i<<": "
	  <<vBranches[ibranch][i].value<<" "
	  <<vBranches[ibranch][i].err<<endl;
    }
  }
  //////////////////////////////////////////////////////////


  //create new average tree
  //////////////////////////////////////////////////////////
  delete tree; 
  double wien, run;
  char *name = (char*)(avg_type==0 ? "run" : (avg_type==1? "slug":
				       (avg_type==2? "pitt":"wien")));
  TFile *new_file = new TFile(Form("%s/%s_averaged_%s", dirname.Data(), name,
				   filename.Data()), "recreate");
  TTree *new_tree = new TTree(treename.Data(),treename.Data());
  Branch_t *bBranches = new Branch_t[int(branchnames.size())]; 
  TBranch *brRun = new_tree->Branch("run", &run,"run/D"); 
  TBranch *brSlug = new_tree->Branch("slug", &slug,"slug/I"); 
  TBranch *brWien = new_tree->Branch("wien", &wien,"wien/D"); 
  TBranch *brNRunlets = new_tree->Branch("nRunlets", &nrunlets,"nRunlets/I"); 
  TBranch **brILeaves = new TBranch*[size]; 
  TBranch **branches = new TBranch*[int(branchnames.size())]; 

  for(int ileaf=0;ileaf<size;++ileaf){
    brILeaves[ileaf] = new_tree->Branch(integerLeaves[ileaf].Data(), 
				     &intLeaves[ileaf],
				     Form("%s/I",integerLeaves[ileaf].Data()));
  }
  for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
    branches[ibranch] = new_tree->Branch(branchnames[ibranch].Data(), 
				    &bBranches[ibranch],
				    "value/D:err/D:rms/D:n/I");
  }
  cout<<"SIZES: "<<N<<" "<<vNRunlets.size()<<endl;
  for(int i=0;i<(int)vNRunlets.size();++i){
    if(vMasterList[i]<1 || vSlug[i]<42 || vSlug[i]>321)continue;
    wien = vWien[i];
    slug = vSlug[i];
    run = vRun[i];
    nrunlets = vNRunlets[i];
    if(vBranches[0][i].n>0){
      for(int ileaf=0;ileaf<size;++ileaf){
	intLeaves[ileaf] = vLeaves[ileaf][i];
      }
      for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
	bBranches[ibranch].value = vBranches[ibranch][i].value;
	bBranches[ibranch].err = vBranches[ibranch][i].err;
	bBranches[ibranch].rms = vBranches[ibranch][i].rms;
	bBranches[ibranch].n = vBranches[ibranch][i].n;
      }
    }else{
      for(int ileaf=0;ileaf<size;++ileaf){
	intLeaves[ileaf] = (int)ERROR;
      }
      for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
	bBranches[ibranch].value = ERROR;
	bBranches[ibranch].err = ERROR;
	bBranches[ibranch].rms = ERROR;
	bBranches[ibranch].n = 0;
      }

    }
    new_tree->Fill();
  }
  for(int i=0;i<(int)vMasterList.size();++i)
    if(vMasterList[i]>0)
      cout<<i<<": "<<vMasterList[i]<<endl;
  cout<<nSignErrors<<" total errors in sign correction.\n";
  //  new_tree->Print();
  new_tree->Write("",TObject::kOverwrite);
  //  new_tree->AutoSave();
  file->Close();
  average_file->Close();
  new_file->Close();
  delete  file;
  delete average_file;
  delete new_file;
  //////////////////////////////////////////////////////////


  return 0;

}
