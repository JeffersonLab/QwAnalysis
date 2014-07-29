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

#include <cstdlib>
#include <cstdio>
#include <utility>

///
/// \ingroup QwTracking
class QwDriftChamberVDC: public QwDriftChamber, public MQwSubsystemCloneable<QwDriftChamberVDC> {
  /******************************************************************
   *  Class: QwDriftChamberVDC
   *
   *
   ******************************************************************/
 public:
  QwDriftChamberVDC(TString name);
  virtual ~QwDriftChamberVDC() { };

  /* Unique virtual member functions from QwDrifChamber base class */

  
  //  void  ReportConfiguration();
  void  SubtractReferenceTimes();
  void  ProcessEvent();
  //  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  //  void  PrintConfigrationBuffer(UInt_t *buffer, UInt_t num_words);
  Int_t LoadChannelMap(TString mapfile);


  // VDC
  //  using QwDriftChamber::CalculateDriftDistance;
  static void DefineOptions(QwOptions& options);
  void ProcessOptions(QwOptions& options);
 
  //Int_t LoadMap ( TString& );        //read the TDC convert QwDelayLine map
  void  ReadEvent ( TString& );     //read the events file

  void ClearEventData();
  
  

 
  
 protected:

  // VDC and HDC
  void  FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  Int_t AddChannelDefinition();
  Int_t BuildWireDataStructure(const UInt_t chan, const EQwDetectorPackage package, const Int_t octant, const Int_t plane, const Int_t wire);
  Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  Int_t LoadTimeWireOffset(TString t0_map); 
  void LoadTtoDParameters(TString ttod_map); 
  void SubtractWireTimeOffset();
  void UpdateHits();
  //  void ApplyTimeCalibration();
  
  // VDC
  void GetHitList(QwHitContainer & grandHitContainer)
  {
    if(fUseTDCHits) grandHitContainer.Append(fTDCHits);
    else            grandHitContainer.Append(fWireHits);
  };

 
  
  Bool_t fUseTDCHits;
  Bool_t fDisableWireTimeOffset;
  Int_t fR3Octant;
  static const UInt_t kBackPlaneNum;
  static const UInt_t kLineNum;
  std::vector< std::vector<QwDelayLine> > fDelayLineArray;   //indexed by backplane and line number
  std::vector< std::vector<QwDelayLineID> > fDelayLinePtrs;  //indexed by slot and channel number
  std::vector< QwHit > fWireHitsVDC;
  std::vector< std::vector< std::vector<Double_t> > > fTimeWireOffsets;
  std::vector< Double_t> fTtoDNumbers;
  
};

#endif
