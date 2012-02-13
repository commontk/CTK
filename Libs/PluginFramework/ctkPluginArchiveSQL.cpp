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

#include "ctkPluginArchiveSQL_p.h"

#include "ctkPluginException.h"
#include "ctkPluginStorageSQL_p.h"
#include "ctkPluginDatabaseException.h"

#include <QStringList>
#include <QFile>


//----------------------------------------------------------------------------
ctkPluginArchiveSQL::ctkPluginArchiveSQL(ctkPluginStorageSQL* pluginStorage,
                                         const QUrl& pluginLocation, const QString& localPluginPath,
                                         int pluginId, int startLevel, const QDateTime& lastModified,
                                         int autostartSetting)
  : key(-1), autostartSetting(autostartSetting), id(pluginId), generation(0)
  , startLevel(startLevel), lastModified(lastModified), location(pluginLocation)
  , localPluginPath(localPluginPath), storage(pluginStorage)
{
}

//----------------------------------------------------------------------------
ctkPluginArchiveSQL::ctkPluginArchiveSQL(QSharedPointer<ctkPluginArchiveSQL> old, int generation,
                                         const QUrl &pluginLocation, const QString &localPluginPath)
  : key(-1), autostartSetting(old->autostartSetting), id(old->id), generation(generation)
  , startLevel(0), location(pluginLocation), localPluginPath(localPluginPath)
  , storage(old->storage)
{
}

void ctkPluginArchiveSQL::readManifest(const QByteArray& manifestResource)
{
  QByteArray manifestRes = manifestResource.isNull() ? this->getPluginResource("META-INF/MANIFEST.MF")
                                                  : manifestResource;
  if (manifestRes.isEmpty())
  {
    throw ctkPluginException(QString("ctkPlugin has no MANIFEST.MF resource, location=") + localPluginPath);
  }

  manifest.read(manifestRes);
}

//----------------------------------------------------------------------------
QString ctkPluginArchiveSQL::getAttribute(const QString& key) const
{
  return manifest.getAttribute(key);
}

//----------------------------------------------------------------------------
QHash<QString,QString> ctkPluginArchiveSQL::getUnlocalizedAttributes() const
{
  return manifest.getMainAttributes();
}

int ctkPluginArchiveSQL::getPluginGeneration() const
{
  return generation;
}

//----------------------------------------------------------------------------
int ctkPluginArchiveSQL::getPluginId() const
{
  return id;
}

//----------------------------------------------------------------------------
QUrl ctkPluginArchiveSQL::getPluginLocation() const
{
  return location;
}

//----------------------------------------------------------------------------
QString ctkPluginArchiveSQL::getLibLocation() const
{
  return localPluginPath;
}

//----------------------------------------------------------------------------
QByteArray ctkPluginArchiveSQL::getPluginResource(const QString& component) const
{
  try
  {
    return storage->getPluginResource(key, component);
  }
  catch (const ctkPluginDatabaseException& exc)
  {
    qDebug() << QString("Getting plugin resource %1 failed:").arg(component) << exc;
    return QByteArray();
  }
}

//----------------------------------------------------------------------------
QStringList ctkPluginArchiveSQL::findResourcesPath(const QString& path) const
{
  try
  {
    return storage->findResourcesPath(key, path);
  }
  catch (const ctkPluginDatabaseException& exc)
  {
    qDebug() << QString("Getting plugin resource paths for %1 failed:").arg(path) << exc;
  }
  return QStringList();
}

//----------------------------------------------------------------------------
int ctkPluginArchiveSQL::getStartLevel() const
{
  return startLevel;
}

//----------------------------------------------------------------------------
void ctkPluginArchiveSQL::setStartLevel(int level)
{
  if (startLevel != level)
  {
    startLevel = level;
    storage->setStartLevel(key, level);
  }
}

//----------------------------------------------------------------------------
QDateTime ctkPluginArchiveSQL::getLastModified() const
{
  return lastModified;
}

//----------------------------------------------------------------------------
void ctkPluginArchiveSQL::setLastModified(const QDateTime& dateTime)
{
  lastModified = dateTime;
  storage->setLastModified(key, dateTime);
}

//----------------------------------------------------------------------------
int ctkPluginArchiveSQL::getAutostartSetting() const
{
  return autostartSetting;
}

//----------------------------------------------------------------------------
void ctkPluginArchiveSQL::setAutostartSetting(int setting)
{
  if (autostartSetting != setting)
  {
    autostartSetting = setting;
    storage->setAutostartSetting(key, setting);
  }
}

//----------------------------------------------------------------------------
void ctkPluginArchiveSQL::purge()
{
  storage->removeArchive(this);
}

//----------------------------------------------------------------------------
void ctkPluginArchiveSQL::close()
{

}
