#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TMath.h"
#include "TString.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TTree.h"
#include "TEventList.h"
#include "TGraphErrors.h"
#include "TH1D.h"

const int nDET = 60, nMON = 5, nMOD = 5, nSLUG = 400, MAX_PER_SLUG = 500;
typedef struct leaf_t{
  int slug_lo;
  int slug_hi;
  int run_lo;
  int run_hi;
};

Int_t FindRunRanges(Int_t **ranges ){
  //ranges format is range[slug]={sluglo,slughi,runlo,runhi};
  int run, slug;
  TChain *ch = new TChain("tree");
  ch->Add(Form("%s/run1/HYDROGEN-CELL_offoff_tree.root", gSystem->Getenv("DB_ROOTFILES")));
  ch->Add(Form("%s/run2/HYDROGEN-CELL_offoff_tree.root", gSystem->Getenv("DB_ROOTFILES")));
  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("run_number",1);
  ch->SetBranchStatus("slug",1);
  ch->SetBranchAddress("run_number", &run);
  ch->SetBranchAddress("slug", &slug);

  //get run ranges for each slug
  for(int islug = 0;islug<322;++islug){
    ch->Draw(">>list",Form("slug==%i",islug),"goff");
    TEventList *list = (TEventList*)gDirectory->Get("list");
    ch->GetEntry(list->GetEntry(0));
    int runlo = run, runhi = run;
    if(list->GetN()>0){
      for(int i=1;i<list->GetN();++i){
	ch->GetEntry(list->GetEntry(i));
	if(run<runlo)runlo = run;
	if(run>runhi)runhi = run;
      }
      ranges[islug][2] = runlo;
      ranges[islug][3] = runhi;
    }
    ranges[islug][0] = islug;
    ranges[islug][1] = islug;
//     cout<<ranges[islug][0]<<" "<<ranges[islug][1]<<" "<<ranges[islug][2]
// 	<<" "<<ranges[islug][3]<<" "<<endl;

  }

  //now analyze for overlap/interleaving

  vector<leaf_t>vRange;
  for(int islug=42;islug<322;++islug){
    if(ranges[islug][2]==0)continue;
    int runlo, runhi, sluglo, slughi;
    sluglo = ranges[islug][0];
    slughi = ranges[islug][1];
    runlo = ranges[islug][2];
    runhi = ranges[islug][3];
    for(int i = islug+1;i<(islug+10>nSLUG ? islug+10: nSLUG);++i){
      if(ranges[i][2]<runhi &&ranges[i][2]!=0){
	slughi = ranges[i][0];
	runlo = (ranges[i][2]<runlo ? ranges[i][2] : runlo);
	runhi = (ranges[i][3]>runhi ? ranges[i][3] : runhi);
      }
    }
    leaf_t temp;
    temp.slug_lo = sluglo;
    temp.slug_hi = slughi;
    temp.run_lo = runlo;
    temp.run_hi = runhi;
    vRange.push_back(temp);
    islug = slughi;
  }

  for(int i=0;i<(int)vRange.size();++i){
    ranges[i][0] = vRange[i].slug_lo;
    ranges[i][1] = vRange[i].slug_hi;
    ranges[i][2] = vRange[i].run_lo;
    ranges[i][3] = vRange[i].run_hi;
    cout<<i<<": "<<vRange[i].slug_lo<<" "<<vRange[i].slug_hi<<" "
	<<vRange[i].run_lo<<" "<<vRange[i].run_hi<<endl;

  }

  delete ch;
  return (int)vRange.size();
}

Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList,
				 Bool_t trunc, char* config){
  string line;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug", Form("%s/mps_only_friend_13993.root",
				 gSystem->Getenv("MPS_ONLY_ROOTFILES")));

  ifstream file(config);

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
  for(int i=0;i<nDET;i++){
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
    if(file.eof())break;
  }
  delete ch;
  return nDet;
}

void FindAverageAndRMS(Double_t *x, Double_t *avg, Int_t n){
  Double_t mean = 0, rms = 0;
  for(int i=0;i<n;i++){
    mean += x[i] / (Double_t)n;
  }
  avg[0] = mean;
  for(int i=0;i<n;i++){
    rms += (x[i] - mean)*(x[i] - mean) / (Double_t)n;
  }
  avg[1] = TMath::Sqrt(rms);
  return;
}


