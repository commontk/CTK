/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#ifndef CTKDICOMAPPPSERVER_P_H
#define CTKDICOMAPPPSERVER_P_H

// Qt includes
#include <QObject>
#include <QtSoapMessage>

// CTK includes
#include <ctkServiceTracker.h>

#include <ctkDicomAppInterface.h>
#include <ctkSimpleSoapServer.h>
#include <ctkSoapMessageProcessorList.h>

/**
 * This class serves as a SOAP server for DICOM Hosted Application
 * specific requests. It forwards the SOAP requests to a service
 * object registered under the ctkDicomAppInterface interface from
 * a specific application plugin.
 */
class ctkDicomAppServer : public QObject, ctkServiceTrackerCustomizer<ctkDicomAppInterface*>
{
  Q_OBJECT

public:
  ctkDicomAppServer(int port, QString path);
  ~ctkDicomAppServer();
public Q_SLOTS:

  void incomingSoapMessage(const QtSoapMessage& message,
                           QtSoapMessage* reply);
  void incomingWSDLMessage(const QString& message, QString* reply);

protected:

  virtual ctkDicomAppInterface* addingService(const ctkServiceReference& reference);
  virtual void modifiedService(const ctkServiceReference& reference, ctkDicomAppInterface* service);
  virtual void removedService(const ctkServiceReference& reference, ctkDicomAppInterface* service);

private:

  QMutex Mutex;
  bool AppInterfaceRegistered;

  ctkSoapMessageProcessorList Processors;
  ctkSimpleSoapServer Server;
  int Port;
  QString Path;

  ctkServiceTracker<ctkDicomAppInterface*> AppInterfaceTracker;
};

#endif // CTKDICOMAPPPSERVER_P_H
