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

#include "ctkPluginFrameworkTestUtil.h"

#include <QFileInfo>
#include <QLibrary>
#include <QDirIterator>
#include <QDebug>

#include <ctkPluginContext.h>
#include <ctkPluginException.h>

//----------------------------------------------------------------------------
QSharedPointer<ctkPlugin> ctkPluginFrameworkTestUtil::installPlugin(
  ctkPluginContext* pc, const QString& plugin)
{
  qDebug() << "installPlugin(" << plugin << ")";
  QFileInfo pluginInfo(plugin);
  if (pluginInfo.exists() && pluginInfo.isFile() &&
      QLibrary::isLibrary(pluginInfo.absoluteFilePath()))
  {
    return pc->installPlugin(QUrl::fromLocalFile(pluginInfo.absoluteFilePath()));
  }
  else
  {
    QString testPluginDir = pc->getProperty("pluginfw.testDir").toString();
    QFileInfo testDirInfo(testPluginDir);
    if (!testDirInfo.exists() || !testDirInfo.isDir())
    {
      throw ctkPluginException(QString("No plugin %1 in %2").arg(plugin).arg(testPluginDir));
    }

    QStringList libFilter;
    libFilter << "*.dll" << "*.so" << "*.dylib";
    QDirIterator dirIter(testDirInfo.absoluteFilePath(), libFilter, QDir::Files);
    while (dirIter.hasNext())
    {
      QString lib = dirIter.next();
      if (dirIter.fileName().contains(plugin))
      {
        return pc->installPlugin(QUrl::fromLocalFile(lib));
      }
    }

    throw ctkPluginException(QString("No plugin %1 in %2").arg(plugin).arg(testPluginDir));
  }
}

