/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This scrip is to plot the slope and CHi square values for
 The R3 treelines, tracks, and partial tracks vs run number

 Entry Conditions:none
 Date: 07-08-2014
 Modified: 11-12-2014
 Assisted By:
 ***********************************************************/
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
#include <TString.h>

//standard includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

//ValerianROOT macro includes
//include for the functions used in all macros without being customized
#include "Read_In_Run_Numbers.h"
#include "less_than_but_non_zero.h"

//ValerianROOT includes
#include "include/TreelineSlope.h"
#include "include/TreelineChi2.h"
#include "include/PartialTrackSlope.h"
#include "include/PartialTrackChi2.h"
#include "include/TrackSlope.h"
#include "include/TrackChi2.h"

//function definitions
void Loop_Through_Tree(TChain * track_that);
void Make_graphs();
void Plot();

//define TGraphErrors
std::vector<std::vector<std::vector<TGraphErrors*> > > g_TREEPLANE_SLOPE_VS_RUN;  //[track type][treeplane][package]
std::vector<std::vector<std::vector<TGraphErrors*> > > g_TREEPLANE_CHI2_VS_RUN;  //[track type][treeplane][package]
std::vector<std::vector<std::vector<TGraphErrors*> > > g_COMPLANE_SLOPE_VS_RUN;  //[track type][UU or VV][package]
std::vector<std::vector<std::vector<TGraphErrors*> > > g_COMPLANE_CHI2_VS_RUN;  //[track type][UU or VV][package]

//define the muiltigraph
std::vector<TMultiGraph*> mg_TREEPLANE_SLOPE_VS_RUN;  //[pkg]
std::vector<TMultiGraph*> mg_TREEPLANE_CHI2_VS_RUN;  //[pkg]
std::vector<TMultiGraph*> mg_COMPLANE_SLOPE_VS_RUN;  //[pkg]
std::vector<TMultiGraph*> mg_COMPLANE_CHI2_VS_RUN;  //[pkg]

//Legend for the plot
std::vector<TLegend*> LEGEND_TREE;  //[pkg]
std::vector<TLegend*> LEGEND_TRACKS;  //[pkg]

//define vectors to store information for GraphError
std::vector<std::vector<std::vector<std::vector<Double_t> > > > TREEPLANE_SLOPE_VAL;  //[[track type]treeplane][package][run index num]
std::vector<std::vector<std::vector<std::vector<Double_t> > > > TREEPLANE_SLOPE_ERROR;  //[track type][treeplane][package][run index num]
std::vector<std::vector<std::vector<std::vector<Double_t> > > > TREEPLANE_CHI2_VAL;  //[track type][treeplane]{package][run index num]
std::vector<std::vector<std::vector<std::vector<Double_t> > > > TREEPLANE_CHI2_ERROR;  //[track type][treeplane][package][run index num]

std::vector<std::vector<std::vector<std::vector<Double_t> > > > COMPLANE_SLOPE_VAL;  //[track type][UU or VV][package][run index num]
std::vector<std::vector<std::vector<std::vector<Double_t> > > > COMPLANE_SLOPE_ERROR;  //[track type][UU or VV][package][run index num]
std::vector<std::vector<std::vector<std::vector<Double_t> > > > COMPLANE_CHI2_VAL;  //[track type][[UU or VV]package][run index num]
std::vector<std::vector<std::vector<std::vector<Double_t> > > > COMPLANE_CHI2_ERROR;  //[track type][UU or VV][package][run index num]

// - always zero - TGraph Errors needs it
std::vector<Double_t> RUN_VAL;  //[run] - need so be double for TGraphError
std::vector<Double_t> RUN_ERROR;  //[run] always zero - needs to be double for TGraphError
//definition of integers
Int_t nentries;  // number of entries in the tree

static const Int_t NUMPACKAGE = 3;
static const Int_t NUMTREEPLANE = 4;
static const Int_t NUMCOMPLANE = 2;
static const Int_t NUMTRACKTYPE = 3;

Double_t FUDGEFACTOR_TREEPLANE_SLOPE = 0.003;  //the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_TREEPLANE_CHI2 = 0.03;  //the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_COMPLANE_SLOPE = 0.003;  //the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_COMPLANE_CHI2 = 0.03;  //the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_RUN[2] = { 0.05, 0.25 };  // the percentage add on to the X axis for pretty plots

// index of vector to what it is
std::string INDEXTOTREEPLANE[NUMTREEPLANE] = { "Front U", "Front V", "Back U",
    "Back V" };
std::string INDEXTOCOMPLANE[NUMCOMPLANE] = { "UU", "VV" };
std::string INDEXTOTRACKTYPE[NUMTRACKTYPE] = { "Treeline", "Partial Track",
    "Track" };
std::string INDEXTOPKG[NUMPACKAGE] = { "Both", "1", "2" };

//array for min and max Slope and Chi values, filled so that they allow
//initalize the max and min values - Max very small, Min large so they must be replaced (done later)
Double_t TREEPLANE_SLOPE_MIN[NUMPACKAGE] = { 0 };  //min slope for a pkg
Double_t TREEPLANE_SLOPE_MAX[NUMPACKAGE] = { 0 };  //max slope for a pkg
Double_t TREEPLANE_CHI2_MIN[NUMPACKAGE] = { 0 };  //min slope for a pkg
Double_t TREEPLANE_CHI2_MAX[NUMPACKAGE] = { 0 };  //max slope for a pkg
Double_t COMPLANE_SLOPE_MIN[NUMPACKAGE] = { 0 };  //min chi for a pkg
Double_t COMPLANE_SLOPE_MAX[NUMPACKAGE] = { 0 };  //max chi for a pkg
Double_t COMPLANE_CHI2_MIN[NUMPACKAGE] = { 0 };  //min chi for a pkg
Double_t COMPLANE_CHI2_MAX[NUMPACKAGE] = { 0 };  //max chi for a pkg

//the pass number
std::string PASS;

