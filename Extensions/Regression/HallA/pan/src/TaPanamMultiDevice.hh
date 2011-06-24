#ifndef PAN_TaPanamMultiDevice
#define PAN_TaPanamMultiDevice 
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaPanamMultiDevice.hh  (header)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamMultiDevice.hh,v 1.2 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//     Class for Panam monitor device. Handle easy display 
//     of Pan devices. 
//
//
////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include <TObject.h>
#include <THStack.h>
#include "TaPanamDevice.hh"
#include "TaPanamADevice.hh"

class TaPanamMultiDevice : public TObject {
  
public:
  
  TaPanamMultiDevice();
  TaPanamMultiDevice(Char_t* name);
  TaPanamMultiDevice(const TaPanamMultiDevice& copy);
  TaPanamMultiDevice& operator=( const TaPanamMultiDevice& assign);  
  virtual ~TaPanamMultiDevice();
  Char_t*  GetName() const { return fName;};
  THStack* GetDStripStack() const { return fStkStrip;};
  THStack* GetDStripRMSStack() const { return fStkStripRMS;};
  THStack* GetDHistoStack() const { return fStkHisto;};
  THStack* GetAStripStack() const { return fStkAStrip;};
  THStack* GetAStripRMSStack() const { return fStkAStripRMS;};
  THStack* GetAHistoStack() const { return fStkAHisto;};
  void    InsertDevice(TaPanamDevice* thedev);  
  void    InsertADevice(TaPanamADevice* theAdev);
  void    CopyDev(vector<TaPanamDevice*> vdev);
  void    CopyADev(vector<TaPanamADevice*> vadev);
  Int_t  TypeOfDev();
  void    DrawStack(Int_t whichstack,Int_t optstat);
  void    InitColors(Int_t devsize);  
  Int_t   InitStacks();
  void    InitStats();  
  void    UpdateStats(UInt_t datatype);
  TaPanamDevice* GetDev(Int_t idx) const {return  fMyPD[idx];};  
  TaPanamADevice* GetADev(Int_t idx) const {return  fMyPAD[idx];};    
  void    DumpInfo();
  

private:

  Char_t*                 fName;
  vector<Int_t>           fColor;  
  vector<Double_t>        fPD;
  vector<Double_t>        fPAD;
  vector<TaPanamDevice*>  fMyPD;
  vector<TaPanamADevice*> fMyPAD;
  vector<THStack*>        fS;
  THStack                 *fStkStrip;   
  THStack                 *fStkStripRMS;   
  THStack                 *fStkHisto;   
  THStack                 *fStkAStrip;   
  THStack                 *fStkAStripRMS;   
  THStack                 *fStkAHisto;
  TaPanamDevice           *fPDev;
  TaPanamADevice          *fPADev;

  ClassDef( TaPanamMultiDevice, 0 ) // 
};

#endif
