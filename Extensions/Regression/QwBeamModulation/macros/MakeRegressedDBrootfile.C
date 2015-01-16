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

#include <vector>
#include <iostream>
#include <fstream>
#include <utility>

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: Mar 6, 2014
//Program averages reduced slugfiles to produce a runlet summary rootfile 
//friendable to Wade's DB rootfiles.
///////////////////////////////////////////////////////////////////////////////

typedef struct leaf_t{
  double value;
  double err;
  double rms;
  int n;
};

const Double_t ERROR = -1.0e6;
using namespace std;
Int_t RoundToNearestWhole(double num)
{
  int val = ((int(num + 0.5) > int(num)) ? int(num) + 1 : int(num));
  return val;
}


Int_t MakeRegressedDBrootfile(Int_t run_period = 1, TString set = "std")
{
  Bool_t debug = 0;
  TString listsDir = "/home/ek4px/lists/detectorLists/";
  TString reducedFileDir = Form("/net/data2/paschkelab2/reduced_slugfiles/");
  TString leafListFileName = reducedFileDir + "regressed_leafList.txt";
  reducedFileDir += Form("run%i/", run_period);
  TString DBFileDir = Form("/net/data2/paschkelab2/DB_rootfiles/run%i/", run_period);

  //get list of leaves
  ////////////////////
  vector<TString> vLeafList;
  ifstream leafListFile(leafListFileName.Data());
  if(!(leafListFile.is_open() && leafListFile.good())){
    cout<<"Leaf list file not found. Exiting.\n";
    return -1;
  }

  while(!leafListFile.eof()){
    string line;
    getline(leafListFile, line);
    vLeafList.push_back(line);
    leafListFile.peek();
    //    cout<<vLeafList.back().Data()<<endl;
  }
  int nLeaves = int(vLeafList.size());
  cout<<nLeaves<<" leaves found.\n";


  //get the DB rootfile
  /////////////////////
  TFile *DBfile = TFile::Open(Form("/net/data2/paschkelab2/DB_rootfiles/run%i/"
				   "HYDROGEN-CELL_off_tree.root", run_period));
  if(DBfile==0){
    cout<<"DB rootfile not found. Exiting.\n";
    return -1;
  }
  TTree *DBtree = (TTree*)DBfile->Get("tree");
  if(DBtree->GetEntries()==0){
    cout<<"DBtree empty. Exiting.\n";
    return -1;
  }
  cout<<DBtree->GetEntries()<<" entries found in DB rootfile.\n";

  //Set needed branch addresses.
  //////////////////////////////
  Int_t run_number, slug_number;
  Int_t ihwp;
  Double_t run_number_decimal, asym, asym1, rdec;
  DBtree->SetBranchAddress("slug", &slug_number);
  DBtree->SetBranchAddress("ihwp_setting", &ihwp);
  DBtree->SetBranchAddress("run_number", &run_number);
  DBtree->SetBranchAddress("asym_qwk_mdallbars", &asym);
  DBtree->SetBranchAddress("asym_qwk_md1barsum", &asym1);
  DBtree->SetBranchAddress("run_number_decimal", &run_number_decimal);
  //A bug in ROOT requires me to add this superfluous line to get 
  //run_number_decimal to stop misreading
  DBtree->SetBranchAddress("run_number_decimal", &rdec);


  //Average quartet level values from slugfiles for each runlet.
  //////////////////////////////////////////////////////////////
  vector<vector<leaf_t> >vLeaf;
  vLeaf.resize(nLeaves);
  vector<Int_t>vRun, vRunlet, vSlug, vErrFlag, vSlopesExist, vMissing, vMissSlug;

  Int_t nErr = 0, nMiss = 0;

  for(int irunlet=0;irunlet<DBtree->GetEntries();++irunlet){
    DBtree->GetEntry(irunlet);
    //A bug in ROOT requires me to add this superfluous line to get 
    //run_number_decimal to stop misreading
    int n =(int)DBtree->GetBranch("asym_qwk_mdallbars")->GetLeaf("n")->GetValue();
    run_number_decimal = DBtree->GetLeaf("run_number_decimal")->GetValue();
    double rnlt_d = (run_number_decimal * 20) - run_number * 20;
    int rnlt = RoundToNearestWhole(rnlt_d);
    if(debug)
      cout<<slug_number<<" "<<Form("%0.2f  %i",run_number_decimal, rnlt)
	<<" "<<asym<<endl;
    vSlug.push_back(slug_number);
    vRun.push_back(run_number);
    vRunlet.push_back(rnlt);

    //fill bad data entries with error value
    if(n==0 || !(slug_number>=42 && slug_number<=321)){
      for(int ileaf=0;ileaf<nLeaves;++ileaf){
	leaf_t lf;
	lf.value = ERROR;
	lf.err = ERROR;
	lf.rms = ERROR;
	lf.n = 0;
	vLeaf[ileaf].push_back(lf);
      }
      vErrFlag.push_back(1);
      vSlopesExist.push_back(0);

      ++nErr;
      continue;
    }
    ifstream rnltAvgFile(Form("%srunlet_averages/%s_regressed_averages%i_%03i.dat",			      reducedFileDir.Data(), set.Data(), run_number,rnlt));
    if(!(rnltAvgFile.is_open() && rnltAvgFile.good())){
      cout<<Form("%srunlet_averages/%s_regressed_averages%i_%03i.dat NOT found.",
		 reducedFileDir.Data(), set.Data(), run_number, rnlt)<<endl;
      for(int ileaf=0;ileaf<nLeaves;++ileaf){ 
	leaf_t lf;
	lf.value = ERROR;
	lf.err = ERROR;
	lf.rms = ERROR;
	lf.n = 0;
	vLeaf[ileaf].push_back(lf);
      }
      vErrFlag.push_back(1);
      vSlopesExist.push_back(0);
      vMissing.push_back(irunlet);
      vMissSlug.push_back(slug_number);
      ++nMiss;
      continue;
    }
    cout<<irunlet<<Form(":  %s_regressed_averages%i_%03i.dat found.\n",
			set.Data(), run_number, rnlt);
    for(int ileaf=0;ileaf<nLeaves;++ileaf){
      if(rnltAvgFile.eof()){
	cout<<"Premature EOF. Exiting.\n";
	return -1;
      }
      TString name, vl, er, rm;
      leaf_t lf;
      rnltAvgFile>>name>>vl>>er>>rm>>lf.n;
      //if "nan" is encountered leave ERROR values
      if(vl.CompareTo("nan")!=0&&rm.CompareTo("nan")&&er.CompareTo("nan")){
	lf.value = atof(vl.Data());
	lf.err = atof(er.Data());
	lf.rms = atof(rm.Data());
      }
      //      rnltAvgFile>>name>>lf.value>>lf.err>>lf.rms>>lf.n;
      if(name!=vLeafList[ileaf]){
	cout<<"Wrong leaf list. "<<name.Data()<<"!="<<vLeafList[ileaf].Data()<<
	  ". Run "<<run_number<<" Runlet "<<rnlt<<". Skipping.\n";
       	return -1;//break;
      }
      vLeaf[ileaf].push_back(lf);
    }
    int err = (TMath::Abs(vLeaf[0][irunlet].n-n)!=0  ? 1 : 0);
    vErrFlag.push_back(err);
    rnltAvgFile.close();
  }

  cout<<nErr<<" error entries. "<<nMiss<<" runlet average files missing.\n"; 
  cout<<"Size:" <<vErrFlag.size()<<endl;
  cout<<"Size:" <<vLeaf[0].size()<<endl;
  cout<<"Size:" <<vSlopesExist.size()<<endl;
  for(int i=0;i<int(vMissing.size());++i)
    cout<<vMissing.at(i)<<": "<<vMissSlug[i]<<"  "<<vRun.at(vMissing.at(i))<<" "<<
      vRunlet.at(vMissing.at(i))<<endl;


//   //create new DB friend tree
//   ///////////////////////////
  TFile *newfile = new TFile(Form("/net/data2/paschkelab2/DB_rootfiles/run%i/"
				  "hydrogen_cell_%s_regressed_tree.root",
				  run_period, set.Data()),
			     "recreate");
  if(newfile==0){
    cout<<"Warning. New corrected tree file failed to open. Exiting.\n";
    return -1;
  }
  TTree *newTree = new TTree(Form("%s_reg_tree", set.Data()), 
			     Form("%s_reg_tree", set.Data())); 
  Int_t slug, runlet, run, good, slopes_exist;
  TBranch *brGood = newTree->Branch("good", &good, "good/I");
  TBranch *brSlopesExist = newTree->Branch("slopes_exist", &slopes_exist,
					   "slopes_exist/I");
  TBranch *brRun = newTree->Branch("run", &run, "run/I");
  TBranch *brRunlet = newTree->Branch("runlet", &runlet, "runlet/I");
  TBranch *brSlug = newTree->Branch("slug", &slug, "slug/I");
  leaf_t *newLeaf = new leaf_t[nLeaves];
  TBranch **brList = new TBranch *[nLeaves];
  for(int ileaf=0;ileaf<nLeaves;++ileaf){
    brList[ileaf] = newTree->Branch(vLeafList[ileaf].Data(),&newLeaf[ileaf],
			      "value/D:err/D:rms/D:n/I");
  }
  for(int irunlet=0;irunlet<(int)vLeaf[0].size();++irunlet){
    slug = vSlug[irunlet];
    run = vRun[irunlet];
    runlet = vRunlet[irunlet];
    slopes_exist = vSlopesExist[irunlet];
    good = (vErrFlag[irunlet]==1 ? 0 : 1);
    for(int ileaf=0;ileaf<nLeaves;++ileaf){
      double unit_convert = 1.0;
      if(vLeafList[ileaf].Contains("asym") || vLeafList[ileaf].Contains("diff"))
	unit_convert = 1.0e6;
      if(vLeaf[ileaf][irunlet].n == 0){
	newLeaf[ileaf].value = ERROR;
	newLeaf[ileaf].err = ERROR;
	newLeaf[ileaf].rms = ERROR;
	newLeaf[ileaf].n = 0;

      }else{
	newLeaf[ileaf].value = vLeaf[ileaf][irunlet].value * unit_convert;
	newLeaf[ileaf].err = vLeaf[ileaf][irunlet].err * unit_convert;
	newLeaf[ileaf].rms = vLeaf[ileaf][irunlet].rms * unit_convert;
	newLeaf[ileaf].n = vLeaf[ileaf][irunlet].n;
      }
    }
    newTree->Fill();
  }
  //  newTree->Print();
  newTree->Write("",TObject::kOverwrite);
  newfile->Close();
  delete newfile;

  return 0;
}
