#ifndef PAN_TaStripChart
#define PAN_TaStripChart

//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan (Online Monitor version)           
//
//        TaStripChart.hh   (interface file)
//        ^^^^^^^^^^^^^^^
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaStripChart.hh,v 1.2 2003/07/31 16:12:00 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//     Stripchart class for online monitoring.
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "PanTypes.hh"
#include <string>
#include <iostream>
#include "TObject.h"
#include "TH1.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TArray.h"

class TaStripChart: public TObject {

public:

  TaStripChart();
  TaStripChart(char* label, char* dataname, Int_t NumbofChan, Int_t NumofEvperChan, Axis_t xmin, Axis_t xmax);
  virtual ~TaStripChart();
  TaStripChart(const TaStripChart& sc);
  TaStripChart& operator=(const TaStripChart &sc);
  
  // void  Streamer(TBuffer &R__b);  // define own streamer
  Int_t  DataSum(Double_t value);
  Int_t  AddBinVal(Double_t value);
  Int_t  Reset();
  Int_t  Fill();
  void   SCDraw();
  void   SCDrawInit();
  void   SetName(char* name){ fName=name;};
  char*  GetName() const { return fName;};
  void   SetDataName(char* dataname){ fDataName=dataname;};
  char*  GetDataName() const { return fDataName;};
  TArrayD* GetArray() const {return fSC_array;};
  Double_t GetSum() const {return fSC_sum;};  
  Double_t GetSumNorm() const {return (fSC_sum/fMaxSum);};  
  TH1D*  GetSCHist() const {return fSC_hist;};

 private:

  char*      fName;            // 
  char*      fDataName;        // 
  TH1D*      fSC_hist;         // 
  TArrayD*   fSC_array;        //
  Double_t   fSC_sum;        //
  Int_t      fMaxSum;        // 
  Int_t      fSC_current_bin;  //
  Int_t      fSC_num_of_chan;  //
  Int_t      fSC_sum_count;  //
  Axis_t     fAbsMin,fAbsMax; //



ClassDef (TaStripChart, 0)   // Collection of StripChart 

};
#endif

