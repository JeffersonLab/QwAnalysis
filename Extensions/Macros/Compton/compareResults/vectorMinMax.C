#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
Int_t vectorMinMax(
    //input
    std::vector<Double_t> v,
    //std::vector<Double_t> e,  
    //output
    Double_t &low, Double_t &high)
{
  if(v.size()<=0) {
    cout<<"Empty vector, can't do anything"<<endl; 
    return 1; 
  }
  low =0.0, high =0.0;

  for(Int_t i = 0; i < (int)v.size(); i++ ) {                                      
    if( v[i] < low ) low = v[i];
    if( v[i] > high) high = v[i];                                                            
  }

  return high;
}
