/**********************************************************\
* File: QwDriftChamberHDC.h                                *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __QWDRIFTCHAMBERHDC__
#define __QWDRIFTCHAMBERHDC__

#include "QwDriftChamber.h"
///
/// \ingroup QwTracking
class QwDriftChamberHDC: public QwDriftChamber {
  /******************************************************************
   *  Class: QwDriftChamberHDC
   *
   *
   ******************************************************************/
 public:
  QwDriftChamberHDC(TString region_tmp);
  ~QwDriftChamberHDC()
    {
      DeleteHistograms();
    };
  
  /* Unique virtual member functions from QwDrifChamber base class */


  // VDC and HDC
  void  ReportConfiguration();
  void  SubtractReferenceTimes();
  void  ProcessEvent();
  Int_t LoadGeometryDefinition(TString mapfile );
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintConfigrationBuffer(UInt_t *buffer, UInt_t num_words);
  Int_t LoadChannelMap ( TString mapfile ) ;


  

 protected:
  // VDC and HDC
  void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  Int_t AddChannelDefinition();
  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
  Double_t  CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);
  

  // HDC
  Double_t trig_h1;//this will keep the first hit time of trig_h1 (plane 7)
  
  //  ClassDef(QwDriftChamber,2);


};

#endif
