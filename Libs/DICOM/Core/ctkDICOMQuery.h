/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkDICOMQuery_h
#define __ctkDICOMQuery_h

// Qt includes 
#include <QObject>
#include <QSqlDatabase>

// CTK includes
#include <ctkPimpl.h>

#include "ctkDICOMCoreExport.h"
#include "ctkDICOMIndexerBase.h"

class ctkDICOMQueryPrivate;
class CTK_DICOM_CORE_EXPORT ctkDICOMQuery : public ctkDICOMIndexerBase
{
  Q_OBJECT
  Q_PROPERTY(QString callingAETitle READ callingAETitle WRITE setCallingAETitle);
  Q_PROPERTY(QString calledAETitle READ calledAETitle WRITE setCallingAETitle);
  Q_PROPERTY(int port READ port WRITE setPort);
public:
  explicit ctkDICOMQuery();
  virtual ~ctkDICOMQuery();
  
  /// Set methods for connectivity
  void setCallingAETitle ( QString callingAETitle );
  const QString& callingAETitle();
  void setCalledAETitle ( QString calledAETitle );
  const QString& calledAETitle();
  void setHost ( QString host );
  const QString& host();
  void setPort ( int port );
  int port();
  
  /// Query a remote DICOM Image Store SCP
  void query(QSqlDatabase database);

  // Add a StudyInstanceUID to be queried
  void addStudyInstanceUID ( QString StudyInstanceUID );

protected:
  QScopedPointer<ctkDICOMQueryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMQuery);
  Q_DISABLE_COPY(ctkDICOMQuery);
};

#endif
