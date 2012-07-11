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
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QWidget>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

// CTK includes
#include <ctkConfig.h>
#include <ctkCommandLineParser.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginContext.h>

// For testing purposes use:
// --hostURL http://localhost:8081/host --applicationURL http://localhost:8082/app dicomapp

//----------------------------------------------------------------------------
void print_usage()
{
  qCritical() << "Usage:";
  qCritical() << "  " << QFileInfo(qApp->arguments().at(0)).fileName() << " --hostURL url1 --applicationURL url2 <plugin-name>";
}

//----------------------------------------------------------------------------
int main(int argv, char** argc)
{
  QApplication app(argv, argc);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkExampleHostedApp");

  ctkCommandLineParser parser;
  parser.setArgumentPrefix("--", "-"); // Use Unix-style argument names

  // Add command line argument names
  parser.addArgument("hostURL", "", QVariant::String, "Hosting system URL");
  parser.addArgument("applicationURL", "", QVariant::String, "Hosted Application URL");
  parser.addArgument("plugin", "", QVariant::String, "Plugin implementing the DicomAppInterface", "org_commontk_dah_exampleapp");
  parser.addArgument("help", "h", QVariant::Bool, "Show this help text");

  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(QCoreApplication::arguments(), &ok);
  if (!ok)
    {
    QTextStream(stderr, QIODevice::WriteOnly) << "Error parsing arguments: "
                                              << parser.errorString() << "\n";
    return EXIT_FAILURE;
    }

  // Show a help message
  if (parsedArgs.contains("help"))
    {
    print_usage();
    QTextStream(stdout, QIODevice::WriteOnly) << parser.helpText();
    return EXIT_SUCCESS;
    }

  if (parsedArgs.contains("hostURL") == false)
    {
    qCritical() << "Missing parameter hostURL.";
    print_usage();
    QTextStream(stdout, QIODevice::WriteOnly) << parser.helpText();
    return EXIT_FAILURE;
    }

  if (parsedArgs.contains("applicationURL") == false)
    {
    qCritical() << "Missing parameter hostURL.";
    print_usage();
    QTextStream(stdout, QIODevice::WriteOnly) << parser.helpText();
    return EXIT_FAILURE;
    }

  QString hostURL = parsedArgs.value("hostURL").toString();
  QString appURL = parsedArgs.value("applicationURL").toString();
  qDebug() << "appURL is: " << appURL << " . Extracted port is: " << QUrl(appURL).port();

  // Get the name of the plugin with the business logic
  // (thus the actual logic of the hosted app)
  QString pluginName = parsedArgs.value("plugin").toString();
  qCritical() << "  Plugin name: " << pluginName;

  ctkProperties fwProps;
  // pass further parameters the plugins
  if(parser.unparsedArguments().count() > 0)
    {
    QString args = parser.unparsedArguments().join(" ");
    fwProps.insert("dah.args", parser.unparsedArguments().join(" "));
    }

  // setup the plugin framework
  fwProps.insert("dah.hostURL", hostURL);
  fwProps.insert("dah.appURL", appURL);
  fwProps.insert(ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN, ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT);
  ctkPluginFrameworkFactory fwFactory(fwProps);
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
    return EXIT_FAILURE;
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
