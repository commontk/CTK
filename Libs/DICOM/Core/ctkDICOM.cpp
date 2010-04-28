/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// Qt includes
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QStringList>

// ctkDICOM includes
#include "ctkDICOM.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkDICOMPrivate: public ctkPrivate<ctkDICOM>
{
public:
  ctkDICOMPrivate(); 
  bool executeScript(const QString& script);

  QSqlDatabase  Database;
  QString       DatabaseFileName;
  QString       LastError;
};

//----------------------------------------------------------------------------
// ctkDICOMPrivate methods

//------------------------------------------------------------------------------
ctkDICOMPrivate::ctkDICOMPrivate()
{
}

//----------------------------------------------------------------------------
// ctkDICOMWidget methods

//------------------------------------------------------------------------------
ctkDICOM::ctkDICOM(QObject* _parent): Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOM);
}

//----------------------------------------------------------------------------
ctkDICOM::~ctkDICOM()
{
}

//----------------------------------------------------------------------------
bool ctkDICOM::openDatabase(const QString& databaseFileName)
{
  CTK_D(ctkDICOM);
  d->Database = QSqlDatabase::addDatabase("QSQLITE","DICOM-DB");
  d->Database.setDatabaseName(databaseFileName);
  if ( ! (d->Database.open()) )
    {
    d->LastError = d->Database.lastError().text();
    return false;
    }
  if ( d->Database.tables().empty() ) 
    {
    initializeDatabase();
    }
  return true;
}

//------------------------------------------------------------------------------
const QString& ctkDICOM::GetLastError() const {
  CTK_D(const ctkDICOM);
  return d->LastError; 
}

//------------------------------------------------------------------------------
const QSqlDatabase& ctkDICOM::database() const {
  CTK_D(const ctkDICOM);
  return d->Database;
}

//------------------------------------------------------------------------------
bool ctkDICOMPrivate::executeScript(const QString& script) {
  QFile scriptFile(script);
  scriptFile.open(QIODevice::ReadOnly);
  if  ( !scriptFile.isOpen() )
    {
    qDebug() << "Script file " << script << " could not be opened!\n";
    return false;
    }

  QString sqlCommands( QTextStream(&scriptFile).readAll() );
  sqlCommands.replace( '\n', ' ' );
  sqlCommands.replace("; ", ";\n");

  QStringList sqlCommandsLines = sqlCommands.split('\n');

  QSqlQuery query(Database);

  for (QStringList::iterator it = sqlCommandsLines.begin(); it != sqlCommandsLines.end()-1; ++it)
  {
    if (! (*it).startsWith("--") )
      {
      query.exec(*it);
      if (query.lastError().type())
        {
        qDebug() << "There was an error during execution of the statement: " << (*it);
        qDebug() << "Error message: " << query.lastError().text();
        return false;
        }
      }
  }
  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOM::initializeDatabase(const char* sqlFileName)
{
  CTK_D(ctkDICOM);
  return d->executeScript(sqlFileName);
}

//------------------------------------------------------------------------------
void ctkDICOM::closeDatabase()
{
  CTK_D(ctkDICOM);
  d->Database.close();
}
