#include <rootClass.h>
#include "asymFit.C"
#include "comptonRunConstants.h"
#include "fileReadDraw.C"

void polFit(Int_t runnum)
{
  Float_t theoExpRatio[nPlanes][nStrips], stripAsymRMS[nPlanes][nStrips];
  Float_t zero[nStrips],stripPlot[nStrips];

  asymFit(runnum,theoExpRatio);//,stripAsymRMS);

  TGraphErrors *grPolPlane[nPlanes];
             
  TCanvas *cPol = new TCanvas("cPol","Pol.measured by each Strip",10,10,700,700); 
  cPol->Divide(2,2);  

  for (Int_t s=0; s <endStrip; s++) { 
    stripPlot[s]=s+1;
    zero[s]=0;
  }                                                                

  for (Int_t p =startPlane; p <endPlane; p++) { 
    cPol->cd(p+1);
    grPolPlane[p] = new TGraphErrors(endStrip,stripPlot,theoExpRatio[p],zero,stripAsymRMS[p]);
    grPolPlane[p]->GetXaxis()->SetTitle("strip number");
    grPolPlane[p]->GetYaxis()->SetTitle("ratioExpTheory"); 
    grPolPlane[p]->SetTitle(Form("Plane %d",p+1));      
    grPolPlane[p]->SetMarkerStyle(20);
    grPolPlane[p]->SetMarkerColor(kRed+2);
    grPolPlane[p]->Draw("AP");                      
  }
  cPol->Update();
  fileReadDraw(runnum);  
}
  
