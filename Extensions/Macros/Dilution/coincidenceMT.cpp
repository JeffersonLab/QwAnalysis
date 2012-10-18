//This script correctly calculations the TS and MD meantimes and then
//patches them together. Next version will also marry them correctly
//based on hit times.
//Written by Josh Magee, thanks to just about everybody else.
//magee@jlab.org
//April 26, 2012

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <QwEvent.h>
#include <QwHit.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TChain.h>


class Tsmd {
  public:
    Tsmd( Int_t, Int_t, Int_t, Int_t, Bool_t , Bool_t , Bool_t , Bool_t , Bool_t , Bool_t, Int_t);
    void setArrays( void );
    void processEvent( TFile * );
//    void processEvent( TChain * );
    void processHits( Int_t , QwEvent * );
    void fillHistos( void );
    void createHistos( void );
    void plotHistos( void );
    void highHitCheck( void );
    static const Int_t nhits=7;
//    void setSubTitle( void );

  private:
    Bool_t    fTsSoftwareFlag;
    Bool_t    fMdSoftwareFlag;
    Bool_t    fCorrelationFlag;
    Bool_t    fTsMinusMdFlag;
    Bool_t    fMdWithCutsFlag;
    Bool_t    fOverlayFlag;
    Bool_t    fSaveCanvas;
    Int_t     mdarray[nhits];
    Int_t     tsarray[nhits];
    Int_t     fEventLow;
    Int_t     fEventHigh;
    Int_t     fMainDet;
    Int_t     fTrigScint;
    Int_t     fRunNum;
    TPaveText *fSubTitle;
    TH2F      *correlateHisto[nhits];
    TH1F      *tsMinusMdHisto[nhits];
    TH1F      *tsSoftwareMT;
    TH1F      *mdSoftwareMT;
    TH1F      *mdCuts;
    TH1F      *mdCutsPeak;
    TH1F      *mdCutsAccidental;
}; //end class definition

void coincidencePlot( Int_t runNum, 
    Int_t mainDet=1,
    Int_t trigScint=2,
    Int_t eventLow=0,
    Int_t eventHigh=4000000,
    Bool_t Overlay=kTRUE, //kTRUE
    Bool_t TSmeantime=kTRUE, 
    Bool_t MDmeantime=kFALSE, 
    Bool_t Correlation=kFALSE,
    Bool_t TsMdTimeDifference=kFALSE,
    Bool_t CoincidenceCuts=kTRUE //kTRUE
    ) {
  gROOT -> Reset();
  gROOT -> SetStyle("Modern");

//  TString filename = Form("QweakNew_%i.root",runNum);
    TString filename = Form("Qweak_%i.root",runNum);
//  TString filename = Form("QweakMarch11_%i.root",runNum);
//  TString filename = Form("QweakMarchGas11_%i.root",runNum);

  TFile *myFile = new TFile(filename);
  if ( !myFile->IsOpen() ) {
    std::cout <<"Error opening ROOTFILE " << myFile->GetName() <<std::endl;
    return;
  }
  else {
    std::cout <<"Successfully opened ROOTFILE " <<myFile->GetName() <<std::endl;
  }

//  TChain *chain = new TChain("event_tree");
//  chain->Add(Form("%i*.root",runNum));
  // Tsmd billy( kFALSE, kFALSE, kFALSE, kFALSE, kFALSE);
  Tsmd billy( mainDet, trigScint, eventLow, eventHigh, Overlay, TSmeantime, MDmeantime, Correlation, TsMdTimeDifference, CoincidenceCuts, runNum);
  billy.processEvent( myFile );
//  billy.processEvent( chain );
  billy.plotHistos();
  
} //end function


Tsmd::Tsmd( Int_t mainDet, Int_t trigScint, Int_t eventLow, Int_t eventHigh, Bool_t overlay, Bool_t tsSoft, Bool_t mdSoftFlag, Bool_t correlFlag, Bool_t tsMinusMd, Bool_t mdCutsF, Int_t runNum) {
  fMainDet = mainDet;
  fEventLow = eventLow;
  fEventHigh = eventHigh;
  fTrigScint = trigScint;
  fOverlayFlag = overlay;
  fTsSoftwareFlag = tsSoft;
  fMdSoftwareFlag = mdSoftFlag;
  fCorrelationFlag = correlFlag;
  fTsMinusMdFlag = tsMinusMd;
  fMdWithCutsFlag = mdCutsF;
  fRunNum = runNum;
  fSaveCanvas = kTRUE;
//  fSaveCanvas = kFALSE;

//  setSubTitle();
  createHistos();
} // end tsmd constructor

