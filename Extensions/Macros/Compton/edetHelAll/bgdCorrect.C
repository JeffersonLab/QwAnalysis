#include "rootClass.h"
#include "comptonRunConstants.h"
#include "stripMask.C"
#include "read3Cfile.C"

///////////////////////////////////////////////////////////////////////////
Int_t bgdCorrect(Double_t rate2[], Double_t rate4[]) {
  cout<<blue<<"\nNoise subtraction turned ON for this analysis\n"<<normal<<endl;
  Double_t d1, d2, d3;
  TString file;
  ifstream fIn;

  std::vector<Double_t> str, corrH1, corrH1Er;
  file = "ratioOfRatesH1L0.txt";
  read3Cfile(file, str, corrH1, corrH1Er) 

  std::vector<Double_t> dum, corrH0, corrH0Er;
  file = "ratioOfRatesH0L0.txt";
  read3Cfile(file, dum, corrH0, corrH0Er) 

  Int_t trSt=0;
  printf("%d\t%f\t%f\t%f\t%f\n",41, rate2[40], rate4[40], corrH1[40], corrH0[40]);
  for (Int_t s =startStrip; s <endStrip; s++) {	  
    if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    //rate2[s] = rate2[s]/ (corrH1.at(trSt) );//same helicity
    rate2[s] = rate2[s]/ (corrH0.at(trSt) );//opposite helicity

    //rate4[s] = rate4[s]/ (corrH0.at(trSt) );//same helicity
    rate4[s] = rate4[s]/ (corrH1.at(trSt) );//opposite helicity
    trSt ++;
  }
  printf("%d\t%f\t%f\t%f\t%f\n",41, rate2[40], rate4[40], corrH1[40], corrH0[40]);

  return 1;
}
