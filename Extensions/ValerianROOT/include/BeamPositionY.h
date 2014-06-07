/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Beam Position Y values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef BEAMPOSITIONY_HH_
#define BEAMPOSITIONY_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Beam posiiton y Structure
struct MyBeamPositionY_t
{
  Int_t R3package;  //package number
  Double_t chi2;  //Chi squared value for the fit
  Double_t slope;      //slope value in m(GeV)^2/mm
  Double_t Error;      //error in the slope in m(GeV)^2/mm
};

//Function to read in the Beam Position Y
//information for a given run number

std::vector<MyBeamPositionY_t> GetBeamPositionY(Int_t run);

#endif /* BEAMPOSITIONY_HH_ */
