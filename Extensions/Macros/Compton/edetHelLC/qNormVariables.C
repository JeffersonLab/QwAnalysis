#ifndef __QNORMVARIABLES_F
#define __QNORMVARIABLES_F

#include <rootClass.h>
#include "comptonRunConstants.h"
///This routine takes in a variable and the corresponding charge/time to give the corresponding normalized counts and the associated error. it should work for both time as well as charge normalization. Instead of the earlier format, it will now be repeatedly called for every instance of a variable to be normalized.
void qNormVariables(Int_t totCounts[nPlanes][nStrips],Double_t totI,Double_t qNormCnts[nPlanes][nStrips],Double_t qNormCntsEr[nPlanes][nStrips])
{
  for(Int_t p = startPlane; p < endPlane; p++) {
    for (Int_t s =startStrip; s <endStrip; s++) { 
      if (!mask[p][s]) continue;
      qNormCnts[p][s]  = (totCounts[p][s])/((Double_t)(totI/helRate));
      qNormCntsEr[p][s] = (TMath::Sqrt(totCounts[p][s]))/(Double_t)(totI/helRate);
    }
  }
}
//void qNormVariables(Int_t totCountsB1L0[nPlanes][nStrips],Int_t totCountsB1L1[nPlanes][nStrips],Int_t totIL0,Int_t totIL1)
//{
//  for(Int_t p = startPlane; p < endPlane; p++) { 
//    for (Int_t s =startStrip; s <endStrip; s++) {        
//      if (!mask[p][s]) continue;
//      qNormCountsB1L1[p][s]  = (totCountsB1L1[p][s])/((Double_t)(totIL1/helRate));
//      qNormCountsB1L0[p][s]  = (totCountsB1L0[p][s])/((Double_t)(totIL0/helRate));
//      BCqNormBkgdSubAllB1L1[p][s]=qNormCountsB1L1[p][s]-qNormCountsB1L0[p][s];
//      BCqNormBkgdSubAllB1L1Er[p][s] = (TMath::Sqrt(BCqNormBkgdSubAllB1L1[p][s]))/(Double_t)(totIL1/helRate);
//      qNormAllB1L0Er[p][s] = (TMath::Sqrt(totCountsB1L0[p][s]))/(Double_t)(totIL0/helRate);
//
//      tNormYieldB1L1[p][s] = totCountsB1L1[p][s]/((Double_t)(totHelB1L1/helRate));
//      tNormYieldB1L0[p][s] = totCountsB1L0[p][s]/((Double_t)(totHelB1L0/helRate));
//      tNormYieldB1L1Er[p][s] = (TMath::Sqrt(totCountsB1L1[p][s]))/(Double_t)(totHelB1L1/helRate);
//      tNormYieldB1L0Er[p][s] = (TMath::Sqrt(totCountsB1L0[p][s]))/(Double_t)(totHelB1L0/helRate);
//    }
//  }
//}
#endif
