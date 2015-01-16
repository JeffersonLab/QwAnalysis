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
#include "math.h"
#include <iostream>
#include <fstream>
#include <utility>

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: Mar 6, 2014
//Program averages reduced slugfiles and outputs runlet averages to file. These
//files are read to produce a summary rootfile friendable to Wade's DB rootfiles.
//Program arguments are in entry numbers as taken from the DB rootfile, so 
//"start" is the entry number of the first runlet to average and "nRunlets" tells
//how many to average before stopping. For some unknown reason the program seems
//to run out of memory and crash if you run it for more than 40 runlets so I have 
//set the default value to 20. "run_period" is either 1 or 2. Choose the 
//regression type using set="std" or set="set11" for example. 
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


Int_t MakeRegressedRunletAverages(int start = 0,  int run_period = 1, 
				int nRunlets = 20, char *set = "std")
{
  Bool_t debug = 0;
  TString reducedFileDir = "/net/data2/paschkelab2/reduced_slugfiles/";
  TString leafListFileName = reducedFileDir + "regressed_leafList.txt";
  reducedFileDir += Form("run%i/", run_period);
  TString DBFileDir = Form("/net/data2/paschkelab2/DB_rootfiles/run%i/", 
			   run_period);


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

  Int_t run_number, slug_number, n;
  Double_t run_number_decimal;


  //Average quartet level values from slugfiles for each runlet.
  //////////////////////////////////////////////////////////////
  Double_t *leaf = new Double_t [nLeaves];

  Double_t *leaf_v = new Double_t[nLeaves];
  Double_t *leaf_e = new Double_t[nLeaves];
  Double_t *leaf_r = new Double_t[nLeaves];
  Int_t *leaf_n = new Int_t[nLeaves];

  vector<Int_t>vRun, vRunlet, vSlug, vErrFlag, vCorrected;
  DBtree->GetEntry(0);
  TFile *reducedFile = 0;
  TTree *reducedTree = 0;  
  Int_t prev_slug = 0, nErr = 0;
  int end = (start+nRunlets <= DBtree->GetEntries() ? start+nRunlets
	     : DBtree->GetEntries());
  for(int irunlet = start;irunlet<end;++irunlet){
    //initialize array
    for(int i=0;i<nLeaves;++i){
      leaf_v[i] = 0;
      leaf_e[i] = 0;
      leaf_r[i] = 0;
      leaf_n[i] = 0;
    }
    cout<<1<< " ";
    DBtree->GetEntry(irunlet);
    double asym =(int)DBtree->GetBranch("asym_qwk_mdallbars")->GetLeaf("value")
      ->GetValue();
    n =(int)DBtree->GetBranch("asym_qwk_mdallbars")->GetLeaf("n")->GetValue();
    run_number =(int)DBtree->GetLeaf("run_number")->GetValue();
    run_number_decimal = DBtree->GetLeaf("run_number_decimal")->GetValue();
    slug_number =(int)DBtree->GetLeaf("slug")->GetValue();
    cout<<2<< " ";
    double rnlt_d = (run_number_decimal * 20) - run_number * 20;
    int rnlt = RoundToNearestWhole(rnlt_d);
    cout<<3<< " ";
    if(debug)
      cout<<slug_number<<" "<<Form("%0.2f  %i",run_number_decimal, rnlt)
	<<" "<<asym<<endl;
    vSlug.push_back(slug_number);
    vRun.push_back(run_number);
    cout<<4<< " ";
    vRunlet.push_back(rnlt);
    ofstream file(Form("%srunlet_averages/%s_regressed_averages%i_%03i.dat",
		       reducedFileDir.Data(), set, run_number, rnlt));
    printf("%srunlet_averages/%s_reg_regressed_averages%i_%03i.dat",
	   reducedFileDir.Data(), set, run_number, rnlt);
    //fill bad data entries with error value
    if(n==0 || !(slug_number>=42 && slug_number<=321)){
      for(int ileaf=0;ileaf<nLeaves;++ileaf){
	leaf_v[ileaf] = ERROR;
	leaf_e[ileaf] = ERROR;
	leaf_r[ileaf] = ERROR;
	leaf_n[ileaf] = 0;
	file<<vLeafList[ileaf].Data()<<"  "<<Form("%18.12e",leaf_v[ileaf])
	    <<"  "<<Form("%18.12e",leaf_e[ileaf])
	    <<"  "<<Form("%18.12e",leaf_r[ileaf])
	    <<"  "<<leaf_n[ileaf]<<endl;
      }
      vErrFlag.push_back(1);
      ++nErr;
      prev_slug = slug_number;
      file.close();
      continue;
    }
    cout<<5<< " ";

    //load reduced slugfile
    if(slug_number!=prev_slug){
      reducedFile = TFile::Open(Form("%s%s_reg_reduced_slug%i.root", 
				     reducedFileDir.Data(),set,
				     slug_number));

      if(reducedFile==0){
	cout<<"Regressed slugfile not found. Exiting.\n";
	//	return -1;
      }
      reducedTree = (TTree*)reducedFile->Get("slug");
    }
    cout<<6<< " ";

    //set the eventlist to the proper runlet
    reducedTree->Draw(">>list",Form("run==%i&&runlet==%i",run_number, rnlt));
    TEventList *list  = (TEventList*)gDirectory->Get("list");
    if(list->GetN()<=0){
      cout<<"Run "<<run_number<<" runlet "<<rnlt<<" not found."<<endl;
      for(int ileaf=0;ileaf<nLeaves;++ileaf){
	leaf_v[ileaf] = ERROR;
	leaf_e[ileaf] = ERROR;
	leaf_r[ileaf] = ERROR;
	leaf_n[ileaf] = 0;
	file<<vLeafList[ileaf].Data()<<"  "<<Form("%18.12e",leaf_v[ileaf])
	    <<"  "<<Form("%18.12e",leaf_e[ileaf])
	    <<"  "<<Form("%18.12e",leaf_r[ileaf])
	    <<"  "<<leaf_n[ileaf]<<endl;
      }
      vErrFlag.push_back(1);
      ++nErr;
      prev_slug = slug_number;
      file.close();
      continue;
    }
    cout<<7<< " ";

    vErrFlag.push_back(0);


    //set number of entries
    int N = list->GetN();
    for(int ileaf=0;ileaf<nLeaves;++ileaf){
      leaf_n[ileaf] = N;
    }

    cout<<8<< " ";

    //set branch addresses for reduced tree
    ///////////////////////////////////////
    for(int ileaf=0;ileaf<nLeaves;++ileaf)
      reducedTree->SetBranchAddress(vLeafList[ileaf].Data(),&leaf[ileaf]);
//       cout<<ileaf+1<<" "<<vLeafList[ileaf].Data()<<": "<<
// 	reducedTree->GetBranchStatus(vLeafList[ileaf].Data())<<endl;

    cout<<9<< " ";

    //average reduced slug file over runlet
    for(int i=0;i<N;++i){
      reducedTree->GetEntry(list->GetEntry(i));

      for(int ileaf=0;ileaf<nLeaves;++ileaf){
	//leaf[ileaf] = reducedTree->GetLeaf(vLeafList[ileaf].Data())->GetValue();
	leaf_v[ileaf] += leaf[ileaf]/ double(N);
	leaf_r[ileaf] += pow(leaf[ileaf], 2) / double(N) ;
      }
    }
    cout<<10<< "\n ";
    list->Clear();
    for(int ileaf=0;ileaf<nLeaves;++ileaf){
      if(N==1){
	leaf_r[ileaf] = 0;
	leaf_e[ileaf] = 0;
      }else{
	leaf_r[ileaf] -= pow(leaf_v[ileaf], 2);
	leaf_r[ileaf] = pow(leaf_r[ileaf], 0.5);
	leaf_e[ileaf] = leaf_r[ileaf] / sqrt(double(N));
      }
      file<<vLeafList[ileaf].Data()<<"  "<<Form("%18.12e",leaf_v[ileaf])
	  <<"  "<<Form("%18.12e",leaf_e[ileaf])
	  <<"  "<<Form("%18.12e",leaf_r[ileaf])
	  <<"  "<<leaf_n[ileaf]<<endl;
      if(vLeafList[ileaf].Contains("asym_qwk_mdallbars"))
	cout<<vLeafList[ileaf].Data()<<": "<<Form("%0.2f", run_number_decimal)<<
	  ": "<<leaf_v[ileaf]<<
	  "  "<<leaf_r[ileaf]<<"  "<<leaf_n[ileaf]<<"\n";
    }
    prev_slug = slug_number;
    file.close();
  }

  return 0;
}

/////////////////////////////////////////////////////////////////
//To compile at the command line                               //
//g++ -Wall `root-config --ldflags --libs --cflags` -O0        //
//MakeRegressedRunletAverages.C -o MakeRegressedRunletAverages //
/////////////////////////////////////////////////////////////////

Int_t main(int argc, char *argv[]){
  if (argc < 1 || argc > 4){
    std::cout<<"Usage:MakeRegressedRunletAverages(int start, int run_period, int runlets)"
	<<std::endl;
    return 0;
  }else if (argc==4){
    MakeRegressedRunletAverages(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]) );
  }else if (argc==3){
    MakeRegressedRunletAverages(atoi(argv[1]),atoi(argv[2]));
  }else if (argc==2){
    MakeRegressedRunletAverages(atoi(argv[1]));
  }else{
    MakeRegressedRunletAverages();
  }
  return 1;
}



