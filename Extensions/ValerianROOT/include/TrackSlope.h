/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Tracks slope values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef TRACKSLOPE_HH_
#define TRACKSLOPE_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Track Slope Structure
struct MyTrackSlope_t
{
  Int_t package;  //package number
  TString VDC;  //The VDC
  TString plane;  //the VDC plane
  Int_t nevents;  //number of events
  Double_t Val;      //slope value
  Double_t error;      //error
};

//Function to read in the slope values for
//the track  match up info in R3
//information for a given run number

std::vector<MyTrackSlope_t> GetTrackSlope(Int_t run, Int_t pass, TString path);

#endif /* TRACKSLOPE_HH_ */
