#ifndef PAN_TaPanamADevice
#define PAN_TaPanamADevice 
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaPanamADevice.hh  (header)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamADevice.hh,v 1.3 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//     Class for Panam monitor device. Handle easy display 
//     of Pan devices which requires asymmetry plots. 
//
//
////////////////////////////////////////////////////////////////////////

#include "TPaveText.h"
#include "TaStripChart.hh"
#include "TaPanamDevice.hh"
#include "VaPair.hh"

class TaPanamADevice : public TaPanamDevice {
  
public:
  
  TaPanamADevice();
  TaPanamADevice(char* name, Int_t namekey, 
                 Int_t SNumChan, Int_t SNumEvperChan,
                 Int_t SANumChan, Int_t SANumEvperChan,
                 Int_t histobin,
                 Axis_t xsmin, Axis_t xsmax,
                 Axis_t xhmin, Axis_t xhmax, 
                 Int_t histoabin,  
                 Axis_t xsamin, Axis_t xsamax,
                 Axis_t xhamin, Axis_t xhamax, 
                 Int_t color, Int_t acolor, Bool_t datatype); 
  TaPanamADevice(const TaPanamADevice& copy);
  TaPanamADevice& operator=( const TaPanamADevice& assign);  
  virtual ~TaPanamADevice();
    
  Double_t  GetADataVal() const {return fADataVal;};
  TaStripChart* GetSAData() const {return fSAData;};
  TaStripChart* GetSADataN() const {return fSADataN;};
  TaStripChart* GetSADataRMS() const {return fSADataRMS;};
  TH1D*   GetHAData() const { return fHAData;};
  TH1D*   GetHADataN() const { return fHADataN;};
  void FillFromPair(VaPair& pair);
  void FillFromPair(VaPair& pair,Int_t normdev);
  void DrawHPad(UInt_t plotidx , UInt_t optionfit);
  virtual void InitAStat(Double_t  x1, Double_t y1, Double_t x2, Double_t y2,Int_t textfont, Double_t textsize);
  virtual void UpdateAStat();
  virtual void UpdateANStat();
  //  TH1D*   GetPlot(char* const plotname, Int_t const plottype) const;

protected:

  void Init();
   //data members 
  vector<TH1D*>  fHArray;
  TaStripChart*  fSAData;      
  TaStripChart*  fSADataRMS;
  TaStripChart*  fSADataN;
  TH1D*          fHAData;
  TH1D*          fHADataN;
  Double_t       fADataVal;          
  Double_t       fADataNVal;          
  TPaveText*     fATitle;
  TPaveText*     fAStat;  
  TPaveText*     fANStat;  
  Char_t         fANeventp[50],fAMeanp[100],fARMSp[50],fASigAvep[50];
  Double_t       fASigmaAverage;          
  Int_t          fSANumOfChan;
  Int_t          fSANumOfEvPerChan;
  Int_t          fHAbins;
  Int_t          fAColor;
  Float_t        fXSAmin;
  Float_t        fXSAmax;  
  Float_t        fXHAmin;
  Float_t        fXHAmax;  
  Bool_t         fWhichData;
#ifdef LEAKING
  void Leaking();
  static UInt_t fLeaKNewAHisto;    // count of histo allocations
  static UInt_t fLeakDelAHisto;    // count of histo deallocations
  static UInt_t fLeakNewASC;   // count of SC allocations
  static UInt_t fLeakDelASC;   // count of SC deallocations
  static UInt_t fLeaKNewAHistoN;    // count of histo allocations
  static UInt_t fLeakDelAHistoN;    // count of histo deallocations
  static UInt_t fLeakNewASCN;   // count of SC allocations
  static UInt_t fLeakDelASCN;   // count of SC deallocations
  static UInt_t fLeakNewASCRMS;   // count of SC allocations
  static UInt_t fLeakDelASCRMS;   // count of SC deallocations
#endif
     
  ClassDef( TaPanamADevice, 0 )  // Base class 
};

#endif
