/*=========================================================================

  Library:   CTK

  Copyright (c) 2010

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

#ifndef __ctkDICOMTester_h
#define __ctkDICOMTester_h

// Qt includes 
#include <QObject>
class QProcess;

// CTKDICOMCore includes
#include "ctkDICOMCoreExport.h"
class ctkDICOMTesterPrivate;

class CTK_DICOM_CORE_EXPORT ctkDICOMTester : public QObject
{
  Q_OBJECT
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
  
  void setDCMQRSCPPort(int port);
  int dcmqrscpPort()const;
  
  QProcess* startDCMQRSCP();
  bool stopDCMQRSCP();

  /// Pushes data using storeSCU
  bool storeData(const QStringList& data);

protected:
  QScopedPointer<ctkDICOMTesterPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMTester);
  Q_DISABLE_COPY(ctkDICOMTester);
};

#endif
