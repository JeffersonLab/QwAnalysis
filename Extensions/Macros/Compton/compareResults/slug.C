#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
////This macro takes in a list of vectors for run# and pol% as the basic input. 
//These are used to cluster the 2nd set of inputs, (a parameter and its error) slug by slug.
//The parameter being clustered could be pol% itself. The parameter to be
//clustered should be of the same vector length as the pol vector. The macro
//finally ouputs 6 vectors, run#, parameter and its error for one HWP state,
//while another set of run#, parameter and its error for another HWP state
Int_t slug(//Input vectors
        std::vector<Double_t> runlist, std::vector<Double_t> pol, std::vector<Double_t> v1, std::vector<Double_t> v1Er,
        //output vectors
        std::vector<Double_t> &wmRunIn, std::vector<Double_t> &wmV1In, std::vector<Double_t> &wmV1InEr,
        std::vector<Double_t> &wmRunOut, std::vector<Double_t> &wmV1Out, std::vector<Double_t> &wmV1OutEr,
        Bool_t bInv = 1 ) 
{
    //for(Int_t i=0; (i+group)<(Int_t)runlist.size(); i=i+group) {///execute only if no.of constituents >= group
    std::vector<Double_t> slugRuns, slugV1, slugV1Er;
    Double_t base, polSum =0.0;
    std::vector<Double_t> polAvg;
    Int_t j=0;///setting to 0 for the first loop
    cout<<"size of runlist "<<runlist.size()<<endl;
    for(Int_t i=0; (i)<(Int_t)runlist.size(); i=i+j) {///execute even if no.of constituents < group 
        Double_t v1Er_Sq_In =0.0, wmNrV1 =0.0, wmDrV1 =0.0;
        //cout<<runlist[i]<<"   "<<pol[i]<<endl;
        base = pol[i];
        polSum = 0.0;
        j=0;
        while(pol[i+j]/base > 0) {//same sign
            if(v1Er.at(i+j) != 0.0) {//to skip an ocassional zero measurement
                v1Er_Sq_In = 1.0/pow(v1Er.at(i+j),2);
                wmNrV1 += v1.at(i+j)*v1Er_Sq_In; ///Numerator 
                wmDrV1 += v1Er_Sq_In; ///Denominator
                polSum += pol[i+j];
            } else cout<<red<<"skipping run "<<runlist[i+j]<<", pol%=0"<<normal<<endl;
            j++;
            if ((i+j) >= (int) runlist.size()) break;
        }
        polAvg.push_back(polSum/j);
        slugRuns.push_back(runlist.at(i));
        slugV1.push_back(wmNrV1/wmDrV1);
        slugV1Er.push_back(TMath::Sqrt(1.0/wmDrV1));
    }
    //for(int i=0; i<(int)slugRuns.size(); i++) { //to check
    //    printf("%5.0f\t%.2f\t%.2f\t%.2f\n",slugRuns[i], polAvg[i], slugV1[i], slugV1Er[i]);
    //}

    for(int i=0; i<(int)slugRuns.size(); i++) { //to check
        if(polAvg[i] > 0) {
            wmRunIn.push_back(slugRuns[i]);
            wmV1In.push_back(slugV1[i]);
            wmV1InEr.push_back(slugV1Er[i]);
        } else {
            wmRunOut.push_back(slugRuns[i]);
            if(bInv) wmV1Out.push_back(-slugV1[i]);//making the sign positive to easily plot them
            else wmV1Out.push_back(slugV1[i]);//this must be a quantity that doesn't change sign with pol
            wmV1OutEr.push_back(slugV1Er[i]);
        }
    }

    //cout<<"size of slugV1 for is "<<slugV1.size()<<endl;
    //cout<<"last value V1: "<<slugRuns.back()<<"\t"<<slugV1.back()<<"+/-"<<slugV1Er.back()<<endl;
    return (Int_t)slugRuns.size();
}
