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
#include "QwOptions.h"

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

  using QwDriftChamber::CalculateDriftDistance;

  void  ReportConfiguration();
  void  SubtractReferenceTimes();
  Int_t LoadChannelMap(TString mapfile);
  //Int_t LoadMap ( TString& );        //read the TDC convert QwDelayLine map
  void  ReadEvent ( TString& );     //read the events file


  void  ProcessEvent();
	
  static void DefineOptions(QwOptions& options);

  void ProcessOptions(QwOptions& options);


  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintConfigrationBuffer(UInt_t *buffer, UInt_t num_words);
 
  void ClearEventData();
   
  Int_t LoadGeometryDefinition(TString mapfile );

   Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector){
    Double_t angle_degree = 45.0; 
    return CalculateDriftDistance(drifttime,detector,angle_degree);
  }

 protected:
  
  Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector, Double_t angle);


  void GetHitList(QwHitContainer & grandHitContainer)
  {
    if(fUseTDCHits==true) grandHitContainer.Append(fTDCHits);
      else grandHitContainer.Append(fWireHits);
  };

  void  FillHistograms();
  void  FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire);
/*   Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire) {return 0;}; */
  Int_t AddChannelDefinition() {return 0;};

  Bool_t fUseTDCHits;
  static const UInt_t kBackPlaneNum;
  static const UInt_t kLineNum;
  std::vector<std::vector<QwDelayLine> > fDelayLineArray;      //indexed by backplane and line number
  std::vector<std::vector<QwDelayLineID> > fDelayLinePtrs;  //indexed by slot and channel number
  std::vector< QwHit > fWireHitsVDC;


};

#endif
