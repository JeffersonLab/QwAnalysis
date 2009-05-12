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
/// \ingroup QwTrackingAnl
class QwDriftChamberHDC: public QwDriftChamber {
  /******************************************************************
   *  Class: QwDriftChamberHDC
   *
   *
   ******************************************************************/
 public:
  QwDriftChamberHDC(TString region_tmp);

  

  

  

  /* Unique virtual member functions from QwDrifChamber base class */

 public:

 void  ReportConfiguration();

 void  SubtractReferenceTimes();

 Double_t  CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);

 Int_t LoadQweakGeometry(TString mapfile );
 void  ProcessEvent();

 protected:
 void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);




 protected:

  Int_t LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire);
  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
  Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire);
 



 
 



  //  ClassDef(QwDriftChamber,2);


};

#endif
