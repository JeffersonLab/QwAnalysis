#include "DetectorPlots.C"
#include "TMath.h"
#include<iostream>
#include <string>

using namespace std;

const Int_t nMON = 5, nMOD = 5, nDET = 32, nCOIL = 10, nSLUG = 322,
  MIN_ENTRIES_PER_RUN = 3,MIN_ENTRIES_PER_SLUG = 7;


struct Header_t{
  Int_t mod_type;
  Int_t cycle;
  Int_t lEntry;
  Int_t hEntry;
};

Int_t ExtractHeader(ifstream &file, Header_t &header){
  string str;
  char *correct_name[3] = { "Mod_type:", "Cycle:", "Entry_range:"};
  char name[255];
  Int_t num;
  file>>name;
  Int_t count = 0;
  while(strcmp(name, correct_name[0])!=0 && count<10){
    file>>name;
    if(file.eof())
      return -1;
    cout<<"?"<<name<<"?"<<correct_name[0]<<endl;
    ++count;
  }
  if(strcmp(name, correct_name[0])!=0){
    cout<<"(1)Unexpected header format. Exiting.\n";
    cout<<"!"<<name<<"!"<<correct_name[0]<<endl;
    return -1;
  }
  file>>num;
  header.mod_type = num;
  file>>name>>num;
  if(strcmp(name, correct_name[1])!=0){
    cout<<"(2)Unexpected header format. Exiting.\n";
    return -1;
  }
  header.cycle = num;
  file>>name>>num;
  if(strcmp(name, correct_name[2])!=0){
    cout<<"(2)Unexpected header format. Exiting.\n";
    return -1;
  }
  header.lEntry = num;
  file>>num;
  header.hEntry = num;
  getline(file, str);
  getline(file, str);
  //  cout<<"Mod_type: "<<header.mod_type<<" Cycle: "<<header.cycle<<" Range: "<<
  //    header.lEntry<<" "<<header.hEntry<<endl;
  return 0;
}


void FindMeanAndError(Double_t *x, Double_t *avg, Int_t n){
  Double_t mean = 0, err = 0;
  for(int i=0;i<n;i++){
    mean += x[i] / (Double_t)n;
  }
  avg[0] = mean;
  for(int i=0;i<n;i++){
    err += (x[i] - mean)*(x[i] - mean) / (Double_t)n;
  }
  avg[1] = TMath::Sqrt(err /(Double_t)n);
}

void FindErrorWeightedMeanAndError(Double_t *x, Double_t *xe, 
				   Double_t *mean, Int_t n){
  Double_t sum = 0, esqinv = 0; 
  for(int i=0;i<n;i++){
    sum += x[i] / (xe[i] * xe[i]);
    esqinv += 1 / (xe[i] * xe[i]);
  }
  mean[0] = sum / esqinv;
  mean[1] = sqrt(1 / esqinv);
}

Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList, Bool_t trunc){
  string line;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));

  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));

  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, line);
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
  getline(file, line);
  Int_t nDet = 0;
  for(int i=0;i<nDET&&!file.eof();i++){
    char id[4] = "   ", detectr[20] = "                   ";
    file>>id>>detectr;
    getline(file, line);
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
  delete ch;
  return nDet;
}

//////////////////////////////////////////////////////////////////
//Plots residual sensitivities to coil modulation signals after //
//detector data has been corrected using slopes. In this case,  //
//the slopes have been averaged over the same run ranges used in//
//correcting the data.                                          //
//                                                              // 
// avg_type: 0 by cycle, 1 by run, 2 by slug(not implemented)  // 
//                                                              // 
//Author: Don Jones                                             //
//Date: Jan 2014                                                //
//////////////////////////////////////////////////////////////////

