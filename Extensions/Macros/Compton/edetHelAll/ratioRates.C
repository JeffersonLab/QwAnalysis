#include "rootClass.h"
#include "comptonRunConstants.h"
#include "stripMask.C"
///////////////////////////////////////////////////////////////////////////
Int_t ratioRates(Int_t run1 = 24761, Int_t run2 = 24762, TString dataType = "Ac") {
  ifstream fIn;
  std::vecotr<Double_t> d1;
  TString file;

  filePre = Form("run_%d/edetAll_%d_%s",run1,run1,dataType.Data());///to to make it stand apart from the LC outputs
  std::vector<Double_t> strip, rate1, rate1Er;
  //file = Form("%s/%s/%sqNormCntsB1H0L0P1.txt",pPath,www,filePre.Data());
  file = Form("%s/%s/%sqNormCntsB1H1L0P1.txt",pPath,www,filePre.Data());
  read3Cfile(file, strip, rate1, rate1Er) 

  filePre = Form("run_%d/edetAll_%d_%s", run2, run2, dataType.Data());///to to make it stand apart from the LC outputs
  std::vector<Double_t> rate2, rate2Er;
  //file = Form("%s/%s/%sqNormCntsB1H1L0P1.txt",pPath,www,filePre.Data());
  file = Form("%s/%s/%sqNormCntsB1H0L0P1.txt",pPath,www,filePre.Data());
  read3Cfile(file, d1, rate2, rate2Er) 

  std::vector<Double_t> str, ratio, ratioEr;
  for(int i=0; i< (int)strip.size(); i++) {
    str.push_back(strip.at(i));
    if(rate2.at(i)!=0) {
      ratio.push_back(rate1.at(i)/rate2.at(i));
      ratioEr.push_back( TMath::Sqrt(pow(rate1Er.at(i)/rate2.at(i),2) + pow((rate1.at(i)/(pow(rate2.at(i),2)))*rate2Er.at(i),2)) );
      //printf("%f\t%f\t%f\t%f\t%f\n",strip.at(i), rate1.at(i), rate2.at(i), ratio.back(), ratioEr.back());
    } else {
      cout<<blue<<"strip # "<<i+1<<" must be masked"<<normal<<endl;
      ratio.push_back(1.0);
      ratioEr.push_back(0.0);
    }
  }

  ofstream fOut;
  //fOut.open("ratioOfRatesH1L0.txt");
  fOut.open("ratioOfRatesH0L0.txt");
  if(fOut.is_open()) {
    for(int i=0; i< (int)ratio.size(); i++) {
      cout <<str.at(i) <<"\t"<<ratio.at(i) <<"\t"<<ratioEr.at(i) <<endl;
      fOut <<str.at(i) <<"\t"<<ratio.at(i) <<"\t"<<ratioEr.at(i) <<endl;
    }
    fOut.close();
  }

  return 1;
}
