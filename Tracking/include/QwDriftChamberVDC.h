/**********************************************************\
* File: QwDriftChamberVDC.h                                *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/


#ifndef __QWDRIFTCHAMBERVDC__
#define __QWDRIFTCHAMBERVDC__






#include "QwDriftChamber.h"


#include "QwDelayLine.h"

#include <stdlib.h>
#include <stdio.h>
#include <utility>





///
/// \ingroup QwTracking
class QwDriftChamberVDC: public QwDriftChamber {
  /******************************************************************
   *  Class: QwDriftChamberVDC
   *
   *
   ******************************************************************/
 public:
  QwDriftChamberVDC(TString region_tmp);
  ~QwDriftChamberVDC()
    {
      DeleteHistograms();
    };
  /* Unique virtual member functions from QwDrifChamber base class */

 public:

 void  ReportConfiguration();

 void  SubtractReferenceTimes();

 //Double_t  CalculateDriftDistance(Double_t drifttime, QwDetectorID detector, Double_t angle);
 // Double_t  CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);

 Int_t LoadChannelMap(TString mapfile);
 //Int_t LoadMap ( TString& );        //read the TDC convert QwDelayLine map
 void  ReadEvent ( TString& );     //read the events file


 void  ProcessEvent();
 Int_t ProcessConfigurationBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
 void  PrintConfigrationBuffer(UInt_t *buffer, UInt_t num_words);
 
 void ClearEventData();
   
 //void GetHitList(QwHitContainer & grandHitContainer){
 //  grandHitContainer.Append(fWireHits);
 //};

  Int_t LoadQweakGeometry(TString mapfile );

  Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector){
    double angle=60,d=0;
    d=CalculateDriftDistance(drifttime,detector,angle);
    return d;
 }


 protected:

   Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector, Double_t angle);

   void  FillHistograms();


 protected:
 void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);




 protected:

  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
  Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire);





  static const UInt_t kBackPlaneNum;
  static const UInt_t kLineNum;
  std::vector<std::vector<QwDelayLine> > fDelayLineArray;      //indexed by backplane and line number
  std::vector<std::vector<QwDelayLineID> > fDelayLinePtrs;  //indexed by slot and channel number
  std::vector< QwHit > fWireHitsVDC;


};

#endif
