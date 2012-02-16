/*
 * QwRegression.cc
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 */

#include "QwRegression.h"

// Qweak headers
#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"

/** Constructor with single event and helicity pattern
 *
 * @param options Options object
 * @param event Single event
 * @param helicitypattern Helicity pattern
 */
QwRegression::QwRegression(
    QwOptions &options,
    QwSubsystemArrayParity& event,
    QwHelicityPattern& helicitypattern)
{
  fEnableRegression = false;
  ProcessOptions(options);
  fSubsystemArray = &event;
  fHelicityPattern = &helicitypattern;
  LoadChannelMap(fRegressionMapFile);
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(event,asym,diff);
}

/** Constructor with single event
 * 
 * @param options Options object
 * @param event Single event
 * 
 */
QwRegression::QwRegression(QwOptions &options, QwSubsystemArrayParity& event)
{
    fEnableRegression = false;
    ProcessOptions(options);
    fSubsystemArray = &event;
    LoadChannelMap(fRegressionMapFile);
    ConnectChannels(event);
      
}

/** Constructor with only helicity pattern
 * 
 * @param options Obtions object
 * @param helicitypattern Helicity Pattern
 * 
 */
QwRegression::QwRegression(QwOptions &options, QwHelicityPattern& helicitypattern)
{
  fEnableRegression = false;
  ProcessOptions(options);
  LoadChannelMap(fRegressionMapFile);
  fHelicityPattern = &helicitypattern;
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;
  ConnectChannels(asym,diff);
}

/** Constructor with options
 * 
 * @param options Obtions object
 * 
 */
QwRegression::QwRegression(QwOptions &options)
{
  fEnableRegression = false;
  ProcessOptions(options);
  LoadChannelMap(fRegressionMapFile);
} 


QwRegression::QwRegression(const QwRegression &source)
{
    this->fDependentVar.resize(source.fDependentVar.size());
    fDependentVarType.resize(source.fDependentVar.size());
    for (size_t i = 0; i < this->fDependentVar.size(); i++) {
      const QwVQWK_Channel* vqwk = dynamic_cast<const QwVQWK_Channel*>(source.fDependentVar[i].second);
      this->fDependentVar[i].first = NULL;
      this->fDependentVar[i].second = new QwVQWK_Channel(*vqwk);
      fDependentVarType[i] = source.fDependentVarType[i];
    }
}

/// Destructor
QwRegression::~QwRegression()
{
  PairIterator element;
  for (element = fDependentVar.begin();
      element != fDependentVar.end(); element++) {
      if (element->second != NULL){
         delete element->second;
      }
  }
  fDependentVar.clear();
}


/** Parse the variable type and name from a section header
 *
 * @param variable String with variable type and name
 * @return Pair of variable type and name
 */
std::pair<QwRegression::EQwRegType,std::string> QwRegression::ParseRegressionVariable(
    const std::string& variable)
{
  std::pair<EQwRegType,std::string> type_name;
  size_t len = variable.length();
  size_t pos1 = variable.find_first_of(":");
  size_t pos2 = variable.find_first_not_of(":",pos1);
  if (pos1 == std::string::npos) {
    type_name.first  = kRegTypeUnknown;
    type_name.second = variable;
  } else {
    std::string type = variable.substr(0,pos1);
    std::string name = variable.substr(pos2,len-pos2);
    if (type == "asym")
      type_name.first = kRegTypeAsym;
    else if (type == "diff")
      type_name.first = kRegTypeDiff;
    else if (type == "mps")
      type_name.first = kRegTypeMps;
    else
      type_name.first = kRegTypeUnknown;
    type_name.second = name;
  }
  return type_name;
}


/** Load the channel map
 *
 * @param mapfile Filename of map file
 * @return Zero when success
 */
