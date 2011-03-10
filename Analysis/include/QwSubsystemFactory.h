#ifndef __QWSUBSYSTEMFACTORY__
#define __QWSUBSYSTEMFACTORY__

// System headers
#include <cxxabi.h>
#include <cstdlib>

// Qweak headers
#include "QwLog.h"

// Forward declarations
class VQwSubsystem;

// Exceptions
struct QwException_SubsystemUnknown {
  QwException_SubsystemUnknown() { }
};

/**
 *  \class VQwSubsystemFactory
 *  \ingroup QwAnalysis
 *  \brief Pure virtual subsystem factory
 *
 * In order to enable the instantiation of subsystems based on run-time
 * information, we generate a map of subsystem factories by type name.
 * This map is filled automatically when the executable is loaded, and
 * contains concrete factories derived from this pure virtual base class.
 */
class VQwSubsystemFactory {

  public:

    /// Default virtual destructor
    virtual ~VQwSubsystemFactory() { }

    /// Create a subsystem of type with name
    static VQwSubsystem* Create(const std::string& type, const std::string& name) {
      return GetSubsystemFactory(type)->Create(name);
    }

    /// Dynamic cast of subsystem into type
    static VQwSubsystem* Cast(VQwSubsystem* subsys, const std::string& type) {
      return GetSubsystemFactory(type)->Cast(subsys);
    }

    /// Inherits from subsystem type
    static bool InheritsFrom(VQwSubsystem* subsys, const std::string& type) {
      return (Cast(subsys,type) != 0);
    }

    /// Subsystem creation (pure virtual)
    virtual VQwSubsystem* Create(const std::string& name) const = 0;
    /// Subsystem dynamic cast (pure virtual)
    virtual VQwSubsystem* Cast(VQwSubsystem* subsys) const = 0;

  protected:

    /// Map from string to concrete subsystem factories
    static std::map<std::string,VQwSubsystemFactory*>& GetRegisteredSubsystems();
    /// List available subsystem factories
    static void ListRegisteredSubsystems();

    /// Get a concrete subsystem factory
    static VQwSubsystemFactory* GetSubsystemFactory(const std::string& type);

}; // class VQwSubsystemFactory

/// Map from string to concrete subsystem factories
inline std::map<std::string,VQwSubsystemFactory*>&
VQwSubsystemFactory::GetRegisteredSubsystems()
{
  static std::map<std::string,VQwSubsystemFactory*> theSubsystemMap;
  return theSubsystemMap;
}

/// List available subsystem factories
inline void VQwSubsystemFactory::ListRegisteredSubsystems()
{
  std::map<std::string,VQwSubsystemFactory*>::iterator subsys;
  for (subsys = GetRegisteredSubsystems().begin();
       subsys != GetRegisteredSubsystems().end(); subsys++ )
    QwMessage << subsys->first << QwLog::endl;
}

/// Get a concrete subsystem factory by std::string
inline VQwSubsystemFactory*
VQwSubsystemFactory::GetSubsystemFactory(const std::string& type)
{
  if (GetRegisteredSubsystems().find(type) != GetRegisteredSubsystems().end())
    return GetRegisteredSubsystems()[type];
  else {
    QwError << "Subsystem " << type << " is not registered!" << QwLog::endl;
    QwMessage << "Available subsystems:" << QwLog::endl;
    ListRegisteredSubsystems();
    QwWarning << "To register this subsystem, add the following line to the top "
              << "of the source file:" << QwLog::endl;
    QwWarning << "  RegisterSubsystemFactor(" << type << ");" << QwLog::endl;
    QwWarning << "Ensure that the dynamic library contains the factory object."
              << QwLog::endl;
    throw QwException_SubsystemUnknown();
  }
}


/**
 *  \class QwSubsystemFactory
 *  \ingroup QwAnalysis
 *  \brief Concrete templated subsystem factory
 *
 * This class represents concrete instances of the virtual VQwSubsystemFactory
 * from which it inherits.  Each concrete factory can create subsystems with
 * a given name.
 */
template<class subsystem_t>
class QwSubsystemFactory: public VQwSubsystemFactory {

  public:

    /// Constructor which stores type name in list of registered subsystems
    QwSubsystemFactory(const std::string& type) {
      VQwSubsystemFactory::GetRegisteredSubsystems()[type] = this;
    }

    /// Concrete subsystem creation
    VQwSubsystem* Create(const std::string& name) const {
      return new subsystem_t(name);
    }

    /// Dynamic cast of subsystem
    subsystem_t* Cast(VQwSubsystem* subsys) const {
      return dynamic_cast<subsystem_t*>(subsys);
    }

}; // class QwSubsystemFactory


/// Polymorphic copy constructor virtual base class
class VQwCloneable {
  public:
    /// Virtual destructor
    virtual ~VQwCloneable() { }

    /// Abstract clone method when no derived method is defined
    virtual VQwSubsystem* Clone() const {
      int status;
      const type_info& ti = typeid(*this);
      char* name = abi::__cxa_demangle(ti.name(), 0, 0, &status);
      QwError << "Clone() is not implemented for class " << name << "!" << QwLog::endl;
      QwMessage << "Modify the class definition of " << name << " to:" << QwLog::endl;
      QwMessage << "  class " << name << ": public VQwSubsystemParity, "
                << "public MQwCloneable<" << name << ">" << QwLog::endl;
      free(name);
      return 0;
    }

    /// Virtual subsystem factory getter
    virtual const VQwSubsystemFactory* Factory() const { return 0; }

}; // class VQwCloneable


/// Polymorphic copy construction by curiously recurring template pattern (mix-in)
/// We have lost covariancy: clone will have the base type, not the derived type...
template <typename subsystem_t>
class MQwCloneable: virtual public VQwCloneable {
  public:
    /// Concrete clone method
    virtual VQwSubsystem* Clone() const {
      return new subsystem_t(static_cast<const subsystem_t&>(*this));
    }

    /// Subsystem factory getter
    const VQwSubsystemFactory* Factory() const { return fFactory; }

    /// Subsystem creation
    static VQwSubsystem* Create(const std::string& name) {
      if (fFactory) return fFactory->Create(name);
      else return 0;
    }

    /// Subsystem dynamic cast
    static subsystem_t* Cast(VQwSubsystem* subsys) {
      if (fFactory) return fFactory->Cast(subsys);
      else return 0;
    }

  private:
    /// Pointer to the subsystem factory of this type
    static const VQwSubsystemFactory* fFactory;

}; // class MQwCloneable

// Macro to create and register the subsystem factory of type A
// Note: a call to this macro should be followed by a semi-colon.
#define RegisterSubsystemFactory(A) template<> const VQwSubsystemFactory* A::MQwCloneable<A>::fFactory = new QwSubsystemFactory<A>(#A)


#endif // __QWSUBSYSTEMFACTORY__
