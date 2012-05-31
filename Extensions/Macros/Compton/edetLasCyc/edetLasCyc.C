/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 * As of now, this script only calculates the ratio, and needs more fine tuning
 */

#include <rootClass.h>
#include "comptonRunConstants.h"
#include "asymFit.C"
#include "theoryAsym.C"
//#include "expAsym.C"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, Bool_t first100k=kFALSE)
{
  cout<<"starting into edetLasCyc.C**************"<<endl;
  gROOT->LoadMacro(" maskedStrips.C+g");
  gROOT->LoadMacro(" getEBeamLasCuts.C+g");
  gROOT->LoadMacro(" evaluateAsym.C+g");
  gROOT->LoadMacro(" expAsym.C+g");
  gROOT->LoadMacro(" asymFit.C+g");
  gROOT->LoadMacro(" fileReadDraw.C+g");
  gROOT->LoadMacro(" edetLasCyc.C+g");
  if(first100k) return;
  expAsym(runnum);

  if(!noiseRun) {
    asymFit(runnum);
    fileReadDraw(runnum);  
  }
}
