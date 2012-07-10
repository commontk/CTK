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

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMRetrieve : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString callingAETitle READ callingAETitle WRITE setCallingAETitle);
  Q_PROPERTY(QString calledAETitle READ calledAETitle WRITE setCallingAETitle);
  Q_PROPERTY(QString host READ host WRITE setHost);
  Q_PROPERTY(int port READ port WRITE setPort);
  Q_PROPERTY(QString moveDestinationAETitle READ moveDestinationAETitle WRITE setMoveDestinationAETitle)
  Q_PROPERTY(bool keepAssociationOpen READ keepAssociationOpen WRITE setKeepAssociationOpen)
  Q_PROPERTY(bool wasCanceled READ wasCanceled WRITE setWasCanceled)

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
  /// did someone cancel us during operation?
  /// (default false)
  void setWasCanceled(const bool wasCanceled);
  bool wasCanceled();
  /// where to insert new data sets obtained via get (must be set for
  /// get to succeed
  Q_INVOKABLE void setDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase);
  Q_INVOKABLE QSharedPointer<ctkDICOMDatabase> database()const;

public Q_SLOTS:
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
  /// Cancel the current operation
  void cancel();

Q_SIGNALS:
  /// Signal is emitted inside the retrieve() function. It ranges from 0 to 100.
  /// In case of an error, you are assured that the progress value 100 is fired
  void progress(int progress);
  /// Signal is emitted inside the retrieve() function. It sends the different step
  /// the function is at.
  void progress(const QString& message);
  /// Signal is emitted inside the retrieve() function. It sends 
  /// detailed feedback for debugging
  void debug(const QString& message);
  /// Signal is emitted inside the retrieve() function. It send any error messages
  void error(const QString& message);
  /// Signal is emitted inside the retrieve() function when finished with value 
  /// true for success or false for error
  void done(const bool& error);

protected:
  QScopedPointer<ctkDICOMRetrievePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMRetrieve);
  Q_DISABLE_COPY(ctkDICOMRetrieve);

  friend class ctkDICOMRetrieveSCUPrivate;  // for access to status signals
};


#endif
