#ifndef __WRITETOFILE_F
#define __WRITETOFILE_F

#include <rootClass.h>
#include "comptonRunConstants.h"

Int_t writeToFile(Int_t runnum,TString dataType)
{
  filePrefix= Form("run_%d/edetLasCyc_%d_%s",runnum,runnum,dataType.Data());
  cout<<blue<<"filePrefix is: "<<filePrefix.Data()<<normal<<endl;
  ofstream outfileExpAsymP,outfileBkgdAsymP,outfileYield,outfilelasOffBkgd,fortranCheck,outScaler;
  for(Int_t p = startPlane; p < endPlane; p++) {
    outfileExpAsymP.open(Form("%s/%s/%sExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfileBkgdAsymP.open(Form("%s/%s/%sBkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfilelasOffBkgd.open(Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    //fortranCheck.open(Form("%s/%s/%sFortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outScaler.open(Form("%s/%s/%sQnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if (outfileExpAsymP.is_open()) {// && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
      cout<<Form("%s/%s/%sExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%sBkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      // cout<<Form("%s/%s/%sfortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%sQnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      //fortranCheck<<totMpsB1H1L1<<"\t"<<totMpsB1H1L0<<"\t"<<totMpsB1H0L1<<"\t"<<totMpsB1H0L0<<"\t"<<totMpsB1H1L1+totMpsB1H1L0+totMpsB1H0L1+totMpsB1H0L0<<endl;
      //fortranCheck<<totIH1L1<<"\t"<<totIH1L0<<"\t"<<totIH0L1<<"\t"<<totIH0L0<<"\t"<<totIH1L1+totIH1L0+totIH0L1+totIH0L0<<endl;
      //outfileYield<<";strip\texpAsymDr\texpAsymDrEr\texpAsymNr"<<endl;
      //outfileExpAsymP<<";strip\texpAsym\tasymEr"<<endl; ///If I want a header for the following text
      Bool_t firstOne=kTRUE;
      for (Int_t s =startStrip; s <endStrip;s++) { 
	if (!mask[p][s]) continue;
	if(!firstOne) {
	  outfileExpAsymP<<"\n";
	  outfileBkgdAsymP<<"\n";
	  outfileYield<<"\n";
	  outfilelasOffBkgd<<"\n";
	  //fortranCheck<<"\n";
	  outScaler<<"\n";
	}
	firstOne =kFALSE;
	outfileExpAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,stripAsym[p][s],stripAsymEr[p][s]);
	outfileBkgdAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,bkgdAsym[p][s],bkgdAsymEr[p][s]);
	outfileYield<<Form("%2.0f\t%g\t%g\t%g",(Double_t)s+1,stripAsymDr[p][s],stripAsymDrEr[p][s],stripAsymNr[p][s]);//has all experimental asym components
	outfilelasOffBkgd<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormB1L0[p][s],qNormB1L0Er[p][s]);
	//fortranCheck<<Form("%2.0f\t%d\t%d\t%d\t%d",(Double_t)s+1,totAccumB1H1L1[p][s],totAccumB1H1L0[p][s],totAccumB1H0L1[p][s],totAccumB1H0L0[p][s]);
	outScaler<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormCountsB1L1[p][s],qNormCountsB1L0[p][s]);
      }
      outfileExpAsymP.close();
      outfileBkgdAsymP.close();
      outfileYield.close();
      outfilelasOffBkgd.close();
      //fortranCheck.close();
      outScaler.close();
      cout<<Form("%s/%s/%sExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%sBkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      //cout<<Form("%s/%s/%sfortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%sQnormCountsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
    } else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values\n\n"<<endl;    
  }
  return runnum;
}
#endif
