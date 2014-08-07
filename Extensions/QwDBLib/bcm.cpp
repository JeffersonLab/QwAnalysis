/*
 * bcm.cpp
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

int main(int argc, char* argv[]) {
    /* Usage information if no arguments */
    if(argc < 2) {
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
    QwTreeBranchPlot bcm("bcm", "runlet_id", "BCM charge asymmetry (ppm)");
    if(wien == -1) {
        bcm.get_data_from_tree(tree_asym, "asym_qwk_charge");
    } else {
        bcm.get_data_from_tree(tree_asym, "asym_qwk_charge", wien);
    }

    // sign*Aq central value vs. runlet
    TCanvas c_bcm_vs_runlet("c_bcm_vs_runlet");
    bcm.ValueRunletPlotSignCorr();
    c_bcm_vs_runlet.Update();
    c_bcm_vs_runlet.Modified();
    c_bcm_vs_runlet.Print("bcm_vs_runlet.png");

    // Aq rms vs. runlet
    TCanvas c_bcm_rms_vs_runlet("c_bcm_rms_vs_runlet");
    bcm.RMSRunletPlot();
    c_bcm_rms_vs_runlet.Update();
    c_bcm_rms_vs_runlet.Modified();
    c_bcm_rms_vs_runlet.Print("bcm_rms_vs_runlet.png");

    // sign*Aq histograms
    TCanvas c_bcm_histogram("c_bcm_histogram");
    bcm.ValuePlotSignCorr();
    c_bcm_histogram.Update();
    c_bcm_histogram.Modified();
    c_bcm_histogram.Print("bcm_histogram.png");

    // set the weight to the error of the bcm charge asymmetry
    bcm.set_weight_error(tree_asym, "asym_qwk_charge");

    TCanvas c_bcm_histogram_weighted_by_own_error("c_bcm_histogram_weighted_by_own_error");
    bcm.ValuePlotSignCorrWeight();
    c_bcm_histogram_weighted_by_own_error.Update();
    c_bcm_histogram_weighted_by_own_error.Modified();
    c_bcm_histogram_weighted_by_own_error.Print("bcm_weighted_by_own_error.png");

    // set the weight to the error of MD all bars
    bcm.set_weight_error(tree_asym, "asym_mdallbars");

    TCanvas c_bcm_histogram_weighted_by_mdallbars_error("c_bcm_histogram_weighted_by_mdallbars_error");
    bcm.ValuePlotSignCorrWeight();
    c_bcm_histogram_weighted_by_mdallbars_error.Update();
    c_bcm_histogram_weighted_by_mdallbars_error.Modified();
    c_bcm_histogram_weighted_by_mdallbars_error.Print("bcm_weighted_by_mdallbars_error.png");

    /* Return */
    return 0;
}
