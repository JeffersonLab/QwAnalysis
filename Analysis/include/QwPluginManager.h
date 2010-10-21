/*
 * QwPluginManager.h
 *
 *  Created on: Oct 21, 2010
 *      Author: wdconinc
 */

#ifndef __QWPLUGINMANAGER__
#define __QWPLUGINMANAGER__

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"
#include "VQwPlugin.h"
#include "QwParameterFile.h"
#include "QwSubsystemArray.h"
#include "QwHelicityPattern.h"



/**
 *  \class   QwPluginManager
 *  \ingroup QwAnalysis
 *  \brief   Manager for analysis plugins
 */
class QwPluginManager: public std::vector<boost::shared_ptr<VQwPlugin> > {

  private:

    typedef std::vector<boost::shared_ptr<VQwPlugin> > PluginPtrs;

  public:

    /// \brief Constructor with options
    QwPluginManager(QwOptions& options);
    /// \brief Virtual destructor
    virtual ~QwPluginManager();

    /// \brief Define configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Add the plugin
    void push_back(VQwPlugin* plugin);

    /// \brief Load plugins from parameter file
    void LoadPluginsFromParameterFile(QwParameterFile& plugins);

    /// \brief 'At initialization' plugin hook
    void AtInitialization();
    /// \brief 'At configure' plugin hook
    void AtConfigure();
    /// \brief 'At start of run' plugin hook
    void AtStartOfRun();
    /// \brief 'At start of event' plugin hook
    void AtStartOfEvent(QwSubsystemArray& detectors);
    /// \brief 'At start of pattern' plugin hook
    void AtStartOfPattern(QwHelicityPattern& pattern);
    /// \brief 'At end of pattern' plugin hook
    void AtEndOfPattern(QwHelicityPattern& pattern);
    /// \brief 'At end of event' plugin hook
    void AtEndOfEvent(QwSubsystemArray& detectors);
    /// \brief 'At end of run' plugin hook
    void AtEndOfRun();

  private:

    /// Private default constructor
    QwPluginManager(){};

    /// Filename of the global plugins definition
    std::string fPluginsMapFile;

};

#endif // __QWPLUGINMANAGER__
