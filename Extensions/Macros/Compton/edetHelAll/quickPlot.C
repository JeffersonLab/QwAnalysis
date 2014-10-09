#include "rootClass.h"
#include "comptonRunConstants.h"
Int_t quickPlot(Int_t run1 = 24503) {
  //gROOT->SetStyle("publication");

  TCanvas *cBgd = new TCanvas("cBgd",Form("bgd yield run"),0,0,900,600);
  cBgd->cd();
  cBgd->SetGrid();
  //TGraphErrors *grH0L0 = new TGraphErrors(Form("%s/%s/%sqNormCntsB1H1L0P%d.txt",pPath,www,filePre.Data(),plane),"%lg %lg %lg");
  //TGraphErrors *grH1L0 = new TGraphErrors(Form("%s/%s/%sqNormCntsB1H0L0P%d.txt",pPath,www,filePre.Data(),plane),"%lg %lg %lg");
  TGraphErrors *gr1 = new TGraphErrors(Form("/w/hallc/compton/users/narayan/my_scratch/www/run_%d/edetAll_%d_AcLasOffBkgdP1.txt",run1,run1),"%lg %lg %lg");
  TGraphErrors *gr2 = new TGraphErrors(Form("/w/hallc/compton/users/narayan/my_scratch/www/run_24762/edetAll_24762_AcLasOffBkgdP1.txt"),"%lg %lg %lg");
  TMultiGraph *gr = new TMultiGraph();
  gr1->SetLineColor(kBlue);
  //gr1->Draw("AP");
  gr2->SetLineColor(kRed);
  //gr2->Draw("P");
  gr->Add(gr1);
  gr->Add(gr2);
  gr->Draw("AP");
  gr->GetYaxis()->SetTitle("#frac{r24761}{r24762}");
  gr->GetXaxis()->SetTitle("strip number");
  gr->SetTitle();

  //cBgd->SaveAs(Form("r%d_bgd_%d.png",runnum,(int)tempBgdCut));
  cBgd->SaveAs(Form("bgd_temp.png"));

  return 1;
}
