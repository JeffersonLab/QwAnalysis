#include "TMath.h"
#include "TFile.h"
#include "TLine.h"
#include "TChain.h"
#include "TPaveText.h"
#include "TString.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TIterator.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObject.h"
#include "TObjArray.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <utility>

int CorrectBModTree(int run, char *stem, double bcm_X = 0.00000614, 
		    double bcm_XP = -0.0895, double bcm_Y = -0.000380, 
		    double bcm_YP = 4.66){
  bool debug = 0;

  //Load Root Tree 
  TChain *ch = new TChain("mps_slug");
  char * dir = "/net/data1/paschkedata1/pass5b_bmod_mpsslug";
  char * new_dir = Form("/net/data1/paschkedata1/pass5b%s",stem);
  ch->Add(Form("%s/mps_only_%i_0*.root",dir, run));
  ch->Add(Form("%s/mps_only_%i_5*.root",dir, run));
  ch->Add(Form("%s/mps_only_%i_1*.root",dir, run));
  ch->Add(Form("%s/mps_only_%i_2*.root",dir, run));
  ch->Add(Form("%s/mps_only_%i_3*.root",dir, run));
  ch->Add(Form("%s/mps_only_%i_ful*.root",dir, run));

  if(ch->GetEntries()<10000){
    cout<<"Too few entries. Exiting\n";
    return -1;
  }

  ch->Draw("qwk_targetX>>hx","ErrorFlag==0||ErrorFlag==67207296","goff");
  TH1D *hx= (TH1D*)gDirectory->Get("hx");
  double meanX = hx->GetMean();
  delete hx;
  ch->Draw("qwk_targetXSlope>>hxp","ErrorFlag==0||ErrorFlag==67207296","goff");
  TH1D *hxp= (TH1D*)gDirectory->Get("hxp");
  double meanXP = hxp->GetMean();
  delete hxp;
  ch->Draw("qwk_targetY>>hy","ErrorFlag==0||ErrorFlag==67207296","goff");
  TH1D *hy= (TH1D*)gDirectory->Get("hy");
  double meanY = hy->GetMean();
  delete hy;
  ch->Draw("qwk_targetYSlope>>hyp","ErrorFlag==0||ErrorFlag==67207296","goff");
  TH1D *hyp= (TH1D*)gDirectory->Get("hyp");
  double meanYP = hyp->GetMean();
  delete hyp;
  cout<<"Mean monitor values: "<<endl;
  cout<<"qwk_targetX: "<<Form("%0.15e", meanX)<<endl;
  cout<<"qwk_targetXP: "<<Form("%0.15e", meanXP)<<endl;
  cout<<"qwk_targetY: "<<Form("%0.15e", meanY)<<endl;
  cout<<"qwk_targetYP: "<<Form("%0.15e", meanYP)<<endl;

  //Get vector of leaf names
  TIterator *it = ch->GetIteratorOnAllLeaves();
  TObject *obj = it->Next();
  int nCorr = 0;
  vector<TString>name, temp;
  while(obj){
    TString nm = obj->GetName();
    if(nm.CompareTo("__noname0")==0){
      obj = it->Next();
      continue;
    }
    if((nm.Contains("md") || nm.Contains("lumi") || nm.Contains("pmt")) && 
       !nm.Contains("Device")){
      if(debug)  cout<<nCorr<<" "<<nm.Data()<<endl;
      name.push_back(nm);
      ++nCorr;
    }else{
      temp.push_back(nm);
    }
    obj = it->Next();
  }
  for(int i=0;i<(int)temp.size();++i){
    if(temp[i].CompareTo("qwk_targetX")==0 ||
       temp[i].CompareTo("qwk_targetXSlope")==0 ||
       temp[i].CompareTo("qwk_targetY")==0 ||
       temp[i].CompareTo("qwk_targetYSlope")==0) continue;
    name.push_back(temp.at(i));
  }
  if(debug || 1){
    cout<<endl;
    for(int i=0;i<(int)name.size();++i){
      cout<<i<<") "<<name[i].Data()<<endl;
    }
  }
  int size = (int)name.size();

  //Set branch addresses in old tree
  ch->ResetBranchAddresses();
  double *leaf = new double[size], tgX, tgXP, tgY, tgYP;
  int chargeIdx = -1;
  for(int i=0;i<size;++i){
    ch->SetBranchAddress(name[i].Data(), &leaf[i]);
    if(name[i].CompareTo("qwk_charge")==0) chargeIdx = i;
  }
  if(chargeIdx<0){
    cout<<"Charge leaf not found. Exiting.\n";
    return -1;
  }
  ch->SetBranchAddress("qwk_targetX", &tgX);
  ch->SetBranchAddress("qwk_targetXSlope", &tgXP);
  ch->SetBranchAddress("qwk_targetY", &tgY);
  ch->SetBranchAddress("qwk_targetYSlope", &tgYP);


  //Set up branches in new tree
  TFile *file = new TFile(Form("%s/mps_only_%i_full.root", new_dir, run), 
			  "recreate");
  if(!file){
    cout<<"New file not created. Exiting.\n";
    return -1;
  }
  TTree *tree = new TTree("mps_slug", "mps_slug");
  TBranch *br[size];
  for(int i=0;i<size;++i){
    br[i] = tree->Branch(name[i].Data(), &leaf[i], Form("%s/D",name[i].Data()));
  }
  TBranch *brX = tree->Branch("qwk_targetX", &tgX , "qwk_targetX/D");
  TBranch *brXP = tree->Branch("qwk_targetXSlope", &tgXP , "qwk_targetXSlope/D");
  TBranch *brY = tree->Branch("qwk_targetY", &tgY , "qwk_targetY/D");
  TBranch *brYP = tree->Branch("qwk_targetYSlope", &tgYP , "qwk_targetYSlope/D");

  //Fill new tree
  for(int ient=0;ient<ch->GetEntries();++ient){
    ch->GetEntry(ient);
    if(ient%10000==0)
      cout<<"Processing entry "<<ient<<" of "<<ch->GetEntries()<<endl;
    double correction =  bcm_X *(tgX - meanX) + bcm_XP *(tgXP - meanXP) 
                       + bcm_Y *(tgY - meanY) + bcm_YP *(tgYP - meanYP);
    for(int i=0;i<nCorr;++i)
      //      leaf[i] *= (1 + correction);
      leaf[i] *= leaf[chargeIdx];
    tree->Fill();
  }
  tree->Write("",TObject::kOverwrite);
  file->Close();
  return 0;
}
