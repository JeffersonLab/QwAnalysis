#include "QwOmnivore.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwOmnivore<VQwSubsystemParity> > theParityOmnivoreFactory("QwParityOmnivore");

// Register this subsystem with the factory
// TODO (wdc) disabled due to extraneous includes
//QwSubsystemFactory<QwOmnivore<VQwSubsystemTracking> > theTrackingOmnivoreFactory("QwTrackingOmnivore");
