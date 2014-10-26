#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"

Int_t simpleAvg(
//Input
Int_t group, std::vector<Double_t> runList, std::vector<Double_t> val1, 
//Output
std::vector<Double_t> &groupList, std::vector<Double_t> &groupVal1) 
{
  for(Int_t r=0; (r+group)<(Int_t)runList.size(); r=r+group) {///to ensure that the loop isn't executed if there aren't enough elements to average
    Double_t runningSumV1 =0.0;
    for(Int_t j=0; j<group && r+j<(Int_t)runList.size(); j++) {
      runningSumV1 += val1.at(r+j);
    }
    groupList.push_back(runList.at(r));
    groupVal1.push_back((Double_t)runningSumV1/group);
    //cout<<r<<"\t"<<runningSumV1<<"\t"<<groupList.back()<<"\t"<<groupVal1.back()<<endl;
  }
  //cout<<blue<<"size of groupList : "<<groupList.size()<<endl;
  return group;
}
