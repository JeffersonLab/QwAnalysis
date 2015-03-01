/*
 * data_quality.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *         Wouter Deconinck <wdconinc@jlab.org>
 *
 * Requires QwTreeLib to be installed into $LD_LIBRARY_PATH
 *
 * Run make, make install to build this file and the library
 *
 * ./data_quality <rootfiles>
 */
#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <math.h>
#include <stdlib.h>
#include "QwTreeBranchPlot.h"
#include "QwTreeBranch.h"

int main(int argc, char* argv[]) {
    /* Usage information if no arguments */
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <rootfiles>" << std::endl;
        exit(0);
    }

    int wien = -1;
    if(argc > 2) {
        wien = atoi(argv[2]);
        cout << "Only plotting wien " << wien << endl;
    }

    /* Get variable names from cli for rootfiles. */
    TString var_1 = argv[1];

    /* Open both rootfiles. */
    TFile* rootfile_asym;
    rootfile_asym = TFile::Open(var_1);

    /* Grab the tree from both rootfile. */
    TTree *tree_dirty;
    TTree *tree_asym;
    tree_dirty = (TTree*)rootfile_asym->Get("tree");
    tree_asym = clean_tree(tree_dirty, 1);

    /* Some fit crap. */
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0000000);

    /* Define objects to hold each branch. Define the axis variables in constructor.*/
    QwTreeBranchPlot uslumi("uslumi", "runlet_id", "uslumi asym (ppm)");
    if(wien == -1) {
        uslumi.get_data_from_tree(tree_asym, "asym_uslumi_sum", "asym_qwk_charge");
    } else {
        uslumi.get_data_from_tree(tree_asym, "asym_uslumi_sum", "asym_qwk_charge", wien);
    }

    //uslumi.set_weight_value(tree_asym, "asym_qwk_charge");

    // sign*asymmetry vs runlet for uslumi
    TCanvas uslumi_vs_runlet("c1");
    uslumi.ValueRunletPlotSignCorr();
    uslumi_vs_runlet.Update();
    uslumi_vs_runlet.Modified();
    uslumi_vs_runlet.Print("uslumi_vs_runlet.png");

    // asymmetry rms vs runlet for uslumi
    TCanvas uslumi_rms_vs_runlet("c1");
    uslumi.RMSRunletPlot();
    uslumi_rms_vs_runlet.Update();
    uslumi_rms_vs_runlet.Modified();
    uslumi_rms_vs_runlet.Print("uslumi_rms_vs_runlet.png");

    // sign*asymmetry for uslumi
    TCanvas uslumi_histogram("c2");
    uslumi.ValuePlotSignCorr();
    uslumi_histogram.Update();
    uslumi_histogram.Modified();
    uslumi_histogram.Print("uslumi_histogram.png");

    // runlet pull plots of uslumi
    TCanvas uslumi_pull_plot("c3");
    uslumi.ValuePullPlot();
    uslumi_pull_plot.Update();
    uslumi_pull_plot.Modified();
    uslumi_pull_plot.Print("uslumi_pull_plot.png");

    /* Return */
    return 0;
}
