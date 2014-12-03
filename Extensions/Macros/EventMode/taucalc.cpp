/*
 * This program was designed to compute the average
 * "effective" deadtime from Q-weak current scans.
 * It uses TauCalc objects to actually compute the deadtimes.
 *
 * useage:
 *      compile taucalc, including the TauCalc libraries
 *      ./taucalc <filename>
 * Options include:
 *      providing a file name
 *
 * Written by Josh Magee
 * magee@jlab.org
 * October 13, 2014
*/

#include <iostream>
#include <fstream>

#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>

#include "TauCalc.h"

using namespace std;

void helpscreen(void); //helpful message
void placeAxis(std::string, std::string, std::string, TCanvas *canvas, TMultiGraph*);
//void summaryGraph(const TauCalc &, const int);

/* default run condition:
 * To Be Determined
*/
int main(int argc, char* argv[]) {
  //Create a TApplication. This is required for anything using the ROOT GUI.
  TApplication *app = new TApplication("myApp",&argc,argv);
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  //TApplication "eats" our command line arguments. Let's get them back.
  int argc_orig    = app->Argc();
  char** argv_orig = app->Argv();

  //debug is to debug overall program
  bool debug=false;

  if (debug)
   std::cout <<"Debug on..." <<std::endl;

  if (argc_orig!=2) {
    helpscreen();
    exit(0);
    return 0;
  }


  /* Each TauCalc object calculates the effective deadtime
   * for a given main detector (MD). So, I create an array
   * of TauCalcs, and then a separate array of doubles to
   * store each MD's individual deadtime.
   */

  const int num_md = 8;
  double tau_list[num_md];
  TauCalc tau_calcs[num_md];

  //initialize tau array to 0
  //and set each TauCalc objects name
  char buffer[10];
  std::string object_name;
  for(int j=0; j<num_md; j++) {
    tau_list[j] = 0;
    sprintf(buffer,"MD %i",j+1); //set internal names/graph titles
    object_name = buffer;
    tau_calcs[j].setName(object_name);
    tau_calcs[j].createCanvas();
    tau_calcs[j].setPlotColor(j+1);
  }

  /* Now our TauCalc object array is initialized and ready to roll.
   * Now we must process a filename, if given. If no filename is given,
   * one needs to insert the data by hand here, or expand the code.
   *
   * Sample code to insert by hand:
   * tau.push_data(0.05994,77.99,0.23);
   * tau.push_data(0.09118,118.56,0.25);
   * tau.push_data(1.1660,1213.55,2.26);
   */

  //list of things to read in from file
  int n, line=1;
  double i, rate, err;

  //process file name information
  //string filename = "dt.txt";
  //const char* filename_str  = filename.c_str();
  char* filename_str = argv_orig[1];
  ifstream file;
  file.open(filename_str,ios::in);

  //check if file is open. If so, continue, if not, exit.
  if ( file.is_open() ) {
    //loop through each line and store information
    if (debug) std::cout <<"File open...\n";
    while ( file.good() ) {
      file >> n;
      file >> i;
      file >> rate;
      file >> err;
      if ( file.eof() ) break;

      //check to make sure main detector numbers are valid (ie, 1-8)
      if ( !(n<=num_md && n>0) ) {
        std::cout <<"One of your main detector numbers is incorrect...\n"
          <<"Error on line " <<line <<std::endl;
        std::cout <<"Exiting program." <<std::endl;
        exit(EXIT_FAILURE);
      }

      if (debug) {
        std::cout <<n <<" \t" <<i <<" \t"
          <<rate <<" \t" <<err <<std::endl;
      }
      line++;
      //store in appropriate tau_calc object
      tau_calcs[n-1].push_data(i,rate,err);
    }
  } else { //else file isn't open
    std::cout <<"Your file " <<filename_str <<" isn't open. \nExiting!"
      <<std::endl;
    exit(EXIT_FAILURE);
  }

  file.close();
  std::cout <<"File closed..." <<std::endl;

  //now let's process the data and output the results
  for(int j=0; j<num_md; j++) {
    tau_calcs[j].processDataAutomatically();
    tau_list[j] = tau_calcs[j].getTauNs();
  }

  std::cout <<"The list of deadtimes by Main Detector number are:" <<std::endl
    <<"\tNumber \tTau (ns)" <<std::endl;
  for(int j=0; j<num_md; j++) {
    std::cout <<"\t" <<j+1 <<" \t" <<tau_list[j] <<std::endl;
  }

  /* One helpful thing would be to provide a "summary" plot,
   * where all 8 detectors are grouped onto one canvas.
   * Since this is unnecessary, this should beimplemented as
   * a separate function, which should be accessed by
   * a command line argument.
   *
   * The goal is to display the fits BUT NOT THE STATS BOXES on this.
   * this is because we want the summary plot to show the correction
   * works, and be able to see the relationship between each main detector.
   * Sadly, the ROOT memory management system makes this somewhat hard.
   *
   * So below I grab pointers to the arrays of TGraphErrors, and then
   * load them into a multigraph. Then I immediately delete the
   * TPaveStats object. I can imagine many scenarios in the future
   * where this kludge will seg fault, but it seems to work for now.
   */

  //creation of pointer arrays
  TGraphErrors *rate_graphs[num_md];
  TGraphErrors *yield_graphs[num_md];
  TGraphErrors *rel_graphs[num_md];
  TGraphErrors *corr_graphs[num_md];

  //creation of related multigraphs
  TMultiGraph *mg_rates = new TMultiGraph("mg_rates","rates");
  TMultiGraph *mg_yield = new TMultiGraph("mg_yield","yields");
  TMultiGraph *mg_rel   = new TMultiGraph("mg_rel","rel");
  TMultiGraph *mg_corr  = new TMultiGraph("mg_corr","corr");

  //create TLegend
  TLegend *leg = new TLegend(0.75,.35,0.90,0.90);
  leg->SetFillColor(0);

  //grab the pointers for each main detector and add them into the
  //array
  for(int j=0; j<num_md; j++) {
    rate_graphs[j] = tau_calcs[j].getRateGraph();
    yield_graphs[j] = tau_calcs[j].getYieldGraph();
    rel_graphs[j] = tau_calcs[j].getRelativeGraph();
    corr_graphs[j] = tau_calcs[j].getCorrectedGraph();

    mg_rates ->Add( rate_graphs[j]  );
    mg_yield ->Add( yield_graphs[j] );
    mg_rel   ->Add( rel_graphs[j]   );
    mg_corr  ->Add( corr_graphs[j]   );

    sprintf(buffer,"MD %i",j+1); //set internal names/graph titles
    object_name = buffer;
    leg->AddEntry(yield_graphs[j],object_name.data(),"lp");
  }


  //now let's delete the stat boxes...
  TPaveStats *rs,*ys,*rels,*cs;
  for(int j=0; j<num_md; j++) {
    rs = (TPaveStats*) rate_graphs[j]->GetListOfFunctions()->FindObject("stats");
    ys = (TPaveStats*) yield_graphs[j]->GetListOfFunctions()->FindObject("stats");
    rels = (TPaveStats*) rel_graphs[j]->GetListOfFunctions()->FindObject("stats");
    cs = (TPaveStats*) corr_graphs[j]->GetListOfFunctions()->FindObject("stats");
    delete rs;
    delete ys;
    delete rels;
    delete cs;
    rs = NULL;
    ys = NULL;
    rels = NULL;
    cs = NULL;
  }


  TCanvas *mycanvas = new TCanvas("mycanvas","title",1000,700);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  mycanvas->Divide(2,2);

  std::string title, xaxis, yaxis;
  mycanvas->cd(1);
  mg_rates->Draw("ap");
  title = "Rates (measured) vs. Current";
  xaxis = "Current (#muA)";
  yaxis = "Rate (kHz)";
  placeAxis(title,xaxis,yaxis,mycanvas,mg_rates);

  mycanvas->cd(2);
  mg_yield->Draw("ap");
  title = "Yield (measured) vs. Current";
  xaxis = "Current (#muA)";
  yaxis = "Yield (kHz/#muA)";
  placeAxis(title,xaxis,yaxis,mycanvas,mg_yield);
  leg->Draw("sames");

  mycanvas->cd(3);
  mg_rel->Draw("ap");
  title = "Relative Rate vs. Corrected Rate";
  xaxis = "Corrected Rate";
  yaxis = "Relative Rate";
  placeAxis(title,xaxis,yaxis,mycanvas,mg_rel);

  mycanvas->cd(4);
  mg_corr->Draw("ap");
  title = "Yield (corrected) vs. Current";
  xaxis = "Current (#muA)";
  yaxis = "Yield (kHz/#muA)";
  placeAxis(title,xaxis,yaxis,mycanvas,mg_corr);

  std::cout <<"Program finished." <<std::endl;


  //run process
  app->Run();

  return 0;
}


