/*
 * QwCorrelation.cc
 *
 *  Created on: Oct 3, 2012
 *      Author: wdconinc
 */

#include "QwCorrelation.h"


/** Constructor with only helicity pattern
 *
 * @param options Obtions object
 * @param helicitypattern Helicity Pattern
 *
 */
QwCorrelation::QwCorrelation(QwOptions &options, QwHelicityPattern& helicitypattern)
{
  ProcessOptions(options);
  LoadChannelMap(fCorrelationMapFile);
  fHelicityPattern = &helicitypattern;
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(asym,diff);
}



/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCorrelation::DefineOptions(QwOptions &options)
{
  options.AddOptions("Linear regression")
    ("correlation-map", po::value<std::string>()->default_value("correlation.map"),
     "variables for correlation analysis");
}

/**
 * Process configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCorrelation::ProcessOptions(QwOptions &options)
{
  fCorrelationMapFile = options.GetValue<std::string>("correlation-map");
}


/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwRegression::LoadChannelMap(const std::string& mapfile)
{
  // Open the file
  QwParameterFile map(mapfile);

  // Read the sections of dependent variables
  bool keep_header = true;
  std::string section_name;
  QwParameterFile* section = 0;
  std::pair<EQwRegType,std::string> type_name;
  while ((section = map.ReadNextSection(section_name,keep_header))) {

    // Store index to the current position in the dv vector
    size_t current_dv_start = fDependentName.size();

    // Add dependent variables from the section header
    section->ReadNextLine();
    if (section->LineHasSectionHeader()) {
      section->TrimSectionHeader();
      section->TrimWhitespace();
      // Parse section header into tokens separated by a comma
      std::string current_token;
      std::string previous_token;
      do {
        previous_token = current_token;
        current_token = section->GetNextToken(",");
        if (current_token.size() == 0) continue;
        // Parse current token into dependent variable type and name
        type_name = ParseRegressionVariable(current_token);
        fDependentType.push_back(type_name.first);
        fDependentName.push_back(type_name.second);
        // Resize the vectors of sensitivities and independent variables
        fSensitivity.resize(fDependentName.size());
        fIndependentType.resize(fDependentName.size());
        fIndependentName.resize(fDependentName.size());
      } while (current_token.size() != 0);
    } else QwError << "Section does not start with header." << QwLog::endl;

    // Add independent variables and sensitivities
    while (section->ReadNextLine()) {
      // Throw away comments, whitespace, empty lines
      section->TrimComment();
      section->TrimWhitespace();
      if (section->LineIsEmpty()) continue;
      // Get first token: independent variable
      std::string current_token = section->GetNextToken(",");
      // Parse current token into independent variable type and name
      type_name = ParseRegressionVariable(current_token);
      // Loop over dependent variables to set sensitivities
      for (size_t dv = current_dv_start; dv < fDependentName.size(); dv++) {
        Double_t sensitivity = atof(section->GetNextToken(",").c_str());
        fSensitivity.at(dv).push_back(sensitivity);
        fIndependentType.at(dv).push_back(type_name.first);
        fIndependentName.at(dv).push_back(type_name.second);
      }
    }
  }
  return 0;
}


/** Connect to the channels
 *
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwRegression::ConnectChannels(
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fCorrelationName.size(); dv++) {

    fCorrelation.push_back(new QwVQWK_Channel(fCorrelationName[dv], VQwDataElement::kDerived));

    // Add independent variables
    fIndependentVar.resize(fCorrelation.size());

    // Get the independent variables
    // first channel
    const VQwHardwareChannel* iv1_ptr = 0;
    switch (fIndependentType.at(dv).first) {
    case kRegTypeAsym:
      iv1_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).first);
      break;
    case kRegTypeDiff:
      iv1_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).first);
      break;
    default:
      QwWarning << "Independent variable for regression has unknown type."
      << QwLog::endl;
      break;
    }
    // second channel
    const VQwHardwareChannel* iv2_ptr = 0;
    switch (fIndependentType.at(dv).second) {
    case kRegTypeAsym:
      iv2_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).second);
      break;
    case kRegTypeDiff:
      iv2_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).second);
      break;
    default:
      QwWarning << "Independent variable for regression has unknown type."
      << QwLog::endl;
      break;
    }
      // combine
      if (iv1_ptr && iv2_ptr) {
        QwMessage << " iv: " << fIndependentName.at(dv).first << " vs. " << fIndependentName.at(dv).second << QwLog::endl;
        fIndependentVar.back().push_back(std::make_pair(iv1_ptr,iv2_ptr));
      } else {
        QwWarning << "Independent variable " << fIndependentName.at(dv).first << " or "
            << fIndependentName.at(dv).second << " for regression of "
            << "dependent variable " << fDependentName.at(dv) << " could not be found."
            << QwLog::endl;
      }
    }
  }
  return 0;
}
/*
 * QwCorrelation.cc
 *
 *  Created on: Oct 3, 2012
 *      Author: wdconinc
 */

