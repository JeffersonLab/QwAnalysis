/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 */

#include <rootClass.h>
#include "comptonRunConstants.h"
#include "asymFit.C"
#include "infoDAQ.C"
#include "expAsym.C"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, TString dataType="Ac", Bool_t first100k=kFALSE)
{
  cout<<"starting into edetLasCyc.C**************"<<endl;
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);

  gROOT->LoadMacro(" rhoToX.C+g");
  gROOT->LoadMacro(" infoDAQ.C+g");
  gROOT->LoadMacro(" getEBeamLasCuts.C+g");
  gROOT->LoadMacro(" evaluateAsym.C+g");
  gROOT->LoadMacro(" expAsym.C+g");
  gROOT->LoadMacro(" asymFit.C+g");
  gROOT->LoadMacro(" fileReadDraw.C+g");
  if(first100k) return;

  infoDAQ(runnum); //!add a method to verify that config tree was successfully read
  Int_t asymReturn = expAsym(runnum,dataType);

  if(asymReturn!=-1) {
    asymFit(runnum,dataType);
    //fileReadDraw(runnum);  ///This function uses the output from different dataTypes together, hence should not be called while executing single dataType
  } else cout <<"\n***expAsym.C failed so exiting\n"<<endl;
}
