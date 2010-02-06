/**
 * \file	QwTrackingTreeRegion.cc
 * \ingroup	QwTracking
 *
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@jlab.org>
 * \date	2009-09-04 18:06:23
 *
 * \brief Implementation of the container for the pattern databases for each detector region.
 *
 * It is useful to separate the pattern database into multiple detector regions
 * to reduce the extent by which a set of hits are compared to a pattern.
 * For instance, there are two databases for the region 3 set of detectors.
 * One for the 'upper' and one for the 'lower' detector sets.  Track segments
 * in the area downstream of the QTOR magnet are approximated by straight lines.
 * Only the VDC is designed to measure track positions, so these two databases
 * designed to contain patterns which ressemble tracks passing through the VDC.
 *
 */

#include "QwTrackingTreeRegion.h"

QwTrackingTreeRegion::QwTrackingTreeRegion()
{
  fDebug = 0; // debug level
}

QwTrackingTreeRegion::~QwTrackingTreeRegion()
{
}
