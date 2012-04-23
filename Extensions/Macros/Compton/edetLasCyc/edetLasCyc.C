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
  cout<<"starting into edetLasCyc.C**************"<<endl;
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  if(first100k) return;
  //  Float_t stripAsymEr[nPlanes][nStrips];//,expTheoRatio[nPlanes][nStrips];
  //  Float_t stripAsym[nPlanes][nStrips];   
  // Float_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
  //  Bool_t debug=0;
  ofstream crossSection;    

  expAsym(runnum,first100k);//,stripAsym,stripAsymEr,stripAsym_v2,stripAsymEr_v2);
  //expAsym function also sets the compton edge (Cedge) which is a global variable
  //hence it can be simply used by theoryAsym function below

//   for(Int_t p = startPlane; p < endPlane; p++) {
//     Cedge = identifyCedgeforPlane(p,stripAsymEr);//!still in test mode
  theoryAsym(Cedge); //function call to theoretical rho-to-x file generation
//   }

  asymFit(runnum);

  fileReadDraw(runnum);  
}
