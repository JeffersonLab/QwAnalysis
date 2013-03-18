#include "QwTrackingDataserver.h"

// Qweak Tracking headers
#include "QwTypes.h"
#include "QwTrackingTreeRegion.h"

// Qweak hit container
#include "QwHitContainer.h"

QwTrackingDataserver::QwTrackingDataserver (const char* name) : VQwDataserver (name)
{
  // Initialize QwHitContainer
  fEventBuffer.OpenDataFile(398);

  // Instantiate hit list
  fHitList = new QwHitContainer;
}

QwTrackingDataserver::~QwTrackingDataserver()
{
  delete fHitList;
}

void QwTrackingDataserver::NextEvent()
{
  // get next event
  fEventBuffer.GetEvent();

  // this will load the QwHitContainer from set of hits read from ASCII file qweak.event
  fHitList->clear();

  //fEventBuffer.GetHitList (*fHitList);
  // sort the array
  fHitList->sort();
  // now we decode our QwHitContainer list and piece together with the
  // rcTreeRegion multi-dimension array.
  //fEventBuffer.ProcessHitContainer(*fHitList);
}

QwHitContainer* QwTrackingDataserver::GetHitList()
{
  return fHitList;
}
