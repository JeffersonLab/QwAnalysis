#include "rootClass.h"
#include "comptonRunConstants.h"
#include "read3Cfile.C"
#include "stripMask.C"

Int_t bgdAsym(Int_t runnum = 24761, TString dataType ="Ac") {
  gROOT->SetStyle("publication");
  Bool_t debug = 0;
  Bool_t kBgd = 0;
  Bool_t kYield = 1;
  Bool_t kYieldFit = 0;
  TGraphErrors *grBgd; 
  TF1 *lFit = new TF1("lFit", "pol0",startStrip+1,endStrip);
  lFit->SetLineColor(kRed);
  filePre = Form("run_%d/edetAll_%d_",runnum,runnum);///to to make it stand apart from the LC outputs
  TString file;

  stripMask();
  if(kBgd) {
    ///Read in the background asymmetry file
    TCanvas *cBgd = new TCanvas("cBgd","Bgd Asymmetry",10,650,900,300);
    cBgd->cd();
    cBgd->SetGridx();
    std::vector <Double_t> trueStrip, bgdAsym, bgdAsymEr;

    file = Form("%s/%s/%s%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),dataType.Data(),plane);
    read3Cfile(file, trueStrip, bgdAsym, bgdAsymEr); 

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
    grBgd->Fit(lFit,"RME");//q:quiet mode

    bgdAsymFit = lFit->GetParameter(0);
    bgdAsymFitEr = lFit->GetParError(0);
    chiSqBgdAsym = lFit->GetChisquare();
    bgdAsymFitNDF = lFit->GetNDF();

    ofstream fOut;
    file = Form("%s/%s/%s%sBgdAsymFitInfo.txt",pPath,www,filePre.Data(),dataType.Data());
    fOut.open(file);
    if(fOut.is_open()) {
      fOut<<runnum<<"\t"<<bgdAsymFit <<"\t"<< bgdAsymFitEr<<"\t"<< chiSqBgdAsym<<"\t"<< bgdAsymFitNDF <<endl;
      fOut.close();
    } else cout<<red<<"couldn't open "<<file<<normal<<endl;

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
  }

  if (kYield) { ///determine yield
    TCanvas *cYield;
    TGraphErrors *grB1L0;
    TMultiGraph *grAsymDrAll;
    cYield = new TCanvas("cYield","detector yield",300,200,900,300);
    std::vector<Double_t> trueStrip, yieldL0, yieldL0Er; 
    file = Form("%s/%s/%s"+dataType+"LasOffBkgdP1.txt",pPath,www,filePre.Data());
    read3Cfile(file, trueStrip, yieldL0, yieldL0Er);

    grB1L0 = new TGraphErrors((Int_t)trueStrip.size(), trueStrip.data(), yieldL0.data(), 0, yieldL0Er.data());
    //grB1L0 = new TGraphErrors(file,"%lg %lg %lg");
    grB1L0->SetFillColor(kBlue);
    grB1L0->SetLineColor(kBlue);
    grB1L0->SetMarkerColor(kBlue);

    //TF1 *crossSecFit = new TF1("crossSecFit",theoCrossSec,startStrip+1,Cedge-1,3);///three parameter fit
    TF1 *bgdFit = new TF1("gaus", "gaus", 1, endStrip);
    if(kYieldFit) {
      bgdFit->SetLineColor(kRed);
      grB1L0->Fit("bgdFit","","",0,endStrip);
    }
    grB1L0->Fit("gaus","","",0,endStrip);
    grAsymDrAll = new TMultiGraph();
    //grAsymDrAll->Add(grYield);
    grAsymDrAll->Add(grB1L0);

    //grAsymDrAll->SetTitle(Form(dataType+" Mode Yield, Plane %d",plane));
    grAsymDrAll->Draw("AP");

    grAsymDrAll->GetXaxis()->SetTitle("strip number");
    grAsymDrAll->GetYaxis()->SetTitle("Counts (Hz/uA)");
    //grAsymDrAll->GetXaxis()->SetNdivisions(416, kFALSE);
    grAsymDrAll->GetXaxis()->SetLimits(startStrip,endStrip+1); 

    //TLegend *legYield = new TLegend(0.75,0.75,0.94,0.92);
    TLegend *legYield = new TLegend(0.25,0.75,0.50,0.92);
    if(kYieldFit) legYield->AddEntry("bgdFit","gaussian fit","l");
    legYield->AddEntry(grB1L0,"background","lpe");
    legYield->Draw();
    cYield->SaveAs(Form("%s/%s/%s"+dataType+"BgdFit.png",pPath,www,filePre.Data()));
    //delete cYield;
    //delete grYield;
    //delete grB1L0;
    //delete grAsymDrAll;
    //delete legYield;
  }
  return 1;
}

