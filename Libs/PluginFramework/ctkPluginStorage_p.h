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

#ifndef CTKPLUGINSTORAGE_P_H
#define CTKPLUGINSTORAGE_P_H

#include <QList>
#include <QStringList>

#include "ctkPluginDatabase_p.h"

// Qt class forward declarations
class QIODevice;


// CTK class forward declarations
class ctkPluginArchive;
class ctkPluginFrameworkContext;

/**
 * Storage of all plugin meta-data and resources
 */
class ctkPluginStorage
{

private:

  QMutex archivesLock;

  /**
   * Plugin id sorted list of all active plugin archives.
   */
  QList<ctkPluginArchive*> archives;

  /**
   * Framework handle.
   */
  ctkPluginFrameworkContext* framework;

  /**
   * SQLite db caching plug-in metadata and resources
   */
  ctkPluginDatabase pluginDatabase;

public:

  /**
   * Create a container for all plugin data in this framework.
   * Try to restore all saved plugin archive state.
   *
   */
  ctkPluginStorage(ctkPluginFrameworkContext* framework);


  /**
   * Insert a plugin (shared library) into the persistent storage
   *
   * @param location Location of the plugin.
   * @param localPath Path to the plugin on the local file system
   * @return Plugin archive object.
   */
  ctkPluginArchive* insertPlugin(const QUrl& location, const QString& localPath);


  /**
   * Insert a new plugin (shared library) into the persistent
   * storagedata as an update
   * to an existing plugin archive. To commit this data a call to
   * <code>replacePluginArchive</code> is needed.
   *
   * @param old ctkPluginArchive to be replaced.
   * @param localPath Path to a plugin on the local file system.
   * @return Plugin archive object.
   */
  ctkPluginArchive* updatePluginArchive(ctkPluginArchive* old, const QString& localPath);


  /**
   * Replace old plugin archive with a new updated plugin archive, that
   * was created with updatePluginArchive.
   *
   * @param oldPA ctkPluginArchive to be replaced.
   * @param newPA new ctkPluginArchive.
   */
  void replacePluginArchive(ctkPluginArchive* oldPA, ctkPluginArchive* newPA);

  /**
   * Persist the plugin start level.
   *
   * @param Plugin archive object
   */
  void setStartLevel(ctkPluginArchive* pa);

  /**
   * Persist the last modification (state change) time
   *
   * @param Plugin archive object
   */
  void setLastModified(ctkPluginArchive* pa);

  /**
   * Persist the auto start setting.
   *
   * @param Plugin archive object
   */
  void setAutostartSetting(ctkPluginArchive* pa);

  /**
   * Remove plugin archive from archives list and persistent storage.
   * The plugin archive is deleted and must not be used afterwards, if
   * this method returns \a true.
   *
   * @param pa Plugin archive to remove.
   * @return true if element was removed.
   */
  bool removeArchive(ctkPluginArchive* pa);


  /**
   * Get all plugin archive objects.
   *
   * @return QList of all PluginArchives.
   */
  QList<ctkPluginArchive*> getAllPluginArchives() const;


  /**
   * Get all plugins to start at next launch of framework.
   * This list is sorted in increasing plugin id order.
   *
   * @return A List with plugin locations.
   */
  QList<QString> getStartOnLaunchPlugins();

  QByteArray getPluginResource(long pluginId, const QString& res) const;

  QStringList findResourcesPath(long pluginId, const QString& path) const;

  /**
   * Close this plugin storage and all bundles in it.
   */
  void close();

  ~ctkPluginStorage();

};


#endif // CTKPLUGINSTORAGE_P_H
