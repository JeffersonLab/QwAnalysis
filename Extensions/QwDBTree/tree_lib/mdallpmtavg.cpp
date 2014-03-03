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

int main(int argc, char* argv[]) {
    /* Usage information if no arguments */
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <rootfiles>" << std::endl;
        exit(0);
    }

    /* Get variable names from cli for rootfiles. */
    TString var_1 = argv[1];

    /* Open both rootfiles. */
    TFile* rootfile_asym;
    rootfile_asym = TFile::Open(var_1);

    /* Grab the tree from both rootfile. */
    TTree *tree_asym;
    tree_asym = (TTree*)rootfile_asym->Get("tree");

    /* Some fit crap. */
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0000000);


    /* Define objects to hold each branch. Define the axis variables in constructor.*/
    QwTreeBranchPlot mdallpmtavg("mdallpmtavg", "runlet_id", "mdallpmtavg asym (ppm)");
    mdallpmtavg.get_data_from_tree(tree_asym, "asym_mdallpmtavg", "asym_qwk_charge");
    //mdallpmtavg.set_weight_value(tree_asym, "asym_qwk_charge");
    mdallpmtavg.CleanZeroes();

    


    /* Create TCanvas. */
    TCanvas c1("c1");
    mdallpmtavg.ValueRunletPlot();
    c1.Update();
    c1.Modified();
    c1.Print("mdallpmtavg_vs_runlet.png");

    TCanvas c2("c2");
    mdallpmtavg.ValuePlot();
    c2.Update();
    c2.Modified();
    c2.Print("mdallpmtavg_histogram.png");

    /* Return */
    return 0;
}
