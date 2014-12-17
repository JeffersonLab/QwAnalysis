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
    EQwRegionID region = detector->GetRegion();
    if (region == kRegionIDTrig) region = kRegionID3;
    octant_option << "R" << region << "-octant";

    // Obtain the current octant number from the command line options
    if (gQwOptions.HasValue(octant_option.str())) {
      // The octant specified on the command line is for package 2...
      detector->SetOctant(gQwOptions.GetValue<Int_t>(octant_option.str()));
      // So if we want package 1 we need to change the octant number
      if (detector->GetPackage() == kPackage1)
        detector->SetOctant((detector->GetOctant() + 3) % 8 + 1);
    } else // no octant explicitly specified
      detector->SetOctant(0);

    // Construct "R-octant = " tag to construct the correct module header
    std::stringstream octant_match;
    octant_match << octant_option.str() << " = " << detector->GetOctant();

    // Find modules matching the current octant
    QwParameterFile* module;
    std::string module_name;
    section->RewindToFileStart();
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


