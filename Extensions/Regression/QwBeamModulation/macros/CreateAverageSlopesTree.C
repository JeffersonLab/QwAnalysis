#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLeaf.h"
#include "TChain.h"
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
//Program averages  BMod slopes tree to produce slug or run average trees. 
//Errors are RMS/sqrt(N).
//
//Requires that $DB_ROOTFILES be set to the directory of the DB rootfiles.
//
//Usage: CreateAverageSlopesTree(TString filename, TString treename)
//run_period:  1 or 2
//    stem:    file indentifier Eg. "_compton_bpm" or "_new_monitors"
//treename:    name of tree in rootfile
//
//
///////////////////////////////////////////////////////////////////////////////

const Int_t nRUNS=20000, nSLUGS=325, nMON = 5, nMOD = 5, nDET = 100;
const Double_t ERROR = -1.0e6;

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

Int_t GetMonitorAndDetectorLists(TChain *ch, TString *monitorList, 
				 TString *detectorList, Bool_t trunc,
				 char * config){
  string str;
  ifstream file(config);

  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, str);
    monitorList[i] = TString(monitr);
    if(!ch->GetBranch(monitorList[i].Data())){
      cout<<monitorList[i].Data()<<" missing. Exiting.\n";
      return -1;
    }else{
      if( trunc && monitorList[i].Contains("qwk_"))
	monitorList[i].Replace(0,4,"");
      cout<<monitorList[i].Data()<<"\n";
    }
  }
  getline(file, str);
  Int_t nDet = 0;
  for(int i=0;i<nDET&&!file.eof();i++){
    char id[4] = "   ", detectr[20] = "                   ";
    file>>id>>detectr;
    getline(file, str);
    detectorList[nDet] = TString(detectr);
    if(!ch->GetBranch(detectorList[nDet].Data())){
      cout<<detectorList[nDet].Data()<<" missing.\n";
    }else{
      if( trunc && detectorList[nDet].Contains("qwk_"))
	detectorList[nDet].Replace(0,4,"");
      cout<<detectorList[nDet].Data()<<endl;
      nDet++;
    }
    file.peek();
  }
  file.close();
  return nDet;
}

