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

void DefineOptionsParity(QwOptions& options)
{
  /* Define general options */
  QwOptions::DefineOptions(options);

  //QwAnalysis_BeamLine Options
  options.AddOptions()("skip", po::value<int>()->default_value(0),"No. of events to skip");
  options.AddOptions()("take", po::value<int>()->default_value(1000),"No. of events to save into tree = (take - skip)");
  


  /* Define parity options */


}

#endif // QWOPTIONSPARITY_H
