/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Q2 with loss values as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef Q2WITHLOSS_HH_
#define Q2WITHLOSS_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyQ2Loss_t
{
  Int_t R3package;  //package number
  Double_t Val;  //the Q2 in m(GeV)^2
  Double_t Error; //the Error on the Q2
};

//Function to read in the Q2 with loss
//information for a given run number

std::vector<MyQ2Loss_t> GetQ2Loss(Int_t run);

#endif /* Q2WITHLOSS_HH_ */
