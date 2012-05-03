/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 * As of now, this script only calculates the ratio, and needs more fine tuning
 */

#include <rootClass.h>
#include "comptonRunConstants.h"
#include "asymFit.C"
#include "theoryAsym.C"
//#include "expAsym.C"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, Bool_t first100k=kFALSE)
{
  if(first100k) return;
  cout<<"starting into edetLasCyc.C**************"<<endl;
  //TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);

  ofstream crossSection;    

  expAsym(runnum);

  asymFit(runnum);

  fileReadDraw(runnum);  
}
