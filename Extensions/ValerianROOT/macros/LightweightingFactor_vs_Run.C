/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This script has 2 purposes
 1) To calculate the lightweighting factor for a group of runs.
 This builds on what I did in elog:
 https://qweak.jlab.org/elog/Tracking/125

 ValerianROOT draw statement: \\
 TrackThat-$>$Draw("Q2Cut.q2Val/Q2Cut.LWPq2Val")

 Therefore getting this value for and average of runs.

 This macro is wonderful because it uses the way ValerianROOT
 was set up and envisioned to make life easier.  This is that
 it uses the data structure.  So one doesn't have to get leafs
 just grab the brach and bam you have it all.  The way
 ValerianROOT uses stucts and vectors is beautiful and was
 designed to make life easier and happy. :-) and in this arena
 happy ROOT means a happy life.

 NOTE: This won't work for simulation as these variables are not filled

 Entry Conditions:
 - pass: the pass number
 - filename: the runlist file name of interest
 Date: 01-27-2017
 Modified:
 Assisted By:
 *********************************************************/

//ROOT includes
#include <TChain.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TLeaf.h>
#include <TBranch.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TPaveStats.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TMath.h>

//standard includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric> //needed for finding average of a vector

//ValerianROOT macro includes
//include for the functions used in all macros without being customized
#include "Read_In_Run_Numbers.h"
#include "less_than_but_non_zero.h"

//ValerianROOT includes
#include "include/Q2WithCut.h"

//function definitions for those not in the header files
void Loop_Through_Tree(TChain * track_that);
void Make_graphs();
void Plot();
void PrintToFile();

//define TGraphErrors
std::vector<TGraphErrors*> g_LIGHTWEIGHTING_VS_RUN;  //[package]

//define TGraphErrors
std::vector<TMultiGraph*> mg_LIGHTWEIGHTING_VS_RUN;  //[package]

//define the Fit for the TGraph of type 4
std::vector<TF1*> f_LIGHTWEIGHTING_VS_RUN;  //[package]

//define vectors to store information for GraphError
std::vector<std::vector<Double_t> > LIGHTWEIGHTING_VAL;  //[package][run index num]
std::vector<std::vector<Double_t> > LIGHTWEIGHTING_ERROR;  //[package][run index num]
std::vector<Double_t> RUN_VAL;  //[run] - need so be double for TGraphError
std::vector<Double_t> RUN_ERROR;  //[run] always zero - needs to be double for TGraphError
//definition of integers
Int_t nentries;  // number of entries in the tree

const Int_t NUMPACKAGES = 3;
const Int_t NUMQ2TYPES = 5;

Double_t FUDGEFACTOR_LIGHTWEIGHTING = 0.001;  //the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_RUN[2] = { 0.05, 0.25 };  // the percentage add on to the X axis for pretty plots

//NOTE Pkg 0 doesn't make a lot of sense. it also over writes as there is only one
// values for run and pkg 0 is not calculated.
// index of vector to what it is
std::string INDEXTOPKG[NUMQ2TYPES] = { "Both", "1", "2" };

//array for min and may q2 values
Double_t LIGHTWEIGHTING_MIN[NUMPACKAGES] = { 0 };  //min q2 for a pkg
Double_t LIGHTWEIGHTING_MAX[NUMPACKAGES] = { 0 };  //max q2 for a pkg

//the pass number
std::string PASS;

//pass the runlist
std::string FILENAME_PREFIX;

//Fit int, are we ploting the fit or not
Int_t i_FIT = 0;  //Fit 0 is no and 1 is yes

void LightweightingFactor_vs_Run(std::string pass, std::string filename)
{
  PASS = pass;
  FILENAME_PREFIX = filename.substr(0, filename.find("."));

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");

/*
  Track_It->Add(
      Form(
          "/group/qweak/www/html/tracking/ValerianROOTFiles/Pass%s_TrackingRuns.root",
          PASS.c_str()));
*/


   Track_It->Add(
   Form(
   "/group/qweak/www/html/tracking/ValerianROOTFiles/Sim_Pass%s_TrackingRuns.root",
   PASS.c_str()));


  //run the functions on interest
  Read_In_Run_Numbers(filename);
  //for (i_FIT = 1; i_FIT < 2; i_FIT++)
  for (i_FIT = 0; i_FIT < 2; i_FIT++)
  {
    Loop_Through_Tree(Track_It);
    Make_graphs();
    Plot();
  }
  PrintToFile();

  RUNNUMLIST.clear();

  return;
}

