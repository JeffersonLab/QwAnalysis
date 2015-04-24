/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This script has 2 purposes
 1) To serve as testing for the development of ValerianROOT
 2) be an example of how to manage the ValerianROOT Tree and
 get plots out, as this ROOT file does not have a structure
 which supports simple Draw statements (sorry to those of
 you who like them)
 3) To plot the Q2 (all versions) vs run number

 This macro is wonderful because it uses the way ValerianROOT
 was set up and envisioned to make life easier.  This is that
 it uses the data structure.  So one doesn't have to get leafs
 just grab the brach and bam you have it all.  The way
 ValerianROOT uses stucts and vectors is beautiful and was
 designed to make life easier and happy. :-) and in this arena
 happy ROOT means a happy life.

 Entry Conditions:
 - pass: the pass number
 - filename: the runlist file name of interest
 Date: 05-13-2014
 Modified: 02-13-2014
 Assisted By: Wouter Deconinck
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
#include <TLegend.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>

//standard includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

//ValerianROOT macro includes
//include for the functions used in all macros without being customized
#include "Read_In_Run_Numbers.h"
#include "less_than_but_non_zero.h"

//ValerianROOT includes
#include "include/Q2WithCut.h"
#include "include/Q2WithLoss.h"
#include "include/Q2WithOutLoss.h"

//function definitions for those not in the header files
void Loop_Through_Tree(TChain * track_that);
void Make_graphs();
void Plot();
void PrintToFile();

//define TGraphErrors
std::vector<std::vector<TGraphErrors*> > g_Q2_VS_RUN;  //[Q2 type][package]

//define the muiltigraph
std::vector<TMultiGraph*> mg_Q2_VS_RUN;  //[package]

//Legend for the plot
std::vector<TLegend*> LEGEND;  //[package]

//define vectors to store information for GraphError
std::vector<std::vector<std::vector<Double_t> > > Q2_VAL;  //[Q2 type][package][run index num]
std::vector<std::vector<std::vector<Double_t> > > Q2_ERROR;  //[Q2 type][package][run index num]
std::vector<Double_t> RUN_VAL;  //[run] - need so be double for TGraphError
std::vector<Double_t> RUN_ERROR;  //[run] always zero - needs to be double for TGraphError
//definition of integers
Int_t nentries;  // number of entries in the tree

const Int_t NUMPACKAGES = 3;
const Int_t NUMQ2TYPES = 5;

Double_t FUDGEFACTOR_Q2 = 0.1;  //m(GeV)^2 the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_RUN[2] = { 0.05, 0.25 };  // the percentage add on to the X axis for pretty plots

// index of vector to what it is
std::string INDEXTOQ2[NUMQ2TYPES] = { "with No Cuts", "with lightweighting",
    "with lightweighting and Pedestal subtracted", "with Loss", "without Loss" };
std::string INDEXTOPKG[NUMQ2TYPES] = { "Both", "1", "2" };

//array for min and may q2 values
Double_t Q2_MIN[NUMPACKAGES] = { 0 };  //min q2 for a pkg
Double_t Q2_MAX[NUMPACKAGES] = { 0 };  //max q2 for a pkg

//the pass number
std::string PASS;

