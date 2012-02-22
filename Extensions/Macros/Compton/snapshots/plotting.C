#include <algorithm>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "iostream"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "THStack.h"
#include "TProfile.h"
#include "TF1.h"
#include "TLine.h"

#include "globals.C"

#ifndef _PLOTTING_C_
#define _PLOTTING_C_

void drawALLtypeTH1(TH2F* bighist, Bool_t printstats=1, TString title="", Bool_t logy=0, TString filename="") {
  std::cout << "drawALLtypeTH1:  (" << bighist->GetName() << ")  " << bighist->GetTitle() << std::endl;
  if (bighist->GetEntries() <= 0) {
    std::cout << "drawALLtypeTH1:  No entries" << std::endl;
    return;
  }
  static const Bool_t debug=0;
  TLegend* leg = 0;
  TH1D* hist[numtypes];
  Bool_t first=1;
    //Int_t colors[numtypes] = {kGreen, kBlue, kRed, kViolet};

  Double_t entries[numtypes];
  Double_t maxi=0, mini=1;//e12;
  Int_t xbinmin=1000, xbinmax=0;
  for (Int_t i=1; i<=numtypes; i++) {
    hist[i-1] = bighist->ProjectionX(Form("%s_type%i",bighist->GetName(),i),i,i);
    entries[i-1] = hist[i-1]->GetEntries();
    if (debug) std::cout << "drawALLtypeTH1:  type " << i << " has " << entries[i-1] << " entries" << std::endl;
    if (entries[i-1]>0) {
      Int_t xbinmintemp = hist[i-1]->FindFirstBinAbove(0, 1);
      Int_t xbinmaxtemp = hist[i-1]->FindLastBinAbove(0, 1);
      xbinmin = std::min(xbinmintemp,xbinmin);
      xbinmax = std::max(xbinmaxtemp,xbinmax);
      maxi = std::max(maxi, hist[i-1]->GetMaximum());
      //      mini = std::min(mini, hist[i-1]->GetMinimum());
      //mini = std::min(mini, hist[i-1]->GetBinContent(hist[i-1]->GetMinimumBin()));
      hist[i-1]->SetLineColor(colors[i-1]);
      if (title=="") {
        hist[i-1]->SetTitle(bighist->GetTitle());
      } else {
        hist[i-1]->SetTitle(title.Data());
      }
    }
  }
  xbinmin -= 1;
  xbinmax += 1;
  std::cout << "drawALLtypeTH1:  min " << mini << ", max " << maxi << std::endl;
  for (Int_t i=1; i<=numtypes; i++) {
    if (debug) std::cout << "drawALLtypeTH1:  type " << i << std::endl;
     if (entries[i-1]>0) {
       if (debug) std::cout << "drawALLtypeTH1:  type " << i << " has " << entries[i-1] << " entries > 0" << std::endl;
       if (first) {
        if (debug) std::cout << "drawALLtypeTH1:  bin range " << xbinmin << " to " << xbinmax << std::endl;
        hist[i-1]->GetXaxis()->SetRange(xbinmin,xbinmax);
        if (mini!=0 && mini < 1e12) hist[i-1]->SetMinimum(mini);
         if (logy) {
          gPad->SetLogy(logy);
          hist[i-1]->SetMaximum(10*maxi);
		  hist[i-1]->SetMinimum(1);
        } else {
          hist[i-1]->SetMaximum(1.2*maxi);          
        }
         hist[i-1]->DrawCopy();
        if (printstats) {
		  gPad->SetTopMargin(0.2);
          leg = new TLegend(0.2,0.7,0.99,0.99);
        } else {
          leg = new TLegend(0.7,0.75,0.99,0.99);
        }
        leg->SetTextSizePixels(20);
        leg->SetFillColor(kWhite);
         first=0;
       } else {
         hist[i-1]->DrawCopy("same");
       }
      if (printstats) {
        leg->AddEntry(hist[i-1],Form("%s <%.3g> RMS=%.3g",titles[i-1],hist[i-1]->GetMean(),hist[i-1]->GetRMS()),"l");
      } else {
        leg->AddEntry(hist[i-1],Form("%s",titles[i-1]),"l");
      }
     } else {
       if (debug) std::cout << "drawALLtypeTH1:  type " << i << " has " << entries[i-1] << " entries <= 0" << std::endl;
     }
  }
  leg->Draw();

  FILE *outfile;
  if (filename!="") {
    std::cout << "opening " << filename << " for writing." << std::endl;
    outfile = fopen(filename.Data(),"w");
    for (Int_t i=1; i<=numtypes; i++) {
      if (entries[i-1]>0) {
        fprintf(outfile, "type%i  mean %13.7g    RMS %13.7g    entries %7f\n",
            i,hist[i-1]->GetMean(),hist[i-1]->GetRMS(),hist[i-1]->GetEntries());
      }
    }
    fclose(outfile);
  }
}


