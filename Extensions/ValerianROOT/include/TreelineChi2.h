/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Treeline Chi2 values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef TREELINECHI2_HH_
#define TREELINECHI2_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Treeline Chi2 Structure
struct MyTreelineChi2_t
{
  Int_t R3package;  //package number
  TString VDC;  //The VDC
  TString plane;  //the VDC plane
  Int_t nevents;  //number of events
  Double_t Val;      //Chi value
  Double_t RMS;      //Error
};

//Function to read in the chi2 values for
//the treeline match up info in R3
//information for a given run number

std::vector<MyTreelineChi2_t> GetTreelineChi2(Int_t run);

#endif /* TREELINECHI2_HH_ */
