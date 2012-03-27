#include <rootClass.h>
#include "comptonRunConstants.h"

Int_t fileReadDraw(Int_t runnum) 
{
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TGraphErrors *grCpp,*grCpp_v2,*grFort,*grDiffPWTL1_2,*grAsymDr,*grAsymNr;
  ifstream in1, in2;
  TLegend *leg;
  TCanvas *c1 = new TCanvas("c1",Form("edet Asymmetry run:%d",runnum),10,10,1000,600);
  TCanvas *cDiff = new TCanvas("cDiff",Form("expAsym Diff for run:%d",runnum),30,30,1000,600);
  TCanvas *cAsymComponent = new TCanvas("cAsymDiff",Form("Nr and Dr of Asym for run:%d",runnum),30,30,1000,600);
  TLine *myline = new TLine(0,0,70,0);
  ifstream fortranOutP1, expAsymPWTL1, expAsymPWTL2, expAsymComponents;
  leg = new TLegend(0.1,0.7,0.4,0.9);
  Float_t stripAsym[nStrips],stripAsymEr[nStrips],stripAsym_v2[nStrips],stripAsymEr_v2[nStrips];//,asymDiffPercent[nStrips];
  Float_t stripNum[nStrips],asymDiff[nStrips],zero[nStrips];
  Float_t stripAsymDr[nStrips],stripAsymDrEr[nStrips];
  Float_t stripAsymNr[nStrips];//,stripAsymNrEr[nStrips];
  Bool_t debug=1,debug1=1;

  for(Int_t s=startStrip; s<endStrip; s++) {
    zero[s]=0;
    stripAsymDr[s]=0.0,stripAsymDrEr[s]=0.0;
    stripAsymNr[s]=0.0;//,stripAsymNrEr[s]=0.0;
  }

  expAsymPWTL1.open(Form("%s/%s/%sexpAsymP1.txt",pPath,webDirectory,filePrefix.Data()));
  expAsymPWTL2.open(Form("%s/%s/%sexpAsymP1_v2.txt",pPath,webDirectory,filePrefix.Data()));
  if(expAsymPWTL1.is_open()) {
    if(expAsymPWTL2.is_open()) {
      cout<<"Plotting the difference between asymmetry for two PWTL"<<endl;
      if(debug) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsym_v2"<<endl;
      for(Int_t s =0 ; s < endStrip; s++) {//the index# below does not necessarily represent strip#
	expAsymPWTL1>>stripNum[s]>>stripAsym[s]>>stripAsymEr[s];
	expAsymPWTL2>>stripNum[s]>>stripAsym_v2[s]>>stripAsymEr_v2[s];
	if(debug) cout<<stripNum[s]<<"\t"<<stripAsym[s]<<"\t"<<stripAsym_v2[s]<<endl;
      }
      if (debug1) cout<<"stripAsym\t"<<"stripAsym_v2\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s <endStrip; s++) {
	asymDiff[s] = (stripAsym[s]- stripAsym_v2[s]);
	//if((stripAsym[s]>0.001)||(stripAsym[s]<-0.001)) asymDiffPercent[s] = asymDiff[s]/stripAsym[s];
	//if (debug1) cout<<asymDiffPercent[s]<<"\t"<<stripAsymEr[s]<<endl;
      }
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymP1_v2.txt",pPath,webDirectory,filePrefix.Data())<<endl;
  }
  else cout<<"did not find "<<Form("%s/%s/%sexpAsymP1.txt",pPath,webDirectory,filePrefix.Data())<<endl;

  expAsymComponents.open(Form("%s/%s/%sexpAsymComponentsP1.txt",pPath,webDirectory,filePrefix.Data()));
  if(expAsymComponents.is_open()) {
    cout<<"Plotting the denominator of exp asymmetry from PWTL1"<<endl;
      for(Int_t s =0 ; s < endStrip; s++) {//the index# below does not necessarily represent strip#
	expAsymComponents>>stripNum[s]>>stripAsymNr[s]>>stripAsymDr[s]>>stripAsymDrEr[s];
      }
  }
  else cout<<"did not find "<<Form("%s/%s/%sexpAsymComponentsP1.txt",pPath,webDirectory,filePrefix.Data())<<endl;

  cAsymComponent->Divide(1,2);
  cAsymComponent->cd(1);
  grAsymNr = new TGraphErrors(endStrip,stripNum,stripAsymNr,zero,stripAsymDrEr);
  grAsymNr->SetMarkerStyle(kFullTriangleUp);
  grAsymNr->SetMarkerSize(0.5);
  grAsymNr->GetXaxis()->SetTitle("strip number");
  grAsymNr->GetYaxis()->SetTitle("laser on difference");
  grAsymNr->SetTitle("laser On difference");//Numerator of experimental asymmetry
  grAsymNr->Draw("AP");
  cAsymComponent->SetGridx(1);

  cAsymComponent->cd(2);
  grAsymDr = new TGraphErrors(endStrip,stripNum,stripAsymDr,zero,stripAsymDrEr);
  grAsymDr->SetMarkerStyle(kFullTriangleDown);//open triangle down
  grAsymDr->SetMarkerSize(0.5);
  grAsymDr->GetXaxis()->SetTitle("strip number");
  grAsymDr->GetYaxis()->SetTitle("Normalized Rates (Hz/uA)");
  grAsymDr->SetTitle("normalized rates(Hz/uA)");//Denominator of experimental asymmetry
  grAsymDr->Draw("AP");
  cAsymComponent->SetGridx(1);

  cDiff->cd();
  grDiffPWTL1_2 = new TGraphErrors(endStrip,stripNum,asymDiff,zero,stripAsymEr);//the error is not totally correct yet!
  grDiffPWTL1_2->SetMarkerStyle(kOpenSquare);
  grDiffPWTL1_2->SetMarkerSize(0.5);
  grDiffPWTL1_2->GetXaxis()->SetTitle("strip number");
  grDiffPWTL1_2->GetYaxis()->SetTitle("expAsym difference (PWTL1-PWTL2)");
  grDiffPWTL1_2->SetMaximum(0.005);
  grDiffPWTL1_2->SetMinimum(-0.005);
  grDiffPWTL1_2->SetTitle("Difference in asymmetry between two trigger width");
  grDiffPWTL1_2->Draw("AP");
  cDiff->SetGridx(1);

  c1->cd();
  grCpp = new TGraphErrors(Form("%s/%s/%sexpAsymP1.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg %lg");
  grCpp_v2 = new TGraphErrors(Form("%s/%s/%sexpAsymP1_v2.txt",pPath,webDirectory,filePrefix.Data()), "%lg %lg %lg");
  grCpp->GetXaxis()->SetTitle("strip number");
  grCpp->GetYaxis()->SetTitle("asymmetry");
  grCpp->SetTitle("laserCycle wise edet asym");
  grCpp->SetMaximum(0.05);
  grCpp->SetMinimum(-0.05);
  c1->SetGridx(1);
  
  grCpp->SetMarkerStyle(kOpenCircle);
  grCpp->SetLineColor(kRed+2);
  grCpp->SetMarkerColor(kRed+2);
  grCpp->SetFillColor(0);
  grCpp->Draw("AP");

  grCpp_v2->SetMarkerStyle(kFullCircle);
  grCpp_v2->SetLineColor(kGreen);
  grCpp_v2->SetMarkerColor(kGreen);
  grCpp_v2->SetFillColor(0);
  grCpp_v2->Draw("P");

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
  leg->AddEntry(grCpp_v2,"eDet Asymmetry v2 data","lpf");
  leg->AddEntry(myline,"zero line","l");
  leg->SetFillColor(0);
  leg->Draw();
  
  c1->SaveAs(Form("%s/%s/%sexpAsymP1.png",pPath,webDirectory,filePrefix.Data()));
  cDiff->SaveAs(Form("%s/%s/%sdiffexpAsymP1.png",pPath,webDirectory,filePrefix.Data()));
  cAsymComponent->SaveAs(Form("%s/%s/%sexpAsymComponentsP1.png",pPath,webDirectory,filePrefix.Data()));
  return runnum;
}
