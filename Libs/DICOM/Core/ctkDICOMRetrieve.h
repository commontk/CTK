/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

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
  Q_PROPERTY(int port READ port WRITE setPort);
  Q_PROPERTY(QString moveDestinationAETitle READ moveDestinationAETitle WRITE setMoveDestinationAETitle)
  Q_PROPERTY(bool keepAssociationOpen READ keepAssociationOpen WRITE setKeepAssociationOpen)

public:
  explicit ctkDICOMRetrieve();
  virtual ~ctkDICOMRetrieve();

  /// Set methods for connectivity
  /// CTK_AE - the AE string by which the peer host might 
  /// recognize your request
  void setCallingAETitle( const QString& callingAETitle );
  QString callingAETitle() const;
  /// CTK_AE - the AE of the serice of peer host that you are calling
  /// which tells the host what you are requesting
  void setCalledAETitle( const QString& calledAETitle );
  QString calledAETitle() const;
  /// peer hostname being connected to
  void setHost( const QString& host );
  QString host() const;
  /// [0, 65365] port on peer host - e.g. 11112
  void setPort( int port );
  int port() const;
  /// Typically CTK_STORE or similar - needs to be something that the
  /// peer host knows about and is able to move data into
  /// Only used when calling moveSeries or moveStudy
  void setMoveDestinationAETitle( const QString& moveDestinationAETitle );
  QString moveDestinationAETitle() const;
  /// prefer to keep using the existing association to peer host when doing
  /// multiple requests (default true)
  void setKeepAssociationOpen(const bool keepOpen);
  bool keepAssociationOpen();
  /// where to insert new data sets obtained via get (must be set for
  /// get to succeed
  Q_INVOKABLE void setDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase);
  Q_INVOKABLE QSharedPointer<ctkDICOMDatabase> database()const;

public slots:
  /// Use CMOVE to ask peer host to store data to move destination
  bool moveSeries( const QString& studyInstanceUID,
                       const QString& seriesInstanceUID );
  /// Use CMOVE to ask peer host to store data to move destination
  bool moveStudy( const QString& studyInstanceUID );
  /// Use CGET to ask peer host to store data to us
  bool getSeries( const QString& studyInstanceUID,
                       const QString& seriesInstanceUID );
  /// Use CGET to ask peer host to store data to us
  bool getStudy( const QString& studyInstanceUID );

signals:
  //TODO: the signature of these signals will change
  //from string to a more specific format when we decide
  //what information to send
  /// emitted when a move response has been received from dcmtk
  void moveResponseHandled( const QString message );
  /// emitted when a dataset is incoming from a CGET
  void storeRequested( const QString message );
  /// emitted when remote server sends us CGET responses
  void retrieveStatusChanged( const QString message );

protected:
  QScopedPointer<ctkDICOMRetrievePrivate> d_ptr;

private:

  Q_DECLARE_PRIVATE(ctkDICOMRetrieve);
  Q_DISABLE_COPY(ctkDICOMRetrieve);

  friend class ctkDICOMRetrieveSCUPrivate;  // for access to status signals
};


#endif
