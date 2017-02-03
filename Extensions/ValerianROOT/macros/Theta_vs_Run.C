/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This script graphs the theta value vs the run number.

 Entry Conditions:
 - pass: the pass number
 - filename: the runlist file name of interest
 Date: 05-02-2015
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
#include <numeric> //needed for finding average of a vector

//ValerianROOT macro includes
//include for the functions used in all macros without being customized
#include "Read_In_Run_Numbers.h"
#include "less_than_but_non_zero.h"

//ValerianROOT includes
#include "include/Angle.h"

//function definitions for those not in the header files
void Loop_Through_Tree(TChain * track_that);
void Make_graphs();
void Plot();
void PrintToFile();

//define TGraphErrors
std::vector<TGraphErrors*> g_THETA_VS_RUN;  //[package]

//define the Fit for the TGraph
std::vector<TF1*> f_THETA_VS_RUN;  //[package]

//define the muiltigraph
std::vector<TMultiGraph*> mg_THETA_VS_RUN;  //[package]

//define vectors to store information for GraphError
std::vector<std::vector<Double_t> > THETA_VAL;  //[package][run index num]
std::vector<std::vector<Double_t> > THETA_ERROR;  //[package][run index num]
std::vector<Double_t> RUN_VAL;  //[run] - need so be double for TGraphError
std::vector<Double_t> RUN_ERROR;  //[run] always zero - needs to be double for TGraphError
//definition of integers
Int_t nentries;  // number of entries in the tree

const Int_t NUMPACKAGES = 3;

Double_t FUDGEFACTOR_THETA = 0.01;  //degrees the factor added on to the y axis for pretty plots
Double_t FUDGEFACTOR_RUN[2] = { 0.05, 0.05 };  // the percentage add on to the X axis for pretty plots

// index of vector to what it is
std::string INDEXTOPKG[NUMPACKAGES] = { "Both", "1", "2" };

//array for min and may q2 values
Double_t THETA_MIN[NUMPACKAGES] = { 0 };  //min q2 for a pkg
Double_t THETA_MAX[NUMPACKAGES] = { 0 };  //max q2 for a pkg

//the pass number
std::string PASS;

//pass the runlist
std::string FILENAME_PREFIX;

//Fit int, are we ploting the fit or not
Int_t i_FIT = 0;  //Fit 0 is no and 1 is yes

