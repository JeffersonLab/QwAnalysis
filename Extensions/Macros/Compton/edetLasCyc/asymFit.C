//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry
#ifndef __asymFit_F
#define __asymFit_F
#endif

#include <rootClass.h>
#include "expAsym.C"
#include "comptonRunConstants.h"
#include "maskedStrips.C"

// Double_t theoryAsym(Double_t strip)
// {
//   Double_t xStrip = xCedge - (Cedge[0] - strip)*stripWidth; //Cedge[0]
//   Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
//   Double_t rhoPlus = 1-rhoStrip*(1+a);
//   Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
//   Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
//   Double_t dsdrho =((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a
//   //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
//   //if(debug) printf("for strip#%d, xStrip:%g, rhoStrip:%g, crossSection:%g\n",strip,xStrip,rhoStrip,dsdrho);
//   return ((-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
// }

Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
  Double_t xStrip = xCedge - (tempCedge - (*thisStrip))*stripWidth*par[0];
  Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
  Double_t rhoPlus = 1-rhoStrip*(1+a);
  Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
  Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
  Double_t dsdrho =((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a
  //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  //if(debug) printf("for strip#%d, xStrip:%g, rhoStrip:%g, crossSection:%g\n",strip,xStrip,rhoStrip,dsdrho);
  return (par[1]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
}

void asymFit(Int_t runnum)
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  //gStyle->SetOptFit(0111); //!this eventually causes the macro to crash after a couple cycles
  //gStyle->SetOptFit(1); //!this eventually causes the macro to crash after a couple cycles
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Double_t pol[nPlanes], polEr[nPlanes], chiSq[nPlanes], effStripWidth[nPlanes], effStripWidthEr[nPlanes];
  Int_t NDF[nPlanes];
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=0;//, asymPlot=1;
  Float_t stripNum[nPlanes][nStrips];
  Float_t stripAsym[nPlanes][nStrips], stripAsymEr[nPlanes][nStrips];   

  TPaveText *pt[nPlanes];
  TLegend *leg[nPlanes];

  printf("read in parameters are: %g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3]);  

  ifstream expAsymPWTL1;
  ofstream polList;
  TLine *myline = new TLine(1,0,65,0);

  ///Read the experimental asymmetry and errors from file (for PWTL1)
  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymPWTL1.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymPWTL1.is_open()) {
      cout<<"Plotting the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	expAsymPWTL1>>stripNum[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	if(debug) cout<<stripNum[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
      }
      expAsymPWTL1.close();
      Cedge[p] = identifyCedgeforPlane(p,stripNum,stripAsymEr);//!still under check
      
      if(debug) cout<<"Compton edge for plane "<<p+1<<" is "<<Cedge[p]<<endl;
    }
    else cout<<"\n*** Alert:did not find "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  //  rhoToX();//! I don't need to call it everytime as long as beam-energy etc stays constant

  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry and Strip number",100,50,1000,1000);
  TGraphErrors *grAsymPlane[nPlanes];//, *grFittedTheo[nPlanes];

  cAsym->Divide(startPlane+1,endPlane); 
  polList.open(Form("%s/%s/%spol_%d.txt",pPath,webDirectory,filePrefix.Data(),runnum));
  polList<<";plane\tpol\tpolEr\tchiSq\tNDF\tCedge"<<endl;
  for (Int_t p =startPlane; p <endPlane; p++) {  
    cAsym->cd(p+1);  
    cAsym->GetPad(p+1)->SetGridx(1); 
    grAsymPlane[p]=new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
    //grAsymPlane[p]=new TGraphErrors(endStrip,stripNum[p],stripAsym[p],zero,stripAsymEr[p]);
    grAsymPlane[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
    //grAsymPlane[p]->GetXaxis()->SetTitleColor(kRed+2);      
    //grAsymPlane[p]->GetYaxis()->SetTitleColor(kRed+2);      
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");   
    grAsymPlane[p]->SetTitle("experimental asymmetry");//Run: %d, Plane %d",runnum,p+1))
    grAsymPlane[p]->GetXaxis()->CenterTitle();
    grAsymPlane[p]->GetYaxis()->CenterTitle();
    grAsymPlane[p]->SetMarkerStyle(kFullCircle); 
    grAsymPlane[p]->SetLineColor(kRed);
    grAsymPlane[p]->SetFillColor(0);   
    grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
    grAsymPlane[p]->SetMaximum(0.048); 
    grAsymPlane[p]->SetMinimum(-0.048);
    grAsymPlane[p]->GetXaxis()->SetLimits(1,65); 
    grAsymPlane[p]->GetXaxis()->SetNdivisions(416, kFALSE);
    grAsymPlane[p]->Draw("AP");  
    tempCedge = Cedge[p];///this should be equated just before the declaration of TF1
    TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+1,Cedge[p],2);///two parameter fit
    polFit->SetParameters(1.0,0.85);
    polFit->SetParLimits(0,0.2,2.0);
    polFit->SetParLimits(1,-1.0,1.0);
    polFit->SetParNames("effStrip","polarization");
    polFit->SetLineColor(kBlue);

    cout<<"starting to fit exp asym"<<endl;
    grAsymPlane[p]->Fit("polFit","0 R M E");
    cout<<"finished fitting exp asym"<<endl;
    polFit->DrawCopy("same");
    effStripWidth[p] = polFit->GetParameter(0);
    effStripWidthEr[p] = polFit->GetParError(0);

    pol[p] = polFit->GetParameter(1);
    polEr[p] = polFit->GetParError(1);
    chiSq[p] = polFit->GetChisquare();
    NDF[p] = polFit->GetNDF();
    polList<<p+1<<"\t"<<pol[p]<<"\t"<<polEr[p]<<"\t"<<chiSq[p]<<"\t"<<NDF[p]<<"\t"<<Cedge[p]<<endl;

    leg[p] = new TLegend(0.101,0.8,0.4,0.9);
    leg[p]->AddEntry(grAsymPlane[0],"experimental asymmetry","pe");///I just need the name
    leg[p]->AddEntry("polFit","QED-Asymmetry fit to exp-Asymmetry","l");//"lpf");//
    leg[p]->SetFillColor(0);
    leg[p]->Draw();

    pt[p] = new TPaveText(0.5092206,0.8119254,0.7294894,0.9957972,"brNDC");///x1,y1,x2,y2
    pt[p]->SetTextSize(0.043);//0.028); 
    pt[p]->SetBorderSize(1);
    pt[p]->SetTextAlign(12);
    pt[p]->SetFillColor(-1);
    pt[p]->SetShadowColor(-1);

    pt[p]->AddText(Form("chi Sq / ndf          : %f",chiSq[p]/NDF[p]));
    pt[p]->AddText(Form("effective strip width : %2.3f #pm %2.3f",effStripWidth[p],effStripWidthEr[p]));
    pt[p]->AddText(Form("Polarization (%)      : %2.1f #pm %2.1f",pol[p]*100.0,polEr[p]*100.0));
    pt[p]->Draw();
    myline->Draw();
    gPad->Update();
    //leg[p]->AddEntry(myline,"zero line","l");
    //cAsym->Update(); 
  }//for (Int_t p =startPlane; p <endPlane; p++)
  polList.close();
  cAsym->SaveAs(Form("%s/%s/%sasymFit.png",pPath,webDirectory,filePrefix.Data()));

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to execute asymFit.C\n",div_output.quot,div_output.rem );  
}

/*Comments
 *stripNum is a 2D array so as to hold the different set of strips that may be unmasked
 */
