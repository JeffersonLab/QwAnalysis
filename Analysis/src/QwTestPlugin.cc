/*
 * QwTestPlugin.cpp
 *
 *  Created on: Oct 21, 2010
 *      Author: wdconinc
 */

#include "QwTestPlugin.h"

// Register this plugin with the manager
QwPluginFactory<QwTestPlugin> theTestPluginFactory("QwTestPlugin");

/// Constructor with name
QwTestPlugin::QwTestPlugin(const std::string& name)
: VQwPlugin(name)
{
  QwMessage << "Test plugin created." << QwLog::endl;
}

/// Virtual destructor
QwTestPlugin::~QwTestPlugin()
{
  QwMessage << "Test plugin destructed." << QwLog::endl;
}

/// 'At start of run' plugin hook
void QwTestPlugin::AtStartOfRun()
{
  QwMessage << "Test plugin at start of run." << QwLog::endl;
}

/// 'At start of run' plugin hook
void QwTestPlugin::AtEndOfRun()
{
  QwMessage << "Test plugin at end of run." << QwLog::endl;
}
