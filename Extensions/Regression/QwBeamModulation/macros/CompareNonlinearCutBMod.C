#include "TFile.h"
#include "TChain.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TPaveStats.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TString.h"

#include <vector>
#include "math.h"
#include <iostream>
#include <fstream>
#include <utility>
//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: Mar 10, 2014
//Program compares results of BMod slopes and residuals for usual modulation
//and modulation with the nonlinear region cut.
///////////////////////////////////////////////////////////////////////////////
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

Int_t CompareNonlinearCutBMod(){
  gROOT->SetStyle("Plain");
  gStyle->SetTitleBorderSize(0);
  gStyle->SetLineWidth(2);
  gStyle->SetStatY(0.9);
  gStyle->SetStatH(0.25);
  gStyle->SetStatW(0.3);
  gStyle->SetOptStat("rmeou");

  const Int_t nDET = 16, nMON = 5, nMOD = 5, nCOIL = 10;
  TChain *ch = new TChain("slopes");
  ch->Add("/net/data1/paschkedata1/MacrocycleSlopesTree_ChiSqMin.set0.root");
  TChain *chnl = new TChain("slopes");
  chnl->Add("/net/data1/paschkedata1/MacrocycleSlopesTree_ChiSqMin.cut180deg_set0.root");
  TString detList[nDET] = {"mdallbars","md1barsum","md2barsum","md3barsum",
			   "md4barsum","md5barsum","md6barsum","md7barsum", 
			   "md8barsum","uslumi_sum", "uslumi1_sum", "uslumi3_sum",
			   "uslumi5_sum", "uslumi7_sum", "md9barsum", "pmtonl"};
  TString monList[nMON] = {"targetX","targetY","targetXSlope","targetYSlope",
			   "bpm3c12X"};
  TString slopeList[nDET][nMON];
  TString resList[nDET][nCOIL], coeffList[nDET][nCOIL];
  double slopes[nDET][nMON], resid[nDET][nCOIL], coeff[nDET][nCOIL],
    slopesnl[nDET][nMON], residnl[nDET][nCOIL], meansl[nDET][nMON],
    meancoeff[nDET][nCOIL], lBin, hBin;
  TH1D *hSlope[nDET][nMON];
  TH1D *hResid[nDET][nCOIL];
  for(int idet=0;idet<nDET;++idet){
    for(int imon=0;imon<nMON;++imon){
      meansl[idet][imon] = 0;
      slopeList[idet][imon] = detList[idet] + "_" + monList[imon];
      lBin = (detList[idet].Contains("pmt")? -1000:-50);
      if(monList[imon].Contains("Slope"))lBin*=10.0;
      hBin = TMath::Abs(lBin);
      hSlope[idet][imon]= new TH1D(Form("hSlope%i_%i",idet,imon),
			       Form("hSlope%i_%i",idet,imon),1000, lBin, hBin);

    }
  }
  for(int idet=0;idet<nDET;++idet){
    for(int icoil=0;icoil<nCOIL;++icoil){
      meancoeff[idet][icoil] = 0;
      if(icoil<nMOD){
	resList[idet][icoil] = "SineRes_"+detList[idet] +  Form("_Coil%i",icoil);
	resList[idet][icoil+nMOD] = "CosineRes_"+detList[idet] + 
	  Form("_Coil%i",icoil);
      }
      coeffList[idet][icoil] = detList[idet] +  Form("_Coil%i",icoil);
      lBin = (detList[idet].Contains("uslumi") ? -3000: 
	      (detList[idet].Contains("pmt") ? -15000: -2000));
      hBin = TMath::Abs(lBin);;
      hResid[idet][icoil]= new TH1D(Form("hResid%i_%i",idet,icoil),
			       Form("hResid%i_%i",idet,icoil),1000, lBin, hBin);
    }
  }

  TEventList *list = new TEventList("list","list",0,0);
  Double_t run, lEnt, hEnt, runnl, lEntnl, hEntnl;
  int good;
  ch->SetBranchAddress("run", &run);
  ch->SetBranchAddress("hEntry", &hEnt);
  ch->SetBranchAddress("lEntry", &lEnt);
  ch->SetBranchAddress("good", &good);
  chnl->SetBranchAddress("run", &runnl);
  chnl->SetBranchAddress("hEntry", &hEntnl);
  chnl->SetBranchAddress("lEntry", &lEntnl);
  for(int idet=0;idet<nDET;++idet){
    for(int imon=0;imon<nMON;++imon){
      ch->SetBranchAddress(slopeList[idet][imon].Data(),&slopes[idet][imon]);
      chnl->SetBranchAddress(slopeList[idet][imon].Data(),&slopesnl[idet][imon]);
    }
    for(int icoil=0;icoil<nCOIL;++icoil){
      ch->SetBranchAddress(resList[idet][icoil].Data(),&resid[idet][icoil]);
      chnl->SetBranchAddress(resList[idet][icoil].Data(),&residnl[idet][icoil]);
      ch->SetBranchAddress(coeffList[idet][icoil].Data(), &coeff[idet][icoil]);
    }
  }
  cout<<"Entries: "<<ch->GetEntries()<<" "<<chnl->GetEntries()<<endl;
  int n = 0, common = 0, nGood = 0;
  ch->GetEntry(0);
  for(int i=0;i<chnl->GetEntries();++i){
    if(i%100==0)cout<<"Processing entry "<<i<<endl;
    chnl->GetEntry(i);
    while(run<=runnl){
      ch->GetEntry(n);
      if(run==runnl && (lEnt>=lEntnl || hEnt>=hEntnl)){
	if(lEnt==lEntnl&&hEnt==hEntnl&&good){
	  for(int idet=0;idet<nDET;++idet){
	    for(int imon=0;imon<nMON;++imon){
	      meansl[idet][imon] += slopes[idet][imon];
	      hSlope[idet][imon]->Fill(slopes[idet][imon]-slopesnl[idet][imon]);
	    }
	    for(int icoil=0;icoil<nCOIL;++icoil){
	      meancoeff[idet][icoil] += resid[idet][icoil];
	      hResid[idet][icoil]->Fill(resid[idet][icoil]-residnl[idet][icoil]);
	    }
	  }
	  list->Enter(n);
	  common++;
	}
	break;
      }
      ++n;
    }
  }
  for(int idet=0;idet<nDET;++idet){
    for(int imon=0;imon<nMON;++imon)
      meansl[idet][imon] /= (double)common;
    for(int icoil=0;icoil<nCOIL;++icoil)
      meancoeff[idet][icoil] /= (double)common;
  }

  cout<<common<<" entries in common."<<nGood<<"\n";
  TCanvas *cMD[nMON];
  TCanvas *cBkg[nMON];
  Int_t order[9] = {5,4,1,2,3,6,9,8,7};
  for(int imon=0;imon<nMON;imon++){
    cMD[imon] = new TCanvas(Form("cMD%i",imon),monList[imon].Data(),
			    0,0,1500,1000); 
    cMD[imon]->Divide(3,3);
    cBkg[imon] = new TCanvas(Form("cBkg%i",imon),monList[imon].Data(),
			     0,0,1900,900); 
    cBkg[imon]->Divide(4,2);
    for(int idet=0;idet<nDET;++idet){
      if(idet<9)
	cMD[imon]->cd(order[idet])->SetLogy();
      else
	cBkg[imon]->cd(idet-8)->SetLogy();
      hSlope[idet][imon]->Draw();
      hSlope[idet][imon]->SetLineWidth(2);
      hSlope[idet][imon]->SetTitle(Form("Difference in Slope %s/%s",
				     detList[idet].Data(), monList[imon].Data()));
      hSlope[idet][imon]->GetXaxis()->SetTitle(Form("#Delta%s/%s(ppm/#mum)",
					detList[idet].Data(),
					monList[imon].Data()));
    }
    cMD[imon]->SaveAs(Form("~/plots/temp/slopes_%s.png",monList[imon].Data()));
  }

  TCanvas *crMD[nMON];
  TCanvas *crBkg[nMON];
  for(int icoil=0;icoil<nMOD;++icoil){
    crMD[icoil] = new TCanvas(Form("crMD%i",icoil), Form("CoilMD%i",icoil),
			    0,0,1500,1000); 
    crMD[icoil]->Divide(3,3);
    crBkg[icoil] = new TCanvas(Form("crBkg%i",icoil), Form("CoilBkg%i",icoil),
			    0,0,1900,1000); 
    crBkg[icoil]->Divide(4,2);
    for(int idet=0;idet<nDET;++idet){
      if(idet<9)
	crMD[icoil]->cd(order[idet])->SetLogy();
      else
	crBkg[icoil]->cd(idet-8)->SetLogy();
      hResid[idet][icoil]->Draw();
      hResid[idet][icoil]->SetLineColor(kRed);
      hResid[idet][icoil]->SetLineWidth(2);
      hResid[idet][icoil]->SetTitle(Form("Difference in Residual %s/Coil%i",
					 detList[idet].Data(), icoil));
      hResid[idet][icoil]->GetXaxis()->SetTitle(Form("#Delta%s/Coil%i(ppm)",
					detList[idet].Data(),icoil));
      gPad->Update();
      TPaveStats *stSin = (TPaveStats*)gPad->GetPrimitive("stats");
      stSin->SetY1NDC(0.35); //new y start position
      stSin->SetY2NDC(0.65); //new y end position
      stSin->SetTextColor(kRed);
      hResid[idet][icoil+nMOD]->Draw("sames");
      hResid[idet][icoil+nMOD]->SetLineWidth(2);
      hResid[idet][icoil+nMOD]->SetTitle(Form("Difference in Slope %s/Coil%i",
				     detList[idet].Data(),  icoil+nMOD));
      hResid[idet][icoil+nMOD]->GetXaxis()->SetTitle(Form("#Delta%s/Coil%i(ppm)",
					detList[idet].Data(), icoil+nMOD));
      gPad->Update();
    }
    crMD[icoil]->SaveAs(Form("~/plots/temp/residuals_coil%i.png",icoil));
  }

  cout<<"\n               ";
  for(int imon=0;imon<nMON;++imon)
    printf("      %s         ", monList[imon].Data()); 
  cout<<endl;
  for(int idet=0;idet<nDET;idet++){
    printf("%11s   ",detList[idet].Data());
    for(int imon=0;imon<nMON;++imon){
      int good = (TMath::Abs(hSlope[idet][imon]->GetMean())<
		  hSlope[idet][imon]->GetMeanError() ? 1 : 0);
      printf("%s%+7.4f +/- %6.4f%s    ",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     hSlope[idet][imon]->GetMean(), hSlope[idet][imon]->GetMeanError(),
	     ANSI_COLOR_RESET);
    }
    cout<<endl;
  }

  cout<<"\n                ";
  for(int imon=0;imon<nMON;++imon)
    printf("%s    ", monList[imon].Data()); 
  cout<<endl;
  for(int idet=0;idet<nDET;idet++){
    printf("%11s   ",detList[idet].Data());
    for(int imon=0;imon<nMON;++imon){
      printf("%+10.5f  ", meansl[idet][imon]);
    }
    cout<<endl;
  }

  cout<<"\n               ";
  for(int imon=0;imon<nMON;++imon)
    printf("      %s         ", monList[imon].Data()); 
  cout<<endl;
  for(int idet=0;idet<nDET;idet++){
    printf("%11s   ",detList[idet].Data());
    for(int imon=0;imon<nMON;++imon){
      int good = (TMath::Abs(hSlope[idet][imon]->GetMean())<
		  hSlope[idet][imon]->GetMeanError() ? 1 : 0);
      printf("%s%+7.4f +/- %7.4f    %s",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     hSlope[idet][imon]->GetMean()/meansl[idet][imon], 
	     hSlope[idet][imon]->GetMeanError()/TMath::Abs(meansl[idet][imon]),
	     ANSI_COLOR_RESET);
    }
    cout<<endl;
  }



  cout<<"\n            ";
  for(int icoil=0;icoil<nCOIL;++icoil)
    printf("        Coil%i          ", icoil); 
  cout<<endl;
  for(int idet=0;idet<nDET;idet++){
    printf("%11s   ",detList[idet].Data());
    for(int icoil=0;icoil<nCOIL;++icoil){
      int bad = (TMath::Abs(hResid[idet][icoil]->GetMean())>
		  hResid[idet][icoil]->GetMeanError() &&  
		  hResid[idet][icoil]->GetMean() > 0 ? 1 : 0);
      if(TMath::Abs(hResid[idet][icoil]->GetMean())<10)
	printf("%s%+7.4f +/- %7.4f    %s",(bad ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	       hResid[idet][icoil]->GetMean(), 
	       hResid[idet][icoil]->GetMeanError(),ANSI_COLOR_RESET);
      else if( TMath::Abs(hResid[idet][icoil]->GetMean())>=10 && 
			  TMath::Abs(hResid[idet][icoil]->GetMean())<100)
	printf("%s%+7.3f +/- %7.3f    %s",(bad ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	       hResid[idet][icoil]->GetMean(), 
	       hResid[idet][icoil]->GetMeanError(),ANSI_COLOR_RESET);
      else 
	printf("%s%+7.2f +/- %7.2f    %s",(bad ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	       hResid[idet][icoil]->GetMean(), 
	       hResid[idet][icoil]->GetMeanError(),ANSI_COLOR_RESET);
    }
    cout<<endl;
  }
  

  cout<<"\n            ";
  for(int icoil=0;icoil<nCOIL;++icoil)
    printf("  Coil%i      ", icoil); 
  cout<<endl;
  for(int idet=0;idet<nDET;idet++){
    printf("%11s   ",detList[idet].Data());
    for(int icoil=0;icoil<nCOIL;++icoil){
      printf("%+10.5f  ", meancoeff[idet][icoil]);
    }
    cout<<endl;
  }
  

  cout<<"\n            ";
  for(int icoil=0;icoil<nCOIL;++icoil)
    printf("         Coil%i       ", icoil); 
  cout<<endl;
  for(int idet=0;idet<nDET;idet++){
    printf("%11s   ",detList[idet].Data());
    for(int icoil=0;icoil<nCOIL;++icoil){
      int bad = (TMath::Abs(hResid[idet][icoil]->GetMean())>
		  hResid[idet][icoil]->GetMeanError() &&  
		  hResid[idet][icoil]->GetMean() > 0 ? 1 : 0);
      printf("%s%+0.1f +/- %0.1f    %s",(bad ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     hResid[idet][icoil]->GetMean()/meancoeff[idet][icoil],
	     hResid[idet][icoil]->GetMeanError()/
	     TMath::Abs(meancoeff[idet][icoil]), ANSI_COLOR_RESET);
    }
    cout<<endl;
  }
  



  return 0;
}
