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


#include "ctkDicomAppServer_p.h"
#include "ctkDicomAppPlugin_p.h"
#include "ctkAppSoapMessageProcessor.h"

#include <ctkDicomAppHostingTypesHelper.h>
#include <ctkDicomAppInterface.h>
#include <ctkExchangeSoapMessageProcessor.h>

#include <ctkServiceReference.h>

#include <QHostAddress>

#include <stdexcept>


ctkDicomAppServer::ctkDicomAppServer(int port)
  : appInterfaceRegistered(false), port(port),
    appInterfaceTracker(ctkDicomAppPlugin::getPluginContext(), this)
{
  appInterfaceTracker.open();

  connect(&server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));
  connect(&server, SIGNAL(incomingWSDLMessage(QString,QString*)),
          this, SLOT(incomingWSDLMessage(QString,QString*)));

  if (!server.listen(QHostAddress::LocalHost, this->port))
  {
    qCritical() << "Listening to 127.0.0.1:" << port << " failed.";
  }
}

void ctkDicomAppServer::incomingWSDLMessage(
  const QString& message, QString* reply)
{
  if (message == "?wsdl")
  {
    QFile wsdlfile(":/dah/ApplicationService.wsdl");
    wsdlfile.open(QFile::ReadOnly | QFile::Text);
    if(wsdlfile.isOpen())
    {
      QTextStream textstream(&wsdlfile);
      *reply = textstream.readAll();
      QString actualURL="http://localhost:";
      actualURL+=QString::number(port)+"/ApplicationInterface"; // FIXME: has to be replaced by url provided by host
      reply->replace("REPLACE_WITH_ACTUAL_URL",actualURL);
      reply->replace("ApplicationService_schema1.xsd",actualURL+"?xsd=1");
      //reply->replace("<soap:body use=\"literal\"/>","<soap:body use=\"literal\"></soap:body>");
    }
  }
  else if (message == "?xsd=1")
  {
    QFile wsdlfile(":/dah/HostService_schema1.xsd");
    wsdlfile.open(QFile::ReadOnly | QFile::Text);
    if(wsdlfile.isOpen())
    {
      QTextStream textstream(&wsdlfile);
      *reply = textstream.readAll();
    }
  }
}

void ctkDicomAppServer::incomingSoapMessage(
  const QtSoapMessage& message,
  QtSoapMessage* reply)

{
  QMutexLocker lock(&mutex);
  processors.process(message, reply);
}

ctkDicomAppInterface* ctkDicomAppServer::addingService(const ctkServiceReference& reference)
{
  QMutexLocker lock(&mutex);

  if (appInterfaceRegistered)
  {
    //TODO maybe use ctkLogService
    qWarning() << "A ctkDicomAppInterface service has already been added";
    return 0;
  }
  appInterfaceRegistered = true;
  ctkDicomAppInterface* appInterface = ctkDicomAppPlugin::getPluginContext()->getService<ctkDicomAppInterface>(reference);
  processors.push_back(new ctkAppSoapMessageProcessor(appInterface));
  processors.push_back(new ctkExchangeSoapMessageProcessor(appInterface));
  return appInterface;
}

void ctkDicomAppServer::modifiedService(const ctkServiceReference& reference, ctkDicomAppInterface* service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)
  // do nothing
}

void ctkDicomAppServer::removedService(const ctkServiceReference& reference, ctkDicomAppInterface* service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)

  QMutexLocker lock(&mutex);
  appInterfaceRegistered = false;
  processors.clear();
}

