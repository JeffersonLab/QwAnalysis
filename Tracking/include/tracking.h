//
// C++ Interface: tracking
//
// Description: Contained the various object definitions associated with
//              tracks, partial tracks, events, etc.  Everything higher
//              level than a simple detector or hit.
//
//
#ifndef TRACKING_H
#define TRACKING_H

#include "globals.h"

#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwASCIIEventBuffer.h"

void SaveHits(QwHitContainer &); //for debugging purposes - Rakitha (04/02/2009)

#endif // TRACKING_H
