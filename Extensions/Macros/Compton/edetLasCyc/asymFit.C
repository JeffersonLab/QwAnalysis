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
  //Double_t xStrip = xCedge - (tempCedge - (*thisStrip))*stripWidth*par[0];//for two parameter fit
  //Double_t xStrip = xCedge - (tempCedge + par[1] - (*thisStrip))*stripWidth*par[0];//for 2nd parameter as Cedge offset
  Double_t xStrip = xCedge - (par[1] - (*thisStrip))*stripWidth*par[0];//for 2nd parameter as Cedge itself
  //Double_t xStrip = xCedge - par[1]*stripWidth - (*thisStrip)*stripWidth*par[0];//Guruji's method of fitting

  Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
  Double_t rhoPlus = 1-rhoStrip*(1+a);
  Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
  Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
  Double_t dsdrho =((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a
  //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  return (par[2]*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
}

void asymFit(Int_t runnum)
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  //gStyle->SetOptFit(1); //!this eventually causes the macro to crash after a couple cycles
  //gStyle->SetOptStat(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Double_t pol[nPlanes],polEr[nPlanes],chiSq[nPlanes],effStripWidth[nPlanes],effStripWidthEr[nPlanes];
  Double_t offset[nPlanes],offsetEr[nPlanes];
  Int_t NDF[nPlanes];
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=0,debug1=0;

  TPaveText *pt[nPlanes];
  TLegend *leg[nPlanes];

  printf("read in parameters are: %g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3]);  

  ifstream infileScaler;
  ofstream polList;
  TLine *myline = new TLine(1,0,65,0);

  Double_t tNormScBkgdSubSigB1[nPlanes][nStrips];
  std::vector<std::vector <Float_t> > activeStrip,tNormScB1L1,tNormScB1L0;
  Int_t numbGoodStrips[nPlanes]={0};
  ///Note: the 's' in this section of the routine does not necessarily represent strip number
  for(Int_t p =startPlane; p <endPlane; p++) {
    infileScaler.open(Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(infileScaler.is_open()) {
      if(p>=(Int_t)activeStrip.size()) {
	activeStrip.resize(p+1),tNormScB1L1.resize(p+1),tNormScB1L0.resize(p+1);
      }
      if(debug) cout<<"Reading the tNormScaler corresponding to Plane "<<p+1<<endl;
      if(debug1) cout<<"activeStrip\t"<<"tNormScalerLasOn\t"<<"tNormScalerLasOff"<<endl;
      while(infileScaler.good()) {
	activeStrip[p].push_back(0.0),tNormScB1L1[p].push_back(0.0),tNormScB1L0[p].push_back(0.0);
	Int_t s=activeStrip[p].size() - 1;
	infileScaler>>activeStrip[p][s]>>tNormScB1L1[p][s]>>tNormScB1L0[p][s];
	tNormScBkgdSubSigB1[p][s] = tNormScB1L1[p][s] - tNormScB1L0[p][s];
	if(debug) printf("[%d][%d]:%2.0f\t%f\t%f\n",p+1,s+1,activeStrip[p][s],tNormScB1L1[p][s],tNormScB1L0[p][s]);
	numbGoodStrips[p]++;//counts in human counting
      }
      infileScaler.close();
    } else cout<<"\n*** Alert:couldn't find "<<Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" needed to generically locate Compton edge"<<endl;
  }

  for(Int_t p =startPlane; p <endPlane; p++) {
    Bool_t trueEdge = 1;
    for(Int_t s =(Int_t)activeStrip[p][0]; s < numbGoodStrips[p]; s++) {//begin at first activeStrip
      if (tNormScBkgdSubSigB1[p][s] < qNormBkgdSubSignalLow) { //!'qNormBkgdSubSignalLow' is arbitrarily chosen by observing how this numb may vary
	Int_t leftStrips = numbGoodStrips[p] - (Int_t)activeStrip[p][s];
	for(Int_t st =0; st <leftStrips;st++) {
	  if (tNormScBkgdSubSigB1[p][s+st]  >= qNormBkgdSubSignalLow) trueEdge = 0;	      
	}
	if (trueEdge) {
	  Cedge[p]=(Int_t)activeStrip[p][s] - 1;//!this can be left as a float after changing the base assignement of Cedge
	  cout<<"Compton edge for plane "<<p+1<<" is "<<Cedge[p]<<"\n"<<endl;
	  break;
	} else cout<<"**** Alert: the generic Cedge determination mechanism didn't work for run # "<<runnum<<endl;
      } else if(s>=numbGoodStrips[p]) cout<<"*** Something unusual in Cedge determination for run # "<<runnum<<endl;
    }
  }

  //rhoToX();//! I don't need to call it everytime as long as beam-energy etc stays constant

  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry and Strip number",100,50,1000,420*endPlane);
  TGraphErrors *grAsymPlane[nPlanes];//, *grFittedTheo[nPlanes];

  cAsym->Divide(startPlane+1,endPlane); 
  polList.open(Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data()));
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
    TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+1,Cedge[p],3);///two parameter fit
    polFit->SetParameters(1.0,tempCedge,0.85);//begin the fitting from the generic Cedge
    //polFit->SetParLimits(0,1.021,1.021);//fixing the strip width to 1.021
    polFit->SetParLimits(0,0.2,2.0);///allowing the strip width to be either 20% or 200% of its real pitch    
    //polFit->SetParLimits(1,tempCedge,tempCedge);///fixed compton edge
    polFit->SetParLimits(1,tempCedge-3.0,tempCedge+2.0);///allowing compton edge to vary by -3 strips to upto +2 strips
    polFit->SetParLimits(2,-1.1,1.1);///allowing polarization to be -100% to +100%

    polFit->SetParNames("effStrip","comptonEdge","polarization");
    polFit->SetLineColor(kBlue);

    if(debug) cout<<"starting to fit exp asym"<<endl;
    grAsymPlane[p]->Fit("polFit","0 R M E");
    //grAsymPlane[p]->Fit("polFit","0 M E");//this makes a significant difference in p3 pol value!
    if(debug1) cout<<"finished fitting exp asym"<<endl;
    polFit->DrawCopy("same");
    effStripWidth[p] = polFit->GetParameter(0);
    effStripWidthEr[p] = polFit->GetParError(0);

    offset[p] = polFit->GetParameter(1);
    offsetEr[p] = polFit->GetParError(1);

    pol[p] = polFit->GetParameter(2);
    polEr[p] = polFit->GetParError(2);

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
    //pt[p]->AddText(Form("Compton Edge          : %f #pm %f",Cedge[p]+offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Compton Edge          : %f #pm %f",offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Polarization (%)      : %2.1f #pm %2.1f",pol[p]*100.0,polEr[p]*100.0));
    pt[p]->Draw();
    myline->Draw();
    gPad->Update();
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
