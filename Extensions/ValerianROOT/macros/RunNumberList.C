/***********************************************************
 Programmer: Valerie Gray
 Purpose:
 This script has the sole purpose to generate the runlist for
 a certain value of interest. This allows for creating runlist
 with certain charateristics.

 Entry Conditions:
 - pass: the pass number
 Date: 03-23-2015
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

//ValerianROOT includes
#include "include/QTOR.h"
#include "include/R2Pkg1.h"
#include "include/R2Pkg2.h"
#include "include/R3Pkg1.h"
#include "include/R3Pkg2.h"

//functions
void Loop_Through_Tree_QTOR(TChain * track_that);
void Loop_Through_Tree_R2Pkg1(TChain * track_that);
void Loop_Through_Tree_R2Pkg2(TChain * track_that);
void Loop_Through_Tree_R3Pkg1(TChain * track_that);
void Loop_Through_Tree_R3Pkg2(TChain * track_that);
void PrintToFile();

//the pass number and value of interest
std::string PASS;
std::string TYPE;
Double_t VALUE;

//List of runnumbers
std::vector<Int_t> RUNNUMLIST;

void RunNumberList(std::string pass, std::string type, Double_t value)
{
  PASS = pass;
  TYPE = type;
  VALUE = value;

  //Create TChain and give it a file
  TChain* Track_It = new TChain("TrackThat");
  Track_It->Add(
      Form(
          "/group/qweak/www/html/tracking/ValerianROOTFiles/Pass%s_TrackingRuns.root",
          PASS.c_str()));

  if (type == "QTOR") //we are looking at Qtor Current
  {
    Loop_Through_Tree_QTOR(Track_It);
  } else if (type == "R2Pkg1") //we are looking at R2 pkg 1
  {
    Loop_Through_Tree_R2Pkg1(Track_It);
  } else if (type == "R2Pkg2") //we are looking at R2 pkg 2
  {
    Loop_Through_Tree_R2Pkg2(Track_It);
  } else if (type == "R3Pkg1") //we are looking at R3 pkg 1
  {
    Loop_Through_Tree_R3Pkg1(Track_It);
  }  else if (type == "R3Pkg2") //we are looking at R3 pkg 2
  {
    Loop_Through_Tree_R3Pkg2(Track_It);
  }

  PrintToFile();

  RUNNUMLIST.clear();

  return;
}

/***********************************************************
 Function: Loop_Through_Tree_QTOR
 Purpose:
 To loop over the tree and pick out all the runs with a QTOR
 current of a cerian value

 Entry Conditions:
 - TChain - track_that

 Global:

 Exit Conditions: TChain* track_that
 Called By: RunNumberList
 Date: 03-23-2015
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
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("Qtor", 1);
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
  track_that->SetBranchAddress("Qtor", &myQtor);
  track_that->SetBranchAddress("Run", &run);

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

    /*
     //debugging
     std::cout << "we are looking getting the run number, " << run << std::endl;
     */

    /*****************
     * With the structures in ValerianROOT we have to now loop through
     * the entries in the vector (*myQtor)
     *
     * if the (*myQtor)[i_entry].current equal to the value of interest?
     * If so add that runlist to the RUNNUMLIST list
     ******************/
    for (UInt_t i = 0; i < myQtor->size(); i++)
    {
      /*
       //debugging
       std::cout << "The QTOR current is: " << (*myQtor)[i].current << std::endl;
       */

      if ((*myQtor)[i].current == VALUE)
      {
        /*
         std::cout << "wanted current: " << VALUE << std::endl;
         std::cout << "RUN with same value" << run << std::endl;
         */
        RUNNUMLIST.push_back(run);
      }
    }
  }
}

/***********************************************************
 Function: Loop_Through_Tree_R2Pkg1
 Purpose:
 To loop over the tree and pick out all the runs with a R2Pkg1
 current of a cerian value

 Entry Conditions:
 - TChain - track_that

 Global:

 Exit Conditions: TChain* track_that
 Called By: RunNumberList
 Date: 03-23-2015
 Modified:
 *********************************************************/
void Loop_Through_Tree_R2Pkg1(TChain * track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyR2Pkg1_t>* myR2Pkg1 = 0;

  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("R2Pkg1", 1);
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
  track_that->SetBranchAddress("R2Pkg1", &myR2Pkg1);
  track_that->SetBranchAddress("Run", &run);

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

    /*
     //debugging
     std::cout << "we are looking getting the run number, " << run << std::endl;
     */

    /*****************
     * With the structures in ValerianROOT we have to now loop through
     * the entries in the vector (*myR2Pkg1)
     *
     * if the (*myR2Pkg1)[i].current equal to the value of interest?
     * If so add that runlist to the RUNNUMLIST list
     ******************/
    for (UInt_t i = 0; i < myR2Pkg1->size(); i++)
    {
      /*
       //debugging
       std::cout << "The R2Pkg1 is: " << (*myR2Pkg1)[i].octant << std::endl;
       */

      if ((*myR2Pkg1)[i].octant == VALUE)
      {
        /*
         std::cout << "wanted: " << VALUE << std::endl;
         std::cout << "RUN with same value" << run << std::endl;
         */
        RUNNUMLIST.push_back(run);
      }
    }
  }
}

/***********************************************************
 Function: Loop_Through_Tree_R2Pkg2
 Purpose:
 To loop over the tree and pick out all the runs with a R2Pkg1
 current of a cerian value

 Entry Conditions:
 - TChain - track_that

 Global:

 Exit Conditions: TChain* track_that
 Called By: RunNumberList
 Date: 03-23-2015
 Modified:
 *********************************************************/
