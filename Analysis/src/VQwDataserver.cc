#include "VQwDataserver.h"
ClassImp(VQwDataserver);

// Qweak headers
#include "QwEventBuffer.h"

void VQwDataserver::CloseDataFile()
{
  // Close CODA file
  fEventBuffer->CloseDataFile();
  fEventBuffer->ReportRunSummary();

  return;
}
