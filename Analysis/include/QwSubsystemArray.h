/**********************************************************\
* File: QwSubsystemArray.h                                 *
*                                                          *
* Author: P. M. King,  Rakitha Beminiwattha                *
* Time-stamp: <2008-07-22 15:50>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAY__
#define __QWSUBSYSTEMARRAY__

#include <vector>
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

  void push_back(VQwSubsystem* subsys);

  VQwSubsystem* GetSubsystem(const TString name);

  //each of the methods below will call their counterpart method separately.

  void  ClearEventData();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
                                   buffer, UInt_t num_words);

  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
                        buffer, UInt_t num_words);

  void  RandomizeEventData(int helicity = 0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  void  ConstructHistograms(TDirectory *folder){TString prefix="";ConstructHistograms(folder,prefix);};
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();


 protected:



};





#endif
