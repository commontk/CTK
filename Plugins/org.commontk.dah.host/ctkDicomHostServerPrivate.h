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

#ifndef CTKDICOMHOSTSERVERPRIVATE_H
#define CTKDICOMHOSTSERVERPRIVATE_H

#include <QObject>
#include <QtSoapMessage>

#include <ctkSimpleSoapServer.h>
#include <ctkSoapMessageProcessorList.h>

struct ctkDicomHostInterface;

class ctkDicomHostServerPrivate : public QObject
{
  Q_OBJECT

public:
  ctkDicomHostServerPrivate(ctkDicomHostInterface* hostInterface, int port, QString path);

  ctkSimpleSoapServer Server;
  int Port;
  QString Path;

public Q_SLOTS:

  void incomingSoapMessage(const QtSoapMessage& message,
                           QtSoapMessage* reply);
  void incomingWSDLMessage(const QString& message, QString* reply);

private:

  ctkSoapMessageProcessorList Processors;
  ctkDicomHostInterface* HostInterface;

};

#endif // CTKDICOMHOSTSERVERPRIVATE_H
