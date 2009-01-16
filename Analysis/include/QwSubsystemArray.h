/**********************************************************\
* File: QwSubsystemArray.h                                 *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-22 15:50>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAY__
#define __QWSUBSYSTEMARRAY__

#include <vector>
#include <Rtypes.h>
#include <TString.h>
#include <TDirectory.h>

#include <boost/shared_ptr.hpp>
#include <boost/mem_fn.hpp>

#include "VQwSubsystem.h"

#include "QwHitContainer.h"


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

  void  ClearEventData();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
				   buffer, UInt_t num_words);
  
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
			buffer, UInt_t num_words);

  void  ProcessEvent();

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};

  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();
  void  DeleteHistograms();
  

  void GetHitList(QwHitContainer & grandHitList);

 protected:


};


void QwSubsystemArray::push_back(VQwSubsystem* subsys){
    if (subsys==NULL){
      std::cerr << "QwSubsystemArray::push_back():  NULL subsys"
		<< std::endl;
      //  This is an empty subsystem...
      //  Do nothing for now.
    } else if (!this->empty() && GetSubsystem(subsys->GetSubsystemName())){
      //  There is already a subsystem with this name!
    } else {
      boost::shared_ptr<VQwSubsystem> tmpptr(subsys);
      SubsysPtrs::push_back(tmpptr);
    }
  };


VQwSubsystem* QwSubsystemArray::GetSubsystem(const TString name)
{
  VQwSubsystem* tmp = NULL;
  if (!empty()){
    for (const_iterator subsys = begin(); subsys != end(); ++subsys){
      if ((*subsys)->GetSubsystemName() == name){
	tmp = (*subsys).get();
      } else {
      }
    }
  }
  return tmp;
};


void  QwSubsystemArray::ClearEventData()
{
  if (!empty())
    std::for_each(begin(), end(), 
		  boost::mem_fn(&VQwSubsystem::ClearEventData));
};
   
Int_t QwSubsystemArray::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
				 buffer, UInt_t num_words)
{
  Int_t status = 0;
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      status += (*subsys)->ProcessConfigurationBuffer(roc_id, bank_id, buffer, num_words);
    }
  return status;
};
  
Int_t QwSubsystemArray::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
		      buffer, UInt_t num_words)
{
  Int_t status = 0;
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      status += (*subsys)->ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
    }
  return status;
};




void  QwSubsystemArray::ProcessEvent()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent));
};

void  QwSubsystemArray::ConstructHistograms(TDirectory *folder)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructHistograms(folder);
    }
};

void  QwSubsystemArray::FillHistograms()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::FillHistograms));
};

void  QwSubsystemArray::DeleteHistograms()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::DeleteHistograms));



};


void QwSubsystemArray::GetHitList(QwHitContainer & grandHitList){
  if (!empty()){
    grandHitList.clear();
    
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      
      ((subsys)->get())->GetHitList(grandHitList);
    }
  }
};

#endif
