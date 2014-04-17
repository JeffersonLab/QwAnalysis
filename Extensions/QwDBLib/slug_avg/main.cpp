/*
 * Compile: g++ `root-config --glibs --cflags` -ggdb -O0 -Wall -Wextra -o slug_avg main.cpp
 * Run: ./slug_avg --debug --run 1 --rootfile ~/db_rootfiles/run1/parity/HYDROGEN-CELL_offoff_tree.root
 *
 * Author: Wade Duvall <wsduvall@jlab.org>
 */
#include <TROOT.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLine.h>
#include <TLegend.h>
#include <TF1.h>
#include <TPaveText.h>

/*
 * main.cpp
 *
 */

/* Structure used to read data out of the rootfile. Should never be used
 *  * outside of this library */
struct temp_value {
    double value;
    double error;
    double rms;
    int n;
};

int main(Int_t argc, Char_t* argv[]) {
    /*
     * Defaults are set. No default mapfile is set, and run level averaging is
     * disabled.
     */
    TString detector_name = "asym_mdallpmtavg";     // default detector
    TString rootfile_name = "";                     // initialize rootfile name
    bool debug            = false;
    int slug_begin        = 0;
    int slug_end          = 0;
    int run               = 0;
    
    // Parse command line options.
    for(Int_t i = 1; i < argc; i++) {
        if(0 == strcmp("--detector", argv[i])) detector_name = argv[i+1];
        if(0 == strcmp("--rootfile", argv[i])) rootfile_name = argv[i+1];
        if(0 == strcmp("--debug", argv[i])) debug = true;
        if(0 == strcmp("--run", argv[i])) run = atoi(argv[i+1]);
    }

    /* print all settings */
    cout << "detector = " << detector_name << endl;
    cout << "rootfile = " << rootfile_name << endl;
    cout << "run = " << run << endl;

    /* Set slugs based on run. */
    if(run == 1) {
            slug_begin = 45;
            slug_end   = 136;
    }
    else if(run == 2) {
            slug_begin = 137;
            slug_end   = 320;
    }

    /* Open both rootfiles. */
    TFile* rootfile;
    rootfile= TFile::Open(rootfile_name);

    /* Grab the tree from both rootfile. */
    TTree *tree;
    tree = (TTree*)rootfile->Get("tree");

    /* Vectors for averaged value, averaged error, slug. */
    vector<double> value;
    vector<double> error;
    vector<double> n;
    vector<double> md_avg;
    vector<double> md_error;
    vector<int> slug;
    vector<int> wien;
    vector<double> avg_value;
    vector<double> avg_error;
    vector<double> avg_det_error;
    vector<double> avg_n;
    vector<int> avg_slug;
    vector<int> avg_wien;

    /* Allocate space for all slugs and initialize. */
    for(int k = 0; k < (slug_end - slug_begin)+1; k++) {
        avg_value.push_back(0);
        avg_error.push_back(0);
        avg_det_error.push_back(0);
        avg_n.push_back(0);
        avg_slug.push_back(0);
        avg_wien.push_back(0);
    }

    /* Loop over all slugs and pull out averages for each slug. */

    /* Temporary values for reading out the tree. */
    int n_events;                 // number of events (read: runlets) in the tree.
    temp_value temp_branch;       // temporary struct to read the tree out into
    temp_value temp_mdallpmtavg;  // temporary struct to read the tree out into
    int temp_slug;                // temporary slug to read the tree out into.
    int temp_wien;                // temporary wien to read the tree out into.

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    if(0 != strcmp("asym_mdallpmtavg", detector_name)) {
        tree->SetBranchAddress(detector_name, &temp_branch);
    }
    tree->SetBranchAddress("asym_mdallpmtavg", &temp_mdallpmtavg);
    tree->SetBranchAddress("slug", &temp_slug);
    tree->SetBranchAddress("wien_slug", &temp_wien);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        if(temp_branch.n > 0 && temp_mdallpmtavg.n > 0 && temp_mdallpmtavg.error != 0) {
            if(0 != strcmp("asym_mdallpmtavg", detector_name)) {
                value.push_back(temp_branch.value);
                error.push_back(temp_branch.error);
                n.push_back(temp_branch.n);
            }
            else {
                value.push_back(temp_mdallpmtavg.value);
                error.push_back(temp_mdallpmtavg.error);
                n.push_back(temp_mdallpmtavg.n);
            }
            md_avg.push_back(temp_mdallpmtavg.value);
            md_error.push_back(temp_mdallpmtavg.error);
            slug.push_back(temp_slug);
            wien.push_back(temp_wien);
        }
    }

    for(unsigned int i = 0; i < value.size(); i++) {
        for(int j = slug_begin; j <= slug_end; j++) {
            if(slug[i] == j) {
                /* Add the averages. */
                avg_value.at(j - slug_begin) += value.at(i)/(md_error.at(i)*md_error.at(i));
                avg_error.at(j - slug_begin) += 1/(md_error.at(i)*md_error.at(i));
                avg_det_error.at(j - slug_begin) += error.at(i)/(md_error.at(i)*md_error.at(i));
                avg_n.at(j - slug_begin) += n.at(i);

                if(avg_slug.at(j - slug_begin) == 0) {
                    avg_slug.at(j - slug_begin) = slug.at(i);
                    avg_wien.at(j - slug_begin) = wien.at(i);
                }
            }
        }
    }

    /* Complete the weighed average. */
    for(unsigned int i = 0; i < avg_value.size(); i++) {
        avg_value[i] = avg_value[i]/(avg_error[i]);
        avg_error[i] = avg_det_error[i]/(avg_error[i]);
    }

    if(debug) {
        for(unsigned int p = 0; p < avg_value.size(); p++) {
            cout << "slug: " << avg_slug[p] << " value: " << avg_value[p] << " error: " << avg_error[p] << endl;
        }
    }

/* Close rootfile. */
rootfile->Close();
return 0;
}
