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


#include "ctkDicomAppServerPrivate.h"

#include <ctkDicomAppInterface.h>
#include <ctkServiceReference.h>

#include <QHostAddress>

#include <stdexcept>
#include <ctkDicomWG23TypesHelper.h>
#include <ctkDicomWG23AppPlugin_p.h>

ctkDicomAppServerPrivate::ctkDicomAppServerPrivate(int port) :
    appInterface(0), port(port)
{
  connect(&server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));

  if (!server.listen(QHostAddress::LocalHost, this->port))
  {
    qCritical() << "Listening to 127.0.0.1:" << port << " failed.";
  }
}

void ctkDicomAppServerPrivate::incomingSoapMessage(const QtSoapMessage& message,
                                              QtSoapMessage* reply)
{
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "AppServer: Received soap method request: " << methodName;

  if(appInterface == NULL)
  {
    ctkPluginContext* context = ctkDicomWG23AppPlugin::getInstance()->getPluginContext();
    ctkServiceReference* serviceRef = context->getServiceReference("ctkDicomAppInterface");
    if (!serviceRef)
    {
      // this will change after merging changes from branch plugin_framework
      throw std::runtime_error("No Dicom App Service found");
    }
    appInterface = qobject_cast<ctkDicomAppInterface*>(context->getService(serviceRef));
  }

  if (methodName == "getState")
  {
    processGetState(message, reply);
  }
  if (methodName == "setState")
  {
    processSetState(message, reply);
  }
  if (methodName == "bringToFront")
  {
    processBringToFront(message, reply);
  }
}

void ctkDicomAppServerPrivate::processGetState(
    const QtSoapMessage &message, QtSoapMessage *reply)
{
    const ctkDicomWG23::State result = appInterface->getState();

    reply->setMethod("GetState");
    QtSoapSimpleType* stateType = new ctkDicomSoapState("state",result);
    reply->addMethodArgument(stateType);
}

void ctkDicomAppServerPrivate::processSetState(
    const QtSoapMessage &message, QtSoapMessage *reply)
{
    const QtSoapType& stateType = message.method()["state"];
    bool result = appInterface->setState(ctkDicomSoapState::getState(stateType));
  
    reply->setMethod("SetState");
    QtSoapType* stateLegal = new ctkDicomSoapBool("stateLegal",result);
    reply->addMethodArgument(stateLegal);
}

void ctkDicomAppServerPrivate::processBringToFront(
    const QtSoapMessage &message, QtSoapMessage *reply)
{
   const QtSoapType& requestedScreenAreaType = message.method()["requestedScreenArea"];
   const QRect requestedScreenArea = ctkDicomSoapRectangle::getQRect(requestedScreenAreaType);

   reply->setMethod("bringToFront");
   bool result = appInterface->bringToFront(requestedScreenArea);
   QtSoapType* received = new ctkDicomSoapBool("received",result);
   reply->addMethodArgument(received);
}
