#include <rootClass.h>
#include "comptonRunConstants.h"
#include "rhoToX.C"

void evaluateAsym(Double_t diffB1L1[nPlanes][nStrips],Double_t diffB1L0[nPlanes][nStrips],Double_t yieldB1L1[nPlanes][nStrips],Double_t yieldB1L0[nPlanes][nStrips],Double_t bcmLCH1L1,Double_t bcmLCH1L0,Double_t bcmLCH0L1,Double_t bcmLCH0L0,Double_t wmNrAsym[nPlanes][nStrips],Double_t wmDrAsym[nPlanes][nStrips],Double_t wmNrBCqNormSum[nPlanes][nStrips],Double_t wmDrBCqNormSum[nPlanes][nStrips],Double_t wmNrBCqNormDiff[nPlanes][nStrips],Double_t wmNrqNormB1L0[nPlanes][nStrips],Double_t wmDrqNormB1L0[nPlanes][nStrips],Double_t wmNrBkgdAsym[nPlanes][nStrips],Double_t wmDrBkgdAsym[nPlanes][nStrips],Double_t qNormLasCycAsym[nPlanes][nStrips],Double_t LasCycAsymErSqr[nPlanes][nStrips])
{
  cout<<"starting into evaluateAsym.C**************"<<endl;
  const Bool_t debug=0,debug1=0;
  Double_t countsLCB1H1L1=0.0,countsLCB1H1L0=0.0,countsLCB1H0L1=0.0,countsLCB1H0L0=0.0;
  Double_t qNormCountsLCB1H1L1=0.0,qNormCountsLCB1H1L0=0.0,qNormCountsLCB1H0L1=0.0,qNormCountsLCB1H0L0=0.0;
  Double_t BCqNormAcB1H1L1LasCyc=0.0,BCqNormAcB1H0L1LasCyc=0.0;
  Double_t BCqNormLasCycDiff=0.0,BCqNormLasCycSum=0.0;
  Double_t term1=0.0,term2=0.0,NplusOn_SqQplusOn=0.0,NminusOn_SqQminusOn=0.0,NplusOff_SqQplusOff=0.0,NminusOff_SqQminusOff=0.0;
  Double_t errB1H1L1=0.0, errB1H0L1=0.0, errB1H1L0=0.0, errB1H0L0=0.0;
  Double_t qNormAcBkgdAsymNr=0.0,qNormAcBkgdAsymDr=0.0,qNormAcBkgdAsym=0.0,term1Bkgd=0.0,term2Bkgd=0.0,errBkgdAsymH1=0.0,errBkgdAsymH0=0.0,bkgdAsymErSqr=0.0;
  Double_t erBCqNormLasCycSumSq=0.0,erqNormB1L0LasCycSq=0.0;
  Double_t qAvgLCH1L1 = bcmLCH1L1 /(helRate);
  Double_t qAvgLCH1L0 = bcmLCH1L0 /(helRate);
  Double_t qAvgLCH0L1 = bcmLCH0L1 /(helRate);
  Double_t qAvgLCH0L0 = bcmLCH0L0 /(helRate);

  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      if (!mask[p][s]) continue;
      ///a factor of 4 is needed to balance out the averaging in the yield and diff reported in the helicity tree
      countsLCB1H1L1=2.0*(yieldB1L1[p][s] + diffB1L1[p][s]);///this is now the true total counts in this laser cycle durng H1,L1
      countsLCB1H1L0=2.0*(yieldB1L0[p][s] + diffB1L0[p][s]);
      countsLCB1H0L1=2.0*(yieldB1L1[p][s] - diffB1L1[p][s]);
      countsLCB1H0L0=2.0*(yieldB1L0[p][s] - diffB1L0[p][s]);
      totyieldB1H1L1[p][s] += countsLCB1H1L1;
      totyieldB1H1L0[p][s] += countsLCB1H1L0;
      totyieldB1H0L1[p][s] += countsLCB1H0L1;
      totyieldB1H0L0[p][s] += countsLCB1H0L0;

      qNormCountsLCB1H1L1=countsLCB1H1L1/qAvgLCH1L1;
      qNormCountsLCB1H1L0=countsLCB1H1L0/qAvgLCH1L0;
      qNormCountsLCB1H0L1=countsLCB1H0L1/qAvgLCH0L1;
      qNormCountsLCB1H0L0=countsLCB1H0L0/qAvgLCH0L0;

      BCqNormAcB1H1L1LasCyc= qNormCountsLCB1H1L1 - qNormCountsLCB1H1L0;
      BCqNormAcB1H0L1LasCyc= qNormCountsLCB1H0L1 - qNormCountsLCB1H0L0;
      //Double_t BCqNormLasCycDiff = (BCqNormAcB1H1L1LasCyc - BCqNormAcB1H0L1LasCyc);//I could use this variable without explicit background correction because in difference, this is automatically taken care
      BCqNormLasCycDiff = (BCqNormAcB1H1L1LasCyc - BCqNormAcB1H0L1LasCyc);
      BCqNormLasCycSum  = (BCqNormAcB1H1L1LasCyc + BCqNormAcB1H0L1LasCyc);
      qNormLasCycAsym[p][s] = (BCqNormLasCycDiff / BCqNormLasCycSum);

      ///Evaluation of error on asymmetry; partitioned the evaluation in a way which avoids re-calculation
      term1 = (1.0-qNormLasCycAsym[p][s])/BCqNormLasCycSum;
      term2 = (1.0+qNormLasCycAsym[p][s])/BCqNormLasCycSum;
      NplusOn_SqQplusOn    = qNormCountsLCB1H1L1 /qAvgLCH1L1;
      NminusOn_SqQminusOn  = qNormCountsLCB1H0L1 /qAvgLCH0L1;
      NplusOff_SqQplusOff  = qNormCountsLCB1H1L0 /qAvgLCH1L0;
      NminusOff_SqQminusOff= qNormCountsLCB1H0L0 /qAvgLCH0L0;

      ///redefining these error variables 
      errB1H1L1 = pow(term1,2) * NplusOn_SqQplusOn; 
      errB1H0L1 = pow(term2,2) * NminusOn_SqQminusOn;
      errB1H1L0 = pow(term1,2) * NplusOff_SqQplusOff;
      errB1H0L0 = pow(term2,2) * NminusOff_SqQminusOff; 

      LasCycAsymErSqr[p][s] = (errB1H1L1 + errB1H0L1 + errB1H1L0 + errB1H0L0);

      ///adding asymmetry evaluation for laser off data
      qNormAcBkgdAsymNr = qNormCountsLCB1H1L0 - qNormCountsLCB1H0L0;
      qNormAcBkgdAsymDr = qNormCountsLCB1H1L0 + qNormCountsLCB1H0L0;
      qNormAcBkgdAsym = qNormAcBkgdAsymNr/qNormAcBkgdAsymDr;
      term1Bkgd = (1-qNormAcBkgdAsym)/qNormAcBkgdAsymDr;
      term2Bkgd = (1+qNormAcBkgdAsym)/qNormAcBkgdAsymDr;

      errBkgdAsymH1 = pow(term1Bkgd,2) * NplusOff_SqQplusOff;
      errBkgdAsymH0 = pow(term2Bkgd,2) * NminusOff_SqQminusOff;
      bkgdAsymErSqr = errBkgdAsymH1 + errBkgdAsymH0;
      
      if (LasCycAsymErSqr[p][s] >0.0) {///eqn 4.17(Bevington)
	wmNrAsym[p][s] += qNormLasCycAsym[p][s]/LasCycAsymErSqr[p][s]; ///Numerator 
	wmDrAsym[p][s] += 1.0/LasCycAsymErSqr[p][s]; ///Denominator 
	if (debug1) printf("*****adding %g(1/asymSq) to wmDrAsym making it: %f\n",1.0/LasCycAsymErSqr[p][s],wmDrAsym[p][s]);
      } else {
	cout<<"check if plane "<<p+1<<" strip "<<s+1<<" is MASKED? It gives non-positive Asym Er"<<endl;
	printf("errB1H1L1:%f, errB1H0L1:%f, errB1H1L0:%f, errB1H0L0:%f\n",errB1H1L1,errB1H0L1,errB1H1L0,errB1H0L0);
      }
      ///Error evaluation for SUM (in asymmetry)
      erBCqNormLasCycSumSq = (NplusOn_SqQplusOn+ NminusOn_SqQminusOn+ NplusOff_SqQplusOff+ NminusOff_SqQminusOff);
      ///Error of laser off background for every laser cycle
      erqNormB1L0LasCycSq = (NplusOff_SqQplusOff+NminusOff_SqQminusOff);

      ///addition of bkgdAsym term over various laser cycles
      if (bkgdAsymErSqr > 0.0) {///eqn 4.17(Bevington)
	wmNrBkgdAsym[p][s] += qNormAcBkgdAsym/bkgdAsymErSqr; ///Numerator 
	wmDrBkgdAsym[p][s] += 1.0/bkgdAsymErSqr; ///Denominator 
	if (debug) printf("*****adding %g(1/bkgdAsymSq) to wmDrBkgdAsym making it: %f\n",1.0/bkgdAsymErSqr,wmDrBkgdAsym[p][s]);
      } else {
	cout<<"check if plane "<<p+1<<" strip "<<s+1<<" is MASKED? the bkgdAsymEr is non-positive"<<endl;
	printf("errBkgdAsymH1:%f, errBkgdAsymH1:%f\n",errBkgdAsymH1,errBkgdAsymH0);
      }


      if (erBCqNormLasCycSumSq >0.0) {
	wmNrBCqNormSum[p][s] += BCqNormLasCycSum/erBCqNormLasCycSumSq; ///Numerator eqn 4.17(Bevington)
	wmDrBCqNormSum[p][s] += 1.0/erBCqNormLasCycSumSq; ///Denominator eqn 4.17(Bevington)
	///The error for the difference and sum are same, hence reusing the variable
	wmNrBCqNormDiff[p][s] += BCqNormLasCycDiff/erBCqNormLasCycSumSq; ///Numerator eqn 4.17(Bevington)
	//wmDrBCqNormDiff[p][s] += 1.0/erBCqNormLasCycSumSq[p][s]; ///Denominator eqn 4.17(Bevington)
	wmNrqNormB1L0[p][s] += qNormAcBkgdAsymDr/erqNormB1L0LasCycSq; //refer bevington
	wmDrqNormB1L0[p][s] += 1.0/erqNormB1L0LasCycSq; //refer bevington
      } else {
	printf("**Alert: getting non-positive erBCqNormLasCycSumSq for plane:%d, strip:%d in line:%d\n",p+1,s+1,__LINE__);
	printf("NplusOn_SqQplusOn:%g, NminusOn_SqQminusOn:%g, NplusOff_SqQplusOff:%g, NminusOff_SqQminusOff:%g,\n"
	       ,NplusOn_SqQplusOn,NminusOn_SqQminusOn,NplusOff_SqQplusOff,NminusOff_SqQminusOff);
      }
    }//for (Int_t s =startStrip; s <endStrip; s++) {	
  }///for (Int_t p =startPlane; p <endPlane; p++) {  
}
