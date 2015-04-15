#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

// Example of Root macro to copy a subset of a Tree to a new Tree
// Only selected entries are copied to the new Tree.

//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 60, nMON = 5, nMOD = 5, nCOIL = 10;
const Double_t  errorFlag = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////

struct Header_t{
  Int_t macrocycle;
  Int_t microcycle[nMOD];
  Int_t id;
  Int_t lEntry;
  Int_t hEntry;
};


Bool_t CheckFile(ifstream &file){
  return (file.is_open() && file.good() ? 1 : 0); 
}

Int_t ExtractHeader(ifstream &file, Header_t &header, Int_t size = 9){
  string str;
  char *correct_name[9] = {"Macrocycle", "Mod0", "Mod1","Mod2", "Mod3", "Mod4",
		     "Id", "first_entry", "last_entry"};
  Int_t *temp = new Int_t[size];
  for(int i=0;i<size;i++){
    char name[30]={0}, num[30]={0};
    file>>name>>num;
    if(strcmp(name, correct_name[i])==0){
      //      cout<<name<<"  "<<correct_name[i]<<endl;
      temp[i] = atoi(num);
    }else{
      cout<<"Unexpected header format. Exiting.\n";
      cout<<name<<"  "<<correct_name[i]<<endl;
      return -1;
    }
  }
  getline(file, str);
  Int_t idx = 0;
  header.macrocycle = temp[idx];
  idx++;
  for(int i=0;i<nMOD;i++){
    header.microcycle[i] = temp[idx];
    idx++;
  }
  header.id = temp[idx];
  idx++;
  header.lEntry = temp[idx];
  idx++;
  header.hEntry = temp[idx];
  return 0;
}

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

void makeAlgebraicNewMonitorSlopesTree( TString stem = "", Int_t run_start = 9939,
					Int_t  run_end = 19000, 
					Double_t fUnitConvertTargetSlope = 1.0e4,
					Int_t chisq = 1) {

  bool transverse = 0;
  //Declare needed variables
  /////////////////////////////////////////////////////////
  bool sine_only = 1;
  char* name = (char*)(chisq ? "_ChiSqMin.set0" : ".set0");  
  Bool_t debug = 0;
  string line;
  Int_t size = 9, nDet;
  Double_t slopesUnitConvert[5];
  TFile *newfile; 
  TTree *newTree = new TTree("slopes", "slopes"); 
  TString MonitorList[nMOD], DetectorList[nDET];
  /////////////////////////////////////////////////////////


  //Get lists of monitors and detectors
  /////////////////////////////////////////////////////////
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_11405*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug",Form("%s/mps_only_friend_13993.root",
				gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  char* configFile = Form("%s/config/setup_mpsonly%s.config",
			  gSystem->Getenv("BMOD_SRC"), stem.Data());

  nDet = GetMonitorAndDetectorLists(ch, MonitorList, DetectorList,0, configFile);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return;
  }
  if(debug)std::cout<<"(4)Working.\n"; 
  delete ch;
  /////////////////////////////////////////////////////////
   

  for(int imon = 0;imon<nMON;imon++){
    slopesUnitConvert[imon] = (MonitorList[imon].Contains("bpm") ? 1.0e3 : 1.0);
    cout<<"Unit coversion: "<<MonitorList[imon].Data()<<" "<<
      slopesUnitConvert[imon]<<endl;
  }

  //Get list of good runs.
  /////////////////////////////////////////////////////////
  ifstream runList(Form("%s/macros/runsBySlug%s.dat", 
			gSystem->Getenv("BMOD_SRC"),
	       (transverse ? "_transverse" : "")));
  if(runList.is_open())cout<<"File list found.\n";
  else{
    cout<<"File list not found.\n";
    return;
  }
  /////////////////////////////////////////////////////////

  //Get IHWP state by slug.
  /////////////////////////////////////////////////////////
  ifstream hwpList(Form("%s/macros/hwpBySlug.dat", 
			gSystem->Getenv("BMOD_SRC")));
  if(hwpList.is_open())cout<<"File list found.\n";
  else{
    cout<<"File list not found.\n";
    return;
  }
  vector<vector<Int_t> >vHWP;
  vHWP.resize(2);
  Int_t nslg = 321;
  for(int i=0;i<=nslg;++i){
    vHWP[0].push_back(i);
    vHWP[1].push_back(-1);
  }
  Int_t slug_num, hwp;
  hwpList>>slug_num>>hwp;
  while(!hwpList.eof()){
    vHWP[1][slug_num] = hwp;
    hwpList>>slug_num>>hwp;
  }
