#include <rootClass.h>
#include "comptonRunConstants.h"

Int_t fileReadDraw(Int_t run) 
{
  TGraphErrors *grCpp;//,*grFort;
  ifstream in1, in2;
  TLegend *leg;
  TCanvas *c1 = new TCanvas("c1",Form("edet Asymmetry run:%d",run),10,10,1000,600);
  TLine *myline = new TLine(0,0,64,0);
  grCpp = new TGraphErrors(Form("analOut/r%d_expAsymP1.txt",run), "%lg %lg %lg");
  grCpp->GetXaxis()->SetTitle("strip number");
  grCpp->GetYaxis()->SetTitle("asymmetry");
  grCpp->SetTitle("laserCycle wise edet asym");
  grCpp->SetMaximum(0.05);
  grCpp->SetMinimum(-0.05);
  c1->SetGridx(1);
  
//   grFort->GetXaxis()->SetTitle("strip number");
//   grFort->GetYaxis()->SetTitle("asymmetry");
//   grFort->SetMarkerColor(4);
//   grFort->SetMarkerStyle(24);
//   grFort->SetLineColor(4);
//   grFort->Draw("AP");

  grCpp->SetMarkerStyle(20);
  grCpp->SetLineColor(2);
  grCpp->SetMarkerColor(2);
  grCpp->Draw("AP");

  myline->SetLineStyle(1);
  myline->Draw();

  //TGraphErrors *grFort;
  //TGraphErrors *grFort;
  leg = new TLegend(0.1,0.7,0.4,0.9);
  leg->AddEntry(grCpp,"laserWise eDet Asymmetry","lpf");
  leg->AddEntry(myline,"zero line","l");
  leg->Draw();

  c1->SaveAs(Form("analOut/r%d_edetAsym.png",run));
  return run;
}
