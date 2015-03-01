/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This script has 2 purpuses
 1) To serve as testing for the development of ValerianROOT
 2) be an example of how to manage the ValerianROOT Tree and
 get plots out, as this ROOT file does not have a struture
 which supports simple Draw statments (sorry to those of
 you who like them)
 3) To plot the Q2 (all versions) vs run number

 Entry Conditions:none
 Date: 05-13-2014
 Modified: 10-09-2014
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

//standard includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

//ValerianROOT macro includes
//include for the functions used in all macros without being coustomized
#include "Read_In_Run_Numbers.h"
#include "less_than_but_non_zero.h"

//function definitions for those not in the header files
void Loop_Through_Tree(TChain * track_that);
void Make_graphs();
void Plot();

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
Double_t FUDGEFACTOR_RUN[2] = {0.05 ,0.25};  // the percentage add on to the X axis for pretty plots

// index of vector to what it is
std::string INDEXTOQ2[NUMQ2TYPES] = { "with No Cuts", "with lightweighting",
    "with lightweighting and Pedestal subtracted", "with Loss", "without Loss" };
std::string INDEXTOPKG[NUMQ2TYPES] = { "Both", "1", "2" };

//array for min and may q2 values
Double_t Q2_MIN[NUMPACKAGES] = { 0 };  //min q2 for a pkg
Double_t Q2_MAX[NUMPACKAGES] = { 0 };  //max q2 for a pkg

//the pass number
std::string PASS;

