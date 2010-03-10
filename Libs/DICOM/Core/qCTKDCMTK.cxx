
// qCTKDCMTK includes
#include "qCTKDCMTK.h"

// Qt includes
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QStringList>

#include <iostream>

//----------------------------------------------------------------------------
class qCTKDCMTKPrivate: public qCTKPrivate<qCTKDCMTK>
{
public:
  qCTKDCMTKPrivate(); 
  bool executeScript(const QString& script);

  QSqlDatabase  Database;
  QString       DatabaseFileName;
  QString       LastError;
};

//----------------------------------------------------------------------------
// qCTKDCMTKPrivate methods
qCTKDCMTKPrivate::qCTKDCMTKPrivate() 
{
}

//----------------------------------------------------------------------------
// qCTKDCMTKWidget methods

qCTKDCMTK::qCTKDCMTK(QObject* _parent): Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTK);
}

//----------------------------------------------------------------------------
qCTKDCMTK::~qCTKDCMTK()
{
}

//----------------------------------------------------------------------------
QCTK_SET_CXX(qCTKDCMTK, const QString&, setDatabaseFileName, DatabaseFileName);

//----------------------------------------------------------------------------
bool qCTKDCMTK::openDatabase(const QString& databaseFileName) 
{
  QCTK_D(qCTKDCMTK);
  d->Database = QSqlDatabase::addDatabase("QSQLITE","DICOM-DB");
  d->Database.setDatabaseName(databaseFileName);
  if ( ! (d->Database.open()) )
    {
    d->LastError = d->Database.lastError().text();
    return false;
    }
  return true;
}
const QString& qCTKDCMTK::GetLastError() const {
  QCTK_D(const qCTKDCMTK);
  return d->LastError; 
}
QSqlDatabase& qCTKDCMTK::database() {
  QCTK_D(qCTKDCMTK);
  return d->Database;
}

bool qCTKDCMTKPrivate::executeScript(const QString& script) {
  QFile scriptFile(script);
  scriptFile.open(QIODevice::ReadOnly);
  QString sqlCommands( QTextStream(&scriptFile).readAll() );
  sqlCommands.replace( '\n', ' ' );
  sqlCommands.replace("; ", ";\n");

  //MITK_INFO << "Query:\n\n" << sqlCommands.toStdString() << "\n";

  QStringList sqlCommandsLines = sqlCommands.split('\n');

  QSqlQuery query(Database);

  for (QStringList::iterator it = sqlCommandsLines.begin(); it != sqlCommandsLines.end()-1; ++it)
  {
    qDebug() << "Statement: " << *it ; 
    if (! (*it).startsWith("--") )
      {
      query.exec(*it);
      if (query.lastError().type())
        {
        qDebug() << "There was an error during execution of the statement: " << (*it);
        return false;
        }
      }
  }


  return true;
}


bool qCTKDCMTK::initializeDatabase() 
{
  QCTK_D(qCTKDCMTK);
  return d->executeScript(":/dicom/dicom-schema.sql");
}
  

