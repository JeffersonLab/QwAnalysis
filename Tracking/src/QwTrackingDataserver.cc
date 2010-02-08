#include "QwTrackingDataserver.h"
ClassImp(QwTrackingDataserver);

// Qweak headers
#include "QwTypes.h"
#include "QwEventBuffer.h"
#include "QwTrackingWorker.h"
#include "QwHitContainer.h"

QwTrackingDataserver::QwTrackingDataserver (const char* name) : VQwDataserver (name)
{
  fEventBuffer = new QwEventBuffer;

}

QwTrackingDataserver::~QwTrackingDataserver()
{

}

Int_t QwTrackingDataserver::GetRun(Int_t run)
{
  // Open run
  if (fEventBuffer->OpenDataFile(run) == CODA_ERROR) return CODA_ERROR;

  // Reset the event buffer
  fEventBuffer->ResetControlParameters();

  return CODA_OK;
}

Int_t QwTrackingDataserver::GetEvent()
{
  // Get event
  return fEventBuffer->GetEvent();
}

Int_t QwTrackingDataserver::GetEventNumber()
{
  // Get event number
  return fEventBuffer->GetEventNumber();
}

void QwTrackingDataserver::FillSubsystemData(QwSubsystemArray* detectors)
{
  // Fill subsystem data
  fEventBuffer->FillSubsystemData(*detectors);

  // TODO Since this is so generic, it could go up to the VQwDataserver
};