void drawslicesTH1(TH2F* bighist, Bool_t printstats = 1, TString title = "", Bool_t /*logy*/ = 0, Int_t startbin = 0, Int_t endbin = 0) 
{
  std::cout << "drawslicesTH1:  (" << bighist->GetName() << ")  " << bighist->GetTitle() << std::endl;
  if (bighist->GetEntries() <= 0) {
    std::cout << "drawslicesTH1:  No entries" << std::endl;
    return;
  }
  static const Bool_t debug=0;
  static const Int_t maxbins = 16;
  TH1D* hist[maxbins];
  Int_t num;
  if (startbin!=0 && endbin!=0) {
    num=endbin-startbin;
  } else {
    num=bighist->GetYaxis()->GetNbins();
  }
  if (num > maxbins) {
    std::cout << "drawslicesTH1:  only plotting the first " << maxbins << " bins of " << num << std::endl;
    num=maxbins;
  }
  THStack *hs = 0;

  Bool_t first=1;
    //Int_t colors[num] = {kGreen, kBlue, kRed, kViolet};
  Double_t entries[maxbins];
  Double_t maxi=0, mini=1e12;
  // ****  The first loop creates and prepares each of the 1D histograms
  for (Int_t i=1; i<=num; i++) {
    hist[i-1] = bighist->ProjectionX(Form("%s_ybin%i",bighist->GetName(),i),i,i);
    entries[i-1] = hist[i-1]->GetEntries();
    if (debug) std::cout << "drawslicesTH1:  type " << i << " has " << entries[i-1] << " entries" << std::endl;
    if (entries[i-1]>0) {
      maxi = std::max(maxi, hist[i-1]->GetMaximum());
      //      mini = std::min(mini, hist[i-1]->GetMinimum());
      mini = std::min(mini, hist[i-1]->GetBinContent(hist[i-1]->GetMinimumBin()));
      hist[i-1]->SetLineColor(i);
      hist[i-1]->SetFillColor(i);
      if (title=="") {
        hist[i-1]->SetTitle(bighist->GetTitle());
        hs = new THStack("hs",bighist->GetTitle());
      } else {
        hist[i-1]->SetTitle(title.Data());
        hs = new THStack("hs",title.Data());
      }
    }
  }
  // ****  The second loop creates and prepares each of the 1D histograms
  std::cout << "drawALLtypeTH1:  min " << mini << ", max " << maxi << std::endl;
  for (Int_t i=1; i<=num; i++) {
    if (debug) std::cout << "drawslicesTH1:  type " << i << std::endl;
     if (entries[i-1]>0) {
       if (debug) std::cout << "drawslicesTH1:  type " << i << " has " << entries[i-1] << " entries > 0" << std::endl;
       if (first) {
        hist[i-1]->SetMaximum(1.2*maxi);
        if (mini!=0 && mini < 1e12) hist[i-1]->SetMinimum(mini);
         //hist[i-1]->DrawCopy();
        hs->Add(hist[i-1]);
//         gPad->SetLogy(logy);
//         if (printstats) {
//           leg = new TLegend(0.4,0.75,0.99,0.99);
//         } else {
//           leg = new TLegend(0.7,0.75,0.99,0.99);
//         }
//         leg->SetFillColor(kWhite);
         first=0;
        
       } else {
         //hist[i-1]->DrawCopy("same");
        hs->Add(hist[i-1]);
       }
//       if (printstats) {
//         leg->AddEntry(hist[i-1],Form("%s <%.3g> RMS=%.3g",titles[i-1],hist[i-1]->GetMean(),hist[i-1]->GetRMS()),"l");
//       } else {
//         leg->AddEntry(hist[i-1],Form("%s",titles[i-1]),"l");
//       }
     } else {
       if (debug) std::cout << "drawslicesTH1:  type " << i << " has " << entries[i-1] << " entries <= 0" << std::endl;
     }
  }
  hs->Draw();
//   leg->Draw();
}



