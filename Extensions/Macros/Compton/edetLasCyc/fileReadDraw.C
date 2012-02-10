#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Int_t fileReadDraw(Int_t run) //TString *tobePlotted)
{
  Double_t strip[nStrips],expAsym[nStrips],asymEr[nStrips],asymRMS[nStrips];
  Double_t stripFort[nStrips],expAsymFort[nStrips],asymErFort[nStrips];

  TGraphErrors *grFort;
  TGraphErrors *grCpp;

  Double_t zero[nStrips];

  for (Int_t s=0; s <endStrip; s++) {
    zero[s]=0;
  }

  ifstream in1, in2;

  // Open file 1
  in1.open(Form("r%d_expAsymP1.txt",run));
  if(in1.is_open()) {
    for(Int_t s =0; s <endStrip; s++) {
      if (maskedStrips(0,s)) continue;
      /// Read data file
      while(in1.good()) {
	in1 >> strip[s]>>expAsym[s]>>asymEr[s]>>asymRMS[s];
	//printf("%f\t%f\t%f\t%f\n",strip[s],expAsym[s],asymEr[s],asymRMS[s]);
      }
    }
  }
  else printf("Note: didn't find the laserCycle based asym file \n");
  
  in2.open(Form("%d-plane-1.output",run));
  if(in2.is_open()) {
    for(Int_t s =0; s <endStrip; s++) {
      if (maskedStrips(0,s)) continue;
      /// Read data file
      while(in2.good()) {
	in2 >> stripFort[s]>>expAsymFort[s]>>asymErFort[s];
	//printf("%f\t%f\t%f\n",stripFort[s],expAsymFort[s],asymErFort[s]);
      }
    }
  }
  else printf("Note: didn't find the runlet based asym file \n");

  //TCanvas *cAsymCompared = new TCanvas("cAsymCompared","Asymmetry evaluation in Fort and Cpp",10,10,600,800);
  TCanvas *c1 = new TCanvas("c1","Comparision of Asymmetry evaluation",10,10,800,600);
  TLine *myline = new TLine(0,0,64,0);
  c1->cd();
  grFort = new TGraphErrors(endStrip,stripFort,expAsymFort,zero,asymErFort);
  grCpp = new TGraphErrors(endStrip,strip,expAsym,zero,asymEr);

//   cAsymCompared->Divide(1,2);
//   cAsymCompared->cd(1);
//   grFort->GetXaxis()->SetTitle("strip number");
//   grFort->GetYaxis()->SetTitle("asymmetry");
   grFort->SetTitle("runlet based asym evaluation");
//   grFort->SetMarkerColor(4);//SetLineColor(4);
//   grFort->SetMarkerStyle(23);
//   grFort->SetLineColor(4);
//   grFort->Draw("AP");

//   myline->SetLineStyle(1);
//   myline->Draw();

//   cAsymCompared->cd(2);
//   grCpp->SetMarkerSize(1);
//   grCpp->SetMarkerColor(2);
//   grCpp->SetLineColor(2);
//   grCpp->GetXaxis()->SetTitle("strip number");
//   grCpp->GetYaxis()->SetTitle("asymmetry");
   grCpp->SetTitle("laser cycle wise asym evaluation");
//   grCpp->SetMarkerStyle(22);
//   grCpp->Draw("A*");

//   myline->SetLineStyle(1);
//   myline->Draw();

//   cAsymCompared->Update();

  grCpp->GetXaxis()->SetTitle("strip number");
  grCpp->GetYaxis()->SetTitle("asymmetry");
  grCpp->SetMaximum(0.045);
  grCpp->SetMinimum(-0.045);

  grCpp->SetMarkerStyle(20);
  grCpp->SetLineColor(2);
  grCpp->SetMarkerColor(2);
  grCpp->Draw("AP");

  grFort->SetMarkerColor(4);
  grFort->SetMarkerStyle(24);
  grFort->SetLineColor(4);
  grCpp->Draw("P");

  myline->SetLineStyle(1);
  myline->Draw();

  c1->SaveAs(Form("expAsym_r%d.png",run));
  return run;
}
