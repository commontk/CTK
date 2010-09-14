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

#include "ctkDicomWG23HostPlugin_p.h"

#include <ctkDicomHostInterface.h>

#include <QHostAddress>

#include <stdexcept>
#include <ctkDicomWG23TypesHelper.h>

ctkDicomHostServerPrivate::ctkDicomHostServerPrivate(QObject *parent) :
    QObject(parent)
{
  ctkPluginContext* context = ctkDicomWG23HostPlugin::getInstance()->getPluginContext();
  ctkServiceReference* serviceRef = context->getServiceReference("ctkDicomHostInterface");
  if (!serviceRef)
  {
    // this will change after mergin changes from branch plugin_framework
    throw std::runtime_error("No Dicom Host Service found");
  }

  serviceBinding = qobject_cast<ctkDicomHostInterface*>(context->getService(serviceRef));

  connect(&server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));

  if (!server.listen(QHostAddress::LocalHost, 8080))
  {
    qCritical() << "Listening to 127.0.0.1:8080 failed.";
  }
}

void ctkDicomHostServerPrivate::incomingSoapMessage(const QtSoapMessage& message,
                                              QtSoapMessage* reply)
{
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "Received soap method request: " << methodName;

  if (methodName == "GetAvailableScreen")
  {
    processGetAvailableScreen(message, reply);
  }
  if (methodName == "notifyStateChanged")
  {
    processNotifyStateChanged(message, reply);
  }
  if (methodName == "notifyStatus")
  {
    processNotifyStatus(message, reply);
  }
}

void ctkDicomHostServerPrivate::processGetAvailableScreen(
    const QtSoapMessage &message, QtSoapMessage *reply)
{
  const QtSoapType& preferredScreenType = message.method()["preferredScreen"];
  QRect preferredScreen(preferredScreenType["RefPointX"].value().toInt(),
                        preferredScreenType["RefPointY"].value().toInt(),
                        preferredScreenType["Width"].value().toInt(),
                        preferredScreenType["Height"].value().toInt());

  QRect result = serviceBinding->getAvailableScreen(preferredScreen);

  reply->setMethod("GetAvailableScreenResponse");
  QtSoapStruct* availableScreenType = new QtSoapStruct(QtSoapQName("availableScreen"));
  availableScreenType->insert(new QtSoapSimpleType(QtSoapQName("Height"), result.height()));
  availableScreenType->insert(new QtSoapSimpleType(QtSoapQName("Width"), result.width()));
  availableScreenType->insert(new QtSoapSimpleType(QtSoapQName("RefPointX"), result.x()));
  availableScreenType->insert(new QtSoapSimpleType(QtSoapQName("RefPointY"), result.y()));
  reply->addMethodArgument(availableScreenType);
}

void ctkDicomHostServerPrivate::processNotifyStateChanged(
    const QtSoapMessage &message, QtSoapMessage *reply)
{
    ctkDicomWG23::State state;
    const QtSoapType& stateType = message.method()["state"];



}

void ctkDicomHostServerPrivate::processNotifyStatus(
    const QtSoapMessage &message, QtSoapMessage *reply)
{
    ctkDicomWG23::State state;
    const QtSoapType& stateType = message.method()["status"];

}
