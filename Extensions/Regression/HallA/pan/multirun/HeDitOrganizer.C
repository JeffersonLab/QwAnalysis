
#include <multirun/HeDitOrganizer.h>


TCut getDitCutHe_byIndex(Int_t ind) {
  // you tell me what "superslug" you are in (with regards
  // to dithering) and I'll give you the cut you need to 
  // clean up your averaging.  Mainly, this should be used
  // by the AvgSlope routine.
  

  TCut superslugselect[2];
  superslugselect[0] = TCut("run<4587 || run>4600");
  superslugselect[1] = TCut("run>=4587 && run<=4600");  // Slugs 39-41

  TCut dcut;
  if (ind>=0 && ind<=1) {
    dcut = superslugselect[ind];
    dcut += "abs(runmod-5068.5)>0.2";
    dcut += "(run<4151 || run>4159)";
    dcut += "slug>3";
    dcut += "lostCount<20";
  } else {
    dcut = "0";
  }
  return dcut;
}


Int_t getDitSetHe(Int_t run) {
  // give me a run number, and I'll give you the index
  // of the run average file that you want to use. Easy!
  Int_t ind;
  if (run<4587 || run>4600) {
    ind=0;
  } else if(run>=4587 && run<=4600) {
    ind = 1;
  } else {
    ind=-1;
  }
  return ind;
}

TCut getDitCutHe_byRun(Int_t run) {
  // give me a run number, and I'll give you a cut which should
  // be applied to the dither slope summary file.  Mainly, this
  // should be used by the AvgSlope routine

  Int_t ind = getDitSetHe(run);
  TCut dcut = getDitCutHe_byIndex(ind);
  return dcut;
}


TString getDitSetFilenameHe(Int_t run) {
  // give me a run number, and I'll give you the NAME
  // of the run average file that you want to use.  Even easier!!!

  Int_t ind = getDitSetHe(run);
  TString filename;
  if (ind>=0 && ind<=1) {
    filename = Form("/w/work1704/parity/kent/pan/slopes/avgSlopes_helium_dither_Superslug%d.res",ind);
  }else{
    filename = "";
  }
  return filename;
}

