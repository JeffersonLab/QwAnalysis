/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Angle values


\date <b>Date:</b> 05-08-2014
\date <b>Modified:</b>

\note <b>Entry Conditions:</b> none

********************************************/
#ifndef ANGLE_HH_
#define ANGLE_HH_

//ROOT includes
#include <TROOT.h>
#include <TString.h>

//standard include
#include <vector>

//Angle Structure
struct MyAngle_t
{
  Int_t R3package;  //package number
  Double_t Val; //scattering angle value in deg
  Double_t Error; //Error in the scattering angle value
};

//Function to read in the Angle
//information for a given run number

std::vector<MyAngle_t> GetAngle(Int_t run);

#endif /* ANGLE_HH_ */
