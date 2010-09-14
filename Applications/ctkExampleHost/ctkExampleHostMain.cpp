/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QWidget>
#include <QUrl>

int main(int argv, char** argc)
{
  QApplication app(argv, argc);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkExampleHost");

  ctkPluginFrameworkFactory fwFactory;
  ctkPluginFramework* framework = fwFactory.getFramework();

  try {
    framework->init();
  }
  catch (const ctkPluginException& exc)
  {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    exit(1);
  }

#ifdef CMAKE_INTDIR
  QString pluginPath = qApp->applicationDirPath() + "/../plugins/" CMAKE_INTDIR "/";
#else
  QString pluginPath = qApp->applicationDirPath() + "/plugins/";
#endif

  qApp->addLibraryPath(pluginPath);

  QStringList libFilter;
  libFilter << "*.dll" << "*.so" << "*.dylib";
  QDirIterator dirIter(pluginPath, libFilter, QDir::Files);
  while(dirIter.hasNext())
  {
    try
    {
      QString fileLocation = dirIter.next();
      if (fileLocation.contains("org_commontk_dicom_wg23"))
      {
        ctkPlugin* plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(fileLocation));
        plugin->start(ctkPlugin::START_TRANSIENT);
      }
    }
    catch (const ctkPluginException& e)
    {
      qCritical() << e.what();
    }
  }

  framework->start();

  QWidget placeholder;
  placeholder.show();

  return app.exec();

}
