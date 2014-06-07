/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Number of Raw Tracks as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef RAWTRACK_HH_
#define RAWTRACK_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//raw track Structure
struct MyRawTrack_t
{
  Int_t R3package;  //package number
  Int_t number;  //the number of raw tracks
};

//Function to read in the octatnnumber
//information for a given run number

std::vector<MyRawTrack_t> GetRawTrack(Int_t run);

#endif /* RAWTRACK_HH_ */
