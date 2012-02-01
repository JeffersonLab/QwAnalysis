//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry

#include <rootClass.h>
#include "theoryAsym.C"
#include "edetExpAsym.C"
//#include "comptonRunConstants.h"///don't reinclude

void asymFit() {
  Int_t runnum=23351;
  Float_t calcAsym[nStrips],zero[nStrips],stripPlot[nStrips];
  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsymRMS[nPlanes][nStrips];

  theoryAsym(calcAsym);
  edetExpAsym(runnum,runnum,stripAsym,stripAsymEr,stripAsymRMS);
  
  TGraphErrors *grTheoryAsym;
  TGraphErrors *grAsymPlane[nPlanes];

  for (Int_t s=0; s <endStrip; s++) {
    stripPlot[s]=s+1;
    zero[s]=0;
  }
  
  if(IHWP_in) {
    for (Int_t s=0; s <endStrip; s++) {
      calcAsym[s] = - calcAsym[s];
    }
  }

  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",10,10,800,800);
  cAsym->Divide(2,2);
  
  TLine *myline = new TLine(0,0,60,0);
  myline->SetLineStyle(1);
  
  for (Int_t p =startPlane; p <endPlane; p++) {
    cAsym->cd(p+1);
    grAsymPlane[p] = new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymRMS[p]);
    grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
    grAsymPlane[p]->SetTitle(Form("Plane %d",p+1));
    grAsymPlane[p]->Draw("A*");
    grTheoryAsym = new TGraphErrors(endStrip,stripPlot,calcAsym,zero,zero);
    grTheoryAsym->Draw("L");    
    myline->Draw();
    cAsym->Update();
  }  
}

/*Comments
 *I'm currently including my rootClass.h and comptonRunConstants.h separately in every file
 *..there must be a way to do it non-repetatively.
 *I am doing this for all strips, I should limit myself to 64 strips at max
 *'m not using the asymStripEr in any way yet
 */
