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
  

  //QwEventRing options
  options.AddOptions()("ring.size", po::value<int>()->default_value(32),"QwEventRing: ring/buffer size");
  options.AddOptions()("ring.bt", po::value<int>()->default_value(4),
                "QwEventRing: minimum beam trip count");
  options.AddOptions()("ring.hld", po::value<int>()->default_value(16),
                "QwEventRing: ring hold off");
  //end of QwEventRing options

  //QwHelicity options
  options.AddOptions()("helicity.30bitseed", po::value<bool>()->zero_tokens(), "QwHelicty: 30bit random seed");
  options.AddOptions()("helicity.24bitseed", po::value<bool>()->zero_tokens(), "QwHelicty: 24bit random seed");
  options.AddOptions()("helicity.patternoffset", po::value<int>(),"QwHelicity: pattern offset. Set 1 when pattern starts with 1 or 0 when starts with 0");
  options.AddOptions()("helicity.patternphase", po::value<int>(),"QwHelicity: pattern phase. Can be set to 4 or 8");

  options.AddOptions()("helicity.delay", po::value<int>(),"QwHelicity: pattern delay. Default delay is 2 patterns, set at the helicity map file.");
  //End of QwHelicity options

  /* Define parity options */


}

#endif // QWOPTIONSPARITY_H
