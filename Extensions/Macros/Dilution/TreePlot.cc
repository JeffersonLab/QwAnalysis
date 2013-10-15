/*
 * JoshTreeLib.cpp
 * Author: Josh Magee <magee@jlab.org>
 * Date:   2013-07-17
 *
 * Small program to walk QwDB rootfile trees for plotting purposes
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <exception>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TLine.h>
#include <TArrow.h>
#include <TPaveLabel.h>
#include <TApplication.h>

#include "PlotLib.h"
#include "TreeLib.h"

using namespace std;

/*Functions found in this macro
 * void plotByRunlet( TString,TString )
 * void histoLeaf( TString filename, TString dataname="asym_qwk_charge")
 * void printSlugInfo( TString filename, TString dataname="asym_qwk_charge", TString outfile="outfile")
 * void printGoodRunlets( TString filename)
 * void findBadRunlets( TString filename)
 * void plotOneWien( TString filename, int wien, TString dataname="diff_bcmdd78" )
 * void plotOneWienRMS( TString filename, int wien, TString dataname="diff_bcmdd78" )
 * void plotYieldByWien( TString filename, TString dataname="diff_bcmdd78" )
 * void plotByWien( TString filename, TString dataname="diff_bcmdd78" )
 * void plotByWien_friend( TString filename, TString dataname="diff_bcmdd78" )
 * plotDetectorArray_friend( TString filename, TString dataname="diff_bcmdd78", TString friendname="DS-4\%-Aluminum_offoff_tree.root", int wien=6 ) {
 * plotUSlumi_friend( TString filename, TString friendname="DS-4\%-Aluminum_offoff_tree.root", int wien=6 ) {
 * void plotByWienRMS( TString filename, TString dataname="diff_bcmdd78" )
 * void histoByWien( TString filename, TString dataname="diff_bcmdd78" )
 * void pullByWien( TString filename, TString dataname="diff_bcmdd78" )
 * void plotHcbdByWien_friend( TString filename, TString friendname, int wien )
 */

void plotByRunlet( TString filename , TString dataname="diff_qwk_charge") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  std::vector<double> value;
  std::vector<double> error;
  std::vector<double> runlet;

  TCanvas *canvas = new TCanvas("canvas","title",1200,900);
  TMultiGraph *multi = new TMultiGraph("multi","");

  get_data_from_tree_runlet(tree,dataname,&value,&error,&runlet);
//  get_data_from_tree_runlet_decimal(tree,dataname,&value,&error,&runlet);

  TGraphErrors *plot = new TGraphErrors(runlet.size(),runlet.data(),value.data(),0,error.data());

  float size=1;
  bluePlot(plot,size);

  multi->Add(plot);

  canvas->cd();
  multi->Draw("ap");

  multi->SetTitle("Charge Asymmetry in DS-4\% Aluminum (run 2)");
  multi->GetYaxis()->SetTitle("Charge Asymmetry (ppm)");
  multi->GetXaxis()->SetTitle("Run Number (arb.)");
  multi->GetYaxis()->SetTitleOffset(1.3);
  multi->SetMinimum(-21);
  multi->SetMaximum(21);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

//  canvas->Print("mygraph.png");

  TF1 *fit = new TF1("fit","pol0");
  fit->SetLineColor(46);
  fit->SetLineStyle(1);
  plot->Fit("fit","sames");

  gPad->Modified();
  gPad->Update();

  TPaveStats *stat1 = (TPaveStats*) plot->GetListOfFunctions()->FindObject("stats");
  setup_stats(stat1,0.82,0.71,0.98,0.84);
  stat1->SetTextColor(9);
  stat1->Draw("sames");

  gPad->Modified();
  gPad->Update();

  float ymin = multi->GetYaxis()->GetXmin();
  float ymax = multi->GetYaxis()->GetXmax();
//  float xmin = multi->GetXaxis()->GetXmin();
  float xmax = multi->GetXaxis()->GetXmax();

  get_wien_starts(tree);
  //Wien values (a,b)= (begin,end)
  float hArr=ymin+1;;      //arrow height
  float tsize=1.5;
  float txtLow = hArr+tsize/2;
  float txtHigh = txtLow+tsize/2;
  float txtSize = 2;
  //wien lows/highs in x-direction
  int w8a = 283;
  int w8b = 736;
  int w9a = 737;
  int w9b = 1449;

  //Two-pass
  TLine *lineTP = new TLine();
  VerticalLine(lineTP,w8a,ymin,ymax);

  TPaveLabel *twoPass = new TPaveLabel(1,txtLow,w8a-60,txtHigh,"Two Pass","");
  twoPass->SetBorderSize(0);
  twoPass->SetFillColor(0);
  twoPass->SetTextSize(txtSize);
  twoPass->Draw();

  //Wien 8
  TLine *line8 = new TLine();
  VerticalLine(line8,w8b,ymin,ymax);

  TArrow *arr8 = new TArrow();
  HorizontalArrow(arr8,w8a,w8b,hArr);

  TPaveLabel *lab8 = new TPaveLabel(w8a,txtLow,w8b,txtHigh,"Wien 8","");
  lab8->SetBorderSize(0);
  lab8->SetFillColor(0);
  lab8->SetTextSize(txtSize);
  lab8->Draw();

  //Wien 9
  TLine *line9 = new TLine();
  VerticalLine(line9,w9b,ymin,ymax);

  TArrow *arr9 = new TArrow();
  HorizontalArrow(arr9,w9a,w9b,hArr);

  TPaveLabel *lab9 = new TPaveLabel(w9a,txtLow,w9b,txtHigh,"Wien 9","");
  lab9->SetBorderSize(0);
  lab9->SetFillColor(0);
  lab9->SetTextSize(txtSize);
  lab9->Draw();

  //Wien 10
  TPaveLabel *lab10 = new TPaveLabel(w9b,txtLow,xmax,txtHigh,"Wien 10","");
  lab10->SetBorderSize(0);
  lab10->SetFillColor(0);
  lab10->SetTextSize(txtSize);
  lab10->Draw();

  gPad->Modified();
  gPad->Update();

} //end plotChargeAsm function

void histoLeaf( TString filename, TString dataname="asym_qwk_charge") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

//  bool debug=false;

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  std::vector<int>    runlet;
  std::vector<double> error;

  TCanvas *canvas = new TCanvas("canvas","title",1200,900);

  get_data_from_tree(tree,dataname,&error,&runlet);
//    histo_data_from_tree(tree,histo,dataname);

  std::vector<double> count;
  for (size_t i=0; i<error.size(); i++) {
    count.push_back(i);
  }
  std::vector<double> runletDouble( runlet.begin(), runlet.end() );