void Q2_vs_Run(std::string pass, std::string filename)
{
  PASS = pass;

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");
  Track_It->Add(
      Form(
          "/group/qweak/www/html/tracking/ValerianROOTFiles/Pass%s_TrackingRuns.root",
          PASS.c_str()));

  //run the functions on interest
  Read_In_Run_Numbers(filename);
  Loop_Through_Tree(Track_It);
  Make_graphs();
  Plot();
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
 - NUMQ2TYPE - the number of different Q2 types
 - NUMPACKAGES - the number of packages

 - Q2_VAL - vector of the Q2 values
 - Q2_ERROR - vector of the Q2 errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 - RUNNUMLIST - the vector containing the list of runs of interest (ROI)

 - nentries - the number of entries in the TrackThat tree

 Exit Conditions: none
 Called By: Q2_vs_Run
 Date: 05-31-2014
 Modified: 06-04-2014
 *********************************************************/
void Loop_Through_Tree(TChain* track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyQ2Cut_t>* myQ2Cut = 0;
  std::vector<MyQ2Loss_t>* myQ2Loss = 0;
  std::vector<MyQ2NoLoss_t>* myQ2NoLoss = 0;
  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("Q2Cut", 1);
  track_that->SetBranchStatus("Q2Loss", 1);
  track_that->SetBranchStatus("Q2NoLoss", 1);
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
  track_that->SetBranchAddress("Q2Loss", &myQ2Loss);
  track_that->SetBranchAddress("Q2NoLoss", &myQ2NoLoss);
  track_that->SetBranchAddress("Run", &run);

  /********************
   * resize the Q2_VAL and Q2_ERROR vectors so that
   * I can fill them without my brains going boom,
   * cause all that push_back with vectors of vectors in confusing!
   * also set all values to zero this is for good house keeping
   * and, so that there is something in it
   *
   * Also do the RUN_VAL and RUN_ERROR - although these are easy
   *********************/
  RUN_VAL.resize(RUNNUMLIST.size(), 0);
  RUN_ERROR.resize(RUNNUMLIST.size(), 0);
  Q2_VAL.resize(NUMQ2TYPES);
  Q2_ERROR.resize(NUMQ2TYPES);

  for (int i_type = 0; i_type < NUMQ2TYPES; i_type++)
  {
    //push back for many vectors involves pushing in vectors...
    //but I should be able to resize them I would think...
    //resize q2 vectors
    Q2_VAL[i_type].resize(NUMPACKAGES);
    Q2_ERROR[i_type].resize(NUMPACKAGES);

    for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
    {
      //resize q2 vector and fill with zero
      //resize q2 vectors
      Q2_VAL[i_type][i_pkg].resize(RUNNUMLIST.size(), 0);
      Q2_ERROR[i_type][i_pkg].resize(RUNNUMLIST.size(), 0);
    }
  }

  int run_index = 0;  //this is the index of a run in the RUNNUMLIST vector

  //get the number of entries in the tree
  nentries = track_that->GetEntries();
  /*  //debugging
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
       * This is Q2TYPE = 0, 1, 2
       * Q2 no cuts, Q2TYPE = 0
       * Q2 lightweighted, Q2TYPE = 1
       * Q2 lightweighted && pedestal, Q2TYPE = 1
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
            /*
             //debugging
             std::cout << "I am Q2 cut hear me roar, this is run num: " << run
             << std::endl << std::endl;

             // for the Q2 with no cuts fill the vectors for the plots

             std::cout << "Type: " << INDEXTOQ2[0] << std::endl;
             std::cout << "Pkg: " << (*myQ2Cut)[i].R3package << std::endl;
             std::cout << "Q2 val: " << (*myQ2Cut)[i].q2Val << std::endl;
             std::cout << "Q2 error: " << (*myQ2Cut)[i].q2Error << std::endl
             << std::endl;
             */

            Q2_VAL[0][(*myQ2Cut)[i].R3package][run_index] = (*myQ2Cut)[i].q2Val;
            Q2_ERROR[0][(*myQ2Cut)[i].R3package][run_index] =
                (*myQ2Cut)[i].q2Error;
            // for the Q2 with lightweighting fill the vectors for the plots

            /*
             std::cout << "Type: " << INDEXTOQ2[1] << std::endl;
             std::cout << "Pkg: " << (*myQ2Cut)[i].R3package << std::endl;
             std::cout << "Q2 val: " << (*myQ2Cut)[i].LWq2Val << std::endl;
             std::cout << "Q2 error: " << (*myQ2Cut)[i].LWq2Error << std::endl
             << std::endl;
             */

            Q2_VAL[1][(*myQ2Cut)[i].R3package][run_index] =
                (*myQ2Cut)[i].LWq2Val;
            Q2_ERROR[1][(*myQ2Cut)[i].R3package][run_index] =
                (*myQ2Cut)[i].LWq2Error;

            // for the Q2 with lightweighting and pedestal subtracted
            //fill the vectors for the plots

            /*
             std::cout << "Type: " << INDEXTOQ2[2] << std::endl;
             std::cout << "Pkg: " << (*myQ2Cut)[i].R3package << std::endl;
             std::cout << "Q2 val: " << (*myQ2Cut)[i].LWq2Val << std::endl;
             std::cout << "Q2 error: " << (*myQ2Cut)[i].LWq2Error << std::endl
             << std::endl;
             */

            Q2_VAL[2][(*myQ2Cut)[i].R3package][run_index] =
                (*myQ2Cut)[i].LWq2Val;
            Q2_ERROR[2][(*myQ2Cut)[i].R3package][run_index] =
                (*myQ2Cut)[i].LWq2Error;

            /*
             Q2_VAL[i_type][l_q2_R3Pkg[i_type]->GetValue(i_pkg)][run_index] =
             (i_type == 0 || i_type == 1 || i_type == 2) ?
             l_q2_Val[i_type]->GetValue(i_pkg) :
             1000 * l_q2_Val[i_type]->GetValue(i_pkg);

             Q2_ERROR[i_type][l_q2_R3Pkg[i_type]->GetValue(i_pkg)][run_index] =
             (i_type == 0) || i_type == 1 || i_type == 2 ?
             l_q2_Error[i_type]->GetValue(i_pkg) :
             1000 * l_q2_Error[i_type]->GetValue(i_pkg);
             */

            break;

            //default case
          default:
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*myQ2Loss)
       *
       * This is Q2TYPE = 3
       ******************/

      for (UInt_t i = 0; i < myQ2Loss->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Q2Loss arena - we have data :-)" << std::cout;
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
        switch ((*myQ2Loss)[i].R3package)
        {
          case 0:  //both pkgs
          case 1:  //pkg 1
          case 2:  //pkg 2
            /*
             //debugging
             std::cout << "I am Q2 Loss hear me roar, this is run num: " << run
             << std::endl << std::endl;

             // for the Q2 Loss fill the vectors for the plots

             std::cout << "Type: " << INDEXTOQ2[3] << std::endl;
             std::cout << "Pkg: " << (*myQ2Loss)[i].R3package << std::endl;
             std::cout << "Q2 val: " << (*myQ2Loss)[i].Val << std::endl;
             std::cout << "Q2 error: " << (*myQ2Loss)[i].Error << std::endl
             << std::endl;
             */

            //the Q2 values and error is multiplied by 1000 so it is in
            //m(Gev)^2 rather then GeV^2
            Q2_VAL[3][(*myQ2Loss)[i].R3package][run_index] = 1000
                * (*myQ2Loss)[i].Val;
            Q2_ERROR[3][(*myQ2Loss)[i].R3package][run_index] = 1000
                * (*myQ2Loss)[i].Error;

            break;

            //default case
          default:
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*myQ2NoLoss)
       *
       * This is Q2TYPE = 4
       ******************/

      for (UInt_t i = 0; i < myQ2NoLoss->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Q2NoLoss arena - we have data :-)" << std::cout;
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
        switch ((*myQ2NoLoss)[i].R3package)
        {
          case 0:  //both pkgs
          case 1:  //pkg 1
          case 2:  //pkg 2
            /*
             //debugging
             std::cout << "I am Q2 No Loss hear me roar, this is run num: "
             << run << std::endl << std::endl;

             // for the Q2 No Loss fill the vectors for the plots

             std::cout << "Type: " << INDEXTOQ2[4] << std::endl;
             std::cout << "Pkg: " << (*myQ2NoLoss)[i].R3package << std::endl;
             std::cout << "Q2 val: " << (*myQ2NoLoss)[i].Val << std::endl;
             std::cout << "Q2 error: " << (*myQ2NoLoss)[i].Error << std::endl
             << std::endl;
             */

            //the Q2 values and error is multiplied by 1000 so it is in
            //m(Gev)^2 rather then GeV^2
            Q2_VAL[4][(*myQ2NoLoss)[i].R3package][run_index] = 1000
                * (*myQ2NoLoss)[i].Val;
            Q2_ERROR[4][(*myQ2NoLoss)[i].R3package][run_index] = 1000
                * (*myQ2NoLoss)[i].Error;

            break;

            //default case
          default:
            break;
        }
      }

      //Increment run index so we can look at the next run
      run_index++;
    }
    //debugging
    //cout << "number of runs: " << RUNNUMLIST.size() << std::endl << "Run index: " << run_index << std::endl;
  }
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
 - g_Q2_VS_RUN - the vector of TGraphErrors

 - mg_Q2_VS_RUN - the vector of mutigraph

 - NUMQ2TYPE - the number of different Q2 types
 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in
 - INDEXTOTYPE - array to get what Q2 type that we are in

 - Q2_VAL - vector of the Q2 values
 - Q2_ERROR - vector of the Q2 errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 Exit Conditions: none
 Called By: Q2_vs_Run
 Date: 06-07-2014
 Modified:
 *********************************************************/
