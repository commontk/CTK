/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

// Qt includes
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QDebug>

#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QWidget>
#include <QUrl>

// CTKPluginFramework includes
#include <ctkConfig.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginContext.h>

// CTK includes
#include <ctkExampleDicomHost.h>
#include <ctkHostAppExampleWidget.h>
#include <ui_ctkExampleHostMainWindow.h>

// STD includes
#include <cstdlib>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkExampleHost");

  ctkPluginFrameworkFactory fwFactory;
  QSharedPointer<ctkPluginFramework> framework = fwFactory.getFramework();

  try
    {
    framework->init();
    }
  catch (const ctkPluginException& exc)
    {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    return EXIT_FAILURE;
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
  pluginsToInstall << "org_commontk_dah_core." << "org_commontk_dah_host."
                   << "org_commontk_dah_examplehost.";

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
    plugin->start();
    }

  QMainWindow mainWindow;
  Ui::MainWindow ui;
  ui.setupUi(&mainWindow);
  if ( argc > 1 )
    {
    ui.controlWidget->setAppFileName(argv[1]);
    }

//  mainWindow.addDockWidget(static_cast<Qt::DockWidgetArea>(4),new ctkHostAppExampleWidget());

//  QVBoxLayout* layout = new QVBoxLayout(&mainWindow);

//  ctkHostAppExampleWidget* placeholder = new ctkHostAppExampleWidget(&mainWindow);

//  layout->addWidget(placeholder);
  mainWindow.show();

  return app.exec();
}
