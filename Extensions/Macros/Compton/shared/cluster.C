#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"

//Int_t cluster(
////Input:
//std::vector<Double_t> runlist, std::vector<Double_t> v1, std::vector<Double_t> v1Er, Int_t group,
////Output:
//std::vector<Double_t> wmRuns, std::vector<Double_t> wmV1, std::vector<Double_t> wmV1Er) 
Int_t cluster(Int_t group, std::vector<Double_t> runlist, std::vector<Double_t> v1, std::vector<Double_t> v1Er,std::vector<Double_t> &wmRuns, std::vector<Double_t> &wmV1, std::vector<Double_t> &wmV1Er ) 
{
  for(Int_t i=0; i<(Int_t)runlist.size(); i=i+group) {
    Double_t v1Er_Sq_In =0.0, wmNrPol23V1 =0.0, wmDrPol23V1 =0.0;
    for(Int_t j=0; j<group && i+j<(Int_t)runlist.size(); j++) {
      //cout<<i<<"\t"<<j<<"\t"<<runlist.at(i+j)<<"\t"<<v1.at(i+j)<<"+/-"<<v1Er.at(i+j)<<normal<<endl;
      v1Er_Sq_In = 1.0/pow(v1Er.at(i+j),2);
      wmNrPol23V1 += v1.at(i+j)*v1Er_Sq_In; ///Numerator 
      wmDrPol23V1 += v1Er_Sq_In; ///Denominator
    }
    wmRuns.push_back(runlist.at(i));
    wmV1.push_back(wmNrPol23V1/wmDrPol23V1);
    wmV1Er.push_back(TMath::Sqrt(1.0/wmDrPol23V1));
  }

  //cout<<"size of wmV1 for is "<<wmV1.size()<<endl;
  //cout<<"last value V1: "<<wmRuns.back()<<"\t"<<wmV1.back()<<"+/-"<<wmV1Er.back()<<endl;
  return group;
}
