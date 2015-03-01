/*
 * QwTreeBranch.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 */
#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <math.h>
#include "QwTreeBranch.h"

using namespace std;

/* Constructor for QwTreeBranch class. */
QwTreeBranch::QwTreeBranch(void) {
    /* Currently all we need is to init the size to 0. */
    size_of = 0;
}

/* Method to fill one entry from a branch into a QwTreeBranch */
void QwTreeBranch::fill(temp_value filler) {
    value.push_back(filler.value);
    error.push_back(filler.error);
    rms.push_back(filler.rms);
    n.push_back(filler.n);
    /* Bump the size stored in the class. This probably needs to be made more
     * rigours. */
    size_of++;
}

/* Method to fill only the runlet. */
void QwTreeBranch::fill_runlet(int filler) {
    runlet.push_back(filler);
}

/* Method to fill only the weight. */
void QwTreeBranch::fill_weight(double filler) {
    weight.push_back(filler);
}

/* Method to fill only the sign. */
void QwTreeBranch::fill_sign(int filler) {
    sign.push_back(filler);
}

/* Debugging method to print the value from a QwTreeBranch. Feel free to change
 * what this does. */
void QwTreeBranch::debug(void) {
    for (int i = 0; i < size_of; i++) {
        cout << weight[i] << endl;
    }
}

/* Getter for value. */
vector<double> QwTreeBranch::get_value(void) {
    return value;
}

/* Getter for error. */
vector<double> QwTreeBranch::get_error(void) {
    return error;
}

/* Getter for rms. */
vector<double> QwTreeBranch::get_rms(void) {
    return rms;
}

/* Getter for n. */
vector<int> QwTreeBranch::get_n(void) {
    return n;
}

/* Getter for runlet. */
vector<int> QwTreeBranch::get_runlet(void) {
    return runlet;
}

vector<double> QwTreeBranch::get_weight(void) {
    return weight;
}

vector<int> QwTreeBranch::get_sign(void) {
    return sign;
}

/* Method that removes the ith entry from all elements of the QwTreeBranch. */
void QwTreeBranch::del_index(int i) {
    value.erase(value.begin()+i);
    error.erase(error.begin()+i);
    rms.erase(rms.begin()+i);
    n.erase(n.begin()+i);
    runlet.erase(runlet.begin()+i);
    /* Prevent segfault and make sure weight is filled before removing. */
    if(!weight.empty()) {
        weight.erase(weight.begin()+i);
    }
    size_of--;
}

/* Getter for size of QwTreeBranch. */
int QwTreeBranch::size(void) {
    return size_of;
}

/* Method to extract data from the db_rootfile. */
void QwTreeBranch::get_data_from_tree(TTree* tree, TString name) {
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_branch;     // temporary struct to read the tree out into.
    int temp_runlet;            // temporary runlet to read the tree out into.
    int temp_sign;              // temporary sign to read the tree out into.

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    tree->SetBranchAddress("runlet_id", &temp_runlet);
    tree->SetBranchAddress("sign_correction", &temp_sign);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill(temp_branch);
        this->fill_runlet(temp_runlet);
        this->fill_sign(temp_sign);
    }
}

/* Method to extract data from the db_rootfile via wien. FIXME: untested.
 * FIXME: set wien as a variable. */
void QwTreeBranch::get_data_from_tree(TTree* tree, TString name, int wien) {
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_branch;     // temporary struct to read the tree out into
    int temp_runlet;            // temporary runlet to read the tree out into.
    int temp_wien;              // temporary wien to read the tree out into.
    int temp_sign;              // temporary sign to read the tree out into.

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    tree->SetBranchAddress("runlet_id", &temp_runlet);
    tree->SetBranchAddress("wien_slug", &temp_wien);
    tree->SetBranchAddress("sign_correction", &temp_sign);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        if(temp_wien == wien) {
            this->fill(temp_branch);
            this->fill_runlet(temp_runlet);
            this->fill_sign(temp_sign);
        }
    }
}

/* Method to extract data from the db_rootfile with weight. */
void QwTreeBranch::get_data_from_tree(TTree* tree, TString name, TString weight_name) {
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_branch;     // temporary struct to read the tree out into
    temp_value temp_weight;     // temporory double to read the weight out into
    int temp_runlet;            // temporary runlet to read the tree out into.
    int temp_sign;              // temporary sign to read the tree out into.

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    tree->SetBranchAddress(weight_name, &temp_weight);
    tree->SetBranchAddress("runlet_id", &temp_runlet);
    tree->SetBranchAddress("sign_correction", &temp_sign);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill(temp_branch);
        this->fill_weight(temp_weight.value);
        this->fill_runlet(temp_runlet);
        this->fill_sign(temp_sign);
    }
}

