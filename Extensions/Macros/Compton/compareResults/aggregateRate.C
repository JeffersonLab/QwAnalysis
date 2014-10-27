#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"

//Double_t aggregateRate(Int_t runnum =24519, TString dataType ="Ac", TString fChoose ="YieldP1.txt")
Double_t aggregateRate(Int_t runnum =24503, TString dataType ="Ac", TString fChoose ="LasOffBkgdP1.txt")
//Double_t aggregateRate(Int_t runnum =24503, TString dataType ="Ac", TString fChoose ="qNormCntsB1H0L0P1.txt")
{///this file is expected to have 3 columns
  //TString file = Form("%s/%s/run_%d/edetLC_%d_"+dataType+"%s", pPath, www, runnum, runnum, fChoose.Data());
  TString file = Form("%s/%s/run_%d/edetLC_%d_"+dataType+"%s", pPath, www, runnum, runnum, fChoose.Data());
                  //edetLC_24519_ActNormYieldB1L1P1.txt
  ifstream fIn;
  ofstream fOut;
  std::vector<Double_t> var1, var2, var3;
  Double_t agg =0.0, aggErSq =0.0, aggEr =0.0;

  ///reading in the file to populate the vector
  fIn.open(file);
  if(fIn.is_open()) {
    Double_t d1, d2, d3, d4;
    while(fIn >>d1 >>d2 >>d3 && fIn.good()) {
    //while(fIn >>d1 >>d2 >>d3 >>d4 && fIn.good()) {
      cout<<d1<<"   "<<d2<<"   "<<d3<<endl;
      agg += d2;
      aggErSq += pow(d3,2);
    }
  } //else cout<<red<<"couldn't open "<<file<<endl;

  aggEr = TMath::Sqrt(aggErSq);
                              
  //fOut.open(Form("%s/%s/run_%d/tNormAggRateB1L0_%d_"+dataType+"P%d.txt",pPath,www,runnum,runnum,plane));
  fOut.open(Form("%s/%s/run_%d/%s_%d_"+dataType+"P1.txt",pPath,www,runnum,fChoose.Data(),runnum));//default to plane1
  fOut<<runnum<<"\t"<<agg<<"\t"<<aggEr<<endl;
  fOut.close();
  cout<<"wrote file "<<Form("%s/%s/run_%d/%s_%d_"+dataType+"P1.txt",pPath,www,runnum,fChoose.Data(),runnum)<<endl;
  cout<<blue<<"The aggregate rate for run "<<runnum<<" is: "<<agg<<" +/- "<<aggEr<<" Hz"<<normal<<endl;

  return agg;
}


