/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the position on bar vs everything

  Everything is a element to the run

\date <b>Date:</b> 05-15-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef POSONBAR_HH_
#define POSONBAR_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//Position on bar Structure
struct MyPosOnBar_t
{
  Int_t R3package;  //package number
  Double_t position;  //position on bar local coordinates, long axis
  Int_t tracks; //number of tracks
  Double_t q2Val;  //q2 value with no waighting - m(Gev)^2
  Double_t q2Error;  //Error on the q2 with no weighting - m(GeV)^2
  Double_t lightVal;  //light yeild value
  Double_t lightError;  //Error on the light yeild
  Double_t  angleVal;  //scattering angle value - deg
  Double_t  angleError;  //Error on the scattering angle - deg
};

//Function to read in the theta match values
//information for a given run number

std::vector<MyPosOnBar_t> GetPosOnBar(Int_t run);

#endif /* POSONBAR_HH_ */
