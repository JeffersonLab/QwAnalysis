#include "TH1F.h"
//////////////////////////////////////////////////////////////////////
//This program returns the abscissa of the righttmost populated bin.//
//////////////////////////////////////////////////////////////////////
Float_t getRightmost(TH1F * hTemp0){
  Float_t maxBinEdge = 0; 
  for(Int_t i=0;i<hTemp0->GetNbinsX();i++){
    if(hTemp0->GetBinContent(i)!= 0)
      maxBinEdge = hTemp0->GetBinLowEdge(i+1);
  }
  return(maxBinEdge);
}



