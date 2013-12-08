/*
 * QwTreeBranchPlot.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 * Adapted from code written by:
 * Josh Magee
 * Josh Hoskins
 *
 */

#include "QwTreeBranchPlot.h"
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TMath.h>

/* Empty constructor. */
QwTreeBranchPlot::QwTreeBranchPlot(void):
    QwTreeBranch() {}

/* Constructor sets the plot names. Will probably add more here as needed. */
QwTreeBranchPlot::QwTreeBranchPlot(TString temp_plot_label, TString temp_x_axis_label,
        TString temp_y_axis_label):
    QwTreeBranch() {
        plot_label = temp_plot_label;
        x_axis_label = temp_x_axis_label;
        y_axis_label = temp_y_axis_label;
}

/* Plots value vs runlet. This is just the start, more work needed here. */
void QwTreeBranchPlot::ValueRunletPlot(void) {
    /* Convert the runlet vector<int> into a vector<double> for plotting. */
    std::vector<double> double_runlet(this->get_runlet().begin(), this->get_runlet().end());

    /* Plot value vs runlet. */
    TGraphErrors* temp_plot = new TGraphErrors(this->size(),
                                               &(double_runlet[0]),
                                               &(this->get_value()[0]),
                                               0,
                                               &(this->get_error()[0]));
    /* Set style. Do we want to store style info in the class? */
    temp_plot->SetMarkerStyle(kFullSquare);
    temp_plot->SetMarkerSize(1);
    temp_plot->SetMarkerColor(kBlack);

    /* Draw. */
    temp_plot->Draw("AP");

    /* Set axis and plot labels. */
    temp_plot->SetTitle(plot_label);
    temp_plot->GetXaxis()->SetTitle(x_axis_label);
    temp_plot->GetYaxis()->SetTitle(y_axis_label);

    /* Fit. */
    temp_plot->Fit("pol0");
}

/* Plots a histo of value. */
void QwTreeBranchPlot::ValuePlot(void) {
    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,-400,400);

    /* Get necessary data from object. */
    vector<double> fill_data = this->get_value();
    cout << fill_data.size() << endl;
    /* Fill the histo. */
    for(int i = 0; i < fill_data.size(); i++){
        temp_histo->Fill(fill_data[i]);
    }

    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

/* Plots a histo of value, sign corrected. */
void QwTreeBranchPlot::ValuePlotSignCorr(void) {
    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,2.5,-2.5);

    /* Get necessary data from object. */
    vector<double> fill_data = this->get_value();
    vector<int> sign = this->get_sign();
    cout << fill_data.size() << endl;
    /* Fill the histo. */
    for(int i = 0; i < fill_data.size(); i++){
        temp_histo->Fill((double)sign[i]*fill_data[i]);
    }
    
    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

/* Plots a histo of value. */
void QwTreeBranchPlot::ValuePlotWeight(void) {
    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,2.5,-2.5);

    /* Get necessary data from object. */
    vector<double> fill_data = this->get_value();
    vector<double> weight = this->get_weight();
    cout << fill_data.size() << endl;
    /* Fill the histo. */
    for(int i = 0; i < fill_data.size(); i++){
        temp_histo->Fill(fill_data[i]/weight[i], TMath::Power(1/weight[i],2));
    }

    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

/* Plots a histo of value, sign corrected. */
void QwTreeBranchPlot::ValuePlotSignCorrWeight(void) {
    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,2.5,-2.5);

    /* Get necessary data from object. */
    vector<double> fill_data = this->get_value();
    vector<int> sign = this->get_sign();
    vector<double> weight = this->get_weight();
    cout << fill_data.size() << endl;
    /* Fill the histo. */
    for(int i = 0; i < fill_data.size(); i++){
        temp_histo->Fill((double)sign[i]*fill_data[i], TMath::Power(1/weight[i],2));
    }

    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}
