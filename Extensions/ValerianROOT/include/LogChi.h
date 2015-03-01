/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Log Chi values


\date <b>Date:</b> 05-08-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef LOGCHI_HH_
#define LOGCHI_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

// Chi Structure
struct MyLogChi_t
{
  Int_t R2package;  //packag number
  Int_t direction;  //plane direction in HDC
  Double_t Val;  // mean log Chi value
  Double_t RMS;  //RMS of the distribution
};

//Function to read in the Chi
//information for a given run number

std::vector<MyLogChi_t> GetLogChi(Int_t run);

#endif /* LOGCHI_HH_ */
