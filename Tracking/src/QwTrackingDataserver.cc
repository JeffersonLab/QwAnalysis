#include "QwTrackingDataserver.h"

// Qweak Tracking headers
#include "QwTypes.h"
#include "Det.h"
#include "treeregion.h"
#include "options.h"
using namespace QwTracking;

// Qweak hit container
#include "QwHitContainer.h"
#include "QwASCIIEventBuffer.h"

//ClassImp(QwTrackingDataserver)

QwTrackingDataserver::QwTrackingDataserver (const char* name) : VQwDataserver (name)
{
  // Initialize QwHitContainer
  fASCIIbuffer.OpenDataFile((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/1000.r2.events").c_str(),"R");

  // Instantiate hit list
  fASCIIgrandHitList = new QwHitContainer;
}

QwTrackingDataserver::~QwTrackingDataserver()
{
  delete fASCIIgrandHitList;
}

void QwTrackingDataserver::NextEvent()
{
  // get next event
  fASCIIbuffer.GetEvent();

  // this will load the QwHitContainer from set of hits read from ASCII file qweak.event
  fASCIIgrandHitList->clear();
  fASCIIbuffer.GetHitList (*fASCIIgrandHitList);
  // sort the array
  fASCIIgrandHitList->sort();
  // now we decode our QwHitContainer list and piece together with the
  // rcTreeRegion multi-dimension array.
  fASCIIbuffer.ProcessHitContainer(*fASCIIgrandHitList);
}

QwHitContainer* QwTrackingDataserver::GetHitList()
{
  return fASCIIgrandHitList;
}