Int_t CreateAverageSlopesTree(int run_period, TString stem, TString treename) 
{

  //Get tree to be averaged
  /////////////////////////////////////////////////////////
  char* output = Form("/net/data2/paschkelab2/DB_rootfiles/run%i", run_period);
  TFile *average_file = new TFile(Form("%s/hydrogen_cell_bmod_slopes_tree%s.root",
				       output,stem.Data()));
  TTree *average_tree = (TTree*)average_file->Get(treename.Data());
  //1 Tree from which to get runlist
  TFile *file = new TFile(Form("%s/%s",output,
			       "HYDROGEN-CELL_off_tree.root"));
  TTree *tree = (TTree*)file->Get("tree");
  /////////////////////////////////////////////////////////
  
  

  //Get lists of monitors and detectors
  /////////////////////////////////////////////////////////
  TString MonitorList[nMOD], DetectorList[nDET];
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_11405*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug",Form("%s/mps_only_friend_13993.root",
				gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  char* configFile = Form("%s/config/setup_mpsonly%s.config",
			  gSystem->Getenv("BMOD_SRC"), stem.Data());

  int nDet = GetMonitorAndDetectorLists(ch, MonitorList, DetectorList, 1, 
					configFile);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  delete ch;
  /////////////////////////////////////////////////////////
   
  //Create list of branches
  /////////////////////////////////////////////////////////
  vector<mytype_t> leafnames;
  for(int idet=0;idet<nDet;++idet){
    for(int imon=0;imon<nMON;++imon){
      mytype_t temp;
      temp.name = Form("%s_%s", DetectorList[idet].Data(), 
		       MonitorList[imon].Data());
      temp.isInt = 0;
      leafnames.push_back(temp);
      cout<<temp.name<<endl;
    }
  }
  /////////////////////////////////////////////////////////


  //Set up list of runs/slugs to be in tree.
  //In order to make all trees friendable we need a master list.
  vector<Int_t>vMasterList;
  for(int i=0;i<nSLUGS;++i){
    vMasterList.push_back(0);
  }
  Int_t rn, sl;
  tree->SetBranchAddress("run_number",&rn);
  tree->SetBranchAddress("slug",&sl);

  for(int i=0;i<tree->GetEntries();++i){
    tree->GetEntry(i);
    if(!(sl>=42&&sl<=321)) continue;
    if(tree->GetBranch("asym_qwk_mdallbars")->GetLeaf("n")->GetValue()>0
       && sl>=42 && sl <= 321){
      ++vMasterList[sl];
    }
  }
  //  tree->ResetBranchAddresses();
  vector<int>vSlug;
  int Idx[nSLUGS];
  for(int i=0;i<(int)vMasterList.size();++i){
    if(vMasterList[i]>0){
      Idx[i] = (int)vSlug.size();
      vSlug.push_back(i);
    }else Idx[i]=-1;
  }
  /////////////////////////////////////////////////////////////

  //Loop over tree filling vectors with slopes.
  //////////////////////////////////////////////////////////
  int slug;
  vector<vector<double> >vBranch_val, vBranch_err;
  vBranch_val.resize((int)leafnames.size());
  vBranch_err.resize((int)leafnames.size());
  for(int ibranch=0; ibranch<(int)leafnames.size();++ibranch){
    for(int i=0; i<(int)vSlug.size();++i){
      vBranch_val[ibranch].push_back(ERROR);
      vBranch_err[ibranch].push_back(ERROR);
    }
  }
  cout<<"size: "<<leafnames.size()<<endl;
  for(int i=0;i<average_tree->GetEntries();++i){
    average_tree->GetEntry(i);
    tree->GetEntry(i);
    if(i%1000==0) cout<<"Processing entry "<<i<<" of "<<tree->GetEntries()<<endl;
    if(Idx[sl] < 0 || average_tree->GetLeaf("slopes_exist")->GetValue() == 0) 
      continue;
    //       cout<<i<<" "<<average_tree->GetEntries()<<endl;
    for(int ibranch=0; ibranch<(int)leafnames.size();++ibranch){
      char *nm = (char*)leafnames[ibranch].name.Data();
      vBranch_val[ibranch][Idx[sl]] = average_tree->GetLeaf(nm)->GetValue();
      vBranch_err[ibranch][Idx[sl]] = average_tree->GetLeaf(Form("%s_err",nm))->GetValue();
    }
  }
  //////////////////////////////////////////////////////////


  //create new average tree
  //////////////////////////////////////////////////////////
  int slopes_exist = 0;
  output = Form("/net/data2/paschkelab2/DB_rootfiles/run%i", run_period);
  TFile *new_file = new TFile(Form("%s/slug_averaged_hydrogen_cell_bmod_slopes_tr"
				   "ee%s.root",output, stem.Data()),"recreate");
  TTree *new_tree = new TTree(treename.Data(),treename.Data());
  TBranch *brSlug = new_tree->Branch("slug", &slug,"slug/I"); 
  TBranch *brSlopesExist = new_tree->Branch("slopes_exist", &slopes_exist,
					    "slopes_exist/I"); 
  TBranch **branches = new TBranch*[int(leafnames.size())]; 
  double *branch_val = new double[(int)leafnames.size()];
  double *branch_err = new double[(int)leafnames.size()];

  for(int ibranch=0;ibranch<int(leafnames.size());++ibranch){
    char *nm = (char*)leafnames[ibranch].name.Data();
    branches[ibranch] = new_tree->Branch(nm, &branch_val[ibranch], 
					 Form("%s/D", nm));
    branches[ibranch] = new_tree->Branch(Form("%s_err",nm), &branch_err[ibranch], 
					 Form("%s_err/D", nm));
  }
  for(int i=0;i<(int)vSlug.size();++i){
    slug = vSlug[i];
    if(vBranch_val[0][i] != ERROR){
      slopes_exist = 1;
      for(int ibranch=0;ibranch<int(leafnames.size());++ibranch){
	branch_val[ibranch] = vBranch_val[ibranch][i];
	branch_err[ibranch] = vBranch_err[ibranch][i];
      }
    }else slopes_exist = 0;
    if(slopes_exist)//REMOVE THIS CONDITION TO CREATE TREE WITH ENTRIES HAVING 
                    //NO SLOPES
      new_tree->Fill();
  }
  //  new_tree->Print();
  new_tree->Write("",TObject::kOverwrite);
  //  new_tree->AutoSave();
  average_file->Close();
  new_file->Close();
  delete average_file;
  delete new_file;
  //////////////////////////////////////////////////////////


  return 0;

}
