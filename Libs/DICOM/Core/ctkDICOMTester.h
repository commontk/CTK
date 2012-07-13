/*=========================================================================

  Library:   CTK

  Copyright (c) 2010

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

#ifndef __ctkDICOMTester_h
#define __ctkDICOMTester_h

// Qt includes
#include <QObject>
class QProcess;

// CTKDICOMCore includes
#include "ctkDICOMCoreExport.h"
class ctkDICOMTesterPrivate;

/// \ingroup DICOM_Core
///
/// \brief Utility class to test DICOM network applications
/// A simple DICOM archive server can be run (startDCMQRSCP()), and images
/// can be stored into the server using storeData(). It internally uses
/// storeSCU.
///
class CTK_DICOM_CORE_EXPORT ctkDICOMTester : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString dcmqrscpExecutable READ dcmqrscpExecutable WRITE setDCMQRSCPExecutable)
  Q_PROPERTY(QString dcmqrscpConfigFile READ dcmqrscpConfigFile WRITE setDCMQRSCPConfigFile)
  Q_PROPERTY(QString storeSCUExecutable READ storeSCUExecutable WRITE setStoreSCUExecutable)
  Q_PROPERTY(QString storeSCPExecutable READ storeSCPExecutable WRITE setStoreSCPExecutable)
  Q_PROPERTY(int dcmqrscpPort READ dcmqrscpPort WRITE setDCMQRSCPPort)
public:
  ctkDICOMTester(QObject* parent = 0);
  explicit ctkDICOMTester(const QString& dcmqrscp, const QString& configFile, QObject* parent = 0);
  virtual ~ctkDICOMTester();

  void setDCMQRSCPExecutable(const QString& dcmqrscp);
  QString dcmqrscpExecutable()const;

  void setDCMQRSCPConfigFile(const QString& configFile);
  QString dcmqrscpConfigFile()const;

  void setStoreSCUExecutable(const QString& storescu);
  QString storeSCUExecutable()const;

  void setStoreSCPExecutable(const QString& storescp);
  QString storeSCPExecutable()const;

  ///  Port number [0,65365] where the dcmqrscp and storescu communicate.
  /// Changing the port won't change the port of any running process.
  /// You must stop and restart any process you want to have its port changed
  ///
  void setDCMQRSCPPort(int port);
  int dcmqrscpPort()const;

  ///  Starts a new DCMQRSCP as a separate process. The process is running until
  /// stopDCMQRSCP is called or ctkDICOMTester is destroyed.
  /// Only one process of DCMQRSCP can be running at a time.
  /// Calling startDCMQRSCP() while a DCMQRSCP process is already running
  /// results into a no-op. The return value is 0.
  /// \sa QProcess::start(),
  ///
  Q_INVOKABLE QProcess* startDCMQRSCP();

  ///  Stop the running DCMQRSCP process. Returns it's exit status or false if
  /// there is no running process.
  ///
  Q_INVOKABLE bool stopDCMQRSCP();

  ///  Pushes data (DCM images referred to by file name in data list) using DCMTK
  /// storeSCU app. It creates a separate process and waits for its termination.
  /// To be working, dcmqrscp must be running
  /// \sa startDCMQRSCP()
  ///
  Q_INVOKABLE bool storeData(const QStringList& data);

protected:
  QScopedPointer<ctkDICOMTesterPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMTester);
  Q_DISABLE_COPY(ctkDICOMTester);
};

#endif
