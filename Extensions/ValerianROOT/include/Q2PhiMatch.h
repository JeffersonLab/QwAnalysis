/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Phi match values as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef Q2PHIMATCH_HH_
#define Q2PHIMATCH_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//Theta Match Structure
struct MyQ2PhiMatch_t
{
  Int_t R3package;  //package number
  Double_t Val;  //the phi mach value (rad)
  Double_t Error; //the Error on the phi match - rad
};

//Function to read in the theta match values
//information for a given run number

std::vector<MyQ2PhiMatch_t> GetQ2PhiMatch(Int_t run);

#endif /* Q2PHIMATCH_HH_ */
