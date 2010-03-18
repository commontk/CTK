/*
 * ctkCLIPluginExplorer.cxx
 *
 *  Created on: Mar 11, 2010
 *      Author: zelzer
 */

#include <QCoreApplication>

#include <PluginFramework/ctkPluginManager.h>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  ctk::PluginManager pluginManager;
  pluginManager.addSearchPath("/home/zelzer/git/CTK/bin-debug/CTK-build/bin/Plugins");
  pluginManager.startAllPlugins();

  return 0;
}
