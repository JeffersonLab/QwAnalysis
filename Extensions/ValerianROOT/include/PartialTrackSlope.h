/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Parital Tracks slope values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef PARTIALTRACKSLOPE_HH_
#define PARTIALTRACKSLOPE_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Partial Track Slope Structure
struct MyPartialTrackSlope_t
{
  Int_t R3package;  //package number
  TString VDC;  //The VDC
  TString plane;  //the VDC plane
  Int_t nevents;  //number of events
  Double_t Val;      //slope value unitless
  Double_t Error;      //Error
};

//Function to read in the slope values for
//the Partial track  match up info in R3
//information for a given run number

std::vector<MyPartialTrackSlope_t> GetPartialTrackSlope(Int_t run);

#endif /* PARTIALTRACKSLOPE_HH_ */
