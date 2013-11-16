/*
 * charge_blocked_width.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 * Requires QwTreeLib to be installed into $LD_LIBRARY_PATH
 *
 * Run make, make install to build this file and the library
 *
 * ./charge_blocked_width <rootfile 1> <rootfile 2>
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
#include "QwTreeLib.h"
#include <stdlib.h> 

int main(int argc, char* argv[]) {
    /* Get variable names from cli for rootfiles. */
    TString var_1 = argv[1];
    TString var_2 = argv[2];

    /* Open both rootfiles. */
    TFile* rootfile_asym;
    rootfile_asym = TFile::Open(var_1);
    TFile* rootfile_slope;
    rootfile_slope = TFile::Open(var_2);

    /* Grab the tree from both rootfile. */
    TTree *tree_asym;
    tree_asym = (TTree*)rootfile_asym->Get("tree");
    TTree *tree_slope;
    tree_slope = (TTree*)rootfile_slope->Get("tree");

    /* Create a TApplication. This is required for anything using the ROOT GUI. */
    TApplication *app = new TApplication("slopes", &argc, argv);
    /* Some fit crap. */
    gStyle->SetOptFit(1111);

    /* Define objects to hold each branch. */
    tree_value md1pmtavg;
    tree_value md3pmtavg;
    tree_value md5pmtavg;
    tree_value pmtltg;
    tree_value md1pmtavg_slope;
    tree_value md3pmtavg_slope;
    tree_value md5pmtavg_slope;
    tree_value pmtltg_slope;

    /* 
     * Get the data from the tree.
     *
     * get_data_from_tree(TTree*, "variable name as appears in branch", &tree_value)
     */ 
    get_data_from_tree(tree_asym, "asym_qwk_md1barsum", &md1pmtavg);
    get_data_from_tree(tree_asym, "asym_qwk_md3barsum", &md3pmtavg);
    get_data_from_tree(tree_asym, "asym_qwk_md5barsum", &md5pmtavg);
    get_data_from_tree(tree_asym, "asym_qwk_pmtltg", &pmtltg);
    get_data_from_tree(tree_slope, "asym_qwk_md1barsum", &md1pmtavg_slope);
    get_data_from_tree(tree_slope, "asym_qwk_md3barsum", &md3pmtavg_slope);
    get_data_from_tree(tree_slope, "asym_qwk_md5barsum", &md5pmtavg_slope);
    get_data_from_tree(tree_slope, "asym_qwk_pmtltg", &pmtltg_slope);

    /* 
     * Clean out all the zeros. This needs to be done betterer, but for now
     * this works.
     */
    int size = md1pmtavg_slope.size();
    for(int i = 0; i < size; i++) {
        if(md1pmtavg_slope.return_value()[i] < -1e5) {
            md1pmtavg.del_index(i);
            md3pmtavg.del_index(i);
            md5pmtavg.del_index(i);
            pmtltg.del_index(i);
            md1pmtavg_slope.del_index(i);
            md3pmtavg_slope.del_index(i);
            md5pmtavg_slope.del_index(i);
            pmtltg_slope.del_index(i);
            i--;
            size--;
        }
    }

    /* Create TCanvas. */
    TCanvas* c;
    c = new TCanvas("c"); 

    /* Define TGraph. */
    TGraph* tg;

    /*
     * Use plot_vector function to plot value leaf.
     *
     * plot_vectors(vector<double> x, vector<double> y, "name x", "name y")
     */
    tg = plot_vectors(pmtltg.return_value(), md1pmtavg_slope.return_value(), "pmtltg charge slope (5+1)", "md1pmtavg energy slope (5+1)");
    /* Standardized plot style can be put into setter methods. */
    tg->SetMarkerStyle(kFullSquare);
    tg->SetMarkerSize(1);
    tg->Draw("AP");

    /* TApplication crap. */
    app->Run();
    return 0;
}