Int_t QwRegression::LoadChannelMap(const std::string& mapfile)
{
  // Return if regression is not enabled
  if (! fEnableRegression) return 0;

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

/** Connect to the dependent and independent channels
 *
 * @param event Helicity event structure
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwRegression::ConnectChannels(
    QwSubsystemArrayParity& event,
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  // Return if regression is not enabled
  if (! fEnableRegression) return 0;

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string reg = "reg_";
    
    if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
    }else{
        switch (fDependentType.at(dv)) {
          case kRegTypeMps:
            dv_ptr = event.ReturnInternalValueForFriends(fDependentName.at(dv));
            break;
          case kRegTypeAsym:
            dv_ptr = asym.ReturnInternalValueForFriends(fDependentName.at(dv));
            break;
          case kRegTypeDiff:
            dv_ptr = diff.ReturnInternalValueForFriends(fDependentName.at(dv));
            break;
          default:
            QwWarning << "Dependent variable for regression has unknown type."
                      << QwLog::endl;
            break;
        }

        vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
        name = vqwk->GetElementName().Data();
        name.insert(0, reg);
        new_vqwk = new QwVQWK_Channel(*vqwk);
        new_vqwk->SetElementName(name);
    }

    // alias
    if(fDependentName.at(dv).at(0) == '@'){
        QwMessage << "dv: " << name << QwLog::endl;
        name.insert(0,reg);
        new_vqwk = new QwVQWK_Channel(name);
    }
    // defined type
    else if(dv_ptr!=NULL){
        QwMessage << "dv: " << fDependentName.at(dv) << QwLog::endl;
    }else {
        QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
                continue; 
    }

    // pair creation
    if(new_vqwk != NULL){
      fDependentVarType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }

      // Add independent variables
      fIndependentVar.resize(fDependentVar.size());
      for (size_t iv = 0; iv < fIndependentName.at(dv).size(); iv++) {
        // Get the independent variables
        const VQwHardwareChannel* iv_ptr = 0;
        switch (fIndependentType.at(dv).at(iv)) {
          case kRegTypeMps:
            iv_ptr = event.ReturnInternalValue(fIndependentName.at(dv).at(iv));
            break;
          case kRegTypeAsym:
            iv_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).at(iv));
            break;
          case kRegTypeDiff:
            iv_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).at(iv));
            break;
          default:
            QwWarning << "Independent variable for regression has unknown type."
                      << QwLog::endl;
            break;
        }
        if (iv_ptr) {
          QwMessage << " iv: " << fIndependentName.at(dv).at(iv) << " (sens = "
                    << fSensitivity.at(dv).at(iv) << ")" << QwLog::endl;
          fIndependentVar.back().push_back(std::make_pair(fSensitivity.at(dv).at(iv),
							  iv_ptr));
        } else {
          QwWarning << "Independent variable " << fIndependentName.at(dv).at(iv) << " for regression of "
                    << "dependent variable " << fDependentName.at(dv) << " could not be found."
                    << QwLog::endl;
        }
      }
  }
  
  return 0;

}
/** Connect to the dependent and independent channels
 *
 * @param asym Asymmetry event structure
 * @param diff Difference event structure
 * @return Zero on success
 */
Int_t QwRegression::ConnectChannels(
    QwSubsystemArrayParity& asym,
    QwSubsystemArrayParity& diff)
{
  // Return if regression is not enabled
  if (! fEnableRegression) return 0;

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = "";
    string reg = "reg_";
    
    if (fDependentType.at(dv)==kRegTypeMps){
      //  Quietly ignore the MPS type when we're connecting the asym & diff
      continue;
    } else if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
    }else{
        switch (fDependentType.at(dv)) {
          case kRegTypeAsym:
            dv_ptr = asym.ReturnInternalValueForFriends(fDependentName.at(dv));
            break;
          case kRegTypeDiff:
            dv_ptr = diff.ReturnInternalValueForFriends(fDependentName.at(dv));
            break;
          default:
            QwWarning << "QwRegression::ConnectChannels(QwSubsystemArrayParity& asym, QwSubsystemArrayParity& diff):  Dependent variable, "
		      << fDependentName.at(dv)
		      << ", for asym/diff regression does not have proper type, type=="
		      << fDependentType.at(dv) << "."<< QwLog::endl;
            break;
        }

        vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
        name = vqwk->GetElementName().Data();
        name.insert(0, reg);
        new_vqwk = new QwVQWK_Channel(*vqwk);
        new_vqwk->SetElementName(name);
    }

    // alias
    if(fDependentName.at(dv).at(0) == '@'){
        QwMessage << "dv: " << name << QwLog::endl;
        name.insert(0,reg);
        new_vqwk = new QwVQWK_Channel(name);
    }
    // defined type
    else if(dv_ptr!=NULL){
        QwMessage << "dv: " << fDependentName.at(dv) << QwLog::endl;
    }else {
        QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
                continue; 
    }

    // pair creation
    if(new_vqwk != NULL){
      fDependentVarType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }

      // Add independent variables
      fIndependentVar.resize(fDependentVar.size());
      for (size_t iv = 0; iv < fIndependentName.at(dv).size(); iv++) {
        // Get the independent variables
        const VQwHardwareChannel* iv_ptr = 0;
        switch (fIndependentType.at(dv).at(iv)) {
          case kRegTypeAsym:
            iv_ptr = asym.ReturnInternalValue(fIndependentName.at(dv).at(iv));
            break;
          case kRegTypeDiff:
            iv_ptr = diff.ReturnInternalValue(fIndependentName.at(dv).at(iv));
            break;
          default:
            QwWarning << "Independent variable for regression has unknown type."
                      << QwLog::endl;
            break;
        }
        if (iv_ptr) {
          QwMessage << " iv: " << fIndependentName.at(dv).at(iv) << " (sens = "
                    << fSensitivity.at(dv).at(iv) << ")" << QwLog::endl;
          fIndependentVar.back().push_back(std::make_pair(fSensitivity.at(dv).at(iv),
							  iv_ptr));
        } else {
          QwWarning << "Independent variable " << fIndependentName.at(dv).at(iv) << " for regression of "
                    << "dependent variable " << fDependentName.at(dv) << " could not be found."
                    << QwLog::endl;
        }
      }
  }
  
  return 0;

}