void projectALLtypeTH1(TH3F* bighist, Bool_t printstats = 0, TString title = "", Bool_t /*logy*/ = 0, Int_t rebinnum = 1, Bool_t dowide = 0) 
{
  std::cout << "projectALLtypeTH1:  (" << bighist->GetName() << ") " << bighist->GetTitle() << std::endl;
  if (bighist->GetEntries() <= 0) {
    std::cout << "projectALLtypeTH1:  No entries" << std::endl;
    return;
  }
  Bool_t first=1;

  //Int_t colors[numtypes] = {kGreen, kBlue, kRed, kViolet};
//  char titles[numtypes][20] = {"laser on", "laser off", "beam off", "laser flash"};
  if (title!="") {
    bighist->SetTitle(title.Data());
  } else {
    title=bighist->GetTitle();
  }
  TLegend* leg = 0;
  TH2D* hist2D[numtypes];
  TH1D* typehist[numtypes];
  TProfile* histprof[numtypes];
  //TH1D* hist1D[numtypes];
  Double_t entries[numtypes];
  Float_t ymax=-1e12, ymin=1e12;
  //Float_t ymaxproj=-1e12, yminproj=1e12, ymaxevents=-1e12, yminevents=1e12;
  //Float_t ymintemp, ymaxtemp;
  Int_t yminbin, ymaxbin;
  char name[255];

  TH2D* hist2DALL = (TH2D*)bighist->Project3D("yx");
  TProfile* histprofALL = hist2DALL->ProfileX();
  ymin = histprofALL->GetMinimum();
  ymax = histprofALL->GetMaximum();
  yminbin = histprofALL->GetMinimumBin();
  ymaxbin = histprofALL->GetMaximumBin();
  std::cout << "projectALLtypeTH1:  minbin " << yminbin << "  minval " << ymin << ";"
          << "    maxbin " << ymaxbin << "  maxval " << ymax << std::endl;
  if (ymax != 0)   ymax = ymax + 0.15*std::fabs(ymax - ymin);

  sprintf(name,"%s",bighist->GetName());
  for (Int_t i=numtypes; i>=1; i--) {//  for (Int_t i=1; i<=numtypes; i++) {
    bighist->GetZaxis()->SetRange(i,i);
    //hist2D->gDirectory->Get(Form("%s_xy%i",name,i));
    hist2D[i-1] = (TH2D*)bighist->Project3D(Form("yx%i",i));
    typehist[i-1] = (TH1D*)bighist->Project3D(Form("y%i",i));
    entries[i-1] = hist2D[i-1]->GetEntries();
    if (entries[i-1]>0) {
//       Int_t yminbin = hist2D[i-1]->FindFirstBinAbove(0, 2);
//       Int_t ymaxbin = hist2D[i-1]->FindLastBinAbove(0, 2);
//       Float_t ymintemp = hist2D[i-1]->GetYaxis()->GetBinLowEdge(yminbin-2);
//       Float_t ymaxtemp = hist2D[i-1]->GetYaxis()->GetBinUpEdge(ymaxbin+2);

//        yminbin = typehist[i-1]->FindFirstBinAbove(0, 1);
//        ymaxbin = typehist[i-1]->FindLastBinAbove(0, 1);
//       ymintemp = typehist[i-1]->GetXaxis()->GetBinLowEdge(yminbin-2);
//       ymaxtemp = typehist[i-1]->GetXaxis()->GetBinUpEdge(ymaxbin+2);
//       ymaxevents = std::max(ymaxevents, ymaxtemp);
//       yminevents = std::min(yminevents, ymintemp);
//       std::cout << "projectALLtypeTH1:  " << i << "  minbin " << yminbin << "  minval " << ymintemp 
//                  << "  maxbin " << ymaxbin << "  maxval " << ymaxtemp
//                  << "   (" << yminevents << "," << ymaxevents << ")" << std::endl;
 
      if (rebinnum>1) {
        hist2D[i-1]->RebinX(rebinnum);
      }
      histprof[i-1] = hist2D[i-1]->ProfileX();
      histprof[i-1]->SetLineColor(colors[i-1]);
      histprof[i-1]->SetMarkerColor(colors[i-1]);
      histprof[i-1]->SetMarkerStyle(20);
      histprof[i-1]->SetMarkerSize(0.2);
      histprof[i-1]->SetTitle(title.Data());
      histprof[i-1]->GetXaxis()->SetLabelSize(0.08);
      histprof[i-1]->GetYaxis()->SetLabelSize(0.08);
      histprof[i-1]->GetXaxis()->SetTitleSize(0.08);
      histprof[i-1]->GetYaxis()->SetTitleSize(0.08);
      histprof[i-1]->GetXaxis()->SetTitleOffset(0.6);
      histprof[i-1]->GetYaxis()->SetTitleOffset(0.6);
      histprof[i-1]->GetXaxis()->CenterTitle();
//       Int_t yminbin = histprof[i-1]->GetMinimumBin();
//       Int_t ymaxbin = histprof[i-1]->GetMaximumBin();
//       Float_t ymintemp = histprof[i-1]->GetBinContent(yminbin);
//       Float_t ymaxtemp = histprof[i-1]->GetBinContent(ymaxbin);

//       hist1D[i-1] = histprof[i-1]->ProjectionX();
//        yminbin = hist1D[i-1]->GetMinimumBin();
//        ymaxbin = hist1D[i-1]->GetMaximumBin();
//       ymintemp = hist1D[i-1]->GetMinimum();
//       ymaxtemp = hist1D[i-1]->GetMaximum();
//       ymaxproj = std::max(ymaxproj, ymaxtemp);
//       yminproj = std::min(yminproj, ymintemp);
//       std::cout << "projectALLtypeTH1:  " << i << "  minbin " << yminbin << "  minval " << ymintemp 
//                  << "  maxbin " << ymaxbin << "  maxval " << ymaxtemp
//                  << "   (" << yminproj << "," << ymaxproj << ")" << std::endl;
    } 
  }
//   ymax = std::min(1.1 * ymaxproj, ymaxevents);
//   ymin = std::max(yminproj, yminevents);
  std::cout << "projectALLtypeTH1:  min " << ymin << ", max " << ymax << std::endl;
  for (Int_t i=numtypes; i>=1; i--) {//  for (Int_t i=1; i<=numtypes; i++) {
    if (entries[i-1]>0) {
      if (first) {
        //gPad->SetLogy(logy);
        //histprof[i-1]->SetMaximum(250);
        histprof[i-1]->GetYaxis()->SetRangeUser(ymin,ymax);
        //if (ymin!=0 && ymin < 1e12) histprof[i-1]->SetMinimum(ymin);
        histprof[i-1]->DrawCopy();
        if (printstats) {
          leg = new TLegend(0.4,0.75,0.99,0.99);
        } else {
          if (dowide) {
            leg = new TLegend(0.50,0.83,0.99,0.97);
            leg->SetNColumns(5);
          } else {
            leg = new TLegend(0.85,0.7,0.99,0.99);
          }
        }
        leg->SetFillColor(kWhite);
        first=0;
      } else {
        histprof[i-1]->DrawCopy("same");
      }
      if (printstats) {
        leg->AddEntry(histprof[i-1],Form("%s <%.3g> RMS=%.3g",titles[i-1],histprof[i-1]->GetMean(),
                         histprof[i-1]->GetRMS()),"l");
      } else {
        leg->AddEntry(histprof[i-1],Form("%s",titles[i-1]),"l");
      }
    }
  }
  //leg->Draw();
  gPad->SetMargin(0.06,0.02,0.1,0.1);
}

