/*
 * example.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 * Requires QwTreeLib to be installed into $LD_LIBRARY_PATH
 *
 * Run make, make install to build this file and the library
 *
 * ./example <rootfile> 
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
    /* Get variable names from cli for rootfiles. */
    TString var_1 = argv[1];

    /* Open both rootfiles. */
    TFile* rootfile_asym;
    rootfile_asym = TFile::Open(var_1);

    /* Grab the tree from both rootfile. */
    TTree *tree_asym;
    tree_asym = (TTree*)rootfile_asym->Get("tree");

    /* Create a TApplication. This is required for anything using the ROOT GUI. */
    TApplication *app = new TApplication("slopes", &argc, argv);
    /* Some fit crap. */
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0000000);


    /* Define objects to hold each branch. Define the axis variables in constructor.*/
    QwTreeBranchPlot mdallpmtavg("mdallpmtavg", "runlet_id", "mdallpmtavg asym (ppm)");

    /* 
     * Get the data from the tree.
     *
     * get_data_from_tree(TTree*, "variable name as appears in branch")
     */ 
    mdallpmtavg.get_data_from_tree(tree_asym, "asym_mdallpmtavg", "asym_qwk_charge");
    //mdallpmtavg.set_weight_value(tree_asym, "asym_qwk_charge");

    /*
     * Will grab only wien 6 from the rootfile. Currently, it disagrees with
     * the wien 6 rootfile by about 40 runlets.
     */ 
    //mdallpmtavg.get_data_from_tree(tree_asym, "asym_mdallpmtavg", "asym_qwk_charge", 6);

    /* 
     * Clean out all the zeros. This needs to be done betterer, but for now
     * this works. Will implement Pauls fix soon.
     */
    int size = mdallpmtavg.size();
    for(int i = 0; i < size; i++) {
        if(mdallpmtavg.get_value()[i] < -1e5) {
            mdallpmtavg.del_index(i);
            i--;
            size--;
        }
    }

    /* Create TCanvas. */
    TCanvas* c;
    c = new TCanvas("c"); 

    /* Use ValueRunletPlot() method to plot value leaf vs runlet. */
    mdallpmtavg.ValueRunletPlot();

    /* Use the ValuePlot method to plot a histo of value. */
    //mdallpmtavg.ValuePlot();

    /* Use the ValuePullPlot method to generate a pull plot */
    //mdallpmtavg.ValuePullPlot();

    /* If uncommented, will print all the values for that object. */
    //mdallpmtavg.debug();

    c->Update();
    c->Modified();

    /* TApplication crap. */
    app->Run();
    return 0;
}
