#include "/w/hallc/compton/users/narayan/archive/rootClass.h"

Int_t readFortOut(
//input file
TString file,
//to be processed vectors
std::vector<Double_t> &runL, std::vector<Double_t> &rates1, std::vector<Double_t> &rates2, std::vector<Double_t> &rates3, std::vector<Double_t> &rates4, 
//limiting runs
Int_t run1, Int_t run2) 
{
  ifstream fRates;
  //fRates.open("/w/hallc/compton/users/narayan/my_scratch/data/aggregate_ratescan.dat");
  fRates.open(file);
  if(fRates.is_open()) {
    Double_t rate1, rate2, rate3, rate4, runn;
    while(fRates>>runn>>rate1>>rate2>>rate3>>rate4 && fRates.good()) {
      if(runn>run1 && runn<run2) {
        runL.push_back(runn);
        rates1.push_back(rate1);
        rates2.push_back(rate2);
        rates3.push_back(rate3);
        rates4.push_back(rate4);
        //cout<<runL.back()<<"  "<<rates4.back()<<endl;
      }
    }
    fRates.close();
  } else cout<<red<<"could not open "<<file<<normal<<endl;
  return 1;
}