void Make_graphs()
{
  /*  //debugging
   std::cout << "made it in the make graph function" << std::endl;*/

//resize the vector of the tgraph to the number of q2 types
  g_Q2_VS_RUN.resize(NUMQ2TYPES);
//resize the muiltigraph to number of packages
  mg_Q2_VS_RUN.resize(NUMPACKAGES);

//define legend
  LEGEND.resize(NUMPACKAGES);

//Make the TGraphs with the information we got in from looping through the tree
  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    /*    //debugging....
     std::cout << "Pkg: " << i_pkg << std::endl;*/

//define legend
    LEGEND[i_pkg] =
        (i_pkg == 0) ?
            new TLegend(0.89, 0.45, 0.99, 0.55) :
            new TLegend(0.79, 0.35, 0.99, 0.65);
    LEGEND[i_pkg]->SetHeader("Q^{2} Type");

//define the multigraph
    mg_Q2_VS_RUN[i_pkg] = new TMultiGraph(Form("mg_Q2_VS_RUN[%d]", i_pkg),
        Form("Q^{2} vs Run number for package %s", INDEXTOPKG[i_pkg].c_str()));

//initalize the max and min values - Max very small, Min large so they must be replaced
    Q2_MAX[i_pkg] = -1e6;
    Q2_MIN[i_pkg] = 1e6;

    for (int i_type = 0; i_type < NUMQ2TYPES; i_type++)
    {
      /*      //debugging....
       std::cout << "Type: " << i_type << std::endl;*/
//resize the type tgraph vector
      g_Q2_VS_RUN[i_type].resize(NUMPACKAGES);
//push back for many vectors involves pushing in vectors...
//but I should be able to resize them I would think...

//take the vectors and turn them into arrays so that TGraphErrors will play nice
//with them by &(vector[0]) :)
      g_Q2_VS_RUN[i_type][i_pkg] = new TGraphErrors(RUNNUMLIST.size(),
          &(RUN_VAL[0]), &(Q2_VAL[i_type][i_pkg][0]), &(RUN_ERROR[0]),
          &(Q2_ERROR[i_type][i_pkg][0]));

//set the attributes of the graph
//Q2 no cuts = black, filled circle
//Q2 lightweighted = violet, filled square
//Q2 lightweight and pediatal subtracted = green, filled hospital sign
//Q2Loss = red, filled triangle point up
//Q2NoLoss = blue, filled triangle point down
      switch (i_type)
      {
        case 0:  //Q2 no cuts
          //set it so the errors print
          gStyle->SetOptFit(1110);
          //set markercolor && error bar color - Black
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerColor(kBlack);
          g_Q2_VS_RUN[i_type][i_pkg]->SetLineColor(kBlack);

          //set Marker size
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerSize(1);

          //set marker style - filled circle
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerStyle(8);
          break;

        case 1:  //Q2 no cuts
          //set it so the errors print
          gStyle->SetOptFit(1110);
          //set markercolor && error bar color - Violet
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerColor(kViolet - 1);
          g_Q2_VS_RUN[i_type][i_pkg]->SetLineColor(kViolet - 1);

          //set Marker size
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerSize(1);

          //set marker style - filled square
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerStyle(21);
          break;

        case 2:  //Q2 light weight and pedistal subtrackted
          //set it so the errors print
          gStyle->SetOptFit(1110);
          //set markercolor && error bar color - Green
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerColor(kGreen + 4);
          g_Q2_VS_RUN[i_type][i_pkg]->SetLineColor(kGreen + 4);

          //set Marker size
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerSize(1);

          //set marker style - filled hospital sign
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerStyle(34);
          break;

        case 3:
          //set it so the errors print
          gStyle->SetOptFit(1110);
          //set markercolor && error bar color - Red
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerColor(kRed);
          g_Q2_VS_RUN[i_type][i_pkg]->SetLineColor(kRed);

          //set Marker size
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerSize(1);

          //set marker style - filled triangle pint up
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerStyle(22);
          break;

        case 4:
          //set it so the errors print
          gStyle->SetOptFit(1110);
          //set markercolor && error bar color - Blue
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerColor(kBlue);
          g_Q2_VS_RUN[i_type][i_pkg]->SetLineColor(kBlue);

          //set Marker size
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerSize(1);

          //set marker style - filled triangle pint up
          g_Q2_VS_RUN[i_type][i_pkg]->SetMarkerStyle(23);
          break;

        default:
          break;
      }
      if (!((i_type == 0 || i_type == 1 || i_type == 2) && i_pkg == 0))
      {
//add this graph to the muitigraph
        mg_Q2_VS_RUN[i_pkg]->Add(g_Q2_VS_RUN[i_type][i_pkg]);

//add to the Legend
        LEGEND[i_pkg]->AddEntry(g_Q2_VS_RUN[i_type][i_pkg],
            Form("Q^{2} %s", INDEXTOQ2[i_type].c_str()), "ep");

//get the min and max Q2 values
        Q2_MAX[i_pkg] = std::max(Q2_MAX[i_pkg],
            *std::max_element(Q2_VAL[i_type][i_pkg].begin(),
                Q2_VAL[i_type][i_pkg].end()));

        Q2_MIN[i_pkg] = std::min(Q2_MIN[i_pkg],
            *std::min_element(Q2_VAL[i_type][i_pkg].begin(),
                Q2_VAL[i_type][i_pkg].end(), less_than_but_non_zero));

/*
        //debugging
        std::cout << "Pkg: " << i_pkg << std::endl;
        std::cout << "Q2_MIN: " << Q2_MIN[i_pkg] << std::endl;
        std::cout << "Q2_MAX: " << Q2_MAX[i_pkg] << std::endl;
*/
      }
    }
  }
  return;
}

