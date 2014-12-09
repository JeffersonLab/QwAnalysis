#ifndef __WEIGHTEDMEAN_F
#define __WEIGHTEDMEAN_F
///This macro is used to calculate weighted mean of a variable
#include "comptonRunConstants.h"

Int_t weightedMean(Double_t wmNr[nStrips],Double_t wmDr[nStrips], Double_t normVar[], Double_t normVarEr[])
{ ///eqn 4.17 Bevington 
  for (Int_t s =startStrip; s <endStrip; s++) {        
    if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    else if(wmDr[s]<=0.0) cout<<red<<"stand. deviation in weighted Mean  is non-positive for strip:"<<s+1<<normal<<endl;
    else {
      normVar[s] = wmNr[s]/wmDr[s];
      normVarEr[s] = TMath::Sqrt(1.0/wmDr[s]);
      //printf("strip[%d]:%f  stripEr:%f\n",s,strip[s],stripEr[s]);
    }
  }
  return 1;
}
//weightedMean : wm
#endif
