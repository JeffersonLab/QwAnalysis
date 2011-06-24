//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaADCCalib.hh   (interface)
//
// Author:  K. Paschke <http://www.jlab.org/~paschke/>
// @(#)pan/src:$Name:  $:$Id: TaADCCalib.hh,v 1.13 2008/01/07 21:07:10 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    ADC calibration analysis.  Depending on constructor argument,
//    this class handles analysis of ADC pedestals or DAC noise 
//    pedestals and slopes.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaADCCalib
#define PAN_TaADCCalib

#include <TObject.h>
#include <TTree.h> 
#include <vector>
#include "VaAnalysis.hh"
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TGraphErrors.h"

using namespace std;

#define ADC_MaxSlot 30
#define ADC_MaxChan 4
#define ADC_MinDAC 3000

//class TaStatistics;
class TaRun;

class TaADCCalib: public VaAnalysis {
  
public:
  
  // Constructors/destructors/operators
  TaADCCalib();
  TaADCCalib(const string& anName);
  ~TaADCCalib();
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  
  // Major functions
  
  // Data access functions
  
private:
  
  //
  // Data members
  //

  Int_t typeFlag;
  //    array will be set true for each slot, channel  with an existing key
  Bool_t chanExists[ADC_MaxSlot][ADC_MaxChan]; 

  TFile *hfile;    // root histo file


  // data members for pedestal analysis
  TH1F **phist;     // Array of histograms
  vector<Double_t>   fSumX;        // sum of X
  vector<Double_t>   fSumX2;       // sum of X^2
  vector<Int_t >     nEntries;     // nEntries for this channel

  // data members  for DAC analysis
  vector<Double_t>   fSumY;        // sum of Y needed for linear fit
  vector<Double_t>   fSumXY;       // sum of XY needed for linear fit
  //
  //   additional stuff needed for graph of residuals
  //
  static const Int_t MaxNoiseDACBin = 65536;
  TGraphErrors **dgraphs;   // array of graphs
  TGraphErrors **rgraphs;   // array of graphs
  vector< vector<Int_t> >  dEntries;   // number of entries in each chan#/DAC bin
  vector< vector<Double_t> >  dADCsum;  // sum of ADC value for each chan#/DAC bin
  vector< vector<Double_t> >  dADCsum2; // sum of ADC value-squared

  Double_t dDACval[MaxNoiseDACBin];
  Double_t dEDACval[MaxNoiseDACBin];
  Double_t dADCavg[MaxNoiseDACBin];
  Double_t dADCvar[MaxNoiseDACBin];
  Double_t dRes[MaxNoiseDACBin];

  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  TaADCCalib(const TaADCCalib& copy);
  TaADCCalib& operator=( const TaADCCalib& assign);
  
  //
  // Member functions
  //
  virtual void Init(const Bool_t&);
  void InitPed();
  void InitDAC();
  virtual ErrCode_t ProcessRun();
  virtual void Finish();
  void FinishPed();
  void FinishDAC();
  void EventAnalysis ();
  void PairAnalysis ();
  void InitChanLists ();
  

#ifndef NODICT
  ClassDef(TaADCCalib, 0)  // ADC calibration analysis
#endif
};

#endif
