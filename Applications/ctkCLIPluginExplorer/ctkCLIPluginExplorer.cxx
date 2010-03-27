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
  pluginManager.addSearchPath("/home/sascha/git/CTK-bin/CTK-build/bin/Plugins");
  pluginManager.startAllPlugins();

  qDebug() << "List of services: " <<  pluginManager.serviceManager()->findServices();

  QObject* service = pluginManager.serviceManager()->loadInterface("org.commontk.cli.ICLIManager");

  return 0;
}
