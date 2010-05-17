#ifndef __QWSUBSYSTEMFACTORY__
#define __QWSUBSYSTEMFACTORY__

// Qweak headers
#include "QwLog.h"

// Forward declarations
class VQwSubsystem;

/// Pure virtual subsystem factory
class QwSubsystemFactory {
  public:
    /// Default virtual destructor
    virtual ~QwSubsystemFactory() { };
    /// Subsystem creation
    virtual VQwSubsystem* Create(const std::string& name) = 0;
};

/// Map from string to concrete subsystem factory objects
std::map<std::string,QwSubsystemFactory*>& GetRegisteredSubsystems();

/// Concrete templated subsystem factory
template<class subsystem>
class QwSubsystemFactoryConcrete: public QwSubsystemFactory {
  public:

    /// Constructor with type name, which is stored in the list of
    /// registered subsystems.
    QwSubsystemFactoryConcrete(const std::string& type) {
      GetRegisteredSubsystems()[type] = this;
    };

    /// Concrete subsystem creation
    virtual VQwSubsystem* Create(const std::string& name) {
      return new subsystem(name);
    };

};

#endif // __QWSUBSYSTEMFACTORY__
