/*!
 *
 * \file QwSubsystemArrayTracking.cc
 *
 * \brief Implementation of the tracking subsystem array
 */

#include "QwSubsystemArrayTracking.h"

// Qweak headers
#include "QwDriftChamber.h"

/**
 * Get the subsystem with the specified name
 * @param name Name of the subsystem
 * @return Subsystem with that name
 */
VQwSubsystemTracking* QwSubsystemArrayTracking::GetSubsystemByName(const TString& name)
{
  return dynamic_cast<VQwSubsystemTracking*>(QwSubsystemArray::GetSubsystemByName(name));
};

/**
 * Get the hit list from the subsystems in this array
 * @param hitlist Hit container to store the hits in
 */
void QwSubsystemArrayTracking::GetHitList(QwHitContainer& hitlist)
{
  if (!empty()){
    hitlist.clear();

    for (iterator subsys = begin(); subsys != end(); ++subsys){
      VQwSubsystemTracking* tsubsys =
        dynamic_cast<VQwSubsystemTracking*>((subsys)->get());
      if (tsubsys != NULL){
        tsubsys->GetHitList(hitlist);
      } else {
        std::cerr << "QwSubsystemArrayTracking::GetHitList: Subsystem \""
                  << ((subsys)->get())->GetSubsystemName()
                  << "\" isn't a tracking subsystem." << std::endl;
      }
    }
  }
};


/**
 * Construct the branch and tree vector
 * @param tree Tree
 * @param prefix Prefix
 * @param values Vector of values
 */
void  QwSubsystemArrayTracking::ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values)
{
  fTreeArrayIndex = values.size();
  values.push_back(0.0);
  tree->Branch("CodaEventNumber",&(values[fTreeArrayIndex]),"CodaEventNumber/D" );
  values.push_back(0.0);
  tree->Branch("CodaEventType",&(values[fTreeArrayIndex+1]),"CodaEventType/D" );

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemTracking* subsys_tracking = dynamic_cast<VQwSubsystemTracking*>(subsys->get());
    subsys_tracking->ConstructBranchAndVector(tree, prefix, values);
  }
};


/**
 * Construct the branch for the flat tree
 * @param tree Tree
 * @param prefix Prefix
 */
void  QwSubsystemArrayTracking::ConstructBranch(TTree *tree, TString& prefix)
{
  tree->Branch("CodaEventNumber",&fCodaEventNumber);
  tree->Branch("CodaEventType",&fCodaEventType);

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemTracking* subsys_tracking = dynamic_cast<VQwSubsystemTracking*>(subsys->get());
    subsys_tracking->ConstructBranch(tree, prefix);
  }
};


/**
 * Construct the branch for the flat tree with tree trim files accepted
 * @param tree Tree
 * @param prefix Prefix
 * @param trim_file Trim file
 */
void  QwSubsystemArrayTracking::ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file)
{
  QwMessage <<" QwSubsystemArrayTracking::ConstructBranch "<<QwLog::endl;

  QwParameterFile* preamble;
  QwParameterFile* nextsection;
  preamble = trim_file.ReadPreamble();

  // Process preamble
  QwVerbose << "QwSubsystemArrayTracking::ConstructBranch  Preamble:" << QwLog::endl;
  QwVerbose << *preamble << QwLog::endl;

  tree->Branch("CodaEventNumber",&fCodaEventNumber);
  tree->Branch("CodaEventType",&fCodaEventType);

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemTracking* subsys_tracking = dynamic_cast<VQwSubsystemTracking*>(subsys->get());

    TString subsysname = subsys_tracking->GetSubsystemName();
    //QwMessage << "Tree leaves created for " << subsysname << QwLog::endl;

    if (trim_file.FileHasSectionHeader(subsysname)) {
      // This section contains modules and their channels to be included in the tree
      nextsection = trim_file.ReadUntilNextSection();
      subsys_tracking->ConstructBranch(tree, prefix, *nextsection);
      QwMessage << "Tree leaves created for " << subsysname << QwLog::endl;
    } else
      QwMessage << "No tree leaves created for " << subsysname << QwLog::endl;
  }
};


/**
 * Fill the tree vector
 * @param values Vector of values
 */
void  QwSubsystemArrayTracking::FillTreeVector(std::vector<Double_t>& values)
{
  size_t index = fTreeArrayIndex;
  values[index++] = this->GetCodaEventNumber();
  values[index++] = this->GetCodaEventType();

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemTracking* subsys_tracking = dynamic_cast<VQwSubsystemTracking*>(subsys->get());
    subsys_tracking->FillTreeVector(values);
  }
};

