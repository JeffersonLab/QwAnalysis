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


void makeSlopesTree(Int_t run_start = 13842, Int_t  run_end = 19000, 
		    Double_t fUnitConvertTargetSlope = 1.0e4, Int_t chisq = 1) {
  // fUnitConvertTargetSlope needs to be the same value used in making the coeff-
  // cient files. This can be found on the line that looks like this in the file
  // QwMpsOnly.cc:
  // fUnitConvert[i] = (MonitorList[i].Contains("Slope") ? 1.0e4 : 1.0);
  // In this example fUnitConvert=1.0e4
  const Int_t nDET = 32, nMOD = 5, nRUNS = 1350, nCOIL = 10;
  string line;
  TFile *newfile; 
  TTree *newTree = new TTree("slopes", "slopes"); 
  Int_t badRuns[nRUNS];
  char  mon[255], monitr[255], det[255], detectr[255], sl[255], x[255], xe[255];
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
  ifstream runList(Form("%s/macros/runsBySlug.dat", 
			 gSystem->Getenv("BMOD_SRC")));


  if(runList.is_open())cout<<"File list found.\n";
  else cout<<"File list not found.\n";

  Int_t n = 0, nBad = 0;
  //  for(int i=0;i<nMOD;i++)n[i] = 0;
//   Double_t runs[nRUNS], slugs[nRUNS];
//   Double_t slopes[nDET][nMOD][nRUNS], slopesErr[nDET][nMOD][nRUNS];
//   Double_t monCoeffs[nMOD][nCOIL][nRUNS], monCoeffsErr[nMOD][nCOIL][nRUNS];
//   Double_t detCoeffs[nDET][nCOIL][nRUNS], detCoeffsErr[nDET][nCOIL][nRUNS];
//   Double_t sineRes[nDET][nMOD][nRUNS], sineResErr[nDET][nMOD][nRUNS];
//   Double_t cosineRes[nDET][nMOD][nRUNS], cosineResErr[nDET][nMOD][nRUNS];
  Double_t runs[nRUNS], slugs[nRUNS];
  vector<vector<vector<Double_t> > > slopes, slopesErr;
  vector<vector<vector<Double_t> > > monCoeffs, monCoeffsErr;
  vector<vector<vector<Double_t> > > detCoeffs, detCoeffsErr;
  vector<vector<vector<Double_t> > > sineRes, sineResErr;
  vector<vector<vector<Double_t> > > cosineRes, cosineResErr;

  slopes.resize(nDET);
  slopesErr.resize(nDET);
  sineRes.resize(nDET);
  sineResErr.resize(nDET);
  cosineRes.resize(nDET);
  cosineResErr.resize(nDET);
  for(int j=0;j<nDET;j++){
    slopes[j].resize(nMOD);
    slopesErr[j].resize(nMOD);
    sineRes[j].resize(nMOD);
    sineResErr[j].resize(nMOD);
    cosineRes[j].resize(nMOD);
    cosineResErr[j].resize(nMOD);
  }

  monCoeffs.resize(nMOD);
  monCoeffsErr.resize(nMOD);
  for(int j=0;j<nMOD;j++){
    monCoeffs[j].resize(nCOIL);
    monCoeffsErr[j].resize(nCOIL);
  }

  detCoeffs.resize(nDET);
  detCoeffsErr.resize(nDET);
  for(int j=0;j<nDET;j++){
    detCoeffs[j].resize(nCOIL);
    detCoeffsErr[j].resize(nCOIL);
  }

  Double_t run = 0, slug = 0;
  FILE *logFile = fopen(Form("%s/log.dat",gSystem->Getenv("BMOD_OUT")), "w+");
  fprintf(logFile,"Run\tSlopes\tMonitor\tDetector\n");
  while(!runList.eof()&&runList.good()&&run<=run_end){
    runList>>sl>>x;
    Int_t log[6] = {0,0,0,0,0,0};

    run = atof(x);
    log[0] = (Int_t)run;
    if(run>=run_end)break;
    Bool_t all_good = 1;
    if(run>=run_start){
      char* name = (char*)(chisq ? "_ChiSqMin.set0" : ".set4");
      ifstream slopesFile(Form("%s/slopes/slopes_%i%s.dat",
			       gSystem->Getenv("BMOD_OUT"),(int)run, name));
      if(slopesFile.is_open()&&slopesFile.good()){
	log[1] = 1;
	printf("%s/slopes/slopes_%i%s.dat found.\n;",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
	slopesFile.peek();
	if(slopesFile.eof()){
	  cout<<"but is EMPTY\n";
	  all_good = 0;
	  log[1] = -1;
	}
	else  cout<<endl;
      }else{
	printf("%s/slopes/slopes_%i%s.dat NOT found.\n;",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
	all_good = 0;
      }
      name = (char*)(chisq ? "_ChiSqMin.set0" : "");
      ifstream monitorFile(Form("%s/slopes/mon_coil_coeff_%i%s.dat",
				gSystem->Getenv("BMOD_OUT"),(int)run, name));
      if(monitorFile.is_open()&&monitorFile.good()){
	log[2] = 1;
	printf("%s/slopes/mon_coil_coeff_%i%s.dat found.\n",
		gSystem->Getenv("BMOD_OUT"),(int)run, name);
	monitorFile.peek();
	if(monitorFile.eof()){
	  cout<<"but is EMPTY\n";
	  all_good = 0;
	  log[2] = -1;
	}
	else  cout<<endl;
      }else{
	printf("%s/slopes/mon_coil_coeff_%i%s.dat NOT found.\n",
		gSystem->Getenv("BMOD_OUT"),(int)run, name);
	all_good = 0;
      }

      ifstream detectorFile(Form("%s/slopes/det_coil_coeff_%i%s.dat",
				 gSystem->Getenv("BMOD_OUT"),(int)run, name));
      if(detectorFile.is_open()&&detectorFile.good()){
	log[3] = 1;
	printf("%s/slopes/det_coil_coeff_%i%s.dat found.\n",
		gSystem->Getenv("BMOD_OUT"),(int)run, name);
	cout<<Form("%s/slopes/det_coil_coeff_%i.dat found ",
		   gSystem->Getenv("BMOD_OUT"),(int)run); 
	detectorFile.peek();
	if(detectorFile.eof()){
	  cout<<"but is EMPTY\n";
	  all_good = 0;
	  log[3] = -1;
	}
	else  cout<<endl;
      }else{
	cout<<Form("%s/slopes/det_coil_coeff_%i.dat NOT found.\n ",
		   gSystem->Getenv("BMOD_OUT"),(int)run); 
	all_good = 0;
      }

      ifstream resSineFile(Form("%s/slopes/run%iSineAmpl%s.dat",
				gSystem->Getenv("BMOD_OUT"),(int)run, name));
      if(resSineFile.is_open()&&resSineFile.good()){
	log[4] = 1;
	printf("%s/slopes/run%iSineAmpl%s.dat found.\n",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
       	resSineFile.peek();
	if(resSineFile.eof()){
	  cout<<"but is EMPTY\n";
	  all_good = 0;
	  log[4] = -1;
	}
	else  cout<<endl;
      }else{
	printf("%s/slopes/run%iSineAmpl%s.dat NOT found.\n",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
	all_good = 0;
      }

      ifstream resCosineFile(Form("%s/slopes/run%iCosineAmpl%s.dat",
				  gSystem->Getenv("BMOD_OUT"),(int)run, name));
      if(resCosineFile.is_open()&&resCosineFile.good()){
	log[5] = 1;
	printf("%s/slopes/run%iCosineAmpl%s.dat found.\n",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
	resCosineFile.peek();
	if(resCosineFile.eof()){
	  cout<<"but is EMPTY\n";
	  all_good = 0;
	  log[5] = -1;
	}
	else  cout<<endl;
      }else{
	printf("%s/slopes/run%iCosineAmpl%s.dat NOT found.\n",
	       gSystem->Getenv("BMOD_OUT"),(int)run, name);
	all_good = 0;
      }

      fprintf(logFile,"%i\t%i\t%i\t%i\t%i\t%i\n",log[0],log[1],log[2],log[3], 
	      log[4],log[5]);
      if(all_good){
	cout<<"All files found for run "<<run<<endl;

	Int_t allDefined = 1;
	for(int i=0;i<nDET;i++){
	  getline(slopesFile,line);
	  for(int j=0;j<nMOD;j++){
	    slopesFile>>x>>xe;
	    getline(slopesFile,line);
	    runs[n] = run;
	    Bool_t notANum = strcmp(xe,"nan")==0 || strcmp( x,"nan")==0;
	    if(notANum)allDefined = 0;
	    slugs[n] = atof(sl);
// 	    slopes[i][j][n] = (notANum ? -99999.0 : atof(x) * slopesUnitConvert[j]);
// 	    slopesErr[i][j][n] = (notANum ? -99999.0 : atof(xe) * slopesUnitConvert[j]);
	    slopes[i][j].push_back((notANum ? -99999.0 : atof(x) * slopesUnitConvert[j]));
	    slopesErr[i][j].push_back((notANum ? -99999.0 : atof(xe) * slopesUnitConvert[j]));
	      //cout<<slopes[i][j][n]<<"+/-"<<slopesErr[i][j][n]<<"  "<<n<<endl;
	  }
	}

	for(int i=0;i<nDET;i++){
	  for(int j=0;j<nMOD;j++){
	    resSineFile>>x>>xe;
// 	    sineRes[i][j][n] = atof(x) * 1.0e6;
// 	    sineResErr[i][j][n] = atof(xe) * 1.0e6;
	    sineRes[i][j].push_back(atof(x) * 1.0e6);
	    sineResErr[i][j].push_back(atof(xe) * 1.0e6);
	    resCosineFile>>x>>xe;
// 	    cosineRes[i][j][n] = atof(x) * 1.0e6;
// 	    cosineResErr[i][j][n] = atof(xe) * 1.0e6;
	    cosineRes[i][j].push_back(atof(x) * 1.0e6);
	    cosineResErr[i][j].push_back(atof(xe) * 1.0e6);
	  }
	  getline(resSineFile,line);
	  getline(resCosineFile,line);
	}

	for(int i=0;i<nMOD;i++){
	  getline(monitorFile,line);
	  if(line == MonitorList[i].Data()){
	    Double_t unitConv = 1000;
	    if(MonitorList[i].Contains("Slope"))
	      unitConv = 1.0e6/fUnitConvertTargetSlope;
	    for(int j=0;j<nCOIL;j++){
	      monitorFile>>x>>xe;
	      getline(monitorFile,line);
// 	      monCoeffs[i][j][n] =  atof(x) * unitConv;
// 	      monCoeffsErr[i][j][n] = atof(xe) * unitConv;
	      monCoeffs[i][j].push_back(atof(x) * unitConv);
	      monCoeffsErr[i][j].push_back(atof(xe) * unitConv);
	    }
	  }else{
	    cout<<"Error in order of monitor coefficients file. Exiting\n";
	    return;
	  } 
	}

	for(int i=0;i<nDET;i++){
	  getline(detectorFile,line);
	  if(line == DetectorList[i].Data()){
	    Double_t unitConv = 1.0e6;
	    for(int j=0;j<nCOIL;j++){
	      detectorFile>>x>>xe;
	      getline(detectorFile,line);
// 	      detCoeffs[i][j][n] = atof(x) * unitConv;
// 	      detCoeffsErr[i][j][n] = atof(xe) * unitConv;
	      detCoeffs[i][j].push_back(atof(x) * unitConv);
	      detCoeffsErr[i][j].push_back(atof(xe) * unitConv);
	    }
	  }else{
	    cout<<"Error in order of detector coefficients file. Exiting\n";
	    return;
	  }
	}
	n++;
	if(!allDefined){
	  badRuns[nBad] = (int)run;
	  nBad++;
	  cout<<"Run "<<run<<" has undefined values.\n";
	}
      }else cout<<"Slopes file NOT found for run "<<run<<endl;
      slopesFile.close();
    }
  }
  fclose(logFile);
  cout<<n<<" runs found. "<<nBad<<" refused for 0 slopes error.\n";

  for(int i=0;i<nMOD;i++)
    if(MonitorList[i].Contains("qwk_"))
      MonitorList[i].Replace(0,4,"");
  for(int i=0;i<nDET;i++)
    if(DetectorList[i].Contains("qwk_"))
      DetectorList[i].Replace(0,4,"");



  Double_t slope[nDET][nMOD], slopeErr[nDET][nMOD];
  Double_t sineResid[nDET][nMOD], sineResidErr[nDET][nMOD];
  Double_t cosineResid[nDET][nMOD], cosineResidErr[nDET][nMOD];
  Double_t monCoeff[nMOD][nCOIL], monCoeffErr[nMOD][nCOIL];
  Double_t detCoeff[nDET][nCOIL], detCoeffErr[nDET][nCOIL];


  //Find list of runs to exclude and use it to make a flag in the tree
  ifstream excludedRunsFile(Form("%s/../temporarily_excluded_runs.dat",
		     gSystem->Getenv("BMOD_OUT")));
  vector<Int_t>excludedRuns;
  while(!excludedRunsFile.eof()){
    excludedRunsFile>>x;
    excludedRuns.push_back(atoi(x));
    getline(excludedRunsFile,line);
  }
  cout<<excludedRuns.size()<<" excluded runs."<<endl;
  

  //Create a new file
  char* name = (char*)(chisq ? "_ChiSqMin.set0" : ".set4");
  newfile = new TFile(Form("/net/data1/paschkedata1/slopesTree%s.root", name),"recreate");
  Int_t idx = 0, good;  
  TBranch *brSlopes[nDET][nMOD];
  TBranch *brSlopesErr[nDET][nMOD];
  TBranch *brSineRes[nDET][nMOD];
  TBranch *brSineResErr[nDET][nMOD];
  TBranch *brCosineResErr[nDET][nMOD];
  TBranch *brCosineRes[nDET][nMOD];
  TBranch *brMonCoeff[nMOD][nCOIL];
  TBranch *brMonCoeffErr[nMOD][nCOIL];
  TBranch *brDetCoeff[nDET][nCOIL];
  TBranch *brDetCoeffErr[nDET][nCOIL];
  TBranch *rn = newTree->Branch("run", &run, "run/D");
  TBranch *goodRn = newTree->Branch("good", &good, "good/I");
  TBranch *slg = newTree->Branch("slug", &slug, "slug/D");

  for(int i=0;i<nDET;i++){
    for(int j=0;j<nMOD;j++){
      brSlopes[i][j] = newTree->Branch(Form("%s_%s",DetectorList[i].Data(),
					    MonitorList[j].Data()), &slope[i][j],
				       Form("%s_%s/D",DetectorList[i].Data(),
					    MonitorList[j].Data()));	
      brSlopesErr[i][j] = newTree->Branch(Form("%s_%s_err",DetectorList[i].Data(),
					       MonitorList[j].Data()), &slopeErr[i][j],
					  Form("%s_%s_err/D",DetectorList[i].Data(),
					       MonitorList[j].Data()));	
      brSineRes[i][j] = newTree->Branch(Form("SineRes_%s_Coil%i",DetectorList[i].Data(),j), 
					&sineResid[i][j],
				       Form("SineRes_%s_Coil%i/D",DetectorList[i].Data(),j));	
      brSineResErr[i][j] = newTree->Branch(Form("SineRes_%s_Coil%i_err",DetectorList[i].Data(),j),
					   &sineResidErr[i][j],
					  Form("SineRes_%s_Coil%i_err/D",DetectorList[i].Data(),j));	
      brCosineRes[i][j] = newTree->Branch(Form("CosineRes_%s_Coil%i",DetectorList[i].Data(),j), 
					  &cosineResid[i][j],
				       Form("CosineRes_%s_Coil%i/D",DetectorList[i].Data(),j));	
      brCosineResErr[i][j] = newTree->Branch(Form("CosineRes_%s_Coil%i_err",DetectorList[i].Data(),j),
					     &cosineResidErr[i][j],
					  Form("CosineRes_%s_Coil%i_err/D",DetectorList[i].Data(),j));	
    }
  }
  for(int i=0;i<nMOD;i++){
    for(int j=0;j<nCOIL;j++){
      brMonCoeff[i][j] = newTree->Branch(Form("%s_Coil%i",MonitorList[i].Data(),j),
					 &monCoeff[i][j],Form("%s_Coil%i/D",
							      MonitorList[i].Data(),j));
      brMonCoeffErr[j][i] = newTree->Branch(Form("%s_Coil%i_err", MonitorList[i].Data(), 
						 j), &monCoeffErr[i][j], 
					    Form("%s_Coil%i_err/D",MonitorList[i].Data(),
						 j));
    }
  }
    
  for(int i=0;i<nDET;i++){
    for(int j=0;j<nCOIL;j++){
      brDetCoeff[i][j] = newTree->Branch(Form("%s_Coil%i",DetectorList[i].Data(),j),
					 &detCoeff[i][j],Form("%s_Coil%i/D",
							      DetectorList[i].Data(),j));
      brDetCoeffErr[i][j] = newTree->Branch(Form("%s_Coil%i_err", DetectorList[i].Data(), 
						 j), &detCoeffErr[i][j], 
					    Form("%s_Coil%i_err/D",DetectorList[i].Data(),
						 j));
    }
  }
    
  for (Int_t k=0;k<n; k++){
    for(int i=0;i<nDET;i++){
      for(int j=0;j<nMOD;j++){
	slope[i][j] = slopes[i][j][k];
	slopeErr[i][j] = slopesErr[i][j][k];
      }
    }

    for(int i=0;i<nMOD;i++){
      for(int j=0;j<nCOIL;j++){
	monCoeff[i][j] = monCoeffs[i][j][k];
	monCoeffErr[i][j] = monCoeffsErr[i][j][k];
      }
    }

    for(int i=0;i<nDET;i++){
      for(int j=0;j<nCOIL;j++){
	detCoeff[i][j] = detCoeffs[i][j][k];
	detCoeffErr[i][j] = detCoeffsErr[i][j][k];
      }
    }

    for(int i=0;i<nDET;i++){
      for(int j=0;j<nMOD;j++){
	sineResid[i][j] = sineRes[i][j][k];
	sineResidErr[i][j] = sineResErr[i][j][k];
	cosineResid[i][j] = cosineRes[i][j][k];
	cosineResidErr[i][j] = cosineResErr[i][j][k];
      }
    }

    run = runs[k];
    good = 1;
    for(int i=0;i<(int)excludedRuns.size()-1;i++)
      if(excludedRuns.at(i)==run) good = 0;
    slug = slugs[k];
    newTree->Fill();
  }
  
  newTree->Print();

  for(int i=0;i<nBad;i++){
    cout<<badRuns[i]<<" ";
    if(i%10==0&&i!=0)cout<<endl;
  }
  cout<<endl;
  //  newTree->AutoSave();
  newTree->Write("",TObject::kOverwrite);
  newfile->Close();
  delete newfile;
}



