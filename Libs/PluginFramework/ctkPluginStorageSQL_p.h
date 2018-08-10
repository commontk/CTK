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

#ifndef ctkPluginStorageSQL_P_H
#define ctkPluginStorageSQL_P_H

#include "ctkPluginStorage_p.h"

#include <QMutex>
#include <QLibrary>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QPluginLoader>
#include <QDirIterator>
#include <QThreadStorage>

// CTK class forward declarations
class ctkPluginFrameworkContext;
class ctkPluginArchiveSQL;

/**
 * \ingroup PluginFramework
 */
class ctkPluginStorageSQL : public ctkPluginStorage
{

public:

  /**
   * Create a container for all plugin data in this framework.
   * Try to restore all saved plugin archive state.
   *
   */
  ctkPluginStorageSQL(ctkPluginFrameworkContext* framework);

  virtual ~ctkPluginStorageSQL();

  /**
   * Inserts a new plugin into the database. This method assumes that
   * the an entry with the same \a location and \a localPath does not
   * yet exist in the database.
   *
   * @param location The URL to the plugin.
   * @param updateLocation Location of the updated plugin.
   * @param localPath The path to the plugin library on the local file system.
   * @param createArchive If \c true (default) a new ctkPluginArchive instance is returned.
   *
   * @throws ctkPluginDatabaseException
   */
  QSharedPointer<ctkPluginArchive> insertPlugin(const QUrl& location, const QString& localPath);

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
  QSharedPointer<ctkPluginArchive> updatePluginArchive(QSharedPointer<ctkPluginArchive> old,
                                                       const QUrl& updateLocation, const QString& localPath);

  /**
   * Replace old plugin archive with a new updated plugin archive, that
   * was created with updatePluginArchive.
   *
   * @param oldPA ctkPluginArchive to be replaced.
   * @param newPA new ctkPluginArchive.
   */
  void replacePluginArchive(QSharedPointer<ctkPluginArchive> oldPA, QSharedPointer<ctkPluginArchive> newPA);

  /**
   * Removes all persisted data related to the given ctkPluginArchive.
   *
   * @throws ctkPluginDatabaseException
   */
  bool removeArchive(QSharedPointer<ctkPluginArchive> pa);

  /**
   * Get all plugin archive objects.
   *
   * @return QList of all PluginArchives.
   */
  QList<QSharedPointer<ctkPluginArchive> > getAllPluginArchives() const;

  /**
   * Get all plugins to start at next launch of framework.
   * This list is sorted in increasing plugin id order.
   *
   * @return A List with plugin locations.
   */
  QList<QString> getStartOnLaunchPlugins() const;

  /**
   * Closes the plugin database. Throws a ctkPluginDatabaseException
   * of type DB_CONNECTION_INVALID if the database is invalid.
   *
   * @throws ctkPluginDatabaseException
   */
  void close(); // Satisfy abstract interface
  void close() const;

  // -------------------------------------------------------------
  // end ctkPluginStorage interface
  // -------------------------------------------------------------

  /**
   * Sets the path of the service database to \a databasePath
   */
  void setDatabasePath(const QString &databasePath);

  /**
   * Returns the path of the plugin database
   */
  QString getDatabasePath() const;

  /**
   * Get a Qt resource cached in the database. The resource path \a res
   * must be relative to the plugin specific resource prefix, but may
   * start with a '/'.
   *
   * @param pluginId The id of the plugin from which to get the resource
   * @param res The path to the resource in the plugin
   * @return The byte array of the cached resource
   *
   * @throws ctkPluginDatabaseException
   */
  QByteArray getPluginResource(int key, const QString& res) const;

  /**
   * Get a list of resource entries under the given path.
   *
   * @param pluginId The id of the plugin from which to get the entries
   * @param path A resource path relative to the plugin specific resource prefix.
   * @return A QStringList containing the cached resource entries.
   *
   * @throws ctkPluginDatabaseException
   */
  QStringList findResourcesPath(int archiveKey, const QString& path) const;

  /**
   * Persist the start level
   *
   * @param pluginId The Plugin id
   * @param startLevel The new start level
   */
  void setStartLevel(int key, int startLevel);

  /**
   * Persist the last modification (state change) time
   *
   * @param pluginId The Plugin id
   * @param lastModified The modification time
   */
  void setLastModified(int key, const QDateTime& lastModified);

  /**
   * Persist the auto start setting.
   *
   * @param pluginId The Plugin id
   * @param autostart The new auto start setting
   */
  void setAutostartSetting(int key, int autostart);

  /**
   * Removes all persisted data related to the given ctkPluginArchiveSQL.
   * This is identical to removeArchive(QSharedPointer<ctkPluginArchive>).
   *
   * @throws ctkPluginDatabaseException
   */
  bool removeArchive(ctkPluginArchiveSQL* pa);

private:

