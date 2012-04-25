#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Int_t fileReadDraw(Int_t runnum) 
{
  cout<<"\nstarting into fileReadDraw.C**************\n"<<endl;
  Bool_t asymDiffPlot=0,expAsymPlot=0,polPlanesPlot=0,yieldPlot=1,asymPlot=0;
  Bool_t pUsed[nPlanes]={0};//!will this trick work to initialize all elements with zero?
  Bool_t debug=0,debug1=0;
  
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TGraphErrors *grPolPlane[nPlanes],*grDiffPWTL1_2[nPlanes];
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
  gROOT->ForceStyle();

  leg = new TLegend(0.1,0.7,0.4,0.9);

  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
  Float_t stripNum1[nPlanes][nStrips],stripNum2[nPlanes][nStrips];
  Float_t asymDiff[nPlanes][nStrips],zero[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];
  Float_t stripAsymNr[nPlanes][nStrips];//,stripAsymNrEr[nStrips];
  Float_t accumB1L0[nPlanes][nStrips],accumB1L0Er[nPlanes][nStrips];
 
  Double_t polarization[nPlanes], polEr[nPlanes];

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
    expAsymPWTL1.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymPWTL1.is_open()) {
      cout<<"Plotting the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) continue; //I think this is causing an offset
	expAsymPWTL1>>stripNum1[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	if(debug) cout<<stripNum1[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
      }
      expAsymPWTL1.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  //   for(Int_t p =startPlane; p <endPlane; p++) {
  //     expAsymComponents.open(Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
  //     if(expAsymComponents.is_open()) {
  //       cout<<"Plotting the yield from PWTL1 for plane "<<p<<endl;
  //       if(debug) cout<<"stripNum\tstripAsymNr\tstripAsymDr\tstripAsymDrEr\taccumB1L0\taccumB1L0Er"<<endl;
  //       for(Int_t s =startStrip ; s < endStrip; s++) {
  // 	if (maskedStrips(p,s)) continue;///I don't want to loop on plane here but I do need to skip the irrelevant strips
  // 	expAsymComponents>>stripNum3[p][s]>>stripAsymNr[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>accumB1L0[p][s]>>accumB1L0Er[p][s];
  // 	if(debug) cout<<stripNum3[p][s]<<"\t"<<stripAsymNr[p][s]<<"\t"<<stripAsymDr[p][s]<<"\t"<<stripAsymDrEr[p][s]<<"\t"<<accumB1L0[p][s]<<"\t"<<accumB1L0Er[p][s]<<endl;
  //       }
  //       expAsymComponents.close();
  //     }
  //     else cout<<"did not find "<<Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  //   }

  //   cAsymComponent->cd(1);
  //   grAsymNr = new TGraphErrors(endStrip,stripNum,stripAsymNr,zero,stripAsymDrEr);
  //   grAsymNr->SetMarkerStyle(kFullTriangleUp);
  //   grAsymNr->SetMarkerSize(0.5);
  //   grAsymNr->GetXaxis()->SetTitle("strip number");
  //   grAsymNr->GetYaxis()->SetTitle("laser on difference");
  //   grAsymNr->SetTitle("laser On difference");//Numerator of experimental asymmetry
  //   grAsymNr->Draw("AP");//!temporary
  //   cAsymComponent->SetGridx(1);

  if (yieldPlot) {
    TCanvas *cYield = new TCanvas("cYield",Form("Yield for run:%d",runnum),30,100,1000,600);
    TGraphErrors *grAsymDr[nPlanes],*grB1L0[nPlanes];

    cYield->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cYield->cd(p+1);
      //grAsymDr[p] = new TGraphErrors(endStrip,stripNum3[p],stripAsymDr[p],zero[p],stripAsymDrEr[p]);
      grAsymDr[p] = new TGraphErrors(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grAsymDr[p]->SetMarkerStyle(kFullCircle);
      grAsymDr[p]->SetMarkerSize(1);
      grAsymDr[p]->SetLineColor(kGreen);
      grAsymDr[p]->SetMarkerColor(kGreen);
      grAsymDr[p]->SetFillColor(0);
      grAsymDr[p]->SetTitle(Form("Plane %d background corrected yield(counts/uA)",p+1));
      grAsymDr[p]->GetXaxis()->SetTitle("strip number");
      grAsymDr[p]->GetYaxis()->SetTitle("charge normalized yield (counts/uA)");
      grAsymDr[p]->Draw("AP");
      TAxis *xaxis1= grAsymDr[p]->GetXaxis();
      xaxis1->SetLimits(0,65);

      //grB1L0[p] = new TGraphErrors(endStrip,stripNum3[p],accumB1L0[p],zero[p],accumB1L0Er[p]);
      grB1L0[p] = new TGraphErrors(Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
      grB1L0[p]->SetLineColor(kBlue);
      grB1L0[p]->SetMarkerColor(kBlue);
      grB1L0[p]->SetFillColor(kBlue);
      grB1L0[p]->SetTitle("background(counts/uA)");    
      grB1L0[p]->Draw("P");
      TAxis *xaxis2= grB1L0[p]->GetXaxis();
      xaxis2->SetLimits(0,65);
      //     legYield[p]->AddEntry(grAsymDr,"background corrected yield(counts/uA)","lpf");
      //     legYield[p]->AddEntry(grB1L0,"background(counts/uA)","lpf");
      //     legYield[p]->SetFillColor(0);
      //     legYield[p]->Draw();
    }
    gPad->Update();
    cYield->Update();
    cYield->SaveAs(Form("%s/%s/%syieldAllPlanes.png",pPath,webDirectory,filePrefix.Data()));
  }

  if(asymDiffPlot) {
    TCanvas *cDiff = new TCanvas("cDiff",Form("expAsym Diff for run:%d",runnum),30,30,1000,600);
    cDiff->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cDiff->cd(p+1);
      grDiffPWTL1_2[p] = new TGraphErrors(endStrip,stripNum1[p],asymDiff[p],zero[p],stripAsymEr[p]);//!the error needs correction
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
    TGraphErrors *grCpp,*grFort;//,*grAsymNr;
    c1->Divide(startPlane+1,endPlane);
    //  for (Int_t p =startPlane; p <endPlane; p++) { 
    for (Int_t p =0; p <1; p++) { 
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
      Float_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];

      for(Int_t p =startPlane; p <endPlane; p++) {
	expAsymPWTL2.open(Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));//only plane1 is used for v2 data
	if(expAsymPWTL2.is_open()) {
	  cout<<"Plotting the expAsym corresponding to PWTL2 for Plane "<<p+1<<endl;
	  //      cout<<"Plotting the difference between asymmetry for two PWTL"<<endl;
	  if(debug1) cout<<"stripNum\t"<<"stripAsym_v2\t"<<"stripAsymEr_v2"<<endl;
	  for(Int_t s =startStrip ; s < endStrip; s++) {
	    if (maskedStrips(p,s)) continue;//this is hard coded for plane-1
	    expAsymPWTL2>>stripNum2[p][s]>>stripAsym_v2[p][s]>>stripAsymEr_v2[p][s];
	    if(debug1) cout<<stripNum2[p][s]<<"\t"<<stripAsym_v2[p][s]<<"\t"<<stripAsymEr_v2[p][s]<<endl;
	    asymDiff[p][s] = (stripAsym[p][s]- stripAsym_v2[p][s]);
	  }
	  expAsymPWTL2.close();
	}
	else cout<<"did not find "<<Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
      }

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

  TF1 *func = new TF1("func","pol0",stripMin,Cedge[0]);//stripMax);
  if (polPlanesPlot) {
    TCanvas *cPol = new TCanvas("cPol","Pol.measured by each Strip",40,200,1000,900); 
    cPol->Divide(startPlane+1,endPlane);
    for (Int_t p =startPlane; p <endPlane; p++) { 
      cPol->cd(p+1);
      grPolPlane[p]=new TGraphErrors(Form("%s/%s/%sexpTheoRatioP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
      grPolPlane[p]->GetXaxis()->SetTitle("strip number");
      grPolPlane[p]->GetYaxis()->SetTitle("ratioExpTheory"); 
      grPolPlane[p]->SetTitle(Form("Run %d, Plane %d",runnum,p+1));      
      grPolPlane[p]->SetMarkerStyle(20);
      grPolPlane[p]->SetMarkerColor(kRed+2);
      grPolPlane[p]->SetMaximum(1.1);
      grPolPlane[p]->SetMinimum(0.5); 
      grPolPlane[p]->Fit(func,"r");
      grPolPlane[p]->Draw("AP");  
      polarization[p] = func->GetParameter(0);
      polEr[p] = func->GetParError(0);
      cout<<"polarization(from Plane"<<p+1<<" = "<<polarization[p]<<" +/- "<<polEr[p]<<endl;
    }
    cPol->Update();
    cPol->SaveAs(Form("%s/%s/%sexpAsymPol.png",pPath,webDirectory,filePrefix.Data()));
  }

//   Float_t polMultiplied[nPlanes][nStrips];

//   for (Int_t p =startPlane; p <endPlane; p++) {
//     //    Cedge = identifyCedgeforPlane(p); 
//     newTheoFile.open(Form("%s/%s/%smodTheoryFileP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
//     if (newTheoFile.is_open()) {
//       for (Int_t s = startStrip; s <=Cedge; s++) { //calcAsym exists only upto Cedge
// 	if (maskedStrips(p,s)) continue;
// 	polMultiplied[p][s] = polarization[p]*theoryAsym[p][s]; 
// 	newTheoFile<<Form("%2.0f\t%f\n",(Float_t)s+1,polMultiplied[p][s]);
//       }
//       newTheoFile.close();
//     } else cout<<"could not open newTheoFile for plane "<<p+1<<endl;
//   }

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
