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

#include "ctkPluginDatabase_p.h"
#include "ctkPluginDatabaseException.h"
#include "ctkPlugin.h"
#include "ctkPluginConstants.h"
#include "ctkPluginException.h"
#include "ctkPluginArchive_p.h"
#include "ctkPluginStorage_p.h"
#include "ctkPluginFrameworkUtil_p.h"
#include "ctkServiceException.h"

#include <QApplication>
#include <QFileInfo>
#include <QUrl>

#include <QDebug>

//database table names
#define PLUGINS_TABLE "Plugins"
#define PLUGIN_RESOURCES_TABLE "PluginResources"


enum TBindIndexes
{
  EBindIndex=0,
  EBindIndex1,
  EBindIndex2,
  EBindIndex3,
  EBindIndex4,
  EBindIndex5,
  EBindIndex6,
  EBindIndex7
};

ctkPluginDatabase::ctkPluginDatabase(ctkPluginStorage* storage)
:m_isDatabaseOpen(false), m_inTransaction(false), m_PluginStorage(storage)
{
}

ctkPluginDatabase::~ctkPluginDatabase()
{
    close();
}

void ctkPluginDatabase::open()
{
  if (m_isDatabaseOpen)
    return;

  QString path;

  //Create full path to database
  if(m_databasePath.isEmpty ())
      m_databasePath = getDatabasePath();

  path = m_databasePath;
  QFileInfo dbFileInfo(path);
  if (!dbFileInfo.dir().exists())
  {
    if(!QDir::root().mkpath(dbFileInfo.path()))
    {
      close();
      QString errorText("Could not create database directory: %1");
      throw ctkPluginDatabaseException(errorText.arg(dbFileInfo.path()), ctkPluginDatabaseException::DB_CREATE_DIR_ERROR);
    }
  }

  m_connectionName = dbFileInfo.completeBaseName();
  QSqlDatabase database;
  if (QSqlDatabase::contains(m_connectionName))
  {
    database = QSqlDatabase::database(m_connectionName);
  }
  else
  {
    database = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    database.setDatabaseName(path);
  }

  if (!database.isValid())
  {
    close();
    throw ctkPluginDatabaseException(QString("Invalid database connection: %1").arg(m_connectionName),
                                  ctkPluginDatabaseException::DB_CONNECTION_INVALID);
  }

  //Create or open database
  if (!database.isOpen())
  {
    if (!database.open())
    {
      close();
      throw ctkPluginDatabaseException(QString("Could not open database. ") + database.lastError().text(),
                                    ctkPluginDatabaseException::DB_SQL_ERROR);
    }
  }
  m_isDatabaseOpen = true;

  //Check if the sqlite version supports foreign key constraints
  QSqlQuery query(database);
  if (!query.exec("PRAGMA foreign_keys"))
  {
    close();
    throw ctkPluginDatabaseException(QString("Check for foreign key support failed."),
                                  ctkPluginDatabaseException::DB_SQL_ERROR);
  }

  if (!query.next())
  {
    close();
    throw ctkPluginDatabaseException(QString("SQLite db does not support foreign keys. It is either older than 3.6.19 or was compiled with SQLITE_OMIT_FOREIGN_KEY or SQLITE_OMIT_TRIGGER"),
                                  ctkPluginDatabaseException::DB_SQL_ERROR);
  }
  query.finish();
  query.clear();

  //Enable foreign key support
  if (!query.exec("PRAGMA foreign_keys = ON"))
  {
    close();
    throw ctkPluginDatabaseException(QString("Enabling foreign key support failed."),
                                  ctkPluginDatabaseException::DB_SQL_ERROR);
  }
  query.finish();


  //Check database structure (tables) and recreate tables if neccessary
  //If one of the tables is missing remove all tables and recreate them
  //This operation is required in order to avoid data coruption
  if (!checkTables())
  {
    if (dropTables())
    {
      createTables();
    }
    else
    {
      //dropTables() should've handled error message
      //and warning
      close();
    }
  }

  removeUninstalledPlugins();

  //Update database based on the recorded timestamps
  updateDB();
}

void ctkPluginDatabase::removeUninstalledPlugins()
{
  checkConnection();

  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  beginTransaction(&query, Write);

  try
  {
    QString statement = "DELETE FROM Plugins WHERE StartLevel==-2";
    executeQuery(&query, statement);
  }
  catch (...)
  {
    rollbackTransaction(&query);
    throw;
  }

  commitTransaction(&query);
}

