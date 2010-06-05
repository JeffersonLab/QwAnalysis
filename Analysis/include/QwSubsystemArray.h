/**********************************************************\
* File: QwSubsystemArray.h                                 *
*                                                          *
* Author: P. M. King,  Rakitha Beminiwattha                *
* Time-stamp: <2008-07-22 15:50>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAY__
#define __QWSUBSYSTEMARRAY__

#include <vector>
#include <map>
#include <Rtypes.h>
#include <TString.h>
#include <TDirectory.h>
#include <TTree.h>

#include <boost/shared_ptr.hpp>
#include <boost/mem_fn.hpp>

// Qweak headers
#include "VQwSubsystem.h"
#include "QwLog.h"

///
/// \ingroup QwAnalysis
class QwSubsystemArray:  public std::vector<boost::shared_ptr<VQwSubsystem> > {
 private:
  typedef std::vector<boost::shared_ptr<VQwSubsystem> >  SubsysPtrs;
 public:
  using SubsysPtrs::const_iterator;
  using SubsysPtrs::iterator;
  using SubsysPtrs::begin;
  using SubsysPtrs::end;
  using SubsysPtrs::size;
  using SubsysPtrs::empty;

 public:
  /// \brief Default constructor
  QwSubsystemArray() {};
  /// \brief Constructor with filename
  QwSubsystemArray(const char* filename);
  /// \brief Virtual destructor
  virtual ~QwSubsystemArray() {};

  /// \brief Add the subsystem to this array
  void push_back(VQwSubsystem* subsys);

  /// \brief Get the subsystem with the specified name
  virtual VQwSubsystem* GetSubsystemByName(const TString& name);

  /// \brief Get the list of subsystems of the specified type
  virtual std::vector<VQwSubsystem*> GetSubsystemByType(const TString& type);

  //each of the methods below will call their counterpart method separately.

  void  ClearEventData();

  /// \brief Process the event buffer for configuration events
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
                                   buffer, UInt_t num_words);

  /// \brief Process the event buffer for events
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
                        buffer, UInt_t num_words);

  /// \brief Randomize the data in this event
  void  RandomizeEventData(int helicity = 0, double time = 0.0);

  /// \brief Encode the data in this event
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  /// \brief Process the decoded data in this event
  void  ProcessEvent();

  /**
   * Retrieve the variable name from other subsystem arrays
   * @param name Variable name to be retrieved
   * @param value (return) Data element with the variable name
   * @return True if the variable is found, false if not found
   */
  const Bool_t RequestExternalValue(TString name, VQwDataElement* value) const {
    //  If this has a parent, we should escalate the call to that object,
    //  but so far we don't have that capability.
    return ReturnInternalValue(name, value);
  };

  /**
   * Retrieve the variable name from subsystems in this subsystem array
   * @param name Variable name to be retrieved
   * @param value (return) Data element with the variable name
   * @return True if the variable was found, false if not found
   */
  const Bool_t ReturnInternalValue(TString name, VQwDataElement* value) const {
    Bool_t foundit = kFALSE;
    //  First try to find the value in the list of published values.
    //  So far this list is not filled though.
    std::map<TString, VQwSubsystem*>::const_iterator iter = fPublishedValuesSubsystem.find(name);
    if (iter != fPublishedValuesSubsystem.end()) {
      foundit = (iter->second)->ReturnInternalValue(name, value);
    }
    //  If the value is not yet published, try asking the subsystems for it.
    for (const_iterator subsys = begin(); (!foundit)&&(subsys != end()); ++subsys){
      foundit = (*subsys)->ReturnInternalValue(name, value);
    }
    return foundit;
  };

  /**
   * Publish the value name with description from a subsystem in this array
   * @param name Name of the variable
   * @param desc Description of the variable
   * @param subsys Subsystem that contains the variable
   * @return True if the variable could be published, false if not published
   */
  const Bool_t PublishInternalValue(const TString name, const TString desc, const VQwSubsystem* subsys) {
    if (fPublishedValuesSubsystem.count(name) > 0) {
      QwError << "Attempting to publish existing variable key!" << QwLog::endl;
      ListPublishedValues();
      return kFALSE;
    }
    fPublishedValuesSubsystem[name] = const_cast<VQwSubsystem*>(subsys);
    fPublishedValuesDescription[name] = desc;
    return kTRUE;
  };

  /**
   * List the published values and description in this subsystem array
   */
  void ListPublishedValues() const {
    QwOut << "List of published values:" << QwLog::endl;
    std::map<TString,TString>::const_iterator iter;
    for (iter  = fPublishedValuesDescription.begin();
         iter != fPublishedValuesDescription.end(); iter++) {
      QwOut << iter->first << ": " << iter->second << QwLog::endl;
    }
  };


  /// \name Histogram construction and maintenance
  // @{
  /// Construct the histograms for this subsystem
  void  ConstructHistograms() {
    ConstructHistograms((TDirectory*) NULL);
  };
  /// Construct the histograms for this subsystem in a folder
  void  ConstructHistograms(TDirectory *folder) {
    TString prefix = "";
    ConstructHistograms(folder, prefix);
  };
  /// \brief Construct the histograms for this subsystem in a folder with a prefix
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  /// \brief Fill the histograms for this subsystem
  void  FillHistograms();
  /// \brief Delete the histograms for this subsystem
  void  DeleteHistograms();
  // @}

  /// \name Tree construction and maintenance
  /// These functions are not purely virtual, since not every subsystem is
  /// expected to implement them.  They are intended for expert output to
  /// trees.
  // @{
  /// Construct the tree for this subsystem
  void  ConstructTree() {
    ConstructTree((TDirectory*) NULL);
  };
  /// Construct the tree for this subsystem in a folder
  void  ConstructTree(TDirectory *folder) {
    TString prefix = "";
    ConstructTree(folder, prefix);
  };
  /// \brief Construct the tree for this subsystem in a folder with a prefix
  void  ConstructTree(TDirectory *folder, TString &prefix);
  /// \brief Fill the tree for this subsystem
  void  FillTree();
  /// \brief Delete the tree for this subsystem
  void  DeleteTree();
  // @}


  /// \brief Print some information about the subsystem
  void Print();

 protected:

  std::map<TString, VQwSubsystem*> fPublishedValuesSubsystem;
  std::map<TString, TString>       fPublishedValuesDescription;


};





#endif
