/***********************************************************
Programmer: Valerie Gray
Purpose:
  This script has 2 purpuses
  1) To serve as testing for the development of ValerianROOT
  2) be an example of how to manage the ValerianROOT Tree and
    get plots out, as this ROOT file does not have a struture
    which supports simple Draw statments (sorry to those of
    you who like them)

Entry Conditions:none
Date: 05-13-2014
Modified:
Assisted By:
*********************************************************/

#include "TChain.h"
#include "TCanvas.h"
#include "TH1D.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//function definitions
void Define_Histograms();
void Loop_Through_Tree(TChain * track_that);
void Plot();
void Print_To_File(int run_num);

//definition of intgers
//I am checking if things are filled so in general one would not
//need these
Int_t nentries;
Int_t nRun;
Int_t nAngle;
Int_t nLogChi;
Int_t nLog;

//definitions of histograms
TH1D* h_Angle_mean;

//definitons of ranges of histograms iff needed

//any other things that will be needed by more than on function

/***********************************
set up the significant figures right - &plusmn is for html,
  change to +/- if not useing,
  or in this case \t for a tab space in the outputfile
Use this by calling: value_with_error(a,da)
***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
 << std::setprecision((size_t) (std::log10((a)/(da)) - std::log10(a) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)

void Example
{
  //Create TChain and give it a file
  TChain* Tracking = new TChain ("TrackThat");
  TrackThat->Add("/home/vmgray/QweakTracking/Pass5_TrackingRuns.root");

  //run the funtions on interest
  Define_Histograms();

  Loop_Through_Tree(track_that);

  Plot();

  Print_To_File(runnum);

  //Get the number of entries in the root tree
  //this is the number of runs in the pass of interest
  nentries = TrackThat->GetEntries();

  //intergers to get the number of entries in a
  //branch

  //loop through those entries (ie runs)
  for(int i = 0; i < nentries; i++)
  {
    
  }

  return;
}

/***********************************************************
Function: Define_Histograms
Purpose:
  To define all the histograms,
    ie. name, axis labels

Entry Conditions:
  none

Global:
  - h_Angle_mean - histogram of the mean of all scattering
    angles in all the runs

Exit Conditions: none
Called By: Example
Date: 05-13-2014
Modified:
*********************************************************/
void Define_Histograms()
{
  h_Angle_mean = = new TH1D ("h_Angle_mean","Mean scattering angle", 100,0.0,8);

  h_Angle_mean->GetYAxis()->SetTitle("Frequency");
  h_Angle_mean->GetXAxis()->SetTitle("Scattering Angle (deg)");

  return;
}

/***********************************************************
Function: Loop_Through_Tree
Purpose:
  To loop over the event and fill in the histograms for the
  offset

Entry Conditions:
  TChain - track_that
Global:
  - h_Angle_mean- histogram of the mean of all scattering
    angles in all the runs

  - nentries - total numeber of entries in the tree
  - nRun - total number of run in the tree
  - nAngle- total number of angle values in the tree
  - nLogChi- total number of Log Chi values in the tree
  - nChi- total number of Chi values in the tree

Exit Conditions: none
Called By: Example
Date: 05-13-2014
Modified:
*********************************************************/
void Loop_Through_Tree(TChain* track_that)
{
  //get the number of entries in the tree
  nentries= track_that->GetEntries();

  //start looping through the tree
  for(int i = 0; i < nentries; i++)
  {
    
  }

  return;
}
