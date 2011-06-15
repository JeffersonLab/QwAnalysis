/**********************************************************\
* File: QwGasElectronMultiplier.h                          *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/

#ifndef __QWGASELECTRONMULTIPLIER__
#define __QWGASELECTRONMULTIPLIER__

#include "TH1D.h"
#include "TH2D.h"

#include "QwHit.h"
#include "QwHitContainer.h"

#include "QwTypes.h"

#include <exception>
#include <iostream>
#include <fstream>
#include <string>

#include "VQwSubsystemTracking.h"


class QwHitContainer;

///
/// \ingroup QwTracking
class QwGasElectronMultiplier: public VQwSubsystemTracking, public MQwCloneable<QwGasElectronMultiplier> {
  /******************************************************************
   *  Class: QwGasElectronMultiplier
   *
   *
   ******************************************************************/
 public:

  QwGasElectronMultiplier(TString region_tmp);
  ~QwGasElectronMultiplier();

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
    grandHitContainer.Append(fGEMHits);
  };


  void FillVFATWord(Int_t VFAT_index);//Fill the current buffer into the fBuffer_VFAT array. '1' is data and '0' is no data for given channel.
  void GetBC(Int_t [], Int_t);//return the VFAT Bean Count
  void GetEC(Int_t [], Int_t);//return the VFAT Event Count
  void GetFlags(Int_t [], Int_t);//return the VFAT Flags
  void GetChipId(Int_t [], Int_t);//return the VFAT ID
  void GetChannelData(Int_t [], Int_t);//return the VFAT Channel Data

  void AddHit(Int_t,Int_t);

 protected:
  // Bool_t fDEBUG;
  TString fRegion;  ///  Name of this subsystem (the region).
  static const Int_t N_GEM = 2;
  static const Int_t GEM_RADIALSTRIPS=512;//No. of radial locations
  static const Int_t GEM_TRANSVERSESTRIPS=256;//No. of transverse locations
  static const Int_t N_VFAT = 12;
  static const Int_t VFAT_DATA_SIZE = 192;
  static const Int_t VFAT_CHANNEL_DATA_SIZE = 128;
  static const Int_t VFAT_EC_SIZE=8;
  static const Int_t VFAT_BC_SIZE=12;
  static const Int_t VFAT_FLG_SIZE=4;
  static const Int_t VFAT_CID_SIZE=12;
  static const Bool_t bDEBUG=kFALSE;//kTRUE;
  static const Bool_t bDEBUG_Hitlist=kFALSE;//kTRUE;//print the hit list


  size_t fCurrentBankIndex;
  Int_t  fCurrentSlot;

  std::vector< QwHit > fHits;

  Int_t fVFATChannel[N_VFAT][12];//stores VFAT data words during an event
  Int_t fBuffer_VFAT[N_VFAT][192];//Set VFAT channels to 1 or 0 based on VFAT data in the fVFATChannel[][] array
  Int_t fVAFTChannelData[N_VFAT][128];//stores the VFAT data. Each VFAT has 128 bits of data
  Int_t fVAFTChipID[N_VFAT][VFAT_CID_SIZE];//stores the VFAT chip Id
  Int_t fVAFTBC[N_VFAT][VFAT_BC_SIZE];//stores the VFAT Bean Count
  Int_t fVAFTEC[N_VFAT][VFAT_EC_SIZE];//stores the VFAT Event Count
  Int_t fVAFTFLG[N_VFAT][VFAT_FLG_SIZE];//stores the VFAT Flags

  std::vector< QwHit > fGEMHits;

  /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms()
   */

  TH1D *VFAT[N_VFAT];
  TH1D *VFAT_BC[N_VFAT];
  TH1D *VFAT_EC[N_VFAT];
  TH1D *VFAT_Flags[N_VFAT];
  TH1D *VFAT_ChipId[N_VFAT];
  TH1D *VFAT_ChannelData[N_VFAT];
  TH2D *GEM[N_GEM];



};

#endif