/***********************************************************
 Function: Loop_Through_Tree
 Purpose:
 To loop over the event and fill in the histograms for the
 offset

 Entry Conditions:
 - TChain - track_that

 Global:
 - NUMPACKAGES - the number of packages

 - LIGHTWEIGHTING_VAL - vector of the Q2 values
 - LIGHTWEIGHTING_ERROR - vector of the Q2 errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 - RUNNUMLIST - the vector containing the list of runs of interest (ROI)

 - nentries - the number of entries in the TrackThat tree

 Exit Conditions: none
 Called By: LightweightingFactor_vs_Run
 Date: 01-27-2017
 Modified:
 *********************************************************/
void Loop_Through_Tree(TChain* track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyQ2Cut_t> *myQ2Cut = 0;
  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("Q2Cut", 1);
  track_that->SetBranchStatus("Run", 1);

  /*******************************
   * Now point branch to our local variables
   *
   * This take the place that ROOT puts these variables in
   * its memory and points that to our memory address
   * (so we are forcing ROOT to use the memory we allocated)
   * - neat trick
   *
   * This is done by:
   *******************************/
  //creates a memory address, which points to the memory address of myQ2* (the pointer)
  //since run is not a pointer this one just points to the run
  track_that->SetBranchAddress("Q2Cut", &myQ2Cut);
  track_that->SetBranchAddress("Run", &run);

  // Resize vectors
  RUN_VAL.resize(RUNNUMLIST.size(), 0);
  LIGHTWEIGHTING_VAL.resize(NUMPACKAGES);
  LIGHTWEIGHTING_ERROR.resize(NUMPACKAGES);

  for (int i_pkg = 1; i_pkg < NUMPACKAGES; i_pkg++)
  {
    //resize q2 vector and fill with zero
    //resize q2 vectors
    LIGHTWEIGHTING_VAL[i_pkg].resize(RUNNUMLIST.size(), 0);
    LIGHTWEIGHTING_ERROR[i_pkg].resize(RUNNUMLIST.size(), 0);
  }

  int run_index = 0;  //this is the index of a run in the RUNNUMLIST vector

  //get the number of entries in the tree
  nentries = track_that->GetEntries();
  /*
   //debugging
   std::cout << "number of entries in the tree " << nentries << std::endl;
   */

  //start looping through the tree
  for (int i_entry = 0; i_entry < nentries; i_entry++)
  {
    /*
     //debugging
     std::cout << "after starting to looping through the tree, the tree entry is "
     << i_entry << std::endl;
     */

    //get the i'th entry in the tree
    track_that->GetEntry(i_entry);

    /*
     //debugging
     std::cout << "we are looking getting the run number, " << run
     << " in the run list is: "
     << std::count(RUNNUMLIST.begin(), RUNNUMLIST.end(), run) << std::endl;
     */

    //check whether run number is in the run list
    if (std::count(RUNNUMLIST.begin(), RUNNUMLIST.end(), run) > 0)
    {
      /*
       //debugging
       std::cout << "our run is in out run list" << std::endl;
       std::cout << "for run " << run << " the count is "
       << std::count(RUNNUMLIST.begin(), RUNNUMLIST.end(), run)
       << " and we have compared them..." << std::endl;
       */

      //fill the run value vector for the plots
      RUN_VAL[run_index] = run;

      /********************
       * Now lets get the information from the vectors in ValerianROOT
       * and put them in the vectors for the plots
       *********************/

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*myQ2Cut)
       ******************/

      for (UInt_t i = 0; i < myQ2Cut->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Q2 Cut arena - we have data :-)" << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 0, 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*myQ2Cut)[i].R3package)
        {
          case 0:  //both pkgs
            // not Q2 cut info for both pkgs move along :-)
            /*
             //debugging
             std::cout << "I am Q2_cut, both pkgs, beware!" << std::endl;
             */

            break;

          case 1:  //pkg 1
          case 2:  //pkg 2
            // From Bevington pg. 44

            //ratio of values
            Double_t ratio = (*myQ2Cut)[i].q2Val / (*myQ2Cut)[i].LWPq2Val;

            // (*myQ2Cut)[i].q2Val "error"
            Double_t q2Val_RMS_calc = TMath::Power((*myQ2Cut)[i].q2Error, 2)
                / TMath::Power((*myQ2Cut)[i].q2Val, 2);

            // (*myQ2Cut)[i].LWPq2Val "error"
            Double_t LWPq2Val_RMS_calc = TMath::Power((*myQ2Cut)[i].LWPq2Error,
                                                      2)
                                         / TMath::Power((*myQ2Cut)[i].LWPq2Val,
                                                        2);

            //Fill pkg individual
            //Value
            LIGHTWEIGHTING_VAL[(*myQ2Cut)[i].R3package][run_index] = ratio;

            //Error - Bevington pg. 44
            // Error = ratio^2 (RMS_1^2/Value_1^2 + RMS_2^2/Value_2^2)
            LIGHTWEIGHTING_ERROR[(*myQ2Cut)[i].R3package][run_index] =
                TMath::Power(ratio, 2) * (q2Val_RMS_calc + LWPq2Val_RMS_calc);

/*
            //Fill both pkg - will have 2 times the entries
            //Value
            LIGHTWEIGHTING_VAL[0][run_index] = ratio;

            //Error - Bevington pg. 44
            // Error = ratio^2 (RMS_1^2/Value_1^2 + RMS_2^2/Value_2^2)
            LIGHTWEIGHTING_ERROR[0][run_index] = TMath::Power(ratio, 2)
                * (q2Val_RMS_calc + LWPq2Val_RMS_calc);
*/

/*
            //debugging,
            std::cout << "Run index: " << run_index << " \t Run: "
            << RUN_VAL[run_index] << "," << run << std::endl;
//            std::cout << "pkg 0: LightweightingVal: "
//            << LIGHTWEIGHTING_VAL[0][run_index] << " \t LightweightingError: "
//            << LIGHTWEIGHTING_ERROR[0][run_index] << std::endl;
            std::cout << "pkg 1: LightweightingVal: "
            << LIGHTWEIGHTING_VAL[1][run_index] << "\t LightweightingError: "
            << LIGHTWEIGHTING_ERROR[1][run_index] << std::endl;
            std::cout << "pkg 2: LightweightingVal: "
            << LIGHTWEIGHTING_VAL[2][run_index] << " \t LightweightingError: "
            << LIGHTWEIGHTING_ERROR[2][run_index] << std::endl << std::endl;
*/

            break;

        }  //end of switch for what PKG it is in
        //Increment run index so we can look at the next run

      }  // end of if in the runs list
      //Increment run index so we can look at the next run
      run_index++;
    }  // end of loop over runs
  }  //end of loop over entries

  return;
}