//  printf("%i \n",error.size());
  TH1D *histo = new TH1D("histo","title",count.size()-1,count.data());

  for (size_t i=0; i<error.size(); i++) {
    histo->SetBinContent(i,error.at(i));
    printf("%i \t%f\n",int(i),error.at(i));
  }
  canvas->cd();
  histo->Draw();


} //end function histoError

void printSlugInfo( TString filename, TString dataname="asym_qwk_charge", TString outfile="outfile") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  printInfo(tree,dataname,outfile);

}

void findBadRunlets( TString filename, TString dataname = "diff_qwk_charge") {
  gROOT->Reset();

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");
  TString outfile_name = "badruns.dat";
//  find_bad_runlets(tree,dataname);
  find_bad_runlets(tree,dataname,outfile_name);
}

void findConditionalRunlets( TString filename, TString dataname = "diff_qwk_charge", float limit = 130.0 ) {
  gROOT->Reset();

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");
  find_conditional_runlets(tree,dataname,limit);
}


void printGoodRunlets( TString filename, TString dataname = "diff_qwk_charge") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");
  TString outfile_name = "goodruns.dat";
  print_good_runlets(tree,dataname,outfile_name);
}


void plotOneWien( TString filename, int wien=6, TString dataname="diff_bcmdd78" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  std::vector<double> wien_runlet;
  std::vector<double> wien_val;
  std::vector<double> wien_err;

//  get_data_by_wien(wien,tree,dataname,&wien_runlet,&wien_val,&wien_err);
  get_data_by_wien_decimal(wien,tree,dataname,&wien_runlet,&wien_val,&wien_err);

  if (int(wien_runlet.size()) == 0) {
    std::cout <<"No data\nExiting\n.";
    exit(0);
  }

  //put data into TGraphErrors
  TGraphErrors *graph = new TGraphErrors(wien_runlet.size(),wien_runlet.data(),wien_val.data(),0,wien_err.data());

  //float size=0.05;
  float size=0.5;
  bluePlot(graph,size);

  TString title  = "Sign Corrected Charge Asymmetry (Run 2 LH2)";
//  TString title  = "Charge Asymmetry Width (Run 2 LH2)";
  TString xtitle = "Run Number Decimal";
  TString ytitle = "Charge Asymmetry (ppm)";

  TCanvas *canvas = new TCanvas("canvas","title",1400,800);

  canvas->cd();
  graph->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,graph);
  placeLabel(Form("Wien %i",wien),0.4,0.73,0.51,0.88);
  TF1 *fit = new TF1("fit","pol0");
  fitGraphWithStats(graph,fit,0.85,0.80,0.99,0.95);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of plotOneWien function


void plotOneWienRMS( TString filename, int wien=6, TString dataname="diff_bcmdd78" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  std::vector<double> wien_runlet;
  std::vector<double> wien_err;

  get_rms_by_wien(wien,tree,dataname,&wien_runlet,&wien_err);

  if (int(wien_runlet.size()) == 0) {
    std::cout <<"No data\nExiting\n.";
    exit(0);
  }

  //put data into TGraphErrors
  TGraphErrors *graph = new TGraphErrors(wien_runlet.size(),wien_runlet.data(),wien_err.data(),0,0);

  //float size=0.05;
  float size=0.5;
  bluePlot(graph,size);

  //TString title  = "Sign Corrected Charge Asymmetry";
  TString title  = "Charge Asymmetry Width";
  TString xtitle = "Run Number Decimal";
  TString ytitle = "Charge Asymmetry Width (ppm)";

  TCanvas *canvas = new TCanvas("canvas","title",1400,800);

  canvas->cd();
  graph->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,graph);
  placeLabel(Form("Wien %i",wien),0.4,0.73,0.51,0.88);
  TF1 *fit = new TF1("fit","pol0");
  fitGraphWithStats(graph,fit,0.85,0.80,0.99,0.95);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of plotOneWien function

