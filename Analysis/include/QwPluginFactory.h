#ifndef __QWPLUGINFACTORY__
#define __QWPLUGINFACTORY__

// Qweak headers
#include "QwLog.h"

// Forward declarations
class VQwPlugin;

// Exceptions
struct QwException_PluginUnknown {
  QwException_PluginUnknown() { };
};

/**
 *  \class VQwPluginFactory
 *  \ingroup QwAnalysis
 *  \brief Pure virtual plugin factory
 *
 * For documentation on the design principles behind this factory system,
 * please consult the VQwSubsystemFactory documentation.
 */
class VQwPluginFactory {

  public:

    /// Default virtual destructor
    virtual ~VQwPluginFactory() { };

    /// Create a subsystem of type with name
    static VQwPlugin* Create(const std::string& type, const std::string& name) {
      return GetPluginFactory(type)->Create(name);
    }

  protected:

    /// Plugin creation (pure virtual)
    virtual VQwPlugin* Create(const std::string& name) = 0;

    /// Map from string to concrete plugin factories
    static std::map<std::string,VQwPluginFactory*>& GetRegisteredPlugins();
    /// List available plugin factories
    static void ListRegisteredPlugins();

    /// Get a concrete plugin factory
    static VQwPluginFactory* GetPluginFactory(const std::string& type);

}; // class VQwPluginFactory

/// Map from string to concrete plugin factories
inline std::map<std::string,VQwPluginFactory*>&
VQwPluginFactory::GetRegisteredPlugins()
{
  static std::map<std::string,VQwPluginFactory*> thePluginMap;
  return thePluginMap;
};

/// List available plugin factories
inline void VQwPluginFactory::ListRegisteredPlugins()
{
  std::map<std::string,VQwPluginFactory*>::iterator plugin;
  for (plugin = GetRegisteredPlugins().begin();
       plugin != GetRegisteredPlugins().end(); plugin++ )
    QwMessage << plugin->first << QwLog::endl;
};

/// Get a concrete plugin factory by std::string
inline VQwPluginFactory*
VQwPluginFactory::GetPluginFactory(const std::string& type)
{
  if (GetRegisteredPlugins().find(type) != GetRegisteredPlugins().end())
    return GetRegisteredPlugins()[type];
  else {
    QwError << "Plugin " << type << " is not registered!" << QwLog::endl;
    QwMessage << "Available plugins:" << QwLog::endl;
    ListRegisteredPlugins();
    QwWarning << "To register this plugin, add the following line to the top "
              << "of the source file:" << QwLog::endl;
    QwWarning << "  QwPluginFactory<" << type << "> the" << type
              << "Factory(\"" << type << "\");" << QwLog::endl;
    QwWarning << "Ensure that the dynamic library contains the factory object."
              << QwLog::endl;
    throw QwException_PluginUnknown();
  }
};


/**
 *  \class QwPluginFactory
 *  \ingroup QwAnalysis
 *  \brief Concrete templated plugin factory
 *
 * For documentation on the design principles behind this factory system,
 * please consult the VQwSubsystemFactory documentation.
 */
template<class plugin_t>
class QwPluginFactory: public VQwPluginFactory {

  public:

    /// Constructor which stores type name in list of registered plugins
    QwPluginFactory(const std::string& type) {
      VQwPluginFactory::GetRegisteredPlugins()[type] = this;
    };

  protected:

    /// Concrete plugin creation
    VQwPlugin* Create(const std::string& name) {
    	return new plugin_t(name);
    };

}; // class QwPluginFactory

#endif // __QWPLUGINFACTORY__