/* Method to extract data from the db_rootfile via wien with weight FIXME:
 * untested. FIXME: set wien as a variable. */
void QwTreeBranch::get_data_from_tree(TTree* tree, TString name, TString weight_name, int wien) {
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_branch;     // temporary struct to read the tree out into
    temp_value temp_weight;     // temporory double to read the weight out into
    int temp_runlet;            // temporary runlet to read the tree out into.
    int temp_wien;              // temporary wien to read the tree out into.
    int temp_sign;              // temporary sign to read the tree out into.

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    tree->SetBranchAddress(weight_name, &temp_weight);
    tree->SetBranchAddress("runlet_id", &temp_runlet);
    tree->SetBranchAddress("wien_slug", &temp_wien);
    tree->SetBranchAddress("sign_correction", &temp_sign);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        if(temp_wien == wien) {
            this->fill(temp_branch);
            this->fill_weight(temp_weight.value);
            this->fill_runlet(temp_runlet);
            this->fill_sign(temp_sign);
        }
    }
}

/* Method to extract a value from the db_rootfile and the fill it into weight. */
void QwTreeBranch::set_weight_value(TTree* tree, TString name) {
    if(weight.size() > 0) {
        cout << "Size of weight is non-zero, probably already filled!" << endl;
        return;
    }
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_weight;     // temporary struct to read the tree out into

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_weight);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill_weight(temp_weight.value);
    }
}

/* Method to extract a error from the db_rootfile and the fill it into weight. */
void QwTreeBranch::set_weight_error(TTree* tree, TString name) {
    if(weight.size() > 0) {
        cout << "Size of weight is non-zero, probably already filled!" << endl;
        return;
    }
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_weight;     // temporary struct to read the tree out into

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_weight);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill_weight(temp_weight.error);
    }
}

/* Method to extract a rms from the db_rootfile and the fill it into weight. */
void QwTreeBranch::set_weight_rms(TTree* tree, TString name) {
    if(weight.size() > 0) {
        cout << "Size of weight is non-zero, probably already filled!" << endl;
        return;
    }
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_weight;     // temporary struct to read the tree out into

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_weight);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill_weight(temp_weight.rms);
    }
}

/* Method to extract a n from the db_rootfile and the fill it into weight. */
void QwTreeBranch::set_weight_n(TTree* tree, TString name) {
    if(weight.size() > 0) {
        cout << "Size of weight is non-zero, probably already filled!" << endl;
        return;
    }
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_weight;     // temporary struct to read the tree out into

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the QwTreeBranch class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_weight);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill_weight(temp_weight.n);
    }
}

TTree* clean_tree(TTree* tree, int clean_scheme) {
    cout << clean_scheme << endl;
    /* Number of runlets in the dirty tree. */
    int entries = tree->GetEntries();

    /* Temp objects to store tree. */
    temp_value mdallpmtavg;
    temp_value uslumi;
    temp_value pmtltg;
    temp_value pmtonl;
    temp_value md9;

    /* Set branches to duplicate. */
    tree->SetBranchStatus("*",1);

    /* Grab these detectors to cut on. */
    if(clean_scheme == 1) {
        tree->SetBranchAddress("asym_mdallpmtavg", &mdallpmtavg);
        tree->SetBranchAddress("asym_uslumi_sum", &uslumi);
        tree->SetBranchAddress("asym_qwk_pmtltg", &pmtltg);
        tree->SetBranchAddress("asym_qwk_pmtonl", &pmtonl);
        tree->SetBranchAddress("asym_md9pmtavg", &md9);
    }
    else if(clean_scheme == 2) {
        tree->SetBranchAddress("yield_qwk_mdallbars", &mdallpmtavg);
        tree->SetBranchAddress("yield_uslumi_sum", &uslumi);
        tree->SetBranchAddress("yield_qwk_pmtltg", &pmtltg);
        tree->SetBranchAddress("yield_qwk_pmtonl", &pmtonl);
        tree->SetBranchAddress("yield_md9pmtavg", &md9);
    }

    /* Define object to hold new tree. */
    TFile *newfile;
    newfile = new TFile("/tmp/deleteme.root","recreate");
    TTree* tree_new;                                           
    tree_new = tree->CloneTree(0);

    for(int i = 0; i < entries; i++) {
        tree->GetEntry(i);
        if(mdallpmtavg.n > 0 && uslumi.n > 0) tree_new->Fill();
    }

    return tree_new;

}
