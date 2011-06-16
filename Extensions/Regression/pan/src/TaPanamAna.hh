//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPanamAna.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamAna.hh,v 1.3 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Online analysis.  This class derives from VaAnalysis.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaPanamAna
#define PAN_TaPanamAna

#include <TObject.h>
#include <TTree.h>
#include <THStack.h>
#include <TGraph.h>
#include <TH2.h>
#include "TaString.hh"
#include "VaAnalysis.hh"
#include "TaPanamDevice.hh"
#include "TaPanamADevice.hh"
#include "TaPanamMultiDevice.hh"



using std::vector;

class TaRun;

class TaPanamAna: public VaAnalysis {
  
public:

  enum NumOfChan { striptime = 180, numlumi = 4, numdalton = 4};
  
  // Constructors/destructors/operators
  TaPanamAna();
  virtual ~TaPanamAna();
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  
  // Major functions
  vector<string> GetHistoForListBox() const; 
  void InitDevicesList(vector<string> arrayofname);
  void InitMonitorDevices();
  //  void InitMonitorMultiDevices();
  void InitDevicePad(Int_t devidx,UInt_t plotidx, Bool_t choice,UInt_t optionfit);
  void DisplayDevice(Int_t devidx,UInt_t plotidx, Bool_t choice,UInt_t optionfit);
  void InitADevicePad(Int_t devidx,UInt_t plotidx, Bool_t choice,UInt_t optionfit);
  void DisplayADevice(Int_t devidx,UInt_t plotidx, Bool_t choice,UInt_t optionfit);
  //  void DefineADCStacks(Bool_t opt);
  void DrawADCStack( Int_t adccrate, UInt_t adcidx, Int_t thestack, Int_t optstat);
  void DrawMDStack(UInt_t stacknum, Int_t thestack, Int_t optstat);
  TaPanamDevice* GetPanamDevice(Char_t* const devname) const;
  TaPanamADevice* GetPanamADevice(Char_t* const adevname) const;
  //  TaPanamDevice* GetPanamMultiDevice(Char_t* const devname) const;
  // Data access functions
  vector<string> GetDeviceUsedlist() const {return fArrayOfDataName;}; 
private:
  
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  TaPanamAna(const TaPanamAna& copy);
  TaPanamAna& operator=( const TaPanamAna& assign);
  
  // Member functions
  void EventAnalysis ();
  void PairAnalysis ();
  void InitChanLists ();
  string itos(Int_t i);
  void FillEventPlots();
  void FillPairPlots();
  
  // Data members
  vector<string>                fArrayOfDataName;
  vector<TaPanamDevice*>        fMonDev;
  vector<TaPanamADevice*>       fMonADev;
  vector<TaPanamADevice*>       fADC;
  vector<TaPanamMultiDevice*>   fMDIADC; 
  vector<TaPanamMultiDevice*>   fMDPADC; 
  vector<TaPanamMultiDevice*>   fMDSADC; 
  TaPanamMultiDevice*           fMADC; 
  TaPanamMultiDevice*           fMBatt; 
  TaPanamMultiDevice*           fMBCM; 
  TaPanamMultiDevice*           fMBCM2; 
  TaPanamMultiDevice*           fMBCMcav; 
  TaPanamMultiDevice*           fMBPMINx; 
  TaPanamMultiDevice*           fMBPMINy; 
  TaPanamMultiDevice*           fMBPMParx; 
  TaPanamMultiDevice*           fMBPMPary; 
  TaPanamMultiDevice*           fMBPMABx; 
  TaPanamMultiDevice*           fMBPMABy; 
  TaPanamMultiDevice*           fMBPMcavx; 
  TaPanamMultiDevice*           fMBPMcavy; 
  TaPanamMultiDevice*           fMLumiBack; 
  TaPanamMultiDevice*           fMLumiFront; 
  TaPanamMultiDevice*           fMDalton; 

  Int_t fTime;
  Int_t fStrip_timeslot;                         // 
  Int_t fADC_count;
  Int_t fBCM_count;
  Int_t fBPM_count;
  char* dlabel;
  Char_t* fLastADCName;

  //#ifdef DICT
  ClassDef(TaPanamAna, 0)  // Monitoring analysis
    //#endif
};

#endif
