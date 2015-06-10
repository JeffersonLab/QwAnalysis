#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TChain.h"
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
const Int_t nMON = 5, nDET = 100;
using namespace std;
Int_t RoundToNearestWhole(double num)
{
  int val = ((int(num + 0.5) > int(num)) ? int(num) + 1 : int(num));
  return val;
}

Int_t GetRunRanges(vector<vector<Int_t> > range, const char *name, bool is_run1){
  range.resize(2);
  ifstream file(name);
  if(!(file.good() && file.is_open())){
    cout<<"Run range file missing.\n"; 
    return -1;
  }
  int lrun = 0, hrun = 0, prev_lrun = 0, prev_hrun = 0;
  while(!file.eof()){
    file>>lrun>>hrun;
    if(lrun<=prev_lrun || lrun<=prev_hrun || hrun<lrun ){
      cout<<"Issue reading run ranges. lrun: "<<lrun<<"  hrun: "<<hrun<<
	"  prev_lrun: "<<prev_lrun<<"  prev_hrun: "<<prev_hrun<<endl; 
      return -1;
    }
    if((is_run1 && hrun<13000) || (!is_run1 && lrun>13000)){
      range[0].push_back(lrun);
      range[1].push_back(hrun);
      //      cout<<lrun<<"  "<<hrun<<endl;
    }

    file.peek();
  }
  file.close();
  return 0;
}

Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList, 
				 Bool_t trunc, char *config){
  string str;
  ifstream file(config);

  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug",Form("%s/mps_only_friend_13993.root",
				gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, str);
    monitorList[i] = TString(monitr);
    if(!ch->GetBranch(monitorList[i].Data())){
      cout<<monitorList[i].Data()<<" "<<ch->GetEntries()<<" missing. Exiting.\n";
      return -1;
    }else{
      if( trunc && monitorList[i].Contains("qwk_")){
	monitorList[i].Replace(0,4,"");
      }
      if(!monitorList[i].Contains("diff"))
	monitorList[i].Prepend("diff_");

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
    if(!ch->GetBranch(detectorList[nDet].Data()) && 
       !detectorList[nDet].Contains("pmtavg")){
      cout<<detectorList[nDet].Data()<<" missing.\n";
    }else{
      if( trunc && detectorList[nDet].Contains("qwk_"))
	detectorList[nDet].Replace(0,4,"");
      detectorList[nDet].Prepend("asym_");
      cout<<detectorList[nDet].Data()<<endl;
      nDet++;
    }
    file.peek();
  }
  file.close();
  delete ch;
  return nDet;
}


