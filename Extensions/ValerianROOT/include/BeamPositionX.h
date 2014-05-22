/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Beam Position X values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef BEAMPOSITIONX_HH_
#define BEAMPOSITIONX_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Beam position X Structure
struct MyBeamPositionX_t
{
  Int_t package;  //package number
  Double_t chi2;  //Chi squared value for the fit
  Double_t slope;  //slope value
  Double_t error;  //error in the slope
};

//Function to read in the beam position X
//information for a given run number

std::vector<MyBeamPositionX_t> GetBeamPositionX(Int_t run, Int_t pass, TString path);

#endif /* BEAMPOSITIONX_HH_ */
