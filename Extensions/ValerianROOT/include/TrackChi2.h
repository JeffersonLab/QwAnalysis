/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Tracks Chi2 values


\date <b>Date:</b> 05-13-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef TRACKCHI2_HH_
#define TRACKCHI2_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Track Chi2 Structure
struct MyTrackChi2_t
{
  Int_t R3package;  //package number
  TString VDC;  //The VDC
  TString plane;  //the VDC plane
  Int_t nevents;  //number of events
  Double_t Val;      //Chi2 value
  Double_t RMS;      //RMS value
};

//Function to read in the Chi2 values for
//the track  match up info in R3
//information for a given run number

std::vector<MyTrackChi2_t> GetTrackChi2(Int_t run);

#endif /* TRACKCHI2_HH_ */
