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

#ifndef __ctkDICOMEcho_h
#define __ctkDICOMEcho_h

// Qt includes
#include <QMap>
#include <QObject>
#include <QString>

// CTK includes
#include <ctkPimpl.h>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"

class ctkDICOMEchoPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMEcho : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName);
  Q_PROPERTY(QString callingAETitle READ callingAETitle WRITE setCallingAETitle);
  Q_PROPERTY(QString calledAETitle READ calledAETitle WRITE setCalledAETitle);
  Q_PROPERTY(QString host READ host WRITE setHost);
  Q_PROPERTY(int port READ port WRITE setPort);
  Q_PROPERTY(int connectionTimeout READ connectionTimeout WRITE setConnectionTimeout);
  Q_PROPERTY(QString jobUID READ jobUID WRITE setJobUID);

public:
  explicit ctkDICOMEcho(QObject* parent = 0);
  virtual ~ctkDICOMEcho();

  ///@{
  /// Name identifying the server
  void setConnectionName(const QString& connectionName);
  QString connectionName() const;
  ///@}

  ///@{
  /// Set methods for connectivity.
  /// Empty by default
  void setCallingAETitle(const QString& callingAETitle);
  QString callingAETitle() const;

  void setCalledAETitle(const QString& calledAETitle);
  QString calledAETitle() const;
  ///@}

  ///@{
  /// Peer hostname being connected to
  /// Empty by default
  void setHost(const QString& host);
  QString host() const;
  ///@}

  ///@{
  /// Specify a port for the packet headers.
  /// \a port ranges from 0 to 65535.
  /// 80 by default.
  void setPort(const int& port);
  int port() const;
  ///@}

  ///@{
  /// Connection timeout, default 3 sec.
  void setConnectionTimeout(const int& timeout);
  int connectionTimeout() const;
  ///@}

  /// Return true if the operation was canceled.
  Q_INVOKABLE bool wasCanceled();

  /// Echo connection.
  Q_INVOKABLE bool echo();

  ///@{
  /// Reference job uid.
  void setJobUID(const QString& jobUID);
  QString jobUID() const;
  ///@}

public Q_SLOTS:
  void cancel();

protected:
  QScopedPointer<ctkDICOMEchoPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMEcho);
  Q_DISABLE_COPY(ctkDICOMEcho);
};

#endif
