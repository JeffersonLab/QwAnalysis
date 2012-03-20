/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 * As of now, this script only calculates the ratio, and needs more fine tuning
 */

#include <rootClass.h>
#include "asymFit.C"
#include "comptonRunConstants.h"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, Bool_t first100k=kFALSE)
{
  if(first100k) return;
  Float_t expTheoRatio[nPlanes][nStrips];
  Float_t zero[nStrips],stripPlot[nStrips], stripAsymEr[nPlanes][nStrips];
  TGraphErrors *grPolPlane[nPlanes];             

  for (Int_t s=0; s <endStrip; s++) { 
    stripPlot[s]=s+1;
    zero[s]=0;
  }                                                                

  asymFit(runnum,expTheoRatio,stripAsymEr);

  TCanvas *cPol = new TCanvas("cPol","Pol.measured by each Strip",10,10,700,700); 
  cPol->Divide(2,2);  
  for (Int_t p =startPlane; p <endPlane; p++) { 
    cPol->cd(p+1);
    grPolPlane[p] = new TGraphErrors(endStrip,stripPlot,expTheoRatio[p],zero,stripAsymEr[p]);
    grPolPlane[p]->GetXaxis()->SetTitle("strip number");
    grPolPlane[p]->GetYaxis()->SetTitle("ratioExpTheory"); 
    grPolPlane[p]->SetTitle(Form("Plane %d",p+1));      
    grPolPlane[p]->SetMarkerStyle(20);
    grPolPlane[p]->SetMarkerColor(kRed+2);
    grPolPlane[p]->Draw("AP");  
    grPolPlane[p]->SetMaximum(1.1);
    grPolPlane[p]->SetMinimum(0.5);
        
  }
  cPol->Update();
  fileReadDraw(runnum);  
}
  
/*****The execution tree is as follows:
 *edetLasCyc.C
 *-- asymFit.C ..................................... fileReadDraw.C
 *----theoryAsym.C, edetExpAsym.C , maskedStrips.C
 *..................---getEBeamLasCuts.C
 *
 *asymFit.C processes all physics routines to evaluate the concerned quantities
 *fileReadDraw.C draws the main results
 ****************/
