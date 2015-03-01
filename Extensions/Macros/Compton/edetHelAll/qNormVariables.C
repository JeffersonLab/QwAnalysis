#ifndef __QNORMVARIABLES_F
#define __QNORMVARIABLES_F

#include "rootClass.h"
#include "comptonRunConstants.h"
///This routine takes in a variable and the corresponding charge/time to give the corresponding normalized counts and the associated error. it should work for both time as well as charge normalization. Instead of the earlier format, it will now be repeatedly called for every instance of a variable to be normalized.
void qNormVariables(Double_t totCounts[nStrips], Double_t totQ, Double_t qNormCnts[nStrips], Double_t qNormCntsEr[nStrips])
{
  for (Int_t s =startStrip; s <endStrip; s++) { 
    qNormCnts[s]  = (totCounts[s])/((Double_t)(totQ));
    qNormCntsEr[s] = (TMath::Sqrt(totCounts[s]))/(Double_t)(totQ);
  }
}
//void qNormVariables(Int_t totCountsB1L0[nStrips],Int_t totCountsB1L1[nStrips],Int_t totIL0,Int_t totIL1)
//{
//    for (Int_t s =startStrip; s <endStrip; s++) {        
//      qNormCountsB1L1[s]  = (totCountsB1L1[s])/((Double_t)(totIL1/helRate));
//      qNormCountsB1L0[s]  = (totCountsB1L0[s])/((Double_t)(totIL0/helRate));
//      BCqNormBkgdSubAllB1L1[s]=qNormCountsB1L1[s]-qNormCountsB1L0[s];
//      BCqNormBkgdSubAllB1L1Er[s] = (TMath::Sqrt(BCqNormBkgdSubAllB1L1[s]))/(Double_t)(totIL1/helRate);
//      qNormAllB1L0Er[s] = (TMath::Sqrt(totCountsB1L0[s]))/(Double_t)(totIL0/helRate);
//
//      tNormYieldB1L1[s] = totCountsB1L1[s]/((Double_t)(totHelB1L1/helRate));
//      tNormYieldB1L0[s] = totCountsB1L0[s]/((Double_t)(totHelB1L0/helRate));
//      tNormYieldB1L1Er[s] = (TMath::Sqrt(totCountsB1L1[s]))/(Double_t)(totHelB1L1/helRate);
//      tNormYieldB1L0Er[s] = (TMath::Sqrt(totCountsB1L0[s]))/(Double_t)(totHelB1L0/helRate);
//    }
//}
#endif
