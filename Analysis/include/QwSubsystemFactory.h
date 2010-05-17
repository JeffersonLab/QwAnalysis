#ifndef __QWSUBSYSTEMFACTORY__
#define __QWSUBSYSTEMFACTORY__

// Qweak headers
#include "QwLog.h"

// Forward declarations
class VQwSubsystem;

/// Pure virtual subsystem factory
class VQwSubsystemFactory {
  public:
    /// Default virtual destructor
    virtual ~VQwSubsystemFactory() { };
    /// Subsystem creation
    virtual VQwSubsystem* Create(const std::string& name) = 0;
}; // class VQwSubsystemFactory

/// Map from string to concrete subsystem factories
inline std::map<std::string,VQwSubsystemFactory*>& GetRegisteredSubsystems() {
  static std::map<std::string,VQwSubsystemFactory*> theSubsystemMap;
  return theSubsystemMap;
};

/// List available subsystem factories
inline void ListSubsystemFactories() {
  std::map<std::string,VQwSubsystemFactory*>::iterator subsys;
  for (subsys = GetRegisteredSubsystems().begin();
       subsys != GetRegisteredSubsystems().end(); subsys++ )
    QwMessage << subsys->first << QwLog::endl;
};

/// Get a concrete subsystem factory
inline VQwSubsystemFactory* GetSubsystemFactory(const std::string& type) {
  if (GetRegisteredSubsystems().find(type) != GetRegisteredSubsystems().end())
    return GetRegisteredSubsystems()[type];
  else {
    QwError << "Subsystem " << type << " is not registered!" << QwLog::endl;
    QwMessage << "Available subsystems:" << QwLog::endl;
    ListSubsystemFactories();
    return 0; // this will most likely crash
  }
};

/// Concrete templated subsystem factory
template<class subsystem>
class QwSubsystemFactory: public VQwSubsystemFactory {
  public:

    /// Constructor which stores type name in list of registered subsystems
    QwSubsystemFactory(const std::string& type) {
      GetRegisteredSubsystems()[type] = this;
    };

    /// Concrete subsystem creation
    virtual VQwSubsystem* Create(const std::string& name) {
      return new subsystem(name);
    };

}; // class QwSubsystemFactory

#endif // __QWSUBSYSTEMFACTORY__
