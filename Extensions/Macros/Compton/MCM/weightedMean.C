#ifndef __WEIGHTEDMEAN_F
#define __WEIGHTEDMEAN_F

#include "comptonRunConstants.h"

Double_t weightedMean()//Double_t wmNrAsym[nStrips],Double_t wmDrAsym[nStrips],Double_t wmNrBCqNormSum[nStrips],Double_t wmDrBCqNormSum[nStrips],Double_t wmNrBCqNormDiff[nStrips],Double_t wmNrqNormB1L0[nStrips],Double_t wmDrqNormB1L0[nStrips],Double_t wmNrBkgdAsym[nStrips],Double_t wmDrBkgdAsym[nStrips])
{ ///eqn 4.17 Bevington 
  Bool_t debug=0;
  for (Int_t s =startStrip; s <endStrip; s++) {        
    if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    else if(wmDrAsym[s]<=0.0) cout<<red<<"stand. deviation in weighted Mean Asym is non-positive for strip:"<<s+1<<normal<<endl;
    else {
      stripAsym[s] = wmNrAsym[s]/wmDrAsym[s];
      stripAsymEr[s] = TMath::Sqrt(1.0/wmDrAsym[s]);
      if(debug) printf("stripAsym[%d]:%f  stripAsymEr:%f\n",s,stripAsym[s],stripAsymEr[s]);

      stripAsymNr[s] = wmNrBCqNormDiff[s]/wmDrBCqNormSum[s];
      //stripAsymNrEr[s] = TMath::Sqrt(1/wmDrBCqNormDiff[s]);
      ///the error in numerator and denominator are same, hence reevaluation is avoided

      stripAsymDr[s] = wmNrBCqNormSum[s]/wmDrBCqNormSum[s];
      stripAsymDrEr[s] = TMath::Sqrt(1.0/wmDrBCqNormSum[s]);
      //if(debug) printf("stripAsymDr[%d][%d]:%f  stripAsymDrEr:%f\n",p,s,stripAsymDr[s],stripAsymDrEr[s]);

      qNormB1L0[s] = wmNrqNormB1L0[s]/wmDrqNormB1L0[s];///pure Beam background
      qNormB1L0Er[s] = TMath::Sqrt(1.0/wmDrqNormB1L0[s]);
      //if(debug) printf("qNormB1L0[%d][%d]:%f  qNormB1L0Er:%f\n",p,s,qNormB1L0[s],qNormB1L0Er[s]);

      bkgdAsym[s] = wmNrBkgdAsym[s]/wmDrBkgdAsym[s];
      bkgdAsymEr[s] = TMath::Sqrt(1.0/wmDrBkgdAsym[s]);
    }
  }
  return 1.0;
}
//weightedMean : wm
#endif
