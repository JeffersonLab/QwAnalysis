/*
 * QwTreeLib.cpp
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
#include "QwTreeLib.h"

using namespace std;

/* Constructor for tree_value class. */
tree_value::tree_value(void) {
    /* Currently all we need is to init the size to 0. */
    size_of = 0;
}

/* Method to fill one entry from a branch into a tree_value */
void tree_value::fill(temp_value filler) {
    value.push_back(filler.value);
    error.push_back(filler.error);
    rms.push_back(filler.rms);
    n.push_back(filler.n);
    /* Bump the size stored in the class. This probably needs to be made more
     * rigours. */
    size_of++;
}

/* Method to fill only the runlet. Should be merged into previous method. */
void tree_value::fill(int filler) {
    runlet.push_back(filler);
}

/* Debugging method to print the value from a tree_value. Feel free to change
 * what this does. */
void tree_value::debug(void) {
    for (int i = 0; i < size_of; i++) {
        cout << value[i] << endl;
    }
}

/* Getter for value. */
vector<double> tree_value::return_value(void) {
    return value;
}

/* Getter for error. */
vector<double> tree_value::return_error(void) {
    return error;
}

/* Getter for rms. */
vector<double> tree_value::return_rms(void) {
    return rms;
}

/* Getter for n. */
vector<int> tree_value::return_n(void) {
    return n;
}

/* Method that removes the ith entry from all elements of the tree_value. */
void tree_value::del_index(int i) {
    value.erase(value.begin()+i);
    error.erase(error.begin()+i);
    rms.erase(rms.begin()+i);
    n.erase(n.begin()+i);
    runlet.erase(runlet.begin()+i);
    size_of--;
}

/* Getter for size of tree_value. */
int tree_value::size(void) {
    return size_of;
}

/* Method to extract data from the db_rootfile. */
void tree_value::get_data_from_tree(TTree* tree, TString name) {
    int n_events;               // number of events (read: runlets) in the tree.
    temp_value temp_branch;     // temporary struct to read the tree out into

    /*
     * Grab number of events from the tree, reset to the beginning of the tree
     * (in case its been read before) and set the branch you wish to grab. Then
     * loop over the tree and fill the vectors in the tree_value class.
     */
    n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        this->fill(temp_branch);
    }

    /* Repeat again for the runlet_id. */
    int temp_int;   // temporary int to read the tree out into.

    tree->ResetBranchAddresses();
    tree->SetBranchAddress("runlet_id", &temp_int);

    for(int i = 0; i < n_events; i++ ) {
        tree->GetEntry(i);
        this->fill(temp_int);
    }
}

/* Legacy plotting function, they remain for now. */
TGraph* plot_vectors(std::vector<double> x, std::vector<double> y) {
    TGraph* tg = new TGraph(y.size(), &(x[0]), &(y[0]));
    return tg;
}

TGraph* plot_vectors(std::vector<int> x, std::vector<double> y, TString x_title, TString y_title) {
    /* Convert the int to a double for plotting. This way is slighly ghetto, so
     * maybe FIXME */
    std::vector<double> double_x(x.begin(), x.end());
    TGraph* tg = new TGraph(y.size(), &(double_x[0]), &(y[0]));
    tg->GetXaxis()->SetTitle(x_title);
    tg->GetYaxis()->SetTitle(y_title);
    tg->SetTitle(y_title);
    return tg;
}

TGraph* plot_vectors(std::vector<double> x, std::vector<double> y, TString x_title, TString y_title) {
    /* Convert the int to a double for plotting. This way is slighly ghetto, so
     * maybe FIXME */
    TGraph* tg = new TGraph(y.size(), &(x[0]), &(y[0]));
    tg->GetXaxis()->SetTitle(x_title);
    tg->GetYaxis()->SetTitle(y_title);
    tg->SetTitle(y_title);
    return tg;
}

TGraph* plot_vectors(std::vector<int> x, std::vector<float> y, TString x_title, TString y_title) {
    /* Convert the int to a double for plotting. This way is slighly ghetto, so
     * maybe FIXME */
    std::vector<float> double_x(x.begin(), x.end());
    TGraph* tg = new TGraph(y.size(), &(double_x[0]), &(y[0]));
    tg->GetXaxis()->SetTitle(x_title);
    tg->GetYaxis()->SetTitle(y_title);
    tg->SetTitle(y_title);
    return tg;
}
