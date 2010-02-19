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

#include "VQwSubsystem.h"

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
  QwSubsystemArray() {};
  ~QwSubsystemArray(){
  };

  /// \brief Add the subsystem to this array
  void push_back(VQwSubsystem* subsys);

  /// \brief Get the subsystem with the specified name
  VQwSubsystem* GetSubsystem(const TString& name);

  //each of the methods below will call their counterpart method separately.

  void  ClearEventData();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
                                   buffer, UInt_t num_words);

  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
                        buffer, UInt_t num_words);

  void  RandomizeEventData(int helicity = 0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();

  Bool_t RequestExternalValue(TString name, VQwDataElement* value) const {
    //  If this has a parent, we should escalate the call to that object,
    //  but so far we don't have that capability.
    return ReturnInternalValue(name, value);
  };

  Bool_t ReturnInternalValue(TString name, VQwDataElement* value) const {
    Bool_t foundit = kFALSE;
    //  First try to find the value in the list of published values.
    //  So far this list is not filled though.
    std::map<TString, VQwSubsystem*>::const_iterator iter = fPublishedValues.find(name);
    if( iter != fPublishedValues.end() ) {
      foundit = (iter->second)->ReturnInternalValue(name, value);
    } 
    //  If the value is not yet published, try asking the subsystems for it.
    for (const_iterator subsys = begin(); (!foundit)&&(subsys != end()); ++subsys){
      foundit = (*subsys)->ReturnInternalValue(name, value);
    }
    return foundit;
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

  std::map<TString, VQwSubsystem*>  fPublishedValues;


};





#endif