/***********************************************************
 Function: Plot
 Purpose:
 To plot the graphs and save those plots

 Entry Conditions:
 none

 Global:
 - mg_Q2_VS_RUN - vector of the muiltigraph

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in

 - LEGEND - the legned

 - RUNNUMLIST - list of the run numbers - now sorted

 - Q2_MAX - max q2 value for a pkg
 - Q2_MIN - min q2 value for a pkg

 - FUDGEFACTOR_RUN - buffer for the RUN axis
 - FUDGEFACTOR_Q2 - buffer for the Q2 axis

 Exit Conditions: none
 Called By: Q2_vs_Run
 Date: 06-06-2014
 Modified: 06-08-2014
 *********************************************************/
void Plot()
{
//Canvas
  std::vector<TCanvas*> c_q2;  //[pkg]
  c_q2.resize(NUMPACKAGES);

  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    c_q2[i_pkg] = new TCanvas(Form("c_q2[%d]", i_pkg),
        Form("Q^{2} vs run number for %s package", INDEXTOPKG[i_pkg].c_str()));

//set title and axis lables
    mg_Q2_VS_RUN[i_pkg]->SetTitle(
        Form(
            "Q^{2} vs Run number for %s package; Run number; Q^{2} (m(GeV^{2}))",
            INDEXTOPKG[i_pkg].c_str()));

//Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    mg_Q2_VS_RUN[i_pkg]->Draw("AP");

    /*    //debugging
     std::cout << "Pkg: " << i_pkg << std::endl << " Min: " << Q2_MIN[i_pkg]
     << std::endl << " Max: " << Q2_MAX[i_pkg] << std::endl;
     std::cout << "Run Num Min: " << RUNNUMLIST.front()
     << std::endl << "Run Num Max: " << RUNNUMLIST.back() << std::endl; */

    mg_Q2_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
        Q2_MIN[i_pkg] - FUDGEFACTOR_Q2, Q2_MAX[i_pkg] + FUDGEFACTOR_Q2);

    Double_t RunRange = Double_t(RUNNUMLIST.back() - RUNNUMLIST.front());

