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

#ifndef CTKPLUGINDATABASE_P_H
#define CTKPLUGINDATABASE_P_H

#include <QtSql>
#include <QList>

// CTK class forward declarations
class ctkPluginStorage;
class ctkPluginArchive;

class ctkPluginDatabase
{

public:

  ctkPluginDatabase(ctkPluginStorage* storage);

  virtual ~ctkPluginDatabase();

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
   * Closes the plugin database. Throws a ctkPluginDatabaseException
   * of type DB_CONNECTION_INVALID if the database is invalid.
   *
   * @throws ctkPluginDatabaseException
   */
  void close();

  /**
   * Checks if the database is open
   */
  bool isOpen() const;

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
  QByteArray getPluginResource(long pluginId, const QString& res) const;

  /**
   * Get a list of resource entries under the given path.
   *
   * @param pluginId The id of the plugin from which to get the entries
   * @param path A resource path relative to the plugin specific resource prefix.
   * @return A QStringList containing the cached resource entries.
   *
   * @throws ctkPluginDatabaseException
   */
  QStringList findResourcesPath(long pluginId, const QString& path) const;

  /**
   * Inserts a new plugin into the database. This method assumes that
   * the an entry with the same \a location and \a localPath does not
   * yet exist in the database.
   *
   * @param location The URL to the plugin.
   * @param localPath The path to the plugin library on the local file system.
   * @param createArchive If \c true (default) a new ctkPluginArchive instance is returned.
   *
   * @throws ctkPluginDatabaseException
   */
  ctkPluginArchive* insertPlugin(const QUrl& location, const QString& localPath, bool createArchive = true);

  /**
   * Removes all persisted data related to the given ctkPluginArchive.
   *
   * @throws ctkPluginDatabaseException
   */
  void removeArchive(const ctkPluginArchive* pa);

  /**
   * Persist the start level
   *
   * @param pluginId The Plugin id
   * @param startLevel The new start level
   */
  void setStartLevel(long pluginId, int startLevel);

  /**
   * Persist the last modification (state change) time
   *
   * @param pluginId The Plugin id
   * @param lastModified The modification time
   */
  void setLastModified(long pluginId, const QDateTime& lastModified);

  /**
   * Persist the auto start setting.
   *
   * @param pluginId The Plugin id
   * @param autostart The new auto start setting
   */
  void setAutostartSetting(long pluginId, int autostart);

  /**
   * Reads the persisted plugin data and returns a ctkPluginArchive object
   * for each plugin which is not in state UNINSTALLED.
   *
   * @throws ctkPluginDatabaseException
   */
  QList<ctkPluginArchive*> getPluginArchives() const;


private:

  enum TransactionType{Read, Write};

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
  void removeUninstalledPlugins();

  /**
   * Helper method that checks if all the expected tables exist in the database.
   *
   * Returns true if they all exist and false if any of them don't
   */
  bool checkTables() const;

  /**
   * Checks the database connection.
   *
   * @throws ctkPluginDatabaseException
   */
  void checkConnection() const;

  /**
   * Compares the persisted plugin modification time with the
   * file system modification time and updates the database
   * if the persisted data is outdated.
   *
   * This should only be called once when the database is initially opened.
   */
  void updateDB();

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
  QString m_connectionName;
  bool m_isDatabaseOpen;
  bool m_inTransaction;
  ctkPluginStorage* m_PluginStorage;
};


#endif // CTKPLUGINDATABASE_P_H