//any other things that will be needed by more than on function
void TreeMatch_vs_Run(std::string pass, std::string filename)
{
  PASS = pass;

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");
  /*  Track_It->Add(
   Form("/group/qweak/www/html/tracking/pass%s/Pass%s_TrackingRuns.root",
   PASS.c_str(), PASS.c_str()));*/

  Track_It->Add(
      Form(
          "~/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_TrackingRuns.root",
          PASS.c_str()));

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
 To loop over the event and fill in the values for the
 graphs

 Entry Conditions:
 - TChain - track_that

 Global:
 - NUMTREEPLANE - the number of different axis
 - NUMCOMPLANE - the number of packages

 - TREEPLANE_SLOPE_VAL - vector of the slope values
 - TREEPLANE_SLOPE_ERROR - vector of the slope errors
 - TREEPLANE_CHI_VAL - vector of the chi values
 - TREEPLANE_CHI_RMS - vector of the chi errors
 - COMPLANE_SLOPE_VAL - vector of the slope values
 - COMPLANE_SLOPE_ERROR - vector of the slope errors
 - COMPLANE_CHI_VAL - vector of the chi values
 - COMPLANE_CHI_RMS - vector of the chi errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 - RUNNUMLIST - the vector containing the list of runs of interest (ROI)

 - nentries - the number of entries in the TrackThat tree

 Exit Conditions: none
 Called By: Q2_vs_Run
 Date: 07-09-2014
 Modified: 11-12-2014
 *********************************************************/
void Loop_Through_Tree(TChain* track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the structs that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector <MyTreelineSlope_t>*      MyTreelineSlope = 0;
  std::vector <MyTreelineChi2_t>*       MyTreelineChi2 = 0;
  std::vector <MyPartialTrackSlope_t>*  MyPartialTrackSlope = 0;
  std::vector <MyPartialTrackChi2_t>*   MyPartialTrackChi2 = 0;
  std::vector <MyTrackSlope_t>*         MyTrackSlope = 0;
  std::vector <MyTrackChi2_t>*          MyTrackChi2 = 0;

  //create an integer for the run number - as this is not a
  //structure it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("TreelineSlope", 1);
  track_that->SetBranchStatus("TreelineChi2", 1);
  track_that->SetBranchStatus("PartialTrackSlope", 1);
  track_that->SetBranchStatus("PartialTrackChi2", 1);
  track_that->SetBranchStatus("TrackSlope", 1);
  track_that->SetBranchStatus("TrackChi2", 1);
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
  track_that->SetBranchAddress("TreelineSlope", &MyTreelineSlope);
  track_that->SetBranchAddress("TreelineChi2", &MyTreelineChi2);
  track_that->SetBranchAddress("PartialTrackSlope", &MyPartialTrackSlope);
  track_that->SetBranchAddress("PartialTrackChi2", &MyPartialTrackChi2);
  track_that->SetBranchAddress("TrackSlope", &MyTrackSlope);
  track_that->SetBranchAddress("TrackChi2", &MyTrackChi2);
  track_that->SetBranchAddress("Run", &run);

  /********************
   * resize the *_SLOPE_* and *_CHI2_* vectors so that
   * I can fill them without my brains going boom,
   * cause all that push_back with vectors of vectors in confusing!
   * also set all values to zero this is for good house keeping
   * and, so that there is something in it
   *
   * Also do the RUN_VAL and RUN_ERROR - although these are easy
   *********************/
  RUN_VAL.resize(RUNNUMLIST.size(), 0);
  RUN_ERROR.resize(RUNNUMLIST.size(), 0);
  TREEPLANE_SLOPE_VAL.resize(NUMTRACKTYPE);  //[track type]
  TREEPLANE_SLOPE_ERROR.resize(NUMTRACKTYPE);
  TREEPLANE_CHI2_VAL.resize(NUMTRACKTYPE);
  TREEPLANE_CHI2_ERROR.resize(NUMTRACKTYPE);

  COMPLANE_SLOPE_VAL.resize(NUMTRACKTYPE);  //[track type]
  COMPLANE_SLOPE_ERROR.resize(NUMTRACKTYPE);
  COMPLANE_CHI2_VAL.resize(NUMTRACKTYPE);
  COMPLANE_CHI2_ERROR.resize(NUMTRACKTYPE);

  for (int i_tracktype = 0; i_tracktype < NUMTRACKTYPE; i_tracktype++)
  {

    TREEPLANE_SLOPE_VAL[i_tracktype].resize(NUMTREEPLANE);  //[track type][treeplane]
    TREEPLANE_SLOPE_ERROR[i_tracktype].resize(NUMTREEPLANE);
    TREEPLANE_CHI2_VAL[i_tracktype].resize(NUMTREEPLANE);
    TREEPLANE_CHI2_ERROR[i_tracktype].resize(NUMTREEPLANE);

    for (int i_treeplane = 0; i_treeplane < NUMTREEPLANE; i_treeplane++)
    {
      TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane].resize(NUMPACKAGE);  //[track type][treeplane][pkg]
      TREEPLANE_SLOPE_ERROR[i_tracktype][i_treeplane].resize(NUMPACKAGE);
      TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane].resize(NUMPACKAGE);
      TREEPLANE_CHI2_ERROR[i_tracktype][i_treeplane].resize(NUMPACKAGE);

      for (int i_pkg = 0; i_pkg < NUMPACKAGE; i_pkg++)
      {
        TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].resize(
            RUNNUMLIST.size(), 0);  //[track type][treeplane][pkg]
        TREEPLANE_SLOPE_ERROR[i_tracktype][i_treeplane][i_pkg].resize(
            RUNNUMLIST.size(), 0);
        TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].resize(
            RUNNUMLIST.size(), 0);
        TREEPLANE_CHI2_ERROR[i_tracktype][i_treeplane][i_pkg].resize(
            RUNNUMLIST.size(), 0);
      }
    }

    COMPLANE_SLOPE_VAL[i_tracktype].resize(NUMCOMPLANE);  //[track type] [UU or VV]
    COMPLANE_SLOPE_ERROR[i_tracktype].resize(NUMCOMPLANE);
    COMPLANE_CHI2_VAL[i_tracktype].resize(NUMCOMPLANE);
    COMPLANE_CHI2_ERROR[i_tracktype].resize(NUMCOMPLANE);

    for (int i_complane = 0; i_complane < NUMCOMPLANE; i_complane++)
    {
      COMPLANE_SLOPE_VAL[i_tracktype][i_complane].resize(NUMPACKAGE);  //[track type] [UU or VV] [pkg]
      COMPLANE_SLOPE_ERROR[i_tracktype][i_complane].resize(NUMPACKAGE);
      COMPLANE_CHI2_VAL[i_tracktype][i_complane].resize(NUMPACKAGE);
      COMPLANE_CHI2_ERROR[i_tracktype][i_complane].resize(NUMPACKAGE);

      for (int i_pkg = 0; i_pkg < NUMPACKAGE; i_pkg++)
      {
        COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].resize(
            RUNNUMLIST.size(), 0);  //[track type] [UU or VV] [pkg]
        COMPLANE_SLOPE_ERROR[i_tracktype][i_complane][i_pkg].resize(
            RUNNUMLIST.size(), 0);
        COMPLANE_CHI2_VAL[i_tracktype][i_complane][i_pkg].resize(
            RUNNUMLIST.size(), 0);
        COMPLANE_CHI2_ERROR[i_tracktype][i_complane][i_pkg].resize(
            RUNNUMLIST.size(), 0);

      }
    }
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
       * the entries in the vector (*MyTreelineSlope)
       *
       * (*MyTreelineSlope) fill fill the following vectors
       * TREEPLANE_*_*[Front/Back U/V][pkg][run_index]
       * COMPLANE_*_*[treeline][UU/VV][pkg][run_index]
       * NOTE: the COMPLANE info is worthless and will not be used
       ******************/
      for (UInt_t i = 0; i < MyTreelineSlope->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Treelines slope arena - we have data :-)"
         << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*MyTreelineSlope)[i].R3package)
        {
          case 1 :
          case 2 :
            /*
             //debugging
             std::cout << "I am Treeline hear me roar, this is run num: " << run
             << std::endl << std::endl;

             // for the Treelines with no cuts fill the vectors for the plots
             std::cout << "Pkg: " << (*MyTreelineSlope)[i].R3package
             << std::endl;
             std::cout << "VDC: " << (*MyTreelineSlope)[i].VDC << std::endl;
             std::cout << "Plane: " << (*MyTreelineSlope)[i].plane << std::endl;
             std::cout << "slope val: " << (*MyTreelineSlope)[i].Val
             << std::endl;
             std::cout << "slope error: " << (*MyTreelineSlope)[i].Error
             << std::endl << std::endl;
             */

            /*****************
             * Now we have to find what treeplane it is in, and then store to
             * the corresponding vector
             *
             *  0 = Front U
             *  1 = Front V
             *  2 = Back U
             *  3 = Back V
             *
             *  or coplane
             *****************/

            if (std::strcmp("Front", (*MyTreelineSlope)[i].VDC) == 0 &&
                std::strcmp("U", (*MyTreelineSlope)[i].plane) == 0)  //Front U plane
            {
              TREEPLANE_SLOPE_VAL[0][0][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[0][0][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Error;
            } else if (std::strcmp("Front", (*MyTreelineSlope)[i].VDC) == 0
                && std::strcmp("V", (*MyTreelineSlope)[i].plane) == 0)  //Front V plane
            {
              TREEPLANE_SLOPE_VAL[0][1][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[0][1][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Error;
            } else if (std::strcmp("Back", (*MyTreelineSlope)[i].VDC) == 0
                && std::strcmp("U", (*MyTreelineSlope)[i].plane) == 0)  //Back U plane
            {
              TREEPLANE_SLOPE_VAL[0][2][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[0][2][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Error;
            } else if (std::strcmp("Back", (*MyTreelineSlope)[i].VDC) == 0
                && std::strcmp("V", (*MyTreelineSlope)[i].plane) == 0)  //Back V plane
            {
              TREEPLANE_SLOPE_VAL[0][3][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[0][3][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Error;
            } else if (std::strcmp("UU", (*MyTreelineSlope)[i].plane) == 0)  //coplane UU again meaninless
            {
              COMPLANE_SLOPE_VAL[0][0][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
              COMPLANE_SLOPE_ERROR[0][0][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
            } else if (std::strcmp("VV", (*MyTreelineSlope)[i].plane) == 0)  //coplane VV again meaninless
            {
              COMPLANE_SLOPE_VAL[0][1][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
              COMPLANE_SLOPE_ERROR[0][1][(*MyTreelineSlope)[i].R3package][run_index] =
                  (*MyTreelineSlope)[i].Val;
            }

            break;

          default :
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*MyTreelineChi2)
       *
       * (*MyTreelineChi) fill fill the following vectors
       * TREEPLANE_*_*[treeplane][Front/Back U/V][pkg][run_index]
       * COMPLANE_*_*[treeline][UU/VV][pkg][run_index]
       * NOTE: the COMPLANE info is worthless and will not be used
       ******************/

      for (UInt_t i = 0; i < MyTreelineChi2->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Treelines Chi2 arena - we have data :-)"
         << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*MyTreelineChi2)[i].R3package)
        {
          case 1 :
          case 2 :
            /*
             //debugging
             std::cout << "I am Treeline hear me roar, this is run num: " << run
             << std::endl << std::endl;

             // for the Treelines with no cuts fill the vectors for the plots
             std::cout << "Pkg: " << (*MyTreelineChi2)[i].R3package << std::endl;
             std::cout << "VDC: " << (*MyTreelineChi2)[i].VDC << std::endl;
             std::cout << "Plane: " << (*MyTreelineChi2)[i].plane << std::endl;
             std::cout << "Chi2 val: " << (*MyTreelineChi2)[i].Val << std::endl;
             std::cout << "Chi2 RMS: " << (*MyTreelineChi2)[i].RMS << std::endl
             << std::endl;
             */

            /*****************
             * Now we have to find what treeplane it is in, and then store to
             * the corresponding vector
             *
             *  0 = Front U
             *  1 = Front V
             *  2 = Back U
             *  3 = Back V
             *
             *  or coplane
             *****************/

            if (std::strcmp("Front", (*MyTreelineChi2)[i].VDC) == 0 &&
                std::strcmp("U", (*MyTreelineChi2)[i].plane) == 0)  //Front U plane
            {
              TREEPLANE_CHI2_VAL[0][0][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[0][0][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].RMS;
            } else if (std::strcmp("Front", (*MyTreelineChi2)[i].VDC) == 0
                && std::strcmp("V", (*MyTreelineChi2)[i].plane) == 0)  //Front V plane
            {
              TREEPLANE_CHI2_VAL[0][1][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[0][1][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].RMS;
            } else if (std::strcmp("Back", (*MyTreelineChi2)[i].VDC) == 0
                && std::strcmp("U", (*MyTreelineChi2)[i].plane) == 0)  //Back U plane
            {
              TREEPLANE_CHI2_VAL[0][2][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[0][2][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].RMS;
            } else if (std::strcmp("Back", (*MyTreelineChi2)[i].VDC) == 0
                && std::strcmp("V", (*MyTreelineChi2)[i].plane) == 0)  //Back V plane
            {
              TREEPLANE_CHI2_VAL[0][3][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[0][3][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].RMS;
            } else if (std::strcmp("UU", (*MyTreelineChi2)[i].plane) == 0)  //coplane UU again meaninless
            {
              COMPLANE_CHI2_VAL[0][0][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].Val;
              COMPLANE_CHI2_ERROR[0][0][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].RMS;
            } else if (std::strcmp("VV", (*MyTreelineChi2)[i].plane) == 0)  //coplane VV again meaninless
            {
              COMPLANE_CHI2_VAL[0][1][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].Val;
              COMPLANE_CHI2_ERROR[0][1][(*MyTreelineChi2)[i].R3package][run_index] =
                  (*MyTreelineChi2)[i].RMS;
            }

            break;

          default :
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*MyPartialTrackSlope)
       *
       * (*MyPartialTrackSlope) fill fill the following vectors
       * TREEPLANE_*_*[treeplane][Front/Back U/V][pkg][run_index]
       * COMPLANE_*_*[Paritial tracks][UU/VV][pkg][run_index]
       ******************/
      for (UInt_t i = 0; i < MyPartialTrackSlope->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Partial tracks Slope arena - we have data :-)"
         << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*MyPartialTrackSlope)[i].R3package)
        {
          case 1 :
          case 2 :
            /*
             //debugging
             std::cout << "I am Partial Tracks hear me roar, this is run num: "
             << run << std::endl << std::endl;

             // for the Treelines with no cuts fill the vectors for the plots
             std::cout << "Pkg: " << (*MyPartialTrackSlope)[i].R3package
             << std::endl;
             std::cout << "VDC: " << (*MyPartialTrackSlope)[i].VDC << std::endl;
             std::cout << "Plane: " << (*MyPartialTrackSlope)[i].plane
             << std::endl;
             std::cout << "slope val: " << (*MyPartialTrackSlope)[i].Val
             << std::endl;
             std::cout << "slope error: " << (*MyPartialTrackSlope)[i].Error
             << std::endl << std::endl;
             */

            /*****************
             * Now we have to find what partial track it is in, and then store to
             * the corresponding vector
             *
             *  For the combintation planes
             *  0 = UU
             *  1 = VV
             *
             * the treelines
             *  0 = Front U
             *  1 = Front V
             *  2 = Back U
             *  3 = Back V
             *****************/

            if (std::strcmp("Front", (*MyPartialTrackSlope)[i].VDC) == 0 &&
                std::strcmp("U", (*MyPartialTrackSlope)[i].plane) == 0)  //Front U plane
            {
              TREEPLANE_SLOPE_VAL[1][0][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[1][0][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Error;
            } else if (std::strcmp("Front", (*MyPartialTrackSlope)[i].VDC) == 0
                && std::strcmp("V", (*MyPartialTrackSlope)[i].plane) == 0)  //Front V plane
            {
              TREEPLANE_SLOPE_VAL[1][1][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[1][1][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Error;
            } else if (std::strcmp("Back", (*MyPartialTrackSlope)[i].VDC) == 0
                && std::strcmp("U", (*MyPartialTrackSlope)[i].plane) == 0)  //Back U plane
            {
              TREEPLANE_SLOPE_VAL[1][2][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[1][2][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Error;
            } else if (std::strcmp("Back", (*MyPartialTrackSlope)[i].VDC) == 0
                && std::strcmp("V", (*MyPartialTrackSlope)[i].plane) == 0)  //Back V plane
            {
              TREEPLANE_SLOPE_VAL[1][3][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[1][3][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Error;
            } else if (std::strcmp("UU", (*MyPartialTrackSlope)[i].plane) == 0)  //UU plane
            {
              COMPLANE_SLOPE_VAL[1][0][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Val;
              COMPLANE_SLOPE_ERROR[1][0][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Error;
            } else if (std::strcmp("VV", (*MyPartialTrackSlope)[i].plane) == 0)  //VV plane
            {
              COMPLANE_SLOPE_VAL[1][1][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Val;
              COMPLANE_SLOPE_ERROR[1][1][(*MyPartialTrackSlope)[i].R3package][run_index] =
                  (*MyPartialTrackSlope)[i].Error;
            }

            break;

          default :
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*MyPartialTrackChi2)
       *
       * (*MyPartialTrackSlope) fill fill the following vectors
       * COMPLANE_*_*[Paritial tracks][UU/VV][pkg][run_index]
       ******************/
      for (UInt_t i = 0; i < MyPartialTrackChi2->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Partial tracks Chi arena - we have data :-)"
         << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*MyPartialTrackChi2)[i].R3package)
        {
          case 1 :
          case 2 :
            /*
             //debugging
             std::cout << "I am Partial Tracks hear me roar, this is run num: "
             << run << std::endl << std::endl;

             // for the Treelines with no cuts fill the vectors for the plots
             std::cout << "Pkg: " << (*MyPartialTrackChi2)[i].R3package
             << std::endl;
             std::cout << "VDC: " << (*MyPartialTrackChi2)[i].VDC << std::endl;
             std::cout << "Plane: " << (*MyPartialTrackChi2)[i].plane
             << std::endl;
             std::cout << "Chi2 val: " << (*MyPartialTrackChi2)[i].Val
             << std::endl;
             std::cout << "Chi2 RMS: " << (*MyPartialTrackChi2)[i].RMS
             << std::endl << std::endl;
             */

            /*****************
             * Now we have to find what partial track it is in, and then store to
             * the corresponding vector
             *
             *  0 = UU
             *  1 = VV
             *****************/

            if (std::strcmp("Front", (*MyPartialTrackChi2)[i].VDC) == 0 &&
                std::strcmp("U", (*MyPartialTrackChi2)[i].plane) == 0)  //Front U plane
            {
              TREEPLANE_CHI2_VAL[1][0][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[1][0][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].RMS;
            } else if (std::strcmp("Front", (*MyPartialTrackChi2)[i].VDC) == 0
                && std::strcmp("V", (*MyPartialTrackChi2)[i].plane) == 0)  //Front V plane
            {
              TREEPLANE_CHI2_VAL[1][1][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[1][1][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].RMS;
            } else if (std::strcmp("Back", (*MyPartialTrackChi2)[i].VDC) == 0
                && std::strcmp("U", (*MyPartialTrackChi2)[i].plane) == 0)  //Back U plane
            {
              TREEPLANE_CHI2_VAL[1][2][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[1][2][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].RMS;
            } else if (std::strcmp("Back", (*MyPartialTrackChi2)[i].VDC) == 0
                && std::strcmp("V", (*MyPartialTrackChi2)[i].plane) == 0)  //Back V plane
            {
              TREEPLANE_CHI2_VAL[1][3][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[1][3][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].RMS;
            } else if (std::strcmp("UU", (*MyPartialTrackChi2)[i].plane) == 0)  //UU plane
            {
              COMPLANE_CHI2_VAL[1][0][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].Val;
              COMPLANE_CHI2_ERROR[1][0][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].RMS;
            } else if (std::strcmp("VV", (*MyPartialTrackChi2)[i].plane) == 0)  //VV plane
            {
              COMPLANE_CHI2_VAL[1][1][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].Val;
              COMPLANE_CHI2_ERROR[1][1][(*MyPartialTrackChi2)[i].R3package][run_index] =
                  (*MyPartialTrackChi2)[i].RMS;
            }

            break;

          default :
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*MyTrackSlope)
       *
       * (*MyTrackSlope) fill fill the following vectors
       * COMPLANE_*_*[tracks][UU/VV][pkg][run_index]
       ******************/
      for (UInt_t i = 0; i < MyTrackSlope->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Tracks Slope arena - we have data :-)"
         << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*MyTrackSlope)[i].R3package)
        {
          case 1 :
          case 2 :
            /*
             //debugging
             std::cout << "I am Tracks hear me roar, this is run num: " << run
             << std::endl << std::endl;

             // for the Tracks with no cuts fill the vectors for the plots
             std::cout << "Pkg: " << (*MyTrackSlope)[i].R3package << std::endl;
             std::cout << "VDC: " << (*MyTrackSlope)[i].VDC << std::endl;
             std::cout << "Plane: " << (*MyTrackSlope)[i].plane << std::endl;
             std::cout << "slope val: " << (*MyTrackSlope)[i].Val << std::endl;
             std::cout << "slope error: " << (*MyTrackSlope)[i].Error
             << std::endl << std::endl;
             */

            /*****************
             * Now we have to find what-------------------------------------------------------------------------- track it is in, and then store to
             * the corresponding vector
             *
             *  0 = UU
             *  1 = VV
             *****************/

            if (std::strcmp("Front", (*MyTrackSlope)[i].VDC) == 0 &&
                std::strcmp("U", (*MyTrackSlope)[i].plane) == 0)  //Front U plane
            {
              TREEPLANE_SLOPE_VAL[2][0][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[2][0][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Error;
            } else if (std::strcmp("Front", (*MyTrackSlope)[i].VDC) == 0
                && std::strcmp("V", (*MyTrackSlope)[i].plane) == 0)  //Front V plane
            {
              TREEPLANE_SLOPE_VAL[2][1][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[2][1][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Error;
            } else if (std::strcmp("Back", (*MyTrackSlope)[i].VDC) == 0
                && std::strcmp("U", (*MyTrackSlope)[i].plane) == 0)  //Back U plane
            {
              TREEPLANE_SLOPE_VAL[2][2][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[2][2][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Error;
            } else if (std::strcmp("Back", (*MyTrackSlope)[i].VDC) == 0
                && std::strcmp("V", (*MyTrackSlope)[i].plane) == 0)  //Back V plane
            {
              TREEPLANE_SLOPE_VAL[2][3][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Val;
              TREEPLANE_SLOPE_ERROR[2][3][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Error;
            } else if (std::strcmp("UU", (*MyTrackSlope)[i].plane) == 0)  //UU plane
            {
              COMPLANE_SLOPE_VAL[2][0][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Val;
              COMPLANE_SLOPE_ERROR[2][0][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Error;
            } else if (std::strcmp("VV", (*MyTrackSlope)[i].plane) == 0)  //VV plane
            {
              COMPLANE_SLOPE_VAL[2][1][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Val;
              COMPLANE_SLOPE_ERROR[2][1][(*MyTrackSlope)[i].R3package][run_index] =
                  (*MyTrackSlope)[i].Error;
            }

            break;

          default :
            break;
        }
      }

      /*****************
       * With the structures in ValerianROOT we have to now loop through
       * the entries in the vector (*MyTrackChi2)
       *
       * (*MyTrackChi2) fill fill the following vectors
       * COMPLANE_*_*[Tracks][UU/VV][pkg][run_index]
       ******************/
      for (UInt_t i = 0; i < MyTrackChi2->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the tracks Chi 2 arena - we have data :-)"
         << std::cout;
         */

        /*****************
         * If there is no data for this run, i_entry, then there is
         * one and only one entry for this run in any one type.
         * This values is -1e6.
         * So we must skip this and move on.  Fill the vector with zeros,
         * which I have done in resizing them, Oh Happy Day!!
         *
         * So by using this switch statement, this is accomplished,
         * IFF the pkg is 1, or 2 in this case
         *
         ******************/

        //what pkg are we in?
        switch ((*MyTrackChi2)[i].R3package)
        {
          case 1 :
          case 2 :
            /*
             //debugging
             std::cout << "I am Tracks hear me roar, this is run num: " << run
             << std::endl << std::endl;

             // for the Treelines with no cuts fill the vectors for the plots
             std::cout << "Pkg: " << (*MyTrackChi2)[i].R3package << std::endl;
             std::cout << "VDC: " << (*MyTrackChi2)[i].VDC << std::endl;
             std::cout << "Plane: " << (*MyTrackChi2)[i].plane << std::endl;
             std::cout << "Chi2 val: " << (*MyTrackChi2)[i].Val << std::endl;
             std::cout << "Chi2 RMS: " << (*MyTrackChi2)[i].RMS << std::endl
             << std::endl;
             */

            /*****************
             * Now we have to find what partial track it is in, and then store to
             * the corresponding vector
             *
             *  0 = UU
             *  1 = VV
             *****************/

            if (std::strcmp("Front", (*MyTrackChi2)[i].VDC) == 0 &&
                std::strcmp("U", (*MyTrackChi2)[i].plane) == 0)  //Front U plane
            {
              TREEPLANE_CHI2_VAL[2][0][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[2][0][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].RMS;
            } else if (std::strcmp("Front", (*MyTrackChi2)[i].VDC) == 0
                && std::strcmp("V", (*MyTrackChi2)[i].plane) == 0)  //Front V plane
            {
              TREEPLANE_CHI2_VAL[2][1][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[2][1][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].RMS;
            } else if (std::strcmp("Back", (*MyTrackChi2)[i].VDC) == 0
                && std::strcmp("U", (*MyTrackChi2)[i].plane) == 0)  //Back U plane
            {
              TREEPLANE_CHI2_VAL[2][2][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[2][2][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].RMS;
            } else if (std::strcmp("Back", (*MyTrackChi2)[i].VDC) == 0
                && std::strcmp("V", (*MyTrackChi2)[i].plane) == 0)  //Back V plane
            {
              TREEPLANE_CHI2_VAL[2][3][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].Val;
              TREEPLANE_CHI2_ERROR[2][3][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].RMS;
            } else if (std::strcmp("UU", (*MyTrackChi2)[i].plane) == 0)  //UU plane
            {
              COMPLANE_CHI2_VAL[2][0][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].Val;
              COMPLANE_CHI2_ERROR[2][0][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].RMS;
            } else if (std::strcmp("VV", (*MyTrackChi2)[i].plane) == 0)  //VV plane
            {
              COMPLANE_CHI2_VAL[2][1][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].Val;
              COMPLANE_CHI2_ERROR[2][1][(*MyTrackChi2)[i].R3package][run_index] =
                  (*MyTrackChi2)[i].RMS;
            }

            break;

          default :
            break;
        }
      }

      //Increment run index so we can look at the next run
      run_index++;
    }

  }
  /*
   //debugging
   cout << "number of runs: " << RUNNUMLIST.size() << std::endl << "Run index: "
   << run_index << std::endl;
   */

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
 - g_TREEPLANE_SLOPE_VS_RUN -treeline vs slope graph
 - g_TREEPLANE_CHI_VS_RUN -treeline vs chi graph
 - g_COMPLANE_SLOPE_VS_RUN -treeline vs slope graph
 - g_COMPLANE_CHI_VS_RUN -treeline vs chi graph

 - mg_TREEPLANE_SLOPE_VS_RUN -treeline vs slope multigraph
 - mg_TREEPLANE_CHI_VS_RUN -treeline vs chi muiltigraph
 - mg_COMPLANE_SLOPE_VS_RUN -treeline vs slope muiltigraph
 - mg_COMPLANE_CHI_VS_RUN -treeline vs chi muiltigraph
 - NUMPACKAGES - the number of packages
 - NUMTREEPLANE - the number of different axis
 - NUMCOMPLANE - the number of packages

 - TREEPLANE_SLOPE_VAL - vector of the slope values
 - TREEPLANE_SLOPE_ERROR - vector of the slope errors
 - TREEPLANE_CHI_VAL - vector of the chi values
 - TREEPLANE_CHI_RMS - vector of the chi errors
 - COMPLANE_SLOPE_VAL - vector of the slope values
 - COMPLANE_SLOPE_ERROR - vector of the slope errors
 - COMPLANE_CHI_VAL - vector of the chi values
 - COMPLANE_CHI_RMS - vector of the chi errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 - RUNNUMLIST - the vector containing the list of runs of interest (ROI)

 - INDEXTOPKG - array to get what package that we are in
 - INDEXTOINDEXTOTREEPLANE - array to get what tree plane we are in
 - INDEXTOINDEXTOCOMPLANE - array to get what combined plane we are in
 - INDEXTOTRACKTYPE - index of what track type, tree, partial, track

 Exit Conditions: none
 Called By: TreeMatch_vs_Run
 Date: 07-11-2014
 Modified: 11-12-2014
 *********************************************************/
void Make_graphs()
{
  /*
   //debugging
   std::cout << "made it in the make graph function" << std::endl;
   */

  //resize and define the mulitgraphs and legend
  mg_TREEPLANE_SLOPE_VS_RUN.resize(NUMPACKAGE);
  mg_TREEPLANE_CHI2_VS_RUN.resize(NUMPACKAGE);
  mg_COMPLANE_SLOPE_VS_RUN.resize(NUMPACKAGE);
  mg_COMPLANE_CHI2_VS_RUN.resize(NUMPACKAGE);
  LEGEND_TREE.resize(NUMPACKAGE);
  LEGEND_TRACKS.resize(NUMPACKAGE);

  for (UInt_t i_pkg = 0; i_pkg < NUMPACKAGE; i_pkg++)
  {
    mg_TREEPLANE_SLOPE_VS_RUN[i_pkg] = new TMultiGraph(
        Form("mg_TREEPLANE_SLOPE_VS_RUN[%d]", i_pkg),
        Form("Treeline level Slope vs run number for %s Package",
            INDEXTOPKG[i_pkg].c_str()));
    mg_TREEPLANE_CHI2_VS_RUN[i_pkg] = new TMultiGraph(
        Form("mg_TREEPLANE_CHI2_VS_RUN[%d]", i_pkg),
        Form("Treeline level Chi^{2} vs run number for %s Package",
            INDEXTOPKG[i_pkg].c_str()));
    mg_COMPLANE_SLOPE_VS_RUN[i_pkg] = new TMultiGraph(
        Form("mg_COMPLANE_SLOPE_VS_RUN[%d]", i_pkg),
        Form("Partial Track and Track level Slope vs run number for %s Package",
            INDEXTOPKG[i_pkg].c_str()));
    mg_COMPLANE_CHI2_VS_RUN[i_pkg] =
        new TMultiGraph(Form("mg_COMPLANE_CHI2_VS_RUN[%d]", i_pkg),
            Form(
                "Partial Track and Track level Chi^{2} vs run number for %s Package",
                INDEXTOPKG[i_pkg].c_str()));

    LEGEND_TREE[i_pkg] = new TLegend(0.80, 0.20, 0.99, 0.80);
    LEGEND_TREE[i_pkg]->SetHeader("Treeline");
    LEGEND_TRACKS[i_pkg] = new TLegend(0.80, 0.40, 0.99, 0.60);
    LEGEND_TRACKS[i_pkg]->SetHeader("Track");

    //initalize the max and min values - Max very small, Min large so they must be replaced
    TREEPLANE_SLOPE_MAX[i_pkg] = -1e6;
    TREEPLANE_SLOPE_MIN[i_pkg] = 1e6;
    TREEPLANE_CHI2_MAX[i_pkg] = -1e6;
    TREEPLANE_CHI2_MIN[i_pkg] = 1e6;
    COMPLANE_SLOPE_MAX[i_pkg] = -1e6;
    COMPLANE_SLOPE_MIN[i_pkg] = 1e6;
    COMPLANE_CHI2_MAX[i_pkg] = -1e6;
    COMPLANE_CHI2_MIN[i_pkg] = 1e6;
  }

//resize the vector of the to the number of treelines and pkgs
//and fill them
  g_TREEPLANE_SLOPE_VS_RUN.resize(NUMTRACKTYPE);
  g_TREEPLANE_CHI2_VS_RUN.resize(NUMTRACKTYPE);

  for (UInt_t i_tracktype = 0; i_tracktype < NUMTRACKTYPE; i_tracktype++)
  {
    g_TREEPLANE_SLOPE_VS_RUN[i_tracktype].resize(NUMTREEPLANE);
    g_TREEPLANE_CHI2_VS_RUN[i_tracktype].resize(NUMTREEPLANE);

    for (UInt_t i_treeplane = 0; i_treeplane < NUMTREEPLANE; i_treeplane++)
    {
      g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane].resize(NUMPACKAGE);
      g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane].resize(NUMPACKAGE);

      //fill the graphs
      for (UInt_t i_pkg = 0; i_pkg < NUMPACKAGE; i_pkg++)
      {
        //take the vectors and turn them into arrays so that TGraphErrors will play nice
        //with them by &(vector[0]) :)
        g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg] =
            new TGraphErrors(RUNNUMLIST.size(), &(RUN_VAL[0]),
                &(TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg][0]),
                &(RUN_ERROR[0]),
                &(TREEPLANE_SLOPE_ERROR[i_tracktype][i_treeplane][i_pkg][0]));

        g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg] =
            new TGraphErrors(RUNNUMLIST.size(), &(RUN_VAL[0]),
                &(TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg][0]),
                &(RUN_ERROR[0]),
                &(TREEPLANE_CHI2_ERROR[i_tracktype][i_treeplane][i_pkg][0]));

        /*******************************
         * Set the attributes of the plot
         *
         * Treelines
         * Black filled circle - Back U plane
         * Red filled triangle point up - Back V plane
         * Blue filled triangle point down - Front U plane
         * Purple filled square - Front V plane
         *
         * Partial Tracks
         * green filled star - Back U plane
         * magenta filled diamond - Back V plane
         * Gray filled hospital sign - Front U plane
         * cyan hallow circle - Front V plane
         *
         * Tracks
         * Orange hallow square - Back U plane
         * yellow triangle point up - Back V plane
         * pink hallow dimond - Front U plane
         * teal triangle point down - Front V plane
         *******************************/
        switch (i_tracktype)
        {
          case 0 :

/*            //debugging
            cout << "Track type: 0, " << INDEXTOTRACKTYPE[i_tracktype].c_str()
            << endl;
            */

            switch (i_treeplane)
            {
              case 0 :  // Front U - Blue filled triangle point down

/*                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
                */

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Blue
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kBlue);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kBlue);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1);
                //set marker style - filled triangle pint up
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    23);

                //CHI2
                //set markercolor && error bar color - Blue
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kBlue);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kBlue);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1);
                //set marker style - filled triangle pint up
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    23);
                break;

              case 1 :  //Front V - Purple filled square

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Violet
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kViolet - 6);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kViolet - 6);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1);
                //set marker style - filled square
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    21);

                //CHI2
                //set markercolor && error bar color - Violet
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kViolet - 6);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kViolet - 6);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1);
                //set marker style - filled square
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    21);
                break;

              case 2 :  //Back U - Black filled circle

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Black
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kBlack);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kBlack);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1);
                //set marker style - filled circle
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    8);

                //CHI2
                //set markercolor && error bar color - Black
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kBlack);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kBlack);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1);
                //set marker style - filled circle
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    8);
                break;

              case 3 :  //Back V - Red filled triangle point up

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Red
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kRed);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kRed);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - filled triangle pint up
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    22);

                //CHI2
                //set markercolor && error bar color - Red
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kRed);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kRed);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - filled triangle pint up
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    22);
                break;

              default :
                break;
            }
            break;

          case 1 :  //Partial Tracks

/*
            //debugging
            cout << "Track type: 1, " << INDEXTOTRACKTYPE[i_tracktype].c_str()
            << endl;
*/

            switch (i_treeplane)
            {
              case 0 :  // Front U - green filled star

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - green
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kGreen + 3);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kGreen + 3);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.7);
                //set marker style - filled star
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    29);

                //CHI2
                //set markercolor && error bar color - green
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kGreen + 3);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kGreen + 3);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.7);
                //set marker style - filled star
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    29);
                break;

              case 1 :  //Front V - magenta filled diamond

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - magenta
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kMagenta);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kMagenta);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - filled dimond
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    33);

                //CHI2
                //set markercolor && error bar color - magenta
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kMagenta);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kMagenta);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - filled star
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    33);
                break;

              case 2 :  //Back U - Gray filled hospital sign

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Gray
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kGray + 2);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kGray + 2);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - filled hospital sign
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    34);

                //CHI2
                //set markercolor && error bar color - Gray
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kGray + 2);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kGray + 2);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - filled hospital sign
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    34);
                break;

              case 3 :  //Back V - cyan hallow circle

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - cyan
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kCyan + 2);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kCyan + 2);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow circle
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    24);

                //CHI2
                //set markercolor && error bar color - Cyan
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kCyan + 2);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kCyan + 2);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow circle
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    24);
                break;

              default :
                break;
            }
            break;

          case 2 :  //tracks

