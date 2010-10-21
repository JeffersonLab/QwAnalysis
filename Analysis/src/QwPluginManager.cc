/*
 * QwPluginManager.cpp
 *
 *  Created on: Oct 21, 2010
 *      Author: wdconinc
 */

#include "QwPluginManager.h"

// Qweak headers
#include "QwLog.h"

/// Constructor
QwPluginManager::QwPluginManager(QwOptions& options)
{
  ProcessOptions(options);
  QwParameterFile plugins(fPluginsMapFile);
  QwMessage << "Loading plugins from " << fPluginsMapFile << "." << QwLog::endl;
  LoadPluginsFromParameterFile(plugins);
}

/// Destructor
QwPluginManager::~QwPluginManager()
{
  // TODO Auto-generated destructor stub
}

void QwPluginManager::DefineOptions(QwOptions &options)
{
  options.AddOptions()("plugins",
      po::value<std::string>()->default_value("plugins.map"),
      "file with plugins to include");
}

/// Process the plugin manager
void QwPluginManager::ProcessOptions(QwOptions &options)
{
  // Filename to use for plugins
  fPluginsMapFile = options.GetValue<std::string>("plugins");
}

/**
 * Fill the plugin manager with the plugins in a file
 * @param plugis Plugins file
 */
void QwPluginManager::LoadPluginsFromParameterFile(QwParameterFile& plugins)
{
  QwParameterFile* section;
  std::string section_name;
  while ((section = plugins.ReadNextSection(section_name))) {

    // Determine type and name of subsystem
    std::string plugin_type = section_name;
    std::string plugin_name;
    if (! section->FileHasVariablePair("=","name",plugin_name)) {
      QwError << "No name defined in section for plugin " << plugin_type << "." << QwLog::endl;
      continue;
    }

    // If subsystem type is explicitly disabled

    // Create plugin
    QwMessage << "Creating plugin of type " << plugin_type << " "
              << "with name " << plugin_name << "." << QwLog::endl;
    VQwPlugin* plugin = 0;
    try {
      plugin =
        VQwPluginFactory::Create(plugin_type, plugin_name);
    } catch (QwException_PluginUnknown) {
      QwError << "No support for plugins of type " << plugin_type << "." << QwLog::endl;
      // Fall-through to next error for more the psychological effect of many warnings
    }
    if (! plugin) {
      QwError << "Could not create plugin " << plugin_type << "." << QwLog::endl;
      continue;
    }

    // Pass parameters
    //plugin->LoadDetectorMaps(*section);
    // Add to array
    this->push_back(plugin);

    // Delete parameter file section
    delete section; section = 0;
  }
}

void QwPluginManager::push_back(VQwPlugin* plugin)
{
  if (plugin == NULL) {
    QwError << "QwPluginManager::push_back(): NULL plugin"
            << QwLog::endl;
    //  This is an empty plugin...
    //  Do nothing for now.

  } else {
    boost::shared_ptr<VQwPlugin> plugin_tmp(plugin);
    PluginPtrs::push_back(plugin_tmp);
  }
};

/// 'At initialization' plugin hook
void QwPluginManager::AtInitialization()
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtInitialization();
};

/// 'At configure' plugin hook
void QwPluginManager::AtConfigure()
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtConfigure();
};

/// 'At start of run' plugin hook
void QwPluginManager::AtStartOfRun()
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtStartOfRun();
};

/// 'At start of event' plugin hook
void QwPluginManager::AtStartOfEvent(QwSubsystemArray& detectors)
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtStartOfEvent(detectors);
};

/// 'At start of pattern' plugin hook
void QwPluginManager::AtStartOfPattern(QwHelicityPattern& pattern)
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtStartOfPattern(pattern);
};

/// 'At end of pattern' plugin hook
void QwPluginManager::AtEndOfPattern(QwHelicityPattern& pattern)
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtEndOfPattern(pattern);
};

/// 'At end of event' plugin hook
void QwPluginManager::AtEndOfEvent(QwSubsystemArray& detectors)
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtEndOfEvent(detectors);
};

/// 'At end of run' plugin hook
void QwPluginManager::AtEndOfRun()
{
  if (!empty())
    for (iterator plugin = begin(); plugin != end(); ++plugin)
      (*plugin)->AtEndOfRun();
};