void makeSlugAveragedSlopes(char* stem ="", Bool_t chi_sq_min = 1, Bool_t writeSl = 1, Bool_t plotSl = 0)
{

  //If only plotting is desired, set writeSl = 0, if only writing is desired
  //set plotSl = 0;
  Int_t **runRanges = new Int_t*[nSLUG];
  for(int i=0;i<nSLUG;++i){
    runRanges[i] = new Int_t[4];
    runRanges[i][0] = 0;
    runRanges[i][1] = 0;
    runRanges[i][2] = 0;
    runRanges[i][3] = 0;
  }
  cout<<"Finding run ranges.\n";
  Int_t nRanges = FindRunRanges(runRanges);
  Int_t minRunsPerSlug = 2;

  gStyle->SetTitleSize(0.9);
  gStyle->SetTitleFontSize(0.1);
  gStyle->SetTitleTextColor(kBlack);

  TChain *slopes = new TChain("slopes");
  slopes->Add(Form("%s/../MacrocycleSlopesTree%s.set0%s.root",
		   gSystem->Getenv("MPS_ONLY_ROOTFILES"), (chi_sq_min ? "_ChiSqMin" : ""),
		   stem));
  TString MonitorList[nMON], MonitorListFull[nMON];
  TString DetectorList[nDET], DetectorListFull[nDET];

  char* configFile = Form("%s/config/setup_mpsonly%s.config",
			  gSystem->Getenv("BMOD_SRC"), stem);
  GetMonitorAndDetectorLists(MonitorListFull, DetectorListFull, 0, configFile);
  Int_t nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList,1,configFile);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return;
  }

  Double_t detSlopes[nDET][nMON], detSlopesErr[nDET][nMON], slug = 0, run;
  Int_t good;
  slopes->SetBranchAddress("slug", &slug);
  slopes->SetBranchAddress("run", &run);
  slopes->SetBranchAddress("good", &good);
  //  cout<<"Status:"<<slopes->GetBranchStatus("good")<<endl;

  bool all_here = 1;
  for(int i=0;i<nMON;++i){
    if(slopes->GetLeaf(Form("mdallpmtavg_%s", MonitorList[i].Data()))==0){
      all_here = 0;
      break;
    }
  }
  if(all_here){
    DetectorList[nDet] = "mdallpmtavg";
    DetectorListFull[nDet] = "mdallpmtavg";
    ++nDet;
  }


  for(int imon=0;imon<nMON;imon++){
    for(int idet=0;idet<nDet;idet++){
      slopes->SetBranchAddress(Form("%s_%s",DetectorList[idet].Data(),
				    MonitorList[imon].Data()),
			       &detSlopes[idet][imon]);
//       slopes->SetBranchAddress(Form("%s_%s_err",DetectorList[idet].Data(),
// 				    MonitorList[imon].Data()),
// 			       &detSlopesErr[idet][imon]);
    }
  }
  Double_t DetectorSlopes[nDET][nMON][nSLUG], 
    DetectorSlopesErr[nDET][nMON][nSLUG];
  Double_t Slug[nSLUG], SlugErr[nSLUG];
  Double_t dSlopesTmp[nDET][nMON][MAX_PER_SLUG], 
    dSlopesTmpErr[nDET][nMON][MAX_PER_SLUG];

  Int_t lRun = 0, hRun = 0, nSlug = 0, lSlug, hSlug;
  for(int irange=0;irange<nRanges;irange++){
    lSlug = runRanges[irange][0];
    hSlug = runRanges[irange][1];
    lRun = runRanges[irange][2];
    hRun = runRanges[irange][3];
    slopes->Draw(">>list",Form("good&&run>=%i&&run<=%i",lRun, hRun),"goff");
    TEventList *list = (TEventList*)gDirectory->Get("list");
    if(list->GetN()<minRunsPerSlug)continue;
    for(int i=0;i<list->GetN();++i){
      slopes->GetEntry(list->GetEntry(i));
      for(int idet=0;idet<nDet && writeSl;idet++){
	for(int imon=0;imon<nMON;imon++){
	  dSlopesTmp[idet][imon][i] = detSlopes[idet][imon];
	  dSlopesTmpErr[idet][imon][i] = detSlopesErr[idet][imon];
	}
      }
    }
    for(int idet=0;idet<nDet && writeSl;idet++){
      for(int imon=0;imon<nMON;imon++){
	double avg[2] = {0,0};
	FindAverageAndRMS(dSlopesTmp[idet][imon],avg,list->GetN());
	DetectorSlopes[idet][imon][nSlug] = avg[0];
	DetectorSlopesErr[idet][imon][nSlug] = avg[1]/sqrt((double)list->GetN());
      }
    }
    Slug[nSlug] = (lSlug + hSlug) / 2.0;
    SlugErr[nSlug] = 0;
    /////CHANGE THESE LINES TO CHANGE OUPUT FILE FORMAT/////
    //Slug range is from lSlug to hSlug
    //Run range is from lRun to hRun

    Int_t order[5] = {0,3,1,4,2};
    TString output_dir = Form("%s%s", gSystem->Getenv("BMOD_OUT"),stem);
    for(int idet=0;idet<nDet && writeSl;idet++){
//       printf("::%s/slopelists/slopeList_asym_%s.%i-%i.dat\n",
// 	     output_dir.Data(), DetectorListFull[idet].Data(), 
// 	     lRun, hRun);
      FILE *f = fopen(Form("%s/slopelists/slopeList_asym_%s.%i-%i.dat",
			   output_dir.Data(), DetectorListFull[idet].Data(), 
			   lRun, hRun),"w");
      if(f==NULL)perror("Error opening file");
      fprintf(f,"!Cut used to extract these slopes :: \"good && run >= %i && "
	      "run <= %i\" or \"good && slug >= %i && slug <= %i\"\n",
	      lRun, hRun, lSlug, hSlug);
      for(int imon=0;imon<nMON;imon++){
	fprintf(f,"%s %14.7e %12.5e\n", MonitorList[order[imon]].Data(),
		DetectorSlopes[idet][order[imon]][nSlug],
		DetectorSlopesErr[idet][order[imon]][nSlug]);
      }
      fprintf(f,"Number of runs used in the calculation :: %i\n", list->GetN());
      fclose(f);
    }
    /////////////////////////////////////////////////////////
    ++nSlug;
  }









  if(!plotSl) return;  
  TCanvas *cMD[nMON];
  TCanvas *cBkg[nMON];
  TCanvas *cChg[nMON];

  TGraphErrors *gr[nDET*nMON];

  int iplot = 0, idx = 0;
  for(int imon=0;imon<nMON;imon++){
    int pad1[9] = {5,4,1,2,3,6,9,8,7}, pad2 = 1, pad3 = 1;
    cMD[iplot] = new TCanvas(Form("cMD_%i",iplot),
			     Form("cMD%s",MonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i",iplot),
			      Form("cBkg%s",MonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i",iplot),
			     Form("charge%s",MonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);
    for(int idet=0;idet<nDet;idet++){
      if(DetectorList[idet].Contains("md")&& !DetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(DetectorList[idet].Contains("lumi")||
	       DetectorList[idet].Contains("pmt")||
	       DetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	pad3++;
      }
      gr[idx] = new TGraphErrors(nSlug,Slug,DetectorSlopes[idet][imon],
				 SlugErr,DetectorSlopesErr[idet][imon]);
      gr[idx]->Draw("ap");
      gr[idx]->SetTitle(Form("Slope %s / %s",
			     DetectorList[idet].Data(),
			     MonitorList[imon].Data()));
      gr[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 DetectorList[idet].Data(),
					 MonitorList[imon].Data(),
					 (MonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      gr[idx]->GetXaxis()->SetTitle("Slug");
      double size = 0.049;
      gr[idx]->GetXaxis()->SetTitleSize(size);
      gr[idx]->GetYaxis()->SetTitleSize(size);
//       gr[idx]->GetXaxis()->SetTitleColor(col[idet]);
//       gr[idx]->GetYaxis()->SetTitleColor(col[idet]);
      gr[idx]->GetXaxis()->SetTitleOffset(0.85);
      gr[idx]->GetYaxis()->SetTitleOffset(0.85);
      gr[idx]->SetMarkerStyle(7);
      gr[idx]->SetMarkerColor(kRed);
      gr[idx]->SetLineColor(kRed);
      gr[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }

  gROOT->ForceStyle();
  
}
