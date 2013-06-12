/*
 * wshutter.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 * Small program to correlate two variables, one of which is charge normalized. To compile:
 *
 * g++ `root-config --glibs --cflags` -ggdb -O0 -Wall -Wextra -o correlator correlator.cpp
 *
 * To plot var1 vs var2:
 *
 * ./correlator <var1> <var2> <rootfile>
 *
 * Example: 
 * ./correlator asym_qwk_md1_qwk_md5 asym_qwk_pmtltg offoff_tree.root
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

void get_data_from_tree(TTree* tree, TString name, std::vector<double>* value) {
    int n_events;
    n_events = (int)tree->GetEntries();
    double temp_branch[4];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
    }
}

void get_data_from_tree(TTree* tree, TString name, std::vector<double>* value, std::vector<double>* error) {
    int n_events;
    n_events = (int)tree->GetEntries();
    double temp_branch[4];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
        error->push_back(temp_branch[1]);
    }
}

void get_data_from_tree(TTree* tree, TString name, std::vector<double>* value, std::vector<double>* error, std::vector<double>* rms) {
    int n_events;
    n_events = (int)tree->GetEntries();
    double temp_branch[4];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
        error->push_back(temp_branch[1]);
        rms->push_back(temp_branch[2]);
    }
}

void get_data_from_tree(TTree* tree, TString name, std::vector<double>* value, std::vector<double>* error, std::vector<double>* rms, std::vector<double>* n) {
    int n_events;
    n_events = (int)tree->GetEntries();
    double temp_branch[4];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
        error->push_back(temp_branch[1]);
        rms->push_back(temp_branch[2]);
        n->push_back(temp_branch[3]);
    }
}

void get_single_value_from_tree(TTree* tree, TString name, std::vector<double>* value) {
    int n_events;
    n_events = (int)tree->GetEntries();
    double temp_branch[4];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
    }
}

void get_single_value_from_tree(TTree* tree, TString name, std::vector<int>* value) {
    int n_events;
    n_events = (int)tree->GetEntries();
    int temp_branch[4];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
    }
}

void plot_histo_branch_value(TTree* tree, TString name) {
    tree->Draw(Form("%s.value",name.Data()),Form("%s.n>0",name.Data()));
}

void plot_histo_single_value(TTree* tree, TString name) {
    tree->Draw(Form("%s",name.Data()));
}

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
    return tg;
}
