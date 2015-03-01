/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Theta offset in R3 values


\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef R3THETAOFFSET_HH_
#define R3THETAOFFSET_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Angle Structure
struct MyThetaOffset_t
{
  Int_t R3package;  //package number
  Double_t Val;      //mean value of the theta offset - rad
  Double_t Error;      //Error on the theat offset
};

//Function to read in the Angle
//information for a given run number

std::vector<MyThetaOffset_t> GetThetaOffset(Int_t run);

#endif /* R3THETAOFFSET_HH_ */
