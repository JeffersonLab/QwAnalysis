//////////////////////////////////////////////////////////////////////
//
// showRunAverage.C
//   Bryan Moffit - July 2005
//  
//  helper routine for PANGUIN.  Plots the run average of a given
//  quantity
//

void showRunAverage(TString treename, TString given_quantity, 
		    TCut given_cut) {

  gROOT->LoadMacro("macro/RunAverage.C");
  TTree *T = (TTree*)gROOT->FindObject(treename);
  TGraphErrors *gr = RunAverage(T, given_quantity, given_cut);
  if(gr!=NULL) {
    gr->SetMarkerStyle(20);
    gPad->SetGridy(1);
    gr->Draw("AP");
  }
}
