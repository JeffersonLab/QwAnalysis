/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This script has the sole purpose to generate a list of all the QTOR
 Values in the ValerianROOT Tree

 Entry Conditions:
 - pass: the pass number
 Date: 03-24-2015
 Modified:
 Assisted By:
 *********************************************************/
//ROOT includes
#include <TChain.h>
#include <TSystem.h>
#include <TLeaf.h>
#include <TBranch.h>

//standard includes
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

//ValerianROOT includes
#include "include/QTOR.h"

//functions
void Loop_Through_Tree_QTOR(TChain * track_that);
void PrintToFile();

//the pass number and value of interest
std::string PASS;

//List of runnumbers
std::vector<Double_t> QTORCURRENT;

void QTOR_Values(std::string pass)
{
  PASS = pass;

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");
  Track_It->Add(
      Form(
          "/group/qweak/www/html/tracking/ValerianROOTFiles/Pass%s_TrackingRuns.root",
          PASS.c_str()));

  Loop_Through_Tree_QTOR(Track_It);
  PrintToFile();

  QTORCURRENT.clear();

  return;
}

/***********************************************************
 Function: Loop_Through_Tree_QTOR
 Purpose:
 To loop over the tree and pick out all the QTOR current

 Entry Conditions:
 - TChain - track_that

 Global:
 - QTORCURRENT - list of the QTOR values numbers

 Exit Conditions: TChain* track_that
 Called By: QTOR_Values
 Date: 03-24-2015
 Modified:
 *********************************************************/
void Loop_Through_Tree_QTOR(TChain * track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyQtor_t>* myQtor = 0;

  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
//  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("Qtor", 1);
//  track_that->SetBranchStatus("Run", 1);

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
  track_that->SetBranchAddress("Qtor", &myQtor);
//  track_that->SetBranchAddress("Run", &run);

  //get the number of entries in the tree
  int nentries = track_that->GetEntries();
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

     //debugging

    /*****************
     * With the structures in ValerianROOT we have to now loop through
     * the entries in the vector (*myQtor)
     *
     * if the (*myQtor)[i_entry].current equal to the value of interest?
     * If so add that runlist to the RUNNUMLIST list
     ******************/
    for (UInt_t i = 0; i < myQtor->size(); i++)
    {
       //debugging
      if (!(std::find(QTORCURRENT.begin(), QTORCURRENT.end(), (*myQtor)[i].current) != QTORCURRENT.end()))
      {
        QTORCURRENT.push_back((*myQtor)[i].current);
      }
    }
  }
}

/***********************************************************
 Function: PrintToFile
 Purpose:
 To sort and print out the QTOR values from the
 ValerianROOT used in this script.

 Entry Conditions:

 Global:
 - QTORCURRENT - list of the QTOR values numbers

 Exit Conditions: none
 Called By: QTOR_Values
 Date: 03-23-2015
 Modified:
 *********************************************************/
void PrintToFile()
{
  //first sort the vector so that the entries go form low to high
   std::sort(QTORCURRENT.begin(), QTORCURRENT.end());

  //print to file
  //this file and everything related to it is fout
  std::ofstream fout;

  //open a file
//This won't work! why I have no idea
  fout.open(
      Form(
          "/home/vmgray/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/inital_runlist/Pass%s_QTOR_CurrentValues.txt",
          PASS.c_str()));

  if (!fout.is_open())
    cout << "File not opened" << endl;

  for (int i_current = 0; i_current < QTORCURRENT.size(); i_current++)
  {
    fout << QTORCURRENT[i_current] << std::endl;
  }

  fout.close();

  return;
}
