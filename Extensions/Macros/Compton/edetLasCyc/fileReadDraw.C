#include <rootClass.h>
#include "comptonRunConstants.h"

Int_t fileReadDraw(Int_t run) 
{
  TGraphErrors *grCpp,*grFort;
  ifstream in1, in2;
  TLegend *leg;
  TCanvas *c1 = new TCanvas("c1",Form("edet Asymmetry run:%d",run),10,10,1000,600);
  TLine *myline = new TLine(0,0,71,0);
  ifstream fortranOutP1;
  leg = new TLegend(0.1,0.7,0.4,0.9);

  grCpp = new TGraphErrors(Form("analOut/r%d_expAsymP1.txt",run), "%lg %lg %lg");

  grCpp->GetXaxis()->SetTitle("strip number");
  grCpp->GetYaxis()->SetTitle("asymmetry");
  grCpp->SetTitle("laserCycle wise edet asym");
  grCpp->SetMaximum(0.05);
  grCpp->SetMinimum(-0.05);
  c1->SetGridx(1);
  
  grCpp->SetMarkerStyle(20);
  grCpp->SetLineColor(2);
  grCpp->SetMarkerColor(2);
  grCpp->SetFillColor(0);
  grCpp->Draw("AP");

  myline->SetLineStyle(1);
  myline->Draw();

  fortranOutP1.open(Form("%d-plane-1-nc.output",run));
  if(fortranOutP1.is_open()) {
    cout<<"found runlet based raw asymmetry file"<<endl;
    grFort = new TGraphErrors(Form("%d-plane-1.output",run), "%lg %lg %lg");
    grFort->SetMarkerColor(4);
    grFort->SetMarkerStyle(24);
    grFort->SetFillColor(0);
    grFort->SetLineColor(4);
    grFort->Draw("P");
    leg->AddEntry(grFort,"runlet based eDet Asymmetry","lpf");
  }
  else cout<<"corresponding fortran file for run "<<run<<" doesn't exist"<<endl;
  
  leg->AddEntry(grCpp,"laserWise eDet Asymmetry","lpf");
  leg->AddEntry(myline,"zero line","l");
  leg->SetFillColor(0);
  leg->Draw();


  c1->SaveAs(Form("analOut/r%d_edetAsym.png",run));
  return run;
}
