/**********************************************************\
* File: QwDriftChamberHDC.h                                *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __QWDRIFTCHAMBERHDC__
#define __QWDRIFTCHAMBERHDC__

#include "QwDriftChamber.h"
#include "QwOptions.h"
///
/// \ingroup QwTrackingg
class QwDriftChamberHDC: public QwDriftChamber, public MQwSubsystemCloneable<QwDriftChamberHDC> {
  /******************************************************************
   *  Class: QwDriftChamberHDC
   *
   *
   ******************************************************************/
 public:
  QwDriftChamberHDC(TString region_tmp);
  virtual ~QwDriftChamberHDC() { };

  /// Copying is not supported for tracking subsystems
  void Copy(const VQwSubsystem *source) {
    QwWarning << "Copy() is not supported for tracking subsystems." << QwLog::endl;
  }

  /* Unique virtual member functions from QwDrifChamber base class */


  //  void  ReportConfiguration();
  void  SubtractReferenceTimes();
  void  ProcessEvent();
  Int_t LoadGeometryDefinition(TString mapfile );
  //  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  //  void  PrintConfigrationBuffer(UInt_t *buffer, UInt_t num_words);
  Int_t LoadChannelMap ( TString mapfile ) ;
  void  ClearEventData();
  static void DefineOptions(QwOptions& options);
  void ProcessOptions(QwOptions& options);

 protected:
  // VDC and HDC
  void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  Int_t AddChannelDefinition();
  Int_t BuildWireDataStructure(const UInt_t chan, const EQwDetectorPackage package, const Int_t plane, const Int_t wire);
  Double_t  CalculateDriftDistance(Double_t drifttime, QwDetectorID detector);

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix) ;
  void  FillHistograms();

  Int_t LoadTimeWireOffset(TString t0_map);
  void LoadTtoDParameters(TString ttod_map);
  void SubtractWireTimeOffset();
  void ApplyTimeCalibration();

  // HDC
  Double_t trig_h1;//this will keep the first hit time of trig_h1 (plane 7)
  std::vector< std::vector< std::vector<Double_t> > > fTimeWireOffsets;
  std::vector< Double_t> fTtoDNumbers;
  //  ClassDef(QwDriftChamber,2);

  Int_t fR2Octant;


};

#endif
