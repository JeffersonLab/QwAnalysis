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
  Int_t package;  //package number
  Double_t position;  //position on bar local coordinates, long axis
  Int_t tracks; //number of tracks
  Double_t q2Val;  //q2 value with no waighting
  Double_t q2error;  //error on the q2 with no weighting
  Double_t lightVal;  //light yeild value
  Double_t lighterror;  //error on the light yeild
  Double_t  angleVal;  //scattering angle value
  Double_t  angleerror;  //error on the scattering angle
};

//Function to read in the theta match values
//information for a given run number

std::vector<MyPosOnBar_t> GetPosOnBar(Int_t run, Int_t pass, TString path);

#endif /* POSONBAR_HH_ */
