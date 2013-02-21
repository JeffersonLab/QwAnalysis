#include <rootClass.h>
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "infoDAQ.C"

Int_t fileReadDraw(Int_t runnum,TString dataType="Ac") 
{
  cout<<"\nstarting into fileReadDraw.C**************\n"<<endl;
  Bool_t kBkgdAsym = 0;
  Bool_t asymDiffPlot=0;//plots the difference in asymmetry as obtained from PWTL1 - PWTL2
  Bool_t yieldPlot=0;//now the asymFit.C plots this already
  Bool_t compareAsym=1;//compares asymmetries from dataType and dataType2(defined locally)
  Bool_t asymComponents=0;
  Bool_t scalerPlot=0;
  Bool_t lasWisePlotAc=0;//plot quantities against laser-cycle 
  Bool_t lasWisePlotSc=0;//plot quantities against laser-cycle 
  Bool_t lasWisePlotBcm=0;//plot quantities against laser-cycle 
  Bool_t lasWisePlotLasPow=0;
  Bool_t bkgdVsBeam=0;//plots quantities againt beam current variations
  Bool_t bkgdSubVsBeam=0;//plots background subtracted compton rates against diff. beam currents
  Bool_t asymLasCyc=0;//plots the experimental asymmetery against laser cycle for a given strip
  Bool_t debug=1,debug1=0,debug2=0;
  const Int_t maxLasCycles=100;
  filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TLine *myline = new TLine(1,0,64,0);
  ifstream in1, in2;
  ifstream expAsymPWTL1,expAsymPWTL2,expAsymComponents,scalerRates,lasCycScaler;
  ofstream newTheoFile;
  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetShadowColor(0);
  pvTxt1->SetBorderSize(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  std::vector<std::vector <Double_t> > stripNum,strAsymNr,strAsymDr,strAsymDrEr;

  Double_t qNormAccumB1H0L0[nPlanes][nStrips][100],qNormAccumB1H0L1[nPlanes][nStrips][100],qNormAccumB1H1L0[nPlanes][nStrips][100],qNormAccumB1H1L1[nPlanes][nStrips][100];
  Double_t qNormAccumB1L0[nPlanes][nStrips][100];//,qNormScalerB1L0[nPlanes][nStrips][100];

  Double_t asymDiff[nPlanes][nStrips],zero[nPlanes][nStrips];
  Double_t accumB1L0[nPlanes][nStrips],accumB1L0Er[nPlanes][nStrips];
  if(!maskSet) infoDAQ(runnum);

  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  gStyle->SetTitleOffset(1.05,"X");
  gStyle->SetTitleOffset(0.6,"Y");
  gStyle->SetTitleSize(0.05,"X");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetLabelSize(0.06,"xyz");

  for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t s=startStrip; s<endStrip; s++) {
      zero[p][s]=0;
      accumB1L0[p][s]=0.0,accumB1L0Er[p][s]=0.0;
    }
  }

  if (kBkgdAsym) {
    ofstream bkgdAsymFit;
    gStyle->SetOptFit(1);
    TF1 *linearFit = new TF1("linearFit", "pol0");
    linearFit->SetLineColor(kBlue);

    TCanvas *cbkgdAsym = new TCanvas("cbkgdAsym",Form("bkgdAsym for run:%d",runnum),70,70,1000,420*endPlane);
    TGraphErrors *grB1L0[nPlanes];
    TLegend *legbkgdAsym[nPlanes];
    Double_t bkgdAsymVal[nPlanes], bkgdAsymValEr[nPlanes],bkgdAsymFitChiSqr[nPlanes],bkgdAsymNDF[nPlanes];
    bkgdAsymFit.open(Form("%s/%s/%s"+dataType+"BkgdAsymFitInfo.txt",pPath,webDirectory,filePrefix.Data()));
    bkgdAsymFit<<Form(";plane\tbkgdAsymVal\tbkgdAsymValEr\tbkgdAsymFit-ChiSqr/NDF")<<endl;

    cbkgdAsym->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cbkgdAsym->GetPad(p+1)->SetGridx(1);
      cbkgdAsym->cd(p+1);

      grB1L0[p] = new TGraphErrors(Form("%s/%s/%s"+dataType+"BkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grB1L0[p]->SetLineColor(kBlue);
      grB1L0[p]->SetMarkerStyle(kFullSquare);
      grB1L0[p]->SetMarkerSize(1);
      grB1L0[p]->SetMarkerColor(kBlue);
      grB1L0[p]->SetFillColor(kBlue);
      grB1L0[p]->SetTitle("laser off asymmetry");
      grB1L0[p]->SetTitle(Form("bkgdAsym for run %d",runnum));
      grB1L0[p]->Draw("AP");//for some reason!:Tmultigraph wants the axis settings to come after having drawn the graph
      grB1L0[p]->GetXaxis()->SetTitle("strip number");
      grB1L0[p]->GetYaxis()->SetTitle("background asymmetry");
      grB1L0[p]->SetMaximum(0.048); //this limit is the same as the residuals
      grB1L0[p]->SetMinimum(-0.048);

      grB1L0[p]->GetXaxis()->SetLimits(1,65); 
      grB1L0[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grB1L0[p]->Fit(linearFit);

      bkgdAsymVal[p] = linearFit->GetParameter(0);
      bkgdAsymValEr[p] = linearFit->GetParError(0);
      bkgdAsymFitChiSqr[p] = linearFit->GetChisquare();
      bkgdAsymNDF[p] = linearFit->GetNDF();
      bkgdAsymFit<<Form("%1.0f\t%f\t%f\t%f\n",(Float_t)p+1,bkgdAsymVal[p],bkgdAsymValEr[p],bkgdAsymFitChiSqr[p]/bkgdAsymNDF[p]);

      legbkgdAsym[p] = new TLegend(0.1,0.75,0.35,0.9);
      legbkgdAsym[p]->AddEntry(grB1L0[p],"background asymmetry","lp");
      legbkgdAsym[p]->AddEntry("linearFit","linear fit","l");
      legbkgdAsym[p]->SetFillColor(0);
      legbkgdAsym[p]->Draw();
    }
    bkgdAsymFit.close();
    cbkgdAsym->Update();    
    cbkgdAsym->SaveAs(Form("%s/%s/%sbkgdAsym.png",pPath,webDirectory,filePrefix.Data()));
    gStyle->SetOptFit(0);
  }

  if(scalerPlot) { 
    std::vector<std::vector <Double_t> > stripNum2,scalerB1L1,scalerB1L0,bkgdSubscalerB1;
    ///these plots are not biased by what I think are the masked strips
    TCanvas *cNoise = new TCanvas("cNoise",Form("scalers for run:%d",runnum),30,30,1000,420*endPlane);
    TGraph *grScalerLasOn[nPlanes],*grScalerLasOff[nPlanes],*grScalerBkgdSub[nPlanes];
    TLegend *legScaler[nPlanes];
    TMultiGraph *grScalerLasCyc[nPlanes];
    cNoise->Divide(startPlane+1,endPlane);

    for(Int_t p =startPlane; p <endPlane; p++) {
      scalerRates.open(Form("%s/%s/%s"+dataType+"QnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      if(scalerRates.is_open()) {
	if(p>=(Int_t)stripNum2.size()) {
	  stripNum2.resize(p+1);
	  scalerB1L1.resize(p+1);
	  scalerB1L0.resize(p+1);
	  bkgdSubscalerB1.resize(p+1);
	}
	cout<<"Reading the Scaler rate file for plane "<<p+1<<endl;
	if(debug) cout<<";\tstripNum\tlasOnScalerCounts\tlasOffScalerCounts"<<endl;
	while(scalerRates.good()) {
	  stripNum2[p].push_back(0.0);
	  scalerB1L1[p].push_back(0.0);
	  scalerB1L0[p].push_back(0.0);
	  bkgdSubscalerB1[p].push_back(0.0);
	  Int_t s=stripNum2[p].size() - 1;
	  scalerRates>>stripNum2[p][s]>>scalerB1L1[p][s]>>scalerB1L0[p][s];
	  bkgdSubscalerB1[p][s] = scalerB1L1[p][s] - scalerB1L0[p][s];
	  if(debug) printf("[%d][%d]:%2.0f\t%f\t%f\n",p+1,s+1,stripNum2[p][s],scalerB1L1[p][s],scalerB1L0[p][s]);
	}
	scalerRates.close();
      } else cout<<"did not find "<<Form("%s/%s/%s"+dataType+"QnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;

      cNoise->GetPad(p+1)->SetGridx(1);
      cNoise->cd(p+1);
      Int_t newSize=stripNum2[p].size();
      if(debug) cout<<"for plane "<<p+1<<" the no.of active strips is "<<newSize<<endl;
      grScalerLasOn[p] = new TGraph(newSize,stripNum2[p].data(),scalerB1L1[p].data());
      grScalerLasOn[p]->SetMarkerStyle(kFullCircle);
      grScalerLasOn[p]->SetMarkerSize(1);
      grScalerLasOn[p]->SetLineColor(kGreen);
      grScalerLasOn[p]->SetMarkerColor(kGreen);
      grScalerLasOn[p]->SetFillColor(kGreen);

      grScalerLasOff[p]= new TGraph(newSize,stripNum2[p].data(),scalerB1L0[p].data());
      grScalerLasOff[p]->SetMarkerStyle(kOpenCircle);
      grScalerLasOff[p]->SetMarkerColor(kBlue);
      grScalerLasOff[p]->SetMarkerSize(1);

      grScalerBkgdSub[p]= new TGraph(newSize,stripNum2[p].data(),bkgdSubscalerB1[p].data());
      grScalerBkgdSub[p]->SetMarkerStyle(kFullDotLarge);
      grScalerBkgdSub[p]->SetMarkerColor(kRed);
      grScalerBkgdSub[p]->SetMarkerSize(1.1);

      grScalerLasCyc[p] = new TMultiGraph();
      grScalerLasCyc[p]->Add(grScalerLasOff[p]);
      grScalerLasCyc[p]->Add(grScalerLasOn[p]);
      grScalerLasCyc[p]->Add(grScalerBkgdSub[p]);
      grScalerLasCyc[p]->SetTitle(Form("Scaler counts for run %d",runnum));
      grScalerLasCyc[p]->Draw("AP");
      grScalerLasCyc[p]->GetXaxis()->SetTitle("strip number");
      grScalerLasCyc[p]->GetYaxis()->SetTitle("charge normalized Scalers(Hz)");
      grScalerLasCyc[p]->GetXaxis()->SetLimits(1,65); 
      grScalerLasCyc[p]->GetXaxis()->SetNdivisions(416, kFALSE);
    
      legScaler[p] = new TLegend(0.6,0.85,0.9,0.97);//(0.1,0.7,0.35,0.9);//x1,y1,x2,y2
      legScaler[p]->AddEntry(grScalerLasOn[p],"laser on scaler(Hz)","p");
      legScaler[p]->AddEntry(grScalerLasOff[p],"laser off scaler(Hz)","p");
      legScaler[p]->AddEntry(grScalerBkgdSub[p],"lasOn-lasOff counts(Hz)","p");
      legScaler[p]->SetFillColor(0);
      legScaler[p]->Draw();
    }
    gPad->Update();
    cNoise->Update();
    cNoise->SaveAs(Form("%s/%s/%sqNormScaler.png",pPath,webDirectory,filePrefix.Data()));
  }


  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymComponents.open(Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymComponents.is_open()) {
      if(p<=(Int_t)stripNum.size()) {
	stripNum.resize(p+1),strAsymDr.resize(p+1),strAsymDrEr.resize(p+1),strAsymNr.resize(p+1);
      }
      cout<<"Reading the yield and difference from PWTL1 for plane "<<p+1<<endl;
      if(debug1) cout<<";\tstripNum\tstrAsymDr\tstrAsymDrEr\tstrAsymNr\n"<<endl;
      while(expAsymComponents.good()) {
	stripNum[p].push_back(0.0),strAsymDr[p].push_back(0.0),strAsymDrEr[p].push_back(0.0),strAsymNr[p].push_back(0.0);
	Int_t s=stripNum[p].size() - 1;
	expAsymComponents>>stripNum[p][s]>>strAsymDr[p][s]>>strAsymDrEr[p][s]>>strAsymNr[p][s];
	if(debug1) printf("[%d][%d]:%2.0f\t%f\t%f\t%f\n",p+1,s+1,stripNum[p][s],strAsymDr[p][s],strAsymDrEr[p][s],strAsymNr[p][s]);
      }
      expAsymComponents.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  if(asymComponents) {
    TCanvas *cAsymComponent = new TCanvas("cAsymDiff",Form("Nr. of Asym for run:%d",runnum),30,30,1000,420*endPlane);
    TGraphErrors *grAsymNr[nPlanes];
    cAsymComponent->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cAsymComponent->GetPad(p+1)->SetGridx(1);
      cAsymComponent->cd(p+1);
      Int_t newSize=stripNum[p].size();
      if(debug) cout<<"for plane "<<p+1<<" the no.of active strips is "<<newSize<<endl;
      grAsymNr[p]= new TGraphErrors(newSize,stripNum[p].data(),strAsymNr[p].data(),zero[p],strAsymDrEr[p].data());
      grAsymNr[p]->SetTitle(Form("asymmetry numerator, run %d",runnum));
      grAsymNr[p]->GetXaxis()->SetTitle("strip number");
      grAsymNr[p]->GetYaxis()->SetTitle("numerator of asym (a.u)");
      grAsymNr[p]->SetMarkerStyle(kOpenCircle);
      grAsymNr[p]->SetMarkerSize(1);
      grAsymNr[p]->SetLineColor(kGreen);
      grAsymNr[p]->SetMarkerColor(kGreen);
      grAsymNr[p]->SetFillColor(kGreen);
      grAsymNr[p]->GetXaxis()->SetLimits(1,65); 
      grAsymNr[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grAsymNr[p]->Draw("A P");
    }
    gPad->Update();
    cAsymComponent->Update();
    cAsymComponent->SaveAs(Form("%s/%s/%sasymNrAllPlanes.png",pPath,webDirectory,filePrefix.Data()));
  }

  if (yieldPlot) {
    TCanvas *cAsymDr = new TCanvas("cAsymDr",Form("Yield for run:%d",runnum),70,70,1000,420*endPlane);
    TGraphErrors *grAsymDr[nPlanes],*grB1L0[nPlanes];
    TLegend *legYield[nPlanes];
    TMultiGraph *grAsymDrAll[nPlanes];

    cAsymDr->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cAsymDr->GetPad(p+1)->SetGridx(1);
      cAsymDr->cd(p+1);
      grAsymDr[p] = new TGraphErrors(Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grAsymDr[p]->SetMarkerStyle(kFullCircle);
      grAsymDr[p]->SetMarkerSize(1);
      grAsymDr[p]->SetLineColor(kGreen);
      grAsymDr[p]->SetMarkerColor(kGreen);
      grAsymDr[p]->SetFillColor(kGreen);

      grB1L0[p] = new TGraphErrors(Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grB1L0[p]->SetLineColor(kBlue);
      grB1L0[p]->SetMarkerStyle(kFullSquare);
      grB1L0[p]->SetMarkerSize(1);
      grB1L0[p]->SetMarkerColor(kBlue);
      grB1L0[p]->SetFillColor(kBlue);

      grAsymDrAll[p] = new TMultiGraph();
      grAsymDrAll[p]->Add(grAsymDr[p]);
      grAsymDrAll[p]->Add(grB1L0[p]);
      grAsymDrAll[p]->SetTitle(Form("Yield for run %d",runnum));
      grAsymDrAll[p]->Draw("AP");//for some reason!:Tmultigraph wants the axis settings to come after having drawn the graph
      grAsymDrAll[p]->GetXaxis()->SetTitle("strip number");
      grAsymDrAll[p]->GetYaxis()->SetTitle("charge normalized accum counts (Hz/uA)");
      grAsymDrAll[p]->GetXaxis()->SetLimits(1,65); 
      grAsymDrAll[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      
      legYield[p] = new TLegend(0.1,0.7,0.4,0.9);
      legYield[p]->AddEntry(grAsymDr[p],"background corrected yield(Hz/uA)","lp");
      legYield[p]->AddEntry(grB1L0[p],"background(Hz/uA)","lp");
      legYield[p]->SetFillColor(0);
      legYield[p]->Draw();
    }
    gPad->Update();
    cAsymDr->Update();
    cAsymDr->SaveAs(Form("%s/%s/%syieldAllPlanes.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(asymDiffPlot) {
    Double_t stripAsym_v1[nPlanes][nStrips],stripAsymEr_v1[nPlanes][nStrips];
    Double_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
    //Int_t dummyStrip[nPlanes];
    for(Int_t p =startPlane; p <endPlane; p++) {
      expAsymPWTL1.open(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      //open the dataType to be compared below
      expAsymPWTL2.open(Form("%s/%s/%sAcExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));//only plane1 is used for v2 data
      if(expAsymPWTL1.is_open() && expAsymPWTL2.is_open()) {
	cout<<"Reading the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
	cout<<"Reading the expAsym corresponding to PWTL2 for Plane "<<p+1<<endl;
	if(debug) cout<<"stripNum\t"<<"stripAsym_v1\t"<<"stripAsym_v2\t"<<"asymDiff"<<endl;
	for(Int_t s =startStrip ; s < endStrip; s++) {
	  if (!mask[p][s]) continue;
	  expAsymPWTL1>>stripNum[p][s]>>stripAsym_v1[p][s]>>stripAsymEr_v1[p][s];
	  expAsymPWTL2>>stripNum[p][s]>>stripAsym_v2[p][s]>>stripAsymEr_v2[p][s];
	  asymDiff[p][s] = (stripAsym_v1[p][s]- stripAsym_v2[p][s]);
	  if(debug) cout<<stripNum[p][s]<<"\t"<<stripAsym_v1[p][s]<<"\t"<<stripAsym_v2[p][s]<<"\t"<<asymDiff[p][s]<<endl;
	}
	expAsymPWTL1.close();
	expAsymPWTL2.close();
      }
      else cout<<"did not find one of the expAsym files eg:"<<Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
    }

    TCanvas *cDiff = new TCanvas("cDiff",Form("expAsym Diff for run:%d",runnum),1,1,1000,420*endPlane);
    TGraphErrors *grDiffPWTL1_2[nPlanes];
    cDiff->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cDiff->cd(p+1);
      cDiff->GetPad(p+1)->SetGridx(1);
      //!!following line needs to be fixed in wake of the modified variable definition
      grDiffPWTL1_2[p] = new TGraphErrors(endStrip,stripNum[p].data(),asymDiff[p],zero[p],stripAsymEr_v1[p]);
      grDiffPWTL1_2[p]->SetMarkerStyle(kOpenSquare);
      grDiffPWTL1_2[p]->SetMarkerSize(0.6);
      grDiffPWTL1_2[p]->SetMarkerColor(kRed+2);
      grDiffPWTL1_2[p]->SetLineColor(kRed+2);
      grDiffPWTL1_2[p]->GetXaxis()->SetTitle("strip number");
      grDiffPWTL1_2[p]->GetYaxis()->SetTitle("expAsym difference (PWTL1-PWTL2)");
      grDiffPWTL1_2[p]->SetMaximum(0.005);
      grDiffPWTL1_2[p]->SetMinimum(-0.005);
      grDiffPWTL1_2[p]->GetXaxis()->SetLimits(1,65); 
      grDiffPWTL1_2[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grDiffPWTL1_2[p]->SetTitle("Diff in exp-asymmetry due to different trigger width");
      grDiffPWTL1_2[p]->Draw("AP");
    }
    cDiff->Update();
    cDiff->SaveAs(Form("%s/%s/%sdiffexpAsym.png",pPath,webDirectory,filePrefix.Data()));
  }

  if (compareAsym) {
    //!temp TString dataType2 = "Sc";//"Rl";// "Rl";////!to be edited for every new comparision type
    TString dataType2 = "Sc";//"Rl";// "Rl";////!to be edited for every new comparision type
    Bool_t kCompEr = 1;
    TLegend *leg;
    leg = new TLegend(0.1,0.7,0.4,0.9);
    TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",50,50,1000,700);
    TCanvas *cAsymComp = new TCanvas("cAsymComp","Asymmetry"+dataType+" Vs Asymmetry"+dataType2,50,50,1000,1000);
    // // cAsymComp->Draw();
    // // 
    // // TPad *pAsymComp = new TPad("pAsymComp","pAsymComp",0,0,0.99,0.99);
    // // pAsymComp->Draw();
    // // pAsymComp->cd();
    TGraphErrors *grDiff[nPlanes], *grAsymPlane[nPlanes],*grFort,*grAsymComp[nPlanes];
    ifstream lasCycAsymP1,fortranOutP1;
    Double_t stripAsym_v2[nPlanes][nStrips],stripNum2[nPlanes][nStrips],stripAsym2[nPlanes][nStrips],asymDiff2[nPlanes][nStrips];
    Double_t stripAsymEr_v2[nPlanes][nStrips],stripAsymEr2[nPlanes][nStrips];
    Int_t size=0;
    TF1 *linearFit = new TF1("linearFit", "pol0",1,55);//!for this run, the Cedge is 55, hardcoded
    //TF1 *pol1Fit = new TF1("pol1Fit", "pol1",1,55);//!for this run, the Cedge is 55, hardcoded
    gStyle->SetOptFit(1);

    cAsym->Divide(1,2);//(startPlane+1,endPlane);
    //for (Int_t p = startPlane; p <endPlane; p++) {
    for (Int_t p = 0; p <1; p++) {
      cAsym->cd(1);//(p+1);
      cAsym->GetPad(1)->SetGridx(1); 
      lasCycAsymP1.open(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      grAsymPlane[p] = new TGraphErrors(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
      grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
      grAsymPlane[p]->SetTitle(Form("comparing experimental asymmetry"));
      grAsymPlane[p]->SetMarkerStyle(kFullCircle);
      grAsymPlane[p]->SetLineColor(kBlue);
      grAsymPlane[p]->SetFillColor(0);
      grAsymPlane[p]->SetMarkerColor(kBlue); ///kRed+2 = Maroon
      grAsymPlane[p]->SetMaximum(0.042);
      grAsymPlane[p]->SetMinimum(-0.042);
      grAsymPlane[p]->GetXaxis()->SetLimits(1,65); 
      grAsymPlane[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grAsymPlane[p]->Draw("AP");

      myline->SetLineStyle(1);
      myline->Draw();    
      leg->AddEntry(grAsymPlane[p],"asymmetry from "+dataType+" data","lpf");
      leg->SetFillColor(0);
 
      if(dataType2=="Rl") {
	fortranOutP1.open("/home/narayan/acquired/vladas/run.24519");
	cout<<"used /home/narayan/acquired/vladas/run.24519 file for comparision to "+dataType<<endl;
      } else {
	fortranOutP1.open(Form("%s/%s/%s"+dataType2+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
	cout<<"used "<<Form("%s/%s/%s"+dataType2+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" for comparision to "<<dataType<<endl;
      }
      if(fortranOutP1.is_open()) {
	if(dataType2=="Rl") grFort = new TGraphErrors("/home/narayan/acquired/vladas/run.24519", "%lg %lg %lg");
	else grFort = new TGraphErrors(Form("%s/%s/%s"+dataType2+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
	grFort->SetMarkerColor(kRed);
	grFort->SetMarkerStyle(24);
	grFort->SetFillColor(0);
	grFort->SetLineColor(kRed);
	grFort->Draw("P");
	leg->AddEntry(grFort,"asymmetry from "+dataType2+" data","lpf");
      } else cout<<red<<"\ncorresponding fortran file for run "<<runnum<<" doesn't exist\n"<<normal<<endl;
      leg->Draw();

      cAsym->cd(2);//to plot the residuals
      cAsym->GetPad(2)->SetGridx(1); 
      if(lasCycAsymP1.is_open() && fortranOutP1.is_open()) {
	if(debug) cout<<"str#\t"<<"asymLasCyc\t"<<"asymRunLet\t"<<"asymDiff\t"<<"asymRatio"<<endl;
	while(1) {
	  lasCycAsymP1>>stripNum2[p][size]>>stripAsym2[p][size]>>stripAsymEr2[p][size];
	  fortranOutP1>>stripNum2[p][size]>>stripAsym_v2[p][size]>>stripAsymEr_v2[p][size];
	  asymDiff2[p][size] = (stripAsym2[p][size]- stripAsym_v2[p][size]);
	  //asymRatio[p][size] = (stripAsym2[p][size]/ stripAsym_v2[p][size]);
	  //asymRatioEr[p][size] = stripAsymEr2[p][size]/ stripAsym_v2[p][size];
	  if(debug) cout<<stripNum2[p][size]<<"\t"<<stripAsym2[p][size]<<"\t"<<stripAsym_v2[p][size]<<"\t"<<asymDiff2[p][size]<<endl;
	  size++;
	  if((Int_t)stripNum2[p][size] >= 55) {
	    cout<<"reached the Compton edge"<<endl;
	    break;
	  }
	  if(lasCycAsymP1.eof()) {
	    cout<<"reached the end of laser cycle asym file\n"<<endl;
	    break;
	  }
	}
	lasCycAsymP1.close();
	fortranOutP1.close();
      } else cout<<"did not find one of the expAsym files eg:"<<Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
      cout<<"size is "<<size<<endl;
      grDiff[p] = new TGraphErrors(size,stripNum2[p],asymDiff2[p],zero[p],stripAsymEr2[p]);
      grDiff[p]->GetXaxis()->SetTitle("strip number");
      grDiff[p]->GetYaxis()->SetTitle("(LaserWise - RunletBased Asym)");
      grDiff[p]->SetTitle(Form("difference of above asymmetry"));
      grDiff[p]->SetMaximum(0.0042);
      grDiff[p]->SetMinimum(-0.0042);
      grDiff[p]->Fit(linearFit,"ER");
      grDiff[p]->SetLineColor(kBlue);
      grDiff[p]->GetXaxis()->SetLimits(1,65); 
      grDiff[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grDiff[p]->Draw("AP");

      // //pAsymComp->cd();
      cAsymComp->cd();
      cAsymComp->SetGridx(1); 
      grAsymComp[p] = new TGraphErrors(size,stripAsym2[p],stripAsym_v2[p],stripAsymEr2[p],stripAsymEr_v2[p]);
      grAsymComp[p]->SetTitle(Form("comparision of asymmetry data"));
      grAsymComp[p]->SetMarkerStyle(kOpenSquare);
      grAsymComp[p]->GetXaxis()->SetTitle("asymmetry from "+dataType+" Mode");
      grAsymComp[p]->GetXaxis()->SetNdivisions(5,4,0);
      grAsymComp[p]->GetXaxis()->SetTitleSize();
      grAsymComp[p]->GetXaxis()->SetLabelSize();

      grAsymComp[p]->GetYaxis()->SetTitle("asymmetry from "+dataType2+" Mode");
      grAsymComp[p]->GetYaxis()->SetNdivisions(5,4,0);
      grAsymComp[p]->GetYaxis()->SetTitleSize();
      grAsymComp[p]->GetYaxis()->SetTitleOffset(1.2);
      grAsymComp[p]->GetYaxis()->SetLabelSize();

      grAsymComp[p]->Fit("pol1","E");
      grAsymComp[p]->Draw("AP");     
      // //pAsymComp->Update(); 
      //cAsymComp->Update();
      // // TPaveStats *psAsymComp = (TPaveStats*)grAsymComp[p]->GetListOfFunctions()->FindObject("stats");
      // // psAsymComp->SetX1NDC(0.60); psAsymComp->SetX2NDC(0.90); psAsymComp->SetY1NDC(0.75);psAsymComp->SetY2NDC(0.90);
      // // //psAsymComp->SetTextColor(kBlue);
    }
    cAsym->Update();
    cAsymComp->Update();

    gStyle->SetOptFit(0);
    cAsym->SaveAs(Form("%s/%s/%scompareAsymDiff"+dataType+dataType2+".png",pPath,webDirectory,filePrefix.Data()));
    cAsymComp->SaveAs(Form("%s/%s/%scompare"+dataType+dataType2+"Asym.png",pPath,webDirectory,filePrefix.Data()));

    if(kCompEr) {
    TCanvas *cPolErr = new TCanvas("cPolErr","polarization error",10,10,600,600);
    TPad *pPolErr = new TPad("pPolErr","pPolErr",0,0,0.99,0.99);
    pPolErr->Draw();
    pPolErr->cd();
    TH1D *hPolEr = new TH1D("polEr laser Cycle", "pol error comparision for run 24519", size, 0, 0.0001);
    TH1D *hRunletPolEr = new TH1D("polEr runlet based", "polarization error comparision", size, 0, 0.0001);
    hPolEr->SetBit(TH1::kCanRebin);
    hRunletPolEr->SetBit(TH1::kCanRebin);
    for(Int_t s=startStrip; s<size && stripNum2[0][s]<=55; s++) {//!plotting only upto Cedge
      hPolEr->Fill(stripAsymEr2[0][s]);
      hRunletPolEr->Fill(stripAsymEr_v2[0][s]);      
    }
    hPolEr->GetXaxis()->SetNdivisions(5,4,0);
    hPolEr->SetLineColor(kBlue);
    hPolEr->Draw("H");
    pPolErr->Update();
    TPaveStats *ps1 = (TPaveStats*)hPolEr->GetListOfFunctions()->FindObject("stats");
    ps1->SetX1NDC(0.60); ps1->SetX2NDC(0.90); ps1->SetY1NDC(0.75);ps1->SetY2NDC(0.90);
    ps1->SetTextColor(kBlue);
    
    hRunletPolEr->SetLineColor(kRed);
    hRunletPolEr->Draw("H,sames"); ///!note the added 's' in the sames; that forces both the stat boxes to be drawn
    pPolErr->Update();
    TPaveStats *ps2 = (TPaveStats*)hRunletPolEr->GetListOfFunctions()->FindObject("stats");
    ps2->SetX1NDC(0.60); ps2->SetX2NDC(0.90); ps2->SetY1NDC(0.55); ps2->SetY2NDC(0.70);
    ps2->SetTextColor(kRed);
    pPolErr->Modified();
    cPolErr->SaveAs(Form("%s/%s/%scompareError.png",pPath,webDirectory,filePrefix.Data()));
    }
  }

  if(lasWisePlotAc) {
    ifstream lasCycAccum;
    Double_t nCycle[100];//arbitrarily set to 100 which certainly will be larger than nLasCycles
    TGraph *lasCycPlot[endStrip];
    TCanvas *cAccumLC1 = new TCanvas("cAccumLC1",Form("accum counts per laser cycle for strips 01-16"),0,0,1200,1200);
    //TCanvas *cAccumLC2 = new TCanvas("cAccumLC2",Form("accum counts per laser cycle for strips 17-32"),20,10,1200,1200);
    //TCanvas *cAccumLC3 = new TCanvas("cAccumLC3",Form("accum counts per laser cycle for strips 33-48"),40,20,1200,1200);
    //TCanvas *cAccumLC4 = new TCanvas("cAccumLC4",Form("accum counts per laser cycle for strips 49-64"),60,30,1200,1200);

    cAccumLC1->Divide(4,4);
    //cAccumLC2->Divide(4,4);
    //cAccumLC3->Divide(4,4);
    //cAccumLC4->Divide(4,4);

    //for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t p =0; p <1; p++) {
      for(Int_t s=startStrip;s<endStrip;s++) { 
	if (!mask[p][s]) continue; //currently only for plane 1	
 	lasCycAccum.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycAccumP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
	Int_t nLasCycles=0;
	if(lasCycAccum.is_open()) {
	  while(lasCycAccum.good()) {
	    lasCycAccum>>nCycle[nLasCycles]>>qNormAccumB1H0L0[p][s][nLasCycles]>>qNormAccumB1H0L1[p][s][nLasCycles]>>qNormAccumB1H1L0[p][s][nLasCycles]>>qNormAccumB1H1L1[p][s][nLasCycles];
	    qNormAccumB1L0[p][s][nLasCycles] = (qNormAccumB1H0L0[p][s][nLasCycles]+qNormAccumB1H1L0[p][s][nLasCycles])/2;
	    if(debug1) printf("%f\t%f\t%f\t%f\n",nCycle[nLasCycles],qNormAccumB1H0L0[p][s][nLasCycles],qNormAccumB1H1L0[p][s][nLasCycles],qNormAccumB1L0[p][s][nLasCycles]);
	    nLasCycles=nLasCycles+1;
	  }
	  lasCycAccum.close();
	} else cout<<"didn't find the file for lasCycAccum"<<endl;
	lasCycPlot[s] = new TGraph(nLasCycles,nCycle,qNormAccumB1L0[p][s]);
	if(s>=0 && s<16)  cAccumLC1->cd(s+1);
	//if(s>=16 && s<32) cAccumLC2->cd(s-16+1);
	//if(s>=32 && s<48) cAccumLC3->cd(s-32+1);
	//if(s>=48 && s<64) cAccumLC4->cd(s-48+1);
	lasCycPlot[s]->SetTitle(Form("Strip %d",s+1));
	lasCycPlot[s]->SetMarkerStyle(kFullCircle);
	lasCycPlot[s]->SetMarkerColor(kRed);
	lasCycPlot[s]->SetLineColor(kRed);
	lasCycPlot[s]->SetLineWidth(2);
	lasCycPlot[s]->GetXaxis()->SetTitle("laser Cycle (#)");
	lasCycPlot[s]->GetYaxis()->SetTitle("charge normalized accum counts (Hz/uA)");
	lasCycPlot[s]->Draw("AP");
      }
      cAccumLC1->Update();
      //cAccumLC2->Update();
      //cAccumLC3->Update();
      //cAccumLC4->Update();
      cAccumLC1->SaveAs(Form("%s/%s/%slasCycAccum1.png",pPath,webDirectory,filePrefix.Data()));
      //cAccumLC2->SaveAs(Form("%s/%s/%slasCycAccum2.png",pPath,webDirectory,filePrefix.Data()));
      //cAccumLC3->SaveAs(Form("%s/%s/%slasCycAccum3.png",pPath,webDirectory,filePrefix.Data()));
      //cAccumLC4->SaveAs(Form("%s/%s/%slasCycAccum4.png",pPath,webDirectory,filePrefix.Data()));
    }
  }

  if(lasWisePlotBcm) {
    TGraph *lasCycBCML1,*lasCycBCML0,*lasCycBCM;
    TCanvas *cLasCycBCM = new TCanvas("cLasCycBCM","Beam stability laser cycle",50,50,700,800);
    TLegend *legBCM[2];
    ifstream lasWiseBCM;
    Double_t nCycle[100];//arbitrarily set to 100 which certainly will be larger than nLasCycles
    Double_t bcmH0L0[200],bcmH0L1[200],bcmH1L0[200],bcmH1L1[200];//!this implicitly puts a limitation on the no.of laser cycles it can handle
    Double_t bcmL0[200],bcmL1[200],bcm[200];
    lasWiseBCM.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycBcmAvg.txt",pPath,webDirectory,runnum,runnum));
    Int_t nLasCycles=0;
    while(lasWiseBCM.good()) {
      lasWiseBCM>>nCycle[nLasCycles]>>bcmH0L0[nLasCycles]>>bcmH0L1[nLasCycles]>>bcmH1L0[nLasCycles]>>bcmH1L1[nLasCycles];
      bcmL0[nLasCycles] = (bcmH0L0[nLasCycles]+bcmH1L0[nLasCycles])/2;//!ideally this addition should be charge weigthed
      bcmL1[nLasCycles] = (bcmH0L1[nLasCycles]+bcmH1L1[nLasCycles])/2;//!ideally this addition should be charge weigthed
      bcm[nLasCycles] = (bcmL0[nLasCycles]+bcmL1[nLasCycles])/2;//!ideally not correct, this addition should be charge weigthed
      nLasCycles=nLasCycles+1;
    }
    lasWiseBCM.close();
    cLasCycBCM->Divide(1,2);
    cLasCycBCM->cd(1);
    lasCycBCML1= new TGraph(nLasCycles,nCycle,bcmL1);
    lasCycBCML1->SetMarkerStyle(kFullCircle);
    lasCycBCML1->SetMarkerColor(kGreen);

    lasCycBCML0= new TGraph(nLasCycles,nCycle,bcmL0);
    //lasCycBCML0->SetTitle(Form("las-off BCM for run %d",runnum));
    lasCycBCML0->SetMarkerStyle(kOpenCircle);
    lasCycBCML0->SetMarkerColor(kBlue);
    //lasCycBCML0->SetLineWidth(2);

    TMultiGraph *lasCycBCMall = new TMultiGraph();
    lasCycBCMall->Add(lasCycBCML1);
    lasCycBCMall->Add(lasCycBCML0);
    lasCycBCMall->SetTitle(Form("BCM for run %d",runnum));
    lasCycBCMall->Draw("AP");
    lasCycBCMall->GetXaxis()->SetTitle("laser Cycle");
    lasCycBCMall->GetYaxis()->SetTitle("beam (uA)");

    legBCM[0] = new TLegend(0.6,0.85,0.9,0.95);//x1,y1,x2,y2//(0.1,0.7,0.4,0.9)
    legBCM[0]->AddEntry(lasCycBCML0,"Laser off BCM per laser cycle","p");
    legBCM[0]->AddEntry(lasCycBCML1,"Laser on BCM per laser cycle","p");
    legBCM[0]->SetFillColor(0);
    legBCM[0]->Draw();

    cLasCycBCM->cd(2);
    lasCycBCM= new TGraph(nLasCycles,nCycle,bcm);
    lasCycBCM->SetTitle(Form("BCM per laser cycle for run %d",runnum));
    lasCycBCM->SetMarkerStyle(kFullCircle);
    lasCycBCM->SetMarkerColor(kRed  );
    lasCycBCM->SetLineWidth(2);
    lasCycBCM->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycBCM->GetYaxis()->SetTitle("beam (uA)");
    lasCycBCM->Draw("AP");
    legBCM[1] = new TLegend(0.7,0.83,0.9,0.94);//x1,y1,x2,y2//(0.1,0.7,0.4,0.9)
    legBCM[1]->AddEntry(lasCycBCM,"avg BCM per laser cycle","p");
    legBCM[1]->SetFillColor(0);
    legBCM[1]->Draw();

    cLasCycBCM->Update();
    cLasCycBCM->SaveAs(Form("%s/%s/%slasCycBCM.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(lasWisePlotLasPow) {
    TGraph *lasCycLasPowL0,*lasCycLasPowL1;
    TCanvas *cLasCycPow = new TCanvas("cLasCycPow","Laser Power stability",0,0,600,400);
    ifstream lasWisePow;
    Double_t nCycle[100];//arbitrarily set to 100 which certainly will be larger than nLasCycles
    Double_t lasPowB1H0L0[200],lasPowB1H1L0[200],lasPowB1L0[200];//!this implicitly puts a limitation on the no.of laser cycles it can handle
    Double_t lasPowB1H0L1[200],lasPowB1H1L1[200],lasPowB1L1[200];//!this implicitly puts a limitation on the no.of laser cycles it can handle
    lasWisePow.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycAvgLasPow.txt",pPath,webDirectory,runnum,runnum));
    Int_t nLasCycles=0;
    while(lasWisePow.good()) {
      lasWisePow>>nCycle[nLasCycles]>>lasPowB1H0L0[nLasCycles]>>lasPowB1H0L1[nLasCycles]>>lasPowB1H1L0[nLasCycles]>>lasPowB1H1L1[nLasCycles];
      lasPowB1L0[nLasCycles] = (lasPowB1H0L0[nLasCycles] + lasPowB1H1L0[nLasCycles])/2;
      lasPowB1L1[nLasCycles] = (lasPowB1H0L1[nLasCycles] + lasPowB1H1L1[nLasCycles])/2;
      nLasCycles=nLasCycles+1;
    }
    lasWisePow.close();
    //cLasCycPow->Divide(1,2);
    //cLasCycPow->cd(1);
    lasCycLasPowL0= new TGraph(nLasCycles,nCycle,lasPowB1L0);
    lasCycLasPowL0->SetTitle(Form("Laser Power for run %d",runnum));
    lasCycLasPowL0->SetMarkerStyle(kOpenCircle);
    lasCycLasPowL0->SetMarkerColor(kBlue);
    lasCycLasPowL0->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycLasPowL0->GetYaxis()->SetTitle("laser Power (a.u)");

    //cLasCycPow->cd(1);
    lasCycLasPowL1= new TGraph(nLasCycles,nCycle,lasPowB1L1);
    lasCycLasPowL1->SetMarkerStyle(kFullCircle);
    lasCycLasPowL1->SetMarkerColor(kGreen);

    TMultiGraph *lasCycLasPow = new TMultiGraph();
    lasCycLasPow->Add(lasCycLasPowL0);
    lasCycLasPow->Add(lasCycLasPowL1);
    lasCycLasPow->Draw("AP");
    lasCycLasPow->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycLasPow->GetYaxis()->SetTitle("laser Power (a.u)");

    cLasCycPow->Update();
    cLasCycPow->SaveAs(Form("%s/%s/run_%d/edetLasCyc_%d_lasCycLasPow.png",pPath,webDirectory,runnum,runnum));
  }

  if(lasWisePlotSc) {
    Double_t nCycle[maxLasCycles];//arbitrarily set to 100 which certainly will be larger than nLasCycles
    Double_t qNormScalerB1H0L0[nPlanes][nStrips][maxLasCycles],qNormScalerB1H0L1[nPlanes][nStrips][maxLasCycles],qNormScalerB1H1L0[nPlanes][nStrips][maxLasCycles],qNormScalerB1H1L1[nPlanes][nStrips][maxLasCycles];
    TGraph *lasCycPlotSc[endStrip];
    TCanvas *cScalerLC1 = new TCanvas("cScalerLC1",Form("scaler counts per laser cycle for strips 01-16"),0,0,1200,1200);
    Double_t qNormScalerB1L0[nPlanes][nStrips][maxLasCycles];
//     TCanvas *cScalerLC2 = new TCanvas("cScalerLC2",Form("scaler counts per laser cycle for strips 17-32"),20,10,1200,1200);
//     TCanvas *cScalerLC3 = new TCanvas("cScalerLC3",Form("scaler counts per laser cycle for strips 33-48"),40,20,1200,1200);
//     TCanvas *cScalerLC4 = new TCanvas("cScalerLC4",Form("scaler counts per laser cycle for strips 49-64"),60,30,1200,1200);
    
    cScalerLC1->Divide(4,4);
//     cScalerLC2->Divide(4,4);
//     cScalerLC3->Divide(4,4);
//     cScalerLC4->Divide(4,4);

    //for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t p =0; p <1; p++) {
      for(Int_t s=startStrip;s<endStrip;s++) { 
	if (!mask[p][s]) continue; //currently only for plane 1	
 	lasCycScaler.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycScalerP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
	Int_t nLasCycles=0;
	if(lasCycScaler.is_open()) {
	  while(lasCycScaler.good()) {
	    lasCycScaler>>nCycle[nLasCycles]>>qNormScalerB1H0L0[p][s][nLasCycles]>>qNormScalerB1H0L1[p][s][nLasCycles]>>qNormScalerB1H1L0[p][s][nLasCycles]>>qNormScalerB1H1L1[p][s][nLasCycles];
	    qNormScalerB1L0[p][s][nLasCycles] = (qNormScalerB1H0L0[p][s][nLasCycles]+qNormScalerB1H1L0[p][s][nLasCycles])/2;//!ideally not correct, this addition should be charge weigthed
	    nLasCycles=nLasCycles+1;
	  }
	  lasCycScaler.close();
	} else cout<<"did not find the file for lasCycScaler"<<endl;
	lasCycPlotSc[s]= new TGraph(nLasCycles,nCycle,qNormScalerB1H0L0[p][s]);

	if(s>= 0 && s<16) cScalerLC1->cd(s+1);
// 	if(s>=16 && s<32) cScalerLC2->cd(s-16+1);
// 	if(s>=32 && s<48) cScalerLC3->cd(s-32+1);
// 	if(s>=48 && s<64) cScalerLC4->cd(s-48+1);

	lasCycPlotSc[s]->SetTitle(Form("Strip %d",s+1));
	lasCycPlotSc[s]->SetMarkerStyle(kFullCircle);
	lasCycPlotSc[s]->SetMarkerColor(kRed);
	lasCycPlotSc[s]->SetLineColor(kRed);
	lasCycPlotSc[s]->SetLineWidth(2);
	lasCycPlotSc[s]->GetXaxis()->SetTitle("laser Cycle (#)");
	lasCycPlotSc[s]->GetYaxis()->SetTitle("charge normalized accum counts (Hz/uA)");
	lasCycPlotSc[s]->Draw("AP");
      }
    }
    cScalerLC1->Update();
    //cScalerLC2->Update();
    //cScalerLC3->Update();
    //cScalerLC4->Update();
    cScalerLC1->SaveAs(Form("%s/%s/%slasCycScaler1.png",pPath,webDirectory,filePrefix.Data()));
    //cScalerLC2->SaveAs(Form("%s/%s/%slasCycScaler2.png",pPath,webDirectory,filePrefix.Data()));
    //cScalerLC3->SaveAs(Form("%s/%s/%slasCycScaler3.png",pPath,webDirectory,filePrefix.Data()));
    //cScalerLC4->SaveAs(Form("%s/%s/%slasCycScaler4.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(bkgdVsBeam) {
    gStyle->SetOptFit(1);
    ofstream noiseFit;
    TGraphErrors *ScalerVsBeam[endStrip]; 
    ifstream infilelasOffBkgd,scalerRate;
    const Int_t numb=6;
    Double_t beamI[numb]={30,50,60,100,150,180};//!assumed 1-to-1 matching between runlist and beamI array
    Double_t stripNumber[nStrips][numb],stripBkgdYield[nStrips][numb],stripBkgdYieldEr[nStrips][numb],zero1[numb];
    Int_t runlist[numb]={23142,23148,23154,23151,23152,23168};
    TString filePrefix2;
    Double_t p0,p1,NDF,chiSq,p1Er,p0Er;
    /*run  : beam***********
     *23142 :  30uA, 
     *23148 :  50uA, 
     *23151 : 100uA, yield does not make sense
     *23152 : 150uA, yield doesn't make sense
     *23153 : 150uA, yield doesn't make sense
     *23154 :  60uA,
     *23168 : 180uA,
     *25029 : 110uA,
     *25032 :  90uA
     ***************/

    for(Int_t n=0; n<numb; n++) {        
      filePrefix2= Form("run_%d/edetLasCyc_%d_",runlist[n],runlist[n]);   
      for(Int_t p =startPlane; p <endPlane; p++) {
	infilelasOffBkgd.open(Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix2.Data(),p+1));
	if(debug1)cout<<"opening file "<<Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix2.Data(),p+1)<<endl;
	if(infilelasOffBkgd.is_open()) {
	  Int_t strip=0;
	  while(infilelasOffBkgd.good()) {
	    infilelasOffBkgd>>stripNumber[strip][n]>>stripBkgdYield[strip][n]>>stripBkgdYieldEr[strip][n];
	    strip = strip +1;
	  }
	  infilelasOffBkgd.close();
	  if(debug1)cout<<"closing file "<<Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix2.Data(),p+1)<<endl;     
	} else {
	  if(debug)cout<<"did not find "<<Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix2.Data(),p+1)<<endl;
	  return -1;
	}
      }
      zero1[n] = 0.0;
    }
    
    TF1 *bkgdFit = new TF1("bkgdFit","[0] + [1]/x",25,105);//specifying current range to fit
    bkgdFit->SetParameters(-0.1,20);
    noiseFit.open(Form("%s/%s/noiseFit.txt",pPath,webDirectory));
    noiseFit<<";strip\tp0\tp0Er\tp1\tp1Er\tchiSq\tNDF"<<endl;
    
    TCanvas *cScalVsBeam1 = new TCanvas("cScalVsBeam1",Form("scaler counts per laser cycle for strips 01-16"),0,0,1200,1000);
    //    TCanvas *cScalVsBeam2 = new TCanvas("cScalVsBeam2",Form("scaler counts per laser cycle for strips 17-32"),20,10,1200,1000);
    //     TCanvas *cScalVsBeam3 = new TCanvas("cScalVsBeam3",Form("scaler counts per laser cycle for strips 33-48"),40,20,1200,1000);
    //     TCanvas *cScalVsBeam4 = new TCanvas("cScalVsBeam4",Form("scaler counts per laser cycle for strips 49-64"),60,30,1200,1000);
    
    cScalVsBeam1->Divide(4,4);
    //     cScalVsBeam2->Divide(4,4);
    //     cScalVsBeam3->Divide(4,4);
    //     cScalVsBeam4->Divide(4,4);
    
    for(Int_t s=startStrip;s<endStrip;s++) {
      //!the masked strips are not taken care of properly here
      ScalerVsBeam[s] = new TGraphErrors(numb,beamI,stripBkgdYield[s],zero1,stripBkgdYieldEr[s]);
      
      if(s>= 0 && s<16) cScalVsBeam1->cd(s+1);
      //       if(s>=16 && s<32) cScalVsBeam2->cd(s-16+1);
      //       if(s>=32 && s<48) cScalVsBeam3->cd(s-32+1);
      //       if(s>=48 && s<64) cScalVsBeam4->cd(s-48+1);
      
      ScalerVsBeam[s]->SetTitle(Form("Strip %d",s+1));
      ScalerVsBeam[s]->SetMarkerStyle(kFullCircle);
      ScalerVsBeam[s]->SetMarkerSize(1);
      ScalerVsBeam[s]->SetMarkerColor(kRed);
      ScalerVsBeam[s]->SetLineColor(kRed);
      ScalerVsBeam[s]->SetLineWidth(2);
      ScalerVsBeam[s]->GetXaxis()->SetTitle("beam current (uA)");
      ScalerVsBeam[s]->GetYaxis()->SetTitle("qNorm (bkgd) accum counts (Hz/uA)");
      //cScalVsBeam1->GetPad(2)->SetGridx(1); 
      ScalerVsBeam[s]->Draw("APE");
      ScalerVsBeam[s]->Fit("bkgdFit","0 R M E");
      p0 = bkgdFit->GetParameter(0);
      p0Er = bkgdFit->GetParError(0);
      p1 = bkgdFit->GetParameter(1);
      p1Er = bkgdFit->GetParError(1);
      chiSq = bkgdFit->GetChisquare();
      NDF = bkgdFit->GetNDF();
      noiseFit<<s+1<<"\t"<<p0<<"\t"<<p0Er<<p1<<"\t"<<p1Er<<"\t"<<chiSq<<"\t"<<NDF<<endl;     
    }
    noiseFit.close();
    gPad->Update();
    cScalVsBeam1->Update();
    //     cScalVsBeam2->Update();
    //     cScalVsBeam3->Update();
    //     cScalVsBeam4->Update();
    cScalVsBeam1->SaveAs(Form("%s/%s/%sScalVsBeamStr01_16.png",pPath,webDirectory,filePrefix2.Data()));
    //     cScalVsBeam2->SaveAs(Form("%s/%s/%sScalVsBeamStr17_32.png",pPath,webDirectory,filePrefix2.Data()));
    //     cScalVsBeam3->SaveAs(Form("%s/%s/%sScalVsBeamStr33_48.png",pPath,webDirectory,filePrefix2.Data()));
    //     cScalVsBeam4->SaveAs(Form("%s/%s/%sScalVsBeamStr49_64.png",pPath,webDirectory,filePrefix2.Data()));
  }
  
  if(bkgdSubVsBeam) {
    Double_t nCycle[maxLasCycles];//arbitrarily set to 100 which certainly will be larger than nLasCycles
    const Int_t numb=6;
    ifstream scalerRate;
    ofstream bkgdSubCountsFit;
    TGraphErrors *bkgdSubScalVsBeam[nPlanes][nStrips],*ScalVsBeamL1[nPlanes][nStrips],*ScalVsBeamL0[nPlanes][nStrips];
    TMultiGraph *ScalVsBeam[endPlane][endStrip];
    TLegend *legScVsBeam[endPlane][endStrip];
    Int_t runlist[numb]={23142,23148,23154,23151,23152,23168};
    TString filePrefix3;
    //Double_t beamI[numb]={30,50,60,100,150,180};//!assumed 1-to-1 matching between runlist and beamI array
    Double_t beamI[numb]={34,54,64,103,152,180};//!assumed 1-to-1 matching between runlist and beamI array
    Double_t qNormScalerB1H0L0[nPlanes][nStrips][maxLasCycles],qNormScalerB1H0L1[nPlanes][nStrips][maxLasCycles],qNormScalerB1H1L0[nPlanes][nStrips][maxLasCycles],qNormScalerB1H1L1[nPlanes][nStrips][maxLasCycles];
    Double_t qNormScalerB1L1[nPlanes][nStrips][maxLasCycles],qNormScalerB1L0[nPlanes][nStrips][maxLasCycles],qNormScalerB1[nPlanes][nStrips][maxLasCycles];
    Double_t qNormScB1[nPlanes][nStrips][numb],qNormScL1[nPlanes][nStrips][numb],qNormScL0[nPlanes][nStrips][numb];
    Double_t zero2[numb];
    Double_t p0,p1,NDF,chiSq,p1Er,p0Er;
    /*run  : beam***********
     *23142 :  30uA, 
     *23148 :  50uA, 
     *23151 : 100uA, yield does not make sense
     *23152 : 150uA, yield doesn't make sense
     *23153 : 150uA, yield doesn't make sense
     *23154 :  60uA,
     *23168 : 180uA,
     *25029 : 110uA,
     *25032 :  90uA
     ***************/
    Int_t chosenCyc=0;///in human counting
    for(Int_t n=0; n<numb; n++) {        
      filePrefix3= Form("run_%d/edetLasCyc_%d_",runlist[n],runlist[n]);   
      for(Int_t p =startPlane; p <endPlane; p++) {
	for(Int_t s=startStrip;s<endStrip;s++) {
	  if (!mask[p][s]) continue; //!!currently only for plane 1	
	  scalerRate.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycScalerP%dS%d.txt",pPath,webDirectory,runlist[n],runlist[n],p+1,s+1));
	  if (scalerRate.is_open()) {
	    Int_t nLasCycles=0;
	    while(scalerRate.good()) {
	      scalerRate>>nCycle[nLasCycles]>>qNormScalerB1H0L0[p][s][nLasCycles]>>qNormScalerB1H0L1[p][s][nLasCycles]>>qNormScalerB1H1L0[p][s][nLasCycles]>>qNormScalerB1H1L1[p][s][nLasCycles];
	      qNormScalerB1L1[p][s][nLasCycles] = (qNormScalerB1H0L1[p][s][nLasCycles]);//+qNormScalerB1H1L1[p][s][nLasCycles])/2;//!ideally this addition should be charge weigthed
	      qNormScalerB1L0[p][s][nLasCycles] = (qNormScalerB1H0L0[p][s][nLasCycles]);//+qNormScalerB1H1L0[p][s][nLasCycles])/2;//!ideally this addition should be charge weigthed
	      qNormScalerB1[p][s][nLasCycles] = qNormScalerB1L1[p][s][nLasCycles] - qNormScalerB1L0[p][s][nLasCycles];
	      nLasCycles=nLasCycles+1;//!this nLasCycles is not necessarily in sync with the laser cycle number
	    }
	    scalerRate.close();	  
	    if(debug2)cout<<"closing file "<<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycScalerP%dS%d.txt",pPath,webDirectory,runlist[n],runlist[n],p+1,s+1)<<endl;
	  } else {
	    cout<<"did not find "          <<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycScalerP%dS%d.txt",pPath,webDirectory,runlist[n],runlist[n],p+1,s+1)<<endl;
	    return -1;
	  }
	}
      
	//!intend to use a chosen nCycle for a given run for the evaluation
	switch (n) {
	case 0:
	  chosenCyc = (Int_t)nCycle[2];//14;//run 23142 //34.5uA
	  break;
	case 1:
	  chosenCyc = (Int_t)nCycle[11];//run 23148 //54.1uA
	  break;
	case 2:
	  chosenCyc = (Int_t)nCycle[1];//run 23154  //64.2uA
	  break;
	case 3:
	  chosenCyc = (Int_t)nCycle[2];//run 23151//103.7uA
	  break;
	case 4:
	  chosenCyc = (Int_t)nCycle[4];//run 23152 //152.4uA
	  break;
	case 5:
	  chosenCyc = (Int_t)nCycle[8];//run 23168 //180.7uA
	  break;
	}
	cout<<"selected laser cycle "<<chosenCyc<<" for run: "<<runlist[n]<<endl;
	if(debug2)printf("qNormScB1\tqNormScL1\tqNormScL0\tnumber for chosenCyc#\n");
	for(Int_t pl =startPlane; pl <endPlane; pl++) {//!!this is currently valid for only plane 1
	  for(Int_t s=startStrip;s<endStrip;s++) {
	    if (!mask[pl][s]) continue; 
	    qNormScB1[pl][s][n] = qNormScalerB1[pl][s][chosenCyc-1];//!note that only one laser cycle is used
	    qNormScL1[pl][s][n] = qNormScalerB1L1[pl][s][chosenCyc-1];//!note that only one laser cycle is used
	    qNormScL0[pl][s][n] = qNormScalerB1L0[pl][s][chosenCyc-1];//!note that only one laser cycle is used
	    if(debug2)printf("%f\t%f\t%f\t%d\t%d\n",qNormScB1[pl][s][n],qNormScL1[pl][s][n],qNormScL0[pl][s][n],n,chosenCyc);
	  }
	}
	zero2[n] = 0.0;
      }
    }
    
    TF1 *bkgdFit = new TF1("bkgdFit","[0] + [1]/x",25,105);//specifying current range to fit
    bkgdFit->SetParameters(-0.1,20);
    bkgdSubCountsFit.open(Form("%s/%s/bkgdSubCountsFit.txt",pPath,webDirectory));
    if(debug) bkgdSubCountsFit<<";strip\tp0\tp0Er\tp1\tp1Er\tchiSq\tNDF"<<endl;
    
    //TCanvas *cScalVsBeamSet1 = new TCanvas("cScalVsBeamB1L1H1Set1",Form("LasOn scalers per laser cycle :strips 01-16"),0,0,1200,1000);
    //TCanvas *cScalVsBeamB1L0H1Set1 = new TCanvas("cScalVsBeamB1L0H1Set1",Form("LasOff scalers per laser cycle:strips 01-16"),0,0,1200,1000);
    TCanvas *cBkgdSubScalVsBeam1 = new TCanvas("cBkgdSubScalVsBeam1",Form("Bkgd subtracted scalers per lasCyc for strips 01-16"),0,0,1200,1000);
    TCanvas *cBkgdSubScalVsBeam2 = new TCanvas("cBkgdSubScalVsBeam2",Form("Bkgd subtracted scalers for  17-32"),20,10,1200,1000);
    TCanvas *cBkgdSubScalVsBeam3 = new TCanvas("cBkgdSubScalVsBeam3",Form("Bkgd subtracted scalers for  33-48"),40,20,1200,1000);
    TCanvas *cBkgdSubScalVsBeam4 = new TCanvas("cBkgdSubScalVsBeam4",Form("Bkgd subtracted scalers for  49-64"),60,30,1200,1000);
    
    cBkgdSubScalVsBeam1->Divide(4,4);
    cBkgdSubScalVsBeam2->Divide(4,4);
    cBkgdSubScalVsBeam3->Divide(4,4);
    cBkgdSubScalVsBeam4->Divide(4,4);
    
    for(Int_t p =startPlane; p <endPlane; p++) {//!!this is currently valid for only plane 1
      for(Int_t s=startStrip;s<endStrip;s++) {
	//!the masked strips are not taken care of properly here
	bkgdSubScalVsBeam[p][s] = new TGraphErrors(numb,beamI,qNormScB1[p][s],zero2,zero2);
	ScalVsBeamL1[p][s] = new TGraphErrors(numb,beamI,qNormScL1[p][s],zero2,zero2);
	ScalVsBeamL0[p][s] = new TGraphErrors(numb,beamI,qNormScL0[p][s],zero2,zero2);
      
	if(s>= 0 && s<16) cBkgdSubScalVsBeam1->cd(s+1);
	if(s>=16 && s<32) cBkgdSubScalVsBeam2->cd(s-16+1);
	if(s>=32 && s<48) cBkgdSubScalVsBeam3->cd(s-32+1);
	if(s>=48 && s<64) cBkgdSubScalVsBeam4->cd(s-48+1);
      
	bkgdSubScalVsBeam[p][s]->SetTitle(Form("Strip %d",s+1));
	bkgdSubScalVsBeam[p][s]->SetMarkerStyle(kFullCircle);
	bkgdSubScalVsBeam[p][s]->SetMarkerSize(1);
	bkgdSubScalVsBeam[p][s]->SetMarkerColor(kRed);
	bkgdSubScalVsBeam[p][s]->SetLineColor(kRed);
	bkgdSubScalVsBeam[p][s]->SetLineWidth(2);
	bkgdSubScalVsBeam[p][s]->Fit("bkgdFit","0R");//ME

	ScalVsBeamL1[p][s]->SetMarkerStyle(kFullCircle);
	ScalVsBeamL1[p][s]->SetMarkerColor(kGreen);
	ScalVsBeamL0[p][s]->SetMarkerStyle(kOpenCircle);
	ScalVsBeamL0[p][s]->SetMarkerColor(kBlue);

	ScalVsBeam[p][s] = new TMultiGraph();
	ScalVsBeam[p][s]->Add(ScalVsBeamL0[p][s]);
	ScalVsBeam[p][s]->Add(ScalVsBeamL1[p][s]);
	ScalVsBeam[p][s]->Add(bkgdSubScalVsBeam[p][s]);
	ScalVsBeam[p][s]->Draw("AP");
	ScalVsBeam[p][s]->SetTitle(Form("strip %d",s+1));
	ScalVsBeam[p][s]->GetXaxis()->SetTitle("beam current(uA)");
	ScalVsBeam[p][s]->GetYaxis()->SetTitle("scalers (Hz/uA)");

	legScVsBeam[p][s] = new TLegend(0.6,0.85,0.9,0.95);//x1,y1,x2,y2//(0.1,0.7,0.4,0.9)
	legScVsBeam[p][s]->AddEntry(ScalVsBeamL1[p][s],"LasOn scalers","p");
	legScVsBeam[p][s]->AddEntry(ScalVsBeamL0[p][s],"LasOff scalers","p");
	legScVsBeam[p][s]->AddEntry(bkgdSubScalVsBeam[p][s],"Las On-Off scalers","p");
	legScVsBeam[p][s]->SetFillColor(0);
	legScVsBeam[p][s]->Draw();

	p0 = bkgdFit->GetParameter(0);
	p0Er = bkgdFit->GetParError(0);
	p1 = bkgdFit->GetParameter(1);
	p1Er = bkgdFit->GetParError(1);
	chiSq = bkgdFit->GetChisquare();
	NDF = bkgdFit->GetNDF();
	bkgdSubCountsFit<<s+1<<"\t"<<p0<<"\t"<<p0Er<<p1<<"\t"<<p1Er<<"\t"<<chiSq<<"\t"<<NDF<<endl;     
      }
      bkgdSubCountsFit.close();
      cBkgdSubScalVsBeam1->SaveAs(Form("%s/%s/%sScalVsBeamStr01_16.png",pPath,webDirectory,filePrefix3.Data()));
      cBkgdSubScalVsBeam2->SaveAs(Form("%s/%s/%sScalVsBeamStr17_32.png",pPath,webDirectory,filePrefix3.Data()));
      cBkgdSubScalVsBeam3->SaveAs(Form("%s/%s/%sScalVsBeamStr33_48.png",pPath,webDirectory,filePrefix3.Data()));
      cBkgdSubScalVsBeam4->SaveAs(Form("%s/%s/%sScalVsBeamStr49_64.png",pPath,webDirectory,filePrefix3.Data()));
    }
  }

  if (asymLasCyc) {
    gStyle->SetOptFit(1);
    TF1 *linearFit = new TF1("linearFit", "pol0");
    TGraphErrors *grAsymLasCyc[nStrips];
    TCanvas *cAsymLC1 = new TCanvas("cAsymLC1","asymmetry per laser cycle for strips 01-16",10, 0,1200,1200);
    TCanvas *cAsymLC2 = new TCanvas("cAsymLC2","asymmetry per laser cycle for strips 17-32",30,10,1200,1200);
    TCanvas *cAsymLC3 = new TCanvas("cAsymLC3","asymmetry per laser cycle for strips 33-48",50,20,1200,1200);
    TCanvas *cAsymLC4 = new TCanvas("cAsymLC4","asymmetry per laser cycle for strips 49-64",70,30,1200,1200);
    cAsymLC1->Divide(4,4);
    cAsymLC2->Divide(4,4);
    cAsymLC3->Divide(4,4);
    cAsymLC4->Divide(4,4);

    //for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t p =0; p <1; p++) {
      for(Int_t s=startStrip;s<endStrip;s++) { 
	if (!mask[p][s]) continue; //currently only for plane 1	
	grAsymLasCyc[s] = new TGraphErrors(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_asymPerLasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1),"%lg %lg %lg");

	if(s>= 0 && s<16) {
	  cAsymLC1->cd(s+1);
	  cAsymLC1->GetPad(s+1)->SetGridx(1);
	}
	if(s>=16 && s<32) {
	  cAsymLC2->cd(s-16+1);
	  cAsymLC2->GetPad(s-16+1)->SetGridx(1);
	}
	if(s>=32 && s<48) {
	  cAsymLC3->cd(s-32+1);
	  cAsymLC1->GetPad(s-32+1)->SetGridx(1);
	}
	if(s>=48 && s<64) {
	  cAsymLC4->cd(s-48+1);
	  cAsymLC1->GetPad(s-48+1)->SetGridx(1);
	}
	grAsymLasCyc[s]->SetTitle(Form("asym for Str %d",s+1));
	grAsymLasCyc[s]->SetMarkerStyle(kFullCircle);
	grAsymLasCyc[s]->SetMarkerColor(kBlue);
	grAsymLasCyc[s]->SetLineColor(kBlue);
	grAsymLasCyc[s]->SetLineWidth(2);
	grAsymLasCyc[s]->GetXaxis()->SetTitle("laser Cycle (#)");
	grAsymLasCyc[s]->GetYaxis()->SetTitle("exp. asym");
	grAsymLasCyc[s]->Draw("AP");
	grAsymLasCyc[s]->Fit(linearFit,"q");
      }
    }
    cAsymLC1->Update();
    cAsymLC2->Update();
    cAsymLC3->Update();
    cAsymLC4->Update();
    cAsymLC1->SaveAs(Form("%s/%s/%sasymLasCycStr01_16.png",pPath,webDirectory,filePrefix.Data()));
    cAsymLC2->SaveAs(Form("%s/%s/%sasymLasCycStr17_32.png",pPath,webDirectory,filePrefix.Data()));
    cAsymLC3->SaveAs(Form("%s/%s/%sasymLasCycStr33_48.png",pPath,webDirectory,filePrefix.Data()));
    cAsymLC4->SaveAs(Form("%s/%s/%sasymLasCycStr49_64.png",pPath,webDirectory,filePrefix.Data()));
    gStyle->SetOptFit(0);


    //----------------------------------------------------    
    //attempt to histogram
  // //std::vector<vector <TH1D> > hAsymLC;
  // TH1D *hAsymLC = new TH1D("asym per laser Cycle", "asymmetry per laser cycle", nLasCycles, 0, 0.0001);//!!temp  
  // hAsymLC->SetBit(TH1::kCanRebin);
  // //hAsymLC.resize(endPlane);
  // //     hAsymLC[p].push_back(new TH1D("asym per laser Cycle", "asymmetry per laser cycle", nLasCycles, 0, 0.0001));//!!temp
  // //   }
  // // }
  // Int_t str = 39;//c++ counting
  //   hAsymLC->Fill(qNormLasCycAsym[0][str]);
  //   cout<<blue<<"for strip "<<str<<", nCycle "<<nCycle+1<<" qNormLasCycAsym[0][str]:"<<qNormLasCycAsym[0][str]<<normal<<endl;

  // cAsymLC->cd();
  // hAsymLC->SetLineColor(kBlue);
  // hAsymLC->Draw("H");
  // cAsymLC->SaveAs(Form("%s/%s/%sasymPerLasCycStrip%d.png",pPath,webDirectory,filePrefix.Data(),str+1));


  }

  return runnum;
}
