//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry

#include <rootClass.h>
#include "theoryAsym.C"
//#include "comptonRunConstants.h"///don't reinclude

void asymFit() {
  vector<Float_t> asymStrip; //contains the theoretically evaluated asymmetry
  Float_t stripPlot[nStrips],zero[nStrips];
  theoryAsym(asymStrip);
  Float_t theoryAsym[nStrips];

  //  TCanvas *cTheoryAsym = new TCanvas("cTheory","theoretical asym Vs Strip#",10,10,600,600);
  
  TGraphErrors *grTheoryAsym;

  for (Int_t s=0; s <nStrips; s++) {
    stripPlot[s]=s+1;
    zero[s]=0;
  }

  for (Int_t s =startStrip; s <endStrip; s++) {
    theoryAsym[s]=asymStrip.at(s); //apparently TGraph can't handle vectors
  }
  
  grTheoryAsym = new TGraphErrors(endStrip,stripPlot,theoryAsym,zero,zero);
  grTheoryAsym->GetXaxis()->SetTitle("strip number");
  grTheoryAsym->GetYaxis()->SetTitle("theoretical asym");
  grTheoryAsym->SetTitle("theoretical asymmetry Vs strip number");
  grTheoryAsym->Draw("AL");
  //  cTheoryAsym->
}

