/*
 * TauCalc.cpp
 * This class calculates the deadtime correction
 * for a given set of data at varying currents.
 *
 * There are ample descriptions of methods and private data members
 * in the related header file.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * October 10, 2014
 */

#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#include <TCanvas.h>
#include <TAxis.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TColor.h>

#include "TauCalc.h"

using std::vector;
using std::string;
using std::cout;

void TauCalc::setName(std::string name) {
  myName = name.data();
}

/* To push data back, we need to check and see if
 * we have stored previous values in the current
 * vector (ie, 0th element).
 *
 * If so, push back the information.
 * If it doesn't exist after looping through
 * the vector, then push_back a new current
 * onto the data vector.
 */
void TauCalc::push_data(double current, double rate, double error=0) {
  if (debug) {
    printf("Push data for %f\t%f\t%f\t%lu\n",current,rate,error,data.size());
  }

  currents.push_back(current);
  data.push_back(rate);
  data_error.push_back(error);
  haveData = true;
}

void TauCalc::createCanvas(void) {
  char buffer[100];
  sprintf(buffer,"canvas%s",myName.data());
  canvas = new TCanvas(buffer,"title",1000,700);
  canvas->Divide(2,2);
}

//graph the measured rates just for fun
//this is typically the "top left" plot
void TauCalc::graphMeasuredRates(void) {
  if (haveData != true) {
    printf("TauCalc object %s has NO data!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  size_t size = currents.size();
  double *I   = currents.data();
  double *raw = data.data();
  double *err = data_error.data();
  rate_graph = new TGraphErrors(size,I,raw,0,err);

  if (canvas) {
    canvas->cd(1);
  }

  rate_graph->Draw("ap"); //draw graph

  //now lets try to fit this...
  rate_graph->Fit("expo");
  rate_graph->GetFunction("expo")->SetLineColor(fit_color);

  //now let's create the axis titles, etc
  char buffer[100];
  std::string title, xaxis, yaxis;
  sprintf(buffer,"Rates (measured) vs. Current (%s)",myName.data());
  title = buffer;
  xaxis = "Current (#muA)";
  yaxis = "Rate (kHz)";

  //this function does all the details
  makeGraphsPretty(rate_graph,title.data(),xaxis.data(),yaxis.data());
}

void TauCalc::calculateMeasuredYields(void) {
  if (debug) std::cout <<"Inside TauCalc::calculateMeasuredYields...\n";
  if ( haveData != true ) {
    printf("TauCalc object %s has NO data!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  //make sure our yield vectors are empty otherwise we could double count
  yields.clear();
  yields_error.clear();
  if (debug) std::cout <<"yields vectors cleared...\n";
  for(size_t j=0; j<currents.size(); j++) {
    yields.push_back( (data[j]/currents[j]) );
    yields_error.push_back( (data_error[j]/currents[j]) );
  }
  haveYields = true;
}

void TauCalc::graphMeasuredYields(void) {
  if (debug) std::cout <<"Inside TauCalc::graphMeasuredYields...\n";
  if (haveYields != true) {
    printf("TauCalc object %s has NO data!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  size_t size = currents.size();
  double *I   = currents.data();
  double *yield = yields.data();
  double *err = yields_error.data();
  yield_graph = new TGraphErrors(size,I,yield,0,err);

  //this is usually plotted at upper right...
  if (canvas) {
    canvas->cd(2);
  }

  if (debug) std::cout <<"Drawing graph...\n";
  yield_graph->Draw("ap"); //draw graph

  //now lets try to fit this...
  if (debug) std::cout <<"Fitting graph...\n";
  yield_graph->Fit("pol1");

  //from the fit, we extract the "true" yield
  TF1 *fit = yield_graph->GetFunction("pol1");
  fit->SetLineColor(fit_color);
  zero_rate_yield = fit->GetParameter(0);
  if (debug) printf("True yield: %f...\n",zero_rate_yield);
  fit = NULL;

  //now let's create the axis titles, etc
  char buffer[100];
  std::string title, xaxis, yaxis;
  sprintf(buffer,"Yield (measured) vs. Current (%s)",myName.data());
  title = buffer;
  xaxis = "Current (#muA)";
  yaxis = "Yield (kHz/#muA)";

  //this function does all the details
  makeGraphsPretty(yield_graph,title.data(),xaxis.data(),yaxis.data());
}

void TauCalc::calculateTrueYields(void) {
  if (debug) std::cout <<"Inside calculateTrueYields...\n";
  yield_graph->Draw("ap"); //draw graph
  if ( haveYields != true ) {
    printf("TauCalc object %s has NO data/yield information!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  true_rates.clear();
  for(size_t j=0; j<currents.size(); j++) {
    true_rates.push_back(zero_rate_yield*currents[j]);
  }
  if (debug) std::cout <<"Leaving calculateTrueYields...\n";
}

void TauCalc::calculateRelativeRates(void) {
  if (debug) std::cout <<"Entering calculateRelativeRates...\n";
  if ( haveYields != true) {
    printf("TauCalc object %s has NO data/yield information!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  std::cout <<"Clearing vector...\n";
  relative_rate.clear();

  if (debug) {
    printf("j \tdata[j] \ttrue_yield \tquotient \tsize\n");
    for(size_t j=0; j<true_rates.size(); j++) {
      printf("%lu \t\t%f \t%f \t%f \t%lu \n",
          j,data[j],true_rates[j],data[j]/true_rates[j],true_rates.size());
    }
  }

  for(size_t j=0; j<true_rates.size(); j++) {
    if (true_rates[j]==0) continue;
    relative_rate.push_back( data[j]/true_rates[j] );
    relative_error.push_back( relative_rate[j] * yields_error[j] );
  }
  if (debug) std::cout <<"Leaving calculateRelativeRates...\n";
}

void TauCalc::graphRelativeRates(void) {
  if ( relative_rate.size() == 0 ) {
    printf("TauCalc object %s relative_rates.size()==0!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  size_t size = true_rates.size();
  double *Y   = true_rates.data();
  double *rel = relative_rate.data();
  relative_graph = new TGraphErrors(size,Y,rel,0,0);

  //this is usually plotted at upper right...
  if (canvas) {
    canvas->cd(3);
  }

  if (debug) std::cout <<"Drawing graph...\n";
  relative_graph->Draw("ap"); //draw graph

  //now lets try to fit this...
  if (debug) std::cout <<"Fitting graph...\n";
  relative_graph->Fit("expo");

  //from the fit, we extract the "true" relative
  TF1 *fit = relative_graph->GetFunction("expo");
  fit->SetLineColor(fit_color);
  tau = -1*fit->GetParameter(1);
  haveTau = true;
  if (debug) printf("Tau = %f ns...\n",tau*1000000);
  fit = NULL;

  //now let's create the axis titles, etc
  char buffer[100];
  std::string title, xaxis, yaxis;
  sprintf(buffer,"Relative Rate vs. Corrected Rate (%s)",myName.data());
  title = buffer;
  xaxis = "Corrected rate (kHz)";
  yaxis = "Measured Relative Rate";

  //this function does all the details
  makeGraphsPretty(relative_graph,title.data(),xaxis.data(),yaxis.data());
}


void TauCalc::calculateCorrectedRates(void) {
  if (debug) std::cout <<"Entering calculateCorrectedRates...\n";
  if ( haveTau != true) {
    printf("TauCalc object %s hasn't calculated Tau yet...!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  std::cout <<"Clearing vector...\n";
  yields_corrected.clear();

    printf("j \tyields[j] \ttrue_yield \tfinal\n");
    for(size_t j=0; j<true_rates.size(); j++) {
      printf("%lu \t\t%f \t%f \t%f \n",
          j,yields[j],true_rates[j],yields[j]*std::exp(true_rates[j]*tau));
    }

  for(size_t j=0; j<true_rates.size(); j++) {
    if (true_rates[j]==0) continue;
    yields_corrected.push_back( yields[j]*std::exp(true_rates[j]*tau) );
    yields_corrected_error.push_back( yields_error[j] );
  }
  if (debug) std::cout <<"Leaving calculateCorrectedRates...\n";
}
void TauCalc::print_data(void) {
  std::cout <<"The data entered are: " <<std::endl;

  printf("For Main Detector %s\n",myName.data());
  printf("Current (uA) \tRate (kHz) \tUncertainty\n");
  for(size_t i=0; i<currents.size(); i++) {
    printf("%f \t\t%f \t%f \n",currents[i],data[i],data_error[i]);
  }
}

void TauCalc::graphCorrectedRates(void) {
  if ( yields_corrected.size() == 0 ) {
    printf("TauCalc object %s yields_corrected.size()==0!\n",myName.data());
    exit(EXIT_FAILURE);
  }

  size_t size = currents.size();
  double *I   = currents.data();
  double *cor = yields_corrected.data();
  double *err = yields_corrected_error.data();
  corrected_graph = new TGraphErrors(size,I,cor,0,err);

  //this is usually plotted at upper right...
  if (canvas) {
    canvas->cd(4);
  }

  if (debug) std::cout <<"Drawing graph...\n";
  corrected_graph->Draw("ap"); //draw graph

  //now lets try to fit this...
  if (debug) std::cout <<"Fitting graph...\n";
  corrected_graph->Fit("pol1");

  //from the fit, we extract the "true" relative
  TF1 *fit = corrected_graph->GetFunction("pol1");
  fit->SetLineColor(fit_color);
  double slope = fit->GetParameter(1);
  if (debug) printf("slope = %f ...\n",slope);
  fit = NULL;

  //now let's create the axis titles, etc
  char buffer[100];
  std::string title, xaxis, yaxis;
  sprintf(buffer,"Yield (corrected) vs. Current (%s)",myName.data());
  title = buffer;
  xaxis = "Current (#muA)";
  yaxis = "Yield (kHz/#muA)";

  //this function does all the details
  makeGraphsPretty(corrected_graph,title.data(),xaxis.data(),yaxis.data());
}

//this method just makes the graphs look sexy
//because, let's face it, ROOT isn't sexy
void TauCalc::makeGraphsPretty(
    TGraphErrors *graph,
    std::string title,
    std::string xaxis,
    std::string yaxis) {

  graph->SetMarkerColor(plot_color);
  graph->SetLineColor(plot_color);
  graph->SetMarkerStyle(plot_marker);
  graph->SetMarkerSize(1.5);

  float size=0.05;
  float titlesize=0.05;
  float labelsize = 0.04;
  graph->SetTitle(title.data());
  graph->GetXaxis()->SetTitle(xaxis.data());
  graph->GetYaxis()->SetTitle(yaxis.data());

  canvas->Modified();
  canvas->Update();

  graph->GetYaxis()->SetTitleOffset(0.95);
  graph->GetXaxis()->SetTitleOffset(1.00);
  graph->GetYaxis()->SetTitleSize(size);
  graph->GetXaxis()->SetTitleSize(size);
  graph->GetYaxis()->SetLabelSize(labelsize);
  graph->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *tit = (TPaveText*) gPad->GetPrimitive("title");
  tit->SetTextSize(titlesize);
  //gStyle->SetTitleSize(titlesize);

  TPaveText *stats = (TPaveText*) gPad->GetPrimitive("stats");
  stats->SetFillColor(0);
  stats->SetX1NDC(0.54);
  stats->SetY1NDC(0.70);
  stats->SetX2NDC(0.90);
  stats->SetY2NDC(0.90);

  gPad->SetGrid();
  canvas->Modified();
  canvas->Update();
}

//reset TauCalc object
void TauCalc::reset() {
  currents.clear();
  data.clear();
  data_error.clear();
  yields_corrected.clear();
  yields_corrected_error.clear();

  yields.clear();
  yields_error.clear();

  setPlotColor("default");

  haveData   = false;
  haveYields = false;
  haveTau    = false;
  correctedData = false;

}

void TauCalc::processDataAutomatically(void) {
  if (haveData == false) {
    std::cout <<"Sorry but you need data to process...\n";
    return;
  }

  graphMeasuredRates();

  calculateMeasuredYields();
  graphMeasuredYields();

  calculateTrueYields();

  calculateRelativeRates();
  graphRelativeRates();

  calculateCorrectedRates(); //this is the sanity check
  graphCorrectedRates();

}

TGraphErrors* TauCalc::getRateGraph(void) {
  return rate_graph;
}

TGraphErrors* TauCalc::getYieldGraph(void) {
  return yield_graph;
}

TGraphErrors* TauCalc::getRelativeGraph(void) {
  return relative_graph;
}

TGraphErrors* TauCalc::getCorrectedGraph(void) {
  return corrected_graph;
}

double TauCalc::getTauNs(void) {
  if (haveTau)
    return tau*1000000;
  else
    return 0.0;
}

double TauCalc::getZeroRate(void) {
  if (haveYields)
    return zero_rate_yield;
  else
    return 0;
}

void TauCalc::setPlotColor(std::string color) {
  if (color == "blue") {
    plot_color  = kBlue+2;
    fit_color   = kBlue+2;
    plot_marker = 20;
  } else if (color == "red") {
    plot_color  = kRed+3;
    fit_color   = kRed+3;
    plot_marker = 21;
  } else if (color == "green") {
    plot_color  = kGreen+3;
    fit_color   = kGreen+3;
    plot_marker = 22;
  } else if (color == "magenta") {
    plot_color  = kMagenta-1;
    fit_color   = kMagenta-1;
    plot_marker = 23;
  } else if (color == "gray") {
    plot_color  = kGray+2;
    fit_color   = kGray+2;
    plot_marker = 24;
  } else if (color == "orange") {
    plot_color  = kOrange+7;
    fit_color   = kOrange+7;
    plot_marker = 25;
  } else if (color == "cyan") {
    plot_color  = kCyan+3;
    fit_color   = kCyan+3;
    plot_marker = 26;
  } else if (color == "black") {
    plot_color  = kBlack;
    fit_color   = kBlack;
    plot_marker = 29;
  } else {
    plot_color  = kBlue+2;
    fit_color   = kRed+2;
    plot_marker = 0;
  }
}

void TauCalc::setPlotColor(int n) {
  switch (n) {
    case 1:
      setPlotColor("blue");
      break;
    case 2:
      setPlotColor("red");
      break;
    case 3:
      setPlotColor("green");
      break;
    case 4:
      setPlotColor("magenta");
      break;
    case 5:
      setPlotColor("gray");
      break;
    case 6:
      setPlotColor("orange");
      break;
    case 7:
      setPlotColor("cyan");
      break;
    case 8:
      setPlotColor("black");
      break;
    default:
      setPlotColor("default");
      break;
  }
}

//default constructor
TauCalc::TauCalc(void) {
  setName("myName");
  reset();
}

//constructor if given a name
TauCalc::TauCalc(std::string name) {
  setName(name);
  reset();
}

//default destructor
TauCalc::~TauCalc(void) {
}


