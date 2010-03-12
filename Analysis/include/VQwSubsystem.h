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

#include "TString.h"
#include "TDirectory.h"


// Qweak headers
#include "QwParameterFile.h"

// Forward declarations
class QwSubsystemArray;
class VQwDataElement;

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

  /// \brief Define options function (note: no virtual static functions in C++)
  static void DefineOptions() { /* No default options defined */ };

  TString GetSubsystemName() const {return fSystemName;};
  Bool_t  HasDataLoaded() const  {return fIsDataLoaded;}

  /// \brief Set the parent of this subsystem to the specified array
  void SetParent(QwSubsystemArray* parent);
  /// \brief Get the parent of this subsystem
  QwSubsystemArray* GetParent(const unsigned int parent = 0) const;
  /// \brief Get the sibling with specified name
  VQwSubsystem* GetSibling(const TString& name) const;

  /// \brief Publish a variable name to the parent subsystem array
  const Bool_t PublishInternalValue(const TString name, const TString desc) const;
  /// \brief Publish all variables of the subsystem
  virtual const Bool_t PublishInternalValues() const {
    return kTRUE; // when not implemented, this returns success
  };

  /// \brief Request a named value which is owned by an external subsystem;
  ///        the request will be handled by the parent subsystem array
  const Bool_t RequestExternalValue(TString name, VQwDataElement* value) const;

  /// \brief Return a named value to the parent subsystem array to be
  ///        delivered to a different subsystem.
  virtual const Bool_t ReturnInternalValue(TString name,
				      VQwDataElement* value) const {
    return kFALSE;
  };

  virtual Int_t LoadChannelMap(TString mapfile) = 0;
  virtual Int_t LoadInputParameters(TString mapfile) = 0;
  virtual void  ClearEventData() = 0;

  virtual Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual void  ProcessEvent() = 0;
  /*! \brief Request processed data from other subsystems for internal
   *         use in the second event processing stage.  Not all derived
   *         classes will require data from other subsystems.
   */
  virtual void  ExchangeProcessedData() { };
  /*! \brief Process the event data again, including data from other
   *         subsystems.  Not all derived classes will require
   *         a second stage of event data processing.
   */
  virtual void  ProcessEvent_2() { };


  // Not all derived classes will have the following functions
  virtual void  RandomizeEventData(int helicity) { };
  virtual void  EncodeEventData(std::vector<UInt_t> &buffer) { };

  /// \name Histogram construction and maintenance
  // @{
  /// Construct the histograms for this subsystem
  virtual void  ConstructHistograms() {
    TString tmpstr("");
    ConstructHistograms((TDirectory*) NULL, tmpstr);
  };
  /// Construct the histograms for this subsystem in a folder
  virtual void  ConstructHistograms(TDirectory *folder) {
    TString tmpstr("");
    ConstructHistograms(folder, tmpstr);
  };
  /// Construct the histograms for this subsystem with a prefix
  virtual void  ConstructHistograms(TString &prefix) {
    ConstructHistograms((TDirectory*) NULL, prefix);
  };
  /// \brief Construct the histograms for this subsystem in a folder with a prefix
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  /// \brief Fill the histograms for this subsystem
  virtual void  FillHistograms() = 0;
  /// \brief Delete the histograms for this subsystem
  virtual void  DeleteHistograms() = 0;
  // @}

  /// \name Tree construction and maintenance
  /// These functions are not purely virtual, since not every subsystem is
  /// expected to implement them.  They are intended for expert output to
  /// trees.
  // @{
  /// \brief Construct the tree for this subsystem
  virtual void  ConstructTree() {
    TString tmpstr("");
    ConstructTree((TDirectory*) NULL, tmpstr);
  };
  /// \brief Construct the tree for this subsystem in a folder
  virtual void  ConstructTree(TDirectory *folder) {
    TString tmpstr("");
    ConstructTree(folder, tmpstr);
  };
  /// \brief Construct the tree for this subsystem with a prefix
  virtual void  ConstructTree(TString &prefix) {
    ConstructTree((TDirectory*) NULL, prefix);
  };
  /// \brief Construct the tree for this subsystem in a folder with a prefix
  virtual void  ConstructTree(TDirectory *folder, TString &prefix) { return; };
  /// \brief Fill the tree for this subsystem
  virtual void  FillTree() { return; };
  /// \brief Delete the tree for this subsystem
  virtual void  DeleteTree() { return; };
  // @}

  /// \brief Print some information about the subsystem
  virtual void  Print();

  virtual void Copy(VQwSubsystem *source);//Must call at the beginning of all subsystems rotuine call to Copy(VQwSubsystem *source)  by  using VQwSubsystem::Copy(source)
  virtual VQwSubsystem&  operator=  (VQwSubsystem *value);//Must call at the beginning of all subsystems rotuine call to operator=  (VQwSubsystem *value)  by VQwSubsystem::operator=(value)


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

  /// Vector of pointers to subsystem arrays that contain this subsystem
  std::vector<QwSubsystemArray*> fArrays;


 protected:
  VQwSubsystem(){};  //  Private constructor.
  VQwSubsystem&  operator=  (const VQwSubsystem &value){
    return *this;
  };


}; // class VQwSubsystem

#endif // __VQWSUBSYSTEM__
