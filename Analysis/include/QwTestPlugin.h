/*
 * QwTestPlugin.h
 *
 *  Created on: Oct 21, 2010
 *      Author: wdconinc
 */

#ifndef __QWTESTPLUGIN__
#define __QWTESTPLUGIN__

// Qweak headers
#include "VQwPlugin.h"

class QwTestPlugin: public VQwPlugin {

  public:

    /// \brief Constructor with name
    QwTestPlugin(const std::string& name);
    /// \brief Virtual destructor
    virtual ~QwTestPlugin();

  public:

    /// \brief 'At start of run' plugin hook
    void AtStartOfRun();
    /// \brief 'At start of run' plugin hook
    void AtEndOfRun();

};

#endif // __QWTESTPLUGIN__