void plotYieldByWien( TString filename, TString dataname="diff_bcmdd78" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  bool debug=false;

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  std::vector<double> wien6_runlet;
  std::vector<double> wien6_val;
  std::vector<double> wien6_err;

  std::vector<double> wien7_runlet;
  std::vector<double> wien7_val;
  std::vector<double> wien7_err;

  std::vector<double> wien8_runlet;
  std::vector<double> wien8_val;
  std::vector<double> wien8_err;

  std::vector<double> wien9_runlet;
  std::vector<double> wien9_val;
  std::vector<double> wien9_err;

  std::vector<double> wien10_runlet;
  std::vector<double> wien10_val;
  std::vector<double> wien10_err;

  get_yield_by_wien_decimal(6,tree,dataname,&wien6_runlet,&wien6_val,&wien6_err);
  get_yield_by_wien_decimal(7,tree,dataname,&wien7_runlet,&wien7_val,&wien7_err);
  get_yield_by_wien_decimal(8,tree,dataname,&wien8_runlet,&wien8_val,&wien8_err);
  get_yield_by_wien_decimal(9,tree,dataname,&wien9_runlet,&wien9_val,&wien9_err);
  get_yield_by_wien_decimal(10,tree,dataname,&wien10_runlet,&wien10_val,&wien10_err);

  check_size(&wien6_runlet,6);
  check_size(&wien7_runlet,7);
  check_size(&wien8_runlet,8);
  check_size(&wien9_runlet,9);
  check_size(&wien10_runlet,10);

  if (int(wien6_runlet.size()) == 0) {
    std::cout <<"wien 6 has no data\nExiting\n.";
    exit(0);
  }

  if (debug) {
    printf("Runlet size: %i",int(wien6_runlet.size()));
    for(size_t j=0; j<wien6_runlet.size(); j++) {
      double runlet = wien6_runlet[j];
      double value = wien6_val[j];
      double error = wien6_err[j];
      printf("%f \t%f \t%f\n",runlet,value,error);
    }
  }

  //put data into TGraphErrors
  TGraphErrors *w6 = new TGraphErrors(wien6_runlet.size(),wien6_runlet.data(),wien6_val.data(),0,wien6_err.data());
  TGraphErrors *w7 = new TGraphErrors(wien7_runlet.size(),wien7_runlet.data(),wien7_val.data(),0,wien7_err.data());
  TGraphErrors *w8 = new TGraphErrors(wien8_runlet.size(),wien8_runlet.data(),wien8_val.data(),0,wien8_err.data());
  TGraphErrors *w9 = new TGraphErrors(wien9_runlet.size(),wien9_runlet.data(),wien9_val.data(),0,wien9_err.data());
  TGraphErrors *w10 = new TGraphErrors(wien10_runlet.size(),wien10_runlet.data(),wien10_val.data(),0,wien10_err.data());

  //float size=0.05;
  float size=0.5;
  bluePlot(w6,size);
  bluePlot(w7,size);
  bluePlot(w8,size);
  bluePlot(w9,size);
  bluePlot(w10,size);

/*  TString title  = "Sign Corrected BCM78 DD Asymmetry";
  TString xtitle = "Runlet_id";
  TString ytitle = "BCM78 DD Asymmetry (ppm)";

  TString title  = "Sign Corrected Charge Asymmetry";
  TString xtitle = "Run Number Decimal";
  TString ytitle = "Charge Asymmetry (ppm)";
*/
  TString title  = "LH2 Current vs. Run number (Run 2)";
  TString xtitle = "Run number";
  TString ytitle = "Current (#muA)";

  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);

  canvas->cd(1);
  w6->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","pol0");
  fitGraphWithStats(w6,fit6,0.85,0.66,0.99,0.99);

  title = "Wien 7";
  canvas->cd(2);
  w7->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w7);
  TF1 *fit7 = new TF1("fit7","pol0");
  fitGraphWithStats(w7,fit7,0.85,0.66,0.99,0.99);

  title = "Wien 8";
  canvas->cd(3);
  w8->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w8);
  TF1 *fit8 = new TF1("fit8","pol0");
  fitGraphWithStats(w8,fit8,0.85,0.66,0.99,0.99);

  title = "Wien 9";
  canvas->cd(4);
  w9->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w9);
  TF1 *fit9 = new TF1("fit9","pol0");
  fitGraphWithStats(w9,fit9,0.85,0.66,0.99,0.99);

  title = "Wien 10";
  canvas->cd(5);
  w10->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w10);
  TF1 *fit10 = new TF1("fit10","pol0");
  fitGraphWithStats(w10,fit10,0.85,0.66,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of plotByWien function


void plotByWien( TString filename, TString dataname="diff_bcmdd78") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  bool debug=false;

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;
  TTree *tree = (TTree*) file->Get("tree");

  std::vector<double> wien6_runlet;
  std::vector<double> wien6_val;
  std::vector<double> wien6_err;

  std::vector<double> wien7_runlet;
  std::vector<double> wien7_val;
  std::vector<double> wien7_err;

  std::vector<double> wien8_runlet;
  std::vector<double> wien8_val;
  std::vector<double> wien8_err;

  std::vector<double> wien9_runlet;
  std::vector<double> wien9_val;
  std::vector<double> wien9_err;

  std::vector<double> wien10_runlet;
  std::vector<double> wien10_val;
  std::vector<double> wien10_err;

  get_data_by_wien_decimal(6,tree,dataname,&wien6_runlet,&wien6_val,&wien6_err);
  get_data_by_wien_decimal(7,tree,dataname,&wien7_runlet,&wien7_val,&wien7_err);
  get_data_by_wien_decimal(8,tree,dataname,&wien8_runlet,&wien8_val,&wien8_err);
  get_data_by_wien_decimal(9,tree,dataname,&wien9_runlet,&wien9_val,&wien9_err);
  get_data_by_wien_decimal(10,tree,dataname,&wien10_runlet,&wien10_val,&wien10_err);

  check_size(&wien6_runlet,6);
  check_size(&wien7_runlet,7);
  check_size(&wien8_runlet,8);
  check_size(&wien9_runlet,9);
  check_size(&wien10_runlet,10);

  if (int(wien6_runlet.size()) == 0) {
    std::cout <<"wien 6 has no data\nExiting\n.";
    exit(0);
  }

  if (debug) {
    printf("Runlet size: %i",int(wien6_runlet.size()));
    for(size_t j=0; j<wien6_runlet.size(); j++) {
      double runlet = wien6_runlet[j];
      double value = wien6_val[j];
      double error = wien6_err[j];
      printf("%f \t%f \t%f\n",runlet,value,error);
    }
  }

  //put data into TGraphErrors
  TGraphErrors *w6 = new TGraphErrors(wien6_runlet.size(),wien6_runlet.data(),wien6_val.data(),0,wien6_err.data());
  TGraphErrors *w7 = new TGraphErrors(wien7_runlet.size(),wien7_runlet.data(),wien7_val.data(),0,wien7_err.data());
  TGraphErrors *w8 = new TGraphErrors(wien8_runlet.size(),wien8_runlet.data(),wien8_val.data(),0,wien8_err.data());
  TGraphErrors *w9 = new TGraphErrors(wien9_runlet.size(),wien9_runlet.data(),wien9_val.data(),0,wien9_err.data());
  TGraphErrors *w10 = new TGraphErrors(wien10_runlet.size(),wien10_runlet.data(),wien10_val.data(),0,wien10_err.data());

  //float size=0.05;
  float size=0.5;
  bluePlot(w6,size);
  bluePlot(w7,size);
  bluePlot(w8,size);
  bluePlot(w9,size);
  bluePlot(w10,size);

  // TString title  = "Sign Corrected BCM78 DD Asymmetry";
  // TString xtitle = "Runlet_id";
  // TString ytitle = "BCM78 DD Asymmetry (ppm)";

  // TString title  = "Sign Corrected Charge Asymmetry (LH2, Run2)";
  // TString xtitle = "Run Number Decimal";
  // TString ytitle = "Charge Asymmetry (ppm)";

  TString title  = "";
  TString xtitle = "";
  TString ytitle = "";

  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);

  canvas->cd(1);
  w6->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","pol0");
  fitGraphWithStats(w6,fit6,0.85,0.66,0.99,0.99);

  title = "Wien 7";
  canvas->cd(2);
  w7->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w7);
  TF1 *fit7 = new TF1("fit7","pol0");
  fitGraphWithStats(w7,fit7,0.85,0.66,0.99,0.99);

  title = "Wien 8";
  canvas->cd(3);
  w8->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w8);
  TF1 *fit8 = new TF1("fit8","pol0");
  fitGraphWithStats(w8,fit8,0.85,0.66,0.99,0.99);

  title = "Wien 9";
  canvas->cd(4);
  w9->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w9);
  TF1 *fit9 = new TF1("fit9","pol0");
  fitGraphWithStats(w9,fit9,0.85,0.66,0.99,0.99);

  title = "Wien 10";
  canvas->cd(5);
  w10->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w10);
  TF1 *fit10 = new TF1("fit10","pol0");
  fitGraphWithStats(w10,fit10,0.85,0.66,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of plotByWien function


void plotByWien_friend( TString filename, TString dataname="diff_bcmdd78", TString friendname="DS-4\%-Aluminum_offoff_tree.root" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  bool debug=false;

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TFile *file2 = new TFile(friendname);
  if ( !file2->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file2 <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<friendname <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");
  TTree *friendtree = (TTree*) file2->Get("tree");

  std::vector<double> wien6_runlet;
  std::vector<double> wien6_val;
  std::vector<double> wien6_err;

  std::vector<double> wien7_runlet;
  std::vector<double> wien7_val;
  std::vector<double> wien7_err;

  std::vector<double> wien8_runlet;
  std::vector<double> wien8_val;
  std::vector<double> wien8_err;

  std::vector<double> wien9_runlet;
  std::vector<double> wien9_val;
  std::vector<double> wien9_err;

  std::vector<double> wien10_runlet;
  std::vector<double> wien10_val;
  std::vector<double> wien10_err;

  get_data_wien_decimal_friend(6,tree,friendtree,dataname,&wien6_runlet,&wien6_val,&wien6_err);
  get_data_wien_decimal_friend(7,tree,friendtree,dataname,&wien7_runlet,&wien7_val,&wien7_err);
  get_data_wien_decimal_friend(8,tree,friendtree,dataname,&wien8_runlet,&wien8_val,&wien8_err);
  get_data_wien_decimal_friend(9,tree,friendtree,dataname,&wien9_runlet,&wien9_val,&wien9_err);
  get_data_wien_decimal_friend(10,tree,friendtree,dataname,&wien10_runlet,&wien10_val,&wien10_err);

  check_size(&wien6_runlet,6);
  check_size(&wien7_runlet,7);
  check_size(&wien8_runlet,8);
  check_size(&wien9_runlet,9);
  check_size(&wien10_runlet,10);

  if (int(wien6_runlet.size()) == 0) {
    std::cout <<"wien 6 has no data\nExiting\n.";
    exit(0);
  }

  if (debug) {
    printf("Runlet size: %i",int(wien6_runlet.size()));
    for(size_t j=0; j<wien6_runlet.size(); j++) {
      double runlet = wien6_runlet[j];
      double value = wien6_val[j];
      double error = wien6_err[j];
      printf("%f \t%f \t%f\n",runlet,value,error);
    }
  }

  //put data into TGraphErrors
  TGraphErrors *w6 = new TGraphErrors(wien6_runlet.size(),wien6_runlet.data(),wien6_val.data(),0,wien6_err.data());
  TGraphErrors *w7 = new TGraphErrors(wien7_runlet.size(),wien7_runlet.data(),wien7_val.data(),0,wien7_err.data());
  TGraphErrors *w8 = new TGraphErrors(wien8_runlet.size(),wien8_runlet.data(),wien8_val.data(),0,wien8_err.data());
  TGraphErrors *w9 = new TGraphErrors(wien9_runlet.size(),wien9_runlet.data(),wien9_val.data(),0,wien9_err.data());
  TGraphErrors *w10 = new TGraphErrors(wien10_runlet.size(),wien10_runlet.data(),wien10_val.data(),0,wien10_err.data());

  //float size=0.05;
  float size=0.5;
  bluePlot(w6,size);
  bluePlot(w7,size);
  bluePlot(w8,size);
  bluePlot(w9,size);
  bluePlot(w10,size);

  TString title  = "Sign Corrected BCM78 DD Asymmetry (Current>130#muA)";
  TString xtitle = "Runlet_id";
  TString ytitle = "BCM78 DD Asymmetry (ppm)";

/*  TString title  = "Sign Corrected Charge Asymmetry (LH2, Run2)";
  TString xtitle = "Run Number Decimal";
  TString ytitle = "Charge Asymmetry (ppm)";
*/
  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);

  canvas->cd(1);
  w6->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","pol0");
  fitGraphWithStats(w6,fit6,0.85,0.66,0.99,0.99);

  title = "Wien 7";
  canvas->cd(2);
  w7->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w7);
  TF1 *fit7 = new TF1("fit7","pol0");
  fitGraphWithStats(w7,fit7,0.85,0.66,0.99,0.99);

  title = "Wien 8";
  canvas->cd(3);
  w8->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w8);
  TF1 *fit8 = new TF1("fit8","pol0");
  fitGraphWithStats(w8,fit8,0.85,0.66,0.99,0.99);

  title = "Wien 9";
  canvas->cd(4);
  w9->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w9);
  TF1 *fit9 = new TF1("fit9","pol0");
  fitGraphWithStats(w9,fit9,0.85,0.66,0.99,0.99);

  title = "Wien 10";
  canvas->cd(5);
  w10->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w10);
  TF1 *fit10 = new TF1("fit10","pol0");
  fitGraphWithStats(w10,fit10,0.85,0.66,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of plotByWien function


void plotDetectorArray_friend( TString filename, TString dataname="diff_bcmdd78", TString friendname="DS-4\%-Aluminum_offoff_tree.root", int wien=6 ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TFile *file2 = new TFile(friendname);
  if ( !file2->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file2 <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<friendname <<".\n" <<endl;

  TTree *tree1 = (TTree*) file->Get("tree");
  TTree *tree2 = (TTree*) file2->Get("tree");

/*  const int number = 9;
  const TString mdall [number] = {
    "md1barsum", "md2barsum", "md3barsum",
    "md4barsum", "md5barsum", "md6barsum",
    "md7barsum", "md8barsum", "mdallbars"
  };
*/
  const int number = 8;
  const TString mdall [number] = {
    "dslumi1", "dslumi2", "dslumi3",
    "dslumi4", "dslumi5", "dslumi6",
    "dslumi7", "dslumi8"
  };

  const Int_t canvasOrder [number] = {
//    4, 1, 2, 3, 6, 9, 8, 7, 5
    4, 1, 2, 3, 6, 9, 8, 7//, 5
  };

  int n_events1 = (int) tree1->GetEntries();
  int n_events2 = (int) tree2->GetEntries();

  if (n_events1 !=n_events2) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  double det[number][4];
  std::vector<double> runlet;
  std::vector<double> detvec[number];

  int sign, wien_slug;
  double run_number_decimal;
  tree1->ResetBranchAddresses();
  tree1->SetBranchAddress("wien_slug",&wien_slug);
  tree1->SetBranchAddress("sign_correction",&sign);
  tree1->SetBranchAddress("run_number_decimal",&run_number_decimal);

  for (int j=0; j<number; j++) {
    tree1->SetBranchAddress(Form("yield_qwk_%s",mdall[j].Data()),&det[j]);
  }

/*  tree1->SetBranchAddress("yield_qwk_md1barsum",&det1);
  tree1->SetBranchAddress("yield_qwk_md2barsum",&det2);
  tree1->SetBranchAddress("yield_qwk_md3barsum",&det3);
  tree1->SetBranchAddress("yield_qwk_md4barsum",&det4);
  tree1->SetBranchAddress("yield_qwk_md5barsum",&det5);
  tree1->SetBranchAddress("yield_qwk_md6barsum",&det6);
  tree1->SetBranchAddress("yield_qwk_md7barsum",&det7);
  tree1->SetBranchAddress("yield_qwk_md8barsum",&det8);
  tree1->SetBranchAddress("yield_qwk_mdallbarsum",&detall);
*/
  double current[4], run_decimal_friend;
  tree2->ResetBranchAddresses();
  tree2->SetBranchAddress("yield_qwk_charge",&current);
  tree2->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  for (int i=0; i<n_events1; i++) {
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    if (run_number_decimal != run_decimal_friend)
      printf("Something's screwed up! Your two trees are not aligned!\n");
    if ( det[0][0]== -1e6 || wien_slug != wien)
      continue;
    if (current[0]<130)
      continue;

    runlet.push_back(run_number_decimal);
    for (int j=0; j<number; j++) {
      detvec[j].push_back(det[j][0]);
    }

  } //end for loop over events

  //put data into TGraphErrors
  //float size=0.05;
  float size=0.20;
  TGraphErrors *graphs[number];
  for (int j=0; j<number; j++) {
    graphs[j] = new TGraphErrors(runlet.size(),runlet.data(),detvec[j].data(),0,0);
    redPlot(graphs[j],size);
  }

  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(3,3);

  for (int j=0; j<number; j++) {
    canvas->cd(canvasOrder[j]);

    //TString title  = Form("LH2 md%ibarsum yield",j+1);
    TString title  = Form("LH2 dslumi%i yield",j+1);
    TString xtitle = "Runlet_id";
    TString ytitle = "Yield (V/#muA)";
    if (j==2) {
      TString text = Form(" Wien %i",wien);
      title += text;
    }
    if (j==8) {
      //title = "mdAllBars yield";
      title = "dslumi yields";
    }
    graphs[j]->Draw("ap");
    placeAxis(title,xtitle,ytitle,canvas,graphs[j]);
//    graphs[j]->SetMinimum(0.026);
//    graphs[j]->SetMaximum(0.045);

    gPad->SetGrid();
    gPad->Modified();
    gPad->Update();

  } //end canvas plotting loop

/*   placeLabel("Wien 6",0.4,0.73,0.51,0.88);
      TF1 *fit6 = new TF1("fit6","pol0");
      fitGraphWithStats(w6,fit6,0.85,0.66,0.99,0.99);
      */


} //end of plotByWien function



void plotUSlumi_friend( TString filename, TString friendname="DS-4\%-Aluminum_offoff_tree.root", int wien=6 ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TFile *file2 = new TFile(friendname);
  if ( !file2->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file2 <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<friendname <<".\n" <<endl;

  TTree *tree1 = (TTree*) file->Get("tree");
  TTree *tree2 = (TTree*) file2->Get("tree");

  const int number = 4;
  const TString posArray [number] = {
    "1pos", "3pos", "5pos", "7pos"
  };

  const TString negArray [number] = {
    "1neg", "3neg", "5neg", "7neg"
  };

  const Int_t canvasOrder [number] = {
//    4, 1, 2, 3, 6, 9, 8, 7, 5
    4, 2, 6, 8
  };

  const Int_t octant [number] = {
    1, 3, 5, 7
  };

  int n_events1 = (int) tree1->GetEntries();
  int n_events2 = (int) tree2->GetEntries();

  if (n_events1 !=n_events2) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  double pos[number][4];
  double neg[number][4];
  std::vector<double> runlet;
  std::vector<double> posvec[number];
  std::vector<double> negvec[number];

  int sign, wien_slug;
  double run_number_decimal;
  tree1->ResetBranchAddresses();
  tree1->SetBranchAddress("wien_slug",&wien_slug);
  tree1->SetBranchAddress("sign_correction",&sign);
  tree1->SetBranchAddress("run_number_decimal",&run_number_decimal);

  for (int j=0; j<number; j++) {
    tree1->SetBranchAddress(Form("yield_qwk_uslumi%s",posArray[j].Data()),&pos[j]);
    tree1->SetBranchAddress(Form("yield_qwk_uslumi%s",negArray[j].Data()),&neg[j]);
  }

  double current[4], run_decimal_friend;
  tree2->ResetBranchAddresses();
  tree2->SetBranchAddress("yield_qwk_charge",&current);
  tree2->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  printf("Walking through tree...\n");
  for (int i=0; i<n_events1; i++) {
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    if (run_number_decimal != run_decimal_friend)
      printf("Something's screwed up! Your two trees are not aligned!\n");
    if ( pos[0][0]== -1e6 || neg[0][0]==-1e6 ||wien_slug != wien)
      continue;
    if (current[0]<130)
      continue;

    runlet.push_back(run_number_decimal);
    for (int j=0; j<number; j++) {
      posvec[j].push_back(pos[j][0]);
      negvec[j].push_back(neg[j][0]);
    }

  } //end for loop over events
  printf("Done walking through tree...\n");

  //put data into TGraphErrors
  //float size=0.05;
  float size=0.20;
  TMultiGraph *multi[number];
  TGraphErrors *posgraphs[number], *neggraphs[number];
  for (int j=0; j<number; j++) {
    multi[j] = new TMultiGraph();
    posgraphs[j] = new TGraphErrors(runlet.size(),runlet.data(),posvec[j].data(),0,0);
    neggraphs[j] = new TGraphErrors(runlet.size(),runlet.data(),negvec[j].data(),0,0);
    redPlot(posgraphs[j],size);
    bluePlot(neggraphs[j],size);
    if (multi[j]) {
      multi[j]->Add(posgraphs[j]);
      multi[j]->Add(neggraphs[j]);
    } else {
      printf("MG # %i DOES NOT EXIST...\n",j);
    }
    //  printf("Adding DS to MG %i...\n",j);
  }

  printf("Added graphs...\n");
  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(3,3);

  for (int j=0; j<number; j++) {
    canvas->cd(canvasOrder[j]);

    TString title  = Form("Octant %i",octant[j]);
    TString xtitle = "Runlet_id";
    TString ytitle = "Yield (V/#muA)";

    //    posgraphs[j]->Draw("ap");
    multi[j]->Draw("ap");
    placeAxis(title,xtitle,ytitle,canvas,multi[j]);
//    graphs[j]->SetMinimum(0.026);
//    graphs[j]->SetMaximum(0.045);

    gPad->SetGrid();
    gPad->Modified();
    gPad->Update();

  } //end canvas plotting loop

  canvas->cd(5);
  placeLabel(Form("Wien %i",wien),0.4,0.73,0.51,0.88);
  placeLabelRed("red: uslumi pos",0.4,.55,0.51,0.70);
  placeLabelBlue("blue: uslumi neg",0.4,.38,0.51,0.53);

} //end of plotByWien function




void plotByWienRMS( TString filename, TString dataname="diff_bcmdd78" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  std::vector<double> wien6_runlet;
  std::vector<double> wien6_err;

  std::vector<double> wien8_runlet;
  std::vector<double> wien8_err;

  std::vector<double> wien9_runlet;
  std::vector<double> wien9_err;

  std::vector<double> wien10_runlet;
  std::vector<double> wien10_err;

  get_rms_by_wien(6,tree,dataname,&wien6_runlet,&wien6_err);
  get_rms_by_wien(8,tree,dataname,&wien8_runlet,&wien8_err);
  get_rms_by_wien(9,tree,dataname,&wien9_runlet,&wien9_err);
  get_rms_by_wien(10,tree,dataname,&wien10_runlet,&wien10_err);

  check_size(&wien6_runlet,6);
  check_size(&wien8_runlet,8);
  check_size(&wien9_runlet,9);
  check_size(&wien10_runlet,10);

  //put data into TGraphErrors
  TGraphErrors *w6 = new TGraphErrors(wien6_runlet.size(),wien6_runlet.data(),wien6_err.data(),0,0);
  TGraphErrors *w8 = new TGraphErrors(wien8_runlet.size(),wien8_runlet.data(),wien8_err.data(),0,0);
  TGraphErrors *w9 = new TGraphErrors(wien9_runlet.size(),wien9_runlet.data(),wien9_err.data(),0,0);
  TGraphErrors *w10 = new TGraphErrors(wien10_runlet.size(),wien10_runlet.data(),wien10_err.data(),0,0);

  //float size=0.05;
  float size=0.5;
  bluePlot(w6,size);
  bluePlot(w8,size);
  bluePlot(w9,size);
  bluePlot(w10,size);

/*  TString title  = "BCM78 DD Asymmetry Width";
  TString xtitle = "Runlet_id";
  TString ytitle = "BCM78 DD Width (ppm)";
*/
  TString title  = "Charge Asymmetry Width";
  TString xtitle = "Runlet_id";
  TString ytitle = "Charge Asymmetry RMS (ppm)";

  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,4);

  canvas->cd(1);
  w6->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","pol0");
  fitGraphWithStats(w6,fit6,0.85,0.66,0.99,0.99);

  title = "Wien 8";
  canvas->cd(2);
  w8->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w8);
  TF1 *fit8 = new TF1("fit8","pol0");
  fitGraphWithStats(w8,fit8,0.85,0.66,0.99,0.99);

  title = "Wien 9";
  canvas->cd(3);
  w9->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w9);
  TF1 *fit9 = new TF1("fit9","pol0");
  fitGraphWithStats(w9,fit9,0.85,0.66,0.99,0.99);

  title = "Wien 10";
  canvas->cd(4);
  w10->Draw("ap");
  placeAxis(title,xtitle,ytitle,canvas,w10);
  TF1 *fit10 = new TF1("fit10","pol0");
  fitGraphWithStats(w10,fit10,0.85,0.66,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of plotByWienRMS function


void histoByWien( TString filename, TString dataname="diff_bcmdd78" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  float xmax = 2.5;
  float xmin = -2.5;
  float Nbins = 250;
  TH1F *h6 = new TH1F("h6","title",Nbins,xmin,xmax);
  TH1F *h7 = new TH1F("h7","title",Nbins,xmin,xmax);
  TH1F *h8 = new TH1F("h8","title",Nbins,xmin,xmax);
  TH1F *h9 = new TH1F("h9","title",Nbins,xmin,xmax);
  TH1F *h10 = new TH1F("h10","title",Nbins,xmin,xmax);

  histo_by_wien(6,tree,dataname,h6);
  histo_by_wien(7,tree,dataname,h7);
  histo_by_wien(8,tree,dataname,h8);
  histo_by_wien(9,tree,dataname,h9);
  histo_by_wien(10,tree,dataname,h10);

/*  histoRMS_by_wien(6,tree,dataname,h6);
  histoRMS_by_wien(7,tree,dataname,h7);
  histoRMS_by_wien(8,tree,dataname,h8);
  histoRMS_by_wien(9,tree,dataname,h9);
  histoRMS_by_wien(10,tree,dataname,h10);
*/
  blueHisto(h6);
  blueHisto(h7);
  blueHisto(h8);
  blueHisto(h9);
  blueHisto(h10);

  TString title  = "BCM78 DD (LH2, Runlets,5+1,Sign Corrected)";
  TString xtitle = "Charge DD Asymmetry (ppm)";
  TString ytitle = "Counts";

/*  TString title  = "Charge Asymmetry Width (LH2, Runlets,5+1,Sign Corrected)";
  TString xtitle = "Charge DD Asymmetry RMS (ppm)";
  TString ytitle = "Counts";
*/
  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);

  canvas->cd(1);
  h6->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","gaus");
  fitHistoWithStats(h6,fit6,0.77,0.25,0.99,0.99);

  title = "Wien 7";
  canvas->cd(2);
  h7->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h7);
  TF1 *fit7 = new TF1("fit7","gaus");
  fitHistoWithStats(h7,fit7,0.77,0.25,0.99,0.99);

  title = "Wien 8";
  canvas->cd(3);
  h8->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h8);
  TF1 *fit8 = new TF1("fit8","gaus");
  fitHistoWithStats(h8,fit8,0.77,0.25,0.99,0.99);

  title = "Wien 9";
  canvas->cd(4);
  h9->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h9);
  TF1 *fit9 = new TF1("fit9","gaus");
  fitHistoWithStats(h9,fit9,0.77,0.25,0.99,0.99);

  title = "Wien 10";
  canvas->cd(5);
  h10->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h10);
  TF1 *fit10 = new TF1("fit10","gaus");
  fitHistoWithStats(h10,fit10,0.77,0.25,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of histoByWien function



void pullByWien( TString filename, TString dataname="diff_bcmdd78" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  float mean;

  std::vector<double> v6;
  std::vector<double> v7;
  std::vector<double> v8;
  std::vector<double> v9;
  std::vector<double> v10;

  std::vector<double> v6rms;
  std::vector<double> v7rms;
  std::vector<double> v8rms;
  std::vector<double> v9rms;
  std::vector<double> v10rms;

  get_wien_from_tree(6,tree,dataname,&v6,&v6rms);
  get_wien_from_tree(7,tree,dataname,&v7,&v7rms);
  get_wien_from_tree(8,tree,dataname,&v8,&v8rms);
  get_wien_from_tree(9,tree,dataname,&v9,&v9rms);
  get_wien_from_tree(10,tree,dataname,&v10,&v10rms);

  float xmax = 10;
  float xmin = -xmax;
  float Nbins = 75;

  //dummy histograms to calculate mean/std dev
  //should really do this a better way, but it works for now
  TH1F *dummy6 = new TH1F("dummy6","title",Nbins,xmin,xmax);
  TH1F *dummy7 = new TH1F("dummy7","title",Nbins,xmin,xmax);
  TH1F *dummy8 = new TH1F("dummy8","title",Nbins,xmin,xmax);
  TH1F *dummy9 = new TH1F("dummy9","title",Nbins,xmin,xmax);
  TH1F *dummy10 = new TH1F("dummy10","title",Nbins,xmin,xmax);

  dummy6->FillN(v6.size(),v6.data(),NULL);
  dummy7->FillN(v7.size(),v7.data(),NULL);
  dummy8->FillN(v8.size(),v8.data(),NULL);
  dummy9->FillN(v9.size(),v9.data(),NULL);
  dummy10->FillN(v10.size(),v10.data(),NULL);


  xmax=5;
  xmin=-xmax;
  Nbins = 100;
  TH1F *h6 = new TH1F("h6","title",Nbins,xmin,xmax);
  TH1F *h7 = new TH1F("h7","title",Nbins,xmin,xmax);
  TH1F *h8 = new TH1F("h8","title",Nbins,xmin,xmax);
  TH1F *h9 = new TH1F("h9","title",Nbins,xmin,xmax);
  TH1F *h10 = new TH1F("h10","title",Nbins,xmin,xmax);

  mean = dummy6->GetMean();
  fill_pull_histo(h6,mean,&v6,&v6rms);

  mean = dummy7->GetMean();
  fill_pull_histo(h7,mean,&v7,&v7rms);

  mean = dummy8->GetMean();
  fill_pull_histo(h8,mean,&v8,&v8rms);

  mean = dummy9->GetMean();
  fill_pull_histo(h9,mean,&v9,&v9rms);

  mean = dummy10->GetMean();
  fill_pull_histo(h10,mean,&v10,&v10rms);

  dummy6=NULL;
  dummy7=NULL;
  dummy8=NULL;
  dummy9=NULL;
  dummy10=NULL;

  blueHisto(h6);
  blueHisto(h7);
  blueHisto(h8);
  blueHisto(h9);
  blueHisto(h10);

  TString title  = "BCM78 DD Pull Plots (Runlets,5+1)";
  TString xtitle = "Standard deviations";
  TString ytitle = "Counts";
/*  TString title = "Charge Asymmetry Pull (LH2, Runlets, 5+1,Sign Corrected)";
  TString xtitle = "Charge Asymmetry Pull";
  TString ytitle = "Counts";
*/
  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);

  canvas->cd(1);
  h6->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","gaus");
  fitHistoWithStats(h6,fit6,0.77,0.25,0.99,0.99);

  canvas->cd(2);
  h7->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h7);
  placeLabel("Wien 7",0.4,0.73,0.51,0.88);
  TF1 *fit7 = new TF1("fit7","gaus");
  fitHistoWithStats(h7,fit7,0.77,0.25,0.99,0.99);

  title = "Wien 8";
  canvas->cd(3);
  h8->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h8);
  TF1 *fit8 = new TF1("fit8","gaus");
  fitHistoWithStats(h8,fit8,0.77,0.25,0.99,0.99);

  title = "Wien 9";
  canvas->cd(4);
  h9->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h9);
  TF1 *fit9 = new TF1("fit9","gaus");
  fitHistoWithStats(h9,fit9,0.77,0.25,0.99,0.99);

  title = "Wien 10";
  canvas->cd(5);
  h10->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h10);
  TF1 *fit10 = new TF1("fit10","gaus");
  fitHistoWithStats(h10,fit10,0.77,0.25,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of histoByWien function


void pullByWien_friend( TString filename, TString dataname, TString filename2 ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TFile *file2 = new TFile(filename2);
  if ( !file2->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file2 <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename2 <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");
  TTree *friendtree = (TTree*) file2->Get("tree");

  float mean;

  std::vector<double> v6;
  std::vector<double> v7;
  std::vector<double> v8;
  std::vector<double> v9;
  std::vector<double> v10;

  std::vector<double> v6rms;
  std::vector<double> v7rms;
  std::vector<double> v8rms;
  std::vector<double> v9rms;
  std::vector<double> v10rms;

  get_wien_from_tree_friend(6,tree,friendtree,dataname,&v6,&v6rms);
  get_wien_from_tree_friend(7,tree,friendtree,dataname,&v7,&v7rms);
  get_wien_from_tree_friend(8,tree,friendtree,dataname,&v8,&v8rms);
  get_wien_from_tree_friend(9,tree,friendtree,dataname,&v9,&v9rms);
  get_wien_from_tree_friend(10,tree,friendtree,dataname,&v10,&v10rms);

  float xmax = 10;
  float xmin = -xmax;
  float Nbins = 75;

  //dummy histograms to calculate mean/std dev
  //should really do this a better way, but it works for now
  TH1F *dummy6 = new TH1F("dummy6","title",Nbins,xmin,xmax);
  TH1F *dummy7 = new TH1F("dummy7","title",Nbins,xmin,xmax);
  TH1F *dummy8 = new TH1F("dummy8","title",Nbins,xmin,xmax);
  TH1F *dummy9 = new TH1F("dummy9","title",Nbins,xmin,xmax);
  TH1F *dummy10 = new TH1F("dummy10","title",Nbins,xmin,xmax);

  dummy6->FillN(v6.size(),v6.data(),NULL);
  dummy7->FillN(v7.size(),v7.data(),NULL);
  dummy8->FillN(v8.size(),v8.data(),NULL);
  dummy9->FillN(v9.size(),v9.data(),NULL);
  dummy10->FillN(v10.size(),v10.data(),NULL);


  xmax=5;
  xmin=-xmax;
  Nbins = 100;
  TH1F *h6 = new TH1F("h6","title",Nbins,xmin,xmax);
  TH1F *h7 = new TH1F("h7","title",Nbins,xmin,xmax);
  TH1F *h8 = new TH1F("h8","title",Nbins,xmin,xmax);
  TH1F *h9 = new TH1F("h9","title",Nbins,xmin,xmax);
  TH1F *h10 = new TH1F("h10","title",Nbins,xmin,xmax);

  mean = dummy6->GetMean();
  fill_pull_histo(h6,mean,&v6,&v6rms);

  mean = dummy7->GetMean();
  fill_pull_histo(h7,mean,&v7,&v7rms);

  mean = dummy8->GetMean();
  fill_pull_histo(h8,mean,&v8,&v8rms);

  mean = dummy9->GetMean();
  fill_pull_histo(h9,mean,&v9,&v9rms);

  mean = dummy10->GetMean();
  fill_pull_histo(h10,mean,&v10,&v10rms);

  dummy6=NULL;
  dummy7=NULL;
  dummy8=NULL;
  dummy9=NULL;
  dummy10=NULL;

  blueHisto(h6);
  blueHisto(h7);
  blueHisto(h8);
  blueHisto(h9);
  blueHisto(h10);

  TString title  = "BCM78 DD Pull Plots (Runlets,5+1) (Current>130#muA)";
  TString xtitle = "Standard deviations";
  TString ytitle = "Counts";
/*  TString title = "Charge Asymmetry Pull (LH2, Runlets, 5+1,Sign Corrected)";
  TString xtitle = "Charge Asymmetry Pull";
  TString ytitle = "Counts";
*/
  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,5);

  canvas->cd(1);
  h6->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h6);
  placeLabel("Wien 6",0.4,0.73,0.51,0.88);
  TF1 *fit6 = new TF1("fit6","gaus");
  fitHistoWithStats(h6,fit6,0.77,0.25,0.99,0.99);

  canvas->cd(2);
  h7->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h7);
  placeLabel("Wien 7",0.4,0.73,0.51,0.88);
  TF1 *fit7 = new TF1("fit7","gaus");
  fitHistoWithStats(h7,fit7,0.77,0.25,0.99,0.99);

  title = "Wien 8";
  canvas->cd(3);
  h8->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h8);
  TF1 *fit8 = new TF1("fit8","gaus");
  fitHistoWithStats(h8,fit8,0.77,0.25,0.99,0.99);

  title = "Wien 9";
  canvas->cd(4);
  h9->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h9);
  TF1 *fit9 = new TF1("fit9","gaus");
  fitHistoWithStats(h9,fit9,0.77,0.25,0.99,0.99);

  title = "Wien 10";
  canvas->cd(5);
  h10->Draw();
  placeAxis(title,xtitle,ytitle,canvas,h10);
  TF1 *fit10 = new TF1("fit10","gaus");
  fitHistoWithStats(h10,fit10,0.77,0.25,0.99,0.99);

  gPad->SetGrid();
  gPad->Modified();
  gPad->Update();

} //end of histoByWien function

