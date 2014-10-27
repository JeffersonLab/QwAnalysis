#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "aggregateRate.C"

const Int_t runBegin = 22737, runEnd = 25527;///whole run2 range

Int_t aggAllRun2(Int_t run1=runBegin, Int_t run2 = runEnd)
{
  for(int i =run1; i <= run2; i++) {
    aggregateRate(i, "Sc", "tNormYieldB1L0P1.txt");
  } 

  return (run2 - run1);
}

/*********Selfie************
 * This macro inputs a file name and assuming it has 3 columns
 * reads in all columns, finds the aggregate of col2 and adds
 * the values in col3 in quadrature taking them as the error.
 * Note: a masked strip will not be a problem in this addition
 * This will be used to get aggregate Yield from
 * edetLC_24519_ActNormYieldB1L1P1.txt type files
 ***************************/