#include "QwCorrelation.h"


/** Constructor with only helicity pattern
 *
 * @param options Obtions object
 * @param helicitypattern Helicity Pattern
 *
 */
QwCorrelation::QwCorrelation(QwOptions &options, QwHelicityPattern& helicitypattern)
{
  ProcessOptions(options);
  LoadChannelMap(fCorrelationMapFile);
  fHelicityPattern = &helicitypattern;
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(asym,diff);
}



/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCorrelation::DefineOptions(QwOptions &options)
{
  options.AddOptions("Linear regression")
    ("correlation-map", po::value<std::string>()->default_value("correlation.map"),
     "variables for correlation analysis");
}

/**
 * Process configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCorrelation::ProcessOptions(QwOptions &options)
{
  fCorrelationMapFile = options.GetValue<std::string>("correlation-map");
}


/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwRegression::LoadChannelMap(const std::string& mapfile)
{
  // Open the file
  QwParameterFile map(mapfile);

  // Read the sections of dependent variables
  bool keep_header = true;
  std::string section_name;
  QwParameterFile* section = 0;
  std::pair<EQwRegType,std::string> type_name;
  while ((section = map.ReadNextSection(section_name,keep_header))) {

    // Store index to the current position in the dv vector
    size_t current_dv_start = fDependentName.size();

    // Add dependent variables from the section header
    section->ReadNextLine();
    if (section->LineHasSectionHeader()) {
      section->TrimSectionHeader();
      section->TrimWhitespace();
      // Parse section header into tokens separated by a comma
      std::string current_token;
      std::string previous_token;
      do {
        previous_token = current_token;
        current_token = section->GetNextToken(",");
        if (current_token.size() == 0) continue;
        // Parse current token into dependent variable type and name
        type_name = ParseRegressionVariable(current_token);
        fDependentType.push_back(type_name.first);
        fDependentName.push_back(type_name.second);
        // Resize the vectors of sensitivities and independent variables
        fSensitivity.resize(fDependentName.size());
        fIndependentType.resize(fDependentName.size());
        fIndependentName.resize(fDependentName.size());
      } while (current_token.size() != 0);
    } else QwError << "Section does not start with header." << QwLog::endl;

    // Add independent variables and sensitivities
    while (section->ReadNextLine()) {
      // Throw away comments, whitespace, empty lines
      section->TrimComment();
      section->TrimWhitespace();
      if (section->LineIsEmpty()) continue;
      // Get first token: independent variable
      std::string current_token = section->GetNextToken(",");
      // Parse current token into independent variable type and name
      type_name = ParseRegressionVariable(current_token);
      // Loop over dependent variables to set sensitivities
      for (size_t dv = current_dv_start; dv < fDependentName.size(); dv++) {
        Double_t sensitivity = atof(section->GetNextToken(",").c_str());
        fSensitivity.at(dv).push_back(sensitivity);
        fIndependentType.at(dv).push_back(type_name.first);
        fIndependentName.at(dv).push_back(type_name.second);
      }
    }
  }
  return 0;
}


/** Connect to the channels
 *
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwRegression::ConnectChannels(
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fCorrelationName.size(); dv++) {

    fCorrelation.push_back(new QwVQWK_Channel(fCorrelationName[dv], VQwDataElement::kDerived));

    // Add independent variables
    fIndependentVar.resize(fCorrelation.size());

    // Get the independent variables
    // first channel
    const VQwHardwareChannel* iv1_ptr = 0;
    switch (fIndependentType.at(dv).first) {
    case kRegTypeAsym:
      iv1_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).first);
      break;
    case kRegTypeDiff:
      iv1_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).first);
      break;
    default:
      QwWarning << "Independent variable for regression has unknown type."
      << QwLog::endl;
      break;
    }
    // second channel
    const VQwHardwareChannel* iv2_ptr = 0;
    switch (fIndependentType.at(dv).second) {
    case kRegTypeAsym:
      iv2_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).second);
      break;
    case kRegTypeDiff:
      iv2_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).second);
      break;
    default:
      QwWarning << "Independent variable for regression has unknown type."
      << QwLog::endl;
      break;
    }
      // combine
      if (iv1_ptr && iv2_ptr) {
        QwMessage << " iv: " << fIndependentName.at(dv).first << " vs. " << fIndependentName.at(dv).second << QwLog::endl;
        fIndependentVar.back().push_back(std::make_pair(iv1_ptr,iv2_ptr));
      } else {
        QwWarning << "Independent variable " << fIndependentName.at(dv).first << " or "
            << fIndependentName.at(dv).second << " for regression of "
            << "dependent variable " << fDependentName.at(dv) << " could not be found."
            << QwLog::endl;
      }
    }
  }
  return 0;
}
/*
 * QwCorrelation.cc
 *
 *  Created on: Oct 3, 2012
 *      Author: wdconinc
 */

