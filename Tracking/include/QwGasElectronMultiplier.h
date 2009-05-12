/**********************************************************\
* File: QwGasElectronMultiplier.h                          *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __QWGASELECTRONMULTIPLIER__
#define __QWGASELECTRONMULTIPLIER__

#include "QwParameterFile.h"

#include "QwHit.h"

#include "QwTypes.h"

#include <exception>
#include<iostream>
#include<fstream>
#include<string>

#include "VQwSubsystemTracking.h"

class QwHitContainer;

///
/// \ingroup QwTrackingAnl
class QwGasElectronMultiplier: public VQwSubsystemTracking{
  /******************************************************************
   *  Class: QwGasElectronMultiplier
   *
   *
   ******************************************************************/
 public:  
  QwGasElectronMultiplier(TString region_tmp):VQwSubsystemTracking(region_tmp){};

  ~QwGasElectronMultiplier()
    {
      DeleteHistograms();
    }

  /*  Member functions derived from VQwSubsystemTracking. */
  Int_t LoadChannelMap(TString mapfile ){};
  Int_t LoadInputParameters(TString mapfile){};
  Int_t LoadQweakGeometry(TString mapfile){};
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info){};
  void  ClearEventData(){};

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return 0;};

  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return 0;};

  void  ProcessEvent(){
    if (! HasDataLoaded()) return;
  };

  void  FillListOfHits(QwHitContainer& hitlist){};
  
  void  ConstructHistograms(TDirectory *folder, TString &prefix){};
  void  FillHistograms(){};
  void  DeleteHistograms(){};
 
  void GetHitList(QwHitContainer & grandHitContainer){
    grandHitContainer.Append(fHits);
  };


 protected:
  Bool_t fDEBUG;
  TString fRegion;  ///  Name of this subsystem (the region).


 protected:
  size_t fCurrentBankIndex;
  Int_t  fCurrentSlot;

 protected:
  std::vector< QwHit > fHits;

 /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms() 
   */
  
  
  
};

#endif