void projectALLtypeTH1andfit(TH3F* bighist, Bool_t printstats = 1, TString title = "", 
    Bool_t /*logy*/ = 0, Int_t rebinnum = 1, const char *textfilename = "projectALLtypeTH1andfit_default.txt")
{
  std::cout << "projectALLtypeTH1andfit:  (" << bighist->GetName() << ")  " << bighist->GetTitle() << std::endl;
  if (bighist->GetEntries() <= 0) {
    std::cout << "projectALLtypeTH1andfit:  No entries" << std::endl;
    return;
  }
  Bool_t first=1;
  //Int_t colors[numtypes] = {kGreen, kBlue, kRed, kViolet};
//  char titles[numtypes][20] = {"laser on", "laser off", "beam off", "laser flash"};
  if (title!="") {
    bighist->SetTitle(title.Data());
  }
  TLegend *leg = 0;
  TH2D* hist2D[numtypes];
  TProfile* histprof[numtypes];
  Double_t entries[numtypes];
  Float_t maxi=0, mini=1e12;
  char name[255];
  sprintf(name,"%s",bighist->GetName());

  std::cout << textfilename << " for writing." << std::endl;
  FILE *outfile;
  outfile = fopen(textfilename,"w");
  for (Int_t i=numtypes; i>=1; i--) {
    bighist->GetZaxis()->SetRange(i,i);
    hist2D[i-1] = (TH2D*)bighist->Project3D(Form("fit_yx%i",i));
    entries[i-1] = hist2D[i-1]->GetEntries();
    std::cout << "type " << i << " has " << entries[i-1] << " entries" << std::endl;
    if (entries[i-1]>0) {
      if (rebinnum>1) {
        hist2D[i-1]->RebinX(rebinnum);
      }
      histprof[i-1] = hist2D[i-1]->ProfileX();
      histprof[i-1]->SetLineColor(colors[i-1]);
      Int_t minbin = histprof[i-1]->GetMinimumBin();
      Int_t maxbin = histprof[i-1]->GetMaximumBin();
      Float_t mincont = histprof[i-1]->GetBinContent(minbin);
      Float_t maxcont = histprof[i-1]->GetBinContent(maxbin);
      maxi = std::max(maxi, maxcont);
      mini = std::min(mini, mincont);
      std::cout << "projectALLtypeTH1andfit:   " << i << "  min " << minbin << "  " << mincont
                   << "  max " << maxbin << "  " << maxcont
                  << "(" << mini << "," << maxi << ")" << std::endl;
      fitpol1[i-1] = new TF1(Form("fitpol%i",i),"[1]*x + [0]");
//       fitpol1[i-1]->FixParameter(0,4.5);
//       fitpol1[i-1]->FixParameter(1,4.2e-06);
      fitpol1[i-1]->SetParNames("intercept","slope");
      fitpol1[i-1]->SetLineWidth(2);
      fitpol1[i-1]->SetLineColor(colors[i-1]+2);
      Float_t histmean = hist2D[i-1]->GetMean();
      Float_t histRMS = hist2D[i-1]->GetRMS();
      Float_t minrange = histmean-2.0*histRMS;
      Float_t maxrange = histmean+2.0*histRMS;
      std::cout << i << "  mean " << histmean << " RMS " << histRMS
          << " gives range (" << minrange << "," << maxrange << ")" << std::endl; 
       fitpol1[i-1]->SetRange(minrange,maxrange);
       histprof[i-1]->Fit(fitpol1[i-1],"R");
//      histprof[i-1]->Fit(fitpol1[i-1]); // NB change back
      fprintf(outfile, "type %i inter    %+10.4g %10.4g\n",i,fitpol1[i-1]->GetParameter(0),fitpol1[i-1]->GetParError(0));
      fprintf(outfile, "type %i slope    %+10.4g %10.4g\n",i,fitpol1[i-1]->GetParameter(1),fitpol1[i-1]->GetParError(1));

    } 
  }
//  std::cout << "projectALLtypeTH1:  min " << mini << ", max " << maxi << std::endl;
  for (Int_t i=numtypes; i>=1; i--) {//  for (Int_t i=1; i<=numtypes; i++) {
    if (entries[i-1]>0) {
      if (first) {
        //gPad->SetLogy(logy);
        histprof[i-1]->SetMaximum(1.3*maxi);
        if (mini!=0 && mini < 1e12) histprof[i-1]->SetMinimum(mini);
        histprof[i-1]->Draw();
        if (printstats) {
          leg = new TLegend(0.5,0.6,0.99,0.99);
        } else {
          leg = new TLegend(0.7,0.75,0.99,0.99);
        }
        leg->SetFillColor(kWhite);
        first=0;
      } else {
        histprof[i-1]->Draw("same");
      }
      if (printstats) {
        leg->AddEntry(histprof[i-1],Form("%s p0=%.3g",titles[i-1],fitpol1[i-1]->GetParameter(0)),"l");
        leg->AddEntry(histprof[i-1],Form("%s p1=%.3g",titles[i-1],fitpol1[i-1]->GetParameter(1)),"l");
      } else {
        leg->AddEntry(histprof[i-1],Form("%s",titles[i-1]),"l");
      }
    }
  }
  leg->Draw();
  fclose(outfile);
}


