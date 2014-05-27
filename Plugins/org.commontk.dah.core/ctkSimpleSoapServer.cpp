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


#include "ctkSimpleSoapServer.h"

#include "ctkSoapConnectionRunnable_p.h"

//----------------------------------------------------------------------------
ctkSimpleSoapServer::ctkSimpleSoapServer(QObject *parent) :
    QTcpServer(parent)
{
  qRegisterMetaType<QtSoapMessage>("QtSoapMessage");
}

//----------------------------------------------------------------------------
#if (QT_VERSION < 0x50000)
void ctkSimpleSoapServer::incomingConnection(int socketDescriptor)
#else
void ctkSimpleSoapServer::incomingConnection(qintptr socketDescriptor)
#endif
{
  qDebug() << "New incoming connection";
  ctkSoapConnectionRunnable* runnable = new ctkSoapConnectionRunnable(socketDescriptor);

  connect(runnable, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          Qt::BlockingQueuedConnection);

  connect(runnable, SIGNAL(incomingWSDLMessage(QString,QString*)),
          this, SIGNAL(incomingWSDLMessage(QString,QString*)),
          Qt::BlockingQueuedConnection);

  QThreadPool::globalInstance()->start(runnable);
}
