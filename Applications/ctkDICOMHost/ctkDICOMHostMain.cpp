/*=========================================================================

  Library:   CTK

  Copyright (c) Isomics Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTreeView>
#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QResource>
#include <QDebug>

// CTKPluginFramework includes
#include <ctkConfig.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginContext.h>

// CTK widget includes
#include <ctkDICOMAppWidget.h>
#include <ctkDICOMBrowser.h>

// Host logic 
#include "ctkDICOMHostMainLogic.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"

// STD includes
#include <iostream>

// ui of this application
#include "ui_ctkDICOMHostMainWidget.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  app.setOrganizationName("commontk");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkDICOM");

  // startup plugin framework and dah
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
  // end startup plugin framework and dah

  // set up Qt resource files
  QResource::registerResource("./Resources/ctkDICOM.qrc");

  QSettings settings;
  QString databaseDirectory;

  // set up the database 
  if (argc > 1)
  {
    QString directory(argv[1]);
    settings.setValue("DatabaseDirectory", directory);
    settings.sync();
  }

  if ( settings.value("DatabaseDirectory", "") == "" )
  {
    databaseDirectory = QString("./ctkDICOM-Database");
    std::cerr << "No DatabaseDirectory on command line or in settings.  Using \"" << qPrintable(databaseDirectory) << "\".\n";
  } else
  {
    databaseDirectory = settings.value("DatabaseDirectory", "").toString();
  }

  QDir qdir(databaseDirectory);
  if ( !qdir.exists(databaseDirectory) ) 
  {
    if ( !qdir.mkpath(databaseDirectory) )
    {
      std::cerr << "Could not create database directory \"" << qPrintable(databaseDirectory) << "\".\n";
      return EXIT_FAILURE;
    }
  }

  QWidget *widget = new QWidget;
  Ui::ctkDICOMHostMainWidget ui;
  ui.setupUi(widget);

  widget->show();

  ctkDICOMHostMainLogic *logic = new ctkDICOMHostMainLogic(ui.placeHolderForHostedApp, ui.placeHolderForDICOMAppWidget, ui.placeHolderForControls);
  logic->connect(ui.configureHostedApp,SIGNAL(clicked()), logic, SLOT(configureHostedApp()));
  logic->connect(ui.sendDataToHostedApp,SIGNAL(clicked()), logic, SLOT(sendDataToHostedApp()));
  logic->connect(logic, SIGNAL(TreeSelectionChanged(const QString &)), ui.selection, SLOT(setText(const QString &)));
  logic->connect(logic, SIGNAL(SelectionValid(bool)), ui.sendDataToHostedApp, SLOT(setEnabled(bool)));
  logic->connect(ui.cbShowControls, SIGNAL(toggled(bool)), ui.placeHolderForControls->children()[0], SLOT(setVisible(bool)));

  //ctkDICOMAppWidget DICOMApp(widget);

  ////DICOMApp.setDatabaseDirectory(databaseDirectory);
  ////DICOMApp.show();

  return app.exec();
}
