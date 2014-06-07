/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Number of Triggers as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef NTRIGGER_HH_
#define NTRIGGER_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyTrigger_t  //not to be confused with Tigger ;)
{
  Int_t number;  //number of triggers
};

//Function to read in the octatnnumber
//information for a given run number

std::vector<MyTrigger_t> GetTrigger(Int_t run);

#endif /* NTRIGGER_HH_ */
