#include <rootClass.h>
#include "comptonRunConstants.h"
#include "asymFit.C"
//#include "infoDAQ.C"
#include "expAsym.C"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, TString dataType="Ac")
{
  cout<<"starting into edetLasCyc.C**************"<<endl;
  Int_t asymflag=0,daqflag=0;
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);

  gROOT->LoadMacro(" rhoToX.C+g");
  gROOT->LoadMacro(" getEBeamLasCuts.C+g");
  gROOT->LoadMacro(" evaluateAsym.C+g");
  gROOT->LoadMacro(" expAsym.C+g");
  gROOT->LoadMacro(" asymFit.C+g");
  gROOT->LoadMacro(" infoDAQ.C+g");
  //gROOT->LoadMacro(" fileReadDraw.C+g");

  daqflag = infoDAQ(runnum); 
  if(daqflag==1) {
    asymflag = expAsym(runnum,dataType);
  } else cout<<"couldn't execute infoDAQ.C"<<endl;

  if(asymflag!=-1) {
    asymFit(runnum,dataType);
    //fileReadDraw(runnum);  ///This function uses the output from different dataTypes together, hence should not be called while executing single dataType
  } else cout <<"\n***expAsym.C failed so exiting\n"<<endl;
}
