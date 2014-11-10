#ifndef __WRITETOFILE_F
#define __WRITETOFILE_F
////This macro writes out a file output in the same format as the fortran output is
#include "rootClass.h"
#include "comptonRunConstants.h"
///This is only intended to write out the files. In the current format, the outqNormL0 and outfilelasOffBkgd should carry the same output physics-wise, for one is the overall average, while other is laserCyc based weighted average respectively
Int_t writeFortOut(TString file, Int_t nCycle,
    //counts
    Double_t cntsB1H1L1[], Double_t cntsB1H1L0[], Double_t cntsB1H0L1[], Double_t cntsB1H0L0[], 
    //charge
    Double_t qH1L1, Double_t qH1L0, Double_t qH0L1, Double_t qH0L0,
    //laser power
    Double_t lasPowB1L1, Double_t lasPowB1L0,
    //time
    Int_t nHelB1L1, Int_t nHelB1L0)
{
  ofstream fOut;
  if(nCycle==0) fOut.open(file);
  else fOut.open(file, std::ofstream::app);
  if (fOut.is_open()) {
    cout<<file<<" created"<<endl;
    ///the fortran version has charge scalers so this won't match
    fOut<<Form("%d\t%.2f\t%.2f\n", nCycle+1, lasPowB1L1, lasPowB1L0);
    fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%10.1f\n", qH1L1, qH1L0, qH0L1, qH0L0, qH1L1+qH1L0+qH0L1+qH0L0);
    //fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%10.1f\n",qIgnoredH1L1,qIgnoredH1L0,qIgnoredH0L1,qIgnoredH0L0,qIgnoredH1L1+qIgnoredH1L0+qIgnoredH0L1+qIgnoredH0L0);
    fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%10.1f\n", (nHelB1L1/2.0)/helRate, (nHelB1L0/2.0)/helRate, (nHelB1L1/2.0)/helRate, (nHelB1L0/2.0)/helRate, (nHelB1L1 + nHelB1L0) / helRate);
    for (Int_t s =startStrip; s <endStrip;s++) { 
      //fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%2d\n",totyieldB1H1L1[s]/4,totyieldB1H1L0[s]/4,totyieldB1H0L1[s]/4,totyieldB1H0L0[s]/4,s+1);///The '/4' is simply to easily compare the counts in fortran
      fOut<<Form("%10.1f\t%10.1f\t%10.1f\t%10.1f\t%2d\n", cntsB1H1L1[s], cntsB1H1L0[s], cntsB1H0L1[s], cntsB1H0L0[s], s+1);
    }
    fOut.close();
  } else cout<<"Couldn't write to file"<<file<<endl;    
  return 1;
  }
#endif
