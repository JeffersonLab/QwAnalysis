/*!
 * \file   QwOptionsParity.h
 * \brief  Load the options for the parity subsystems
 *
 * To maintain the separation of the tracking and parity subsystems in the
 * libraries, we can only define the options for the parity subsystems in
 * this header.  This header should be loaded in executables that use the
 * parity subsystem options.
 *
 * \author Wouter Deconinck
 * \date   2009-12-10
 */

#ifndef QWOPTIONSPARITY_H
#define QWOPTIONSPARITY_H

// Qweak options header (should be first)
#include "QwOptions.h"

// Qweak headers
#include "QwMainCerenkovDetector.h"
#include "QwLumi.h"
#include "QwEventRing.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwRootFile.h"

void DefineOptionsParity(QwOptions& options)
{
  /* Define general options */
  QwOptions::DefineOptions(options);

  /* Define parity options */
  QwMainCerenkovDetector::DefineOptions(options);
  QwLumi::DefineOptions(options);
  QwEventRing::DefineOptions(options);
  QwHelicity::DefineOptions(options);
  QwHelicityPattern::DefineOptions(options);
}

#endif // QWOPTIONSPARITY_H
