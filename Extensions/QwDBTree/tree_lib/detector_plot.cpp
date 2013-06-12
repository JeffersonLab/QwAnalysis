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
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <math.h>
#include "QwTreeLib.h"

int main(int argc, char* argv[]) {
    /* Get variable names from cli. */
    TString detector_name = argv[1];
    TString rootfile_var = argv[2];
    /* Create a pointer to the root tree. */
    TFile* rootfile;
    rootfile = TFile::Open(rootfile_var);

    /* Grab the tree from the rootfile. */
    TTree *tree;
    tree = (TTree*)rootfile->Get("tree");

    /* Create a TApplication. This is required for anything using the ROOT GUI. */
    TApplication *app = new TApplication("slopes", &argc, argv);
    gStyle->SetOptFit(1111);

    /* Get inital variables needed for checks. */
    vector<double> detector;
    vector<double> qwk_charge;
    vector<int> runlet;
    vector<int> run;
    get_data_from_tree(tree, detector_name, &detector);
    get_data_from_tree(tree, "asym_qwk_charge", &qwk_charge);
    get_single_value_from_tree(tree, "run", &run);
    get_single_value_from_tree(tree, "runlet_id", &runlet);

    /* remove all the zeros that come from regression failures */
    for(std::vector<double>::iterator it = detector.begin(); it != detector.end();) {
        if(*it == -1e6) {
            detector.erase(it);
        }
        else
            it++;
    }

    TCanvas* c;
    c = new TCanvas("c"); 
    TGraph* tg;
    tg = plot_vectors(runlet, detector, "runlet_id",detector_name);
    tg->SetMarkerStyle(kFullSquare);
    tg->SetMarkerSize(.25);
    tg->Draw("AP");

    /* Output all runlets with asymmetry greater than 10 */
    for(int i = 0; i < detector.size(); i++) {
        if(detector[i] > 10)
            cout << run[i] << "  " << runlet[i] << "  " << detector[i] << endl;
    }


    cout << "Done" << endl;
    app->Run();

    return 0;
}
