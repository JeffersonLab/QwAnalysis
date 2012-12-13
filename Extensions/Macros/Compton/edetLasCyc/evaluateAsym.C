#include <rootClass.h>
#include "comptonRunConstants.h"
#include "rhoToX.C"

void evaluateAsym(Int_t newAccumB1H1L1[nPlanes][nStrips],Int_t newAccumB1H1L0[nPlanes][nStrips],Int_t newAccumB1H0L1[nPlanes][nStrips],Int_t newAccumB1H0L0[nPlanes][nStrips],Double_t bcmLasCycH1L1,Double_t bcmLasCycH1L0,Double_t bcmLasCycH0L1,Double_t bcmLasCycH0L0,Double_t wmNrAsym[nPlanes][nStrips],Double_t wmDrAsym[nPlanes][nStrips],Double_t wmNrBCqNormSum[nPlanes][nStrips],Double_t wmDrBCqNormSum[nPlanes][nStrips],Double_t wmNrBCqNormDiff[nPlanes][nStrips],Double_t wmNrqNormB1L0[nPlanes][nStrips],Double_t wmDrqNormB1L0[nPlanes][nStrips],Double_t wmNrBkgdAsym[nPlanes][nStrips],Double_t wmDrBkgdAsym[nPlanes][nStrips],Double_t qNormLasCycAsym[nPlanes][nStrips],Double_t LasCycAsymErSqr[nPlanes][nStrips])
{
  cout<<"starting into evaluateAsym.C**************"<<endl;
  const Bool_t debug=0,debug1=1;
//   Int_t tLasOn = nMpsB1H1L1 + nMpsB1H0L1;
//   Int_t tLasOff= nMpsB1H1L0 + nMpsB1H0L0;

  Double_t qLasCycH1L1 = bcmLasCycH1L1 /MpsRate;//this really gives total-charge for this laser cycle
  Double_t qLasCycH0L1 = bcmLasCycH0L1 /MpsRate;
  Double_t qLasCycH1L0 = bcmLasCycH1L0 /MpsRate;
  Double_t qLasCycH0L0 = bcmLasCycH0L0 /MpsRate;

  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      if (!mask[p][s]) continue;
      Double_t qNormAcB1H1L1LasCyc = newAccumB1H1L1[p][s] /qLasCycH1L1;
      Double_t qNormAcB1H1L0LasCyc = newAccumB1H1L0[p][s] /qLasCycH1L0;
      Double_t qNormAcB1H0L1LasCyc = newAccumB1H0L1[p][s] /qLasCycH0L1;
      Double_t qNormAcB1H0L0LasCyc = newAccumB1H0L0[p][s] /qLasCycH0L0;

      Double_t BCqNormAcB1H1L1LasCyc= qNormAcB1H1L1LasCyc - qNormAcB1H1L0LasCyc;
      Double_t BCqNormAcB1H0L1LasCyc= qNormAcB1H0L1LasCyc - qNormAcB1H0L0LasCyc;
      //Double_t BCqNormLasCycDiff = (BCqNormAcB1H1L1LasCyc - BCqNormAcB1H0L1LasCyc);//I should use this variable without explicit background correction because in difference, this is automatically taken care
      Double_t BCqNormLasCycDiff = (qNormAcB1H1L1LasCyc - qNormAcB1H0L1LasCyc);
      Double_t BCqNormLasCycSum  = (BCqNormAcB1H1L1LasCyc + BCqNormAcB1H0L1LasCyc);
      qNormLasCycAsym[p][s] = (BCqNormLasCycDiff / BCqNormLasCycSum);

      ///Evaluation of error on asymmetry; partitioned the evaluation in a way which avoids re-calculation
      Double_t term1 = (1-qNormLasCycAsym[p][s])/BCqNormLasCycSum;
      Double_t term2 = (1+qNormLasCycAsym[p][s])/BCqNormLasCycSum;
      Double_t NplusOn_SqQplusOn    = qNormAcB1H1L1LasCyc /qLasCycH1L1;
      Double_t NminusOn_SqQminusOn  = qNormAcB1H0L1LasCyc /qLasCycH0L1;
      Double_t NplusOff_SqQplusOff  = qNormAcB1H1L0LasCyc /qLasCycH1L0;
      Double_t NminusOff_SqQminusOff= qNormAcB1H0L0LasCyc /qLasCycH0L0;

      ///redefining these error variables 
      Double_t errB1H1L1 = pow(term1,2) * NplusOn_SqQplusOn; 
      Double_t errB1H0L1 = pow(term2,2) * NminusOn_SqQminusOn;
      Double_t errB1H1L0 = pow(term1,2) * NplusOff_SqQplusOff;
      Double_t errB1H0L0 = pow(term2,2) * NminusOff_SqQminusOff; 

      LasCycAsymErSqr[p][s] = (errB1H1L1 + errB1H0L1 + errB1H1L0 + errB1H0L0);

      ///adding asymmetry evaluation for laser off data
      Double_t qNormAcBkgdAsymNr = qNormAcB1H1L0LasCyc - qNormAcB1H0L0LasCyc;
      Double_t qNormAcBkgdAsymDr = qNormAcB1H1L0LasCyc + qNormAcB1H0L0LasCyc;
      Double_t qNormAcBkgdAsym = qNormAcBkgdAsymNr/qNormAcBkgdAsymDr;
      Double_t term1Bkgd = (1-qNormAcBkgdAsym)/qNormAcBkgdAsymDr;
      Double_t term2Bkgd = (1+qNormAcBkgdAsym)/qNormAcBkgdAsymDr;

      Double_t errBkgdAsymH1 = pow(term1Bkgd,2) * NplusOff_SqQplusOff;
      Double_t errBkgdAsymH0 = pow(term2Bkgd,2) * NminusOff_SqQminusOff;
      Double_t bkgdAsymErSqr = errBkgdAsymH1 + errBkgdAsymH0;
      

      if (debug1) {
	//printf("***qNormLasCycAsym[p][s][%d][%d]= %g +/- %g)\n",p,s,qNormLasCycAsym[p][s],LasCycAsymErSqr[p][s]);
	//printf("*********formed by normalized BC (%g -/+ %g) \n", BCqNormLasCycDiff,BCqNormLasCycSum);
	//printf("counts[%d][%d]: %d\t%d\t%d\t%d\n",p,s,newAccumB1H1L1[p][s],newAccumB1H1L0[p][s],newAccumB1H0L1[p][s],newAccumB1H0L0[p][s]);
      }
      if (LasCycAsymErSqr[p][s] >0.0) {///eqn 4.17(Bevington)
	wmNrAsym[p][s] += qNormLasCycAsym[p][s]/LasCycAsymErSqr[p][s]; ///Numerator 
	wmDrAsym[p][s] += 1.0/LasCycAsymErSqr[p][s]; ///Denominator 
	if (debug) printf("*****adding %g(1/asymSq) to wmDrAsym making it: %f\n",1.0/LasCycAsymErSqr[p][s],wmDrAsym[p][s]);
      } else {
	cout<<"check if plane "<<p+1<<" strip "<<s+1<<" is MASKED? It gives non-positive Asym Er"<<endl;
	printf("errB1H1L1:%f, errB1H0L1:%f, errB1H1L0:%f, errB1H0L0:%f\n",errB1H1L1,errB1H0L1,errB1H1L0,errB1H0L0);
      }
      ///Error evaluation for SUM (in asymmetry)
      Double_t erBCqNormLasCycSumSq = (NplusOn_SqQplusOn+ NminusOn_SqQminusOn+ NplusOff_SqQplusOff+ NminusOff_SqQminusOff);
      ///Error of laser off background for every laser cycle
      Double_t erqNormB1L0LasCycSq = (NplusOff_SqQplusOff+NminusOff_SqQminusOff);



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
	printf("newAccumB1H1L1:%d, bcmLasCycH1L1:%g\n",newAccumB1H1L1[p][s],bcmLasCycH1L1);
	printf("NplusOn_SqQplusOn:%g, NminusOn_SqQminusOn:%g, NplusOff_SqQplusOff:%g, NminusOff_SqQminusOff:%g,\n"
	       ,NplusOn_SqQplusOn,NminusOn_SqQminusOn,NplusOff_SqQplusOff,NminusOff_SqQminusOff);
      }
    }//for (Int_t s =startStrip; s <endStrip; s++) {	
  }///for (Int_t p =startPlane; p <endPlane; p++) {  
}