/** Connect to the dependent and independent channels
 *
 * @param event Helicity event structure
 * @return Zero on success
 */
Int_t QwRegression::ConnectChannels(QwSubsystemArrayParity& event)
{
  // Return if regression is not enabled
  if (! fEnableRegression) return 0;

  /// Fill vector of pointers to the relevant data elements
  for (size_t dv = 0; dv < fDependentName.size(); dv++) {
    // Get the dependent variables

    VQwHardwareChannel* dv_ptr = 0;
    QwVQWK_Channel* new_vqwk = NULL;
    QwVQWK_Channel* vqwk = NULL;
    string name = " s";
    string reg = "reg_";

    if (fDependentType.at(dv)==kRegTypeAsym || fDependentType.at(dv)==kRegTypeDiff){
      //  Quietly skip the asymmetry or difference types.
      continue;
    } else if(fDependentType.at(dv) != kRegTypeMps){
      QwWarning << "QwRegression::ConnectChannels(QwSubsystemArrayParity& event):  Dependent variable, "
	      << fDependentName.at(dv)
	      << ", for MPS regression does not have MPS type, type=="
	      << fDependentType.at(dv) << "."<< QwLog::endl;
      continue;
    } else {
      if(fDependentName.at(dv).at(0) == '@' ){
        name = fDependentName.at(dv).substr(1,fDependentName.at(dv).length());
        new_vqwk = new QwVQWK_Channel(name);
      } else {
        dv_ptr = event.ReturnInternalValueForFriends(fDependentName.at(dv));

	vqwk = dynamic_cast<QwVQWK_Channel*>(dv_ptr);
        name = vqwk->GetElementName().Data();
        name.insert(0,reg);
        new_vqwk = new QwVQWK_Channel(*vqwk);
        new_vqwk->SetElementName(name);
      }
    }

    // alias
    if(new_vqwk==NULL){
      QwWarning << "Dependent variable " << fDependentName.at(dv) << " could not be found, "
                << "or is not a VQWK channel." << QwLog::endl;
      continue; 
    } else {
      QwMessage << "dv: " << new_vqwk->GetElementName() << QwLog::endl;
      // pair creation
      fDependentVarType.push_back(fDependentType.at(dv));
      fDependentVar.push_back(std::make_pair(vqwk, new_vqwk));
    }

      // Add independent variables
      fIndependentVar.resize(fDependentVar.size());
      for (size_t iv = 0; iv < fIndependentName.at(dv).size(); iv++) {
        // Get the independent variables
        const VQwHardwareChannel* iv_ptr = 0;
        if(fIndependentType.at(dv).at(iv) == kRegTypeMps){
            iv_ptr = event.ReturnInternalValue(fIndependentName.at(dv).at(iv));
	}else{
            QwWarning << "Independent variable for MPS regression has unknown type."
                      << QwLog::endl;
        }
        if (iv_ptr) {
          QwMessage << " iv: " << fIndependentName.at(dv).at(iv) << " (sens = "
                    << fSensitivity.at(dv).at(iv) << ")" << QwLog::endl;
          fIndependentVar.back().push_back(std::make_pair(fSensitivity.at(dv).at(iv),
							  iv_ptr));
        } else {
          QwWarning << "Independent variable " << fIndependentName.at(dv).at(iv) << " for regression of "
                    << "dependent variable " << fDependentName.at(dv) << " could not be found."
                    << QwLog::endl;
        }
      }
  }
  
  return 0;
}

