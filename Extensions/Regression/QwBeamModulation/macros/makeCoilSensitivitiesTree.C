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


void makeCoilSensitivitiesTree(Int_t run_start = 13842, Int_t  run_end = 19000, 
		    Double_t fUnitConvertTargetSlope = 1.0e4, Int_t chisq = 1) {
  // fUnitConvertTargetSlope needs to be the same value used in making the coeff-
  // cient files. This can be found on the line that looks like this in the file
  // QwMpsOnly.cc:
  // fUnitConvert[i] = (MonitorList[i].Contains("Slope") ? 1.0e4 : 1.0);
  // In this example fUnitConvert=1.0e4
  const Int_t nDET = 32, nMOD = 5, nMON = 5;
  string line;
  TFile *newfile; 
  TTree *newTree = new TTree("coil_sens", "coil_sens"); 
  char  mon[255], monitr[255], det[255], detectr[255], sl[255], x[255];
  TString MonitorList[nMOD];
  TString DetectorList[nDET];
  Double_t slopesUnitConvert[5];

  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));

  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  for(int i=0;i<5;i++){
    file>>mon>>monitr;
    getline(file, line);
    MonitorList[i] = TString(monitr);
    slopesUnitConvert[i] = (MonitorList[i].Contains("Slope") ? 1.0 : 1.0e3);
    if(!ch->GetBranch(MonitorList[i].Data())){
      cout<<MonitorList[i].Data()<<" missing. Exiting.\n";
      return;
    }else
      cout<<MonitorList[i].Data()<<" unit conversion: "<<
	slopesUnitConvert[i]<<endl;
  }
  getline(file, line);
  Int_t nDet = 0;
  for(int i=0;i<nDET;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[nDet] = TString(detectr);
    if(!ch->GetBranch(DetectorList[nDet].Data())){
      cout<<DetectorList[nDet].Data()<<" missing.\n";
    }else{
      cout<<DetectorList[nDet].Data()<<endl;
      nDet++;
    }
    
  }
  delete ch;

  ifstream runList(Form("%s/macros/runsBySlug_new.dat", 
			gSystem->Getenv("BMOD_SRC")));


  if(runList.is_open())cout<<"File list found.\n";
  else cout<<"File list not found.\n";


  vector<Double_t>  vRun, vSlug, vCycle, vCoil;
  vector< vector<Double_t> >  vMonChiSq, vDetChiSq;
  vector< vector<Double_t> >  vMeanDet, vMeanDetErr;
  vector< vector<Double_t> >  vMeanMon, vMeanMonErr;
  vector< vector<Double_t> >  vMonCoeffSin, vMonCoeffSinErr;
  vector< vector<Double_t> >  vMonCoeffCos, vMonCoeffCosErr;
  vector< vector<Double_t> >  vDetCoeffSin, vDetCoeffSinErr;
  vector< vector<Double_t> >  vDetCoeffCos, vDetCoeffCosErr;
  vMeanDet.resize(nDET);
  vDetChiSq.resize(nDET);
  vMeanDetErr.resize(nDET);
  vDetCoeffSin.resize(nDET);
  vDetCoeffSinErr.resize(nDET);
  vDetCoeffCos.resize(nDET);
  vDetCoeffCosErr.resize(nDET);
  vMeanMon.resize(nMON);
  vMonChiSq.resize(nMON);
  vMeanMonErr.resize(nMON);
  vMonCoeffSin.resize(nMON);
  vMonCoeffSinErr.resize(nMON);
  vMonCoeffCos.resize(nMON);
  vMonCoeffCosErr.resize(nMON);

  Double_t run = 0, slug = 0, nCycle = 0, cycle = 0, coil = 0, nRuns = 0;

  //Read information for all runs in runlist.
  while(!runList.eof()&&runList.good()&&run<=run_end){
    runList>>sl>>x;
    run = atof(x);
    slug = atof(sl);
    if(run>=run_end)break;
    if(run>=run_start){
      char* name = (char*)(chisq ? "_ChiSqMin.set0" : ".set4");

      //Find coil sensitivities file for run
      ifstream coilSensFile(Form("%s/coil_sensitivities/coil_sens_%i%s.dat",
			       gSystem->Getenv("BMOD_OUT"),(int)run, name));

      if(coilSensFile.is_open()&&coilSensFile.good()){
	printf("%s/coil_sensitivities/coil_sens_%i%s.dat found.\n",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
	nRuns++;
 
	//For each good run extract information by microcycle
 	while(!coilSensFile.eof()&&coilSensFile.good()){
	  bool coil_found = 0;
	  // vector<Double_t>  vRun, vSlug, vCycle, vCoil, vChiSq;

	  //Find the coil number
	  int n = 0;
	  char num[3];
	  while(!coil_found && !coilSensFile.eof()){
	    char *cl = new char[5]();
	    num[0] = 9;
	    coilSensFile>>cl>>num;
	    getline(coilSensFile, line);
	    if(strcmp(cl,"Coil")==0){
	      coil_found = 1;
	      coil = atof(num);
	      //	      cout<<"Coil "<<coil<<" found. Cycle="<<nCycle<<"\n";
	      if(coil>=0 && coil<5){
		vCoil.push_back(coil);
		vCycle.push_back(nCycle);
		vRun.push_back(run);
		vSlug.push_back(slug);
		nCycle++;
	      }else{
		printf("Warning! Unknown coil number %i in run %i. Exiting.\n",
		       (int)coil, (int)run);
		return;
	      }
	    }else{
	      cout<<"coil:"<<cl<<"  number:"<<atof(num)<<"  eof:"<<
		coilSensFile.eof()<<" coil_found"<<coil_found<<"  n"<<n<<endl;
	    }
	    delete [] cl;
	    n++;
	  }

	  //extract sensitivities for monitors then detectors
	  char *name = new char[25](), *chSq = new char[25]();
	  char *cons = new char[25](), *consE = new char[25]();
	  char *cos = new char[25](), *cosE = new char[25]();
	  char *sin = new char[25](), *sinE = new char[25]();
	  for(int imon=0;imon<nMON;imon++){
	    coilSensFile>>name>>cons>>consE>>sin>>sinE>>cos>>cosE>>chSq;
	    getline(coilSensFile, line);
	    if(MonitorList[imon].CompareTo(name)==0){
	      vMeanMon[imon].push_back(atof(cons));
	      vMeanMonErr[imon].push_back(atof(consE));
	      vMonCoeffSin[imon].push_back(atof(sin));
	      vMonCoeffSinErr[imon].push_back(atof(sinE));
	      vMonCoeffCos[imon].push_back(atof(cos));
	      vMonCoeffCosErr[imon].push_back(atof(cosE));
	      vMonChiSq[imon].push_back(atof(chSq));
	    }else{
	      printf("Monitor sensitivities either out of order or not found"
		     " in file. Exiting.\n");
	      cout<<MonitorList[imon].Data()<<" "<<name<<endl;
	      return ;
	    }
	  }
	  for(int idet=0;idet<nDet;idet++){
	    coilSensFile>>name>>cons>>consE>>sin>>sinE>>cos>>cosE>>chSq;
	    getline(coilSensFile, line);
	    if(DetectorList[idet].CompareTo(name)==0){
	      vMeanDet[idet].push_back(atof(cons));
	      vMeanDetErr[idet].push_back(atof(consE));
	      vDetCoeffSin[idet].push_back(atof(sin));
	      vDetCoeffSinErr[idet].push_back(atof(sinE));
	      vDetCoeffCos[idet].push_back(atof(cos));
	      vDetCoeffCosErr[idet].push_back(atof(cosE));
	      vDetChiSq[idet].push_back(atof(chSq));
	    }else{
	      printf("Warning! %s (%s) sensitivity out of order or missing in file.\n",
		     DetectorList[idet].Data(), name);
	      return;
	    }
	  }
	  delete [] name;
	  delete [] chSq;
	  delete []cons;
	  delete [] consE;
	  delete [] sin;
	  delete [] sinE;
	  delete [] cos;
	  delete [] cosE;

	  //if last coil skip to end of file
	  while(coilSensFile.peek()=='\n' && !coilSensFile.eof()){
	    getline(coilSensFile,line);
	  }
	}
	coilSensFile.close();
      }else{
	printf("%s/coil_sensitivities/coil_sens_%i%s.dat NOT found.\n",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
      }
    }
    getline(runList, line);
  }

  runList.close();
  cout<<nRuns<<" runs found.\n";
  cout<<nCycle<<" cycles found.\n";

  for(int i=0;i<nMOD;i++)
    if(MonitorList[i].Contains("qwk_"))
      MonitorList[i].Replace(0,4,"");
  for(int i=0;i<nDet;i++)
    if(DetectorList[i].Contains("qwk_"))
      DetectorList[i].Replace(0,4,"");




  Double_t chiSquareMon[nMON], chiSquareDet[nDET];
  Double_t constantMon[nMON], constantMonErr[nMON];
  Double_t monCoeffSin[nMON], monCoeffSinErr[nMON];
  Double_t monCoeffCos[nMON], monCoeffCosErr[nMON];
  Double_t constantDet[nDET], constantDetErr[nDET];
  Double_t detCoeffSin[nDET], detCoeffSinErr[nDET];
  Double_t detCoeffCos[nDET], detCoeffCosErr[nDET];


  //Find list of runs to exclude and use it to make a flag in the tree
  ifstream excludedRunsFile(Form("%s/../temporarily_excluded_runs.dat",
		     gSystem->Getenv("BMOD_OUT")));
  vector<Int_t>excludedRuns;
  while(!excludedRunsFile.eof()){
    excludedRunsFile>>x;
    excludedRuns.push_back(atoi(x));
    getline(excludedRunsFile,line);
  }
  cout<<excludedRuns.size()-1<<" excluded runs."<<endl;
  

  //Create a new file
  char* name = (char*)(chisq ? "_ChiSqMin.set0" : ".set4");
  newfile = new TFile(Form("/net/data1/paschkedata1/coilSensitivitiesTree%s.root", 
			   name),"recreate");
  Int_t idx = 0, good;  
  TBranch *brChiSqMon[nMON], *brChiSqDet[nDET];
  TBranch *brConstMon[nMON], *brConstMonErr[nMON];
  TBranch *brConstDet[nDET], *brConstDetErr[nDET];
  TBranch *brMonCoeffSin[nMON], *brMonCoeffSinErr[nMON];
  TBranch *brMonCoeffCos[nMON], *brMonCoeffCosErr[nMON];
  TBranch *brDetCoeffSin[nDET], *brDetCoeffSinErr[nDET];
  TBranch *brDetCoeffCos[nDET], *brDetCoeffCosErr[nDET];
  TBranch *brRun = newTree->Branch("run", &run, "run/D");
  TBranch *brGoodRun = newTree->Branch("good", &good, "good/I");
  TBranch *brSlug = newTree->Branch("slug", &slug, "slug/D");
  TBranch *brCycle = newTree->Branch("cycle", &cycle, "cycle/D");
  TBranch *brCoil = newTree->Branch("coil", &coil, "coil/D");

  //Initialize detector branches
  for(int idet=0;idet<nDet;idet++){
    brChiSqDet[idet] = newTree->Branch(Form("ChiSq_%s",DetectorList[idet].Data()), 
				       &chiSquareDet[idet],
				       Form("ChiSq_%s/D",DetectorList[idet].Data()));
    brConstDet[idet] = newTree->Branch(Form("const_%s",DetectorList[idet].Data()), 
				       &constantDet[idet],
				       Form("const_%s/D",DetectorList[idet].Data()));
    brConstDetErr[idet] = newTree->Branch(Form("const_%s_err",DetectorList[idet].Data()), 
				       &constantDetErr[idet],
				       Form("const_%s_err/D",DetectorList[idet].Data()));
    brDetCoeffSin[idet] = newTree->Branch(Form("%s_Sine",DetectorList[idet].Data()), 
				       &detCoeffSin[idet],
				       Form("%s_Sine/D",DetectorList[idet].Data()));
	
    brDetCoeffSinErr[idet] = newTree->Branch(Form("%s_Sine_err",DetectorList[idet].Data()), 
				       &detCoeffSinErr[idet],
				       Form("%s_Sine_err/D",DetectorList[idet].Data()));
    brDetCoeffCos[idet] = newTree->Branch(Form("%s_Cosine",DetectorList[idet].Data()), 
				       &detCoeffCos[idet],
				       Form("%s_Cosine/D",DetectorList[idet].Data()));
	
    brDetCoeffCosErr[idet] = newTree->Branch(Form("%s_Cosine_err",DetectorList[idet].Data()),
				       &detCoeffCosErr[idet],
				       Form("%s_Cosine_err/D",DetectorList[idet].Data()));
  }
    
  //Initialize monitor branches
  for(int imon=0;imon<nMON;imon++){
    brChiSqMon[imon] = newTree->Branch(Form("ChiSq_%s",MonitorList[imon].Data()), 
				       &chiSquareMon[imon],
				       Form("ChiSq_%s/D",MonitorList[imon].Data()));
    brConstMon[imon] = newTree->Branch(Form("const_%s",MonitorList[imon].Data()), 
				       &constantMon[imon],
				       Form("const_%s/D",MonitorList[imon].Data()));
    brConstMonErr[imon] = newTree->Branch(Form("const_%s_err",MonitorList[imon].Data()), 
				       &constantMonErr[imon],
				       Form("const_%s_err/D",MonitorList[imon].Data()));
    brMonCoeffSin[imon] = newTree->Branch(Form("%s_Sine",MonitorList[imon].Data()), 
				       &monCoeffSin[imon],
				       Form("%s_Sine/D",MonitorList[imon].Data()));
	
    brMonCoeffSinErr[imon] = newTree->Branch(Form("%s_Sine_err",MonitorList[imon].Data()), 
				       &monCoeffSinErr[imon],
				       Form("%s_Sine_err/D",MonitorList[imon].Data()));
    brMonCoeffCos[imon] = newTree->Branch(Form("%s_Cosine",MonitorList[imon].Data()), 
				       &monCoeffCos[imon],
				       Form("%s_Cosine/D",MonitorList[imon].Data()));
	
    brMonCoeffCosErr[imon] = newTree->Branch(Form("%s_Cosine_err",MonitorList[imon].Data()), 
				       &monCoeffCosErr[imon],
				       Form("%s_Cosine_err/D",MonitorList[imon].Data()));
  }

  ////////////////////////////////////////////////////////////////    
  //Set values for variables pointed to by branches and fill tree
  //
  for (Int_t icycle=0;icycle<nCycle; icycle++){

    for(int idet=0;idet<nDet;idet++){
      chiSquareDet[idet] = vDetChiSq[idet][icycle];
      constantDet[idet] = vMeanDet[idet][icycle];
      constantDetErr[idet] = vMeanDetErr[idet][icycle];
      detCoeffSin[idet] = vDetCoeffSin[idet][icycle];
      detCoeffSinErr[idet] = vDetCoeffSinErr[idet][icycle];
      detCoeffCos[idet] = vDetCoeffCos[idet][icycle];
      detCoeffCosErr[idet] = vDetCoeffCosErr[idet][icycle];
    }

    for(int imon=0;imon<nMOD;imon++){
      chiSquareMon[imon] = vMonChiSq[imon][icycle];
      constantMon[imon] = vMeanMon[imon][icycle];
      constantMonErr[imon] = vMeanMonErr[imon][icycle];
      monCoeffSin[imon] = vMonCoeffSin[imon][icycle];
      monCoeffSinErr[imon] = vMonCoeffSinErr[imon][icycle];
      monCoeffCos[imon] = vMonCoeffCos[imon][icycle];
      monCoeffCosErr[imon] = vMonCoeffCosErr[imon][icycle];
    }

    run = vRun.at(icycle);
    slug = vSlug.at(icycle);
    cycle = vCycle.at(icycle);
    coil = vCoil.at(icycle);

    good = 1;
    for(int i=0;i<(int)(excludedRuns.size()-1);i++)
      if((int)excludedRuns.at(i)==run) good = 0;

    newTree->Fill();
  }
  
  //  newTree->Print();

  //  newTree->AutoSave();
  newTree->Write("",TObject::kOverwrite);
  newfile->Close();
  delete newfile;
}



