#ifndef __WRITETOFILE_F
#define __WRITETOFILE_F

#include <rootClass.h>
#include "comptonRunConstants.h"
///This is only intended to write out the files. In the current format, the outqNormL0 and outfilelasOffBkgd should carry the same output physics-wise, for one is the overall average, while other is laserCyc based weighted average respectively
Int_t writeToFile(Int_t runnum,TString dataType)
{
  filePrefix= Form("run_%d/edetLasCyc_%d_%s",runnum,runnum,dataType.Data());
  cout<<blue<<"filePrefix is: "<<filePrefix.Data()<<normal<<endl;
  ofstream outfileExpAsymP,outfileBkgdAsymP,outfileYield,outfilelasOffBkgd,fortranCheck,outScaler,outtNormYieldB1L0,outtNormYieldB1L1,outqNormL0;
  for(Int_t p = startPlane; p < endPlane; p++) {
    outfileExpAsymP.open(Form("%s/%s/%sExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfileBkgdAsymP.open(Form("%s/%s/%sBkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfilelasOffBkgd.open(Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    fortranCheck.open(Form("%s/%s/%sFortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outScaler.open(Form("%s/%s/%sqNormCntsB1L1P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outqNormL0.open(Form("%s/%s/%sqNormCntsB1L0P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outtNormYieldB1L1.open(Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outtNormYieldB1L0.open(Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if (outfileExpAsymP.is_open()) {// && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
      //cout<<Form("%s/%s/%sExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%sBkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%sFortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%sqNormCntsB1L1P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%sqNormCntsB1L0P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //cout<<Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      fortranCheck<<Form("%f\t%f\t%f\t%f\t%f\n",totIAllH1L1,totIAllH1L0,totIAllH0L1,totIAllH0L0,totIAllH1L1+totIAllH1L0+totIAllH0L1+totIAllH0L0);
      fortranCheck<<";\n";
      fortranCheck<<Form("%f\t%f\t%f\n",totHelB1L1/helRate,totHelB1L0/helRate,(totHelB1L1+totHelB1L0)/helRate);
      //outfileYield<<";strip\texpAsymDr\texpAsymDrEr\texpAsymNr"<<endl;
      //outfileExpAsymP<<";strip\texpAsym\tasymEr"<<endl; ///If I want a header for the following text
      Bool_t firstOne=kTRUE;
      for (Int_t s =startStrip; s <endStrip;s++) { 
        //if (!mask[p][s]) continue;
        if(!firstOne) {
          outfileExpAsymP<<"\n";
          outfileBkgdAsymP<<"\n";
          outfileYield<<"\n";
          outfilelasOffBkgd<<"\n";
          fortranCheck<<"\n";
          outScaler<<"\n";
          outqNormL0<<"\n";
          outtNormYieldB1L1<<"\n";
          outtNormYieldB1L0<<"\n";
        }
        firstOne =kFALSE;
        outfileExpAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,stripAsym[p][s],stripAsymEr[p][s]);
        outfileBkgdAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,bkgdAsym[p][s],bkgdAsymEr[p][s]);
        outfileYield<<Form("%2.0f\t%g\t%g\t%g",(Double_t)s+1,stripAsymDr[p][s],stripAsymDrEr[p][s],stripAsymNr[p][s]);//has all expt asym components
        outfilelasOffBkgd<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormB1L0[p][s],qNormB1L0Er[p][s]);
        fortranCheck<<Form("%d\t%d\t%d\t%d\t%d",s+1,totyieldB1H1L1[p][s],totyieldB1H1L0[p][s],totyieldB1H0L1[p][s],totyieldB1H0L0[p][s]);
        outScaler <<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormCountsB1L1[p][s],qNormCountsB1L1Er[p][s]);
        outqNormL0<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormCountsB1L0[p][s],qNormCountsB1L0Er[p][s]);
        outtNormYieldB1L1<<Form("%2.0f\t%g\t%g",(Double_t)s+1,tNormYieldB1L1[p][s],tNormYieldB1L1Er[p][s]);
        outtNormYieldB1L0<<Form("%2.0f\t%g\t%g",(Double_t)s+1,tNormYieldB1L0[p][s],tNormYieldB1L0Er[p][s]);
      }
      outfileExpAsymP.close();
      outfileBkgdAsymP.close();
      outfileYield.close();
      outfilelasOffBkgd.close();
      fortranCheck.close();
      outScaler.close();
      outqNormL0.close();
      outtNormYieldB1L1.close();
      outtNormYieldB1L0.close();
      //cout<<Form("%s/%s/%sExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sBkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sFortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sqNormCntsB1L1P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sqNormCntsB1L0P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
    } else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values\n\n"<<endl;    
    }
    return runnum;
  }
#endif
