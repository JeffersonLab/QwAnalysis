/*!
 * \file   QwOptionsTracking.h
 * \brief  Load the options for the tracking subsystems
 *
 * To maintain the separation of the tracking and parity subsystems in the
 * libraries, we can only define the options for the tracking subsystems in
 * this header.  This header should be loaded in executables that use the
 * tracking subsystem options.
 *
 * \author Wouter Deconinck
 * \date   2009-12-10
 */

#ifndef QWOPTIONSTRACKING_H
#define QWOPTIONSTRACKING_H

// Qweak options header (should be first)
#include "QwOptions.h"

// Qweak tracking subsystems that provide options
#include "QwTrackingWorker.h"
#include "QwDriftChamberVDC.h"
#include "QwMagneticField.h"

void DefineOptionsTracking(QwOptions& options)
{
  /* Define general options */
  QwOptions::DefineOptions(options);

  /* Define tracking options */
  QwTrackingWorker::DefineOptions(options);
  QwDriftChamberVDC::DefineOptions(options);
  QwMagneticField::DefineOptions(options);
}

#endif // QWOPTIONSTRACKING_H
