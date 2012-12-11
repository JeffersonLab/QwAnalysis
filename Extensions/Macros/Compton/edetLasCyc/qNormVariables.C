#ifndef __QNORMVARIABLES_F
#define __QNORMVARIABLES_F

#include <rootClass.h>
#include "comptonRunConstants.h"

void qNormVariables(Int_t totCountsB1H0L0[nPlanes][nStrips],Int_t totCountsB1H0L1[nPlanes][nStrips],Int_t totCountsB1H1L0[nPlanes][nStrips],Int_t totCountsB1H1L1[nPlanes][nStrips],Int_t totIH0L0,Int_t totIH0L1,Int_t totIH1L0,Int_t totIH1L1)
{
  for(Int_t p = startPlane; p < endPlane; p++) { 
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (!mask[p][s]) continue;
      qNormCountsB1L1[p][s]  = (totCountsB1H1L1[p][s] + totCountsB1H0L1[p][s])/((Double_t)(totIH1L1 + totIH0L1)/MpsRate);
      qNormCountsB1L0[p][s]  = (totCountsB1H1L0[p][s] + totCountsB1H0L0[p][s])/((Double_t)(totIH1L0 + totIH0L0)/MpsRate);
    }
  }
}
#endif
