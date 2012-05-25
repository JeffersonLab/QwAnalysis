#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"


void evaluateAsym(Int_t AccumB1H1L1[nPlanes][nStrips],Int_t AccumB1H0L1[nPlanes][nStrips],Int_t AccumB1H1L0L[nPlanes][nStrips],Int_t AccumB1H0L0L[nPlanes][nStrips],Int_t AccumB1H1L0R[nPlanes][nStrips],Int_t AccumB1H0L0R[nPlanes][nStrips],Float_t iCounterH1L1,Float_t iCounterH0L1,Float_t iCounterH1L0L,Float_t iCounterH0L0L,Float_t iCounterH1L0R,Float_t iCounterH0L0R,Float_t laserOnOffRatioH1,Float_t laserOnOffRatioH0,Float_t weightedMeanNrAsym[nPlanes][nStrips],Float_t weightedMeanDrAsym[nPlanes][nStrips],Float_t weightedMeanNrBCqNormSum[nPlanes][nStrips],Float_t weightedMeanDrBCqNormSum[nPlanes][nStrips],Float_t weightedMeanNrBCqNormDiff[nPlanes][nStrips],Float_t weightedMeanNrqNormB1L0[nPlanes][nStrips],Float_t weightedMeanDrqNormB1L0[nPlanes][nStrips])
{

  Bool_t debug=0;
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      if (maskedStrips(p,s)) continue;
      Float_t qNormAcB1H1L1LasCyc   = AccumB1H1L1[p][s]  /iCounterH1L1 ;
      Float_t qNormAcB1H0L1LasCyc   = AccumB1H0L1[p][s]  /iCounterH0L1 ;
      Float_t qNormAcB1H1L0LLasCyc = AccumB1H1L0L[p][s] /iCounterH1L0L ;
      Float_t qNormAcB1H0L0LLasCyc = AccumB1H0L0L[p][s] /iCounterH0L0L ;
      Float_t qNormAcB1H1L0RLasCyc = AccumB1H1L0R[p][s] /iCounterH1L0R ;
      Float_t qNormAcB1H0L0RLasCyc = AccumB1H0L0R[p][s] /iCounterH0L0R ;

      //adding new variable to track the laser-off, beam background
      Float_t qNormAcB1L0LasCyc= qNormAcB1H1L0LLasCyc+qNormAcB1H0L0LLasCyc+qNormAcB1H1L0RLasCyc+qNormAcB1H0L0RLasCyc;
      Float_t BCqNormAcB1H1L1LasCyc=qNormAcB1H1L1LasCyc-laserOnOffRatioH1*(qNormAcB1H1L0LLasCyc+qNormAcB1H1L0RLasCyc);
      Float_t BCqNormAcB1H0L1LasCyc=qNormAcB1H0L1LasCyc-laserOnOffRatioH0*(qNormAcB1H0L0LLasCyc+qNormAcB1H0L0RLasCyc); 
      Float_t BCqNormLasCycDiff = (BCqNormAcB1H1L1LasCyc - BCqNormAcB1H0L1LasCyc);
      Float_t BCqNormLasCycSum  = (BCqNormAcB1H1L1LasCyc + BCqNormAcB1H0L1LasCyc);

//       if (BCqNormLasCycSum <= 0.0 && s<=Cedge[p]) {//!the Cedge is not determined yet
// 	//	printf("\n**Warning**:BCqNormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCqNormLasCycSum,nCycle);
// 	printf("note: AccumB1H1L1:%d, AccumB1H1L0R:%d, AccumB1H0L1:%d, AccumB1H0L0R:%d\n"
// 	       ,AccumB1H1L1[p][s],AccumB1H1L0R[p][s],AccumB1H0L1[p][s],AccumB1H0L0R[p][s]);
// 	//	printf("and comptQH1L1:%f, comptQH1L0R:%f, comptQH0L1:%f, comptQH0L0R:%f",
// 	//	       comptQH1L1,comptQH1L0R,comptQH0L1,comptQH0L0R);
//       }
//       else {
	Float_t qNormLasCycAsym = (BCqNormLasCycDiff / BCqNormLasCycSum);
	      
	///Evaluation of error on asymmetry; I've partitioned the evaluation in a way which avoids re-calculation
	Float_t term1 = (1-qNormLasCycAsym)/BCqNormLasCycSum;
	Float_t term2 = (1+qNormLasCycAsym)/BCqNormLasCycSum;
	Float_t NplusOn_SqQplusOn      = qNormAcB1H1L1LasCyc  /iCounterH1L1;
	Float_t NminusOn_SqQminusOn    = qNormAcB1H0L1LasCyc  /iCounterH0L1;
	Float_t NplusOffL_SqQplusOffL  = qNormAcB1H1L0LLasCyc/iCounterH1L0L;
	Float_t NminusOffL_SqQminusOffL= qNormAcB1H0L0LLasCyc/iCounterH0L0L;
	Float_t NplusOffR_SqQplusOffR  = qNormAcB1H1L0RLasCyc/iCounterH1L0R;
	Float_t NminusOffR_SqQminusOffR= qNormAcB1H0L0RLasCyc/iCounterH0L0R;

	///redefining these error variables 
	Float_t errB1H1L1 = pow(term1,2) * NplusOn_SqQplusOn; 
	Float_t errB1H0L1 = pow(term2,2) * NminusOn_SqQminusOn;
	Float_t errB1H1L0L= pow(term1,2) * NplusOffL_SqQplusOffL ;
	Float_t errB1H0L0L= pow(term2,2) * NminusOffL_SqQminusOffL; 
	Float_t errB1H1L0R= pow(term1,2) * NplusOffR_SqQplusOffR ;
	Float_t errB1H0L0R= pow(term2,2) * NminusOffR_SqQminusOffR;

	Float_t LasCycAsymErSqr = (errB1H1L1 + errB1H0L1 + errB1H1L0R + errB1H0L0R + errB1H1L0L + errB1H0L0L);

	if (debug) {
	  printf("qNormLasCycAsym[for p:%d,s:%d]= %f (stat.err:%f)\n",p,s,qNormLasCycAsym,LasCycAsymErSqr);
	  printf("formed by normalized BC (%f -/+ %f) \n", BCqNormLasCycDiff,BCqNormLasCycSum);
	  printf("AccumB1H1L1[%d][%d]: %d\tAccumB1H0L1: %d\tAccumB1H1L0R: %d\tAccumB1H0L0R: %d\n",
		 p,s,AccumB1H1L1[p][s],AccumB1H0L1[p][s],AccumB1H1L0R[p][s],AccumB1H0L0R[p][s]);
	}
	if(LasCycAsymErSqr!=(Float_t)0.0) {///eqn 4.17(Bevington)
	  weightedMeanNrAsym[p][s] += qNormLasCycAsym/LasCycAsymErSqr; ///Numerator 
	  weightedMeanDrAsym[p][s] += 1.0/LasCycAsymErSqr; ///Denominator 
	}
	else cout<<"check plane "<<p<<" strip "<<s<<" . It gives zero Asym Er"<<endl;

	///Error evaluation for SUM (in asymmetry)
	Float_t erBCqNormLasCycSumSq = (NplusOn_SqQplusOn+NminusOn_SqQminusOn+NplusOffL_SqQplusOffL+NminusOffL_SqQminusOffL+NplusOffR_SqQplusOffR+NminusOffR_SqQminusOffR);
	///Error of laser off background for every laser cycle
	Float_t erqNormB1L0LasCycSq = (NplusOffL_SqQplusOffL+NminusOffL_SqQminusOffL+NplusOffR_SqQplusOffR+NminusOffR_SqQminusOffR);

	if(erBCqNormLasCycSumSq !=0.0) {
	  weightedMeanNrBCqNormSum[p][s] += BCqNormLasCycSum/erBCqNormLasCycSumSq; ///Numerator eqn 4.17(Bevington)
	  weightedMeanDrBCqNormSum[p][s] += 1.0/erBCqNormLasCycSumSq; ///Denominator eqn 4.17(Bevington)
	  ///The error for the difference and sum are same, hence reusing the variable
	  weightedMeanNrBCqNormDiff[p][s] += BCqNormLasCycDiff/erBCqNormLasCycSumSq; ///Numerator eqn 4.17(Bevington)
	  //weightedMeanDrBCqNormDiff[p][s] += 1.0/erBCqNormLasCycSumSq[p][s]; ///Denominator eqn 4.17(Bevington)
	  weightedMeanNrqNormB1L0[p][s] += qNormAcB1L0LasCyc/erqNormB1L0LasCycSq; //refer bevington
	  weightedMeanDrqNormB1L0[p][s] += 1.0/erqNormB1L0LasCycSq; //refer bevington
	} else {
	  printf("**Alert: getting zero for erBCqNormLasCycSumSq for plane:%d, strip:%d in line:%d\n",p+1,s+1,__LINE__);
	  printf("AccumB1H1L1:%d, iCounterH1L1:%g\n",AccumB1H1L1[p][s],iCounterH1L1);
	}
    }//for (Int_t s =startStrip; s <endStrip; s++) {	
  }///for (Int_t p =startPlane; p <endPlane; p++) {


  
}
