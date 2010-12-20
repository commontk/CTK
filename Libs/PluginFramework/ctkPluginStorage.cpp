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

#include "ctkPluginStorage_p.h"

#include <QPluginLoader>
#include <QFileInfo>
#include <QUrl>
#include <QDir>

// CTK includes
#include "ctkPluginArchive_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkPluginDatabaseException.h"


ctkPluginStorage::ctkPluginStorage(ctkPluginFrameworkContext* framework)
  : framework(framework), pluginDatabase(this)
{
  // See if we have a storage database
  QString path = ctkPluginFrameworkUtil::getFileStorage(framework, "").absoluteFilePath("plugins.db");

  pluginDatabase.setDatabasePath(path);
  pluginDatabase.open();
  archives << pluginDatabase.getPluginArchives();
}

ctkPluginArchive* ctkPluginStorage::insertPlugin(const QUrl& location, const QString& localPath)
{
  ctkPluginArchive* pa = pluginDatabase.insertPlugin(location, localPath);
  archives.push_back(pa);
  return pa;
}

ctkPluginArchive* ctkPluginStorage::updatePluginArchive(ctkPluginArchive* old, const QString& localPath)
{
  Q_UNUSED(old)
  Q_UNUSED(localPath)
  //TODO: updatePluginArchive
  //return new BundleArchiveImpl((BundleArchiveImpl)old, is);
  return 0;
}

void ctkPluginStorage::replacePluginArchive(ctkPluginArchive* oldPA, ctkPluginArchive* newPA)
{
  Q_UNUSED(oldPA)
  Q_UNUSED(newPA)
  //TODO: replacePluginArchive
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

void ctkPluginStorage::setStartLevel(ctkPluginArchive* pa)
{
  pluginDatabase.setStartLevel(pa->getPluginId(), pa->getStartLevel());
}

void ctkPluginStorage::setLastModified(ctkPluginArchive* pa)
{
  pluginDatabase.setLastModified(pa->getPluginId(), pa->getLastModified());
}

void ctkPluginStorage::setAutostartSetting(ctkPluginArchive* pa)
{
  pluginDatabase.setAutostartSetting(pa->getPluginId(), pa->getAutostartSetting());
}

QList<ctkPluginArchive*> ctkPluginStorage::getAllPluginArchives() const
{
  return archives;
}

QList<QString> ctkPluginStorage::getStartOnLaunchPlugins()
{
  QList<QString> res;
  QListIterator<ctkPluginArchive*> i(archives);
  while(i.hasNext())
  {
    ctkPluginArchive* pa = i.next();
    if (pa->getAutostartSetting() != -1)
    {
      res.push_back(pa->getPluginLocation().toString());
    }
  }
  return res;
}

ctkPluginStorage::~ctkPluginStorage()
{
  close();
  // ctkPluginArchive pointers in archives list are deleted
  // in ~ctkPluginPrivate()
}

void ctkPluginStorage::close()
{
  pluginDatabase.close();
}

bool ctkPluginStorage::removeArchive(ctkPluginArchive* pa)
{
  QMutexLocker lock(&archivesLock);

  bool removed = false;
  try
  {
    pluginDatabase.removeArchive(pa);
    removed = archives.removeAll(pa);
  }
  catch (const ctkPluginDatabaseException& exc)
  {
    qDebug() << "Removing plugin archive failed:" << exc;
    removed = false;
  }

  return removed;
}

QByteArray ctkPluginStorage::getPluginResource(long pluginId, const QString& res) const
{
  try
  {
    return pluginDatabase.getPluginResource(pluginId, res);
  }
  catch (const ctkPluginDatabaseException& exc)
  {
    qDebug() << QString("Getting plugin resource %1 failed:").arg(res) << exc;
    return QByteArray();
  }
}

QStringList ctkPluginStorage::findResourcesPath(long pluginId, const QString& path) const
{
  try
  {
    return pluginDatabase.findResourcesPath(pluginId, path);
  }
  catch (const ctkPluginDatabaseException& exc)
  {
    qDebug() << QString("Getting plugin resource paths for %1 failed:").arg(path) << exc;
    return QStringList();
  }
}
