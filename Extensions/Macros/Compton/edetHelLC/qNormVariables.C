#ifndef __QNORMVARIABLES_F
#define __QNORMVARIABLES_F

#include <rootClass.h>
#include "comptonRunConstants.h"

void qNormVariables(Int_t totCountsB1L0[nPlanes][nStrips],Int_t totCountsB1L1[nPlanes][nStrips],Int_t totIL0,Int_t totIL1)
{
  for(Int_t p = startPlane; p < endPlane; p++) { 
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (!mask[p][s]) continue;
      qNormCountsB1L1[p][s]  = (totCountsB1L1[p][s])/((Double_t)(totIL1/helRate));
      qNormCountsB1L0[p][s]  = (totCountsB1L0[p][s])/((Double_t)(totIL0/helRate));
    }
  }
}
#endif
