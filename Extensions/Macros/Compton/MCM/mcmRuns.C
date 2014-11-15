#include "../shared/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "../shared/cluster.C"
#include "../shared/simpleAvg.C"
#include "../shared/readFortOut.C"
const Int_t runBegin = 25285, runEnd = 25299;///MCM runs

Int_t mcmRuns(Int_t run1 = runBegin, Int_t run2 = runEnd) {
  gROOT->SetStyle("publication");
  Bool_t debug=1,debug1=0;
  Bool_t kPrintV1=0; 
  Bool_t bAbsPol =0, bPolPlot =1, bPolErCompare =0;
  Bool_t bRates = 0;
  Bool_t bBgdFitInfo = 1;
  Bool_t bChiSqrHist=0;
  ifstream fIn;
  TString file;
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www";///newest results
  const TString destDir="/w/hallc/compton/users/narayan/svn/Compton/MCM/";
  TString ver1 = "MCM";//"cleanLC";///run from a folder named cleanOnlyLC

  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10,dum11;
  Int_t runRange = run2-run1+1;
  TH1D *hPolV1 = new TH1D("polV1", ver1, runRange, 80, 95);
  TH1D *hPolErV1 = new TH1D("polEr "+ver1,"pol error "+ver1,runRange,0,0.1);
  TH1D *hChiSqV1 = new TH1D("chiSqr "+ver1, "chi sqr "+ver1, runRange, 0, 0.1);
  TF1 *lFit = new TF1("lFit", "pol0",run1,run2);

  hPolV1->SetBit(TH1::kCanRebin);
  hPolErV1->SetBit(TH1::kCanRebin);
  hChiSqV1->SetBit(TH1::kCanRebin);

  std::vector< Double_t> runListV1,polV1,polErV1,comptEdgeV1,chiSqV1,edgeErV1;
  std::vector<Int_t> goodCycV1;
  std::vector<Int_t> fitStat;
  std::vector<Double_t> eBeam,eBeamEr;
  ///write the lasCyc based polarization into one central file
  ///some temporary variables, which will be repeatedly used and overwritten
  Double_t tRun,tPol,tPolEr,tChiSq,tCE,tCEEr;
  Double_t tBgd, tBgdEr, tRes, tResEr, tChiSq2;
  Int_t tNDF,pl,goodCyc, tNDF2;

  std::vector<Double_t> ratesSc1, ratesSc2, ratesSc3, ratesSc4, runScR;
  std::vector<Double_t> ratesAc1, ratesAc2, ratesAc3, ratesAc4, runAcR;

  if(bRates) {
    file= "/w/hallc/compton/users/narayan/my_scratch/data/acaggrate_run2.dat"; 
    readFortOut(file,runAcR,ratesAc1,ratesAc2,ratesAc3,ratesAc4,run1,run2);
    cout<<blue<<"no.of runs in the aggregate Ac list: "<<ratesAc1.size()<<normal<<endl;

    file ="/w/hallc/compton/users/narayan/my_scratch/data/aggrate_run2.dat";
    readFortOut(file,runScR,ratesSc1,ratesSc2,ratesSc3,ratesSc4,run1,run2);
    cout<<blue<<"no.of runs in the aggregate Sc list: "<<ratesSc1.size()<<normal<<endl;
  }

  //file = Form("%s/data/polAcMCM_09Oct14.info",pPath);
  file = "polAcMCM_15Nov14.info";
  fIn.open(file.Data());
  Int_t fitStatus=100;
  //const Float_t loLimit=0.05;
  //const Float_t hiLimit=4.5;
  if (fIn.is_open()) { 
    //while(fIn>>tRun>>tPol>>tPolEr>>chiSq>>NDF>>tCE>>tCEEr>>effStrip>>effStripEr>>pl>>goodCyc && fIn.good()) {
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>fitStatus >>pl >>goodCyc && fIn.good()) {
      if(debug1) cout<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<"\t"<<tChiSq/tNDF<<"\t"<<tCE<<"\t"<<tCEEr<<"\t"<<goodCyc<<endl;
      kPrintV1 = (tRun >= run1 && tRun <= run2 && tPolEr<4.0 && tCEEr>0.1 && tPolEr>0.1 );// && chiSq/NDF<5.0);
      if (kPrintV1) {
        runListV1.push_back((Double_t)tRun);
        polV1.push_back(fabs(tPol));
        polErV1.push_back(tPolEr);
        comptEdgeV1.push_back(tCE);
        edgeErV1.push_back(tCEEr);
        chiSqV1.push_back(tChiSq/tNDF);
        goodCycV1.push_back(goodCyc);
        fitStat.push_back(fitStatus);
        //chiSqV1.push_back(chiSq);

        hPolV1->Fill(fabs(tPol),tPolEr);
        hChiSqV1->Fill(tChiSq/tNDF);
      } else if(debug) cout<<"for runnum:"<<tRun<<", tPol:"<<tPol<<" +/- "<<tPolEr<<", CE:"<<tCE<<" +/- "<<tCEEr<<endl;
    }
    fIn.close();
  } else cout<<red<<"*** Could not open "<<file<<normal<<endl; 
  if(debug)cout<<red<<"found valid "<<runListV1.size()<<" analyzed runs in "+ver1<<normal<<endl;

  //Int_t group = 1;
  //std::vector<Double_t> wmRuns, wmPol, wmPolEr;

  if(bAbsPol) {
    if(debug) cout<<blue<<"Histogramming Absolute polarization"<<normal<<endl; 
    TCanvas *cAbsPol = new TCanvas("cAbsPol", "absolute polarization",100,400,450,450);
    hPolV1->Draw("H");
    //hPolV1->Fit("gaus");
    cAbsPol->SaveAs(destDir+Form("absPolMCM.png"));
  }

  if(bPolPlot) {
    if(debug) cout<<blue<<"Polarization for this range"<<normal<<endl; 
    //coordinates in TCanvas are in order :x,y of left top corner;x,y of right bottom corner
    TCanvas *polAvgP1 = new TCanvas("polAvgP1","Polarization trend",0,0,800,400);
    TGraphErrors *grPolV1;
    TLegend *leg = new TLegend(0.15,0.82,0.35,0.95);

    grPolV1 = new TGraphErrors((Int_t)polV1.size(),runListV1.data(),polV1.data(),0,polErV1.data());

    grPolV1->SetMarkerStyle(kOpenCircle);
    grPolV1->SetMarkerColor(kBlue);
    grPolV1->SetLineColor(kBlue);
    grPolV1->GetXaxis()->SetTitle("Run number");
    grPolV1->GetYaxis()->SetTitle("polarization (%)");
    //grPolV1->GetXaxis()->SetLimits(run1-5,run2+5); 
    //grPolV1->SetMaximum(1.5);
    //grPolV1->SetMinimum(-2.0);
    grPolV1->Draw("AP");
    lFit->SetLineColor(kBlue);
    grPolV1->Fit(lFit,"EMR");

    //polAvgP1->Update();///forces the generation of 'stats' box
    //TPaveStats *ps2 = (TPaveStats*)grPolV1->GetListOfFunctions()->FindObject("stats");
    //ps2->SetX1NDC(0.65); ps2->SetX2NDC(0.85);
    //ps2->SetTextColor(kBlue);
    //polAvgP1->Modified();

    leg->AddEntry(grPolV1,ver1,"lpe");
    leg->AddEntry(lFit,"linear fit","l");
    leg->SetFillColor(0);
    leg->Draw();

    polAvgP1->SaveAs(destDir+Form("polAvg_%d_%d.png",run1,run2));
  }//if(bPolPlot)

  if(bPolErCompare) {
    if(debug) cout<<blue<<"Comparing Pol-error from the two methods"<<normal<<endl; 
    TCanvas *cPolErr = new TCanvas("cPolErr","polarization error",10,10,1000,600);

    hPolErV1->SetLineColor(kBlue);
    //hPolErV1->SetTitle(Form("error on Polarization for Qweak phase-1 data"));
    //hPolErV1->GetYaxis()->SetTitle("polarization error (statistical)");
    hPolErV1->Draw("H");
    hPolErV1->Fit("gaus");

    cPolErr->SaveAs(Form(destDir + "polEr_%d_%d.png",run1,run2));
  }//if(bPolErCompare) 

  if(bChiSqrHist) {
    if(debug) cout<<blue<<"Histogramming ChiSqr per Degree of freedom of V2 & V1"<<normal<<endl; 
    TCanvas *cChiSqr = new TCanvas("cChiSqrHist","chi-Sqr/ndf",10,10,1000,600);

    hChiSqV1->SetLineColor(kBlue);
    hChiSqV1->Draw("H");
    hChiSqV1->Fit("gaus");
    cChiSqr->SaveAs(Form(destDir + "chiSqrHist_%d_%d.png",run1,run2));   
  }//if(bChiSqrHist)

  if(bBgdFitInfo) {
    cout<<blue<<"Background and residual fit for run2 "<<normal<<endl;
    TH1D *hBgd = new TH1D("bgd ", "bgd asym", runRange, 0, 0.1);
    TH1D *hChi = new TH1D("chiSqBgd ", "bgd fit ChiSq", runRange, 0, 0.1);
    TH1D *hRes = new TH1D("res ", "fit residue", runRange, 0, 0.1);
    hBgd->SetBit(TH1::kCanRebin);
    hChi->SetBit(TH1::kCanRebin);
    hRes->SetBit(TH1::kCanRebin);
    std::vector<Double_t> bgdRuns, bgdFit, bgdFitEr, residueFit, residueFitEr;
    std::vector<Double_t> chiSqFitBgd, chiSqFitRes; 

    file = Form("%s/data/bgdMCM_10Oct14.info",pPath);
    fIn.open(file);
    if (fIn.is_open()) {
      while(fIn >>tRun >>tRes >>tResEr >>tChiSq2 >>tNDF2 >>tBgd >>tBgdEr >>tChiSq >>tNDF && fIn.good()) {
        bgdRuns.push_back(tRun);
        bgdFit.push_back(tBgd), bgdFitEr.push_back(tBgdEr), chiSqFitBgd.push_back(tChiSq/tNDF);
        residueFit.push_back(tRes), residueFitEr.push_back(tResEr), chiSqFitRes.push_back(tChiSq2/tNDF2);
        //cout<<bgdRuns.back()<<"\t"<<bgdFit.back()<<"\t"<<bgdFitEr.back()<<endl;
        hBgd->Fill(tBgd,tBgdEr);
        hChi->Fill(tChiSq);
        hRes->Fill(tRes);
      }
      fIn.close();
    } else cout<<"couldn't open "<<file<<endl;

    //TCanvas *cBgd = new TCanvas("cBgd","background fit for run2",200,400,1000,600);
    //cBgd->Divide(2,1);
    //cBgd->cd(1);
    //hBgd->Draw();
    //cBgd->cd(2);
    //hChi->Draw();
    TCanvas *cBgd2 = new TCanvas("cBgd2","background fit for run2",200,400,800,450);
    cBgd2->cd();
    TGraphErrors *grB = new TGraphErrors((Int_t)bgdRuns.size(), bgdRuns.data(), bgdFit.data(), 0, bgdFitEr.data());
    grB->SetMarkerStyle(kOpenSquare);
    grB->SetMarkerColor(kRed);
    grB->SetLineColor(kRed);
    grB->Draw("AP");
    lFit->SetLineColor(kRed);
    grB->Fit(lFit,"EMR");
  }
  return 1;
  }
