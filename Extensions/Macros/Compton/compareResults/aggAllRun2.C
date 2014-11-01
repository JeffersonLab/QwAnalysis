#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "aggregateRate.C"

//const Int_t runBegin = 22737, runEnd = 22740;/// test range
//const Int_t runBegin = 22737, runEnd = 25527;///whole run2 range

Int_t aggAllRun2(TString dataType = "Ac", TString fChoose = "YieldP1.txt")//Int_t run1=runBegin, Int_t run2 = runEnd)
{
  TString file;
  ifstream fIn;

  ///read in the good runs' list
  //file = "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/testSub.temp";
  file = "/w/hallc/compton/users/narayan/my_scratch/data/goodList_longPol.txt";
  Double_t tRun;
  std::vector<Double_t> goodRuns;
  fIn.open(file);
  if(fIn.is_open()) {
    while(fIn>>tRun && fIn.good()) {
      goodRuns.push_back(tRun);
    }
    fIn.close();
  } else cout<<red<<"couldn't open the list of good runs"<<normal<<endl;
  cout<<blue<<goodRuns.size()<<" runs found in the goodList"<<normal<<endl;

  cout<<"aggregating the entries of "<<fChoose<<" of dataType "<<dataType<<" in the goodList"<<endl;
  //for(int i =run1; i <= run2; i++) {
  for(int i =0; i < (Int_t)goodRuns.size(); i++) {
    //aggregateRate(i, "Sc", "tNormYieldB1L0P1.txt");
    //aggregateRate(i, "Sc", "YieldP1.txt");
    //aggregateRate(i, "Ac", "YieldP1.txt");
    //aggregateRate(goodRuns.at(i), "Ac", "LasOffBkgdP1.txt");
    aggregateRate(goodRuns.at(i), dataType, fChoose);
  } 

  //return (run2 - run1);
  return goodRuns.size();
}

/*********Selfie************
 * This macro inputs a file name and assuming it has 3 columns
 * reads in all columns, finds the aggregate of col2 and adds
 * the values in col3 in quadrature taking them as the error.
 * Note: a masked strip will not be a problem in this addition
 * This will be used to get aggregate Yield from
 * edetLC_24519_ActNormYieldB1L1P1.txt type files
 ***************************/
