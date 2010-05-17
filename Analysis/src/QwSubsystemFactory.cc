#include "QwSubsystemFactory.h"

/// Return the list of registered subsystems
std::map<std::string,QwSubsystemFactory*>& GetRegisteredSubsystems() {
  static std::map<std::string,QwSubsystemFactory*> theSubsystemMap;
  return theSubsystemMap;
}
