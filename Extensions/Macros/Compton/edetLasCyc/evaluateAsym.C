#include <rootClass.h>
#include "comptonRunConstants.h"
#include "rhoToX.C"

void evaluateAsym(Int_t newAccumB1H1L1[nPlanes][nStrips],Int_t newAccumB1H1L0[nPlanes][nStrips],Int_t newAccumB1H0L1[nPlanes][nStrips],Int_t newAccumB1H0L0[nPlanes][nStrips],Float_t bcmLasCycH1L1,Float_t bcmLasCycH1L0,Float_t bcmLasCycH0L1,Float_t bcmLasCycH0L0,Float_t weightedMeanNrAsym[nPlanes][nStrips],Float_t weightedMeanDrAsym[nPlanes][nStrips],Float_t weightedMeanNrBCqNormSum[nPlanes][nStrips],Float_t weightedMeanDrBCqNormSum[nPlanes][nStrips],Float_t weightedMeanNrBCqNormDiff[nPlanes][nStrips],Float_t weightedMeanNrqNormB1L0[nPlanes][nStrips],Float_t weightedMeanDrqNormB1L0[nPlanes][nStrips],Float_t weightedMeanNrBkgdAsym[nPlanes][nStrips],Float_t weightedMeanDrBkgdAsym[nPlanes][nStrips],Float_t qNormLasCycAsym[nPlanes][nStrips],Float_t LasCycAsymErSqr[nPlanes][nStrips])
{
  cout<<"starting into evaluateAsym.C**************"<<endl;
  const Bool_t debug=0;
//   Int_t tLasOn = nMpsB1H1L1 + nMpsB1H0L1;
//   Int_t tLasOff= nMpsB1H1L0 + nMpsB1H0L0;

  Float_t qLasCycH1L1 = bcmLasCycH1L1 /MpsRate;//this really gives total-charge for this laser cycle
  Float_t qLasCycH0L1 = bcmLasCycH0L1 /MpsRate;
  Float_t qLasCycH1L0 = bcmLasCycH1L0 /MpsRate;
  Float_t qLasCycH0L0 = bcmLasCycH0L0 /MpsRate;

  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      if (!mask[p][s]) continue;
      Float_t qNormAcB1H1L1LasCyc = newAccumB1H1L1[p][s] /qLasCycH1L1;
      Float_t qNormAcB1H1L0LasCyc = newAccumB1H1L0[p][s] /qLasCycH1L0;
      Float_t qNormAcB1H0L1LasCyc = newAccumB1H0L1[p][s] /qLasCycH0L1;
      Float_t qNormAcB1H0L0LasCyc = newAccumB1H0L0[p][s] /qLasCycH0L0;

      Float_t BCqNormAcB1H1L1LasCyc= qNormAcB1H1L1LasCyc - qNormAcB1H1L0LasCyc;
      Float_t BCqNormAcB1H0L1LasCyc= qNormAcB1H0L1LasCyc - qNormAcB1H0L0LasCyc;
      //Float_t BCqNormLasCycDiff = (BCqNormAcB1H1L1LasCyc - BCqNormAcB1H0L1LasCyc);//I should use this variable without explicit background correction because in difference, this is automatically taken care
      Float_t BCqNormLasCycDiff = (qNormAcB1H1L1LasCyc - qNormAcB1H0L1LasCyc);
      Float_t BCqNormLasCycSum  = (BCqNormAcB1H1L1LasCyc + BCqNormAcB1H0L1LasCyc);
      qNormLasCycAsym[p][s] = (BCqNormLasCycDiff / BCqNormLasCycSum);

      ///Evaluation of error on asymmetry; partitioned the evaluation in a way which avoids re-calculation
      Float_t term1 = (1-qNormLasCycAsym[p][s])/BCqNormLasCycSum;
      Float_t term2 = (1+qNormLasCycAsym[p][s])/BCqNormLasCycSum;
      Float_t NplusOn_SqQplusOn    = qNormAcB1H1L1LasCyc /qLasCycH1L1;
      Float_t NminusOn_SqQminusOn  = qNormAcB1H0L1LasCyc /qLasCycH0L1;
      Float_t NplusOff_SqQplusOff  = qNormAcB1H1L0LasCyc /qLasCycH1L0;
      Float_t NminusOff_SqQminusOff= qNormAcB1H0L0LasCyc /qLasCycH0L0;

      ///redefining these error variables 
      Float_t errB1H1L1 = pow(term1,2) * NplusOn_SqQplusOn; 
      Float_t errB1H0L1 = pow(term2,2) * NminusOn_SqQminusOn;
      Float_t errB1H1L0 = pow(term1,2) * NplusOff_SqQplusOff;
      Float_t errB1H0L0 = pow(term2,2) * NminusOff_SqQminusOff; 

      LasCycAsymErSqr[p][s] = (errB1H1L1 + errB1H0L1 + errB1H1L0 + errB1H0L0);

      ///adding asymmetry evaluation for laser off data
      Float_t qNormAcBkgdAsymNr = qNormAcB1H1L0LasCyc - qNormAcB1H0L0LasCyc;
      Float_t qNormAcBkgdAsymDr = qNormAcB1H1L0LasCyc + qNormAcB1H0L0LasCyc;
      Float_t qNormAcBkgdAsym = qNormAcBkgdAsymNr/qNormAcBkgdAsymDr;
      Float_t term1Bkgd = (1-qNormAcBkgdAsym)/qNormAcBkgdAsymDr;
      Float_t term2Bkgd = (1+qNormAcBkgdAsym)/qNormAcBkgdAsymDr;

      Float_t errBkgdAsymH1 = pow(term1Bkgd,2) * NplusOff_SqQplusOff;
      Float_t errBkgdAsymH0 = pow(term2Bkgd,2) * NminusOff_SqQminusOff;
      Float_t bkgdAsymErSqr = errBkgdAsymH1 + errBkgdAsymH0;
      

      if (debug) {
	printf("***qNormLasCycAsym[p][s][%d][%d]= %g +/- %g)\n",p,s,qNormLasCycAsym[p][s],LasCycAsymErSqr[p][s]);
	printf("*********formed by normalized BC (%g -/+ %g) \n", BCqNormLasCycDiff,BCqNormLasCycSum);
	//printf("newAccumB1H1L1[%d][%d]: %f\tnewAccumB1H0L1: %f\tnewAccumB1H1L0: %f\tnewAccumB1H0L0: %f\n",
	//      p,s,newAccumB1H1L1[p][s],newAccumB1H0L1[p][s],newAccumB1H1L0[p][s],newAccumB1H0L0[p][s]);
      }
      if (LasCycAsymErSqr[p][s] >0.0) {///eqn 4.17(Bevington)
	weightedMeanNrAsym[p][s] += qNormLasCycAsym[p][s]/LasCycAsymErSqr[p][s]; ///Numerator 
	weightedMeanDrAsym[p][s] += 1.0/LasCycAsymErSqr[p][s]; ///Denominator 
	if (debug) printf("*****adding %g(1/asymSq) to weightedMeanDrAsym making it: %f\n",1.0/LasCycAsymErSqr[p][s],weightedMeanDrAsym[p][s]);
      } else {
	cout<<"check if plane "<<p+1<<" strip "<<s+1<<" is MASKED? It gives non-positive Asym Er"<<endl;
	printf("errB1H1L1:%f, errB1H0L1:%f, errB1H1L0:%f, errB1H0L0:%f\n",errB1H1L1,errB1H0L1,errB1H1L0,errB1H0L0);
      }
      ///Error evaluation for SUM (in asymmetry)
      Float_t erBCqNormLasCycSumSq = (NplusOn_SqQplusOn+ NminusOn_SqQminusOn+ NplusOff_SqQplusOff+ NminusOff_SqQminusOff);
      ///Error of laser off background for every laser cycle
      Float_t erqNormB1L0LasCycSq = (NplusOff_SqQplusOff+NminusOff_SqQminusOff);



      ///addition of bkgdAsym term over various laser cycles
      if (bkgdAsymErSqr > 0.0) {///eqn 4.17(Bevington)
	weightedMeanNrBkgdAsym[p][s] += qNormAcBkgdAsym/bkgdAsymErSqr; ///Numerator 
	weightedMeanDrBkgdAsym[p][s] += 1.0/bkgdAsymErSqr; ///Denominator 
	if (debug) printf("*****adding %g(1/bkgdAsymSq) to weightedMeanDrBkgdAsym making it: %f\n",1.0/bkgdAsymErSqr,weightedMeanDrBkgdAsym[p][s]);
      } else {
	cout<<"check if plane "<<p+1<<" strip "<<s+1<<" is MASKED? the bkgdAsymEr is non-positive"<<endl;
	printf("errBkgdAsymH1:%f, errBkgdAsymH1:%f\n",errBkgdAsymH1,errBkgdAsymH0);
      }


      if (erBCqNormLasCycSumSq >0.0) {
	weightedMeanNrBCqNormSum[p][s] += BCqNormLasCycSum/erBCqNormLasCycSumSq; ///Numerator eqn 4.17(Bevington)
	weightedMeanDrBCqNormSum[p][s] += 1.0/erBCqNormLasCycSumSq; ///Denominator eqn 4.17(Bevington)
	///The error for the difference and sum are same, hence reusing the variable
	weightedMeanNrBCqNormDiff[p][s] += BCqNormLasCycDiff/erBCqNormLasCycSumSq; ///Numerator eqn 4.17(Bevington)
	//weightedMeanDrBCqNormDiff[p][s] += 1.0/erBCqNormLasCycSumSq[p][s]; ///Denominator eqn 4.17(Bevington)
	weightedMeanNrqNormB1L0[p][s] += qNormAcBkgdAsymDr/erqNormB1L0LasCycSq; //refer bevington
	weightedMeanDrqNormB1L0[p][s] += 1.0/erqNormB1L0LasCycSq; //refer bevington
      } else if(debug) {
	printf("**Alert: getting non-positive erBCqNormLasCycSumSq for plane:%d, strip:%d in line:%d\n",p+1,s+1,__LINE__);
	printf("newAccumB1H1L1:%d, bcmLasCycH1L1:%g\n",newAccumB1H1L1[p][s],bcmLasCycH1L1);
	printf("NplusOn_SqQplusOn:%g, NminusOn_SqQminusOn:%g, NplusOff_SqQplusOff:%g, NminusOff_SqQminusOff:%g,\n"
	       ,NplusOn_SqQplusOn,NminusOn_SqQminusOn,NplusOff_SqQplusOff,NminusOff_SqQminusOff);
      }
    }//for (Int_t s =startStrip; s <endStrip; s++) {	
  }///for (Int_t p =startPlane; p <endPlane; p++) {  
}