void ctkPluginDatabase::updateDB()
{
  checkConnection();

  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  beginTransaction(&query, Write);

  QString statement = "SELECT ID, Location, LocalPath, Timestamp, SymbolicName, Version FROM Plugins WHERE State != ?";
  QList<QVariant> bindValues;
  bindValues.append(ctkPlugin::UNINSTALLED);

  QList<qlonglong> outdatedIds;
  QList<QPair<QString,QString> > outdatedPlugins;
  QStringList outdatedServiceNames;
  try
  {
    executeQuery(&query, statement, bindValues);

    while (query.next())
    {
      QFileInfo pluginInfo(query.value(EBindIndex2).toString());
      if (pluginInfo.lastModified() > getQDateTimeFromString(query.value(EBindIndex3).toString()))
      {
        outdatedIds.append(query.value(EBindIndex).toLongLong());
        outdatedPlugins.append(qMakePair(query.value(EBindIndex1).toString(), query.value(EBindIndex2).toString()));
      }
    }
  }
  catch (...)
  {
    rollbackTransaction(&query);
    throw;
  }

  query.finish();
  query.clear();

  try
  {
    statement = "DELETE FROM Plugins WHERE ID=?";
    QListIterator<qlonglong> idIter(outdatedIds);
    while (idIter.hasNext())
    {
      bindValues.clear();
      bindValues.append(idIter.next());
      executeQuery(&query, statement, bindValues);
    }
  }
  catch (...)
  {
    rollbackTransaction(&query);
    throw;
  }

  commitTransaction(&query);

  QListIterator<QPair<QString,QString> > locationIter(outdatedPlugins);
  while (locationIter.hasNext())
  {
    const QPair<QString,QString>& locations = locationIter.next();
    insertPlugin(QUrl(locations.first), locations.second, false);
  }
}


ctkPluginArchive* ctkPluginDatabase::insertPlugin(const QUrl& location, const QString& localPath, bool createArchive)
{
  checkConnection();

  // Assemble the data for the sql record
  QFileInfo fileInfo(localPath);
  if (!fileInfo.exists())
  {
    throw std::invalid_argument((localPath + " does not exist").toStdString());
  }

  const QString libTimestamp = getStringFromQDateTime(fileInfo.lastModified());

  QString resourcePrefix = fileInfo.baseName();
  if (resourcePrefix.startsWith("lib"))
  {
    resourcePrefix = resourcePrefix.mid(3);
  }
  resourcePrefix.replace("_", ".");

  resourcePrefix = QString(":/") + resourcePrefix + "/";

  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  beginTransaction(&query, Write);

  QString statement = "INSERT INTO Plugins(Location,LocalPath,SymbolicName,Version,State,LastModified,Timestamp,StartLevel,AutoStart)"
      "VALUES(?,?,?,?,?,'',?,-1,-1)";

  QList<QVariant> bindValues;
  bindValues.append(location.toString());
  bindValues.append(localPath);
  bindValues.append(QString("na"));
  bindValues.append(QString("na"));
  bindValues.append(ctkPlugin::INSTALLED);
  bindValues.append(libTimestamp);

  qlonglong pluginId = -1;
  try
  {
    executeQuery(&query, statement, bindValues);
    QVariant lastId = query.lastInsertId();
    if (lastId.isValid())
    {
      pluginId = lastId.toLongLong();
    }
  }
  catch (...)
  {
    rollbackTransaction(&query);
    throw;
  }

  // Load the plugin and cache the resources
  QPluginLoader pluginLoader;
  pluginLoader.setFileName(localPath);
  if (!pluginLoader.load())
  {
    rollbackTransaction(&query);
    ctkPluginException exc(QString("The plugin could not be loaded: %1").arg(localPath));
    exc.setCause(pluginLoader.errorString());
    throw exc;
  }

  QDirIterator dirIter(resourcePrefix, QDirIterator::Subdirectories);
  while (dirIter.hasNext())
  {
    QString resourcePath = dirIter.next();
    if (QFileInfo(resourcePath).isDir()) continue;

    QFile resourceFile(resourcePath);
    resourceFile.open(QIODevice::ReadOnly);
    QByteArray resourceData = resourceFile.readAll();
    resourceFile.close();

    statement = "INSERT INTO PluginResources(PluginID, ResourcePath, Resource) VALUES(?,?,?)";
    bindValues.clear();
    bindValues.append(QVariant::fromValue<qlonglong>(pluginId));
    bindValues.append(resourcePath.mid(resourcePrefix.size()-1));
    bindValues.append(resourceData);

    try
    {
      executeQuery(&query, statement, bindValues);
    }
    catch (...)
    {
      rollbackTransaction(&query);
      throw;
    }
  }

  pluginLoader.unload();

  try
  {
    ctkPluginArchive* archive = new ctkPluginArchive(m_PluginStorage, location, localPath,
                                               pluginId);;

    statement = "UPDATE Plugins SET SymbolicName=?,Version=? WHERE ID=?";
    QString versionString = archive->getAttribute(ctkPluginConstants::PLUGIN_VERSION);
    bindValues.clear();
    bindValues.append(archive->getAttribute(ctkPluginConstants::PLUGIN_SYMBOLICNAME));
    bindValues.append(versionString.isEmpty() ? "0.0.0" : versionString);
    bindValues.append(pluginId);

    if (!createArchive)
    {
      delete archive;
      archive = 0;
    }

    executeQuery(&query, statement, bindValues);

    commitTransaction(&query);

    return archive;
  }
  catch (...)
  {
      rollbackTransaction(&query);
      throw;
  }

}