void Loop_Through_Tree_R2Pkg2(TChain * track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyR2Pkg2_t>* myR2Pkg2 = 0;

  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("R2Pkg2", 1);
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
  track_that->SetBranchAddress("R2Pkg2", &myR2Pkg2);
  track_that->SetBranchAddress("Run", &run);

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

    /*
     //debugging
     std::cout << "we are looking getting the run number, " << run << std::endl;
     */

    /*****************
     * With the structures in ValerianROOT we have to now loop through
     * the entries in the vector (*myR2Pkg2)
     *
     * if the (*myR2Pkg2)[i].current equal to the value of interest?
     * If so add that runlist to the RUNNUMLIST list
     ******************/
    for (UInt_t i = 0; i < myR2Pkg2->size(); i++)
    {
      /*
       //debugging
       std::cout << "The R2Pkg2 is: " << (*myR2Pkg2)[i].octant << std::endl;
       */

      if ((*myR2Pkg2)[i].octant == VALUE)
      {
        /*
         std::cout << "wanted: " << VALUE << std::endl;
         std::cout << "RUN with same value" << run << std::endl;
         */
        RUNNUMLIST.push_back(run);
      }
    }
  }
}

/***********************************************************
 Function: Loop_Through_Tree_R3Pkg1
 Purpose:
 To loop over the tree and pick out all the runs with a R2Pkg1
 current of a cerian value

 Entry Conditions:
 - TChain - track_that

 Global:

 Exit Conditions: TChain* track_that
 Called By: RunNumberList
 Date: 03-23-2015
 Modified:
 *********************************************************/
void Loop_Through_Tree_R3Pkg1(TChain * track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyR3Pkg1_t>* myR3Pkg1 = 0;

  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("R3Pkg1", 1);
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
  track_that->SetBranchAddress("R3Pkg1", &myR3Pkg1);
  track_that->SetBranchAddress("Run", &run);

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

    /*
     //debugging
     std::cout << "we are looking getting the run number, " << run << std::endl;
     */

    /*****************
     * With the structures in ValerianROOT we have to now loop through
     * the entries in the vector (*myR3Pkg1)
     *
     * if the (*myR3Pkg1)[i].current equal to the value of interest?
     * If so add that runlist to the RUNNUMLIST list
     ******************/
    for (UInt_t i = 0; i < myR3Pkg1->size(); i++)
    {
      /*
       //debugging
       std::cout << "The R3Pkg1 is: " << (*myR3Pkg1)[i].octant << std::endl;
       */

      if ((*myR3Pkg1)[i].octant == VALUE)
      {
        /*
         std::cout << "wanted: " << VALUE << std::endl;
         std::cout << "RUN with same value" << run << std::endl;
         */
        RUNNUMLIST.push_back(run);
      }
    }
  }
}

/***********************************************************
 Function: Loop_Through_Tree_R3Pkg2
 Purpose:
 To loop over the tree and pick out all the runs with a R2Pkg1
 current of a cerian value

 Entry Conditions:
 - TChain - track_that

 Global:

 Exit Conditions: TChain* track_that
 Called By: RunNumberList
 Date: 03-23-2015
 Modified:
 *********************************************************/
void Loop_Through_Tree_R3Pkg2(TChain * track_that)
{
  /*******************************
   * define the pointer to the vector of the object we want from the tree
   * This allows us to use the stucts that ValerianROOT was made on
   * This is done by:
   *******************************/
  //creates memory address to store the address of a vector (we set to 0 at moment)
  std::vector<MyR3Pkg2_t>* myR3Pkg2 = 0;

  //create and interger for the run number - as this is not a
  //stucture it does not need a pointer
  Int_t run;

  /*******************************
   * Get the branches that are need for this script
   * This is done by:
   *******************************/
  // Enable branches that we will use in this macro
  track_that->SetBranchStatus("R3Pkg2", 1);
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
  track_that->SetBranchAddress("R3Pkg2", &myR3Pkg2);
  track_that->SetBranchAddress("Run", &run);

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

    /*
     //debugging
     std::cout << "we are looking getting the run number, " << run << std::endl;
     */

    /*****************
     * With the structures in ValerianROOT we have to now loop through
     * the entries in the vector (*myR3Pkg2)
     *
     * if the (*myR3Pkg2)[i].current equal to the value of interest?
     * If so add that runlist to the RUNNUMLIST list
     ******************/
    for (UInt_t i = 0; i < myR3Pkg2->size(); i++)
    {
      /*
       //debugging
       std::cout << "The R3Pkg1 is: " << (*myR3Pkg1)[i].octant << std::endl;
       */

      if ((*myR3Pkg2)[i].octant == VALUE)
      {
        /*
         std::cout << "wanted: " << VALUE << std::endl;
         std::cout << "RUN with same value" << run << std::endl;
         */
        RUNNUMLIST.push_back(run);
      }
    }
  }
}

/***********************************************************
 Function: PrintToFile
 Purpose:
 To print out the run values from the ValerianROOT used in this script.

 Entry Conditions:

 Global:
 - RUNNUMLIST - list of the run numbers - now sorted

 Exit Conditions: none
 Called By: RunNumberList
 Date: 03-23-2015
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
          "/home/vmgray/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/inital_runlist/Pass_%s/%s/Pass%s_%s_%g.txt",
          PASS.c_str(), TYPE.c_str(), PASS.c_str(), TYPE.c_str(), VALUE));

  if (!fout.is_open())
    cout << "File not opened" << endl;

  for (int i_run = 0; i_run < RUNNUMLIST.size(); i_run++)
  {
    fout << RUNNUMLIST[i_run] << std::endl;
  }

  fout.close();

  return;
}
