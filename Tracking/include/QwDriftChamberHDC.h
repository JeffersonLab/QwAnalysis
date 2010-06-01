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

 public:

 void  ReportConfiguration();

 void  SubtractReferenceTimes();

 

 Int_t LoadGeometryDefinition(TString mapfile );
 void  ProcessEvent();



 Double_t  CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);

 protected:
 void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);




 protected:

  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
  Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire);

  Double_t trig_h1;//this will keep the first hit time of trig_h1 (plane 7)
 



 
 



  //  ClassDef(QwDriftChamber,2);


};

#endif