//   for(int i=0;i<=nslg;++i)
//     printf("%i: %i   %i\n", i, vHWP[0][i],vHWP[1][i]);
  /////////////////////////////////////////////////////////

  //Get scale factor for creating new monitors.
  /////////////////////////////////////////////////////////
  Double_t scale_x, scale_y;
  ifstream scale_file(Form("%s/config/new_monitors.config",
			   gSystem->Getenv("BMOD_SRC")));
  if(!(scale_file.is_open() && scale_file.good())){
    std::cout<<"New monitor scale factor file not found. Exiting.\n";
    return;
  }
  std::string temp_line;
  while(scale_file.peek()=='!')
    getline(scale_file, temp_line);
  char tmp[255], arun1[255], arun2[255], brun1[255], brun2[255];
  scale_file>>tmp>>arun1>>arun2;
  scale_file>>tmp>>brun1>>brun2;
  /////////////////////////////////////////////////////////

  //Initialize vectors for holding data from files.
  /////////////////////////////////////////////////////////
  Double_t macrocycle, cyclet[nMOD];
  vector<Double_t>  vRun, vSlug, vMacrocycle, vFirstEntry, vLastEntry, vId;
  vector< vector<Double_t> > vCyclet;
  vector< vector< vector<Double_t> > > vDetectorSlope, vDetectorSlopeError;

  vCyclet.resize(nMOD);
  vDetectorSlope.resize(nDet);
  vDetectorSlopeError.resize(nDet);

  for(int idet=0;idet<nDet;idet++){
    vDetectorSlope[idet].resize(nMON);
    vDetectorSlopeError[idet].resize(nMON);
  }

  /////////////////////////////////////////////////////////


  Double_t run = 0, slug = 0, nCycle = 0, nRuns = 0;
  vector<Double_t> temp;
  TString output_dir = Form("%s%s",gSystem->Getenv("BMOD_OUT"),stem.Data());
  cout<<"Output directory:  "<<output_dir.Data()<<endl;
  //Read information for all runs in runlist.
  /////////////////////////////////////////////////////////
  int tgX=-1, tgXP=-1, tgY=-1, tgYP=-1, eng=-1;
  for(int i=0;i<nMON;++i){
    if(MonitorList[i].Contains("targetXSlope")){
      tgXP = i;
    }else if(MonitorList[i].Contains("targetX")){
      tgX = i;
    }else if(MonitorList[i].Contains("targetYSlope")){
      tgYP = i;
    }else if(MonitorList[i].Contains("targetY")){
      tgY = i;
    }else if(MonitorList[i].Contains("bpm3c12X")){
      eng = i;
    }
  }
  if(tgX==-1 || tgXP==-1 || tgY==-1 || tgYP==-1 || eng==-1){
    cout<<"Error. Not all target indices found. Exiting.\n";
      return;
  }
  int mx1 = 0, mx2=1, my1=2, my2=3, energy = 4;
  MonitorList[mx1] = "MX1";
  MonitorList[mx2] = "MX2";
  MonitorList[my1] = "MY1";
  MonitorList[my2] = "MY2";
  MonitorList[energy] = "bpm3c12X";
  while(!runList.eof()&&runList.good()&&run<=run_end){
    char sl[30], rn[30];
    Bool_t all_found = 0;
    runList>>sl>>rn;
    run = atof(rn);
    cout<<"Run: "<<run<<endl;
    slug = atof(sl);
    if(run>=run_end)break;
    if(run>=run_start){
      scale_x = 1000 * (run < 13000 ? atof(arun1) : atof(arun2));
      scale_y = 1000 * (run < 13000 ? atof(brun1) : atof(brun2));
      //Open slopes files.
      /////////////////////////////////////////////////////////
      ifstream slopesFile(Form("%s/macrocycle_slopes/macrocycle_slopes_%i%s.dat",
			       output_dir.Data(), (int)run, name));
      if(CheckFile(slopesFile)){
	cout<<"Slopes file found for run "<<run<<".\n";
	all_found = 1;
      }
      else{
	cout<<"Slopes file NOT found for run "<<run<<".\n";
	cout<<Form("%s/macrocycle_slopes/macrocycle_slopes_%i%s.dat",
		   output_dir.Data(), (int)run, name)<<endl;
      }
      /////////////////////////////////////////////////////////


      //If run found extract all slopes
      /////////////////////////////////////////////////////////
      while(!slopesFile.eof() && all_found){
	string line;
	vRun.push_back(run);
	vSlug.push_back(slug);
	nCycle++;
	
	//Extract header information and check if they match.
	/////////////////////////////////////////////////////
	Header_t slopes_header;
	ExtractHeader(slopesFile, slopes_header, size);
	//	cout<<"slopes"<<endl;
	vMacrocycle.push_back((double)slopes_header.macrocycle);
	vId.push_back((double)slopes_header.id);
	vFirstEntry.push_back((double)slopes_header.lEntry);
	vLastEntry.push_back((double)slopes_header.hEntry);
	for(int imod=0;imod<nMOD;imod++)
	  vCyclet[imod].push_back((double)slopes_header.microcycle[imod]);
	/////////////////////////////////////////////////////


	//Extract detector slopes
	/////////////////////////
	for(int idet=0;idet<nDet;idet++){
	  char *pref = new char[30](), *det = new char[30]();
	  slopesFile>>pref;
	  if(!(strcmp(pref, "det")==0)){
	    cout<<"Detector slopes file not in expected format.\n";
	    cout<<pref<<" "<<nDet<<endl;
	    return;
	  }
	  slopesFile>>det;
	  if(!(DetectorList[idet].CompareTo(det)==0)){
	    cout<<"Detector names not in expected format. Exiting.\n";
	    cout<<det<<" "<<nDet<<endl;
	    return;
	  }
	  double dslope[nMON], dslope_err[nMON];
	  for(int imon=0;imon<nMON; ++imon){
	    char *sl = new char[30](), *sl_err = new char[30]();
	    slopesFile>>sl>>sl_err;
	    Bool_t notANum = strcmp(sl,"nan")==0 || strcmp( sl_err,"nan")==0;
	    if(debug) 
	      cout<<DetectorList[idet].Data()<<" "<<atof(sl)<<" "
		  <<atof(sl_err)<<endl;
	    Double_t convert = slopesUnitConvert[imon];
	    dslope[imon] = (notANum ? errorFlag : atof(sl) * convert);
	    dslope_err[imon] = (notANum ? errorFlag : atof(sl_err) * convert);
	    getline(slopesFile, line);
	    delete[] sl;
	    delete[] sl_err;
	  }
	  vDetectorSlope[idet][mx1].push_back(0.5*(dslope[tgX] +
						   dslope[tgXP]/ scale_x));
	  vDetectorSlope[idet][mx2].push_back(0.5*(dslope[tgX] -
						   dslope[tgXP]/scale_x));
	  vDetectorSlope[idet][my1].push_back(0.5*(dslope[tgY] +
						   dslope[tgYP]/ scale_y));
	  vDetectorSlope[idet][my2].push_back(0.5*(dslope[tgY] - 
						   dslope[tgYP]/scale_y));
	  vDetectorSlope[idet][energy].push_back(dslope[eng]);
	  delete[] pref;
	  delete[] det;
	}
	/////////////////////////

	/////////////////////////////////////////////////////
	slopesFile.peek();
      }
      nRuns++;
    }
  }

  runList.close();
  cout<<nRuns<<" runs found.\n";
  cout<<nCycle<<" macrocycles found.\n";

  for(int imon=0;imon<nMON;imon++)
    if(MonitorList[imon].Contains("qwk_"))
      MonitorList[imon].Replace(0,4,"");
  for(int idet=0;idet<nDet;idet++)
    if(DetectorList[idet].Contains("qwk_"))
      DetectorList[idet].Replace(0,4,"");
  int num = 0;
  for(int i=0;i<(int)temp.size();i++)
    if(temp[i]!=temp[(i==0? 0:i-1)]){
      num++;
      cout<<num<<" "<<temp[i]<<endl;
    }

  //Find list of runs to exclude and use it to make a flag in the tree
  ////////////////////////////////////////////////////////////////////
  char * f_name = Form("%s/../temporarily_excluded_macrocycles%s."
		       "dat", gSystem->Getenv("MPS_ONLY_ROOTFILES"), stem.Data());
  ifstream excludedCyclesFile(f_name);
  if(!excludedCyclesFile.is_open()){
    cout<<"Excluded macrocycles file \""<<f_name<<"\"not found. Exiting.\n";
    return;
  }

  vector<vector<Int_t> >excludedCycles;
  excludedCycles.resize(3);
  Int_t b_run, low, high;
  excludedCyclesFile>>b_run>>low>>high;
  while(!excludedCyclesFile.eof()){
    excludedCycles[0].push_back(b_run);
    excludedCycles[1].push_back(low);
    excludedCycles[2].push_back(high);
    excludedCyclesFile>>b_run>>low>>high;
  }
  cout<<excludedCycles[0].size()<<" excluded cycles."<<endl;
  excludedCyclesFile.close();  
  ////////////////////////////////////////////////////////////////////


  //Create a new file for tree
  ////////////////////////////
  newfile =new TFile(Form("%s/../MacrocycleSlopesTree%s%s_nm"
			  ".root", gSystem->Getenv("MPS_ONLY_ROOTFILES"),name, 
			  stem.Data()),"recreate");
  ////////////////////////////


  //Temporary values for setting branch addresses
  ///////////////////////////////////////////////
  Int_t good;  
  Double_t lEntry, hEntry, Id, ihwp, is_run1;
  Double_t slope[nDET][nMOD], slopeErr[nDET][nMOD];
  ///////////////////////////////////////////////


  //Create branches in new tree
  /////////////////////////////
  TBranch *brFirstEntry = newTree->Branch("lEntry", &lEntry, "lEntry/D");
  TBranch *brLastEntry = newTree->Branch("hEntry", &hEntry, "hEntry/D");
  TBranch *brId = newTree->Branch("hId", &Id, "Id/D");
  TBranch *brRun = newTree->Branch("run", &run, "run/D");
  TBranch *brIsRun1 = newTree->Branch("is_run1", &is_run1, "is_run1/D");
  TBranch *brGoodRun = newTree->Branch("good", &good, "good/I");
  TBranch *brSlug = newTree->Branch("slug", &slug, "slug/D");
  TBranch *brHWP = newTree->Branch("ihwp", &ihwp, "ihwp/D");
  TBranch *brMacrocycle = newTree->Branch("macrocycle", &macrocycle, 
					  "macrocycle/D");
  TBranch *brSlopes[nDET][nMOD];
  TBranch *brCyclet[nMOD];
  for(int imod=0;imod<nMOD;imod++){
    brCyclet[imod]= newTree->Branch(Form("cyclet%i",imod), &cyclet[imod],
				    Form("cyclet%i/D",imod));
  }
  /////////////////////////////


  //Initialize all tree branches
  //////////////////////////////
  for(int idet=0;idet<nDet;idet++){
    char *dname = (char*)DetectorList[idet].Data();
    for(int imon=0;imon<nMON;imon++){
      char *mname = (char*)MonitorList[imon].Data();
      brSlopes[idet][imon] = newTree->Branch(Form("%s_%s",dname, mname), 
					     &slope[idet][imon],
					     Form("%s_%s/D",dname, mname));	
    }
  }
  //////////////////////////////



  for(Int_t icycle=0;icycle<(int)vRun.size(); ++icycle){
    //    cout<<"icycle:"<<icycle<<endl;
    for(int imod=0;imod<nMOD;++imod){
      //      cout<<"imod:"<<imod<<endl;
      cyclet[imod] = vCyclet[imod].at(icycle);
    }

    for(int idet=0;idet<nDet;++idet){
      //      cout<<"idet:"<<idet<<endl;
      for(int imon=0;imon<nMON;imon++){
	//	cout<<"imon:"<<imon<<endl;
	slope[idet][imon] = vDetectorSlope[idet][imon].at(icycle);
      }
    }

    run = vRun.at(icycle);
    is_run1 = (run<13000 ? 1.0 : 0.0);
    Id = vId.at(icycle);
    lEntry = vFirstEntry.at(icycle);
    hEntry = vLastEntry.at(icycle);
    slug = vSlug.at(icycle);
    ihwp = (double)vHWP[1][(int)slug];
    macrocycle = vMacrocycle.at(icycle);

    for(int imod=0;imod<nMOD;imod++){
      cyclet[imod] = vCyclet[imod].at(icycle);
    }
    good = 1;
    for(int i=0;i<(int)(excludedCycles[0].size());i++){
      if((int)excludedCycles[0].at(i)==(int)run){
	if(excludedCycles[1].at(i)==(int)lEntry && 
	   excludedCycles[2].at(i)== (int)hEntry){
	  good = 0;
	}else if(excludedCycles[1].at(i)==(int)lEntry || 
		 excludedCycles[2].at(i)== (int)hEntry){
	  cout<<"Disagreement on range of macrocycle.\n"<<
	    "Excluded runs file: "<<run<<" "<<excludedCycles[1][i]<<" "<<
	    excludedCycles[2][i]<<"\n"<<"Slopes tree:        "<<lEntry<<
	    " "<<hEntry<<endl;
	}
      }    
    }

    newTree->Fill();
  }

  
  //  newTree->Print();

  //  newTree->AutoSave();
  newTree->Write("",TObject::kOverwrite);
  newfile->Close();
  return;
}



