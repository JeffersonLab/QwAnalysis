//////////////////////////////////////////////////////////////////////
//
// daily.C
//   Bryan Moffit - July 2005
//
//   Handle the compilation of the ok data for Daily summaries.
//

#include <vector>
#include <iostream>
#include "multirun/TaVarChooser.h"
#include "multirun/TaMakePairFile.h"
#include "src/TaFileName.hh"
#include "TString.h"
#include "macro/ParamSave.macro"

void runDaily(UInt_t nruns, UInt_t *a_runs, UInt_t id) {

  vector <pair <UInt_t,UInt_t> > runlist;
  for(UInt_t i=0; i<nruns; i++) {
    runlist.push_back(make_pair(a_runs[i],0));
  }

  cout << "Runs entered (" << runlist.size() << ")" << endl;
  for(UInt_t irun=0; irun<runlist.size(); irun++) {
    cout << "\t" << runlist[irun].first;
    if((irun+1)%5==0) cout << endl;
  }
  cout << endl;

  TaFileName::Setup(id,"daily");
  TString filename = (TaFileName ("root")).Tstring();

  TaMakePairFile *mpf = new TaMakePairFile(filename,"multirun/chooser.txt");
  mpf->SetRunList(runlist);
  mpf->SetDBRunlist("runlist.txt");
  mpf->RunLoop();
  mpf->Finish();

  TFile daily(filename);
  TTree *s = (TTree*)daily.Get("S");

  // Check to see if there's more than one "slowsign" in the rootfile
  //  If so, separate the statistics according to that slowsign.
  string IHWP[2] = {"OUT","IN"};
  string signcut[2] = {"sign==1","sign==-1"};
  
  for(UInt_t istate=0; istate<2; istate++) {
    Stat_t datasign = s->Draw("sign",(TString)signcut[istate],"goff");

    if(datasign==0) continue;

    ParamSave *ps = new ParamSave(id,IHWP[istate]);
    ps->PutComment("Insertable halfwave plate = "+IHWP[istate]);

    ps->PutCut("ok_cutL&&"+signcut[istate], *s);
    ps->PutCut("ok_cutR&&"+signcut[istate], *s);
    ps->PutCut("ok_cutB&&"+signcut[istate], *s);
    ps->PutCut("(ok_cutR||ok_cutL)&&"+signcut[istate], *s);
    
    ps->PutAvg("bcm1", "avg", *s, signcut[istate]);
    ps->PutAvg("bcm2", "avg", *s, signcut[istate]);
    ps->PutAvg("bcm3", "avg", *s, signcut[istate]);
    ps->PutAvg("bcm6", "avg", *s, signcut[istate]);
    ps->PutAvg("bcmcav1", "avg", *s, signcut[istate]);
    ps->PutAvg("bcmcav2", "avg", *s, signcut[istate]);
    ps->PutAvg("bcmcav3", "avg", *s, signcut[istate]);  
    ps->PutAvg("bcm10", "avg", *s, "ok_cutC&&"+signcut[istate]);
    ps->PutAvg("bpm4ax", "avg", *s, signcut[istate]);
    ps->PutAvg("bpm4ay", "avg", *s, signcut[istate]);
    ps->PutAvg("bpm4bx", "avg", *s, signcut[istate]);
    ps->PutAvg("bpm4by", "avg", *s, signcut[istate]);
    ps->PutAvg("bpm12x", "avg", *s, signcut[istate]);
    ps->PutAvg("det1", "avg_n", *s, "ok_cutL&&"+signcut[istate]);
    ps->PutAvg("det3", "avg_n", *s, "ok_cutR&&"+signcut[istate]);
    
    ps->PutMonAsym("bcm1", *s, signcut[istate]);
    ps->PutMonAsym("bcm2", *s, signcut[istate]);
    ps->PutMonAsym("bcm3", *s, signcut[istate]);
    ps->PutMonAsym("bcm6", *s, signcut[istate]);
    ps->PutMonAsym("bcmcav1", *s, signcut[istate]);
    ps->PutMonAsym("bcmcav2", *s, signcut[istate]);
    ps->PutMonAsym("bcmcav3", *s, signcut[istate]);
    ps->PutMonAsym("bcm10", *s, "ok_cutC&&"+signcut[istate]);
    
    ps->PutMonDiff("bpm4ax", *s, signcut[istate]);
    ps->PutMonDiff("bpm4ay", *s, signcut[istate]);
    ps->PutMonDiff("bpm4bx", *s, signcut[istate]);
    ps->PutMonDiff("bpm4by", *s, signcut[istate]);
    ps->PutMonDiff("bpm12x", *s, signcut[istate]);
    
    ps->PutMonDDAsym("bcm1", "bcm2", *s, signcut[istate]);
    ps->PutMonDDAsym("bcm1", "bcm3", *s, signcut[istate]);
    ps->PutMonDDAsym("bcm2", "bcm3", *s, signcut[istate]);
    ps->PutMonDDDiff("bpm4ax", "bpm4bx", *s, signcut[istate]);
    ps->PutMonDDDiff("bpm4ay", "bpm4by", *s, signcut[istate]);
    
    ps->PutMonDD("det1", "det3", "reg_asym_n", *s, "ok_cutB&&"+signcut[istate]);
    ps->PutDetAsymn("blumi1", *s, signcut[istate]);
    ps->PutDetAsymn("blumi2", *s, signcut[istate]);
    ps->PutDetAsymn("blumi3", *s, signcut[istate]);
    ps->PutDetAsymn("blumi4", *s, signcut[istate]);
    ps->PutDetAsymn("blumi5", *s, signcut[istate]);
    ps->PutDetAsymn("blumi6", *s, signcut[istate]);
    ps->PutDetAsymn("blumi7", *s, signcut[istate]);
    ps->PutDetAsymn("blumi8", *s, signcut[istate]);
    ps->PutDetAsymn("blumi_ave", *s, signcut[istate]);
    ps->PutDetAsymn("blumi_sum", *s, signcut[istate]);
    ps->PutDetAsymn("flumi1", *s, signcut[istate]);
    ps->PutDetAsymn("flumi2", *s, signcut[istate]);
    ps->PutDetAsymn("flumi_ave", *s, signcut[istate]);
    ps->PutDetAsymn("flumi_sum", *s, signcut[istate]);
    
    ps->PutDetAsymn("det1", *s, "ok_cutL&&"+signcut[istate]);
    //       ps->PutDetAsymn("det2", *s, "ok_cutL&&"+signcut[istate]);
    ps->PutDetAsymn("det3", *s, "ok_cutR&&"+signcut[istate]);
    //       ps->PutDetAsymn("det4", *s, "ok_cutR&&"+signcut[istate]);
    ps->PutDetAsymn("det_lo", *s, "ok_cutB&&"+signcut[istate]);
    //       ps->PutDetAsymn("det_hi", *s, "ok_cutB&&"+signcut[istate]);
    //       ps->PutDetAsymn("det_all", *s, "ok_cutB&&"+signcut[istate]);
    //       ps->PutDetAsymn("det_ave", *s, "ok_cutB&&"+signcut[istate]);

    ps->Print();
    delete ps;
  }



}
