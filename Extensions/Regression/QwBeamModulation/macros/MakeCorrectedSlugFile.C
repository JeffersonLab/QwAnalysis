// *************************** //
//
// Macro to create a copy of the "reduced" slugfiles with dithering corrections.
// Read the monitors and slopes to use for the corrections from text files.
//
// Rewritten by Don Jones 05/21/14
// Author: Manolis Kargiantoulakis, 8/29/13. 
// Code adapted from Scott MacEwan's (?) MakeSlug.C
//
// *************************** //



#include "TFile.h"
#include "TList.h"
#include "TChain.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TGraph.h"
#include "TSystem.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <utility>
#include <sstream>

using namespace std;

//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 60, nMON = 5, nMOD = 5;
const Double_t  errorFlag = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////
Bool_t globalEXIT=0;

void sigint_handler(int sig)
{
  std::cout << "handling signal no. " << sig << "\n";
  globalEXIT=1;
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
    if(!ch->GetBranch(detectorList[nDet].Data())){
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
  return nDet;
}

Int_t MakeCorrectedSlugFile(int slug, TString stem ="", int harddisk=2)
{
  Int_t run_period = ((slug >= 42 && slug <=136) ? 1 : 0);
  if( slug >= 137 && slug<=321 )
    run_period = 2;
  if(run_period == 0){
    cout<<"Invalid slug number. Exiting.\n";
    return -1;
  }
  TStopwatch timer;
  const Int_t debug=-1, minEntriesPerSlug = 1;
  TChain *ch = new TChain("slopes");
  char* output = Form("%s/../bmod_out%s", gSystem->Getenv("BMOD_OUT"), 
		      stem.Data());
  ch->Add(Form("%s/../MacrocycleSlopesTree_ChiSqMin.set0%s.root", 
	       gSystem->Getenv("BMOD_OUT"), stem.Data()));

  TString rootFileDir = Form("/net/data2/paschkelab2/reduced_slugfiles/run%i/", 
			     run_period);
  TString outFileDir = Form("/net/data2/paschkelab%i/reduced_slugfiles/run%i/", 
			    harddisk, run_period);
  TString rootFileName_in = rootFileDir + Form("reduced_slug%i.root", slug);
  TString rootFileName_out = outFileDir + 
    Form("corrected_slug%i%s.root", slug, stem.Data());

  
  TString slopeListsDir = output;
  slopeListsDir += "/slopelists/";

  //-> Read set of monitors to use for corrections

  //Get lists of monitors and detectors
  /////////////////////////////////////////////////////////
  TString monitor[nMOD], detector[nDET];
  char * configFile = Form("%s/config/setup_mpsonly%s.config",
			   gSystem->Getenv("BMOD_SRC"), stem.Data());
  Int_t nDet = GetMonitorAndDetectorLists(monitor, detector,0, configFile);



  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  cout << " " << nMON << " monitors will be used for corrections, \n"
       << " expecting that many slopes for each detector\n\n";

  cout << " Correcting for " << nDet << " detectors \n\n";
  /////////////////////////////////////////////////////////
  

  //Find the proper run range
  ifstream run_range_file(Form("%srun_ranges.dat", slopeListsDir.Data())); 
  if( !(run_range_file.is_open() && run_range_file.good()) ){
    cout<<"Run range file not found. Run the following command from slopeslists"
	<<" directory then rerun program:\n";
    cout<<"ls slopeList_"<<detector[0].Data()<<".* |"
	<<" sed 's/slopeList_"<<detector[0].Data()<<".//g'" 
	<<"|sed 's/-/  /g' |sed 's/.dat//g' > run_ranges.dat\n";
    return -1;
  }else{
    printf("%srun_ranges.dat found\n", slopeListsDir.Data());
  }

  int run_lo, run_hi, slug_lo, slug_hi, slopes_exist = 0;
  while (!run_range_file.eof() && !slopes_exist) {
    run_range_file >>run_lo>>run_hi;
    string line;
    ifstream slopesFile(Form("%sslopeList_%s.%i-%i.dat",
			     slopeListsDir.Data(), detector[0].Data(), 
			     run_lo, run_hi));
    if(!(slopesFile.is_open()&&slopesFile.good())){
      cout<<"Slopes file not found. Exiting.\n";
      printf("%sslopeList_%s.%i-%i.dat",
	     slopeListsDir.Data(),detector[0].Data(),run_lo, run_hi);
      return -1;
    }
    getline(slopesFile, line);
    //    cout<<line<<endl;
    string find_this = "slug >= ";
    size_t pos_lslug = line.find(find_this) + find_this.length();
    size_t n_digits = line.find(" ",pos_lslug) - pos_lslug ;
    find_this = line.substr(pos_lslug, n_digits);
    slug_lo = atoi(find_this.data());
    //    cout<<find_this<<" "<<n_digits<<" "<<slug_lo<<endl;

    find_this = "slug <= ";
    size_t pos_hslug = line.find(find_this) + find_this.length();
    n_digits = line.find("\"",pos_hslug) - pos_hslug;  
    find_this = line.substr(pos_hslug, n_digits);
    slug_hi = atoi(find_this.data());
    //    cout<<find_this<<" "<<n_digits<<" "<<slug_hi<<endl;

    if(slug>=slug_lo &&slug<=slug_hi){
      cout<<"Slopes found for slug "<<slug<<".\n";
      slopes_exist = 1;
      break;
    }
    run_range_file.peek();
  }
  run_range_file.close();
  if(!slopes_exist){
    cout<<"No slopes found for slug "<<slug<<". Exiting.\n";
    return -1;
  }

  Double_t **correction_slope = new Double_t *[nDet], 
    **err_correction_slope = new Double_t *[nDet];
  for(int idet = 0; idet<nDet;++idet){
    correction_slope[idet] = new Double_t[nMON];
    err_correction_slope[idet] = new Double_t[nMON];
  }
  
 
  cout << run_lo << " - " << run_hi << endl;
  cout << endl;
  
  for (int iDet=0; iDet<nDet; iDet++) {
      
    ifstream datFile;
    string line;
      
    TString fileName = Form("slopeList_%s.%d-%d.dat",
			    detector[iDet].Data(), run_lo, run_hi );
    fileName.Prepend(slopeListsDir);
    datFile.open( fileName );
    if (!datFile) { 
      cout << fileName.Data()<<" not found.\n";
      continue;
    }
      
    if (debug>2)
      cout << " Reading slopes from file :: " << fileName << endl;
    int iMon;
    for (iMon=0; iMon<nMON;) {
      getline(datFile,line); 
      if(!datFile.good())  break; 
      if (line[0]=='!') continue;  // allow for comments in files.
      std::stringstream ss;
      double sl, err;
      TString mon;
      ss.str(line);
      ss>>mon>>sl>>err;
 
      //Extract by name
      bool found = false;
      int idx;
      for(idx=0;idx<nMON;++idx){
	if(monitor[idx].Contains(mon.Data())){
	  found = true;
	  break;
	}
	//	cout<<monitor[idx].Data()<<" "<<mon.Data()<<" "<<found<<endl;
      }
      if(!found){
	cout<<"Monitor not found in list. Exiting.\n";
	return -1;
      }
      correction_slope[iDet][idx] = sl;
      err_correction_slope[iDet][idx] = err;

      // Divide position slopes by 1000 to match units in tree
      if (monitor[idx].Contains("bpm") || 
	  (monitor[idx].Contains("target") && 
	   !monitor[idx].Contains("Slope"))) { 
	correction_slope[iDet][idx] /= 1000.0; 
	err_correction_slope[iDet][idx] /= 1000.0;
      }
      if (0) cout << " Slope " << idx << " :: " <<detector[iDet].Data()
		  << "_"<<monitor[idx].Data()<<" "
		  << correction_slope[iDet][idx] << " +- " 
		  << err_correction_slope[iDet][idx] << endl;
      iMon++;
    }
    datFile.close(); 
    if (iMon!=nMON)    
      cout << "\n\n\n\t !!! Attention !!!\n"
	   << " Expected " << nMON << " lines in file, read only " 
	   << iMon << " !!\n\n\n\n";
      
  }
  
  

  // -> Now read in the input "reduced" tree
  Double_t x, y, xp, yp, x1, y1, x2, y2, energy;
  Int_t run;
  TLeaf *inputDetectorLeaf[nDET], *inputMonitorLeaf[nMON];
  Double_t inputDetectorValue[nDET];
  Double_t inputMonitorValue[nMON];
  Double_t correctedDetectorValue[nDET];

  TFile *inputRootFile = TFile::Open(rootFileName_in);
  if (inputRootFile==0) {
    printf(" Did not find given input ROOT file :: \n%s",
	   rootFileName_in.Data() );
    exit(1);
  }    
  printf("Opened %s\n",rootFileName_in.Data());
  
  TTree *tree = (TTree*)gROOT->FindObject("slug");
  if(tree->GetEntries()<minEntriesPerSlug){
    cout<<"Only "<<tree->GetEntries()<<" entries in slug "<<slug<<". Exiting.\n";
    return -1;
  }
  // Only enable the important branches
  tree->SetBranchStatus("*",0); // disable all branches
  for (int iDet=0; iDet<nDet; iDet++) {
    tree->SetBranchStatus(Form("%s",detector[iDet].Data()), 1);	
    inputDetectorLeaf[iDet] = (TLeaf*) tree->GetLeaf(detector[iDet].Data());
    tree->SetBranchAddress(Form("%s",detector[iDet].Data()), 
			   &inputDetectorValue[iDet]);	
  }
  //define new monitors if needed
  Double_t scale_xp = 1.0, scale_yp = 1.0;
  if(stem.Contains("new_monitors")){
    ifstream scale_file(Form("%s/config/new_monitors.config",
			     gSystem->Getenv("BMOD_SRC")));
    if(!(scale_file.is_open() && scale_file.good())){
      std::cout<<"New monitor scale factor file not found. Exiting.\n";
      return -1;
    }
    std::string temp_line;
    while(scale_file.peek()=='!')
      getline(scale_file, temp_line);
    char temp[255], arun1[255], arun2[255], brun1[255], brun2[255];
    scale_file>>temp>>arun1>>arun2;
    scale_file>>temp>>brun1>>brun2;
    scale_xp = 1000 * (slug < 137 ? atof(arun1) : atof(arun2));
    scale_yp = 1000 * (slug < 137 ? atof(brun1) : atof(brun2));
    scale_file.close();
    cout<<"New monitor scale factors: "<<scale_xp<<" "<<scale_yp<<endl;
    tree->SetBranchStatus("diff_qwk_targetX", 1);
    tree->SetBranchStatus("diff_qwk_targetXSlope", 1);
    tree->SetBranchStatus("diff_qwk_targetY", 1);
    tree->SetBranchStatus("diff_qwk_targetYSlope", 1);
    tree->SetBranchStatus("diff_qwk_bpm3c12X", 1);
    tree->SetBranchAddress("diff_qwk_targetX", &x);
    tree->SetBranchAddress("diff_qwk_targetXSlope", &xp);
    tree->SetBranchAddress("diff_qwk_targetY", &y);
    tree->SetBranchAddress("diff_qwk_targetYSlope", &yp);
    tree->SetBranchAddress("diff_qwk_bpm3c12X", &energy);
  }
  else{
    for (int iMon=0; iMon<nMON; iMon++) {
      tree->SetBranchStatus(Form("%s",monitor[iMon].Data()), 1);
      inputMonitorLeaf[iMon] = (TLeaf*) tree->GetLeaf(monitor[iMon].Data());
      tree->SetBranchAddress(Form("%s",monitor[iMon].Data()), 
			     &inputMonitorValue[iMon]);
    }
  }
  tree->SetBranchStatus("run",1);				
  tree->SetBranchAddress("run", &run);				

  
  // Create the root file and tree to hold the corrected data
     TFile *correctedRootFile = new TFile(rootFileName_out.Data(),"RECREATE");
  if (correctedRootFile == NULL) {
    printf("Error: opening file %s for output\n",rootFileName_out.Data());
    return -1;
  }
  TTree *corrected_tree = new TTree("corrected_tree", "Corrected slug tree");
  TBranch* branches[nDET];

  TBranch* branch_flag = corrected_tree->Branch("slopes_exist", &slopes_exist, "slopes_exist/I");
  TBranch* br_mx1;
  TBranch* br_mx2;
  TBranch* br_my1;
  TBranch* br_my2;
  //  Int_t iEvent=0, iRun=0, iRunlet=0;

  
  // Modify the leaf names for the corrected detectors
  TString newLeafName[nDET];
  for (int iDet=0; iDet<nDet; iDet++) {
    newLeafName[iDet] = Form("corrected_%s", detector[iDet].Data() );
    if (debug > 0) 
      printf (" Corrected leaf name :: %s\n", newLeafName[iDet].Data());
    // Create the branch for the corrected slug tree
    branches[iDet] = corrected_tree->Branch(newLeafName[iDet].Data(), 
					    &correctedDetectorValue[iDet],
					    Form("%s/D",newLeafName[iDet].Data())
					    );
  }
  if(stem.Contains("new_monitors")){
    br_mx1 = corrected_tree->Branch("diff_MX1", &x1, "diff_MX1/D");
    br_mx2 = corrected_tree->Branch("diff_MX2", &x2, "diff_MX2/D");
    br_my1 = corrected_tree->Branch("diff_MY1", &y1, "diff_MY1/D");
    br_my2 = corrected_tree->Branch("diff_MY2", &y2, "diff_MY2/D");
  }


  //-> Step through the tree reading detector and monitor values.
  // Use the appropriate slopes to correct the detector values
  // and save them to the corrected tree.
  Int_t nEntries = tree->GetEntries();
  printf("There are %i entries in the tree.\n",nEntries);
  int ient = 0, percent = 0, order[nMON] = {-1,-1,-1,-1,-1};
  const int idxX1 = 0, idxX2 = 1, idxY1 = 2, idxY2 = 3, idxE = 4;
  if(stem.Contains("new_monitors")){
    for(int imon=0;imon<nMON;++imon){
      if(monitor[imon].Contains("MX1")) order[idxX1] = imon;
      else if(monitor[imon].Contains("MX2")) order[idxX2] = imon;
      else if(monitor[imon].Contains("MY1")) order[idxY1] = imon;
      else if(monitor[imon].Contains("MY2")) order[idxY2] = imon;
      else if(monitor[imon].Contains("bpm")) order[idxE] = imon;
    }
    for(int imon=0;imon<nMON;++imon){
      if(order[imon]==-1){
	cout<<"Index not found. Exiting.\n";
	return -1;
      }
    }
  }
  for (Int_t iEntry=0; iEntry<nEntries; iEntry++) {
    tree->GetEntry(iEntry);
    if(ient>tree->GetEntries()/10){
      percent += 10;
      cout<<percent<<" % complete.\n";
      ient = 0;
    }
    ++ient;
    for (int iDet=0; iDet<nDet; iDet++) {
      if (inputDetectorLeaf[iDet]) {
	correctedDetectorValue[iDet]=inputDetectorLeaf[iDet]->GetValue();
	if (debug>1 && iEntry==0 && iDet<3) 
	  cout << " Initial value :: " << correctedDetectorValue[iDet] << endl;
	if (slopes_exist) {
	  if(stem.Contains("new_monitors")){
	    inputMonitorValue[order[idxX1]] = x1 = x + scale_xp * xp;
	    inputMonitorValue[order[idxX2]] = x2 = x - scale_xp * xp;
	    inputMonitorValue[order[idxY1]] = y1 = y + scale_yp * yp;
	    inputMonitorValue[order[idxY2]] = y2 = y - scale_yp * yp;
	    inputMonitorValue[order[idxE]] = energy;
	  }
	  for (int iMon=0; iMon<nMON; iMon++) { // Apply corrections
	  
// 	  cout << " \n\n Correction slope value at this point ( "
// 	       << iDet << " , " << iMon << " , " << iRange << " ) :: "
// 	       << correction_slope[iDet][iMon][iRange] << "\n\n";

	    correctedDetectorValue[iDet] -= correction_slope[iDet][iMon] 
	                                    * inputMonitorValue[iMon];
	    if (debug > -1 && iEntry==0 && iDet<3) 
	      cout << " After correction " << monitor[iMon].Data() << " ( "
		   << correction_slope[iDet][iMon] << " * " 
		   << inputMonitorValue[iMon] << " ) :: "
		   << correctedDetectorValue[iDet] << endl;
	  }
	}
      }
      else
	correctedDetectorValue[iDet] = -1e6;
      
      if (debug > 1 && iEntry<10) 
	printf("%i %i ::  %10e  -->  %10e \n", iEntry, iDet,
	       inputDetectorValue[iDet], correctedDetectorValue[iDet] );
    }
    corrected_tree->Fill();
  }
  corrected_tree->Print();
  corrected_tree->Write("",TObject::kOverwrite);
  correctedRootFile->Close();
  timer.Print();
  Double_t fillTime = timer.CpuTime();
  printf("Time: %f ms per event\n",fillTime/nEntries*1000);
  return 0;
}
