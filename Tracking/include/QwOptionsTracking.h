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
#include "QwTreeEventBuffer.h"
#include "QwDriftChamberVDC.h"
#include "QwDriftChamberHDC.h"
#include "QwMagneticField.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwRayTracer.h"


void DefineOptionsTracking(QwOptions& options)
{
  /* Define general options */
  QwOptions::DefineOptions(options);

  /* Define tracking options */
  QwTrackingWorker::DefineOptions(options);
  QwTreeEventBuffer::DefineOptions(options);
  QwDriftChamber::DefineOptions(options);
  QwDriftChamberVDC::DefineOptions(options);
  QwDriftChamberHDC::DefineOptions(options);
  QwMagneticField::DefineOptions(options);
  QwTriggerScintillator::DefineOptions(options);
  QwMainDetector::DefineOptions(options);
  QwRayTracer::DefineOptions(options);
}

#endif // QWOPTIONSTRACKING_H
