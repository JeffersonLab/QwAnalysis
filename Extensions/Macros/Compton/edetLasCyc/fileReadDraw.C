#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Int_t fileReadDraw(Int_t run) //TString *tobePlotted)
{
  //Float_t stripFort[nStrips],expAsymFort[nStrips],asymErFort[nStrips];
  Float_t strip[nStrips],expAsym[nStrips],asymEr[nStrips],asymRMS[nStrips];
  Float_t zero[nStrips];
  Bool_t debug=1;
  //TGraphErrors *grFort;
  TGraphErrors *grCpp;

  for (Int_t s=0; s <endStrip; s++) {
    zero[s]=0.0;
  }

  ifstream in1;
  ifstream in2;

  // Open file 1
  in1.open(Form("analOut/r%d_expAsymP1.txt",run));
  if(in1.is_open()) {   
    cout<<"Reading file "<<Form("analOut/r%d_expAsymP1.txt",run)<<endl;
    for(Int_t s =0; s <endStrip; s++) {
      if (maskedStrips(0,s)) continue;
      /// Read data file
      if(in1.good()) {
	in1 >> strip[s]>>expAsym[s]>>asymEr[s]>>asymRMS[s];
	if(debug) cout<<strip[s]<<"\t"<<expAsym[s]<<"\t"<<asymEr[s]<<"\t"<<asymRMS[s]<<endl;
      }
    }
  }
  else printf("didn't find the file 1 \n");
  
//   in2.open(Form("%d-plane-1.output",run));
//   for(Int_t s =0; s <endStrip; s++) {
//     if (maskedStrips(0,s)) continue;
//     /// Read data file
//     if(in2.is_open()) {
//       if(in2.good()) {
// 	in2 >> stripFort[s]>>expAsymFort[s]>>asymErFort[s];
// 	//printf("%f\t%f\t%f\n",stripFort[s],expAsymFort[s],asymErFort[s]);
//       }
//     } 
//     else printf("didn't find the file 2 \n");
//   }

  TCanvas *c1 = new TCanvas("c1",Form("Comparision of Asym evaluation run:%d",run),10,10,1000,600);
  TLine *myline = new TLine(0,0,64,0);
  //  c1->cd();
  //   grFort = new TGraphErrors(endStrip,stripFort,expAsymFort,zero,asymErFort);
   grCpp = new TGraphErrors(endStrip,strip,expAsym,zero,asymEr);
   grCpp->GetXaxis()->SetTitle("strip number");
   grCpp->GetYaxis()->SetTitle("asymmetry");
   grCpp->SetTitle("laser based asym evaluation");
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

  return run;
}

/*
 * This graph somehow draws some points corresponding to x=0, the real data is just as I would want
 * ..these points(=0) are not showing up now if I am limit my Y-scale, but may show for some other run
 */
