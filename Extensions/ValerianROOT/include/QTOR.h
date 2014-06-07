/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Qtor values

  Everything is a subdirectory to the run

\date <b>Date:</b> 05-08-2014
\date <b>Modified:</b> 05-09-2014

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef QTOR_HH_
#define QTOR_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyQtor_t
{
  Double_t current;  //Qtor current - A
};

//Function to ead in the Qtor current
//information for a given run number

std::vector<MyQtor_t> GetQtor(Int_t run);

#endif /* QTOR_HH_ */
