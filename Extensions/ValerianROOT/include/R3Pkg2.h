/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the octant number of R3 package 2

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef R3PKG2_HH_
#define R3PKG2_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//QTOR current Structure
struct MyR3Pkg2_t
{
  Int_t octant;  //the octant value
};

//Function to read in the octatnnumber
//information for a given run number

std::vector<MyR3Pkg2_t> GetR3Pkg2(Int_t run);

#endif /* R3PKG2_HH_ */
