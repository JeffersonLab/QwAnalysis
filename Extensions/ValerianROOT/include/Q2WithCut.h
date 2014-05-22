/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Q2 with cuts values as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-15-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef Q2WITHCUT_HH_
#define Q2WITHCUT_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyQ2Cut_t
{
  Int_t package;  //package number
  Double_t q2Val;  //the Q2 in m(GeV)^2
  Double_t q2error; //the error on the Q2
  Double_t LWq2Val;  //the light weighted Q2 in m(GeV)^2
  Double_t LWq2error; //the error on the lightweighted Q2
  Double_t LWPq2Val;  //the lightweighted, pedistal subtracted Q2 in m(GeV)^2
  Double_t LWPq2error; //the lightweighted, pedistal subtracted error on the Q2
};

//Function to read in the Q2 with cuts
//information for a given run number

std::vector<MyQ2Cut_t> GetQ2Cut(Int_t run, Int_t pass, TString path);

#endif /* Q2WITHCUT_HH_ */
