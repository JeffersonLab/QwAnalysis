/**********************************************************\
* File: VQwSubsystemTracking.cc                           *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "VQwSubsystemTracking.h"

// Qweak headers
#include "QwParameterFile.h"

Int_t VQwSubsystemTracking::LoadGeometryDefinition(TString filename)
{
  // Open parameter file
  QwParameterFile* map = new QwParameterFile(filename.Data());

  // Add detector map name to file
  fDetectorMaps.insert(map->GetParamFileNameContents());

  // Clear current set of detectors before reading geometry file
  fDetectorInfo.clear();

  // Read preamble
  QwParameterFile* preamble = map->ReadSectionPreamble();
  preamble->TrimComment("!#");  // Remove everything after a '!' or '#' character
  preamble->TrimWhitespace();   // Get rid of leading and trailing spaces

  // Read detector sections
  QwParameterFile* section;
  std::string section_name;
  while ((section = map->ReadNextSection(section_name))) {

    // Create new detector info object
    QwDetectorInfo* detector = new QwDetectorInfo(section_name);

    // Load preamble parameters
    detector->LoadGeometryDefinition(preamble);

    // Load section parameters
    detector->LoadGeometryDefinition(section);

    // Construct "R2-octant" to request the requested octant
    std::stringstream octant_option;
    octant_option << "R" << detector->GetRegion() << "-octant";

    // Obtain the current octant number from the command line options
    detector->SetOctant(gQwOptions.GetValue<Int_t>(octant_option.str()));
    // The octant specified on the command line is for package 2...
    if (detector->GetPackage() == kPackage1)
      detector->SetOctant((detector->GetOctant() + 3) % 8 + 1);

    // Construct "R2-octant = 2" to construct the correct module header
    std::stringstream octant_match;
    octant_match << octant_option << " = " << detector->GetOctant();

    // Find modules matching the current octant
    QwParameterFile* module;
    std::string module_name;
    while ((module = section->ReadNextModule(module_name))) {
      // Find matching module
      if (module_name == octant_match.str()) {
        // Load section parameters
        detector->LoadGeometryDefinition(module);
      }

      // Delete object created by ReadNextModule
      delete module;
    }

    // Add detector to the list
    QwMessage << *detector << QwLog::endl;
    fDetectorInfo.push_back(detector);

    // Delete object created by ReadNextSection
    delete section;
  }
  
  // Delete QwParameterFile object
  delete map;

  return 0; // \todo why?
}

