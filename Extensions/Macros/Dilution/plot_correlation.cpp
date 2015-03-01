/*
 * Short program to look at correlations of mdallbars to uslumi_sum.
 * For Q-weak experiment.
 *
 * J. Magee
 * magee@jlab.org
 * July 11, 2014
 * plot_correlation.cpp
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TPaveStats.h>
#include <TApplication.h>

#include "SlugCalc.h"
#include "SlugCalcVec.h"
#include "PlotLib.h"

using namespace std;

void helpscreen(void);

int main(int argc, char* argv[]) {
  /* Program expects one to pass one of Wade's db_rootfiles.
   * If no rootfile passed, die.
   */
  if (argc<=1) {
    helpscreen();
    return 1;
  }

  bool debug = false;
  //now let's open the given file
  std::string filename = argv[1];
  TFile *rootfile = TFile::Open(filename.data());

  //check if file is open. If not, exit.
  if ( !rootfile->IsOpen() ) {
    printf("Rootfile %s isn't open! Exiting!\n",filename.data());
    exit(EXIT_FAILURE);
  }

  /* At this point, we have an open rootfile. Let's grab the Tree,
   * Branches, and Leaves we care about.
   *
   * Note: leaf names are prefaced with an "l"
   */
  TTree *tree = (TTree*) rootfile->Get("tree");
  TBranch *md_branch   = tree->GetBranch("asym_qwk_mdallbars");
  TBranch *bkgd_branch = tree->GetBranch("asym_uslumi_sum");

  TLeaf *lMd_value = (TLeaf*) md_branch->GetLeaf("value");
  TLeaf *lMd_error = (TLeaf*) md_branch->GetLeaf("err");

  TLeaf *lBkgd_value = (TLeaf*) bkgd_branch->GetLeaf("value");
  TLeaf *lBkgd_error = (TLeaf*) bkgd_branch->GetLeaf("err");

  //we also need various other leaves
  TLeaf *lRunlet = (TLeaf*) tree->GetLeaf("run_number_decimal");
  TLeaf *lSlug   = (TLeaf*) tree->GetLeaf("slug");
  TLeaf *lSign   = (TLeaf*) tree->GetLeaf("sign_correction");

  /* We now need to create our local, function defined variables.
   * Our main objects are SlugCalcVec objects; these are vectors
   * of SlugCalc objects. Each slug is contained in its own SlugCalc
   * object. All the runs/runlets are given to it, and the average is
   * computed, and then held in the SlugCalcVec object.
   *
   * For simplicity and clarity, I also opt to create holder variables
   * for things like runlet number, etc.
   */

  //create my vector of SlugCalc objects
  SlugCalcVec mdall_vector;
  SlugCalcVec bkgd_vector;

  //define "holder" variables
  int sign;
  int slug;
  float runlet;
  float md_value;
  float md_error;
  float bkgd_value;
  float bkgd_error;


  //physics asymmetry, discussed below.
  //if not given by command line, set to 0.
  float A_phys = argv[2] ? atof (argv[2]) : 0;

  /* Let's now walk through the tree, event-by-event and
   * average the detector and bkgd values.
   *
   * Subtle point on the logic:
   * We are interested in the background contribution to the NULL asymmetry;
   * this contribution is independent of sign flip/wien reversal  (if it
   * were, it would obviously cancel with sign flips. This might be 
   * obvious to some but bears repetition.). This has several consequences:
   *
   * 1) The background monitor asymmetry should NOT be sign corrected.
   * 2) The main detector asymmetry should also NOT be sign corrected.
   * 3) The main detector should have the flip-dependent asymmetry
   *    (PHYS asymmetry) removed (I.E. sign*A_phys).
   *
   * 2 and 3 are consequences from the spin-dependent asymmetry portion
   * changing incoherently with the independent portion. To properly see
   * this, just set A_phys to 0. The sensitivity slopes *may* be similar,
   * but the fit intercepts will differ. If on doesn't subtract the A_phys
   * the intercept essentially shows the variation from weighted mean.
   * After subtracting A_phys, the intercept shows the variation from the NULL.
   */

  for (int j=0; j<(int) tree->GetEntries(); j++) {
    tree->GetEntry(j);
    md_branch->GetEntry(j);
    bkgd_branch->GetEntry(j);

    runlet     = lRunlet->GetValue();
    slug       = lSlug->GetValue();
    sign       = lSign->GetValue();

    md_value   = 1000*( lMd_value->GetValue() ) - sign*A_phys;
    bkgd_value = ( lBkgd_value->GetValue() );

    if ( fabs(md_value)==1000000 || fabs(bkgd_value)==1000000 ) continue;
    //these runs had poor background asymmetry
    if (runlet>=15320 && runlet<=15460) continue;
    if (runlet>=15590 && runlet<=15675) continue;
    if (fabs(bkgd_value)>100) continue;

    md_error   = 1000*lMd_error->GetValue();
    bkgd_error = lBkgd_error->GetValue();

    mdall_vector.push_back(slug,runlet,md_value,md_error);
    bkgd_vector.push_back(slug,runlet,bkgd_value,bkgd_error);

    if (debug)
      printf("%d \t%d \t%d \t%.2f \t%.2f\n",j,sign,slug,runlet,md_value);

  } //end tree walking

  if (debug) {
    printf("Size of vectors: \t%d \t%d\n",mdall_vector.Size(),bkgd_vector.Size());
    printf("End of tree walking...\n");

    for(int j=0; j<mdall_vector.Size(); j++) {
      printf("%d \t%d \t%d \t%f \t\t%.2f \t%.2f \t%.2f \t%.2f\n",
          j,mdall_vector.Size(), bkgd_vector.Size(),mdall_vector[j].GetSlug(),
          mdall_vector[j].GetAvg(), mdall_vector[j].GetError(),
          bkgd_vector[j].GetAvg(), bkgd_vector[j].GetError()
          );
    }
  }

  /* I am having trouble grabbing the arrays directly from the
   * SlugCalcVec objects. This seems to only happen when I grab data
   * from the rootfile. When I create data by hand instead of the
   * root file, everything works splendidly - I don't understand this.  *
   * This is an example of data "by hand":
   *    mdall_vector.push_back(4,1,10,1);
   *    mdall_vector.push_back(5,1,20,2);
   *
   *    bkgd_vector.push_back(4,1,3,2);
   *    bkgd_vector.push_back(5,1,6,2);
   *
   * For the time being, I will copy the data into regular C++
   * style vectors and process the data from there.
   */
  std::vector<double> myMdVec, myMdErrVec,myBkVec;

  for(int j=0; j<mdall_vector.Size(); j++) {
    myMdVec.push_back(mdall_vector[j].GetAvg());
    myMdErrVec.push_back(mdall_vector[j].GetError());
    myBkVec.push_back(bkgd_vector[j].GetAvg());
  }

  /* Now that we have all the data localized in the SlugCalcVec objects,
   * we can concentrate on plotting the information and extracting best
   * fit information. This will be the remainder of the logic.
   */

  //Create a TApplication. This is required for anything using the ROOT GUI.
  std::cout <<"Creating TApplication...\n";
  TApplication *app = new TApplication("myApp",&argc,argv);
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  std::cout <<"Creating canvas...\n";
  TCanvas *canvas = new TCanvas("canvas","title",800,600);
  canvas->cd();

  //Manolis assumed no error on bkgd detector asymmetries
  std::cout <<"Creating TGraphErrors...\n";
  TGraphErrors *graph = new TGraphErrors
    (
      myMdVec.size(),
      myBkVec.data(),myMdVec.data(),
      0,myMdErrVec.data()
    );

  /* Here is the TGraphError object using my SlugCalcVec objects.
   * For some reason this segfaults with the ROOT data.
   * TGraphErrors *graph = new TGraphErrors
   * ( mdall_vector.Size(),
   *   bkgd_vector.GetAvgArray(),mdall_vector.GetAvgArray(),
   *   0, mdall_vector.GetErrorArray() );
   */
  if (debug) std::cout <<"TGraphErrors created...\n";

  if (debug) std::cout <<"Drawing TGraphErrors...\n";
  graph->Draw("ap");

  bluePlot(graph,1.0);

  std::string title = "mdallbars vs uslumi_sum (run 2, LH2)";
  std::string xaxis = "uslumi_sum asymmetry (ppm)";
  std::string yaxis = "mdallbars asymmetry (ppb)";

  placeAxis(title,xaxis,yaxis,canvas,graph);
  gPad->Modified();
  gPad->Update();
  canvas->SetGrid();

  TF1 *fit = new TF1("fit","pol1");
  fit->SetParNames("Intercept","Slope");
  fit->SetLineColor(46);
  fit->SetLineStyle(7);
  graph->Fit("fit","sames");

  canvas->Modified();
  canvas->Update();

  TPaveStats *stats = (TPaveStats*) graph->GetListOfFunctions()->FindObject("stats");
  if (stats) {
    stats->SetX1NDC(0.79);
    stats->SetY1NDC(0.86);
    stats->SetX2NDC(0.99);
    stats->SetY2NDC(0.99);
    //stats->SetTextColor(9);
    stats->Draw();
  } else {
    std::cout <<"Stats box not found.\n";
  }

  canvas->Modified();
  canvas->Update();
  app->Run();
  return 0;
} //end main function

void helpscreen(void) {
  std::cout <<"Welcome to Magee's plot_correlation emporium.\n"
    <<"Please compile in the command line and pass a rootfile, shown below:\n"
    <<"To compile:"
    <<"\tg++ -Wall -m64 `root-config --libs --cflags` -Iinclude \ \n"
    <<"\tsrc/SlugCalc.cpp src/SlugCalcVec.cpp src/PlotLib.cpp \ \n"
    <<"\tplot_correlation.cpp -o plot_correlation\n"

    <<"To run, just pass the rootfile you are interested in analysing:\n"
    <<"\t plot_correlation Qweak_10709.root\n"
    <<std::endl;
  exit(EXIT_FAILURE);
}