/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwRegression::DefineOptions(QwOptions &options)
{
  options.AddOptions("Linear regression")
    ("enable-regression", po::value<bool>()->zero_tokens()->default_value(false),
     "enable linear regression");
  options.AddOptions("Linear regression")
    ("regression-map", po::value<std::string>()->default_value("regression.map"),
     "variables and sensitivities for regression");
}

/**
 * Process configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwRegression::ProcessOptions(QwOptions &options)
{
  fEnableRegression = options.GetValue<bool>("enable-regression");
  fRegressionMapFile = options.GetValue<std::string>("regression-map");
}


/// Do the linear regression
void QwRegression::LinearRegression(EQwRegType type)
{


  // Return if regression is not enabled
  if (! fEnableRegression){
    QwError << "Regression is not enabled!" << QwLog::endl;
    return;
  }
  // Linear regression for each dependent variable
  for (size_t dv = 0; dv < fDependentVar.size(); dv++) {
    // if second is NULL, can't do regression
    if (fDependentVar.at(dv).second == NULL){
        QwError<<"Second is value is NULL, unable to calculate regression."<<QwLog::endl;
        continue;
    }
    // For correct type (asym, diff, mps)
    if (fDependentVarType.at(dv) != type) continue;

    // Clear data in second, if first is NULL
    if (fDependentVar.at(dv).first == NULL){
        fDependentVar.at(dv).second->ClearEventData();
    }else{
        // Update second value
        fDependentVar.at(dv).second->AssignValueFrom(fDependentVar.at(dv).first);
    }
    // Add corrections
    for (size_t iv = 0; iv < fIndependentVar.at(dv).size(); iv++) {
      fDependentVar.at(dv).second->ScaledAdd(fIndependentVar.at(dv).at(iv).first,
					     fIndependentVar.at(dv).at(iv).second);
    }
  }
}


/**
 * Construct the branch and tree vector
 * @param tree Tree
 * @param prefix Prefix
 * @param values Vector of values
 */
void  QwRegression::ConstructBranchAndVector(
    TTree *tree,
    TString& prefix,
    std::vector<Double_t>& values)
{
  PairConstIterator element;
  for (element = fDependentVar.begin();
       element != fDependentVar.end(); ++element) {
    element->second->ConstructBranchAndVector(tree, prefix, values);
  }
};

/**
 * Fill the tree vector
 * @param values Vector of values
 */
void QwRegression::FillTreeVector(std::vector<Double_t>& values) const
{
  // Fill the data element
  PairConstIterator element;
  for (element = fDependentVar.begin();
       element != fDependentVar.end(); ++element) {
    if (element->second == NULL) continue;
    element->second->FillTreeVector(values);
  }
};

    
void QwRegression::AccumulateRunningSum(QwRegression value)
{
  for (size_t i = 0; i < value.fDependentVar.size(); i++){
    this->fDependentVar[i].second->AccumulateRunningSum(value.fDependentVar[i].second);
  }
}


void QwRegression::CalculateRunningAverage()
{
  for(size_t i = 0; i < fDependentVar.size(); i++) {
    fDependentVar[i].second->CalculateRunningAverage();
  }
  
  return;
}


void QwRegression::PrintValue() const
{
  QwMessage<<"=== QwRegression ==="<<QwLog::endl<<QwLog::endl;
  for(size_t i = 0; i < fDependentVar.size(); i++) {
    fDependentVar[i].second->PrintValue();
  }
}

