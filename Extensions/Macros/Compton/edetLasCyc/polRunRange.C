#include <rootClass.h>
#include "comptonRunConstants.h"
Int_t polRunRange(Int_t run1, Int_t run2)
{
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  ifstream poltext;
  ofstream polAll;
  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7;
  Bool_t debug=0,kPlsPrint=0;
  //gStyle->Reset();
  //gStyle->SetStripDecimals(Bool_t strip=kTRUE);
  polAll.open(Form("%s/%s/polList_%d_%d.txt",pPath,webDirectory,run1,run2));
  if (!polAll.is_open()) cout<<"could not open file to assimilate pol"<<endl;

  for (Int_t r=run1; r<=run2; r++) {
    Double_t absPol[nPlanes],pol[nPlanes]={0.0},polEr[nPlanes]={0.0},chiSq[nPlanes],comptEdge[nPlanes];
    Int_t plane[nPlanes],NDF[nPlanes],runnum[nPlanes]; 
    TString filePrefix= Form("run_%d/edetLasCyc_%d_",r,r);
    poltext.open(Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data()));
    if(poltext.is_open()) {
      if(debug) cout<<"opened file "<<Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data())<<endl;
      poltext >>dum1>>dum2>>dum3>>dum4>>dum5>>dum6>>dum7;
      if(debug) cout<<dum1<<"\t"<<dum7<<endl;
      for (Int_t p=startPlane; p<endPlane; p++) {
	poltext>>runnum[p]>>pol[p]>>polEr[p]>>chiSq[p]>>NDF[p]>>comptEdge[p]>>plane[p];
	if(debug) cout<<plane[p]<<"\t"<<pol[p]<<"\t"<<polEr[p]<<endl;
      }
      poltext.close();
    } else cout<<"*** Alert: could not open pol.txt file for run "<<r<<endl;
    absPol[0] = fabs(pol[0]);
    if(debug) cout<<"\nthe polarization for run "<<r<<" is "<<absPol[0]<<endl;
    kPlsPrint = (runnum[0]==r && polEr[0]<3.0 && absPol[0]<95.0 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <5.0);    
    if (kPlsPrint) {
      polAll<<Form("%5.0f\t%2.2f\t%1.2f\t%f\t%d\t%f\n",(Float_t)r,fabs(pol[0]),polEr[0],chiSq[0],NDF[0],comptEdge[0]);
    }
  }
  polAll.close();
  TCanvas *polAvgP1;
  polAvgP1 = new TCanvas("polAvgP1","Avg Polarization Plane 1",10,10,1000,900);
  polAvgP1->Divide(1,2);
  polAvgP1->cd(1);
  TGraphErrors *grPolPlane1,*grRunletPolP1;
  grPolPlane1 = new TGraphErrors(Form("%s/%s/polList_%d_%d.txt",pPath,webDirectory,run1,run2),"%lg %lg %lg");
  //grRunletPolP1 = new TGraphErrors(Form("res.plane.%d.ss.eq.1.0033",1),"%lg %lg %lg");
  //grRunletPolP1 = new TGraphErrors(Form("~/acquired/vladas/modFiles/runletPol_24052_25546.txt"),"%lg %lg %lg");
  grRunletPolP1 = new TGraphErrors(Form("runletPol_22659_23001.txt"),"%lg %lg %lg");


  // TBox *bCedge1;//,*bCedge2,*bCedge3;
  //  bCedge1 = new TBox(run1,60,run2,96);
  //bCedge2 = new TBox(24062,60,24400,96);
  //bCedge3 = new TBox(24062,60,24400,96);
  polAvgP1->GetPad(1)->SetGridx(1);
  grPolPlane1->SetMarkerStyle(kOpenCircle);
  grPolPlane1->SetMarkerColor(kRed);
  grPolPlane1->SetFillColor(0);
  grPolPlane1->SetTitle("Compton electron detector: Laser cycle based analysis");
  grPolPlane1->GetXaxis()->SetTitleSize(0.05);
  grPolPlane1->GetXaxis()->SetTitleOffset(0.84);
  grPolPlane1->GetXaxis()->CenterTitle();
  grPolPlane1->GetXaxis()->SetLabelSize(0.05);
  grPolPlane1->GetXaxis()->SetTitle("Run number");
  grPolPlane1->GetXaxis()->SetLimits(run1+5,run2+5); 

  grPolPlane1->GetYaxis()->SetTitleSize(0.05);
  grPolPlane1->GetYaxis()->SetTitleOffset(0.9);
  grPolPlane1->GetYaxis()->CenterTitle();
  grPolPlane1->GetYaxis()->SetLabelSize(0.06);
  grPolPlane1->GetYaxis()->SetTitle("polarization (%)");
  grPolPlane1->GetYaxis()->SetDecimals(1);

  // bCedge1->SetFillStyle(0);  
  // bCedge1->SetLineColor(kBlue);
  
  grPolPlane1->Draw("AP");
  //bCedge1->Draw("0 l");

  polAvgP1->cd(2);
  polAvgP1->GetPad(2)->SetGridx(1);
  grRunletPolP1->SetFillColor(0);
  grRunletPolP1->SetTitle("Compton electron detector: Runlet based analysis");
  grRunletPolP1->GetXaxis()->SetLimits(run1+5,run2+5); 
  grRunletPolP1->GetXaxis()->SetTitle("Run number");
  grRunletPolP1->GetYaxis()->SetTitle("polarization (%)");
  grRunletPolP1->Draw("AP");

  polAvgP1->SaveAs(Form("%s/%s/pol_%d_%d.png",pPath,webDirectory,run1,run2));
  return (run2 - run1+1);//the number of runs processed
}