void detector_check( TString filename, string dataname = "diff_bcmdd78", int wien=6) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *tree = (TTree*) file->Get("tree");

  const int size = 9;
/*  const TString detectors [size] = {
    "md1", "md2", "md3",
    "md1", "md2", "md3",
    "md1", "md2",
  };
*/
  const int pad [size] = {
    4, 1, 2, 3, 6, 8, 9, 7, 5
  };

  TH2F *histArray[8];
  for (int n=0; n<=7; n++) {
    histArray[n] = new TH2F(Form("Detector Histo #%i",n),"title",100,0,100,100,0,100);
  }

  fillHistArray(tree,dataname,wien,histArray,8);

  TCanvas *canvas = new TCanvas("canvas","title");
  canvas->Divide(3,3);

  for (int i=0; i<size; i++) {
    canvas->cd(pad[i]);
    histArray[i]->Draw();
    redHisto(histArray[i]);
    placeAxis("title","xaxis","yaxis",canvas,histArray[i]);

    canvas->Modified();
    canvas->Update();
  }

  std::cout <<"If you're reading this, root didn't die.\n";



} //end routine detector_check


void plotHcbdByWien_friend( TString filename, TString friendname, int wien ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TFile *file2 = new TFile(friendname);
  if ( !file2->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file2 <<".\n" <<endl;
    exit(1);
  }
  std::cout <<"Successfully opened ROOTFILE " <<friendname <<".\n" <<endl;

  TTree *tree1 = (TTree*) file->Get("tree");
  TTree *tree2 = (TTree*) file2->Get("tree");

  const int number = 5;

  const TString names [number] = {
    "targetX", "targetXSlope", "targetY", "targetYSlope", "bpm3c12X"
  };

  const TString titleNames [number] = {
    "#DeltaX","#DeltaX Angle",
    "#DeltaY","#DeltaY Angle",
    "#DeltaE"
  };

  const TString axisNames [number] = {
    "X-position (mm)","X angle (mrad)",
    "Y-position (mm)","Y angle (mrad)",
    "Energy (unitless)"
  };

  double treeValues[number][4];
  std::vector<double> runlet;

  std::vector<double> myValues[number];
  std::vector<double> myErrors[number];

  int n_events1 = (int) tree1->GetEntries();
  int n_events2 = (int) tree2->GetEntries();

  if (n_events1 !=n_events2) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  int sign, wien_slug;
  double run_number_decimal;
  tree1->ResetBranchAddresses();
  tree1->SetBranchAddress("wien_slug",&wien_slug);
  tree1->SetBranchAddress("sign_correction",&sign);
  tree1->SetBranchAddress("run_number_decimal",&run_number_decimal);
  for (int j=0; j<number; j++) {
    tree1->SetBranchAddress(Form("diff_qwk_%s",names[j].Data()),&treeValues[j][0]);
  }

  double current[4], run_decimal_friend;
  tree2->ResetBranchAddresses();
  tree2->SetBranchAddress("yield_qwk_charge",&current);
  tree2->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  for (int i=0; i<n_events1; i++) {
    tree1->GetEntry(i);
    tree2->GetEntry(i);

    if (run_number_decimal != run_decimal_friend)
      printf("Something's screwed up! Your two trees are not aligned!\n");
    if ( treeValues[0][0] ==-1e6 ||wien_slug != wien)
      continue;
    if (current[0]<130)
      continue;

    runlet.push_back(run_number_decimal);
    for (int j=0; j<number; j++) {
      myValues[j].push_back(treeValues[j][0]);
      myErrors[j].push_back(treeValues[j][1]);
    }

  } //end for loop over events

  if (int(runlet.size()) == 0) {
    std::cout <<"No data\nExiting\n.";
    exit(0);
  }

  //put data into TGraphErrors
  //float size=0.05;
  float size=0.20;
  TMultiGraph *multi[number];
  TGraphErrors *graphs[number];
  for (int j=0; j<number; j++) {
    multi[j] = new TMultiGraph();
    graphs[j] = new TGraphErrors(runlet.size(),runlet.data(),myValues[j].data(),0,myErrors[j].data());
    bluePlot(graphs[j],size);
    if (multi[j]) {
      multi[j]->Add(graphs[j]);
    } else {
      printf("MG # %i DOES NOT EXIST...\n",j);
    }

  }

  TCanvas *canvas = new TCanvas("canvas","title",1200,1500);
  canvas->Divide(1,number);

  for (int j=0; j<number; j++) {
    canvas->cd(j+1);

    TString title = Form("Detector %s",names[j].Data());
    TString xtitle = "Runlet_id";
    TString ytitle = axisNames[j];
    multi[j]->Draw("ap");
    placeAxis(title,xtitle,ytitle,canvas,multi[j]);

    gPad->SetGrid();
    gPad->Modified();
    gPad->Update();
  }

} //end of plotByWien function