void Q2_vs_Run_typical_ROOT_method(std::string pass, std::string filename)
{
  PASS = pass;

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");
  Track_It->Add(Form(
      "/group/qweak/www/html/tracking/pass%s/Pass%s_TrackingRuns.root",
                PASS.c_str(), PASS.c_str()));

  //run the functions on interest
  Read_In_Run_Numbers(filename);
  Loop_Through_Tree(Track_It);
  Make_graphs();
  Plot();

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
 Called By: Q2_vs_Run_typical_ROOT_method
 Date: 05-31-2014
 Modified: 10-09-2014
 *********************************************************/
void Loop_Through_Tree(TChain* track_that)
{
  //get the number of entries in the tree
  nentries = track_that->GetEntries();
  /*  //debugging
   std::cout << "number of entries in the tree " << nentries << std::endl;*/

  /*******************************
   * Get the branches, and leaf values that are need for this script
   * This is done by:
   *******************************/

  //Run info
  track_that->SetBranchStatus("Run", 1);
  TLeaf* l_run = track_that->GetLeaf("Run");

//  // Run info (alternative)
//  Int_t v_run;
//  track_that->SetBranchAddress("Run",&v_run);

  std::vector<TLeaf*> l_q2_Val(NUMQ2TYPES);
  std::vector<TLeaf*> l_q2_Error(NUMQ2TYPES);
  std::vector<TLeaf*> l_q2_R3Pkg(NUMQ2TYPES);

  //Q2 with no cuts
  track_that->SetBranchStatus("Q2Cut", 1);
  l_q2_Val[0] = track_that->GetLeaf("Q2Cut.q2Val");
  l_q2_Error[0] = track_that->GetLeaf("Q2Cut.q2Error");
  l_q2_R3Pkg[0] = track_that->GetLeaf("Q2Cut.R3package");

  //Q2 with light weighting
  l_q2_Val[1] = track_that->GetLeaf("Q2Cut.LWq2Val");
  l_q2_Error[1] = track_that->GetLeaf("Q2Cut.LWq2Error");
  l_q2_R3Pkg[1] = track_that->GetLeaf("Q2Cut.R3package");

  //Q2 with lightweighting and Pedestal subtracted
  l_q2_Val[2] = track_that->GetLeaf("Q2Cut.LWPq2Val");
  l_q2_Error[2] = track_that->GetLeaf("Q2Cut.LWPq2Error");
  l_q2_R3Pkg[2] = track_that->GetLeaf("Q2Cut.R3package");

//  //Q2 with cuts (alternative)
// doen't work since my root tree is too structure is to complicated
//  Double_t v_q2Cut_Val, v_q2Cut_Error;
//  track_that->SetBranchAddress("Q2Cut.Val",&v_q2Cut_Val);
//  track_that->SetBranchAddress("Q2Cut.Error",&v_q2Cut_Error);

  //Q2 with loss
  l_q2_Val[3] = track_that->GetLeaf("Q2Loss.Val");
  l_q2_Error[3] = track_that->GetLeaf("Q2Loss.Error");
  l_q2_R3Pkg[3] = track_that->GetLeaf("Q2Loss.R3package");

  //Q2 without Loss
  l_q2_Val[4] = track_that->GetLeaf("Q2NoLoss.Val");
  l_q2_Error[4] = track_that->GetLeaf("Q2NoLoss.Error");
  l_q2_R3Pkg[4] = track_that->GetLeaf("Q2NoLoss.R3package");
  /*  //debugging
   std::cout << "Val: " << l_q2_Val[2] << std::endl;
   std::cout << "Error: " << l_q2_Error[2] << std::endl;
   std::cout << "Pkg: " << l_q2_R3Pkg[2] << std::endl;*/

  //for sizing the Q2 storage vectors
  RUN_VAL.resize(RUNNUMLIST.size(), 0);
  RUN_ERROR.resize(RUNNUMLIST.size(), 0);
  Q2_VAL.resize(NUMQ2TYPES);
  Q2_ERROR.resize(NUMQ2TYPES);
  int run_index = 0;
  /*

   //debugging
   std::cout << "right before looping through the tree, run index is "
   << run_index << std::endl;
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
    //get the run number for that entry
    Int_t run = l_run->GetValue();

    /*
     //debugging
     std::cout << "the count of this run, " << run << " in the run list is: "
     << std::count(RUNNUMLIST.begin(), RUNNUMLIST.end(), run) << std::endl;
     */

    //check whether run number is in the run list
    if (std::count(RUNNUMLIST.begin(), RUNNUMLIST.end(), run) > 0)
    {

/*      //debugging
      std::cout << "for run " << run << " the count is "
      << std::count(RUNNUMLIST.begin(), RUNNUMLIST.end(), run)
      << " and we have compared them..." << std::endl;*/

      RUN_VAL[run_index] = run;

      /********************
       //resize the Q2_VAL and Q2_ERROR vectors so that I can fill them without
       //my brains going boom, cause all that push_back with vectors of vectors in confusing!
       //Also set all values to zero this is for good house keeping and, so that there
       //is something in it
       *********************/
      for (int i_type = 0; i_type < NUMQ2TYPES; i_type++)
      {
        /*        //debugging....
         std::cout << "In type loop, Type: " << i_type << std::endl;*/
        //push back for many vectors involves pushing in vectors...
        //but I should be able to resize them I would think...
        //resize q2 vectors
        Q2_VAL[i_type].resize(NUMPACKAGES);
        Q2_ERROR[i_type].resize(NUMPACKAGES);

        for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
        {
          /*                    std::cout << "in pkg loop, Pkg: " << l_q2_R3Pkg[i_type]->GetValue(i_pkg)
           << std::endl;*/

          //resize q2 vector and fill with zero
          //resize q2 vectors
          Q2_VAL[i_type][i_pkg].resize(RUNNUMLIST.size(), 0);
          Q2_ERROR[i_type][i_pkg].resize(RUNNUMLIST.size(), 0);

          /*****************
           If there is no data for this run, i_entry, then there is one and only one
           entry for this run in any one type.  So we must skip this and move on.
           Fill the vector with zeros, which I have done in resizing them, Oh Happy Day!!
           ******************/
          if (l_q2_R3Pkg[i_type]->GetValue(0) > -1)
          {
            /*                        //debugging
             std::cout << "and yes I am not > -1" << std::endl;*/
            //there is no Q2_cut value for both pkgs
            if ((i_type == 0 || i_type == 1 || i_type == 2) && i_pkg == 2)
            {
              /*                            //debugging
               std::cout << "I am Q2_cut, both pkgs, beware!" << std::endl;*/
              //this is not needed as a initialize the vectors so everything is zero
              //Q2_VAL[run_index][i_type][0] = 0;
              //Q2_ERROR[run_index][i_type][0] = 0;
            } else
            {
              /*                            //debugging
               std::cout << "Made it to the else, not type 0, or 1 or 2 && pkg 0 run num: "
               << run << std::endl;
               std::cout << "Type: " << i_type << std::endl;
               std::cout << "Pkg: " << l_q2_R3Pkg[i_type]->GetValue(i_pkg)
               << std::endl;
               std::cout << "Q2 val: " << l_q2_Val[i_type]->GetValue(i_pkg)
               << std::endl;
               std::cout << "Q2 error: " << l_q2_Error[i_type]->GetValue(i_pkg)
               << std::endl;*/
              Q2_VAL[i_type][l_q2_R3Pkg[i_type]->GetValue(i_pkg)][run_index] =
                  (i_type == 0 || i_type == 1 || i_type == 2) ?
                      l_q2_Val[i_type]->GetValue(i_pkg) :
                      1000 * l_q2_Val[i_type]->GetValue(i_pkg);

              Q2_ERROR[i_type][l_q2_R3Pkg[i_type]->GetValue(i_pkg)][run_index] =
                  (i_type == 0) || i_type == 1 || i_type == 2 ?
                      l_q2_Error[i_type]->GetValue(i_pkg) :
                      1000 * l_q2_Error[i_type]->GetValue(i_pkg);

            }
          }
        }
      }
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
 Called By: Q2_vs_Run_typical_ROOT_method
 Date: 06-07-2014
 Modified: 10-09-2014
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
    LEGEND[i_pkg]->SetHeader("Q^2 Type");

    //define the multigraph
    mg_Q2_VS_RUN[i_pkg] = new TMultiGraph(Form("mg_Q2_VS_RUN[%d]", i_pkg),
        Form("Q2 vs Run number for package %s", INDEXTOPKG[i_pkg].c_str()));

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
            Form("Q2 %s", INDEXTOQ2[i_type].c_str()), "ep");

        //get the min and max Q2 values
        Q2_MAX[i_pkg] = std::max(Q2_MAX[i_pkg],
            *std::max_element(Q2_VAL[i_type][i_pkg].begin(),
                Q2_VAL[i_type][i_pkg].end()));
        Q2_MIN[i_pkg] = std::min(Q2_MIN[i_pkg],
            *std::min_element(Q2_VAL[i_type][i_pkg].begin(),
                Q2_VAL[i_type][i_pkg].end(), less_than_but_non_zero));
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

 - Q2_MAX max q2 value for a pkg
 - Q2_MIN min q2 value for a pkg

 Exit Conditions: none
 Called By: Q2_vs_Run_typical_ROOT_method
 Date: 06-06-2014
 Modified: 10-09-2014
 *********************************************************/
void Plot()
{
  //Canvas
  std::vector<TCanvas*> c_q2;  //[pkg]
  c_q2.resize(NUMPACKAGES);

  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    c_q2[i_pkg] = new TCanvas(Form("c_q2[%d]", i_pkg),
        Form("Q2 vs run number for %s package", INDEXTOPKG[i_pkg].c_str()));

    //set title and axis lables
    mg_Q2_VS_RUN[i_pkg]->SetTitle(
        Form("Q2 vs Run number for %s package; Run number; Q2 (m(GeV)^2)",
            INDEXTOPKG[i_pkg].c_str()));

    //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    mg_Q2_VS_RUN[i_pkg]->Draw("AP");

/*    //debugging
    std::cout << "Pkg: " << i_pkg << std::endl << " Min: " << Q2_MIN[i_pkg]
    << std::endl << " Max: " << Q2_MAX[i_pkg] << std::endl;
    std::cout << "Run Num Min: " << RUNNUMLIST.front()
    << std::endl << "Run Num Max: " << RUNNUMLIST.back() << std::endl; */

    mg_Q2_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(Q2_MIN[i_pkg] - FUDGEFACTOR_Q2,
        Q2_MAX[i_pkg] + FUDGEFACTOR_Q2);

    Double_t RunRange = Double_t(RUNNUMLIST.back() - RUNNUMLIST.front());

    mg_Q2_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
        Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
        Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

    //draw the legend
    LEGEND[i_pkg]->Draw();

    //update the pad
    gPad->Update();

    c_q2[i_pkg]->SaveAs(
        Form("~/QwAnalysis_trunk/Extensions/ValerianROOT/Typical_ROOT_Run_%s_Q2_vs_run_%s_pkg.png", PASS.c_str(),
            INDEXTOPKG[i_pkg].c_str()));
  }

  return;
}
