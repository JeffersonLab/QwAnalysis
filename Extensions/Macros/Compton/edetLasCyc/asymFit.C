//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry

#include <rootClass.h>
#include "theoryAsym.C"
#include "expAsym.C"
#include "comptonRunConstants.h"

void asymFit(Int_t runnum, Float_t expTheoRatio[nPlanes][nStrips],Float_t stripAsymEr[nPlanes][nStrips])
{
  TString filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  TString readEntry;
  Bool_t debug=1;
  Float_t calcAsym[nStrips],stripNum[nStrips];
  Float_t stripAsym[nPlanes][nStrips];
  Float_t stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
  ifstream theoAsym;
  ofstream expTheoRatioFile;
  //  Double_t par[4];
  Int_t count=Cedge;

//   theoryAsym(Cedge,par);
  theoryAsym(Cedge);
  theoAsym.open(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge));
  if (theoAsym.is_open()) {
    if(debug) cout<<"stripNum\t"<<"calcAsym"<<endl;
    while(theoAsym.good()) {
      theoAsym>>stripNum[count]>>calcAsym[count];
      //       theoAsym>> readEntry; //read the contents of the line in a string first
      //       if (readEntry.IsDigit()) { //check if the string is a combination of numbers of not
      // 	readEntry>>stripNum[count]>>calcAsym[count];//!the "-1" is put to keep consistency with all other arrays
      if(debug) cout<<stripNum[count]<<"\t"<<calcAsym[count]<<endl;
      count--;
    }
    theoAsym.close();
  } 
  else cout<<"\n***Error:Could not find the file "<<Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,Cedge)<<endl;
  
//   printf("\nthe parameters received from theoretical fitting are par[0]:%f,par[1]:%f,par[2]:%f,par[3]:%f\n\n" ,par[0],par[1],par[2],par[3]);
//   TF1 *fn2 = new TF1("fn2",Form("[0] + [1]*(%f + %f*x + %f*x*x + %f*x*x*x)",par[0],par[1],par[2],par[3]),10,Cedge);
//   fn2->SetParameters(0.9,0);
//   fn2->SetParLimits(0,-1,1);
//   fn2->SetParLimits(1,2,-2);

  expAsym(runnum,stripAsym,stripAsymEr,stripAsym_v2,stripAsymEr_v2);
  
  if(debug) cout<<"\nexpTheoRatio\tstripAsym\tcalcAsym"<<endl;
  for (Int_t p =startPlane; p <endPlane; p++) {  
    expTheoRatioFile.open(Form("%s/%s/%sexpTheoRatioP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if (expTheoRatioFile.is_open()) {
      cout<<"created file for writing ratio of exp to theory asymmetry for plane "<<p+1<<endl;
      for (Int_t s =startStrip; s <= Cedge; s++) {  //!calcAsym is assigned only upto Cedge
	if (maskedStrips(p,s)) continue;
	expTheoRatio[p][s]= stripAsym[p][s]/calcAsym[s];
	expTheoRatioFile<<Form("%2.0f\t%f\t%f\n",(Float_t)s+1,expTheoRatio[p][s],stripAsymEr[p][s]);
	if(debug) printf("expTheoRatio[%d][%d]:%f = %f / %e\n",p,s,expTheoRatio[p][s],stripAsym[p][s],calcAsym[s]);
	//if(p==0) printf("%d\t%f\t%f\t%f\t%e\n",s,stripAsym[0][s],stripAsymEr[0][s],calcAsym[s],expTheoRatio[0][s]);
      }
      expTheoRatioFile.close();
      cout<<"filled successfully "<<Form("%s/%s/%sexpTheoRatioP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;
    }
    else cout<<"could not open a file to write expTheoRatio"<<endl;  
  }
}
/*Comments
 *I'm currently including my rootClass.h and comptonRunConstants.h separately in every file
 *..there must be a way to do it non-repetatively.
 *I am doing this for all strips, I should limit myself to 64 strips at max
 */
