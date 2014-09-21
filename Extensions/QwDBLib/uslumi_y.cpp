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
    tree_asym = clean_tree(tree_dirty, 2);

    /* Some fit crap. */
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0000000);

    /* Define objects to hold each branch. Define the axis variables in constructor.*/
    QwTreeBranchPlot uslumi_sum("uslumi_sum", "runlet_id", "uslumi_sum yield (ppm)");
    if(wien == -1) {
        uslumi_sum.get_data_from_tree(tree_asym, "yield_uslumi_sum", "yield_qwk_charge");
    } else {
        uslumi_sum.get_data_from_tree(tree_asym, "yield_uslumi_sum", "yield_qwk_charge", wien);
    }

    //uslumi_sum.set_weight_value(tree_asym, "yield_qwk_charge");

    // yeild vs runlet for uslumi_sum
    TCanvas uslumi_sum_vs_runlet("c1");
    uslumi_sum.ValueRunletPlot();
    uslumi_sum_vs_runlet.Update();
    uslumi_sum_vs_runlet.Modified();
    uslumi_sum_vs_runlet.Print("yield_uslumi_sum_vs_runlet.png");

    /* Return */
    return 0;
}
