#include "rootClass.h"
#include "comptonRunConstants.h"
#include "asymFit.C"
#include "expAsym.C"
#include "infoDAQ.C"
//#include "fileReadDraw.C"

Int_t edetLC(Int_t runnum=24519, TString dataType="Ac")
{
  cout<<"starting into edetLC.C**************"<<endl;
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  ofstream analFlags;
  
  gROOT->LoadMacro(" stripMask.C+g");
  gROOT->LoadMacro(" rhoToX.C+g");
  gROOT->LoadMacro(" getEBeamLasCuts.C+g");
  gROOT->LoadMacro(" evaluateAsym.C+g");
  gROOT->LoadMacro(" expAsym.C+g");
  gROOT->LoadMacro(" asymFit.C+g");
  gROOT->LoadMacro(" infoDAQ.C+g");
  gROOT->LoadMacro(" writeToFile.C+g");
  gROOT->LoadMacro(" qNormVariables.C+g");
  gROOT->LoadMacro(" weightedMean.C+g");
  //gROOT->LoadMacro(" fileReadDraw.C+g");

  plane=1;
  Int_t asymSuc=0;
  daqCheck = infoDAQ(runnum); 
  asymflag = expAsym(runnum,dataType);

  if(asymflag!=-1) {
    if(asymflag==0) cout<<blue<<"this run has NO useful LASER CYCLE"<<normal<<endl;
    else asymSuc = asymFit(runnum,dataType);
    //fileReadDraw(runnum);  ///This function uses the output from different dataTypes together, hence should not be called while executing single dataType
  } else cout <<"\n***expAsym.C failed so exiting\n"<<endl;

  analFlags.open(Form("%s/%s/%sanalysisFlags.txt",pPath,webDirectory,filePre.Data()));
  if (analFlags.is_open()) {
    analFlags<<";runnum\tkRadCor\tkDeadTime\tk2parDT\tkRejectBMod\tkNoiseSub"<<endl;
    analFlags<<runnum<<"\t"<<kRadCor<<"\t"<< kDeadTime<<"\t"<< k2parDT<<"\t"<< kRejectBMod<<"\t"<< kNoiseSub<<endl;
    analFlags.close();
  }

  if(daqCheck<0 || asymflag <0 || asymSuc<0) {
  analFlags.open(Form("%s/%s/%sWhyNotExecuted.txt",pPath,webDirectory,filePre.Data()));
  if (analFlags.is_open()) {
    analFlags<<";runnum\tinfoDAQ\texpAsym\tasymFit"<<endl;
    analFlags<<runnum<<"\t"<< daqCheck<<"\t"<< asymflag<<"\t"<<asymSuc<<endl;
    analFlags.close();
  }
  }

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to execute edetLC.C\n",div_output.quot,div_output.rem );  
  return 2;
}