/*
    Double_t x_min = Double_t(RUNNUMLIST.front())
        - RunRange * FUDGEFACTOR_RUN[0];
    Double_t x_max = Double_t(RUNNUMLIST.back())
        + RunRange * FUDGEFACTOR_RUN[1];

    //debugging
    std::cout << "Runlist vector size: " << RUNNUMLIST.size() << std::endl;
    std::cout << "Runlist Min: " << RUNNUMLIST.front() << std::endl;
    std::cout << "Runlist Max: " << RUNNUMLIST.back() << std::endl;

    std::cout << "Run val vector size: " << RUN_VAL.size() << std::endl;
    std::cout << "Run Val Min: " << RUN_VAL.front() << std::endl;
    std::cout << "Run Val Max: " << RUN_VAL.back() << std::endl;

    std::cout << "RunRange: " << RunRange << std::endl;
    std::cout << "x_min: " << x_min << std::endl;
    std::cout << "x_max: " << x_max << std::endl;

    mg_Q2_VS_RUN[i_pkg]->GetXaxis()->SetLimits(x_min, x_max);
*/

    //   original
     mg_Q2_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
     Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
     Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);


    //draw the legend
    LEGEND[i_pkg]->Draw();

    //move the axis label so can read everything
    mg_Q2_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

    //update the pad
    gPad->Update();

    c_q2[i_pkg]->SaveAs(
        Form(
            "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/Pass_%s_Q2_vs_run_pkg_%s.png",
            PASS.c_str(), PASS.c_str(), INDEXTOPKG[i_pkg].c_str()));
  }

  return;
}

