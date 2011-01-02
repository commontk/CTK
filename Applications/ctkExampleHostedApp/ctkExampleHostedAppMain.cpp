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

#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginContext.h>

// for testing purposes use:
// --hostURL http://localhost:8081/host --applicationURL http://localhost:8082/app dicomapp


#include <QApplication>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QWidget>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

void print_usage()
{
  qCritical() << "Usage:";
  qCritical() << "  " << QFileInfo(qApp->arguments().at(0)).fileName() << " --hostURL url1 --applicationURL url2 <plugin-name>";
}

int main(int argv, char** argc)
{
  QApplication app(argv, argc);
  qDebug() << "################################################################";

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkExampleHostedApp");

  // parse the command line
  qDebug() << "################################################################";

  if(qApp->arguments().size() < 5)
  {
    qCritical() << "Wrong number of command line arguments.";
    print_usage();
    exit(1);
  }

  if(qApp->arguments().at(1) != "--hostURL")
  {
    qCritical() << "First argument must be --hostURL.";
    print_usage();
    exit(1);
  }
  QString hostURL = qApp->arguments().at(2);
  qDebug() << "hostURL is: " << hostURL << " . Extracted port is: " << QUrl(hostURL).port();

  if(qApp->arguments().at(3) != "--applicationURL")
  {
    qCritical() << "First argument must be --applicationURL.";
    print_usage();
    exit(1);
  }
  QString appURL = qApp->arguments().at(4);
  qDebug() << "appURL is: " << appURL << " . Extracted port is: " << QUrl(appURL).port();

  // setup the plugin framework
  ctkProperties fwProps;
  fwProps.insert("dah.hostURL", hostURL);
  fwProps.insert("dah.appURL", appURL);
  ctkPluginFrameworkFactory fwFactory(fwProps);
  QSharedPointer<ctkPluginFramework> framework = fwFactory.getFramework();

  try {
    framework->init();
  }
  catch (const ctkPluginException& exc)
  {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    exit(2);
  }

#ifdef CMAKE_INTDIR
  QString pluginPath = qApp->applicationDirPath() + "/../plugins/" CMAKE_INTDIR "/";
#else
  QString pluginPath = qApp->applicationDirPath() + "/plugins/";
#endif

  qApp->addLibraryPath(pluginPath);

  // construct the name of the plugin with the business logic
  // (thus the actual logic of the hosted app)
  QString pluginName;
  if(qApp->arguments().size()>5)
  {
    pluginName = qApp->arguments().at(5);
  }
  else
  {
    pluginName = "org_commontk_dah_exampleapp";
  }

  // try to find the plugin and install all plugins available in 
  // pluginPath containing the string "org_commontk_dah" (but do not start them)
  QSharedPointer<ctkPlugin> appPlugin;
  QStringList libFilter;
  libFilter << "*.dll" << "*.so" << "*.dylib";
  QDirIterator dirIter(pluginPath, libFilter, QDir::Files);
  while(dirIter.hasNext())
  {
    try
    {
      QString fileLocation = dirIter.next();
      if (fileLocation.contains("org_commontk_dah"))
      {
        QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(fileLocation));
        if (fileLocation.contains(pluginName))
        {
          appPlugin = plugin;
        }
        //plugin->start(ctkPlugin::START_TRANSIENT);
      }
    }
    catch (const ctkPluginException& e)
    {
      qCritical() << e.what();
    }
  }

  // if we did not find the business logic: abort
  if(!appPlugin)
  {
    qCritical() << "Could not find plugin.";
    qCritical() << "  Plugin name: " << pluginName;
    qCritical() << "  Plugin path: " << pluginPath;
    exit(3);
  }

  // start the plugin framework
  framework->start();

  // start the plugin with the business logic
  try
  {
    appPlugin->start();
  }
  catch (const ctkPluginException& e)
  {
    qCritical() << e;
  }

  return app.exec();
}