/***********************************************************
 Function: Make_graphs
 Purpose:
 To make the graphs for the 3 plots of interest
 Also make the legend

 Entry Conditions:
 none

 Global:
 - g_LIGHTWEIGHTING_VS_RUN - the vector of TGraphErrors

 - mg_LIGHTWEIGHTING_VS_RUN - the vector of mutigraph

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in

 - Q2_VAL - vector of the Q2 values
 - Q2_ERROR - vector of the Q2 errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 Exit Conditions: none
 Called By: LightweightingFactor_vs_Run
 Date: 01-23-2017
 Modified:
 *********************************************************/
void Make_graphs()
{
  /*  //debugging
   std::cout << "made it in the make graph function" << std::endl;*/

  //resize the vector of the tgraph to the number of q2 types
  g_LIGHTWEIGHTING_VS_RUN.resize(NUMPACKAGES);

  //resize the muiltigraph to number of packages
  mg_LIGHTWEIGHTING_VS_RUN.resize(NUMPACKAGES);

  //resize the fit to the number of packages
  f_LIGHTWEIGHTING_VS_RUN.resize(NUMPACKAGES);

  //Make the TGraphs with the information we got in from looping through the tree
  for (int i_pkg = 1; i_pkg < NUMPACKAGES; i_pkg++)
  {
    /*    //debugging....
     std::cout << "Pkg: " << i_pkg << std::endl;*/

    //initalize the max and min values - Max very small, Min large so they must be replaced
    LIGHTWEIGHTING_MAX[i_pkg] = -1e6;
    LIGHTWEIGHTING_MIN[i_pkg] = 1e6;

    //take the vectors and turn them into arrays so that TGraphErrors will play nice
    //with them by &(vector[0]) :) n, x, y, ex, ey
    g_LIGHTWEIGHTING_VS_RUN[i_pkg] = new TGraphErrors(
        RUNNUMLIST.size(), &(RUN_VAL[0]), &(LIGHTWEIGHTING_VAL[i_pkg][0]),
        &(RUN_ERROR[0]), &(LIGHTWEIGHTING_ERROR[i_pkg][0]));

    /*
     * Add in the attributes of the graph, blue, filled circle point down
     */
    //set it so the errors print
    gStyle->SetOptFit(1110);
    //set markercolor && error bar color - Blue
    g_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetMarkerColor(kBlue);
    g_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetLineColor(kBlue);

    //set Marker size 1 = 8 pixels
    g_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetMarkerSize(0.5);

    //set marker style - filled circle
    g_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetMarkerStyle(8);

    /*
     //Set the title, axis, etc
     g_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetTitle(
     Form("Lightweighting Factor vs. Run for package %s",
     INDEXTOPKG[i_pkg].c_str()));
     g_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetXaxis()->SetTitle("Run Number");
     g_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetYaxis()->SetTitle(
     "Lightweighting ratio");
     */

    //set it so the errors print
    gStyle->SetOptFit(1110);

    if (i_FIT == 0)
    // Don't plot fit
    {
      // Do NOTHING :)
    } else if (i_FIT == 1)
    {
      //define the fits and color it
      f_LIGHTWEIGHTING_VS_RUN[i_pkg] = new TF1(
          Form("g_LIGHTWEIGHTING_VS_RUN[%d]", i_pkg), "pol0");
      f_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetLineColor(kBlue);
      //fit this graph to a constant, and get the stats - EX0 intally
      g_LIGHTWEIGHTING_VS_RUN[i_pkg]->Fit(
          Form("g_LIGHTWEIGHTING_VS_RUN[%d]", i_pkg));
      //f_Q2_VS_RUN_TYPE4[i_pkg]->SetLineColor(kBlue);
      //g_Q2_VS_RUN[i_type][i_pkg]->Fit("pol0", "N");
      //g_Q2_VS_RUN[i_type][i_pkg]->GetFunction("pol0")->SetLineColor(kBlue);
    }  //end of if fit

    //work with the mulitigraph
    //define the multigraph
    mg_LIGHTWEIGHTING_VS_RUN[i_pkg] = new TMultiGraph(
        Form("mg_LIGHTWEIGHTING_VS_RUN[%d]", i_pkg),
        Form("Lightweighting ratio vs Run number for package %s",
             INDEXTOPKG[i_pkg].c_str()));
    //add this graph to the muitigraph
    mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->Add(g_LIGHTWEIGHTING_VS_RUN[i_pkg]);

    //get the min and max Theta values
    LIGHTWEIGHTING_MAX[i_pkg] = std::max(
        LIGHTWEIGHTING_MAX[i_pkg],
        *std::max_element(LIGHTWEIGHTING_VAL[i_pkg].begin(),
                          LIGHTWEIGHTING_VAL[i_pkg].end()));

    LIGHTWEIGHTING_MIN[i_pkg] = std::min(
        LIGHTWEIGHTING_MIN[i_pkg],
        *std::min_element(LIGHTWEIGHTING_VAL[i_pkg].begin(),
                          LIGHTWEIGHTING_VAL[i_pkg].end(),
                          less_than_but_non_zero));

  }  //end of loop over pkgs

  return;
}

