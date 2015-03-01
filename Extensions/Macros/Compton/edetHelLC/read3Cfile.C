#include "rootClass.h"
#include "comptonRunConstants.h"
#include "stripMask.C"

Int_t read3Cfile(TString file, std::vector<Double_t> &strip, std::vector<Double_t> &var, std::vector<Double_t> &varEr) {

  Double_t dum1, dum2, dum3; 

  stripMask();///invoke masked strips

  ifstream fIn;
  fIn.open(file);
  if(fIn.is_open()) {
    cout<<"Reading the file "<<file<<endl;
    //cout<<"strip\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
    while( fIn>>dum1>>dum2>>dum3 && fIn.good() ) {
      if (std::find(skipStrip.begin(),skipStrip.end(),dum1)!=skipStrip.end()) {
        //cout<<blue<<"skipping strip "<<dum1<<normal<<endl;
        continue; 
      }
      strip.push_back(dum1),var.push_back(dum2),varEr.push_back(dum3);
      //cout<<blue<<strip.back()<<"\t"<<var.back()<<"\t"<<varEr.back()<<normal<<endl;
    }
    fIn.close();
  } else {
    cout<<red<<"did not find file "<<file<<normal<<endl;
    return -1;
  }

  return 1;
}
