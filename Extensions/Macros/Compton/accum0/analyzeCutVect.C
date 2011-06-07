#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TCut.h"
#include "TCanvas.h"
#include <vector>
//#include "cutOnLaser.C"
#include "getRightmost.C"
#include "getLeftmost.C"
#include "TROOT.h"
#include "TCint.h"
#include <iostream>
#include <unistd.h>
////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the results.   //
//It also stores the results of each laser wise asymmetry and differences //
//in a file called "polstats.txt".                                        //
//The inputs are name of vecto of cuts, name of tree and cut number.      //
//laser-wise cut for run "20409.001" the arguments are(20409,1,0).        //
////////////////////////////////////////////////////////////////////////////

Int_t analyzeCutVect(vector<Int_t> &cut, TTree *myTree, Int_t k){
  //gROOT->Reset();
  const Int_t NCUTS = 100;//initialized size of TCut arrays.
  const Float_t LSCALE = 0.7;//used to scale laser power on yield graph.

  //File for storing histogram statistics.
  //  FILE * polstats = fopen("polstats.txt","a+");
  //Open the rootfile.
  //In case these still exist in memory.
  gDirectory->Delete("hbcm");
  gDirectory->Delete("hb");
  gDirectory->Delete("h1");
  gDirectory->Delete("h2");
  gDirectory->Delete("h3");
  gDirectory->Delete("h5");
  gDirectory->Delete("h6");
  gDirectory->Delete("ht");
  gDirectory->Delete("hDifferenceOn");
  gDirectory->Delete("hDifferenceOff");
  gDirectory->Delete("hAsymm");
  gDirectory->Delete("hTemp1");
  // TFile *file = TFile::Open(Form("/net/cdaq/cdaql5data/compton.old/rootfiles/Compton_Pass1_%d.00%d.root",runnum,num));
  // TTree *myTree = (TTree*)file->Get("Hel_Tree");

  //Define the desired cuts.
  Int_t scaleFactor = 10000; //~ratio of off and on periods to entries.
  //  Int_t count = (Int_t)(myTree->Draw("pattern_number","","goff")
  //                                     /(Double_t)scaleFactor);
  //  std::vector<Int_t> cut;
  Int_t nCuts =(Int_t)cut.size();
  printf("nCuts=%d\n",nCuts);
 
  if(nCuts>2*NCUTS){
    printf("Number of cuts exceeds program limits.");
    printf("Need to increase NCUTS value in accum0.C.\n");
    return 0;
  }
  Double_t hBinX = myTree->GetMaximum("yield_sca_laser_PowT");
  Double_t lBinX = myTree->GetMinimum("yield_sca_laser_PowT");
  Double_t diffX = hBinX - lBinX;
  hBinX += diffX*0.1;
  lBinX -= diffX*0.1;

  TH1F *hTemp1 = new TH1F("hTemp1","hTemp1",1000,lBinX,hBinX);
  myTree->Draw("yield_sca_laser_PowT>>hTemp1","yield_sca_laser_PowT>10","goff");
  Float_t cutval1 = hTemp1->GetMean()*0.9, cutval2 = hTemp1->GetMean()*1.1;
  cout<<cutval1<<"\t"<<cutval2<<endl;

  Float_t maxCur = myTree->GetMaximum("yield_bcm2_3h05a");
  TCut bcmCut = Form("yield_bcm2_3h05a>(%f*0.8)",maxCur);
  TCut cut1, cut2, cut3, cut4;
  TCut lasOnCut = bcmCut && Form("yield_sca_laser_PowT>%f",cutval1)
                         && Form("yield_sca_laser_PowT<%f",cutval2);
  //  cout<<bcmCut<<"\tboohoo3"<<endl;
  TCut lasOffCut = bcmCut && "yield_sca_laser_PowT<=1.0";//all unlocked times
  TCut lasOFFCut = "";//only laser OFF periods i.e. when flipper is up
  TCut lasWiseCut[NCUTS];
  TCut bkgCut[NCUTS];
  TCut asymCut[NCUTS];

  for(Int_t i=0; i<nCuts;i++)
    printf("cut.at[%d]=%d\n",i, cut.at(i));

  for(Int_t i=0; i*2<nCuts-2; i++){
    cut1 = (Form("Entry$>=%d",cut.at(2*i)));
    cut2 = (Form("Entry$<=%d",cut.at(2*i+1)));
    cut3 = (Form("Entry$>=%d",cut.at(2*i+2)));
    cut4 = (Form("Entry$<=%d",cut.at(2*i+3))); 
    lasWiseCut[i] = cut1 && cut4;
    bkgCut[i] = lasOffCut && lasWiseCut[i];
    asymCut[i] = lasOnCut && lasWiseCut[i];
    if (i==0)
      lasOFFCut = (cut1 && cut2) || (cut3 && cut4);
    else lasOFFCut = lasOFFCut ||(cut3 && cut4);
  }
  //Create a canvas for plots.
  TCanvas *c1 = new TCanvas("c1","c1",0,0,1000,1000);
  c1->Divide(2,3);
  c1->cd(1);
  //Draw Current vs Pattern Number
  hBinX = myTree->GetMaximum("pattern_number");
  lBinX = myTree->GetMinimum("pattern_number");
  diffX = hBinX - lBinX;
  hBinX += diffX*0.1;
  lBinX -= diffX*0.1;

  Double_t hBinY = myTree->GetMaximum("yield_bcm2_3h05a");
  Double_t lBinY = myTree->GetMinimum("yield_bcm2_3h05a");
  Double_t diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  //Draw Accumulator 0 Yield.
  TH2F *hbcm = new TH2F("hbcm", "hbcm", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  hbcm->GetXaxis()->SetTitle("Pattern Number");
  hbcm->GetYaxis()->SetTitle("BCM Yield");
  hbcm->SetMarkerColor(kOrange+7);
  hbcm->SetTitle("BCM Yield vs Pattern Number  (bcm2_3h05a)");
  myTree->Draw("yield_bcm2_3h05a:pattern_number>>hbcm");
 
 //Draw Laser On and Off Accumulator Yield vs Pattern Number
  c1->cd(2);
  myTree->Draw("yield_fadc_compton_accum0.hw_sum");
  hBinY = getRightmost(htemp);
  lBinY = getLeftmost(htemp);
  Float_t maxPowT = myTree->GetMaximum("yield_sca_laser_PowT");
  //cout<<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<endl;
  diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  //  cout<<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<endl;
  Float_t scale = LSCALE*(Float_t)(hBinY-lBinY)/(Float_t)maxPowT;
  // printf("maxPowT=%f\nscale=%f\n",maxPowT,scale);
  TH2F *hb = new TH2F("hb", "hb", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *h1 = new TH2F("h1", "h1", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *h2 = new TH2F("h2", "h2", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *ht = new TH2F("ht", "ht", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  c1->cd(3);
  myTree->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hb",
                scale,lBinY+diffY*0.05));
  myTree->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>h1",lasOnCut,"goff");
  h1->SetMarkerColor(kGreen);
  h1->GetXaxis()->SetTitle("Pattern Number");
  h1->GetYaxis()->SetTitle("Accumulator Values");
  h1->SetTitle("Accumulator vs Pattern # (Red=Scaled Laser Power, Green=On, Blue=Off)");
  myTree->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>h2",lasOffCut,"goff");
  //myTree->Draw("yield_fadc_compton_accum0.hw_sum+2e5:pattern_number>>ht",lasOFFCut && bcmCut,"goff");
  TH2F *h5 = new TH2F("h5", "h5", 1000, 0, 2.2e5, 1000, 0, 10);
  TH2F *h6 = new TH2F("h6", "h6", 1000, 0, 2.2e5, 1000, 0, 10);

  myTree->Draw("yield_sca_laser_PowT:pattern_number>>h5",lasOffCut,"goff");
  myTree->Draw("yield_sca_laser_PowT+0.1:pattern_number>>h6",lasOFFCut && bcmCut,"goff");
  h2->SetMarkerColor(kBlue);
  h1->Draw();
  h2->Draw("same");
  hb->SetMarkerColor(kRed);
  hb->Draw("same"); 
  /*
  TH2F *h5 = new TH2F("h5", "h5", 1000, 0, 2.2e5, 1000, 0, 10);
  TH2F *h6 = new TH2F("h6", "h6", 1000, 0, 2.2e5, 1000, 0, 10);

  myTree->Draw("yield_sca_laser_PowT:pattern_number>>h5",lasOffCut,"goff");
  myTree->Draw("yield_sca_laser_PowT+0.1:pattern_number>>h6",lasOFFCut && bcmCut,"goff");
  TCanvas *c2 = new TCanvas("c2","c2",0,0,600,360);
  h5->SetMarkerColor(kBlue);
  h6->SetMarkerColor(kRed);
  h6->Draw();
  h5->Draw("same");
  */


  //Draw Background Subtracted Differences for Laser On and Off and
  //asymmetries for Laser On.

  TH1F *hAsymm = new TH1F("hAsymm","hAsymm",400,-2,2);
  std::vector<Float_t> rmsOn, rmsOff, rmsAsymm;
  std::vector<Float_t> meanOn, meanOff, meanAsymm;
  Float_t bkg;
  std::vector<Int_t> entriesAsymm, entriesOn, entriesOff;
  std::vector<Float_t> weightsOn, weightsOff, weightsAsymm;
  Int_t wghtAsymm = 0, wghtOff = 0, wghtOn = 0;
  Int_t sumEntriesAsymm = 0, sumEntriesOff = 0, sumEntriesOn = 0;
  // fprintf(polstats,"Run %d.%d\nmeanAsymm\trmsAsymm\tmeanOn\t
  //                  rmsOn\tmeanOff\trmsOff\n",runnum,num);
  c1->cd(4);

  Hel_Tree->Draw("asym_fadc_compton_accum0.hw_sum
                  *yield_fadc_compton_accum0.hw_sum*2>>hDiffOn",	 
		  asymCut[k],"goff");
  hDiffOn->GetXaxis()->SetTitle("Accumulator Differences");
  hDiffOn->SetTitle("Laser On Accumulator Differences (2*Yield*Asymmetry)");
  hDiffOn->Draw();
  c1->cd(5);

  Hel_Tree->Draw("asym_fadc_compton_accum0.hw_sum
                  *yield_fadc_compton_accum0.hw_sum*2>>hDiffOff",
		 bkgCut[k],"goff");
  hDiffOff->GetXaxis()->SetTitle("Accumulator Differences");
  hDiffOff->SetTitle("Laser Off Accumulator Differences (2*Yield*Asymmetry)");
  hDiffOff->Draw();
  Hel_Tree->Draw("yield_fadc_compton_accum0.hw_sum",bkgCut[k],"goff");
  bkg = htemp->GetMean();
  c1->cd(6);

  Hel_Tree->Draw(Form("asym_fadc_compton_accum0.hw_sum
                       *yield_fadc_compton_accum0.hw_sum
                       /(yield_fadc_compton_accum0.hw_sum-%f)>>hAsymm",
		      bkg),asymCut[k],"goff");
  hAsymm->GetXaxis()->SetTitle("Accumulator Asymmetry");
  hAsymm->SetTitle("Accumulator Asymmetry (Yield*Asymmetry)/(Yield-Background)");
  hAsymm->Draw();
  /*
  for(Int_t i=0; i*2<nCuts; i++){

    //Reserve histograms[0] for final summed result.
    Hel_Tree->Draw(Form("asym_fadc_compton_accum0.hw_sum
                         *yield_fadc_compton_accum0.hw_sum*2",i),
		   asymCut[i],"goff");
    Hel_Tree->Draw(Form("asym_fadc_compton_accum0.hw_sum
                         *yield_fadc_compton_accum0.hw_sum*2",i),
		   bkgCut[i],"goff");

    if(i==0) hDiffOn->htemp->Clone();
    else hDiffOn->Add(htemp);
    if(i==0) hDiffOff->Clone(htemp);
    else hDiffOff->Add(htemp);
    Hel_Tree->Draw("yield_fadc_compton_accum0.hw_sum",bkgCut[i],"goff");
    bkg = htemp->GetMean();
    Hel_Tree->Draw(Form("asym_fadc_compton_accum0.hw_sum
                         *yield_fadc_compton_accum0.hw_sum
                         /(yield_fadc_compton_accum0.hw_sum-%f)",
			 background[i],i),asymCut,"goff");
    if(i==0) hAsymm->Clone(htemp);
    else hAsymm->Add(htemp);
    rmsAsymm.push_back(hAsymm.back()->GetRMS());
    printf("rmsAsymm[%d] = %f\n",rmsAsymm.back());
    rmsOn.push_back(hDiffOn.back()->GetRMS());
    printf("rmsOn[%d] = %f\n",rmsOn.back());
    rmsOff.push_back(hDiffOff.back()->GetRMS());
    printf("rmsOff[%d] = %f\n",rmsOff.back());
    meanAsymm.push_back(hAsymm.back()->GetMean());
    printf("meanAsymm[%d] = %f\n",meanAsymm.back());
    meanOn.push_back(hDiffOn.back()->GetMean());
    printf("meanOn[%d] = %f\n",meanOn.back());
    meanOff.push_back(hDiffOff.back()->GetMean());
    printf("meanOff[%d] = %f\n",meanOff.back());
    fprintf(polstats,"%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\n",
	    meanAsymm.back(),rmsAsymm.back(),meanOn.back(),rmsOn.back(),
	    meanOff.back(),rmsOff.back());
    //Make weights so that the histograms can be added for a single plot.
    wghtAsymm += hAssym[i]->GetEntries()/(rmsAsymm[i]*rmsAsymm[i]);
    wghtOff += hDiffOff[i]->GetEntries()/(rmsOff[i]*rmsOff[i]);
    wghtOn += hDiffOn[i]->GetEntries()/(rmsOn[i]*rmsOn[i]);
    entriesAssym[i] = hAssym->GetEntries();
    entriesOn[i] = hDiffOn->GetEntries();
    entriesOff[i] = hDiffOff->GetEntries();
    sumEntriesAssym += hAssym->GetEntries();
    sumEntriesOn += hDiffOn->GetEntries();
    sumEntriesOff += hDiffOff->GetEntries();
  }

  for(Int_t i=0; i*2<nCuts; i++){
    weightsAsymm[i] = sumEntriesAsymm/(rmsAsymm[i]*rmsAsymm[i]*wgtAsymm);
    weightsOn[i] = sumEntriesOn/(rmsOn[i]*rmsOn[i]*wgtOn);
    weightsOf[i] = sumEntriesOff/(rmsOff[i]*rmsOff[i]*wgtOff);
    hAsymmetry->Add(hAsymm[i],weightsAsymm[i]);
    hDifferenceOn->Add(hAsymm[i],weightsAsymm[i]);
    hDifferenceOff->Add(hAsymm[i],weightsAsymm[i]);
  }
  c1->cd(4);

  hDifferenceOn->GetXaxis()->SetTitle("Accumulator Differences");
  hDifferenceOn->SetTitle("Laser On Accumulator Differences (2*Yield*Asymmetry)");
  hDifferenceOn->Draw();

  c1->cd(5);
  hDifferenceOff->GetXaxis()->SetTitle("Accumulator Differences");
  hDifferenceOff->SetTitle("Laser Off Accumulator Differences (2*Yield*Asymmetry)");
  hDifferenceOff->Draw();

  c1->cd(6);
  hAsymmetry->GetXaxis()->SetTitle("Accumulator Asymmetry");
  hAsymmetry->SetTitle("Accumulator Asymmetry (Yield*Asymmetry)/(Yield-Background)");
  hAsymmetry->Draw();
  cut.clear();
  */
  return 1;
}

