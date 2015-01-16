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
//Program averages reduced slugfiles to produce a summary rootfile friendable 
//to Wade's DB rootfiles.
///////////////////////////////////////////////////////////////////////////////

typedef struct leaf_t{
  double value;
  double err;
  double rms;
  int n;
};

const Double_t ERROR = -1.0e-6;
using namespace std;
Int_t RoundToNearestWhole(double num)
{
  int val = ((int(num + 0.5) > int(num)) ? int(num) + 1 : int(num));
  return val;
}


Int_t MakeCorrectedRunletAverages(TString stem = "", int start = 0,  
				  int run_period = 1, int nRunlets = 20)
{
  Bool_t debug = 0;
  TString correctedFileDir = Form("/net/data2/paschkelab2/reduced_slugfiles/");
  TString leafListFileName = correctedFileDir + 
    Form("corrected_leafList%s.txt", stem.Data());
  correctedFileDir += Form("run%i/", run_period);
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
  if(stem.Contains("new_monitors")){
    vLeafList.push_back("diff_MX1");
    vLeafList.push_back("diff_MX2");
    vLeafList.push_back("diff_MY1");
    vLeafList.push_back("diff_MY2");
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
  TFile *correctedFile = 0;
  TFile *reducedFile = 0;
  TTree *correctedTree = 0;  
  Int_t prev_slug = 0, nErr = 0;
  int end = (start + nRunlets <= DBtree->GetEntries() ? start + nRunlets : 
	     DBtree->GetEntries());
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
    ofstream file(Form("%srunlet_averages/corrected_averages%i_%03i%s.dat",
		       correctedFileDir.Data(), run_number, rnlt, stem.Data()));
    printf("%srunlet_averages/corrected_averages%i_%03i%s.dat  ",
	   correctedFileDir.Data(), run_number, rnlt, stem.Data());

    int slopes_exist = 0;
    //fill bad data entries with error value
    if(n==0 || !(slug_number>=42 && slug_number<=321)){
      file<<"slopes_exist"<<"  "<<slopes_exist<<endl;
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
      file.close();
      prev_slug = slug_number;
      continue;
    }
    cout<<5<< " ";

    //load corrected slugfile
    if(slug_number!=prev_slug){
      correctedFile = TFile::Open(Form("%scorrected_slug%i%s.root",
				       correctedFileDir.Data(), slug_number,
				       stem.Data()));
      reducedFile = TFile::Open(Form("%sreduced_slug%i.root",
				     correctedFileDir.Data(), slug_number));
      if(correctedFile==0){
	cout<<"Corrected slugfile not found. \n";
	//advance to next slug
	for(irunlet = irunlet+1; irunlet<DBtree->GetEntries();++irunlet){
	  DBtree->GetEntry(irunlet);
	  if(slug_number!=prev_slug)
	    break;
	  prev_slug = slug_number;
	}
	irunlet--;
      }else{
	correctedTree = (TTree*)correctedFile->Get("corrected_tree");
      }
    }
    cout<<6<< " ";


    //set the eventlist to the proper runlet
    correctedTree->AddFriend("slug", reducedFile);
    correctedTree->Draw(">>list",Form("run==%i&&runlet==%i",run_number, rnlt));
    TEventList *list  = (TEventList*)gDirectory->Get("list");
    if(list->GetN()<=0){
      cout<<"Run "<<run_number<<" runlet "<<rnlt<<" not found."<<endl;
      file<<"slopes_exist"<<"  "<<0<<endl;
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
      vCorrected.push_back(0);
      ++nErr;
      file.close();
      prev_slug = slug_number;
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

    //set branch addresses for corrected tree
    ///////////////////////////////////////
    correctedTree->SetBranchAddress("slopes_exist", &slopes_exist);
    for(int ileaf=0;ileaf<nLeaves;++ileaf)
      correctedTree->SetBranchAddress(vLeafList[ileaf].Data(),&leaf[ileaf]);
//       cout<<ileaf+1<<" "<<vLeafList[ileaf].Data()<<": "<<
// 	correctedTree->GetBranchStatus(vLeafList[ileaf].Data())<<endl;

    cout<<9<< " ";
    correctedTree->GetEntry(list->GetEntry(0));
    vCorrected.push_back(slopes_exist);
    //average corrected slug file over runlet
    for(int i=0;i<N;++i){
      correctedTree->GetEntry(list->GetEntry(i));

      for(int ileaf=0;ileaf<nLeaves;++ileaf){
	leaf_v[ileaf] += leaf[ileaf]/ double(N);
	leaf_r[ileaf] += pow(leaf[ileaf], 2) / double(N) ;
      }
    }
    cout<<10<< "\n ";
    list->Clear();
    file<<"slopes_exist"<<"  "<<slopes_exist<<endl;
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
	cout<<vLeafList[ileaf].Data()<<": "<<Form("%0.2f",run_number_decimal)<<
	  ": "<<leaf_v[ileaf]<<
	  "  "<<leaf_r[ileaf]<<"  "<<leaf_n[ileaf]<<"\n";
    }
    file.close();
    prev_slug = slug_number;
  }

  return 0;
}

/////////////////////////////////////////////////////////////
//To compile at the command line                           //
//g++ -Wall `root-config --ldflags --libs --cflags` -O0    //
//MakeCorrectedRunletAverages.C -o MakeCorrectedRunletAverages //
/////////////////////////////////////////////////////////////

Int_t main(int argc, char *argv[]){
  if (argc < 1 || argc > 4){
    std::cout<<"Usage:MakeCorrectedRunletAverages(int start,"<<
      " int run_period, int runlets)"<<std::endl;
    return 0;
  }else if (argc==4){
    MakeCorrectedRunletAverages(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]) );
  }else if (argc==3){
    MakeCorrectedRunletAverages(atoi(argv[1]),atoi(argv[2]));
  }else if (argc==2){
    MakeCorrectedRunletAverages(atoi(argv[1]));
  }else{
    MakeCorrectedRunletAverages();
  }
  return 1;
}

