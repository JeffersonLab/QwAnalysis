//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry

#include <rootClass.h>
#include "theoryAsym.C"
#include "edetExpAsym.C"

void asymFit(Int_t runnum, Float_t expTheoRatio[nPlanes][nStrips])
{
  Bool_t debug=1,debug1=0;
  Float_t calcAsym[nStrips],stripNum[nStrips];
  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  ifstream theoAsym;
  theoAsym.open(Form("analOut/theoryAsymForCedge_%d.txt",Cedge));

  if (!theoAsym.is_open()) {
    cout<<"theoretial asymmetry file not found hence calling the function to evaluate"<<endl;
    theoryAsym(Cedge);
    theoAsym.open(Form("analOut/theoryAsymForCedge_%d.txt",Cedge));
  }
  
  if (theoAsym.is_open()) {
    for(Int_t s =0 ; s <endStrip; s++) {
      theoAsym>>stripNum[s]>>calcAsym[s];
      if(debug1) cout<<stripNum[s]<<"\t"<<calcAsym[s]<<endl;
    }
  }
  else cout<<"\n***Error:Could not find the file with theoretical asymmetry**\n"<<endl;

  edetExpAsym(runnum,stripAsym,stripAsymEr);
  
  TGraphErrors *grTheoryAsym;
  TGraphErrors *grAsymPlane[nPlanes];

  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",10,10,800,800);
  cAsym->Divide(2,2);
  
  TLine *myline = new TLine(0,0,64,0);
  myline->SetLineStyle(1);
  
  for (Int_t p =startPlane; p <endPlane; p++) {
    cAsym->cd(p+1);
    //grAsymPlane[p] = new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymRMS[p]);
    grAsymPlane[p] = new TGraphErrors(Form("analOut/r%d_expAsymP%d.txt",runnum,p+1), "%lg %lg %lg");
    grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
    grAsymPlane[p]->SetTitle(Form("Plane %d",p+1));
    grAsymPlane[p]->SetMarkerStyle(20);
    grAsymPlane[p]->SetLineColor(kRed+2);
    grAsymPlane[p]->SetMarkerColor(kRed+2); ///Maroon
    grAsymPlane[p]->Draw("AP");
    myline->Draw();

    grTheoryAsym = new TGraphErrors(Form("analOut/theoryAsymForCedge_%d.txt",Cedge), "%lg %lg");
    //grTheoryAsym = new TGraphErrors(endStrip,stripPlot,calcAsym,zero,zero);
    grTheoryAsym->SetLineColor(4);
    grTheoryAsym->Draw("L");    
    cAsym->Update();
  } 
  if(debug) cout<<"\nstrip#\t\texpTheoRatio\tstripAsym\tcalcAsym"<<endl;

  for (Int_t p =startPlane; p <endPlane; p++) {  
    for (Int_t s =startStrip; s <endStrip; s++) {  
      if (maskedStrips(p,s)) continue;
      if (calcAsym[s]!=0.0) expTheoRatio[p][s]= stripAsym[p][s]/calcAsym[s];
      if(debug) printf("expTheoRatio[%d][%d]:%f = %f / %f\n",p,s,expTheoRatio[p][s],stripAsym[p][s],calcAsym[s]);
    }    
  }
}

/*Comments
 *I'm currently including my rootClass.h and comptonRunConstants.h separately in every file
 *..there must be a way to do it non-repetatively.
 *I am doing this for all strips, I should limit myself to 64 strips at max
 */
