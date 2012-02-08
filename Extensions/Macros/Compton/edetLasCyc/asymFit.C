//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry

#include <rootClass.h>
#include "theoryAsym.C"
#include "edetExpAsym.C"

void asymFit(Float_t theoExpRatio[nPlanes][nStrips], Float_t stripAsymRMS[nPlanes][nStrips])
{
  Bool_t debug2=0;
  Float_t calcAsym[nStrips];
  Float_t zero[nStrips],stripPlot[nStrips];
  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  
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
    myline->Draw();
    grTheoryAsym = new TGraphErrors(endStrip,stripPlot,calcAsym,zero,zero);
    grTheoryAsym->SetLineColor(4);
    grTheoryAsym->Draw("L");    
    cAsym->Update();
  } 
  if(debug2) cout<<"\nstrip#\t\ttheoExpRatio\tcalcAsym\tstripAsym"<<endl;

  for (Int_t p =startPlane; p <endPlane; p++) {  
    for (Int_t s =startStrip; s <endStrip; s++) {  
      if (maskedStrips(p,s)) continue;
      if (stripAsym[p][s]!=0.0) theoExpRatio[p][s]= stripAsym[p][s]/calcAsym[s];
      if(debug2) printf("theoExpRatio[%d][%d]:%f = %f / %f\n",p,s,theoExpRatio[p][s],calcAsym[s],stripAsym[p][s]);
    }    
  }
}

/*Comments
 *I'm currently including my rootClass.h and comptonRunConstants.h separately in every file
 *..there must be a way to do it non-repetatively.
 *I am doing this for all strips, I should limit myself to 64 strips at max
 *'m not using the asymStripEr in any way yet
 */
