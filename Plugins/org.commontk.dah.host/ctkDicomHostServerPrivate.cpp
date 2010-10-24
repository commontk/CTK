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

#include "ctkDicomHostServerPrivate.h"

#include <ctkDicomHostInterface.h>

#include <QHostAddress>

#include <stdexcept>
#include <ctkDicomAppHostingTypesHelper.h>

#include <ctkExchangeSoapMessageProcessor.h>
#include "ctkHostSoapMessageProcessor_p.h"


ctkDicomHostServerPrivate::ctkDicomHostServerPrivate(ctkDicomHostInterface* hostInterface, int port) :
    port(port), hostInterface(hostInterface)
{
  connect(&server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));
  connect(&server, SIGNAL(incomingWSDLMessage(QString,QString*)),
          this, SLOT(incomingWSDLMessage(QString,QString*)));

  if (!server.listen(QHostAddress::LocalHost, this->port))
  {
    qCritical() << "Listening to 127.0.0.1:" << port << " failed.";
  }

  ctkHostSoapMessageProcessor* hostProcessor = new ctkHostSoapMessageProcessor( hostInterface );
  processors.push_back(hostProcessor);
  ctkExchangeSoapMessageProcessor* exchangeProcessor = new ctkExchangeSoapMessageProcessor( hostInterface );
  processors.push_back(exchangeProcessor);
}

void ctkDicomHostServerPrivate::incomingWSDLMessage(
  const QString& message, QString* reply)
{
  if (message == "?wsdl")
  {
    QFile wsdlfile(":/dah/HostService.wsdl");
    wsdlfile.open(QFile::ReadOnly | QFile::Text);
    if(wsdlfile.isOpen())
    {
      QTextStream textstream(&wsdlfile);
      *reply = textstream.readAll();
      QString actualURL="http://localhost:";
      actualURL+=QString::number(port)+"/HostInterface"; // FIXME: has to be replaced by url provided by host
      reply->replace("REPLACE_WITH_ACTUAL_URL",actualURL);
      reply->replace("HostService_schema1.xsd",actualURL+"?xsd=1");
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

void ctkDicomHostServerPrivate::incomingSoapMessage(
  const QtSoapMessage& message, QtSoapMessage* reply)
{
  processors.process(message, reply);
}
