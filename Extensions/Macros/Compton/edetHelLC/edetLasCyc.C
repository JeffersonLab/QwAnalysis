#include <rootClass.h>
#include "comptonRunConstants.h"
#include "asymFit.C"
//#include "infoDAQ.C"
#include "expAsym.C"
#include "fileReadDraw.C"

Int_t edetLasCyc(Int_t runnum=24519, TString dataType="Ac")
{
  cout<<"starting into edetLasCyc.C**************"<<endl;
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Int_t daqflag=0;
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
  gROOT->LoadMacro(" writeToFile.C+g");
  gROOT->LoadMacro(" qNormVariables.C+g");
  //gROOT->LoadMacro(" fileReadDraw.C+g");

  daqflag = infoDAQ(runnum); 
  if(daqflag==1) {
    asymflag = expAsym(runnum,dataType);
  } else {
    cout<<"couldn't execute infoDAQ.C"<<endl;
    cout<<"probably root file doesn't exist"<<endl;
    asymflag = -1;
  }

  if(asymflag!=-1) {
    if(asymflag==0) cout<<blue<<"this run has NO useful LASER CYCLE"<<normal<<endl;
    else asymFit(runnum,dataType);
    //fileReadDraw(runnum);  ///This function uses the output from different dataTypes together, hence should not be called while executing single dataType
  } else cout <<"\n***expAsym.C failed so exiting\n"<<endl;

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to execute edetLasCyc.C\n",div_output.quot,div_output.rem );  
  return 2;
}
