//////////////////////////////////////////////////////////////////////
//
// slug.C
//   Bryan Moffit - July 2005
//
//   Handle the compilation of the ok data for SLUG summaries.
//

#include <vector>
#include <iostream>
#include "multirun/TaVarChooser.h"
#include "multirun/TaMakePairFile.h"
#include "multirun/TaRunlist.h"
#include "src/TaFileName.hh"
#include "TString.h"
#include "macro/ParamSave.macro"

int runSlugH(Int_t slugnumber) {

  // plug in some run list here...
  TString runlistStr = "/work/halla/parity/disk1/lkaufman/pan/runlistH.txt";
  //TString runlistStr = "/work/halla/parity/disk1/lkaufman/pan/testlistH.txt";

  // set Chooser file name here.
  TString chooserStr = "/work/halla/parity/disk1/lkaufman/pan/multirun/chooserH.txt" ;
  
  TaRunlist dblist(runlistStr.Data());
  if(!dblist.GoodRunlist()) {
    cout << "Bad runlist" << endl;
    gROOT->ProcessLine(".qqqqqqqq");  
    return 1;
  }
  
  vector <Int_t> templist = dblist.GetListOfRuns(slugnumber);

  vector <pair <UInt_t,UInt_t> > runlist;
  for(UInt_t i=0; i<templist.size(); i++) {
    runlist.push_back(make_pair(templist[i],0));
  }

  cout << "Runs entered (" << runlist.size() << ")" << endl;
  for(UInt_t irun=0; irun<runlist.size(); irun++) {
    cout << "\t" << runlist[irun].first;
    if((irun+1)%5==0) cout << endl;
  }
  cout << endl;

  TaFileName::Setup(slugnumber,"slug");
  TString filename = (TaFileName ("root")).Tstring();
  //  filename = "./testpairfile.root";

  TaMakePairFile *mpf = new TaMakePairFile(filename,chooserStr);
  mpf->SetRunList(runlist);
  mpf->SetDBRunlist(runlistStr);

  mpf->RunLoop();
  mpf->Finish();
  
  cout << "done" << endl;
  gROOT->ProcessLine(".q");
  return 1;

}
