/*
 * QwDBInterface.cc
 *
 *  Created on: Dec 14, 2010
 *      Author: wdconinc
 */

#include "QwDBInterface.h"

// Qweak headers
#include "QwDatabase.h"

std::map<TString, TString> QwDBInterface::fPrefix;

TString QwDBInterface::DetermineMeasurementTypeID(TString type, TString suffix,
						  Bool_t forcediffs)
{
  if (fPrefix.empty()){
    fPrefix["yield"]      = "y";
    fPrefix["difference"] = "d";
    fPrefix["asymmetry"]  = "a";
    fPrefix["asymmetry1"] = "a12";
    fPrefix["asymmetry2"] = "aeo";
  }
  TString measurement_type("");
  if (fPrefix.count(type)==1){
    measurement_type = fPrefix[type];
    if (measurement_type[0] == 'a' && 
	(forcediffs
	 || (suffix == "p" || suffix == "a" 
	     || suffix == "m"))	){
      //  Change the type to difference for position,
      //  angle, or slope asymmetry variables.
      measurement_type[0] = 'd';
    } else if (measurement_type[0] == 'y') {
      measurement_type += suffix;
    }
  }
  QwDebug << "\n"
	  << type << ", " << suffix
	  << " \'" <<  measurement_type.Data() << "\'" << QwLog::endl;
  return measurement_type;
}


void QwDBInterface::SetMonitorID(QwDatabase *db)
{
  fDeviceId = db->GetMonitorID(fDeviceName.Data());
}

void QwDBInterface::SetMainDetectorID(QwDatabase *db)
{
  fDeviceId = db->GetMainDetectorID(fDeviceName.Data());
}

void QwDBInterface::SetLumiDetectorID(QwDatabase *db)
{
  fDeviceId = db->GetLumiDetectorID(fDeviceName.Data());
}
