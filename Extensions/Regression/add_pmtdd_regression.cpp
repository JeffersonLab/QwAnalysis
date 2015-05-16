/*******************************************************************************
 * Short program that reads in a rootfile, grabs all 16 main detector tubes,
 * calculates a PMTDD, and writes it to a new rootfile.
 *
 * Author: Wade Duvall <wsduvall@jlab.org>
 ******************************************************************************/
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
#include <TH1D.h>
#include <TPaveText.h>
#include <map>

int main(Int_t argc, Char_t* argv[]) {
    TString rootfile_name = argv[1];

    /* Open rootfile to be copied. */
    TFile* rootfile_old;
    rootfile_old = TFile::Open(rootfile_name);

    /* Grab the tree from both rootfile. */
    TTree* tree_old;
    tree_old = (TTree*)rootfile_old->Get("reg");

    /* Number of runlets in the old tree. */
    int entries_old = tree_old->GetEntries();

    /* Temp objects to store tree. */
    std::vector<double> temp_main_detectors;

    /* Reserve vectors for all 16 MD's. */
    temp_main_detectors.reserve(16);

    /* Get branches needed to create PMTDD. */
    tree_old->SetBranchStatus("*",1);
    for(int i = 0; i < 8; i++) {
        tree_old->SetBranchAddress(
                Form("reg_asym_qwk_md%dneg",i+1),&temp_main_detectors[i]);
        tree_old->SetBranchAddress(
                Form("reg_asym_qwk_md%dpos",i+1),&temp_main_detectors[i+8]);
    }

    /* Create new root tree and file. */                     
    TFile* rootfile_new;
    rootfile_new = new TFile(Form("new_%s",rootfile_name.Data()),"recreate");

    /* Set branch for new leaf */
    std::vector<double> temp_pmtdd;
    temp_pmtdd.reserve(8);

    /* Clone the tree */
    TTree* tree_new;
    tree_new = tree_old->CloneTree(0);
    for(int i = 0; i < 8; i++) {
        tree_new->Branch(Form("reg_asym_md%dpmddd",i+1), &temp_pmtdd[i]);
    }

    /* Fill new tree */
    for(int i = 0; i < entries_old; i++) {
        tree_old->GetEntry(i);
        for(int j = 0; j < 8; j++) {
            temp_pmtdd[j] = temp_main_detectors[i] - temp_main_detectors[i+8];
        }
        tree_new->Fill();
    }

    /* Save rootfile. */
    rootfile_new->Write();

    return 0;
}
