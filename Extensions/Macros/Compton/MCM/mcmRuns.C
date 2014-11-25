#include "../shared/rootClass.h"
#include "../shared/cluster.C"
#include "../shared/simpleAvg.C"
#include "../shared/readFortOut.C"
#include "../shared/read3CfileArray.C"

const Int_t runBegin = 25285, runEnd = 25299;///MCM runs

Int_t mcmRuns(Int_t run1 = runBegin, Int_t run2 = runEnd) {
  gROOT->SetStyle("publication");
  Bool_t debug=1,debug1=0;
  Bool_t bPolHist    =0;
  Bool_t bPolPlot   =0;
  Bool_t bPolEr     =0;
  Bool_t bRates     =0;
  Bool_t bBgdFitInfo=0;
  Bool_t bChiSqr    =0;
  Bool_t bCE        =0;
  Bool_t bCycles    =0;
  Bool_t bPolCE     =0;///correlation plot between polEr and edgeEr
  Bool_t bBPM       =0;
  Bool_t bDiff      =0;
  Bool_t bDiffAsym  =1;
  Bool_t bRatio     =0;
  ifstream fIn;
  TString file;
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www";///newest results
  const TString destDir="/w/hallc/compton/users/narayan/svn/Compton/MCM/";
  TString ver = "08";//"cleanLC";///run from a folder named cleanOnlyLC

  Int_t runRange = run2-run1+1;
  TH1D *hPol = new TH1D("pol", "pol", runRange, 80, 95);
  TH1D *hPolEr = new TH1D("polEr ","pol error ",runRange,0,0.1);
  TH1D *hChiSq = new TH1D("chiSqr ", "chi sqr ", runRange, 0, 0.1);
  TH1D *hCE = new TH1D("CE", "CE", runRange, 0.1, 0.2);
  TF1 *lFit = new TF1("lFit", "pol0",run1,run2);

  hPol->SetBit(TH1::kCanRebin);
  hCE->SetBit(TH1::kCanRebin);
  hPolEr->SetBit(TH1::kCanRebin);
  hChiSq->SetBit(TH1::kCanRebin);

  std::vector< Double_t> rList,pol,polEr,CE,chiSq,edgeEr,goodCyc;
  std::vector<Int_t> fitStat;
  std::vector<Double_t> eBeam,eBeamEr;
  ///write the lasCyc based polarization into one central file
  ///some temporary variables, which will be repeatedly used and overwritten
  Double_t tRun,tPol,tPolEr,tChiSq,tCE,tCEEr;
  Double_t tBgd, tBgdEr, tRes, tResEr, tChiSq2;
  Int_t tNDF,pl,nCyc, tNDF2;

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
  //file = "polAcMCM_15Nov14.info";
  //file = "polAcMCM_16Nov14.info";
  //file = "polAcMCM_01.info";
  //file = "polAcMCM_02.info";
  file = "polAcMCM_"+ver+".info";
  fIn.open(file);
  Int_t fitStatus=100;
  if (fIn.is_open()) { 
    //while(fIn>>tRun>>tPol>>tPolEr>>chiSq>>NDF>>tCE>>tCEEr>>effStrip>>effStripEr>>pl>>nCyc && fIn.good()) {
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>fitStatus >>pl >>nCyc && fIn.good()) {
      if(debug1) cout<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<"\t"<<tChiSq/tNDF<<"\t"<<tCE<<"\t"<<tCEEr<<"\t"<<nCyc<<endl;
      if (tRun < run1 || tRun > run2 || tPolEr>10 || tCEEr>6.0 || tCEEr<0.1 || tPolEr<0.1) {
        cout<<magenta<<"for runnum:"<<tRun<<", tPol:"<<tPol<<"+/-"<<tPolEr<<", CE:"<<tCE<<"+/-"<<tCEEr<<normal<<endl;
      } else {
        rList.push_back((Double_t)tRun);
        pol.push_back(fabs(tPol));
        polEr.push_back(tPolEr);
        CE.push_back(tCE);
        edgeEr.push_back(tCEEr);
        chiSq.push_back(tChiSq/tNDF);
        goodCyc.push_back(nCyc);
        fitStat.push_back(fitStatus);

        hPol->Fill(fabs(tPol),tPolEr);
        //hPol->Fill(fabs(tPol));
        hPolEr->Fill(tPolEr);
        hCE->Fill(tCE);
        hChiSq->Fill(tChiSq/tNDF);
      }
    }
    fIn.close();
  } else cout<<red<<"*** Could not open "<<file<<normal<<endl; 
  if(debug)cout<<blue<<"found valid "<<rList.size()<<" analyzed runs in "+ver<<normal<<endl;

  if(bRatio) {
    file = "polScMCM_07.info";
    std::vector< Double_t> rListSc,polSc,polErSc,CESc,chiSqSc,edgeErSc,goodCycSc;
    fIn.open(file);
    if (fIn.is_open()) { 
      //while(fIn>>tRun>>tPol>>tPolEr>>chiSq>>NDF>>tCE>>tCEEr>>effStrip>>effStripEr>>pl>>nCyc && fIn.good()) {
      while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>fitStatus >>pl >>nCyc && fIn.good()) {
        if(std::find(rList.begin(), rList.end(), tRun)== rList.end()) continue; //ignore runs not in Ac list
        //cout<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<"\t"<<tChiSq/tNDF<<"\t"<<tCE<<"\t"<<tCEEr<<"\t"<<nCyc<<endl;
        //if (tRun < run1 || tRun > run2 || tPolEr>10 || tCEEr>6.0 || tCEEr<0.1 || tPolEr<0.1) {
        //  cout<<magenta<<"for runnum:"<<tRun<<", tPol:"<<tPol<<"+/-"<<tPolEr<<", CE:"<<tCE<<"+/-"<<tCEEr<<normal<<endl;
        //} else {
        rListSc.push_back((Double_t)tRun);
        polSc.push_back(fabs(tPol));
        polErSc.push_back(tPolEr);
        CESc.push_back(tCE);
        edgeErSc.push_back(tCEEr);
        chiSqSc.push_back(tChiSq/tNDF);
        goodCycSc.push_back(nCyc);
        //}
      }
      fIn.close();
    } else cout<<red<<"*** Could not open "<<file<<normal<<endl; 
    if(debug)cout<<blue<<rListSc.size()<<" analyzed runs in Scaler data"<<normal<<endl;

    std::vector<Double_t> rAcSc, rAcScEr; 
    for(Int_t r=0; r<(Int_t)rList.size(); r++) {
      if(rList[r] != rListSc[r]) cout<<rList[r]<<"\t"<<rListSc[r]<<endl;
      else {
        rAcSc.push_back(pol.at(r)/polSc.at(r));
        //rAcScEr.push_back(TMath::Sqrt(pow(pol.at(r)/polSc.at(r),2) + pow((pol.at(r)/(pow(polSc.at(r),2)))*polErSc.at(r), 2)));
        rAcScEr.push_back(polEr.at(r));
      }
      //rAcScEr.push_back(0.0);
    }   
    TCanvas *cRatio = new TCanvas("cRatio","cRatio",0,0,800,400);
    cRatio->SetGridx();
    TGraphErrors *grR = new TGraphErrors((Int_t)rList.size(), rList.data(), rAcSc.data(), 0, rAcScEr.data());
    grR->SetMarkerStyle(kOpenSquare);
    grR->SetMarkerColor(kBlue);
    grR->SetLineColor(kBlue);
    grR->Draw("AP");
    grR->GetXaxis()->SetTitle("run number");
    grR->GetYaxis()->SetTitle("ratio (Ac/Sc)");
    cRatio->SaveAs("RatioAcSc.png");

    TCanvas *cSc = new TCanvas("cSc","cSc",0,0,800,400);
    cSc->SetGridx();
    TGraphErrors *gr1 = new TGraphErrors((Int_t)rListSc.size(), rListSc.data(), polSc.data(), 0, polErSc.data());
    gr1->Draw("AP");
    cSc->SaveAs("scalerPlot.png");
    }


    if(bDiffAsym) {
      Int_t run = 25291;
      Int_t nStrips = 64;
      TCanvas *cDiffAsym = new TCanvas("cDiffAsym", "cDiffAsym",0,0,800,400);
      cDiffAsym->SetGridx();
      ///Read in all files that are needed
      Double_t strip[nStrips], asym[nStrips], asymEr[nStrips];
      file = Form("/w/hallc/compton/users/narayan/my_scratch/txt/r%d/edetLC_%d_AcExpAsymP1.txt",run,run);
      read3CfileArray(file, strip, asym, asymEr);

      Double_t stripRL[nStrips], asymRL[nStrips], asymRLEr[nStrips];
      file = Form("mcm_asym_%d.dat",run);
      read3CfileArray(file, stripRL, asymRL, asymRLEr);

      Double_t diff[nStrips], diffEr[nStrips];
      for(int s=0; s<nStrips-4; s++) {
        diff[s] = asymRL[s] - asym[s];
        diffEr[s] = asymEr[s]; 
      }

      TGraphErrors *grDiffAsym = new TGraphErrors(60, strip, diff, 0, diffEr);
      grDiffAsym->SetMarkerColor(kRed);
      grDiffAsym->Draw("AP");
      grDiffAsym->GetXaxis()->SetTitle("strip number");
      grDiffAsym->GetYaxis()->SetTitle("asym(RL - LC)");
      cDiffAsym->SaveAs(Form("DiffAsym_RL_LC_%d.png",run));
    }

    if(bPolHist) {
      if(debug) cout<<blue<<"Histogramming Absolute polarization"<<normal<<endl; 
      TCanvas *cPolHist = new TCanvas("cPolHist", "absolute polarization",100,400,500,500);
      hPol->GetXaxis()->SetTitle("pol MCM");
      hPol->Draw("H");
      //hPol->Fit("gaus");
      cPolHist->SaveAs(destDir+Form("absPolMCM.png"));
    }

    if(bPolPlot) {
      //coordinates in TCanvas are in order :x,y of left top corner;x,y of right bottom corner
      TCanvas *cMCM = new TCanvas("cMCM","Polarization trend",0,0,800,400);
      cMCM->SetGridx();
      cMCM->cd();
      TMultiGraph *grMCM = new TMultiGraph();
      TLegend *leg = new TLegend(0.15,0.82,0.32,0.94);

      ///using a fixed value for moller
      Double_t polM = 86.17;
      //Double_t polMStatEr = 0.14;
      //Double_t polMSystEr = 0.70;
      TLine *moller = new TLine(run1, polM, run2, polM);
      moller->SetLineColor(kRed-14);
      moller->SetLineWidth(2);

      /////using the multiple values for moller
      //TGraphErrors *grM = new TGraphErrors("mollerJosh_18Nov14.txt","%lg %lg %lg");///Moller data
      //grM->SetMarkerColor(kRed-14);
      //grM->SetLineColor(kRed-14);
      //grMCM->Add(grM);

      //TF1 *lFit2 = new TF1("lFit2", "pol0",run1,run2);
      //lFit2->SetLineColor(kRed-14);
      //grM->Fit(lFit2,"EM");
      //cMCM->Update();
      //TPaveStats *ps2 = (TPaveStats*)grM->GetListOfFunctions()->FindObject("stats");
      //ps2->SetX1NDC(0.65); ps2->SetX2NDC(0.85);
      //ps2->SetTextColor(kRed-14);
      //ps2->SetFillStyle(0);
      //cMCM->Modified();
      //leg->AddEntry(grM,"Moller","lpe");

      TGraphErrors *grC = new TGraphErrors((Int_t)pol.size(),rList.data(),pol.data(),0,polEr.data());///Compton data
      //TGraphErrors *grC = new TGraphErrors("polAcMCM_03.info", "%lg %lg %lg");
      grC->SetMarkerStyle(kOpenCircle);
      grC->SetMarkerColor(kBlue);
      grC->SetLineColor(kBlue);
      grMCM->Add(grC);

      grMCM->Draw("AP");
      grMCM->GetXaxis()->SetTitle("Run number");
      grMCM->GetYaxis()->SetTitle("polarization (%)");
      lFit->SetLineColor(kBlue);
      grC->Fit(lFit,"EM");
      cMCM->Update();
      TPaveStats *ps1 = (TPaveStats*)grC->GetListOfFunctions()->FindObject("stats");
      ps1->SetX1NDC(0.65); ps1->SetX2NDC(0.90);
      ps1->SetFillStyle(0);
      ps1->SetTextColor(kBlue);
      cMCM->Modified();
      moller->Draw();

      leg->AddEntry(grC,"Compton","lpe");
      leg->SetFillColor(0);
      leg->Draw();

      cMCM->SaveAs(destDir+Form("MCM_"+ver+".png"));
    }//if(bPolPlot)

    if(bDiff) {
      TCanvas *cDiff = new TCanvas("cDiff","diff runlet", 0,0,800,400);
      cDiff->SetGridx();
      TGraphErrors *grLC = new TGraphErrors("lasCycMCM01.info","%lg %lg %lg");
      grLC->SetMarkerColor(kBlue);
      TGraphErrors *grRL = new TGraphErrors("runletMCM.dat","%lg %lg %lg");
      grRL->SetMarkerColor(kRed);
      TMultiGraph *gr = new TMultiGraph();
      gr->Add(grLC);
      gr->Add(grRL);
      gr->Draw("AP");
      gr->GetXaxis()->SetTitle("run number");
      gr->GetYaxis()->SetTitle("polarization (%)");
      TLegend *leg = new TLegend(0.75,0.81,0.90,0.94);
      leg->AddEntry(grRL,"runlet","lpe");
      leg->AddEntry(grLC,"lasCyc","lpe");
      leg->SetFillStyle(0);
      leg->Draw();
      cDiff->SaveAs("RL_LC_MCM.png");
    }

    if(bPolEr) {
      TCanvas *cPolErr = new TCanvas("cPolErr","polarization error",10,10,500,500);
      hPolEr->SetLineColor(kBlue);
      //hPolEr->SetTitle(Form("error on Polarization for Qweak phase-1 data"));
      hPolEr->GetXaxis()->SetTitle("polarization error (statistical)");
      hPolEr->Draw("H");
      //hPolEr->Fit("gaus");

      cPolErr->SaveAs(Form(destDir + "polEr_%d_%d.png",run1,run2));
    }//if(bPolEr) 

    if(bChiSqr) {
      if(debug) cout<<blue<<"Histogramming ChiSqr"<<normal<<endl; 
      TCanvas *cChiSqr = new TCanvas("cChiSqrHist","chi-Sqr/ndf",10,10,500,500);
      hChiSq->SetLineColor(kBlue);
      hChiSq->GetXaxis()->SetTitle("ChiSq/ndf");
      hChiSq->Draw("H");
      //hChiSq->Fit("gaus");
      cChiSqr->SaveAs(Form(destDir + "chiSqrHist_%d_%d.png",run1,run2));   

      ///accross run2
      TCanvas *cChiSq2 = new TCanvas("cCqRun2","chi sq Vs run", 0,0,800,400);
      cChiSq2->SetGridx();
      TGraph *gr1 = new TGraph((Int_t)rList.size(), rList.data(), chiSq.data());
      gr1->SetMarkerStyle(kFullCircle);
      //gr1->SetMarkerSize(0.5);
      gr1->SetMarkerColor(kMagenta);
      gr1->GetYaxis()->SetTitle("chi Sq");
      gr1->GetXaxis()->SetTitle("run number");
      gr1->Draw("AP");
      cChiSq2->SaveAs(Form(ver+"_chiSq.png"));
    }//if(bChiSqrHist)

    if(bBgdFitInfo) {
      Bool_t bRes=1;
      Bool_t bBgd=1;
      cout<<blue<<"Background and residual fit for run2 "<<normal<<endl;
      TH1D *hBgd = new TH1D("bgd ", "bgd asym", runRange, 0, 0.1);
      TH1D *hChi = new TH1D("chiSqBgd ", "bgd fit ChiSq", runRange, 0, 0.1);
      TH1D *hRes = new TH1D("res ", "fit residue", runRange, 0, 0.1);
      hBgd->SetBit(TH1::kCanRebin);
      hChi->SetBit(TH1::kCanRebin);
      hRes->SetBit(TH1::kCanRebin);
      std::vector<Double_t> bgdRuns, bgdFit, bgdFitEr, residueFit, residueFitEr;
      std::vector<Double_t> chiSqFitBgd, chiSqFitRes; 

      //file = Form("%s/data/bgdMCM_10Oct14.info",pPath);
      //file = Form("bgdMCM_15Nov14.info");
      //file = Form("bgdMCM_16Nov14.info");
      //file = Form("bgdMCM_01.info");
      file = Form("bgdMCM_02.info");
      fIn.open(file);
      if (fIn.is_open()) {
        while(fIn >>tRun >>tRes >>tResEr >>tChiSq2 >>tNDF2 >>tBgd >>tBgdEr >>tChiSq >>tNDF && fIn.good()) {
          if(tRun==25284 || tRun==25292) continue;
          bgdRuns.push_back(tRun);
          residueFit.push_back(tRes), residueFitEr.push_back(tResEr), chiSqFitRes.push_back(tChiSq2/tNDF2);
          bgdFit.push_back(tBgd), bgdFitEr.push_back(tBgdEr),  chiSqFitBgd.push_back(tChiSq/tNDF);
          //cout<<bgdRuns.back()<<"\t"<<bgdFit.back()<<"\t"<<bgdFitEr.back()<<endl;
          hBgd->Fill(tBgd,tBgdEr);
          hChi->Fill(tChiSq);
          hRes->Fill(tRes);
        }
        fIn.close();
      } else cout<<"couldn't open "<<file<<endl;

      if(bBgd) {
        //TCanvas *cBgd = new TCanvas("cBgd","background fit for run2",200,400,1000,600);
        //cBgd->Divide(2,1);
        //cBgd->cd(1);
        //hBgd->Draw();
        //cBgd->cd(2);
        //hChi->Draw();
        TCanvas *cBgd2 = new TCanvas("cBgd2","background fit for run2",200,400,800,400);
        cBgd2->SetGridx();
        cBgd2->cd();
        TGraphErrors *grB = new TGraphErrors((Int_t)bgdRuns.size(), bgdRuns.data(), bgdFit.data(), 0, bgdFitEr.data());
        grB->SetMarkerStyle(kOpenSquare);
        grB->SetMarkerColor(kRed);
        grB->SetLineColor(kRed);
        grB->Draw("AP");
        lFit->SetLineColor(kRed);
        grB->Fit(lFit,"EM");
        cBgd2->SaveAs("bgdFitMCM_"+ver+".png");
      }
      if(bRes) {
        TCanvas *cRes = new TCanvas("cRes","fit residue",200,400,800,400);
        cRes->SetGridx();
        cRes->cd();
        TGraphErrors *grB = new TGraphErrors((Int_t)bgdRuns.size(), bgdRuns.data(), residueFit.data(), 0, residueFitEr.data());
        grB->SetMarkerStyle(kOpenSquare);
        grB->SetMarkerColor(kRed);
        grB->SetLineColor(kRed);
        grB->Draw("AP");
        lFit->SetLineColor(kRed);
        grB->Fit(lFit,"EM");
        cRes->SaveAs("resFitMCM_"+ver+".png");

      }
    }

    if(bCE) {
      //if(debug) cout<<blue<<"Compton edge analysis"<<normal<<endl; 
      TCanvas *c1 = new TCanvas("c1","Compton edge",100,100,800,400);
      c1->SetGridx();
      TGraphErrors *gr1 = new TGraphErrors((Int_t)rList.size(), rList.data(), CE.data(), 0, edgeEr.data());
      gr1->SetMarkerStyle(kFullSquare);
      gr1->SetMarkerSize(0.5);
      gr1->SetMarkerColor(kRed);
      gr1->GetXaxis()->SetTitle("Run number"); 
      gr1->GetYaxis()->SetTitle("Compton edge");
      gr1->GetYaxis()->SetDecimals(0);
      gr1->Draw("AP");
      c1->SaveAs(Form(ver+"_edgeEr.png"));
      //TGraphErrors *gr2 = new TGraphErrors((Int_t)rList.size(), rList.data(), roundedCE.data(), 0, edgeEr.data());
      //TGraphErrors *gr2 = new TGraphErrors((Int_t)rList.size(), rList.data(), diffCE.data(), 0, edgeEr.data());
      //TGraph *gr2 = new TGraph((Int_t)rList.size(), rList.data(), diffCE.data());
      //TGraph *gr2 = new TGraph((Int_t)rList.size(), edgeEr.data(), diffCE.data());

      ///correlation plot
      if(bPolCE) {
        TCanvas *c2 = new TCanvas("c2","correlation edgeEr polEr",100,100,500,500);
        c2->SetGrid();
        TGraph *gr2 = new TGraph((Int_t)rList.size(), edgeEr.data(), polEr.data());
        gr2->SetMarkerStyle(kFullSquare);
        gr2->SetMarkerColor(kBlue);
        gr2->GetXaxis()->SetTitle("edge error"); 
        gr2->GetYaxis()->SetTitle("pol error");
        gr2->Draw("AP");
        c2->SaveAs(Form("correlation_polEr_edgeEr.png"));
      }
    }
    if(bCycles) {
      //TCanvas *cCyc1 = new TCanvas("cCyc1","hist Cycles",10,10,500,500);
      //hCycles->GetXaxis()->SetTitle("Cycles");
      //hCycles->Draw("H");
      //cCyc1->SaveAs(Form("histCycles_%d_%d.png",run1,run2));

      TCanvas *cCyc = new TCanvas("cCycle2","Cycles Vs run", 0,0,800,400);
      cCyc->SetGridx();
      TGraph *gr1 = new TGraph((Int_t)rList.size(), rList.data(), goodCyc.data());
      gr1->SetMarkerStyle(kFullCircle);
      //gr1->SetMarkerSize(0.5);
      gr1->SetMarkerColor(kGreen);
      gr1->GetXaxis()->SetTitle("Run number");
      gr1->GetYaxis()->SetTitle("used time (s)");
      gr1->Draw("AP");
      cCyc->SaveAs(Form(ver+"_usedTime.png"));
      ///correlation plot
      TCanvas *cCycle3 = new TCanvas("cCycle3","cCycle3 correlation",10,10,500,500);
      cCycle3->cd();
      cCycle3->SetGrid();
      //TGraphErrors *gPC = new TGraphErrors((Int_t)absPol.size(), goodCyc.data(), absPol.data(), 0, polEr.data());
      TGraph *gPC = new TGraph((Int_t)goodCyc.size(), goodCyc.data(), polEr.data());
      gPC->SetMarkerStyle(kOpenCircle);
      gPC->SetMarkerSize(0.5);
      gPC->SetMarkerColor(kBlue);
      gPC->SetLineColor(kBlue);
      gPC->GetXaxis()->SetTitle("used time (s)");
      gPC->GetYaxis()->SetTitle("polarization error(%)");
      gPC->GetXaxis()->SetNdivisions(5);
      gPC->Draw("AP");
      cCycle3->SaveAs(Form("correlation_time_polEr.png"));
    }

    if(bBPM) {
      Bool_t bDiffX=1, bDiffY=1;
      file = "diffBPMYMCM_16Nov14.info";
      fIn.open(file);
      Double_t d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12;
      std::vector<Double_t> rListBpm, rms3p02aY, rms3p02bY, rms3p03aY, rms3c20Y;
      std::vector<Double_t> rms3p02aX, rms3p02bX, rms3p03aX, rms3c20X;
      if (fIn.is_open()) { 
        while(fIn >>d0 >>d1 >>d2 >>d3 >>d4 >>d5 >>d6 >>d7 >>d8 >>d9 >>d10 >>d11 >>d12 && fIn.good()) {
          if(d0<run1 || d1>run2) continue; ///the list needs only results between these runs
          //cout<<d0<<"  "<<d3<<"  "<<d6<<"  "<<d9<<"  "<<d12<<endl;
          rListBpm.push_back(d0);
          rms3p02aX.push_back(d3);
          rms3p02bX.push_back(d6);
          rms3p03aX.push_back(d9);
          rms3c20X.push_back(d12);
          rms3p02aY.push_back(d3);
          rms3p02bY.push_back(d6);
          rms3p03aY.push_back(d9);
          rms3c20Y.push_back(d12);
        }
        fIn.close();
      } else {
        cout<<red<<file<<" failed to open"<<normal<<endl;
        return -1;
      }
      cout<<blue<<rms3p02aY.size()<<" runs finally processed for this analysis from "<<file<<normal<<endl;

      ///accross run2
      if(bDiffX) {
        TCanvas *cBPMX = new TCanvas("cBPMX","BPMX Vs run", 0,0,800,400);
        cBPMX->SetGridx();
        TGraph *gr1 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3p02aX.data());
        //gr1->SetMarkerStyle(kFullCircle);
        gr1->SetMarkerColor(kRed);
        TGraph *gr2 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3p02bX.data());
        gr2->SetMarkerColor(kRed-8);
        TGraph *gr3 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3p03aX.data());
        gr3->SetMarkerColor(kRed-14);
        //TGraph *gr4 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3c20X.data());
        //gr4->SetMarkerColor(kRed+4);

        TMultiGraph *grX = new TMultiGraph();
        grX->Add(gr1);
        grX->Add(gr2);
        grX->Add(gr3);
        //grX->Add(gr4);
        grX->Draw("AP");
        grX->GetXaxis()->SetTitle("run number");
        grX->GetXaxis()->SetTitle("RMS BPM pos diffX");

        TLegend *legX = new TLegend(0.8,0.80,0.90,0.94);
        legX->AddEntry(gr1,"3p02aX","lpe");
        legX->AddEntry(gr2,"3p02bX","lpe");
        legX->AddEntry(gr3,"3p03aX","lpe");
        //legX->AddEntry(gr4,"3c20X","lpe");
        legX->SetFillStyle(0);//transparent
        legX->Draw();
        cBPMX->SaveAs(Form(ver+"bpmDiffX.png"));
      }
      if(bDiffY) {
        TCanvas *cBPMY = new TCanvas("cBPMY","BPMY Vs run", 0,0,800,400);
        cBPMY->SetGridx();
        TGraph *gr1 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3p02aY.data());
        //gr1->SetMarkerStyle(kFullCircle);
        gr1->SetMarkerColor(kRed);
        TGraph *gr2 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3p02bY.data());
        gr2->SetMarkerColor(kRed-8);
        TGraph *gr3 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3p03aY.data());
        gr3->SetMarkerColor(kRed-14);
        //TGraph *gr4 = new TGraph((Int_t)rListBpm.size(), rListBpm.data(), rms3c20Y.data());
        //gr4->SetMarkerColor(kRed+4);

        TMultiGraph *grY = new TMultiGraph();
        grY->Add(gr1);
        grY->Add(gr2);
        grY->Add(gr3);
        //grY->Add(gr4);
        grY->Draw("AP");
        grY->GetXaxis()->SetTitle("run number");
        grY->GetYaxis()->SetTitle("RMS BPM pos diffY");

        TLegend *legY = new TLegend(0.8,0.80,0.90,0.94);
        legY->AddEntry(gr1,"3p02aY","lpe");
        legY->AddEntry(gr2,"3p02bY","lpe");
        legY->AddEntry(gr3,"3p03aY","lpe");
        //legY->AddEntry(gr4,"3c20Y","lpe");
        legY->SetFillStyle(0);//transparent
        legY->Draw();
        cBPMY->SaveAs(Form(ver+"bpmDiffY.png"));
      }
    }
    return 1;
  }
