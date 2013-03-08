//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry
#ifndef __asymFit_F
#define __asymFit_F
#endif

#include <rootClass.h>
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "infoDAQ.C"
Bool_t kVladas_meth=0,kVladas_data=0;
Double_t theoCrossSec(Double_t *thisStrip, Double_t *parCx)//3 parameter fit for cross section
{///parCx[1]: to be found Cedge
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end()) {
    TF1::RejectPoint();
    return 0;
  }
  xStrip = xCedge - (parCx[1] - (*thisStrip))*stripWidth*parCx[0];
  rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
  rhoPlus = 1-rhoStrip*(1+a_const);
  rhoMinus = 1-rhoStrip*(1 - a_const);//just a term in eqn 24
  dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a_const)); // 2nd term of eqn 22
  return (parCx[2]*((rhoStrip*(1 - a_const)*rhoStrip*(1 - a_const)/rhoMinus)+1+dsdrho1*dsdrho1));//eqn.22,without factor 2*pi*(re^2)/a_const
}

///3 parameter method
Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
  //itStrip = skipStrip.find(*thisStrip); //for some reason the std::set did not work !
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end() ) { 
    cout<<red<<"ignored strip: "<<*thisStrip<<normal<<endl;
    TF1::RejectPoint();
    return 0;
  }
  //xStrip = xCedge - (tempCedge + par[1] - (*thisStrip))*stripWidth*par[0];//for 2nd parameter as Cedge offset
  //xStrip = xCedge + par[1] - (tempCedge -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as actual position inside Cedge strip
  //xStrip = xCedge + par[1]*stripWidth - (*thisStrip)*stripWidth*par[0];//Guruji's method of fitting!!didn't work
  //xStrip = xCedge -0.5*stripWidth - (par[1] -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as Cedge itself
  if(!kVladas_meth) {//!my method
  xStrip = xCedge - (par[1] -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as Cedge itself
  rhoStrip = param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3];
  } else {  //!Vladas's numbers
    xStrip = 17.315 - 0.2*(par[1] - (*thisStrip))*par[0]; //for 2nd parameter as Cedge itself
    rhoStrip = 2.81648E-06 + xStrip*0.0602395 + xStrip*xStrip*(-0.000148674) + xStrip*xStrip*xStrip*1.84876E-07 + xStrip*xStrip*xStrip*xStrip*1.05068E-08 + xStrip*xStrip*xStrip*xStrip*xStrip*(-2.537E-10);
    //cout<<red<<(*thisStrip)<<"\t"<<xStrip<<"\t"<<rhoStrip<<normal<<endl;
  }
  rhoPlus = 1-rhoStrip*(1+a_const);
  rhoMinus = 1-rhoStrip*(1 - a_const);//just a term in eqn 24
  dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a_const)); // 2nd term of eqn 22
  dsdrho =((rhoStrip*(1 - a_const)*rhoStrip*(1 - a_const)/rhoMinus)+1+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a_const
  //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  return (par[2]*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
}

