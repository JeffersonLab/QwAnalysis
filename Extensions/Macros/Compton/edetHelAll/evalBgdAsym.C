#include "rootClass.h"
#include "comptonRunConstants.h"
#include "bgdCorrect.C"
///LC: laser cycle
///BC: background corrected
///*B1H1L1: beam on, helicity plus, laser on; and so on for synonymous suffixs
Int_t evalBgdAsym(Double_t countsLCB1H1L0[], Double_t countsLCB1H0L0[], Double_t qAvgLCH1L0, Double_t qAvgLCH0L0) 
{
  cout<<"starting into evalBgdAsym.C**************"<<endl;
  const Bool_t debug =0;
  Double_t qNormBkgdAsymNr=0.0,qNormBkgdAsymDr=0.0,qNormBkgdAsym=0.0,term1Bkgd=0.0,term2Bkgd=0.0,errBkgdAsymH1=0.0,errBkgdAsymH0=0.0,bkgdAsymErSqr=0.0;
  Double_t qNormCntsLCB1H1L0[nStrips], qNormCntsLCB1H0L0[nStrips];
  Double_t newCntsB1H1L0[nStrips], newCntsB1H0L0[nStrips];
  Double_t NplusOff_SqQplusOff =0.0, NminusOff_SqQminusOff =0.0;
  Double_t erqNormB1L0LasCycSq=0.0;

  for (Int_t s =startStrip; s <endStrip; s++) {	  
    newCntsB1H1L0[s]=countsLCB1H1L0[s] * c2B1H1L0[s];
    newCntsB1H0L0[s]=countsLCB1H0L0[s] * c2B1H0L0[s];
    qNormCntsLCB1H1L0[s]= newCntsB1H1L0[s]/qAvgLCH1L0;
    qNormCntsLCB1H0L0[s]= newCntsB1H0L0[s]/qAvgLCH0L0;
  }

  printf("%d\t%f\t%f\n", 40, qNormCntsLCB1H1L0[40], qNormCntsLCB1H0L0[40]);
  if(kBgdCorrect) {
    cout<<blue<<"applying background correction for flipper effect"<<normal<<endl;
    if (h1GTh0 == 0) bgdCorrect(qNormCntsLCB1H1L0, qNormCntsLCB1H0L0);
    else if (h1GTh0 == 1) bgdCorrect(qNormCntsLCB1H0L0, qNormCntsLCB1H1L0);
  } else cout<<blue<<"not applying background correction"<<normal<<endl;
  printf("%d\t%f\t%f\n", 40, qNormCntsLCB1H1L0[40], qNormCntsLCB1H0L0[40]);

  for (Int_t s =startStrip; s <endStrip; s++) {	  
    ///adding asymmetry evaluation for laser off data
    qNormBkgdAsymNr = qNormCntsLCB1H1L0[s] - qNormCntsLCB1H0L0[s];
    qNormBkgdAsymDr = qNormCntsLCB1H1L0[s] + qNormCntsLCB1H0L0[s];

    if(debug) printf("%d\t%f\t%f\t%f\t%f\n",s+1, countsLCB1H1L0[s], newCntsB1H1L0[s], qAvgLCH1L0, qNormCntsLCB1H1L0[s]);
    if(debug) printf("%d\t%f\t%f\t%f\t%f\n",s+1, countsLCB1H0L0[s], newCntsB1H0L0[s], qAvgLCH0L0, qNormCntsLCB1H0L0[s]);
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
        <<s+1<<"\t"<<newCntsB1H1L0[s]<<"\t"<<newCntsB1H0L0[s]<<"\t"<<
        "\nqNormBkgdAsymNr:"<<qNormBkgdAsymNr<<" ,Dr:"<<qNormBkgdAsymDr<<
        "\nqNormCntsLCB1H1L0[s]:"<<qNormCntsLCB1H1L0[s]<<", H0L0:"<<qNormCntsLCB1H0L0[s]<<
        "\nterm1Bkgd: "<<term1Bkgd<<" ,term2Bkgd:"<<term2Bkgd<<
        "\nerrBkgdAsymH1:"<< errBkgdAsymH1<<" ,\tH0:"<<errBkgdAsymH0<<normal<<endl;
      //continue;//skip this strip for this laser cycle
      return -1;///in background, I can't think why this should ever be negative
    }
    if(debug) printf("for strip:%d, adding %g(1/bkgdAsymErSq) to wmDrBkgdAsym making it: %f\n",s+1,1.0/bkgdAsymErSqr,wmDrBkgdAsym[s]);
  }
  //cout<<red<<"something uncommon in bgd asym of plane "<<plane<<" strip "<<s+1<<", it has null yield\n"<<normal<<
  //  "laserOff counts for H1 & H0 are "<<qNormCntsLCB1H1L0[s]<<"\t"<<qNormCntsLCB1H0L0[s]<<"\t"<<endl;
  //cout<<red<<"returning from evaluateAsym.C at line "<<__LINE__<<normal<<endl;
  //return -1;//continue;
  return 1;
  }
