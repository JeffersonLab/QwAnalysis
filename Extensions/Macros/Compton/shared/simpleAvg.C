#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"

Int_t simpleAvg(
//Input
Int_t group, std::vector<Double_t> runList, std::vector<Double_t> val1, 
//Output
std::vector<Double_t> &groupList, std::vector<Double_t> &groupVal1_33) 
{
  for(Int_t r=0; r<(Int_t)runList.size(); r=r+group) {
    Double_t runningSumV1 =0.0;
    for(Int_t j=0; j<group && r+j<(Int_t)runList.size(); j++) {
      runningSumV1 += val1.at(r+j);
    }
    groupList.push_back(runList.at(r));
    groupVal1_33.push_back((Double_t)runningSumV1/group);
  }
  //cout<<blue<<"size of groupList : "<<groupList.size()<<endl;
  return group;
}