void ctkPluginDatabase::setStartLevel(long pluginId, int startLevel)
{
  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  QString statement = "UPDATE Plugins SET StartLevel=? WHERE ID=?";
  QList<QVariant> bindValues;
  bindValues.append(startLevel);
  bindValues.append(QVariant::fromValue(pluginId));

  executeQuery(&query, statement, bindValues);
}

void ctkPluginDatabase::setLastModified(long pluginId, const QDateTime& lastModified)
{
  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  QString statement = "UPDATE Plugins SET LastModified=? WHERE ID=?";
  QList<QVariant> bindValues;
  bindValues.append(getStringFromQDateTime(lastModified));
  bindValues.append(QVariant::fromValue(pluginId));

  executeQuery(&query, statement, bindValues);
}

void ctkPluginDatabase::setAutostartSetting(long pluginId, int autostart)
{
  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  QString statement = "UPDATE Plugins SET AutoStart=? WHERE ID=?";
  QList<QVariant> bindValues;
  bindValues.append(autostart);
  bindValues.append(QVariant::fromValue(pluginId));

  executeQuery(&query, statement, bindValues);
}

QStringList ctkPluginDatabase::findResourcesPath(long pluginId, const QString& path) const
{
  checkConnection();

  QString statement = "SELECT SUBSTR(ResourcePath,?) FROM PluginResources WHERE PluginID=? AND SUBSTR(ResourcePath,1,?)=?";

  QString resourcePath = path.startsWith('/') ? path : QString("/") + path;
  if (!resourcePath.endsWith('/'))
    resourcePath += "/";

  QList<QVariant> bindValues;
  bindValues.append(resourcePath.size()+1);
  bindValues.append(qlonglong(pluginId));
  bindValues.append(resourcePath.size());
  bindValues.append(resourcePath);

  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  executeQuery(&query, statement, bindValues);

  QStringList paths;
  while (query.next())
  {
    QString currPath = query.value(EBindIndex).toString();
    int slashIndex = currPath.indexOf('/');
    if (slashIndex > 0)
    {
      currPath = currPath.left(slashIndex+1);
    }

    paths << currPath;
  }

  return paths;
}

void ctkPluginDatabase::removeArchive(const ctkPluginArchive *pa)
{
  checkConnection();

  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  QString statement = "DELETE FROM Plugins WHERE ID=?";

  QList<QVariant> bindValues;
  bindValues.append(pa->getPluginId());

  executeQuery(&query, statement, bindValues);
}

void ctkPluginDatabase::executeQuery(QSqlQuery *query, const QString &statement, const QList<QVariant> &bindValues) const
{
  Q_ASSERT(query != 0);

  bool success = false;
  enum {Prepare =0 , Execute=1};

  for (int stage=Prepare; stage <= Execute; ++stage)
  {
    if ( stage == Prepare)
      success = query->prepare(statement);
    else // stage == Execute
      success = query->exec();

    if (!success)
    {
      QString errorText = "Problem: Could not %1 statement: %2\n"
              "Reason: %3\n"
              "Parameters: %4\n";
      QString parameters;
      if (bindValues.count() > 0)
      {
        for (int i = 0; i < bindValues.count(); ++i)
        {
          parameters.append(QString("\n\t[") + QString::number(i) + "]: " + bindValues.at(i).toString());
        }
      }
      else
      {
        parameters = "None";
      }

      ctkPluginDatabaseException::Type errorType;
      int result = query->lastError().number();
      if (result == 26 || result == 11) //SQLILTE_NOTADB || SQLITE_CORRUPT
      {
        qWarning() << "ctkPluginFramework:- Database file is corrupt or invalid:" << getDatabasePath();
        errorType = ctkPluginDatabaseException::DB_FILE_INVALID;
      }
      else if (result == 8) //SQLITE_READONLY
        errorType = ctkPluginDatabaseException::DB_WRITE_ERROR;
      else
        errorType = ctkPluginDatabaseException::DB_SQL_ERROR;

      query->finish();
      query->clear();

      throw ctkPluginDatabaseException(errorText.arg(stage == Prepare ? "prepare":"execute")
                  .arg(statement).arg(query->lastError().text()).arg(parameters), errorType);
    }

    if (stage == Prepare)
    {
      foreach(const QVariant &bindValue, bindValues)
        query->addBindValue(bindValue);
    }
  }
}


