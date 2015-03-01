#include "rootClass.h"
#include "comptonRunConstants.h"
#include "read3CfileArray.C"
///////////////////////////////////////////////////////////////////////////
Int_t ratioRates(Int_t run1 = 24761, Int_t run2 = 24762, TString f1 ="AcqNormCntsB1H1L0P1") {
//Int_t ratioRates(Int_t run1 = 24761, Int_t run2 = 24762, TString f1 ="AcqNormCntsB1H0L0P1") {
  ifstream fIn;
  TString file;
  Double_t strip[nStrips], rate1[nStrips], rate1Er[nStrips];
  Double_t dum[nStrips], rate2[nStrips], rate2Er[nStrips];

  filePre = Form("run_%d_NoBgdSub/edetAll_%d_",run1,run1);///to to make it stand apart from the LC outputs
  file = Form("%s/%s/%s"+f1+".txt",pPath,www,filePre.Data());
  read3CfileArray(file, strip, rate1, rate1Er); 

  filePre = Form("run_%d/edetAll_%d_", run2, run2);///to to make it stand apart from the LC outputs
  file = Form("%s/%s/%s"+f1+".txt",pPath,www,filePre.Data());
  read3CfileArray(file, dum, rate2, rate2Er);

  //forming the ratio using the above two files
  Double_t ratio[nStrips], ratioEr[nStrips];
  for(int s=0; s<nStrips; s++) {
    if(rate2[s]!=0) {
      ratio[s] = (rate1[s]/rate2[s]);
      ratioEr[s] = TMath::Sqrt(pow(rate1Er[s]/rate2[s],2) + pow((rate1[s]/(pow(rate2[s],2)))*rate2Er[s],2));
      //printf("%f\t%f\t%f\t%f\t%f\n",strip[s], rate1[s], rate2[s], ratio[s], ratioEr[s]);
    } else {
      cout<<blue<<"strip # "<<s+1<<" must be masked"<<normal<<endl;
      ratio[s] = 1.0;
      ratioEr[s] = 0.0;
    }
  }
  ofstream fOut;
  file = Form("ratio_"+f1+"_%d_%d.txt",run1,run2);

  fOut.open(file);
  if(fOut.is_open()) {
    for(int s=0; s< nStrips; s++) {
      //cout <<str[s] <<"\t"<<ratio[s] <<"\t"<<ratioEr[s] <<endl;
      fOut <<strip[s] <<"\t"<<ratio[s] <<"\t"<<ratioEr[s] <<endl;
    }
    fOut.close();
  }
  cout<<"wrote file "<<file<<endl;

  //file = Form("%s/%s/%sqNormCntsB1H0L0P1.txt",pPath,www,filePre.Data());
  //file = Form("%s/%s/%sqNormCntsB1H0L0P1.txt",pPath,www,filePre.Data());
  //file = Form("ratioOfRatesH1L0_%d.txt",run1);
  return 1;
}

///This macro should function as an independent module. It reads in two files each of 3 column width, 
//..forms a ratio of the 2nd column using the 3rd column as error
//..writes the ratio into a file in a local directory
