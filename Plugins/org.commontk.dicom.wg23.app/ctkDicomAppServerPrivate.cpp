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

#include <ctkExchangeSoapMessageProcessor.h>
#include "ctkAppSoapMessageProcessor.h"

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

void ctkDicomAppServerPrivate::incomingSoapMessage(
  const QtSoapMessage& message,
  QtSoapMessage* reply)
{
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
    
    ctkAppSoapMessageProcessor* appProcessor = new ctkAppSoapMessageProcessor( appInterface );
    processors.push_back(appProcessor);
    ctkExchangeSoapMessageProcessor* exchangeProcessor = new ctkExchangeSoapMessageProcessor( appInterface );
    processors.push_back(exchangeProcessor);
  }
  
  processors.process(message, reply);
}

