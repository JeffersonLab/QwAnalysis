/*
 * QwDBInterface.cc
 *
 *  Created on: Dec 14, 2010
 *      Author: wdconinc
 */

#include "QwDBInterface.h"

// Qweak headers
#include "QwDatabase.h"

void QwDBInterface::SetMonitorID(QwDatabase *db)
{
  fDeviceId = db->GetMonitorID(fDeviceName.Data());
}

void QwDBInterface::SetMainDetectorID(QwDatabase *db)
{
  fDeviceId = db->GetMainDetectorID(fDeviceName.Data());
}

void QwDBInterface::SetLumiDetectorID(QwDatabase *db)
{
  fDeviceId = db->GetLumiDetectorID(fDeviceName.Data());
}
