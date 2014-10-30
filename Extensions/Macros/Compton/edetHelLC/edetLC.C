#include "rootClass.h"
#include "comptonRunConstants.h"
#include "asymFit.C"
#include "expAsym.C"
#include "infoDAQ.C"
#include "stripMask.C"
//#include "fileReadDraw.C"

Int_t edetLC(Int_t runnum, TString dataType="Ac")
{
  cerr<<"starting into edetLC.C**************"<<endl;
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  ofstream analFlags;

  plane=1;
  Int_t asymSuc=0;
  retInfoDAQ = infoDAQ(runnum); 
  asymflag = expAsym(runnum,dataType);

  if(asymflag!=-1) {
    if(asymflag==0) cerr<<blue<<"this run has NO useful LASER CYCLE"<<normal<<endl;
    else asymSuc = asymFit(runnum,dataType);
    //fileReadDraw(runnum);  ///This function uses the output from different dataTypes together, hence should not be called while executing single dataType
  } else cerr <<"\n***expAsym.C failed so exiting\n"<<endl;

  analFlags.open(Form("%s/%s/%sanalysisFlags.txt",pPath, txt,filePre.Data()));
  if (analFlags.is_open()) {
    analFlags<<";runnum\tkRadCor\tkDeadTime\tk2parDT\tkRejectBMod\tkNoiseSub\tkOnlyGoodLasCyc"<<endl;
    analFlags<<runnum<<"\t"<<kRadCor<<"\t"<< kDeadTime<<"\t"<< k2parDT<<"\t"<< kRejectBMod<<"\t"<< kNoiseSub<<"\t"<<kOnlyGoodLasCyc<<endl;
    analFlags.close();
  }

  if(retInfoDAQ<0 || asymflag <0 || asymSuc<0) {
    analFlags.open(Form("%s/%s/%sWhyNotExecuted.txt",pPath, txt,filePre.Data()));
    if (analFlags.is_open()) {
      analFlags<<";runnum\tinfoDAQ\texpAsym\tasymFit"<<endl;
      analFlags<<runnum<<"\t"<< retInfoDAQ<<"\t"<< asymflag<<"\t"<<asymSuc<<endl;
      analFlags.close();
    }
  }

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to execute edetLC.C\n",div_output.quot,div_output.rem );  
  gSystem->Exec(Form("%s/make_erun_page.sh %d",pPath, runnum));
  return 2;
}
