#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Int_t fileReadDraw(Int_t runnum) 
{
  cout<<"\nstarting into fileReadDraw.C**************\n"<<endl;
  Bool_t asymDiffPlot=1,expAsymPlot=0,yieldPlot=1,asymPlot=0;
  Bool_t pUsed[nPlanes]={0};//!will this trick work to initialize all elements with zero?
  Bool_t debug=1,debug1=0;
  
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TGraphErrors *grDiffPWTL1_2[nPlanes];
  ifstream in1, in2;
  TLegend *leg;
  //TCanvas *cAsymComponent = new TCanvas("cAsymDiff",Form("Nr and Dr of Asym for run:%d",runnum),30,30,1000,600);
  TLine *myline = new TLine(0,0,70,0);
  ifstream fortranOutP1, expAsymPWTL1, expAsymPWTL2, expAsymComponents;//, theoAsym;
  ofstream newTheoFile;
  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetShadowColor(0);
  pvTxt1->SetBorderSize(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  TGaxis::SetMaxDigits(2);
  //gROOT->ForceStyle();

  leg = new TLegend(0.1,0.7,0.4,0.9);
  
  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  Float_t stripNum[nPlanes][nStrips],stripNum2[nPlanes][nStrips];
  Float_t asymDiff[nPlanes][nStrips],zero[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];
  Float_t stripAsymNr[nPlanes][nStrips];//,stripAsymNrEr[nStrips];
  Float_t accumB1L0[nPlanes][nStrips],accumB1L0Er[nPlanes][nStrips];
 
  for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t s=startStrip; s<endStrip; s++) {
      zero[p][s]=0;
      stripAsymDr[p][s]=0.0,stripAsymDrEr[p][s]=0.0;
      stripAsymNr[p][s]=0.0;//,stripAsymNrEr[p][s]=0.0;
      accumB1L0[p][s]=0.0,accumB1L0Er[p][s]=0.0;
    }
  }

  for(Int_t p =startPlane; p <endPlane; p++) {
    pUsed[p]=kTRUE;
  }
  if(debug) cout<<"planes used: "<<pUsed[0]<<"\t"<<pUsed[1]<<"\t"<<pUsed[2]<<"\t"<<pUsed[3]<<endl;

  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymComponents.open(Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymComponents.is_open()) {
      cout<<"Reading the yield and difference from PWTL1 for plane "<<p+1<<endl;
      if(debug) cout<<";stripNum\tstripAsymDr\tstripAsymDrEr\tstripAsymNr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
  	if (maskedStrips(p,s)) continue;
  	expAsymComponents>>stripNum[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>stripAsymNr[p][s];
      }
      expAsymComponents.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  if (yieldPlot) {
    TCanvas *cYield = new TCanvas("cYield",Form("Yield for run:%d",runnum),30,100,900,600);
    TGraphErrors *grAsymDr[nPlanes],*grB1L0[nPlanes];
    TLegend *legYield;
    legYield = new TLegend(0.1,0.7,0.4,0.9);

    cYield->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cYield->GetPad(p+1)->SetGridx(1);
      cYield->cd(p+1);
      grAsymDr[p] = new TGraphErrors(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grAsymDr[p]->SetMarkerStyle(kFullCircle);
      grAsymDr[p]->SetMarkerSize(1);
      grAsymDr[p]->SetLineColor(kGreen);
      grAsymDr[p]->SetMarkerColor(kGreen);
      grAsymDr[p]->SetFillColor(kGreen);
      grAsymDr[p]->SetTitle(Form("Plane %d background corrected yield(Hz/uA)",p+1));
      grAsymDr[p]->GetXaxis()->SetTitle("strip number");
      grAsymDr[p]->GetYaxis()->SetTitle("charge normalized yield (Hz/uA)");
      //grAsymDr[p]->GetYaxis()->SetTitleOffset(1.3);
      grAsymDr[p]->GetXaxis()->SetLimits(1,65); 
      grAsymDr[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grAsymDr[p]->Draw("A B");
      //TAxis *xaxis1= grAsymDr[p]->GetXaxis();
      //xaxis1->SetLimits(1,65);

      grB1L0[p] = new TGraphErrors(Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grB1L0[p]->SetLineColor(kBlue);
      grB1L0[p]->SetMarkerStyle(kFullSquare);
      grB1L0[p]->SetMarkerSize(1);
      grB1L0[p]->SetMarkerColor(kBlue);
      grB1L0[p]->SetFillColor(kBlue);
      grB1L0[p]->SetTitle("background(Hz/uA)");
      grB1L0[p]->Draw("B");
      //grB1L0[p]->GetXaxis()->SetLimits(1,65); 

      legYield->AddEntry(grAsymDr[p],"background corrected yield(Hz/uA)","lp");
      legYield->AddEntry(grB1L0[p],"background(Hz/uA)","lp");
      legYield->SetFillColor(0);
      legYield->Draw();
    }
    gPad->Update();
    cYield->Update();
    cYield->SaveAs(Form("%s/%s/%syieldAllPlanes.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(asymDiffPlot) {
    Float_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
    for(Int_t p =startPlane; p <endPlane; p++) {
      expAsymPWTL1.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      expAsymPWTL2.open(Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));//only plane1 is used for v2 data
      if(expAsymPWTL1.is_open() && expAsymPWTL2.is_open()) {
	cout<<"Reading the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
	cout<<"Reading the expAsym corresponding to PWTL2 for Plane "<<p+1<<endl;
	if(debug) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsym_v2\t"<<"asymDiff"<<endl;
	for(Int_t s =startStrip ; s < endStrip; s++) {
	  if (maskedStrips(p,s)) continue;
	  expAsymPWTL1>>stripNum[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	  expAsymPWTL2>>stripNum[p][s]>>stripAsym_v2[p][s]>>stripAsymEr_v2[p][s];
	  asymDiff[p][s] = (stripAsym[p][s]- stripAsym_v2[p][s]);
	  if(debug) cout<<stripNum[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsym_v2[p][s]<<"\t"<<asymDiff[p][s]<<endl;
	}
	expAsymPWTL1.close();
	expAsymPWTL2.close();
      }
      else cout<<"did not find one of the expAsym files eg:"<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
    }

    TCanvas *cDiff = new TCanvas("cDiff",Form("expAsym Diff for run:%d",runnum),30,30,1000,600);
    cDiff->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cDiff->cd(p+1);
      grDiffPWTL1_2[p] = new TGraphErrors(endStrip,stripNum[p],asymDiff[p],zero[p],stripAsymEr[p]);//!the error needs correction
      grDiffPWTL1_2[p]->SetMarkerStyle(kOpenSquare);
      grDiffPWTL1_2[p]->SetMarkerSize(0.5);
      grDiffPWTL1_2[p]->GetXaxis()->SetTitle("strip number");
      grDiffPWTL1_2[p]->GetYaxis()->SetTitle("expAsym difference (PWTL1-PWTL2)");
      grDiffPWTL1_2[p]->SetMaximum(0.005);
      grDiffPWTL1_2[p]->SetMinimum(-0.005);
      grDiffPWTL1_2[p]->SetTitle("Diff in exp-asymmetry due to different trigger width");
      grDiffPWTL1_2[p]->Draw("AP");
    }
    cDiff->Update();
    cDiff->SaveAs(Form("%s/%s/%sdiffexpAsym.png",pPath,webDirectory,filePrefix.Data()));
  }

  if (expAsymPlot) {
    TCanvas *c1 = new TCanvas("c1",Form("edet Asymmetry run:%d",runnum),10,10,1000,600);
    TGraphErrors *grCpp,*grFort;
    c1->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      grCpp = new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grCpp->GetXaxis()->SetTitle("strip number");
      grCpp->GetYaxis()->SetTitle("asymmetry");
      grCpp->SetTitle("exp asym");
      grCpp->SetMaximum(0.05);
      grCpp->SetMinimum(-0.05);
      grCpp->SetMarkerStyle(kFullCircle);
      grCpp->SetLineColor(kRed);
      grCpp->SetMarkerColor(kRed);
      grCpp->SetFillColor(0);
      grCpp->Draw("AP");
    }

    if(v2processed) {
      TGraphErrors *grCpp_v2;
      grCpp_v2 = new TGraphErrors(Form("%s/%s/%sexpAsymP1_v2.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg %lg");
      grCpp_v2->SetMarkerStyle(kFullCircle);
      grCpp_v2->SetLineColor(kGreen);
      grCpp_v2->SetMarkerColor(kGreen);
      grCpp_v2->SetFillColor(0);
      grCpp_v2->Draw("P");
    }
    myline->SetLineStyle(1);
    myline->Draw();

    fortranOutP1.open(Form("%d-plane-1-nc.output",runnum));
    if(fortranOutP1.is_open()) {
      cout<<"found runlet based raw asymmetry file"<<endl;
      grFort = new TGraphErrors(Form("%d-plane-1.output",runnum), "%lg %lg %lg");
      grFort->SetMarkerColor(4);
      grFort->SetMarkerStyle(24);
      grFort->SetFillColor(0);
      grFort->SetLineColor(4);
      grFort->Draw("P");
      // leg->AddEntry(grFort,"runlet based eDet Asymmetry","lpf");
      fortranOutP1.close();
    }
    else cout<<"corresponding fortran file for run "<<runnum<<" doesn't exist"<<endl;
  
    c1->SaveAs(Form("%s/%s/%sexpAsym.png",pPath,webDirectory,filePrefix.Data()));
  }

  if (asymPlot) {
    TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",50,50,1000,600);
    TGraphErrors *grTheoryAsym[nPlanes], *grAsymPlane[nPlanes];
    cAsym->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) {
      //for (Int_t p =0; p <1; p++) {
      cAsym->cd(p+1);
      grAsymPlane[p] = new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
      grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
      grAsymPlane[p]->SetTitle(Form("experimental asymmetry"));//Run: %d, Plane %d",runnum,p+1));
      grAsymPlane[p]->SetMarkerStyle(kFullCircle);
      grAsymPlane[p]->SetLineColor(kRed);
      grAsymPlane[p]->SetFillColor(0);
      grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
      //grAsymPlane[p]->Fit("fn2","R");
      grAsymPlane[p]->SetMaximum(0.042);
      grAsymPlane[p]->SetMinimum(-0.042);
      grAsymPlane[p]->Draw("AP");
      myline->Draw();
      
      grTheoryAsym[p] = new TGraphErrors(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge[0]), "%lg %lg");
      //grTheoryAsym[p] = new TGraphErrors(Form("%s/%s/%smodTheoryFileP1.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg");     
      //grTheoryAsym[p] = new TGraphErrors(dummyStrip,stripNum[p],theoryAsym[p],zero[p],zero[p]);
      grTheoryAsym[p]->SetLineColor(kBlue);
      grTheoryAsym[p]->SetLineWidth(3);
      grTheoryAsym[p]->SetFillColor(0);
      grTheoryAsym[p]->SetTitle("theoretical asymmetry");
      grTheoryAsym[p]->Draw("L");    
      cAsym->Update();
      leg->AddEntry(grAsymPlane[p],"experimental asymmetry","lpf");
      leg->AddEntry(grTheoryAsym[p],"theoretical asymmetry","lpf");
      //      leg->AddEntry(myline,"zero line","l");
      leg->SetFillColor(0);
      leg->Draw();
    } 

    cAsym->Update();
    cAsym->SaveAs(Form("%s/%s/%sexpTheoAsym.png",pPath,webDirectory,filePrefix.Data()));
  }
  return runnum;
}
