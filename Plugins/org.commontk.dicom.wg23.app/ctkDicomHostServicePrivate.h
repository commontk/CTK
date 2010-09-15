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


#ifndef DICOMHOSTINTERFACEIMPL_P_H
#define DICOMHOSTINTERFACEIMPL_P_H

#include <ctkDicomHostInterface.h>

#include <QEventLoop>
#include <QtSoapHttpTransport>

class ctkDicomHostService : public ctkDicomHostInterface
{
  Q_OBJECT

public:
    ctkDicomHostService();

    virtual QString generateUID();
    virtual QRect getAvailableScreen(const QRect& preferredScreen);
    virtual QString getOutputLocation(const QStringList& preferredProtocols);
    virtual void notifyStateChanged(ctkDicomWG23::State state);
    virtual void notifyStatus(const ctkDicomWG23::Status& status);

private slots:

    void responseReady();

private:

    QEventLoop blockingLoop;
    QtSoapHttpTransport http;
};

#endif // DICOMHOSTINTERFACEIMPL_P_H
