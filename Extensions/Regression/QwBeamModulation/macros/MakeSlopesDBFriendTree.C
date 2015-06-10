#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TDirectory.h"

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: April 8, 2014
//Program gets average slopes from files to produce a slopes summary rootfile  
//friendable to Wade's DB rootfiles.
//
//**********************************************************************
//Run this using script MakeSlopesDBFriendTree.sh which will produce the 
//requisite run range list before running this program.
//**********************************************************************
///////////////////////////////////////////////////////////////////////////////

struct leaf_t{
  double value;
  double err;
  double rms;
  int n;
};

//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 100, nMON = 5, nMOD = 5, nCOIL = 10;
const Double_t  ERROR = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////

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


Int_t MakeSlopesDBFriendTree(int run_period = 1, TString stem = "", 
			     Bool_t add_pmtavg = 0){

  //Declare needed variables
  /////////////////////////////////////////////////////////

  Bool_t debug = 0;
  Int_t nDet;
  TString MonitorList[nMOD], DetectorList[nDET];
  TString MonitorListFull[nMOD], DetectorListFull[nDET];
  /////////////////////////////////////////////////////////


  //Get list of run ranges
  /////////////////////////////////////////////////////////
  string line, match = "asym_qwk_charge.";
  vector<vector<Int_t> >vRunRange;
  vRunRange.resize(2);
  ifstream runRangeFile(Form("%s%s/slopelists/run_ranges.dat", 
			     gSystem->Getenv("BMOD_OUT"),stem.Data()));
  if(!(runRangeFile.is_open()&&runRangeFile.good())){
    cout<<"Run range file not found. Run the following command from slopeslists"
	<<" directory then rerun program:\n";
    cout<<"ls slopeList_asym_qwk_md1barsum.* |"
	<<" sed 's/slopeList_asym_qwk_md1barsum.//g'" 
	<<"|sed 's/-/  /g' |sed 's/.dat//g' > run_ranges.dat\n";
    return -1;
  }else{
    printf("Run ranges file found: %s%s/slopelists/"
	   "run_ranges.dat\n", gSystem->Getenv("BMOD_OUT"), stem.Data());
  }
  while(!runRangeFile.eof()){
    int start_run, end_run;
    runRangeFile>>start_run>>end_run;
    vRunRange[0].push_back(start_run);
    cout<<"Run range: "<<start_run;
    vRunRange[1].push_back(end_run);
    cout<<" "<<end_run<<endl;;
    runRangeFile.peek();
  }
  /////////////////////////////////////////////////////////



  //Get lists of monitors and detectors
  /////////////////////////////////////////////////////////
  TChain *ch = new TChain("mps_slug");
  char * configFile = Form("%s/config/setup_mpsonly%s.config",
			   gSystem->Getenv("BMOD_SRC"), stem.Data());
  nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList,1, configFile);  
  GetMonitorAndDetectorLists(MonitorListFull, DetectorListFull,0, configFile);  
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  if(debug)std::cout<<"(4)Working.\n"; 
  if(add_pmtavg){
    DetectorList[nDet] = "mdallpmtavg";
    DetectorListFull[nDet] = "mdallpmtavg";
    ++nDet;
  }
  delete ch;
  /////////////////////////////////////////////////////////


  //get the DB rootfile
  /////////////////////////////////////////////////////////

  TFile *DBfile = TFile::Open(Form("%s/run%i/HYDROGEN-CELL_off_tree.root", 
				   gSystem->Getenv("DB_ROOTFILES"),
				   run_period));
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
  /////////////////////////////////////////////////////////

  //Set needed branch addresses.
  /////////////////////////////////////////////////////////
  Int_t run_number;
  Double_t asym;
  DBtree->SetBranchAddress("run_number", &run_number);
  DBtree->SetBranchAddress("asym_qwk_mdallbars", &asym);
  /////////////////////////////////////////////////////////


  //Create a new file for tree
  /////////////////////////////////////////////////////////
  TString slopesFileName = Form("%s/run%i/hydrogen_cell_bmod_slopes_tree"
				"%s.root", gSystem->Getenv("DB_ROOTFILES"),
				run_period, stem.Data());
  TFile *slopesFile = new TFile(slopesFileName.Data(),"recreate");
  if(!slopesFile){
    cout<<"Slopes tree file failed to be created. Exiting.\n";
    return -1;
  }
  gSystem->cd(Form("%s/run%i",gSystem->Getenv("DB_ROOTFILES"), run_period));
  TTree *slopesTree = new TTree("slopes", "slopes"); 
  /////////////////////////////////////////////////////////


  //Create new slopes tree and its branches
  /////////////////////////////////////////////////////////
  Int_t lRun, hRun, slopes_exist;
  Double_t **slopes, **slopesErr, run_num_dec;
  slopes = new Double_t *[nDET];
  slopesErr = new Double_t *[nDET];
  for(int idet=0;idet<nDET;++idet){
    slopes[idet] = new Double_t [nMON];
    slopesErr[idet] = new Double_t [nMON];
  }
  TBranch *brRunlet = slopesTree->Branch("run_number_decimal", 
					 &run_num_dec, 
					 "run_number_decimal/D");

  TBranch *brSlopesExist = slopesTree->Branch("slopes_exist", &slopes_exist,
					   "slopes_exist/I");
  TBranch *brFirstRun = slopesTree->Branch("first_run", &lRun, "first_run/I");
  TBranch *brLastRun = slopesTree->Branch("last_run", &hRun, "last_run/I");
  TBranch *brSlopes[nDET][nMOD];
  TBranch *brSlopesErr[nDET][nMOD];
  for(int idet=0;idet<nDet;idet++){
    char *dname = (char*)DetectorList[idet].Data();
    for(int imon=0;imon<nMON;imon++){
      char *mname = (char*)MonitorList[imon].Data();
      brSlopes[idet][imon] = slopesTree->Branch(Form("%s_%s",dname, mname), 
					     &slopes[idet][imon],
					     Form("%s_%s/D",dname, mname));	
      brSlopesErr[idet][imon] = slopesTree->Branch(Form("%s_%s_err",dname, mname),
						&slopesErr[idet][imon],
						Form("%s_%s_err/D",dname, mname));
    }
  }

  /////////////////////////////////////////////////////////



  //Get slopes from files and create tree.
  /////////////////////////////////////////////////////////
  Int_t irange = 0, nNotFound = 0, nFound = 0;

  //   for(int irnlt=280;irnlt<480;++irnlt){
  for(int irnlt=0;irnlt<DBtree->GetEntries();++irnlt){
    if(irnlt%1000==0)cout<<"Processing entry "<<irnlt<<endl;
    DBtree->GetEntry(irnlt);
    run_num_dec = DBtree->GetLeaf("run_number_decimal")->GetValue();
    while(vRunRange[1][irange]<run_number&&irange<(int)vRunRange[0].size()-1){
      ++irange;
    }
    Bool_t run_range_found = run_number >= vRunRange[0][irange] &&
                             run_number <= vRunRange[1][irange];
    if(!run_range_found){
      cout<<vRunRange[1][irange-1]<<" "<<vRunRange[0][irange]<<"   "<<run_number<<"   "<<vRunRange[1][irange]<<endl;
      ++nNotFound;
      lRun = (int)ERROR;
      hRun = (int)ERROR;
      slopes_exist = 0;
      for(int idet=0;idet<nDet;idet++){
	for(int imon=0;imon<nMON;++imon){
	  slopes[idet][imon] = ERROR;
	  slopesErr[idet][imon] = ERROR;
	}
      }

    }else{
      ++nFound;
      slopes_exist = 1;
      lRun = vRunRange[0][irange];
      hRun = vRunRange[1][irange];
      for(int idet=0;idet<nDet;idet++){
	char *file_name = (Form("%s%s/slopelists/"
				"slopeList_asym_%s.%i-%i.dat",
				gSystem->Getenv("BMOD_OUT"), stem.Data(),
			 DetectorListFull[idet].Data(), 
			 vRunRange[0][irange], vRunRange[1][irange]));
	ifstream sl_file(file_name);
	if(!(sl_file.is_open()&&sl_file.good())){
	  cout<<"Slopes file "<<file_name<<" not found. Exiting.\n";
	    return -1;
	}
	string ln;
	getline(sl_file, ln);
	int num_monitors = 0;
	for(int imon=0;imon<nMON;++imon){
	  double slp, err;
	  TString name;
	  sl_file>>name>>slp>>err;
	  getline(sl_file, ln);
	  //	  cout<<slp<<" "<<err<<endl;
	  int idx;
	  for(idx=0; idx<nMON; ++idx){
	    if(MonitorList[idx].Contains(name.Data())){
	      ++num_monitors;
	      break;
	    }
	  }
	  slopes[idet][idx] = slp;
	  slopesErr[idet][idx] = err;
// 	  if(DetectorList[idet].Contains("mdallbars"))
// 	    cout<<run_num_dec<<": "<<DetectorList[idet].Data()<<"_"
// 		<<MonitorList[idx].Data()<<"  "<<slopes[idet][idx]
// 		<<" "<<slopesErr[idet][idx]<<endl;
	}
	if(num_monitors != nMON){
	  cout<<"Only "<<num_monitors<<" monitors found. Exiting.\n";
	  return -1;
	}
      }
    }
    slopesTree->Fill();
  }
  cout<<"Found "<<nFound<<" runlets. "<<nNotFound<<" runlets not found.\n";
  slopesFile = slopesTree->GetCurrentFile();
  slopesFile->Write("",TObject::kOverwrite);
  cout<<slopesTree->GetEntries()<<" entries"<<endl;
  cout<<"Writing "<<slopesFileName.Data()<<endl;
  slopesFile->Close();
  return 0;
}
