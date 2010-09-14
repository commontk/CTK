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


#ifndef CTKDICOMAPPSERVERPRIVATE_H
#define CTKDICOMAPPSERVERPRIVATE_H

#include <QObject>
#include <QtSoapMessage>

#include <ctkSimpleSoapServer.h>

class ctkDicomAppInterface;

class ctkDicomAppServerPrivate : public QObject
{
  Q_OBJECT

public:
  ctkDicomAppServerPrivate(ctkDicomAppInterface* appInterface, int port);

  ctkSimpleSoapServer server;
  ctkDicomAppInterface* appInterface;
  int port;

public slots:

  void incomingSoapMessage(const QtSoapMessage& message,
                           QtSoapMessage* reply);

private:

  void processGetState(const QtSoapMessage& message,
                       QtSoapMessage* reply);
  void processSetState(const QtSoapMessage& message,
                       QtSoapMessage* reply);
  void processBringToFront(const QtSoapMessage& message,
                           QtSoapMessage* reply);

};

#endif // CTKDICOMAPPSERVERPRIVATE_H
