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

// Qt includes
#include <QHostAddress>

// CTK includes
#include "ctkDicomAppServer_p.h"
#include "ctkDicomAppPlugin_p.h"
#include "ctkAppSoapMessageProcessor.h"

#include <ctkDicomAppHostingTypesHelper.h>
#include <ctkDicomAppInterface.h>
#include <ctkExchangeSoapMessageProcessor.h>

#include <ctkServiceReference.h>

// STD includes
#include <stdexcept>

//----------------------------------------------------------------------------
ctkDicomAppServer::ctkDicomAppServer(int port, QString path)
  : AppInterfaceRegistered(false), Port(port), Path(path),
    AppInterfaceTracker(ctkDicomAppPlugin::getPluginContext(), this)
{
  this->AppInterfaceTracker.open();

  connect(&this->Server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));
  connect(&this->Server, SIGNAL(incomingWSDLMessage(QString,QString*)),
          this, SLOT(incomingWSDLMessage(QString,QString*)));

  if (!this->Server.listen(QHostAddress::LocalHost, this->Port))
    {
    qCritical() << "Listening to 127.0.0.1:" << port << " failed.";
    }
}

//----------------------------------------------------------------------------
ctkDicomAppServer::~ctkDicomAppServer()
{
  this->Server.close ();
}

//----------------------------------------------------------------------------
void ctkDicomAppServer::incomingWSDLMessage(
  const QString& message, QString* reply)
{
  if (message == "?wsdl")
  {
    QFile wsdlfile(":/dah/ApplicationService-20100825.wsdl");
    wsdlfile.open(QFile::ReadOnly | QFile::Text);
    if(wsdlfile.isOpen())
      {
      QTextStream textstream(&wsdlfile);
      *reply = textstream.readAll();
      QString actualURL="http://localhost:";
      //actualURL+=QString::number(Port)+"/ApplicationInterface"; // FIXME: has to be replaced by url provided by host
	  actualURL+=QString::number(Port)+Path; 
      reply->replace("REPLACE_WITH_ACTUAL_URL",actualURL);
      reply->replace("ApplicationService-20100825.xsd",actualURL+"?xsd=1");
      //reply->replace("<soap:body use=\"literal\"/>","<soap:body use=\"literal\"></soap:body>");
      }
  }
  else if (message == "?xsd=1")
  {
    QFile wsdlfile(":/dah/HostService-20100825.xsd");
    wsdlfile.open(QFile::ReadOnly | QFile::Text);
    if(wsdlfile.isOpen())
      {
      QTextStream textstream(&wsdlfile);
      *reply = textstream.readAll();
      }
  }
}

//----------------------------------------------------------------------------
void ctkDicomAppServer::incomingSoapMessage(
  const QtSoapMessage& message,
  QtSoapMessage* reply)

{
  QMutexLocker lock(&this->Mutex);
  this->Processors.process(message, reply);
}

//----------------------------------------------------------------------------
ctkDicomAppInterface* ctkDicomAppServer::addingService(const ctkServiceReference& reference)
{
  QMutexLocker lock(&this->Mutex);

  if (this->AppInterfaceRegistered)
    {
    //TODO maybe use ctkLogService
    qWarning() << "A ctkDicomAppInterface service has already been added";
    return 0;
    }
  this->AppInterfaceRegistered = true;
  ctkDicomAppInterface* appInterface = ctkDicomAppPlugin::getPluginContext()->getService<ctkDicomAppInterface>(reference);
  this->Processors.push_back(new ctkAppSoapMessageProcessor(appInterface));
  this->Processors.push_back(new ctkExchangeSoapMessageProcessor(appInterface));
  return appInterface;
}

//----------------------------------------------------------------------------
void ctkDicomAppServer::modifiedService(const ctkServiceReference& reference, ctkDicomAppInterface* service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)
  // do nothing
}

//----------------------------------------------------------------------------
void ctkDicomAppServer::removedService(const ctkServiceReference& reference, ctkDicomAppInterface* service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)

  QMutexLocker lock(&this->Mutex);
  this->AppInterfaceRegistered = false;
  this->Processors.clear();
}
