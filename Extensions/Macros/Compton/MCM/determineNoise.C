#include "rootClass.h"
#include "comptonRunConstants.h"
///////////////////////////////////////////////////////////////////////////
Int_t determineNoise(Int_t runnum, Double_t strip[], TString dataType = "Ac") {
  cout<<blue<<"\nFinding the correct file for Noise subtraction\n"<<normal<<endl;

  Double_t tB0H1L1, tB0H1L0, tB0H0L1, tB0H0L0, tBeamOff;
  Double_t lasPowB0H1L1, lasPowB0H1L0, lasPowB0H0L1, lasPowB0H0L0, tBeamOn;
  Double_t rateB0H1L1[nStrips], rateB0H1L0[nStrips], rateB0H0L1[nStrips], rateB0H0L0[nStrips];
  ifstream fIn;
  TString file;
  //Float_t runDum, tDum;
  Float_t noiseDum, runletDum;
  
  //if(dataType =="Ac") file = Form("%s/data/noisecorr_run.dat", pPath) ; 
  //else if(dataType =="Sc") file = Form("%s/data/sing_noisecorr_run.dat", pPath);
  //else {
  //  cout<<red<<"dataType not known hence exiting determineNoise.C"<<normal<<endl;
  //  return -1;
  //}
  //fIn.open(file);
  //if (fIn.is_open()) {
  //  while(1) {
  //    fIn >>runDum >>tDum >>noiseDum >>runletDum;
  //    //cout<< runDum <<"   "<<tDum<<"   " <<noiseDum<<"   " <<runletDum<<endl;
  //    if(fIn.eof()) {
  //      cout<<red<<"\n\nexiting the rate file because encountered the end of file\n\n"<<normal<<endl;///shouldn't happen
  //      fIn.close();
  //      return -1;
  //    } else if(runDum == runnum) {
  //      if(dataType =="Ac") cout<<blue<<"Use noise from beamoff_"<<noiseDum<<"."<<runletDum<<".dat for noise correction of run "<<runnum<<normal<<endl; 
  //      else cout<<blue<<"Use noise from singles_boff_"<<noiseDum<<"."<<runletDum<<".dat for noise correction of run "<<runnum<<normal<<endl; 
  //      fIn.close();
  //      break;
  //    }
  //  }
  //  fIn.close();
  //} else {
  //  cout<<red<<"couldn't open the "<<file<<normal<<endl;
  //  return -1;
  //}
  //cout<<red<<"temporarily using run 25286 for noise correction to ALL runs"<<normal<<endl;
  noiseDum = 25286;
  runletDum = 0;
  if(dataType =="Ac") cout<<blue<<"Use beamoff_"<<noiseDum<<"."<<runletDum<<".dat for noise correction of run "<<runnum<<normal<<endl; 
  else cout<<blue<<"Use singles_boff_"<<noiseDum<<"."<<runletDum<<".dat for noise correction of run "<<runnum<<normal<<endl; 
  if(dataType =="Ac") file = Form("%s/data/beamOffRates/beamoff_%d.%d.dat", pPath, (Int_t)noiseDum, (Int_t)runletDum);
  else file = Form("%s/data/beamOffRates/singles_boff_%d.%d.dat", pPath, (Int_t)noiseDum, (Int_t)runletDum);
  fIn.open(file);
  if(fIn.is_open()) {
    cout<<blue<<"explicit noise subtraction"<<normal<<endl; 
    fIn >>tB0H1L1 >>tB0H1L0 >>tB0H0L1 >>tB0H0L0 >>tBeamOff;
    fIn >>lasPowB0H1L1 >>lasPowB0H1L0 >>lasPowB0H0L1 >>lasPowB0H0L0 >>tBeamOn;
    timeB0 = tB0H1L1+tB0H1L0+tB0H0L1+tB0H0L0;
    for(Int_t s=0; s<endStrip; s++) {
      fIn>>rateB0H1L1[s]>>rateB0H1L0[s]>>rateB0H0L1[s]>>rateB0H0L0[s]>>strip[s];
      rateB0[s]= (rateB0H1L1[s]+rateB0H1L0[s]+rateB0H0L1[s]+rateB0H0L0[s])*4.0/timeB0;
      //printf("%f\t%f\t%f\t%f\t%f\n",rateB0H1L1[s],rateB0H1L0[s],rateB0H0L1[s],rateB0H0L0[s],strip[s]);
      //printf("%f\t%f\n",strip[s],rateB0[s]);
    }
    fIn.close();
  } else cout<<red<<"couldn't open the "<< file<<normal<<endl;

  return 1;
}
