#ifndef __WRITETOFILE_F
#define __WRITETOFILE_F
////This macro writes out a file output in the same format as the fortran output is
#include "rootClass.h"
#include "comptonRunConstants.h"
///This is only intended to write out the files. In the current format, the outqNormL0 and outfilelasOffBkgd should carry the same output physics-wise, for one is the overall average, while other is laserCyc based weighted average respectively
Int_t writeToFile(TString file)
{
  ofstream fOut;
  fOut.open(file);
  if (fOut.is_open()) {
    cout<<file<<" created"<<endl;
    ///the fortran version has charge scalers so this won't match
    fOut<<Form("%.1f\t%.1f\t%.1f\t%.1f\t%.1f\n",qAllH1L1,qAllH1L0,qAllH0L1,qAllH0L0,qAllH1L1+qAllH1L0+qAllH0L1+qAllH0L0);
    fOut<<Form("%.1f\t%.1f\t%.1f\t%.1f\t%.1f\n",qIgnoredH1L1,qIgnoredH1L0,qIgnoredH0L1,qIgnoredH0L0,qIgnoredH1L1+qIgnoredH1L0+qIgnoredH0L1+qIgnoredH0L0);
    fOut<<Form("%.1f\t%.1f\t%.1f\t%.1f\t%.1f\n", (totHelB1H1L1)/helRate, (totHelB1H1L0)/helRate, (totHelB1H0L1)/helRate, (totHelB1H0L0)/helRate, (totHelB1H1L1 + totHelB1H1L0 + totHelB1H0L1 + totHelB1H0L0) / helRate);
    for (Int_t s =startStrip; s <endStrip;s++) { 
      fOut<<Form("%.1f\t%.1f\t%.1f\t%.1f\t%2d\n",totyieldB1H1L1[s]/4,totyieldB1H1L0[s]/4,totyieldB1H0L1[s]/4,totyieldB1H0L0[s]/4,s+1);///The '/4' is simply to easily compare the counts in fortran
    }
    fOut.close();
  } else cout<<"Couldn't write to file"<<file<<endl;    
  return 1;
  }
#endif
