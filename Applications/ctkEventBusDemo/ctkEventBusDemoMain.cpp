/*=============================================================================

  Library: CTK

  Copyright (c) 2010 BioComputing Competence Centre - Super Computing Solutions

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include <ctkConfig.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginContext.h>
#include <ctkServiceReference.h>

#include "ctkEventAdminBus.h"

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QWidget>
#include <QFileInfo>

#include "ctkEventBusDemoMainWindow.h"

int main(int argv, char** argc) {
  QApplication app(argv, argc);
  qDebug() << "################################################################";

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkEventBusDemoApp");

  qDebug() << "################################################################";

  // setup the plugin framework
  ctkPluginFrameworkFactory fwFactory;
  QSharedPointer<ctkPluginFramework> framework = fwFactory.getFramework();

  try {
    framework->init();
  } catch (const ctkPluginException& exc) {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    exit(2);
  }

#ifdef CMAKE_INTDIR
  QString pluginPath = CTK_PLUGIN_DIR CMAKE_INTDIR "/";
#else
  QString pluginPath = CTK_PLUGIN_DIR;
#endif

  qApp->addLibraryPath(pluginPath);

  QStringList libFilter;
  libFilter << "*.dll" << "*.so" << "*.dylib";
  QDirIterator dirIter(pluginPath, libFilter, QDir::Files);

  QStringList pluginsToInstall;
  pluginsToInstall << "org_commontk_eventbus";
    qDebug() << pluginPath;
  QList<QSharedPointer<ctkPlugin> > installedPlugins;
  while(dirIter.hasNext())
  {
    try
    {
      QString fileLocation = dirIter.next();
      foreach(QString pluginToInstall, pluginsToInstall)
      {          
        if (fileLocation.contains(pluginToInstall))
        {
          QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(fileLocation));
          installedPlugins << plugin;
          break;
        }
      }
    }
    catch (const ctkPluginException& e)
    {
      qCritical() << e.what();
    }
  }

  framework->start();

  foreach(QSharedPointer<ctkPlugin> plugin, installedPlugins)
  {
      plugin->start(ctkPlugin::START_TRANSIENT);
  }

  ctkServiceReference ebr = framework->getPluginContext()->getServiceReference("ctkEventAdminBus");
  ctkEventAdminBus *eb = framework->getPluginContext()->getService<ctkEventAdminBus>(ebr);

  ctkEventBusDemoMainWindow win(eb);
  win.show();

  return app.exec();
}
