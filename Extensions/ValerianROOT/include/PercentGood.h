/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the percent of Good Tracks as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-26-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef PERCENTGOOD_HH_
#define PERCENTGOOD_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyPercentGood_t
{
  Int_t R3package;  //Package number
  Double_t percent;  //the percent of good tracks
};

//Function to read in the number
//information for a given run number

std::vector<MyPercentGood_t> GetPercentGood(Int_t run);

#endif /* PERCENTGOOD_HH_ */
