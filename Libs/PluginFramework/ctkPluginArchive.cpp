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

#include "ctkPluginArchive_p.h"

#include "ctkPluginException.h"
#include "ctkPluginStorage_p.h"

#include <QStringList>
#include <QFile>


const QString ctkPluginArchive::AUTOSTART_SETTING_STOPPED("stopped");
const QString ctkPluginArchive::AUTOSTART_SETTING_EAGER("eager");
const QString ctkPluginArchive::AUTOSTART_SETTING_ACTIVATION_POLICY("activation_policy");

ctkPluginArchive::ctkPluginArchive(ctkPluginStorage* pluginStorage,
                                   const QUrl& pluginLocation, const QString& localPluginPath,
                                   int pluginId)
                                     : autostartSetting(-1), id(pluginId), startLevel(-1),
                                     location(pluginLocation), localPluginPath(localPluginPath),
                                     storage(pluginStorage)
{
  QByteArray manifestResource = this->getPluginResource("META-INF/MANIFEST.MF");
  if (manifestResource.isEmpty())
  {
    throw ctkPluginException(QString("ctkPlugin has no MANIFEST.MF resource, location=") + pluginLocation.toString());
  }
  manifest.read(manifestResource);
}

QString ctkPluginArchive::getAttribute(const QString& key) const
{
  return manifest.getAttribute(key);
}

QHash<QString,QString> ctkPluginArchive::getUnlocalizedAttributes() const
{
  return manifest.getMainAttributes();
}

int ctkPluginArchive::getPluginId() const
{
  return id;
}

QUrl ctkPluginArchive::getPluginLocation() const
{
  return location;
}

QString ctkPluginArchive::getLibLocation() const
{
  return localPluginPath;
}

QByteArray ctkPluginArchive::getPluginResource(const QString& component) const
{
  return storage->getPluginResource(getPluginId(), component);
}

QStringList ctkPluginArchive::findResourcesPath(const QString& path) const
{
  return storage->findResourcesPath(getPluginId(), path);
}

int ctkPluginArchive::getStartLevel() const
{
  return startLevel;
}

void ctkPluginArchive::setStartLevel(int level)
{
  if (startLevel != level)
  {
    startLevel = level;
    storage->setStartLevel(this);
  }
}

QDateTime ctkPluginArchive::getLastModified() const
{
  return lastModified;
}

void ctkPluginArchive::setLastModified(const QDateTime& dateTime)
{
  lastModified = dateTime;
  storage->setLastModified(this);
}

int ctkPluginArchive::getAutostartSetting() const
{
  return autostartSetting;
}

void ctkPluginArchive::setAutostartSetting(int setting)
{
  if (autostartSetting != setting)
  {
    autostartSetting = setting;
    storage->setAutostartSetting(this);
  }
}

void ctkPluginArchive::purge()
{
  storage->removeArchive(this);
}
