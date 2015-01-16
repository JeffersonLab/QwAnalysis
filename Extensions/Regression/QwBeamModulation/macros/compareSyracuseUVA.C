#include <vector>
#include "TString.h"
#include "TProfile.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TChain.h"
#include "TMath.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TFile.h"
#include "TTree.h"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
using namespace std;


void compareSyracuseUVA(){

  const int nMON = 5, nDET = 9;
  TChain *ch1 = new TChain("slopes");
  ch1->Add("/net/data1/paschkedata1/MacrocycleSlopesTree_ChiSqMin.set0.root");
  TChain *ch2 = new TChain("mps_slug");
  ch2->Add("/net/data1/paschkedata1/Syracuse_slope_tree_original_monitors_20140616.root");
  char *detector[nDET] = {"md2barsum","md3barsum","md4barsum","md1barsum","mdallbars","md5barsum","md8barsum","md7barsum","md6barsum"};
  char *monitor[nMON] = {"targetX","targetXSlope","bpm3c12X","targetY","targetYSlope"};
  vector<double>slug;
  double slopes1[nDET][nMON][300], slopes2[nDET][nMON][300], diff[nDET][nMON][300];
  TH1D *h[50000];
  int n = 0, start = 137;
  double max[nDET][nMON][3], min[nDET][nMON][3];
  for(int idet=0;idet<nDET;++idet){
    for(int imon=0;imon<nMON;++imon){
      for(int i=0;i<3;i++){
	max[idet][imon][i] = 0;
	min[idet][imon][i] = 0;
      }
    }
  }
  int ent = 0;
  for(int i=start; i<322;++i){
    cout<<"Slug: "<<i<<" n="<<n<<endl;
    if(ch1->GetEntries(Form("slug==%i&&good",i))>10 && 
       ch2->GetEntries(Form("slug==%i",i))>10){
      slug.push_back(i);
      for(int idet=0;idet<nDET;++idet){
	for(int imon=0;imon<nMON;++imon){
	  ch1->Draw(Form("%s_%s>>h%i",detector[idet],monitor[imon],n),
		    Form("slug==%i&&good",i),"goff");
	  //	  printf("%s_%s>>h%i\n",detector[idet],monitor[imon],n);
	  h[n] = (TH1D*)gDirectory->Get(Form("h%i",n));
	  if(max[idet][imon][0]<h[n]->GetMean() || i == start)
	    max[idet][imon][0] = h[n]->GetMean();
	  if(min[idet][imon][0]>h[n]->GetMean() || i == start)
	    min[idet][imon][0] = h[n]->GetMean();
	  slopes1[idet][imon][ent] = h[n]->GetMean();
	  ++n;
	  ch2->Draw(Form("slope_%s_%s>>h%i",detector[idet],monitor[imon],n),
		    Form("slug==%i&&run!=14424&&run!=14200",i),"goff");
	  h[n] = (TH1D*)gDirectory->Get(Form("h%i",n));
	  slopes2[idet][imon][ent] = h[n]->GetMean();
	  if(max[idet][imon][1]<h[n]->GetMean() || i == start)
	    max[idet][imon][1] = h[n]->GetMean();
	  if(min[idet][imon][1]>h[n]->GetMean() || i == start)
	    min[idet][imon][1] = h[n]->GetMean();
	  ++n;
	  diff[idet][imon][ent] = slopes1[idet][imon][ent] - 
				     slopes2[idet][imon][ent];
	  if(max[idet][imon][2]<diff[idet][imon][ent] || i ==start)
	    max[idet][imon][2] = diff[idet][imon][ent];
	  if(min[idet][imon][2]>diff[idet][imon][ent] || i ==start)
	    min[idet][imon][2] = diff[idet][imon][ent];
	}
      }
      ++ent;
    }

  }

  TCanvas *c[nMON];
  TGraph *gr[nDET][nMON];
  n=0;
  double *x = &slug[0];
  cout<<"size: "<<slug.size()<<endl; 
  for(int imon=0;imon<nMON;++imon){
    c[imon] = new TCanvas(Form("c%i",imon),Form("c%i",imon),0,0,1400,1000);
    c[imon]->Divide(3,3);
    for(int idet=0;idet<nDET;++idet){
      c[imon]->cd(idet+1);
      gr[idet][imon] = new TGraph((int)slug.size(), x, diff[idet][imon]);
      gr[idet][imon]->SetTitle(Form("%s_%s vs slug (UVA-SYR)", 
				    detector[idet],monitor[imon]));
      gr[idet][imon]->SetMarkerColor(kRed);
      gr[idet][imon]->SetMarkerStyle(7);
      gr[idet][imon]->Draw("ap");
      gPad->Update();
      gr[idet][imon]->GetXaxis()->SetTitle("Slug");
      gr[idet][imon]->GetYaxis()->SetTitle(Form("%s_%s ", 
				    detector[idet],monitor[imon]));
      gPad->Update();
    }
  }






}
//   TCanvas *cHist[nMON];
//   TH1D *hDiff[nDET][nMON];
//   for(int imon=0;imon<nMON;++imon)
//     for(int idet=0;idet<nDET;++idet)
//       hDiff[idet][imon] = new hDiff(Form("hDiff%i",n),Form("hDiff%i",n),100,min[idet][imon][2],max[idet][imon][2]);

//   for(int imon=0;imon<nMON;++imon){
//     cDiff[imon] = new TCanvas(Form("cDiff%i",imon),Form("cDiff%i",imon),
// 			      0,0,1400,1000);
//     cDiff[imon]->Divide(3,3);
//     for(int idet=0;idet<nDET;++idet){
//       cDiff->cd(idet+1);
//       for(int i=0;i<(int)slopes1[idet][imon].size();++i)
// 	hDiff[idet][imon]->Fill(diff[idet][imon][i]);
//       hDiff[idet][imon]->Draw();
//     }
//   }

// }
