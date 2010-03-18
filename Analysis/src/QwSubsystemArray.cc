/**********************************************************\
* File: QwSubsystemArray.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwSubsystemArray.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"


//*****************************************************************

/**
 * Add the subsystem to this array.  Do nothing if the subsystem is null or if
 * there is already a subsystem with that name in the array.
 * @param subsys Subsystem to add to the array
 */
void QwSubsystemArray::push_back(VQwSubsystem* subsys)
{
  if (subsys == NULL) {
    std::cerr << "QwSubsystemArray::push_back():  NULL subsys"
              << std::endl;
    //  This is an empty subsystem...
    //  Do nothing for now.
  } else if (!this->empty() && GetSubsystem(subsys->GetSubsystemName())){
    //  There is already a subsystem with this name!
    std::cerr << "QwSubsystemArray::push_back():  subsys" << subsys->GetSubsystemName()
              << " already exists" << std::endl;
  } else {
    boost::shared_ptr<VQwSubsystem> subsys_tmp(subsys);
    SubsysPtrs::push_back(subsys_tmp);
    // Set the parent of the subsystem to this array
    subsys_tmp->SetParent(this);
    // Instruct the subsystem to publish variables
    if (subsys_tmp->PublishInternalValues() == kFALSE) {
      QwError << "Not all variables for " << subsys_tmp->GetSubsystemName()
              << " could be published!" << QwLog::endl;
    }
  }
};


/**
 * Get the subsystem in this array with the spcified name
 * @param name Name of the subsystem
 * @return Pointer to the subsystem
 */
VQwSubsystem* QwSubsystemArray::GetSubsystem(const TString& name)
{
  VQwSubsystem* tmp = NULL;
  if (!empty()) {
    // Loop over the subsystems
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {
      // Check the name of this subsystem
      // std::cout<<"QwSubsystemArray::GetSubsystem available name=="<<(*subsys)->GetSubsystemName()<<"== to be compared to =="<<name<<"==\n";
      if ((*subsys)->GetSubsystemName() == name) {
	tmp = (*subsys).get();
	//std::cout<<"QwSubsystemArray::GetSubsystem found a matching name \n";
      } else {
        // nothing
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
  return 0;
};

Int_t QwSubsystemArray::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
		      buffer, UInt_t num_words)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
    }
  return 0;
};


void  QwSubsystemArray::ProcessEvent()
{
  if (!empty()){
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent));
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ExchangeProcessedData));
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent_2));
  }
};


void  QwSubsystemArray::RandomizeEventData(int helicity)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->RandomizeEventData(helicity);
    }
};
void  QwSubsystemArray::EncodeEventData(std::vector<UInt_t> &buffer)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->EncodeEventData(buffer);
    }
};


//*****************************************************************

void  QwSubsystemArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //std::cout<<" here in QwSubsystemArray::ConstructHistograms with prefix ="<<prefix<<"\n";

  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructHistograms(folder,prefix);
    }
  }
  //std::cout<<"\n";
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

/**
 * Construct the tree for this subsystem
 * @param folder Directory where to construct the tree
 * @param prefix Prefix for the name of the tree
 */
void  QwSubsystemArray::ConstructTree(TDirectory *folder, TString &prefix)
{
  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ConstructTree(folder, prefix);
    }
  }
};

/**
 * Fill the tree for this subsystem
 */
void  QwSubsystemArray::FillTree()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::FillTree));
};

/**
 * Delete the tree for this subsystem
 */
void  QwSubsystemArray::DeleteTree()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::DeleteTree));
};

//*****************************************************************

void  QwSubsystemArray::Print()
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys)
      std::cout << (*subsys)->GetSubsystemName() << std::endl;
};
