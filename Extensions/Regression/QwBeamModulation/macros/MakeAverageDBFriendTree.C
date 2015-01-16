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
#include <iostream>
#include <fstream>
#include <utility>

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: April 14, 2014
//Program averages runlet average trees to produce slug average trees. Averages  
//are weighted by statistical error of mdallbars.
///////////////////////////////////////////////////////////////////////////////

const Int_t nRUNS=20000, nSLUGS=325;

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

const Double_t ERROR = -1.0e6;
using namespace std;
Int_t RoundToNearestWhole(double num)
{
  int val = ((int(num + 0.5) > int(num)) ? int(num) + 1 : int(num));
  return val;
}


Int_t MakeAverageDBFriendTree(Int_t run_period, Bool_t run_average,
				  TString filename, TString treename) 
{
  Bool_t debug = 0;

  //Get tree to be averaged
  /////////////////////////////////////////////////////////
  TString dirname = Form("/net/data2/paschkelab2/DB_rootfiles/run%i",
			 run_period);


  TString db_filename = Form("%s/%s",dirname.Data(),
			"HYDROGEN-CELL_offoff_tree.root");
  TFile *db_file = new TFile(db_filename.Data());
  TTree *tree = (TTree*)db_file->Get("tree");


  TString reduced_filename = Form("%s/%s",dirname.Data(),
				 "hydrogen_cell_reduced_tree.root");
  TFile *reduced_file = new TFile(reduced_filename.Data());
  TTree *reduced_tree = (TTree*)reduced_file->Get("reduced_tree");


  TString corrected_filename = Form("%s/%s",dirname.Data(),
				 "hydrogen_cell_corrected_tree.root");
  TFile *corrected_file = new TFile(corrected_filename.Data());
  TTree *corrected_tree = (TTree*)corrected_file->Get("corrected_tree");

  TTree *average_tree;
  TFile *average_file = 0;
  TString average_filename =   Form("%s/%s",dirname.Data(), filename.Data());
  if(average_filename.CompareTo(db_filename.Data())==0){
    average_tree = tree;
  }else  if(average_filename.CompareTo(reduced_filename.Data())==0){
    average_tree = reduced_tree;
  }else  if(average_filename.CompareTo(corrected_filename.Data())==0){
    average_tree = corrected_tree;
  }else{
    average_file = new TFile(average_filename.Data());
    average_tree = (TTree*)average_file->Get(treename.Data());
  }

  cout<<"Entries "<<tree->GetEntries()<<" "<<reduced_tree->GetEntries()<<" "<<
    corrected_tree->GetEntries()<<" "<<average_tree->GetEntries()<<endl;

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


  //Initialize vectors.
  /////////////////////////////////////////////////////////
  Branch_t initval;
  initval.value = 0;
  initval.rms = 0;
  initval.err = 0;
  initval.n = 0;

  Int_t N = (run_average ? nRUNS : nSLUGS);
  vector<Int_t>vNRunlets, vSlug;
  for(int i=0;i<N;++i){
    vNRunlets.push_back(0);
    vSlug.push_back(0);
  }
  vector<vector<Branch_t> >vBranches;
  vector<vector<Double_t> >vWeights, vNeffective;
  vBranches.resize(branchnames.size());
  vWeights.resize(branchnames.size());
  vNeffective.resize(branchnames.size());
  for(int i=0;i<int(branchnames.size());++i){
    for(int j=0;j<N;++j){
      vBranches.at(i).push_back(initval);
      vWeights.at(i).push_back(0);
      vNeffective.at(i).push_back(0);
    }
  }

  cout<<"All initialized and ready!"<<endl;
  /////////////////////////////////////////////////////////


  //Loop over tree filling vectors with MD weighted averages.
  //////////////////////////////////////////////////////////
  const int nINTLEAVES = 6;
  TString integerLeaves[nINTLEAVES] = {"ihwp_setting","phwp_setting",
				       "precession_reversal","wien_reversal",
				       "sign_correction", "run_number"};
  TObjArray *obj = corrected_tree->GetListOfBranches();
  TString mdallbars;
  for(int i=0;i<obj->GetSize();++i){
    mdallbars = obj->At(i)->GetName();
    if( mdallbars.Contains("asym_qwk_mdallbars") )
      break;
  }
  if( !mdallbars.Contains("asym_qwk_mdallbars") ){
    cout<<"Leaf asym_qwk_mdallbars not found. Exiting.\n";
    return -1;
  }

  Int_t slug, run, slopes_exist, good;
  Int_t size = (run_average? nINTLEAVES:nINTLEAVES-1), nrunlets = 0;
  vector<vector<Int_t> >vLeaves;
  vLeaves.resize(size);
  for(int i=0;i<size;++i)
    vLeaves[i].resize((run_average ? nRUNS : nSLUGS));
  Int_t *intLeaves = new Int_t[size];
  tree->SetBranchAddress("slug",&slug);
  for(int i=0;i<size;++i){
    tree->SetBranchAddress(integerLeaves[i].Data(), &intLeaves[i]);
  }
  reduced_tree->SetBranchAddress("run", &run);
  reduced_tree->SetBranchAddress("good", &good);
  corrected_tree->SetBranchAddress("slopes_exist", &slopes_exist);
  for(int ient=0; ient<tree->GetEntries(); ++ient){
    if(ient%1000==0)cout<<"Processing entry "<<ient<<endl;
    tree->GetEntry(ient);
    reduced_tree->GetEntry(ient);
    corrected_tree->GetEntry(ient);
    average_tree->GetEntry(ient);
    int idx = (run_average ? run : slug); 
    //    cout<<ient<<":"<<run<<" "<<good<<" "<<slopes_exist<<" "<<endl;
    if(!(good&&slopes_exist))continue;
    ++vNRunlets[idx];
    vSlug[idx] = slug;
    //    cout<<ient<<" "<<mdallbars.Data()<<endl;
    double err_MD = corrected_tree->GetBranch(mdallbars.Data())->
      GetLeaf("err")->GetValue();
    //    cout<<ient<<" Error: "<<err_MD<<endl;
    for(int ileaf=0;ileaf<size;++ileaf){
      vLeaves[ileaf][idx] = intLeaves[ileaf];
//       cout<<ient<<" "<<slug<<" "<<integerLeaves[ileaf].Data()
// 	  <<": "<<intLeaves[ileaf]<<endl;
    }
    for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
      double value = average_tree->GetBranch(branchnames[ibranch].Data())->
	GetLeaf("value")->GetValue();
      double err = average_tree->GetBranch(branchnames[ibranch].Data())->
	GetLeaf("err")->GetValue();
      int n = (int)average_tree->GetBranch(branchnames[ibranch].Data())->
	GetLeaf("n")->GetValue();
      //    cout<<branchnames[ibranch].Data()<<":"<<value<<" "<<err<<" "<<n<<endl;
      vBranches[ibranch][idx].value += value / (err_MD * err_MD);
      vBranches[ibranch][idx].err += pow(err / (err_MD * err_MD) , 2);
      vBranches[ibranch][idx].rms += pow(value / err_MD, 2);
      vWeights[ibranch][idx] += pow(err_MD , -2);
      vBranches[ibranch][idx].n += n;
      vNeffective[ibranch][idx] += pow(err_MD , -4);
    }
  }
  for(int idx=0;idx<=N;++idx){
    for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
      if(vBranches[ibranch][idx].n>0){
	vNeffective[ibranch][idx] = pow(vWeights[0][idx], 2) /  
	  vNeffective[ibranch][idx];
	vBranches[ibranch][idx].value /=  vWeights[ibranch][idx];
	vBranches[ibranch][idx].rms /=  vWeights[ibranch][idx];
	vBranches[ibranch][idx].rms -= pow(vBranches[ibranch][idx].value , 2);
	vBranches[ibranch][idx].rms = pow( vBranches[ibranch][idx].rms, 0.5);
	vBranches[ibranch][idx].err = pow(vBranches[ibranch][idx].err, 0.5);
	vBranches[ibranch][idx].err /= vWeights[ibranch][idx];
      }
    }
  }
  //////////////////////////////////////////////////////////


  //create new average tree
  //////////////////////////////////////////////////////////
  TFile *new_file = new TFile(Form("%s/%s_averaged_%s", dirname.Data(),
				   (run_average ? "run" : "slug"),
				   filename.Data()), "recreate");
  TTree *new_tree = new TTree(treename.Data(),treename.Data());
  Branch_t *bBranches = new Branch_t[int(branchnames.size())]; 
  TBranch *brSlug = new_tree->Branch("slug", &slug,"slug/I"); 
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
  int ntot = (run_average ? nRUNS : nSLUGS);
  for(int i=0;i<ntot;++i){
    if(vBranches[0][i].n>0){
      slug = vSlug[i];
      nrunlets = vNRunlets[i];
      for(int ileaf=0;ileaf<size;++ileaf){
	intLeaves[ileaf] = vLeaves[ileaf][i];
      }
      for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
	bBranches[ibranch].value = vBranches[ibranch][i].value;
	bBranches[ibranch].err = vBranches[ibranch][i].err;
	bBranches[ibranch].rms = vBranches[ibranch][i].rms;
	bBranches[ibranch].n = vBranches[ibranch][i].n;
      }
      new_tree->Fill();
    }
  }
  new_tree->Print();
  new_tree->Write("",TObject::kOverwrite);
  //  new_tree->AutoSave();
  db_file->Close();
  reduced_file->Close();
  corrected_file->Close();
  if(average_file)
    average_file->Close();
  new_file->Close();
  delete db_file;
  delete reduced_file;
  delete corrected_file;
  delete average_file;
  delete new_file;
  //////////////////////////////////////////////////////////


  return 0;

}
