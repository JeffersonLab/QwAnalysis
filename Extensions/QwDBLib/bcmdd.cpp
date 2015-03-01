/*
 * bcmdd.cpp
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
    QwTreeBranchPlot bcmdd("bcmdd", "runlet_id", "BCM asymmetry double difference (ppm)");
    TString bcmdd_name;
    if (wien < 0 || wien > 10) {
        std::cout << "Wien " << wien << " not supported." << std::endl;
        return -1;
    } else if (wien < 6) {
        bcmdd_name = "asym_bcmdd12";
    } else if (wien > 5) {
        bcmdd_name = "asym_bcmdd78";
    }
    bcmdd.get_data_from_tree(tree_asym, bcmdd_name, wien);

    // sign*BCM 1-2 DD central value vs. runlet
    TCanvas c_bcmdd_vs_runlet("c_bcmdd_vs_runlet");
    bcmdd.ValueRunletPlotSignCorr();
    c_bcmdd_vs_runlet.Update();
    c_bcmdd_vs_runlet.Modified();
    c_bcmdd_vs_runlet.Print("bcmdd_vs_runlet.png");

    // BCM 1-2 DD rms vs. runlet
    TCanvas c_bcmdd_rms_vs_runlet("c_bcmdd_rms_vs_runlet");
    bcmdd.RMSRunletPlot();
    c_bcmdd_rms_vs_runlet.Update();
    c_bcmdd_rms_vs_runlet.Modified();
    c_bcmdd_rms_vs_runlet.Print("bcmdd_rms_vs_runlet.png");

    // sign*BCM 1-2 double difference histograms
    TCanvas c_bcmdd_histogram("c_bcmdd_histogram");
    bcmdd.ValuePlotSignCorr();
    c_bcmdd_histogram.Update();
    c_bcmdd_histogram.Modified();
    c_bcmdd_histogram.Print("bcmdd_histogram.png");

    // sign*BCM 1-2 double difference pull plot
    TCanvas c_bcmdd_pull_plot("c_bcmdd_pull_plot");
    bcmdd.ValuePullPlot();
    c_bcmdd_pull_plot.Update();
    c_bcmdd_pull_plot.Modified();
    c_bcmdd_pull_plot.Print("bcmdd_pull_plot.png");

    // set the weight to the error of the bcm DD asymmetry
    bcmdd.set_weight_error(tree_asym, bcmdd_name);

    TCanvas c_bcmdd_histogram_weighted_by_own_error("c_bcmdd_histogram_weighted_by_own_error");
    bcmdd.ValuePlotSignCorrWeight();
    c_bcmdd_histogram_weighted_by_own_error.Update();
    c_bcmdd_histogram_weighted_by_own_error.Modified();
    c_bcmdd_histogram_weighted_by_own_error.Print("bcmdd_weighted_by_own_error.png");

    // set the weight to the error of MD all bars
    bcmdd.set_weight_error(tree_asym, "asym_mdallbars");

    TCanvas c_bcmdd_histogram_weighted_by_mdallbars_error("c_bcmdd_histogram_weighted_by_mdallbars_error");
    bcmdd.ValuePlotSignCorrWeight();
    c_bcmdd_histogram_weighted_by_mdallbars_error.Update();
    c_bcmdd_histogram_weighted_by_mdallbars_error.Modified();
    c_bcmdd_histogram_weighted_by_mdallbars_error.Print("bcmdd_weighted_by_mdallbars_error.png");


    /* Return */
    return 0;
}
