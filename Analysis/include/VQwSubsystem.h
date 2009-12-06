/*!
 * \file   VQwSubsystem.h
 * \brief  Definition of the pure virtual base class of all subsystems
 *
 * \author P. M. King
 * \date   2007-05-08 15:40
 */

#ifndef __VQWSUBSYSTEM__
#define __VQWSUBSYSTEM__

#include <iostream>
#include <vector>
#include <Rtypes.h>
#include <TString.h>
#include <TDirectory.h>

#include "QwParameterFile.h"

/**
 *  \class   VQwSubsystem
 *  \ingroup QwAnalysis
 *  \brief   The pure virtual base class of all subsystems
 *
 * Virtual base class for the classes containing the event-based
 * information from each tracking subsystem and the parity analysis.
 * The subclasses VQwSubsystemTracking and VQwSubsystemParity are base
 * classes for the classes containing the event-based information
 * from each tracking subsystem and parity analysis, respectively.
 *
 * \dot
 * digraph example {
 *   node [shape=round, fontname=Helvetica, fontsize=10];
 *   VQwSubsystem [ label="VQwSubsystem" URL="\ref VQwSubsystem"];
 *   VQwSubsystemParity [ label="VQwSubsystemParity" URL="\ref VQwSubsystemParity"];
 *   VQwSubsystemTracking [ label="VQwSubsystemTracking" URL="\ref VQwSubsystemTracking"];
 *   VQwSubsystem -> VQwSubsystemParity;
 *   VQwSubsystem -> VQwSubsystemTracking;
 * }
 * \enddot
 *
 * This will define the interfaces used in communicating with the
 * CODA routines.
 */
class VQwSubsystem {

 public:

  VQwSubsystem(TString region_tmp):fSystemName(region_tmp),fIsDataLoaded(kFALSE),fCurrentROC_ID(-1),fCurrentBank_ID(-1){};

  virtual ~VQwSubsystem(){};

  TString GetSubsystemName() const {return fSystemName;};
  Bool_t  HasDataLoaded() const  {return fIsDataLoaded;}

  virtual Int_t LoadChannelMap(TString mapfile) = 0;
  virtual Int_t LoadInputParameters(TString mapfile) = 0;
  virtual void  ClearEventData() = 0;

  virtual Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual void  ProcessEvent() = 0;

  // Not all derived classes will have the following functions
  virtual void  RandomizeEventData(int helicity) { };
  virtual void  EncodeEventData(std::vector<UInt_t> &buffer) { };


  /*! \brief Construct the histograms for this subsystem
   */
  virtual void  ConstructHistograms()
    {
      TString tmpstr("");
      ConstructHistograms((TDirectory*)NULL, tmpstr);
    };
  /*! \brief Construct the histograms for this subsystem in a folder
   */
  virtual void  ConstructHistograms(TDirectory *folder)
    {
      TString tmpstr("");
      ConstructHistograms(folder,tmpstr);
    };
  /*! \brief Construct the histograms for this subsystem with a prefix
   */
  virtual void  ConstructHistograms(TString &prefix)
    {
      ConstructHistograms((TDirectory*)NULL, prefix);
    };
  /*! \brief Construct the histograms for this subsystem in a folder with a prefix
   */
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  /*! \brief Fill the histograms for this subsystem
   */
  virtual void  FillHistograms() = 0;
  /*! \brief Delete the histograms for this subsystem
   */
  virtual void  DeleteHistograms() = 0;

  /*! \brief Print some information about the subsystem
   */
  virtual void  Print();


 protected:

  /*! \brief Clear all registration of ROC and Bank IDs for this subsystem
   */
  void  ClearAllBankRegistrations();

  /*! \brief Tell the object that it will decode data from this ROC and sub-bank
   */
  Int_t RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id);

  /*! \brief Tell the object that it will decode data from this sub-bank in the ROC currently open for registration
   */
  Int_t RegisterSubbank(const UInt_t bank_id);

  Int_t GetSubbankIndex(const UInt_t roc_id, const UInt_t bank_id) const;
  void  SetDataLoaded(Bool_t flag){fIsDataLoaded = flag;};

 protected:
  Int_t FindIndex(const std::vector<UInt_t> &myvec, const UInt_t value) const ;

 protected:

  TString  fSystemName; ///< Name of this subsystem

  Bool_t   fIsDataLoaded; ///< Has this subsystem gotten data to be processed?

 protected:

  Int_t fCurrentROC_ID; ///< ROC ID that is currently being processed
  Int_t fCurrentBank_ID; ///< Bank ID that is currently being processed

  /// Vector of ROC IDs associated with this subsystem
  std::vector<UInt_t> fROC_IDs;
  /// Vector of Bank IDs per ROC ID associated with this subsystem
  std::vector< std::vector<UInt_t> > fBank_IDs;




 protected:
  VQwSubsystem(){};  //  Private constructor.
  VQwSubsystem&  operator=  (const VQwSubsystem &value){
    return *this;
  };


}; // class VQwSubsystem

#endif // __VQWSUBSYSTEM__
