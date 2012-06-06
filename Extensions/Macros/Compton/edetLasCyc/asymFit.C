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

Double_t theoCrossSec(Double_t *thisStrip, Double_t *parCx)
{///parCx[1]: to be found Cedge
  Double_t xStrip = xCedge - (parCx[0] - (*thisStrip))*stripWidth;//*parCx[0];
  Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
  Double_t rhoPlus = 1-rhoStrip*(1+a);
  Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
  Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
  return (parCx[1]*((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1));//eqn.22,without factor 2*pi*(re^2)/a
}

Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
  Double_t xStrip = xCedge - (tempCedge - (*thisStrip))*stripWidth*par[0];
  //Double_t xStrip = xCedge - (par[0] - (*thisStrip))*stripWidth*par[1];
  //Double_t xStrip = xCedge - (tempCedge - par[2] - (*thisStrip))*stripWidth*par[0];
  Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
  Double_t rhoPlus = 1-rhoStrip*(1+a);
  Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
  Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
  Double_t dsdrho =((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a
  //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  return (par[1]*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
}

void asymFit(Int_t runnum)
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  //gStyle->SetOptFit(0111); //!this eventually causes the macro to crash after a couple cycles
  //gStyle->SetOptFit(1); //!this eventually causes the macro to crash after a couple cycles
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Double_t pol[nPlanes],polEr[nPlanes],chiSq[nPlanes],effStripWidth[nPlanes],effStripWidthEr[nPlanes];
  //Double_t offset[nPlanes],offsetEr[nPlanes];
  Int_t NDF[nPlanes];
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=0;//, asymPlot=1;
  Float_t stripNum[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips], stripAsymNr[nPlanes][nStrips], stripAsymDrEr[nPlanes][nStrips];   

  TPaveText *pt[nPlanes];
  TLegend *leg[nPlanes];

  printf("read in parameters are: %g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3]);  

  ifstream infileYield;// expAsymPWTL1,
  ofstream polList;
  TLine *myline = new TLine(1,0,65,0);

  for(Int_t p =startPlane; p <endPlane; p++) {
    infileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(infileYield.is_open()) {
      cout<<"Reading the Yield corresponding to Plane "<<p+1<<endl;
      if(debug) cout<<"stripNum\t"<<"Yield\t"<<"YieldEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) {
	  stripAsymDr[p][s]=0.0;
	  stripAsymDrEr[p][s]=0.0;
	  stripAsymNr[p][s]=0.0;
	}
	else infileYield>>stripNum[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>stripAsymNr[p][s];
	if(debug) cout<<stripNum[p][s]<<"\t"<<stripAsymDr[p][s]<<"\t"<<stripAsymDrEr[p][s]<<"\t"<<stripAsymNr[p][s]<<endl;
      }
      infileYield.close();
      Cedge[p] = identifyCedgeforPlane(p,stripNum,stripAsymDr);//!still under check      
      cout<<"Compton edge for plane "<<p+1<<" is "<<Cedge[p]<<endl;
    }
    else cout<<"\n*** Alert:did not find "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  ///Read the experimental asymmetry and errors from file (for PWTL1)
  //  rhoToX();//! I don't need to call it everytime as long as beam-energy etc stays constant

  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry and Strip number",100,50,1000,1300);
  TGraphErrors *grAsymPlane[nPlanes];//, *grFittedTheo[nPlanes];

  cAsym->Divide(startPlane+1,endPlane); 
  polList.open(Form("%s/%s/%spol_%d.txt",pPath,webDirectory,filePrefix.Data(),runnum));
  polList<<";plane\tpol\tpolEr\tchiSq\tNDF\tCedge"<<endl;
  for (Int_t p =startPlane; p <endPlane; p++) {  
    cAsym->cd(p+1);  
    cAsym->GetPad(p+1)->SetGridx(1); 
    grAsymPlane[p]=new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
    grAsymPlane[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
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
    polFit->SetParameters(1.0,0.85);//,0.5);
    //polFit->SetParLimits(0,1.021,1.021);//fixing the strip width to 1.021
    polFit->SetParLimits(0,0.2,2.0);///allowing the strip width to be either 20% or 200% of its real pitch    
    polFit->SetParLimits(1,-1.0,1.0);///allowing polarization to be -100% to +100%

    //polFit->SetParNames("effStrip","polarization");//,"edgeOffset");
    polFit->SetParNames("effStrip","polarization");
    polFit->SetLineColor(kBlue);

    cout<<"starting to fit exp asym"<<endl;
    grAsymPlane[p]->Fit("polFit","0 R M E");
    //grAsymPlane[p]->Fit("polFit","0 M E");//this makes a significant difference in p3 pol value!
    cout<<"finished fitting exp asym"<<endl;
    polFit->DrawCopy("same");
    effStripWidth[p] = polFit->GetParameter(0);
    effStripWidthEr[p] = polFit->GetParError(0);

    pol[p] = polFit->GetParameter(1);
    polEr[p] = polFit->GetParError(1);

//     offset[p] = polFit->GetParameter(2);
//     offsetEr[p] = polFit->GetParError(2);

    chiSq[p] = polFit->GetChisquare();
    NDF[p] = polFit->GetNDF();

    polList<<p+1<<"\t"<<pol[p]<<"\t"<<polEr[p]<<"\t"<<chiSq[p]<<"\t"<<NDF[p]<<"\t"<<Cedge[p]<<endl;

    leg[p] = new TLegend(0.101,0.8,0.4,0.9);
    leg[p]->AddEntry(grAsymPlane[0],"experimental asymmetry","pe");///I just need the name
    leg[p]->AddEntry("polFit","QED-Asymmetry fit to exp-Asymmetry","l");//"lpf");//
    leg[p]->SetFillColor(0);
    leg[p]->Draw();

    pt[p] = new TPaveText(0.5092206,0.8119254,0.84894,0.9957972,"brNDC");///x1,y1,x2,y2
    pt[p]->SetTextSize(0.043);//0.028); 
    pt[p]->SetBorderSize(1);
    pt[p]->SetTextAlign(12);
    pt[p]->SetFillColor(-1);
    pt[p]->SetShadowColor(-1);

    pt[p]->AddText(Form("chi Sq / ndf          : %f",chiSq[p]/NDF[p]));
    pt[p]->AddText(Form("effective strip width : %2.3f #pm %2.3f",effStripWidth[p],effStripWidthEr[p]));
    pt[p]->AddText(Form("Polarization (%)      : %2.1f #pm %2.1f",pol[p]*100.0,polEr[p]*100.0));
    //pt[p]->AddText(Form("Compton Edge Offset   : %f #pm %f",offset[p],offsetEr[p]));
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