void draw1typeTH2(TH3F* bighist, Int_t type, Bool_t printstats = 0, TString title = "", Bool_t logz = 0, Int_t rebinx = 1, Int_t rebiny = 1) 
{
  gPad->SetLogz(logz);
  std::cout << "draw1typeTH2:  (" << bighist->GetName() << ")  " << bighist->GetTitle() << "   for " << type << std::endl;
  //TLegend *leg = 0;
  TH2D* hist2D;
  Double_t entries;
  char name[255];
  sprintf(name,"%s",bighist->GetName());
  if (type>0 && type<=numtypes) { // if a valid typw was chosen
	  bighist->GetZaxis()->SetRange(type,type);
  }
  hist2D = (TH2D*)bighist->Project3D(Form("yx_%i",type));
  entries = hist2D->GetEntries();
  if (entries>0) {
    if (title!="") {
      hist2D->SetTitle(title.Data());
    }
    if (rebinx>1)  hist2D->RebinX(rebinx);
    if (rebiny>1)  hist2D->RebinY(rebiny);
    Int_t xbinmin = hist2D->FindFirstBinAbove(0, 1);
    Int_t xbinmax = hist2D->FindLastBinAbove(0, 1);
    Int_t yminbin = hist2D->FindFirstBinAbove(0, 2);
    Int_t ymaxbin = hist2D->FindLastBinAbove(0, 2);
    hist2D->GetXaxis()->SetRange(xbinmin,xbinmax);
    hist2D->GetYaxis()->SetRange(yminbin,ymaxbin);

    hist2D->GetXaxis()->SetLabelSize(0.06);
    hist2D->GetYaxis()->SetLabelSize(0.06);
    hist2D->GetXaxis()->SetTitleSize(0.06);
    hist2D->GetYaxis()->SetTitleSize(0.06);
    hist2D->GetXaxis()->SetTitleOffset(0.8);
    hist2D->GetYaxis()->SetTitleOffset(1.1);
    
    hist2D->DrawCopy("colz");
    gPad->SetMargin(0.15,0.12,0.1,0.1);
  } 
}

