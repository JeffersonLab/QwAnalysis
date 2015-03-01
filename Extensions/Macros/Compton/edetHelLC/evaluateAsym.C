#include "rootClass.h"
#include "comptonRunConstants.h"
///LC: laser cycle
///BC: background corrected
///*B1H1L1: beam on, helicity plus, laser on; and so on for synonymous suffixs
Int_t evaluateAsym(Double_t countsLCB1H1L1[], Double_t countsLCB1H1L0[], Double_t countsLCB1H0L1[], Double_t countsLCB1H0L0[], Double_t qAvgLCH1L1, Double_t qAvgLCH1L0, Double_t qAvgLCH0L1, Double_t qAvgLCH0L0, Double_t tOn, Double_t tOff, Double_t attenFactor[])
{
  cout<<"starting into evaluateAsym.C**************"<<endl;
  const Bool_t debug =0;
  Double_t qNormCountsLCB1H1L1=0.0,qNormCountsLCB1H1L0=0.0,qNormCountsLCB1H0L1=0.0,qNormCountsLCB1H0L0=0.0;
  Double_t BCqNormLCB1H1L1=0.0,BCqNormLCB1H0L1=0.0;
  Double_t BCqNormDiffLC=0.0,BCqNormSumLC=0.0;
  Double_t term1=0.0,term2=0.0,NplusOn_SqQplusOn=0.0,NminusOn_SqQminusOn=0.0,NplusOff_SqQplusOff=0.0,NminusOff_SqQminusOff=0.0;
  Double_t errB1H1L1=0.0, errB1H0L1=0.0, errB1H1L0=0.0, errB1H0L0=0.0;
  Double_t erBCqNormSumLCSq=0.0;
  Double_t newCntsB1H1L1,newCntsB1H1L0,newCntsB1H0L1,newCntsB1H0L0;

  for(int s=0; s<nStrips; s++) {
    if(rateB0[s] > 200.0) rateB0[s] = 0.0;
  }///to avoid using wrong noise correction; The file still records 

  for (Int_t s =startStrip; s <endStrip; s++) {	  
    newCntsB1H1L1 = countsLCB1H1L1[s] * c2B1H1L1[s]; 
    newCntsB1H1L0 = countsLCB1H1L0[s] * c2B1H1L0[s];
    newCntsB1H0L1 = countsLCB1H0L1[s] * c2B1H0L1[s];
    newCntsB1H0L0 = countsLCB1H0L0[s] * c2B1H0L0[s];
    if(s==myStr) {
      printf("%sraw  counts: %9d\t%9d\t%9d\t%9d\n", blue, (int)countsLCB1H1L1[s], (int)countsLCB1H1L0[s], (int)countsLCB1H0L1[s], (int)countsLCB1H0L0[s]);
      printf("post corrDT: %9d\t%9d\t%9d\t%9d\n", (int)newCntsB1H1L1, (int)newCntsB1H1L0, (int)newCntsB1H0L1, (int)newCntsB1H0L0);
      if(debug) printf("corrDT : %f\t%f\t%f\t%f\n", c2B1H1L1[s], c2B1H1L0[s], c2B1H0L1[s], c2B1H0L0[s]);
    }
  ///// Modification due to explicit (electronic) noise subtraction /////      
    if(kNoiseSub) {
      newCntsB1H1L1 = newCntsB1H1L1 - (rateB0[s]*tOn )/(attenFactor[s]);
      newCntsB1H1L0 = newCntsB1H1L0 - (rateB0[s]*tOff)/(attenFactor[s]);
      newCntsB1H0L1 = newCntsB1H0L1 - (rateB0[s]*tOn )/(attenFactor[s]);
      newCntsB1H0L0 = newCntsB1H0L0 - (rateB0[s]*tOff)/(attenFactor[s]);
    }
    qNormCountsLCB1H1L1 = newCntsB1H1L1/qAvgLCH1L1;
    qNormCountsLCB1H1L0 = newCntsB1H1L0/qAvgLCH1L0;
    qNormCountsLCB1H0L1 = newCntsB1H0L1/qAvgLCH0L1;
    qNormCountsLCB1H0L0 = newCntsB1H0L0/qAvgLCH0L0;
    BCqNormLCB1H1L1 = qNormCountsLCB1H1L1 - qNormCountsLCB1H1L0;
    BCqNormLCB1H0L1 = qNormCountsLCB1H0L1 - qNormCountsLCB1H0L0;
    BCqNormDiffLC = (BCqNormLCB1H1L1 - BCqNormLCB1H0L1);
    BCqNormSumLC  = (BCqNormLCB1H1L1 + BCqNormLCB1H0L1);

    if(BCqNormSumLC>0.0) {
      qNormAsymLC[s] = (BCqNormDiffLC / BCqNormSumLC);
      ///Evaluation of error on asymmetry; partitioned the evaluation in a way which avoids re-calculation
      term1 = (1.0-qNormAsymLC[s])/BCqNormSumLC;
      term2 = (1.0+qNormAsymLC[s])/BCqNormSumLC;
      NplusOn_SqQplusOn    = countsLCB1H1L1[s] /(qAvgLCH1L1*qAvgLCH1L1);
      NminusOn_SqQminusOn  = countsLCB1H0L1[s] /(qAvgLCH0L1*qAvgLCH0L1);
      NplusOff_SqQplusOff  = countsLCB1H1L0[s] /(qAvgLCH1L0*qAvgLCH1L0);
      NminusOff_SqQminusOff= countsLCB1H0L0[s] /(qAvgLCH0L0*qAvgLCH0L0);
      errB1H1L1 = pow(term1,2) * NplusOn_SqQplusOn; 
      errB1H0L1 = pow(term2,2) * NminusOn_SqQminusOn;
      errB1H1L0 = pow(term1,2) * NplusOff_SqQplusOff;
      errB1H0L0 = pow(term2,2) * NminusOff_SqQminusOff; 

      asymErSqrLC[s] = (errB1H1L1 + errB1H0L1 + errB1H1L0 + errB1H0L0);

      if (asymErSqrLC[s] >0.0) {///eqn 4.17(Bevington)
        if(s==myStr) {
          printf("post corrNs: %9d\t%9d\t%9d\t%9d\t%7d\n", (int)newCntsB1H1L1, (int)newCntsB1H1L0, (int)newCntsB1H0L1, (int)newCntsB1H0L0, (int)(rateB0[s]));
          cout<<"expAsym[s"<<s+1<<"]: "<<qNormAsymLC[s]<<"+/-"<<TMath::Sqrt(asymErSqrLC[s])<<normal<<endl;
          ///checking the data if the HWP may have changed between the run
          if(checkAsym == 0.0) checkAsym = qNormAsymLC[s];/// if checkAsym is 0, this is 1st laser cycle
          else if((checkAsym/qNormAsymLC[s] < 0.0) && qNormAsymLC[s]>TMath::Sqrt(asymErSqrLC[s])) {
            cout<<red<<"evaluateAsym concludes that sign of asym changed for strip "<<s+1<<normal<<endl;
            cout<<"will try to extract polarization from upto the counts of the previous laser cycle"<<endl;
            cout<<"hence exiting the evaluateAsym.C macro"<<endl;
            return -3;
          }
        }
        wmNrAsym[s] += qNormAsymLC[s]/asymErSqrLC[s]; ///Numerator 
        wmDrAsym[s] += 1.0/asymErSqrLC[s]; ///Denominator
        //if (debug) printf("*****adding %g(1/asymSq) to wmDrAsym making it: %f\n",1.0/asymErSqrLC[s],wmDrAsym[s]);
      } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
      else if(s<nStrips/2) {
        cout<<"for a non masked strip "<<s+1<<"! Asym Er shouldn't be non-positive"<<endl;
        printf("errB1H1L1:%f, errB1H0L1:%f, errB1H1L0:%f, errB1H0L0:%f\n",errB1H1L1,errB1H0L1,errB1H1L0,errB1H0L0);
        printf("%f\t%f\t%f\t%f\t%f\t%f\n",term1, term2, NplusOn_SqQplusOn, NminusOn_SqQminusOn, NplusOff_SqQplusOff, NminusOff_SqQminusOff);
        printf("B1H1L1: %f\t%f\t%f\n",newCntsB1H1L1,countsLCB1H1L1[0],c2B1H1L1[0]);
        printf("B1H1L0: %f\t%f\t%f\n",newCntsB1H1L0,countsLCB1H1L0[0],c2B1H1L0[0]);
        return -1;
      }
      ///Error evaluation for SUM (in asymmetry)
      erBCqNormSumLCSq = (NplusOn_SqQplusOn+ NminusOn_SqQminusOn+ NplusOff_SqQplusOff+ NminusOff_SqQminusOff);

      if (erBCqNormSumLCSq >0.0) {
        wmNrBCqNormSum[s] += BCqNormSumLC/erBCqNormSumLCSq; ///Numerator eqn 4.17(Bevington)
        wmDrBCqNormSum[s] += 1.0/erBCqNormSumLCSq; ///Denominator eqn 4.17(Bevington)
        ///The error for the difference and sum are same, hence reusing the variable
        wmNrBCqNormDiff[s] += BCqNormDiffLC/erBCqNormSumLCSq; ///Numerator eqn 4.17(Bevington)
        //wmDrBCqNormDiff[s] += 1.0/erBCqNormSumLCSq[s]; ///Denominator eqn 4.17(Bevington)
      } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
      else if(s<nStrips/2) {///if the reporting strip is most possibly a pre CE strip
        printf("**Alert: getting non-positive erBCqNormSumLCSq for strip:%d in line:%d\n",s+1,__LINE__);
        printf("NplusOn_SqQplusOn:%g, NminusOn_SqQminusOn:%g, NplusOff_SqQplusOff:%g, NminusOff_SqQminusOff:%g,\n"
            ,NplusOn_SqQplusOn,NminusOn_SqQminusOn,NplusOff_SqQplusOff,NminusOff_SqQminusOff);
        cout<<"exiting evaluateAsym.C and won't use this nCycle for asym eval"<<endl;
        return -2;
      } else {//the counts should never be negative 
        cout<<red<<"Reporting a strip with lasOff counts >= lasOn counts."<<normal<<endl;
        cout<<"Most possibly, a post CE strip, is affected by noise fluctuation due to low statistics"<<endl;
        cout<<"strip\tnewCntsB1H1L1\t...H1L0\t...H0L1\t...H0L0\n"<<
          s+1<<"\t"<<newCntsB1H1L1<<"\t\t"<<newCntsB1H1L0<<"\t"<<newCntsB1H0L1<<"\t"<<newCntsB1H0L0<<endl;
        cout<<blue<<"check if it needs further attention"<<normal<<endl;
        continue;//break;//continue;
        ///the above can often happen for post-CE strips, hence 'break' instead of 'continue' is a better solution
        ///... eg.run 23300, in nCycle 29
      }
    }
  }//for (Int_t s =startStrip; s <endStrip; s++) {

  return 1;
}
