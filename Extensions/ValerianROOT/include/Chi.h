/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Chi values


\date <b>Date:</b> 05-08-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef CHI_HH_
#define CHI_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

// Chi Structure
struct MyChi_t  //hot please enjoy!
{
  Int_t R2package;  //packag number
  Int_t direction;  //plane direction in HDC
  Double_t Val;  // mean of the Chi2 value
  Double_t RMS;  //RMS of the distribution
};

//Function to read in the Chi
//information for a given run number

std::vector<MyChi_t> GetChi(Int_t run);

#endif /* CHI_HH_ */
