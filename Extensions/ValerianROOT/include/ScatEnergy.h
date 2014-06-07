/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file for
  the Scattering energy values as seen in the auto
  Q2 script

  Everything is a element to the run

\date <b>Date:</b> 05-14-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef SCATENERGY_HH_
#define SCATENERGY_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard includes
#include <vector>

//Scattering energy Structure
struct MyScatEnergy_t  //no this is not what you are thinking...
{
  Int_t R3package;  //package number
  Double_t Val;  //the of the scattering energy GeV
  Double_t Error; //the Error on the scattering energy
};

//Function to read in the scattering energy
//information for a given run number

std::vector<MyScatEnergy_t> GetScatEnergy(Int_t run);
//this fuction does not read out calories spent ;)

#endif /* SCATENERGY_HH_ */
