#ifndef __WEIGHTEDMEAN_F
#define __WEIGHTEDMEAN_F

#include "comptonRunConstants.h"

Double_t weightedMean(Double_t weightedMeanNrAsym[nPlanes][nStrips],Double_t weightedMeanDrAsym[nPlanes][nStrips],Double_t weightedMeanNrBCqNormSum[nPlanes][nStrips],Double_t weightedMeanDrBCqNormSum[nPlanes][nStrips],Double_t weightedMeanNrBCqNormDiff[nPlanes][nStrips],Double_t weightedMeanNrqNormB1L0[nPlanes][nStrips],Double_t weightedMeanDrqNormB1L0[nPlanes][nStrips],Double_t weightedMeanNrBkgdAsym[nPlanes][nStrips],Double_t weightedMeanDrBkgdAsym[nPlanes][nStrips])
{
  Bool_t debug=0;
  for (Int_t p =startPlane; p <endPlane; p++) { ///eqn 4.17 Bevington
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (!mask[p][s]) continue;
      if(weightedMeanDrAsym[p][s]<=0.0) cout<<"stand. deviation in weighted Mean Asym is non-positive for p"<<p<<" s"<<s<<endl;
      else {
	stripAsym[p][s] = weightedMeanNrAsym[p][s]/weightedMeanDrAsym[p][s];
	if (weightedMeanDrAsym[p][s]<0.0) {
	  stripAsymEr[p][s] = TMath::Sqrt(-(1.0/weightedMeanDrAsym[p][s]));//!!shouldn't need to do this
	  cout<<red<<"*** Warning: the weightedMeanDrAsym for plane:"<<p+1<<", strip:"<<s+1<<" is (negative) :"<<weightedMeanDrAsym[p][s]<<normal<<endl;
	} else stripAsymEr[p][s] = TMath::Sqrt(1.0/weightedMeanDrAsym[p][s]);
	if(debug) printf("stripAsym[%d][%d]:%f  stripAsymEr:%f\n",p,s,stripAsym[p][s],stripAsymEr[p][s]);

	stripAsymNr[p][s] = weightedMeanNrBCqNormDiff[p][s]/weightedMeanDrBCqNormSum[p][s];
	//stripAsymNrEr[p][s] = TMath::Sqrt(1/weightedMeanDrBCqNormDiff[p][s]);
	///the error in numerator and denominator are same, hence reevaluation is avoided

	stripAsymDr[p][s] = weightedMeanNrBCqNormSum[p][s]/weightedMeanDrBCqNormSum[p][s];
	if (weightedMeanDrBCqNormSum[p][s]<0.0) {
	  stripAsymDrEr[p][s] = TMath::Sqrt(-(1.0/weightedMeanDrBCqNormSum[p][s]));
	  cout<<"***Warning: stripAsymDrEr coming negative "<<stripAsymDrEr[p][s]<<endl;
	} else stripAsymDrEr[p][s] = TMath::Sqrt(1.0/weightedMeanDrBCqNormSum[p][s]);
	//if(debug) printf("stripAsymDr[%d][%d]:%f  stripAsymDrEr:%f\n",p,s,stripAsymDr[p][s],stripAsymDrEr[p][s]);

	qNormB1L0[p][s] = weightedMeanNrqNormB1L0[p][s]/weightedMeanDrqNormB1L0[p][s];///pure Beam background
	if (weightedMeanDrqNormB1L0[p][s]<0.0) qNormB1L0Er[p][s] = TMath::Sqrt(-1.0/weightedMeanDrqNormB1L0[p][s]);
	else qNormB1L0Er[p][s] = TMath::Sqrt(1.0/weightedMeanDrqNormB1L0[p][s]);
	//if(debug) printf("qNormB1L0[%d][%d]:%f  qNormB1L0Er:%f\n",p,s,qNormB1L0[p][s],qNormB1L0Er[p][s]);
 
	bkgdAsym[p][s] = weightedMeanNrBkgdAsym[p][s]/weightedMeanDrBkgdAsym[p][s];
	if (weightedMeanDrBkgdAsym[p][s]<0.0) bkgdAsymEr[p][s] = TMath::Sqrt(-(1.0/weightedMeanDrBkgdAsym[p][s]));//!!shouldn't need to do this
	else bkgdAsymEr[p][s] = TMath::Sqrt(1.0/weightedMeanDrBkgdAsym[p][s]);
      }
    }
  }
  return 1.0;
}
#endif
