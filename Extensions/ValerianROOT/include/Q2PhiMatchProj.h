/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Phi projection match values as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef Q2PHIMATCHPROJ_HH_
#define Q2PHIMATCHPROJ_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//Theta Match Structure
struct MyQ2PhiMatchProj_t
{
  Int_t R3package;  //package number
  Double_t Val;  //the theta mach value (rad)
  Double_t Error; //the Error on the theta match - rad
};

//Function to read in the phi projected match values
//information for a given run number

std::vector<MyQ2PhiMatchProj_t> GetQ2PhiMatchProj(Int_t run);

#endif /* Q2PHIMATCHPROJ_HH_ */
