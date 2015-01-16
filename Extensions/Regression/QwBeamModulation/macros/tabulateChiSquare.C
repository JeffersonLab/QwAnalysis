#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TMath.h"

 
Int_t tabulateChiSquare(int run=13993){
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
  const Int_t nDET = 29, nMOD = 5, nCOIL = 10, nTYPE=2;

  char  mon[255], monitr[255], det[255], detectr[255], x[255], xe[255];
  string line;
  TString MonitorList[nMOD];
  TString DetectorList[nDET];

  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));
  for(int i=0;i<5;i++){
    file>>mon>>monitr;
    getline(file, line);
    MonitorList[i] = TString(monitr);
  }
  getline(file, line);
  for(int i=0;i<nDET;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[i] = TString(detectr);
    cout<<DetectorList[i].Data()<<endl;
  }

  vector<vector<Double_t> >  monCoeffs, monCoeffsErr;
  vector<vector<Double_t> >  detCoeffs, detCoeffsErr;
  vector<vector<vector<Double_t> > > slopes, slopesErr;
  monCoeffs.resize(nMOD);
  monCoeffsErr.resize(nMOD);
  
  detCoeffs.resize(nDET);
  detCoeffsErr.resize(nDET);
  
  slopes.resize(nTYPE);
  slopesErr.resize(nTYPE);
  for(int j=0;j<nTYPE;j++){
    slopes[j].resize(nDET);
    slopesErr[j].resize(nDET);
  }


  ifstream monitorFile(Form("%s/slopes/mon_coil_coeff_%i.dat",
				gSystem->Getenv("BMOD_OUT"),(int)run));
  if(monitorFile.is_open()&&monitorFile.good()){
    for(int j=0;j<nMOD;j++){
      getline(monitorFile,line);
      double conv = (MonitorList[j].Contains("Slope") ? 1.0e-6 : 1.0);
      for(int i=0;i<nCOIL;i++){
	monitorFile>>x>>xe;
	monCoeffs[j].push_back(atof(x)*conv);
	monCoeffsErr[j].push_back(atof(xe)*conv);
	cout<<atof(x)<<"+/-"<<atof(xe)<<"\n";
	getline(monitorFile,line);
      }
    }
  }else return -1;

  ifstream detectorFile(Form("%s/slopes/det_coil_coeff_%i.dat",
				gSystem->Getenv("BMOD_OUT"),(int)run));
  if(detectorFile.is_open()&&detectorFile.good()){
    for(int j=0;j<nDET;j++){
      getline(detectorFile,line);
      cout<<line<<endl;
      for(int i=0;i<nCOIL;i++){
	detectorFile>>x>>xe;
	detCoeffs[j].push_back(atof(x));
	detCoeffsErr[j].push_back(atof(xe));
	cout<<atof(x)<<"+/-"<<atof(xe)<<"\n";
	getline(detectorFile,line);

      }
    }
  }else return -2;

  ifstream slopesFile1(Form("%s/slopes/slopes_%i.set4.dat",
			   gSystem->Getenv("BMOD_OUT"), run));
  ifstream slopesFile2(Form("%s/slopes/slopes_%i.set4_chsq.dat",
			   gSystem->Getenv("BMOD_OUT"), run));

  if (slopesFile1.is_open()&&slopesFile2.is_open()){
    cout<<"File found.\n";
  }
  else{
    cout<<"slopesFile not found. Exiting.\n";
    return -3;
  }

  TString detector;

  for(int i=0;i<nDET;i++){
    if(slopesFile1.is_open()&&slopesFile2.is_open()){
      getline(slopesFile1, line);
      getline(slopesFile2, line);
      detector = TString("det ");
      detector += DetectorList[i].Data();
      if(line==detector.Data()){
	cout<<line<<"\n";
	for(int j=0;j<5;j++){
	  slopesFile1>>x>>xe;
	  Bool_t notANum = strcmp(xe,"nan")==0 || strcmp( x,"nan")==0;
	  slopes[0][i].push_back( (notANum ? -99999.0 : atof(x)));
	  slopesErr[0][i].push_back( (notANum ? -99999.0 : atof(xe)));
	  cout<<slopes[0][i][j]<<"+/-"<<slopesErr[0][i][j]<<"    ";
	  getline(slopesFile1, line);
	  slopesFile2>>x>>xe;
	  notANum = strcmp(xe,"nan")==0 || strcmp( x,"nan")==0;
	  slopes[1][i].push_back( (notANum ? -99999.0 : atof(x)));
	  slopesErr[1][i].push_back( (notANum ? -99999.0 : atof(xe)));
	  cout<<slopes[1][i][j]<<"+/-"<<slopesErr[1][i][j]<<"\n";
	  getline(slopesFile2, line);
	}
      }
    }
  }
  
  cout<<"\nComparison of Chi square from 5 Coil inversion and 10 Coil Chi"
    "square  minimization.\n\n"; 
  cout<<"                 5 Coil Inversion    10 Coil Chi Square\n"
      <<"                                        Minimzation\n";
  for(int i=0;i<nDET;i++){
    double valChiSq = 0, val = 0;
    for(int j=0;j<nCOIL;j++){
      double dDdC_ChiSq = 0, dDdC = 0;
      for(int k=0;k<nMOD;k++){
	dDdC += slopes[0][i][k]*monCoeffs[k][j];
	dDdC_ChiSq += slopes[1][i][k]*monCoeffs[k][j];
      }
      dDdC -= detCoeffs[i][j];
      dDdC_ChiSq -= detCoeffs[i][j];
      val += TMath::Power(dDdC, 2); 
      valChiSq += TMath::Power(dDdC_ChiSq, 2);
    }
    printf("%s%14s   %12.3e         %12.3e%s\n",
	   (val<valChiSq ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	   DetectorList[i].Data(), val, valChiSq, ANSI_COLOR_RESET);
  }
  monitorFile.close();
  detectorFile.close();
  slopesFile1.close();
  slopesFile2.close();
  return 0;
}
