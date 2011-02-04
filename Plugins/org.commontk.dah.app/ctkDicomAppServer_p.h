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

#include <QObject>
#include <QtSoapMessage>

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
  ctkDicomAppServer(int port);

public slots:

  void incomingSoapMessage(const QtSoapMessage& message,
                           QtSoapMessage* reply);
  void incomingWSDLMessage(const QString& message, QString* reply);

protected:

  ctkDicomAppInterface* addingService(const ctkServiceReference& reference);
  void modifiedService(const ctkServiceReference& reference, ctkDicomAppInterface* service);
  void removedService(const ctkServiceReference& reference, ctkDicomAppInterface* service);

private:

  QMutex mutex;
  bool appInterfaceRegistered;

  ctkSoapMessageProcessorList processors;
  ctkSimpleSoapServer server;
  int port;

  ctkServiceTracker<ctkDicomAppInterface*> appInterfaceTracker;
};

#endif // CTKDICOMAPPPSERVER_P_H
