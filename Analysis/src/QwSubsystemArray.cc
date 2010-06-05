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
#include "QwParameterFile.h"

//*****************************************************************

/**
 * Create a subsystem array based on the content of a map file.
 * @param filename Map file
 */
QwSubsystemArray::QwSubsystemArray(const char* filename)
{
  QwMessage << "Loading subsystems from " << filename << QwLog::endl;
  QwParameterFile detectors(filename);

  VQwSubsystem* subsys = 0;
  while (detectors.ReadNextLine()) {
    // Trim comments and whitespace
    detectors.TrimComment('!');
    detectors.TrimComment('#');
    detectors.TrimWhitespace();

    // Subsystem block start
    std::string subsystype, subsysname;
    if (detectors.HasVariablePair(":", subsystype, subsysname)) {
      QwMessage << "Adding subsystem of type " << subsystype
                << " with name " << subsysname << QwLog::endl;
      subsys = VQwSubsystemFactory::GetSubsystemFactory(subsystype)->Create(subsysname);
      this->push_back(subsys);
    }

    std::string key, value;

    // Map file definition
    if (detectors.HasVariablePair("=", key, value)) {
      if (key == "map" && value.size() > 0) {
        if (subsys) subsys->LoadChannelMap(value);
        else QwError << "Map defined without subsystem!" << QwLog::endl;
      }
    }

    // Geometry file definition
    if (detectors.HasVariablePair("=", key, value)) {
      if (key == "geom" && value.size() > 0) {
        if (subsys) subsys->LoadGeometryDefinition(value);
        else QwError << "Geometry defined without subsystem!" << QwLog::endl;
      }
    }

    // Parameter file definition
    if (detectors.HasVariablePair("=", key, value)) {
      if (key == "param" && value.size() > 0) {
        if (subsys) subsys->LoadInputParameters(value);
        else QwError << "Parameters defined without subsystem!" << QwLog::endl;
      }
    }
  }
}


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
  } else if (!this->empty() && GetSubsystemByName(subsys->GetSubsystemName())){
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
VQwSubsystem* QwSubsystemArray::GetSubsystemByName(const TString& name)
{
  VQwSubsystem* tmp = NULL;
  if (!empty()) {
    // Loop over the subsystems
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {
      // Check the name of this subsystem
      // std::cout<<"QwSubsystemArray::GetSubsystemByName available name=="<<(*subsys)->GetSubsystemName()<<"== to be compared to =="<<name<<"==\n";
      if ((*subsys)->GetSubsystemName() == name) {
        tmp = (*subsys).get();
        //std::cout<<"QwSubsystemArray::GetSubsystemByName found a matching name \n";
      } else {
        // nothing
      }
    }
  }
  return tmp;
};


/**
 * Get the list of subsystems in this array of the spcified type
 * @param type Type of the subsystem
 * @return Vector of subsystems
 */
std::vector<VQwSubsystem*> QwSubsystemArray::GetSubsystemByType(const TString& type)
{
  // Vector of subsystem pointers
  std::vector<VQwSubsystem*> subsys_list;

  // If this array is not empty
  if (!empty()) {

    // Create dummy subsystem of requested type
    VQwSubsystem* subsys_of_requested_type =
      VQwSubsystemFactory::GetSubsystemFactory(type)->Create("dummy");

    // Loop over the subsystems
    for (const_iterator subsys = begin(); subsys != end(); ++subsys) {

      // Test for equality of types (typeid is pointer to type_info, so dereference)
      if (typeid(*(*subsys).get()) == typeid(*subsys_of_requested_type)) {
        subsys_list.push_back((*subsys).get());
      }

    } // end of loop over subsystems

    // Delete dummy subsystem again
    delete subsys_of_requested_type;

  } // end of if !empty()

  return subsys_list;
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


//*****************************************************************
void  QwSubsystemArray::RandomizeEventData(int helicity, double time)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      (*subsys)->RandomizeEventData(helicity, time);
    }
};

//*****************************************************************
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
