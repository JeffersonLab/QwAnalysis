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
  Float_t stripNum1[nPlanes][nStrips];
  Float_t stripAsym[nPlanes][nStrips], stripAsymEr[nPlanes][nStrips];   
  Double_t par[5];

  ifstream theoAsym, expAsymPWTL1;
  TLine *myline = new TLine(0,0,70,0);
  TLegend *leg;//, *legYield[nPlanes];
  leg = new TLegend(0.101,0.8,0.4,0.9); 
  gStyle->SetOptFit(1);
  gROOT->ForceStyle();

  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymPWTL1.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymPWTL1.is_open()) {
      cout<<"Plotting the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug) cout<<"stripNum1\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
	if (maskedStrips(p,s)) continue; //I think this is causing an offset
	expAsymPWTL1>>stripNum1[p][s]>>stripAsym[p][s]>>stripAsymEr[p][s];
	if(debug) cout<<stripNum1[p][s]<<"\t"<<stripAsym[p][s]<<"\t"<<stripAsymEr[p][s]<<endl;
      }
      expAsymPWTL1.close();
    }
    else cout<<"did not find "<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
  }

  Float_t stripNum[nPlanes][endStrip], theoryAsym[nPlanes][endStrip];//I shouldn't need to have an index for planes!
  char buffer[4096];
  TString tokenize;
  Int_t col = 2;      // number of columns to be read from file
  Int_t dummyStrip=0;
  for (Int_t p = startPlane; p <endPlane; p++) {
    Cedge = identifyCedgeforPlane(p,stripAsymEr);//!still in test mode
    theoAsym.open(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge));
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
    } else cout<<"could not find the theoryAsym section file"<<endl;
  }

  Float_t zero[nStrips],stripPlot[nStrips];
  for (Int_t s=0; s <endStrip; s++) { 
    stripPlot[s]=s+1;
    zero[s]=0;
  }

  TGraphErrors *grtheory = new TGraphErrors(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge), "%lg %lg");
  grtheory->GetXaxis()->SetTitle("strip number");
  grtheory->GetYaxis()->SetTitle("theoretical Asymmetry"); 
  grtheory->GetYaxis()->CenterTitle(); 
  grtheory->SetTitle("theoretical asym Vs strip number");//"#rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(4);
  grtheory->SetMarkerColor(4);
  grtheory->Fit("pol4","0");//,"goff");
  grtheory->Draw("goff");//("AP");
  
  TF1 *theoFit = (TF1*)grtheory->GetFunction("pol4");
  for(Int_t i=0; i<=4; i++) {
    par[i] = theoFit->GetParameter(i);
    printf("%dth parameter is %g\n",i,par[i]);
  }
  
  //TString trial = Form("[0] *(%g + (%g*x) + (%g*x*x) + (%g*x*x*x) + (%g*x*x*x*x))",par[0],par[1],par[2],par[3],par[4]);
  //TString trial = Form("[0] + [1]*(%g + (%g*x) + (%g*x*x) + (%g*x*x*x) + (%g*x*x*x*x))",par[0],par[1],par[2],par[3],par[4]);
  TString trial = Form("[1]*(%g + (%g)*(x+[0]) + (%g)*pow(x+[0],2) + (%g)*pow(x+[0],3) + (%g)*pow(x+[0],4))",par[0],par[1],par[2],par[3],par[4]);
  //TString trial = Form("[1]*(%g + (%g)*(x*[0]) + (%g)*pow(x*[0],2) + (%g)*pow(x*[0],3) + (%g)*pow(x*[0],4))",par[0],par[1],par[2],par[3],par[4]);

  cout<<"the theoretical fit function is "<< trial.Data()<<endl;

//   TF1 *fn2 = new TF1("fn2",Form("[0]*(%f + %f*x + %f*x*x + %f*x*x*x + %f*x*x*x*x)",par[0],par[1],par[2],par[3],par[4]),startStrip+1,Cedge);//!for some reason this did not work !
//   TF1 *fn2=new TF1("fn2",newfunct,1,Cedge,2);

  TF1 *fn2 = new TF1("fn2",Form("%s",trial.Data()),1,54);//startStrip+1,Cedge);
  fn2->SetParameter(0,-0.5);
  fn2->SetParLimits(0,-1.5,1.5);
  //fn2->FixParameter(0,-0.5);
  //fn2->FixParameter(0,0);
  fn2->SetParameter(1,0.8);
  fn2->SetParLimits(1,-1.00,1.00);
  fn2->SetParNames("in-strip position","polarization");
  fn2->SetLineColor(kBlue);
  fn2->SetLineWidth(3);

  if(asymPlot) {
    TCanvas *cAsym = new TCanvas("cAsym","Asymmetry and Strip number",50,50,1000,600);
    TGraphErrors *grAsymPlane[nPlanes];
    cAsym->Divide(startPlane+1,endPlane); 
    for (Int_t p =startPlane; p <endPlane; p++) {  
      //for (Int_t p =0; p <1; p++) {
      cAsym->cd(p+1);  
      grAsymPlane[p]=new TGraphErrors(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
      //grAsymPlane[p]=new TGraphErrors(endStrip,stripPlot,stripAsym[p],zero,stripAsymEr[p]);
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
      grAsymPlane[p]->Draw("AP");  
      grAsymPlane[p]->Fit("fn2","R M E");  
      myline->Draw();
      cAsym->GetPad(p+1)->SetGridx(1); 
      cAsym->Update();         
      leg->AddEntry(grAsymPlane[p],"experimental asymmetry","pe");
      leg->AddEntry("fn2","QED-Asymmetry fit to exp-Asymmetry","l");//"lpf");//
      leg->SetFillColor(0);
      leg->Draw();
    }
    //leg->AddEntry(myline,"zero line","l");
    cAsym->Update(); 
    cAsym->SaveAs(Form("%s/%s/%sasymFitP1_firstParFixedat0.5.png",pPath,webDirectory,filePrefix.Data()));
  }
}

/*Comments
 *
 */