void ctkPluginDatabase::close()
{
  if (m_isDatabaseOpen)
  {
    QSqlDatabase database = QSqlDatabase::database(m_connectionName, false);
    if (database.isValid())
    {
      if(database.isOpen())
      {
        database.close();
        m_isDatabaseOpen = false;
        return;
      }
    }
    else
    {
      throw ctkPluginDatabaseException(QString("Problem closing database: Invalid connection %1").arg(m_connectionName));
    }
  }
}


void ctkPluginDatabase::setDatabasePath(const QString &databasePath)
{
    m_databasePath = QDir::toNativeSeparators(databasePath);
}


QString ctkPluginDatabase::getDatabasePath() const
{
  QString path = m_databasePath;
  if(path.isEmpty())
  {
    path = QDir::homePath() + "/ctkpluginfw/plugins.db";
    qWarning() << "No database path set. Using default:" << path;
  }

  path = QDir::toNativeSeparators(path);
  qDebug() << "Using database:" << path;
  return path;
}


QByteArray ctkPluginDatabase::getPluginResource(long pluginId, const QString& res) const
{
  checkConnection();

  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);

  QString statement = "SELECT Resource FROM PluginResources WHERE PluginID=? AND ResourcePath=?";

  QString resourcePath = res.startsWith('/') ? res : QString("/") + res;
  QList<QVariant> bindValues;
  bindValues.append(qlonglong(pluginId));
  bindValues.append(resourcePath);

  executeQuery(&query, statement, bindValues);

  if (query.next())
  {
    return query.value(EBindIndex).toByteArray();
  }

  return QByteArray();
}


void ctkPluginDatabase::createTables()
{
    QSqlDatabase database = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(database);

    //Begin Transaction
    beginTransaction(&query, Write);

    QString statement("CREATE TABLE Plugins("
                      "ID INTEGER PRIMARY KEY,"
                      "Location TEXT NOT NULL UNIQUE,"
                      "LocalPath TEXT NOT NULL UNIQUE,"
                      "SymbolicName TEXT NOT NULL,"
                      "Version TEXT NOT NULL,"
                      "State INTEGER NOT NULL,"
                      "LastModified TEXT NOT NULL,"
                      "Timestamp TEXT NOT NULL,"
                      "StartLevel INTEGER NOT NULL,"
                      "AutoStart INTEGER NOT NULL)");
    try
    {
      executeQuery(&query, statement);
    }
    catch (...)
    {
      rollbackTransaction(&query);
      throw;
    }

    statement = "CREATE TABLE PluginResources("
                "PluginID INTEGER NOT NULL,"
                "ResourcePath TEXT NOT NULL, "
                "Resource BLOB NOT NULL,"
                "FOREIGN KEY(PluginID) REFERENCES Plugins(ID) ON DELETE CASCADE)";
    try
    {
      executeQuery(&query, statement);
    }
    catch (...)
    {
      rollbackTransaction(&query);
      throw;
    }

    try
    {
      commitTransaction(&query);
    }
    catch (...)
    {
      rollbackTransaction(&query);
      throw;
    }

}


bool ctkPluginDatabase::checkTables() const
{
  bool bTables(false);
  QStringList tables = QSqlDatabase::database(m_connectionName).tables();
  if (tables.contains(PLUGINS_TABLE)
      && tables.contains(PLUGIN_RESOURCES_TABLE))
  {
    bTables = true;
  }
  return bTables;
}


bool ctkPluginDatabase::dropTables()
{
  //Execute transaction for deleting the database tables
  QSqlDatabase database = QSqlDatabase::database(m_connectionName);
  QSqlQuery query(database);
  QStringList expectedTables;
  expectedTables << PLUGINS_TABLE << PLUGIN_RESOURCES_TABLE;

  if (database.tables().count() > 0)
  {
    beginTransaction(&query, Write);
    QStringList actualTables = database.tables();

    foreach(const QString expectedTable, expectedTables)
    {
      if (actualTables.contains(expectedTable))
      {
        try
        {
          executeQuery(&query, QString("DROP TABLE ") + expectedTable);
        }
        catch (...)
        {
          rollbackTransaction(&query);
          throw;
        }
      }
      try
      {
        commitTransaction(&query);
      }
      catch (...)
      {
        rollbackTransaction(&query);
        throw;
      }
    }
  }
  return true;
}


