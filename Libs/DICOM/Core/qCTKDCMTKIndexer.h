#ifndef __qCTKDCMTKIndexer_h
#define __qCTKDCMTKIndexer_h

// QT includes 
#include <QSqlDatabase>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class qCTKDCMTKIndexerPrivate;
class CTK_DICOM_CORE_EXPORT qCTKDCMTKIndexer
{
public:
  explicit qCTKDCMTKIndexer();
  virtual ~qCTKDCMTKIndexer();
  /// add directory to database and optionally copy files to destinationDirectory
  void addDirectory(QSqlDatabase database, const QString& directoryName, const QString& destinationDirectoryName = "");
  void refreshDatabase(QSqlDatabase database, const QString& directoryName);

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKIndexer);
};

#endif
