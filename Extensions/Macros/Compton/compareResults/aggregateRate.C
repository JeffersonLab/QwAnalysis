#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"

Float_t aggregateRate(Int_t runnum =24503, TString dataType ="Ac", TString fChoose ="YieldP1.txt")
//Float_t aggregateRate(Int_t runnum =24503, TString dataType ="Ac", TString fChoose ="qNormCntsB1H0L0P1.txt")
{///this file is expected to have 3 columns
  //TString destDir = Form("%s/txt_ver001_28Oct14/r%d/", pPath, runnum);
  TString destDir = Form("%s/txt_ver002_30Oct14/r%d/", pPath, runnum);

  TString file = destDir + Form("edetLCNoDT_%d_"+dataType+"%s", runnum, fChoose.Data());
  //TString file = destDir + Form("edetLC_%d_"+dataType+"%s", runnum, fChoose.Data());
                  
  Bool_t debug =0, debug1 =0;;
  ifstream fIn;
  ofstream fOut;
  std::vector<Float_t> var1, var2, var3;
  Float_t agg =0.0, aggErSq =0.0, aggEr =0.0;

  ///reading in the file to populate the vector
  fIn.open(file);
  if(fIn.is_open()) {
    Float_t d1, d2, d3;
    while(fIn >>d1 >>d2 >>d3 && fIn.good()) {
      if(debug1) cout<<d1<<"   "<<d2<<"   "<<d3<<endl;
      agg += d2;
      aggErSq += pow(d3,2);
    }
  } else {
    if(debug) cout<<red<<"couldn't open "<<file<<endl;
    return -1;
  }

  aggEr = TMath::Sqrt(aggErSq);
                              
  file = destDir + Form("Agg"+dataType+"NoDT_%d_%s",runnum,fChoose.Data());
  //file = destDir + Form("Agg"+dataType+"_%d_%s",runnum,fChoose.Data());
  fOut.open(file);
  fOut<<runnum<<"\t"<<agg<<"\t"<<aggEr<<endl;
  fOut.close();
  if(debug1) cout<<"wrote "<<file<<endl;
  if(debug) cout<<blue<<"The aggregate rate for run "<<runnum<<" is: "<<agg<<" +/- "<<aggEr<<" Hz"<<normal<<endl;

  return agg;
}


