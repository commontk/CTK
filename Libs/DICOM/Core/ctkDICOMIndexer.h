#ifndef __ctkDICOMIndexer_h
#define __ctkDICOMIndexer_h

// QT includes 
#include <QSqlDatabase>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class ctkDICOMIndexerPrivate;
class CTK_DICOM_CORE_EXPORT ctkDICOMIndexer
{
public:
  explicit ctkDICOMIndexer();
  virtual ~ctkDICOMIndexer();
  /// add directory to database and optionally copy files to destinationDirectory
  void addDirectory(QSqlDatabase database, const QString& directoryName, const QString& destinationDirectoryName = "");
  void refreshDatabase(QSqlDatabase database, const QString& directoryName);

private:
  QCTK_DECLARE_PRIVATE(ctkDICOMIndexer);
};

#endif