bool ctkPluginDatabase::isOpen() const
{
  return m_isDatabaseOpen;
}


void ctkPluginDatabase::checkConnection() const
{
  if(!m_isDatabaseOpen)
  {
    throw ctkPluginDatabaseException("Database not open.", ctkPluginDatabaseException::DB_NOT_OPEN_ERROR);
  }

  if (!QSqlDatabase::database(m_connectionName).isValid())
  {
    throw ctkPluginDatabaseException(QString("Database connection invalid: %1").arg(m_connectionName),
                                  ctkPluginDatabaseException::DB_CONNECTION_INVALID);
  }
}


void ctkPluginDatabase::beginTransaction(QSqlQuery *query, TransactionType type)
{
  bool success;
  if (type == Read)
      success = query->exec(QLatin1String("BEGIN"));
  else
      success = query->exec(QLatin1String("BEGIN IMMEDIATE"));

  if (!success) {
      int result = query->lastError().number();
      if (result == 26 || result == 11) //SQLITE_NOTADB || SQLITE_CORRUPT
      {
        throw ctkPluginDatabaseException(QString("ctkPluginFramework: Database file is corrupt or invalid: %1").arg(getDatabasePath()),
                                      ctkPluginDatabaseException::DB_FILE_INVALID);
      }
      else if (result == 8) //SQLITE_READONLY
      {
        throw ctkPluginDatabaseException(QString("ctkPluginFramework: Insufficient permissions to write to database: %1").arg(getDatabasePath()),
                                      ctkPluginDatabaseException::DB_WRITE_ERROR);
      }
      else
        throw ctkPluginDatabaseException(QString("ctkPluginFramework: ") + query->lastError().text(),
                                      ctkPluginDatabaseException::DB_SQL_ERROR);
  }

}


void ctkPluginDatabase::commitTransaction(QSqlQuery *query)
{
  Q_ASSERT(query != 0);
  query->finish();
  query->clear();
  if (!query->exec(QLatin1String("COMMIT")))
  {
    throw ctkPluginDatabaseException(QString("ctkPluginFramework: ") + query->lastError().text(),
                                  ctkPluginDatabaseException::DB_SQL_ERROR);
  }
}


void ctkPluginDatabase::rollbackTransaction(QSqlQuery *query)
{
  Q_ASSERT(query !=0);
  query->finish();
  query->clear();

  if (!query->exec(QLatin1String("ROLLBACK")))
  {
    throw ctkPluginDatabaseException(QString("ctkPluginFramework: ") + query->lastError().text(),
                                  ctkPluginDatabaseException::DB_SQL_ERROR);
  }
}

QList<ctkPluginArchive*> ctkPluginDatabase::getPluginArchives() const
{
  checkConnection();

  QSqlQuery query(QSqlDatabase::database(m_connectionName));
  QString statement("SELECT ID, Location, LocalPath FROM Plugins WHERE State != ?");
  QList<QVariant> bindValues;
  bindValues.append(ctkPlugin::UNINSTALLED);

  executeQuery(&query, statement, bindValues);

  QList<ctkPluginArchive*> archives;
  while (query.next())
  {
    const long id = query.value(EBindIndex).toLongLong();
    const QUrl location(query.value(EBindIndex1).toString());
    const QString localPath(query.value(EBindIndex2).toString());

    if (id <= 0 || location.isEmpty() || localPath.isEmpty())
    {
      throw ctkPluginDatabaseException(QString("Database integrity corrupted, row %1 contains empty values.").arg(id),
                                    ctkPluginDatabaseException::DB_FILE_INVALID);
    }

    try
    {
      ctkPluginArchive* pa = new ctkPluginArchive(m_PluginStorage, location, localPath, id);
      archives.append(pa);
    }
    catch (const ctkPluginException& exc)
    {
      qWarning() << exc;
    }
  }

  return archives;
}

QString ctkPluginDatabase::getStringFromQDateTime(const QDateTime& dateTime) const
{
  return dateTime.toString(Qt::ISODate);
}

QDateTime ctkPluginDatabase::getQDateTimeFromString(const QString& dateTimeString) const
{
  return QDateTime::fromString(dateTimeString, Qt::ISODate);
}
