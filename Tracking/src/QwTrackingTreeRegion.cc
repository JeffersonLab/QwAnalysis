//
// C++ Implementation: QwTrackingTreeRegion
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "QwTrackingTreeRegion.h"


namespace QwTracking {

QwTrackingTreeRegion::QwTrackingTreeRegion() {
 /* Set debug level */
  debug = 0;

  if( debug )
      cout<<"###### Calling QwTrackingTreeRegion::QwTrackingTreeRegion ()"<<endl;

  if( debug )
      cout<<"###### Leaving QwTrackingTreeRegion::QwTrackingTreeRegion ()"<<endl;
}

QwTrackingTreeRegion::~QwTrackingTreeRegion() {

  if( debug )
      cout<<"###### Calling QwTrackingTreeRegion::~QwTrackingTreeRegion ()"<<endl;

  if( debug )
      cout<<"###### Leaving QwTrackingTreeRegion::~QwTrackingTreeRegion ()"<<endl;
}

} // QwTracking
