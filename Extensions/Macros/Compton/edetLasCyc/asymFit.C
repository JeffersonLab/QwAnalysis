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

// Double_t theoCrossSec(Double_t *thisStrip, Double_t *parCx)//2 param fit for cross section
// {///parCx[1]: to be found Cedge
//   Double_t xStrip = xCedge - (tempCedge - (*thisStrip))*stripWidth*parCx[0];
//   Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
//   Double_t rhoPlus = 1-rhoStrip*(1+a);
//   Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
//   Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
//   return (parCx[1]*((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1));//eqn.22,without factor 2*pi*(re^2)/a
// }

 Double_t theoCrossSec(Double_t *thisStrip, Double_t *parCx)//3 parameter fit for cross section
 {///parCx[1]: to be found Cedge
   Double_t xStrip = xCedge - (parCx[1] - (*thisStrip))*stripWidth*parCx[0];
   Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
   Double_t rhoPlus = 1-rhoStrip*(1+a);
   Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
   Double_t dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a)); // 2nd term of eqn 22
   return (parCx[2]*((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1));//eqn.22,without factor 2*pi*(re^2)/a
 }

/////2 parameter method
// Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
// {
//   Double_t xStrip = xCedge - (tempCedge - (*thisStrip))*stripWidth*par[0];//for two parameter fit
//   //Double_t xStrip = xCedge - (tempCedge + par[1] - (*thisStrip))*stripWidth*par[0];//for 2nd parameter as Cedge offset
//   //Double_t xStrip = xCedge - par[1]*stripWidth - (*thisStrip)*stripWidth*par[0];//Guruji's method of fitting
//   Double_t rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
//   Double_t rhoPlus = 1-rhoStrip*(1 + a);
//   Double_t rhoMinus = 1-rhoStrip*(1 - a);//just a term in eqn 24
//   Double_t dsdrho1 = rhoPlus/rhoMinus;// 2nd term of eqn 22
//   Double_t dsdrho =((rhoStrip*(1 - a)*rhoStrip*(1 - a)/rhoMinus)+1+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a
//   //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
//   return (par[1]*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
// }

