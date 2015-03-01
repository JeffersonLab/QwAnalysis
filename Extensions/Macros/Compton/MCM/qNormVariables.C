#ifndef __QNORMVARIABLES_F
#define __QNORMVARIABLES_F

#include "rootClass.h"
#include "comptonRunConstants.h"
///This routine takes in a variable and the corresponding charge/time to give the corresponding normalized counts and the associated error. it should work for both time as well as charge normalization. Instead of the earlier format, it will now be repeatedly called for every instance of a variable to be normalized.
void qNormVariables(
    //input
    Double_t totCounts[nStrips], Double_t totQ
    //output
    , Double_t qNormCnts[nStrips], Double_t qNormCntsEr[nStrips])
{
  for (Int_t s =startStrip; s <endStrip; s++) { 
    qNormCnts[s]  = (totCounts[s])/(totQ);
    qNormCntsEr[s] = (TMath::Sqrt(totCounts[s]))/(totQ);
  }
}
#endif