TH1F* drawscalesubbytypeTH1(TH2F* hist, Int_t type1=1, Int_t type2=2, Float_t intmin=0, Float_t intmax=0, 
							Bool_t logy=0, Bool_t dotitles=1, Bool_t plotbelownormonly=0, Float_t plotmaxbinval = 0) {
  gPad->SetLogy(logy);
  TLegend *leg = 0;
  Bool_t scaleandsub;
  Double_t int1, int2;
  Int_t intminbin, intmaxbin;
  Double_t maxi;
  char name[255];
  sprintf(name,"%s",hist->GetName());
  TH1F *hist1 = (TH1F*)hist->ProjectionX(Form("%shist1",name),type1,type1);
  TH1F *hist2 = (TH1F*)hist->ProjectionX(Form("%shist2",name),type2,type2);
  if (dotitles==0) {
    hist1->SetTitle(";;;");
    std::cout << "not doing titles" << std::endl;
  } else {
    std::cout << "title is: " << hist1->GetTitle() << std::endl;
  }
  hist1->SetLineColor(colors[type1-1]);
  hist1->SetMarkerColor(colors[type1-1]);
  hist2->SetLineColor(colors[type2-1]);
  hist2->SetMarkerColor(colors[type2-1]);
  // intmin and intmax = 0 implies integrate the whole spectrum
  if (intmin==0 && intmax==0) {
    intminbin = 1;
    intmaxbin = hist1->GetNbinsX();
    int1 = hist1->Integral();
    int2 = hist2->Integral();
  } else {
    intminbin = hist1->FindBin(intmin);
    intmaxbin = hist1->FindBin(intmax);
    int1 = hist1->Integral(intminbin,intmaxbin);
    int2 = hist2->Integral(intminbin,intmaxbin);
  }
  std::cout << "drawscalesubbytypeTH1: integrating from " << intmin << " to " << intmax << ", "
      << "bins " << intminbin << " to " << intmaxbin << ", "
      << "integral  int1 " << int1 << "   int2 " << int2 << std::endl;
//   int1 = hist->Integral(intminbin,intmaxbin,type1,type1);
//   int2 = hist->Integral(intminbin,intmaxbin,type2,type2);
  if (int1 > 20 && int2 > 20) {
    scaleandsub = 1;
  }
  else scaleandsub = 0;
  TH1F *hsub = (TH1F*)hist1->Clone();
  if (scaleandsub) {
    hist2->Scale(int1/int2); 
    hsub->Add(hist2,-1);
    hsub->SetLineColor(kBlack);
    hsub->SetMarkerColor(kBlack);
    hsub->SetMinimum(1);    
  }  
  if (scaleandsub && logy==0) {
	  if (plotmaxbinval==0) 
		  maxi = 1.1 * hsub->GetMaximum();
	  else {
		  maxi = 2 * hsub->GetBinContent(hsub->FindBin(plotmaxbinval));
	  }
	  std::cout << "drawscalesubbytypeTH1: setting maximum to " << maxi << " from sub" << std::endl;
  } else {
	  if (plotmaxbinval==0) 
		  maxi = 1.1 * std::max(hist1->GetMaximum(),hist2->GetMaximum());
	  else {
		  maxi = 2 * hsub->GetBinContent(hsub->FindBin(plotmaxbinval));
	  }
	  std::cout << "drawscalesubbytypeTH1: setting maximum to " << maxi << std::endl;
  }
  hist1->SetMaximum(maxi);
  Int_t xbinmin = hist1->FindFirstBinAbove(0, 1);
  Int_t xbinmax = hist1->FindLastBinAbove(0, 1);
  if (plotbelownormonly) {
	  xbinmax = intminbin;
  }
  hist1->GetXaxis()->SetRange(xbinmin-10,xbinmax+10);
  hist1->DrawCopy("h");
  hist2->DrawCopy("h,same");
  if (dotitles) {
    leg = new TLegend(0.7,0.75,0.99,0.99);
    leg->AddEntry(hist1,"laser on","l");
    leg->AddEntry(hist2,"laser off","l");
  }
  TLine t1;
  t1.SetLineColor(kRed);
  Double_t lineymax = 0.1*maxi;
  Double_t xpos1 = hist1->GetXaxis()->GetBinLowEdge(intminbin);
  Double_t xpos2 = hist1->GetXaxis()->GetBinUpEdge(intmaxbin);
  std::cout << "drawscalesubbytypeTH1: low and high line pos " << xpos1 << " and " << xpos2 << std::endl;
  if (intminbin < xbinmax && intminbin > xbinmin) 
    t1.DrawLine(xpos1,0,xpos1,lineymax);
  if (intmaxbin < xbinmax && intmaxbin > xbinmin) 
    t1.DrawLine(xpos2,0,xpos2,lineymax);
  if (scaleandsub) {
    hsub->DrawCopy("h,same");
    if (dotitles) leg->AddEntry(hsub,"laser on-off","l");
  }
  if (dotitles) {
    leg->SetFillColor(kWhite);
    leg->Draw();
  }
  if (scaleandsub) {
    return hsub;
  } else {
    return hist2;
  }
}

#endif

/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