/*
            //debugging
            cout << "Track type: 2, " << INDEXTOTRACKTYPE[i_tracktype].c_str()
            << endl;
*/

            switch (i_treeplane)
            {
              case 0 :  // Front U - Orange hallow square

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Orange
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kOrange - 3);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kOrange - 3);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow square
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    25);

                //CHI2
                //set markercolor && error bar color - Orange
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kOrange + 3);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kOrange + 3);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow square
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    25);
                break;

              case 1 :  //Front V - yellow triangle point up

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - yellow
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kYellow - 6);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kYellow - 6);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow triangle up
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    26);

                //CHI2
                //set markercolor && error bar color - yellow
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kYellow - 6);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kYellow - 6);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow triangle up
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    26);
                break;

              case 2 :  //Back U - pink hallow dimond

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - pink
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kPink - 9);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kPink - 9);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow dimond
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    27);

                //CHI2
                //set markercolor && error bar color - pink
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kPink - 9);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kPink - 9);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow dimond
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    27);
                break;

              case 3 :  //Back V - teal triangle point down

/*
                //debugging
                cout << "Track type: " << INDEXTOTRACKTYPE[i_tracktype].c_str()
                << " Plane: " << INDEXTOTREEPLANE[i_treeplane].c_str() << endl;
*/

                //set it so the errors print
                gStyle->SetOptFit(1110);

                //SLOPE
                //set markercolor && error bar color - Teal
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kTeal - 6);
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kTeal - 6);
                //set Marker size
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - hallow triangle pint down
                g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    32);

                //CHI2
                //set markercolor && error bar color - Teal
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerColor(
                    kTeal - 6);
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetLineColor(
                    kTeal - 6);
                //set Marker size
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerSize(
                    1.5);
                //set marker style - Hallow triangle pint down
                g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]->SetMarkerStyle(
                    32);
                break;

              default :
                break;
            }
            break;

          default :
            break;
        }
        //add the wanted graphs to the multigraphs
        mg_TREEPLANE_SLOPE_VS_RUN[i_pkg]->Add(
            g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg]);
        //add the wanted graphs to the multigraphs
        mg_TREEPLANE_CHI2_VS_RUN[i_pkg]->Add(
            g_TREEPLANE_CHI2_VS_RUN[i_tracktype][i_treeplane][i_pkg]);

        //add to the Legend
        LEGEND_TREE[i_pkg]->AddEntry(
            g_TREEPLANE_SLOPE_VS_RUN[i_tracktype][i_treeplane][i_pkg],
            Form("%s - %s", INDEXTOTREEPLANE[i_treeplane].c_str(),
                INDEXTOTRACKTYPE[i_tracktype].c_str()), "ep");

        if (i_pkg != 0)
        {
          /*
           //debugging
           std::cout << "TREEPLANE SLOPE \n Entry \t Run \t Value \t PKG: " << i_pkg << std::endl;
           for (UInt_t i_run = 0; i_run < RUNNUMLIST.size(); i_run++)
           {
           std::cout << i_run << " \t " << RUNNUMLIST[i_run] << " \t "
           << TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg][i_run] << std::endl;
           }

           std::cout << "Max: "
           << *std::max_element(TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].end())
           << std::endl;

           std::cout << "Min: "
           << *std::min_element(TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].end(),
           less_than_but_non_zero)
           << std::endl;

           std::cout << "Total Max: "
           << std::max(TREEPLANE_SLOPE_MAX[i_pkg],
           *std::max_element(TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].end()))
           << std::endl;

           std::cout << "Total Min: "
           << std::min(TREEPLANE_SLOPE_MIN[i_pkg],
           *std::min_element(TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].end(),
           less_than_but_non_zero))
           << std::endl << std::endl;
           */

          //get the min and max Treeline slope and Chi2 values
          TREEPLANE_SLOPE_MAX[i_pkg] = std::max(TREEPLANE_SLOPE_MAX[i_pkg],
              *std::max_element(
                  TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
                  TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].end()));

          TREEPLANE_SLOPE_MIN[i_pkg] = std::min(TREEPLANE_SLOPE_MIN[i_pkg],
              *std::min_element(
                  TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
                  TREEPLANE_SLOPE_VAL[i_tracktype][i_treeplane][i_pkg].end(),
                  less_than_but_non_zero));

          /*
           //debugging
           std::cout << "TREEPLANE_SLOPE_MAX: " << TREEPLANE_SLOPE_MAX[i_pkg] << std::endl;
           std::cout << "TREEPLANE_SLOPE_MIN: " << TREEPLANE_SLOPE_MIN[i_pkg] << std::endl << std::endl;
           */

          /*       //debugging
           std::cout << "TREEPLANE CHI \n Entry \t Run \t Value" << std::endl;
           for (UInt_t i_run = 0; i_run < RUNNUMLIST.size(); i_run++)
           {
           std::cout << i_run << " \t " << RUNNUMLIST[i_run] << " \t "
           << TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg][i_run] << std::endl;
           }

           std::cout << "Max: "
           << *std::max_element(TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].end())
           << std::endl;

           std::cout << "Min: "
           << *std::min_element(TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].end(),
           less_than_but_non_zero)
           << std::endl;

           std::cout << "Total Max: "
           << std::max(TREEPLANE_CHI2_MAX[i_pkg],
           *std::max_element(TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].end()))        //debugging
           std::cout << "TREEPLANE_SLOPE_MAX: " << TREEPLANE_SLOPE_MAX[i_pkg] << std::endl;
           std::cout << "TREEPLANE_SLOPE_MIN: " << TREEPLANE_SLOPE_MIN[i_pkg] << std::endl << std::endl;

           << std::endl;

           std::cout << "Total Min: "
           << std::min(TREEPLANE_CHI2_MIN[i_pkg],
           *std::min_element(TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
           TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].end(),
           less_than_but_non_zero))
           << std::endl << std::endl;
           */

          TREEPLANE_CHI2_MAX[i_pkg] = std::max(TREEPLANE_CHI2_MAX[i_pkg],
              *std::max_element(
                  TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
                  TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].end()));

          TREEPLANE_CHI2_MIN[i_pkg] = std::min(TREEPLANE_CHI2_MIN[i_pkg],
              *std::min_element(
                  TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].begin(),
                  TREEPLANE_CHI2_VAL[i_tracktype][i_treeplane][i_pkg].end(),
                  less_than_but_non_zero));

          /*
           //debugging
           std::cout << "TREEPLANE_CHI2_MAX: " << TREEPLANE_CHI2_MAX[i_pkg] << std::endl;
           std::cout << "TREEPLANE_CHI2_MIN: " << TREEPLANE_CHI2_MIN[i_pkg] << std::endl << std::endl;
           */
        }

      }
    }

  }

