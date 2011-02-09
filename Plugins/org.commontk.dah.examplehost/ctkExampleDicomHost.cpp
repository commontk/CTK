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
#include <QProcess>
#include <QtDebug>
#include <QRect>

// CTK includes
#include "ctkExampleDicomHost.h"
#include "ctkDicomAppHostingTypesHelper.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
ctkExampleDicomHost::ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort, int appPort) :
    ctkDicomAbstractHost(hostPort, appPort),
    PlaceholderWidget(placeholderWidget),
    ApplicationState(ctkDicomAppHosting::IDLE)
{
  //connect(&this->AppProcess,SIGNAL(readyReadStandardOutput()),SLOT(forwardConsoleOutput()));
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::StartApplication(QString AppPath)
{
  QStringList arguments;
  arguments.append("--hostURL");
  arguments.append(QString("http://localhost:") + QString::number(this->getHostPort()) + "/HostInterface" );
  arguments.append("--applicationURL");
  arguments.append(QString("http://localhost:") + QString::number(this->getAppPort()) + "/ApplicationInterface" );
  //by default, the ctkExampleHostedApp uses the org.commontk.dah.exampleapp plugin
  //arguments.append("dicomapp"); // the app plugin to use - has to be changed later
  //if (!QProcess::startDetached (
  //{
  //    qCritical() << "application failed to start!";
  //}
  //qDebug() << "starting application: " << AppPath << " " << arguments;
  qDebug() << "starting application: " << AppPath << " " << arguments;
  this->AppProcess.setProcessChannelMode(QProcess::MergedChannels);
  this->AppProcess.start(AppPath, arguments);
}

//----------------------------------------------------------------------------
QRect ctkExampleDicomHost::getAvailableScreen(const QRect& preferredScreen)
{
  qDebug()<< "Application asked for this area:"<< preferredScreen;

  QRect rect (this->PlaceholderWidget->getAbsolutePosition());

  emit giveAvailableScreen(rect);
  return rect;
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::notifyStateChanged(ctkDicomAppHosting::State state)
{
  qDebug()<< "new state received:"<< static_cast<int>(state);
  qDebug()<< "new state received:"<< ctkDicomSoapState::toStringValue(state);

  emit stateChangedReceived(state);
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::notifyStatus(const ctkDicomAppHosting::Status& status)
{
  qDebug()<< "new status received:"<<status.codeMeaning;
  emit statusReceived(status);;
}

//----------------------------------------------------------------------------
ctkExampleDicomHost::~ctkExampleDicomHost()
{
  qDebug() << "Exiting host: trying to terminate app";
  this->AppProcess.terminate();
  qDebug() << "Exiting host: trying to kill app";
  this->AppProcess.kill();
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::forwardConsoleOutput()
{
  while( this->AppProcess.bytesAvailable() )
  {
    QString line( this->AppProcess.readLine() );
    line.prepend(">>>> ");
    std::cout << line.toStdString();
  }
}

//----------------------------------------------------------------------------
bool ctkExampleDicomHost::notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData)
{
  Q_UNUSED(data)
  Q_UNUSED(lastData)
  return false;
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkExampleDicomHost::getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData)
{
  Q_UNUSED(includeBulkData)
  Q_UNUSED(acceptableTransferSyntaxUIDs)

  //stupid test: only works with one uuid
  QList<ctkDicomAppHosting::ObjectLocator> locators;
  QUuid uuid;
  QUuid testUuid("{11111111-1111-1111-1111-111111111111}");
  foreach(uuid, objectUUIDs)
    {
    //stupid test: only works with one uuid
    if (uuid == testUuid)
      {
      ctkDicomAppHosting::ObjectLocator objectLocator;
      objectLocator.locator = QUuid();
      objectLocator.source = QUuid();
      //need to filter transfert syntax with acceptable ones
      objectLocator.transferSyntax = "transSyntaxUId";
      objectLocator.length = 0;
      objectLocator.offset = 0;
      objectLocator.URI = "testFile.txt";
      locators.append (objectLocator);
      }
    return locators;
    }

  return QList<ctkDicomAppHosting::ObjectLocator>();
}

//----------------------------------------------------------------------------
void ctkExampleDicomHost::releaseData(const QList<QUuid>& objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}
