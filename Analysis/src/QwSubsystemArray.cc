/**********************************************************\
* File: QwSubsystemArray.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwSubsystemArray.h"
#include "QwHistogramHelper.h"
#include <stdexcept>



//*****************************************************************

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
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessConfigurationBuffer(roc_id, bank_id, buffer, num_words);
    }
};
  
Int_t QwSubsystemArray::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
		      buffer, UInt_t num_words)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
    }
};


void  QwSubsystemArray::ProcessEvent()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent));
};


//*****************************************************************

void  QwSubsystemArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      //      std::cout<<" here in QwSubsystemArray::ConstructHistograms \n";
      (*subsys)->ConstructHistograms(folder,prefix);
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

//*****************************************************************