/***********************************************************
 Function: Plot
 Purpose:
 To plot the graphs and save those plots

 Entry Conditions:
 none

 Global:
 - g_LIGHTWEIGHTING_VS_RUN - vector of the graph

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in

 - RUNNUMLIST - list of the run numbers - now sorted

 - LIGHTWWIGHTING_MAX - max lightweighting value for a pkg
 - LIGHTWEIGHTING_MIN - min lightweighting value for a pkg

 - FUDGEFACTOR_RUN - buffer for the RUN axis
 - FUDGEFACTOR_LIGTHWEIGHTING - buffer for the Q2 axis

 Exit Conditions: none
 Called By: LightingFactor_vs_Run
 Date: 01-27-2017
 Modified:
 *********************************************************/
void Plot()
{
  if (i_FIT == 0)  // Not ploting fit
  {
    //Canvas
    std::vector<TCanvas*> c_lightweighting;  //[pkg]
    c_lightweighting.resize(NUMPACKAGES);

    for (int i_pkg = 1; i_pkg < NUMPACKAGES; i_pkg++)
    {
      c_lightweighting[i_pkg] = new TCanvas(
          Form("c_lightweighting[%d]", i_pkg),
          Form("Lightweighting factor vs run number for %s package",
               INDEXTOPKG[i_pkg].c_str()));

      //set title and axis lables
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetTitle(
          Form(
              "Lightweighting ratio vs Run number for %s package; Run number; Lightweighting Ratio",
              INDEXTOPKG[i_pkg].c_str()));

      //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->Draw("AP");

      /*    //debugging
       std::cout << "Pkg: " << i_pkg << std::endl << " Min: " << Q2_MIN[i_pkg]
       << std::endl << " Max: " << Q2_MAX[i_pkg] << std::endl;
       std::cout << "Run Num Min: " << RUNNUMLIST.front()
       << std::endl << "Run Num Max: " << RUNNUMLIST.back() << std::endl; */

      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
          LIGHTWEIGHTING_MIN[i_pkg] - FUDGEFACTOR_LIGHTWEIGHTING,
          LIGHTWEIGHTING_MAX[i_pkg] + FUDGEFACTOR_LIGHTWEIGHTING);

      Double_t RunRange = Double_t(RUNNUMLIST.back() - RUNNUMLIST.front());

      //   original
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
          Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
          Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

      //move the axis label so can read everything
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

      //update the pad
      gPad->Update();

      c_lightweighting[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Lightweighting_vs_Run/Pass_%s_Lightweighting_vs_Run_pkg_%s_%s.png",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));

      c_lightweighting[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Lightweighting_vs_Run/Pass_%s_Lightweighting_vs_Run_pkg_%s_%s.C",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));
      c_lightweighting[i_pkg]->Clear();
      c_lightweighting[i_pkg]->Close();
    }
  }

  if (i_FIT == 1)  // ploting fit
  {
    //Canvas
    std::vector<TCanvas*> c_lightweighting_fit;  //[pkg]
    c_lightweighting_fit.resize(NUMPACKAGES);

    for (int i_pkg = 1; i_pkg < NUMPACKAGES; i_pkg++)
    {
      c_lightweighting_fit[i_pkg] = new TCanvas(
          Form("c_lightweighting[%d]", i_pkg),
          Form("Lightweighting factor vs run number for %s package",
               INDEXTOPKG[i_pkg].c_str()));

      //set title and axis lables
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->SetTitle(
          Form(
              "Lightweighting ratio vs Run number for %s package; Run number; Lightweighting Ratio",
              INDEXTOPKG[i_pkg].c_str()));

      //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->Draw("AP");

/*
      //debugging
      std::cout << std::endl << "Pkg: " << i_pkg << std::endl << " Min: "
      << LIGHTWEIGHTING_MIN[i_pkg] << std::endl << " Max: "
      << LIGHTWEIGHTING_MAX[i_pkg] << std::endl;
      std::cout << "Run Num Min: " << RUNNUMLIST.front() << std::endl
      << "Run Num Max: " << RUNNUMLIST.back() << std::endl;
*/

      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
          LIGHTWEIGHTING_MIN[i_pkg] - FUDGEFACTOR_LIGHTWEIGHTING,
          LIGHTWEIGHTING_MAX[i_pkg] + FUDGEFACTOR_LIGHTWEIGHTING);

      Double_t RunRange = Double_t(RUNNUMLIST.back() - RUNNUMLIST.front());

      //   original
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
          Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
          Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

      //move the axis label so can read everything
      mg_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

      //update the pad
      gPad->Update();

      //Add the fit on top
      //https://root.cern.ch/root/html532/TGraph.html#TGraph:Fit
      //gStyle->SetOptFit(0111); //Probality, Chi2/nDF, paramters, errors
      f_LIGHTWEIGHTING_VS_RUN[i_pkg]->Draw("same");  //("pol0")->Draw("same");
      gPad->Update();

      //do something with the stats so that they are colored like the lines
      //I have no idea what this does I found it online...
      TPaveStats* stats1 =
          (TPaveStats*) g_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetListOfFunctions()->FindObject(
              "stats");
      stats1->SetTextColor(kBlue);

      stats1->SetX1NDC(0.79);  //left
      stats1->SetX2NDC(0.99);  //right
      stats1->SetY1NDC(0.78);  //bottom
      stats1->SetY2NDC(0.90);  //top

      c_lightweighting_fit[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Lightweighting_vs_Run/Pass_%s_Lightweighting_vs_Run_withFit_pkg_%s_%s.png",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));

      c_lightweighting_fit[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Lightweighting_vs_Run/Pass_%s_Lightweighting_vs_Run_withFit_pkg_%s_%s.C",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));
      c_lightweighting_fit[i_pkg]->Clear();
      c_lightweighting_fit[i_pkg]->Close();
    }
  }

  return;
}

/***********************************************************
 Function: PrintToFile
 Purpose:
 To print out the values from the ValerianROOT used in this script.

 Entry Conditions:
 none

 Global:

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in
 - INDEXTOTYPE - array to get what Q2 type that we are in

 - LIGHTWEIGHTING_VAL - vector of the Q2 values
 - LIGHTWEIGHTING_ERROR - vector of the Q2 errors
 - RUN_VAL - vector of the run values
 these of course are 0 since there are none.

 - RUNNUMLIST - list of the run numbers - now sorted

 Exit Conditions: none
 Called By: LightweightingFactor_vs_Run
 Date: 01-27-2017
 Modified:
 *********************************************************/
void PrintToFile()
{
  //print to file
  //this file and everything related to it is fout
  std::ofstream fout;

  //open a file
  //This won't work! why I have no idea
  fout.open(
      Form(
          "/home/vmgray/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Lightweighting_vs_Run/Pass_%s_Lightweighting_vs_Run_%s.txt",
          PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
          FILENAME_PREFIX.c_str()));

  if (!fout.is_open())
    cout << "File not opened" << endl;

  fout << "Run \t pkg \t\t 0 Val & Error " << std::endl;

  for (int i_run = 0; i_run < RUNNUMLIST.size(); i_run++)
  {
    for (int i_pkg = 1; i_pkg < NUMPACKAGES; i_pkg++)
    {
      fout << RUN_VAL[i_run] << " \t " << INDEXTOPKG[i_pkg].c_str() << " \t\t "
      << LIGHTWEIGHTING_VAL[i_pkg][i_run] << " +- "
      << LIGHTWEIGHTING_ERROR[i_pkg][i_run] << std::endl;
    }  // loop over packages
    fout << std::endl;
  }  //loop over run #s

  //Print the parameters for fit
  for (int i_pkg = 1; i_pkg < NUMPACKAGES; i_pkg++)
  {
    fout << "fit parameters: " << INDEXTOPKG[i_pkg].c_str() << " \t\t p0: "
    << f_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetParameter(0) << " +/- "
    << f_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetParError(0) << " \t\t Chi2: "
    << f_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetChisquare() << " \t\t NDf: "
    << f_LIGHTWEIGHTING_VS_RUN[i_pkg]->GetNDF() << std::endl;
  }

  fout << std::endl;

  fout << "Note: both has all the pkg 1 and 2 points in it. There is no both."
  << std::endl;

  fout.close();

  return;
}
