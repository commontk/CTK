#ifndef __qCTKDCMTK_h
#define __qCTKDCMTK_h

// QT includes 
#include <QObject>
#include <QSqlDatabase>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class qCTKDCMTKPrivate;
class CTK_DICOM_CORE_EXPORT qCTKDCMTK : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit qCTKDCMTK(QObject* parent = 0);
  virtual ~qCTKDCMTK();
  
  ///
  /// open the SQLite database in @param file. If the file does not
  /// exist, a new database is created and initialized with the
  /// default schema
  virtual bool openDatabase(const QString& file);

  const QSqlDatabase& database() const;
  const QString& GetLastError() const; 
  
  ///
  /// close the database. It must not be used afterwards.
  void closeDatabase();  
  ///
  /// delete all data and reinitialize the database.
  bool initializeDatabase(const char* schemaFile = ":/dicom/dicom-schema.sql");
private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTK);
};

#endif
