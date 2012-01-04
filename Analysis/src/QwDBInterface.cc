/*
 * QwDBInterface.cc
 *
 *  Created on: Dec 14, 2010
 *      Author: wdconinc
 *      Author: jhlee
 */

#include "QwDBInterface.h"

// Qweak headers
//#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"

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


void QwDBInterface::SetMonitorID(QwParityDB *db)
{
  fDeviceId = db->GetMonitorID(fDeviceName.Data());
}

void QwDBInterface::SetMainDetectorID(QwParityDB *db)
{
  fDeviceId = db->GetMainDetectorID(fDeviceName.Data());
}

void QwDBInterface::SetLumiDetectorID(QwParityDB *db)
{
  fDeviceId = db->GetLumiDetectorID(fDeviceName.Data());
}

template <class T>
T QwDBInterface::TypedDBClone()
{
  T row(0);
  return row;
}



/// Specifications of the templated function
/// template <class T> inline T QwDBInterface::TypedDBClone();
template<> QwParitySSQLS::md_data
QwDBInterface::TypedDBClone<QwParitySSQLS::md_data>() {
  QwParitySSQLS::md_data row(0);
  row.analysis_id         = fAnalysisId;
  row.main_detector_id    = fDeviceId;
  row.measurement_type_id = fMeasurementTypeId;
  row.subblock            = fSubblock;
  row.n                   = fN;
  row.value               = fValue;
  row.error               = fError;
  return row;
}
template<> QwParitySSQLS::lumi_data
QwDBInterface::TypedDBClone<QwParitySSQLS::lumi_data>() {
  QwParitySSQLS::lumi_data row(0);
  row.analysis_id         = fAnalysisId;
  row.lumi_detector_id    = fDeviceId;
  row.measurement_type_id = fMeasurementTypeId;
  row.subblock            = fSubblock;
  row.n                   = fN;
  row.value               = fValue;
  row.error               = fError;
  return row;
}
template<> QwParitySSQLS::beam
QwDBInterface::TypedDBClone<QwParitySSQLS::beam>() {
  QwParitySSQLS::beam row(0);
  row.analysis_id         = fAnalysisId;
  row.monitor_id          = fDeviceId;
  row.measurement_type_id = fMeasurementTypeId;
  row.subblock            = fSubblock;
  row.n                   = fN;
  row.value               = fValue;
  row.error               = fError;
  return row;
}





// QwErrDBInterface 


void QwErrDBInterface::SetMonitorID(QwParityDB *db)
{
  fDeviceId = db->GetMonitorID(fDeviceName.Data());
}

void QwErrDBInterface::SetMainDetectorID(QwParityDB *db)
{
  fDeviceId = db->GetMainDetectorID(fDeviceName.Data());
}

void QwErrDBInterface::SetLumiDetectorID(QwParityDB *db)
{
  fDeviceId = db->GetLumiDetectorID(fDeviceName.Data());
}

template <class T>
T QwErrDBInterface::TypedDBClone()
{
  T row(0);
  return row;
}

template<> QwParitySSQLS::md_errors
QwErrDBInterface::TypedDBClone<QwParitySSQLS::md_errors>() {
  QwParitySSQLS::md_errors row(0);
  row.analysis_id         = fAnalysisId;
  row.main_detector_id    = fDeviceId;
  row.error_code_id       = fErrorCodeId;
  row.n                   = fN;
  return row;
};


template<> QwParitySSQLS::lumi_errors
QwErrDBInterface::TypedDBClone<QwParitySSQLS::lumi_errors>() {
  QwParitySSQLS::lumi_errors row(0);
  row.analysis_id         = fAnalysisId;
  row.lumi_detector_id    = fDeviceId;
  row.error_code_id       = fErrorCodeId;
  row.n                   = fN;
  return row;
};




template<> QwParitySSQLS::beam_errors
QwErrDBInterface::TypedDBClone<QwParitySSQLS::beam_errors>() {
  QwParitySSQLS::beam_errors row(0);
  row.analysis_id         = fAnalysisId;
  row.monitor_id          = fDeviceId;
  row.error_code_id       = fErrorCodeId;
  row.n                   = fN;
  return row;
};




