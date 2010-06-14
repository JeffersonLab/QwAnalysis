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
    /// Subsystem creation
    virtual VQwSubsystem* Create(const std::string& name) = 0;

    /// Map from string to concrete subsystem factories
    static std::map<std::string,VQwSubsystemFactory*>& GetRegisteredSubsystems();
    /// List available subsystem factories
    static void ListRegisteredSubsystems();

    /// Get a concrete subsystem factory by std::string
    static VQwSubsystemFactory* GetSubsystemFactory(const std::string& type);

    /// Get a concrete subsystem factory by TString
    static VQwSubsystemFactory* GetSubsystemFactory(const TString& type) {
      return GetSubsystemFactory(std::string(type.Data()));
    };
    /// Get a concrete subsystem factory by const char* string
    static VQwSubsystemFactory* GetSubsystemFactory(const char* type) {
      return GetSubsystemFactory(std::string(type));
    };

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
template<class subsystem>
class QwSubsystemFactory: public VQwSubsystemFactory {
  public:

    /// Constructor which stores type name in list of registered subsystems
    QwSubsystemFactory(const std::string& type) {
      VQwSubsystemFactory::GetRegisteredSubsystems()[type] = this;
    };

    /// Concrete subsystem creation
    virtual VQwSubsystem* Create(const std::string& name) {
      return new subsystem(name);
    };

}; // class QwSubsystemFactory

#endif // __QWSUBSYSTEMFACTORY__
