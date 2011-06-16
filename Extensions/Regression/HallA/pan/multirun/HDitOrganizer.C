
#include <multirun/HDitOrganizer.h>


TCut getDitCutH_byIndex(Int_t ind) {
  // you tell me what "superslug" you are in (with regards
  // to dithering) and I'll give you the cut you need to 
  // clean up your averaging.  Mainly, this should be used
  // by the AvgSlope routine.
  

  TCut superslugselect[8];
  superslugselect[0] = TCut("run<=5773");
  superslugselect[1] = TCut("run>5773&&run<=5803");
  superslugselect[2] = TCut("run>5803&&run<=5860");
  superslugselect[3] = TCut("run>5860&&run<=5890");
  superslugselect[4] = TCut("run>5890&&run<=5905");
  superslugselect[5] = TCut("(run>5930&&run<=5982)||(run>5994&&run<=6000)");
  superslugselect[6] = TCut("run>5905&&run<=6077&&!((run>5930&&run<=5982)||(run>5994&&run<=6000))");
  superslugselect[7] = TCut("run>6077");

  TCut dcut;
  if (ind>=0 && ind<8) {
    dcut = superslugselect[ind];
    dcut += "lostCount<20";
  } else {
    dcut = "0";
  }
  return dcut;
}


Int_t getDitSetH(Int_t run) {
  // give me a run number, and I'll give you the index
  // of the run average file that you want to use. Easy!
  Int_t ind;
  if (run<=5773) {
    ind=0;
  } else if (run>5773&&run<=5803) {
    ind=1;
  } else if (run>5803&&run<=5860) {
    ind=2;
  } else if (run>5860&&run<=5890) {
    ind=3;
  } else if (run>5890&&run<=5905) {
    ind=4;
  } else if ((run>5930&&run<=5982)||(run>5994&&run<=6000)) {
    ind=5;
  } else if (run>5905&&run<=6077) {
    // order in the list separates this one from section in previous
    ind=6;
  } else if (run>6077) {
    // order in the list separates this one from section in previous
    ind=7;
  } else {
    // default case... nothing works!
    ind=-1;
  }
  return ind;
}

TCut getDitCutH_byRun(Int_t run) {
  // give me a run number, and I'll give you a cut which should
  // be applied to the dither slope summary file.  Mainly, this
  // should be used by the AvgSlope routine

  Int_t ind = getDitSetH(run);
  TCut dcut = getDitCutH_byIndex(ind);
  return dcut;
}


TString getDitSetFilenameH(Int_t run) {
  // give me a run number, and I'll give you the NAME
  // of the run average file that you want to use.  Even easier!!!

  Int_t ind = getDitSetH(run);
  TString filename;
  if (ind>=0 && ind<8) {
    filename = Form("/w/work1704/parity/kent/pan/slopes/avgSlopes_hydrogen_dither_Superslug%d.res",ind);
  }else{
    filename = "";
  }
  return filename;
}