  enum TransactionType{Read, Write};

  /**
   * Opens the plugin database. If the database does not
   * yet exist, it is created using the path from getDatabasePath().
   *
   * @see setDatabasePath(const QString&)
   * @see getDatabasePath()
   * @see ctkPluginDatabaseException
   *
   * @throws ctkPluginDatabaseException
   */
  void open();

  /**
   * Checks if the database is open
   */
  bool isOpen() const;

  /**
   * Find position for ctkPluginArchive with specified id
   *
   * @param id Plugin archive id to find.
   * @return Position in the m_archives List.
   */
  int find(long id) const;

  /**
   * Find position for ctkPluginArchive
   *
   * @param id Plugin archive id to find.
   * @return Position in the m_archives List.
   */
  int find(ctkPluginArchive* pa) const;

  void initNextFreeIds();

  /**
   * Reads the persisted plugin data and creates a ctkPluginArchive object
   * for each plugin which is not in state UNINSTALLED.
   *
   * @throws ctkPluginDatabaseException
   */
  void restorePluginArchives();
  
  /**
   * Get load hints from the framework for plugins.
   */
  QLibrary::LoadHints getPluginLoadHints() const;

  /**
   *  Helper method that creates the database tables:
   *
   * @throws ctkPluginDatabaseException
   */
  void createTables();
  bool dropTables();

  /**
   * Remove all plugins which have been marked as uninstalled
   * (startLevel == -2).
   */
  void cleanupDB();

  /**
   * Helper method that checks if all the expected tables exist in the database.
   *
   * Returns true if they all exist and false if any of them don't
   */
  bool checkTables() const;

  /**
   * Creates or returns an existing, thread-local database connection.
   *
   * @param open Create and open connection.
   * @return Database connection.
   * @throws ctkPluginDatabaseException
   */
  QSqlDatabase getConnection(bool create = true) const;

  /**
   * Creates a thread-unique database connection name.
   *
   * @return Database connection name.
   */
  QString getConnectionName() const;

  /**
   * Creates the directory for the database.
   *
   * @throws ctkPluginDatabaseException
   */
  void createDatabaseDirectory() const;

  /**
   * Compares the persisted plugin modification time with the
   * file system modification time and updates the database
   * if the persisted data is outdated.
   *
   * This should only be called once when the database is initially opened.
   */
  void updateDB();

  void insertArchive(QSharedPointer<ctkPluginArchiveSQL> pa);

  void insertArchive(QSharedPointer<ctkPluginArchiveSQL> pa, QSqlQuery* query);

  void removeArchiveFromDB(ctkPluginArchiveSQL *pa, QSqlQuery *query);

  /**
   * Helper function that executes the sql query specified in \a statement.
   * It is assumed that the \a statement uses positional placeholders and
   * corresponding parameters are placed in the list of \a bindValues.
   *
   * Aside: This function may be safely called standalone or within an explicit
   * transaction.  If called standalone, it's single query is implicitly
   * wrapped in it's own transaction.
   *
   * @throws ctkPluginDatabaseException
   */
  void executeQuery(QSqlQuery* query, const QString &statement, const QList<QVariant> &bindValues = QList<QVariant>()) const;

  /**
   * Begins a transcaction based on the \a type which can be Read or Write.
   *
   * @throws ctkPluginDatabaseException
   */
  void beginTransaction(QSqlQuery* query, TransactionType);

  /**
   * Commits a transaction
   *
   * @throws ctkPluginDatabaseException
   */
  void commitTransaction(QSqlQuery* query);

  /**
   * Rolls back a transaction
   *
   * @throws ctkPluginDatabaseException
   */
  void rollbackTransaction(QSqlQuery* query);

  /**
   * Returns a string representation of a QDateTime instance.
   */
  QString getStringFromQDateTime(const QDateTime& dateTime) const;

  /**
   * Returns a QDateTime from a string representation.
   */
  QDateTime getQDateTimeFromString(const QString& dateTimeString) const;


  QString m_databasePath;
  mutable QThreadStorage<QString> m_connectionNames;

  QMutex m_archivesLock;

  /**
   * Plugin id sorted list of all active plugin archives.
   */
  QList<QSharedPointer<ctkPluginArchive> > m_archives;

  /**
   * Framework handle.
   */
  ctkPluginFrameworkContext* m_framework;

  /**
   * Keep track of the next free plug-in id
   */
  long m_nextFreeId;

  /**
   * Keep track of the next free generation for each plugin
   */
  QHash<int,int> /* <plugin id, generation> */ m_generations;
};


#endif // ctkPluginStorageSQL_P_H
