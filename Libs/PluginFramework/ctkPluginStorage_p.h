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

#include <QUrl>
#include <QStringList>
#include <QSharedPointer>

// CTK class forward declarations
class ctkPluginArchive;

/**
 * \ingroup PluginFramework
 *
 * Interface for managing all plugin meta-data and resources
 */
class ctkPluginStorage
{

public:

   virtual ~ctkPluginStorage() {}

  /**
   * Insert a plugin (shared library) into the persistent storage
   *
   * @param location Location of the plugin.
   * @param localPath Path to the plugin on the local file system
   * @return Plugin archive object.
   */
  virtual QSharedPointer<ctkPluginArchive> insertPlugin(const QUrl& location, const QString& localPath) = 0;

  /**
   * Insert a new plugin (shared library) into the persistent
   * storagedata as an update
   * to an existing plugin archive. To commit this data a call to
   * <code>replacePluginArchive</code> is needed.
   *
   * @param old ctkPluginArchive to be replaced.
   * @param updateLocation Location of the updated plugin.
   * @param localPath Path to a plugin on the local file system.
   * @return Plugin archive object.
   */
  virtual QSharedPointer<ctkPluginArchive> updatePluginArchive(QSharedPointer<ctkPluginArchive> old,
                                                               const QUrl& updateLocation, const QString& localPath) = 0;

  /**
   * Replace old plugin archive with a new updated plugin archive, that
   * was created with updatePluginArchive.
   *
   * @param oldPA ctkPluginArchive to be replaced.
   * @param newPA new ctkPluginArchive.
   */
  virtual void replacePluginArchive(QSharedPointer<ctkPluginArchive> oldPA, QSharedPointer<ctkPluginArchive> newPA) = 0;

  /**
   * Remove plugin archive from archives list and persistent storage.
   * The plugin archive is deleted and must not be used afterwards, if
   * this method returns \a true.
   *
   * @param pa Plugin archive to remove.
   * @return true if element was removed.
   */
  virtual bool removeArchive(QSharedPointer<ctkPluginArchive> pa) = 0;

  /**
   * Get all plugin archive objects.
   *
   * @return QList of all PluginArchives.
   */
  virtual QList<QSharedPointer<ctkPluginArchive> > getAllPluginArchives() const = 0;

  /**
   * Get all plugins to start at next launch of framework.
   * This list is sorted in increasing plugin id order.
   *
   * @return A List with plugin locations.
   */
  virtual QList<QString> getStartOnLaunchPlugins() const = 0;

  /**
   * Close this plugin storage and all bundles in it.
   */
  virtual void close() = 0;

};


#endif // CTKPLUGINSTORAGE_P_H
