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

#include "ctkPluginArchive_p.h"

#include "ctkPluginException.h"
#include "ctkPluginStorage_p.h"

#include <QStringList>
#include <QFile>

namespace ctk {

  const QString PluginArchive::AUTOSTART_SETTING_STOPPED("stopped");
  const QString PluginArchive::AUTOSTART_SETTING_EAGER("eager");
  const QString PluginArchive::AUTOSTART_SETTING_ACTIVATION_POLICY("activation_policy");

  PluginArchive::PluginArchive(PluginStorage* pluginStorage,
                const QUrl& pluginLocation, const QString& localPluginPath,
                int pluginId)
                  : autostartSetting(-1), id(pluginId), lastModified(0),
                  location(pluginLocation), localPluginPath(localPluginPath),
                  storage(pluginStorage)
  {
    QByteArray manifestResource = this->getPluginResource("META-INF/MANIFEST.MF");
    if (manifestResource.isEmpty())
    {
      throw PluginException(QString("Plugin has no MANIFEST.MF resource, location=") + pluginLocation.toString());
    }
    manifest.read(manifestResource);
  }

  QString PluginArchive::getAttribute(const QString& key) const
  {
    return manifest.getAttribute(key);
  }

  QHash<QString,QString> PluginArchive::getLocalizationEntries(const QString& localeFile) const
  {
    //TODO
    return QHash<QString,QString>();
  }

  QHash<QString,QString> PluginArchive::getUnlocalizedAttributes() const
  {
    //TODO
    return QHash<QString,QString>();
  }

  int PluginArchive::getPluginId() const
  {
    return id;
  }

  QUrl PluginArchive::getPluginLocation() const
  {
    return location;
  }

  QString PluginArchive::getLibLocation() const
  {
    return localPluginPath;
  }

  QByteArray PluginArchive::getPluginResource(const QString& component) const
  {
    return storage->getPluginResource(getPluginId(), component);
  }

  QStringList PluginArchive::findResourcesPath(const QString& path) const
  {
    return storage->findResourcesPath(getPluginId(), path);
  }

  int PluginArchive::getStartLevel() const
  {
    //TODO
    return 0;
  }

  void PluginArchive::setStartLevel(int level)
  {
    //TODO
//    if (startLevel != level)
//    {
//      startLevel = level;
//      putContent(...);
//    }
  }

  qtimestamp PluginArchive::getLastModified() const
  {
    return lastModified;
  }

  void PluginArchive::setLastModified(qtimestamp clockticks)
  {
    lastModified = clockticks;
    //TDOO
    //putContent(...)
  }

  int PluginArchive::getAutostartSetting() const
  {
    return autostartSetting;
  }

  void PluginArchive::setAutostartSetting(int setting)
  {
    if (autostartSetting != setting)
    {
      autostartSetting = setting;
      //TODO
      //putContent(...)
    }
  }

  void PluginArchive::purge()
  {
    storage->removeArchive(this);
  }


}