Int_t MakeCorrectedDBrootfile(Int_t run_period = 1, TString stem = "")
{
  Bool_t debug = 0;
  TString correctedFileDir = "/net/data2/paschkelab2/reduced_slugfiles/";
  TString leafListFileName =  Form("%s/config/setup_mpsonly%s.config",
				   gSystem->Getenv("BMOD_SRC"), stem.Data());
  correctedFileDir += Form("run%i/", run_period);
  TString DBFileDir = Form("%s/run%i/", gSystem->Getenv("DB_ROOTFILES"),
			   run_period);
  TString run_range_filename = Form("%s/slopelists/run_ranges.dat", 
				     gSystem->Getenv("BMOD_OUT"));


  //get run ranges for correction slopes
  //////////////////////////////////////
  vector<vector<Int_t> >vRunRanges;
  vRunRanges.resize(2);
  ifstream run_range_file(run_range_filename.Data());
  if( !(run_range_file.is_open() && run_range_file.good()) ){
    cout<<"Run range file not found. Run the following command from slopeslists"
	<<" directory then rerun program:\n";
    cout<<"ls slopeList_asym_qwk_md1barsum.* |"
	<<" sed 's/slopeList_asym_qwk_md1barsum.//g'" 
	<<"|sed 's/-/  /g' |sed 's/.dat//g' > run_ranges.dat\n";
    return -1;
  }
  if(!(run_range_file.good() && run_range_file.is_open())){
    cout<<"Run range file missing. Exiting\n"; 
    return -1;
  }
  int lrun = 0, hrun = 0, prev_lrun = 0, prev_hrun = 0;
  while(!run_range_file.eof()){
    run_range_file>>lrun>>hrun;
    if(lrun<=prev_lrun || lrun<=prev_hrun || hrun<lrun ){
      cout<<"Issue reading run ranges. Exiting. lrun: "<<lrun<<"  hrun: "<<hrun<<
	"  prev_lrun: "<<prev_lrun<<"  prev_hrun: "<<prev_hrun<<endl; 
      return -1;
    }
    if((run_period==1 && hrun<13000) || (run_period==2 && lrun>13000)){
      vRunRanges[0].push_back(lrun);
       vRunRanges[1].push_back(hrun);
    }

    run_range_file.peek();
  }
  run_range_file.close();



  //get list of leaves
  ////////////////////
  TString monitor[nMON], detector[nDET];
  char * configFile = Form("%s/config/setup_mpsonly%s.config",
			   gSystem->Getenv("BMOD_SRC"), stem.Data());
  Int_t nDet = GetMonitorAndDetectorLists(monitor, detector,0, configFile);

  vector<TString> vLeafList;
  for(int i=0;i<nDet;++i){
    TString name = Form("corrected_%s", detector[i].Data());
    vLeafList.push_back(name);
  }
  //include uncorrelated monitor leaves if in monitor list
  for(int i=0;i<nMON;++i){
    if(monitor[i].Contains("diff_M"))
    vLeafList.push_back(monitor[i]);
  }
  int nLeaves = int(vLeafList.size());
  cout<<nLeaves<<" leaves found.\n";


  //get the DB rootfile
  /////////////////////
  TFile *DBfile = TFile::Open(Form("%s/run%i/HYDROGEN-CELL_off_tree.root", 
				   gSystem->Getenv("DB_ROOTFILES"),run_period));
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

  //Get average quartet level values from slugfiles for each runlet.
  //////////////////////////////////////////////////////////////////
  vector<vector<leaf_t> >vLeaf;
  vLeaf.resize(nLeaves);
  vector<Int_t>vRun, vRunlet, vSlug, vErrFlag, vSlopesExist, vMissing, vMissSlug;

  Int_t nErr = 0, nMiss = 0, range_idx = 0;

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

//     //check if slopes are expected to exist
    Bool_t slopesShouldExist = false;
    for(int i=range_idx;i<(int)vRunRanges[0].size();++i){
      if(run_number>=vRunRanges[0][i] && run_number<=vRunRanges[1][i]){
	slopesShouldExist = true;
	break;
      }
      if(vRunRanges[0][i]>run_number)break;
      ++range_idx;
    }

    //fill bad data entries with error value
    if(n==0 || !(slug_number>=42 && slug_number<=321) || !slopesShouldExist){
      //      cout<<run_number_decimal<<" "<<slopesShouldExist<<"  "<<range_idx<<"\n";
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
    ifstream rnltAvgFile(Form("%srunlet_averages/corrected_averages%i_%03i%s.dat",
			      correctedFileDir.Data(), run_number, rnlt, 
			      stem.Data()));
    if(!(rnltAvgFile.is_open() && rnltAvgFile.good())){
      cout<<Form("%srunlet_averages/corrected_averages%i_%03i%s.dat NOT found.\n",
		 correctedFileDir.Data(), run_number, rnlt, stem.Data());
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
      int rnlt = irunlet;
      vMissing.push_back(rnlt);
      vMissSlug.push_back(slug_number);
      ++nMiss;
      continue;
    }
    cout<<irunlet<<Form(":  corrected_averages%i_%03i%s.dat found.\n",
			run_number, rnlt, stem.Data());

    for(int ileaf=0;ileaf<nLeaves;++ileaf){
      if(rnltAvgFile.eof()){
	cout<<"Premature EOF. Exiting. \n";
	return -1;
      }
      TString name;
      leaf_t lf;
      rnltAvgFile>>name>>lf.value>>lf.err>>lf.rms>>lf.n;
      if(name!=vLeafList[ileaf]){
	cout<<"Wrong leaf list. "<<name.Data()<<"!="<<vLeafList[ileaf].Data()<<
	  ". Run "<<run_number<<" Runlet "<<rnlt<<" Entry "<<irunlet <<". Skipping.\n";
       	return -1;//break;
      }
      vLeaf[ileaf].push_back(lf);
    }
    int err = (TMath::Abs(vLeaf[0][irunlet].n-n)!=0  ? 1 : 0);
    vErrFlag.push_back(err);
    vSlopesExist.push_back(1);
    rnltAvgFile.close();
  }

  cout<<nErr<<" error entries. "<<nMiss<<" runlet average files missing.\n"; 
  cout<<"Size:" <<vErrFlag.size()<<endl;
  cout<<"Size:" <<vSlopesExist.size()<<endl;
  for(int i=0;i<int(vMissing.size());++i)
    cout<<vMissing.at(i)<<": "<<vMissSlug.at(i)<<" "<<
      vRun.at(vMissing.at(i))<<" "<<vRunlet.at(vMissing.at(i))<<endl;



//   //create new DB friend tree
//   ///////////////////////////
  TFile *newfile = new TFile(Form("%s/run%i/hydrogen_cell_corrected_tree%s.root",
				  gSystem->Getenv("DB_ROOTFILES"),
				  run_period, stem.Data()), "recreate");
  if(newfile==0){
    cout<<"Warning. New corrected tree file failed to open. Exiting.\n";
    return -1;
  }
  TTree *newTree = new TTree("corrected_tree", "corrected_tree"); 
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
