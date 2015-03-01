#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/stripMask.C"

Int_t erWeightedMean(TString file="/w/hallc/compton/users/narayan/my_scratch/www/run_24917/edetLCNoDT_24917_AcYieldP1.txt")
{
  TString destDir = Form("%s/%s/",pPath,www);
  cout<<"setting "<<destDir<<" as the local directory"<<endl;
  ifstream fIn;
  ofstream fOut;
  std::vector<Double_t> var1, var2, var3;
  Double_t wmNr =0.0, wmDr =0.0, erSq =0.0;
  Double_t mean, meanEr;

  stripMask();//to identify which strips are masked
  ///reading in the file to populate the vector
  fIn.open(file);
  if(fIn.is_open()) {
    Double_t d1, d2, d3, d4;
    while(fIn >>d1 >>d2 >>d3 >>d4 && fIn.good()) {
      //cout<<d1<<"   "<<d2<<"   "<<d3<<endl;
      var1.push_back(d1);
      var2.push_back(d2);
      var3.push_back(d3);
    }
  } else cout<<red<<"couldn't open "<<file<<endl;

  ///Weighted mean of the 2nd column of vector using 3rd column as error weight
  for(int i=0; i<(int)var2.size(); i++) {
    if(std::find(skipStrip.begin(), skipStrip.end(), var1.at(i)) != skipStrip.end()) continue;
    else if(var3.at(i) <= 0.0) {
      cout<<red<<"the unmasked strip "<<var1.at(i)<<" has zero yield"<<normal<<endl;
      return -1;
    //} else if(i<=var1.at(50)) {
    } else {
      erSq = 1.0/pow(var3.at(i),2);
      wmNr += var2.at(i) * erSq;//Numerator of weighted mean
      wmDr += erSq;//Denominator of weighted mean
      cout<<var1.at(i)<<"   "<<erSq<<"   "<<wmNr<<"   "<<wmDr<<endl;
    }
  }
  mean = wmNr/wmDr; 
  meanEr = TMath::Sqrt(1.0/wmDr);
  cout<<"The mean is: "<<mean<<"+/-"<<meanEr<<endl;

  return 1;
}
/*********Selfie************
 * This macro inputs a file name and assuming it has 4 columns
 * reads in all columns, takes an error weighted mean of col2
 * using corresponding values in col3 as the error
 * Note: only the "same" quantity should be added in error weighted average
 ***************************/
