#ifndef __qCTKDCMTKIndexer_h
#define __qCTKDCMTKIndexer_h

// QT includes 
#include <QSqlDatabase>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class qCTKDCMTKIndexerPrivate;
class Q_CTK_DICOM_CORE_EXPORT qCTKDCMTKIndexer  
{
public:
  explicit qCTKDCMTKIndexer();
  virtual ~qCTKDCMTKIndexer();
  void AddDirectory(QSqlDatabase database, const QString& directoryName);
  void refreshDatabase(QSqlDatabase database, const QString& directoryName);

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKIndexer);
};

#endif
