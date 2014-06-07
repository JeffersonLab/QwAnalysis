/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Q2 without loss values as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef Q2WITHOUTLOSS_HH_
#define Q2WITHOUTLOSS_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyQ2NoLoss_t  //this Q2 found!
{
  Int_t R3package;  //package number
  Double_t Val;  //the Q2 in m(GeV)^2
  Double_t Error; //the Error on the Q2
};

//Function to read in the Q2 with loss
//information for a given run number

std::vector<MyQ2NoLoss_t> GetQ2NoLoss(Int_t run);

#endif /* Q2WITHOUTLOSS_HH_ */
