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

//*****************************************************************

/**
 * Create a subsystem array based on the configuration option 'detectors'
 */
QwSubsystemArray::QwSubsystemArray(QwOptions& options)
{
  const char* filename = options.GetValue<std::string>("detectors").c_str();
  QwMessage << "Loading subsystems from " << filename << QwLog::endl;
  QwParameterFile detectors(filename);
  LoadSubsystemsFromParameterFile(detectors);
}

/**
 * Create a subsystem array with the contents of a map file
 * @param filename Map file
 */
QwSubsystemArray::QwSubsystemArray(const char* filename)
{
  QwMessage << "Loading subsystems from " << filename << QwLog::endl;
  QwParameterFile detectors(filename);
  LoadSubsystemsFromParameterFile(detectors);
}

/**
 * Fill the subsystem array with the contents of a map file
 * @param filename Map file
 */
void QwSubsystemArray::LoadSubsystemsFromParameterFile(QwParameterFile& detectors)
{
  // This is how this should work
  QwParameterFile* preamble;
  preamble = detectors.ReadPreamble();
  // Process preamble
  QwVerbose << "Preamble:" << QwLog::endl;
  QwVerbose << *preamble << QwLog::endl;

  QwParameterFile* section;
  std::string section_name;
  while ((section = detectors.ReadNextSection(section_name))) {
    // Process section
    QwMessage << "Adding subsystem of type " << section_name << QwLog::endl;
    QwVerbose << "Section:" << QwLog::endl;
    QwVerbose << *section << QwLog::endl;

    // Determine type and name of subsystem
    std::string subsys_type = section_name;
    std::string subsys_name;
    if (! section->FileHasVariablePair("=","name",subsys_name)) {
      QwError << "No name defined in section for subsystem " << subsys_type << QwLog::endl;
      continue;
    }
    // Create subsystem
    VQwSubsystem* subsys =
      VQwSubsystemFactory::GetSubsystemFactory(subsys_type)->Create(subsys_name);
    if (! subsys) {
      QwError << "Could not create subsystem " << subsys_type << QwLog::endl;
      continue;
    }
    // Pass detector maps
    subsys->LoadDetectorMaps(*section);
    // Add to array
    this->push_back(subsys);

    // Delete parameter file section
    delete section; section = 0;
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
 * Define configuration options for global array
 * @param options Options
 */
void QwSubsystemArray::DefineOptions(QwOptions &options)
{
  options.AddOptions()("detectors",
                       po::value<std::string>()->default_value("detectors.map"),
                       "map file with detectors to include");
}

/**
 * Handle configuration options for all subsystems in the array
 * @param options Options
 */
void QwSubsystemArray::ProcessOptions(QwOptions &options)
{
  for (iterator subsys_iter = begin(); subsys_iter != end(); ++subsys_iter) {
    VQwSubsystem* subsys = dynamic_cast<VQwSubsystem*>(subsys_iter->get());
    subsys->ProcessOptions(options);
  }
}


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
  if (!empty()) {
    fCodaEventNumber = 0;
    fCodaEventType   = 0;
    std::for_each(begin(), end(),
		  boost::mem_fn(&VQwSubsystem::ClearEventData));
  }
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
