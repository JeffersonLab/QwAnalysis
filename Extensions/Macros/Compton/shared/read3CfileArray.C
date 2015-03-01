#ifndef __READFILEARRAY_H
#define __READFILEARRAY_H

#include "rootClass.h"
#include "comptonRunConstants.h"

//Int_t read3CfileArray(TString file, Double_t strip[], Double_t var[], Double_t varEr[]) {
Int_t read3CfileArray(TString file, Double_t* strip, Double_t* var, Double_t* varEr) {

  ifstream fIn;
  fIn.open(file);
  if(fIn.is_open()) {
    cout<<"Reading the file "<<file<<endl;
    //cout<<"strip\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
    for(int s=0; s<nStrips && fIn.good(); s++) {
        fIn>>strip[s] >>var[s] >>varEr[s];
        //cout<<blue<<strip[s]<<"\t"<<var[s]<<"\t"<<varEr[s]<<normal<<endl;
    }
    fIn.close();
  } else {
    cout<<red<<"did not find file "<<file<<normal<<endl;
    return -1;
  }

  return 1;
}

#endif

