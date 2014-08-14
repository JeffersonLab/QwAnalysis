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

/* Two quickly thrown together functions to get the max and min sizes of vectors. */
double max_vector(vector<double> vector) {
    double max = vector.at(0);
    for(unsigned int i = 1; i < vector.size(); i++) {
        if(vector.at(i) > max) max = vector.at(i);
    }
    return max;
}

double min_vector(vector<double> vector) {
    double min = vector.at(0);
    for(unsigned int i = 1; i < vector.size(); i++) {
        if(vector.at(i) < min) min = vector.at(i);
    }
    return min;
}

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

/* Clean out the zero entries.
 *
 * Clean out all the zeros. This needs to be done betterer, but for now
 * this works. Will implement Pauls fix soon.
 */
void QwTreeBranchPlot::CleanZeroes(void) {
    int size = this->size();
    for(int i = 0; i < size; i++) {
        if(get_value()[i] < -1e5) {
            del_index(i);
            i--;
            size--;
        }
    }
}

/* Plots value vs runlet. This is just the start, more work needed here. */
void QwTreeBranchPlot::ValueRunletPlot(void) {
    /*
     * Convert the runlet vector<int> into a vector<double> for plotting. Only
     * works in clang, changing to a for loop for now, till I can figure it
     * out.
     */ 
    //std::vector<double> double_runlet(this->get_runlet().begin(), this->get_runlet().end());
    std::vector<double> double_runlet;
    for(int i = 0; i < this->size(); i++) 
        double_runlet.push_back((double)this->get_runlet()[i]);

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

/* Plots value vs runlet, sign corrected. This is just the start, more work needed here. */
void QwTreeBranchPlot::ValueRunletPlotSignCorr(void) {
    /*
     * Convert the runlet vector<int> into a vector<double> for plotting. Only
     * works in clang, changing to a for loop for now, till I can figure it
     * out.
     */ 
    //std::vector<double> double_runlet(this->get_runlet().begin(), this->get_runlet().end());
    std::vector<double> double_runlet;
    for(int i = 0; i < this->size(); i++) 
        double_runlet.push_back((double)this->get_runlet()[i]);

    /* Sign correct value. */
    std::vector<double> value_sign_corr;
    for(int i = 0; i < this->size(); i++)
        value_sign_corr.push_back(this->get_value()[i] * (double)this->get_sign()[i]);

    /* Plot value vs runlet. */
    TGraphErrors* temp_plot = new TGraphErrors(this->size(),
                                               &(double_runlet[0]),
                                               &(value_sign_corr[0]),
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

/* Plots value vs runlet, weighted. This is just the start, more work needed here. */
void QwTreeBranchPlot::ValueRunletPlotWeight(void) {
    /* Check if there is actually a weight. */
    if(get_weight().empty()) {
        cout << "Weight not filled. Exiting." << endl;
        exit(0);
    }

    /*
     * Convert the runlet vector<int> into a vector<double> for plotting. Only
     * works in clang, changing to a for loop for now, till I can figure it
     * out.
     */ 
    //std::vector<double> double_runlet(this->get_runlet().begin(), this->get_runlet().end());
    std::vector<double> double_runlet;
    for(int i = 0; i < this->size(); i++) 
        double_runlet.push_back((double)this->get_runlet()[i]);
    
    /* Weighted value. */
    std::vector<double> value_sign_weight;
    for(int i = 0; i < this->size(); i++)
        value_sign_weight.push_back(this->get_value()[i] *
                                    TMath::Power(1/this->get_weight()[i],2));

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

/* Plots value vs runlet, sign corrected and weighted. This is just the start, more work needed here. */
void QwTreeBranchPlot::ValueRunletPlotSignCorrWeight(void) {
    /*
     * Convert the runlet vector<int> into a vector<double> for plotting. Only
     * works in clang, changing to a for loop for now, till I can figure it
     * out.
     */ 
    //std::vector<double> double_runlet(this->get_runlet().begin(), this->get_runlet().end());

    /* Check if there is actually a weight. */
    if(get_weight().size() == 0) {
        cout << "Weight not filled. Exiting." << endl;
        exit(0);
    }

    std::vector<double> double_runlet;
    for(int i = 0; i < this->size(); i++) 
        double_runlet.push_back((double)this->get_runlet()[i]);

    /* Weighted, sign corrected value. */
    std::vector<double> value_weight_sign_corr;
    for(int i = 0; i < this->size(); i++)
        value_weight_sign_corr.push_back(this->get_value()[i] *
                                        (double)this->get_sign()[i] *
                                        TMath::Power(1/this->get_weight()[i],2));

    /* Plot value vs runlet. */
    TGraphErrors* temp_plot = new TGraphErrors(this->size(),
                                               &(double_runlet[0]),
                                               &(value_weight_sign_corr[0]),
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
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,2.5,-2.5);

    /* Get necessary data from object. */
    vector<double> fill_value = this->get_value();
    /* Fill the histo. */
    for(unsigned int i = 0; i < fill_value.size(); i++){
        temp_histo->Fill(fill_value[i]);
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
    vector<double> fill_value = this->get_value();
    vector<int> sign = this->get_sign();
    /* Fill the histo. */
    for(unsigned int i = 0; i < fill_value.size(); i++){
        temp_histo->Fill((double)sign[i]*fill_value[i]);
    }
    
    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

/* Plots a histo of value. */
void QwTreeBranchPlot::ValuePlotWeight(void) {

    /* Check if there is actually a weight. */
    if(get_weight().size() == 0) {
        cout << "Weight not filled. Exiting." << endl;
        exit(0);
    }

    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,2.5,-2.5);

    /* Get necessary data from object. */
    vector<double> fill_value = this->get_value();
    vector<double> weight = this->get_weight();
    /* Fill the histo. */
    for(unsigned int i = 0; i < fill_value.size(); i++){
        temp_histo->Fill(fill_value[i]/weight[i], TMath::Power(1/weight[i],2));
    }

    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

/* Plots a histo of value, sign corrected. */
void QwTreeBranchPlot::ValuePlotSignCorrWeight(void) {

    /* Check if there is actually a weight. */
    if(get_weight().size() == 0) {
        cout << "Weight not filled. Exiting." << endl;
        exit(0);
    }

    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo = new TH1D("h1",plot_label, 100,min_vector(this->get_value()),max_vector(this->get_value()));

    /* Get necessary data from object. */
    vector<double> fill_value = this->get_value();
    vector<int> sign = this->get_sign();
    vector<double> weight = this->get_weight();
    /* Fill the histo. */
    for(unsigned int i = 0; i < fill_value.size(); i++){
        temp_histo->Fill((double)sign[i]*fill_value[i], TMath::Power(1/weight[i],2));
    }

    /* Set label. Use the y axis since it works with the ValueRunletPlot (y_axis is the value). */
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

/* Plots pull plot of value. */
void QwTreeBranchPlot::ValuePullPlot(void) {
    /*
     * Create a histo. The values are taken from Josh's script, will be added to the class later.
     * FIXME: The way the histogram is sized/bin number is set is ghetto.
     */
    TH1D* temp_histo_mean = new TH1D("h3",plot_label, 100,min_vector(this->get_value()),max_vector(this->get_value()));

    /* Get necessary data from object. */
    vector<double> fill_value = this->get_value();
    vector<double> fill_error = this->get_error();
    vector<int> sign = this->get_sign();
    /* Fill the histo. */
    for(unsigned int i = 0; i < fill_value.size(); i++){
        temp_histo_mean->Fill((double)sign[i]*fill_value[i]);
    }

    double mean = temp_histo_mean->GetMean(1);

    TH1D* temp_histo = new TH1D("h4",plot_label, 100, -5, 5);
    for(unsigned int i = 0; i < fill_value.size(); i++){
        double difference = (double)sign[i]*fill_value[i] - mean;
        temp_histo->Fill(difference/fill_error[i]);
    }
    temp_histo->GetXaxis()->SetTitle(y_axis_label);
    temp_histo->Draw();
    temp_histo->Fit("gaus");
}

void QwTreeBranchPlot::RMSRunletPlot(void) {
    /*
     * Convert the runlet vector<int> into a vector<double> for plotting. Only
     * works in clang, changing to a for loop for now, till I can figure it
     * out.
     */ 
    //std::vector<double> double_runlet(this->get_runlet().begin(), this->get_runlet().end());
    std::vector<double> double_runlet;
    for(int i = 0; i < this->size(); i++) 
        double_runlet.push_back((double)this->get_runlet()[i]);

    /* Plot value vs runlet. */
    TGraphErrors* temp_plot = new TGraphErrors(this->size(),
                                               &(double_runlet[0]),
                                               &(this->get_rms()[0]),
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
