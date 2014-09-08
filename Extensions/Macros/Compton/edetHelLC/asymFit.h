#ifndef ASYMFIT_H
#define ASYMFIT_H

#include "rootbasic.h"
#include <TString.h>

////////////////////////////////////////////////////////////////////////
// Pre-define all the functions

// 3 Paramter fit for the cross section
Double_t theoCrossSec(Double_t *thisStrip, Double_t *parCx);

// 3 Parameter method for asymmetry
Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par);

// What does this do Amar?
Int_t asymFit(Int_t runnum=24519,TString dataType=TString("Ac"));

#endif // ASYMFIT_H
