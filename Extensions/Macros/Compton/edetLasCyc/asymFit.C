//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry

#include <rootClass.h>
#include "theoryAsym.C"
#include "edetExpAsym.C"
#include "comptonRunConstants.h"

void asymFit(Int_t runnum, Float_t expTheoRatio[nPlanes][nStrips])
{
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=0;
  Float_t calcAsym[nStrips];//,stripNum[nStrips];
  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  ifstream theoAsym;
  Double_t par[4];

  theoryAsym(Cedge,par);
//   theoAsym.open(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge));
//   printf("\nthe parameters received from theoretical fitting are par[0]:%f,par[1]:%f,par[2]:%f,par[3]:%f\n\n" ,par[0],par[1],par[2],par[3]);
//   TF1 *fn2 = new TF1("fn2",Form("[0] + [1]*(%f + %f*x + %f*x*x + %f*x*x*x)",par[0],par[1],par[2],par[3]),10,Cedge);
//   fn2->SetParameters(0.9,0);
//   fn2->SetParLimits(0,-1,1);
//   fn2->SetParLimits(1,2,-2);
//   if (theoAsym.is_open()) {
//     for(Int_t s =0 ; s <endStrip; s++) {
//       theoAsym>>stripNum[s]>>calcAsym[s];
//       if(debug) cout<<stripNum[s]<<"\t"<<calcAsym[s]<<endl;
//     }
//   }
//   else cout<<"\n***Error:Could not find the file "<<Form("%stheoryAsymForCedge_%d.txt",wwwPath.Data(),Cedge)<<endl;

  edetExpAsym(runnum,stripAsym,stripAsymEr);
  
  TGraphErrors *grTheoryAsym, *grAsymPlane[nPlanes];

  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",10,10,800,800);
  cAsym->Divide(2,2);
  
  TLine *myline = new TLine(0,0,70,0);
  myline->SetLineStyle(1);
  
  for (Int_t p =startPlane; p <endPlane; p++) {
    cAsym->cd(p+1);
    //grAsymPlane[p] = new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymRMS[p]);
    grAsymPlane[p] = new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
    grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
    grAsymPlane[p]->SetTitle(Form("Run: %d, Plane %d",runnum,p+1));
    grAsymPlane[p]->SetMarkerStyle(20);
    grAsymPlane[p]->SetLineColor(kRed+2);
    grAsymPlane[p]->SetMarkerColor(kRed+2); ///Maroon
    grAsymPlane[p]->Fit("fn2","R");
    grAsymPlane[p]->Draw("AP");
    myline->Draw();

    grTheoryAsym = new TGraphErrors(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge), "%lg %lg");
    //grTheoryAsym = new TGraphErrors(endStrip,stripPlot,calcAsym,zero,zero);
    grTheoryAsym->SetLineColor(4);
    grTheoryAsym->Draw("L");    
    cAsym->Update();
  } 
  if(debug) cout<<"\nstrip#\t\texpTheoRatio\tstripAsym\tcalcAsym"<<endl;

  for (Int_t p =startPlane; p <endPlane; p++) {  
    for (Int_t s =startStrip; s < endStrip; s++) {  
      if (maskedStrips(p,s)) continue;
      if (calcAsym[s]!= 0.0) expTheoRatio[p][s]= stripAsym[p][s]/calcAsym[s];
      if(debug) printf("expTheoRatio[%d][%d]:%f = %f / %e\n",p,s,expTheoRatio[p][s],stripAsym[p][s],calcAsym[s]);
    }     
  }
}

/*Comments
 *I'm currently including my rootClass.h and comptonRunConstants.h separately in every file
 *..there must be a way to do it non-repetatively.
 *I am doing this for all strips, I should limit myself to 64 strips at max
 */
