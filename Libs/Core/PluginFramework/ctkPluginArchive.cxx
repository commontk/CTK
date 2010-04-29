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

  PluginArchive::PluginArchive(PluginStorage* pluginStorage, QIODevice* is,
                const QString& pluginLocation, int pluginId, const QString& resourcePrefix)
                  : autostartSetting(-1), id(pluginId), lastModified(0),
                  location(pluginLocation), resourcePrefix(resourcePrefix), storage(pluginStorage)
  {
    QString manifestResource = this->getPluginResource("META-INF/MANIFEST.MF");
    if (manifestResource.isNull())
    {
      throw PluginException(QString("Plugin has no MANIFEST.MF resource, location=") + pluginLocation);
    }
    QFile manifestFile(manifestResource);
    manifest.read(&manifestFile);
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

  QString PluginArchive::getPluginLocation() const
  {
    return location;
  }

  QString PluginArchive::getPluginResource(const QString& component) const
  {
    QString resourcePath = QString(":/") + resourcePrefix;
    if (component.startsWith('/')) resourcePath += component;
    else resourcePath += QString("/") + component;

    return storage->getPluginResource(resourcePath);
  }

  QStringList PluginArchive::findResourcesPath(const QString& path) const
  {
    QString resourcePath = QString(":/") + resourcePrefix;
    if (path.startsWith('/')) resourcePath += path;
    else resourcePath += QString("/") + path;

    return storage->findResourcesPath(resourcePath);
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
    //TODO
  }

  void PluginArchive::close()
  {
    //TODO
  }


}
