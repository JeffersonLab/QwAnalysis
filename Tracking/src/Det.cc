//
// C++ Implementation: Det.cc
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

// Qweak headers
#include "QwTypes.h"
#include "Det.h"

// Global variables for tracking modules (deprecated)
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
