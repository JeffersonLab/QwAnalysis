//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry
#ifndef __asymFit_F
#define __asymFit_F
#endif

#include <rootClass.h>
//#include "theoryAsym.C"
#include "expAsym.C"
#include "comptonRunConstants.h"
#include "maskedStrips.C"

// Double_t newfunct(Double_t *x, Double_t *par) {
//   return (par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0] + par[4]*x[0]*x[0]*x[0]*x[0]);
// }

void asymFit(Int_t runnum)
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=0, asymPlot=1;
  Float_t stripNum[nPlanes][nStrips];
  Float_t stripAsym[nPlanes][nStrips], stripAsymEr[nPlanes][nStrips];   
  Float_t pol, polEr;//  Double_t par[5];

  TPaveText *pt = new TPaveText(0.6151484,0.8342246,0.8955988,0.8983957,"brNDC"); ///x1,y1,x2,y2
  pt->SetFillColor(19);
  pt->SetTextSize(0.028); 
  pt->SetBorderSize(1);
  pt->SetTextAlign(12);
  pt->SetFillColor(-1);
  pt->SetShadowColor(-1);
  //  pt->Draw();

  Float_t zero[nStrips],stripPlot[nStrips];
  for (Int_t s=0; s <endStrip; s++) { 
    stripPlot[s]=s+1;
    zero[s]=0;
  }

  ifstream theoAsym, expAsymPWTL1;
  TLine *myline = new TLine(1,0,65,0);
  TLegend *leg;
  leg = new TLegend(0.101,0.8,0.4,0.9); 
  gStyle->SetOptFit(1);
  gROOT->ForceStyle();

  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymPWTL1.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymPWTL1.is_open()) {
      cout<<"Plotting the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	//	if (maskedStrips(p,s)) continue; //I think this is causing an offset
	expAsymPWTL1>>stripNum[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	if(debug) cout<<stripNum[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
      }
      expAsymPWTL1.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  Float_t theoryAsym[nPlanes][endStrip];//I shouldn't need to have an index for planes!
  char buffer[4096];
  TString tokenize;
  Int_t col = 2;      // number of columns to be read from file
  Int_t dummyStrip=0;
  for (Int_t p = startPlane; p <endPlane; p++) {
    Cedge[p] = identifyCedgeforPlane(p,stripNum,stripAsymEr);//!still in test mode
    theoAsym.open(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge[p]));
    dummyStrip=0;
    if (theoAsym.is_open()) {
      theoAsym.getline(buffer,4096);                 
      while (theoAsym.good()) {
	if (buffer[0] != ';') {                            
	  for (Int_t i = 0; i < col; i++) {
	    if (0 == i) {
	      tokenize = strtok(buffer, " \t");
	      stripNum[p][dummyStrip] = tokenize.Atof();
	      if(debug) printf("i:%d; line: %d,tok:%g\n",i,__LINE__,tokenize.Atof());
	    } else {
	      tokenize = strtok(NULL, " \t");
	      theoryAsym[p][dummyStrip] = tokenize.Atof();
	      if(debug) printf("i:%d; line: %d,tok:%g\n",i,__LINE__,tokenize.Atof());
	    }
	  }
	  theoAsym.getline(buffer,4096);
	  dummyStrip++;
	}
      }
      theoAsym.close(); 
    } else {
      cout<<"Error ***: could not find the theoryAsym section file"<<endl;
      return;
    }
  }

  TF1 *fn1 = new TF1("fn1","[0]*x");///single parameter fit for polarization
  fn1->SetParameter(0,0.8);
  fn1->SetParLimits(0,-1.00,1.00);
  fn1->SetParNames("polarization");

  //TCanvas *cDummy = new TCanvas("cDummy","won't use later",1050,50,1000,600);
  TGraphErrors *grDummy;
  //cDummy->Divide(startPlane+1,endPlane); 
  for (Int_t p =startPlane; p <endPlane; p++) {  
    //cDummy->cd(p+1);
    grDummy = new TGraphErrors(Cedge[p]-3); //!this -3 is hardcoded for 3 masked strips in this plane
    grDummy->SetTitle("lets see");
    for (Int_t i=0; i<Cedge[p]-3; i++) {//!this -3 is hardcoded for 3 masked strips in this plane
      //if (maskedStrips(0,i)) continue;///sure??!
      grDummy->SetPoint(i,theoryAsym[p][i],stripAsym[p][i]);
      grDummy->SetPointError(i,zero[i],stripAsymEr[p][i]);
      printf("%d\t%f\t%f\n",i+1,theoryAsym[p][i],stripAsym[p][i]);
    }
    grDummy->Draw("goff");//AP");
    grDummy->Fit("fn1","0 M E","goff");
    pol=fn1->GetParameter(0);
    polEr = fn1->GetParError(0);
    printf("The pol parameter for run: %d and plane:%d is %f\n",runnum,p+1,pol);//fn1->GetParameter(0));
  }

  if(asymPlot) {
    TCanvas *cAsym = new TCanvas("cAsym","Asymmetry and Strip number",50,50,1000,600);
    TGraphErrors *grAsymPlane[nPlanes], *grFittedTheo[nPlanes];
    cAsym->Divide(startPlane+1,endPlane); 
    for (Int_t p =startPlane; p <endPlane; p++) {  
      cAsym->cd(p+1);  
      cAsym->GetPad(p+1)->SetGridx(1); 
      grAsymPlane[p]=new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
      //grAsymPlane[p]=new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymEr[p]);
      grAsymPlane[p]->Draw("AP");  
      grAsymPlane[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
      grAsymPlane[p]->GetXaxis()->SetTitleColor(kRed+2);      
      grAsymPlane[p]->GetYaxis()->SetTitleColor(kRed+2);      
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

      grFittedTheo[p] = new TGraphErrors(Cedge[p]-3); //!this -3 is hardcoded for 3 masked strips in this plane
      grFittedTheo[p]->SetTitle("hmm lets see");
      grFittedTheo[p]->SetLineColor(kBlue);
      grFittedTheo[p]->SetLineWidth(3);

      for (Int_t i=0; i<Cedge[p]-3; i++) {//!this -3 is hardcoded for 3 masked strips in this plane
	grFittedTheo[p]->SetPoint(i,stripNum[p][i],theoryAsym[p][i]*pol);
	if(debug) printf("%d\t%f\t%f\n",i+1,theoryAsym[p][i],stripAsym[p][i]);
      }
      grFittedTheo[p]->Draw("L");//goff");

      myline->Draw("same");
      leg->AddEntry(grAsymPlane[p],"experimental asymmetry","pe");
      leg->AddEntry("fn1","QED-Asymmetry fit to exp-Asymmetry","l");//"lpf");//
      leg->Draw("same");

      pt->AddText(Form("Polarization:  %2.2f #pm %2.2f",pol*100.0,polEr*100.0));            
      pt->Draw();

      leg->SetFillColor(0);
      gPad->Update();
    }
    //leg->AddEntry(myline,"zero line","l");
    cAsym->Update(); 
    cAsym->SaveAs(Form("%s/%s/%sasymFitP1.png",pPath,webDirectory,filePrefix.Data()));
  }
}

/*Comments
 *
 */