void Theta_vs_Run(std::string pass, std::string filename)
{
  PASS = pass;
  FILENAME_PREFIX = filename.substr(0, filename.find("."));

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");
  Track_It->Add(
      Form(
          "/group/qweak/www/html/tracking/ValerianROOTFiles/Pass%s_TrackingRuns.root",
          PASS.c_str()));

/*
  Track_It->Add(
      Form(
          "/group/qweak/www/html/tracking/ValerianROOTFiles/Sim_Pass%s_TrackingRuns.root",
          PASS.c_str()));
*/

  //run the functions on interest
  Read_In_Run_Numbers(filename);
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

 - THETA_VAL - vector of the theta values
 - THETA_ERROR - vector of the theta errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 - RUNNUMLIST - the vector containing the list of runs of interest (ROI)

 - nentries - the number of entries in the TrackThat tree

 Exit Conditions: none
 Called By: Theta_vs_Run
 Date: 05-02-2015
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
  std::vector<MyAngle_t> *myAngle = 0;
  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("Angle", 1);
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
  //creates a memory address, which points to the memory address of myAngle (the pointer)
  //since run is not a pointer this one just points to the run
  track_that->SetBranchAddress("Angle", &myAngle);
  track_that->SetBranchAddress("Run", &run);

  /********************
   * resize the THETA_VAL and THETA_ERROR vectors so that
   * I can fill them without my brains going boom,
   * cause all that push_back with vectors of vectors in confusing!
   * also set all values to zero this is for good house keeping
   * and, so that there is something in it
   *
   * Also do the RUN_VAL and RUN_ERROR - although these are easy
   *********************/
  RUN_VAL.resize(RUNNUMLIST.size(), 0);
  RUN_ERROR.resize(RUNNUMLIST.size(), 0);
  THETA_VAL.resize(NUMPACKAGES);
  THETA_ERROR.resize(NUMPACKAGES);

  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    //resize theta vector and fill with zero
    //resize theta vectors
    THETA_VAL[i_pkg].resize(RUNNUMLIST.size(), 0);
    THETA_ERROR[i_pkg].resize(RUNNUMLIST.size(), 0);
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

      for (UInt_t i = 0; i < myAngle->size(); i++)
      {
        /*
         //debugging
         std::cout << "In the Angle arena - we have data :-)" << std::cout;
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
        switch ((*myAngle)[i].R3package)
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

            //the Angle values and error in Degrees
            THETA_VAL[(*myAngle)[i].R3package][run_index] = (*myAngle)[i].Val;
            THETA_ERROR[(*myAngle)[i].R3package][run_index] =
                (*myAngle)[i].Error;

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
 - g_THETA_VS_RUN - the vector of TGraphErrors

 - mg_THETA_VS_RUN - the vector of mutigraph

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in

 - THETA_VAL - vector of the theta values
 - THETA_ERROR - vector of the theta errors
 - RUN_VAL - vector of the run values
 - RUN_ERROR - vector of the errors on the runs
 these of course are 0 since there are none.

 Exit Conditions: none
 Called By: Theta_vs_Run
 Date: 05-02-2015
 Modified:
 *********************************************************/
void Make_graphs()
{
  /*  //debugging
   std::cout << "made it in the make graph function" << std::endl;*/

  //resize the vector of the tgraph to the number of q2 types
  g_THETA_VS_RUN.resize(NUMPACKAGES);
  //resize the muiltigraph to number of packages
  mg_THETA_VS_RUN.resize(NUMPACKAGES);
  //resize the fit to the number of packages
  f_THETA_VS_RUN.resize(NUMPACKAGES);

//Make the TGraphs with the information we got in from looping through the tree
  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    /*    //debugging....
     std::cout << "Pkg: " << i_pkg << std::endl;*/

    /* I don't think we need a ledgend
     //define legend
     LEGEND[i_pkg] =
     (i_pkg == 0) ?
     new TLegend(0.89, 0.45, 0.99, 0.55) :
     new TLegend(0.79, 0.35, 0.99, 0.65);
     LEGEND[i_pkg]->SetHeader("Q^{2} Type");
     */

    //initalize the max and min values - Max very small, Min large so they must be replaced
    THETA_MAX[i_pkg] = -1e6;
    THETA_MIN[i_pkg] = 1e6;

    //resize the type tgraph vector
    g_THETA_VS_RUN.resize(NUMPACKAGES);
    //take the vectors and turn them into arrays so that TGraphErrors will play nice
    //with them by &(vector[0]) :)
    g_THETA_VS_RUN[i_pkg] = new TGraphErrors(RUNNUMLIST.size(), &(RUN_VAL[0]),
                                             &(THETA_VAL[i_pkg][0]),
                                             &(RUN_ERROR[0]),
                                             &(THETA_ERROR[i_pkg][0]));

    /*
     * Add in the attributes of the graph, blue, filled circle point down
     */
    //set it so the errors print
    gStyle->SetOptFit(1110);
    //set markercolor && error bar color - Blue
    g_THETA_VS_RUN[i_pkg]->SetMarkerColor(kBlue);
    g_THETA_VS_RUN[i_pkg]->SetLineColor(kBlue);

    //set Marker size
    g_THETA_VS_RUN[i_pkg]->SetMarkerSize(1);

    //set marker style - filled circle
    g_THETA_VS_RUN[i_pkg]->SetMarkerStyle(8);

    if (i_FIT == 0)
    // Don't plot fit
    {
      //fit this graph to a constant, and get the stats - EX0 intally
      //g_Q2_VS_RUN[i_type][i_pkg]->Fit(
      //    Form("f_THETA_VS_RUN[%d]", i_pkg), "N");
      //f_THETA_VS_RUN[i_pkg]->SetLineColor(kBlue);
      //g_Q2_VS_RUN[i_type][i_pkg]->Fit("pol0", "N");
      //g_Q2_VS_RUN[i_type][i_pkg]->GetFunction("pol0")->SetLineColor(kBlue);
    } else if (i_FIT == 1)
    {
      //define the fits and color it
      f_THETA_VS_RUN[i_pkg] = new TF1(Form("f_THETA_VS_RUN[%d]", i_pkg),
                                      "pol0");
      f_THETA_VS_RUN[i_pkg]->SetLineColor(kBlue);
      //fit this graph to a constant, and get the stats - EX0 intally
      g_THETA_VS_RUN[i_pkg]->Fit(Form("f_THETA_VS_RUN[%d]", i_pkg));
      //f_THETA_VS_RUN[i_pkg]->SetLineColor(kBlue);
      //g_Q2_VS_RUN[i_type][i_pkg]->Fit("pol0", "N");
      //g_Q2_VS_RUN[i_type][i_pkg]->GetFunction("pol0")->SetLineColor(kBlue);
    }

    //work with the mulitigraph
    //define the multigraph
    mg_THETA_VS_RUN[i_pkg] = new TMultiGraph(
        Form("mg_THETA_VS_RUN[%d]", i_pkg),
        Form("Scattering Angle vs Run number for package %s",
             INDEXTOPKG[i_pkg].c_str()));
    //add this graph to the muitigraph
    mg_THETA_VS_RUN[i_pkg]->Add(g_THETA_VS_RUN[i_pkg]);

    //get the min and max Theta values
    THETA_MAX[i_pkg] = std::max(
        THETA_MAX[i_pkg],
        *std::max_element(THETA_VAL[i_pkg].begin(), THETA_VAL[i_pkg].end()));

    THETA_MIN[i_pkg] = std::min(
        THETA_MIN[i_pkg],
        *std::min_element(THETA_VAL[i_pkg].begin(), THETA_VAL[i_pkg].end(),
                          less_than_but_non_zero));

    /*
     //debugging
     std::cout << "Pkg: " << i_pkg << std::endl;
     std::cout << "THETA_MIN: " << THETA_MIN[i_pkg] << std::endl;
     std::cout << "THETA_MAX: " << THETA_MAX[i_pkg] << std::endl;
     */
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
 - mg_THETA_VS_RUN - vector of the muiltigraph

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in

 - RUNNUMLIST - list of the run numbers - now sorted

 - THETA_MAX - max q2 value for a pkg
 - THETA_MIN - min q2 value for a pkg

 - FUDGEFACTOR_RUN - buffer for the RUN axis
 - FUDGEFACTOR_THETA - buffer for the Theta axis

 Exit Conditions: none
 Called By: Theta_vs_Run
 Date: 05-02-2015
 Modified:
 *********************************************************/
void Plot()
{
  if (i_FIT == 0)
  {
    //Canvas
    std::vector<TCanvas*> c_theta;  //[pkg]
    c_theta.resize(NUMPACKAGES);

    for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
    {
      c_theta[i_pkg] = new TCanvas(
          Form("c_theta[%d]", i_pkg),
          Form("Scattering Angle vs run number for %s package",
               INDEXTOPKG[i_pkg].c_str()));

      //set title and axis lables
      mg_THETA_VS_RUN[i_pkg]->SetTitle(
          Form(
              "Scattering Angle vs Run number for %s package; Run number; #theta [#circ]",
              INDEXTOPKG[i_pkg].c_str()));

      //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
      mg_THETA_VS_RUN[i_pkg]->Draw("AP");

      /*    //debugging
       std::cout << "Pkg: " << i_pkg << std::endl << " Min: " << THETA_MIN[i_pkg]
       << std::endl << " Max: " << THETA_MAX[i_pkg] << std::endl;
       std::cout << "Run Num Min: " << RUNNUMLIST.front()
       << std::endl << "Run Num Max: " << RUNNUMLIST.back() << std::endl; */

      mg_THETA_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
          THETA_MIN[i_pkg] - FUDGEFACTOR_THETA,
          THETA_MAX[i_pkg] + FUDGEFACTOR_THETA);

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
      mg_THETA_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
          Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
          Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

      //move the axis label so can read everything
      mg_THETA_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

      //update the pad
      gPad->Update();

      c_theta[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Theta_vs_Run/Pass_%s_theta_vs_run_pkg_%s_%s.png",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));

      c_theta[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Theta_vs_Run/Pass_%s_theta_vs_run_pkg_%s_%s.C",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));
    }
  } else if (i_FIT == 1)
  {
    //Canvas
    std::vector<TCanvas*> c_theta_fit;  //[pkg]
    c_theta_fit.resize(NUMPACKAGES);

    for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
    {
      c_theta_fit[i_pkg] = new TCanvas(
          Form("c_theta_fit[%d]", i_pkg),
          Form("Scattering Angle vs run number for %s package",
               INDEXTOPKG[i_pkg].c_str()));

      //set title and axis lables
      mg_THETA_VS_RUN[i_pkg]->SetTitle(
          Form(
              "Scattering Angle vs Run number for %s package; Run number; #theta [#circ]",
              INDEXTOPKG[i_pkg].c_str()));

      //Draw this wonderful data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
      mg_THETA_VS_RUN[i_pkg]->Draw("AP");

      /*    //debugging
       std::cout << "Pkg: " << i_pkg << std::endl << " Min: " << THETA_MIN[i_pkg]
       << std::endl << " Max: " << THETA_MAX[i_pkg] << std::endl;
       std::cout << "Run Num Min: " << RUNNUMLIST.front()
       << std::endl << "Run Num Max: " << RUNNUMLIST.back() << std::endl; */

      mg_THETA_VS_RUN[i_pkg]->GetYaxis()->SetRangeUser(
          THETA_MIN[i_pkg] - FUDGEFACTOR_THETA,
          THETA_MAX[i_pkg] + FUDGEFACTOR_THETA);

      Double_t RunRange = Double_t(RUNNUMLIST.back() - RUNNUMLIST.front());

      //   original
      mg_THETA_VS_RUN[i_pkg]->GetXaxis()->SetLimits(
          Double_t(RUNNUMLIST.front()) - RunRange * FUDGEFACTOR_RUN[0],
          Double_t(RUNNUMLIST.back()) + RunRange * FUDGEFACTOR_RUN[1]);

      //move the axis label so can read everything
      mg_THETA_VS_RUN[i_pkg]->GetYaxis()->SetTitleOffset(1.25);

      //update the pad
      gPad->Update();

      //Add the fit on top
      //https://root.cern.ch/root/html532/TGraph.html#TGraph:Fit
      //gStyle->SetOptFit(0111); //Probality, Chi2/nDF, paramters, errors
      f_THETA_VS_RUN[i_pkg]->Draw("same");  //("pol0")->Draw("same");
      gPad->Update();

      //do something with the stats so that they are colored like the lines
      //I have no idea what this does I found it online...
      TPaveStats* stats1 =
          (TPaveStats*) g_THETA_VS_RUN[i_pkg]->GetListOfFunctions()->FindObject(
              "stats");
      stats1->SetTextColor(kBlue);

      stats1->SetX1NDC(0.81);  //left
      stats1->SetX2NDC(0.99);  //right
      stats1->SetY1NDC(0.78);  //bottom
      stats1->SetY2NDC(0.90);  //top

      c_theta_fit[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Theta_vs_Run/Pass_%s_theta_vs_run_withFit_pkg_%s_%s.png",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));

      c_theta_fit[i_pkg]->SaveAs(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Theta_vs_Run/Pass_%s_theta_vs_run_withFit_pkg_%s_%s.C",
              PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
              INDEXTOPKG[i_pkg].c_str(), FILENAME_PREFIX.c_str()));
    }
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

 - NUMPACKAGES - the number of packages

 - INDEXTOPKG - array to get what package that we are in

 - THETA_VAL - vector of the theta values
 - THETA_ERROR - vector of the theta errors
 - RUN_VAL - vector of the run values
 these of course are 0 since there are none.

 - RUNNUMLIST - list of the run numbers - now sorted

 Exit Conditions: none
 Called By: Theta_vs_Run
 Date: 05-02-2015
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
          "/home/vmgray/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass%s_Analysis/%s/Theta_vs_Run/Pass%s_Theta_vs_Run_%s.txt",
          PASS.c_str(), FILENAME_PREFIX.c_str(), PASS.c_str(),
          FILENAME_PREFIX.c_str()));

  if (!fout.is_open())
    cout << "File not opened" << endl;

  fout << "Run \t pkg \t\t  Val & Error " << std::endl;

  for (int i_run = 0; i_run < RUNNUMLIST.size(); i_run++)
  {
    for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
    {
      fout << RUN_VAL[i_run] << " \t " << INDEXTOPKG[i_pkg].c_str() << " \t\t "
      << THETA_VAL[i_pkg][i_run] << " +- " << THETA_ERROR[i_pkg][i_run]
      << std::endl;
    }

    fout << std::endl;

  }

  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    fout << "Average \t "
    << INDEXTOPKG[i_pkg].c_str()
    << " \t\t "
    << std::accumulate(THETA_VAL[i_pkg].begin(), THETA_VAL[i_pkg].end(), 0.0) / THETA_VAL[i_pkg].size()
    << std::endl;
  }
  fout << std::endl;

  //Print the parameters for fit 4 Q2 without loss
  for (int i_pkg = 0; i_pkg < NUMPACKAGES; i_pkg++)
  {
    fout << "fit parameters: " << INDEXTOPKG[i_pkg].c_str() << " \t\t p0: "
    << f_THETA_VS_RUN[i_pkg]->GetParameter(0) << " +/- "
    << f_THETA_VS_RUN[i_pkg]->GetParError(0) << " \t\t Chi2: "
    << f_THETA_VS_RUN[i_pkg]->GetChisquare() << " \t\t NDf: "
    << f_THETA_VS_RUN[i_pkg]->GetNDF() << std::endl;
  }
  fout.close();

  return;

}
