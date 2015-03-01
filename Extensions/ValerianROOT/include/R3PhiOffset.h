/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Phi offset in R3 values


\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef R3PHIOFFSET_HH_
#define R3PHIOFFSET_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Angle Structure
struct MyPhiOffset_t
{
  Int_t R3package;  //package number
  Double_t Val;      //mean value of the phi offset - rad
  Double_t Error;      //Error on the phi offset
};

//Function to read in the Angle
//information for a given run number

std::vector<MyPhiOffset_t> GetPhiOffset(Int_t run);

#endif /* R3PHIOFFSET_HH_ */