void helpscreen (void) {
  std::cout <<"Welcome to the Q-weak dead time calculator.\n"
    <<"This code is still a work in progress, but is functional.\n"
    <<"However, to run, first compile the libraries, and then run.\n"
    <<"You must supply a filename of data, 4 space-separated columns:\n"
    <<"MD# \tcurrent \trate \terror_on_rate\n"
    <<std::endl
    <<"Usage example:\n\t"
    <<"g++ -Wall -g `root-config --libs --cflags` -Iinclude \n"
    <<"\tsrc/TauCalc.cpp taucalc.cpp -o calc_dt\n"
    <<"./calc_dt <filename>\n";
  exit(0);
}

void placeAxis(std::string title,
    std::string xaxis,
    std::string yaxis,
    TCanvas *canvas,
    TMultiGraph *multi) {
  float size=0.04;
  float titlesize=0.05;
  float labelsize = 0.03;
  multi->SetTitle(title.data());
  multi->GetXaxis()->SetTitle(xaxis.data());
  multi->GetYaxis()->SetTitle(yaxis.data());

  canvas->Modified();
  canvas->Update();

  multi->GetYaxis()->SetTitleOffset(0.90);
  multi->GetXaxis()->SetTitleOffset(0.90);
  multi->GetYaxis()->SetTitleSize(size);
  multi->GetXaxis()->SetTitleSize(size);
  multi->GetYaxis()->SetLabelSize(labelsize);
  multi->GetXaxis()->SetLabelSize(labelsize);

  TPaveText *tit = (TPaveText*) gPad->GetPrimitive("title");
  tit->SetTextSize(titlesize);
  //gStyle->SetTitleSize(titlesize);

  gPad->SetGrid();
  canvas->Modified();
  canvas->Update();
}


/*
 * void summaryGraph(const TCalc &, const int); //prototype
 * void summaryGraph(const TauCalc &tau_calc, const int num_md) {
 * std::cout <<"This works?!\n";
 * }
 */