/***********************************************************
 Function: Print_To_File
 Purpose:
 To print out the values from the ValerianROOT used in this script.

 Entry Conditions:
 none

 Global:

 - NUMQ2TYPE - the number of different Q2 types
 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in
 - INDEXTOTYPE - array to get what Q2 type that we are in

 - Q2_VAL - vector of the Q2 values
 - Q2_ERROR - vector of the Q2 errors
 - RUN_VAL - vector of the run values
 these of course are 0 since there are none.

 - RUNNUMLIST - list of the run numbers - now sorted

 Exit Conditions: none
 Called By: Q2_vs_Run
 Date: 02-10-2015
 Modified: 02-13-2015
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
          "/home/vmgray/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/Pass%s_Q2_vs_Run.txt",
          PASS.c_str(), PASS.c_str()));

  if (!fout.is_open())
    cout << "File not opened" << endl;

  fout << "Run \t pkg \t\t 0 Val & Error "
  "\t\t\t 1 Val & Error "
  "\t\t\t 2 Val & Error "
  "\t\t\t 3 Val & Error "
  "\t\t\t 4 Val & Error"
  << std::endl;

  for (int i_run = 0; i_run < RUNNUMLIST.size(); i_run++)
  {
    for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
    {
      fout << RUN_VAL[i_run] << " \t " << INDEXTOPKG[i_pkg].c_str() << " \t\t "
      << Q2_VAL[0][i_pkg][i_run] << " +- " << Q2_ERROR[0][i_pkg][i_run]
      << " \t\t\t " << Q2_VAL[1][i_pkg][i_run] << " +- "
      << Q2_ERROR[1][i_pkg][i_run] << " \t\t\t " << Q2_VAL[2][i_pkg][i_run]
      << " +- " << Q2_ERROR[2][i_pkg][i_run] << " \t\t\t "
      << Q2_VAL[3][i_pkg][i_run] << " +- " << Q2_ERROR[3][i_pkg][i_run]
      << " \t\t\t " << Q2_VAL[4][i_pkg][i_run] << " +- "
      << Q2_ERROR[4][i_pkg][i_run] << std::endl;
    }

    fout << std::endl;

  }

  //Table to Q2 type number to what it is
  fout << std::endl << std::endl;
  fout << "# \t \t Q2 type" << std::endl;

  for (int i_type = 0; i_type < NUMQ2TYPES; i_type++)
  {
    fout << i_type << " \t " << INDEXTOQ2[i_type].c_str() << std::endl;
  }

  fout.close();

  return;

}
