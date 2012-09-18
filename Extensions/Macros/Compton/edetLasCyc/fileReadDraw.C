#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Int_t fileReadDraw(Int_t runnum) 
{
  cout<<"\nstarting into fileReadDraw.C**************\n"<<endl;
  Bool_t asymDiffPlot=0;//plots the difference in asymmetry as obtained from PWTL1 - PWTL2
  Bool_t yieldPlot=0;
  Bool_t asymPlot=0;//plots expAsym against theoretical asym, not needed when asymFit.C is plotting it
  Bool_t asymComponents=0;
  Bool_t scalerPlot=0;
  Bool_t lasWisePlotAc=0;//plot quantities against on laser-cycle 
  Bool_t lasWisePlotSc=0;//plot quantities against on laser-cycle 
  Bool_t lasWisePlotBcm=1;//plot quantities against on laser-cycle 
  Bool_t lasWisePlotLasPow=1;
  Bool_t vsBeam=1;//plots quantities againt beam current variations

  Bool_t pUsed[nPlanes]={0};//!will this trick work to initialize all elements with zero?
  Bool_t debug=1,debug1=0;
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TLine *myline = new TLine(0,0,70,0);
  ifstream in1, in2;
  ifstream fortranOutP1,expAsymPWTL1,expAsymPWTL2,expAsymComponents,scalerRates,lasCycAccum,lasCycScaler;
  ofstream newTheoFile;
  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetShadowColor(0);
  pvTxt1->SetBorderSize(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  //TGaxis::SetMaxDigits(2);
  //gROOT->ForceStyle();

  std::vector<std::vector <Float_t> > stripNum,stripAsymNr,stripAsymDr,stripAsymDrEr;
  std::vector<std::vector <Float_t> > stripNum2,scalerB1L1,scalerB1L0;

  Float_t nCycle[100];//arbitrarily set to 100 which certainly will be larger than nLasCycles
  Float_t qNormAccumB1H0L0[nPlanes][nStrips][100],qNormAccumB1H0L1[nPlanes][nStrips][100],qNormAccumB1H1L0[nPlanes][nStrips][100],qNormAccumB1H1L1[nPlanes][nStrips][100];
  Float_t qNormScalerB1H0L0[nPlanes][nStrips][100],qNormScalerB1H0L1[nPlanes][nStrips][100],qNormScalerB1H1L0[nPlanes][nStrips][100],qNormScalerB1H1L1[nPlanes][nStrips][100];
  Float_t qNormAccumB1L0[nPlanes][nStrips][100],qNormScalerB1L0[nPlanes][nStrips][100];

  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  Float_t asymDiff[nPlanes][nStrips],zero[nPlanes][nStrips];
  Float_t accumB1L0[nPlanes][nStrips],accumB1L0Er[nPlanes][nStrips];

  for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t s=startStrip; s<endStrip; s++) {
      zero[p][s]=0;
      accumB1L0[p][s]=0.0,accumB1L0Er[p][s]=0.0;
    }
  }

  for(Int_t p =startPlane; p <endPlane; p++) {
    pUsed[p]=kTRUE;
  }
  if(debug) cout<<"planes used: "<<pUsed[0]<<"\t"<<pUsed[1]<<"\t"<<pUsed[2]<<"\t"<<pUsed[3]<<endl;

  if(scalerPlot) {
    TCanvas *cNoise = new TCanvas("cNoise",Form("scalers for run:%d",runnum),30,30,1000,420*endPlane);
    TGraph *grScalerLasOn[nPlanes],*grScalerLasOff[nPlanes];
    TLegend *legScaler[nPlanes];

    cNoise->Divide(startPlane+1,endPlane);

    for(Int_t p =startPlane; p <endPlane; p++) {
      scalerRates.open(Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      if(scalerRates.is_open()) {
	if(p>=(Int_t)stripNum2.size()) {
	  stripNum2.resize(p+1),scalerB1L1.resize(p+1),scalerB1L0.resize(p+1);
	}
	cout<<"Reading the Scaler rate file for plane "<<p+1<<endl;
	if(debug) cout<<";\tstripNum\tlasOnScalerCounts\tlasOffScalerCounts"<<endl;
	while(scalerRates.good()) {
	  stripNum2[p].push_back(0.0),scalerB1L1[p].push_back(0.0),scalerB1L0[p].push_back(0.0);
	  Int_t s=stripNum2[p].size() - 1;
	  scalerRates>>stripNum2[p][s]>>scalerB1L1[p][s]>>scalerB1L0[p][s];
	  if(debug) printf("[%d][%d]:%2.0f\t%f\t%f\n",p+1,s+1,stripNum2[p][s],scalerB1L1[p][s],scalerB1L0[p][s]);
	}
	scalerRates.close();
      }
      else cout<<"did not find "<<Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;

      cNoise->GetPad(p+1)->SetGridx(1);
      cNoise->cd(p+1);
      Int_t newSize=stripNum2[p].size();
      //if(debug) cout<<"for plane "<<p+1<<" the no.of active strips is "<<newSize<<endl;
      grScalerLasOn[p] = new TGraph(newSize,stripNum2[p].data(),scalerB1L1[p].data());
      grScalerLasOn[p]->SetMarkerStyle(kFullCircle);
      grScalerLasOn[p]->SetMarkerSize(1);
      grScalerLasOn[p]->SetLineColor(kGreen);
      grScalerLasOn[p]->SetMarkerColor(kGreen);
      grScalerLasOn[p]->SetFillColor(kGreen);
      grScalerLasOn[p]->GetXaxis()->SetTitle("strip number");
      grScalerLasOn[p]->GetYaxis()->SetTitle("time normalized scaler(Hz)");

      grScalerLasOn[p]->GetXaxis()->SetLimits(1,65); 
      grScalerLasOn[p]->GetXaxis()->SetNdivisions(416, kFALSE);
      grScalerLasOn[p]->Draw("A P");

      grScalerLasOff[p]= new TGraph(newSize,stripNum2[p].data(),scalerB1L0[p].data());
      grScalerLasOff[p]->SetMarkerStyle(kOpenCircle);
      grScalerLasOff[p]->SetMarkerColor(kBlue);
      grScalerLasOff[p]->Draw("P");
    
      legScaler[p] = new TLegend(0.1,0.7,0.35,0.9);//x1,y1,x2,y2
      legScaler[p]->AddEntry(grScalerLasOn[p],"laser on scaler(Hz)","p");
      legScaler[p]->AddEntry(grScalerLasOff[p],"laser off scaler(Hz)","p");
      legScaler[p]->SetFillColor(0);
      legScaler[p]->Draw();
    }
    gPad->Update();
    cNoise->Update();
    cNoise->SaveAs(Form("%s/%s/%stNormScaler.png",pPath,webDirectory,filePrefix.Data()));
  }


  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymComponents.open(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymComponents.is_open()) {
      if(p<=(Int_t)stripNum.size()) {
	stripNum.resize(p+1),stripAsymDr.resize(p+1),stripAsymDrEr.resize(p+1),stripAsymNr.resize(p+1);
      }
      cout<<"Reading the yield and difference from PWTL1 for plane "<<p+1<<endl;
      if(debug1) cout<<";\tstripNum\tstripAsymDr\tstripAsymDrEr\tstripAsymNr\n"<<endl;
      while(expAsymComponents.good()) {
	stripNum[p].push_back(0.0),stripAsymDr[p].push_back(0.0),stripAsymDrEr[p].push_back(0.0),stripAsymNr[p].push_back(0.0);
	Int_t s=stripNum[p].size() - 1;
	expAsymComponents>>stripNum[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>stripAsymNr[p][s];
	if(debug1) printf("[%d][%d]:%2.0f\t%f\t%f\t%f\n",p+1,s+1,stripNum[p][s],stripAsymDr[p][s],stripAsymDrEr[p][s],stripAsymNr[p][s]);
      }
      expAsymComponents.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
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
      grAsymNr[p]= new TGraphErrors(newSize,stripNum[p].data(),stripAsymNr[p].data(),zero[p],stripAsymDrEr[p].data());
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
    TCanvas *cYield = new TCanvas("cYield",Form("Yield for run:%d",runnum),70,70,1000,420*endPlane);
    TGraphErrors *grAsymDr[nPlanes],*grB1L0[nPlanes];
    TLegend *legYield[nPlanes];

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

      legYield[p] = new TLegend(0.1,0.7,0.4,0.9);
      legYield[p]->AddEntry(grAsymDr[p],"background corrected yield(Hz/uA)","lp");
      legYield[p]->AddEntry(grB1L0[p],"background(Hz/uA)","lp");
      legYield[p]->SetFillColor(0);
      legYield[p]->Draw();
    }
    gPad->Update();
    cYield->Update();
    cYield->SaveAs(Form("%s/%s/%syieldAllPlanes.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(asymDiffPlot) {
    Double_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
    //Int_t dummyStrip[nPlanes];
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

    TCanvas *cDiff = new TCanvas("cDiff",Form("expAsym Diff for run:%d",runnum),1,1,1000,420*endPlane);
    TGraphErrors *grDiffPWTL1_2[nPlanes];
    cDiff->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cDiff->cd(p+1);
      cDiff->GetPad(p+1)->SetGridx(1);
      //!!following line needs to be fixed in wake of the modified variable definition
      grDiffPWTL1_2[p] = new TGraphErrors(endStrip,stripNum[p].data(),asymDiff[p],zero[p],stripAsymEr[p]);
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

  if (asymPlot) {
    TLegend *leg;
    leg = new TLegend(0.1,0.7,0.4,0.9);
    TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",50,50,1000,420*endPlane);
    TGraphErrors *grTheoryAsym[nPlanes], *grAsymPlane[nPlanes],*grFort;
    cAsym->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) {
      cAsym->cd(p+1);
      grAsymPlane[p] = new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
      grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
      grAsymPlane[p]->SetTitle(Form("experimental asymmetry"));
      grAsymPlane[p]->SetMarkerStyle(kFullCircle);
      grAsymPlane[p]->SetLineColor(kRed);
      grAsymPlane[p]->SetFillColor(0);
      grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
      //grAsymPlane[p]->Fit("fn2","R");
      grAsymPlane[p]->SetMaximum(0.042);
      grAsymPlane[p]->SetMinimum(-0.042);
      grAsymPlane[p]->Draw("AP");

      myline->SetLineStyle(1);
      myline->Draw();
      
      if(v2processed) {
	TGraphErrors *grCpp_v2[nPlanes];
	grCpp_v2[p] = new TGraphErrors(Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
	grCpp_v2[p]->SetMarkerStyle(kFullCircle);
	grCpp_v2[p]->SetLineColor(kGreen);
	grCpp_v2[p]->SetMarkerColor(kGreen);
	grCpp_v2[p]->SetFillColor(0);
	grCpp_v2[p]->Draw("P");
      }

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
      //leg->AddEntry(myline,"zero line","l");
      leg->SetFillColor(0);
 
      fortranOutP1.open(Form("%d-plane-1-nc.output",runnum));
      if(fortranOutP1.is_open()) {
	cout<<"found runlet based raw asymmetry file"<<endl;
	grFort = new TGraphErrors(Form("%d-plane-1.output",runnum), "%lg %lg %lg");
	grFort->SetMarkerColor(4);
	grFort->SetMarkerStyle(24);
	grFort->SetFillColor(0);
	grFort->SetLineColor(4);
	grFort->Draw("P");
	leg->AddEntry(grFort,"runlet based eDet Asymmetry","lpf");
	fortranOutP1.close();
      } else cout<<"corresponding fortran file for run "<<runnum<<" doesn't exist"<<endl;
      leg->Draw();
    }

    cAsym->Update();
    cAsym->SaveAs(Form("%s/%s/%sexpTheoAsym.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(lasWisePlotAc) {
    TGraph *lasCycPlot[endStrip];
    TCanvas *cAccumLC1 = new TCanvas("cAccumLC1",Form("accum counts per laser cycle for strips 01-16"),0,0,1200,1200);
    TCanvas *cAccumLC2 = new TCanvas("cAccumLC2",Form("accum counts per laser cycle for strips 17-32"),20,10,1200,1200);
    TCanvas *cAccumLC3 = new TCanvas("cAccumLC3",Form("accum counts per laser cycle for strips 33-48"),40,20,1200,1200);
    TCanvas *cAccumLC4 = new TCanvas("cAccumLC4",Form("accum counts per laser cycle for strips 49-64"),60,30,1200,1200);

    cAccumLC1->Divide(4,4);
    cAccumLC2->Divide(4,4);
    cAccumLC3->Divide(4,4);
    cAccumLC4->Divide(4,4);

    for(Int_t p =startPlane; p <endPlane; p++) {
      for(Int_t s=startStrip;s<endStrip;s++) { 
	if (maskedStrips(p,s)) continue; //currently only for plane 1	
 	lasCycAccum.open(Form("%s/%s/%slasCycAccumP%dS%d.txt",pPath,webDirectory,filePrefix.Data(),p+1,s+1));
	Int_t nLasCycles=0;
	while(lasCycAccum.good()) {
	  lasCycAccum>>nCycle[nLasCycles]>>qNormAccumB1H0L0[p][s][nLasCycles]>>qNormAccumB1H0L1[p][s][nLasCycles]>>qNormAccumB1H1L0[p][s][nLasCycles]>>qNormAccumB1H1L1[p][s][nLasCycles];

	  qNormAccumB1L0[p][s][nLasCycles] = (qNormAccumB1H0L0[p][s][nLasCycles]+qNormAccumB1H1L0[p][s][nLasCycles])/2;
	  if(debug1) printf("%f\t%f\t%f\t%f\n",nCycle[nLasCycles],qNormAccumB1H0L0[p][s][nLasCycles],qNormAccumB1H1L0[p][s][nLasCycles],qNormAccumB1L0[p][s][nLasCycles]);
	  nLasCycles=nLasCycles+1;
	}
	lasCycAccum.close();

	lasCycPlot[s] = new TGraph(nLasCycles,nCycle,qNormAccumB1L0[p][s]);
	if(s>=0 && s<16)  cAccumLC1->cd(s+1);
	if(s>=16 && s<32) cAccumLC2->cd(s-16+1);
	if(s>=32 && s<48) cAccumLC3->cd(s-32+1);
	if(s>=48 && s<64) cAccumLC4->cd(s-48+1);
	lasCycPlot[s]->SetTitle(Form("Strip %d",s+1));
	lasCycPlot[s]->SetMarkerStyle(kFullCircle);
	lasCycPlot[s]->SetMarkerColor(kRed);
	lasCycPlot[s]->SetLineColor(kRed);
	lasCycPlot[s]->SetLineWidth(2);
	lasCycPlot[s]->GetXaxis()->SetTitle("laser Cycle (#)");
	lasCycPlot[s]->GetYaxis()->SetTitle("charge normalized accum counts (Hz/uA)");
	lasCycPlot[s]->GetYaxis()->SetTitleOffset(1.2);
	lasCycPlot[s]->GetYaxis()->SetLabelSize(0.03);
	lasCycPlot[s]->Draw("AP");
      }
      cAccumLC1->Update();
      cAccumLC2->Update();
      cAccumLC3->Update();
      cAccumLC4->Update();
      cAccumLC1->SaveAs(Form("%s/%s/%slasCycAccum1.png",pPath,webDirectory,filePrefix.Data()));
      cAccumLC2->SaveAs(Form("%s/%s/%slasCycAccum2.png",pPath,webDirectory,filePrefix.Data()));
      cAccumLC3->SaveAs(Form("%s/%s/%slasCycAccum3.png",pPath,webDirectory,filePrefix.Data()));
      cAccumLC4->SaveAs(Form("%s/%s/%slasCycAccum4.png",pPath,webDirectory,filePrefix.Data()));
    }
  }

  if(lasWisePlotBcm) {
    TGraph *lasCycBCML1,*lasCycBCML0,*lasCycBCM;
    TCanvas *cLasCycBCM = new TCanvas("cLasCycBCM","Beam stability laser cycle",50,50,800,900);
    ifstream lasWiseBCM;
    Float_t bcmH0L0[200],bcmH0L1[200],bcmH1L0[200],bcmH1L1[200];//!this implicitly puts a limitation on the no.of laser cycles it can handle
    Float_t bcmL0[200],bcmL1[200],bcm[200];
    lasWiseBCM.open(Form("%s/%s/%slasCycBcmAvg.txt",pPath,webDirectory,filePrefix.Data()));
    Int_t nLasCycles=0;
    while(lasWiseBCM.good()) {
      lasWiseBCM>>nCycle[nLasCycles]>>bcmH0L0[nLasCycles]>>bcmH0L1[nLasCycles]>>bcmH1L0[nLasCycles]>>bcmH1L1[nLasCycles];
      bcmL0[nLasCycles] = (bcmH0L0[nLasCycles]+bcmH1L0[nLasCycles])/2;//!ideally not correct, this addition should be charge weigthed
      bcmL1[nLasCycles] = (bcmH0L1[nLasCycles]+bcmH1L1[nLasCycles])/2;//!ideally not correct, this addition should be charge weigthed
      bcm[nLasCycles] = (bcmL0[nLasCycles]+bcmL1[nLasCycles])/2;//!ideally not correct, this addition should be charge weigthed
      nLasCycles=nLasCycles+1;
    }
    lasWiseBCM.close();
    cLasCycBCM->Divide(1,2);
    cLasCycBCM->cd(1);
    lasCycBCML1= new TGraph(nLasCycles,nCycle,bcmL1);
    lasCycBCML1->SetTitle(Form(" BCM for run %d",runnum));
    lasCycBCML1->SetMarkerStyle(kFullCircle);
    lasCycBCML1->SetMarkerColor(kGreen);
    lasCycBCML1->SetLineWidth(2);
    lasCycBCML1->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycBCML1->GetYaxis()->SetTitle("beam (uA)");
    lasCycBCML1->GetYaxis()->SetLabelSize(0.03);
    lasCycBCML1->Draw("AP");

    lasCycBCML0= new TGraph(nLasCycles,nCycle,bcmL0);
    //lasCycBCML0->SetTitle(Form("las-off BCM for run %d",runnum));
    lasCycBCML0->SetMarkerStyle(kOpenCircle);
    lasCycBCML0->SetMarkerColor(kBlue);
    lasCycBCML0->SetLineWidth(2);
    lasCycBCML0->GetYaxis()->SetLabelSize(0.03);
    lasCycBCML0->Draw("P");

    cLasCycBCM->cd(2);
    lasCycBCM= new TGraph(nLasCycles,nCycle,bcm);
    lasCycBCM->SetTitle(Form("BCM for run %d",runnum));
    lasCycBCM->SetMarkerStyle(kFullCircle);
    lasCycBCM->SetMarkerColor(kRed  );
    lasCycBCM->SetLineWidth(2);
    lasCycBCM->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycBCM->GetYaxis()->SetTitle("beam (uA)");
    lasCycBCM->GetYaxis()->SetLabelSize(0.03);
    lasCycBCM->Draw("AP");

    cLasCycBCM->Update();
    cLasCycBCM->SaveAs(Form("%s/%s/%slasCycBCM.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(lasWisePlotLasPow) {
    TGraph *lasCycLasPowL0,*lasCycLasPowL1;
    TCanvas *cLasCycPow = new TCanvas("cLasCycPow","Laser Power stability",0,0,600,400);
    ifstream lasWisePow;
    Float_t lasPowB1H0L0[200],lasPowB1H1L0[200],lasPowB1L0[200];//!this implicitly puts a limitation on the no.of laser cycles it can handle
    Float_t lasPowB1H0L1[200],lasPowB1H1L1[200],lasPowB1L1[200];//!this implicitly puts a limitation on the no.of laser cycles it can handle
    lasWisePow.open(Form("%s/%s/%slasCycAvgLasPow.txt",pPath,webDirectory,filePrefix.Data()));
    Int_t nLasCycles=0;
    while(lasWisePow.good()) {
      lasWisePow>>nCycle[nLasCycles]>>lasPowB1H0L0[nLasCycles]>>lasPowB1H0L1[nLasCycles]>>lasPowB1H1L0[nLasCycles]>>lasPowB1H1L1[nLasCycles];
      lasPowB1L0[nLasCycles] = (lasPowB1H0L0[nLasCycles] + lasPowB1H1L0[nLasCycles])/2;
      lasPowB1L1[nLasCycles] = (lasPowB1H0L1[nLasCycles] + lasPowB1H1L1[nLasCycles])/2;
      nLasCycles=nLasCycles+1;
    }
    lasWisePow.close();
    cLasCycPow->Divide(1,2);
    cLasCycPow->cd(2);
    lasCycLasPowL0= new TGraph(nLasCycles,nCycle,lasPowB1L0);
    lasCycLasPowL0->SetTitle(Form("Laser Power for run %d",runnum));
    lasCycLasPowL0->SetMarkerStyle(kOpenCircle);
    lasCycLasPowL0->SetMarkerColor(kGreen);
    lasCycLasPowL0->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycLasPowL0->GetYaxis()->SetTitle("laser Power (a.u)");
    lasCycLasPowL0->GetYaxis()->SetLabelSize(0.03);
    lasCycLasPowL0->Draw("AP");

    cLasCycPow->cd(1);
    lasCycLasPowL1= new TGraph(nLasCycles,nCycle,lasPowB1L1);
    lasCycLasPowL1->SetMarkerStyle(kFullCircle);
    lasCycLasPowL1->SetMarkerColor(kGreen);
    lasCycLasPowL1->GetXaxis()->SetTitle("laser Cycle (#)");
    lasCycLasPowL1->GetYaxis()->SetTitle("laser Power (a.u)");
    lasCycLasPowL1->GetYaxis()->SetLabelSize(0.03);
    lasCycLasPowL1->Draw("AP");

    cLasCycPow->Update();
    cLasCycPow->SaveAs(Form("%s/%s/%slasCycLasPow.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(lasWisePlotSc) {
    TGraph *lasCycPlotSc[endStrip];
    TCanvas *cScalerLC1 = new TCanvas("cScalerLC1",Form("scaler counts per laser cycle for strips 01-16"),0,0,1200,1200);
    TCanvas *cScalerLC2 = new TCanvas("cScalerLC2",Form("scaler counts per laser cycle for strips 17-32"),20,10,1200,1200);
    TCanvas *cScalerLC3 = new TCanvas("cScalerLC3",Form("scaler counts per laser cycle for strips 33-48"),40,20,1200,1200);
    TCanvas *cScalerLC4 = new TCanvas("cScalerLC4",Form("scaler counts per laser cycle for strips 49-64"),60,30,1200,1200);
    
    cScalerLC1->Divide(4,4);
    cScalerLC2->Divide(4,4);
    cScalerLC3->Divide(4,4);
    cScalerLC4->Divide(4,4);

    for(Int_t p =startPlane; p <endPlane; p++) {
      for(Int_t s=startStrip;s<endStrip;s++) { 
	if (maskedStrips(p,s)) continue; //currently only for plane 1	
 	lasCycScaler.open(Form("%s/%s/%slasCycScalerP%dS%d.txt",pPath,webDirectory,filePrefix.Data(),p+1,s+1));
	Int_t nLasCycles=0;
	while(lasCycScaler.good()) {
	  lasCycScaler>>nCycle[nLasCycles]>>qNormScalerB1H0L0[p][s][nLasCycles]>>qNormScalerB1H0L1[p][s][nLasCycles]>>qNormScalerB1H1L0[p][s][nLasCycles]>>qNormScalerB1H1L1[p][s][nLasCycles];
	  qNormScalerB1L0[p][s][nLasCycles] = (qNormScalerB1H0L0[p][s][nLasCycles]+qNormScalerB1H1L0[p][s][nLasCycles])/2;//!ideally not correct, this addition should be charge weigthed
	  nLasCycles=nLasCycles+1;
	}
	lasCycScaler.close();
	lasCycPlotSc[s]= new TGraph(nLasCycles,nCycle,qNormScalerB1L0[p][s]);

	if(s>= 0 && s<16) cScalerLC1->cd(s+1);
	if(s>=16 && s<32) cScalerLC2->cd(s-16+1);
	if(s>=32 && s<48) cScalerLC3->cd(s-32+1);
	if(s>=48 && s<64) cScalerLC4->cd(s-48+1);

	lasCycPlotSc[s]->SetTitle(Form("Strip %d",s+1));
	lasCycPlotSc[s]->SetMarkerStyle(kFullCircle);
	lasCycPlotSc[s]->SetMarkerColor(kRed);
	lasCycPlotSc[s]->SetLineColor(kRed);
	lasCycPlotSc[s]->SetLineWidth(2);
	lasCycPlotSc[s]->GetXaxis()->SetTitle("laser Cycle (#)");
	lasCycPlotSc[s]->GetYaxis()->SetTitle("charge normalized accum counts (Hz/uA)");
	lasCycPlotSc[s]->GetYaxis()->SetTitleOffset(1.2);
	lasCycPlotSc[s]->GetYaxis()->SetLabelSize(0.03);
	lasCycPlotSc[s]->Draw("AP");
      }
      cScalerLC1->Update();
      cScalerLC2->Update();
      cScalerLC3->Update();
      cScalerLC4->Update();
      cScalerLC1->SaveAs(Form("%s/%s/%slasCycScaler1.png",pPath,webDirectory,filePrefix.Data()));
      cScalerLC2->SaveAs(Form("%s/%s/%slasCycScaler2.png",pPath,webDirectory,filePrefix.Data()));
      cScalerLC3->SaveAs(Form("%s/%s/%slasCycScaler3.png",pPath,webDirectory,filePrefix.Data()));
      cScalerLC4->SaveAs(Form("%s/%s/%slasCycScaler4.png",pPath,webDirectory,filePrefix.Data()));
    }
  }

  if(vsBeam) {
    gStyle->SetOptFit(1);
    TGraphErrors *ScalerVsBeam[endStrip]; 
    TCanvas *cScalVsBeam1 = new TCanvas("cScalVsBeam1",Form("scaler counts per laser cycle for strips 01-16"),0,0,1200,1000);
    TCanvas *cScalVsBeam2 = new TCanvas("cScalVsBeam2",Form("scaler counts per laser cycle for strips 17-32"),20,10,1200,1000);
    TCanvas *cScalVsBeam3 = new TCanvas("cScalVsBeam3",Form("scaler counts per laser cycle for strips 33-48"),40,20,1200,1000);
    TCanvas *cScalVsBeam4 = new TCanvas("cScalVsBeam4",Form("scaler counts per laser cycle for strips 49-64"),60,30,1200,1000);
    
    cScalVsBeam1->Divide(4,4);
    cScalVsBeam2->Divide(4,4);
    cScalVsBeam3->Divide(4,4);
    cScalVsBeam4->Divide(4,4);

    ifstream infilelasOffBkgd,scalerRate;
    const Int_t numb=6;
    Double_t beamI[numb]={30,50,60,100,150,180};//!assumed 1-to-1 matching between runlist and beamI array
    Int_t runlist[numb]={23142,23148,23154,23151,23152,23168};
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

    Double_t stripNumber[nStrips][numb],stripBkgdYield[nStrips][numb],stripBkgdYieldEr[nStrips][numb],zero[numb];

    for(Int_t n=0; n<numb; n++) {        
      TString filePrefix= Form("run_%d/edetLasCyc_%d_",runlist[n],runlist[n]);   
      for(Int_t p =startPlane; p <endPlane; p++) {
	infilelasOffBkgd.open(Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
	if(debug1)cout<<"opening file "<<Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
	if(infilelasOffBkgd.is_open()) {
	  Int_t strip=0;
	  while(infilelasOffBkgd.good()) {
	    infilelasOffBkgd>>stripNumber[strip][n]>>stripBkgdYield[strip][n]>>stripBkgdYieldEr[strip][n];
	    strip = strip +1;
	  }
	  infilelasOffBkgd.close();
	  if(debug1)cout<<"closing file "<<Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;     
	} else {
	  if(debug)cout<<"did not find "<<Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
	  return -1;
	}
      }
      zero[n] = 0.0;
    }
    
    TF1 *bkgdFit = new TF1("bkgdFit","[0] + [1]/x",25,105);//specifying current range to fit
    bkgdFit->SetParameters(-0.1,20);//,0.5);
    //bkgdFit->SetParLimits(0,0.2,2.0);///allowing the strip width to be either 20% or 200% of its real pitch    
    //bkgdFit->SetParLimits(1,-1.0,1.0);///allowing polarization to be -100% to +100%

    for(Int_t s=startStrip;s<endStrip;s++) {
      //!the masked strips are not taken care of properly here
      ScalerVsBeam[s] = new TGraphErrors(numb,beamI,stripBkgdYield[s],zero,stripBkgdYieldEr[s]);
	
      if(s>= 0 && s<16) cScalVsBeam1->cd(s+1);
      if(s>=16 && s<32) cScalVsBeam2->cd(s-16+1);
      if(s>=32 && s<48) cScalVsBeam3->cd(s-32+1);
      if(s>=48 && s<64) cScalVsBeam4->cd(s-48+1);
      
      ScalerVsBeam[s]->SetTitle(Form("Strip %d",s+1));
      ScalerVsBeam[s]->SetMarkerStyle(kFullCircle);
      ScalerVsBeam[s]->SetMarkerSize(1);
      ScalerVsBeam[s]->SetMarkerColor(kRed);
      ScalerVsBeam[s]->SetLineColor(kRed);
      ScalerVsBeam[s]->SetLineWidth(2);
      ScalerVsBeam[s]->GetXaxis()->SetTitle("beam current (uA)");
      ScalerVsBeam[s]->GetYaxis()->SetTitle("qNorm (bkgd) accum counts (Hz/uA)");
      ScalerVsBeam[s]->GetYaxis()->SetTitleOffset(1.2);
      ScalerVsBeam[s]->GetYaxis()->SetLabelSize(0.03);
      //cScalVsBeam1->GetPad(2)->SetGridx(1); 
      ScalerVsBeam[s]->Draw("APE");
      ScalerVsBeam[s]->Fit("bkgdFit","R");
    }
    
    gPad->Update();
    cScalVsBeam1->Update();
    cScalVsBeam2->Update();
    cScalVsBeam3->Update();
    cScalVsBeam4->Update();
    cScalVsBeam1->SaveAs(Form("%s/%s/%sScalVsBeamStr01_16.png",pPath,webDirectory,filePrefix.Data()));
    cScalVsBeam2->SaveAs(Form("%s/%s/%sScalVsBeamStr17_32.png",pPath,webDirectory,filePrefix.Data()));
    cScalVsBeam3->SaveAs(Form("%s/%s/%sScalVsBeamStr33_48.png",pPath,webDirectory,filePrefix.Data()));
    cScalVsBeam4->SaveAs(Form("%s/%s/%sScalVsBeamStr49_64.png",pPath,webDirectory,filePrefix.Data()));
  }
  return runnum;
}
