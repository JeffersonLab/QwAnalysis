#ifndef __QWSUBSYSTEMFACTORY__
#define __QWSUBSYSTEMFACTORY__

// Qweak headers
#include "QwLog.h"

// Forward declarations
class VQwSubsystem;

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
    virtual ~VQwSubsystemFactory() { };
    /// Subsystem creation (pure virtual)
    virtual VQwSubsystem* Create(const std::string& name) = 0;
    /// Subsystem cast (pure virtual)
    virtual VQwSubsystem* Cast(VQwSubsystem* subsys) = 0;

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
};

/// List available subsystem factories
inline void VQwSubsystemFactory::ListRegisteredSubsystems()
{
  std::map<std::string,VQwSubsystemFactory*>::iterator subsys;
  for (subsys = GetRegisteredSubsystems().begin();
       subsys != GetRegisteredSubsystems().end(); subsys++ )
    QwMessage << subsys->first << QwLog::endl;
};

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
    QwMessage << "To register this subsystem, add the following line to the top "
              << "of the source file:" << QwLog::endl;
    QwMessage << "  QwSubsystemFactory<" << type << "> the" << type
              << "Factory(\"" << type << "\");" << QwLog::endl;
    return 0; // this will most likely crash
  }
};


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
    };

  protected:

    /// Concrete subsystem creation
    VQwSubsystem* Create(const std::string& name) {
      return new subsystem_t(name);
    };

    /// Dynamic cast of subsystem
    subsystem_t* Cast(VQwSubsystem* subsys) {
      return dynamic_cast<subsystem_t*>(subsys);
    }

}; // class QwSubsystemFactory

#endif // __QWSUBSYSTEMFACTORY__
