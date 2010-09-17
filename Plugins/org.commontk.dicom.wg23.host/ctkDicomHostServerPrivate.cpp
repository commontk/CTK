/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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
#include <ctkDicomWG23TypesHelper.h>

#include <ctkExchangeSoapMessageProcessor.h>
#include "ctkHostSoapMessageProcessor.h"


ctkDicomHostServerPrivate::ctkDicomHostServerPrivate(ctkDicomHostInterface* hostInterface, int port) :
    hostInterface(hostInterface), port(port)
{
  connect(&server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));

  if (!server.listen(QHostAddress::LocalHost, this->port))
  {
    qCritical() << "Listening to 127.0.0.1:" << port << " failed.";
  }

  ctkHostSoapMessageProcessor hostProcessor( hostInterface );
  processors.push_back(hostProcessor);
  //ctkExchangeSoapMessageProcessor exchangeProcessor( hostInterface );
  //processors.push_back(exchangeProcessor);
}

void ctkDicomHostServerPrivate::incomingSoapMessage(
  const QtSoapMessage& message, QtSoapMessage* reply)
{
  processors.process(message, reply);
}

