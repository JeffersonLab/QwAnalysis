#ifndef __WRITEFORTFILE_F
#define __WRITEFORTFILE_F
////This macro writes out a file output in a format similar to fortran output; but the mistake of division by 4 is not repeated here. Unlike the macro writeFortOut.C, this macro is to be called once at the end of the execution to write out all the counts for the run
#include "rootClass.h"
#include "comptonRunConstants.h"
///This is only intended to write out the files. In the current format, the outqNormL0 and outfilelasOffBkgd should carry the same output physics-wise, for one is the overall average, while other is laserCyc based weighted average respectively
Int_t writeFortFile(TString file, Int_t nCycle,
    //counts
    Double_t cntsB1H1L1[], Double_t cntsB1H1L0[], Double_t cntsB1H0L1[], Double_t cntsB1H0L0[], 
    //charge used
    Double_t qH1L1, Double_t qH1L0, Double_t qH0L1, Double_t qH0L0,
    //charge ignored
    Double_t qIgnoredH1L1, Double_t qIgnoredH1L0, Double_t qIgnoredH0L1, Double_t qIgnoredH0L0,
    //laser power
    Double_t lasPowB1L1, Double_t lasPowB1L0,
    //time
    Double_t tOn, Double_t tOff)
{
  ofstream fOut;
  fOut.open(file);
  cout<<file<<" created"<<endl;

  if (fOut.is_open()) {
    ////default version
    fOut<<Form("%d\t%.2f\t%.2f\n", nCycle+1, lasPowB1L1, lasPowB1L0);
    fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%10.1f\n", qH1L1, qH1L0, qH0L1, qH0L0, qH1L1+qH1L0+qH0L1+qH0L0);
    fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%10.1f\n", qIgnoredH1L1, qIgnoredH1L0, qIgnoredH0L1, qIgnoredH0L0, qIgnoredH1L1+qIgnoredH1L0+qIgnoredH0L1+qIgnoredH0L0);
    fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%10.1f\n", tOn/2, (tOff/2.0), (tOn/2.0), (tOff/2.0), (tOn + tOff));
    for (Int_t s =startStrip; s <endStrip;s++) { 
      fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%2d\n", cntsB1H1L1[s], cntsB1H1L0[s], cntsB1H0L1[s], cntsB1H0L0[s], s+1);
    }
    fOut.close();
  } else cout<<"Couldn't write to file"<<file<<endl;    
  return 1;
}
#endif
