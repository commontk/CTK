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

#ifndef CTKPLUGINARCHIVESQL_P_H
#define CTKPLUGINARCHIVESQL_P_H

#include <QString>
#include <QHash>
#include <QUrl>
#include <QDateTime>

#include "ctkPluginArchive_p.h"
#include "ctkPluginManifest_p.h"

// CTK foraward declarations
class ctkPluginStorageSQL;

/**
 * \ingroup PluginFramework
 *
 * Class for managing plugin data.
 *
 */
class ctkPluginArchiveSQL : public ctkPluginArchive
{

public:

  /**
   * Construct new plugin archive.
   *
   */
  ctkPluginArchiveSQL(ctkPluginStorageSQL* pluginStorage, const QUrl& pluginLocation,
                      const QString& localPluginPath, int pluginId,
                      int startLevel = -1, const QDateTime &lastModified = QDateTime(),
                      int autostartSetting = -1);

  /**
   * Construct new bundle archive in an existing bundle archive.
   *
   */
  ctkPluginArchiveSQL(QSharedPointer<ctkPluginArchiveSQL> old, int generation,
                      const QUrl& pluginLocation, const QString& localPluginPath);


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
   * @returns the (raw/unlocalized) attributes
   */
  QHash<QString,QString> getUnlocalizedAttributes() const;

  /**
   * Get the plugin generation associated with this plugin archive.
   *
   * @return A integer representing the generation.
   */
  int getPluginGeneration() const;

  /**
   * Get plugin identifier for this plugin archive.
   *
   * @return ctkPlugin identifier.
   */
  int getPluginId() const;

  /**
   * Get plugin location for this plugin archive.
   *
   * @return Bundle location.
   */
  QUrl getPluginLocation() const;

  /**
   * Get the path to the plugin library on the local
   * file system
   *
   * @return Path to the plugin library
   */
  QString getLibLocation() const;


  /**
   * Get a Qt resource as a byte array from a plugin. The resource
   * is cached and may be aquired even if the plugin is not active.
   *
   * @param component Resource to get the byte array from.
   * @return QByteArray to the entry (empty if it doesn't exist).
   */
  QByteArray getPluginResource(const QString& component) const;


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
  QDateTime getLastModified() const;


  /**
   * Set stored last modified timestamp.
   */
  void setLastModified(const QDateTime& timemillisecs);


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
   * Remove plugin from persistent storage.
   * This will delete the current ctkPluginArchiveSQL instance.
   */
  void purge();

  /**
   * Close archive and all its open files.
   */
  void close();

  /**
   * Create a ctkPluginManifest using the Qt resource under META-INF/MANIFEST.MF
   */
  void readManifest(const QByteArray &manifestResource = QByteArray());

public:

  int key;

private:

  int autostartSetting;
  int id;
  int generation;
  int startLevel;
  QDateTime lastModified;
  QUrl location;
  QString localPluginPath;
  ctkPluginManifest manifest;
  ctkPluginStorageSQL* storage;

};



#endif // CTKPLUGINARCHIVESQL_P_H
