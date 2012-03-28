/* This is the main file that calls subsequent scripts in the process of evaluating polarization.
 * As of now, this script only calculates the ratio, and needs more fine tuning
 */

#include <rootClass.h>
#include "asymFit.C"
#include "comptonRunConstants.h"
#include "fileReadDraw.C"

void edetLasCyc(Int_t runnum, Bool_t first100k=kFALSE)
{
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  if(first100k) return;
  Float_t expTheoRatio[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
//  Float_t zero[nStrips],stripPlot[nStrips];
//   for (Int_t s=0; s <endStrip; s++) { 
//     stripPlot[s]=s+1;
//     zero[s]=0;
//   }                                                                

  asymFit(runnum,expTheoRatio,stripAsymEr);

  fileReadDraw(runnum);  
}
  
/*****The execution tree is as follows:
 *edetLasCyc.C
 *-- asymFit.C ..................................... fileReadDraw.C
 *----theoryAsym.C, edetExpAsym.C , maskedStrips.C
 *..................---getEBeamLasCuts.C
 *
 *asymFit.C processes all physics routines to evaluate the concerned quantities
 *fileReadDraw.C draws the main results
 ****************/
