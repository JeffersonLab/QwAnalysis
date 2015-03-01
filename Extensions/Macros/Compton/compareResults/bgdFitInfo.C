#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "cluster.C"
#include "simpleAvg.C"
const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
//const Int_t runBegin = 23933, runEnd = 24500;
//const Int_t runBegin = 24250, runEnd = 24450;
//const Int_t runBegin = 24320, runEnd = 24400;
Int_t bgdFitInfo(Int_t run1=runBegin, Int_t run2=runEnd)
{
  gROOT->SetStyle("publication");
  ifstream fIn;
  ofstream fOut;
  TString file;
  const TString destDir="/w/hallc/compton/users/narayan/svn/Compton/compareResults/";
  Double_t tRun,tChiSq;
  Double_t tBgd, tBgdEr, tRes, tResEr, tChiSq2;
  Int_t tNDF, tNDF2;
  Int_t runRange = run2-run1+1;
  Bool_t bRes =0, bBgd =1;
  Bool_t bHist = 0;//, bGraph = 1;
  ///read in the good runs' list
  file ="/w/hallc/compton/users/narayan/my_scratch/data/goodList_longPol.txt";
  std::vector<Double_t> goodRuns;
  fIn.open(file);
  if(fIn.is_open()) {
    while(fIn>>tRun && fIn.good()) {
      goodRuns.push_back(tRun);
    }
    fIn.close();
  } else cout<<red<<"couldn't open the list of good runs"<<normal<<endl;
  cout<<blue<<goodRuns.size()<<" runs found in the goodList"<<endl;

  ///read in the those excluded from good runs' while plotting pol%
  file ="/w/hallc/compton/users/narayan/svn/Compton/compareResults/badList.txt";
  std::vector<Double_t> badRuns;
  fIn.open(file);
  if(fIn.is_open()) {
    while(fIn>>tRun && fIn.good()) {
      badRuns.push_back(tRun);
    }
    fIn.close();
  } else cout<<red<<"couldn't open the list of bad runs"<<normal<<endl;
  cout<<blue<<badRuns.size()<<" runs found in the badList"<<endl;

  cout<<blue<<"Background and residual fit for run2 "<<normal<<endl;
  TH1D *hBgd = new TH1D("bgd ", "bgd asym", runRange, -0.0000001, 0.0000001);
  TH1D *hChi = new TH1D("chiSqBgd ", "bgd fit ChiSq", runRange, 0, 0.1);
  TH1D *hRes = new TH1D("res ", "fit residue", runRange, -0.0000001, 0.0000001);
  TH1D *hChiRes = new TH1D("chiRes", "fit residue chiSq", runRange, 0, 0.0000001);
  hBgd->SetBit(TH1::kCanRebin);
  hChi->SetBit(TH1::kCanRebin);
  hRes->SetBit(TH1::kCanRebin);
  hChiRes->SetBit(TH1::kCanRebin);
  std::vector<Double_t> bgdRuns, bgdFit, bgdFitEr, residueFit, residueFitEr;
  std::vector<Double_t> chiSqFitBgd, chiSqFitRes; 
  std::vector<Double_t> wmRuns, wmBgd, wmBgdEr;
  TF1 *linearFit = new TF1("linearFit", "pol0",runBegin,runEnd);
  Int_t group = 30;
  //fIn.open("run2BgdResidualFits.info");///previous residual generation
  //fIn.open("bgdResAcFits_01Oct14.info");
  //fIn.open("bgdResAcFits_02Oct14.info");
  //fIn.open("bgdResAcFits_05Oct14.info");
  //file = "bgdResAcFits_15Oct14.info";
  file = "bgdAsymNoFlipper_17Oct14.info";
  fIn.open(file);
  fOut.open("checkOut.temp");
  if (fIn.is_open()) {
    //while(fIn >>tRun >>tRes >>tResEr >>tChiSq2 >>tNDF2 >>tBgd >>tBgdEr >>tChiSq >>tNDF && fIn.good()) {
    while(fIn >>tRun >>tBgd >>tBgdEr >>tChiSq >>tNDF && fIn.good()) {
      if(tRun > run2) break;
      if(std::find(goodRuns.begin(), goodRuns.end(), tRun) != goodRuns.end()) {
        if(std::find(badRuns.begin(), badRuns.end(), tRun) == badRuns.end()) { 
          //if(tBgdEr>0.005 || fabs(tBgd)>0.005) {
          //  cout<<blue<<tRun<<"\t"<<tRes <<"\t"<<tResEr <<"\t"<<tChiSq2 <<"\t"<<tNDF2<<"\t"<<tBgd<<"\t"<<tBgdEr <<"\t"<< tChiSq<<"\t"<<tNDF<<normal<<endl;
          //  continue;
          //} 
          if(tResEr>0.005 || fabs(tRes)>0.005) {
            cout<<magenta<<tRun<<"\t"<<tRes <<"\t"<<tResEr <<"\t"<<tChiSq2 <<"\t"<<tNDF2<<"\t"<<tBgd<<"\t"<<tBgdEr <<"\t"<< tChiSq<<"\t"<<tNDF<<normal<<endl;
            continue;
          } 
          bgdRuns.push_back(tRun);
          bgdFit.push_back(tBgd), bgdFitEr.push_back(tBgdEr), chiSqFitBgd.push_back(tChiSq/tNDF);
          residueFit.push_back(tRes), residueFitEr.push_back(tResEr), chiSqFitRes.push_back(tChiSq2/tNDF2);
          hBgd->Fill(tBgd);
          hChi->Fill(tChiSq/tNDF);
          hRes->Fill(tRes,tResEr);
          hChiRes->Fill(tChiSq2/tNDF2);
        }
      } //else fOut<<tRun<<"\t rejected"<<endl;
    }
    fIn.close();
  } else cout<<red<<file<<" did not open"<<normal<<endl;
  fOut.close();
  cout<<blue<<bgdRuns.size()<<" runs found in the bgd Fit info list"<<normal<<endl;
  if(bHist) {
    TCanvas *cBgd = new TCanvas("cBgd","background fit for run2",0,0,1000,600);
    cBgd->Divide(2,1);
    cBgd->cd(1);
    //hBgd->SetNdivisions(505);//SetNdivisions
    //hBgd->SetLineColor(kBlue);
    //hBgd->Draw();
    //hBgd->Fit("gaus");
    hRes->SetNdivisions(505);//SetNdivisions
    hRes->SetLineColor(kRed);
    hRes->Draw();
    cBgd->cd(2);
    //hChi->Draw();
    hChiRes->Draw();
    //cBgd->SaveAs("bgdHist.png");
    cBgd->SaveAs("resHist.png");
  }
  if(bRes) {
    cluster(group, bgdRuns,residueFit,residueFitEr,wmRuns, wmBgd, wmBgdEr);///to average the residues
    TCanvas *cBgd2 = new TCanvas("cBgd2","residue fit for run2",200,400,800,450);
    cBgd2->cd();
    TGraphErrors *grB = new TGraphErrors((Int_t)wmRuns.size(), wmRuns.data(), wmBgd.data(), 0, wmBgdEr.data());
    grB->SetMarkerStyle(kOpenSquare);
    grB->SetMarkerColor(kMagenta);
    grB->SetLineColor(kMagenta);
    grB->Draw("AP");
    grB->GetYaxis()->SetTitle("polarization fit residue");
    grB->GetXaxis()->SetTitle("Run number");
    linearFit->SetLineColor(kMagenta);
    grB->Fit(linearFit,"EM");
    grB->GetXaxis()->SetLimits(run1-5,run2+5); 
    cBgd2->SaveAs("resRun2.png");
  }

  if(bBgd) {
    cluster(group, bgdRuns,bgdFit,bgdFitEr,wmRuns, wmBgd, wmBgdEr);///to average the bgd asym
    TCanvas *cBgd2 = new TCanvas("cBgd2","background fit for run2",200,400,800,450);
    cBgd2->cd();
    TGraphErrors *grB = new TGraphErrors((Int_t)wmRuns.size(), wmRuns.data(), wmBgd.data(), 0, wmBgdEr.data());
    grB->SetMarkerStyle(kOpenSquare);
    grB->SetMarkerColor(kBlue);
    grB->SetLineColor(kBlue);
    grB->Draw("AP");
     grB->SetMaximum(0.0012);
     grB->SetMinimum(-0.0014);
    grB->GetYaxis()->SetTitle("bgd asymmetry");
    grB->GetXaxis()->SetTitle("Run number");
    grB->Fit(linearFit,"EM");
    grB->GetXaxis()->SetLimits(run1-5,run2+5); 
    linearFit->SetLineColor(kBlue);
    //cBgd2->SaveAs(Form("bgdRun2_cut2000_G%d.png",group));
    cBgd2->SaveAs(Form("bgdRun2_cut1261_G%d.png",group));
  }
  return (Int_t)bgdRuns.size();//the number of runs used
}

