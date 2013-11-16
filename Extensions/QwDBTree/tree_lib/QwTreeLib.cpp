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

tree_value::tree_value(void) {
    size_of = 0;
}

void tree_value::fill(temp_value filler) {
    value.push_back(filler.value);
    error.push_back(filler.error);
    rms.push_back(filler.rms);
    n.push_back(filler.n);
    size_of++;
}

void tree_value::fill(int filler) {
    runlet.push_back(filler);
}

void tree_value::debug(void) {
    for (int i = 0; i < size_of; i++) {
        cout << value[i] << endl;
    }
}

vector<double> tree_value::return_value(void) {
    return value;
}

vector<double> tree_value::return_error(void) {
    return error;
}

vector<double> tree_value::return_rms(void) {
    return rms;
}

vector<int> tree_value::return_n(void) {
    return n;
}

void tree_value::del_index(int i) {
    value.erase(value.begin()+i);
    error.erase(error.begin()+i);
    rms.erase(rms.begin()+i);
    n.erase(n.begin()+i);
    runlet.erase(runlet.begin()+i);
    size_of--;
}

int tree_value::size(void) {
    return size_of;
}


void get_data_from_tree(TTree* tree, TString name, tree_value* data) {
    int n_events;
    n_events = (int)tree->GetEntries();
    temp_value temp_branch;
    int temp_int;
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        data->fill(temp_branch);
    }
    tree->ResetBranchAddresses();
    tree->SetBranchAddress("runlet_id", &temp_int);
    for(int i = 0; i < n_events; i++ ) {
        tree->GetEntry(i);
        data->fill(temp_int);
    }
}

void get_single_value_from_tree(TTree* tree, TString name, std::vector<double>* value) {
    int n_events;
    n_events = (int)tree->GetEntries();
    double temp_branch[1];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
    }
}

void get_single_value_from_tree(TTree* tree, TString name, std::vector<float>* value) {
    int n_events;
    n_events = (int)tree->GetEntries();
    float temp_branch[1];
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
    int temp_branch[1];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        value->push_back(temp_branch[0]);
    }
}

void get_single_value_from_tree(TTree* tree, TString name, std::vector<TString>* value) {
    int n_events;
    n_events = (int)tree->GetEntries();
    char temp_branch[256];
    cout << name.Data() << endl;
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(name, &temp_branch);
    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        TString temp_str = string(temp_branch);
        value->push_back(temp_branch);
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
