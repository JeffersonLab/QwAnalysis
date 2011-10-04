/**********************************************************\
* File: VQwBCM.h                                          *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "VQwBCM.h"

#include "QwBCM.h"
#include "QwCombinedBCM.h"

// System headers
#include <stdexcept>

// Qweak database headers
#include "QwDBInterface.h"

//  Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"


/**
 * \brief A fast way of creating a BCM of VQWK type with a generic name.
 */
VQwBCM* VQwBCM::Create(TString type)
{
  return Create("bcm","bcm_generic",type);
}

VQwBCM* VQwBCM::Create(TString subsystemname, TString name, TString type,
    TString clock)
{
  Bool_t localDebug = kFALSE;
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating BCM of type: "<<type<<" with name: "<<
    name<<". Subsystem Name: " <<subsystemname<<" and clock name="<<clock<<"\n";
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of BCM's supported by this code!!!
  if( type == "VQWK") {
    return new QwBCM<QwVQWK_Channel>(subsystemname,name,type);
  } else if ( type == "SIS3801" ) {
    return new QwBCM<QwSIS3801_Channel>(subsystemname,name,type,clock);
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwBCM<QwSIS3801D24_Channel>(subsystemname,name,type,clock);
  } else { // Unsupported one!
    QwWarning << "BCM of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
  return 0;
}

// QwCombinedBCM<T> Factory function
VQwBCM* VQwBCM::CreateCombo(TString type)
{
  return CreateCombo("bcm","bcm_generic",type);
}

VQwBCM* VQwBCM::CreateCombo(TString subsystemname, TString name,
    TString type)
{
  Bool_t localDebug = kFALSE;
  type.ToUpper();
  if( localDebug ) QwMessage<<"Creating CombinedBCM of type: "<<type<<" with name: "<<
    name<<". Subsystem Name: " <<subsystemname<<"\n";
  // (jc2) As a first try, let's do this the ugly way (but rather very
  // simple), just list out the types of BCM's supported by this code!!!
  if( type == "VQWK") {
    return new QwCombinedBCM<QwVQWK_Channel>(subsystemname,name,type);
  } else if ( type == "SIS3801" ) { // Default SCALER channel
    return new QwCombinedBCM<QwSIS3801_Channel>(subsystemname,name,type);
  } else if ( type == "SCALER" || type == "SIS3801D24" ) {
    return new QwCombinedBCM<QwSIS3801D24_Channel>(subsystemname,name,type);
  } else { // Unsupported one!
    QwWarning << "BCM of type="<<type<<" is UNSUPPORTED!!\n";
    exit(-1);
  }
}