///3 parameter method
Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
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
  Bool_t debug=0,debug1=0,debug2=0;
  ifstream paramfile;
  TPaveText *pt[nPlanes];
  TLegend *leg[nPlanes],*legYield[nPlanes];

  rhoToX();//! I don't need to call it everytime as long as beam-energy etc stays constant
  paramfile.open(Form("%s/%s/paramFile.txt",pPath,webDirectory));
  cout<<"reading in the rho to X fitting parameters, they were:" <<endl;
  paramfile>>param[0]>>param[1]>>param[2]>>param[3];
  paramfile.close();
  printf("%g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3]);

  ifstream infileScaler, expAsymPWTL1, infileYield;
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
      if(debug1) cout<<"p\ts\tbkgdSubScaler\tactiveStrip\t"<<"tNormScL1\t"<<"tNormScL0\t"<<"(L1-L0)/L1"<<endl;
      while(infileScaler.good()) {
	activeStrip[p].push_back(0.0);
	tNormScB1L1[p].push_back(0.0);
	tNormScB1L0[p].push_back(0.0);
	Int_t s=activeStrip[p].size() - 1;///this 's' does not represent the actual strip number
	infileScaler>>activeStrip[p][s]>>tNormScB1L1[p][s]>>tNormScB1L0[p][s];///the content of the 'activeStrip' vector contains the true strip number
	tNormScBkgdSubSigB1[p][s] = tNormScB1L1[p][s] - tNormScB1L0[p][s];
	if(debug1) printf("%d\t%d\t%f\t%2.0f\t%f\t%f\t%f\n",p+1,s+1,tNormScBkgdSubSigB1[p][s],activeStrip[p][s],tNormScB1L1[p][s],tNormScB1L0[p][s],tNormScBkgdSubSigB1[p][s]/tNormScB1L1[p][s]);
	numbGoodStrips[p]++;//counts in human counting ///this is basically =activeStrip[p].size()
      }
      infileScaler.close();
    } else cout<<"\n*** Alert:couldn't find "<<Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" needed to generically locate Compton edge"<<endl;
  }

  ///start finding the generic compton edge
  for(Int_t p =startPlane; p <endPlane; p++) {
    Bool_t trueEdge = 1;
    cout<<"looking for generic compton edge for plane "<<p+1<<endl; 
    for(Int_t s =(Int_t)activeStrip[p][0]; s < numbGoodStrips[p]; s++) {//begin at first activeStrip
      if (tNormScBkgdSubSigB1[p][s]/tNormScB1L1[p][s] < qNormBkgdSubScalToSigRatioLow) { //!'qNormBkgdSubScalToSigRatioLow' is arbitrarily chosen at 10% 
	Int_t probableEdge = (Int_t)activeStrip[p][s]-1; ///since the above condition is fulfiled after crossing Cedge
	if(debug1) cout<<"probable Cedge : "<<probableEdge<<endl;
	Int_t leftStrips = numbGoodStrips[p] - probableEdge;
	for(Int_t st =1; st <=leftStrips;st++) {///starting to check next strip onwards
	  if (tNormScBkgdSubSigB1[p][s+st]/tNormScB1L1[p][s+st]  >= qNormBkgdSubScalToSigRatioLow) trueEdge = 0;
	  ///on purpose the requirement for reconsidering the selected comptonEdge is a little more strict.
	  ///this method of comparision auto-rejects the cases when the bkgd subtracted counts are negative
	  if(debug1) printf("tNormScBkgdSubSigB1[%d][%d]:%f, leftStrips:%d, trueEdge:%d\n",p+1,s+1+st+1,tNormScBkgdSubSigB1[p][s+st+1],leftStrips,trueEdge);
	}
	if (trueEdge) {
	  Cedge[p]=(Double_t)activeStrip[p][s] - 1;
	  cout<<"\nCompton edge for plane "<<p+1<<" auto-determined to strip "<<Cedge[p]<<"\n"<<endl;
	  break;
	} else cout<<"**** Alert: the generic Cedge determination mechanism didn't work for run # "<<runnum<<endl;
      } else if(s>=numbGoodStrips[p]) cout<<"*** Something unusual in Cedge determination for run # "<<runnum<<endl;
    }
  }

  TCanvas *cAsym,*cYield,*cResidual;
  TGraphErrors *grAsymPlane[nPlanes],*grYieldPlane[nPlanes],*grResiduals[nPlanes],*grB1L0[nPlanes];
  TMultiGraph *grAsymDrAll[nPlanes];

  cAsym = new TCanvas("cAsym","Asymmetry and Strip number",10,10,1000,420*endPlane);
  cResidual = new TCanvas("cResidual","Asymmetry - Fit residual",10,500,1000,420*endPlane);
  cYield = new TCanvas("cYield","detector yield",300,200,1000,420*endPlane);

  cAsym->Divide(1,endPlane); 
  cResidual->Divide(1,endPlane); 
  cYield->Divide(1,endPlane); 

  Float_t stripNum[nPlanes][nStrips],stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips],stripAsymNr[nPlanes][nStrips];
  Float_t fitResidue[nPlanes][nStrips];
  Float_t zero[nStrips];//,stripCount[nStrips]

  polList.open(Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data()));
  polList<<";plane\tpol\tpolEr\tchiSq\tNDF\tCedge"<<endl;
  for (Int_t p =startPlane; p <endPlane; p++) {  
    cAsym->cd(p+1);  
    //cAsym[p]->Modified();
    //cAsym->GetPad(p+1)->SetGridx(1);
    grAsymPlane[p]=new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
    grAsymPlane[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");   
    grAsymPlane[p]->SetTitle(Form("experimental asymmetry Run: %d, Plane %d",runnum,p+1));
    grAsymPlane[p]->GetXaxis()->CenterTitle();
    grAsymPlane[p]->GetYaxis()->CenterTitle();
    grAsymPlane[p]->GetYaxis()->SetLabelSize(0.06);
    grAsymPlane[p]->GetXaxis()->SetLabelSize(0.06);
    grAsymPlane[p]->GetXaxis()->SetTitleOffset(1.3);
    grAsymPlane[p]->GetYaxis()->SetTitleOffset(1.3);
    grAsymPlane[p]->GetXaxis()->SetTitleSize(0.5);

    grAsymPlane[p]->SetMarkerStyle(kFullCircle);
    grAsymPlane[p]->SetLineColor(kRed);
    grAsymPlane[p]->SetFillColor(0);   
    grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
    grAsymPlane[p]->SetMaximum(0.048); 
    grAsymPlane[p]->SetMinimum(-0.048);
    grAsymPlane[p]->GetXaxis()->SetLimits(1,65); 
    grAsymPlane[p]->GetXaxis()->SetNdivisions(416, kFALSE);

    //cAsym->SetGridx(1); 
    grAsymPlane[p]->Draw("AP");  
    //cAsym->Update();
    tempCedge = Cedge[p];///this should be equated before the declaration of TF1

//     ///2 parameter fit
//     TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+1,Cedge[p],2);
//     polFit->SetParameters(1.0,0.85);//begin the fitting from the generic Cedge
//     polFit->SetParLimits(0,0.2,2.0);///allowing the strip width to be either 20% or 200% of its real pitch    
//     polFit->SetParLimits(1,-1.1,1.1);///allowing polarization to be - 110% to +110%
//     polFit->SetParNames("effStrip","polarization");  
//     polFit->SetLineColor(kBlue);
//     grAsymPlane[p]->Fit("polFit","0 R M E");
//     polFit->DrawCopy("same");
//     pol[p] = polFit->GetParameter(1);
//     polEr[p] = polFit->GetParError(1);
//     offset[p] = Cedge[p];//2 parameter fit has Cedge fixed
//     offsetEr[p] = 0;//2 parameter fit has Cedge fixed

    ///3 parameter fit
    TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+1,Cedge[p],3);
    polFit->SetParameters(1.0,tempCedge,0.85);//begin the fitting from the generic Cedge
    //polFit->SetParLimits(0,1.021,1.021);//fixing the strip width to 1.021
    polFit->SetParLimits(0,0.8,1.8);///allowing the strip width to be either 80% or 180% of its real pitch    
    polFit->SetParLimits(1,tempCedge,tempCedge);///fixed compton edge
    //polFit->SetParLimits(1,tempCedge-0.50,tempCedge+0.50);///allowing compton edge to vary by -3 strips to upto +2 strips
    polFit->SetParLimits(2,-1.0,1.0);///allowing polarization to be - 110% to +110%
    polFit->SetParNames("effStrip","comptonEdge","polarization");
    polFit->SetLineColor(kBlue);
    if(debug) cout<<"starting to fit exp asym"<<endl;
    grAsymPlane[p]->Fit("polFit","0 R M E");
    //grAsymPlane[p]->Fit("polFit","0 M E");//this makes a significant difference in p3 pol value!
    if(debug1) cout<<"finished fitting exp asym"<<endl;
    polFit->DrawCopy("same");
    offset[p] = polFit->GetParameter(1);
    offsetEr[p] = polFit->GetParError(1);
    pol[p] = polFit->GetParameter(2);
    polEr[p] = polFit->GetParError(2);

    effStripWidth[p] = polFit->GetParameter(0);
    effStripWidthEr[p] = polFit->GetParError(0);

    chiSq[p] = polFit->GetChisquare();
    NDF[p] = polFit->GetNDF();

    polList<<p+1<<"\t"<<pol[p]<<"\t"<<polEr[p]<<"\t"<<chiSq[p]<<"\t"<<NDF[p]<<"\t"<<Cedge[p]<<endl;

    leg[p] = new TLegend(0.101,0.8,0.4,0.9);
    leg[p]->AddEntry(grAsymPlane[0],"experimental asymmetry","lpe");///I just need the name
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
    pt[p]->AddText(Form("Compton Edge          : %2.2f #pm %2.2f",offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Polarization (%)      : %2.1f #pm %2.1f",pol[p]*100.0,polEr[p]*100.0));
    pt[p]->Draw();
    myline->Draw();
    //gPad->Update();

    ///determining the residue of the above fit
    for (Int_t s = startStrip; s < endStrip; s++) {//ensure that all variables gets initiated by zero
      stripNum[p][s]=0.0,stripAsym[p][s]=0.0,stripAsymEr[p][s]=0.0;
      stripAsymDr[p][s]=0.0,stripAsymNr[p][s]=0.0,stripAsymDrEr[p][s]=0.0;
      zero[s] = 0.0;
    }

    expAsymPWTL1.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymPWTL1.is_open()) {
      if(debug2) cout<<"Reading the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug2) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) continue;
	expAsymPWTL1>>stripNum[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	if(debug2) cout<<stripNum[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
      }
      expAsymPWTL1.close();
    }
    else cout<<"did not find the expAsym file "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
    
    for (Int_t s = startStrip; s < endStrip; s++) {
      if (maskedStrips(p,s)) continue;
      fitResidue[p][s] = stripAsym[p][s] - polFit->Eval(s+1);
    }

    cResidual->cd(p+1);  
    grResiduals[p]=new TGraphErrors(endStrip,stripNum[p],fitResidue[p],zero,stripAsymEr[p]);
    grResiduals[p]->SetMarkerStyle(kOpenCircle);
    grResiduals[p]->SetMaximum(0.03); 
    grResiduals[p]->SetMinimum(-0.03);
    grResiduals[p]->GetXaxis()->SetLimits(1,65); 
    grResiduals[p]->GetXaxis()->SetNdivisions(416, kFALSE);
    grResiduals[p]->SetTitle("Residuals");
    grResiduals[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
    grResiduals[p]->GetYaxis()->SetTitle("asymmetry - Fit");
    grResiduals[p]->GetXaxis()->CenterTitle();
    grResiduals[p]->GetYaxis()->CenterTitle();
    grResiduals[p]->Draw("AP");

    infileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(infileYield.is_open()) {
      if(debug2) cout<<"Reading the Yield corresponding to Plane "<<p+1<<endl;
      if(debug2) cout<<"stripNum\t"<<"stripYield\t"<<"stripYieldEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) continue;
	infileYield>>stripNum[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>stripAsymNr[p][s];
	if(debug2) cout<<stripNum[p][s]<<"\t"<<stripAsymDr[p][s]<<"\t"<<stripAsymDrEr[p][s]<<endl;
      }
      infileYield.close();
    }
    else cout<<"did not find the Yield file "<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
    
    cYield->cd(p+1);
    grYieldPlane[p]=new TGraphErrors(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
    grYieldPlane[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
    grYieldPlane[p]->GetYaxis()->SetTitle("yield (Hz/uA)");   
    grYieldPlane[p]->SetTitle(Form("detector yield Run: %d, Plane %d",runnum,p+1));
    grYieldPlane[p]->GetXaxis()->CenterTitle();
    grYieldPlane[p]->GetYaxis()->CenterTitle();
    grYieldPlane[p]->SetLineColor(kGreen);
    grYieldPlane[p]->SetFillColor(kGreen);   
    grYieldPlane[p]->SetMarkerColor(kGreen); ///kRed+2 = Maroon
    grYieldPlane[p]->GetXaxis()->SetLimits(1,65); 
    grYieldPlane[p]->GetXaxis()->SetNdivisions(416, kFALSE);
    
    /////2 parameter fit for cross section
    //TF1 *crossSecFit = new TF1("crossSecFit",theoCrossSec,startStrip+1,Cedge[p]-1,2);///three parameter fit
    //crossSecFit->SetParameters(1,20.0);//begin the fitting from the generic Cedge
    //crossSecFit->SetParLimits(0,0.2,2.0);
    
    ///3 parameter fit for cross section
    TF1 *crossSecFit = new TF1("crossSecFit",theoCrossSec,startStrip+1,Cedge[p]-1,3);///three parameter fit
    crossSecFit->SetParameters(1,Cedge[p],20.0);//begin the fitting from the generic Cedge
    crossSecFit->SetParLimits(0,0.2,2.0);
    crossSecFit->SetParLimits(1,Cedge[p]-5,Cedge[p]+5);
    
    crossSecFit->SetLineColor(kRed);
    grYieldPlane[p]->Fit("crossSecFit","R M E");

    grB1L0[p] = new TGraphErrors(Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
    grB1L0[p]->SetLineColor(kBlue);
    grB1L0[p]->SetMarkerColor(kBlue);
    grB1L0[p]->SetFillColor(kBlue);

    grAsymDrAll[p] = new TMultiGraph();
    grAsymDrAll[p]->Add(grYieldPlane[p]);
    grAsymDrAll[p]->Add(grB1L0[p]);

    grAsymDrAll[p]->SetTitle(Form("Yield for run %d",runnum));
    grAsymDrAll[p]->Draw("AB");//for some reason!:Tmultigraph wants the axis settings to come after having drawn the graph
    grAsymDrAll[p]->GetXaxis()->SetTitle("strip number");
    grAsymDrAll[p]->GetYaxis()->SetTitle("charge normalized accum counts (Hz/uA)");
    grAsymDrAll[p]->GetXaxis()->SetLabelSize(0.06);
    grAsymDrAll[p]->GetYaxis()->SetLabelSize(0.06);
    grAsymDrAll[p]->GetXaxis()->SetLimits(1,65); 
    grAsymDrAll[p]->GetXaxis()->SetNdivisions(416, kFALSE);

    legYield[p] = new TLegend(0.101,0.75,0.44,0.9);
    legYield[p]->AddEntry(grYieldPlane[p],"background corrected detector yield","lpe");
    legYield[p]->AddEntry("crossSecFit","theoretical cross-section","l");
    legYield[p]->AddEntry(grB1L0[p],"background(Hz/uA)","lpe");
    legYield[p]->SetFillColor(0);
    legYield[p]->Draw();
  }//for (Int_t p =startPlane; p <endPlane; p++)
  polList.close();

  cAsym->SaveAs(Form("%s/%s/%sasymFit.png",pPath,webDirectory,filePrefix.Data()));
  cResidual->SaveAs(Form("%s/%s/%sasymFitResidual.png",pPath,webDirectory,filePrefix.Data()));
  cYield->SaveAs(Form("%s/%s/%sYieldFit.png",pPath,webDirectory,filePrefix.Data()));

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to execute asymFit.C\n",div_output.quot,div_output.rem );  
}

/*Comments
 *The acceptance limit for the ratio of the background subtracted Signal over Signal is set at 10%
 *stripNum is a 2D array so as to hold the different set of strips that may be unmasked
 */
