#ifndef __WRITEFILEARRAY_H
#define __WRITEFILEARRAY_H

#include "rootClass.h"
//#include "comptonRunConstants.h"

Int_t write3CfileArray(TString file, Double_t strip[], Double_t var[], Double_t varEr[]) {

  ofstream fOut;
  fOut.open(file);
  if(fOut.is_open()) {
    cout<<"Writing file "<<file<<endl;
    //cout<<"strip\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
    for(int s=0; s<nStrips; s++) {
        fOut <<strip[s]<<"\t"<<var[s] <<"\t"<<varEr[s] <<endl;
        //cout<<blue<<strip[s]<<"\t"<<var[s]<<"\t"<<varEr[s]<<normal<<endl;
    }
    fOut.close();
  } else {
    cout<<red<<"couldn't open file to write "<<file<<normal<<endl;
    return -1;
  }

  return 1;
}

#endif

