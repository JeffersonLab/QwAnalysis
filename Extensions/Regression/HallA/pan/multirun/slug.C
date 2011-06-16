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

void runSlug(Int_t slugnumber) {

  TaRunlist dblist("runlist.txt");
  if(!dblist.GoodRunlist()) {
    cout << "Bad runlist" << endl;
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

  TaMakePairFile *mpf = new TaMakePairFile(filename,"multirun/chooser.txt",0);
  mpf->SetRunList(runlist);
  mpf->SetDBRunlist("runlist.txt");
  mpf->RunLoop();
  mpf->Finish();

  TFile slugfn(filename);
  TTree *s = (TTree*)slugfn.Get("S");

  ParamSave *ps = new ParamSave(slugnumber,"slug");
  ps->PutComment(Form("Slug = %d",slugnumber));

  ps->PutCut("ok_regL", *s);
  ps->PutCut("ok_regR", *s);
  ps->PutCut("ok_regB", *s);
  ps->PutCut("ok_cutC", *s);
  ps->PutCut("(ok_regR||ok_regL)", *s);
  
  ps->PutAvg("bcm1", "avg", *s, "ok_cut");
  ps->PutAvg("bcm2", "avg", *s, "ok_cut");
  ps->PutAvg("bcm3", "avg", *s, "ok_cut");
  ps->PutAvg("bcm4", "avg", *s, "ok_cut");
  ps->PutAvg("bcm5", "avg", *s, "ok_cut");
  ps->PutAvg("bcm6", "avg", *s, "ok_cut");
  ps->PutAvg("bcm7", "avg", *s, "ok_cut");
  ps->PutAvg("bcm8", "avg", *s, "ok_cut");
  ps->PutAvg("bcm9", "avg", *s, "ok_cut");
  ps->PutAvg("bcm10", "avg", *s, "ok_cut");
  ps->PutAvg("bcm11", "avg", *s, "ok_cut");
  ps->PutAvg("bcm12", "avg", *s, "ok_cut");
  ps->PutAvg("bcm13", "avg", *s, "ok_cut");
  ps->PutAvg("bpm4ax", "avg", *s, "ok_cut");
  ps->PutAvg("bpm4ay", "avg", *s, "ok_cut");
  ps->PutAvg("bpm4bx", "avg", *s, "ok_cut");
  ps->PutAvg("bpm4by", "avg", *s, "ok_cut");
  ps->PutAvg("bpm12x", "avg", *s, "ok_cut");
  ps->PutAvg("det1", "avg_n", *s, "ok_cut");
  ps->PutAvg("det2", "avg_n", *s, "ok_cut");
  ps->PutAvg("det3", "avg_n", *s, "ok_cut");
  ps->PutAvg("det4", "avg_n", *s, "ok_cut");
  ps->PutAvg("det_l", "avg_n", *s, "ok_cut");
  ps->PutAvg("det_r", "avg_n", *s, "ok_cut");
  ps->PutAvg("det_lo", "avg_n", *s, "ok_cut");
  ps->PutAvg("det_hi", "avg_n", *s, "ok_cut");
  ps->PutAvg("det_all", "avg_n", *s, "ok_cut");

  ps->PutMonAsym("bcm1", *s, "ok_cut");
  ps->PutMonAsym("bcm2", *s, "ok_cut");
  ps->PutMonAsym("bcm3", *s, "ok_cut");
  ps->PutMonAsym("bcm4", *s, "ok_cut");
  ps->PutMonAsym("bcm5", *s, "ok_cut");
  ps->PutMonAsym("bcm6", *s, "ok_cut");
  ps->PutMonAsym("bcm7", *s, "ok_cut");
  ps->PutMonAsym("bcm8", *s, "ok_cut");
  ps->PutMonAsym("bcm9", *s, "ok_cut");
  ps->PutMonAsym("bcm10", *s, "ok_cut");
  ps->PutMonAsym("bcm11", *s, "ok_cut");
  ps->PutMonAsym("bcm12", *s, "ok_cut");
  ps->PutMonAsym("bcm13", *s, "ok_cutC");
  ps->PutMonAsym("bcmcav2", *s, "ok_cut");
  ps->PutMonAsym("bcmcav3", *s, "ok_cut");
      
  ps->PutMonDiff("bpm4ax", *s, "ok_cut");
  ps->PutMonDiff("bpm4ay", *s, "ok_cut");
  ps->PutMonDiff("bpm4bx", *s, "ok_cut");
  ps->PutMonDiff("bpm4by", *s, "ok_cut");
  ps->PutMonDiff("bpm12x", *s, "ok_cut");
  ps->PutMonDiff("bpmcav2x", *s, "ok_cut");
  ps->PutMonDiff("bpmcav2y", *s, "ok_cut");
  ps->PutMonDiff("bpmcav3x", *s, "ok_cut");
  ps->PutMonDiff("bpmcav3y", *s, "ok_cut");
      
  ps->PutMonDDAsym("bcm1", "bcm3", *s, "ok_cut");
  ps->PutMonDDAsym("bcmcav2", "bcmcav3", *s, "ok_cut");
  ps->PutMonDDDiff("bpm4ax", "bpm4bx", *s, "ok_cut");
  ps->PutMonDDDiff("bpm4ay", "bpm4by", *s, "ok_cut");
  ps->PutMonDDDiff("bpmcav2x", "bpmcav3x", *s, "ok_cut");
  ps->PutMonDDDiff("bpmcav2y", "bpmcav3y", *s, "ok_cut");
  ps->PutMonDD("det1", "det2", "reg_asym_n", *s, "ok_cut");
  ps->PutMonDD("det1", "det3", "reg_asym_n", *s, "ok_cut");
  ps->PutMonDD("det1", "det4", "reg_asym_n", *s, "ok_cut");
  ps->PutMonDD("det2", "det3", "reg_asym_n", *s, "ok_cut");
  ps->PutMonDD("det2", "det4", "reg_asym_n", *s, "ok_cut");
  ps->PutMonDD("det3", "det4", "reg_asym_n", *s, "ok_cut");
      
  ps->PutDetAsymn("blumi1", *s, "ok_cut");
  ps->PutDetAsymn("blumi2", *s, "ok_cut");
  ps->PutDetAsymn("blumi3", *s, "ok_cut");
  ps->PutDetAsymn("blumi4", *s, "ok_cut");
  ps->PutDetAsymn("blumi5", *s, "ok_cut");
  ps->PutDetAsymn("blumi6", *s, "ok_cut");
  ps->PutDetAsymn("blumi7", *s, "ok_cut");
  ps->PutDetAsymn("blumi8", *s, "ok_cut");
  ps->PutDetAsymn("blumi_h", *s, "ok_cut");
  ps->PutDetAsymn("blumi_v", *s, "ok_cut");
  ps->PutDetAsymn("blumi_d1", *s, "ok_cut");
  ps->PutDetAsymn("blumi_d2", *s, "ok_cut");
  ps->PutDetAsymn("blumi_c", *s, "ok_cut");
  ps->PutDetAsymn("blumi_x", *s, "ok_cut");
  ps->PutDetAsymn("blumi_ave", *s, "ok_cut");
  ps->PutDetAsymn("blumi_sum", *s, "ok_cut");

  ps->PutDetAsymn("det1", *s, "ok_cut");
  ps->PutDetAsymn("det2", *s, "ok_cut");
  ps->PutDetAsymn("det3", *s, "ok_cut");
  ps->PutDetAsymn("det4", *s, "ok_cut");
  ps->PutDetAsymn("det_l", *s, "ok_cut");
  ps->PutDetAsymn("det_r", *s, "ok_cut");
  ps->PutDetAsymn("det_lo", *s, "ok_cut");
  ps->PutDetAsymn("det_hi", *s, "ok_cut");
  ps->PutDetAsymn("det_all", *s, "ok_cut");
  ps->PutDetAsymn("det_ave", *s, "ok_cut");




//   ps->PutAvg("bcm1", "avg", *s, "");
//   ps->PutAvg("bcm2", "avg", *s, "");
//   ps->PutAvg("bcm3", "avg", *s, "");
//   ps->PutAvg("bcm5", "avg", *s, "");
//   //  ps->PutAvg("bcm10", "avg", *s, "ok_cutC");
//   ps->PutAvg("bcmcav2", "avg", *s, "");
//   ps->PutAvg("bcmcav3", "avg", *s, "");  

//   //  ps->PutAvg("bpm1x", "avg", *s, "");
//   //  ps->PutAvg("bpm1y", "avg", *s, "");
//   ps->PutAvg("bpm4ax", "avg", *s, "");
//   ps->PutAvg("bpm4ay", "avg", *s, "");
//   ps->PutAvg("bpm4bx", "avg", *s, "");
//   ps->PutAvg("bpm4by", "avg", *s, "");
//   ps->PutAvg("bpm12x", "avg", *s, "");
//   //  ps->PutAvg("bpm14x", "avg", *s, "");
//   //ps->PutAvg("det1", "avg_n", *s, "avg_n_bcm1<100");
//   //ps->PutAvg("det2", "avg_n", *s, "avg_n_bcm1<100");
//   ps->PutAvg("det1", "avg_n", *s, "ok_regL");
//   ps->PutAvg("det2", "avg_n", *s, "ok_regR");
//   ps->PutAvg("det3", "avg_n", *s, "ok_regL");
//   ps->PutAvg("det4", "avg_n", *s, "ok_regR");
//   ps->PutAvg("det_all", "avg_n", *s, "ok_regB");
//   //  ps->PutAvg("blumi_sum", "avg_n", *s, "");
 
//   ps->PutMonAsym("bcm1", *s, "");
//   ps->PutMonAsym("bcm2", *s, "");
//   //  ps->PutMonAsym("bcm10", *s, "ok_cutC");
//   ps->PutMonAsym("bcmcav2", *s, "");
//   ps->PutMonAsym("bcmcav3", *s, "");

//   //  ps->PutMonDiff("bpm1x", *s, "");
//   //  ps->PutMonDiff("bpm1y", *s, "");  
//   ps->PutMonDiff("bpm4ax", *s, "");
//   ps->PutMonDiff("bpm4ay", *s, "");
//   ps->PutMonDiff("bpm4bx", *s, "");
//   ps->PutMonDiff("bpm4by", *s, "");
//   ps->PutMonDiff("bpm12x", *s, "");
//   //  ps->PutMonDiff("bpm14x", *s, "");
  
//   ps->PutMonDDAsym("bcm1", "bcm2", *s, "");
//   ps->PutMonDDDiff("bpm4ax", "bpm4bx", *s, "");
//   ps->PutMonDDDiff("bpm4ay", "bpm4by", *s, "");
  
//   ps->PutMonDD("det1", "det3", "asym_n", *s, "ok_regB");
//   ps->PutMonDD("det1", "det3", "reg_asym_n", *s, "ok_regB");
//   ps->PutMonDD("det2", "det4", "asym_n", *s, "ok_regB");
//   ps->PutMonDD("det2", "det4", "reg_asym_n", *s, "ok_regB");
//   ps->PutDetAsymn("blumi1", *s, "");
//   ps->PutDetAsymn("blumi2", *s, "");
//   ps->PutDetAsymn("blumi3", *s, "");
//   ps->PutDetAsymn("blumi4", *s, "");
//   ps->PutDetAsymn("blumi5", *s, "");
//   ps->PutDetAsymn("blumi6", *s, "");
//   ps->PutDetAsymn("blumi7", *s, "");
//   ps->PutDetAsymn("blumi8", *s, "");
// //   ps->PutDetAsymn("blumi_h", *s, "");
// //   ps->PutDetAsymn("blumi_v", *s, "");
// //   ps->PutDetAsymn("blumi_d1", *s, "");
// //   ps->PutDetAsymn("blumi_d2", *s, "");
// //   ps->PutDetAsymn("blumi_c", *s, "");
// //   ps->PutDetAsymn("blumi_x", *s, "");
//   ps->PutDetAsymn("blumi_ave", *s, "");
//   ps->PutDetAsymn("blumi_sum", *s, "");
  
//   ps->PutDetAsymn("det1", *s, "ok_regR");
//   ps->PutDetAsymn("det2", *s, "ok_regR");
//   ps->PutDetAsymn("det3", *s, "ok_regL");
//   ps->PutDetAsymn("det4", *s, "ok_regL");
//   //  ps->PutDetAsymn("det_l", *s, "ok_cutB");
//   ps->PutDetAsymn("det_all", *s, "ok_regB");
//   ps->PutDetAsymn("det_ave", *s, "ok_regB");


  ps->Print();
  delete ps;
 

}