#include "QwCorrelation.h"


/** Constructor with only helicity pattern
 *
 * @param options Obtions object
 * @param helicitypattern Helicity Pattern
 *
 */
QwCorrelation::QwCorrelation(QwOptions &options, QwHelicityPattern& helicitypattern)
{
  ProcessOptions(options);
  LoadChannelMap(fCorrelationMapFile);
  fHelicityPattern = &helicitypattern;
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(asym,diff);
}



/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCorrelation::DefineOptions(QwOptions &options)
{
  options.AddOptions("Linear regression")
    ("correlation-map", po::value<std::string>()->default_value("correlation.map"),
     "variables for correlation analysis");
}

/**
 * Process configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwCorrelation::ProcessOptions(QwOptions &options)
{
  fCorrelationMapFile = options.GetValue<std::string>("correlation-map");
}


/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwRegression::LoadChannelMap(const std::string& mapfile)
{
  // Open the file
  QwParameterFile map(mapfile);

  // Read the sections of dependent variables
  bool keep_header = true;
  std::string section_name;
  QwParameterFile* section = 0;
  std::pair<EQwRegType,std::string> type_name;
  while ((section = map.ReadNextSection(section_name,keep_header))) {

    // Store index to the current position in the dv vector
    size_t current_dv_start = fDependentName.size();

    // Add dependent variables from the section header
    section->ReadNextLine();
    if (section->LineHasSectionHeader()) {
      section->TrimSectionHeader();
      section->TrimWhitespace();
      // Parse section header into tokens separated by a comma
      std::string current_token;
      std::string previous_token;
      do {
        previous_token = current_token;
        current_token = section->GetNextToken(",");
        if (current_token.size() == 0) continue;
        // Parse current token into dependent variable type and name
        type_name = ParseRegressionVariable(current_token);
        fDependentType.push_back(type_name.first);
        fDependentName.push_back(type_name.second);
        // Resize the vectors of sensitivities and independent variables
        fSensitivity.resize(fDependentName.size());
        fIndependentType.resize(fDependentName.size());
        fIndependentName.resize(fDependentName.size());
      } while (current_token.size() != 0);
    } else QwError << "Section does not start with header." << QwLog::endl;

    // Add independent variables and sensitivities
    while (section->ReadNextLine()) {
      // Throw away comments, whitespace, empty lines
      section->TrimComment();
      section->TrimWhitespace();
      if (section->LineIsEmpty()) continue;
      // Get first token: independent variable
      std::string current_token = section->GetNextToken(",");
      // Parse current token into independent variable type and name
      type_name = ParseRegressionVariable(current_token);
      // Loop over dependent variables to set sensitivities
      for (size_t dv = current_dv_start; dv < fDependentName.size(); dv++) {
        Double_t sensitivity = atof(section->GetNextToken(",").c_str());
        fSensitivity.at(dv).push_back(sensitivity);
        fIndependentType.at(dv).push_back(type_name.first);
        fIndependentName.at(dv).push_back(type_name.second);
      }
    }
  }
  return 0;
}


/** Connect to the channels
 *
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwRegression::ConnectChannels(
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fCorrelationName.size(); dv++) {

    fCorrelation.push_back(new QwVQWK_Channel(fCorrelationName[dv], VQwDataElement::kDerived));

    // Add independent variables
    fIndependentVar.resize(fCorrelation.size());

    // Get the independent variables
    // first channel
    const VQwHardwareChannel* iv1_ptr = 0;
    switch (fIndependentType.at(dv).first) {
    case kRegTypeAsym:
      iv1_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).first);
      break;
    case kRegTypeDiff:
      iv1_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).first);
      break;
    default:
      QwWarning << "Independent variable for regression has unknown type."
      << QwLog::endl;
      break;
    }
    // second channel
    const VQwHardwareChannel* iv2_ptr = 0;
    switch (fIndependentType.at(dv).second) {
    case kRegTypeAsym:
      iv2_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).second);
      break;
    case kRegTypeDiff:
      iv2_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).second);
      break;
    default:
      QwWarning << "Independent variable for regression has unknown type."
      << QwLog::endl;
      break;
    }
      // combine
      if (iv1_ptr && iv2_ptr) {
        QwMessage << " iv: " << fIndependentName.at(dv).first << " vs. " << fIndependentName.at(dv).second << QwLog::endl;
        fIndependentVar.back().push_back(std::make_pair(iv1_ptr,iv2_ptr));
      } else {
        QwWarning << "Independent variable " << fIndependentName.at(dv).first << " or "
            << fIndependentName.at(dv).second << " for regression of "
            << "dependent variable " << fDependentName.at(dv) << " could not be found."
            << QwLog::endl;
      }
    }
  }
  return 0;
}
