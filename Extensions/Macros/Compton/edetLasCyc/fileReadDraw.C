#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Int_t fileReadDraw(Int_t runnum) 
{
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TGraphErrors *grCpp,*grCpp_v2,*grFort;//,*grAsymNr;
  TGraphErrors *grPolPlane[nPlanes],*grAsymDr[nPlanes],*grB1L0[nPlanes],*grDiffPWTL1_2[nPlanes];
  TGraphErrors *grTheoryAsym, *grAsymPlane[nPlanes];
  ifstream in1, in2;
  TLegend *leg, *legYield;
  TCanvas *c1 = new TCanvas("c1",Form("edet Asymmetry run:%d",runnum),10,10,1000,600);
  TCanvas *cDiff = new TCanvas("cDiff",Form("expAsym Diff for run:%d",runnum),30,30,1000,600);
  TCanvas *cYield = new TCanvas("cYield",Form("Yield for run:%d",runnum),30,100,1000,900);
  //TCanvas *cAsymComponent = new TCanvas("cAsymDiff",Form("Nr and Dr of Asym for run:%d",runnum),30,30,1000,600);
  TCanvas *cPol = new TCanvas("cPol","Pol.measured by each Strip",40,60,700,700); 
  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",10,40,800,600);
  TLine *myline = new TLine(0,0,70,0);
  ifstream fortranOutP1, expAsymPWTL1, expAsymPWTL2, expAsymComponents;
  leg = new TLegend(0.1,0.7,0.4,0.9);
  legYield = new TLegend(0.1,0.7,0.4,0.9);

  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
  Float_t stripNum1[nPlanes][nStrips],stripNum2[nPlanes][nStrips],stripNum3[nPlanes][nStrips];
  Float_t asymDiff[nPlanes][nStrips],zero[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];
  Float_t stripAsymNr[nPlanes][nStrips];//,stripAsymNrEr[nStrips];
  Float_t accumB1L0[nPlanes][nStrips],accumB1L0Er[nPlanes][nStrips];

  Bool_t pUsed[nPlanes]={0};//!not sure if this trick will work to initialize all elements with zero
  Bool_t debug=0,debug1=0;
  
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
      cout<<"Plotting the difference between asymmetry for two PWTL"<<endl;
      if(debug) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) continue;
	expAsymPWTL1>>stripNum1[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	if(debug) cout<<stripNum1[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
      }
      expAsymPWTL1.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  for(Int_t p =0; p <1; p++) {
    expAsymPWTL2.open(Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));//only plane1 is used for v2 data
    if(expAsymPWTL2.is_open()) {
      cout<<"Plotting the difference between asymmetry for two PWTL"<<endl;
      if(debug1) cout<<"stripNum\t"<<"stripAsym_v2\t"<<"stripAsymEr_v2"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(0,s)) continue;//this is hard coded for plane-1
	expAsymPWTL2>>stripNum2[p][s]>>stripAsym_v2[p][s]>>stripAsymEr_v2[p][s];
	if(debug1) cout<<stripNum1[p][s]<<"\t"<<stripAsym_v2[p][s]<<"\t"<<stripAsymEr_v2[p][s]<<endl;
      }
      for(Int_t s =startStrip ; s <endStrip; s++) {
	asymDiff[p][s] = (stripAsym[p][s]- stripAsym_v2[p][s]);
      }
      expAsymPWTL2.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }


  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymComponents.open(Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymComponents.is_open()) {
      cout<<"Plotting the yield from PWTL1 for plane "<<p<<endl;
      if(debug) cout<<"stripNum\tstripAsymNr\tstripAsymDr\tstripAsymDrEr\taccumB1L0\taccumB1L0Er"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) continue;///I don't want to loop on plane here but I do need to skip the irrelevant strips
	expAsymComponents>>stripNum3[p][s]>>stripAsymNr[p][s]>>stripAsymDr[p][s]>>stripAsymDrEr[p][s]>>accumB1L0[p][s]>>accumB1L0Er[p][s];
	if(debug) cout<<stripNum3[p][s]<<"\t"<<stripAsymNr[p][s]<<"\t"<<stripAsymDr[p][s]<<"\t"<<stripAsymDrEr[p][s]<<"\t"<<accumB1L0[p][s]<<"\t"<<accumB1L0Er[p][s]<<endl;
      }
      expAsymComponents.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