//resize the coplane vectors and fill them
  g_COMPLANE_SLOPE_VS_RUN.resize(NUMTRACKTYPE);
  g_COMPLANE_CHI2_VS_RUN.resize(NUMTRACKTYPE);

//i_tracktype starts from 1 because we don't have treeline information here
  for (UInt_t i_tracktype = 1; i_tracktype < NUMTRACKTYPE; i_tracktype++)
  {
    g_COMPLANE_SLOPE_VS_RUN[i_tracktype].resize(NUMCOMPLANE);
    g_COMPLANE_CHI2_VS_RUN[i_tracktype].resize(NUMCOMPLANE);

    for (UInt_t i_complane = 0; i_complane < NUMCOMPLANE; i_complane++)
    {
      g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane].resize(NUMPACKAGE);
      g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane].resize(NUMPACKAGE);

      //i_pkg starts at 1 since have no information form both pkgs
      for (UInt_t i_pkg = 1; i_pkg < NUMPACKAGE; i_pkg++)
      {
        //take the vectors and turn them into arrays so that TGraphErrors will play nice
        //with them by &(vector[0]) :)
        g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg] =
            new TGraphErrors(RUNNUMLIST.size(), &(RUN_VAL[0]),
                &(COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg][0]),
                &(RUN_ERROR[0]),
                &(COMPLANE_SLOPE_ERROR[i_tracktype][i_complane][i_pkg][0]));

        g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg] =
            new TGraphErrors(RUNNUMLIST.size(), &(RUN_VAL[0]),
                &(COMPLANE_CHI2_VAL[i_tracktype][i_complane][i_pkg][0]),
                &(RUN_ERROR[0]),
                &(COMPLANE_CHI2_ERROR[i_tracktype][i_complane][i_pkg][0]));

        /*******************************
         * Set the attributes of the plot
         * Black filled circle - UU Partial Track
         * Red filled triangle point up - UU Track
         * Blue filled triangle point down - VV Partial Track
         * Purple filled square - VV Track
         *******************************/
        if (i_tracktype == 1 && i_complane == 0)  //UU Partial Track -  Black filled circle
        {
          //set it so the errors print
          gStyle->SetOptFit(1110);

          //SLOPE
          //set markercolor && error bar color - Black
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kBlack);
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kBlack);
          //set Marker size
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled circle
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              8);

          //CHI2
          //set markercolor && error bar color - Black
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kBlack);
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kBlack);
          //set Marker size
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled circle
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              8);
        } else if (i_tracktype == 2 && i_complane == 0)  //UU Track -  Red filled triangle point up
        {
          //set it so the errors print
          gStyle->SetOptFit(1110);

          //SLOPE
          //set markercolor && error bar color - Red
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kRed);
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kRed);
          //set Marker size
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled triangle pint up
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              22);

          //CHI2
          //set markercolor && error bar color - Red
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kRed);
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kRed);
          //set Marker size
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled triangle pint up
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              22);
        } else if (i_tracktype == 1 && i_complane == 1)  // VV Partial Track - Blue filled triangle point down
        {
          //set it so the errors print
          gStyle->SetOptFit(1110);

          //SLOPE
          //set markercolor && error bar color - Blue
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kBlue);
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kBlue);
          //set Marker size
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled triangle pint up
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              23);

          //CHI2
          //set markercolor && error bar color - Blue
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kBlue);
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kBlue);
          //set Marker size
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled triangle pint up
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              23);
        } else if (i_tracktype == 2 && i_complane == 1)  //VV Track - Purple filled square
        {
          //set it so the errors print
          gStyle->SetOptFit(1110);

          //SLOPE
          //set markercolor && error bar color - Violet
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kViolet - 1);
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kViolet - 1);
          //set Marker size
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled square
          g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              21);

          //CHI2
          //set markercolor && error bar color - Violet
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerColor(
              kViolet - 1);
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetLineColor(
              kViolet - 1);
          //set Marker size
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerSize(
              1);
          //set marker style - filled square
          g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]->SetMarkerStyle(
              21);
        }

        //add the wanted graphs to the multigraphs
        mg_COMPLANE_SLOPE_VS_RUN[i_pkg]->Add(
            g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg]);
        //add the wanted graphs to the multigraphs
        mg_COMPLANE_CHI2_VS_RUN[i_pkg]->Add(
            g_COMPLANE_CHI2_VS_RUN[i_tracktype][i_complane][i_pkg]);

        //add to the Legend
        LEGEND_TRACKS[i_pkg]->AddEntry(
            g_COMPLANE_SLOPE_VS_RUN[i_tracktype][i_complane][i_pkg],
            Form("%s - %s", INDEXTOCOMPLANE[i_complane].c_str(),
                INDEXTOTRACKTYPE[i_tracktype].c_str()), "ep");

        /*
         //get min and max values
         //debugging
         std: cout << "COMPLANE SLOPE \n Entry \t Run \t Value \t PKG: " << i_pkg << std::endl;
         for (UInt_t i_run = 1; i_run < RUNNUMLIST.size(); i_run++)
         {
         std::cout << i_run << " \t " << RUNNUMLIST[i_run] << " \t "
         << COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg][i_run]
         << std::endl;
         }

         std::cout << "Max: "
         << *std::max_element(COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].begin(),
         COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].end())
         << std::endl;

         std::cout << "Min: "
         << *std::min_element(COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].begin(),
         COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].end(),
         less_than_but_non_zero)
         << std::endl;

         std::cout << "Total Max: "
         << std::max(COMPLANE_SLOPE_MAX[i_pkg],
         *std::max_element(COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].begin(),
         COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].end()))
         << std::endl;

         std::cout << "Total Min: "
         << std::min(COMPLANE_CHI2_MIN[i_pkg],
         *std::min_element(COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].begin(),
         COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].end(),
         less_than_but_non_zero))
         << std::endl << std::endl;
         */

        //get the min and max Treeline slope and Chi2 values
        COMPLANE_SLOPE_MAX[i_pkg] = std::max(COMPLANE_SLOPE_MAX[i_pkg],
            *std::max_element(
                COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].begin(),
                COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].end()));

        COMPLANE_SLOPE_MIN[i_pkg] = std::min(COMPLANE_SLOPE_MIN[i_pkg],
            *std::min_element(
                COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].begin(),
                COMPLANE_SLOPE_VAL[i_tracktype][i_complane][i_pkg].end(),
                less_than_but_non_zero));

        COMPLANE_CHI2_MAX[i_pkg] = std::max(COMPLANE_CHI2_MAX[i_pkg],
            *std::max_element(
                COMPLANE_CHI2_VAL[i_tracktype][i_complane][i_pkg].begin(),
                COMPLANE_CHI2_VAL[i_tracktype][i_complane][i_pkg].end()));

        COMPLANE_CHI2_MIN[i_pkg] = std::min(COMPLANE_CHI2_MIN[i_pkg],
            *std::min_element(
                COMPLANE_CHI2_VAL[i_tracktype][i_complane][i_pkg].begin(),
                COMPLANE_CHI2_VAL[i_tracktype][i_complane][i_pkg].end(),
                less_than_but_non_zero));

        /*
         std::cout << "COMPLANE_SLOPE_MAX: " << COMPLANE_SLOPE_MAX[i_pkg] << std::endl;
         std::cout << "COMPLANE_SLOPE_MIN: " << COMPLANE_SLOPE_MIN[i_pkg] << std::endl;
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
 - mg_TREEPLANE_SLOPE_VS_RUN -treeline vs slope multigraph
 - mg_TREEPLANE_CHI_VS_RUN -treeline vs chi muiltigraph
 - mg_COMPLANE_SLOPE_VS_RUN -treeline vs slope muiltigraph
 - mg_COMPLANE_CHI_VS_RUN -treeline vs chi muiltigraph

 - NUMPACKAGES - the number of packages
 - NUMTREEPLANE - the number of different axis
 - NUMCOMPLANE - the number of packages

 - INDEXTOPKG - array to get what package that we are in
 - INDEXTOINDEXTOTREEPLANE - array to get what tree plane we are in
 - INDEXTOINDEXTOCOMPLANE - array to get what combined plane we are in
 - INDEXTOTRACKTYPE - index of what track type, tree, partial, track

 - TREEPLANE_SLOPE_MIN - Min slope value for the treelines
 - TREEPLANE_SLOPE_MAX - Max slope value for the treelines
 - TREEPLANE_CHI2_MIN - Min chi2 value for the treelines
 - TREEPLANE_CHI2_MAX - Max chi2 value for the treelines
 - CPMPLANE_SLOPE_MIN - Min slope value for the complanes
 - CPMPLANE_SLOPE_MAX - Max slope value for the complanes
 - CPMPLANE_CHI2_MIN - Min chi2 value for the complanes
 - CPMPLANE_CHI2_MAX - Max chi2 value for the complanes

 - LEGEND_TREE - the legned for the treelines
 - LEGEND_TRACKS - the legned fpr partial and tracks

 - FUDGEFACTOR_TREEPLANE_SLOPE - buffer for the treelines Slope axis
 - FUDGEFACTOR_TREEPLANE_CHI2 - buffer for the treelines Chi2 axis
 - FUDGEFACTOR_COMPLANE_SLOPE - buffer for the coplane Slope axis
 - FUDGEFACTOR_COMPLANE_CHI2 - buffer for the coplane Chi2 axis
 - FUDGEFACTOR_RUN - buffer for the RUN axis

 - RUNNUMLIST - list of the run numbers - now sorted

 Exit Conditions: none
 Called By: TreeMatch_vs_Run
 Date: 10-15-2014
 Modified: 11-12-2014
 *********************************************************/
