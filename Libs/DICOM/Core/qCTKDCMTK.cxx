
// qCTKDCMTK includes
#include "qCTKDCMTK.h"

// Qt includes
#include <QSqlDatabase>
#include <QSqlError>
#include <iostream>

//----------------------------------------------------------------------------
class qCTKDCMTKPrivate: public qCTKPrivate<qCTKDCMTK>
{
public:
  qCTKDCMTKPrivate(); 
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

