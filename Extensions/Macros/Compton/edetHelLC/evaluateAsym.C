#include "rootClass.h"
#include "comptonRunConstants.h"
///LC: laser cycle
///BC: background corrected
///*B1H1L1: beam on, helicity plus, laser on; and so on for synonymous suffixs
Int_t evaluateAsym(Double_t countsLCB1H1L1[], Double_t countsLCB1H1L0[], Double_t countsLCB1H0L1[], Double_t countsLCB1H0L0[], Double_t qAvgLCH1L1, Double_t qAvgLCH1L0, Double_t qAvgLCH0L1, Double_t qAvgLCH0L0)
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

  for (Int_t s =startStrip; s <endStrip; s++) {	  
    newCntsB1H1L1=countsLCB1H1L1[s] * c2B1H1L1[s]; 
    newCntsB1H1L0=countsLCB1H1L0[s] * c2B1H1L0[s];
    newCntsB1H0L1=countsLCB1H0L1[s] * c2B1H0L1[s];
    newCntsB1H0L0=countsLCB1H0L0[s] * c2B1H0L0[s];
    qNormCountsLCB1H1L1= newCntsB1H1L1/qAvgLCH1L1;
    qNormCountsLCB1H1L0= newCntsB1H1L0/qAvgLCH1L0;
    qNormCountsLCB1H0L1= newCntsB1H0L1/qAvgLCH0L1;
    qNormCountsLCB1H0L0= newCntsB1H0L0/qAvgLCH0L0;
    BCqNormLCB1H1L1= qNormCountsLCB1H1L1 - qNormCountsLCB1H1L0;
    BCqNormLCB1H0L1= qNormCountsLCB1H0L1 - qNormCountsLCB1H0L0;
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
        wmNrAsym[s] += qNormAsymLC[s]/asymErSqrLC[s]; ///Numerator 
        wmDrAsym[s] += 1.0/asymErSqrLC[s]; ///Denominator
        if (debug) printf("*****adding %g(1/asymSq) to wmDrAsym making it: %f\n",1.0/asymErSqrLC[s],wmDrAsym[s]);
      } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
      else {
        cout<<"for a non masked strip "<<s+1<<"! Asym Er shouldn't be non-positive"<<endl;
        printf("errB1H1L1:%f, errB1H0L1:%f, errB1H1L0:%f, errB1H0L0:%f\n",errB1H1L1,errB1H0L1,errB1H1L0,errB1H0L0);
        printf("%f\t%f\t%f\t%f\t%f\t%f\n",term1, term2, NplusOn_SqQplusOn, NminusOn_SqQminusOn, NplusOff_SqQplusOff, NminusOff_SqQminusOff);
        printf("%f\t%f\t%f\n",newCntsB1H1L1,countsLCB1H1L1[0],c2B1H1L1[0]);
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
      } else {
        printf("**Alert: getting non-positive erBCqNormSumLCSq for strip:%d in line:%d\n",s+1,__LINE__);
        printf("NplusOn_SqQplusOn:%g, NminusOn_SqQminusOn:%g, NplusOff_SqQplusOff:%g, NminusOff_SqQminusOff:%g,\n"
            ,NplusOn_SqQplusOn,NminusOn_SqQminusOn,NplusOff_SqQplusOff,NminusOff_SqQminusOff);
      }
    } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    else {//the counts should never be negative 
      cout<<red<<"Reporting a strip with equal no.of laser on & off counts. The lasOn could be engulfed in a beamTrip"<<normal<<endl;
      cout<<"strip\tnewCntsB1H1L1\t...H1L0\t...H0L1\t...H0L0\n"<<
        s+1<<"\t"<<newCntsB1H1L1<<"\t\t"<<newCntsB1H1L0<<"\t"<<newCntsB1H0L1<<"\t"<<newCntsB1H0L0<<endl;
      if(s<nStrips/2) {///if the reporting strip is most possibly a pre CE strip
        //  "\nqNormCountsLCB1H1L1:"<<qNormCountsLCB1H1L1<<" ,H1L0:"<<qNormCountsLCB1H1L0<<", H0L1:"<<qNormCountsLCB1H0L1<<" ,H0L0:"<<qNormCountsLCB1H0L0<<
        //  "\nBCqNormLCB1H1L1:"<<BCqNormLCB1H1L1<<" ,H0L1:"<<BCqNormLCB1H0L1<<endl;
        cout<<"exiting evaluateAsym.C and won't use this nCycle for asym eval"<<endl;
        return -1;
      } else {
        cout<<blue<<"check if it needs further attention"<<normal<<endl;
        break;//continue;
        ///the above can often happen for post-CE strips, hence 'break' instead of 'continue' is a better solution
        ///... eg.run 23300, in nCycle 29
      }
    }
  }//for (Int_t s =startStrip; s <endStrip; s++) {	
  cout<<"this nCycle e.g: expAsym[s38]: "<<qNormAsymLC[37]<<"+/-"<<TMath::Sqrt(asymErSqrLC[37])<<endl;
  return 1;
}
