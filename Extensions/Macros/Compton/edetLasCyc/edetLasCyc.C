/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 * As of now, this script only calculates the ratio, and needs more fine tuning
 */

#include <rootClass.h>
#include "comptonRunConstants.h"
//#include "asymFit.C"
#include "theoryAsym.C"
#include "expAsym.C"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, Bool_t first100k=kFALSE)
{
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  if(first100k) return;
  Float_t stripAsymEr[nPlanes][nStrips];//,expTheoRatio[nPlanes][nStrips];
  Float_t stripAsym[nPlanes][nStrips];   
  TLegend *leg;//, *legYield[nPlanes];
  Float_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
  TLine *myline = new TLine(0,0,70,0);
  Bool_t asymPlot=0; //debug=1;
  ofstream theoreticalAsym,crossSection;    
  leg = new TLegend(0.1,0.7,0.4,0.9); 

  //TCanvas *cTheoAsym = new TCanvas("theoAsym","Theoretical asymmetry",10,20,400,400);
  TGraph *grtheory = new TGraph(Form("%s/%s/QEDasym.txt",pPath,webDirectory), "%lg %lg");                   
  grtheory->GetXaxis()->SetTitle("dist from compton scattered electrons(m)");
  grtheory->GetYaxis()->SetTitle("#rho"); 
  grtheory->GetYaxis()->CenterTitle(); 
  grtheory->SetTitle("#rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(2);
  grtheory->SetMarkerColor(2);
  grtheory->Fit("pol3");
  
  expAsym(runnum,stripAsym,stripAsymEr,stripAsym_v2,stripAsymEr_v2);
  //  asymFit(runnum,expTheoRatio,stripAsymEr);

  for(Int_t p = startPlane; p < endPlane; p++) {
    Cedge = identifyCedgeforPlane(p);
    theoryAsym(Cedge); //function call to theoretical rho-to-x file generation
  }

   Float_t zero[nStrips],stripPlot[nStrips];
   for (Int_t s=0; s <endStrip; s++) { 
     stripPlot[s]=s+1;
     zero[s]=0;
   }
   if(asymPlot) {
  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",50,50,1000,600);
  TGraphErrors *grTheoryAsym[nPlanes], *grAsymPlane[nPlanes];
  cAsym->Divide(startPlane+1,endPlane); 
  for (Int_t p =startPlane; p <endPlane; p++) {  
    //for (Int_t p =0; p <1; p++) {
    cAsym->cd(p+1);  
    grAsymPlane[p] = new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymEr[p]);
    grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");   
    grAsymPlane[p]->SetTitle(Form("experimental asymmetry"));//Run: %d, Plane %d",runnum,p+1))
    grAsymPlane[p]->SetMarkerStyle(kFullCircle); 
    grAsymPlane[p]->SetLineColor(kRed);
    grAsymPlane[p]->SetFillColor(0);   
    grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
    //grAsymPlane[p]->Fit("fn2","R");  
    grAsymPlane[p]->SetMaximum(0.042); 
    grAsymPlane[p]->SetMinimum(-0.042);
    grAsymPlane[p]->Draw("AP");        
    myline->Draw();
    grTheoryAsym[p] = new TGraphErrors(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge));
    //grTheoryAsym[p] = new TGraphErrors(Form("%s/%s/%smodTheoryFileP1.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg"); 
    //grTheoryAsym[p] = new TGraphErrors(endStrip,stripPlot,calcAsym[p],zero,zero); 
    grTheoryAsym[p]->SetLineColor(kBlue);
    grTheoryAsym[p]->SetLineWidth(3);   
    grTheoryAsym[p]->SetFillColor(0);    
    grTheoryAsym[p]->SetTitle("theoretical asymmetry");
    grTheoryAsym[p]->Draw("L"); 
    cAsym->Update();         
    leg->AddEntry(grAsymPlane[p],"experimental asymmetry","lpf");
    leg->AddEntry(grTheoryAsym[p],"theoretical asymmetry","lpf");
    leg->SetFillColor(0);
    leg->Draw();
  } 
  //leg->AddEntry(myline,"zero line","l");
  cAsym->SetGridx(1); 
  cAsym->Update(); 
}  
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
