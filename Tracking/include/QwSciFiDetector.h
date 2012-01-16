/**********************************************************\
* File: QwSciFiDetector.h                          *
* Author: Jeong Han Lee
* Time-stamp: Sunday, January 15 17:26:13 EST 2012
\**********************************************************/

#ifndef __QWSCIFIDETECTOR__
#define __QWSCIFIDETECTOR__

#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "QwHit.h"
#include "QwHitContainer.h"

#include "QwTypes.h"
#include "QwDetectorInfo.h"

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "VQwSubsystemTracking.h"
#include "QwF1TDContainer.h"

class QwHitContainer;

///
/// \ingroup QwTracking
class QwSciFiDetector: public VQwSubsystemTracking, public MQwSubsystemCloneable<QwSciFiDetector> {

 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwSciFiDetector();

 public:
  QwSciFiDetector(const TString& name);
  virtual ~QwSciFiDetector();
  
  /// Copying is not supported for tracking subsystems
  void Copy(const VQwSubsystem *source) {
    QwWarning << "Copy() is not supported for tracking subsystems." << QwLog::endl;
  }

  /*  Member functions derived from VQwSubsystemTracking. */
  Int_t LoadChannelMap(TString mapfile );
  Int_t LoadInputParameters(TString mapfile);
  Int_t LoadGeometryDefinition(TString mapfile);

  void  ClearEventData();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();

  void  FillListOfHits(QwHitContainer& hitlist);

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void GetHitList(QwHitContainer & grandHitContainer){
    //    grandHitContainer.Append(fGEMHits);
  };


 protected:

  std::vector< QwHit > fHits;

};

#endif
