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
#include "QwTreeBranch.h"
#include "QwTreeBranchPair.h"

using namespace std;

QwTreeBranchPair::QwTreeBranchPair(QwTreeBranch x_branch, QwTreeBranch y_branch) {
    x = x_branch;
    y = y_branch;
}

QwTreeBranch QwTreeBranchPair::get_x(void) {
    return x;
}

QwTreeBranch QwTreeBranchPair::get_y(void) {
    return y;
}
