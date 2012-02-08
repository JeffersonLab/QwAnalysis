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

  ifstream in1;
  ifstream in2;

  // Open file 1
  in1.open(Form("r%d_expAsymP1.txt",run));

  for(Int_t s =0; s <endStrip; s++) {
    if (maskedStrips(0,s)) continue;
    /// Read data file
    if(in1.is_open()) {
      if(in1.good()) {
	in1 >> strip[s]>>expAsym[s]>>asymEr[s]>>asymRMS[s];
	//printf("%f\t%f\t%f\t%f\n",strip[s],expAsym[s],asymEr[s],asymRMS[s]); //!?why do I get an extra line
      }
    }
    else printf("didn't find the file 1 \n");    
  }
  
  in2.open(Form("%d-plane-1.output",run));
  for(Int_t s =0; s <endStrip; s++) {
    if (maskedStrips(0,s)) continue;
    /// Read data file
    if(in2.is_open()) {
      if(in2.good()) {
	in2 >> stripFort[s]>>expAsymFort[s]>>asymErFort[s];
	//printf("%f\t%f\t%f\n",stripFort[s],expAsymFort[s],asymErFort[s]); //!?why do I get an extra line
      }
    } 
    else printf("didn't find the file 2 \n");
  }

  TCanvas *cAsymCompared = new TCanvas("cAsymCompared","Comparision of Asymmetryevaluation",10,10,800,1000);
  TLine *myline = new TLine(0,0,60,0);
  cAsymCompared->Divide(1,2);
  cAsymCompared->cd(1);
  grFort = new TGraphErrors(endStrip,stripFort,expAsymFort,zero,asymErFort);
  grFort->GetXaxis()->SetTitle("strip number");
  grFort->GetYaxis()->SetTitle("asymmetry");
  grFort->SetTitle("C++Fortran evaluated asymm");
  grFort->SetMarkerStyle(21);
  grFort->Draw("A*");

  myline->SetLineStyle(1);
  myline->Draw();

  cAsymCompared->cd(2);
  grCpp = new TGraphErrors(endStrip,strip,expAsym,zero,asymEr);
  grCpp->SetMarkerStyle(24);//,Size_t 24);
  grCpp->SetLineColor(4);
  grCpp->GetXaxis()->SetTitle("strip number");
  grCpp->GetYaxis()->SetTitle("asymmetry");
  grCpp->SetTitle("C++ evaluated asymm");
  grCpp->Draw("A*");

  myline->SetLineStyle(1);
  myline->Draw();

  cAsymCompared->Update();

  TCanvas *c1 = new TCanvas("c1","Comparision of Asymmetryevaluation",10,10,800,1000);
  grFort->Draw("A*");
  grCpp->Draw("*");

  myline->SetLineStyle(1);
  myline->Draw();


  return 1;
}
 
// grCpp->SetTitle("C++ evaluated asymm compared to Fortran");
