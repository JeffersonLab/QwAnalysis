#ifndef __WRITETOFILE_F
#define __WRITETOFILE_F

#include "rootClass.h"
#include "comptonRunConstants.h"
///This is only intended to write out the files. In the current format, the outqNormL0 and outfilelasOffBkgd should carry the same output physics-wise, for one is the overall average, while other is laserCyc based weighted average respectively
Int_t writeToFile(Int_t runnum,TString dataType)
{
  filePre = Form(filePrefix+"%s",runnum,runnum,dataType.Data());
  cout<<blue<<"in writeToFile macro filePre.Data() is: "<<filePre.Data()<<normal<<endl;
  ofstream outfileExpAsymP,outfileBkgdAsymP,outfileYield,outfilelasOffBkgd,fortranCheck,outScaler,outtNormYieldB1L0,outtNormYieldB1L1,outqNormL0;
  outfileExpAsymP.open(Form("%s/%s/%sExpAsymP%d.txt",pPath,www,filePre.Data(),plane));
  outfileBkgdAsymP.open(Form("%s/%s/%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),plane));
  outfileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,www,filePre.Data(),plane));
  outfilelasOffBkgd.open(Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,www,filePre.Data(),plane));
  fortranCheck.open(Form("%s/%s/%sFortranCheckP%d.txt",pPath,www,filePre.Data(),plane));
  outScaler.open(Form("%s/%s/%sqNormCntsB1L1P%d.txt",pPath,www,filePre.Data(),plane));
  outqNormL0.open(Form("%s/%s/%sqNormCntsB1L0P%d.txt",pPath,www,filePre.Data(),plane));
  outtNormYieldB1L1.open(Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath,www,filePre.Data(),plane));
  outtNormYieldB1L0.open(Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,www,filePre.Data(),plane));
  if (outfileExpAsymP.is_open()) {// && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
    //cout<<Form("%s/%s/%sExpAsymP%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%sYieldP%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%sFortranCheckP%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%sqNormCntsB1L1P%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%sqNormCntsB1L0P%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    //cout<<Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,www,filePre.Data(),plane)<<" file created"<<endl;
    fortranCheck<<Form("%f\t%f\t%f\t%f\t%f",totIAllH1L1,totIAllH1L0,totIAllH0L1,totIAllH0L0,totIAllH1L1+totIAllH1L0+totIAllH0L1+totIAllH0L0)<<endl;;
    fortranCheck<<Form("%f\t%f\t%f",totHelB1L1/helRate,totHelB1L0/helRate,(totHelB1L1+totHelB1L0)/helRate)<<endl;
    //outfileYield<<";strip\texpAsymDr\texpAsymDrEr\texpAsymNr"<<endl;
    //outfileExpAsymP<<";strip\texpAsym\tasymEr"<<endl; ///If I want a header for the following text
    for (Int_t s =startStrip; s <endStrip;s++) { 
      outfileExpAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,stripAsym[s],stripAsymEr[s])<<endl;
      outfileBkgdAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,bkgdAsym[s],bkgdAsymEr[s])<<endl;
      outfileYield<<Form("%2.0f\t%g\t%g\t%g",(Double_t)s+1,stripAsymDr[s],stripAsymDrEr[s],stripAsymNr[s])<<endl;//has all expt asym components
      outfilelasOffBkgd<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormB1L0[s],qNormB1L0Er[s])<<endl;
      fortranCheck<<Form("%d\t%f\t%f\t%f\t%f",s+1,totyieldB1H1L1[s],totyieldB1H1L0[s],totyieldB1H0L1[s],totyieldB1H0L0[s])<<endl;
      outScaler <<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormCountsB1L1[s],qNormCountsB1L1Er[s])<<endl;
      outqNormL0<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormCountsB1L0[s],qNormCountsB1L0Er[s])<<endl;
      outtNormYieldB1L1<<Form("%2.0f\t%g\t%g",(Double_t)s+1,tNormYieldB1L1[s],tNormYieldB1L1Er[s])<<endl;
      outtNormYieldB1L0<<Form("%2.0f\t%g\t%g",(Double_t)s+1,tNormYieldB1L0[s],tNormYieldB1L0Er[s])<<endl;
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
    //cout<<Form("%s/%s/%sExpAsymP%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%sYieldP%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%sFortranCheckP%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%sqNormCntsB1L1P%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%sqNormCntsB1L0P%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
    //cout<<Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,www,filePre.Data(),plane)<<" filled and closed"<<endl;
  } else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values\n\n"<<endl;    
  return runnum;
  }
#endif
