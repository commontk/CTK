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

#include "ctkPluginStorage_p.h"

#include <QPluginLoader>
#include <QFileInfo>
#include <QUrl>
#include <QDir>

// CTK includes
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginDatabaseException.h"


  PluginStorage::PluginStorage(PluginFrameworkContext* framework)
    : framework(framework), pluginDatabase(this)
  {
//    // See if we have a storage database
//    bundlesDir = Util.getFileStorage(framework, "bs");
//    if (bundlesDir == null) {
//      throw RuntimeException("No plugin storage area available!");
//    }

    pluginDatabase.open();
    archives << pluginDatabase.getPluginArchives();
  }

  PluginArchive* PluginStorage::insertPlugin(const QUrl& location, const QString& localPath)
  {
    PluginArchive* pa = pluginDatabase.insertPlugin(location, localPath);
    archives.push_back(pa);
    return pa;
  }

  PluginArchive* PluginStorage::updatePluginArchive(PluginArchive* old, const QString& localPath)
  {
    //return new BundleArchiveImpl((BundleArchiveImpl)old, is);
    return 0;
  }

  void PluginStorage::replacePluginArchive(PluginArchive* oldPA, PluginArchive* newPA)
  {
//    int pos;
//    long id = oldBA.getBundleId();
//    synchronized (archives) {
//      pos = find(id);
//      if (pos >= archives.size() || archives.get(pos) != oldBA) {
//        throw new Exception("replaceBundleJar: Old bundle archive not found, pos=" + pos);
//      }
//      archives.set(pos, newBA);
//    }
  }

  QList<PluginArchive*> PluginStorage::getAllPluginArchives() const
  {
    return archives;
  }

  QList<QString> PluginStorage::getStartOnLaunchPlugins()
  {
    QList<QString> res;
    QListIterator<PluginArchive*> i(archives);
    while(i.hasNext())
    {
      PluginArchive* pa = i.next();
      if (pa->getAutostartSetting() != -1)
      {
        res.push_back(pa->getPluginLocation().toString());
      }
    }
    return res;
  }

  PluginStorage::~PluginStorage()
  {
    close();
  }

  void PluginStorage::close()
  {
    pluginDatabase.close();
    qDeleteAll(archives);
  }

  bool PluginStorage::removeArchive(PluginArchive* pa)
  {
    QMutexLocker lock(&archivesLock);

    bool removed = false;
    try
    {
      pluginDatabase.removeArchive(pa);
      removed = archives.removeAll(pa);
      delete pa;
    }
    catch (const PluginDatabaseException& exc)
    {
      qDebug() << "Removing plugin archive failed:" << exc;
      removed = false;
    }

    return removed;
  }

  QByteArray PluginStorage::getPluginResource(long pluginId, const QString& res) const
  {
    try
    {
      return pluginDatabase.getPluginResource(pluginId, res);
    }
    catch (const PluginDatabaseException& exc)
    {
      qDebug() << QString("Getting plugin resource %1 failed:").arg(res) << exc;
      return QByteArray();
    }
  }

  QStringList PluginStorage::findResourcesPath(long pluginId, const QString& path) const
  {
    try
    {
      return pluginDatabase.findResourcesPath(pluginId, path);
    }
    catch (const PluginDatabaseException& exc)
    {
      qDebug() << QString("Getting plugin resource paths for %1 failed:").arg(path) << exc;
      return QStringList();
    }

}
