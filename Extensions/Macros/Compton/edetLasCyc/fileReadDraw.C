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
	//printf("%f\t%f\t%f\t%f\n",strip[s],expAsym[s],asymEr[s],asymRMS[s]);
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
	//printf("%f\t%f\t%f\n",stripFort[s],expAsymFort[s],asymErFort[s]);
      }
    } 
    else printf("didn't find the file 2 \n");
  }

  //  TCanvas *cAsymCompared = new TCanvas("cAsymCompared","Asymmetry evaluation in Fort and Cpp",10,10,600,800);
  TCanvas *c1 = new TCanvas("c1","Comparision of Asymmetry evaluation",10,10,800,600);
  TLine *myline = new TLine(0,0,64,0);

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
   grCpp->SetTitle("laser based asym evaluation");
//   grCpp->SetMarkerStyle(22);
//   grCpp->Draw("A*");

//   myline->SetLineStyle(1);
//   myline->Draw();

//   cAsymCompared->Update();

  grFort->GetXaxis()->SetTitle("strip number");
  grFort->GetYaxis()->SetTitle("asymmetry");
  grFort->SetMaximum(0.02);
  grFort->SetMinimum(-0.05);

  grFort->SetMarkerColor(4);
  grFort->SetMarkerStyle(24);
  grFort->SetLineColor(4);
  grFort->Draw("AP");

  grCpp->SetMarkerStyle(20);
  grCpp->SetLineColor(2);
  grCpp->SetMarkerColor(2);
  grCpp->Draw("P");

  myline->SetLineStyle(1);
  myline->Draw();

  return 1;
}
 
// grCpp->SetTitle("C++ evaluated asymm compared to Fortran");
