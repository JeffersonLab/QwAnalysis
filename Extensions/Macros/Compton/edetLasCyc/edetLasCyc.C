/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 */

#include <rootClass.h>
#include "comptonRunConstants.h"
#include "asymFit.C"
#include "infoDAQ.C"
//#include "expAsym.C"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, Bool_t first100k=kFALSE)
{
  cout<<"starting into edetLasCyc.C**************"<<endl;
  gROOT->LoadMacro(" rhoToX.C+g");
  gROOT->LoadMacro(" infoDAQ.C+g");
  gROOT->LoadMacro(" getEBeamLasCuts.C+g");
  //gROOT->LoadMacro(" evaluateAsym_r4409.C+g");
  gROOT->LoadMacro(" evaluateAsym.C+g");
  //gROOT->LoadMacro(" expAsym_r4409.C+g");
  gROOT->LoadMacro(" expAsym.C+g");
  //gROOT->LoadMacro(" asymFit_r4409.C+g");
  gROOT->LoadMacro(" asymFit.C+g");
  gROOT->LoadMacro(" fileReadDraw.C+g");
  //gROOT->LoadMacro(" edetLasCyc.C+g");
  if(first100k) return;

  infoDAQ(runnum); //!add a method to verify that config tree was successfully read
  Int_t asymReturn = expAsym(runnum);

  if(asymReturn!=-1) {
    asymFit(runnum);
    fileReadDraw(runnum);  
  } else cout <<"\n***expAsym.C failed so exiting\n"<<endl;
}
