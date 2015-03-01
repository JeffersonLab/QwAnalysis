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

/* Structure used to read data out of the rootfile. Should never be used
 * outside of this library */
struct temp_value {
    double value;
    double error;
    double rms;
    int n;
};


int main(int argc, char* argv[]) {
    /* Get variable names from cli for rootfiles. */
    TString var_1 = argv[1];

    /* Open both rootfiles. */
    TFile* rootfile_old;
    rootfile_old = TFile::Open(var_1);

    /* Grab the tree from both rootfile. */
    TTree* tree_old;
    tree_old = (TTree*)rootfile_old->Get("tree");

    /* Number of runlets in the dirty tree. */
    int entries_old = tree_old->GetEntries();

    /* Temp objects to store tree. */
    temp_value mdallpmtavg;
    temp_value uslumi;
    temp_value pmtltg;
    temp_value pmtonl;
    temp_value md9;

    /* Set branches to duplicate. */
    tree_old->SetBranchStatus("*",1);
    tree_old->SetBranchAddress("asym_mdallpmtavg", &mdallpmtavg);
    tree_old->SetBranchAddress("asym_uslumi_sum", &uslumi);
    tree_old->SetBranchAddress("asym_qwk_pmtltg", &pmtltg);
    tree_old->SetBranchAddress("asym_qwk_pmtonl", &pmtonl);
    tree_old->SetBranchAddress("asym_md9pmtavg", &md9);

    /* Create new root tree and file. */
    TFile* rootfile_new;
    rootfile_new = new TFile("new_rootfile.root","recreate");

    TTree* tree_new;
    tree_new = tree_old->CloneTree(0);

    for(int i = 0; i < entries_old; i++) {
        tree_old->GetEntry(i);
        if(mdallpmtavg.n > 0 && uslumi.n > 0) tree_new->Fill();
    }

    /* Save rootfile. */
    rootfile_new->Write();

    return 0;
}
