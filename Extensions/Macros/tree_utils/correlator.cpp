/*
 * correlator.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 * Small program to correlate two variables. To compile:
 *
 * g++ `root-config --glibs --cflags` -ggdb -O0 -Wall -Wextra -o correlator correlator.cpp
 *
 * To plot var1 vs var2:
 *
 * ./correlator <var1> <var2>
 *
 * Example: 
 * ./correlator asym_qwk_md1_qwk_md5 asym_qwk_pmtltg
 *
 */
#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>

using namespace std;

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

int main(int argc, char* argv[]) {
    /* Get variable names from cli. */
    TString y_var = argv[1];
    TString x_var = argv[2];
    cout << "Plotting " << y_var << " vs " << x_var << endl;
    /* Create a pointer to the root tree. */
    TFile* rootfile;
    rootfile = TFile::Open("offoff_tree.root");

    /* Grab the tree from the rootfile. */
    TTree *tree;
    tree = (TTree*)rootfile->Get("tree");

    /* Create a TApplication. This is required for anything using the root GUI. */
    TApplication *app = new TApplication("slopes", &argc, argv);
    gStyle->SetOptFit(1111);
    /* Create a TCanvas. */
    TCanvas* c1;
    c1 = new TCanvas("c1","c1");
    c1->SetBorderMode(0);
    
    /* Define vector for x and y. */
    std::vector<double> y_value;
    std::vector<double> y_error;
    std::vector<double> x_value;
    std::vector<double> x_error;
    get_data_from_tree(tree, y_var, &y_value, &y_error);
    get_data_from_tree(tree, x_var, &x_value, &x_error);

    /* Debug statment. Uncomment to activate. */
    for(unsigned int i = 0; i < y_value.size(); i++) {
        //cout << y_value[i] << "  " << x_value[i] << endl;
    }

    TGraph* tg1;
    tg1 = new TGraphErrors(y_value.size(), // Size of vector
                           &(x_value[0]),  // Pointer to data for X
                           &(y_value[0]),  // Pointer to data for Y
                           &(x_error[0]),  // Pointer to error for X
                           &(y_error[0])); // Pointer to error for Y
    tg1->Fit("pol1","F");
    tg1->Draw("AP");
    tg1->SetTitle(y_var + " vs " + x_var);
    tg1->GetXaxis()->SetTitle(x_var);
    tg1->GetYaxis()->SetTitle(y_var);

    cout << "Done" << endl;
    app->Run();

    return 0;
}
