#include "rootClass.h"
#include "comptonRunConstants.h"
#include "stripMask.C"
///////////////////////////////////////////////////////////////////////////
Int_t noiseCorrect(Double_t rate1[], Double_t rate2[], Double_t rate3[], Double_t rate4[], Int_t nHelLCB1L1, Int_t nHelLCB1L0) {
  cout<<blue<<"\nNoise subtraction turned ON for this analysis\n"<<normal<<endl;
  Double_t tB0H1L1,tB0H1L0,tB0H0L1,tB0H0L0,tBeamOff;
  Double_t lasPowB0H1L1, lasPowB0H1L0, lasPowB0H0L1, lasPowB0H0L0, tBeamOn;
  Double_t rateB0H1L1[nStrips],rateB0H1L0[nStrips],rateB0H0L1[nStrips],rateB0H0L0[nStrips],strip[nStrips];
  ifstream fIn;

  fIn.open(Form("%s/data/beamoff_24752.0.dat",pPath));
  if(fIn.is_open()) {
    cout<<blue<<"explicit noise subtraction"<<normal<<endl; 
    fIn>>tB0H1L1>>tB0H1L0>>tB0H0L1>>tB0H0L0>>tBeamOff;
    fIn>>lasPowB0H1L1>>lasPowB0H1L0>>lasPowB0H0L1>>lasPowB0H0L0>>tBeamOn;
    timeB0 = tB0H1L1+tB0H1L0+tB0H0L1+tB0H0L0;
    for(Int_t s=0;s<endStrip;s++) {
      fIn>>rateB0H1L1[s]>>rateB0H1L0[s]>>rateB0H0L1[s]>>rateB0H0L0[s]>>strip[s];
      rateB0[s]= (rateB0H1L1[s]+rateB0H1L0[s]+rateB0H0L1[s]+rateB0H0L0[s])*4.0/timeB0;
      //printf("%f\t%f\t%f\t%f\t%f\n",rateB0H1L1[s],rateB0H1L0[s],rateB0H0L1[s],rateB0H0L0[s],strip[s]);
      //printf("%f\t%f\n",strip[s],rateB0[s]);
    }
  } else cout<<red<<"couldn't open the beam off file"<<normal<<endl;
  
  ///// Modification due to explicit (electronic) noise subtraction /////      
  for (Int_t s =startStrip; s <endStrip; s++) {	  
    if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    ///the noiseSubtraction assumes that the +ve and -ve helicities are equal in number, hence dividing by 2
    rate1[s]=rate1[s]-rateB0[s]*(nHelLCB1L1/2.0)/helRate;
    rate2[s]=rate2[s]-rateB0[s]*(nHelLCB1L0/2.0)/helRate;
    rate3[s]=rate3[s]-rateB0[s]*(nHelLCB1L1/2.0)/helRate;
    rate4[s]=rate4[s]-rateB0[s]*(nHelLCB1L0/2.0)/helRate;
    //printf("%d\t%f\t%f\t%f\t%f\t%d\t%d\n",s+1, rate1[s], rate2[s], rate3[s], rate4[s], nHelLCB1L1, nHelLCB1L0);
  }
  //////////////

  return 1;
}
