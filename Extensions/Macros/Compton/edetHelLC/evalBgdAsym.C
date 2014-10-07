#include "rootClass.h"
#include "comptonRunConstants.h"
#include "rhoToX.C"
///LC: laser cycle
///BC: background corrected
///*B1H1L1: beam on, helicity plus, laser on; and so on for synonymous suffixs
Int_t evalBgdAsym(Int_t countsLCB1H1L0[], Int_t countsLCB1H0L0[], Double_t qAvgLCH1L0, Double_t qAvgLCH0L0) 
{
  cout<<"starting into evalBgdAsym.C**************"<<endl;
  const Bool_t debug =0;
  Double_t qNormBkgdAsymNr=0.0,qNormBkgdAsymDr=0.0,qNormBkgdAsym=0.0,term1Bkgd=0.0,term2Bkgd=0.0,errBkgdAsymH1=0.0,errBkgdAsymH0=0.0,bkgdAsymErSqr=0.0;
  Double_t qNormCountsLCB1H1L0 =0.0, qNormCountsLCB1H0L0=0.0;
  Double_t newCntsB1H1L0 =0.0, newCntsB1H0L0 =0.0;
  Double_t NplusOff_SqQplusOff =0.0, NminusOff_SqQminusOff =0.0;
  Double_t erqNormB1L0LasCycSq=0.0;

  for (Int_t s =startStrip; s <endStrip; s++) {	  
    newCntsB1H1L0=countsLCB1H1L0[s] * c2B1H1L0[s];
    newCntsB1H0L0=countsLCB1H0L0[s] * c2B1H0L0[s];
    qNormCountsLCB1H1L0= newCntsB1H1L0/qAvgLCH1L0;
    qNormCountsLCB1H0L0= newCntsB1H0L0/qAvgLCH0L0;

    ///adding asymmetry evaluation for laser off data
    qNormBkgdAsymNr = qNormCountsLCB1H1L0 - qNormCountsLCB1H0L0;
    qNormBkgdAsymDr = qNormCountsLCB1H1L0 + qNormCountsLCB1H0L0;

    if(debug) printf("%d\t%d\t%f\t%f\t%f\n",s+1, countsLCB1H1L0[s], newCntsB1H1L0, qAvgLCH1L0, qNormCountsLCB1H1L0);
    if(debug) printf("%d\t%d\t%f\t%f\t%f\n",s+1, countsLCB1H0L0[s], newCntsB1H0L0, qAvgLCH0L0, qNormCountsLCB1H0L0);
    ///addition of bkgdAsym term over various laser cycles
    if (qNormBkgdAsymDr > 0.0) {///eqn 4.17(Bevington)
      qNormBkgdAsym = qNormBkgdAsymNr/qNormBkgdAsymDr;
      term1Bkgd = (1-qNormBkgdAsym)/qNormBkgdAsymDr;
      term2Bkgd = (1+qNormBkgdAsym)/qNormBkgdAsymDr;
      NplusOff_SqQplusOff  = countsLCB1H1L0[s] /(qAvgLCH1L0*qAvgLCH1L0);
      NminusOff_SqQminusOff= countsLCB1H0L0[s] /(qAvgLCH0L0*qAvgLCH0L0);
      errBkgdAsymH1 = pow(term1Bkgd,2) * NplusOff_SqQplusOff;
      errBkgdAsymH0 = pow(term2Bkgd,2) * NminusOff_SqQminusOff;
      bkgdAsymErSqr = errBkgdAsymH1 + errBkgdAsymH0;

      wmNrBkgdAsym[s] += qNormBkgdAsym/bkgdAsymErSqr; ///Numerator 
      wmDrBkgdAsym[s] += 1.0/bkgdAsymErSqr; ///Denominator 
      ///Error of laser off background for every laser cycle
      erqNormB1L0LasCycSq = (NplusOff_SqQplusOff + NminusOff_SqQminusOff);
      wmNrqNormB1L0[s] += qNormBkgdAsymDr / erqNormB1L0LasCycSq; //refer bevington
      wmDrqNormB1L0[s] += 1.0 / erqNormB1L0LasCycSq; //refer bevington

    } else if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    //else if(bkgdAsymErSqr == 0.0) {
    //  cout<<red<<"\ncheck if plane "<<plane<<" strip "<<s+1<<" is MASKED? the bkgdAsymErSqr is non-positive"<<
    //    "\nThis could also happen due to beamTrip coincident with whole laserOff period"<<normal<<endl;
    else {
      cout<<red<<"the bkgdAsymErSqr shouldn't be normally negative; check:strip\tcountsLCB1H1L0\t..H0L0\n"
        <<s<<"\t"<<newCntsB1H1L0<<"\t"<<newCntsB1H0L0<<"\t"<<
        "\nqNormBkgdAsymNr:"<<qNormBkgdAsymNr<<" ,Dr:"<<qNormBkgdAsymDr<<
        "\nqNormCountsLCB1H1L0:"<<qNormCountsLCB1H1L0<<", H0L0:"<<qNormCountsLCB1H0L0<<
        "\nterm1Bkgd: "<<term1Bkgd<<" ,term2Bkgd:"<<term2Bkgd<<
        "\nerrBkgdAsymH1:"<< errBkgdAsymH1<<" ,\tH0:"<<errBkgdAsymH0<<normal<<endl;
      //continue;//skip this strip for this laser cycle
      return -1;///in background, I can't think why this should ever be negative
    }
    if(debug) printf("for strip:%d, adding %g(1/bkgdAsymErSq) to wmDrBkgdAsym making it: %f\n",s+1,1.0/bkgdAsymErSqr,wmDrBkgdAsym[s]);
  }
  //cout<<red<<"something uncommon in bgd asym of plane "<<plane<<" strip "<<s+1<<", it has null yield\n"<<normal<<
  //  "laserOff counts for H1 & H0 are "<<qNormCountsLCB1H1L0<<"\t"<<qNormCountsLCB1H0L0<<"\t"<<endl;
  //cout<<red<<"returning from evaluateAsym.C at line "<<__LINE__<<normal<<endl;
  //return -1;//continue;
  return 1;
  }
