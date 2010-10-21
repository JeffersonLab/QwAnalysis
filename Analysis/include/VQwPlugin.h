/*!
 * \file   VQwPlugin.h
 * \brief  Definition of the pure virtual base class of all plugins
 *
 * \author W. Deconinck
 * \date   2010-10-21
 */

#ifndef __VQWPLUGIN__
#define __VQWPLUGIN__

// System headers
#include <string>
#include <vector>

// Qweak headers
// Note: the plugin factory header is included here because every plugin
// has to register itself with a plugin factory.
#include "QwLog.h"
#include "QwPluginFactory.h"
#include "QwSubsystemArray.h"
#include "QwHelicityPattern.h"


/**
 *  \class   VQwPlugin
 *  \ingroup QwAnalysis
 *  \brief   The pure virtual base class of all plugins
 *
 * Virtual base class for the classes containing the hooks for plugins.
 * Each class that inherits from VQwPlugin and implements the analysis
 * hooks will be able to run as a plugin during analysis.
 */
class VQwPlugin {

  public:

    /// Constructor with name
    VQwPlugin(const std::string& name)
    : fPluginName(name) {
      QwMessage << "Constructing plugin " << fPluginName << QwLog::endl;
    };

    /// Default destructor
    virtual ~VQwPlugin() {
      QwMessage << "Destructing plugin " << fPluginName << QwLog::endl;
    };

  public:

    /// \name Unimplemented hooks for plugin functionality
    // @{
    virtual void AtInitialization() { };
    virtual void AtConfigure() { };
    virtual void AtStartOfRun() { };
    virtual void AtStartOfEvent(QwSubsystemArray& detectors) { };
    virtual void AtStartOfPattern(QwHelicityPattern& pattern) { };
    virtual void AtEndOfPattern(QwHelicityPattern& pattern) { };
    virtual void AtEndOfEvent(QwSubsystemArray& detectors) { };
    virtual void AtEndOfRun() { };
    // @}


  private:

    /// Private default constructor
    VQwPlugin() { };

    std::string fPluginName; ///< Name of this subsystem

}; // class VQwPlugin

#endif // __VQWPLUGIN__
