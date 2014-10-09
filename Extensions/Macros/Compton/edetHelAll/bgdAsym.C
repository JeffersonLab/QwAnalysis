#include "rootClass.h"
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "stripMask.C"

Int_t bgdAsym(Int_t runnum = 24761, TString dataType ="Ac") {
  gROOT->SetStyle("publication");
  Bool_t debug = 0;
  TGraphErrors *grBgd; 
  TCanvas *cBgd = new TCanvas("cBgd","Bgd Asymmetry",10,650,900,300);
  TF1 *linearFit = new TF1("linearFit", "pol0",startStrip+1,endStrip);
  linearFit->SetLineColor(kRed);
  filePre = Form("run_%d/edetAll_%d_",runnum,runnum);///to to make it stand apart from the LC outputs

  ifstream fBgdAsym;
  std::vector <Double_t> trueStrip, bgdAsym, bgdAsymEr;
  Double_t dum1,dum2,dum3;

  stripMask();
  ///Read in the background asymmetry file
  cBgd->cd();
  cBgd->SetGridx();
  fBgdAsym.open(Form("%s/%s/%s%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),dataType.Data(),plane));
  //fBgdAsym.open(Form("%s/%s/%s"+dataType+"BkgdAymP%d.txt",pPath,www,filePre.Data(),plane));
  if(fBgdAsym.is_open()) {
    if(debug) cout<<"Reading the bgdAsym file for Plane "<<plane<<endl;
    if(debug) cout<<"strip\t"<<"bgdAsym\t"<<"bgdAsymEr"<<endl;
    while( fBgdAsym>>dum1>>dum2>>dum3 && fBgdAsym.good() ) {
      if (std::find(skipStrip.begin(),skipStrip.end(),dum1)!=skipStrip.end()) {
        if(debug) cout<<green<<"skipping strip "<<dum1<<normal<<endl;
        continue; 
      }
      trueStrip.push_back(dum1), bgdAsym.push_back(dum2), bgdAsymEr.push_back(dum3);
      //if(debug) cout<<blue<<dum1<<"\t"<<bgdAsym.back()<<"\t"<<bgdAsymEr.back()<<normal<<endl;
    }
    fBgdAsym.close();
  } else cout<<red<<"did not find the bgdAsym file "<<Form("%s/%s/%s%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),dataType.Data(),plane)<<normal<<endl;

  if(debug) {
    for(int i=0; i<(Int_t)trueStrip.size(); i++) {
      printf("%f\t%f\t%f\n",trueStrip[i],bgdAsym[i],bgdAsymEr[i]);
    }
  }

  grBgd = new TGraphErrors((Int_t)trueStrip.size(),trueStrip.data(),bgdAsym.data(),0,bgdAsymEr.data());
  grBgd->Draw("AP");
  grBgd->GetXaxis()->SetTitle("Strip number");
  grBgd->GetYaxis()->SetTitle("bgd asymmetry");   
  grBgd->SetMarkerStyle(kFullCircle);
  grBgd->SetLineColor(kBlue);
  grBgd->SetMarkerColor(kBlue); ///kRed+2 = Maroon
  grBgd->GetXaxis()->SetLimits(0,65); 
  grBgd->Fit(linearFit,"RE");//q:quiet mode

  bgdAsymFit = linearFit->GetParameter(0);
  bgdAsymFitEr = linearFit->GetParError(0);
  chiSqBgdAsym = linearFit->GetChisquare();
  bgdAsymFitNDF = linearFit->GetNDF();

  //TPaveText *ptBgd;
  //ptBgd = new TPaveText(0.67,0.67,0.9,0.9,"brNDC");///x1,y1,x2,y2
  //ptBgd->SetTextSize(0.048);//0.028); 
  //ptBgd->SetBorderSize(1);
  //ptBgd->SetTextAlign(12);
  //ptBgd->SetFillColor(0);
  //ptBgd->SetShadowColor(-1);
  //ptBgd->AddText(Form("chi Sq / ndf  : %0.1f / %d",chiSqBgdAsym,bgdAsymFitNDF));
  //ptBgd->AddText(Form("linear fit    : %f #pm %f",bgdAsymFit, bgdAsymFitEr));
  //ptBgd->Draw();
  cBgd->SaveAs(Form("%s/%s/%s"+dataType+"BgdAsym.png",pPath,www,filePre.Data()));
  return 1;
}