/*void Tsmd::setSubTitle( void ) {
  fSubTitle = new TPaveText(.005,55,.95,60);
//  fSubTitle->SetX1NDC(0.1);
//  fSubTitle->SetX2NDC(0.7);
//  fSubTitle->SetY1NDC(0.6);
//  fSubTitle->SetY2NDC(0.7);
//  fSubTitle = new TPaveText(-600,100000,975,365000);
  fSubTitle->UseCurrentStyle();
  fSubTitle->AddText(Form("Run %i Octant %i",fRunNum,fMainDet));
} //end setSubTitle
*/
void Tsmd::createHistos( void ) {
  //need to add Bool_tean logic

  if (fCorrelationFlag) {
    for (Int_t n=0; n<7; n++) {
      correlateHisto[n] = new TH2F(Form("correlateHisto[%i]",n), Form("Hit #%i",n), 1600,-300,1300,1600,-300,1300);
    }
  } //end fCorrelationFlag

  if (fTsMinusMdFlag) {
    for (Int_t n=0; n<7; n++) {
      tsMinusMdHisto[n] = new TH1F(Form("tsMinusMdHisto[%i]",n), Form("Hit #%i",n), 51,-19.5,30.5);
    }
  } // end fTsMinusMdFlag

  if (fTsSoftwareFlag) tsSoftwareMT = new TH1F("tsSoftwareMT","TS Software Meantime",1800,-400,1400);
  if (fMdSoftwareFlag || fOverlayFlag) mdSoftwareMT = new TH1F("mdSoftwareMT","MD Software Meantime",1800,-400,1400);
  if (fMdWithCutsFlag || fOverlayFlag) {
    mdCuts = new TH1F("mdCuts","Main Detector MT with TS Cut",1800,-400,1400);
    mdCutsPeak = new TH1F("mdCutsPeak","Main Detector MT with TS Cut",1800,-400,1400);
    mdCutsAccidental = new TH1F("mdCutsAccidental","Main Detector MT with TS Cut",1800,-400,1400);
  }
} //end definition createhistos

void Tsmd::setArrays ( void ) {
  for (Int_t i=0; i<7; i++ ) {
    mdarray[i]=0;
    tsarray[i]=0;
  }
} //end constructor definition

void Tsmd::processEvent( TFile *file ) {
//void Tsmd::processEvent( TChain *chain ) {

  TTree*   event_tree  = (TTree*) file ->Get("event_tree");
  TBranch* ev_branch   = event_tree->GetBranch("events");
//  TBranch* ev_branch   = chain->GetBranch("events");
  QwEvent* qwevent     = 0;

  Long_t num_entries = event_tree->GetEntries();
//  Long_t num_entries = chain->GetEntries();
  Long_t localEntry  = 0;

  ev_branch -> SetAddress(&qwevent);


  for (Long_t i=0; i<num_entries; i++)
    {//;
      if (!(i>=fEventLow && i<=fEventHigh)) {continue;}
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
//      localEntry = chain->LoadTree(i);
//      chain->GetEntry(localEntry);

      if(i % 1000 == 0) {
printf(" Total events %ld events process so far : %ld\n", num_entries, i);
      }

      Int_t nhit = qwevent->GetNumberOfHits();

      setArrays();
      processHits( nhit, qwevent);
//      highHitCheck();
   }//; end loop through num_entries
  std::cout <<"Finishing processing events and filling histograms." <<std::endl;
}// end processEvents

void Tsmd::processHits( Int_t nhit, QwEvent * qwevent ) {
  for (Int_t hit=0; hit<nhit; hit++){
    const QwHit* qwhit = qwevent->GetHit(hit);
    const Int_t fregion = qwhit -> GetRegion();
    const Int_t felement = qwhit -> GetElement();
    const Int_t fplane = qwhit -> GetPlane();
    const Int_t hitnumber = qwhit -> GetHitNumber();

    if (fregion==4 && fplane==fTrigScint && felement==3)  {
      //   tsarray[ts_hitnum] = qwhit -> GetTimeNs();
      tsarray[hitnumber] = qwhit -> GetTimeNs();
      //	    std::cout <<"Event: " <<i <<"\tTS hit#: " <<ts_hitnum <<endl;
    }
    if (fregion==5 && fplane==fMainDet && felement==3) {
      //   mdarray[md_hitnum] = qwhit -> GetTimeNs();
      mdarray[hitnumber] = qwhit -> GetTimeNs();
      //   std::cout <<"Event: " <<i <<"\tMD hit#: " <<md_hitnum <<endl;
    }
  } //end loop through hits

  fillHistos();
}//end processHits function

