#include "QwTypes.h"
#include "TString.h"

/**
 * Determine the EQwPMTInstrumentType value corresponding to a
 * text name of the type, as used in the channel map files.
 * The text comparison is not case sensitive.
 * @param  name Text name of an instrument type, such as "IntegrationPMT"
 * @return      EQwPMTInstrumentType value corresponding to the name
 */
EQwPMTInstrumentType GetQwPMTInstrumentType(TString name){
  //  The name is passed by value because we will
  //  change it to lower case.
  name.ToLower();
  EQwPMTInstrumentType result;
  result = kQwUnknownPMT;
  if (name=="integrationpmt"){
    result = kQwIntegrationPMT;
  } else if (name=="scalerpmt"){
    result = kQwScalerPMT;
  }
  else if (name=="combinationpmt"){
    result = kQwCombinedPMT;
  }
  return result;
};


EQwBeamInstrumentType GetQwBeamInstrumentType(TString name){
  //  The name is passed by value because we will
  //  change it to lower case.
  name.ToLower();
  EQwBeamInstrumentType result;
  result = kQwUnknownDeviceType;
  if (name=="bpmstripline"){
    result = kQwBPMStripline;
  } else if (name=="bcm"){
    result = kQwBCM;
  }
  else if (name=="combinedbcm"){
    result = kQwCombinedBCM;
  }
  else if (name=="combinedbpm"){
    result = kQwCombinedBPM;
  }
  else if (name=="energycalculator"){
    result = kQwEnergyCalculator;
  }
  else if (name=="halomonitor"){
    result = kQwHaloMonitor;
  }
  return result;
};


/**
 * Get the text name of a EQwPMTInstrumentType, as it would be
 * used in the channel map files.
 * @param  type   EQwPMTInstrumentType value for which the name should be 
 *                returned, such as kQwPMTIntegrationPMT
 * @return        Text name corresponding to the type
 */
TString GetQwPMTInstrumentTypeName(EQwPMTInstrumentType type){
  TString result;
  result = "UnknownPMT";
  if (type==kQwIntegrationPMT){
    result = "IntegrationPMT";
  } 
  else if (type==kQwScalerPMT){
    result = "ScalerPMT";
  } 
  else if (type==kQwCombinedPMT){
    result = "CombinationPMT";
  }
  return result;
};


TString GetQwBeamInstrumentTypeName(EQwBeamInstrumentType type){
  TString result;
  result = "kQwUnknownDeviceType";
  if (type==kQwBPMStripline){
    result = "bpmtripline";
  } 
  else if (type==kQwBCM){
    result = "bcm";
  }
  else if (type==kQwCombinedBCM){
    result = "combinedbcm";
  }
  else if (type==kQwCombinedBPM){
    result = "combinedbpm";
  }
  else if (type==kQwEnergyCalculator){
    result = "energycalculator";
  }
  else if (type==kQwHaloMonitor){
    result = "halomonitor";
  }
  return result;
};