//   cAsymComponent->cd(1);
//   grAsymNr = new TGraphErrors(endStrip,stripNum,stripAsymNr,zero,stripAsymDrEr);
//   grAsymNr->SetMarkerStyle(kFullTriangleUp);
//   grAsymNr->SetMarkerSize(0.5);
//   grAsymNr->GetXaxis()->SetTitle("strip number");
//   grAsymNr->GetYaxis()->SetTitle("laser on difference");
//   grAsymNr->SetTitle("laser On difference");//Numerator of experimental asymmetry
//   grAsymNr->Draw("AP");//!temporary
//   cAsymComponent->SetGridx(1);

  cYield->Divide(startPlane+1,endPlane);
  for (Int_t p =startPlane; p <endPlane; p++) { 
    cYield->cd(p+1);
    grAsymDr[p] = new TGraphErrors(endStrip,stripNum3[p],stripAsymDr[p],zero[p],stripAsymDrEr[p]);
    //grAsymDr->SetMarkerStyle(kFullTriangleDown);//open triangle down
    //grAsymDr->SetMarkerSize(0.5);
    grAsymDr[p]->SetLineColor(kGreen);//no difference :(
    grAsymDr[p]->SetMarkerColor(kBlack);
    grAsymDr[p]->SetFillColor(kGreen);
    grAsymDr[p]->GetXaxis()->SetTitle("strip number");
    grAsymDr[p]->GetYaxis()->SetTitle("charge normalized yield (counts/uA)");
    grAsymDr[p]->SetTitle("Yield(counts/uA)");//Denominator of experimental asymmetry
    grAsymDr[p]->Draw("AB");
    grB1L0[p] = new TGraphErrors(endStrip,stripNum3[p],accumB1L0[p],zero[p],accumB1L0Er[p]);
    grB1L0[p]->SetLineColor(kBlue);
    grB1L0[p]->SetMarkerColor(kBlue);
    grB1L0[p]->SetFillColor(kBlue);
    grB1L0[p]->Draw("B");
  }
  cYield->Update();
  //  cYield->SetGridx(1);
  
  cDiff->cd();
  //for (Int_t p =startPlane; p <endPlane; p++) { 
  for (Int_t p =0; p <1; p++) { 
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
  //cDiff->SetGridx(1);

  c1->cd();//!I need to rename the source file appropriately for multiple planes to work
  grCpp = new TGraphErrors(Form("%s/%s/%sexpAsymP1.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg %lg");
  grCpp_v2 = new TGraphErrors(Form("%s/%s/%sexpAsymP1_v2.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg %lg");
  grCpp->GetXaxis()->SetTitle("strip number");
  grCpp->GetYaxis()->SetTitle("asymmetry");
  grCpp->SetTitle("laserCycle wise e-detector asym");
  grCpp->SetMaximum(0.05);
  grCpp->SetMinimum(-0.05);
  // c1->SetGridx(1);
  
  grCpp->SetMarkerStyle(kFullCircle);
  grCpp->SetLineColor(kRed);
  grCpp->SetMarkerColor(kRed);
  grCpp->SetFillColor(0);
  grCpp->Draw("AP");

  grCpp_v2->SetMarkerStyle(kFullCircle);
  grCpp_v2->SetLineColor(kGreen);
  grCpp_v2->SetMarkerColor(kGreen);
  grCpp_v2->SetFillColor(0);
  if(v2processed) grCpp_v2->Draw("P");

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
    leg->AddEntry(grFort,"runlet based eDet Asymmetry","lpf");
    fortranOutP1.close();
  }
  else cout<<"corresponding fortran file for run "<<runnum<<" doesn't exist"<<endl;
  
  leg->AddEntry(grCpp,"eDet Asymmetry","lpf");
  if(v2processed) leg->AddEntry(grCpp_v2,"eDet Asymmetry v2 data","lpf");
  leg->AddEntry(myline,"zero line","l");
  leg->SetFillColor(0);
  if(v2processed) leg->Draw();
  
  // cPol->Divide(2,2);  
  cPol->cd();
  for (Int_t p =startPlane; p <endPlane; p++) { 
    //cPol->cd(p+1);
    grPolPlane[p]=new TGraphErrors(Form("%s/%s/%sexpTheoRatioP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
    grPolPlane[p]->GetXaxis()->SetTitle("strip number");
    grPolPlane[p]->GetYaxis()->SetTitle("ratioExpTheory"); 
    grPolPlane[p]->SetTitle(Form("Plane %d",p+1));      
    grPolPlane[p]->SetMarkerStyle(20);
    grPolPlane[p]->SetMarkerColor(kRed+2);
    grPolPlane[p]->Draw("AP");  
    grPolPlane[p]->SetMaximum(1.1);
    grPolPlane[p]->SetMinimum(0.5);        
  }
  cPol->Update();

  //  cAsym->Divide(2,2);
  cAsym->cd();
  for (Int_t p =startPlane; p <endPlane; p++) {
    //    cAsym->cd(p+1);
    //grAsymPlane[p] = new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymRMS[p]);
    grAsymPlane[p] = new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
    grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
    grAsymPlane[p]->SetTitle(Form("Run: %d, Plane %d",runnum,p+1));
    grAsymPlane[p]->SetMarkerStyle(20);
    grAsymPlane[p]->SetLineColor(kRed+2);
    grAsymPlane[p]->SetMarkerColor(kRed+2); ///Maroon
    //grAsymPlane[p]->Fit("fn2","R");
    grAsymPlane[p]->SetMaximum(0.042);
    grAsymPlane[p]->SetMinimum(-0.042);
    grAsymPlane[p]->Draw("AP");
    myline->Draw();

    grTheoryAsym = new TGraphErrors(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge), "%lg %lg");
    //grTheoryAsym = new TGraphErrors(endStrip,stripPlot,calcAsym,zero,zero);
    grTheoryAsym->SetLineColor(kBlue);
    grTheoryAsym->Draw("L");    
    cAsym->Update();
  } 
  cPol->SaveAs(Form("%s/%s/%sexpAsymPolFromP1.png",pPath,webDirectory,filePrefix.Data()));
  c1->SaveAs(Form("%s/%s/%sexpAsymP1.png",pPath,webDirectory,filePrefix.Data()));
  cAsym->SaveAs(Form("%s/%s/%sexpTheoAsymP1.png",pPath,webDirectory,filePrefix.Data()));
  cDiff->SaveAs(Form("%s/%s/%sdiffexpAsymP1.png",pPath,webDirectory,filePrefix.Data()));
  cYield->SaveAs(Form("%s/%s/%syieldAllPlanes.png",pPath,webDirectory,filePrefix.Data()));
  return runnum;
}
