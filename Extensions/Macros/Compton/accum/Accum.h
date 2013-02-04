#ifndef ACUUM_H
#define ACUUM_H

class TH2F;
#include "VComptonMacro.h"
/*
 *
 */
class Accum : public VComptonMacro {
public:
  void init( ComptonSession* );
  void run();

  // Some useful styles/settings
  static Style_t fStyles[3];
  static Color_t fColors[3];

protected:
  void GetH1ValErr(const char* hist, Int_t *ents, Double_t *val, Double_t *err);
  void H2SetStyle(TH2F* hist, Int_t color);

private:
  TCanvas *fCanvasTemp;
  TCanvas *fCanvasBCM;
  TCanvas *fCanvasBPM[2];

  void ProcessBCMs();
  void ProcessBPMs();
  const char* scaling(const char* var, Double_t lowTo, Double_t highTo,
      Double_t lowFrom, Double_t highFrom) {
    return Form("(%f)+((%s)-(%f))*(((%f)-(%f))/((%f)-(%f)))",lowTo,var,lowFrom,
        highTo,lowTo,highFrom,lowFrom);
  }
};

#endif
