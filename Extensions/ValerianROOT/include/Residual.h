/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Residual values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef RESIDUAL_HH_
#define RESIDUAL_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Residual Structure
struct MyResidual_t
{
  Int_t R2package;  //packag number
  Int_t plane;  //plane in HDC
  Double_t Val;  // mean value - cm
  Double_t RMS;  //RMS of the distribution
};

//Function to read in the Residual
//information for a given run number

std::vector<MyResidual_t> GetResidual(Int_t run);

#endif /* RESIDUAL_HH_ */
