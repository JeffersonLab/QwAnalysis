/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Number of Good Tracks as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef GOODTRACK_HH_
#define GOODTRACK_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyGoodTrack_t
{
  Int_t R3package;  //Package number
  Int_t number;  //the number of good tracks
};

//Function to read in the number
//information for a given run number

std::vector<MyGoodTrack_t> GetGoodTrack(Int_t run);

#endif /* GOODTRACK_HH_ */