void Tsmd::fillHistos() {
  if (fTsSoftwareFlag) {
    for (Int_t k=0; k<7; k++) {
      if(tsarray[k]!=0) tsSoftwareMT->Fill(tsarray[k]);
    }
  }

  if (fMdSoftwareFlag || fOverlayFlag) {
    for (Int_t k=0; k<7; k++) {
      if(mdarray[k]!=0) mdSoftwareMT->Fill(mdarray[k]);
    }
  }

  if (fMdWithCutsFlag || fOverlayFlag) {
    for (Int_t k=0; k<7; k++) {
//Run 1
//      if(tsarray[k]!=0 && tsarray[k]>-186 && tsarray[k]<-178 && mdarray[k]!=0) mdCuts->Fill(mdarray[k]);
//      if(tsarray[k]!=0 && tsarray[k]>-186 && tsarray[k]<-178 && mdarray[k]!=0 && mdarray[k]>=-200 && mdarray[k]<=-160) mdCutsPeak->Fill(mdarray[k]);
//      if(tsarray[k]!=0 && tsarray[k]>-186 && tsarray[k]<-178 && mdarray[k]!=0 && mdarray[k]>=5 && mdarray[k]<=45) mdCutsAccidental->Fill(mdarray[k]);
//Run 1 N->Delta
      if(tsarray[k]!=0 && tsarray[k]>-190 && tsarray[k]<-178 && mdarray[k]!=0) mdCuts->Fill(mdarray[k]);
      if(tsarray[k]!=0 && tsarray[k]>-190 && tsarray[k]<-178 && mdarray[k]!=0 && mdarray[k]>=-210 && mdarray[k]<=-150) mdCutsPeak->Fill(mdarray[k]);
      if(tsarray[k]!=0 && tsarray[k]>-190 && tsarray[k]<-178 && mdarray[k]!=0 && mdarray[k]>=5 && mdarray[k]<=65) mdCutsAccidental->Fill(mdarray[k]);
//run 2
//      if(tsarray[k]!=0 && tsarray[k]>-190 && tsarray[k]<-178 && mdarray[k]!=0) mdCuts->Fill(mdarray[k]);
//      if(tsarray[k]!=0 && tsarray[k]>-190 && tsarray[k]<-178 && mdarray[k]!=0 && mdarray[k]>=-220 && mdarray[k]<=-160) mdCutsPeak->Fill(mdarray[k]);
//      if(tsarray[k]!=0 && tsarray[k]>-190 && tsarray[k]<-178 && mdarray[k]!=0 && mdarray[k]>=-40 && mdarray[k]<=20) mdCutsAccidental->Fill(mdarray[k]);
    }
  }

  if (fCorrelationFlag) {
    for (Int_t k=0; k<7; k++) {
      if(tsarray[k]!=0 && mdarray[k]!=0) correlateHisto[k]->Fill(tsarray[k],mdarray[k]);
    }
  }

  if (fTsMinusMdFlag) {
    for (Int_t k=0; k<7; k++) {
      if(tsarray[k]!=0 && mdarray[k]!=0) tsMinusMdHisto[k]->Fill(tsarray[k]-mdarray[k]);
    }
  }
} //end definition fillHistos

void Tsmd::highHitCheck( void ) {
  for (Int_t n=4; n<=7; n++) {
    if (tsarray[n]!=0) std::cout <<"The TS has " << n <<" hits: make sure to plot them!\n";
    if (mdarray[n]!=0) std::cout <<"The MD has " << n <<" hits: make sure to plot them!\n";
  }
} //end definition highHitCheck

