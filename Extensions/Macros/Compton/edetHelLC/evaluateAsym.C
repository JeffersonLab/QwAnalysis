#include "rootClass.h"
#include "comptonRunConstants.h"
#include "rhoToX.C"
///LC: laser cycle
///BC: background corrected
///*B1H1L1: beam on, helicity plus, laser on; and so on for synonymous suffixs
Int_t evaluateAsym(Double_t countsLCB1H1L1[nPlanes][nStrips],Double_t countsLCB1H1L0[nPlanes][nStrips],Double_t countsLCB1H0L1[nPlanes][nStrips],Double_t countsLCB1H0L0[nPlanes][nStrips],Double_t qAvgLCH1L1,Double_t qAvgLCH1L0,Double_t qAvgLCH0L1,Double_t qAvgLCH0L0,Double_t wmNrAsym[nPlanes][nStrips],Double_t wmDrAsym[nPlanes][nStrips],Double_t wmNrBCqNormSum[nPlanes][nStrips],Double_t wmDrBCqNormSum[nPlanes][nStrips],Double_t wmNrBCqNormDiff[nPlanes][nStrips],Double_t wmNrqNormB1L0[nPlanes][nStrips],Double_t wmDrqNormB1L0[nPlanes][nStrips],Double_t wmNrBkgdAsym[nPlanes][nStrips],Double_t wmDrBkgdAsym[nPlanes][nStrips],Double_t qNormAsymLC[nPlanes][nStrips],Double_t asymErSqrLC[nPlanes][nStrips])
{
  cout<<"starting into evaluateAsym.C**************"<<endl;
  const Bool_t debug=0,debug1=0;
  Double_t qNormCountsLCB1H1L1=0.0,qNormCountsLCB1H1L0=0.0,qNormCountsLCB1H0L1=0.0,qNormCountsLCB1H0L0=0.0;
  Double_t BCqNormLCB1H1L1=0.0,BCqNormLCB1H0L1=0.0;
  Double_t BCqNormDiffLC=0.0,BCqNormSumLC=0.0;
  Double_t term1=0.0,term2=0.0,NplusOn_SqQplusOn=0.0,NminusOn_SqQminusOn=0.0,NplusOff_SqQplusOff=0.0,NminusOff_SqQminusOff=0.0;
  Double_t errB1H1L1=0.0, errB1H0L1=0.0, errB1H1L0=0.0, errB1H0L0=0.0;
  Double_t qNormAcBkgdAsymNr=0.0,qNormAcBkgdAsymDr=0.0,qNormAcBkgdAsym=0.0,term1Bkgd=0.0,term2Bkgd=0.0,errBkgdAsymH1=0.0,errBkgdAsymH0=0.0,bkgdAsymErSqr=0.0;
  Double_t erBCqNormSumLCSq=0.0,erqNormB1L0LasCycSq=0.0;
  Double_t newCntsB1H1L1,newCntsB1H1L0,newCntsB1H0L1,newCntsB1H0L0;

  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      newCntsB1H1L1=countsLCB1H1L1[p][s] * c2B1H1L1[p][s]; 
      newCntsB1H1L0=countsLCB1H1L0[p][s] * c2B1H1L0[p][s];
      newCntsB1H0L1=countsLCB1H0L1[p][s] * c2B1H0L1[p][s];
      newCntsB1H0L0=countsLCB1H0L0[p][s] * c2B1H0L0[p][s];
      qNormCountsLCB1H1L1= newCntsB1H1L1/qAvgLCH1L1;
      qNormCountsLCB1H1L0= newCntsB1H1L0/qAvgLCH1L0;
      qNormCountsLCB1H0L1= newCntsB1H0L1/qAvgLCH0L1;
      qNormCountsLCB1H0L0= newCntsB1H0L0/qAvgLCH0L0;
      BCqNormLCB1H1L1= qNormCountsLCB1H1L1 - qNormCountsLCB1H1L0;
      BCqNormLCB1H0L1= qNormCountsLCB1H0L1 - qNormCountsLCB1H0L0;
      BCqNormDiffLC = (BCqNormLCB1H1L1 - BCqNormLCB1H0L1);
      BCqNormSumLC  = (BCqNormLCB1H1L1 + BCqNormLCB1H0L1);
      if(BCqNormSumLC>0.0) {
        qNormAsymLC[p][s] = (BCqNormDiffLC / BCqNormSumLC);
        ///Evaluation of error on asymmetry; partitioned the evaluation in a way which avoids re-calculation
        term1 = (1.0-qNormAsymLC[p][s])/BCqNormSumLC;
        term2 = (1.0+qNormAsymLC[p][s])/BCqNormSumLC;
        NplusOn_SqQplusOn    = countsLCB1H1L1[p][s] /(qAvgLCH1L1*qAvgLCH1L1);
        NminusOn_SqQminusOn  = countsLCB1H0L1[p][s] /(qAvgLCH0L1*qAvgLCH0L1);
        NplusOff_SqQplusOff  = countsLCB1H1L0[p][s] /(qAvgLCH1L0*qAvgLCH1L0);
        NminusOff_SqQminusOff= countsLCB1H0L0[p][s] /(qAvgLCH0L0*qAvgLCH0L0);
        errB1H1L1 = pow(term1,2) * NplusOn_SqQplusOn; 
        errB1H0L1 = pow(term2,2) * NminusOn_SqQminusOn;
        errB1H1L0 = pow(term1,2) * NplusOff_SqQplusOff;
        errB1H0L0 = pow(term2,2) * NminusOff_SqQminusOff; 

        asymErSqrLC[p][s] = (errB1H1L1 + errB1H0L1 + errB1H1L0 + errB1H0L0);

        ///adding asymmetry evaluation for laser off data
        qNormAcBkgdAsymNr = qNormCountsLCB1H1L0 - qNormCountsLCB1H0L0;
        qNormAcBkgdAsymDr = qNormCountsLCB1H1L0 + qNormCountsLCB1H0L0;
        qNormAcBkgdAsym = qNormAcBkgdAsymNr/qNormAcBkgdAsymDr;
        term1Bkgd = (1-qNormAcBkgdAsym)/qNormAcBkgdAsymDr;
        term2Bkgd = (1+qNormAcBkgdAsym)/qNormAcBkgdAsymDr;

        errBkgdAsymH1 = pow(term1Bkgd,2) * NplusOff_SqQplusOff;
        errBkgdAsymH0 = pow(term2Bkgd,2) * NminusOff_SqQminusOff;
        bkgdAsymErSqr = errBkgdAsymH1 + errBkgdAsymH0;

        ///addition of bkgdAsym term over various laser cycles
        if (bkgdAsymErSqr > 0.0) {///eqn 4.17(Bevington)
          wmNrBkgdAsym[p][s] += qNormAcBkgdAsym/bkgdAsymErSqr; ///Numerator 
          wmDrBkgdAsym[p][s] += 1.0/bkgdAsymErSqr; ///Denominator 
          if(debug) printf("adding %g(1/bkgdAsymSq) to wmDrBkgdAsym making it: %f\n",1.0/bkgdAsymErSqr,wmDrBkgdAsym[p][s]);
        } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
        else if(bkgdAsymErSqr == 0.0) {
          cout<<red<<"\ncheck if plane "<<p+1<<" strip "<<s+1<<" is MASKED? the bkgdAsymErSqr is non-positive"<<
            "\nThis could also happen due to beamTrip coincident with whole laserOff period"<<normal<<endl;
        } else {
          cout<<"the bkgdAsymErSqr shouldn't be normally negative; check:strip\tcountsLCB1H1L0\tH0L0\n"
            <<s<<"\t"<<newCntsB1H1L0<<"\t"<<newCntsB1H0L0<<"\t"<<
            "\nqNormAcBkgdAsymNr:"<<qNormAcBkgdAsymNr<<" ,Dr:"<<qNormAcBkgdAsymDr<<
            "\nqNormCountsLCB1H1L0:"<<qNormCountsLCB1H1L0<<", H0L0:"<<qNormCountsLCB1H0L0<<
            "\nterm1Bkgd: "<<term1Bkgd<<" ,term2Bkgd:"<<term2Bkgd<<
            "\nerrBkgdAsymH1:"<< errBkgdAsymH1<<" ,\tH0:"<<errBkgdAsymH0<<endl;
          continue;//skip this strip for this laser cycle
        }
        //cout<<red<<"something uncommon in bgd asym of plane "<<p+1<<" strip "<<s+1<<", it has null yield\n"<<normal<<
        //  "laserOff counts for H1 & H0 are "<<qNormCountsLCB1H1L0<<"\t"<<qNormCountsLCB1H0L0<<"\t"<<endl;
        //cout<<red<<"returning from evaluateAsym.C at line "<<__LINE__<<normal<<endl;
        //return -1;//continue;
        if (asymErSqrLC[p][s] >0.0) {///eqn 4.17(Bevington)
          wmNrAsym[p][s] += qNormAsymLC[p][s]/asymErSqrLC[p][s]; ///Numerator 
          wmDrAsym[p][s] += 1.0/asymErSqrLC[p][s]; ///Denominator
          if (debug1) printf("*****adding %g(1/asymSq) to wmDrAsym making it: %f\n",1.0/asymErSqrLC[p][s],wmDrAsym[p][s]);
        } else {
          cout<<"check if plane "<<p+1<<" strip "<<s+1<<" is MASKED? It gives non-positive Asym Er"<<endl;
          printf("errB1H1L1:%f, errB1H0L1:%f, errB1H1L0:%f, errB1H0L0:%f\n",errB1H1L1,errB1H0L1,errB1H1L0,errB1H0L0);
          return -1;
        }
        ///Error evaluation for SUM (in asymmetry)
        erBCqNormSumLCSq = (NplusOn_SqQplusOn+ NminusOn_SqQminusOn+ NplusOff_SqQplusOff+ NminusOff_SqQminusOff);
        ///Error of laser off background for every laser cycle
        erqNormB1L0LasCycSq = (NplusOff_SqQplusOff+NminusOff_SqQminusOff);

        if (erBCqNormSumLCSq >0.0) {
          wmNrBCqNormSum[p][s] += BCqNormSumLC/erBCqNormSumLCSq; ///Numerator eqn 4.17(Bevington)
          wmDrBCqNormSum[p][s] += 1.0/erBCqNormSumLCSq; ///Denominator eqn 4.17(Bevington)
          ///The error for the difference and sum are same, hence reusing the variable
          wmNrBCqNormDiff[p][s] += BCqNormDiffLC/erBCqNormSumLCSq; ///Numerator eqn 4.17(Bevington)
          //wmDrBCqNormDiff[p][s] += 1.0/erBCqNormSumLCSq[p][s]; ///Denominator eqn 4.17(Bevington)
          wmNrqNormB1L0[p][s] += qNormAcBkgdAsymDr/erqNormB1L0LasCycSq; //refer bevington
          wmDrqNormB1L0[p][s] += 1.0/erqNormB1L0LasCycSq; //refer bevington
        } else {
          printf("**Alert: getting non-positive erBCqNormSumLCSq for plane:%d, strip:%d in line:%d\n",p+1,s+1,__LINE__);
          printf("NplusOn_SqQplusOn:%g, NminusOn_SqQminusOn:%g, NplusOff_SqQplusOff:%g, NminusOff_SqQminusOff:%g,\n"
              ,NplusOn_SqQplusOn,NminusOn_SqQminusOn,NplusOff_SqQplusOff,NminusOff_SqQminusOff);
        }
      } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
      else {//the counts should never be negative 
        cout<<red<<"Reporting a strip with equal no.of laser on & off counts. The lasOn could be engulfed in a beamTrip"<<normal<<endl;
        cout<<"plane\tstrip\tnewCntsB1H1L1\t...H1L0\t...H0L1\t...H0L0\n"<<
          p+1<<"\t"<<s+1<<"\t"<<newCntsB1H1L1<<"\t\t"<<newCntsB1H1L0<<"\t"<<newCntsB1H0L1<<"\t"<<newCntsB1H0L0<<endl;
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
  }///for (Int_t p =startPlane; p <endPlane; p++) { 
  cout<<"this nCycle e.g: expAsym[p1][s22]: "<<qNormAsymLC[0][21]<<"+/-"<<TMath::Sqrt(asymErSqrLC[0][21])<<endl;
  return 1;
  }
