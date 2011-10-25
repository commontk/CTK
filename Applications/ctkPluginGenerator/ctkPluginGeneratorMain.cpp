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

#include <ctkConfig.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginGeneratorConstants.h>
#include <ctkPluginContext.h>

#include "ctkPluginGenerator_p.h"

#include <QApplication>
#include <QSettings>
#include <QDirIterator>
#include <QInputDialog>
#include <QDebug>

int main(int argv, char** argc)
{
  QApplication app(argv, argc);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkPluginGenerator");

  // init global template defaults
  QSettings settings;
  if (!settings.contains(ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER))
  {
    QFile license(":/generatordefaults/license.txt");
    license.open(QIODevice::ReadOnly);
    QString licenseText = license.readAll();
    bool ok;
    QString organization = QInputDialog::getText(0, qApp->translate("OrganizationInputDialog", "CTK Plugin Generator"),
                                                 qApp->translate("OrganizationInputDialog", "Enter the name of your organization:"),
                                                 QLineEdit::Normal, qApp->translate("OrganizationInputDialog", "<your-organization>"), &ok);
    if (!ok)
    {
      exit(0);
    }
    organization.replace("\\n", "\n");
    settings.setValue(ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER, licenseText.arg(organization));
  }

  ctkPluginFrameworkFactory fwFactory;
  QSharedPointer<ctkPluginFramework> framework = fwFactory.getFramework();

  try {
    framework->init();
  }
  catch (const ctkPluginException& exc)
  {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    exit(1);
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
  while(dirIter.hasNext())
  {
    try
    {
      QString fileLocation = dirIter.next();
      if (fileLocation.contains("org_commontk_plugingenerator"))
      {
        QSharedPointer<ctkPlugin> plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(fileLocation));
        plugin->start(ctkPlugin::START_TRANSIENT);
      }
    }
    catch (const ctkPluginException& e)
    {
      qCritical() << e.what();
    }
  }

  framework->start();

  ctkPluginGenerator generator(framework.data());
  generator.show();

  return app.exec();

}