void Tsmd::plotHistos( void ) {
  std::cout <<"Producing plots now..." <<std::endl;

  if (fTsSoftwareFlag) {
    TCanvas *tsSoftwareCanvas = new TCanvas("tsSoftwareCanvas","title");
    tsSoftwareMT->SetTitle(Form("Run %i Trigger Scintillator Software Meantime",fRunNum));
    tsSoftwareMT->GetXaxis()->SetTitle("Time (ns)");
    gPad->SetLogy();
    tsSoftwareMT->Draw();
    if (fSaveCanvas) tsSoftwareCanvas->Print(Form("rootfile_plots/run%i/trigscint_software_meantime_%i.png",fRunNum,fMainDet));
//    fSubTitle->Draw();
//    gPad->Update();
  } //end of fTsSoftware plots

  if (fOverlayFlag) {
    TCanvas *overlay = new TCanvas("overlay","Overlay plot");
    mdSoftwareMT->SetTitle(Form("Run %i Main Detector Software Meantime", fRunNum));
    mdSoftwareMT->GetXaxis()->SetTitle("Time (ns)");
    gPad->SetLogy();
    mdSoftwareMT->SetLineColor(2);
    mdSoftwareMT->Draw();

    mdCuts->SetLineColor(4);
    mdCuts->Draw("sames");
    gPad->Update();

    TPaveStats *st1 = (TPaveStats*) mdSoftwareMT->GetListOfFunctions()->FindObject("stats");
    st1->SetTextColor(2);
    st1->SetX1NDC(0.78);
    st1->SetX2NDC(0.98);
    st1->SetY1NDC(0.78);
    st1->SetY2NDC(0.94);

    TPaveStats *st2 = (TPaveStats*) mdCuts->GetListOfFunctions()->FindObject("stats");
    st2->SetTextColor(4);
    st2->SetX1NDC(0.78);
    st2->SetX2NDC(0.98);
    st2->SetY1NDC(0.62);
    st2->SetY2NDC(0.77);

    if (fSaveCanvas) overlay->Print(Form("rootfile_plots/run%i/overlay_%i.png",fRunNum,fMainDet));
  } //end fOverlay section

  if (fMdSoftwareFlag) {
    TCanvas *mdSoftwareCanvas = new TCanvas("mdSoftwareCanvas","title");
    mdSoftwareMT->SetTitle(Form("Run %i Main Detector Software Meantime",fRunNum));
    mdSoftwareMT->GetXaxis()->SetTitle("Time (ns)");
    gPad->SetLogy();
    mdSoftwareMT->SetLineColor(2);
    mdSoftwareMT->Draw();
    if (fSaveCanvas) mdSoftwareCanvas->Print(Form("rootfile_plots/run%i/md_software_meantime_%i.png",fRunNum, fMainDet));
  } //end of fMdSoftware plots

  if (fMdWithCutsFlag) {
    TCanvas *mdCutsCanvas = new TCanvas("mdCutsCanvas","title");
    mdCuts->SetTitle(Form("Run %i Main Detector Meantime with TS Coincidence Condition",fRunNum));
    mdCuts->GetXaxis()->SetTitle("Time (ns)");
    gPad->SetLogy();
    mdCuts->SetLineColor(4);
    mdCuts->Draw("");
    if (fSaveCanvas) mdCutsCanvas->Print(Form("rootfile_plots/run%i/cuts_%i.png",fRunNum,fMainDet));

    TCanvas *peak = new TCanvas("peak","Main Detector Peak");
    mdCutsPeak->SetTitle(Form("Run %i Main Detector Peak",fRunNum));
    mdCutsPeak->GetXaxis()->SetTitle("Time (ns)");
    gPad->SetLogy();
    mdCutsPeak->SetLineColor(2);
    mdCutsPeak->Draw();
    if (fSaveCanvas) peak->Print(Form("rootfile_plots/run%i/peak_%i.png",fRunNum, fMainDet));

    TCanvas *accident = new TCanvas("accident","Main Detector Accidentals");
    mdCutsAccidental->SetTitle(Form("Run %i Main Detector Accidentals",fRunNum));
    mdCutsAccidental->GetXaxis()->SetTitle("Time (ns)");
    gPad->SetLogy();
    mdCutsAccidental->SetLineColor(2);
    mdCutsAccidental->Draw();
    if (fSaveCanvas) accident->Print(Form("rootfile_plots/run%i/accidentals_%i.png",fRunNum,fMainDet));
   } //end fMdWithCutsFlag

  if (fCorrelationFlag) {
    TCanvas* canvases[3];
    for (Int_t n=1; n<3; n++) {
      canvases[n-1] = new TCanvas(Form("c%i",n),"title");
      correlateHisto[n-1] -> SetTitle(Form("Run %i TS meantime vs. MD meantime for Hit #%i",fRunNum,n));
      correlateHisto[n-1] -> GetXaxis() -> SetTitle("TS Meantime (ns)");
      correlateHisto[n-1] -> GetYaxis() -> SetTitle("MD Meantime (ns)");
      correlateHisto[n-1] -> SetStats(kFALSE);
      correlateHisto[n-1] -> Draw("colz");
      //    correlateHisto[n] -> Draw("prof");
    }
    /*  TCanvas *k1 = new TCanvas("k1","title");
        TProfile *pr = (TProfile *) correlateHisto[1] -> ProfileY("profile",-300,1300);
        pr -> Draw();

        TCanvas *k1 = new TCanvas("k1","title");
        TProfile *ps = (TProfile *) correlateHisto[1] -> ProfileX("profile",-300,1300);
        ps -> Draw();
        */
  } //end correlation plots

  if (fTsMinusMdFlag) {
    TCanvas* minusCanvases[3];
    for (Int_t n=1; n<3; n++) {
      minusCanvases[n-1] = new TCanvas(Form("d%i",n),"title");
      tsMinusMdHisto[n-1] -> SetTitle(Form("Run %i TS meantime -  MD meantime for Hit #%i",fRunNum,n));
      tsMinusMdHisto[n-1] -> GetXaxis() -> SetTitle("Time (ns)");
      tsMinusMdHisto[n-1] -> SetLineColor(4);
      tsMinusMdHisto[n-1] -> SetFillColor(9);
      tsMinusMdHisto[n-1] -> Draw();
    }
  }//end fTsMinusMd plots
} //end definition printtHistos


