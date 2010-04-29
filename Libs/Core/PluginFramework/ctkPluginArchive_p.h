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

#ifndef CTKPLUGINARCHIVE_P_H
#define CTKPLUGINARCHIVE_P_H

#include <QString>
#include <QHash>

#include "ctkPluginManifest_p.h"

// Qt forward declarations
class QIODevice;

namespace ctk {

typedef quint64 qtimestamp;

// CTK foraward declarations
class PluginStorage;

/**
 * Class for managing plugin data.
 *
 */
class PluginArchive {

public:

  /**
   * Autostart setting stopped.
   * @see PluginArchiveInterface#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_STOPPED; // = "stopped"

  /**
   * Autostart setting eager.
   * @see PluginArchiveInterface#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_EAGER; // = "eager"

  /**
   * Autostart setting declared activation policy.
   * @see PluginArchiveInterface#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_ACTIVATION_POLICY; // = "activation_policy"

private:

  int autostartSetting;
  int id;
  qtimestamp lastModified;
  QString location;
  QString resourcePrefix;
  PluginManifest manifest;
  PluginStorage* storage;

public:

  /**
   * Construct new plugin archive.
   *
   */
  PluginArchive(PluginStorage* pluginStorage, QIODevice* is,
                const QString& pluginLocation, int pluginId,
                const QString& resourcePrefix);

  /**
   * Get an attribute from the manifest of a plugin.
   *
   * Not localized
   *
   * @param key Name of attribute to get.
   * @return A string with result or null if the entry doesn't exists.
   */
   QString getAttribute(const QString& key) const;

  /**
   * Gets all localization entries from this plugin. Will typically
   * read the file OSGI-INF/plugin_&lt;locale&gt;.properties.
   *
   * @param localeFile Filename within archive for localization properties.
   * @return null or a mapping of the entries.
   */
   QHash<QString,QString> getLocalizationEntries(const QString& localeFile) const;


  /**
   * @returns the (raw/unlocalized) attributes
   */
   QHash<QString,QString> getUnlocalizedAttributes() const;


  /**
   * Get plugin identifier for this plugin archive.
   *
   * @return Plugin identifier.
   */
   int getPluginId() const;


  /**
   * Get bundle location for this plugin archive.
   *
   * @return Bundle location.
   */
   QString getPluginLocation() const;


  /**
   * Get a Qt resource string to a named resource inside a plugin.
   *
   * @param component Entry to get reference to.
   * @return QString to the entry or null if it doesn't exist.
   */
   QString getPluginResource(const QString& component) const;


  /**
   * Returns a QStringList of all the paths
   * to entries within the plugin whose longest sub-path matches the supplied
   * path argument.
   *
   * @param name
   * @return
   */
   QStringList findResourcesPath(const QString& path) const;


  /**
   * Get stored plugin start level.
   */
   int getStartLevel() const;


  /**
   * Set stored plugin start level.
   */
   void setStartLevel(int level);


  /**
   * Get last modified timestamp.
   */
   qtimestamp getLastModified() const;


  /**
   * Set stored last modified timestamp.
   */
   void setLastModified(qtimestamp timemillisecs);


  /**
   * Get auto-start setting.
   *
   * @return the autostart setting. "-1" if the plugin is not started.
   */
   int getAutostartSetting() const;


  /**
   * Set the auto-start setting.
   *
   * @param setting the autostart setting to use.
   */
   void setAutostartSetting(int setting);


  /**
   * Get certificate chains associated with a plugin.
   *
   * @param onlyTrusted Only return trusted certificates.
   * @return All certificates or null if bundle is unsigned.
   */
  //QList<> getCertificateChains(bool onlyTrusted) const;


  /**
   * Mark certificate associated with the plugin as trusted.
   *
   */
  //void trustCertificateChain(QList<> trustedChain);


  /**
   * Remove plugin from persistent storage.
   */
   void purge();


  /**
   * Close archive and all its open files.
   */
   void close();

};

}


#endif // CTKPLUGINARCHIVE_P_H
