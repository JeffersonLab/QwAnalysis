#ifndef PAN_TaPanamDevice
#define PAN_TaPanamDevice 
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaPanamDevice.hh  (header)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamDevice.hh,v 1.3 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//     Class for Panam monitor device. Handle easy display 
//     of Pan devices. 
//
//
////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TObject.h"
#include "TPaveText.h"
#include "TH1.h"
#include <string>
#include <iostream>
#include "PanTypes.hh"
#include "TaStripChart.hh"
#include "TaRun.hh"
#include "TPad.h"

using namespace std;

class TaPanamDevice : public TObject {
  
public:
  
  TaPanamDevice();
  TaPanamDevice(char* name, Int_t namekey, 
                Int_t SNumChan, Int_t SNumEvperChan,
                Int_t histobin, 
                Axis_t xsmin, Axis_t xsmax, 
                Axis_t xhmin, Axis_t xhmax, 
                Int_t color); 
  TaPanamDevice(const TaPanamDevice& copy);
  TaPanamDevice& operator=( const TaPanamDevice& assign);  
  virtual ~TaPanamDevice();
    
  TaStripChart* GetSData() const {return fSData;};
  TaStripChart* GetSDataRMS() const {return fSDataRMS;};
  char* GetName() const {return fName;};
  Double_t  GetDataVal() const {return fDataVal;};
  TH1D*   GetHData() const { return fHData;};
  virtual void InitSCPad(UInt_t plotidx);
  virtual void DisplaySC(UInt_t plotidx);  
  virtual void DrawHPad(UInt_t optionfit);  
  virtual void InitStat(Double_t  x1, Double_t y1, Double_t x2, Double_t y2,Int_t textfont, Double_t textsize);
  virtual void UpdateStat();
  virtual void SetTiltle();
  virtual void FillFromEvent(TaRun& run);
  TH1D*   GetPlot(char* const plotname, Int_t const plottype) const;

protected:

  void Init();
   //data members 
  vector<TaStripChart*> fSCArray;
  TaStripChart*  fSData;
  TaStripChart*  fSDataRMS;
  TH1D*          fHData;
  TPaveText*    fTitle;
  TPaveText*    fStat;  
  char*         fName;
  char*         fNeventp,*fMeanp,*fRMSp,*fSigAvep;
  Double_t      fDataVal;
  Double_t      fSigmaAverage;          
  Int_t         fDevicekey;
  Int_t         fSNumOfChan;
  Int_t         fSNumOfEvPerChan;
  Int_t         fHbins;
  Int_t         fColor;
  Float_t       fXSmin;
  Float_t       fXSmax;  
  Float_t       fXHmin;
  Float_t       fXHmax;  
#ifdef LEAKING
  void Leaking();
  static UInt_t fLeaKNewHisto;    // count of histo allocations
  static UInt_t fLeakDelHisto;    // count of histo deallocations
  static UInt_t fLeakNewSC;   // count of SC allocations
  static UInt_t fLeakDelSC;   // count of SC deallocations
  static UInt_t fLeakNewSCRMS;   // count of SC allocations
  static UInt_t fLeakDelSCRMS;   // count of SC deallocations
#endif
    
  ClassDef( TaPanamDevice, 0 )  // Base class 
};

#endif
