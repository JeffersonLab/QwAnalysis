#include <rootClass.h>
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/comptonRunConstants.h"
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/rhoToX.C"
const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
Int_t reCreateRLfiles(Int_t run1=runBegin, Int_t run2=runEnd) {
  const Bool_t debug=0;
  ifstream fileAsymRL;
  ofstream fileReWrite;
  const char *asymRLDir = "/u/home/narayan/acquired/vladas/FOR-AMRENDRA";
  const char *destDir="/w/hallc/compton/users/narayan/runletCompare/asymRL_Apr2013";
  Double_t stripRL[nStrips],asymRL[nStrips],asymRLEr[nStrips];
  for (Int_t r=run1; r<=run2; r++) {
  //for (Int_t r=run1; r<=(run1); r++) {
  Int_t c1=0;///count the no.of lines read
  Int_t c2=0;///count the should-be strip number
    fileAsymRL.open(Form("%s/as.plane.1.run.%d",asymRLDir,r));
    if(fileAsymRL.is_open()) {
      if(debug) cout<<"opened "<<Form("%s/as.plane.1.run.%d",asymRLDir,r)<<endl;
      fileReWrite.open(Form("%s/run_%d_asymRL.txt",destDir,r));
      while(fileAsymRL.good()) {
        fileAsymRL>>stripRL[c1]>>asymRL[c1]>>asymRLEr[c1];
        if(debug) cout<<"temp1 "<<blue<<c1<<"\t"<<stripRL[c1]<<"\t"<<asymRL[c1]<<"\t"<<asymRLEr[c1]<<normal<<endl;
        if(stripRL[c1]==c2+1) {
          fileReWrite<<stripRL[c1]<<"\t"<<asymRL[c1]<<"\t"<<asymRLEr[c1]<<endl;///regular case
        } else {//case of missing strip
          fileReWrite<<Form("%d\t%f\t%f\n",(c2+1),0.0,0.0);///fill in the missing strip
          fileReWrite<<stripRL[c1]<<"\t"<<asymRL[c1]<<"\t"<<asymRLEr[c1]<<endl;///write in what was read
          c2++; ///here onwards the offset will be of +1 strip
          if(debug) cout<<"fixed the absence of strip "<<c2<<endl;
        }
        c2++;///this is how it would be with no missing strips
        if(stripRL[c1]==64 || fileAsymRL.eof()) break;
        c1++;//counting no.of runs being compared finally
      }
      fileAsymRL.close();
      fileReWrite.close();
    } else if(debug) cout<<blue<<"runletAsym file for run "<<r<<" couldn't be opened"<<normal<<endl;
  }
  return 1;//(run2 - run1+1);//the number of runs processed
}

