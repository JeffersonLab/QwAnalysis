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
    QwTreeBranchPlot mdallpmtavg("mdallpmtavg", "runlet_id", "mdallpmtavg asym (ppm)");
    if(wien == -1) {
        mdallpmtavg.get_data_from_tree(tree_asym, "asym_mdallpmtavg", "asym_qwk_charge");
    } else {
        mdallpmtavg.get_data_from_tree(tree_asym, "asym_mdallpmtavg", "asym_qwk_charge", wien);
    }

    //mdallpmtavg.set_weight_value(tree_asym, "asym_qwk_charge");

    // sign*asymmetry vs runlet for mdallpmtavg
    TCanvas mdallpmtavg_vs_runlet("c1");
    mdallpmtavg.ValueRunletPlotSignCorr();
    mdallpmtavg_vs_runlet.Update();
    mdallpmtavg_vs_runlet.Modified();
    mdallpmtavg_vs_runlet.Print("mdallpmtavg_vs_runlet.png");

    // asymmetry rms vs runlet for mdallpmtavg
    TCanvas mdallpmtavg_rms_vs_runlet("c1");
    mdallpmtavg.RMSRunletPlot();
    mdallpmtavg_rms_vs_runlet.Update();
    mdallpmtavg_rms_vs_runlet.Modified();
    mdallpmtavg_rms_vs_runlet.Print("mdallpmtavg_rms_vs_runlet.png");

    // sign*asymmetry for mdallpmtavg
    TCanvas mdallpmtavg_histogram("c2");
    mdallpmtavg.ValuePlotSignCorr();
    mdallpmtavg_histogram.Update();
    mdallpmtavg_histogram.Modified();
    mdallpmtavg_histogram.Print("mdallpmtavg_histogram.png");

    // runlet pull plots of mdallpmtavg
    TCanvas mdallpmtavg_pull_plot("c3");
    mdallpmtavg.ValuePullPlot();
    mdallpmtavg_pull_plot.Update();
    mdallpmtavg_pull_plot.Modified();
    mdallpmtavg_pull_plot.Print("mdallpmtavg_pull_plot.png");

    /* Return */
    return 0;
}