void asymFit(Int_t runnum,TString dataType="Ac")
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  //gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  //gStyle->SetCenterTitle();
  gStyle->SetTitleOffset(1.05,"X");
  gStyle->SetTitleSize(0.05,"X");
  gStyle->SetTitleOffset(0.6,"Y");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetLabelSize(0.06,"xyz");
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Double_t pol[nPlanes],polEr[nPlanes],chiSq[nPlanes],effStripWidth[nPlanes],effStripWidthEr[nPlanes];
  Double_t offset[nPlanes],offsetEr[nPlanes];
  Int_t NDF[nPlanes],resFitNDF[nPlanes];
  Double_t resFit[nPlanes],resFitEr[nPlanes], chiSqResidue[nPlanes];
  filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=1,debug1=0,debug2=0;
  Bool_t polSign,kYieldFit=0,kYield=1,kResidual=1;
  ifstream paramfile;
  TPaveText *pt[nPlanes], *ptRes[nPlanes];
  TLegend *leg[nPlanes],*legYield[nPlanes];

  if (!maskSet) infoDAQ(runnum);

  ifstream infileScaler, expAsymPWTL1, infileYield;
  ofstream polList;
  TLine *myline = new TLine(1,0,65,0);
  TF1 *polFit,*linearFit;
  Double_t qNormScBkgdSubSigB1[nPlanes][nStrips];
  std::vector<std::vector <Double_t> > activeStrip,qNormScB1L1,qNormScB1L0;
  Int_t numbGoodStrips[nPlanes]={0};

  if(dataType == "Sc") qNormBkgdSubSigToBkgdRatioLow=1.25;
  if(dataType == "Ac") qNormBkgdSubSigToBkgdRatioLow=4.00;
  if(dataType == "Ev") qNormBkgdSubSigToBkgdRatioLow=1.25;

  ///Note: the 's' in this section of the routine does not necessarily represent strip number
  for(Int_t p =startPlane; p <endPlane; p++) {
    infileScaler.open(Form("%s/%s/%s"+dataType+"QnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(infileScaler.is_open()) {
      if(p>=(Int_t)activeStrip.size()) {
	activeStrip.resize(p+1),qNormScB1L1.resize(p+1),qNormScB1L0.resize(p+1);
      }
      if(debug) cout<<"Reading the qNormScaler corresponding to Plane "<<p+1<<endl;
      if(debug2) cout<<"\np\ts\tbkgdSubScal\tactiveStr\t"<<"qNormScL1\t"<<"qNormScL0\t"<<"(L1-L0)/L1\t"<<"(L1-L0)/L0"<<endl;
      while(infileScaler.good()) {
	activeStrip[p].push_back(0.0);
	qNormScB1L1[p].push_back(0.0);
	qNormScB1L0[p].push_back(0.0);
	Int_t s=activeStrip[p].size() - 1;///this 's' does not represent the actual strip number
	infileScaler>>activeStrip[p][s]>>qNormScB1L1[p][s]>>qNormScB1L0[p][s];///the content of the 'activeStrip' vector contains the true strip # in human counting
	qNormScBkgdSubSigB1[p][s] = qNormScB1L1[p][s] - qNormScB1L0[p][s];
	if(debug2) printf("%d\t%d\t%f\t%2.0f\t%f\t%f\t%f\t%f\n",p+1,s+1,qNormScBkgdSubSigB1[p][s],activeStrip[p][s],qNormScB1L1[p][s],qNormScB1L0[p][s],qNormScBkgdSubSigB1[p][s]/qNormScB1L1[p][s],qNormScBkgdSubSigB1[p][s]/qNormScB1L0[p][s]);
	numbGoodStrips[p]++;//counts in human counting ///this is basically =activeStrip[p].size()
      }
      infileScaler.close();
    } else cout<<"\n*** Alert:couldn't find "<<Form("%s/%s/%s"+dataType+"QnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" needed to generically locate Compton edge"<<endl;
  }

  ///start finding the generic compton edge 
  for(Int_t p =startPlane; p <endPlane; p++) {
    Bool_t trueEdge = 0;
    cout<<"looking for generic compton edge for plane "<<p+1<<endl; 
    for(Int_t s =(Int_t)activeStrip[p][0]; s < numbGoodStrips[p]; s++) {//begin at first activeStrip
      if (qNormScBkgdSubSigB1[p][s]/qNormScB1L0[p][s] <= qNormBkgdSubSigToBkgdRatioLow) { //!'qNormBkgdSubSigToBkgdRatioLow' is set=5.0 
	trueEdge = 1;
	Double_t probableEdge = activeStrip[p][s-1]; ///since the above condition is fulfiled after crossing Cedge
	cout<<"probable Cedge : "<<probableEdge<<endl;
	Int_t leftStrips = numbGoodStrips[p] - (Int_t)probableEdge;
	for(Int_t st =1; st <=leftStrips;st++) {///starting to check next strip onwards
	  if (qNormScBkgdSubSigB1[p][s+st]/qNormScB1L0[p][s+st]  >= qNormBkgdSubSigToBkgdRatioLow) trueEdge = 0;
	  if(debug2) printf("qNormScBkgdSubSigB1[%d][%d]:%f, leftStrips:%d, trueEdge:%d\n",p+1,s+1+st+1,qNormScBkgdSubSigB1[p][s+st+1],leftStrips,trueEdge);
	}
	if (trueEdge) {
	  Cedge[p]= probableEdge;
	  cout<<"\nCompton edge for plane "<<p+1<<" auto-determined to strip "<<Cedge[p]<<"\n"<<endl;
	  break;
	} else cout<<"**** Did not find Cedge for plane "<<p+1<<" in run # "<<runnum<<" in this interation"<<endl;
      } else if(s==numbGoodStrips[p]-1) cout<<"**** Alert:*** Something unusual in Cedge determination for run # "<<runnum<<" Did not find Cedge till last strip"<<endl;
    }
  }
 
  TCanvas *cAsym;
  TGraphErrors *grAsymPlane[nPlanes];

  cAsym = new TCanvas("cAsym","Asymmetry and Strip number",10,10,1000,300*endPlane);
  cAsym->Divide(1,endPlane); 

  Double_t stripNum[nPlanes][nStrips], fitResidue[nPlanes][nStrips];
  Double_t zero[nStrips];

  polList.open(Form("%s/%s/%s"+dataType+"Pol.txt",pPath,webDirectory,filePrefix.Data()));
  polList<<";run\tpol\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\teffStrip\teffStripEr\tplane"<<endl;
  for (Int_t p =startPlane; p <endPlane; p++) {  
    if (!kVladas_meth) xCedge = rhoToX(p); ///this function should be called after determining the Cedge

    paramfile.open(Form("%s/%s/checkfileP%d.txt",pPath,webDirectory,p+1));
    cout<<"reading in the rho to X fitting parameters for plane "<<p+1<<", they were:" <<endl;
    paramfile>>param[0]>>param[1]>>param[2]>>param[3];
    paramfile.close();
    if(debug) printf("%g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3]);
    
    cAsym->cd(p+1);  
    cAsym->GetPad(p+1)->SetGridx(1);

    if(kVladas_data) grAsymPlane[p]=new TGraphErrors("/home/narayan/acquired/vladas/run.24519","%lg %lg %lg");  
    //grAsymPlane[p]=new TGraphErrors("/home/narayan/acquired/vladas/r24519_lasCycAsym_runletErr.txt","%lg %lg %lg");  
    else grAsymPlane[p]=new TGraphErrors(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");

    grAsymPlane[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");   
    grAsymPlane[p]->SetTitle(Form(dataType+" Mode Asymmetry, Plane %d",p+1));//Form("experimental asymmetry Run: %d, Plane %d",runnum,p+1));
    grAsymPlane[p]->SetMarkerStyle(kFullCircle);
    grAsymPlane[p]->SetLineColor(kRed);
    grAsymPlane[p]->SetFillColor(0);   
    grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
    grAsymPlane[p]->SetMaximum(0.048); 
    grAsymPlane[p]->SetMinimum(-0.048);
    grAsymPlane[p]->GetXaxis()->SetLimits(1,65); 
    grAsymPlane[p]->GetXaxis()->SetNdivisions(416, kFALSE);

    grAsymPlane[p]->Draw("AP");  
    tempCedge = Cedge[p];//-0.5;///this should be equated before the declaration of TF1
    linearFit = new TF1("linearFit", "pol0",startStrip+1,tempCedge);
    linearFit->SetLineColor(kRed);

    ///3 parameter fit
    polFit = new TF1("polFit",theoreticalAsym,startStrip+1,tempCedge,3);
    //TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+10,Cedge[p],3);//use strips after the first 10 strips
    polFit->SetParameters(1.0,tempCedge,0.85);//begin the fitting from stripWidth parameter = 1, Cedge=auto-determined, polarization=85%
    //polFit->SetParameters(1.0,0.0001,0.85);//2nd parameter as Compton edge internal position
    
    //polFit->SetParLimits(0,1.021,1.021);//fixing the strip width to 1.021
    polFit->SetParLimits(0,0.8,1.8);///allowing the strip width to be either 80% or 180% of its real pitch    
    polFit->SetParLimits(1,tempCedge,tempCedge);///fixed compton edge
    //polFit->SetParLimits(1,tempCedge-1.0,tempCedge+2.0);///allowing compton edge to vary by -3 strips to upto +2 strips
    polFit->SetParLimits(2,-1.0,1.0);///allowing polarization to be - 100% to +100%

    polFit->SetParNames("effStrip","comptonEdge","polarization");
    polFit->SetLineColor(kBlue);
    if(debug1) cout<<"starting to fit exp asym"<<endl;
    cout<<red<<"the maxdist used:"<<xCedge<<normal<<endl;
    grAsymPlane[p]->Fit("polFit","0 R M E");
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

    if(debug) cout<<"\nwrote the polarization relevant values to file "<<endl;
    polList<<Form("%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%2.3f\t%.3f\t%d\n",(Double_t)runnum,pol[p]*100,polEr[p]*100,chiSq[p],NDF[p],offset[p],offsetEr[p],effStripWidth[p],effStripWidthEr[p],p+1);
    if(debug) {
      cout<<Form("runnum\tpol.\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\teffStripWidth effStripWidthEr plane");
      cout<<Form("\n%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%2.3f\t%.3f\t%d\n\n",(Double_t)runnum,pol[p]*100,polEr[p]*100,chiSq[p],NDF[p],offset[p],offsetEr[p],effStripWidth[p],effStripWidthEr[p],p+1);      
    }
    leg[p] = new TLegend(0.101,0.75,0.43,0.9);
    leg[p]->AddEntry(grAsymPlane[0],"experimental asymmetry","lpe");///I just need the name
    leg[p]->AddEntry("polFit","QED asymmetry fit to exp. asymmetry","l");//"lpf");//
    leg[p]->SetFillColor(0);
    leg[p]->Draw();

    polSign = pol[p] > 0 ? 1 : 0;
    if (polSign) pt[p] = new TPaveText(0.44,0.12,0.68,0.48,"brNDC");///left edge,bottom edge,right edge, top edge
    else  pt[p] = new TPaveText(0.44,0.52,0.68,0.88,"brNDC");
    if(debug) cout<<"polSign is: "<<polSign<<endl;

    pt[p]->SetTextSize(0.060);//0.028); 
    pt[p]->SetBorderSize(1);
    pt[p]->SetTextAlign(12);
    pt[p]->SetFillColor(-1);
    pt[p]->SetShadowColor(-1);

    pt[p]->AddText(Form("chi Sq / ndf       : %.3f",chiSq[p]/NDF[p]));
    pt[p]->AddText(Form("eff. strip width  : %2.3f #pm %2.3f",effStripWidth[p],effStripWidthEr[p]));
    //pt[p]->AddText(Form("Compton Edge      : %f #pm %f",Cedge[p]+offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Compton Edge : %2.2f #pm %2.2f",offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Polarization      : %2.2f #pm %2.2f",pol[p]*100.0,polEr[p]*100.0));
    pt[p]->Draw();
    myline->Draw();
  }//for (Int_t p =startPlane; p <endPlane; p++)
  
  if(kResidual) {
    TCanvas *cResidual;
    TGraphErrors *grResiduals[nPlanes]; 
    cResidual = new TCanvas("cResidual","residual (Asymmetry - Fit)",10,500,900,300*endPlane);
    cResidual->Divide(1,endPlane); 

    for (Int_t p =startPlane; p <endPlane; p++) {
      ///determining the residue of the above fit
      for (Int_t s = startStrip; s < endStrip; s++) {//ensure that all variables gets initiated by zero
	stripNum[p][s]=0.0,stripAsym[p][s]=0.0,stripAsymEr[p][s]=0.0;
	stripAsymDr[p][s]=0.0,stripAsymNr[p][s]=0.0,stripAsymDrEr[p][s]=0.0;
	zero[s] = 0.0;
      }
      if(kVladas_data) expAsymPWTL1.open("/home/narayan/acquired/vladas/run.24519");
      else expAsymPWTL1.open(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      if(expAsymPWTL1.is_open()) {
	if(debug2) cout<<"Reading the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
	if(debug2) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
	for(Int_t s =startStrip ; s < endStrip; s++) {
	  if (!mask[p][s]) continue;
	  expAsymPWTL1>>stripNum[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	  if(debug2) cout<<stripNum[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
	}
	expAsymPWTL1.close();
      }
      else cout<<"did not find the expAsym file "<<Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;

      if(debug2) cout<<"fitResidue[p][s]\tstripAsym[p][s]\tpolFit->Eval(s+1)"<<endl;
      for (Int_t s = startStrip; s <= Cedge[p]; s++) {
	if (!mask[p][s]) continue;
	fitResidue[p][s] = stripAsym[p][s] - polFit->Eval(s+1);
	if(debug2) cout<<fitResidue[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<polFit->Eval(s+1)<<endl;
      }
      cResidual->cd(p+1);  
      cResidual->GetPad(p+1)->SetGridx(1);
      grResiduals[p]=new TGraphErrors((Int_t)Cedge[p],stripNum[p],fitResidue[p],zero,stripAsymEr[p]);
      grResiduals[p]->SetMarkerStyle(kOpenCircle);
      grResiduals[p]->SetMaximum(0.012);/// half of asymmetry axis 
      grResiduals[p]->SetMinimum(-0.012);/// half of asymmetry axis 
      grResiduals[p]->GetXaxis()->SetLimits(1,Cedge[p]); 
      grResiduals[p]->SetTitle(Form(dataType+" Mode Asymmetry Fit Residual, Plane %d",p+1));
      grResiduals[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
      grResiduals[p]->GetYaxis()->SetTitle("asymmetry - Fit");
 
      grResiduals[p]->Draw("AP");
      grResiduals[p]->Fit(linearFit,"REq");//q:quiet mode
 
      resFit[p] = linearFit->GetParameter(0);
      resFitEr[p] = linearFit->GetParError(0);
      chiSqResidue[p] = linearFit->GetChisquare();
      resFitNDF[p] = linearFit->GetNDF();

      ptRes[p] = new TPaveText(0.67,0.67,0.9,0.9,"brNDC");///x1,y1,x2,y2
      ptRes[p]->SetTextSize(0.048);//0.028); 
      ptRes[p]->SetBorderSize(1);
      ptRes[p]->SetTextAlign(12);
      ptRes[p]->SetFillColor(0);
      ptRes[p]->SetShadowColor(-1);
      ptRes[p]->AddText(Form("chi Sq / ndf  : %0.1f / %d",chiSqResidue[p],resFitNDF[p]));
      ptRes[p]->AddText(Form("linear fit    : %f #pm %f",resFit[p],resFitEr[p]));
      ptRes[p]->Draw();
    }//for (Int_t p =startPlane; p <endPlane; p++)
    cResidual->SaveAs(Form("%s/%s/%s"+dataType+"AsymFitResidual.png",pPath,webDirectory,filePrefix.Data()));
  }

  if (kYield) { ///determine yield
    TCanvas *cYield;
    TGraphErrors *grYieldPlane[nPlanes],*grB1L0[nPlanes];
    TMultiGraph *grAsymDrAll[nPlanes];
    cYield = new TCanvas("cYield","detector yield",300,200,1000,300*endPlane);
    cYield->Divide(1,endPlane); 

    for (Int_t p =startPlane; p <endPlane; p++) {
      infileYield.open(Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      if(infileYield.is_open()) {
	if(debug2) cout<<"Reading the Yield corresponding to Plane "<<p+1<<endl;
	if(debug2) cout<<"stripNum\t"<<"stripYield\t"<<"stripYieldEr"<<endl;
	for(Int_t s =startStrip ; s < endStrip; s++) {
	  if (!mask[p][s]) continue;
	  infileYield>>stripNum[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>stripAsymNr[p][s];
	  if(debug2) cout<<stripNum[p][s]<<"\t"<<stripAsymDr[p][s]<<"\t"<<stripAsymDrEr[p][s]<<endl;
	}
	infileYield.close();
      }
      else cout<<"did not find the Yield file "<<Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
     
      cYield->cd(p+1);
      cYield->GetPad(p+1)->SetGridx(1);
      grYieldPlane[p]=new TGraphErrors(Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
      grYieldPlane[p]->SetLineColor(kGreen);
      grYieldPlane[p]->SetFillColor(kGreen);   
      grYieldPlane[p]->SetMarkerColor(kGreen); ///kRed+2 = Maroon
 
      ///3 parameter fit for cross section
      if(kYieldFit) {
	TF1 *crossSecFit = new TF1("crossSecFit",theoCrossSec,startStrip+1,Cedge[p]-1,3);///three parameter fit
	crossSecFit->SetParameters(1,Cedge[p],20.0);//begin the fitting from the generic Cedge
	crossSecFit->SetParLimits(0,0.2,2.0);
	crossSecFit->SetParLimits(1,Cedge[p],Cedge[p]); //effectively fixing the Compton edge    
	crossSecFit->SetLineColor(kRed);
	grYieldPlane[p]->Fit("crossSecFit","0 R M E q");
      }
      grB1L0[p] = new TGraphErrors(Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grB1L0[p]->SetLineColor(kBlue);
      grB1L0[p]->SetMarkerColor(kBlue);
      grB1L0[p]->SetFillColor(kBlue);

      grAsymDrAll[p] = new TMultiGraph();
      grAsymDrAll[p]->Add(grYieldPlane[p]);
      grAsymDrAll[p]->Add(grB1L0[p]);

      grAsymDrAll[p]->SetTitle(Form(dataType+" Mode Yield, Plane %d",p+1));
      grAsymDrAll[p]->Draw("AB");//for some reason!:Tmultigraph wants the axis settings to come after having drawn the graph

      grAsymDrAll[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
      grAsymDrAll[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grAsymDrAll[p]->GetXaxis()->SetLimits(1,65); 
      grAsymDrAll[p]->GetYaxis()->SetTitle("qNorm Counts (Hz/uA)");
 
      legYield[p] = new TLegend(0.101,0.75,0.44,0.9);
      legYield[p]->AddEntry(grYieldPlane[p],"background corrected detector yield","lpe");
      if(kYieldFit) legYield[p]->AddEntry("crossSecFit","theoretical cross-section","l");
      legYield[p]->AddEntry(grB1L0[p],"background(Hz/uA)","lpe");
      legYield[p]->SetFillColor(0);
      legYield[p]->Draw();
    }//for (Int_t p =startPlane; p <endPlane; p++)
    cYield->SaveAs(Form("%s/%s/%s"+dataType+"YieldFit.png",pPath,webDirectory,filePrefix.Data()));
  }
  polList.close();

  //!Vladas
  if(kVladas_data && !kVladas_meth) cAsym->SaveAs(Form("%s/%s/%sasymFit_runletData_anMeth.png",pPath,webDirectory,filePrefix.Data()));
  else if(kVladas_data && kVladas_meth) cAsym->SaveAs(Form("%s/%s/%sasymFit_runletData_vtMeth.png",pPath,webDirectory,filePrefix.Data()));
  else if(!kVladas_data && kVladas_meth) cAsym->SaveAs(Form("%s/%s/%sasymFit_vtMeth.png",pPath,webDirectory,filePrefix.Data()));
  //else if(!kVladas_data && !kVladas_meth) cAsym->SaveAs(Form("%s/%s/%sAcAsymFit.png",pPath,webDirectory,filePrefix.Data()));
  else if(!kVladas_data && !kVladas_meth) cAsym->SaveAs(Form("%s/%s/%s"+dataType+"AsymFit.png",pPath,webDirectory,filePrefix.Data()));

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to execute asymFit.C\n",div_output.quot,div_output.rem );  
}

/*Comments
 *In auto determination of Compton edge, I'm using (L1 - L0)/L0 instead of (L1 - L0)/L1 
 *..because in the latter both Nr and Dr reduce at the Compton edge, hence the former should be more pronounced
 *the use of qNormScBkgdSubSigB1[p][s]/qNormScB1L0[p][s] <= qNormBkgdSubSigToBkgdRatioLow ; is a better comparision
 *..for identifying compton edge compared to qNormScB1L1[p][s]/qNormScB1L0[p][s] <= qNormBkgdSubSigToBkgdRatioLow
 *..since the former puts the magnitude of signal-over-bkgd in the perspective of the background; while the later 
 *..simply does a numerical comparision of the signal-over-bkgd.
 
*The acceptance limit for the ratio of the background subtracted Signal over Signal is set at 10%
 *StripNum is a 2D array so as to hold the different set of strips that may be unmasked
 */
