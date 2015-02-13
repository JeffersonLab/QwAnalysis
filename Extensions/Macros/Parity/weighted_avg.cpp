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
    TString det = argv[1];
    TString rootfile_name = argv[2];

    /* Open both rootfiles. */
    TFile* rootfile;
    rootfile= TFile::Open(rootfile_name);

    /* Grab the tree from both rootfile. */
    TTree *tree;
    tree = (TTree*)rootfile->Get("tree");

    /* Create a TApplication. This is required for anything using the ROOT GUI. */
    TApplication *app = new TApplication("slopes", &argc, argv);

    /* Vectors for averaged value, averaged error, slug. */
    vector<double> yield;
    vector<double> error;
    vector<double> run;

    /* Temporary variables */
    temp_value temp_yield;
    int temp_run;
    int temp_sign_correction;

    int n_events = (int)tree->GetEntries();     
    tree->ResetBranchAddresses();
    tree->SetBranchAddress(Form("%s",det.Data()), &temp_yield);
    tree->SetBranchAddress("runlet_id", &temp_run);
    tree->SetBranchAddress("sign_correction", &temp_sign_correction);

    for(int i = 0; i < n_events; i++) {
        tree->GetEntry(i);
        if(temp_yield.n > 0) {
            yield.push_back(temp_sign_correction*temp_yield.value);
            error.push_back(temp_yield.error);
            run.push_back(temp_run);
        }
    }

    // Calculate weighted average
    double average = 0;
    double w_error = 0;
    for(int i = 0; i < yield.size(); i++) {
        average += yield[i]*TMath::Power(error[i],-2);
        w_error += TMath::Power(error[i],-2);
    }
    average = average/w_error;
    w_error = 1/TMath::Sqrt(w_error);
    //cout << yield.size() << endl;
    std::cout << average*1000 << " +/- " << w_error*1000 << std::endl;

    /* gStyle options stolen from Manolis. */
    gROOT->SetStyle("Plain");

    gStyle->SetCanvasColor(0);
    gStyle->SetPadColor(0);
    gStyle->SetFrameFillColor(-1);
    gStyle->SetTitleFillColor(0);
    gStyle->SetFillColor(0);
    gStyle->SetStatStyle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetCanvasBorderSize(1);
    gStyle->SetFrameBorderSize(1);
    gStyle->SetLegendBorderSize(1);
    gStyle->SetStatBorderSize(0);
    gStyle->SetTitleBorderSize(0);

    gStyle->SetPadColor(0); 
    gStyle->SetPadBorderMode(1);
    gStyle->SetPadBorderSize(1);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadRightMargin(0.06);
    gStyle->SetPadLeftMargin(0.06);

    gStyle->SetLabelSize(.04,"xyzh");
    gStyle->SetTitleSize(.07,"h");
    gStyle->SetTitleW(.8);
    gStyle->SetTitleSize(.045,"xzy");
    gStyle->SetLabelOffset(0.01,"xz");
    gStyle->SetLabelOffset(0.01,"y");
    gStyle->SetTitleOffset(1.1,"xz");
    gStyle->SetTitleOffset(0.6,"y");

    gStyle->SetOptStat(1111);
    gStyle->SetOptFit(1111);

    gStyle->SetPadGridX(kTRUE);
    gStyle->SetPadGridY(kTRUE);

    //TCanvas* tc;
    //tc = new TCanvas("tc");
    //tc->Draw();
    //TPad* pad1 = new TPad("pad1","pad1",0.005,0.900,0.990,0.990);
    //TPad* pad2 = new TPad("pad2","pad2",0.005,0.005,0.990,0.900);
    //pad1->SetFillColor(0);
    //pad1->Draw();
    //pad2->Draw();
    //pad2->SetFillColor(0);
    //pad1->cd();
    //TPaveText *text = new TPaveText(.05,.1,.95,.8);
    //text->AddText(Form("Run 2 uslumi yields for tube %s",det.Data()));
    //text->Draw();

    //pad2->cd();
    //TGraph* tg = new TGraph(yield.size(), &(run[0]), &(yield[0]));
    //tg->SetMarkerStyle(kFullSquare);
    //tg->SetMarkerSize(0.5);
    //tg->SetTitle(Form(";run;%s",det.Data()));
    //tg->Draw("AP");

    /* Close rootfile. */
    rootfile->Close();
    /* TApplication crap. */
    //app->Run();
    return 0;
}
