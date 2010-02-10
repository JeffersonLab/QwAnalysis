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
  Int_t status = fEventBuffer->OpenDataFile(run);
  if (status != CODA_OK) return status;

  // Reset the event buffer
  fEventBuffer->ResetControlParameters();

  // Store run number
  fCurrentRun = run;

  return status;
}

Int_t QwTrackingDataserver::GetEvent()
{
  // Get event
  Int_t status = fEventBuffer->GetEvent();
  if (status != CODA_OK) return status;

  // Store event number
  fCurrentEvent = fEventBuffer->GetEventNumber();

  return status;
}

void QwTrackingDataserver::FillSubsystemData(QwSubsystemArray* detectors)
{
  // Fill subsystem data
  fEventBuffer->FillSubsystemData(*detectors);

  // TODO Since this is so generic, it could go up to the VQwDataserver
};