void plotVariableRunRangeResiduals(Int_t start = 13842, Int_t end = 19000, 
				   Int_t saveToFile = 0, Int_t avg_type = 0,
				   Bool_t plot_sine = 1)
{
  TString DetectorList[nDET], MonitorList[nMON];
  Int_t nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList, 1);

  Double_t slug, run, cycle = 0;
  vector<vector<Double_t> >vList, vSlug, vSlugErr, vRun, vRunErr, 
    vCycle, vCycleErr;
  vList.resize(2);
  vSlug.resize(nMOD);
  vSlugErr.resize(nMOD);
  vRun.resize(nMOD);
  vRunErr.resize(nMOD);
  vCycle.resize(nMOD);
  vCycleErr.resize(nMOD);
  vector<vector<vector<Double_t> > > vSin, vSinErr, vCos, vCosErr;
  vSin.resize(nDet);
  vSinErr.resize(nDet);
  vCos.resize(nDet);
  vCosErr.resize(nDet);
  for(int idet=0;idet<nDet;++idet){
    vSin[idet].resize(nMOD);
    vSinErr[idet].resize(nMOD);
    vCos[idet].resize(nMOD);
    vCosErr[idet].resize(nMOD);
  }
  Double_t ***temp_res, ***temp_res_sq, N[nMOD][nSLUG]= {{0}};
  temp_res = new Double_t **[nDet];
  temp_res_sq = new Double_t **[nDet];
  for(int idet=0;idet<nDet;++idet){
    temp_res[idet] = new Double_t *[nMOD];
    temp_res_sq[idet] = new Double_t *[nMOD];
    for(int i=0;i<nMOD;++i){
      temp_res[idet][i] = new Double_t [nSLUG]();
      temp_res_sq[idet][i] = new Double_t [nSLUG]();
    }
  }

  //Get list of runs and slugs
  /////////////////////////////////////////////////
  ifstream runList(Form("%s/macros/runsBySlug.dat", 
			gSystem->Getenv("BMOD_SRC")));
  if(runList.is_open())cout<<"File list found.\n";
  else{
    cout<<"File list not found. Exiting.\n";
    return;
  }
  runList>>slug>>run;
  string line;
  getline(runList, line);
  while(!runList.eof()&&runList.good()&&run<=end){
    if(run>=start){
      vList[0].push_back(slug);
      vList[1].push_back(run);
      cout<<"slug: "<<vList[0].back()<<"  run: "<<vList[1].back()<<endl;
    }
    runList>>slug>>run;
    getline(runList, line);
  }
  /////////////////////////////////////////////////


  //Get residuals from files for each run.
  /////////////////////////////////////////////////
  for(int i = 0;i<(int)vList[0].size();++i){
    int slg = (int)vList[0][i];
    ifstream resFile(Form("%s/macrocycle_slopes/variable_run_range_resi"
			     "duals/residuals%i_ChiSqMin.set0.dat", 
			  gSystem->Getenv("BMOD_OUT"),(int)vList[1][i]));
    if(resFile.is_open() && resFile.good()){
      cout<<Form("%s/macrocycle_slopes/variable_run_range_residuals/"
		 "residuals%i_ChiSqMin.set0.dat ", gSystem->Getenv("BMOD_OUT"),
		 (int)vList[1][i])<<" found.\n";


      vector<vector<vector<Double_t> > >v_temp_res, v_temp_err;
      v_temp_res.resize(nDet);
      v_temp_err.resize(nDet);
      for(int idet=0;idet<nDet;++idet){
	v_temp_res[idet].resize(nCOIL);
	v_temp_err[idet].resize(nCOIL);
      }

      Header_t header;
      ExtractHeader(resFile, header);
      while(!resFile.eof() && resFile.good()){
	char det[255], pm[255];
	Int_t mod = header.mod_type;
	Double_t sin, sin_err, cos, cos_err;
	++N[mod][slg];
	for(int idet=0;idet<nDet;++idet){
	  resFile>>det>>sin>>pm>>sin_err>>cos>>pm>>cos_err;
	  getline(resFile, line);
	  double prev_val = temp_res[idet][mod][slg];
	  double new_val = (plot_sine ? sin*1.0e6 : cos*1.0e6);
	  temp_res[idet][mod][slg] = (prev_val*(N[mod][slg]-1)+new_val)/
	    N[mod][slg];
	  prev_val = temp_res_sq[idet][mod][slg];
	  new_val *= new_val; 
	  temp_res_sq[idet][mod][slg] = (prev_val*(N[mod][slg]-1)+new_val)/
	    N[mod][slg];
	  v_temp_res[idet][mod].push_back(sin*1.0e6);
	  v_temp_err[idet][mod].push_back(sin_err*1.0e6);
	  v_temp_res[idet][mod+nMOD].push_back(cos*1.0e6);
	  v_temp_err[idet][mod+nMOD].push_back(cos_err*1.0e6);
	}
	ExtractHeader(resFile, header);
	//	cout<<"EOF?"<<resFile.eof()<<endl;
	++cycle;
      }
      if(avg_type == 0){
	for(int imod=0;imod<nMOD;++imod){
	  for(int icyc=0;icyc<(int)v_temp_res[0][imod].size();++icyc){
	    vCycle[imod].push_back(cycle);
	    vCycleErr[imod].push_back(0);
	    for(int idet=0;idet<nDet;++idet){
	      vSin[idet][imod].push_back(v_temp_res[idet][imod][icyc]);
	      //vSinErr[idet][imod].push_back(v_temp_err[idet][imod][icyc]);
	      vSinErr[idet][imod].push_back(0);
	      vCos[idet][imod].push_back(v_temp_res[idet][imod+nMOD][icyc]);
	      // vCosErr[idet][imod].push_back(v_temp_err[idet][imod+nMOD][icyc]);
	      vCosErr[idet][imod].push_back(0);
// 	      cout<<DetectorList[idet].Data()<<" "<<vSin[idet][imod].back()<<
// 		"+/-"<<vSinErr[idet][imod].back()<<"      "<<
// 		vCos[idet][imod].back()<<
// 		"+/-"<<vCosErr[idet][imod].back()<<endl;
	    }
	    //	    cout<<endl;
	  }
	}
      }else if(avg_type == 1){
	Double_t mean[2] = {0,0};
	for(int imod=0;imod<nMOD;++imod){
	  int size = (int)v_temp_res[0][imod].size();
	  if(size >= MIN_ENTRIES_PER_RUN){
	    vRun[imod].push_back(vList[1][i]);
	    vRunErr[imod].push_back(0);
	    for(int idet=0;idet<nDet;++idet){
	      FindMeanAndError(v_temp_res[idet][imod].data(),
					    mean, size);
	      vSin[idet][imod].push_back(mean[0]);
	      vSinErr[idet][imod].push_back(mean[1]);
// 	      cout<<vSin[idet][imod].back()<<"+/-"<<
// 		vSinErr[idet][imod].back()<<endl;
	      FindMeanAndError(v_temp_res[idet][imod+nMOD].data(),
			       mean, size);
	      vCos[idet][imod].push_back(mean[0]);
	      vCosErr[idet][imod].push_back(mean[1]);
	    }
	  }
	}
	v_temp_res.clear();
	v_temp_err.clear();
      }
    }else{
      cout<<Form("$BMOD_OUT/macrocycle_slopes/variable_run_range_residuals/"
		 "residuals%i_ChiSqMin.set0.dat NOT found.\n", (int)vList[1][i]);
    }
  }
  if(avg_type == 2){
    for(int islug=0;islug<nSLUG;++islug){
      for(int imod=0;imod<nMOD;++imod){
	if(N[imod][islug] > MIN_ENTRIES_PER_SLUG){
	  vSlug[imod].push_back((double)islug);
	  vSlugErr[imod].push_back(0.0);
	  for(int idet=0;idet<nDet;++idet){
	    if(plot_sine){
	      vSin[idet][imod].push_back(temp_res[idet][imod][islug]);
	      double error = sqrt(temp_res_sq[idet][imod][islug] - 
				pow(temp_res[idet][imod][islug],2));
	      vSinErr[idet][imod].push_back(error/sqrt(N[imod][islug]));
	    }else{
	      vCos[idet][imod].push_back(temp_res[idet][imod][islug]);
	      double error = sqrt(temp_res_sq[idet][imod][islug] - 
				pow(temp_res[idet][imod][islug],2));
	      vCosErr[idet][imod].push_back(error/sqrt(N[imod][islug]));
	    }
	  }
	}
      }
    }
  }
  /////////////////////////////////////////////////


  //Plot results.
  /////////////////////////////////////////////////
  DetectorPlots plotSine(nDet, 2);
  DetectorPlots plotCosine(nDet, 2);
  if(avg_type == 0){
    if(plot_sine){
      plotSine.SetTitleX("Cycle_Number");
      plotSine.SetCoilType(0);
      plotSine.SetMarkerColor(0,kRed);
      plotSine.SetDetectorList(DetectorList, nDet);
      plotSine.SetMonitorList(MonitorList);
      plotSine.Plot(vCycle, vCycleErr, vSin, vSinErr);
      if(saveToFile)plotSine.SaveToFile();
    }else{
      plotCosine.SetTitleX("Cycle_Number");
      plotCosine.SetCoilType(1);
      plotCosine.SetMarkerColor(0,kBlue);
      plotCosine.SetDetectorList(DetectorList, nDet);
      plotCosine.SetMonitorList(MonitorList);
      plotCosine.Plot(vCycle, vCycleErr, vCos, vCosErr);
      if(saveToFile)plotCosine.SaveToFile();
    }
  }else if(avg_type == 1){
    if(plot_sine){
      plotSine.SetTitleX("Run_Number");
      plotSine.SetCoilType(0);
      plotSine.SetMarkerColor(0,kRed);
      plotSine.SetLineColor(0,kRed);
      plotSine.SetDetectorList(DetectorList, nDet);
      plotSine.SetMonitorList(MonitorList);
      plotSine.Plot(vRun, vRunErr, vSin, vSinErr);
      if(saveToFile)plotSine.SaveToFile();
    }else{
      plotCosine.SetTitleX("Run_Number");
      plotCosine.SetCoilType(1);
      plotCosine.SetMarkerColor(0,kBlue);
      plotCosine.SetLineColor(0,kBlue);
      plotCosine.SetDetectorList(DetectorList, nDet);
      plotCosine.SetMonitorList(MonitorList);
      plotCosine.Plot(vRun, vRunErr, vCos, vCosErr);
      if(saveToFile)plotCosine.SaveToFile();
    }
  }else if(avg_type == 2){
    if(plot_sine){
      plotSine.SetTitleX("Slug_Number");
      plotSine.SetCoilType(0);
      plotSine.SetMarkerColor(0,kRed);
      plotSine.SetLineColor(0,kRed);
      plotSine.SetDetectorList(DetectorList, nDet);
      plotSine.SetMonitorList(MonitorList);
      plotSine.Plot(vSlug, vSlugErr, vSin, vSinErr);
      if(saveToFile)plotSine.SaveToFile();
    }else{
      plotCosine.SetTitleX("Slug_Number");
      plotCosine.SetCoilType(1);
      plotCosine.SetMarkerColor(0,kBlue);
      plotCosine.SetLineColor(0,kBlue);
      plotCosine.SetDetectorList(DetectorList, nDet);
      plotCosine.SetMonitorList(MonitorList);
      plotCosine.Plot(vSlug, vSlugErr, vCos, vCosErr);
      if(saveToFile)plotCosine.SaveToFile();
    }
  }else cout<<"Invalid average type.\n";
  /////////////////////////////////////////////////


  return;
}
