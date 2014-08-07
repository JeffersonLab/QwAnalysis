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
    QwTreeBranchPlot dslumi("dslumi", "runlet_id", "dslumi asym (ppm)");
    if(wien == -1) {
        dslumi.get_data_from_tree(tree_asym, "asym_dslumi_sum", "asym_qwk_charge");
    } else {
        dslumi.get_data_from_tree(tree_asym, "asym_dslumi_sum", "asym_qwk_charge", wien);
    }

    //dslumi.set_weight_value(tree_asym, "asym_qwk_charge");

    // sign*asymmetry vs runlet for dslumi
    TCanvas dslumi_vs_runlet("c1");
    dslumi.ValueRunletPlotSignCorr();
    dslumi_vs_runlet.Update();
    dslumi_vs_runlet.Modified();
    dslumi_vs_runlet.Print("dslumi_vs_runlet.png");

    // asymmetry rms vs runlet for dslumi
    TCanvas dslumi_rms_vs_runlet("c1");
    dslumi.RMSRunletPlot();
    dslumi_rms_vs_runlet.Update();
    dslumi_rms_vs_runlet.Modified();
    dslumi_rms_vs_runlet.Print("dslumi_rms_vs_runlet.png");

    // sign*asymmetry for dslumi
    TCanvas dslumi_histogram("c2");
    dslumi.ValuePlotSignCorr();
    dslumi_histogram.Update();
    dslumi_histogram.Modified();
    dslumi_histogram.Print("dslumi_histogram.png");

    // runlet pull plots of dslumi
    TCanvas dslumi_pull_plot("c3");
    dslumi.ValuePullPlot();
    dslumi_pull_plot.Update();
    dslumi_pull_plot.Modified();
    dslumi_pull_plot.Print("dslumi_pull_plot.png");

    /* Return */
    return 0;
}
