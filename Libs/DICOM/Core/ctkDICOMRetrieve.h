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

#ifndef __ctkDICOMRetrieve_h
#define __ctkDICOMRetrieve_h


// Qt includes 
#include <QObject>
#include <QDir>
#include <QSharedPointer>

#include "ctkDICOMCoreExport.h"

// CTK Core includes
#include "ctkDICOMDatabase.h"

class ctkDICOMRetrievePrivate;
class CTK_DICOM_CORE_EXPORT ctkDICOMRetrieve : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString callingAETitle READ callingAETitle WRITE setCallingAETitle);
  Q_PROPERTY(QString calledAETitle READ calledAETitle WRITE setCallingAETitle);
  Q_PROPERTY(QString host READ host WRITE setHost);
  Q_PROPERTY(int callingPort READ callingPort WRITE setCallingPort);
  Q_PROPERTY(int calledPort READ calledPort WRITE setCalledPort);
  Q_PROPERTY(QString moveDestinationAETitle READ moveDestinationAETitle WRITE setMoveDestinationAETitle)

public:
  explicit ctkDICOMRetrieve();
  virtual ~ctkDICOMRetrieve();

  /// Set methods for connectivity
  /// CTK_AE
  void setCallingAETitle( const QString& callingAETitle );
  QString callingAETitle() const;
  /// CTK_AE
  void setCalledAETitle( const QString& calledAETitle );
  QString calledAETitle() const;
  /// localhost
  void setHost( const QString& host );
  QString host() const;
  /// [0, 65365] 11113
  void setCallingPort( int port );
  int callingPort() const;
  /// [0, 65365] 11112
  void setCalledPort( int port );
  int calledPort() const;
  /// Typically CTK_CLIENT_AE
  void setMoveDestinationAETitle( const QString& moveDestinationAETitle );
  QString moveDestinationAETitle() const;

  /// method for database
  void setRetrieveDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase);
  QSharedPointer<ctkDICOMDatabase> retrieveDatabase()const;

  // Could be a slot...
  bool retrieveSeries( const QString& seriesInstanceUID );
  bool retrieveStudy( const QString& studyInstanceUID );

protected:
  QScopedPointer<ctkDICOMRetrievePrivate> d_ptr;

private:
  void retrieve( QDir directory );

  Q_DECLARE_PRIVATE(ctkDICOMRetrieve);
  Q_DISABLE_COPY(ctkDICOMRetrieve);

};


#endif
