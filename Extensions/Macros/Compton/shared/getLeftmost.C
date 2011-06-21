#include "TH1F.h"
////////////////////////////////////////////////////////////////////
//This program returns the abscissa of the leftmost populated bin.//
////////////////////////////////////////////////////////////////////
Float_t getLeftmost(TH1F * hTemp0){
  Float_t minBinEdge = 0; 
  for(Int_t i=hTemp0->GetNbinsX(); i>0; i--){
    if(hTemp0->GetBinContent(i-1)!= 0)
      minBinEdge = hTemp0->GetBinLowEdge(i-1);
  }
  return(minBinEdge);
}