void Plot()
{
  /*
   //debugging
   std::cout << "made it in the make plotting function" << std::endl;
   */

//define Canvases
  std::vector<TCanvas*> c_slope_tree;  //[pkg]
  std::vector<TCanvas*> c_chi2_tree;  //[pkg]
  std::vector<TCanvas*> c_slope_track;  //[pkg]
  std::vector<TCanvas*> c_chi2_track;  //[pkg]
  c_slope_tree.resize(NUMPACKAGE);
  c_chi2_tree.resize(NUMPACKAGE);
  c_slope_track.resize(NUMPACKAGE);
  c_chi2_track.resize(NUMPACKAGE);

//get the run range
  Double_t RunRange = Double_t(RUNNUMLIST.back() - RUNNUMLIST.front());

  for (int i_pkg = 1; i_pkg < NUMPACKAGE; i_pkg++)
  {
    c_slope_tree[i_pkg] = new TCanvas(Form("c_slope_tree[%d]", i_pkg),
        Form("Slope of treelines vs run number for %s package",
            INDEXTOPKG[i_pkg].c_str()));

    c_chi2_tree[i_pkg] = new TCanvas(Form("c_chi2_tree[%d]", i_pkg),
        Form("#chi^{2} of treelines vs run number for %s package",
            INDEXTOPKG[i_pkg].c_str()));

    c_slope_track[i_pkg] =
        new TCanvas(Form("c_slope_track[%d]", i_pkg),
            Form(
                "Slope of treelines in the partial tracks and tracks vs run number for %s package",
                INDEXTOPKG[i_pkg].c_str()));

    c_chi2_track[i_pkg] =
        new TCanvas(Form("c_chi2_track[%d]", i_pkg),
            Form(
                "#chi^{2} of treelines in the partial tracks and tracks vs run number for %s package",
                INDEXTOPKG[i_pkg].c_str()));

    //set title and axis lables (happens on the mulitgraphs)
    mg_TREEPLANE_SLOPE_VS_RUN[i_pkg]->SetTitle(
        Form(
            "Slope of treelines vs run number for %s package; Run number; slope (unitless)",
            INDEXTOPKG[i_pkg].c_str()));

    mg_TREEPLANE_CHI2_VS_RUN[i_pkg]->SetTitle(
        Form(
            "#chi^{2} of treelines vs run number for %s package; Run number; #chi^{2} (unitless)",
            INDEXTOPKG[i_pkg].c_str()));

    mg_COMPLANE_SLOPE_VS_RUN[i_pkg]->SetTitle(
        Form(
            "Slope of treelines in the partial tracks and tracks vs run number for %s package;"
                "Run number; slope (unitless)", INDEXTOPKG[i_pkg].c_str()));

    mg_COMPLANE_CHI2_VS_RUN[i_pkg]->SetTitle(
        Form(
            "#chi^{2} of treelines in the partial tracks and tracks vs run number for %s package;"
                "Run number; #chi^{2} (unitless)", INDEXTOPKG[i_pkg].c_str()));

    //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    /**************
     * Tree slope
     *************/
    //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    c_slope_tree[i_pkg]->cd();
    mg_TREEPLANE_SLOPE_VS_RUN[i_pkg]->Draw("AP");

    /*
     //debugging
     std::cout << "\n Treeplane Slope \n Pkg: " << i_pkg << std::endl << " Min: "
     << TREEPLANE_SLOPE_MIN[i_pkg] << std::endl << " Max: "
     << TREEPLANE_SLOPE_MAX[i_pkg] << std::endl;
     std::cout << "Run Num Min: " << RUNNUMLIST.front() << std::endl
     << "Run Num Max: " << RUNNUMLIST.back() << std::endl;
     */

    mg_TREEPLANE_SLOPE_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
        TREEPLANE_SLOPE_MIN[i_pkg] - FUDGEFACTOR_TREEPLANE_SLOPE,
        TREEPLANE_SLOPE_MAX[i_pkg] + FUDGEFACTOR_TREEPLANE_SLOPE);

    mg_TREEPLANE_SLOPE_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
        Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
        Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

    //draw the legend
    LEGEND_TREE[i_pkg]->SetTextSize(0.025);
    LEGEND_TREE[i_pkg]->Draw();

    //move the axis label so can read everything
    mg_TREEPLANE_SLOPE_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

    //update the pad
    gPad->Update();

    c_slope_tree[i_pkg]->SaveAs(
        Form(
            "~/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Plots/Pass_%s_Treeline_Slope_vs_run_%s_pkg.png",
            PASS.c_str(), PASS.c_str(), INDEXTOPKG[i_pkg].c_str()));

    /**************
     * Tree Chi2
     *************/
    //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    c_chi2_tree[i_pkg]->cd();
    mg_TREEPLANE_CHI2_VS_RUN[i_pkg]->Draw("AP");

    /*
     //debugging
     std::cout << "Treeplane Chi2 \n Pkg: " << i_pkg << std::endl << " Min: "
     << TREEPLANE_CHI2_MIN[i_pkg] << std::endl << " Max: "
     << TREEPLANE_CHI2_MAX[i_pkg] << std::endl;
     std::cout << "Run Num Min: " << RUNNUMLIST.front() << std::endl
     << "Run Num Max: " << RUNNUMLIST.back() << std::endl;
     */

    mg_TREEPLANE_CHI2_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
        TREEPLANE_CHI2_MIN[i_pkg] - FUDGEFACTOR_TREEPLANE_CHI2,
        TREEPLANE_CHI2_MAX[i_pkg] + FUDGEFACTOR_TREEPLANE_CHI2);

    mg_TREEPLANE_CHI2_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
        Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
        Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

    //draw the legend
    LEGEND_TREE[i_pkg]->Draw();
    //move the axis label so can read everything
    mg_TREEPLANE_CHI2_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

    //update the pad
    gPad->Update();

    c_chi2_tree[i_pkg]->SaveAs(
        Form(
            "~/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Plots/Pass_%s_Treeline_Chi2_vs_run_%s_pkg.png",
            PASS.c_str(), PASS.c_str(), INDEXTOPKG[i_pkg].c_str()));

    /**************
     * coplane Slope
     *************/
    //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    c_slope_track[i_pkg]->cd();
    mg_COMPLANE_SLOPE_VS_RUN[i_pkg]->Draw("AP");

    /*
     //debugging
     std::cout << "coplane Slope \n Pkg: " << i_pkg << std::endl << " Min: "
     << COMPLANE_SLOPE_MIN[i_pkg] << std::endl << " Max: "
     << COMPLANE_SLOPE_MAX[i_pkg] << std::endl;
     std::cout << "Run Num Min: " << RUNNUMLIST.front() << std::endl
     << "Run Num Max: " << RUNNUMLIST.back() << std::endl;
     */

    mg_COMPLANE_SLOPE_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
        COMPLANE_SLOPE_MIN[i_pkg] - FUDGEFACTOR_COMPLANE_SLOPE,
        COMPLANE_SLOPE_MAX[i_pkg] + FUDGEFACTOR_COMPLANE_SLOPE);

    mg_COMPLANE_SLOPE_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
        Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
        Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

    //draw the legend
    LEGEND_TRACKS[i_pkg]->Draw();

    //move the axis label so can read everything
    mg_COMPLANE_SLOPE_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

    //update the pad
    gPad->Update();

    c_slope_track[i_pkg]->SaveAs(
        Form(
            "~/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Plots/Pass_%s_Coplane_Slope_vs_run_%s_pkg.png",
            PASS.c_str(), PASS.c_str(), INDEXTOPKG[i_pkg].c_str()));

    /**************
     * coplane chi2
     *************/
    //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
    c_chi2_track[i_pkg]->cd();
    mg_COMPLANE_CHI2_VS_RUN[i_pkg]->Draw("AP");

    /*
     //debugging
     std::cout << "Coplane Chi2 \n Pkg: " << i_pkg << std::endl << " Min: "
     << COMPLANE_CHI2_MIN[i_pkg] << std::endl << " Max: "
     << COMPLANE_CHI2_MAX[i_pkg] << std::endl;
     std::cout << "Run Num Min: " << RUNNUMLIST.front() << std::endl
     << "Run Num Max: " << RUNNUMLIST.back() << std::endl;
     */

    mg_COMPLANE_CHI2_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
        COMPLANE_CHI2_MIN[i_pkg] - FUDGEFACTOR_COMPLANE_CHI2,
        COMPLANE_CHI2_MAX[i_pkg] + FUDGEFACTOR_COMPLANE_CHI2);

    mg_COMPLANE_CHI2_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
        Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
        Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

    //draw the legend
    LEGEND_TRACKS[i_pkg]->Draw();

    //move the axis label so can read everything
    mg_COMPLANE_CHI2_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

    //update the pad
    gPad->Update();

    c_chi2_track[i_pkg]->SaveAs(
        Form(
            "~/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Plots/Pass_%s_Coplane_Chi2_vs_run_%s_pkg.png",
            PASS.c_str(), PASS.c_str(), INDEXTOPKG[i_pkg].c_str()));

  }

  return;
}
