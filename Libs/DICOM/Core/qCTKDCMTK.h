#ifndef __qCTKDCMTK_h
#define __qCTKDCMTK_h

// QT includes 
#include <QObject>
#include <QSqlDatabase>

// qCTK includes
#include <qCTKPimpl.h>

#include "CTKDICOMCoreExport.h"

class qCTKDCMTKPrivate;
class Q_CTK_DICOM_CORE_EXPORT qCTKDCMTK : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit qCTKDCMTK(QObject* parent = 0);
  virtual ~qCTKDCMTK();
  
  /// 
  /// 
  void setDatabaseFileName(const QString& file);
  
  ///
  /// open the SQLite database in file
  virtual bool openDatabase(const QString& file);

  QSqlDatabase& database();
  const QString& GetLastError() const; 
  
  
  
  ///
  /// delete all data and initialize the database with
  /// the default schema
  bool initializeDatabase();
private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTK);
};

#endif
